#ifndef TRADE_TARIFF_C_QUOTA_BALANCE_EVENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_BALANCE_EVENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *last_import_date_in_allocation;
    char *old_balance;
    char *new_balance;
    char *imported_amount;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaBalanceEventOplogModel;

typedef struct {
    QuotaBalanceEventOplogModel *items;
    size_t len;
    size_t cap;
} QuotaBalanceEventOplogModelList;

#endif
