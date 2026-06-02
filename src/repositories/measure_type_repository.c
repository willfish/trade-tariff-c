#include "repositories/measure_type_repository.h"

#include "sql_loader.h"

#include <stdlib.h>
#include <string.h>

#define MEASURE_TYPE_REPOSITORY_COLS 9

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

char *measure_type_repository_id_array_param(const char *const *ids, size_t id_count)
{
    if (ids == NULL || id_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < id_count; i++) {
        if (ids[i] == NULL || ids[i][0] == '\0') {
            continue;
        }
        len += strlen(ids[i]) + 3;
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
    for (size_t i = 0; i < id_count; i++) {
        if (ids[i] == NULL || ids[i][0] == '\0') {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        *cursor++ = '"';
        size_t value_len = strlen(ids[i]);
        memcpy(cursor, ids[i], value_len);
        cursor += value_len;
        *cursor++ = '"';
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

    char *template = sql_loader_load("sql/measure_types/for_ids.sql");
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

int measure_type_repository_hydrate_row(const DbRows *rows, int row, MeasureTypeModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < MEASURE_TYPE_REPOSITORY_COLS) {
        return 0;
    }

    *model = (MeasureTypeModel){
        .measure_type_id = owned_value(rows, row, 0),
        .description = owned_value(rows, row, 1),
        .measure_type_series_id = owned_value(rows, row, 2),
        .measure_type_series_description = owned_value(rows, row, 3),
        .measure_component_applicable_code = owned_value(rows, row, 4),
        .order_number_capture_code = owned_value(rows, row, 5),
        .trade_movement_code = owned_value(rows, row, 6),
        .validity_start_date = owned_value(rows, row, 7),
        .validity_end_date = owned_value(rows, row, 8),
    };

    if (model->measure_type_id == NULL) {
        measure_type_model_free(model);
        return 0;
    }
    return 1;
}

int measure_type_repository_fetch_for_ids(const CommodityRequestContext *context,
                                          const char *const *ids,
                                          size_t id_count,
                                          MeasureTypeModelList *measure_types)
{
    if (context == NULL || measure_types == NULL) {
        return 0;
    }

    char *id_param = measure_type_repository_id_array_param(ids, id_count);
    char *sql = build_sql(context);
    if (id_param == NULL || sql == NULL) {
        free(id_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("measure_types_for_ids", sql, id_param);
    free(id_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        MeasureTypeModel model = { 0 };
        if (!measure_type_repository_hydrate_row(&rows, row, &model)) {
            measure_type_model_list_free(measure_types);
            db_rows_free(&rows);
            return 0;
        }
        if (!measure_type_model_list_append(measure_types, model)) {
            measure_type_model_list_free(measure_types);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int measure_type_repository_self_test(void)
{
    const char *ids[] = { "103", NULL, "", "305" };
    char *param = measure_type_repository_id_array_param(ids, sizeof(ids) / sizeof(ids[0]));
    int ok = param != NULL && strcmp(param, "{\"103\",\"305\"}") == 0;
    free(param);
    return ok;
}
