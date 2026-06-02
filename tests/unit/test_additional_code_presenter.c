#include "presenters/additional_code_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <cmocka.h>

static void derives_resource_identity_and_code_from_typed_model(void **state)
{
    (void)state;

    AdditionalCodeModel model = {
        .additional_code_sid = "9001",
        .additional_code_type_id = "8",
        .additional_code = "123",
    };

    char *code = additional_code_presenter_code(&model);

    assert_string_equal(additional_code_presenter_resource_type(), "additional_code");
    assert_string_equal(additional_code_presenter_id(&model), "9001");
    assert_string_equal(code, "8123");

    free(code);
}

static void formats_description_in_presenter_layer(void **state)
{
    (void)state;

    AdditionalCodeModel model = {
        .description = "Additional <br><br><br>duty",
    };

    char *description = additional_code_presenter_description(&model, 1);

    assert_string_equal(description, "Additional <br><br>duty");

    free(description);
}

static void returns_null_for_missing_code_parts(void **state)
{
    (void)state;

    AdditionalCodeModel model = { .additional_code_type_id = "8" };

    assert_null(additional_code_presenter_code(&model));
    assert_null(additional_code_presenter_code(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_resource_identity_and_code_from_typed_model),
        cmocka_unit_test(formats_description_in_presenter_layer),
        cmocka_unit_test(returns_null_for_missing_code_parts),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
