#include "presenters/oplog/additional_code_type_presenter.h"

#include <string.h>

const char *additional_code_type_oplog_presenter_resource_type(void)
{
    return "additional_code_type";
}

const char *additional_code_type_oplog_presenter_id(const AdditionalCodeTypeOplogModel *model)
{
    return model == NULL ? NULL : model->additional_code_type_id;
}

int additional_code_type_oplog_presenter_self_test(void)
{
    AdditionalCodeTypeOplogModel model = { .additional_code_type_id = "id" };
    return strcmp(additional_code_type_oplog_presenter_resource_type(), "additional_code_type") == 0 &&
           strcmp(additional_code_type_oplog_presenter_id(&model), "id") == 0;
}
