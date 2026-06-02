#ifndef TRADE_TARIFF_C_QUOTA_UNBLOCKING_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_UNBLOCKING_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *unblocking_date;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaUnblockingEventOplogModel;

typedef struct {
    QuotaUnblockingEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaUnblockingEventOplogModelList;

#endif
