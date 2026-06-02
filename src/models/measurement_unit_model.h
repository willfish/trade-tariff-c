#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_MODEL_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_MODEL_H

#include <stddef.h>

typedef struct {
    char *measurement_unit_code;
    char *description;
    char *abbreviation;
} MeasurementUnitModel;

typedef struct {
    MeasurementUnitModel *items;
    size_t len;
    size_t cap;
} MeasurementUnitModelList;

void measurement_unit_model_free(MeasurementUnitModel *model);
void measurement_unit_model_list_free(MeasurementUnitModelList *list);
int measurement_unit_model_list_append(MeasurementUnitModelList *list,
                                       MeasurementUnitModel model);
int measurement_unit_model_self_test(void);

#endif
