#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_TREE_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_TREE_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *producline_suffix;
    char *number_indents;
    char *position;
    char *depth;
    char *validity_start_date;
    char *validity_end_date;
    char *description;
    int leaf;
    int has_chemicals;
} GoodsNomenclatureTreeNode;

typedef struct {
    GoodsNomenclatureTreeNode *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureTreeNodeList;

void goods_nomenclature_tree_node_free(GoodsNomenclatureTreeNode *node);
void goods_nomenclature_tree_node_list_free(GoodsNomenclatureTreeNodeList *list);
int goods_nomenclature_tree_node_list_append(GoodsNomenclatureTreeNodeList *list,
                                             GoodsNomenclatureTreeNode node);
int goods_nomenclature_tree_node_is_ten_digit_goods_nomenclature(const GoodsNomenclatureTreeNode *node);
int goods_nomenclature_tree_self_test(void);

#endif
