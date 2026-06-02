#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measurement_unit_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasurementUnitOplogModel;

typedef struct {
    MeasurementUnitOplogModel *items;
    size_t len;
    size_t cap;
} MeasurementUnitOplogModelList;

#endif
