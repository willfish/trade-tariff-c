#include "db.h"

#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PGconn *db_connection = NULL;
static char *prepared_statement_name = NULL;
static char *prepared_sql = NULL;

static char *copy_string(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

void db_result_free(DbResult *result)
{
    free(result->data);
    free(result->error);
    result->data = NULL;
    result->error = NULL;
    result->len = 0;
    result->ok = 0;
}

void db_rows_free(DbRows *rows)
{
    if (rows == NULL) {
        return;
    }

    if (rows->values != NULL) {
        int count = rows->rows * rows->cols;
        for (int i = 0; i < count; i++) {
            free(rows->values[i]);
        }
    }

    free(rows->values);
    free(rows->nulls);
    free(rows->error);
    rows->values = NULL;
    rows->nulls = NULL;
    rows->error = NULL;
    rows->rows = 0;
    rows->cols = 0;
    rows->ok = 0;
}

static void clear_prepared_state(void)
{
    free(prepared_statement_name);
    free(prepared_sql);
    prepared_statement_name = NULL;
    prepared_sql = NULL;
}

static PGconn *database_connection(void)
{
    if (db_connection != NULL && PQstatus(db_connection) == CONNECTION_OK) {
        return db_connection;
    }

    if (db_connection != NULL) {
        PQfinish(db_connection);
        db_connection = NULL;
        clear_prepared_state();
    }

    const char *database_url = getenv("DATABASE_URL");
    if (database_url == NULL || *database_url == '\0') {
        database_url = "postgres:///tariff_development";
    }

    db_connection = PQconnectdb(database_url);
    return db_connection;
}

static int ensure_prepared(PGconn *connection, const char *statement_name, const char *sql, DbResult *result)
{
    if (prepared_statement_name != NULL &&
        prepared_sql != NULL &&
        strcmp(prepared_statement_name, statement_name) == 0 &&
        strcmp(prepared_sql, sql) == 0) {
        return 1;
    }

    if (prepared_statement_name != NULL) {
        size_t command_len = strlen("DEALLOCATE ") + strlen(prepared_statement_name) + 1;
        char *command = malloc(command_len);
        if (command == NULL) {
            result->error = copy_string("Unable to allocate deallocate command");
            return 0;
        }
        snprintf(command, command_len, "DEALLOCATE %s", prepared_statement_name);
        PGresult *deallocated = PQexec(connection, command);
        free(command);
        if (deallocated != NULL) {
            PQclear(deallocated);
        }
        clear_prepared_state();
    }

    PGresult *prepared = PQprepare(connection, statement_name, sql, 1, NULL);
    if (prepared == NULL) {
        result->error = copy_string(PQerrorMessage(connection));
        PQfinish(connection);
        db_connection = NULL;
        clear_prepared_state();
        return 0;
    }

    ExecStatusType status = PQresultStatus(prepared);
    if (status != PGRES_COMMAND_OK) {
        result->error = copy_string(PQresultErrorMessage(prepared));
        PQclear(prepared);
        return 0;
    }

    PQclear(prepared);
    prepared_statement_name = copy_string(statement_name);
    prepared_sql = copy_string(sql);
    if (prepared_statement_name == NULL || prepared_sql == NULL) {
        result->error = copy_string("Unable to allocate prepared query");
        clear_prepared_state();
        return 0;
    }

    return 1;
}

DbResult db_fetch_single_text(const char *statement_name, const char *sql, const char *param)
{
    DbResult result = { .data = NULL, .len = 0, .error = NULL, .ok = 0 };
    PGconn *connection = database_connection();

    if (connection == NULL || PQstatus(connection) != CONNECTION_OK) {
        result.error = copy_string(connection == NULL ? "Unable to allocate database connection" : PQerrorMessage(connection));
        return result;
    }

    if (!ensure_prepared(connection, statement_name, sql, &result)) {
        return result;
    }

    const char *params[1] = { param };
    PGresult *query_result = PQexecPrepared(connection, statement_name, 1, params, NULL, NULL, 0);
    if (query_result == NULL) {
        result.error = copy_string(PQerrorMessage(connection));
        PQfinish(connection);
        db_connection = NULL;
        clear_prepared_state();
        return result;
    }

    ExecStatusType status = PQresultStatus(query_result);
    if (status != PGRES_TUPLES_OK) {
        result.error = copy_string(PQresultErrorMessage(query_result));
        PQclear(query_result);
        return result;
    }

    int rows = PQntuples(query_result);
    if (rows == 0 || PQgetisnull(query_result, 0, 0)) {
        result.ok = 1;
        PQclear(query_result);
        return result;
    }

    int len = PQgetlength(query_result, 0, 0);
    result.data = malloc((size_t)len + 1);
    if (result.data == NULL) {
        result.error = copy_string("Unable to allocate query result");
        PQclear(query_result);
        return result;
    }

    memcpy(result.data, PQgetvalue(query_result, 0, 0), (size_t)len);
    result.data[len] = '\0';
    result.len = (size_t)len;
    result.ok = 1;

    PQclear(query_result);
    return result;
}

static DbRows db_fetch_rows_params(const char *statement_name,
                                   const char *sql,
                                   int param_count,
                                   const char *const *params)
{
    DbRows rows = { .values = NULL, .nulls = NULL, .rows = 0, .cols = 0, .error = NULL, .ok = 0 };
    DbResult prepare_error = { .data = NULL, .len = 0, .error = NULL, .ok = 0 };
    PGconn *connection = database_connection();

    if (connection == NULL || PQstatus(connection) != CONNECTION_OK) {
        rows.error = copy_string(connection == NULL ? "Unable to allocate database connection" : PQerrorMessage(connection));
        return rows;
    }

    if (!ensure_prepared(connection, statement_name, sql, &prepare_error)) {
        rows.error = prepare_error.error;
        prepare_error.error = NULL;
        db_result_free(&prepare_error);
        return rows;
    }

    PGresult *query_result = PQexecPrepared(connection, statement_name, param_count, params, NULL, NULL, 0);
    if (query_result == NULL) {
        rows.error = copy_string(PQerrorMessage(connection));
        PQfinish(connection);
        db_connection = NULL;
        clear_prepared_state();
        return rows;
    }

    ExecStatusType status = PQresultStatus(query_result);
    if (status != PGRES_TUPLES_OK) {
        rows.error = copy_string(PQresultErrorMessage(query_result));
        PQclear(query_result);
        return rows;
    }

    rows.rows = PQntuples(query_result);
    rows.cols = PQnfields(query_result);
    int count = rows.rows * rows.cols;
    rows.values = calloc((size_t)count, sizeof(char *));
    rows.nulls = calloc((size_t)count, sizeof(unsigned char));
    if ((count > 0 && rows.values == NULL) || (count > 0 && rows.nulls == NULL)) {
        rows.error = copy_string("Unable to allocate query rows");
        PQclear(query_result);
        db_rows_free(&rows);
        return rows;
    }

    for (int row = 0; row < rows.rows; row++) {
        for (int col = 0; col < rows.cols; col++) {
            int index = row * rows.cols + col;
            if (PQgetisnull(query_result, row, col)) {
                rows.nulls[index] = 1;
                continue;
            }

            rows.values[index] = copy_string(PQgetvalue(query_result, row, col));
            if (rows.values[index] == NULL) {
                rows.error = copy_string("Unable to allocate query row value");
                PQclear(query_result);
                db_rows_free(&rows);
                return rows;
            }
        }
    }

    rows.ok = 1;
    PQclear(query_result);
    return rows;
}

DbRows db_fetch_rows(const char *statement_name, const char *sql, const char *param)
{
    const char *params[1] = { param };
    return db_fetch_rows_params(statement_name, sql, 1, params);
}

DbRows db_fetch_rows2(const char *statement_name, const char *sql, const char *param1, const char *param2)
{
    const char *params[2] = { param1, param2 };
    return db_fetch_rows_params(statement_name, sql, 2, params);
}

const char *db_rows_value(const DbRows *rows, int row, int col)
{
    if (rows == NULL || row < 0 || col < 0 || row >= rows->rows || col >= rows->cols) {
        return NULL;
    }

    return rows->values[row * rows->cols + col];
}

int db_rows_is_null(const DbRows *rows, int row, int col)
{
    if (rows == NULL || row < 0 || col < 0 || row >= rows->rows || col >= rows->cols) {
        return 1;
    }

    return rows->nulls[row * rows->cols + col] != 0;
}
