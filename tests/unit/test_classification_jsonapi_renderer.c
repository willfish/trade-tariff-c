#include "renderers/classification_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static const char *string_at(yyjson_val *root, const char *key)
{
    yyjson_val *value = yyjson_obj_get(root, key);
    return yyjson_is_str(value) ? yyjson_get_str(value) : NULL;
}

static yyjson_val *obj_path(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = yyjson_obj_get(root, first);
    return second == NULL || value == NULL ? value : yyjson_obj_get(value, second);
}

static void renders_classification_resources(void **state)
{
    (void)state;

    ClassificationSectionModel section = {
        .id = "15",
        .numeral = "XV",
        .title = "Base metals and articles of base metal",
        .position = "15",
        .section_note = "Section note",
    };
    ClassificationChapterModel chapter = {
        .goods_nomenclature_sid = "44810",
        .goods_nomenclature_item_id = "7200000000",
        .description = "IRON AND STEEL",
        .validity_start_date = "1971-12-31T00:00:00.000Z",
        .chapter_note = "Chapter note",
    };
    ClassificationHeadingModel heading = {
        .goods_nomenclature_sid = "45432",
        .goods_nomenclature_item_id = "7212000000",
        .description = "Flat-rolled products",
        .validity_start_date = "1972-01-01T00:00:00.000Z",
    };
    GuideModel guides_items[] = {
        {
            .id = "12",
            .title = "Using a commodity code",
            .url = "https://www.gov.uk/guidance/using-a-commodity-code",
        },
    };
    GuideModelList guides = {
        .items = guides_items,
        .len = 1,
        .cap = 1,
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *root = yyjson_mut_arr(mut_doc);
    assert_non_null(root);
    assert_true(yyjson_mut_arr_append(root, classification_section_jsonapi_resource(mut_doc, &section)));
    assert_true(yyjson_mut_arr_append(root, classification_chapter_jsonapi_resource(mut_doc, &chapter, &guides, "uk")));
    assert_true(yyjson_mut_arr_append(root, classification_heading_jsonapi_resource(mut_doc, &heading, "uk")));
    yyjson_mut_doc_set_root(mut_doc, root);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *resources = yyjson_doc_get_root(doc);
    yyjson_val *section_resource = yyjson_arr_get(resources, 0);
    yyjson_val *chapter_resource = yyjson_arr_get(resources, 1);
    yyjson_val *heading_resource = yyjson_arr_get(resources, 2);
    assert_string_equal(string_at(section_resource, "type"), "section");
    assert_string_equal(string_at(yyjson_obj_get(section_resource, "attributes"), "section_note"), "Section note");
    assert_string_equal(string_at(chapter_resource, "type"), "chapter");
    assert_string_equal(string_at(yyjson_obj_get(chapter_resource, "attributes"), "formatted_description"), "Iron and steel");
    yyjson_val *guide_refs = obj_path(yyjson_obj_get(yyjson_obj_get(chapter_resource, "relationships"), "guides"), "data", NULL);
    assert_int_equal(yyjson_arr_size(guide_refs), 1);
    assert_string_equal(string_at(yyjson_arr_get(guide_refs, 0), "type"), "guide");
    assert_string_equal(string_at(yyjson_arr_get(guide_refs, 0), "id"), "12");
    assert_string_equal(string_at(heading_resource, "type"), "heading");
    assert_string_equal(string_at(yyjson_obj_get(heading_resource, "attributes"), "description_plain"), "Flat-rolled products");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_classification_resources),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
