#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_JSONAPI_RENDERER_H

#include "models/measurement_unit_model.h"

#include <yyjson.h>

yyjson_mut_val *measurement_unit_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                             const char *measurement_unit_code);
yyjson_mut_val *measurement_unit_jsonapi_resource(yyjson_mut_doc *doc,
                                                  const MeasurementUnitModel *unit);
int measurement_unit_jsonapi_renderer_self_test(void);

#endif
