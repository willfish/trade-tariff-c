#include "presenters/oplog/geographical_area_description_presenter.h"

#include <string.h>

const char *geographical_area_description_oplog_presenter_resource_type(void)
{
    return "geographical_area_description";
}

const char *geographical_area_description_oplog_presenter_id(const GeographicalAreaDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->geographical_area_description_period_sid;
}

int geographical_area_description_oplog_presenter_self_test(void)
{
    GeographicalAreaDescriptionOplogModel model = { .geographical_area_description_period_sid = "id" };
    return strcmp(geographical_area_description_oplog_presenter_resource_type(), "geographical_area_description") == 0 &&
           strcmp(geographical_area_description_oplog_presenter_id(&model), "id") == 0;
}
