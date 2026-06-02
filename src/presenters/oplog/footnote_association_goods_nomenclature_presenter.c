#include "presenters/oplog/footnote_association_goods_nomenclature_presenter.h"

#include <string.h>

const char *footnote_association_goods_nomenclature_oplog_presenter_resource_type(void)
{
    return "footnote_association_goods_nomenclature";
}

const char *footnote_association_goods_nomenclature_oplog_presenter_id(const FootnoteAssociationGoodsNomenclatureOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int footnote_association_goods_nomenclature_oplog_presenter_self_test(void)
{
    FootnoteAssociationGoodsNomenclatureOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(footnote_association_goods_nomenclature_oplog_presenter_resource_type(), "footnote_association_goods_nomenclature") == 0 &&
           strcmp(footnote_association_goods_nomenclature_oplog_presenter_id(&model), "id") == 0;
}
