#include "repositories/goods_nomenclature_tree_repository.h"

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

static void hydrates_tree_node_row(void **state)
{
    (void)state;

    char *values[11] = {
        copy_literal("123456"),
        copy_literal("8543400000"),
        copy_literal("80"),
        copy_literal("3"),
        copy_literal("854340000080"),
        copy_literal("5"),
        copy_literal("2024-01-01T00:00:00.000Z"),
        NULL,
        copy_literal("Electronic cigarettes"),
        copy_literal("1"),
        copy_literal("0"),
    };
    unsigned char nulls[11] = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 11,
        .error = NULL,
        .ok = 1,
    };

    GoodsNomenclatureTreeNode node = { 0 };
    assert_true(goods_nomenclature_tree_repository_hydrate_row(&rows, 0, &node));
    assert_string_equal(node.goods_nomenclature_sid, "123456");
    assert_string_equal(node.goods_nomenclature_item_id, "8543400000");
    assert_string_equal(node.producline_suffix, "80");
    assert_string_equal(node.number_indents, "3");
    assert_string_equal(node.position, "854340000080");
    assert_string_equal(node.depth, "5");
    assert_string_equal(node.validity_start_date, "2024-01-01T00:00:00.000Z");
    assert_null(node.validity_end_date);
    assert_string_equal(node.description, "Electronic cigarettes");
    assert_int_equal(node.leaf, 1);
    assert_int_equal(node.has_chemicals, 0);

    goods_nomenclature_tree_node_free(&node);
    for (size_t i = 0; i < 11; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(hydrates_tree_node_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
