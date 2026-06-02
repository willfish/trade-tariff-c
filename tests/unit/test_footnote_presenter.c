#include "presenters/footnote_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity_and_code(void **state)
{
    (void)state;

    Footnote footnote = {
        .id = "CD123",
        .code = "CD123",
        .description = "Footnote text",
    };

    assert_string_equal(footnote_presenter_resource_type(), "footnote");
    assert_string_equal(footnote_presenter_id(&footnote), "CD123");
    assert_string_equal(footnote_presenter_code(&footnote), "CD123");
    assert_null(footnote_presenter_id(NULL));
    assert_null(footnote_presenter_code(NULL));
}

static void formats_description_for_service(void **state)
{
    (void)state;

    Footnote footnote = {
        .description = "Product utilizing sodium",
    };

    char *uk_description = footnote_presenter_formatted_description(&footnote, "uk");
    char *xi_description = footnote_presenter_formatted_description(&footnote, "xi");
    assert_non_null(uk_description);
    assert_non_null(xi_description);
    assert_string_equal(uk_description, "Product utilising sodium");
    assert_string_equal(xi_description, "Product utilizing sodium");

    free(uk_description);
    free(xi_description);
}

static void formats_null_description_as_empty_string(void **state)
{
    (void)state;

    char *description = footnote_presenter_formatted_description(NULL, "uk");
    assert_non_null(description);
    assert_string_equal(description, "");
    free(description);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_and_code),
        cmocka_unit_test(formats_description_for_service),
        cmocka_unit_test(formats_null_description_as_empty_string),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
