#include "renderers/commodity_tree_slice_renderer.h"

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

static GoodsNomenclatureTreeNode node(const char *sid,
                                      const char *item_id,
                                      const char *suffix,
                                      const char *indents,
                                      const char *position,
                                      const char *depth,
                                      const char *description,
                                      int leaf)
{
    return (GoodsNomenclatureTreeNode){
        .goods_nomenclature_sid = copy_literal(sid),
        .goods_nomenclature_item_id = copy_literal(item_id),
        .producline_suffix = copy_literal(suffix),
        .number_indents = copy_literal(indents),
        .position = copy_literal(position),
        .depth = copy_literal(depth),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .validity_end_date = NULL,
        .description = copy_literal(description),
        .leaf = leaf,
    };
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
        .measure_type_description = copy_literal("Value added tax"),
        .geographical_area_sid = copy_literal("1011"),
        .geographical_area_id = copy_literal("1011"),
        .additional_code_sid = strcmp(sid, "1") == 0 ? copy_literal("9001") : NULL,
        .additional_code_type_id = strcmp(sid, "1") == 0 ? copy_literal("8") : NULL,
        .additional_code_id = strcmp(sid, "1") == 0 ? copy_literal("123") : NULL,
        .ordernumber = strcmp(sid, "1") == 0 ? copy_literal("060004") : NULL,
        .trade_movement_code = copy_literal(trade_movement_code),
        .measure_type_series_id = copy_literal("C"),
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
        .measurement_unit_qualifier_code = copy_literal("X"),
        .duty_expression_description = copy_literal("Percentage duty"),
    };
}

static MeasureTypeModel measure_type(const char *id, const char *description)
{
    return (MeasureTypeModel){
        .measure_type_id = copy_literal(id),
        .description = copy_literal(description),
        .measure_type_series_id = copy_literal("C"),
        .measure_type_series_description = copy_literal("Duties"),
        .measure_component_applicable_code = copy_literal("1"),
        .order_number_capture_code = copy_literal("2"),
        .trade_movement_code = copy_literal("0"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static MeasurementUnitModel measurement_unit(const char *code)
{
    return (MeasurementUnitModel){
        .measurement_unit_code = copy_literal(code),
        .description = copy_literal("Kilogram"),
        .abbreviation = copy_literal("kg"),
    };
}

static MeasurementUnitQualifierModel measurement_unit_qualifier(const char *code)
{
    return (MeasurementUnitQualifierModel){
        .measurement_unit_qualifier_code = copy_literal(code),
        .description = copy_literal("per hectolitre"),
    };
}

static MeasureConditionModel condition(const char *measure_sid,
                                       const char *condition_sid)
{
    return (MeasureConditionModel){
        .measure_condition_sid = copy_literal(condition_sid),
        .measure_sid = copy_literal(measure_sid),
        .condition_code = copy_literal("B"),
        .component_sequence_number = copy_literal("1"),
    };
}

static MeasureConditionModel condition_with_code(const char *measure_sid,
                                                 const char *condition_sid,
                                                 const char *condition_code)
{
    MeasureConditionModel model = condition(measure_sid, condition_sid);
    free(model.condition_code);
    model.condition_code = copy_literal(condition_code);
    return model;
}

static MeasureConditionCodeModel condition_code(const char *code, const char *description)
{
    return (MeasureConditionCodeModel){
        .condition_code = copy_literal(code),
        .description = copy_literal(description),
        .validity_start_date = copy_literal("1970-01-01T00:00:00.000Z"),
    };
}

static MeasureConditionComponentModel condition_component(const char *condition_sid,
                                                          const char *duty_expression_id)
{
    return (MeasureConditionComponentModel){
        .measure_condition_sid = copy_literal(condition_sid),
        .duty_expression_id = copy_literal(duty_expression_id),
        .duty_amount = copy_literal("8.95"),
    };
}

static GeographicalAreaModel geographical_area(const char *sid,
                                               const char *area_id,
                                               const char *parent_sid)
{
    return (GeographicalAreaModel){
        .geographical_area_sid = copy_literal(sid),
        .geographical_area_id = copy_literal(area_id),
        .description = copy_literal("Area description"),
        .parent_geographical_area_sid = parent_sid == NULL ? NULL : copy_literal(parent_sid),
    };
}

static MeasureExcludedGeographicalAreaModel excluded_area(const char *measure_sid,
                                                          const char *area_id)
{
    return (MeasureExcludedGeographicalAreaModel){
        .measure_sid = copy_literal(measure_sid),
        .geographical_area_sid = copy_literal("200"),
        .geographical_area_id = copy_literal(area_id),
    };
}

static AdditionalCodeModel additional_code(const char *sid)
{
    return (AdditionalCodeModel){
        .additional_code_sid = copy_literal(sid),
        .additional_code_type_id = copy_literal("8"),
        .additional_code = copy_literal("123"),
        .description = copy_literal("Additional duty"),
    };
}

static ClassificationModel classification(void)
{
    return (ClassificationModel){
        .section = {
            .id = copy_literal("15"),
            .numeral = copy_literal("XV"),
            .title = copy_literal("Base metals and articles of base metal"),
            .position = copy_literal("15"),
        },
        .chapter = {
            .goods_nomenclature_sid = copy_literal("44810"),
            .goods_nomenclature_item_id = copy_literal("7200000000"),
            .description = copy_literal("IRON AND STEEL"),
            .validity_start_date = copy_literal("1971-12-31T00:00:00.000Z"),
        },
        .heading = {
            .goods_nomenclature_sid = copy_literal("45432"),
            .goods_nomenclature_item_id = copy_literal("7212000000"),
            .description = copy_literal("Flat-rolled products"),
            .validity_start_date = copy_literal("1972-01-01T00:00:00.000Z"),
        },
        .has_section = 1,
        .has_chapter = 1,
        .has_heading = 1,
    };
}

static ImportTradeSummaryModel import_trade_summary(void)
{
    return (ImportTradeSummaryModel){
        .id = copy_literal("summary-id"),
        .basic_third_country_duty = copy_literal("8.5 %"),
    };
}

static GuideModel guide(const char *id)
{
    return (GuideModel){
        .id = copy_literal(id),
        .title = copy_literal("Using a commodity code"),
        .url = copy_literal("https://www.gov.uk/guidance/using-a-commodity-code"),
    };
}

static Footnote footnote(const char *measure_sid,
                         const char *code)
{
    return (Footnote){
        .measure_sid = measure_sid == NULL ? NULL : copy_literal(measure_sid),
        .id = copy_literal(code),
        .code = copy_literal(code),
        .description = copy_literal("Footnote text"),
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

static QuotaOrderNumberModel quota_order_number(const char *sid, const char *order_number_id)
{
    return (QuotaOrderNumberModel){
        .quota_order_number_sid = copy_literal(sid),
        .quota_order_number_id = copy_literal(order_number_id),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static QuotaDefinitionModel quota_definition(const char *sid,
                                             const char *order_number_sid,
                                             const char *order_number_id)
{
    return (QuotaDefinitionModel){
        .quota_definition_sid = copy_literal(sid),
        .quota_order_number_sid = copy_literal(order_number_sid),
        .quota_order_number_id = copy_literal(order_number_id),
        .initial_volume = copy_literal("1000.0"),
        .balance = copy_literal("900.0"),
        .measurement_unit_description = copy_literal("Kilogram"),
        .measurement_unit_abbreviation = copy_literal("kg"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static QuotaClosedAndTransferredEventModel quota_transfer_event(const char *quota_definition_sid,
                                                                const char *target_quota_definition_sid)
{
    return (QuotaClosedAndTransferredEventModel){
        .quota_definition_sid = copy_literal(quota_definition_sid),
        .occurrence_timestamp = copy_literal("2024-01-31T00:00:00.000Z"),
        .target_quota_definition_sid = copy_literal(target_quota_definition_sid),
        .closing_date = copy_literal("2024-01-31"),
        .transferred_amount = copy_literal("12.5"),
        .quota_definition_validity_start_date = copy_literal("2023-01-01T00:00:00.000Z"),
        .quota_definition_validity_end_date = copy_literal("2023-12-31T00:00:00.000Z"),
        .quota_definition_measurement_unit_description = copy_literal("Kilogram"),
        .quota_definition_measurement_unit_abbreviation = copy_literal("kg"),
        .target_quota_definition_validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .target_quota_definition_validity_end_date = copy_literal("2024-12-31T00:00:00.000Z"),
        .target_quota_definition_measurement_unit_description = copy_literal("Kilogram"),
        .target_quota_definition_measurement_unit_abbreviation = copy_literal("kg"),
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

static yyjson_val *resource_with_type_and_id(yyjson_val *resources, const char *type, const char *id)
{
    size_t idx;
    size_t max;
    yyjson_val *resource;

    yyjson_arr_foreach(resources, idx, max, resource)
    {
        const char *resource_type = string_at(resource, "type", NULL);
        const char *resource_id = string_at(resource, "id", NULL);
        if (resource_type != NULL && resource_id != NULL &&
            strcmp(resource_type, type) == 0 &&
            strcmp(resource_id, id) == 0) {
            return resource;
        }
    }

    return NULL;
}

static size_t resource_count_with_type_and_id(yyjson_val *resources, const char *type, const char *id)
{
    size_t count = 0;
    size_t idx;
    size_t max;
    yyjson_val *resource;

    yyjson_arr_foreach(resources, idx, max, resource)
    {
        const char *resource_type = string_at(resource, "type", NULL);
        const char *resource_id = string_at(resource, "id", NULL);
        if (resource_type != NULL && resource_id != NULL &&
            strcmp(resource_type, type) == 0 &&
            strcmp(resource_id, id) == 0) {
            count++;
        }
    }

    return count;
}

static void renders_root_and_ancestor_slice(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300",
                          "8708999790",
                          "80",
                          "4",
                          "870899979080",
                          "6",
                          "Consigned from Brazil; consigned from Israel",
                          1),
        .import_trade_summary = import_trade_summary(),
    };

    assert_true(goods_nomenclature_tree_node_list_append(
        &aggregate.ancestors,
        node("100", "8708000000", "10", "1", "870800000010", "3", "Parts and accessories", 0)));
    assert_true(goods_nomenclature_tree_node_list_append(
        &aggregate.ancestors,
        node("200", "8708990000", "10", "2", "870899000010", "4", "Other", 0)));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *data = yyjson_obj_get(root, "data");
    yyjson_val *attributes = yyjson_obj_get(data, "attributes");
    yyjson_val *duty_calculator = obj_path(yyjson_obj_get(data, "meta"), "duty_calculator", NULL);
    yyjson_val *relationships = yyjson_obj_get(data, "relationships");
    yyjson_val *ancestor_data = obj_path(yyjson_obj_get(relationships, "ancestors"), "data", NULL);
    yyjson_val *import_measures = obj_path(yyjson_obj_get(relationships, "import_measures"), "data", NULL);
    yyjson_val *export_measures = obj_path(yyjson_obj_get(relationships, "export_measures"), "data", NULL);
    yyjson_val *import_trade_summary_ref = obj_path(yyjson_obj_get(relationships, "import_trade_summary"), "data", NULL);
    yyjson_val *included = yyjson_obj_get(root, "included");

    assert_string_equal(string_at(data, "id", NULL), "300");
    assert_string_equal(string_at(data, "type", NULL), "commodity");
    assert_string_equal(string_at(attributes, "goods_nomenclature_item_id", NULL), "8708999790");
    assert_string_equal(string_at(attributes, "producline_suffix", NULL), "80");
    assert_string_equal(string_at(attributes, "description", NULL), "Consigned from Brazil; consigned from Israel");
    assert_true(yyjson_get_bool(yyjson_obj_get(attributes, "consigned")));
    assert_string_equal(string_at(attributes, "consigned_from", NULL), "Brazil, Israel");
    assert_true(yyjson_is_null(yyjson_obj_get(attributes, "basic_duty_rate")));
    assert_false(yyjson_get_bool(yyjson_obj_get(attributes, "meursing_code")));
    assert_string_equal(string_at(duty_calculator, "source", NULL), "uk");
    assert_false(yyjson_get_bool(yyjson_obj_get(duty_calculator, "zero_mfn_duty")));
    assert_false(yyjson_get_bool(yyjson_obj_get(duty_calculator, "trade_defence")));
    assert_false(yyjson_get_bool(yyjson_obj_get(duty_calculator, "entry_price_system")));
    assert_true(yyjson_get_bool(yyjson_obj_get(attributes, "declarable")));
    assert_int_equal(yyjson_arr_size(ancestor_data), 2);
    assert_int_equal(yyjson_arr_size(import_measures), 0);
    assert_int_equal(yyjson_arr_size(export_measures), 0);
    assert_string_equal(string_at(import_trade_summary_ref, "id", NULL), "summary-id");
    assert_string_equal(string_at(import_trade_summary_ref, "type", NULL), "import_trade_summary");
    assert_int_equal(yyjson_arr_size(included), 3);

    yyjson_val *first_ref = yyjson_arr_get(ancestor_data, 0);
    yyjson_val *first_included = yyjson_arr_get(included, 0);
    yyjson_val *first_included_attrs = yyjson_obj_get(first_included, "attributes");
    assert_string_equal(string_at(first_ref, "id", NULL), "100");
    assert_string_equal(string_at(first_ref, "type", NULL), "commodity");
    assert_string_equal(string_at(first_included_attrs, "goods_nomenclature_item_id", NULL), "8708000000");
    assert_string_equal(string_at(first_included_attrs, "description_plain", NULL), "Parts and accessories");

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void include_plan_controls_included_resources_without_removing_relationships(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790", "80", "4", "870899979080", "6", "Other parts", 1),
        .import_trade_summary = import_trade_summary(),
    };

    assert_true(goods_nomenclature_tree_node_list_append(
        &aggregate.ancestors,
        node("100", "8708000000", "10", "1", "870800000010", "3", "Parts and accessories", 0)));
    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("1", "103", "0")));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("1", "01")));

    CommodityIncludePlan plan = commodity_include_plan_empty();
    char *json = commodity_tree_slice_render_jsonapi_with_plan(&aggregate, "uk", &plan);
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationships = yyjson_obj_get(yyjson_obj_get(root, "data"), "relationships");
    yyjson_val *ancestor_data = obj_path(yyjson_obj_get(relationships, "ancestors"), "data", NULL);
    yyjson_val *import_measure_data = obj_path(yyjson_obj_get(relationships, "import_measures"), "data", NULL);
    yyjson_val *included = yyjson_obj_get(root, "included");

    assert_int_equal(yyjson_arr_size(ancestor_data), 1);
    assert_int_equal(yyjson_arr_size(import_measure_data), 1);
    assert_int_equal(yyjson_arr_size(included), 0);

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_typed_measure_relationships_and_resources(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790", "80", "4", "870899979080", "6", "Other parts", 1),
        .import_trade_summary = import_trade_summary(),
    };

    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("1", "103", "0")));
    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("2", "109", "1")));
    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("3", "109", "2")));
    assert_true(measure_type_model_list_append(&aggregate.measure_types, measure_type("103", "Third country duty")));
    assert_true(measure_type_model_list_append(&aggregate.measure_types, measure_type("109", "Supplementary unit")));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("1", "01")));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("3", "01")));
    assert_true(measurement_unit_model_list_append(&aggregate.measurement_units, measurement_unit("KGM")));
    assert_true(measurement_unit_qualifier_model_list_append(&aggregate.measurement_unit_qualifiers, measurement_unit_qualifier("X")));
    assert_true(measure_condition_model_list_append(&aggregate.measure_conditions, condition("1", "789")));
    assert_true(measure_condition_code_model_list_append(&aggregate.measure_condition_codes, condition_code("B", "Condition text")));
    assert_true(measure_condition_component_model_list_append(&aggregate.measure_condition_components, condition_component("789", "01")));
    assert_true(geographical_area_model_list_append(&aggregate.geographical_areas, geographical_area("100", "1011", NULL)));
    assert_true(geographical_area_model_list_append(&aggregate.geographical_areas, geographical_area("200", "AD", "100")));
    assert_true(measure_excluded_geographical_area_model_list_append(&aggregate.excluded_geographical_areas, excluded_area("1", "AD")));
    assert_true(additional_code_model_list_append(&aggregate.additional_codes, additional_code("9001")));
    assert_true(footnote_list_append(&aggregate.measure_footnotes, footnote("1", "CD123")));
    assert_true(footnote_list_append(&aggregate.measure_footnotes, footnote("3", "CD123")));
    assert_true(measure_legal_act_model_list_append(&aggregate.measure_legal_acts, suspension_legal_act("1", "R230002")));
    assert_true(quota_order_number_model_list_append(&aggregate.quota_order_numbers, quota_order_number("777", "060004")));
    assert_true(quota_definition_model_list_append(&aggregate.quota_definitions, quota_definition("888", "777", "060004")));
    assert_true(quota_closed_and_transferred_event_model_list_append(
        &aggregate.quota_closed_and_transferred_events,
        quota_transfer_event("777", "888")));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(yyjson_obj_get(root, "data"), "attributes");
    yyjson_val *duty_calculator = obj_path(yyjson_obj_get(yyjson_obj_get(root, "data"), "meta"), "duty_calculator", NULL);
    yyjson_val *relationships = yyjson_obj_get(yyjson_obj_get(root, "data"), "relationships");
    yyjson_val *import_measures = obj_path(yyjson_obj_get(relationships, "import_measures"), "data", NULL);
    yyjson_val *export_measures = obj_path(yyjson_obj_get(relationships, "export_measures"), "data", NULL);
    yyjson_val *included = yyjson_obj_get(root, "included");

    assert_int_equal(yyjson_arr_size(import_measures), 2);
    assert_int_equal(yyjson_arr_size(export_measures), 2);
    assert_string_equal(string_at(attributes, "basic_duty_rate", NULL), "8.95 EUC");
    assert_false(yyjson_get_bool(yyjson_obj_get(attributes, "meursing_code")));
    assert_false(yyjson_get_bool(yyjson_obj_get(duty_calculator, "zero_mfn_duty")));
    assert_false(yyjson_get_bool(yyjson_obj_get(duty_calculator, "trade_defence")));
    assert_string_equal(string_at(yyjson_arr_get(import_measures, 0), "id", NULL), "1");
    assert_string_equal(string_at(yyjson_arr_get(import_measures, 1), "id", NULL), "3");
    assert_string_equal(string_at(yyjson_arr_get(export_measures, 0), "id", NULL), "2");
    assert_string_equal(string_at(yyjson_arr_get(export_measures, 1), "id", NULL), "3");
    assert_string_equal(string_at(yyjson_arr_get(included, 0), "type", NULL), "measure");
    assert_string_equal(string_at(yyjson_arr_get(included, 2), "id", NULL), "3");
    assert_int_equal(yyjson_arr_size(included), 26);
    yyjson_val *preference_code = resource_with_type_and_id(included, "preference_code", "100");
    yyjson_val *first_duty_expression = resource_with_type_and_id(included, "duty_expression", "1-duty_expression");
    yyjson_val *second_duty_expression = resource_with_type_and_id(included, "duty_expression", "2-duty_expression");
    yyjson_val *third_duty_expression = resource_with_type_and_id(included, "duty_expression", "3-duty_expression");
    yyjson_val *third_country_measure_type = resource_with_type_and_id(included, "measure_type", "103");
    yyjson_val *supplementary_measure_type = resource_with_type_and_id(included, "measure_type", "109");
    assert_non_null(preference_code);
    assert_non_null(first_duty_expression);
    assert_non_null(second_duty_expression);
    assert_non_null(third_duty_expression);
    assert_non_null(third_country_measure_type);
    assert_non_null(supplementary_measure_type);
    assert_string_equal(string_at(obj_path(preference_code, "attributes", NULL), "description", NULL),
                        "Erga Omnes third country duty rates");
    assert_string_equal(string_at(obj_path(first_duty_expression, "attributes", NULL), "base", NULL), "8.95 % EUC / KGM");
    assert_string_equal(string_at(obj_path(first_duty_expression, "attributes", NULL), "formatted_base", NULL), "<span>8.95</span> % EUC / KGM");
    assert_string_equal(string_at(obj_path(first_duty_expression, "attributes", NULL), "verbose_duty", NULL), "8.95% EUC / KGM");
    assert_string_equal(string_at(obj_path(second_duty_expression, "attributes", NULL), "base", NULL), "");
    assert_string_equal(string_at(obj_path(third_country_measure_type, "attributes", NULL), "description", NULL), "Third country duty");
    assert_int_equal(yyjson_get_int(yyjson_obj_get(obj_path(third_country_measure_type, "attributes", NULL), "measure_component_applicable_code")), 1);
    assert_non_null(resource_with_type_and_id(included, "import_trade_summary", "summary-id"));
    assert_non_null(resource_with_type_and_id(included, "measure_component", "1-01"));
    assert_non_null(resource_with_type_and_id(included, "measurement_unit", "KGM"));
    assert_non_null(resource_with_type_and_id(included, "measurement_unit_qualifier", "X"));
    assert_non_null(resource_with_type_and_id(included, "measure_condition", "789"));
    yyjson_val *condition_code_resource = resource_with_type_and_id(included, "measure_condition_code", "B");
    assert_non_null(condition_code_resource);
    assert_string_equal(string_at(obj_path(condition_code_resource, "attributes", NULL), "description", NULL), "Condition text");
    assert_non_null(resource_with_type_and_id(included, "measure_condition_component", "789-01"));
    assert_non_null(resource_with_type_and_id(included, "geographical_area", "1011"));
    assert_non_null(resource_with_type_and_id(included, "geographical_area", "AD"));
    assert_non_null(resource_with_type_and_id(included, "additional_code", "9001"));
    assert_non_null(resource_with_type_and_id(included, "footnote", "CD123"));
    assert_non_null(resource_with_type_and_id(included, "legal_act", "R230002"));
    yyjson_val *suspension_legal_act = resource_with_type_and_id(included, "suspension_legal_act", "R230002");
    assert_non_null(suspension_legal_act);
    assert_string_equal(string_at(obj_path(suspension_legal_act, "attributes", NULL), "validity_start_date", NULL), "2024-01-01");
    assert_non_null(resource_with_type_and_id(included, "order_number", "060004"));
    yyjson_val *definition_resource = resource_with_type_and_id(included, "definition", "888");
    yyjson_val *transfer_event_resource = resource_with_type_and_id(
        included,
        "quota_closed_and_transferred_event",
        "777-2024-01-31T00:00:00.000Z");
    assert_non_null(definition_resource);
    assert_non_null(transfer_event_resource);
    assert_string_equal(string_at(obj_path(transfer_event_resource, "attributes", NULL), "closing_date", NULL), "2024-01-31");
    yyjson_val *first_measure_relationships = yyjson_obj_get(yyjson_arr_get(included, 0), "relationships");
    yyjson_val *first_measure_components = obj_path(yyjson_obj_get(first_measure_relationships, "measure_components"), "data", NULL);
    yyjson_val *first_measure_conditions = obj_path(yyjson_obj_get(first_measure_relationships, "measure_conditions"), "data", NULL);
    yyjson_val *excluded_countries = obj_path(yyjson_obj_get(first_measure_relationships, "excluded_countries"), "data", NULL);
    yyjson_val *additional_code_ref = obj_path(yyjson_obj_get(first_measure_relationships, "additional_code"), "data", NULL);
    yyjson_val *footnote_refs = obj_path(yyjson_obj_get(first_measure_relationships, "footnotes"), "data", NULL);
    yyjson_val *order_number_ref = obj_path(yyjson_obj_get(first_measure_relationships, "order_number"), "data", NULL);
    yyjson_val *suspension_ref = obj_path(yyjson_obj_get(first_measure_relationships, "suspension_legal_act"), "data", NULL);
    yyjson_val *preference_code_ref = obj_path(yyjson_obj_get(first_measure_relationships, "preference_code"), "data", NULL);
    yyjson_val *duty_expression_ref = obj_path(yyjson_obj_get(first_measure_relationships, "duty_expression"), "data", NULL);
    assert_int_equal(yyjson_arr_size(first_measure_components), 1);
    assert_string_equal(string_at(yyjson_arr_get(first_measure_components, 0), "id", NULL), "1-01");
    assert_int_equal(yyjson_arr_size(first_measure_conditions), 1);
    assert_string_equal(string_at(yyjson_arr_get(first_measure_conditions, 0), "id", NULL), "789");
    yyjson_val *condition_relationships = yyjson_obj_get(resource_with_type_and_id(included, "measure_condition", "789"), "relationships");
    yyjson_val *condition_code_ref = obj_path(yyjson_obj_get(condition_relationships, "measure_condition_code"), "data", NULL);
    assert_string_equal(string_at(condition_code_ref, "id", NULL), "B");
    assert_string_equal(string_at(condition_code_ref, "type", NULL), "measure_condition_code");
    assert_int_equal(yyjson_arr_size(excluded_countries), 1);
    assert_string_equal(string_at(yyjson_arr_get(excluded_countries, 0), "id", NULL), "AD");
    assert_string_equal(string_at(additional_code_ref, "id", NULL), "9001");
    assert_string_equal(string_at(preference_code_ref, "id", NULL), "100");
    assert_string_equal(string_at(duty_expression_ref, "id", NULL), "1-duty_expression");
    assert_string_equal(string_at(suspension_ref, "type", NULL), "suspension_legal_act");
    assert_string_equal(string_at(suspension_ref, "id", NULL), "R230002");
    assert_int_equal(yyjson_arr_size(footnote_refs), 1);
    assert_string_equal(string_at(yyjson_arr_get(footnote_refs, 0), "id", NULL), "CD123");
    assert_string_equal(string_at(order_number_ref, "id", NULL), "060004");
    yyjson_val *definition_relationships = yyjson_obj_get(definition_resource, "relationships");
    yyjson_val *incoming_transfer_ref = obj_path(
        yyjson_obj_get(definition_relationships, "incoming_quota_closed_and_transferred_event"),
        "data",
        NULL);
    assert_string_equal(string_at(incoming_transfer_ref, "type", NULL), "quota_closed_and_transferred_event");
    assert_string_equal(string_at(incoming_transfer_ref, "id", NULL), "777-2024-01-31T00:00:00.000Z");

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_typed_duty_calculator_scalar_meta(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "0808108000", "80", "4", "080810800080", "6", "Other apples", 1),
        .import_trade_summary = import_trade_summary(),
    };

    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("1", "103", "0")));
    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("2", "552", "0")));
    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("3", "672", "0")));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("1", "01")));
    free(aggregate.measure_components.items[0].duty_amount);
    aggregate.measure_components.items[0].duty_amount = copy_literal("0");
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("2", "01")));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("3", "12")));
    assert_true(measure_condition_model_list_append(&aggregate.measure_conditions, condition_with_code("1", "789", "V")));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "xi");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *duty_calculator = obj_path(yyjson_obj_get(yyjson_obj_get(yyjson_doc_get_root(doc), "data"), "meta"),
                                           "duty_calculator",
                                           NULL);

    assert_non_null(duty_calculator);
    assert_string_equal(string_at(duty_calculator, "source", NULL), "xi");
    assert_true(yyjson_get_bool(yyjson_obj_get(duty_calculator, "entry_price_system")));
    assert_true(yyjson_get_bool(yyjson_obj_get(duty_calculator, "meursing_code")));
    assert_true(yyjson_get_bool(yyjson_obj_get(duty_calculator, "trade_defence")));
    assert_true(yyjson_get_bool(yyjson_obj_get(duty_calculator, "zero_mfn_duty")));
    assert_true(yyjson_is_obj(yyjson_obj_get(duty_calculator, "applicable_additional_codes")));
    assert_true(yyjson_is_obj(yyjson_obj_get(duty_calculator, "applicable_measure_units")));
    assert_true(yyjson_is_obj(yyjson_obj_get(duty_calculator, "applicable_vat_options")));

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_typed_applicable_vat_options(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "3000000000", "80", "4", "300000000080", "6", "Other", 1),
        .import_trade_summary = import_trade_summary(),
    };

    MeasureModel vat = measure("9", "305", "0");
    free(vat.measure_type_description);
    vat.measure_type_description = copy_literal("Value added tax");
    assert_true(measure_model_list_append(&aggregate.applicable_measures, vat));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("9", "01")));
    free(aggregate.measure_components.items[0].duty_amount);
    aggregate.measure_components.items[0].duty_amount = copy_literal("20");
    assert_true(measurement_unit_model_list_append(&aggregate.measurement_units, measurement_unit("KGM")));
    assert_true(measurement_unit_qualifier_model_list_append(&aggregate.measurement_unit_qualifiers, measurement_unit_qualifier("X")));
    assert_true(geographical_area_model_list_append(&aggregate.geographical_areas, geographical_area("100", "1011", NULL)));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *duty_calculator = obj_path(yyjson_obj_get(yyjson_obj_get(yyjson_doc_get_root(doc), "data"), "meta"),
                                           "duty_calculator",
                                           NULL);
    yyjson_val *vat_options = yyjson_obj_get(duty_calculator, "applicable_vat_options");

    assert_true(yyjson_is_obj(vat_options));
    assert_string_equal(yyjson_get_str(yyjson_obj_get(vat_options, "VAT")), "Value added tax (20%)");

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_typed_applicable_additional_codes(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "3000000000", "80", "4", "300000000080", "6", "Other", 1),
        .import_trade_summary = import_trade_summary(),
    };

    MeasureModel coded = measure("1", "552", "0");
    free(coded.measure_type_description);
    coded.measure_type_description = copy_literal("Anti-dumping duty");
    free(coded.additional_code_sid);
    coded.additional_code_sid = copy_literal("9001");
    assert_true(measure_model_list_append(&aggregate.applicable_measures, coded));

    MeasureModel none = measure("2", "552", "0");
    free(none.measure_type_description);
    none.measure_type_description = copy_literal("Anti-dumping duty");
    free(none.additional_code_sid);
    none.additional_code_sid = NULL;
    free(none.additional_code_type_id);
    none.additional_code_type_id = NULL;
    free(none.additional_code_id);
    none.additional_code_id = NULL;
    assert_true(measure_model_list_append(&aggregate.applicable_measures, none));

    assert_true(additional_code_model_list_append(&aggregate.additional_codes, additional_code("9001")));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *duty_calculator = obj_path(yyjson_obj_get(yyjson_obj_get(yyjson_doc_get_root(doc), "data"), "meta"),
                                           "duty_calculator",
                                           NULL);
    yyjson_val *groups = yyjson_obj_get(duty_calculator, "applicable_additional_codes");
    yyjson_val *group = yyjson_obj_get(groups, "552");
    yyjson_val *heading = yyjson_obj_get(group, "heading");
    yyjson_val *codes = yyjson_obj_get(group, "additional_codes");

    assert_true(yyjson_is_obj(groups));
    assert_string_equal(string_at(group, "measure_type_description", NULL), "Anti-dumping duty");
    assert_string_equal(string_at(heading, "overlay", NULL), "From which company are you buying these goods?");
    assert_int_equal(yyjson_arr_size(codes), 2);
    assert_string_equal(string_at(yyjson_arr_get(codes, 0), "code", NULL), "8123");
    assert_string_equal(string_at(yyjson_arr_get(codes, 0), "overlay", NULL), "Additional duty");
    assert_string_equal(string_at(yyjson_arr_get(codes, 0), "measure_sid", NULL), "1");
    assert_string_equal(string_at(yyjson_arr_get(codes, 1), "code", NULL), "none");
    assert_string_equal(string_at(yyjson_arr_get(codes, 1), "overlay", NULL), "No additional code");
    assert_string_equal(string_at(yyjson_arr_get(codes, 1), "measure_sid", NULL), "2");

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_typed_applicable_measure_units(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "3000000000", "80", "4", "300000000080", "6", "Other", 1),
        .import_trade_summary = import_trade_summary(),
    };

    assert_true(measure_model_list_append(&aggregate.applicable_measures, measure("1", "103", "0")));
    assert_true(measure_component_model_list_append(&aggregate.measure_components, component("1", "01")));
    free(aggregate.measure_components.items[0].measurement_unit_code);
    aggregate.measure_components.items[0].measurement_unit_code = copy_literal("ASV");
    assert_true(measurement_unit_model_list_append(&aggregate.measurement_units, measurement_unit("ASV")));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *duty_calculator = obj_path(yyjson_obj_get(yyjson_obj_get(yyjson_doc_get_root(doc), "data"), "meta"),
                                           "duty_calculator",
                                           NULL);
    yyjson_val *measure_units = yyjson_obj_get(duty_calculator, "applicable_measure_units");
    yyjson_val *asv = yyjson_obj_get(measure_units, "ASV");
    yyjson_val *hlt = yyjson_obj_get(measure_units, "HLT");

    assert_true(yyjson_is_obj(measure_units));
    assert_non_null(asv);
    assert_non_null(hlt);
    assert_string_equal(string_at(asv, "measurement_unit_code", NULL), "ASV");
    assert_string_equal(string_at(asv, "unit", NULL), "percent");
    assert_string_equal(string_at(hlt, "measurement_unit_code", NULL), "HLT");
    assert_string_equal(string_at(hlt, "coerced_measurement_unit_code", NULL), "LTR");
    assert_string_equal(string_at(hlt, "multiplier", NULL), "0.01");

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_classification_relationships_and_resources(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "7212600050", "80", "3", "721260005080", "5", "Not further worked", 1),
        .classification = classification(),
        .import_trade_summary = import_trade_summary(),
    };
    assert_true(guide_model_list_append(&aggregate.chapter_guides, guide("12")));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationships = yyjson_obj_get(yyjson_obj_get(root, "data"), "relationships");
    yyjson_val *section = obj_path(yyjson_obj_get(relationships, "section"), "data", NULL);
    yyjson_val *chapter = obj_path(yyjson_obj_get(relationships, "chapter"), "data", NULL);
    yyjson_val *heading = obj_path(yyjson_obj_get(relationships, "heading"), "data", NULL);
    yyjson_val *included = yyjson_obj_get(root, "included");
    yyjson_val *chapter_resource = yyjson_arr_get(included, 1);
    yyjson_val *chapter_guides = obj_path(yyjson_obj_get(yyjson_obj_get(chapter_resource, "relationships"), "guides"), "data", NULL);

    assert_string_equal(string_at(section, "id", NULL), "15");
    assert_string_equal(string_at(section, "type", NULL), "section");
    assert_string_equal(string_at(chapter, "id", NULL), "44810");
    assert_string_equal(string_at(chapter, "type", NULL), "chapter");
    assert_string_equal(string_at(heading, "id", NULL), "45432");
    assert_string_equal(string_at(heading, "type", NULL), "heading");
    assert_int_equal(yyjson_arr_size(chapter_guides), 1);
    assert_string_equal(string_at(yyjson_arr_get(chapter_guides, 0), "id", NULL), "12");
    assert_int_equal(yyjson_arr_size(included), 5);
    assert_string_equal(string_at(yyjson_arr_get(included, 0), "type", NULL), "section");
    assert_string_equal(string_at(yyjson_arr_get(included, 1), "type", NULL), "chapter");
    assert_string_equal(string_at(yyjson_arr_get(included, 2), "type", NULL), "guide");
    assert_string_equal(string_at(yyjson_arr_get(included, 2), "id", NULL), "12");
    assert_string_equal(string_at(yyjson_arr_get(included, 3), "type", NULL), "heading");
    assert_string_equal(string_at(yyjson_arr_get(included, 4), "type", NULL), "import_trade_summary");

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void renders_commodity_footnote_relationships_and_resources(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "7212600050", "80", "3", "721260005080", "5", "Not further worked", 1),
        .import_trade_summary = import_trade_summary(),
    };

    assert_true(footnote_list_append(&aggregate.commodity_footnotes, footnote(NULL, "TN701")));
    assert_true(footnote_list_append(&aggregate.commodity_footnotes, footnote(NULL, "TN702")));
    assert_true(footnote_list_append(&aggregate.measure_footnotes, footnote("1", "TN701")));

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(json);

    yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
    assert_non_null(doc);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationships = yyjson_obj_get(yyjson_obj_get(root, "data"), "relationships");
    yyjson_val *footnotes = obj_path(yyjson_obj_get(relationships, "footnotes"), "data", NULL);
    yyjson_val *included = yyjson_obj_get(root, "included");

    assert_int_equal(yyjson_arr_size(footnotes), 2);
    assert_string_equal(string_at(yyjson_arr_get(footnotes, 0), "id", NULL), "TN701");
    assert_string_equal(string_at(yyjson_arr_get(footnotes, 1), "id", NULL), "TN702");
    assert_int_equal(yyjson_arr_size(included), 3);
    assert_non_null(resource_with_type_and_id(included, "footnote", "TN701"));
    assert_non_null(resource_with_type_and_id(included, "footnote", "TN702"));
    assert_non_null(resource_with_type_and_id(included, "import_trade_summary", "summary-id"));
    assert_int_equal(resource_count_with_type_and_id(included, "footnote", "TN701"), 1);

    yyjson_doc_free(doc);
    free(json);
    commodity_aggregate_free(&aggregate);
}

static void typed_slice_preserves_root_and_ancestor_fields(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790", "80", "4", "870899979080", "6", "Other parts", 1),
        .import_trade_summary = import_trade_summary(),
    };
    assert_true(goods_nomenclature_tree_node_list_append(
        &aggregate.ancestors,
        node("200", "8708990000", "10", "2", "870899000010", "4", "Other", 0)));

    char *typed_json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    assert_non_null(typed_json);

    const char *legacy_json =
        "{\"data\":{\"id\":\"300\",\"type\":\"commodity\",\"attributes\":{"
        "\"goods_nomenclature_item_id\":\"8708999790\","
        "\"producline_suffix\":\"80\","
        "\"description\":\"Other parts\","
        "\"number_indents\":\"4\","
        "\"description_plain\":\"Other parts\","
        "\"declarable\":true},"
        "\"relationships\":{\"ancestors\":{\"data\":[{\"id\":\"200\",\"type\":\"commodity\"}]}}},"
        "\"included\":[{\"id\":\"200\",\"type\":\"commodity\",\"attributes\":{"
        "\"goods_nomenclature_item_id\":\"8708990000\","
        "\"description\":\"Other\","
        "\"number_indents\":\"2\","
        "\"description_plain\":\"Other\"}}]}";

    yyjson_doc *typed_doc = yyjson_read(typed_json, strlen(typed_json), 0);
    yyjson_doc *legacy_doc = yyjson_read(legacy_json, strlen(legacy_json), 0);
    assert_non_null(typed_doc);
    assert_non_null(legacy_doc);

    yyjson_val *typed_data = yyjson_obj_get(yyjson_doc_get_root(typed_doc), "data");
    yyjson_val *legacy_data = yyjson_obj_get(yyjson_doc_get_root(legacy_doc), "data");
    yyjson_val *typed_attrs = yyjson_obj_get(typed_data, "attributes");
    yyjson_val *legacy_attrs = yyjson_obj_get(legacy_data, "attributes");
    yyjson_val *typed_relationships = yyjson_obj_get(typed_data, "relationships");
    yyjson_val *legacy_relationships = yyjson_obj_get(legacy_data, "relationships");
    yyjson_val *typed_ancestor_refs = obj_path(yyjson_obj_get(typed_relationships, "ancestors"), "data", NULL);
    yyjson_val *legacy_ancestor_refs = obj_path(yyjson_obj_get(legacy_relationships, "ancestors"), "data", NULL);

    assert_string_equal(string_at(typed_data, "id", NULL), string_at(legacy_data, "id", NULL));
    assert_string_equal(string_at(typed_attrs, "goods_nomenclature_item_id", NULL),
                        string_at(legacy_attrs, "goods_nomenclature_item_id", NULL));
    assert_string_equal(string_at(typed_attrs, "number_indents", NULL),
                        string_at(legacy_attrs, "number_indents", NULL));
    assert_true(yyjson_get_bool(yyjson_obj_get(typed_attrs, "declarable")) ==
                yyjson_get_bool(yyjson_obj_get(legacy_attrs, "declarable")));
    assert_int_equal(yyjson_arr_size(typed_ancestor_refs), yyjson_arr_size(legacy_ancestor_refs));
    assert_string_equal(string_at(yyjson_arr_get(typed_ancestor_refs, 0), "id", NULL),
                        string_at(yyjson_arr_get(legacy_ancestor_refs, 0), "id", NULL));

    yyjson_doc_free(typed_doc);
    yyjson_doc_free(legacy_doc);
    free(typed_json);
    commodity_aggregate_free(&aggregate);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_root_and_ancestor_slice),
        cmocka_unit_test(include_plan_controls_included_resources_without_removing_relationships),
        cmocka_unit_test(renders_typed_measure_relationships_and_resources),
        cmocka_unit_test(renders_typed_duty_calculator_scalar_meta),
        cmocka_unit_test(renders_typed_applicable_vat_options),
        cmocka_unit_test(renders_typed_applicable_additional_codes),
        cmocka_unit_test(renders_typed_applicable_measure_units),
        cmocka_unit_test(renders_classification_relationships_and_resources),
        cmocka_unit_test(renders_commodity_footnote_relationships_and_resources),
        cmocka_unit_test(typed_slice_preserves_root_and_ancestor_fields),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
