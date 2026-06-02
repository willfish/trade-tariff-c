#include "presenters/oplog/subheading_presenter.h"

#include <string.h>

const char *subheading_oplog_presenter_resource_type(void)
{
    return "subheading";
}

const char *subheading_oplog_presenter_id(const SubheadingOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int subheading_oplog_presenter_self_test(void)
{
    SubheadingOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(subheading_oplog_presenter_resource_type(), "subheading") == 0 &&
           strcmp(subheading_oplog_presenter_id(&model), "id") == 0;
}
