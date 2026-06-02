#ifndef TRADE_TARIFF_C_MEASURE_COMPONENT_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_COMPONENT_PRESENTER_H

#include "models/measure_component_model.h"

const char *measure_component_presenter_resource_type(void);
char *measure_component_presenter_id(const MeasureComponentModel *model);
const char *measure_component_presenter_duty_expression_abbreviation(const MeasureComponentModel *model);
int measure_component_presenter_self_test(void);

#endif
