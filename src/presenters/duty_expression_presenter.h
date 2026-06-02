#ifndef TRADE_TARIFF_C_DUTY_EXPRESSION_PRESENTER_H
#define TRADE_TARIFF_C_DUTY_EXPRESSION_PRESENTER_H

#include "models/duty_expression_model.h"

const char *duty_expression_presenter_resource_type(void);
const char *duty_expression_presenter_id(const DutyExpressionModel *model);
const char *duty_expression_presenter_base(const DutyExpressionModel *model);
const char *duty_expression_presenter_formatted_base(const DutyExpressionModel *model);
const char *duty_expression_presenter_verbose_duty(const DutyExpressionModel *model);
int duty_expression_presenter_self_test(void);

#endif
