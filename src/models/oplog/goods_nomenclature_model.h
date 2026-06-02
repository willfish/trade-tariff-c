#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *producline_suffix;
    char *validity_start_date;
    char *validity_end_date;
    char *statistical_indicator;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
    char *path;
} GoodsNomenclatureOplogModel;

typedef struct {
    GoodsNomenclatureOplogModel *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureOplogModelList;

#endif
