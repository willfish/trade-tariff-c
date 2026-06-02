#include "presenters/oplog/quota_order_number_origin_presenter.h"

#include <string.h>

const char *quota_order_number_origin_oplog_presenter_resource_type(void)
{
    return "quota_order_number_origin";
}

const char *quota_order_number_origin_oplog_presenter_id(const QuotaOrderNumberOriginOplogModel *model)
{
    return model == NULL ? NULL : model->quota_order_number_origin_sid;
}

int quota_order_number_origin_oplog_presenter_self_test(void)
{
    QuotaOrderNumberOriginOplogModel model = { .quota_order_number_origin_sid = "id" };
    return strcmp(quota_order_number_origin_oplog_presenter_resource_type(), "quota_order_number_origin") == 0 &&
           strcmp(quota_order_number_origin_oplog_presenter_id(&model), "id") == 0;
}
