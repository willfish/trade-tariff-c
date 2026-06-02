#include "domain/commodity_aggregate.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static GoodsNomenclatureTreeNode node(const char *sid, const char *item_id)
{
    return (GoodsNomenclatureTreeNode){
        .goods_nomenclature_sid = copy_literal(sid),
        .goods_nomenclature_item_id = copy_literal(item_id),
        .producline_suffix = copy_literal("80"),
        .number_indents = copy_literal("3"),
        .position = copy_literal("870899979080"),
        .depth = copy_literal("5"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .description = copy_literal("Other"),
    };
}

static MeasureModel measure(const char *sid, const char *goods_sid, const char *movement)
{
    return (MeasureModel){
        .measure_sid = copy_literal(sid),
        .goods_nomenclature_sid = copy_literal(goods_sid),
        .measure_type_id = copy_literal("103"),
        .geographical_area_id = copy_literal("1011"),
        .trade_movement_code = copy_literal(movement),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .effective_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static MeasureModel quota_measure(const char *sid, const char *order_number)
{
    MeasureModel model = measure(sid, "300", "0");
    model.ordernumber = copy_literal(order_number);
    return model;
}

static MeasureConditionModel measure_condition(const char *sid,
                                               const char *certificate_type_code,
                                               const char *certificate_code)
{
    return (MeasureConditionModel){
        .measure_condition_sid = copy_literal(sid),
        .measure_sid = copy_literal("1"),
        .condition_code = copy_literal("B"),
        .certificate_type_code = certificate_type_code == NULL ? NULL : copy_literal(certificate_type_code),
        .certificate_code = certificate_code == NULL ? NULL : copy_literal(certificate_code),
    };
}

static QuotaDefinitionModel quota_definition(const char *sid, const char *order_number)
{
    return (QuotaDefinitionModel){
        .quota_definition_sid = copy_literal(sid),
        .quota_order_number_id = copy_literal(order_number),
        .quota_order_number_sid = copy_literal("777"),
        .initial_volume = copy_literal("1000.0"),
        .balance = copy_literal("900.0"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
}

static void builds_batch_sids_in_rails_applicable_order(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790"),
    };
    assert_true(goods_nomenclature_tree_node_list_append(&aggregate.ancestors, node("100", "8708000000")));
    assert_true(goods_nomenclature_tree_node_list_append(&aggregate.ancestors, node("200", "8708990000")));

    size_t sid_count = 0;
    char **sids = commodity_aggregate_goods_nomenclature_sids(&aggregate, &sid_count);

    assert_non_null(sids);
    assert_int_equal(sid_count, 3);
    assert_string_equal(sids[0], "100");
    assert_string_equal(sids[1], "200");
    assert_string_equal(sids[2], "300");

    free(sids);
    commodity_aggregate_free(&aggregate);
}

static void owns_applicable_measures_and_derives_trade_direction_counts(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790"),
    };
    MeasureModelList measures = { 0 };
    assert_true(measure_model_list_append(&measures, measure("1", "300", "0")));
    assert_true(measure_model_list_append(&measures, measure("2", "300", "1")));
    assert_true(measure_model_list_append(&measures, measure("3", "300", "2")));

    assert_true(commodity_aggregate_set_applicable_measures(&aggregate, measures));
    assert_int_equal(aggregate.applicable_measures.len, 3);
    assert_int_equal(commodity_aggregate_import_measure_count(&aggregate), 2);
    assert_int_equal(commodity_aggregate_export_measure_count(&aggregate), 2);

    commodity_aggregate_free(&aggregate);
}

static void derives_unique_order_numbers_from_measures(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790"),
    };
    MeasureModelList measures = { 0 };
    assert_true(measure_model_list_append(&measures, quota_measure("1", "060004")));
    assert_true(measure_model_list_append(&measures, quota_measure("2", "060004")));
    assert_true(measure_model_list_append(&measures, quota_measure("3", "060005")));

    assert_true(commodity_aggregate_set_applicable_measures(&aggregate, measures));

    size_t order_number_count = 0;
    char **order_numbers = commodity_aggregate_order_numbers(&aggregate, &order_number_count);
    assert_non_null(order_numbers);
    assert_int_equal(order_number_count, 2);
    assert_string_equal(order_numbers[0], "060004");
    assert_string_equal(order_numbers[1], "060005");

    free(order_numbers);
    commodity_aggregate_free(&aggregate);
}

static void derives_unique_quota_definition_sids_from_definitions(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790"),
    };
    QuotaDefinitionModelList definitions = { 0 };
    assert_true(quota_definition_model_list_append(&definitions, quota_definition("888", "060004")));
    assert_true(quota_definition_model_list_append(&definitions, quota_definition("888", "060004")));
    assert_true(quota_definition_model_list_append(&definitions, quota_definition("999", "060005")));

    assert_true(commodity_aggregate_set_quota_definitions(&aggregate, definitions));

    size_t sid_count = 0;
    char **sids = commodity_aggregate_quota_definition_sids(&aggregate, &sid_count);
    assert_non_null(sids);
    assert_int_equal(sid_count, 2);
    assert_string_equal(sids[0], "888");
    assert_string_equal(sids[1], "999");

    free(sids);
    commodity_aggregate_free(&aggregate);
}

static void derives_unique_appendix_5a_document_codes_from_conditions(void **state)
{
    (void)state;

    CommodityAggregate aggregate = {
        .commodity = node("300", "8708999790"),
    };
    MeasureConditionModelList conditions = { 0 };
    assert_true(measure_condition_model_list_append(&conditions, measure_condition("10", "C", "990")));
    assert_true(measure_condition_model_list_append(&conditions, measure_condition("11", "C", "990")));
    assert_true(measure_condition_model_list_append(&conditions, measure_condition("12", "Y", "123")));
    assert_true(measure_condition_model_list_append(&conditions, measure_condition("13", NULL, "777")));

    assert_true(commodity_aggregate_set_measure_conditions(&aggregate, conditions));

    size_t document_code_count = 0;
    char **document_codes = commodity_aggregate_appendix_5a_document_codes(&aggregate, &document_code_count);
    assert_non_null(document_codes);
    assert_int_equal(document_code_count, 2);
    assert_string_equal(document_codes[0], "C990");
    assert_string_equal(document_codes[1], "Y123");

    for (size_t i = 0; i < document_code_count; i++) {
        free(document_codes[i]);
    }
    free(document_codes);
    commodity_aggregate_free(&aggregate);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_batch_sids_in_rails_applicable_order),
        cmocka_unit_test(owns_applicable_measures_and_derives_trade_direction_counts),
        cmocka_unit_test(derives_unique_order_numbers_from_measures),
        cmocka_unit_test(derives_unique_quota_definition_sids_from_definitions),
        cmocka_unit_test(derives_unique_appendix_5a_document_codes_from_conditions),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
