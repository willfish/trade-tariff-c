#include "presenters/oplog/quota_order_number_origin_exclusion_presenter.h"

#include <string.h>

const char *quota_order_number_origin_exclusion_oplog_presenter_resource_type(void)
{
    return "quota_order_number_origin_exclusion";
}

const char *quota_order_number_origin_exclusion_oplog_presenter_id(const QuotaOrderNumberOriginExclusionOplogModel *model)
{
    return model == NULL ? NULL : model->quota_order_number_origin_sid;
}

int quota_order_number_origin_exclusion_oplog_presenter_self_test(void)
{
    QuotaOrderNumberOriginExclusionOplogModel model = { .quota_order_number_origin_sid = "id" };
    return strcmp(quota_order_number_origin_exclusion_oplog_presenter_resource_type(), "quota_order_number_origin_exclusion") == 0 &&
           strcmp(quota_order_number_origin_exclusion_oplog_presenter_id(&model), "id") == 0;
}
