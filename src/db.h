#ifndef DB_H
#define DB_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t len;
    char *error;
    int ok;
} DbResult;

typedef struct {
    char **values;
    unsigned char *nulls;
    int rows;
    int cols;
    char *error;
    int ok;
} DbRows;

DbResult db_fetch_single_text(const char *statement_name, const char *sql, const char *param);
DbRows db_fetch_rows(const char *statement_name, const char *sql, const char *param);
DbRows db_fetch_rows2(const char *statement_name, const char *sql, const char *param1, const char *param2);
const char *db_rows_value(const DbRows *rows, int row, int col);
int db_rows_is_null(const DbRows *rows, int row, int col);
void db_result_free(DbResult *result);
void db_rows_free(DbRows *rows);

#endif
