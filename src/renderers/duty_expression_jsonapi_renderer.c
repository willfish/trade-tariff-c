#include "renderers/duty_expression_jsonapi_renderer.h"

#include "presenters/duty_expression_presenter.h"
#include "presenters/measure_duty_expression_presenter.h"
#include "renderers/jsonapi_relationship.h"

#include <string.h>

yyjson_mut_val *duty_expression_jsonapi_resource(yyjson_mut_doc *doc,
                                                 const MeasureModel *measure,
                                                 const MeasureComponentModelList *components)
{
    if (doc == NULL || measure == NULL || measure->measure_sid == NULL || components == NULL) {
        return NULL;
    }

    DutyExpressionModel model = measure_duty_expression_presenter_build(measure, components);
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       duty_expression_presenter_resource_type(),
                                                       duty_expression_presenter_id(&model));
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    int ok = duty_expression_presenter_id(&model) != NULL &&
             duty_expression_presenter_base(&model) != NULL &&
             duty_expression_presenter_formatted_base(&model) != NULL &&
             duty_expression_presenter_verbose_duty(&model) != NULL &&
             resource != NULL &&
             attributes != NULL &&
             yyjson_mut_obj_add_strcpy(doc, attributes, "base", duty_expression_presenter_base(&model)) &&
             yyjson_mut_obj_add_strcpy(doc, attributes, "formatted_base", duty_expression_presenter_formatted_base(&model)) &&
             yyjson_mut_obj_add_strcpy(doc, attributes, "verbose_duty", duty_expression_presenter_verbose_duty(&model)) &&
             yyjson_mut_obj_add_val(doc, resource, "attributes", attributes);

    duty_expression_model_free(&model);
    return ok ? resource : NULL;
}

int duty_expression_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    MeasureModel measure = {
        .measure_sid = "1",
    };
    MeasureComponentModel component = {
        .measure_sid = "1",
        .duty_expression_id = "01",
        .duty_amount = "8.500000",
    };
    MeasureComponentModelList components = {
        .items = &component,
        .len = 1,
        .cap = 1,
    };
    yyjson_mut_val *resource = duty_expression_jsonapi_resource(doc, &measure, &components);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
