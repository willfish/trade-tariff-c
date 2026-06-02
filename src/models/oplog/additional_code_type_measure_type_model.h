#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_TYPE_MEASURE_TYPE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_TYPE_MEASURE_TYPE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_type_id;
    char *additional_code_type_id;
    char *validity_start_date;
    char *validity_end_date;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} AdditionalCodeTypeMeasureTypeOplogModel;

typedef struct {
    AdditionalCodeTypeMeasureTypeOplogModel *items;
    size_t len;
    size_t cap;
} AdditionalCodeTypeMeasureTypeOplogModelList;

#endif
