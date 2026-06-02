#ifndef TRADE_TARIFF_C_QUOTA_UNSUSPENSION_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_UNSUSPENSION_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *unsuspension_date;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaUnsuspensionEventOplogModel;

typedef struct {
    QuotaUnsuspensionEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaUnsuspensionEventOplogModelList;

#endif
