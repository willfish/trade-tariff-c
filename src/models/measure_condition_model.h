#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_MODEL_H

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
    char *certificate_type_code;
    char *certificate_code;
    char *action_code;
    char *action;
    char *measure_condition_code_description;
    char *certificate_type_description;
    char *certificate_description;
    char *measurement_unit_description;
    char *measurement_unit_qualifier_description;
} MeasureConditionModel;

typedef struct {
    MeasureConditionModel *items;
    size_t len;
    size_t cap;
} MeasureConditionModelList;

void measure_condition_model_free(MeasureConditionModel *model);
void measure_condition_model_list_free(MeasureConditionModelList *list);
int measure_condition_model_list_append(MeasureConditionModelList *list,
                                        MeasureConditionModel model);
int measure_condition_model_self_test(void);

#endif
