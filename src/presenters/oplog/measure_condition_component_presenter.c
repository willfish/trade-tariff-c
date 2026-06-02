#include "presenters/oplog/measure_condition_component_presenter.h"

#include <string.h>

const char *measure_condition_component_oplog_presenter_resource_type(void)
{
    return "measure_condition_component";
}

const char *measure_condition_component_oplog_presenter_id(const MeasureConditionComponentOplogModel *model)
{
    return model == NULL ? NULL : model->measure_condition_sid;
}

int measure_condition_component_oplog_presenter_self_test(void)
{
    MeasureConditionComponentOplogModel model = { .measure_condition_sid = "id" };
    return strcmp(measure_condition_component_oplog_presenter_resource_type(), "measure_condition_component") == 0 &&
           strcmp(measure_condition_component_oplog_presenter_id(&model), "id") == 0;
}
