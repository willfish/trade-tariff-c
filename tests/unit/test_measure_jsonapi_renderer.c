#include "renderers/measure_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static MeasureModel measure(const char *sid,
                            const char *measure_type_id,
                            const char *trade_movement_code)
{
    return (MeasureModel){
        .measure_sid = copy_literal(sid),
        .goods_nomenclature_sid = copy_literal("300"),
        .goods_nomenclature_item_id = copy_literal("8708999790"),
        .measure_type_id = copy_literal(measure_type_id),
        .geographical_area_sid = copy_literal("1011"),
        .geographical_area_id = copy_literal("1011"),
        .trade_movement_code = copy_literal(trade_movement_code),
        .measure_type_series_id = copy_literal("C"),
        .reduction_indicator = NULL,
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .effective_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static MeasureComponentModel component(const char *measure_sid,
                                       const char *duty_expression_id)
{
    return (MeasureComponentModel){
        .measure_sid = copy_literal(measure_sid),
        .duty_expression_id = copy_literal(duty_expression_id),
        .duty_amount = copy_literal("8.95"),
        .monetary_unit_code = copy_literal("EUC"),
        .measurement_unit_code = copy_literal("KGM"),
        .duty_expression_description = copy_literal("Percentage duty"),
    };
}

static MeasureComponentModel resolved_component(const char *measure_sid,
                                                const char *resolved_for_measure_sid,
                                                const char *duty_expression_id)
{
    MeasureComponentModel model = component(measure_sid, duty_expression_id);
    model.resolved_for_measure_sid = copy_literal(resolved_for_measure_sid);
    model.resolved_meursing = 1;
    free(model.duty_amount);
    model.duty_amount = copy_literal("100.00");
    free(model.monetary_unit_code);
    model.monetary_unit_code = copy_literal("EUR");
    free(model.measurement_unit_code);
    model.measurement_unit_code = copy_literal("DTN");
    return model;
}

static MeasureConditionModel condition(const char *measure_sid,
                                       const char *condition_sid)
{
    return (MeasureConditionModel){
        .measure_condition_sid = copy_literal(condition_sid),
        .measure_sid = copy_literal(measure_sid),
        .condition_code = copy_literal("B"),
    };
}

static MeasureConditionModel certificate_condition(const char *measure_sid,
                                                   const char *condition_sid,
                                                   const char *certificate_type_code,
                                                   const char *certificate_code)
{
    MeasureConditionModel model = condition(measure_sid, condition_sid);
    model.certificate_type_code = copy_literal(certificate_type_code);
    model.certificate_code = copy_literal(certificate_code);
    return model;
}

static MeasureExcludedGeographicalAreaModel excluded_area(const char *measure_sid,
                                                          const char *area_id)
{
    return (MeasureExcludedGeographicalAreaModel){
        .measure_sid = copy_literal(measure_sid),
        .geographical_area_sid = copy_literal("456"),
        .geographical_area_id = copy_literal(area_id),
    };
}

static Footnote footnote(const char *measure_sid,
                         const char *code)
{
    return (Footnote){
        .measure_sid = copy_literal(measure_sid),
        .id = copy_literal(code),
        .code = copy_literal(code),
        .description = copy_literal("Footnote text"),
    };
}

static MeasureLegalActModel legal_act(const char *measure_sid,
                                      const char *regulation_id)
{
    return (MeasureLegalActModel){
        .measure_sid = copy_literal(measure_sid),
        .measure_type_id = copy_literal("103"),
        .regulation_id = copy_literal(regulation_id),
        .role = copy_literal("1"),
        .source = copy_literal("generating"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static MeasureLegalActModel suspension_legal_act(const char *measure_sid,
                                                 const char *regulation_id)
{
    return (MeasureLegalActModel){
        .measure_sid = copy_literal(measure_sid),
        .measure_type_id = copy_literal("103"),
        .regulation_id = copy_literal(regulation_id),
        .source = copy_literal("suspension"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .effective_start_date = copy_literal("2024-01-01"),
        .effective_end_date = copy_literal("2024-12-31"),
    };
}

static yyjson_val *obj_path(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = yyjson_obj_get(root, first);
    return second == NULL || value == NULL ? value : yyjson_obj_get(value, second);
}

static const char *string_at(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = obj_path(root, first, second);
    return yyjson_is_str(value) ? yyjson_get_str(value) : NULL;
}

static void renders_measure_resource_from_typed_model(void **state)
{
    (void)state;

    MeasureModel model = measure("123", "103", "0");
    MeasureComponentModelList components = { 0 };
    MeasureConditionModelList conditions = { 0 };
    MeasureConditionPermutationGroupModelList condition_permutation_groups = { 0 };
    MeasureConditionComponentModelList condition_components = { 0 };
    Appendix5aModelList appendix_5as = { 0 };
    GeographicalAreaModelList geographical_areas = { 0 };
    MeasureExcludedGeographicalAreaModelList excluded_geographical_areas = { 0 };
    FootnoteList footnotes = { 0 };
    MeasureLegalActModelList legal_acts = { 0 };
    assert_true(measure_component_model_list_append(&components, component("123", "01")));
    assert_true(measure_condition_model_list_append(&conditions, condition("123", "789")));
    MeasureConditionPermutationModel permutation = {
        .id = copy_literal("68053af2923e00204c3ca7c6a3150cf7"),
    };
    assert_true(measure_condition_permutation_model_add_condition_sid(&permutation, "789"));
    MeasureConditionPermutationGroupModel group = {
        .id = copy_literal("123-B"),
        .measure_sid = copy_literal("123"),
        .condition_code = copy_literal("B"),
    };
    assert_true(measure_condition_permutation_model_list_append(&group.permutations, permutation));
    assert_true(measure_condition_permutation_group_model_list_append(&condition_permutation_groups, group));
    assert_true(measure_excluded_geographical_area_model_list_append(&excluded_geographical_areas, excluded_area("123", "AD")));
    assert_true(measure_excluded_geographical_area_model_list_append(&excluded_geographical_areas, excluded_area("456", "FR")));
    assert_true(footnote_list_append(&footnotes, footnote("123", "CD123")));
    assert_true(footnote_list_append(&footnotes, footnote("456", "CD999")));
    assert_true(measure_legal_act_model_list_append(&legal_acts, legal_act("123", "R230001")));
    assert_true(measure_legal_act_model_list_append(&legal_acts, suspension_legal_act("123", "R230002")));
    assert_true(measure_legal_act_model_list_append(&legal_acts, legal_act("456", "R230002")));
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    yyjson_mut_val *resource = measure_jsonapi_resource(mut_doc,
                                                        &model,
                                                        &(PreferenceCodeDeclarable){ 0 },
                                                        &components,
                                                        &(MeasureComponentModelList){ 0 },
                                                        &conditions,
                                                        &condition_permutation_groups,
                                                        &condition_components,
                                                        &appendix_5as,
                                                        &geographical_areas,
                                                        &excluded_geographical_areas,
                                                        &footnotes,
                                                        &legal_acts);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *preference_code = obj_path(yyjson_obj_get(relationships, "preference_code"), "data", NULL);
    yyjson_val *measure_components = obj_path(yyjson_obj_get(relationships, "measure_components"), "data", NULL);
    yyjson_val *measure_conditions = obj_path(yyjson_obj_get(relationships, "measure_conditions"), "data", NULL);
    yyjson_val *condition_permutation_refs = obj_path(yyjson_obj_get(relationships, "measure_condition_permutation_groups"), "data", NULL);
    yyjson_val *excluded_countries = obj_path(yyjson_obj_get(relationships, "excluded_countries"), "data", NULL);
    yyjson_val *footnote_refs = obj_path(yyjson_obj_get(relationships, "footnotes"), "data", NULL);
    yyjson_val *legal_act_refs = obj_path(yyjson_obj_get(relationships, "legal_acts"), "data", NULL);
    yyjson_val *suspension_ref = obj_path(yyjson_obj_get(relationships, "suspension_legal_act"), "data", NULL);

    assert_string_equal(string_at(root, "id", NULL), "123");
    assert_string_equal(string_at(root, "type", NULL), "measure");
    assert_true(yyjson_get_bool(yyjson_obj_get(attributes, "import")));
    assert_false(yyjson_get_bool(yyjson_obj_get(attributes, "export")));
    assert_int_equal(yyjson_get_int(yyjson_obj_get(attributes, "id")), 123);
    assert_string_equal(string_at(preference_code, "id", NULL), "100");
    assert_int_equal(yyjson_arr_size(measure_components), 1);
    assert_string_equal(string_at(yyjson_arr_get(measure_components, 0), "id", NULL), "123-01");
    assert_int_equal(yyjson_arr_size(measure_conditions), 1);
    assert_string_equal(string_at(yyjson_arr_get(measure_conditions, 0), "id", NULL), "789");
    assert_int_equal(yyjson_arr_size(condition_permutation_refs), 1);
    assert_string_equal(string_at(yyjson_arr_get(condition_permutation_refs, 0), "id", NULL), "123-B");
    assert_int_equal(yyjson_arr_size(excluded_countries), 1);
    assert_string_equal(string_at(yyjson_arr_get(excluded_countries, 0), "id", NULL), "AD");
    assert_int_equal(yyjson_arr_size(footnote_refs), 1);
    assert_string_equal(string_at(yyjson_arr_get(footnote_refs, 0), "id", NULL), "CD123");
    assert_int_equal(yyjson_arr_size(legal_act_refs), 2);
    assert_string_equal(string_at(yyjson_arr_get(legal_act_refs, 0), "id", NULL), "R230001");
    assert_string_equal(string_at(suspension_ref, "type", NULL), "suspension_legal_act");
    assert_string_equal(string_at(suspension_ref, "id", NULL), "R230002");
    assert_string_equal(string_at(obj_path(yyjson_obj_get(relationships, "measure_type"), "data", NULL), "id", NULL), "103");
    assert_string_equal(string_at(obj_path(yyjson_obj_get(relationships, "geographical_area"), "data", NULL), "id", NULL), "1011");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_model_free(&model);
    measure_component_model_list_free(&components);
    measure_condition_model_list_free(&conditions);
    measure_condition_permutation_group_model_list_free(&condition_permutation_groups);
    measure_excluded_geographical_area_model_list_free(&excluded_geographical_areas);
    footnote_list_free(&footnotes);
    measure_legal_act_model_list_free(&legal_acts);
}

static void renders_preference_code_from_loaded_conditions(void **state)
{
    (void)state;

    MeasureModel model = measure("123", "112", "0");
    MeasureComponentModelList components = { 0 };
    MeasureConditionModelList conditions = { 0 };
    MeasureConditionComponentModelList condition_components = { 0 };
    Appendix5aModelList appendix_5as = { 0 };
    GeographicalAreaModelList geographical_areas = { 0 };
    MeasureExcludedGeographicalAreaModelList excluded_geographical_areas = { 0 };
    FootnoteList footnotes = { 0 };
    MeasureLegalActModelList legal_acts = { 0 };
    PreferenceCodeDeclarable declarable = { 0 };
    assert_true(measure_condition_model_list_append(&conditions,
                                                    certificate_condition("123", "789", "N", "990")));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measure_jsonapi_resource(mut_doc,
                                                        &model,
                                                        &declarable,
                                                        &components,
                                                        &(MeasureComponentModelList){ 0 },
                                                        &conditions,
                                                        &(MeasureConditionPermutationGroupModelList){ 0 },
                                                        &condition_components,
                                                        &appendix_5as,
                                                        &geographical_areas,
                                                        &excluded_geographical_areas,
                                                        &footnotes,
                                                        &legal_acts);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *relationships = yyjson_obj_get(yyjson_doc_get_root(doc), "relationships");
    yyjson_val *preference_code = obj_path(yyjson_obj_get(relationships, "preference_code"), "data", NULL);
    assert_string_equal(string_at(preference_code, "id", NULL), "115");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_model_free(&model);
    measure_condition_model_list_free(&conditions);
}

static void renders_resolved_meursing_components_from_typed_models(void **state)
{
    (void)state;

    MeasureModel model = measure("123", "103", "0");
    MeasureComponentModelList components = { 0 };
    MeasureComponentModelList resolved_components = { 0 };
    MeasureConditionModelList conditions = { 0 };
    MeasureConditionComponentModelList condition_components = { 0 };
    Appendix5aModelList appendix_5as = { 0 };
    GeographicalAreaModelList geographical_areas = { 0 };
    MeasureExcludedGeographicalAreaModelList excluded_geographical_areas = { 0 };
    FootnoteList footnotes = { 0 };
    MeasureLegalActModelList legal_acts = { 0 };
    assert_true(measure_component_model_list_append(&components, component("123", "12")));
    assert_true(measure_component_model_list_append(&resolved_components, resolved_component("900", "123", "04")));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measure_jsonapi_resource(mut_doc,
                                                        &model,
                                                        &(PreferenceCodeDeclarable){ 0 },
                                                        &components,
                                                        &resolved_components,
                                                        &conditions,
                                                        &(MeasureConditionPermutationGroupModelList){ 0 },
                                                        &condition_components,
                                                        &appendix_5as,
                                                        &geographical_areas,
                                                        &excluded_geographical_areas,
                                                        &footnotes,
                                                        &legal_acts);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *resolved_refs = obj_path(yyjson_obj_get(relationships, "resolved_measure_components"), "data", NULL);

    assert_true(yyjson_get_bool(yyjson_obj_get(attributes, "meursing")));
    assert_string_equal(string_at(attributes, "resolved_duty_expression", NULL), "+ <span>100.00</span> EUR / DTN");
    assert_int_equal(yyjson_arr_size(resolved_refs), 1);
    assert_string_equal(string_at(yyjson_arr_get(resolved_refs, 0), "id", NULL), "900-04");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_model_free(&model);
    measure_component_model_list_free(&components);
    measure_component_model_list_free(&resolved_components);
}

static void renders_import_and_export_relationship_refs_in_source_order(void **state)
{
    (void)state;

    MeasureModelList measures = { 0 };
    assert_true(measure_model_list_append(&measures, measure("1", "103", "0")));
    assert_true(measure_model_list_append(&measures, measure("2", "109", "1")));
    assert_true(measure_model_list_append(&measures, measure("3", "109", "2")));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *import_data = measure_jsonapi_relationship_data(mut_doc, &measures, 1);
    yyjson_mut_val *export_data = measure_jsonapi_relationship_data(mut_doc, &measures, 0);
    assert_non_null(import_data);
    assert_non_null(export_data);

    yyjson_mut_val *root = yyjson_mut_obj(mut_doc);
    assert_true(yyjson_mut_obj_add_val(mut_doc, root, "import", import_data));
    assert_true(yyjson_mut_obj_add_val(mut_doc, root, "export", export_data));
    yyjson_mut_doc_set_root(mut_doc, root);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *read_root = yyjson_doc_get_root(doc);
    yyjson_val *imports = yyjson_obj_get(read_root, "import");
    yyjson_val *exports = yyjson_obj_get(read_root, "export");
    assert_int_equal(yyjson_arr_size(imports), 2);
    assert_int_equal(yyjson_arr_size(exports), 2);
    assert_string_equal(string_at(yyjson_arr_get(imports, 0), "id", NULL), "1");
    assert_string_equal(string_at(yyjson_arr_get(imports, 1), "id", NULL), "3");
    assert_string_equal(string_at(yyjson_arr_get(exports, 0), "id", NULL), "2");
    assert_string_equal(string_at(yyjson_arr_get(exports, 1), "id", NULL), "3");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_model_list_free(&measures);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_measure_resource_from_typed_model),
        cmocka_unit_test(renders_preference_code_from_loaded_conditions),
        cmocka_unit_test(renders_resolved_meursing_components_from_typed_models),
        cmocka_unit_test(renders_import_and_export_relationship_refs_in_source_order),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
