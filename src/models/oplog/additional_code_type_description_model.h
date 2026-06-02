#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_TYPE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_TYPE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *additional_code_type_id;
    char *language_id;
    char *description;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} AdditionalCodeTypeDescriptionOplogModel;

typedef struct {
    AdditionalCodeTypeDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} AdditionalCodeTypeDescriptionOplogModelList;

#endif
