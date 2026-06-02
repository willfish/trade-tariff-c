#include "presenters/oplog/quota_suspension_period_presenter.h"

#include <string.h>

const char *quota_suspension_period_oplog_presenter_resource_type(void)
{
    return "quota_suspension_period";
}

const char *quota_suspension_period_oplog_presenter_id(const QuotaSuspensionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->quota_suspension_period_sid;
}

int quota_suspension_period_oplog_presenter_self_test(void)
{
    QuotaSuspensionPeriodOplogModel model = { .quota_suspension_period_sid = "id" };
    return strcmp(quota_suspension_period_oplog_presenter_resource_type(), "quota_suspension_period") == 0 &&
           strcmp(quota_suspension_period_oplog_presenter_id(&model), "id") == 0;
}
