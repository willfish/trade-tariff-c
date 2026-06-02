#include "presenters/oplog/measure_type_presenter.h"

#include <string.h>

const char *measure_type_oplog_presenter_resource_type(void)
{
    return "measure_type";
}

const char *measure_type_oplog_presenter_id(const MeasureTypeOplogModel *model)
{
    return model == NULL ? NULL : model->measure_type_id;
}

int measure_type_oplog_presenter_self_test(void)
{
    MeasureTypeOplogModel model = { .measure_type_id = "id" };
    return strcmp(measure_type_oplog_presenter_resource_type(), "measure_type") == 0 &&
           strcmp(measure_type_oplog_presenter_id(&model), "id") == 0;
}
