#include "presenters/oplog/quota_unsuspension_event_presenter.h"

#include <string.h>

const char *quota_unsuspension_event_oplog_presenter_resource_type(void)
{
    return "quota_unsuspension_event";
}

const char *quota_unsuspension_event_oplog_presenter_id(const QuotaUnsuspensionEventOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_unsuspension_event_oplog_presenter_self_test(void)
{
    QuotaUnsuspensionEventOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_unsuspension_event_oplog_presenter_resource_type(), "quota_unsuspension_event") == 0 &&
           strcmp(quota_unsuspension_event_oplog_presenter_id(&model), "id") == 0;
}
