#include "presenters/oplog/regulation_group_presenter.h"

#include <string.h>

const char *regulation_group_oplog_presenter_resource_type(void)
{
    return "regulation_group";
}

const char *regulation_group_oplog_presenter_id(const RegulationGroupOplogModel *model)
{
    return model == NULL ? NULL : model->regulation_group_id;
}

int regulation_group_oplog_presenter_self_test(void)
{
    RegulationGroupOplogModel model = { .regulation_group_id = "id" };
    return strcmp(regulation_group_oplog_presenter_resource_type(), "regulation_group") == 0 &&
           strcmp(regulation_group_oplog_presenter_id(&model), "id") == 0;
}
