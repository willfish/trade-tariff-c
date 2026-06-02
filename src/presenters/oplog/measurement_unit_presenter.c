#include "presenters/oplog/measurement_unit_presenter.h"

#include <string.h>

const char *measurement_unit_oplog_presenter_resource_type(void)
{
    return "measurement_unit";
}

const char *measurement_unit_oplog_presenter_id(const MeasurementUnitOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int measurement_unit_oplog_presenter_self_test(void)
{
    MeasurementUnitOplogModel model = { .oid = "id" };
    return strcmp(measurement_unit_oplog_presenter_resource_type(), "measurement_unit") == 0 &&
           strcmp(measurement_unit_oplog_presenter_id(&model), "id") == 0;
}
