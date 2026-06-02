#include "presenters/oplog/quota_closed_and_transferred_event_presenter.h"

#include <string.h>

const char *quota_closed_and_transferred_event_oplog_presenter_resource_type(void)
{
    return "quota_closed_and_transferred_event";
}

const char *quota_closed_and_transferred_event_oplog_presenter_id(const QuotaClosedAndTransferredEventOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_closed_and_transferred_event_oplog_presenter_self_test(void)
{
    QuotaClosedAndTransferredEventOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_closed_and_transferred_event_oplog_presenter_resource_type(), "quota_closed_and_transferred_event") == 0 &&
           strcmp(quota_closed_and_transferred_event_oplog_presenter_id(&model), "id") == 0;
}
