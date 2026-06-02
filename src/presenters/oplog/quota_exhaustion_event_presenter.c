#include "presenters/oplog/quota_exhaustion_event_presenter.h"

#include <string.h>

const char *quota_exhaustion_event_oplog_presenter_resource_type(void)
{
    return "quota_exhaustion_event";
}

const char *quota_exhaustion_event_oplog_presenter_id(const QuotaExhaustionEventOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_exhaustion_event_oplog_presenter_self_test(void)
{
    QuotaExhaustionEventOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_exhaustion_event_oplog_presenter_resource_type(), "quota_exhaustion_event") == 0 &&
           strcmp(quota_exhaustion_event_oplog_presenter_id(&model), "id") == 0;
}
