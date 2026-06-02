#include "repositories/oplog_table_repository.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int identifier_safe(const char *value)
{
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    for (const char *p = value; *p != '\0'; p++) {
        if ((*p < 'a' || *p > 'z') && (*p < '0' || *p > '9') && *p != '_') {
            return 0;
        }
    }
    return 1;
}

char *oplog_table_repository_build_select(const OplogTableQuery *query)
{
    if (query == NULL ||
        !identifier_safe(query->schema) ||
        !identifier_safe(query->table) ||
        query->columns == NULL ||
        query->where_sql == NULL) {
        return NULL;
    }

    const char *order_sql = query->order_sql == NULL ? "" : query->order_sql;
    int needed = snprintf(NULL,
                          0,
                          "SELECT %s FROM %s.%s WHERE %s%s",
                          query->columns,
                          query->schema,
                          query->table,
                          query->where_sql,
                          order_sql);
    if (needed < 0) {
        return NULL;
    }

    char *sql = malloc((size_t)needed + 1);
    if (sql == NULL) {
        return NULL;
    }
    snprintf(sql,
             (size_t)needed + 1,
             "SELECT %s FROM %s.%s WHERE %s%s",
             query->columns,
             query->schema,
             query->table,
             query->where_sql,
             order_sql);
    return sql;
}

DbRows oplog_table_repository_fetch(const char *statement_name,
                                    const OplogTableQuery *query,
                                    const char *param)
{
    char *sql = oplog_table_repository_build_select(query);
    if (sql == NULL) {
        DbRows rows = { .values = NULL, .nulls = NULL, .rows = 0, .cols = 0, .error = NULL, .ok = 0 };
        return rows;
    }
    DbRows rows = db_fetch_rows(statement_name, sql, param);
    free(sql);
    return rows;
}

int oplog_table_repository_self_test(void)
{
    OplogTableQuery query = {
        .schema = "uk",
        .table = "measure_components",
        .columns = "measure_sid, duty_expression_id",
        .where_sql = "measure_sid = $1",
        .order_sql = " ORDER BY duty_expression_id",
    };
    char *sql = oplog_table_repository_build_select(&query);
    int ok = sql != NULL &&
             strcmp(sql,
                    "SELECT measure_sid, duty_expression_id FROM uk.measure_components WHERE measure_sid = $1 ORDER BY duty_expression_id") == 0;
    free(sql);
    query.schema = "uk;drop";
    sql = oplog_table_repository_build_select(&query);
    ok = ok && sql == NULL;
    free(sql);
    return ok;
}
