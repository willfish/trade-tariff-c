#include "request/commodity_include_plan.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

static void default_plan_matches_rails_commodity_defaults(void **state)
{
    (void)state;

    CommodityIncludePlan plan = commodity_include_plan_default();
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_SECTION));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_CHAPTER_GUIDES));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_HEADING));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_ANCESTORS));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER_DEFINITION));
}

static void parses_supported_include_subset(void **state)
{
    (void)state;

    CommodityIncludePlan plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "section,chapter,import_measures.preference_code",
        &plan,
        invalid,
        sizeof(invalid));

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_OK);
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_SECTION));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_CHAPTER));
    assert_true(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE));
    assert_false(commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_IMPORT_MEASURES));
}

static void reports_unsupported_include_path(void **state)
{
    (void)state;

    CommodityIncludePlan plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "section,import_measures.bad_relation",
        &plan,
        invalid,
        sizeof(invalid));

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_UNSUPPORTED);
    assert_string_equal(invalid, "import_measures.bad_relation");
}

static void enabled_any_matches_selected_relationship_families(void **state)
{
    (void)state;

    CommodityIncludePlan plan = commodity_include_plan_empty();
    char invalid[64];
    CommodityIncludeParseResult result = commodity_include_plan_parse(
        "chapter,import_measures.measure_type",
        &plan,
        invalid,
        sizeof(invalid));
    const CommodityIncludeId matching_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_TYPE,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_TYPE,
    };
    const CommodityIncludeId missing_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER,
        COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER,
    };

    assert_int_equal(result, COMMODITY_INCLUDE_PARSE_OK);
    assert_true(commodity_include_plan_enabled_any(&plan, matching_ids, 2));
    assert_false(commodity_include_plan_enabled_any(&plan, missing_ids, 2));
    assert_false(commodity_include_plan_enabled_any(NULL, matching_ids, 2));
    assert_false(commodity_include_plan_enabled_any(&plan, NULL, 2));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(default_plan_matches_rails_commodity_defaults),
        cmocka_unit_test(parses_supported_include_subset),
        cmocka_unit_test(reports_unsupported_include_path),
        cmocka_unit_test(enabled_any_matches_selected_relationship_families),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
