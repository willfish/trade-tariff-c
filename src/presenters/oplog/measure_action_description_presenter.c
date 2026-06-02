#include "presenters/oplog/measure_action_description_presenter.h"

#include <string.h>

const char *measure_action_description_oplog_presenter_resource_type(void)
{
    return "measure_action_description";
}

const char *measure_action_description_oplog_presenter_id(const MeasureActionDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->language_id;
}

int measure_action_description_oplog_presenter_self_test(void)
{
    MeasureActionDescriptionOplogModel model = { .language_id = "id" };
    return strcmp(measure_action_description_oplog_presenter_resource_type(), "measure_action_description") == 0 &&
           strcmp(measure_action_description_oplog_presenter_id(&model), "id") == 0;
}
