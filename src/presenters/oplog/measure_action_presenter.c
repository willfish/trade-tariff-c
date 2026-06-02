#include "presenters/oplog/measure_action_presenter.h"

#include <string.h>

const char *measure_action_oplog_presenter_resource_type(void)
{
    return "measure_action";
}

const char *measure_action_oplog_presenter_id(const MeasureActionOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int measure_action_oplog_presenter_self_test(void)
{
    MeasureActionOplogModel model = { .oid = "id" };
    return strcmp(measure_action_oplog_presenter_resource_type(), "measure_action") == 0 &&
           strcmp(measure_action_oplog_presenter_id(&model), "id") == 0;
}
