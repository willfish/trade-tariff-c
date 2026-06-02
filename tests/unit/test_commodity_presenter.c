#include "presenters/commodity_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static MeasureModel measure(const char *sid,
                            const char *measure_type_id,
                            const char *movement)
{
    return (MeasureModel){
        .measure_sid = (char *)sid,
        .measure_type_id = (char *)measure_type_id,
        .trade_movement_code = (char *)movement,
    };
}

static MeasureModel described_measure(const char *sid,
                                      const char *measure_type_id,
                                      const char *movement,
                                      const char *description)
{
    MeasureModel model = measure(sid, measure_type_id, movement);
    model.measure_type_description = (char *)description;
    return model;
}

static MeasureModel series_measure(const char *sid,
                                   const char *measure_type_id,
                                   const char *movement,
                                   const char *series_id)
{
    MeasureModel model = measure(sid, measure_type_id, movement);
    model.measure_type_series_id = (char *)series_id;
    return model;
}

static MeasureComponentModel component(const char *measure_sid,
                                       const char *duty_expression_id,
                                       const char *amount)
{
    return (MeasureComponentModel){
        .measure_sid = (char *)measure_sid,
        .duty_expression_id = (char *)duty_expression_id,
        .duty_amount = (char *)amount,
    };
}

static MeasureComponentModel unit_component(const char *measure_sid,
                                            const char *measurement_unit_code,
                                            const char *measurement_unit_qualifier_code)
{
    MeasureComponentModel model = component(measure_sid, "01", "0");
    model.measurement_unit_code = (char *)measurement_unit_code;
    model.measurement_unit_qualifier_code = (char *)measurement_unit_qualifier_code;
    return model;
}

static CommodityAggregate aggregate_for(MeasureModel *measures,
                                        size_t measure_count,
                                        MeasureComponentModel *components,
                                        size_t component_count)
{
    return (CommodityAggregate){
        .applicable_measures = {
            .items = measures,
            .len = measure_count,
            .cap = measure_count,
        },
        .measure_components = {
            .items = components,
            .len = component_count,
            .cap = component_count,
        },
    };
}

static CommodityAggregate aggregate_with_codes(MeasureModel *measures,
                                               size_t measure_count,
                                               MeasureComponentModel *components,
                                               size_t component_count,
                                               AdditionalCodeModel *additional_codes,
                                               size_t additional_code_count)
{
    CommodityAggregate aggregate = aggregate_for(measures, measure_count, components, component_count);
    aggregate.additional_codes.items = additional_codes;
    aggregate.additional_codes.len = additional_code_count;
    aggregate.additional_codes.cap = additional_code_count;
    return aggregate;
}

static AdditionalCodeModel additional_code(const char *sid,
                                           const char *type_id,
                                           const char *code,
                                           const char *description)
{
    return (AdditionalCodeModel){
        .additional_code_sid = (char *)sid,
        .additional_code_type_id = (char *)type_id,
        .additional_code = (char *)code,
        .description = (char *)description,
    };
}

static CommodityAggregate aggregate_with_conditions(MeasureModel *measures,
                                                    size_t measure_count,
                                                    MeasureConditionModel *conditions,
                                                    size_t condition_count)
{
    return (CommodityAggregate){
        .applicable_measures = {
            .items = measures,
            .len = measure_count,
            .cap = measure_count,
        },
        .measure_conditions = {
            .items = conditions,
            .len = condition_count,
            .cap = condition_count,
        },
    };
}

static void derives_tree_node_identity(void **state)
{
    (void)state;

    CommodityModel model = {
        .goods_nomenclature_sid = "123",
    };
    GoodsNomenclatureTreeNode node = {
        .goods_nomenclature_sid = "456",
    };

    assert_string_equal(commodity_presenter_resource_type(), "commodity");
    assert_string_equal(commodity_presenter_id(&model), "123");
    assert_string_equal(commodity_presenter_tree_node_id(&node), "456");
    assert_null(commodity_presenter_id(NULL));
    assert_null(commodity_presenter_tree_node_id(NULL));
}

static void basic_duty_rate_uses_single_import_third_country_measure(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "103", "0"),
        measure("2", "142", "0"),
    };
    MeasureComponentModel components[] = {
        component("1", "01", "8.5"),
        component("2", "01", "0"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 2, components, 2);

    char *basic_duty_rate = commodity_presenter_basic_duty_rate(&aggregate);

    assert_non_null(basic_duty_rate);
    assert_string_equal(basic_duty_rate, "8.5 %");
    free(basic_duty_rate);
}

static void basic_duty_rate_is_null_when_there_are_multiple_third_country_measures(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "103", "0"),
        measure("2", "105", "0"),
    };
    MeasureComponentModel components[] = {
        component("1", "01", "8.5"),
        component("2", "01", "10"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 2, components, 2);

    char *basic_duty_rate = commodity_presenter_basic_duty_rate(&aggregate);

    assert_null(basic_duty_rate);
}

static void meursing_code_uses_import_measure_components(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "672", "0"),
    };
    MeasureComponentModel components[] = {
        component("1", "12", "0"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 1, components, 1);

    assert_true(commodity_presenter_meursing_code(&aggregate));
}

static void meursing_code_ignores_export_measure_components(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "672", "1"),
    };
    MeasureComponentModel components[] = {
        component("1", "12", "0"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 1, components, 1);

    assert_false(commodity_presenter_meursing_code(&aggregate));
}

static void zero_mfn_duty_requires_all_import_third_country_measures_to_have_one_zero_component(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "103", "0"),
        measure("2", "105", "0"),
        measure("3", "142", "0"),
    };
    MeasureComponentModel components[] = {
        component("1", "01", "0"),
        component("2", "01", "0.0"),
        component("3", "01", "8.5"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 3, components, 3);

    assert_true(commodity_presenter_zero_mfn_duty(&aggregate));
}

static void zero_mfn_duty_is_false_when_any_third_country_measure_is_non_zero(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "103", "0"),
        measure("2", "105", "0"),
    };
    MeasureComponentModel components[] = {
        component("1", "01", "0"),
        component("2", "01", "8.5"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 2, components, 2);

    assert_false(commodity_presenter_zero_mfn_duty(&aggregate));
}

static void trade_defence_uses_import_defence_measure_types(void **state)
{
    (void)state;

    MeasureModel import_measures[] = {
        measure("1", "552", "0"),
    };
    MeasureModel export_measures[] = {
        measure("1", "552", "1"),
    };
    CommodityAggregate import_aggregate = aggregate_for(import_measures, 1, NULL, 0);
    CommodityAggregate export_aggregate = aggregate_for(export_measures, 1, NULL, 0);

    assert_true(commodity_presenter_trade_defence(&import_aggregate));
    assert_false(commodity_presenter_trade_defence(&export_aggregate));
}

static void entry_price_system_is_xi_only_and_uses_import_condition_v(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        measure("1", "103", "0"),
    };
    MeasureConditionModel conditions[] = {
        {
            .measure_sid = "1",
            .condition_code = "V",
        },
    };
    CommodityAggregate aggregate = aggregate_with_conditions(measures, 1, conditions, 1);

    assert_false(commodity_presenter_entry_price_system(&aggregate, "uk"));
    assert_true(commodity_presenter_entry_price_system(&aggregate, "xi"));
}

static void applicable_vat_options_use_import_vat_measure_components(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        described_measure("1", "305", "0", "Value added tax"),
        described_measure("2", "305", "1", "Export VAT"),
    };
    MeasureComponentModel components[] = {
        component("1", "01", "20"),
        component("2", "01", "5"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 2, components, 2);

    DutyCalculatorVatOptionModelList options = commodity_presenter_applicable_vat_options(&aggregate);

    assert_int_equal(options.len, 1);
    assert_string_equal(options.items[0].key, "VAT");
    assert_string_equal(options.items[0].description, "Value added tax (20%)");

    duty_calculator_vat_option_model_list_free(&options);
}

static void applicable_vat_options_use_additional_code_key_and_description(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        described_measure("1", "305", "0", "Value added tax"),
    };
    measures[0].additional_code_sid = "9001";
    measures[0].additional_code_type_id = "8";
    measures[0].additional_code_id = "123";
    MeasureComponentModel components[] = {
        component("1", "01", "20"),
    };
    AdditionalCodeModel additional_codes[] = {
        {
            .additional_code_sid = "9001",
            .additional_code_type_id = "8",
            .additional_code = "123",
            .description = "Reduced-rate VAT",
        },
    };
    CommodityAggregate aggregate = aggregate_with_codes(measures, 1, components, 1, additional_codes, 1);

    DutyCalculatorVatOptionModelList options = commodity_presenter_applicable_vat_options(&aggregate);

    assert_int_equal(options.len, 1);
    assert_string_equal(options.items[0].key, "8123");
    assert_string_equal(options.items[0].description, "Reduced-rate VAT");

    duty_calculator_vat_option_model_list_free(&options);
}

static void applicable_additional_codes_group_import_applicable_code_measures(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        described_measure("1", "552", "0", "Anti-dumping duty"),
        described_measure("2", "552", "0", "Anti-dumping duty"),
        described_measure("3", "552", "1", "Export anti-dumping duty"),
    };
    measures[0].additional_code_sid = "9001";
    measures[0].geographical_area_id = "CN";
    measures[1].geographical_area_id = "CN";
    measures[2].additional_code_sid = "9002";
    measures[2].geographical_area_id = "CN";
    AdditionalCodeModel additional_codes[] = {
        additional_code("9001", "8", "123", "Company specific duty"),
        additional_code("9002", "8", "999", "Export code"),
    };
    CommodityAggregate aggregate = aggregate_with_codes(measures, 3, NULL, 0, additional_codes, 2);

    DutyCalculatorAdditionalCodeGroupModelList groups = commodity_presenter_applicable_additional_codes(&aggregate);

    assert_int_equal(groups.len, 1);
    assert_string_equal(groups.items[0].measure_type_id, "552");
    assert_string_equal(groups.items[0].measure_type_description, "Anti-dumping duty");
    assert_string_equal(groups.items[0].heading_overlay, "From which company are you buying these goods?");
    assert_int_equal(groups.items[0].additional_codes.len, 2);
    assert_string_equal(groups.items[0].additional_codes.items[0].code, "8123");
    assert_string_equal(groups.items[0].additional_codes.items[0].overlay, "Company specific duty");
    assert_string_equal(groups.items[0].additional_codes.items[0].geographical_area_id, "CN");
    assert_string_equal(groups.items[0].additional_codes.items[0].measure_sid, "1");
    assert_string_equal(groups.items[0].additional_codes.items[1].code, "none");
    assert_string_equal(groups.items[0].additional_codes.items[1].overlay, "No additional code");
    assert_string_equal(groups.items[0].additional_codes.items[1].measure_sid, "2");

    duty_calculator_additional_code_group_model_list_free(&groups);
}

static void applicable_additional_codes_ignore_unknown_additional_code_types(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        described_measure("1", "999", "0", "Unknown duty"),
    };
    measures[0].additional_code_sid = "9001";
    measures[0].geographical_area_id = "CN";
    AdditionalCodeModel additional_codes[] = {
        additional_code("9001", "7", "123", "Meursing code"),
    };
    CommodityAggregate aggregate = aggregate_with_codes(measures, 1, NULL, 0, additional_codes, 1);

    DutyCalculatorAdditionalCodeGroupModelList groups = commodity_presenter_applicable_additional_codes(&aggregate);

    assert_int_equal(groups.len, 0);

    duty_calculator_additional_code_group_model_list_free(&groups);
}

static void applicable_measure_units_use_import_unit_components(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        series_measure("1", "103", "0", "C"),
        series_measure("2", "103", "1", "C"),
    };
    MeasureComponentModel components[] = {
        unit_component("1", "KGM", NULL),
        unit_component("2", "LTR", NULL),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 2, components, 2);

    DutyCalculatorMeasureUnitModelList units = commodity_presenter_applicable_measure_units(&aggregate);

    assert_int_equal(units.len, 1);
    assert_string_equal(units.items[0].key, "KGM");
    assert_string_equal(units.items[0].measurement_unit_code.value, "KGM");
    assert_true(units.items[0].measurement_unit_qualifier_code.is_null);
    assert_string_equal(units.items[0].unit.value, "kilograms");
    assert_string_equal(units.items[0].measurement_unit_type.value, "weight");

    duty_calculator_measure_unit_model_list_free(&units);
}

static void applicable_measure_units_expand_compound_overlay_units(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        series_measure("1", "103", "0", "C"),
    };
    MeasureComponentModel components[] = {
        unit_component("1", "ASV", "X"),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 1, components, 1);

    DutyCalculatorMeasureUnitModelList units = commodity_presenter_applicable_measure_units(&aggregate);

    assert_int_equal(units.len, 2);
    assert_string_equal(units.items[0].key, "ASV");
    assert_string_equal(units.items[0].measurement_unit_code.value, "ASV");
    assert_string_equal(units.items[0].unit.value, "percent");
    assert_string_equal(units.items[1].key, "HLT");
    assert_string_equal(units.items[1].measurement_unit_code.value, "HLT");
    assert_string_equal(units.items[1].coerced_measurement_unit_code.value, "LTR");
    assert_string_equal(units.items[1].multiplier.value, "0.01");

    duty_calculator_measure_unit_model_list_free(&units);
}

static void applicable_measure_units_ignore_non_unit_series(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        series_measure("1", "103", "0", "P"),
    };
    MeasureComponentModel components[] = {
        unit_component("1", "KGM", NULL),
    };
    CommodityAggregate aggregate = aggregate_for(measures, 1, components, 1);

    DutyCalculatorMeasureUnitModelList units = commodity_presenter_applicable_measure_units(&aggregate);

    assert_int_equal(units.len, 0);

    duty_calculator_measure_unit_model_list_free(&units);
}

static void applicable_measure_units_fallback_to_loaded_unit_rows(void **state)
{
    (void)state;

    MeasureModel measures[] = {
        series_measure("1", "103", "0", "C"),
    };
    MeasureComponentModel components[] = {
        unit_component("1", "ZZZ", "A"),
    };
    MeasurementUnitModel measurement_units[] = {
        {
            .measurement_unit_code = "ZZZ",
            .description = "Test unit",
            .abbreviation = "tu",
        },
    };
    CommodityAggregate aggregate = aggregate_for(measures, 1, components, 1);
    aggregate.measurement_units.items = measurement_units;
    aggregate.measurement_units.len = 1;
    aggregate.measurement_units.cap = 1;

    DutyCalculatorMeasureUnitModelList units = commodity_presenter_applicable_measure_units(&aggregate);

    assert_int_equal(units.len, 1);
    assert_string_equal(units.items[0].key, "ZZZA");
    assert_string_equal(units.items[0].measurement_unit_code.value, "ZZZ");
    assert_string_equal(units.items[0].measurement_unit_qualifier_code.value, "A");
    assert_string_equal(units.items[0].abbreviation.value, "tu");
    assert_string_equal(units.items[0].unit_question.value, "Please enter unit: Test unit");
    assert_string_equal(units.items[0].unit_hint.value, "Please correctly enter unit: Test unit");
    assert_true(units.items[0].unit.is_null);
    assert_false(units.items[0].expansion.present);

    duty_calculator_measure_unit_model_list_free(&units);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_tree_node_identity),
        cmocka_unit_test(basic_duty_rate_uses_single_import_third_country_measure),
        cmocka_unit_test(basic_duty_rate_is_null_when_there_are_multiple_third_country_measures),
        cmocka_unit_test(meursing_code_uses_import_measure_components),
        cmocka_unit_test(meursing_code_ignores_export_measure_components),
        cmocka_unit_test(zero_mfn_duty_requires_all_import_third_country_measures_to_have_one_zero_component),
        cmocka_unit_test(zero_mfn_duty_is_false_when_any_third_country_measure_is_non_zero),
        cmocka_unit_test(trade_defence_uses_import_defence_measure_types),
        cmocka_unit_test(entry_price_system_is_xi_only_and_uses_import_condition_v),
        cmocka_unit_test(applicable_vat_options_use_import_vat_measure_components),
        cmocka_unit_test(applicable_vat_options_use_additional_code_key_and_description),
        cmocka_unit_test(applicable_additional_codes_group_import_applicable_code_measures),
        cmocka_unit_test(applicable_additional_codes_ignore_unknown_additional_code_types),
        cmocka_unit_test(applicable_measure_units_use_import_unit_components),
        cmocka_unit_test(applicable_measure_units_expand_compound_overlay_units),
        cmocka_unit_test(applicable_measure_units_ignore_non_unit_series),
        cmocka_unit_test(applicable_measure_units_fallback_to_loaded_unit_rows),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
