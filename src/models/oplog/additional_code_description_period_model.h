#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_DESCRIPTION_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_DESCRIPTION_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *additional_code_description_period_sid;
    char *additional_code_sid;
    char *additional_code_type_id;
    char *additional_code;
    char *validity_start_date;
    char *created_at;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} AdditionalCodeDescriptionPeriodOplogModel;

typedef struct {
    AdditionalCodeDescriptionPeriodOplogModel *items;
    size_t len;
    size_t cap;
} AdditionalCodeDescriptionPeriodOplogModelList;

#endif
