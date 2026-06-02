#ifndef TRADE_TARIFF_C_MEASURE_ACTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_ACTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *action_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureActionOplogModel;

typedef struct {
    MeasureActionOplogModel *items;
    size_t len;
    size_t cap;
} MeasureActionOplogModelList;

#endif
