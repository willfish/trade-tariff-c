#ifndef TRADE_TARIFF_C_MEASURE_COMPONENT_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_COMPONENT_JSONAPI_RENDERER_H

#include "models/measure_component_model.h"

#include <yyjson.h>

char *measure_component_jsonapi_id(const MeasureComponentModel *component);
yyjson_mut_val *measure_component_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const MeasureComponentModel *component);
yyjson_mut_val *measure_component_jsonapi_relationship_data_for_measure(yyjson_mut_doc *doc,
                                                                        const MeasureComponentModelList *components,
                                                                        const char *measure_sid);
yyjson_mut_val *measure_component_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const MeasureComponentModel *component);
int measure_component_jsonapi_renderer_self_test(void);

#endif
