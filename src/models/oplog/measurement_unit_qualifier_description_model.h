#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measurement_unit_qualifier_code;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasurementUnitQualifierDescriptionOplogModel;

typedef struct {
    MeasurementUnitQualifierDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} MeasurementUnitQualifierDescriptionOplogModelList;

#endif
