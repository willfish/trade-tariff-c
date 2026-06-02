#include "presenters/guide_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

static void derives_identity(void **state)
{
    (void)state;

    GuideModel guide = {
        .id = "12",
        .title = "Using a commodity code",
        .url = "https://www.gov.uk/guidance/using-a-commodity-code",
    };

    assert_string_equal(guide_presenter_resource_type(), "guide");
    assert_string_equal(guide_presenter_id(&guide), "12");
    assert_null(guide_presenter_id(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
