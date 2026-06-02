#ifndef TRADE_TARIFF_C_NOMENCLATURE_GROUP_MEMBERSHIP_OPLOG_MODEL_H
#define TRADE_TARIFF_C_NOMENCLATURE_GROUP_MEMBERSHIP_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_group_type;
    char *goods_nomenclature_group_id;
    char *validity_start_date;
    char *validity_end_date;
    char *goods_nomenclature_item_id;
    char *productline_suffix;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} NomenclatureGroupMembershipOplogModel;

typedef struct {
    NomenclatureGroupMembershipOplogModel *items;
    size_t len;
    size_t cap;
} NomenclatureGroupMembershipOplogModelList;

#endif
