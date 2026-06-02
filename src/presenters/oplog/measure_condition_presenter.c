#include "presenters/oplog/measure_condition_presenter.h"

#include <string.h>

const char *measure_condition_oplog_presenter_resource_type(void)
{
    return "measure_condition";
}

const char *measure_condition_oplog_presenter_id(const MeasureConditionOplogModel *model)
{
    return model == NULL ? NULL : model->measure_condition_sid;
}

int measure_condition_oplog_presenter_self_test(void)
{
    MeasureConditionOplogModel model = { .measure_condition_sid = "id" };
    return strcmp(measure_condition_oplog_presenter_resource_type(), "measure_condition") == 0 &&
           strcmp(measure_condition_oplog_presenter_id(&model), "id") == 0;
}
