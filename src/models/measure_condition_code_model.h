#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_CODE_MODEL_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_CODE_MODEL_H

#include <stddef.h>

typedef struct {
    char *condition_code;
    char *description;
    char *validity_start_date;
    char *validity_end_date;
} MeasureConditionCodeModel;

typedef struct {
    MeasureConditionCodeModel *items;
    size_t len;
    size_t cap;
} MeasureConditionCodeModelList;

void measure_condition_code_model_free(MeasureConditionCodeModel *model);
void measure_condition_code_model_list_free(MeasureConditionCodeModelList *list);
int measure_condition_code_model_list_append(MeasureConditionCodeModelList *list,
                                             MeasureConditionCodeModel model);
int measure_condition_code_model_self_test(void);

#endif
