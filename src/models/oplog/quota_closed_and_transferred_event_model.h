#ifndef TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *oid;
    char *quota_definition_sid;
    char *target_quota_definition_sid;
    char *occurrence_timestamp;
    char *operation;
    char *operation_date;
    char *transferred_amount;
    char *closing_date;
    char *filename;
} QuotaClosedAndTransferredEventOplogModel;

typedef struct {
    QuotaClosedAndTransferredEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaClosedAndTransferredEventOplogModelList;

#endif
