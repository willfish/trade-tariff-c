#include "presenters/oplog/additional_code_presenter.h"

#include <string.h>

const char *additional_code_oplog_presenter_resource_type(void)
{
    return "additional_code";
}

const char *additional_code_oplog_presenter_id(const AdditionalCodeOplogModel *model)
{
    return model == NULL ? NULL : model->additional_code_sid;
}

int additional_code_oplog_presenter_self_test(void)
{
    AdditionalCodeOplogModel model = { .additional_code_sid = "id" };
    return strcmp(additional_code_oplog_presenter_resource_type(), "additional_code") == 0 &&
           strcmp(additional_code_oplog_presenter_id(&model), "id") == 0;
}
