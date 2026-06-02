#ifndef TRADE_TARIFF_C_QUOTA_SUSPENSION_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_SUSPENSION_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_suspension_period_sid;
    char *quota_definition_sid;
    char *suspension_start_date;
    char *suspension_end_date;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaSuspensionPeriodOplogModel;

typedef struct {
    QuotaSuspensionPeriodOplogModel *items;
    size_t len;
    size_t cap;
} QuotaSuspensionPeriodOplogModelList;

#endif
