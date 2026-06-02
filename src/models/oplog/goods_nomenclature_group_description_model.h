#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_GROUP_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_GROUP_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_group_type;
    char *goods_nomenclature_group_id;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GoodsNomenclatureGroupDescriptionOplogModel;

typedef struct {
    GoodsNomenclatureGroupDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureGroupDescriptionOplogModelList;

#endif
