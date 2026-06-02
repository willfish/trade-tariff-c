#include "presenters/oplog/complete_abrogation_regulation_presenter.h"

#include <string.h>

const char *complete_abrogation_regulation_oplog_presenter_resource_type(void)
{
    return "legal_act";
}

const char *complete_abrogation_regulation_oplog_presenter_id(const CompleteAbrogationRegulationOplogModel *model)
{
    return model == NULL ? NULL : model->complete_abrogation_regulation_id;
}

int complete_abrogation_regulation_oplog_presenter_self_test(void)
{
    CompleteAbrogationRegulationOplogModel model = { .complete_abrogation_regulation_id = "id" };
    return strcmp(complete_abrogation_regulation_oplog_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(complete_abrogation_regulation_oplog_presenter_id(&model), "id") == 0;
}
