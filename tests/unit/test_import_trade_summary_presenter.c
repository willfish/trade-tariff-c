#include "presenters/import_trade_summary_presenter.h"

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

static MeasureModel measure(const char *sid,
                            const char *measure_type_id,
                            const char *geographical_area_id)
{
    return (MeasureModel){
        .measure_sid = copy_literal(sid),
        .measure_type_id = copy_literal(measure_type_id),
        .geographical_area_id = copy_literal(geographical_area_id),
        .trade_movement_code = copy_literal("0"),
    };
}

static MeasureComponentModel component(const char *measure_sid,
                                       const char *duty_expression_id,
                                       const char *duty_amount)
{
    return (MeasureComponentModel){
        .measure_sid = copy_literal(measure_sid),
        .duty_expression_id = copy_literal(duty_expression_id),
        .duty_amount = copy_literal(duty_amount),
    };
}

static void builds_summary_from_single_matching_import_measure_types(void **state)
{
    (void)state;

    MeasureModelList measures = { 0 };
    MeasureComponentModelList components = { 0 };
    ImportTradeSummaryModel summary = { 0 };

    assert_true(measure_model_list_append(&measures, measure("1", "103", "1011")));
    assert_true(measure_model_list_append(&measures, measure("2", "142", "CA")));
    assert_true(measure_model_list_append(&measures, measure("3", "143", "CA")));
    assert_true(measure_component_model_list_append(&components, component("1", "01", "8.5")));
    assert_true(measure_component_model_list_append(&components, component("2", "01", "0")));
    assert_true(measure_component_model_list_append(&components, component("3", "01", "12")));

    assert_true(import_trade_summary_presenter_build(&measures, &components, &summary));
    assert_string_equal(import_trade_summary_presenter_resource_type(), "import_trade_summary");
    assert_non_null(summary.id);
    assert_ptr_equal(import_trade_summary_presenter_id(&summary), summary.id);
    assert_string_equal(summary.basic_third_country_duty, "8.5 %");
    assert_string_equal(summary.preferential_tariff_duty, "0 %");
    assert_string_equal(summary.preferential_quota_duty, "12 %");

    import_trade_summary_model_free(&summary);
    measure_model_list_free(&measures);
    measure_component_model_list_free(&components);
}

static void omits_summary_values_when_measure_group_is_not_single(void **state)
{
    (void)state;

    MeasureModelList measures = { 0 };
    MeasureComponentModelList components = { 0 };
    ImportTradeSummaryModel summary = { 0 };

    assert_true(measure_model_list_append(&measures, measure("1", "103", "1011")));
    assert_true(measure_model_list_append(&measures, measure("2", "103", "1011")));
    assert_true(measure_component_model_list_append(&components, component("1", "01", "8.5")));
    assert_true(measure_component_model_list_append(&components, component("2", "01", "9.5")));

    assert_true(import_trade_summary_presenter_build(&measures, &components, &summary));
    assert_non_null(summary.id);
    assert_null(summary.basic_third_country_duty);
    assert_null(summary.preferential_tariff_duty);
    assert_null(summary.preferential_quota_duty);

    import_trade_summary_model_free(&summary);
    measure_model_list_free(&measures);
    measure_component_model_list_free(&components);
}

static void derives_identity(void **state)
{
    (void)state;

    ImportTradeSummaryModel summary = {
        .id = "abc",
    };

    assert_string_equal(import_trade_summary_presenter_resource_type(), "import_trade_summary");
    assert_string_equal(import_trade_summary_presenter_id(&summary), "abc");
    assert_null(import_trade_summary_presenter_id(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
        cmocka_unit_test(builds_summary_from_single_matching_import_measure_types),
        cmocka_unit_test(omits_summary_values_when_measure_group_is_not_single),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
