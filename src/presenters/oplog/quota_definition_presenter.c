#include "presenters/oplog/quota_definition_presenter.h"

#include <string.h>

const char *quota_definition_oplog_presenter_resource_type(void)
{
    return "quota_definition";
}

const char *quota_definition_oplog_presenter_id(const QuotaDefinitionOplogModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

int quota_definition_oplog_presenter_self_test(void)
{
    QuotaDefinitionOplogModel model = { .quota_definition_sid = "id" };
    return strcmp(quota_definition_oplog_presenter_resource_type(), "quota_definition") == 0 &&
           strcmp(quota_definition_oplog_presenter_id(&model), "id") == 0;
}
