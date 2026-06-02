#include "presenters/oplog/goods_nomenclature_group_presenter.h"

#include <string.h>

const char *goods_nomenclature_group_oplog_presenter_resource_type(void)
{
    return "goods_nomenclature_group";
}

const char *goods_nomenclature_group_oplog_presenter_id(const GoodsNomenclatureGroupOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_group_id;
}

int goods_nomenclature_group_oplog_presenter_self_test(void)
{
    GoodsNomenclatureGroupOplogModel model = { .goods_nomenclature_group_id = "id" };
    return strcmp(goods_nomenclature_group_oplog_presenter_resource_type(), "goods_nomenclature_group") == 0 &&
           strcmp(goods_nomenclature_group_oplog_presenter_id(&model), "id") == 0;
}
