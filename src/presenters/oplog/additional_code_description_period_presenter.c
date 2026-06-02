#include "presenters/oplog/additional_code_description_period_presenter.h"

#include <string.h>

const char *additional_code_description_period_oplog_presenter_resource_type(void)
{
    return "additional_code_description_period";
}

const char *additional_code_description_period_oplog_presenter_id(const AdditionalCodeDescriptionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->additional_code_description_period_sid;
}

int additional_code_description_period_oplog_presenter_self_test(void)
{
    AdditionalCodeDescriptionPeriodOplogModel model = { .additional_code_description_period_sid = "id" };
    return strcmp(additional_code_description_period_oplog_presenter_resource_type(), "additional_code_description_period") == 0 &&
           strcmp(additional_code_description_period_oplog_presenter_id(&model), "id") == 0;
}
