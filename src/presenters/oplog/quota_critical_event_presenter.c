#include "presenters/oplog/quota_critical_event_presenter.h"

#include <string.h>

const char *quota_critical_event_oplog_presenter_resource_type(void)
{
    return "quota_critical_event";
}

const char *quota_critical_event_oplog_presenter_id(const QuotaCriticalEventOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_critical_event_oplog_presenter_self_test(void)
{
    QuotaCriticalEventOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_critical_event_oplog_presenter_resource_type(), "quota_critical_event") == 0 &&
           strcmp(quota_critical_event_oplog_presenter_id(&model), "id") == 0;
}
