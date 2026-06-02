#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_JSONAPI_RENDERER_H

#include "models/appendix_5a_model.h"
#include "models/measure_condition_component_model.h"
#include "models/measure_condition_model.h"

#include <yyjson.h>

yyjson_mut_val *measure_condition_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const MeasureConditionModel *condition);
yyjson_mut_val *measure_condition_jsonapi_relationship_data_for_measure(yyjson_mut_doc *doc,
                                                                        const MeasureConditionModelList *conditions,
                                                                        const char *measure_sid);
yyjson_mut_val *measure_condition_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const MeasureConditionModel *condition,
                                                   const MeasureConditionComponentModelList *components,
                                                   const Appendix5aModelList *appendix_5as);
int measure_condition_jsonapi_renderer_self_test(void);

#endif
