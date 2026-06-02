#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_CODE_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_CODE_PRESENTER_H

#include "models/measure_condition_code_model.h"

const char *measure_condition_code_presenter_resource_type(void);
const char *measure_condition_code_presenter_id(const MeasureConditionCodeModel *model);
int measure_condition_code_presenter_self_test(void);

#endif
