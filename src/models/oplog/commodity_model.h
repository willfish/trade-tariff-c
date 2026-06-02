#ifndef TRADE_TARIFF_C_COMMODITY_OPLOG_MODEL_H
#define TRADE_TARIFF_C_COMMODITY_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *producline_suffix;
    char *validity_start_date;
    char *validity_end_date;
    char *statistical_indicator;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} CommodityOplogModel;

typedef struct {
    CommodityOplogModel *items;
    size_t len;
    size_t cap;
} CommodityOplogModelList;

#endif
