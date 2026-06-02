#include "repositories/commodity_footnote_repository.h"

#include "sql_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMODITY_FOOTNOTE_REPOSITORY_COLS 3

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

char *commodity_footnote_repository_sid_array_param(const char *const *goods_nomenclature_sids,
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

static char *build_sql(const CommodityRequestContext *context)
{
    if (context == NULL || context->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load("sql/commodity_footnotes/for_goods_nomenclature_sids.sql");
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

int commodity_footnote_repository_hydrate_row(const DbRows *rows,
                                              int row,
                                              Footnote *footnote)
{
    if (rows == NULL || footnote == NULL || row < 0 || row >= rows->rows || rows->cols < COMMODITY_FOOTNOTE_REPOSITORY_COLS) {
        return 0;
    }

    *footnote = (Footnote){
        .measure_sid = NULL,
        .id = owned_value(rows, row, 0),
        .code = owned_value(rows, row, 1),
        .description = owned_value(rows, row, 2),
    };

    if (footnote->id == NULL || footnote->code == NULL) {
        footnote_free(footnote);
        return 0;
    }

    return 1;
}

int commodity_footnote_repository_fetch_for_goods_nomenclature_sids(const CommodityRequestContext *context,
                                                                    const char *const *goods_nomenclature_sids,
                                                                    size_t sid_count,
                                                                    FootnoteList *footnotes)
{
    if (context == NULL || footnotes == NULL) {
        return 0;
    }

    char *sid_param = commodity_footnote_repository_sid_array_param(goods_nomenclature_sids, sid_count);
    char *sql = build_sql(context);
    if (sid_param == NULL || sql == NULL) {
        free(sid_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("commodity_footnotes_for_goods_nomenclature_sids", sql, sid_param);
    free(sid_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        Footnote footnote = { 0 };
        if (!commodity_footnote_repository_hydrate_row(&rows, row, &footnote) ||
            !footnote_list_append(footnotes, footnote)) {
            footnote_list_free(footnotes);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int commodity_footnote_repository_self_test(void)
{
    const char *sids[] = { "123", "bad", "456" };
    char *param = commodity_footnote_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));
    int ok = param != NULL && strcmp(param, "{123,456}") == 0;
    free(param);
    return ok;
}
