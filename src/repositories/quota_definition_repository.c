#include "repositories/quota_definition_repository.h"

#include "sql_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUOTA_DEFINITION_REPOSITORY_COLS 21

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

char *quota_definition_repository_text_array_param(const char *const *order_numbers,
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

    char *template = sql_loader_load("sql/quota_definitions/for_order_number_ids.sql");
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

int quota_definition_repository_hydrate_row(const DbRows *rows,
                                            int row,
                                            QuotaDefinitionModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < QUOTA_DEFINITION_REPOSITORY_COLS) {
        return 0;
    }

    *model = (QuotaDefinitionModel){
        .quota_definition_sid = owned_value(rows, row, 0),
        .quota_order_number_id = owned_value(rows, row, 1),
        .quota_order_number_sid = owned_value(rows, row, 2),
        .initial_volume = owned_value(rows, row, 3),
        .balance = owned_value(rows, row, 4),
        .description = owned_value(rows, row, 5),
        .measurement_unit_code = owned_value(rows, row, 6),
        .monetary_unit_code = owned_value(rows, row, 7),
        .measurement_unit_qualifier_code = owned_value(rows, row, 8),
        .last_allocation_date = owned_value(rows, row, 9),
        .suspension_period_start_date = owned_value(rows, row, 10),
        .suspension_period_end_date = owned_value(rows, row, 11),
        .blocking_period_start_date = owned_value(rows, row, 12),
        .blocking_period_end_date = owned_value(rows, row, 13),
        .validity_start_date = owned_value(rows, row, 14),
        .validity_end_date = owned_value(rows, row, 15),
        .measurement_unit_description = owned_value(rows, row, 16),
        .measurement_unit_abbreviation = owned_value(rows, row, 17),
        .critical_state = owned_value(rows, row, 18),
        .latest_critical_state = owned_value(rows, row, 19),
        .exhausted_event_sid = owned_value(rows, row, 20),
    };

    if (model->quota_definition_sid == NULL ||
        model->quota_order_number_id == NULL ||
        model->quota_order_number_sid == NULL ||
        model->initial_volume == NULL ||
        model->balance == NULL ||
        model->validity_start_date == NULL) {
        quota_definition_model_free(model);
        return 0;
    }
    return 1;
}

int quota_definition_repository_fetch_for_order_numbers(const CommodityRequestContext *context,
                                                        const char *const *order_numbers,
                                                        size_t order_number_count,
                                                        QuotaDefinitionModelList *definitions)
{
    if (context == NULL || definitions == NULL) {
        return 0;
    }

    char *order_numbers_param = quota_definition_repository_text_array_param(order_numbers, order_number_count);
    char *sql = build_sql(context);
    if (order_numbers_param == NULL || sql == NULL) {
        free(order_numbers_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("quota_definitions_for_order_number_ids", sql, order_numbers_param);
    free(order_numbers_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        QuotaDefinitionModel model = { 0 };
        if (!quota_definition_repository_hydrate_row(&rows, row, &model)) {
            quota_definition_model_list_free(definitions);
            db_rows_free(&rows);
            return 0;
        }
        if (!quota_definition_model_list_append(definitions, model)) {
            quota_definition_model_list_free(definitions);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int quota_definition_repository_self_test(void)
{
    const char *order_numbers[] = { "054001", "bad,value", "", "060004" };
    char *param = quota_definition_repository_text_array_param(order_numbers, sizeof(order_numbers) / sizeof(order_numbers[0]));
    int ok = param != NULL && strcmp(param, "{\"054001\",\"060004\"}") == 0;
    free(param);
    return ok;
}
