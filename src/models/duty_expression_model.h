#ifndef TRADE_TARIFF_C_DUTY_EXPRESSION_MODEL_H
#define TRADE_TARIFF_C_DUTY_EXPRESSION_MODEL_H

typedef struct {
    char *id;
    char *base;
    char *formatted_base;
    char *verbose_duty;
} DutyExpressionModel;

void duty_expression_model_free(DutyExpressionModel *model);
int duty_expression_model_self_test(void);

#endif
