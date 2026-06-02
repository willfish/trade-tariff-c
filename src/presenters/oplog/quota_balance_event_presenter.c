#include "presenters/oplog/quota_balance_event_presenter.h"

#include <string.h>

const char *quota_balance_event_oplog_presenter_resource_type(void)
{
    return "quota_balance_event";
}

const char *quota_balance_event_oplog_presenter_id(const QuotaBalanceEventOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_balance_event_oplog_presenter_self_test(void)
{
    QuotaBalanceEventOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_balance_event_oplog_presenter_resource_type(), "quota_balance_event") == 0 &&
           strcmp(quota_balance_event_oplog_presenter_id(&model), "id") == 0;
}
