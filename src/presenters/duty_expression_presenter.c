#include "presenters/duty_expression_presenter.h"

#include <string.h>

const char *duty_expression_presenter_resource_type(void)
{
    return "duty_expression";
}

const char *duty_expression_presenter_id(const DutyExpressionModel *model)
{
    return model == NULL ? NULL : model->id;
}

const char *duty_expression_presenter_base(const DutyExpressionModel *model)
{
    return model == NULL ? NULL : model->base;
}

const char *duty_expression_presenter_formatted_base(const DutyExpressionModel *model)
{
    return model == NULL ? NULL : model->formatted_base;
}

const char *duty_expression_presenter_verbose_duty(const DutyExpressionModel *model)
{
    return model == NULL ? NULL : model->verbose_duty;
}

int duty_expression_presenter_self_test(void)
{
    DutyExpressionModel model = {
        .id = "123-duty_expression",
        .base = "8.50 %",
        .formatted_base = "<span>8.50</span> %",
        .verbose_duty = "8.50%",
    };

    return strcmp(duty_expression_presenter_resource_type(), "duty_expression") == 0 &&
           strcmp(duty_expression_presenter_id(&model), "123-duty_expression") == 0 &&
           strcmp(duty_expression_presenter_base(&model), "8.50 %") == 0 &&
           strcmp(duty_expression_presenter_formatted_base(&model), "<span>8.50</span> %") == 0 &&
           strcmp(duty_expression_presenter_verbose_duty(&model), "8.50%") == 0 &&
           duty_expression_presenter_id(NULL) == NULL &&
           duty_expression_presenter_base(NULL) == NULL &&
           duty_expression_presenter_formatted_base(NULL) == NULL &&
           duty_expression_presenter_verbose_duty(NULL) == NULL;
}
