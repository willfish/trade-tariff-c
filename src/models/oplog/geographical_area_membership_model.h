#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_MEMBERSHIP_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_MEMBERSHIP_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *geographical_area_sid;
    char *geographical_area_group_sid;
    char *validity_start_date;
    char *validity_end_date;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
    char *hjid;
    char *geographical_area_hjid;
    char *geographical_area_group_hjid;
} GeographicalAreaMembershipOplogModel;

typedef struct {
    GeographicalAreaMembershipOplogModel *items;
    size_t len;
    size_t cap;
} GeographicalAreaMembershipOplogModelList;

#endif
