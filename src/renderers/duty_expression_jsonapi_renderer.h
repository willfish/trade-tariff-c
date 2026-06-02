#ifndef TRADE_TARIFF_C_DUTY_EXPRESSION_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_DUTY_EXPRESSION_JSONAPI_RENDERER_H

#include "models/measure_component_model.h"
#include "models/measure_model.h"

#include <yyjson.h>

yyjson_mut_val *duty_expression_jsonapi_resource(yyjson_mut_doc *doc,
                                                 const MeasureModel *measure,
                                                 const MeasureComponentModelList *components);
int duty_expression_jsonapi_renderer_self_test(void);

#endif
