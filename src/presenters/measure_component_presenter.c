#include "presenters/measure_component_presenter.h"

#include "models/duty_expression_description_model.h"
#include "presenters/duty_expression_description_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *measure_component_presenter_resource_type(void)
{
    return "measure_component";
}

char *measure_component_presenter_id(const MeasureComponentModel *model)
{
    if (model == NULL || model->measure_sid == NULL || model->duty_expression_id == NULL) {
        return NULL;
    }

    size_t len = strlen(model->measure_sid) + 1 + strlen(model->duty_expression_id) + 1;
    char *id = malloc(len);
    if (id == NULL) {
        return NULL;
    }
    snprintf(id, len, "%s-%s", model->measure_sid, model->duty_expression_id);
    return id;
}

const char *measure_component_presenter_duty_expression_abbreviation(const MeasureComponentModel *model)
{
    DutyExpressionDescriptionModel duty_expression = {
        .duty_expression_id = model == NULL ? NULL : model->duty_expression_id,
        .description = model == NULL ? NULL : model->duty_expression_description,
    };
    return duty_expression_description_presenter_abbreviation(&duty_expression);
}

int measure_component_presenter_self_test(void)
{
    MeasureComponentModel model = { .measure_sid = "456", .duty_expression_id = "01" };
    char *id = measure_component_presenter_id(&model);
    int ok = id != NULL &&
           strcmp(measure_component_presenter_resource_type(), "measure_component") == 0 &&
           strcmp(id, "456-01") == 0 &&
           measure_component_presenter_id(NULL) == NULL &&
           strcmp(measure_component_presenter_duty_expression_abbreviation(&model), "%") == 0;
    free(id);
    return ok;
}
