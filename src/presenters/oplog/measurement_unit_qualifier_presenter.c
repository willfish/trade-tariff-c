#include "presenters/oplog/measurement_unit_qualifier_presenter.h"

#include <string.h>

const char *measurement_unit_qualifier_oplog_presenter_resource_type(void)
{
    return "measurement_unit_qualifier";
}

const char *measurement_unit_qualifier_oplog_presenter_id(const MeasurementUnitQualifierOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int measurement_unit_qualifier_oplog_presenter_self_test(void)
{
    MeasurementUnitQualifierOplogModel model = { .oid = "id" };
    return strcmp(measurement_unit_qualifier_oplog_presenter_resource_type(), "measurement_unit_qualifier") == 0 &&
           strcmp(measurement_unit_qualifier_oplog_presenter_id(&model), "id") == 0;
}
