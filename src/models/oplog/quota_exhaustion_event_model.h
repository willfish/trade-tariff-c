#ifndef TRADE_TARIFF_C_QUOTA_EXHAUSTION_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_EXHAUSTION_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *exhaustion_date;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaExhaustionEventOplogModel;

typedef struct {
    QuotaExhaustionEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaExhaustionEventOplogModelList;

#endif
