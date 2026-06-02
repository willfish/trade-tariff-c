#ifndef TRADE_TARIFF_C_DUTY_EXPRESSION_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_DUTY_EXPRESSION_OPLOG_PRESENTER_H

#include "models/oplog/duty_expression_model.h"

const char *duty_expression_oplog_presenter_resource_type(void);
const char *duty_expression_oplog_presenter_id(const DutyExpressionOplogModel *model);
int duty_expression_oplog_presenter_self_test(void);

#endif
