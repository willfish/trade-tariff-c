#include "presenters/oplog/full_temporary_stop_regulation_presenter.h"

#include <string.h>

const char *full_temporary_stop_regulation_oplog_presenter_resource_type(void)
{
    return "legal_act";
}

const char *full_temporary_stop_regulation_oplog_presenter_id(const FullTemporaryStopRegulationOplogModel *model)
{
    return model == NULL ? NULL : model->full_temporary_stop_regulation_id;
}

int full_temporary_stop_regulation_oplog_presenter_self_test(void)
{
    FullTemporaryStopRegulationOplogModel model = { .full_temporary_stop_regulation_id = "id" };
    return strcmp(full_temporary_stop_regulation_oplog_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(full_temporary_stop_regulation_oplog_presenter_id(&model), "id") == 0;
}
