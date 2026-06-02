#include "presenters/oplog/base_regulation_presenter.h"

#include <string.h>

const char *base_regulation_oplog_presenter_resource_type(void)
{
    return "legal_act";
}

const char *base_regulation_oplog_presenter_id(const BaseRegulationOplogModel *model)
{
    return model == NULL ? NULL : model->base_regulation_id;
}

int base_regulation_oplog_presenter_self_test(void)
{
    BaseRegulationOplogModel model = { .base_regulation_id = "id" };
    return strcmp(base_regulation_oplog_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(base_regulation_oplog_presenter_id(&model), "id") == 0;
}
