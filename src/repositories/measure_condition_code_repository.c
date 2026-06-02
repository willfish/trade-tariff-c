#include "repositories/measure_condition_code_repository.h"

#include "sql_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEASURE_CONDITION_CODE_REPOSITORY_COLS 4

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

static char *owned_value(const DbRows *rows, int row, int col)
{
    if (db_rows_is_null(rows, row, col)) {
        return NULL;
    }
    const char *value = db_rows_value(rows, row, col);
    return value == NULL ? NULL : copy_literal(value);
}

char *measure_condition_code_repository_code_array_param(const char *const *codes,
                                                         size_t code_count)
{
    if (codes == NULL || code_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < code_count; i++) {
        if (codes[i] == NULL || codes[i][0] == '\0') {
            continue;
        }
        len += strlen(codes[i]) + 3;
        valid_count++;
    }
    if (valid_count == 0) {
        return copy_literal("{}");
    }

    char *param = malloc(len);
    if (param == NULL) {
        return NULL;
    }

    char *cursor = param;
    *cursor++ = '{';
    size_t written = 0;
    for (size_t i = 0; i < code_count; i++) {
        if (codes[i] == NULL || codes[i][0] == '\0') {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        *cursor++ = '"';
        size_t value_len = strlen(codes[i]);
        memcpy(cursor, codes[i], value_len);
        cursor += value_len;
        *cursor++ = '"';
        written++;
    }
    *cursor++ = '}';
    *cursor = '\0';
    return param;
}

static char *date_expression(const char *actual_date)
{
    if (actual_date == NULL || *actual_date == '\0') {
        return copy_literal("CURRENT_DATE");
    }

    size_t len = strlen(actual_date);
    char *expr = malloc(len + strlen("''::date") + 1);
    if (expr == NULL) {
        return NULL;
    }
    snprintf(expr, len + strlen("''::date") + 1, "'%s'::date", actual_date);
    return expr;
}

static char *build_sql(const CommodityRequestContext *context)
{
    if (context == NULL || context->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load("sql/measure_condition_codes/for_codes.sql");
    char *date = date_expression(context->actual_date);
    if (template == NULL || date == NULL) {
        free(template);
        free(date);
        return NULL;
    }

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", context->schema },
        { "ACTUAL_DATE", date },
    };
    char *sql = sql_loader_apply_bindings(template, bindings, sizeof(bindings) / sizeof(bindings[0]));
    free(template);
    free(date);
    return sql;
}

int measure_condition_code_repository_hydrate_row(const DbRows *rows,
                                                  int row,
                                                  MeasureConditionCodeModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < MEASURE_CONDITION_CODE_REPOSITORY_COLS) {
        return 0;
    }

    *model = (MeasureConditionCodeModel){
        .condition_code = owned_value(rows, row, 0),
        .description = owned_value(rows, row, 1),
        .validity_start_date = owned_value(rows, row, 2),
        .validity_end_date = owned_value(rows, row, 3),
    };

    if (model->condition_code == NULL) {
        measure_condition_code_model_free(model);
        return 0;
    }
    return 1;
}

int measure_condition_code_repository_fetch_for_codes(const CommodityRequestContext *context,
                                                      const char *const *codes,
                                                      size_t code_count,
                                                      MeasureConditionCodeModelList *condition_codes)
{
    if (context == NULL || condition_codes == NULL) {
        return 0;
    }

    char *code_param = measure_condition_code_repository_code_array_param(codes, code_count);
    char *sql = build_sql(context);
    if (code_param == NULL || sql == NULL) {
        free(code_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("measure_condition_codes_for_codes", sql, code_param);
    free(code_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        MeasureConditionCodeModel model = { 0 };
        if (!measure_condition_code_repository_hydrate_row(&rows, row, &model)) {
            measure_condition_code_model_list_free(condition_codes);
            db_rows_free(&rows);
            return 0;
        }
        if (!measure_condition_code_model_list_append(condition_codes, model)) {
            measure_condition_code_model_list_free(condition_codes);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int measure_condition_code_repository_self_test(void)
{
    const char *codes[] = { "B", NULL, "", "V" };
    char *param = measure_condition_code_repository_code_array_param(codes, sizeof(codes) / sizeof(codes[0]));
    int ok = param != NULL && strcmp(param, "{\"B\",\"V\"}") == 0;
    free(param);
    return ok;
}
