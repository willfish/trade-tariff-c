#ifndef TRADE_TARIFF_C_QUOTA_ASSOCIATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_ASSOCIATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *main_quota_definition_sid;
    char *sub_quota_definition_sid;
    char *relation_type;
    char *coefficient;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaAssociationOplogModel;

typedef struct {
    QuotaAssociationOplogModel *items;
    size_t len;
    size_t cap;
} QuotaAssociationOplogModelList;

#endif
