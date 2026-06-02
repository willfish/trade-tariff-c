#include "presenters/oplog/goods_nomenclature_description_period_presenter.h"

#include <string.h>

const char *goods_nomenclature_description_period_oplog_presenter_resource_type(void)
{
    return "goods_nomenclature_description_period";
}

const char *goods_nomenclature_description_period_oplog_presenter_id(const GoodsNomenclatureDescriptionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_description_period_sid;
}

int goods_nomenclature_description_period_oplog_presenter_self_test(void)
{
    GoodsNomenclatureDescriptionPeriodOplogModel model = { .goods_nomenclature_description_period_sid = "id" };
    return strcmp(goods_nomenclature_description_period_oplog_presenter_resource_type(), "goods_nomenclature_description_period") == 0 &&
           strcmp(goods_nomenclature_description_period_oplog_presenter_id(&model), "id") == 0;
}
