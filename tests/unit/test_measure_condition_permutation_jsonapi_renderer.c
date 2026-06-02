#include "renderers/measure_condition_permutation_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

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

static yyjson_doc *render_value(yyjson_mut_doc *mut_doc, yyjson_mut_val *value, char **json_out)
{
    yyjson_mut_doc_set_root(mut_doc, value);
    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);
    *json_out = json;
    return doc;
}

static void renders_group_resource_with_permutation_relationship(void **state)
{
    (void)state;

    char *condition_sids[] = { "789" };
    MeasureConditionPermutationModel permutation = {
        .id = "68053af2923e00204c3ca7c6a3150cf7",
        .measure_condition_sids = condition_sids,
        .measure_condition_sid_count = 1,
    };
    MeasureConditionPermutationGroupModel group = {
        .id = "123-A",
        .measure_sid = "123",
        .condition_code = "A",
        .permutations = {
            .items = &permutation,
            .len = 1,
        },
    };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    char *json = NULL;
    yyjson_doc *doc = render_value(mut_doc,
                                   measure_condition_permutation_group_jsonapi_resource(mut_doc, &group),
                                   &json);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *permutations = obj_path(yyjson_obj_get(relationships, "permutations"), "data", NULL);

    assert_string_equal(string_at(root, "id", NULL), "123-A");
    assert_string_equal(string_at(root, "type", NULL), "measure_condition_permutation_group");
    assert_string_equal(string_at(attributes, "condition_code", NULL), "A");
    assert_int_equal(yyjson_arr_size(permutations), 1);
    assert_string_equal(string_at(yyjson_arr_get(permutations, 0), "type", NULL), "measure_condition_permutation");
    assert_string_equal(string_at(yyjson_arr_get(permutations, 0), "id", NULL), "68053af2923e00204c3ca7c6a3150cf7");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

static void renders_permutation_resource_with_measure_condition_relationship(void **state)
{
    (void)state;

    char *condition_sids[] = { "789", "790" };
    MeasureConditionPermutationModel permutation = {
        .id = "hash",
        .measure_condition_sids = condition_sids,
        .measure_condition_sid_count = 2,
    };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    char *json = NULL;
    yyjson_doc *doc = render_value(mut_doc,
                                   measure_condition_permutation_jsonapi_resource(mut_doc, &permutation),
                                   &json);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *conditions = obj_path(yyjson_obj_get(relationships, "measure_conditions"), "data", NULL);

    assert_string_equal(string_at(root, "id", NULL), "hash");
    assert_string_equal(string_at(root, "type", NULL), "measure_condition_permutation");
    assert_int_equal(yyjson_arr_size(conditions), 2);
    assert_string_equal(string_at(yyjson_arr_get(conditions, 0), "type", NULL), "measure_condition");
    assert_string_equal(string_at(yyjson_arr_get(conditions, 0), "id", NULL), "789");
    assert_string_equal(string_at(yyjson_arr_get(conditions, 1), "id", NULL), "790");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

static void renders_measure_relationship_data_for_matching_groups_only(void **state)
{
    (void)state;

    MeasureConditionPermutationGroupModel groups[] = {
        { .id = "123-A", .measure_sid = "123", .condition_code = "A" },
        { .id = "456-A", .measure_sid = "456", .condition_code = "A" },
    };
    MeasureConditionPermutationGroupModelList list = {
        .items = groups,
        .len = 2,
    };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    char *json = NULL;
    yyjson_doc *doc = render_value(mut_doc,
                                   measure_condition_permutation_group_jsonapi_relationship_data_for_measure(mut_doc,
                                                                                                             &list,
                                                                                                             "123"),
                                   &json);
    yyjson_val *root = yyjson_doc_get_root(doc);

    assert_int_equal(yyjson_arr_size(root), 1);
    assert_string_equal(string_at(yyjson_arr_get(root, 0), "id", NULL), "123-A");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_group_resource_with_permutation_relationship),
        cmocka_unit_test(renders_permutation_resource_with_measure_condition_relationship),
        cmocka_unit_test(renders_measure_relationship_data_for_matching_groups_only),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
