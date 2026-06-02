#include "presenters/oplog/heading_presenter.h"

#include <string.h>

const char *heading_oplog_presenter_resource_type(void)
{
    return "heading";
}

const char *heading_oplog_presenter_id(const HeadingOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int heading_oplog_presenter_self_test(void)
{
    HeadingOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(heading_oplog_presenter_resource_type(), "heading") == 0 &&
           strcmp(heading_oplog_presenter_id(&model), "id") == 0;
}
