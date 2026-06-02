#ifndef TRADE_TARIFF_C_MEASURE_ACTION_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_ACTION_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *action_code;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureActionDescriptionOplogModel;

typedef struct {
    MeasureActionDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} MeasureActionDescriptionOplogModelList;

#endif
