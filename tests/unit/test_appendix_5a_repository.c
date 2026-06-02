#include "repositories/appendix_5a_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void builds_postgres_text_array_for_document_codes(void **state)
{
    (void)state;

    const char *document_codes[] = { "C990", NULL, "", "Y123" };
    char *param = appendix_5a_repository_document_code_array_param(document_codes, sizeof(document_codes) / sizeof(document_codes[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"C990\",\"Y123\"}");

    free(param);
}

static void hydrates_appendix_5a_row(void **state)
{
    (void)state;

    char *values[] = {
        "C",
        "990",
        "CDS guidance text",
    };
    unsigned char nulls[] = { 0, 0, 0 };
    DbRows rows = {
        .ok = 1,
        .rows = 1,
        .cols = 3,
        .values = values,
        .nulls = nulls,
    };
    Appendix5aModel model = { 0 };

    assert_true(appendix_5a_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.certificate_type_code, "C");
    assert_string_equal(model.certificate_code, "990");
    assert_string_equal(model.cds_guidance, "CDS guidance text");

    appendix_5a_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_for_document_codes),
        cmocka_unit_test(hydrates_appendix_5a_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
