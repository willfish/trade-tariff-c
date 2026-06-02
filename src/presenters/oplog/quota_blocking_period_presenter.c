#include "presenters/oplog/quota_blocking_period_presenter.h"

#include <string.h>

const char *quota_blocking_period_oplog_presenter_resource_type(void)
{
    return "quota_blocking_period";
}

const char *quota_blocking_period_oplog_presenter_id(const QuotaBlockingPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->quota_blocking_period_sid;
}

int quota_blocking_period_oplog_presenter_self_test(void)
{
    QuotaBlockingPeriodOplogModel model = { .quota_blocking_period_sid = "id" };
    return strcmp(quota_blocking_period_oplog_presenter_resource_type(), "quota_blocking_period") == 0 &&
           strcmp(quota_blocking_period_oplog_presenter_id(&model), "id") == 0;
}
