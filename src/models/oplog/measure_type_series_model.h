#ifndef TRADE_TARIFF_C_MEASURE_TYPE_SERIES_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_TYPE_SERIES_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_type_series_id;
    char *validity_start_date;
    char *validity_end_date;
    char *measure_type_combination;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureTypeSeriesOplogModel;

typedef struct {
    MeasureTypeSeriesOplogModel *items;
    size_t len;
    size_t cap;
} MeasureTypeSeriesOplogModelList;

#endif
