#include "presenters/oplog/geographical_area_description_period_presenter.h"

#include <string.h>

const char *geographical_area_description_period_oplog_presenter_resource_type(void)
{
    return "geographical_area_description_period";
}

const char *geographical_area_description_period_oplog_presenter_id(const GeographicalAreaDescriptionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->geographical_area_description_period_sid;
}

int geographical_area_description_period_oplog_presenter_self_test(void)
{
    GeographicalAreaDescriptionPeriodOplogModel model = { .geographical_area_description_period_sid = "id" };
    return strcmp(geographical_area_description_period_oplog_presenter_resource_type(), "geographical_area_description_period") == 0 &&
           strcmp(geographical_area_description_period_oplog_presenter_id(&model), "id") == 0;
}
