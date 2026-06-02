#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_PERMUTATION_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_PERMUTATION_JSONAPI_RENDERER_H

#include "models/measure_condition_permutation_model.h"

#include <yyjson.h>

yyjson_mut_val *measure_condition_permutation_group_jsonapi_resource_identifier(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationGroupModel *group);
yyjson_mut_val *measure_condition_permutation_group_jsonapi_relationship_data_for_measure(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationGroupModelList *groups,
    const char *measure_sid);
yyjson_mut_val *measure_condition_permutation_group_jsonapi_resource(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationGroupModel *group);
yyjson_mut_val *measure_condition_permutation_jsonapi_resource(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationModel *permutation);
int measure_condition_permutation_jsonapi_renderer_self_test(void);

#endif
