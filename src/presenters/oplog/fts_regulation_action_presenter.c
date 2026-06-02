#include "presenters/oplog/fts_regulation_action_presenter.h"

#include <string.h>

const char *fts_regulation_action_oplog_presenter_resource_type(void)
{
    return "fts_regulation_action";
}

const char *fts_regulation_action_oplog_presenter_id(const FtsRegulationActionOplogModel *model)
{
    return model == NULL ? NULL : model->fts_regulation_id;
}

int fts_regulation_action_oplog_presenter_self_test(void)
{
    FtsRegulationActionOplogModel model = { .fts_regulation_id = "id" };
    return strcmp(fts_regulation_action_oplog_presenter_resource_type(), "fts_regulation_action") == 0 &&
           strcmp(fts_regulation_action_oplog_presenter_id(&model), "id") == 0;
}
