#include "presenters/oplog/explicit_abrogation_regulation_presenter.h"

#include <string.h>

const char *explicit_abrogation_regulation_oplog_presenter_resource_type(void)
{
    return "legal_act";
}

const char *explicit_abrogation_regulation_oplog_presenter_id(const ExplicitAbrogationRegulationOplogModel *model)
{
    return model == NULL ? NULL : model->explicit_abrogation_regulation_id;
}

int explicit_abrogation_regulation_oplog_presenter_self_test(void)
{
    ExplicitAbrogationRegulationOplogModel model = { .explicit_abrogation_regulation_id = "id" };
    return strcmp(explicit_abrogation_regulation_oplog_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(explicit_abrogation_regulation_oplog_presenter_id(&model), "id") == 0;
}
