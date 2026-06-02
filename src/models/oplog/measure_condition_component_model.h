#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_condition_sid;
    char *duty_expression_id;
    char *duty_amount;
    char *monetary_unit_code;
    char *measurement_unit_code;
    char *measurement_unit_qualifier_code;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureConditionComponentOplogModel;

typedef struct {
    MeasureConditionComponentOplogModel *items;
    size_t len;
    size_t cap;
} MeasureConditionComponentOplogModelList;

#endif
