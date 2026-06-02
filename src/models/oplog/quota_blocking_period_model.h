#ifndef TRADE_TARIFF_C_QUOTA_BLOCKING_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_BLOCKING_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_blocking_period_sid;
    char *quota_definition_sid;
    char *blocking_start_date;
    char *blocking_end_date;
    char *blocking_period_type;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaBlockingPeriodOplogModel;

typedef struct {
    QuotaBlockingPeriodOplogModel *items;
    size_t len;
    size_t cap;
} QuotaBlockingPeriodOplogModelList;

#endif
