#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_ORIGIN_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_ORIGIN_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *derived_goods_nomenclature_item_id;
    char *derived_productline_suffix;
    char *goods_nomenclature_item_id;
    char *productline_suffix;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GoodsNomenclatureOriginOplogModel;

typedef struct {
    GoodsNomenclatureOriginOplogModel *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureOriginOplogModelList;

#endif
