#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_JSONAPI_RENDERER_H

#include "models/measure_condition_component_model.h"

#include <yyjson.h>

char *measure_condition_component_jsonapi_id(const MeasureConditionComponentModel *component);
yyjson_mut_val *measure_condition_component_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                        const MeasureConditionComponentModel *component);
yyjson_mut_val *measure_condition_component_jsonapi_relationship_data_for_condition(yyjson_mut_doc *doc,
                                                                                    const MeasureConditionComponentModelList *components,
                                                                                    const char *measure_condition_sid);
yyjson_mut_val *measure_condition_component_jsonapi_resource(yyjson_mut_doc *doc,
                                                             const MeasureConditionComponentModel *component);
int measure_condition_component_jsonapi_renderer_self_test(void);

#endif
