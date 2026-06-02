#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_condition_sid;
    char *duty_expression_id;
    char *duty_amount;
    char *monetary_unit_code;
    char *measurement_unit_code;
    char *measurement_unit_qualifier_code;
    char *duty_expression_description;
} MeasureConditionComponentModel;

typedef struct {
    MeasureConditionComponentModel *items;
    size_t len;
    size_t cap;
} MeasureConditionComponentModelList;

void measure_condition_component_model_free(MeasureConditionComponentModel *model);
void measure_condition_component_model_list_free(MeasureConditionComponentModelList *list);
int measure_condition_component_model_list_append(MeasureConditionComponentModelList *list,
                                                  MeasureConditionComponentModel model);
int measure_condition_component_model_self_test(void);

#endif
