#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *additional_code_description_period_sid;
    char *language_id;
    char *additional_code_sid;
    char *additional_code_type_id;
    char *additional_code;
    char *description;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} AdditionalCodeDescriptionOplogModel;

typedef struct {
    AdditionalCodeDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} AdditionalCodeDescriptionOplogModelList;

#endif
