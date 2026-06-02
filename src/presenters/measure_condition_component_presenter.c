#include "presenters/measure_condition_component_presenter.h"

#include "models/duty_expression_description_model.h"
#include "presenters/duty_expression_description_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *measure_condition_component_presenter_resource_type(void)
{
    return "measure_condition_component";
}

char *measure_condition_component_presenter_id(const MeasureConditionComponentModel *model)
{
    if (model == NULL || model->measure_condition_sid == NULL || model->duty_expression_id == NULL) {
        return NULL;
    }

    size_t len = strlen(model->measure_condition_sid) + 1 + strlen(model->duty_expression_id) + 1;
    char *id = malloc(len);
    if (id == NULL) {
        return NULL;
    }

    snprintf(id, len, "%s-%s", model->measure_condition_sid, model->duty_expression_id);
    return id;
}

const char *measure_condition_component_presenter_duty_expression_abbreviation(const MeasureConditionComponentModel *model)
{
    DutyExpressionDescriptionModel duty_expression = {
        .duty_expression_id = model == NULL ? NULL : model->duty_expression_id,
        .description = model == NULL ? NULL : model->duty_expression_description,
    };
    return duty_expression_description_presenter_abbreviation(&duty_expression);
}

int measure_condition_component_presenter_self_test(void)
{
    MeasureConditionComponentModel model = { .measure_condition_sid = "789", .duty_expression_id = "17" };
    char *id = measure_condition_component_presenter_id(&model);
    int ok = id != NULL &&
             strcmp(measure_condition_component_presenter_resource_type(), "measure_condition_component") == 0 &&
             strcmp(id, "789-17") == 0 &&
             strcmp(measure_condition_component_presenter_duty_expression_abbreviation(&model), "MAX") == 0;
    free(id);
    return ok;
}
