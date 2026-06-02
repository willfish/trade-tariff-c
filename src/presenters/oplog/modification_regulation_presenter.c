#include "presenters/oplog/modification_regulation_presenter.h"

#include <string.h>

const char *modification_regulation_oplog_presenter_resource_type(void)
{
    return "legal_act";
}

const char *modification_regulation_oplog_presenter_id(const ModificationRegulationOplogModel *model)
{
    return model == NULL ? NULL : model->modification_regulation_id;
}

int modification_regulation_oplog_presenter_self_test(void)
{
    ModificationRegulationOplogModel model = { .modification_regulation_id = "id" };
    return strcmp(modification_regulation_oplog_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(modification_regulation_oplog_presenter_id(&model), "id") == 0;
}
