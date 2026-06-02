#include "renderers/geographical_area_jsonapi_renderer.h"

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

static GeographicalAreaModel area(const char *sid,
                                  const char *area_id,
                                  const char *parent_sid)
{
    return (GeographicalAreaModel){
        .geographical_area_sid = copy_literal(sid),
        .geographical_area_id = copy_literal(area_id),
        .description = copy_literal("Area description"),
        .parent_geographical_area_sid = parent_sid == NULL ? NULL : copy_literal(parent_sid),
    };
}

static MeasureExcludedGeographicalAreaModel excluded_area(const char *measure_sid,
                                                          const char *area_id)
{
    return (MeasureExcludedGeographicalAreaModel){
        .measure_sid = copy_literal(measure_sid),
        .geographical_area_sid = copy_literal("456"),
        .geographical_area_id = copy_literal(area_id),
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

static void renders_area_resource_with_child_relationships(void **state)
{
    (void)state;

    GeographicalAreaModelList areas = { 0 };
    assert_true(geographical_area_model_list_append(&areas, area("100", "1011", NULL)));
    assert_true(geographical_area_model_list_append(&areas, area("200", "AD", "100")));
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    yyjson_mut_val *resource = geographical_area_jsonapi_resource(mut_doc, &areas.items[0], &areas);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    yyjson_val *children = obj_path(yyjson_obj_get(yyjson_obj_get(root, "relationships"), "children_geographical_areas"), "data", NULL);

    assert_string_equal(string_at(root, "id", NULL), "1011");
    assert_string_equal(string_at(root, "type", NULL), "geographical_area");
    assert_string_equal(string_at(attributes, "geographical_area_sid", NULL), "100");
    assert_int_equal(yyjson_arr_size(children), 1);
    assert_string_equal(string_at(yyjson_arr_get(children, 0), "id", NULL), "AD");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    geographical_area_model_list_free(&areas);
}

static void renders_excluded_area_relationship_refs_for_one_measure(void **state)
{
    (void)state;

    MeasureExcludedGeographicalAreaModelList excluded = { 0 };
    assert_true(measure_excluded_geographical_area_model_list_append(&excluded, excluded_area("123", "AD")));
    assert_true(measure_excluded_geographical_area_model_list_append(&excluded, excluded_area("456", "FR")));
    assert_true(measure_excluded_geographical_area_model_list_append(&excluded, excluded_area("123", "DE")));
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    yyjson_mut_val *data = geographical_area_jsonapi_excluded_relationship_data_for_measure(mut_doc, &excluded, "123");
    assert_non_null(data);
    yyjson_mut_doc_set_root(mut_doc, data);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    assert_int_equal(yyjson_arr_size(root), 2);
    assert_string_equal(string_at(yyjson_arr_get(root, 0), "id", NULL), "AD");
    assert_string_equal(string_at(yyjson_arr_get(root, 1), "id", NULL), "DE");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_excluded_geographical_area_model_list_free(&excluded);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_area_resource_with_child_relationships),
        cmocka_unit_test(renders_excluded_area_relationship_refs_for_one_measure),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
