#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_description_period_sid;
    char *language_id;
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *productline_suffix;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GoodsNomenclatureDescriptionOplogModel;

typedef struct {
    GoodsNomenclatureDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureDescriptionOplogModelList;

#endif
