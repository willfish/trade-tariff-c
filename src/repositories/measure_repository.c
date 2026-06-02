#include "repositories/measure_repository.h"

#include "sql_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEASURE_REPOSITORY_COLS 21

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
    if (value == NULL) {
        return NULL;
    }

    return copy_literal(value);
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

char *measure_repository_sid_array_param(const char *const *goods_nomenclature_sids,
                                         size_t sid_count)
{
    if (goods_nomenclature_sids == NULL || sid_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < sid_count; i++) {
        if (!is_numeric_sid(goods_nomenclature_sids[i])) {
            continue;
        }
        len += strlen(goods_nomenclature_sids[i]) + 1;
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
        if (!is_numeric_sid(goods_nomenclature_sids[i])) {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        size_t value_len = strlen(goods_nomenclature_sids[i]);
        memcpy(cursor, goods_nomenclature_sids[i], value_len);
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

static const char *excluded_measure_types_for_schema(const char *schema)
{
    if (schema != NULL && strcmp(schema, "xi") == 0) {
        return "'442','447','SPL','AHC','AIL','ATT','CEX','CHM','COE','COI','CVD','DPO','ECM','EHC','EQC','EWP','HOP','HSE','IWP','PHC','PRE','PRT','QRC','SFS','046','122','123','143','146','147','653','654'";
    }
    return "'442','447','SPL'";
}

static char *build_sql(const CommodityRequestContext *context)
{
    if (context == NULL || context->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load("sql/measures/for_goods_nomenclature_sids.sql");
    char *date = date_expression(context->actual_date);
    if (template == NULL || date == NULL) {
        free(template);
        free(date);
        return NULL;
    }

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", context->schema },
        { "ACTUAL_DATE", date },
        { "EXCLUDED_MEASURE_TYPES", excluded_measure_types_for_schema(context->schema) },
    };
    char *sql = sql_loader_apply_bindings(template, bindings, sizeof(bindings) / sizeof(bindings[0]));
    free(template);
    free(date);
    return sql;
}

int measure_repository_hydrate_row(const DbRows *rows, int row, MeasureModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < MEASURE_REPOSITORY_COLS) {
        return 0;
    }

    *model = (MeasureModel){
        .measure_sid = owned_value(rows, row, 0),
        .goods_nomenclature_sid = owned_value(rows, row, 1),
        .goods_nomenclature_item_id = owned_value(rows, row, 2),
        .measure_type_id = owned_value(rows, row, 3),
        .measure_type_description = owned_value(rows, row, 4),
        .geographical_area_sid = owned_value(rows, row, 5),
        .geographical_area_id = owned_value(rows, row, 6),
        .additional_code_sid = owned_value(rows, row, 7),
        .additional_code_type_id = owned_value(rows, row, 8),
        .additional_code_id = owned_value(rows, row, 9),
        .ordernumber = owned_value(rows, row, 10),
        .reduction_indicator = owned_value(rows, row, 11),
        .national = owned_value(rows, row, 12),
        .trade_movement_code = owned_value(rows, row, 13),
        .measure_type_series_id = owned_value(rows, row, 14),
        .measure_generating_regulation_role = owned_value(rows, row, 15),
        .measure_generating_regulation_id = owned_value(rows, row, 16),
        .validity_start_date = owned_value(rows, row, 17),
        .validity_end_date = owned_value(rows, row, 18),
        .effective_start_date = owned_value(rows, row, 19),
        .effective_end_date = owned_value(rows, row, 20),
    };

    if (model->measure_sid == NULL ||
        model->goods_nomenclature_sid == NULL ||
        model->measure_type_id == NULL ||
        model->geographical_area_id == NULL ||
        model->trade_movement_code == NULL ||
        model->validity_start_date == NULL ||
        model->effective_start_date == NULL) {
        measure_model_free(model);
        return 0;
    }

    return 1;
}

int measure_repository_fetch_for_goods_nomenclature_sids(const CommodityRequestContext *context,
                                                         const char *const *goods_nomenclature_sids,
                                                         size_t sid_count,
                                                         MeasureModelList *measures)
{
    if (context == NULL || measures == NULL) {
        return 0;
    }

    char *sid_param = measure_repository_sid_array_param(goods_nomenclature_sids, sid_count);
    char *sql = build_sql(context);
    if (sid_param == NULL || sql == NULL) {
        free(sid_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("measures_for_goods_nomenclature_sids", sql, sid_param);
    free(sid_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        MeasureModel model = { 0 };
        if (!measure_repository_hydrate_row(&rows, row, &model)) {
            measure_model_list_free(measures);
            db_rows_free(&rows);
            return 0;
        }
        if (!measure_model_list_append(measures, model)) {
            measure_model_list_free(measures);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int measure_repository_self_test(void)
{
    const char *sids[] = { "123", "bad", "456" };
    char *param = measure_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));
    int ok = param != NULL && strcmp(param, "{123,456}") == 0;
    free(param);
    return ok;
}
