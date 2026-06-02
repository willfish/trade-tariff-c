#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_ORIGIN_EXCLUSION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_ORIGIN_EXCLUSION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_order_number_origin_sid;
    char *excluded_geographical_area_sid;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaOrderNumberOriginExclusionOplogModel;

typedef struct {
    QuotaOrderNumberOriginExclusionOplogModel *items;
    size_t len;
    size_t cap;
} QuotaOrderNumberOriginExclusionOplogModelList;

#endif
