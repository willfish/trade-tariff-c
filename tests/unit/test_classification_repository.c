#include "repositories/classification_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <cmocka.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = test_malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static void hydrates_classification_row(void **state)
{
    (void)state;

    char *values[16] = {
        copy_literal("15"),
        copy_literal("XV"),
        copy_literal("Base metals and articles of base metal"),
        copy_literal("15"),
        copy_literal("Section note"),
        copy_literal("44810"),
        copy_literal("7200000000"),
        copy_literal("IRON AND STEEL"),
        copy_literal("1971-12-31T00:00:00.000Z"),
        NULL,
        copy_literal("Chapter note"),
        copy_literal("45432"),
        copy_literal("7212000000"),
        copy_literal("Flat-rolled products"),
        copy_literal("1972-01-01T00:00:00.000Z"),
        NULL,
    };
    unsigned char nulls[16] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 1,
    };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 16,
        .error = NULL,
        .ok = 1,
    };

    ClassificationModel model = { 0 };
    assert_true(classification_repository_hydrate_row(&rows, 0, &model));
    assert_true(model.has_section);
    assert_true(model.has_chapter);
    assert_true(model.has_heading);
    assert_string_equal(model.section.id, "15");
    assert_string_equal(model.section.numeral, "XV");
    assert_string_equal(model.chapter.goods_nomenclature_sid, "44810");
    assert_string_equal(model.chapter.chapter_note, "Chapter note");
    assert_string_equal(model.heading.goods_nomenclature_sid, "45432");
    assert_null(model.heading.validity_end_date);

    classification_model_free(&model);
    for (size_t i = 0; i < 16; i++) {
        test_free(values[i]);
    }
}

static void infers_absent_classification_resources_from_null_ids(void **state)
{
    (void)state;

    char *values[16] = { 0 };
    unsigned char nulls[16] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1,
    };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 16,
        .error = NULL,
        .ok = 1,
    };

    ClassificationModel model = { 0 };
    assert_true(classification_repository_hydrate_row(&rows, 0, &model));
    assert_false(model.has_section);
    assert_false(model.has_chapter);
    assert_false(model.has_heading);

    classification_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(hydrates_classification_row),
        cmocka_unit_test(infers_absent_classification_resources_from_null_ids),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
