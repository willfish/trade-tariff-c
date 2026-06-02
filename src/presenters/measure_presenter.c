#include "presenters/measure_presenter.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

const char *measure_presenter_resource_type(void)
{
    return "measure";
}

const char *measure_presenter_id(const MeasureModel *model)
{
    return model == NULL ? NULL : model->measure_sid;
}

long long measure_presenter_numeric_id(const MeasureModel *model)
{
    const char *sid = measure_presenter_id(model);
    char *end = NULL;
    errno = 0;
    long long value = strtoll(sid == NULL ? "" : sid, &end, 10);
    return errno == 0 && sid != NULL && end != sid && end != NULL && *end == '\0' ? value : 0;
}

const char *measure_presenter_origin(const MeasureModel *model)
{
    return model != NULL &&
                   model->measure_sid != NULL &&
                   model->measure_sid[0] == '-'
               ? "uk"
               : "eu";
}

int measure_presenter_excise(const MeasureModel *model)
{
    return model != NULL && streq(model->measure_type_series_id, "Q");
}

int measure_presenter_vat(const MeasureModel *model)
{
    return model != NULL &&
           (streq(model->measure_type_id, "305") ||
            streq(model->measure_type_id, "VTS"));
}

static int component_is_meursing_placeholder(const MeasureComponentModel *component)
{
    return component != NULL &&
           (streq(component->duty_expression_id, "12") ||
            streq(component->duty_expression_id, "14") ||
            streq(component->duty_expression_id, "21") ||
            streq(component->duty_expression_id, "25") ||
            streq(component->duty_expression_id, "27") ||
            streq(component->duty_expression_id, "29"));
}

int measure_presenter_meursing(const MeasureModel *model,
                               const MeasureComponentModelList *components)
{
    if (model == NULL || model->measure_sid == NULL || components == NULL) {
        return 0;
    }

    for (size_t i = 0; i < components->len; i++) {
        const MeasureComponentModel *component = &components->items[i];
        if (streq(component->measure_sid, model->measure_sid) &&
            component_is_meursing_placeholder(component)) {
            return 1;
        }
    }

    return 0;
}

int measure_presenter_self_test(void)
{
    MeasureModel model = {
        .measure_sid = "-456",
        .measure_type_id = "305",
        .measure_type_series_id = "Q",
    };
    MeasureComponentModel component = {
        .measure_sid = "-456",
        .duty_expression_id = "12",
    };
    MeasureComponentModelList components = {
        .items = &component,
        .len = 1,
        .cap = 1,
    };
    return strcmp(measure_presenter_resource_type(), "measure") == 0 &&
           strcmp(measure_presenter_id(&model), "-456") == 0 &&
           measure_presenter_numeric_id(&model) == -456 &&
           strcmp(measure_presenter_origin(&model), "uk") == 0 &&
           measure_presenter_excise(&model) &&
           measure_presenter_vat(&model) &&
           measure_presenter_meursing(&model, &components);
}
