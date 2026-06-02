#include "presenters/oplog/goods_nomenclature_origin_presenter.h"

#include <string.h>

const char *goods_nomenclature_origin_oplog_presenter_resource_type(void)
{
    return "goods_nomenclature_origin";
}

const char *goods_nomenclature_origin_oplog_presenter_id(const GoodsNomenclatureOriginOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int goods_nomenclature_origin_oplog_presenter_self_test(void)
{
    GoodsNomenclatureOriginOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(goods_nomenclature_origin_oplog_presenter_resource_type(), "goods_nomenclature_origin") == 0 &&
           strcmp(goods_nomenclature_origin_oplog_presenter_id(&model), "id") == 0;
}
