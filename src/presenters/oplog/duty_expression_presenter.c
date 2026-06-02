#include "presenters/oplog/duty_expression_presenter.h"

#include <string.h>

const char *duty_expression_oplog_presenter_resource_type(void)
{
    return "duty_expression";
}

const char *duty_expression_oplog_presenter_id(const DutyExpressionOplogModel *model)
{
    return model == NULL ? NULL : model->duty_expression_id;
}

int duty_expression_oplog_presenter_self_test(void)
{
    DutyExpressionOplogModel model = { .duty_expression_id = "id" };
    return strcmp(duty_expression_oplog_presenter_resource_type(), "duty_expression") == 0 &&
           strcmp(duty_expression_oplog_presenter_id(&model), "id") == 0;
}
