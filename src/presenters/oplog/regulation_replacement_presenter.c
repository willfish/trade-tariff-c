#include "presenters/oplog/regulation_replacement_presenter.h"

#include <string.h>

const char *regulation_replacement_oplog_presenter_resource_type(void)
{
    return "regulation_replacement";
}

const char *regulation_replacement_oplog_presenter_id(const RegulationReplacementOplogModel *model)
{
    return model == NULL ? NULL : model->geographical_area_id;
}

int regulation_replacement_oplog_presenter_self_test(void)
{
    RegulationReplacementOplogModel model = { .geographical_area_id = "id" };
    return strcmp(regulation_replacement_oplog_presenter_resource_type(), "regulation_replacement") == 0 &&
           strcmp(regulation_replacement_oplog_presenter_id(&model), "id") == 0;
}
