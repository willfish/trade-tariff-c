#include "models/duty_expression_model.h"

#include <stdlib.h>

void duty_expression_model_free(DutyExpressionModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->id);
    free(model->base);
    free(model->formatted_base);
    free(model->verbose_duty);
    model->id = NULL;
    model->base = NULL;
    model->formatted_base = NULL;
    model->verbose_duty = NULL;
}

int duty_expression_model_self_test(void)
{
    DutyExpressionModel model = { 0 };
    duty_expression_model_free(&model);
    return model.id == NULL &&
           model.base == NULL &&
           model.formatted_base == NULL &&
           model.verbose_duty == NULL;
}
