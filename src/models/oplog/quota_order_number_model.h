#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_order_number_sid;
    char *quota_order_number_id;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaOrderNumberOplogModel;

typedef struct {
    QuotaOrderNumberOplogModel *items;
    size_t len;
    size_t cap;
} QuotaOrderNumberOplogModelList;

#endif
