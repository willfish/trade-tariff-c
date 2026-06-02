#include "repositories/quota_order_number_repository.h"

#include "sql_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUOTA_ORDER_NUMBER_REPOSITORY_COLS 4

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

static int valid_order_number(const char *value)
{
    if (value == NULL || *value == '\0') {
        return 0;
    }
    for (const char *p = value; *p; p++) {
        if (*p == '"' || *p == '\\' || *p == '{' || *p == '}' || *p == ',') {
            return 0;
        }
    }
    return 1;
}

char *quota_order_number_repository_text_array_param(const char *const *order_numbers,
                                                     size_t order_number_count)
{
    if (order_numbers == NULL || order_number_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < order_number_count; i++) {
        if (!valid_order_number(order_numbers[i])) {
            continue;
        }
        len += strlen(order_numbers[i]) + 3;
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
    for (size_t i = 0; i < order_number_count; i++) {
        if (!valid_order_number(order_numbers[i])) {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        *cursor++ = '"';
        size_t value_len = strlen(order_numbers[i]);
        memcpy(cursor, order_numbers[i], value_len);
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

    char *template = sql_loader_load("sql/quota_order_numbers/for_order_number_ids.sql");
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

int quota_order_number_repository_hydrate_row(const DbRows *rows,
                                              int row,
                                              QuotaOrderNumberModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < QUOTA_ORDER_NUMBER_REPOSITORY_COLS) {
        return 0;
    }

    *model = (QuotaOrderNumberModel){
        .quota_order_number_sid = owned_value(rows, row, 0),
        .quota_order_number_id = owned_value(rows, row, 1),
        .validity_start_date = owned_value(rows, row, 2),
        .validity_end_date = owned_value(rows, row, 3),
    };

    if (model->quota_order_number_sid == NULL ||
        model->quota_order_number_id == NULL ||
        model->validity_start_date == NULL) {
        quota_order_number_model_free(model);
        return 0;
    }
    return 1;
}

int quota_order_number_repository_fetch_for_order_numbers(const CommodityRequestContext *context,
                                                          const char *const *order_numbers,
                                                          size_t order_number_count,
                                                          QuotaOrderNumberModelList *order_number_models)
{
    if (context == NULL || order_number_models == NULL) {
        return 0;
    }

    char *order_numbers_param = quota_order_number_repository_text_array_param(order_numbers, order_number_count);
    char *sql = build_sql(context);
    if (order_numbers_param == NULL || sql == NULL) {
        free(order_numbers_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("quota_order_numbers_for_order_number_ids", sql, order_numbers_param);
    free(order_numbers_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        QuotaOrderNumberModel model = { 0 };
        if (!quota_order_number_repository_hydrate_row(&rows, row, &model)) {
            quota_order_number_model_list_free(order_number_models);
            db_rows_free(&rows);
            return 0;
        }
        if (!quota_order_number_model_list_append(order_number_models, model)) {
            quota_order_number_model_list_free(order_number_models);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int quota_order_number_repository_self_test(void)
{
    const char *order_numbers[] = { "054001", "bad,value", "", "060004" };
    char *param = quota_order_number_repository_text_array_param(order_numbers, sizeof(order_numbers) / sizeof(order_numbers[0]));
    int ok = param != NULL && strcmp(param, "{\"054001\",\"060004\"}") == 0;
    free(param);
    return ok;
}
