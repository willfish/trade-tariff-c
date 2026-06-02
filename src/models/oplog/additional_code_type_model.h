#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_TYPE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_TYPE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *additional_code_type_id;
    char *validity_start_date;
    char *validity_end_date;
    char *application_code;
    char *meursing_table_plan_id;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} AdditionalCodeTypeOplogModel;

typedef struct {
    AdditionalCodeTypeOplogModel *items;
    size_t len;
    size_t cap;
} AdditionalCodeTypeOplogModelList;

#endif
