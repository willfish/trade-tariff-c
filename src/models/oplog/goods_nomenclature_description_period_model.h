#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_DESCRIPTION_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_DESCRIPTION_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_description_period_sid;
    char *goods_nomenclature_sid;
    char *validity_start_date;
    char *goods_nomenclature_item_id;
    char *productline_suffix;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GoodsNomenclatureDescriptionPeriodOplogModel;

typedef struct {
    GoodsNomenclatureDescriptionPeriodOplogModel *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureDescriptionPeriodOplogModelList;

#endif
