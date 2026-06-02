#include "repositories/goods_nomenclature_tree_repository.h"

#include "sql_loader.h"

#include <stdio.h>
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

static char *build_sql(const char *relative_path, const GoodsNomenclatureTreeRepositoryQuery *query)
{
    if (relative_path == NULL || query == NULL || query->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load(relative_path);
    char *date = date_expression(query->actual_date);
    if (template == NULL || date == NULL) {
        free(template);
        free(date);
        return NULL;
    }

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", query->schema },
        { "ACTUAL_DATE", date },
    };
    char *sql = sql_loader_apply_bindings(template, bindings, sizeof(bindings) / sizeof(bindings[0]));
    free(template);
    free(date);
    return sql;
}

int goods_nomenclature_tree_repository_hydrate_row(const DbRows *rows,
                                                   int row,
                                                   GoodsNomenclatureTreeNode *node)
{
    if (rows == NULL || node == NULL || row < 0 || row >= rows->rows || rows->cols < 10) {
        return 0;
    }

    int leaf_col = 9;
    int has_chemicals_col = 10;
    *node = (GoodsNomenclatureTreeNode){
        .goods_nomenclature_sid = owned_value(rows, row, 0),
        .goods_nomenclature_item_id = owned_value(rows, row, 1),
        .producline_suffix = owned_value(rows, row, 2),
        .number_indents = owned_value(rows, row, 3),
        .position = owned_value(rows, row, 4),
        .depth = owned_value(rows, row, 5),
        .validity_start_date = owned_value(rows, row, 6),
        .validity_end_date = owned_value(rows, row, 7),
        .description = owned_value(rows, row, 8),
        .leaf = strcmp(db_rows_value(rows, row, leaf_col) == NULL ? "0" : db_rows_value(rows, row, leaf_col), "1") == 0,
        .has_chemicals = rows->cols > has_chemicals_col &&
                         strcmp(db_rows_value(rows, row, has_chemicals_col) == NULL ? "0" : db_rows_value(rows, row, has_chemicals_col), "1") == 0,
    };

    if (node->goods_nomenclature_sid == NULL ||
        node->goods_nomenclature_item_id == NULL ||
        node->producline_suffix == NULL ||
        node->number_indents == NULL ||
        node->position == NULL ||
        node->depth == NULL ||
        node->validity_start_date == NULL) {
        goods_nomenclature_tree_node_free(node);
        return 0;
    }

    return 1;
}

int goods_nomenclature_tree_repository_fetch_origin(const GoodsNomenclatureTreeRepositoryQuery *query,
                                                    GoodsNomenclatureTreeNode *node)
{
    if (query == NULL || query->commodity_id == NULL || node == NULL) {
        return 0;
    }

    char *sql = build_sql("sql/goods_tree/origin.sql", query);
    if (sql == NULL) {
        return 0;
    }

    DbRows rows = db_fetch_rows("goods_tree_origin", sql, query->commodity_id);
    free(sql);
    if (!rows.ok || rows.rows == 0) {
        db_rows_free(&rows);
        return 0;
    }

    int ok = goods_nomenclature_tree_repository_hydrate_row(&rows, 0, node);
    db_rows_free(&rows);
    return ok;
}

int goods_nomenclature_tree_repository_fetch_ancestors(const GoodsNomenclatureTreeRepositoryQuery *query,
                                                       GoodsNomenclatureTreeNodeList *ancestors)
{
    if (query == NULL || query->commodity_id == NULL || ancestors == NULL) {
        return 0;
    }

    char *sql = build_sql("sql/goods_tree/ancestors.sql", query);
    if (sql == NULL) {
        return 0;
    }

    DbRows rows = db_fetch_rows("goods_tree_ancestors", sql, query->commodity_id);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        GoodsNomenclatureTreeNode node = { 0 };
        if (!goods_nomenclature_tree_repository_hydrate_row(&rows, row, &node)) {
            goods_nomenclature_tree_node_list_free(ancestors);
            db_rows_free(&rows);
            return 0;
        }
        if (!goods_nomenclature_tree_node_list_append(ancestors, node)) {
            goods_nomenclature_tree_node_list_free(ancestors);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int goods_nomenclature_tree_repository_self_test(void)
{
    char *values[11] = {
        copy_literal("123456"),
        copy_literal("8543400000"),
        copy_literal("80"),
        copy_literal("3"),
        copy_literal("854340000080"),
        copy_literal("5"),
        copy_literal("2024-01-01T00:00:00.000Z"),
        NULL,
        copy_literal("Electronic cigarettes"),
        copy_literal("1"),
        copy_literal("0"),
    };
    unsigned char nulls[11] = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 11,
        .error = NULL,
        .ok = 1,
    };

    GoodsNomenclatureTreeNode node = { 0 };
    GoodsNomenclatureTreeNodeList list = { 0 };
    int ok = goods_nomenclature_tree_repository_hydrate_row(&rows, 0, &node) &&
             node.validity_end_date == NULL &&
             node.leaf == 1 &&
             node.has_chemicals == 0 &&
             goods_nomenclature_tree_node_list_append(&list, node) &&
             list.len == 1 &&
             strcmp(list.items[0].goods_nomenclature_item_id, "8543400000") == 0;

    if (!ok) {
        goods_nomenclature_tree_node_free(&node);
    }
    goods_nomenclature_tree_node_list_free(&list);
    for (int i = 0; i < 11; i++) {
        free(values[i]);
    }
    return ok;
}
