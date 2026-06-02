#ifndef TRADE_TARIFF_C_QUOTA_REOPENING_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_REOPENING_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *reopening_date;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaReopeningEventOplogModel;

typedef struct {
    QuotaReopeningEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaReopeningEventOplogModelList;

#endif
