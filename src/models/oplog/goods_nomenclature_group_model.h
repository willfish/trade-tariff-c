#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_GROUP_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_GROUP_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_group_type;
    char *goods_nomenclature_group_id;
    char *validity_start_date;
    char *validity_end_date;
    char *nomenclature_group_facility_code;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GoodsNomenclatureGroupOplogModel;

typedef struct {
    GoodsNomenclatureGroupOplogModel *items;
    size_t len;
    size_t cap;
} GoodsNomenclatureGroupOplogModelList;

#endif
