#include "presenters/oplog/additional_code_type_description_presenter.h"

#include <string.h>

const char *additional_code_type_description_oplog_presenter_resource_type(void)
{
    return "additional_code_type_description";
}

const char *additional_code_type_description_oplog_presenter_id(const AdditionalCodeTypeDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->additional_code_type_id;
}

int additional_code_type_description_oplog_presenter_self_test(void)
{
    AdditionalCodeTypeDescriptionOplogModel model = { .additional_code_type_id = "id" };
    return strcmp(additional_code_type_description_oplog_presenter_resource_type(), "additional_code_type_description") == 0 &&
           strcmp(additional_code_type_description_oplog_presenter_id(&model), "id") == 0;
}
