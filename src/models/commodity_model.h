#ifndef TRADE_TARIFF_C_COMMODITY_RESOURCE_MODEL_H
#define TRADE_TARIFF_C_COMMODITY_RESOURCE_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *producline_suffix;
    char *description;
    char *number_indents;
    char *validity_start_date;
    char *validity_end_date;
} CommodityModel;

typedef struct {
    CommodityModel *items;
    size_t len;
    size_t cap;
} CommodityModelList;

#endif
