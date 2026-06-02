#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_PRESENTER_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_PRESENTER_H

#include "models/measurement_unit_model.h"

const char *measurement_unit_presenter_resource_type(void);
const char *measurement_unit_presenter_id(const MeasurementUnitModel *model);
char *measurement_unit_presenter_verbose_expansion_prefix(const char *expansion);
char *measurement_unit_presenter_normalize_verbose_owned(char *value);
int measurement_unit_presenter_self_test(void);

#endif
