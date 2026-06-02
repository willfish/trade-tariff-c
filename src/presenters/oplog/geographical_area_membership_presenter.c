#include "presenters/oplog/geographical_area_membership_presenter.h"

#include <string.h>

const char *geographical_area_membership_oplog_presenter_resource_type(void)
{
    return "geographical_area_membership";
}

const char *geographical_area_membership_oplog_presenter_id(const GeographicalAreaMembershipOplogModel *model)
{
    return model == NULL ? NULL : model->geographical_area_sid;
}

int geographical_area_membership_oplog_presenter_self_test(void)
{
    GeographicalAreaMembershipOplogModel model = { .geographical_area_sid = "id" };
    return strcmp(geographical_area_membership_oplog_presenter_resource_type(), "geographical_area_membership") == 0 &&
           strcmp(geographical_area_membership_oplog_presenter_id(&model), "id") == 0;
}
