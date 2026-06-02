#include "presenters/measure_condition_permutation_presenter.h"

#include "presenters/measure_condition_presenter.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const MeasureConditionModel **items;
    size_t len;
    size_t cap;
} ConditionRefList;

typedef struct {
    char *measure_sid;
    ConditionRefList conditions;
} MeasureConditionBucket;

typedef struct {
    MeasureConditionBucket *items;
    size_t len;
    size_t cap;
} MeasureConditionBucketList;

typedef struct {
    char *condition_code;
    ConditionRefList conditions;
} ConditionCodeBucket;

typedef struct {
    ConditionCodeBucket *items;
    size_t len;
    size_t cap;
} ConditionCodeBucketList;

static int blank(const char *value)
{
    return value == NULL || *value == '\0';
}

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

const char *measure_condition_permutation_presenter_resource_type(void)
{
    return "measure_condition_permutation";
}

const char *measure_condition_permutation_presenter_id(const MeasureConditionPermutationModel *model)
{
    return model == NULL ? NULL : model->id;
}

const char *measure_condition_permutation_group_presenter_resource_type(void)
{
    return "measure_condition_permutation_group";
}

const char *measure_condition_permutation_group_presenter_id(const MeasureConditionPermutationGroupModel *model)
{
    return model == NULL ? NULL : model->id;
}

static char *copy_literal(const char *value)
{
    if (value == NULL) {
        return NULL;
    }
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

static int positive_action(const char *action_code)
{
    static const char *codes[] = { "01", "07", "24", "25", "26", "27", "28", "29", "34", "36" };
    for (size_t i = 0; i < sizeof(codes) / sizeof(codes[0]); i++) {
        if (streq(action_code, codes[i])) {
            return 1;
        }
    }
    return 0;
}

static char *document_code(const MeasureConditionModel *condition)
{
    return measure_condition_presenter_document_code(condition);
}

static int condition_excluded(const MeasureConditionModel *condition)
{
    if (condition == NULL) {
        return 1;
    }

    char *doc_code = document_code(condition);
    int excluded = 0;
    if (streq(doc_code, "999L")) {
        excluded = 1;
    } else if (positive_action(condition->action_code)) {
        excluded = 0;
    } else if (!blank(condition->condition_duty_amount)) {
        excluded = 0;
    } else if (streq(condition->action_code, "08")) {
        excluded = 0;
    } else if (doc_code == NULL) {
        excluded = 1;
    }
    free(doc_code);
    return excluded;
}

static char *permutation_key(const MeasureConditionModel *condition)
{
    if (condition == NULL) {
        return NULL;
    }
    if (!blank(condition->certificate_type_code) ||
        !blank(condition->certificate_code) ||
        !blank(condition->condition_duty_amount)) {
        const char *type = condition->certificate_type_code == NULL ? "" : condition->certificate_type_code;
        const char *code = condition->certificate_code == NULL ? "" : condition->certificate_code;
        const char *amount = condition->condition_duty_amount == NULL ? "" : condition->condition_duty_amount;
        size_t len = strlen(type) + strlen(code) + strlen(amount) + 3;
        char *key = malloc(len);
        if (key == NULL) {
            return NULL;
        }
        snprintf(key, len, "%s-%s-%s", type, code, amount);
        return key;
    }
    return copy_literal(condition->measure_condition_sid);
}

static uint32_t leftrotate(uint32_t value, uint32_t bits)
{
    return (value << bits) | (value >> (32 - bits));
}

static char *md5_hex(const char *input)
{
    static const uint32_t shifts[] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
    };
    static const uint32_t constants[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
    };
    size_t input_len = input == NULL ? 0 : strlen(input);
    uint64_t bit_len = (uint64_t)input_len * 8;
    size_t padded_len = input_len + 1;
    while (padded_len % 64 != 56) {
        padded_len++;
    }
    unsigned char *msg = calloc(padded_len + 8, 1);
    if (msg == NULL) {
        return NULL;
    }
    if (input_len > 0) {
        memcpy(msg, input, input_len);
    }
    msg[input_len] = 0x80;
    for (size_t i = 0; i < 8; i++) {
        msg[padded_len + i] = (unsigned char)((bit_len >> (8 * i)) & 0xff);
    }
    uint32_t a0 = 0x67452301, b0 = 0xefcdab89, c0 = 0x98badcfe, d0 = 0x10325476;
    for (size_t offset = 0; offset < padded_len + 8; offset += 64) {
        uint32_t words[16];
        for (size_t i = 0; i < 16; i++) {
            words[i] = (uint32_t)msg[offset + i * 4] |
                       ((uint32_t)msg[offset + i * 4 + 1] << 8) |
                       ((uint32_t)msg[offset + i * 4 + 2] << 16) |
                       ((uint32_t)msg[offset + i * 4 + 3] << 24);
        }
        uint32_t a = a0, b = b0, c = c0, d = d0;
        for (uint32_t i = 0; i < 64; i++) {
            uint32_t f, g;
            if (i < 16) {
                f = (b & c) | (~b & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | (~d & c);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            } else {
                f = c ^ (b | ~d);
                g = (7 * i) % 16;
            }
            uint32_t temp = d;
            d = c;
            c = b;
            b = b + leftrotate(a + f + constants[i] + words[g], shifts[i]);
            a = temp;
        }
        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    }
    free(msg);
    char *hex = malloc(33);
    if (hex == NULL) {
        return NULL;
    }
    snprintf(hex, 33,
             "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             a0 & 0xff, (a0 >> 8) & 0xff, (a0 >> 16) & 0xff, (a0 >> 24) & 0xff,
             b0 & 0xff, (b0 >> 8) & 0xff, (b0 >> 16) & 0xff, (b0 >> 24) & 0xff,
             c0 & 0xff, (c0 >> 8) & 0xff, (c0 >> 16) & 0xff, (c0 >> 24) & 0xff,
             d0 & 0xff, (d0 >> 8) & 0xff, (d0 >> 16) & 0xff, (d0 >> 24) & 0xff);
    return hex;
}

static int condition_ref_list_append(ConditionRefList *list, const MeasureConditionModel *condition)
{
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        const MeasureConditionModel **next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }
    list->items[list->len++] = condition;
    return 1;
}

static void condition_ref_list_free(ConditionRefList *list)
{
    if (list == NULL) {
        return;
    }
    free(list->items);
    *list = (ConditionRefList){ 0 };
}

static void condition_code_bucket_list_free(ConditionCodeBucketList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        free(list->items[i].condition_code);
        condition_ref_list_free(&list->items[i].conditions);
    }
    free(list->items);
    *list = (ConditionCodeBucketList){ 0 };
}

static ConditionCodeBucket *bucket_for_condition_code(ConditionCodeBucketList *list, const char *condition_code)
{
    if (list == NULL || condition_code == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < list->len; i++) {
        if (streq(list->items[i].condition_code, condition_code)) {
            return &list->items[i];
        }
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        ConditionCodeBucket *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            return NULL;
        }
        list->items = next;
        list->cap = next_cap;
    }
    ConditionCodeBucket *bucket = &list->items[list->len++];
    *bucket = (ConditionCodeBucket){ .condition_code = copy_literal(condition_code) };
    if (bucket->condition_code == NULL) {
        list->len--;
        return NULL;
    }
    return bucket;
}

static void measure_condition_bucket_list_free(MeasureConditionBucketList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        free(list->items[i].measure_sid);
        condition_ref_list_free(&list->items[i].conditions);
    }
    free(list->items);
    *list = (MeasureConditionBucketList){ 0 };
}

static MeasureConditionBucket *bucket_for_measure(MeasureConditionBucketList *list, const char *measure_sid)
{
    if (list == NULL || measure_sid == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < list->len; i++) {
        if (streq(list->items[i].measure_sid, measure_sid)) {
            return &list->items[i];
        }
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        MeasureConditionBucket *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            return NULL;
        }
        list->items = next;
        list->cap = next_cap;
    }
    MeasureConditionBucket *bucket = &list->items[list->len++];
    *bucket = (MeasureConditionBucket){ .measure_sid = copy_literal(measure_sid) };
    if (bucket->measure_sid == NULL) {
        list->len--;
        return NULL;
    }
    return bucket;
}

static char *permutation_id(const ConditionRefList *conditions)
{
    size_t len = 1;
    for (size_t i = 0; i < conditions->len; i++) {
        len += strlen(conditions->items[i]->measure_condition_sid);
        if (i + 1 < conditions->len) {
            len++;
        }
    }
    char *content = malloc(len);
    if (content == NULL) {
        return NULL;
    }
    content[0] = '\0';
    for (size_t i = 0; i < conditions->len; i++) {
        if (i > 0) {
            strcat(content, "\n");
        }
        strcat(content, conditions->items[i]->measure_condition_sid);
    }
    char *id = md5_hex(content);
    free(content);
    return id;
}

static int append_permutation(MeasureConditionPermutationModelList *list, const ConditionRefList *conditions)
{
    MeasureConditionPermutationModel model = { .id = permutation_id(conditions) };
    if (model.id == NULL) {
        return 0;
    }
    for (size_t i = 0; i < conditions->len; i++) {
        if (!measure_condition_permutation_model_add_condition_sid(&model, conditions->items[i]->measure_condition_sid)) {
            measure_condition_permutation_model_free(&model);
            return 0;
        }
    }
    return measure_condition_permutation_model_list_append(list, model);
}

static int condition_class_priority(const MeasureConditionModel *condition)
{
    const char *klass = measure_condition_presenter_measure_condition_class(condition);
    if (streq(klass, "document")) {
        return 1;
    }
    if (streq(klass, "exemption")) {
        return 2;
    }
    if (streq(klass, "threshold")) {
        return 3;
    }
    return 4;
}

static int compare_condition_priority(const void *left, const void *right)
{
    const MeasureConditionModel *a = *(const MeasureConditionModel *const *)left;
    const MeasureConditionModel *b = *(const MeasureConditionModel *const *)right;
    int pa = condition_class_priority(a);
    int pb = condition_class_priority(b);
    if (pa != pb) {
        return pa - pb;
    }
    return strcmp(a->measure_condition_sid, b->measure_condition_sid);
}

static char *group_id(const char *measure_sid, const char *condition_code)
{
    size_t len = strlen(measure_sid) + strlen(condition_code) + 2;
    char *id = malloc(len);
    if (id == NULL) {
        return NULL;
    }
    snprintf(id, len, "%s-%s", measure_sid, condition_code);
    return id;
}

static int append_group(MeasureConditionPermutationGroupModelList *out,
                        const char *measure_sid,
                        const char *condition_code,
                        MeasureConditionPermutationModelList permutations)
{
    MeasureConditionPermutationGroupModel group = {
        .id = group_id(measure_sid, condition_code),
        .measure_sid = copy_literal(measure_sid),
        .condition_code = copy_literal(condition_code),
        .permutations = permutations,
    };
    if (group.id == NULL || group.measure_sid == NULL || group.condition_code == NULL) {
        measure_condition_permutation_group_model_free(&group);
        return 0;
    }
    return measure_condition_permutation_group_model_list_append(out, group);
}

static int append_singleton_permutation(MeasureConditionPermutationModelList *permutations,
                                        const MeasureConditionModel *condition)
{
    ConditionRefList singleton = { 0 };
    int ok = condition_ref_list_append(&singleton, condition) &&
             append_permutation(permutations, &singleton);
    condition_ref_list_free(&singleton);
    return ok;
}

static int condition_key_matching_count(const MeasureConditionBucket *bucket,
                                        const MeasureConditionModel *condition)
{
    char *key = permutation_key(condition);
    if (key == NULL) {
        return 0;
    }
    size_t matching = 0;
    for (size_t i = 0; i < bucket->conditions.len; i++) {
        char *other_key = permutation_key(bucket->conditions.items[i]);
        if (streq(key, other_key)) {
            matching++;
        }
        free(other_key);
    }
    free(key);
    return (int)matching;
}

static int compare_condition_code_bucket(const void *left, const void *right)
{
    const ConditionCodeBucket *a = left;
    const ConditionCodeBucket *b = right;
    return strcmp(a->condition_code, b->condition_code);
}

static int append_cartesian_permutations(const ConditionCodeBucketList *groups,
                                         size_t group_index,
                                         ConditionRefList *current,
                                         MeasureConditionPermutationModelList *permutations)
{
    if (group_index == groups->len) {
        return append_permutation(permutations, current);
    }

    const ConditionCodeBucket *group = &groups->items[group_index];
    for (size_t i = 0; i < group->conditions.len; i++) {
        if (!condition_ref_list_append(current, group->conditions.items[i])) {
            return 0;
        }
        if (!append_cartesian_permutations(groups, group_index + 1, current, permutations)) {
            current->len--;
            return 0;
        }
        current->len--;
    }
    return 1;
}

static int append_unmatched_cartesian_permutations(const MeasureConditionBucket *bucket,
                                                   MeasureConditionPermutationModelList *permutations)
{
    ConditionCodeBucketList groups = { 0 };
    for (size_t i = 0; i < bucket->conditions.len; i++) {
        const MeasureConditionModel *condition = bucket->conditions.items[i];
        if (condition_key_matching_count(bucket, condition) > 1) {
            continue;
        }
        ConditionCodeBucket *group = bucket_for_condition_code(&groups, condition->condition_code);
        if (group == NULL || !condition_ref_list_append(&group->conditions, condition)) {
            condition_code_bucket_list_free(&groups);
            return 0;
        }
    }
    if (groups.len > 1) {
        qsort(groups.items, groups.len, sizeof(*groups.items), compare_condition_code_bucket);
    }

    ConditionRefList current = { 0 };
    int ok = groups.len == 0 ||
             append_cartesian_permutations(&groups, 0, &current, permutations);
    condition_ref_list_free(&current);
    condition_code_bucket_list_free(&groups);
    return ok;
}

static int build_unmatched_groups(const MeasureConditionBucket *bucket,
                                  MeasureConditionPermutationGroupModelList *out)
{
    for (size_t i = 0; i < bucket->conditions.len; i++) {
        const char *condition_code = bucket->conditions.items[i]->condition_code;
        int already_grouped = 0;
        for (size_t j = 0; j < i; j++) {
            if (streq(bucket->conditions.items[j]->condition_code, condition_code)) {
                already_grouped = 1;
                break;
            }
        }
        if (already_grouped) {
            continue;
        }

        MeasureConditionPermutationModelList permutations = { 0 };
        for (size_t j = i; j < bucket->conditions.len; j++) {
            if (streq(bucket->conditions.items[j]->condition_code, condition_code) &&
                !append_singleton_permutation(&permutations, bucket->conditions.items[j])) {
                measure_condition_permutation_model_list_free(&permutations);
                return 0;
            }
        }
        if (!append_group(out, bucket->measure_sid, condition_code, permutations)) {
            measure_condition_permutation_model_list_free(&permutations);
            return 0;
        }
    }
    return 1;
}

static int build_matched_group(const MeasureConditionBucket *bucket,
                               MeasureConditionPermutationGroupModelList *out)
{
    MeasureConditionPermutationModelList permutations = { 0 };
    ConditionRefList matched_conditions = { 0 };

    for (size_t i = 0; i < bucket->conditions.len; i++) {
        char *key = permutation_key(bucket->conditions.items[i]);
        size_t matching = 0;
        int first_for_key = 1;
        for (size_t j = 0; j < bucket->conditions.len; j++) {
            char *other_key = permutation_key(bucket->conditions.items[j]);
            if (streq(key, other_key)) {
                matching++;
                if (j < i) {
                    first_for_key = 0;
                }
            }
            free(other_key);
        }
        free(key);
        if (matching > 1 && first_for_key) {
            if (!condition_ref_list_append(&matched_conditions, bucket->conditions.items[i])) {
                condition_ref_list_free(&matched_conditions);
                measure_condition_permutation_model_list_free(&permutations);
                return 0;
            }
        }
    }

    if (matched_conditions.len > 1) {
        qsort(matched_conditions.items, matched_conditions.len, sizeof(*matched_conditions.items), compare_condition_priority);
    }
    for (size_t i = 0; i < matched_conditions.len; i++) {
        if (!append_singleton_permutation(&permutations, matched_conditions.items[i])) {
            condition_ref_list_free(&matched_conditions);
            measure_condition_permutation_model_list_free(&permutations);
            return 0;
        }
    }
    condition_ref_list_free(&matched_conditions);

    if (!append_unmatched_cartesian_permutations(bucket, &permutations)) {
        measure_condition_permutation_model_list_free(&permutations);
        return 0;
    }

    if (!append_group(out, bucket->measure_sid, "n/a", permutations)) {
        measure_condition_permutation_model_list_free(&permutations);
        return 0;
    }
    return 1;
}

static int bucket_has_matched_conditions(const MeasureConditionBucket *bucket)
{
    for (size_t i = 0; i < bucket->conditions.len; i++) {
        char *key = permutation_key(bucket->conditions.items[i]);
        size_t matching = 0;
        for (size_t j = 0; j < bucket->conditions.len; j++) {
            char *other_key = permutation_key(bucket->conditions.items[j]);
            if (streq(key, other_key)) {
                matching++;
            }
            free(other_key);
        }
        free(key);
        if (matching > 1) {
            return 1;
        }
    }
    return 0;
}

int measure_condition_permutation_presenter_build_groups(const MeasureConditionModelList *conditions,
                                                         MeasureConditionPermutationGroupModelList *out)
{
    if (conditions == NULL || out == NULL) {
        return 0;
    }
    *out = (MeasureConditionPermutationGroupModelList){ 0 };

    MeasureConditionBucketList buckets = { 0 };
    for (size_t i = 0; i < conditions->len; i++) {
        const MeasureConditionModel *condition = &conditions->items[i];
        if (condition_excluded(condition) || condition->measure_sid == NULL || condition->measure_condition_sid == NULL) {
            continue;
        }
        MeasureConditionBucket *bucket = bucket_for_measure(&buckets, condition->measure_sid);
        if (bucket == NULL || !condition_ref_list_append(&bucket->conditions, condition)) {
            measure_condition_bucket_list_free(&buckets);
            measure_condition_permutation_group_model_list_free(out);
            return 0;
        }
    }

    for (size_t i = 0; i < buckets.len; i++) {
        int ok = bucket_has_matched_conditions(&buckets.items[i])
                     ? build_matched_group(&buckets.items[i], out)
                     : build_unmatched_groups(&buckets.items[i], out);
        if (!ok) {
            measure_condition_bucket_list_free(&buckets);
            measure_condition_permutation_group_model_list_free(out);
            return 0;
        }
    }

    measure_condition_bucket_list_free(&buckets);
    return 1;
}

int measure_condition_permutation_presenter_self_test(void)
{
    MeasureConditionModel conditions[] = {
        { .measure_condition_sid = "1", .measure_sid = "100", .condition_code = "A", .action_code = "01" },
        { .measure_condition_sid = "2", .measure_sid = "100", .condition_code = "B", .action_code = "01" },
    };
    MeasureConditionModelList list = { .items = conditions, .len = 2 };
    MeasureConditionPermutationGroupModelList groups = { 0 };
    int ok = measure_condition_permutation_presenter_build_groups(&list, &groups) &&
             strcmp(measure_condition_permutation_presenter_resource_type(), "measure_condition_permutation") == 0 &&
             strcmp(measure_condition_permutation_group_presenter_resource_type(),
                    "measure_condition_permutation_group") == 0 &&
             groups.len == 2 &&
             strcmp(measure_condition_permutation_group_presenter_id(&groups.items[0]), "100-A") == 0 &&
             groups.items[0].permutations.len == 1 &&
             measure_condition_permutation_presenter_id(&groups.items[0].permutations.items[0]) != NULL &&
             groups.items[0].permutations.items[0].measure_condition_sid_count == 1;
    measure_condition_permutation_group_model_list_free(&groups);
    return ok;
}
