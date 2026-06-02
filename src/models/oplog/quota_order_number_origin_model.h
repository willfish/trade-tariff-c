#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_ORIGIN_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_ORIGIN_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_order_number_origin_sid;
    char *quota_order_number_sid;
    char *geographical_area_id;
    char *validity_start_date;
    char *validity_end_date;
    char *geographical_area_sid;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaOrderNumberOriginOplogModel;

typedef struct {
    QuotaOrderNumberOriginOplogModel *items;
    size_t len;
    size_t cap;
} QuotaOrderNumberOriginOplogModelList;

#endif
