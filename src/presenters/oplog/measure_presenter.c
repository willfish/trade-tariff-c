#include "presenters/oplog/measure_presenter.h"

#include <string.h>

const char *measure_oplog_presenter_resource_type(void)
{
    return "measure";
}

const char *measure_oplog_presenter_id(const MeasureOplogModel *model)
{
    return model == NULL ? NULL : model->measure_sid;
}

int measure_oplog_presenter_self_test(void)
{
    MeasureOplogModel model = { .measure_sid = "id" };
    return strcmp(measure_oplog_presenter_resource_type(), "measure") == 0 &&
           strcmp(measure_oplog_presenter_id(&model), "id") == 0;
}
