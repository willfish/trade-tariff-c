#include "presenters/oplog/goods_nomenclature_indent_presenter.h"

#include <string.h>

const char *goods_nomenclature_indent_oplog_presenter_resource_type(void)
{
    return "goods_nomenclature_indent";
}

const char *goods_nomenclature_indent_oplog_presenter_id(const GoodsNomenclatureIndentOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_indent_sid;
}

int goods_nomenclature_indent_oplog_presenter_self_test(void)
{
    GoodsNomenclatureIndentOplogModel model = { .goods_nomenclature_indent_sid = "id" };
    return strcmp(goods_nomenclature_indent_oplog_presenter_resource_type(), "goods_nomenclature_indent") == 0 &&
           strcmp(goods_nomenclature_indent_oplog_presenter_id(&model), "id") == 0;
}
