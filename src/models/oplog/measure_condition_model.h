#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_condition_sid;
    char *measure_sid;
    char *condition_code;
    char *component_sequence_number;
    char *condition_duty_amount;
    char *condition_monetary_unit_code;
    char *condition_measurement_unit_code;
    char *condition_measurement_unit_qualifier_code;
    char *action_code;
    char *certificate_type_code;
    char *certificate_code;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureConditionOplogModel;

typedef struct {
    MeasureConditionOplogModel *items;
    size_t len;
    size_t cap;
} MeasureConditionOplogModelList;

#endif
