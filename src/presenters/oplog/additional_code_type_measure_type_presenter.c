#include "presenters/oplog/additional_code_type_measure_type_presenter.h"

#include <string.h>

const char *additional_code_type_measure_type_oplog_presenter_resource_type(void)
{
    return "additional_code_type_measure_type";
}

const char *additional_code_type_measure_type_oplog_presenter_id(const AdditionalCodeTypeMeasureTypeOplogModel *model)
{
    return model == NULL ? NULL : model->measure_type_id;
}

int additional_code_type_measure_type_oplog_presenter_self_test(void)
{
    AdditionalCodeTypeMeasureTypeOplogModel model = { .measure_type_id = "id" };
    return strcmp(additional_code_type_measure_type_oplog_presenter_resource_type(), "additional_code_type_measure_type") == 0 &&
           strcmp(additional_code_type_measure_type_oplog_presenter_id(&model), "id") == 0;
}
