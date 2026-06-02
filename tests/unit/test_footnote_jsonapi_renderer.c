#include "renderers/footnote_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static Footnote footnote(const char *measure_sid,
                         const char *code)
{
    return (Footnote){
        .measure_sid = copy_literal(measure_sid),
        .id = copy_literal(code),
        .code = copy_literal(code),
        .description = copy_literal("Footnote text"),
    };
}

static yyjson_val *obj_path(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = yyjson_obj_get(root, first);
    return second == NULL || value == NULL ? value : yyjson_obj_get(value, second);
}

static const char *string_at(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = obj_path(root, first, second);
    return yyjson_is_str(value) ? yyjson_get_str(value) : NULL;
}

static void renders_footnote_resource_from_typed_model(void **state)
{
    (void)state;

    Footnote model = footnote("123", "CD123");
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    yyjson_mut_val *resource = footnote_jsonapi_resource(mut_doc, &model, "uk");
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id", NULL), "CD123");
    assert_string_equal(string_at(root, "type", NULL), "footnote");
    assert_string_equal(string_at(attributes, "code", NULL), "CD123");
    assert_string_equal(string_at(attributes, "description", NULL), "Footnote text");
    assert_string_equal(string_at(attributes, "formatted_description", NULL), "Footnote text");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    footnote_free(&model);
}

static void renders_relationship_refs_for_one_measure(void **state)
{
    (void)state;

    FootnoteList footnotes = { 0 };
    assert_true(footnote_list_append(&footnotes, footnote("123", "CD123")));
    assert_true(footnote_list_append(&footnotes, footnote("456", "CD999")));
    assert_true(footnote_list_append(&footnotes, footnote("123", "CD124")));
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    yyjson_mut_val *data = footnote_jsonapi_relationship_data_for_measure(mut_doc, &footnotes, "123");
    assert_non_null(data);
    yyjson_mut_doc_set_root(mut_doc, data);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    assert_int_equal(yyjson_arr_size(root), 2);
    assert_string_equal(string_at(yyjson_arr_get(root, 0), "id", NULL), "CD123");
    assert_string_equal(string_at(yyjson_arr_get(root, 1), "id", NULL), "CD124");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    footnote_list_free(&footnotes);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_footnote_resource_from_typed_model),
        cmocka_unit_test(renders_relationship_refs_for_one_measure),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
