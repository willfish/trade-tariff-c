#include "presenters/oplog/measure_component_presenter.h"

#include <string.h>

const char *measure_component_oplog_presenter_resource_type(void)
{
    return "measure_component";
}

const char *measure_component_oplog_presenter_id(const MeasureComponentOplogModel *model)
{
    return model == NULL ? NULL : model->measure_sid;
}

int measure_component_oplog_presenter_self_test(void)
{
    MeasureComponentOplogModel model = { .measure_sid = "id" };
    return strcmp(measure_component_oplog_presenter_resource_type(), "measure_component") == 0 &&
           strcmp(measure_component_oplog_presenter_id(&model), "id") == 0;
}
