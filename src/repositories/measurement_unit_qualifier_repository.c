#include "repositories/measurement_unit_qualifier_repository.h"

#include "sql_loader.h"

#include <stdlib.h>
#include <string.h>

#define MEASUREMENT_UNIT_QUALIFIER_REPOSITORY_COLS 2

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

char *measurement_unit_qualifier_repository_code_array_param(const char *const *codes,
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

static char *build_sql(const CommodityRequestContext *context)
{
    if (context == NULL || context->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load("sql/measurement_unit_qualifiers/for_codes.sql");
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

int measurement_unit_qualifier_repository_hydrate_row(const DbRows *rows,
                                                      int row,
                                                      MeasurementUnitQualifierModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < MEASUREMENT_UNIT_QUALIFIER_REPOSITORY_COLS) {
        return 0;
    }

    *model = (MeasurementUnitQualifierModel){
        .measurement_unit_qualifier_code = owned_value(rows, row, 0),
        .description = owned_value(rows, row, 1),
    };

    if (model->measurement_unit_qualifier_code == NULL) {
        measurement_unit_qualifier_model_free(model);
        return 0;
    }

    return 1;
}

int measurement_unit_qualifier_repository_fetch_for_codes(const CommodityRequestContext *context,
                                                          const char *const *codes,
                                                          size_t code_count,
                                                          MeasurementUnitQualifierModelList *qualifiers)
{
    if (context == NULL || qualifiers == NULL) {
        return 0;
    }

    char *code_param = measurement_unit_qualifier_repository_code_array_param(codes, code_count);
    char *sql = build_sql(context);
    if (code_param == NULL || sql == NULL) {
        free(code_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("measurement_unit_qualifiers_for_codes", sql, code_param);
    free(code_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        MeasurementUnitQualifierModel model = { 0 };
        if (!measurement_unit_qualifier_repository_hydrate_row(&rows, row, &model)) {
            measurement_unit_qualifier_model_list_free(qualifiers);
            db_rows_free(&rows);
            return 0;
        }
        if (!measurement_unit_qualifier_model_list_append(qualifiers, model)) {
            measurement_unit_qualifier_model_list_free(qualifiers);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int measurement_unit_qualifier_repository_self_test(void)
{
    const char *codes[] = { "X", NULL, "", "A" };
    char *param = measurement_unit_qualifier_repository_code_array_param(codes, sizeof(codes) / sizeof(codes[0]));
    int ok = param != NULL && strcmp(param, "{\"X\",\"A\"}") == 0;
    free(param);
    return ok;
}
