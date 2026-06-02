#include "presenters/measure_condition_permutation_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static MeasureConditionModel condition(const char *sid,
                                       const char *measure_sid,
                                       const char *condition_code,
                                       const char *action_code)
{
    return (MeasureConditionModel){
        .measure_condition_sid = copy_literal(sid),
        .measure_sid = copy_literal(measure_sid),
        .condition_code = copy_literal(condition_code),
        .action_code = action_code == NULL ? NULL : copy_literal(action_code),
    };
}

static void append_condition(MeasureConditionModelList *list, MeasureConditionModel model)
{
    assert_true(measure_condition_model_list_append(list, model));
}

static void builds_unmatched_groups_by_condition_code(void **state)
{
    (void)state;

    MeasureConditionModelList conditions = { 0 };
    MeasureConditionPermutationGroupModelList groups = { 0 };
    append_condition(&conditions, condition("1", "100", "A", "01"));
    append_condition(&conditions, condition("2", "100", "B", "01"));

    assert_true(measure_condition_permutation_presenter_build_groups(&conditions, &groups));

    assert_int_equal(groups.len, 2);
    assert_string_equal(groups.items[0].id, "100-A");
    assert_string_equal(groups.items[0].condition_code, "A");
    assert_int_equal(groups.items[0].permutations.len, 1);
    assert_string_equal(groups.items[0].permutations.items[0].id, "c4ca4238a0b923820dcc509a6f75849b");
    assert_string_equal(groups.items[0].permutations.items[0].measure_condition_sids[0], "1");
    assert_string_equal(groups.items[1].id, "100-B");
    assert_string_equal(groups.items[1].condition_code, "B");
    assert_string_equal(groups.items[1].permutations.items[0].id, "c81e728d9d4c2f636f067f89cc14862c");

    measure_condition_permutation_group_model_list_free(&groups);
    measure_condition_model_list_free(&conditions);
}

static void presents_identity_and_resource_types(void **state)
{
    (void)state;

    MeasureConditionPermutationModel permutation = { .id = "abc" };
    MeasureConditionPermutationGroupModel group = { .id = "100-A" };

    assert_string_equal(measure_condition_permutation_presenter_resource_type(),
                        "measure_condition_permutation");
    assert_string_equal(measure_condition_permutation_group_presenter_resource_type(),
                        "measure_condition_permutation_group");
    assert_string_equal(measure_condition_permutation_presenter_id(&permutation), "abc");
    assert_string_equal(measure_condition_permutation_group_presenter_id(&group), "100-A");
    assert_null(measure_condition_permutation_presenter_id(NULL));
    assert_null(measure_condition_permutation_group_presenter_id(NULL));
}

static void builds_single_matched_group_with_cartesian_unmatched_tail(void **state)
{
    (void)state;

    MeasureConditionModelList conditions = { 0 };
    MeasureConditionPermutationGroupModelList groups = { 0 };
    MeasureConditionModel matched_a = condition("1", "100", "B", "01");
    MeasureConditionModel matched_b = condition("2", "100", "C", "01");
    matched_a.certificate_type_code = copy_literal("N");
    matched_a.certificate_code = copy_literal("990");
    matched_b.certificate_type_code = copy_literal("N");
    matched_b.certificate_code = copy_literal("990");
    append_condition(&conditions, matched_a);
    append_condition(&conditions, matched_b);
    append_condition(&conditions, condition("3", "100", "A", "01"));
    append_condition(&conditions, condition("4", "100", "D", "01"));

    assert_true(measure_condition_permutation_presenter_build_groups(&conditions, &groups));

    assert_int_equal(groups.len, 1);
    assert_string_equal(groups.items[0].id, "100-n/a");
    assert_string_equal(groups.items[0].condition_code, "n/a");
    assert_int_equal(groups.items[0].permutations.len, 2);
    assert_string_equal(groups.items[0].permutations.items[0].id, "c4ca4238a0b923820dcc509a6f75849b");
    assert_int_equal(groups.items[0].permutations.items[0].measure_condition_sid_count, 1);
    assert_string_equal(groups.items[0].permutations.items[0].measure_condition_sids[0], "1");
    assert_string_equal(groups.items[0].permutations.items[1].id, "3ef5a9683a8bc9c90bdf7bf356d0b0d1");
    assert_int_equal(groups.items[0].permutations.items[1].measure_condition_sid_count, 2);
    assert_string_equal(groups.items[0].permutations.items[1].measure_condition_sids[0], "3");
    assert_string_equal(groups.items[0].permutations.items[1].measure_condition_sids[1], "4");

    measure_condition_permutation_group_model_list_free(&groups);
    measure_condition_model_list_free(&conditions);
}

static void follows_rails_exclusion_rules(void **state)
{
    (void)state;

    MeasureConditionModelList conditions = { 0 };
    MeasureConditionPermutationGroupModelList groups = { 0 };
    MeasureConditionModel universal_waiver = condition("1", "100", "A", "01");
    MeasureConditionModel negative_without_document_or_threshold = condition("2", "100", "B", "04");
    MeasureConditionModel negative_action_08 = condition("3", "100", "C", "08");
    MeasureConditionModel unknown_action_without_document = condition("4", "100", "D", NULL);
    universal_waiver.certificate_type_code = copy_literal("999");
    universal_waiver.certificate_code = copy_literal("L");
    append_condition(&conditions, universal_waiver);
    append_condition(&conditions, negative_without_document_or_threshold);
    append_condition(&conditions, negative_action_08);
    append_condition(&conditions, unknown_action_without_document);

    assert_true(measure_condition_permutation_presenter_build_groups(&conditions, &groups));

    assert_int_equal(groups.len, 1);
    assert_string_equal(groups.items[0].id, "100-C");
    assert_string_equal(groups.items[0].permutations.items[0].measure_condition_sids[0], "3");

    measure_condition_permutation_group_model_list_free(&groups);
    measure_condition_model_list_free(&conditions);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_unmatched_groups_by_condition_code),
        cmocka_unit_test(presents_identity_and_resource_types),
        cmocka_unit_test(builds_single_matched_group_with_cartesian_unmatched_tail),
        cmocka_unit_test(follows_rails_exclusion_rules),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
