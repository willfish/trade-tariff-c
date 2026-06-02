#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_CODE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_CODE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *condition_code;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureConditionCodeDescriptionOplogModel;

typedef struct {
    MeasureConditionCodeDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} MeasureConditionCodeDescriptionOplogModelList;

#endif
