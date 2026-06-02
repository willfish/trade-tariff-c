#ifndef TRADE_TARIFF_C_MEASURE_DUTY_EXPRESSION_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_DUTY_EXPRESSION_PRESENTER_H

#include "models/duty_expression_model.h"
#include "models/measure_component_model.h"
#include "models/measure_model.h"

char *measure_duty_expression_presenter_formatted(const MeasureModel *measure,
                                                  const MeasureComponentModelList *components);
char *measure_duty_expression_presenter_resolved(const MeasureModel *measure,
                                                 const MeasureComponentModelList *components);
char *measure_duty_expression_presenter_id(const MeasureModel *measure);
DutyExpressionModel measure_duty_expression_presenter_build(const MeasureModel *measure,
                                                            const MeasureComponentModelList *components);
int measure_duty_expression_presenter_self_test(void);

#endif
