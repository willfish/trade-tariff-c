#include "repositories/commodity_repository.h"

#include "repositories/oplog_table_repository.h"

#include <stdlib.h>
#include <string.h>

static char *owned_value(const DbRows *rows, int row, int col)
{
    if (db_rows_is_null(rows, row, col)) {
        return NULL;
    }

    const char *value = db_rows_value(rows, row, col);
    if (value == NULL) {
        return NULL;
    }
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

DbRows commodity_repository_fetch_measure_components(const char *schema, const char *measure_sid)
{
    OplogTableQuery query = {
        .schema = schema,
        .table = "measure_components",
        .columns = "measure_sid, duty_expression_id, duty_amount, monetary_unit_code, measurement_unit_code, measurement_unit_qualifier_code, NULL",
        .where_sql = "measure_sid = $1",
        .order_sql = " ORDER BY duty_expression_id",
    };
    return oplog_table_repository_fetch("commodity_measure_components", &query, measure_sid);
}

void commodity_repository_measure_component_free(MeasureComponentModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->measure_sid);
    free(model->duty_expression_id);
    free(model->duty_amount);
    free(model->monetary_unit_code);
    free(model->measurement_unit_code);
    free(model->measurement_unit_qualifier_code);
    free(model->duty_expression_description);
    *model = (MeasureComponentModel){ 0 };
}

int commodity_repository_hydrate_measure_component_row(const DbRows *rows,
                                                       int row,
                                                       MeasureComponentModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < 7) {
        return 0;
    }

    *model = (MeasureComponentModel){
        .measure_sid = owned_value(rows, row, 0),
        .duty_expression_id = owned_value(rows, row, 1),
        .duty_amount = owned_value(rows, row, 2),
        .monetary_unit_code = owned_value(rows, row, 3),
        .measurement_unit_code = owned_value(rows, row, 4),
        .measurement_unit_qualifier_code = owned_value(rows, row, 5),
        .duty_expression_description = owned_value(rows, row, 6),
    };

    if (model->measure_sid == NULL || model->duty_expression_id == NULL) {
        commodity_repository_measure_component_free(model);
        return 0;
    }

    return 1;
}

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

int commodity_repository_self_test(void)
{
    char *values[7] = {
        copy_literal("42"),
        copy_literal("01"),
        copy_literal("8.95"),
        copy_literal("GBP"),
        copy_literal("KGM"),
        NULL,
        copy_literal("%"),
    };
    unsigned char nulls[7] = { 0, 0, 0, 0, 0, 1, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 7,
        .error = NULL,
        .ok = 1,
    };
    MeasureComponentModel model = { 0 };
    int ok = commodity_repository_hydrate_measure_component_row(&rows, 0, &model) &&
             strcmp(model.measure_sid, "42") == 0 &&
             strcmp(model.duty_expression_id, "01") == 0 &&
             model.measurement_unit_qualifier_code == NULL;

    commodity_repository_measure_component_free(&model);
    for (int i = 0; i < 7; i++) {
        free(values[i]);
    }
    return ok;
}
