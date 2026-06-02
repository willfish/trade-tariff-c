#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measurement_unit_code;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasurementUnitDescriptionOplogModel;

typedef struct {
    MeasurementUnitDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} MeasurementUnitDescriptionOplogModelList;

#endif
