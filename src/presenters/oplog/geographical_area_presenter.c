#include "presenters/oplog/geographical_area_presenter.h"

#include <string.h>

const char *geographical_area_oplog_presenter_resource_type(void)
{
    return "geographical_area";
}

const char *geographical_area_oplog_presenter_id(const GeographicalAreaOplogModel *model)
{
    return model == NULL ? NULL : model->geographical_area_sid;
}

int geographical_area_oplog_presenter_self_test(void)
{
    GeographicalAreaOplogModel model = { .geographical_area_sid = "id" };
    return strcmp(geographical_area_oplog_presenter_resource_type(), "geographical_area") == 0 &&
           strcmp(geographical_area_oplog_presenter_id(&model), "id") == 0;
}
