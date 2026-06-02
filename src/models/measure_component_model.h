#ifndef TRADE_TARIFF_C_MEASURE_COMPONENT_MODEL_H
#define TRADE_TARIFF_C_MEASURE_COMPONENT_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *duty_expression_id;
    char *duty_amount;
    char *monetary_unit_code;
    char *measurement_unit_code;
    char *measurement_unit_qualifier_code;
    char *duty_expression_description;
    char *resolved_for_measure_sid;
    int resolved_meursing;
} MeasureComponentModel;

typedef struct {
    MeasureComponentModel *items;
    size_t len;
    size_t cap;
} MeasureComponentModelList;

void measure_component_model_free(MeasureComponentModel *model);
void measure_component_model_list_free(MeasureComponentModelList *list);
int measure_component_model_list_append(MeasureComponentModelList *list,
                                        MeasureComponentModel model);
int measure_component_model_self_test(void);

#endif
