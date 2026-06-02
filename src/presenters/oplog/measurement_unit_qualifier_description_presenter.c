#include "presenters/oplog/measurement_unit_qualifier_description_presenter.h"

#include <string.h>

const char *measurement_unit_qualifier_description_oplog_presenter_resource_type(void)
{
    return "measurement_unit_qualifier_description";
}

const char *measurement_unit_qualifier_description_oplog_presenter_id(const MeasurementUnitQualifierDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->language_id;
}

int measurement_unit_qualifier_description_oplog_presenter_self_test(void)
{
    MeasurementUnitQualifierDescriptionOplogModel model = { .language_id = "id" };
    return strcmp(measurement_unit_qualifier_description_oplog_presenter_resource_type(), "measurement_unit_qualifier_description") == 0 &&
           strcmp(measurement_unit_qualifier_description_oplog_presenter_id(&model), "id") == 0;
}
