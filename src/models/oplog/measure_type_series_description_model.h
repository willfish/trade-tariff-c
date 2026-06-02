#ifndef TRADE_TARIFF_C_MEASURE_TYPE_SERIES_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_TYPE_SERIES_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_type_series_id;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureTypeSeriesDescriptionOplogModel;

typedef struct {
    MeasureTypeSeriesDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} MeasureTypeSeriesDescriptionOplogModelList;

#endif
