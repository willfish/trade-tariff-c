#include "presenters/oplog/prorogation_regulation_action_presenter.h"

#include <string.h>

const char *prorogation_regulation_action_oplog_presenter_resource_type(void)
{
    return "prorogation_regulation_action";
}

const char *prorogation_regulation_action_oplog_presenter_id(const ProrogationRegulationActionOplogModel *model)
{
    return model == NULL ? NULL : model->prorogation_regulation_id;
}

int prorogation_regulation_action_oplog_presenter_self_test(void)
{
    ProrogationRegulationActionOplogModel model = { .prorogation_regulation_id = "id" };
    return strcmp(prorogation_regulation_action_oplog_presenter_resource_type(), "prorogation_regulation_action") == 0 &&
           strcmp(prorogation_regulation_action_oplog_presenter_id(&model), "id") == 0;
}
