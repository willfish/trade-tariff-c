#include "presenters/oplog/quota_association_presenter.h"

#include <string.h>

const char *quota_association_oplog_presenter_resource_type(void)
{
    return "quota_association";
}

const char *quota_association_oplog_presenter_id(const QuotaAssociationOplogModel *model)
{
    return model == NULL ? NULL : model->main_quota_definition_sid;
}

int quota_association_oplog_presenter_self_test(void)
{
    QuotaAssociationOplogModel model = { .main_quota_definition_sid = "id" };
    return strcmp(quota_association_oplog_presenter_resource_type(), "quota_association") == 0 &&
           strcmp(quota_association_oplog_presenter_id(&model), "id") == 0;
}
