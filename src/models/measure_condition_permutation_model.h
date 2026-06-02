#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_PERMUTATION_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_PERMUTATION_MODEL_H

#include <stddef.h>

typedef struct {
    char *id;
    char **measure_condition_sids;
    size_t measure_condition_sid_count;
    size_t measure_condition_sid_cap;
} MeasureConditionPermutationModel;

typedef struct {
    MeasureConditionPermutationModel *items;
    size_t len;
    size_t cap;
} MeasureConditionPermutationModelList;

typedef struct {
    char *id;
    char *measure_sid;
    char *condition_code;
    MeasureConditionPermutationModelList permutations;
} MeasureConditionPermutationGroupModel;

typedef struct {
    MeasureConditionPermutationGroupModel *items;
    size_t len;
    size_t cap;
} MeasureConditionPermutationGroupModelList;

void measure_condition_permutation_model_free(MeasureConditionPermutationModel *model);
void measure_condition_permutation_model_list_free(MeasureConditionPermutationModelList *list);
int measure_condition_permutation_model_list_append(MeasureConditionPermutationModelList *list,
                                                    MeasureConditionPermutationModel model);
int measure_condition_permutation_model_add_condition_sid(MeasureConditionPermutationModel *model,
                                                          const char *measure_condition_sid);
void measure_condition_permutation_group_model_free(MeasureConditionPermutationGroupModel *model);
void measure_condition_permutation_group_model_list_free(MeasureConditionPermutationGroupModelList *list);
int measure_condition_permutation_group_model_list_append(MeasureConditionPermutationGroupModelList *list,
                                                          MeasureConditionPermutationGroupModel model);
int measure_condition_permutation_model_self_test(void);

#endif
