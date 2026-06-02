#ifndef TRADE_TARIFF_C_MEASURE_EXCLUDED_GEOGRAPHICAL_AREA_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_EXCLUDED_GEOGRAPHICAL_AREA_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *excluded_geographical_area;
    char *geographical_area_sid;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureExcludedGeographicalAreaOplogModel;

typedef struct {
    MeasureExcludedGeographicalAreaOplogModel *items;
    size_t len;
    size_t cap;
} MeasureExcludedGeographicalAreaOplogModelList;

#endif
