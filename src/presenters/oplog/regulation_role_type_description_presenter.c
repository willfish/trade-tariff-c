#include "presenters/oplog/regulation_role_type_description_presenter.h"

#include <string.h>

const char *regulation_role_type_description_oplog_presenter_resource_type(void)
{
    return "regulation_role_type_description";
}

const char *regulation_role_type_description_oplog_presenter_id(const RegulationRoleTypeDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->regulation_role_type_id;
}

int regulation_role_type_description_oplog_presenter_self_test(void)
{
    RegulationRoleTypeDescriptionOplogModel model = { .regulation_role_type_id = "id" };
    return strcmp(regulation_role_type_description_oplog_presenter_resource_type(), "regulation_role_type_description") == 0 &&
           strcmp(regulation_role_type_description_oplog_presenter_id(&model), "id") == 0;
}
