#include "presenters/oplog/nomenclature_group_membership_presenter.h"

#include <string.h>

const char *nomenclature_group_membership_oplog_presenter_resource_type(void)
{
    return "nomenclature_group_membership";
}

const char *nomenclature_group_membership_oplog_presenter_id(const NomenclatureGroupMembershipOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int nomenclature_group_membership_oplog_presenter_self_test(void)
{
    NomenclatureGroupMembershipOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(nomenclature_group_membership_oplog_presenter_resource_type(), "nomenclature_group_membership") == 0 &&
           strcmp(nomenclature_group_membership_oplog_presenter_id(&model), "id") == 0;
}
