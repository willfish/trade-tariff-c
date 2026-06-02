#include "loaders/commodity_load_plan.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

static void empty_include_plan_avoids_optional_relationship_loads(void **state)
{
    (void)state;

    CommodityIncludePlan include_plan = commodity_include_plan_empty();
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(&include_plan);

    assert_true(load_plan.ancestors);
    assert_true(load_plan.classification);
    assert_true(load_plan.commodity_footnotes);
    assert_true(load_plan.applicable_measures);
    assert_true(load_plan.measure_components);
    assert_true(load_plan.measurement_units);
    assert_true(load_plan.import_trade_summary);
    assert_true(load_plan.measure_conditions);
    assert_false(load_plan.chapter_guides);
    assert_false(load_plan.measure_types);
    assert_false(load_plan.resolved_measure_components);
    assert_false(load_plan.measurement_unit_qualifiers);
    assert_false(load_plan.appendix_5as);
    assert_false(load_plan.measure_condition_codes);
    assert_false(load_plan.measure_condition_components);
    assert_false(load_plan.geographical_areas);
    assert_false(load_plan.excluded_geographical_areas);
    assert_true(load_plan.additional_codes);
    assert_false(load_plan.measure_footnotes);
    assert_false(load_plan.measure_legal_acts);
    assert_false(load_plan.quota_order_numbers);
    assert_false(load_plan.quota_definitions);
}

static void default_include_plan_preserves_current_eager_relationship_loads(void **state)
{
    (void)state;

    CommodityIncludePlan include_plan = commodity_include_plan_default();
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(&include_plan);

    assert_true(load_plan.ancestors);
    assert_true(load_plan.classification);
    assert_true(load_plan.chapter_guides);
    assert_true(load_plan.commodity_footnotes);
    assert_true(load_plan.applicable_measures);
    assert_true(load_plan.measure_components);
    assert_true(load_plan.measure_types);
    assert_true(load_plan.resolved_measure_components);
    assert_true(load_plan.measurement_units);
    assert_true(load_plan.measurement_unit_qualifiers);
    assert_true(load_plan.import_trade_summary);
    assert_true(load_plan.measure_conditions);
    assert_true(load_plan.appendix_5as);
    assert_true(load_plan.measure_condition_codes);
    assert_true(load_plan.measure_condition_components);
    assert_true(load_plan.geographical_areas);
    assert_true(load_plan.excluded_geographical_areas);
    assert_true(load_plan.additional_codes);
    assert_true(load_plan.measure_footnotes);
    assert_true(load_plan.measure_legal_acts);
    assert_true(load_plan.quota_order_numbers);
    assert_true(load_plan.quota_definitions);
}

static void measure_resource_includes_keep_relationship_member_models(void **state)
{
    (void)state;

    CommodityIncludePlan include_plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "import_measures",
        &include_plan,
        invalid,
        sizeof(invalid));
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(&include_plan);

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_OK);
    assert_true(load_plan.applicable_measures);
    assert_true(load_plan.measure_components);
    assert_true(load_plan.measurement_units);
    assert_true(load_plan.measure_conditions);
    assert_true(load_plan.resolved_measure_components);
    assert_true(load_plan.excluded_geographical_areas);
    assert_true(load_plan.measure_footnotes);
    assert_true(load_plan.measure_legal_acts);
    assert_true(load_plan.additional_codes);
    assert_false(load_plan.geographical_areas);
    assert_false(load_plan.measure_types);
    assert_false(load_plan.quota_order_numbers);
}

static void geographical_area_includes_load_area_models_without_full_measure_resources(void **state)
{
    (void)state;

    CommodityIncludePlan include_plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "import_measures.geographical_area.contained_geographical_areas",
        &include_plan,
        invalid,
        sizeof(invalid));
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(&include_plan);

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_OK);
    assert_true(load_plan.geographical_areas);
    assert_true(load_plan.applicable_measures);
    assert_true(load_plan.measure_components);
    assert_true(load_plan.measurement_units);
    assert_true(load_plan.measure_conditions);
    assert_false(load_plan.excluded_geographical_areas);
    assert_true(load_plan.additional_codes);
    assert_false(load_plan.measure_footnotes);
    assert_false(load_plan.measure_legal_acts);
}

static void excluded_area_includes_load_excluded_area_linkage_only(void **state)
{
    (void)state;

    CommodityIncludePlan include_plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "export_measures.excluded_geographical_areas",
        &include_plan,
        invalid,
        sizeof(invalid));
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(&include_plan);

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_OK);
    assert_true(load_plan.excluded_geographical_areas);
    assert_true(load_plan.applicable_measures);
    assert_true(load_plan.measure_components);
    assert_true(load_plan.measurement_units);
    assert_true(load_plan.measure_conditions);
    assert_false(load_plan.geographical_areas);
    assert_true(load_plan.additional_codes);
    assert_false(load_plan.measure_footnotes);
    assert_false(load_plan.measure_legal_acts);
}

static void nested_order_number_includes_load_order_number_and_definition_models(void **state)
{
    (void)state;

    CommodityIncludePlan include_plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "export_measures.order_number.definition",
        &include_plan,
        invalid,
        sizeof(invalid));
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(&include_plan);

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_OK);
    assert_true(load_plan.quota_order_numbers);
    assert_true(load_plan.quota_definitions);
    assert_false(load_plan.measure_legal_acts);
}

static void null_include_plan_disables_all_table_loads(void **state)
{
    (void)state;

    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(NULL);

    assert_false(load_plan.ancestors);
    assert_false(load_plan.classification);
    assert_false(load_plan.chapter_guides);
    assert_false(load_plan.commodity_footnotes);
    assert_false(load_plan.applicable_measures);
    assert_false(load_plan.measure_components);
    assert_false(load_plan.measure_types);
    assert_false(load_plan.resolved_measure_components);
    assert_false(load_plan.measurement_units);
    assert_false(load_plan.measurement_unit_qualifiers);
    assert_false(load_plan.import_trade_summary);
    assert_false(load_plan.measure_conditions);
    assert_false(load_plan.appendix_5as);
    assert_false(load_plan.measure_condition_codes);
    assert_false(load_plan.measure_condition_components);
    assert_false(load_plan.geographical_areas);
    assert_false(load_plan.excluded_geographical_areas);
    assert_false(load_plan.additional_codes);
    assert_false(load_plan.measure_footnotes);
    assert_false(load_plan.measure_legal_acts);
    assert_false(load_plan.quota_order_numbers);
    assert_false(load_plan.quota_definitions);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(empty_include_plan_avoids_optional_relationship_loads),
        cmocka_unit_test(default_include_plan_preserves_current_eager_relationship_loads),
        cmocka_unit_test(measure_resource_includes_keep_relationship_member_models),
        cmocka_unit_test(geographical_area_includes_load_area_models_without_full_measure_resources),
        cmocka_unit_test(excluded_area_includes_load_excluded_area_linkage_only),
        cmocka_unit_test(nested_order_number_includes_load_order_number_and_definition_models),
        cmocka_unit_test(null_include_plan_disables_all_table_loads),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
