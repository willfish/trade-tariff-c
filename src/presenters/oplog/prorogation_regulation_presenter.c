#include "presenters/oplog/prorogation_regulation_presenter.h"

#include <string.h>

const char *prorogation_regulation_oplog_presenter_resource_type(void)
{
    return "legal_act";
}

const char *prorogation_regulation_oplog_presenter_id(const ProrogationRegulationOplogModel *model)
{
    return model == NULL ? NULL : model->prorogation_regulation_id;
}

int prorogation_regulation_oplog_presenter_self_test(void)
{
    ProrogationRegulationOplogModel model = { .prorogation_regulation_id = "id" };
    return strcmp(prorogation_regulation_oplog_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(prorogation_regulation_oplog_presenter_id(&model), "id") == 0;
}
