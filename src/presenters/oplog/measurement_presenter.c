#include "presenters/oplog/measurement_presenter.h"

#include <string.h>

const char *measurement_oplog_presenter_resource_type(void)
{
    return "measurement";
}

const char *measurement_oplog_presenter_id(const MeasurementOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int measurement_oplog_presenter_self_test(void)
{
    MeasurementOplogModel model = { .oid = "id" };
    return strcmp(measurement_oplog_presenter_resource_type(), "measurement") == 0 &&
           strcmp(measurement_oplog_presenter_id(&model), "id") == 0;
}
