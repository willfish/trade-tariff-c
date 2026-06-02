#include "presenters/classification_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

static void derives_section_chapter_and_heading_identity(void **state)
{
    (void)state;

    ClassificationSectionModel section = { .id = "15" };
    ClassificationChapterModel chapter = { .goods_nomenclature_sid = "44810" };
    ClassificationHeadingModel heading = { .goods_nomenclature_sid = "45432" };

    assert_string_equal(classification_section_presenter_resource_type(), "section");
    assert_string_equal(classification_section_presenter_id(&section), "15");
    assert_string_equal(classification_chapter_presenter_resource_type(), "chapter");
    assert_string_equal(classification_chapter_presenter_id(&chapter), "44810");
    assert_string_equal(classification_heading_presenter_resource_type(), "heading");
    assert_string_equal(classification_heading_presenter_id(&heading), "45432");
}

static void returns_null_identity_for_absent_models(void **state)
{
    (void)state;

    assert_null(classification_section_presenter_id(NULL));
    assert_null(classification_chapter_presenter_id(NULL));
    assert_null(classification_heading_presenter_id(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_section_chapter_and_heading_identity),
        cmocka_unit_test(returns_null_identity_for_absent_models),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
