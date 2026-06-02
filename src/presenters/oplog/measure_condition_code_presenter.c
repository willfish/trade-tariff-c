#include "presenters/oplog/measure_condition_code_presenter.h"

#include <string.h>

const char *measure_condition_code_oplog_presenter_resource_type(void)
{
    return "measure_condition_code";
}

const char *measure_condition_code_oplog_presenter_id(const MeasureConditionCodeOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int measure_condition_code_oplog_presenter_self_test(void)
{
    MeasureConditionCodeOplogModel model = { .oid = "id" };
    return strcmp(measure_condition_code_oplog_presenter_resource_type(), "measure_condition_code") == 0 &&
           strcmp(measure_condition_code_oplog_presenter_id(&model), "id") == 0;
}
