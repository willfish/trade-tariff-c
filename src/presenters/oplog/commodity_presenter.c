#include "presenters/oplog/commodity_presenter.h"

#include <string.h>

const char *commodity_oplog_presenter_resource_type(void)
{
    return "commodity";
}

const char *commodity_oplog_presenter_id(const CommodityOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int commodity_oplog_presenter_self_test(void)
{
    CommodityOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(commodity_oplog_presenter_resource_type(), "commodity") == 0 &&
           strcmp(commodity_oplog_presenter_id(&model), "id") == 0;
}
