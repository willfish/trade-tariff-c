#include "presenters/goods_nomenclature_description_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void returns_consigned_countries_like_rails(void **state)
{
    (void)state;

    const struct {
        const char *description;
        const char *expected;
    } examples[] = {
        { "consigned from Vietnam", "Vietnam" },
        { "consigned from Viet Nam", "Viet Nam" },
        { "consigned from Taiwan or Malaysia", "Taiwan or Malaysia" },
        { "consigned from Vietnam, Pakistan or the Philippines", "Vietnam, Pakistan or the Philippines" },
        { "Originating in or consigned from China:<br>- in quantities below 300 units", "China" },
        { "Consigned from T\xC3\xBCrkiye", "T\xC3\xBCrkiye" },
        { "consigned from or originating in Taiwan", "Taiwan" },
        { "Consigned from Brazil; consigned from Israel", "Brazil, Israel" },
    };

    for (size_t i = 0; i < sizeof(examples) / sizeof(examples[0]); i++) {
        GoodsNomenclatureDescriptionModel model =
            goods_nomenclature_description_new(examples[i].description, "uk");
        char *actual = goods_nomenclature_description_presenter_consigned_from(&model);
        assert_non_null(actual);
        assert_string_equal(actual, examples[i].expected);
        free(actual);
    }
}

static void returns_null_without_consigned_phrase(void **state)
{
    (void)state;

    GoodsNomenclatureDescriptionModel model =
        goods_nomenclature_description_new("some description", "uk");
    assert_null(goods_nomenclature_description_presenter_consigned_from(&model));
    assert_null(goods_nomenclature_description_presenter_consigned_from(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(returns_consigned_countries_like_rails),
        cmocka_unit_test(returns_null_without_consigned_phrase),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
