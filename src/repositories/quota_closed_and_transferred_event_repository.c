#include "repositories/quota_closed_and_transferred_event_repository.h"

#include "sql_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUOTA_CLOSED_AND_TRANSFERRED_EVENT_REPOSITORY_COLS 13

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

static int valid_sid(const char *value)
{
    if (value == NULL || *value == '\0') {
        return 0;
    }
    for (const char *p = value; *p; p++) {
        if (*p < '0' || *p > '9') {
            return 0;
        }
    }
    return 1;
}

char *quota_closed_and_transferred_event_repository_text_array_param(const char *const *quota_definition_sids,
                                                                     size_t quota_definition_sid_count)
{
    if (quota_definition_sids == NULL || quota_definition_sid_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < quota_definition_sid_count; i++) {
        if (!valid_sid(quota_definition_sids[i])) {
            continue;
        }
        len += strlen(quota_definition_sids[i]) + 3;
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
    for (size_t i = 0; i < quota_definition_sid_count; i++) {
        if (!valid_sid(quota_definition_sids[i])) {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        *cursor++ = '"';
        size_t value_len = strlen(quota_definition_sids[i]);
        memcpy(cursor, quota_definition_sids[i], value_len);
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

    char *template = sql_loader_load("sql/quota_closed_and_transferred_events/for_target_definition_sids.sql");
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

int quota_closed_and_transferred_event_repository_hydrate_row(const DbRows *rows,
                                                              int row,
                                                              QuotaClosedAndTransferredEventModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < QUOTA_CLOSED_AND_TRANSFERRED_EVENT_REPOSITORY_COLS) {
        return 0;
    }

    *model = (QuotaClosedAndTransferredEventModel){
        .quota_definition_sid = owned_value(rows, row, 0),
        .occurrence_timestamp = owned_value(rows, row, 1),
        .target_quota_definition_sid = owned_value(rows, row, 2),
        .closing_date = owned_value(rows, row, 3),
        .transferred_amount = owned_value(rows, row, 4),
        .quota_definition_validity_start_date = owned_value(rows, row, 5),
        .quota_definition_validity_end_date = owned_value(rows, row, 6),
        .quota_definition_measurement_unit_description = owned_value(rows, row, 7),
        .quota_definition_measurement_unit_abbreviation = owned_value(rows, row, 8),
        .target_quota_definition_validity_start_date = owned_value(rows, row, 9),
        .target_quota_definition_validity_end_date = owned_value(rows, row, 10),
        .target_quota_definition_measurement_unit_description = owned_value(rows, row, 11),
        .target_quota_definition_measurement_unit_abbreviation = owned_value(rows, row, 12),
    };

    if (model->quota_definition_sid == NULL ||
        model->occurrence_timestamp == NULL ||
        model->target_quota_definition_sid == NULL) {
        quota_closed_and_transferred_event_model_free(model);
        return 0;
    }
    return 1;
}

int quota_closed_and_transferred_event_repository_fetch_for_target_definition_sids(
    const CommodityRequestContext *context,
    const char *const *quota_definition_sids,
    size_t quota_definition_sid_count,
    QuotaClosedAndTransferredEventModelList *events)
{
    if (context == NULL || events == NULL) {
        return 0;
    }

    char *sid_param = quota_closed_and_transferred_event_repository_text_array_param(quota_definition_sids,
                                                                                    quota_definition_sid_count);
    char *sql = build_sql(context);
    if (sid_param == NULL || sql == NULL) {
        free(sid_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("quota_closed_and_transferred_events_for_target_definition_sids", sql, sid_param);
    free(sid_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        QuotaClosedAndTransferredEventModel model = { 0 };
        if (!quota_closed_and_transferred_event_repository_hydrate_row(&rows, row, &model)) {
            quota_closed_and_transferred_event_model_list_free(events);
            db_rows_free(&rows);
            return 0;
        }
        if (!quota_closed_and_transferred_event_model_list_append(events, model)) {
            quota_closed_and_transferred_event_model_list_free(events);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int quota_closed_and_transferred_event_repository_self_test(void)
{
    const char *sids[] = { "100", "bad", "", "200" };
    char *param = quota_closed_and_transferred_event_repository_text_array_param(sids, sizeof(sids) / sizeof(sids[0]));
    int ok = param != NULL && strcmp(param, "{\"100\",\"200\"}") == 0;
    free(param);
    return ok;
}
