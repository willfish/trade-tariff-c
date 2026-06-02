#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_PRESENTER_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_PRESENTER_H

#include "models/measurement_unit_qualifier_model.h"

const char *measurement_unit_qualifier_presenter_resource_type(void);
const char *measurement_unit_qualifier_presenter_id(const MeasurementUnitQualifierModel *model);
char *measurement_unit_qualifier_presenter_formatted_description(const MeasurementUnitQualifierModel *model,
                                                                 const char *service);
int measurement_unit_qualifier_presenter_self_test(void);

#endif
