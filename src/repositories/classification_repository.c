#include "repositories/classification_repository.h"

#include "sql_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLASSIFICATION_REPOSITORY_COLS 16

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

    char *template = sql_loader_load("sql/classification/for_commodity_id.sql");
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

int classification_repository_hydrate_row(const DbRows *rows,
                                          int row,
                                          ClassificationModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < CLASSIFICATION_REPOSITORY_COLS) {
        return 0;
    }

    ClassificationSectionModel section = {
        .id = owned_value(rows, row, 0),
        .numeral = owned_value(rows, row, 1),
        .title = owned_value(rows, row, 2),
        .position = owned_value(rows, row, 3),
        .section_note = owned_value(rows, row, 4),
    };
    ClassificationChapterModel chapter = {
        .goods_nomenclature_sid = owned_value(rows, row, 5),
        .goods_nomenclature_item_id = owned_value(rows, row, 6),
        .description = owned_value(rows, row, 7),
        .validity_start_date = owned_value(rows, row, 8),
        .validity_end_date = owned_value(rows, row, 9),
        .chapter_note = owned_value(rows, row, 10),
    };
    ClassificationHeadingModel heading = {
        .goods_nomenclature_sid = owned_value(rows, row, 11),
        .goods_nomenclature_item_id = owned_value(rows, row, 12),
        .description = owned_value(rows, row, 13),
        .validity_start_date = owned_value(rows, row, 14),
        .validity_end_date = owned_value(rows, row, 15),
    };

    *model = (ClassificationModel){
        .section = section,
        .chapter = chapter,
        .heading = heading,
        .has_section = section.id != NULL,
        .has_chapter = chapter.goods_nomenclature_sid != NULL,
        .has_heading = heading.goods_nomenclature_sid != NULL,
    };

    if ((model->has_section && (model->section.id == NULL || model->section.numeral == NULL || model->section.title == NULL || model->section.position == NULL)) ||
        (model->has_chapter && (model->chapter.goods_nomenclature_sid == NULL || model->chapter.goods_nomenclature_item_id == NULL || model->chapter.validity_start_date == NULL)) ||
        (model->has_heading && (model->heading.goods_nomenclature_sid == NULL || model->heading.goods_nomenclature_item_id == NULL || model->heading.validity_start_date == NULL))) {
        classification_model_free(model);
        return 0;
    }

    return 1;
}

int classification_repository_fetch_for_commodity(const CommodityRequestContext *context,
                                                  ClassificationModel *model)
{
    if (context == NULL || context->commodity_id == NULL || model == NULL) {
        return 0;
    }

    char *sql = build_sql(context);
    if (sql == NULL) {
        return 0;
    }

    DbRows rows = db_fetch_rows("classification_for_commodity_id", sql, context->commodity_id);
    free(sql);
    if (!rows.ok || rows.rows == 0) {
        db_rows_free(&rows);
        return 0;
    }

    int ok = classification_repository_hydrate_row(&rows, 0, model);
    db_rows_free(&rows);
    return ok;
}

int classification_repository_self_test(void)
{
    return classification_model_self_test();
}
