#include "domain/goods_nomenclature_tree.h"

#include <stdlib.h>
#include <string.h>

void goods_nomenclature_tree_node_free(GoodsNomenclatureTreeNode *node)
{
    if (node == NULL) {
        return;
    }

    free(node->goods_nomenclature_sid);
    free(node->goods_nomenclature_item_id);
    free(node->producline_suffix);
    free(node->number_indents);
    free(node->position);
    free(node->depth);
    free(node->validity_start_date);
    free(node->validity_end_date);
    free(node->description);
    *node = (GoodsNomenclatureTreeNode){ 0 };
}

void goods_nomenclature_tree_node_list_free(GoodsNomenclatureTreeNodeList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        goods_nomenclature_tree_node_free(&list->items[i]);
    }
    free(list->items);
    *list = (GoodsNomenclatureTreeNodeList){ 0 };
}

int goods_nomenclature_tree_node_list_append(GoodsNomenclatureTreeNodeList *list,
                                             GoodsNomenclatureTreeNode node)
{
    if (list == NULL) {
        goods_nomenclature_tree_node_free(&node);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        GoodsNomenclatureTreeNode *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            goods_nomenclature_tree_node_free(&node);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = node;
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

int goods_nomenclature_tree_self_test(void)
{
    GoodsNomenclatureTreeNodeList list = { 0 };
    GoodsNomenclatureTreeNode node = {
        .goods_nomenclature_sid = copy_literal("107217"),
        .goods_nomenclature_item_id = copy_literal("8543400000"),
        .producline_suffix = copy_literal("80"),
        .number_indents = copy_literal("3"),
        .position = copy_literal("854340000080"),
        .depth = copy_literal("5"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .validity_end_date = NULL,
        .description = copy_literal("Electronic cigarettes"),
        .leaf = 1,
        .has_chemicals = 0,
    };

    int ok = goods_nomenclature_tree_node_list_append(&list, node) &&
             list.len == 1 &&
             strcmp(list.items[0].goods_nomenclature_item_id, "8543400000") == 0 &&
             list.items[0].leaf == 1;

    goods_nomenclature_tree_node_list_free(&list);
    return ok;
}
