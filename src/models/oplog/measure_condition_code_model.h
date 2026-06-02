#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_CODE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_CODE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *condition_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureConditionCodeOplogModel;

typedef struct {
    MeasureConditionCodeOplogModel *items;
    size_t len;
    size_t cap;
} MeasureConditionCodeOplogModelList;

#endif
