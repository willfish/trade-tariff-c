#include "presenters/oplog/quota_unblocking_event_presenter.h"

#include <string.h>

const char *quota_unblocking_event_oplog_presenter_resource_type(void)
{
    return "quota_unblocking_event";
}

const char *quota_unblocking_event_oplog_presenter_id(const QuotaUnblockingEventOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_unblocking_event_oplog_presenter_self_test(void)
{
    QuotaUnblockingEventOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_unblocking_event_oplog_presenter_resource_type(), "quota_unblocking_event") == 0 &&
           strcmp(quota_unblocking_event_oplog_presenter_id(&model), "id") == 0;
}
