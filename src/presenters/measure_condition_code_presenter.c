#include "presenters/measure_condition_code_presenter.h"

#include <string.h>

const char *measure_condition_code_presenter_resource_type(void)
{
    return "measure_condition_code";
}

const char *measure_condition_code_presenter_id(const MeasureConditionCodeModel *model)
{
    return model == NULL ? NULL : model->condition_code;
}

int measure_condition_code_presenter_self_test(void)
{
    MeasureConditionCodeModel model = { .condition_code = "B" };
    return strcmp(measure_condition_code_presenter_resource_type(), "measure_condition_code") == 0 &&
           strcmp(measure_condition_code_presenter_id(&model), "B") == 0 &&
           measure_condition_code_presenter_id(NULL) == NULL;
}
