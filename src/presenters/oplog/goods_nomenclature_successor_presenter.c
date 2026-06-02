#include "presenters/oplog/goods_nomenclature_successor_presenter.h"

#include <string.h>

const char *goods_nomenclature_successor_oplog_presenter_resource_type(void)
{
    return "goods_nomenclature_successor";
}

const char *goods_nomenclature_successor_oplog_presenter_id(const GoodsNomenclatureSuccessorOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int goods_nomenclature_successor_oplog_presenter_self_test(void)
{
    GoodsNomenclatureSuccessorOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(goods_nomenclature_successor_oplog_presenter_resource_type(), "goods_nomenclature_successor") == 0 &&
           strcmp(goods_nomenclature_successor_oplog_presenter_id(&model), "id") == 0;
}
