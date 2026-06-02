#include "presenters/oplog/quota_order_number_presenter.h"

#include <string.h>

const char *quota_order_number_oplog_presenter_resource_type(void)
{
    return "quota_order_number";
}

const char *quota_order_number_oplog_presenter_id(const QuotaOrderNumberOplogModel *model)
{
    return model == NULL ? NULL : model->quota_order_number_sid;
}

int quota_order_number_oplog_presenter_self_test(void)
{
    QuotaOrderNumberOplogModel model = { .quota_order_number_sid = "id" };
    return strcmp(quota_order_number_oplog_presenter_resource_type(), "quota_order_number") == 0 &&
           strcmp(quota_order_number_oplog_presenter_id(&model), "id") == 0;
}
