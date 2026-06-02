#include "presenters/oplog/measure_condition_code_description_presenter.h"

#include <string.h>

const char *measure_condition_code_description_oplog_presenter_resource_type(void)
{
    return "measure_condition_code_description";
}

const char *measure_condition_code_description_oplog_presenter_id(const MeasureConditionCodeDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->language_id;
}

int measure_condition_code_description_oplog_presenter_self_test(void)
{
    MeasureConditionCodeDescriptionOplogModel model = { .language_id = "id" };
    return strcmp(measure_condition_code_description_oplog_presenter_resource_type(), "measure_condition_code_description") == 0 &&
           strcmp(measure_condition_code_description_oplog_presenter_id(&model), "id") == 0;
}
