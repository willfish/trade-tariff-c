#ifndef TRADE_TARIFF_C_MEASURE_TYPE_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_TYPE_JSONAPI_RENDERER_H

#include "models/measure_type_model.h"

#include <yyjson.h>

yyjson_mut_val *measure_type_jsonapi_resource(yyjson_mut_doc *doc,
                                              const MeasureTypeModel *measure_type);
int measure_type_jsonapi_renderer_self_test(void);

#endif
