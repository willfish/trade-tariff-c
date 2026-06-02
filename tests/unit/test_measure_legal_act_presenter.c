#include "presenters/measure_legal_act_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity_code_url_and_description(void **state)
{
    (void)state;

    MeasureLegalActModel model = {
        .measure_sid = "123",
        .measure_type_id = "103",
        .regulation_id = "R230001",
        .source = "generating",
        .information_text = "Regulation text",
    };

    char *code = measure_legal_act_presenter_regulation_code(&model);
    char *url = measure_legal_act_presenter_regulation_url(&model);

    assert_string_equal(measure_legal_act_presenter_resource_type(), "legal_act");
    assert_string_equal(measure_legal_act_presenter_id(&model), "R230001");
    assert_false(measure_legal_act_presenter_is_suspension(&model));
    assert_string_equal(code, "R0001/23");
    assert_non_null(strstr(url, "CELEX%3A32023R0001"));
    assert_string_equal(measure_legal_act_presenter_description(&model), "Regulation text");

    free(code);
    free(url);
}

static void hides_reduced_info_fields(void **state)
{
    (void)state;

    MeasureLegalActModel measure_type_reduced = {
        .measure_type_id = "305",
        .regulation_id = "R230001",
        .information_text = "Regulation text",
    };
    MeasureLegalActModel regulation_reduced = {
        .measure_type_id = "103",
        .regulation_id = "IYY99990",
        .information_text = "Regulation text",
    };

    char *type_code = measure_legal_act_presenter_regulation_code(&measure_type_reduced);
    char *type_url = measure_legal_act_presenter_regulation_url(&measure_type_reduced);
    char *regulation_code = measure_legal_act_presenter_regulation_code(&regulation_reduced);

    assert_true(measure_legal_act_presenter_reduced_info(&measure_type_reduced));
    assert_true(measure_legal_act_presenter_reduced_info(&regulation_reduced));
    assert_string_equal(type_code, "");
    assert_string_equal(type_url, "");
    assert_string_equal(regulation_code, "");
    assert_null(measure_legal_act_presenter_description(&measure_type_reduced));
    free(type_code);
    free(type_url);
    free(regulation_code);

    char *suspension_code = measure_legal_act_presenter_suspension_regulation_code(&measure_type_reduced);
    assert_string_equal(suspension_code, "R0001/23");

    free(suspension_code);
}

static void finds_suspension_for_measure(void **state)
{
    (void)state;

    MeasureLegalActModelList legal_acts = { 0 };
    MeasureLegalActModel generating = {
        .measure_sid = strdup("123"),
        .regulation_id = strdup("R230001"),
        .source = strdup("generating"),
    };
    MeasureLegalActModel suspension = {
        .measure_sid = strdup("123"),
        .regulation_id = strdup("R230002"),
        .source = strdup("suspension"),
    };
    MeasureLegalActModel other_measure_suspension = {
        .measure_sid = strdup("456"),
        .regulation_id = strdup("R230003"),
        .source = strdup("suspension"),
    };
    assert_true(measure_legal_act_model_list_append(&legal_acts, generating));
    assert_true(measure_legal_act_model_list_append(&legal_acts, suspension));
    assert_true(measure_legal_act_model_list_append(&legal_acts, other_measure_suspension));

    const MeasureLegalActModel *found = measure_legal_act_presenter_suspension_for_measure(&legal_acts, "123");

    assert_non_null(found);
    assert_true(measure_legal_act_presenter_is_suspension(found));
    assert_string_equal(measure_legal_act_presenter_id(found), "R230002");

    measure_legal_act_model_list_free(&legal_acts);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_code_url_and_description),
        cmocka_unit_test(hides_reduced_info_fields),
        cmocka_unit_test(finds_suspension_for_measure),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
