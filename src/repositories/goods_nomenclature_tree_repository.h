#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_TREE_REPOSITORY_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_TREE_REPOSITORY_H

#include "db.h"
#include "domain/goods_nomenclature_tree.h"

typedef struct {
    const char *schema;
    const char *actual_date;
    const char *commodity_id;
} GoodsNomenclatureTreeRepositoryQuery;

int goods_nomenclature_tree_repository_fetch_origin(const GoodsNomenclatureTreeRepositoryQuery *query,
                                                    GoodsNomenclatureTreeNode *node);
int goods_nomenclature_tree_repository_fetch_ancestors(const GoodsNomenclatureTreeRepositoryQuery *query,
                                                       GoodsNomenclatureTreeNodeList *ancestors);
int goods_nomenclature_tree_repository_hydrate_row(const DbRows *rows,
                                                   int row,
                                                   GoodsNomenclatureTreeNode *node);
int goods_nomenclature_tree_repository_self_test(void);

#endif
