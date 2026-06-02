#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_MODEL_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_MODEL_H

#include <stddef.h>

typedef struct {
    char *measurement_unit_qualifier_code;
    char *description;
} MeasurementUnitQualifierModel;

typedef struct {
    MeasurementUnitQualifierModel *items;
    size_t len;
    size_t cap;
} MeasurementUnitQualifierModelList;

void measurement_unit_qualifier_model_free(MeasurementUnitQualifierModel *model);
void measurement_unit_qualifier_model_list_free(MeasurementUnitQualifierModelList *list);
int measurement_unit_qualifier_model_list_append(MeasurementUnitQualifierModelList *list,
                                                 MeasurementUnitQualifierModel model);
int measurement_unit_qualifier_model_self_test(void);

#endif
