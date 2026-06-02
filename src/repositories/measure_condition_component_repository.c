#include "repositories/measure_condition_component_repository.h"

#include "sql_loader.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MEASURE_CONDITION_COMPONENT_REPOSITORY_COLS 7

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

    const char *p = value;
    if (*p == '-') {
        p++;
    }
    if (*p == '\0') {
        return 0;
    }

    for (; *p; p++) {
        if (!isdigit((unsigned char)*p)) {
            return 0;
        }
    }
    return 1;
}

char *measure_condition_component_repository_sid_array_param(const char *const *measure_condition_sids,
                                                             size_t sid_count)
{
    if (measure_condition_sids == NULL || sid_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < sid_count; i++) {
        if (!is_numeric_sid(measure_condition_sids[i])) {
            continue;
        }
        len += strlen(measure_condition_sids[i]) + 1;
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
        if (!is_numeric_sid(measure_condition_sids[i])) {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        size_t value_len = strlen(measure_condition_sids[i]);
        memcpy(cursor, measure_condition_sids[i], value_len);
        cursor += value_len;
        written++;
    }
    *cursor++ = '}';
    *cursor = '\0';
    return param;
}

static char *build_sql(const CommodityRequestContext *context)
{
    if (context == NULL || context->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load("sql/measure_condition_components/for_measure_condition_sids.sql");
    if (template == NULL) {
        return NULL;
    }

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", context->schema },
    };
    char *sql = sql_loader_apply_bindings(template, bindings, sizeof(bindings) / sizeof(bindings[0]));
    free(template);
    return sql;
}

int measure_condition_component_repository_hydrate_row(const DbRows *rows,
                                                       int row,
                                                       MeasureConditionComponentModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < MEASURE_CONDITION_COMPONENT_REPOSITORY_COLS) {
        return 0;
    }

    *model = (MeasureConditionComponentModel){
        .measure_condition_sid = owned_value(rows, row, 0),
        .duty_expression_id = owned_value(rows, row, 1),
        .duty_amount = owned_value(rows, row, 2),
        .monetary_unit_code = owned_value(rows, row, 3),
        .measurement_unit_code = owned_value(rows, row, 4),
        .measurement_unit_qualifier_code = owned_value(rows, row, 5),
        .duty_expression_description = owned_value(rows, row, 6),
    };

    if (model->measure_condition_sid == NULL || model->duty_expression_id == NULL) {
        measure_condition_component_model_free(model);
        return 0;
    }

    return 1;
}

int measure_condition_component_repository_fetch_for_measure_condition_sids(const CommodityRequestContext *context,
                                                                            const char *const *measure_condition_sids,
                                                                            size_t sid_count,
                                                                            MeasureConditionComponentModelList *components)
{
    if (context == NULL || components == NULL) {
        return 0;
    }

    char *sid_param = measure_condition_component_repository_sid_array_param(measure_condition_sids, sid_count);
    char *sql = build_sql(context);
    if (sid_param == NULL || sql == NULL) {
        free(sid_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("measure_condition_components_for_condition_sids", sql, sid_param);
    free(sid_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        MeasureConditionComponentModel model = { 0 };
        if (!measure_condition_component_repository_hydrate_row(&rows, row, &model)) {
            measure_condition_component_model_list_free(components);
            db_rows_free(&rows);
            return 0;
        }
        if (!measure_condition_component_model_list_append(components, model)) {
            measure_condition_component_model_list_free(components);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int measure_condition_component_repository_self_test(void)
{
    const char *sids[] = { "789", "bad", "-456" };
    char *param = measure_condition_component_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));
    int ok = param != NULL && strcmp(param, "{789,-456}") == 0;
    free(param);
    return ok;
}
