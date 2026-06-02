#ifndef TRADE_TARIFF_C_MEASURE_TYPE_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_TYPE_PRESENTER_H

#include "models/measure_type_model.h"

typedef struct {
    int is_null;
    int is_int;
    long int_value;
    const char *string_value;
} MeasureTypePresentedCode;

const char *measure_type_presenter_resource_type(void);
const char *measure_type_presenter_id(const MeasureTypeModel *model);
MeasureTypePresentedCode measure_type_presenter_code(const char *value);
int measure_type_presenter_self_test(void);

#endif
