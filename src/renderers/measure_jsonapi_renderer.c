#include "renderers/measure_jsonapi_renderer.h"

#include "presenters/additional_code_presenter.h"
#include "presenters/duty_expression_presenter.h"
#include "presenters/geographical_area_presenter.h"
#include "presenters/measure_duty_expression_presenter.h"
#include "presenters/measure_presenter.h"
#include "presenters/measure_type_presenter.h"
#include "presenters/preference_code_presenter.h"
#include "presenters/quota_order_number_presenter.h"
#include "renderers/footnote_jsonapi_renderer.h"
#include "renderers/geographical_area_jsonapi_renderer.h"
#include "renderers/jsonapi_relationship.h"
#include "renderers/measure_component_jsonapi_renderer.h"
#include "renderers/measure_condition_jsonapi_renderer.h"
#include "renderers/measure_condition_permutation_jsonapi_renderer.h"
#include "renderers/measure_legal_act_jsonapi_renderer.h"

#include <stdlib.h>

static int add_string_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
}

yyjson_mut_val *measure_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                    const MeasureModel *measure)
{
    const char *id = measure_presenter_id(measure);
    if (doc == NULL || id == NULL) {
        return NULL;
    }
    return jsonapi_resource_identifier(doc, measure_presenter_resource_type(), id);
}

yyjson_mut_val *measure_jsonapi_relationship_data(yyjson_mut_doc *doc,
                                                  const MeasureModelList *measures,
                                                  int import_relationship)
{
    if (doc == NULL || measures == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < measures->len; i++) {
        const MeasureModel *measure = &measures->items[i];
        int include = import_relationship ? measure_model_import(measure) : measure_model_export(measure);
        if (!include) {
            continue;
        }

        yyjson_mut_val *ref = measure_jsonapi_resource_identifier(doc, measure);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

static yyjson_mut_val *preference_code_relationship(yyjson_mut_doc *doc,
                                                    const MeasureModel *measure,
                                                    const PreferenceCodeDeclarable *preference_declarable,
                                                    const MeasureConditionModelList *conditions)
{
    const char *code = preference_code_presenter_code_for_measure(preference_declarable, measure, conditions);
    return jsonapi_nullable_relationship(doc,
                                         preference_code_presenter_resource_type(),
                                         preference_code_presenter_id(code));
}

static yyjson_mut_val *measure_attributes(yyjson_mut_doc *doc,
                                          const MeasureModel *measure,
                                          const MeasureComponentModelList *components,
                                          const MeasureComponentModelList *resolved_components)
{
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (attributes == NULL || measure == NULL || measure->measure_sid == NULL) {
        return NULL;
    }

    char *resolved_duty_expression = measure_duty_expression_presenter_resolved(measure, resolved_components);
    if (resolved_duty_expression == NULL) {
        return NULL;
    }

    int ok =
        yyjson_mut_obj_add_strcpy(doc, attributes, "origin", measure_presenter_origin(measure)) &&
        yyjson_mut_obj_add_bool(doc, attributes, "import", measure_model_import(measure)) &&
        yyjson_mut_obj_add_bool(doc, attributes, "export", measure_model_export(measure)) &&
        yyjson_mut_obj_add_int(doc, attributes, "id", measure_presenter_numeric_id(measure)) &&
        add_string_or_null(doc, attributes, "effective_start_date", measure->effective_start_date) &&
        add_string_or_null(doc, attributes, "effective_end_date", measure->effective_end_date) &&
        yyjson_mut_obj_add_bool(doc, attributes, "excise", measure_presenter_excise(measure)) &&
        yyjson_mut_obj_add_bool(doc, attributes, "vat", measure_presenter_vat(measure)) &&
        add_string_or_null(doc, attributes, "reduction_indicator", measure->reduction_indicator) &&
        yyjson_mut_obj_add_bool(doc, attributes, "meursing", measure_presenter_meursing(measure, components)) &&
        yyjson_mut_obj_add_strcpy(doc, attributes, "resolved_duty_expression", resolved_duty_expression) &&
        yyjson_mut_obj_add_bool(doc, attributes, "universal_waiver_applies", 0);

    free(resolved_duty_expression);
    return ok ? attributes : NULL;
}

static yyjson_mut_val *measure_relationships(yyjson_mut_doc *doc,
                                             const MeasureModel *measure,
                                             const PreferenceCodeDeclarable *preference_declarable,
                                             const MeasureComponentModelList *components,
                                             const MeasureComponentModelList *resolved_components,
                                             const MeasureConditionModelList *conditions,
                                             const MeasureConditionPermutationGroupModelList *condition_permutation_groups,
                                             const MeasureExcludedGeographicalAreaModelList *excluded_geographical_areas,
                                             const FootnoteList *footnotes,
                                             const MeasureLegalActModelList *legal_acts)
{
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    if (relationships == NULL || measure == NULL || measure->measure_sid == NULL) {
        return NULL;
    }

    char *duty_expression_id = measure_duty_expression_presenter_id(measure);
    if (duty_expression_id == NULL) {
        return NULL;
    }

    yyjson_mut_val *duty_expression = jsonapi_single_relationship(doc,
                                                                  duty_expression_presenter_resource_type(),
                                                                  duty_expression_id);
    yyjson_mut_val *measure_type = jsonapi_single_relationship(doc,
                                                               measure_type_presenter_resource_type(),
                                                               measure->measure_type_id);
    yyjson_mut_val *measure_legal_act_data = measure_legal_act_jsonapi_relationship_data_for_measure(doc,
                                                                                                     legal_acts,
                                                                                                     measure->measure_sid);
    yyjson_mut_val *suspension_legal_act = measure_legal_act_jsonapi_suspension_relationship(doc,
                                                                                             legal_acts,
                                                                                             measure->measure_sid);
    yyjson_mut_val *measure_condition_data = measure_condition_jsonapi_relationship_data_for_measure(doc,
                                                                                                     conditions,
                                                                                                     measure->measure_sid);
    yyjson_mut_val *measure_component_data = measure_component_jsonapi_relationship_data_for_measure(doc,
                                                                                                     components,
                                                                                                     measure->measure_sid);
    yyjson_mut_val *geographical_area = jsonapi_single_relationship(doc,
                                                                    geographical_area_presenter_resource_type(),
                                                                    measure->geographical_area_id);
    yyjson_mut_val *measure_footnote_data = footnote_jsonapi_relationship_data_for_measure(doc,
                                                                                           footnotes,
                                                                                           measure->measure_sid);
    yyjson_mut_val *order_number = jsonapi_nullable_relationship(doc,
                                                                 quota_order_number_presenter_resource_type(),
                                                                 measure->ordernumber);
    yyjson_mut_val *preference_code = preference_code_relationship(doc, measure, preference_declarable, conditions);
    yyjson_mut_val *measure_condition_permutation_group_data =
        measure_condition_permutation_group_jsonapi_relationship_data_for_measure(
            doc,
            condition_permutation_groups,
            measure->measure_sid);
    yyjson_mut_val *resolved_measure_component_data = measure_component_jsonapi_relationship_data_for_measure(doc,
                                                                                                             resolved_components,
                                                                                                             measure->measure_sid);
    yyjson_mut_val *national_measurement_units = jsonapi_empty_array_relationship(doc);
    yyjson_mut_val *excluded_country_data = geographical_area_jsonapi_excluded_relationship_data_for_measure(
        doc,
        excluded_geographical_areas,
        measure->measure_sid);

    int ok = duty_expression != NULL &&
             measure_type != NULL &&
             suspension_legal_act != NULL &&
             geographical_area != NULL &&
             order_number != NULL &&
             preference_code != NULL &&
             measure_condition_permutation_group_data != NULL &&
             national_measurement_units != NULL &&
             yyjson_mut_obj_add_val(doc, relationships, "duty_expression", duty_expression) &&
             yyjson_mut_obj_add_val(doc, relationships, "measure_type", measure_type) &&
             jsonapi_add_data_relationship(doc, relationships, "legal_acts", measure_legal_act_data) &&
             yyjson_mut_obj_add_val(doc, relationships, "suspension_legal_act", suspension_legal_act) &&
             jsonapi_add_data_relationship(doc, relationships, "measure_conditions", measure_condition_data) &&
             jsonapi_add_data_relationship(doc, relationships, "measure_components", measure_component_data) &&
             yyjson_mut_obj_add_val(doc, relationships, "geographical_area", geographical_area) &&
             jsonapi_add_data_relationship(doc, relationships, "footnotes", measure_footnote_data) &&
             yyjson_mut_obj_add_val(doc, relationships, "order_number", order_number) &&
             yyjson_mut_obj_add_val(doc, relationships, "preference_code", preference_code) &&
             jsonapi_add_data_relationship(doc,
                                           relationships,
                                           "measure_condition_permutation_groups",
                                           measure_condition_permutation_group_data) &&
             jsonapi_add_data_relationship(doc,
                                           relationships,
                                           "resolved_measure_components",
                                           resolved_measure_component_data) &&
             yyjson_mut_obj_add_val(doc, relationships, "national_measurement_units", national_measurement_units) &&
             jsonapi_add_data_relationship(doc, relationships, "excluded_countries", excluded_country_data);

    if (measure->additional_code_sid != NULL) {
        yyjson_mut_val *additional_code = jsonapi_single_relationship(doc,
                                                                      additional_code_presenter_resource_type(),
                                                                      measure->additional_code_sid);
        ok = ok && additional_code != NULL &&
             yyjson_mut_obj_add_val(doc, relationships, "additional_code", additional_code);
    }

    free(duty_expression_id);
    return ok ? relationships : NULL;
}

yyjson_mut_val *measure_jsonapi_resource(yyjson_mut_doc *doc,
                                         const MeasureModel *measure,
                                         const PreferenceCodeDeclarable *preference_declarable,
                                         const MeasureComponentModelList *components,
                                         const MeasureComponentModelList *resolved_components,
                                         const MeasureConditionModelList *conditions,
                                         const MeasureConditionPermutationGroupModelList *condition_permutation_groups,
                                         const MeasureConditionComponentModelList *condition_components,
                                         const Appendix5aModelList *appendix_5as,
                                         const GeographicalAreaModelList *geographical_areas,
                                         const MeasureExcludedGeographicalAreaModelList *excluded_geographical_areas,
                                         const FootnoteList *footnotes,
                                         const MeasureLegalActModelList *legal_acts)
{
    (void)condition_components;
    (void)appendix_5as;
    (void)geographical_areas;
    if (doc == NULL || measure == NULL || measure->measure_sid == NULL) {
        return NULL;
    }

    const char *id = measure_presenter_id(measure);
    if (id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc, measure_presenter_resource_type(), id);
    yyjson_mut_val *attributes = measure_attributes(doc, measure, components, resolved_components);
    yyjson_mut_val *relationships = measure_relationships(doc,
                                                          measure,
                                                          preference_declarable,
                                                          components,
                                                          resolved_components,
                                                          conditions,
                                                          condition_permutation_groups,
                                                          excluded_geographical_areas,
                                                          footnotes,
                                                          legal_acts);
    if (resource == NULL ||
        attributes == NULL ||
        relationships == NULL ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        return NULL;
    }

    return resource;
}

int measure_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    MeasureModel model = {
        .measure_sid = "123",
        .measure_type_id = "103",
        .geographical_area_id = "1011",
        .trade_movement_code = "0",
        .measure_type_series_id = "Q",
        .effective_start_date = "2024-01-01T00:00:00.000Z",
    };
    MeasureComponentModelList components = { 0 };
    MeasureConditionModelList conditions = { 0 };
    MeasureConditionComponentModelList condition_components = { 0 };
    GeographicalAreaModelList geographical_areas = { 0 };
    MeasureExcludedGeographicalAreaModelList excluded_geographical_areas = { 0 };
    FootnoteList footnotes = { 0 };
    MeasureLegalActModelList legal_acts = { 0 };
    yyjson_mut_val *resource = measure_jsonapi_resource(doc,
                                                        &model,
                                                        &(PreferenceCodeDeclarable){ 0 },
                                                        &components,
                                                        &(MeasureComponentModelList){ 0 },
                                                        &conditions,
                                                        &(MeasureConditionPermutationGroupModelList){ 0 },
                                                        &condition_components,
                                                        &(Appendix5aModelList){ 0 },
                                                        &geographical_areas,
                                                        &excluded_geographical_areas,
                                                        &footnotes,
                                                        &legal_acts);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
