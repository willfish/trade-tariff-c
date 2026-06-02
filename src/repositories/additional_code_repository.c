#include "repositories/additional_code_repository.h"

#include "sql_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADDITIONAL_CODE_REPOSITORY_COLS 4

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

static int is_numeric_sid(const char *value)
{
    if (value == NULL || *value == '\0') {
        return 0;
    }

    for (const char *p = value; *p; p++) {
        if (!isdigit((unsigned char)*p)) {
            return 0;
        }
    }
    return 1;
}

char *additional_code_repository_sid_array_param(const char *const *additional_code_sids,
                                                 size_t sid_count)
{
    if (additional_code_sids == NULL || sid_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < sid_count; i++) {
        if (!is_numeric_sid(additional_code_sids[i])) {
            continue;
        }
        len += strlen(additional_code_sids[i]) + 1;
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
    for (size_t i = 0; i < sid_count; i++) {
        if (!is_numeric_sid(additional_code_sids[i])) {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        size_t value_len = strlen(additional_code_sids[i]);
        memcpy(cursor, additional_code_sids[i], value_len);
        cursor += value_len;
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

    char *template = sql_loader_load("sql/additional_codes/for_additional_code_sids.sql");
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

int additional_code_repository_hydrate_row(const DbRows *rows,
                                           int row,
                                           AdditionalCodeModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < ADDITIONAL_CODE_REPOSITORY_COLS) {
        return 0;
    }

    *model = (AdditionalCodeModel){
        .additional_code_sid = owned_value(rows, row, 0),
        .additional_code_type_id = owned_value(rows, row, 1),
        .additional_code = owned_value(rows, row, 2),
        .description = owned_value(rows, row, 3),
    };

    if (model->additional_code_sid == NULL ||
        model->additional_code_type_id == NULL ||
        model->additional_code == NULL) {
        additional_code_model_free(model);
        return 0;
    }

    return 1;
}

int additional_code_repository_fetch_for_additional_code_sids(const CommodityRequestContext *context,
                                                              const char *const *additional_code_sids,
                                                              size_t sid_count,
                                                              AdditionalCodeModelList *additional_codes)
{
    if (context == NULL || additional_codes == NULL) {
        return 0;
    }

    char *sid_param = additional_code_repository_sid_array_param(additional_code_sids, sid_count);
    char *sql = build_sql(context);
    if (sid_param == NULL || sql == NULL) {
        free(sid_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("additional_codes_for_additional_code_sids", sql, sid_param);
    free(sid_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        AdditionalCodeModel model = { 0 };
        if (!additional_code_repository_hydrate_row(&rows, row, &model)) {
            additional_code_model_list_free(additional_codes);
            db_rows_free(&rows);
            return 0;
        }
        if (!additional_code_model_list_append(additional_codes, model)) {
            additional_code_model_list_free(additional_codes);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int additional_code_repository_self_test(void)
{
    const char *sids[] = { "123", "bad", "", "456" };
    char *param = additional_code_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));
    int ok = param != NULL && strcmp(param, "{123,456}") == 0;
    free(param);
    return ok;
}
