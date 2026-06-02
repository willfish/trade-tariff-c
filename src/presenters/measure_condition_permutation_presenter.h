#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_PERMUTATION_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_PERMUTATION_PRESENTER_H

#include "models/measure_condition_model.h"
#include "models/measure_condition_permutation_model.h"

const char *measure_condition_permutation_presenter_resource_type(void);
const char *measure_condition_permutation_presenter_id(const MeasureConditionPermutationModel *model);
const char *measure_condition_permutation_group_presenter_resource_type(void);
const char *measure_condition_permutation_group_presenter_id(const MeasureConditionPermutationGroupModel *model);
int measure_condition_permutation_presenter_build_groups(const MeasureConditionModelList *conditions,
                                                         MeasureConditionPermutationGroupModelList *out);
int measure_condition_permutation_presenter_self_test(void);

#endif
