#ifndef TRADE_TARIFF_C_QUOTA_CRITICAL_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_CRITICAL_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *critical_state;
    char *critical_state_change_date;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaCriticalEventOplogModel;

typedef struct {
    QuotaCriticalEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaCriticalEventOplogModelList;

#endif
