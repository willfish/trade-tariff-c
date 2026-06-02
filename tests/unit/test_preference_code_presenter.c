#include "presenters/preference_code_presenter.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

static MeasureModel measure(const char *sid, const char *measure_type_id, const char *trade_movement_code)
{
    return (MeasureModel){
        .measure_sid = (char *)sid,
        .measure_type_id = (char *)measure_type_id,
        .geographical_area_id = (char *)"1011",
        .trade_movement_code = (char *)trade_movement_code,
    };
}

static MeasureConditionModel certificate_condition(const char *measure_sid,
                                                   const char *certificate_type_code,
                                                   const char *certificate_code)
{
    return (MeasureConditionModel){
        .measure_sid = (char *)measure_sid,
        .certificate_type_code = (char *)certificate_type_code,
        .certificate_code = (char *)certificate_code,
    };
}

static void maps_autonomous_suspension_without_authorised_use(void **state)
{
    (void)state;
    PreferenceCodeDeclarable declarable = { 0 };
    PreferenceCodeMeasure measure = {
        .measure_type_id = "112",
        .geographical_area_id = "1011",
        .import = 1,
        .authorised_use = 0,
        .gsp_or_dcts = 0,
    };

    assert_string_equal(preference_code_presenter_determine_code(&declarable, &measure), "110");
    assert_string_equal(preference_code_presenter_description("110"), "Erga omnes autonomous tariff suspension");
}

static void exposes_jsonapi_identity(void **state)
{
    (void)state;

    assert_string_equal(preference_code_presenter_resource_type(), "preference_code");
    assert_string_equal(preference_code_presenter_id("110"), "110");
    assert_null(preference_code_presenter_id(NULL));
}

static void maps_preference_quota_authorised_use(void **state)
{
    (void)state;
    PreferenceCodeDeclarable declarable = { 0 };
    PreferenceCodeMeasure measure = {
        .measure_type_id = "143",
        .geographical_area_id = "1011",
        .import = 1,
        .authorised_use = 1,
        .gsp_or_dcts = 0,
    };

    assert_string_equal(preference_code_presenter_determine_code(&declarable, &measure), "323");
}

static void ignores_export_measures(void **state)
{
    (void)state;
    PreferenceCodeDeclarable declarable = { 0 };
    PreferenceCodeMeasure measure = {
        .measure_type_id = "112",
        .geographical_area_id = "1011",
        .import = 0,
        .authorised_use = 0,
        .gsp_or_dcts = 0,
    };

    assert_null(preference_code_presenter_determine_code(&declarable, &measure));
}

static void derives_context_and_measure_code_from_loaded_rows(void **state)
{
    (void)state;

    MeasureModelList measures = {
        .items = (MeasureModel[]){
            measure("1", "464", "0"),
            measure("2", "103", "0"),
            measure("3", "112", "0"),
        },
        .len = 3,
    };
    MeasureConditionModelList conditions = {
        .items = (MeasureConditionModel[]){
            certificate_condition("2", "A", "123"),
            certificate_condition("3", "N", "990"),
        },
        .len = 2,
    };

    PreferenceCodeDeclarable declarable =
        preference_code_presenter_declarable_from_measures(&measures, &conditions);

    assert_true(declarable.authorised_use_provisions_submission);
    assert_true(declarable.special_nature);
    assert_string_equal(preference_code_presenter_code_for_measure(&declarable,
                                                                   &measures.items[1],
                                                                   &conditions),
                        "140");
    assert_string_equal(preference_code_presenter_code_for_measure(&declarable,
                                                                   &measures.items[2],
                                                                   &conditions),
                        "115");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(maps_autonomous_suspension_without_authorised_use),
        cmocka_unit_test(exposes_jsonapi_identity),
        cmocka_unit_test(maps_preference_quota_authorised_use),
        cmocka_unit_test(ignores_export_measures),
        cmocka_unit_test(derives_context_and_measure_code_from_loaded_rows),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
