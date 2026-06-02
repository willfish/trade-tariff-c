#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_CODE_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_CODE_JSONAPI_RENDERER_H

#include "models/measure_condition_code_model.h"

#include <yyjson.h>

yyjson_mut_val *measure_condition_code_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                   const char *condition_code);
yyjson_mut_val *measure_condition_code_jsonapi_resource(yyjson_mut_doc *doc,
                                                       const MeasureConditionCodeModel *condition_code);
int measure_condition_code_jsonapi_renderer_self_test(void);

#endif
