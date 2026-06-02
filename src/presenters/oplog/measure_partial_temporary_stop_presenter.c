#include "presenters/oplog/measure_partial_temporary_stop_presenter.h"

#include <string.h>

const char *measure_partial_temporary_stop_oplog_presenter_resource_type(void)
{
    return "measure_partial_temporary_stop";
}

const char *measure_partial_temporary_stop_oplog_presenter_id(const MeasurePartialTemporaryStopOplogModel *model)
{
    return model == NULL ? NULL : model->measure_sid;
}

int measure_partial_temporary_stop_oplog_presenter_self_test(void)
{
    MeasurePartialTemporaryStopOplogModel model = { .measure_sid = "id" };
    return strcmp(measure_partial_temporary_stop_oplog_presenter_resource_type(), "measure_partial_temporary_stop") == 0 &&
           strcmp(measure_partial_temporary_stop_oplog_presenter_id(&model), "id") == 0;
}
