#ifndef TRADE_TARIFF_C_QUOTA_DEFINITION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_QUOTA_DEFINITION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *quota_order_number_id;
    char *validity_start_date;
    char *validity_end_date;
    char *quota_order_number_sid;
    char *volume;
    char *initial_volume;
    char *measurement_unit_code;
    char *maximum_precision;
    char *critical_state;
    char *critical_threshold;
    char *monetary_unit_code;
    char *measurement_unit_qualifier_code;
    char *description;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} QuotaDefinitionOplogModel;

typedef struct {
    QuotaDefinitionOplogModel *items;
    size_t len;
    size_t cap;
} QuotaDefinitionOplogModelList;

#endif
