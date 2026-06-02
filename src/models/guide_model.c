#include "models/guide_model.h"

#include <stdlib.h>
#include <string.h>

void guide_model_free(GuideModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->id);
    free(model->title);
    free(model->url);
    *model = (GuideModel){ 0 };
}

void guide_model_list_free(GuideModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        guide_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (GuideModelList){ 0 };
}

int guide_model_list_append(GuideModelList *list, GuideModel model)
{
    if (list == NULL) {
        guide_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        GuideModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            guide_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = model;
    return 1;
}

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

int guide_model_self_test(void)
{
    GuideModelList list = { 0 };
    GuideModel guide = {
        .id = copy_literal("1"),
        .title = copy_literal("Guide title"),
        .url = copy_literal("https://example.test/guide"),
    };

    int ok = guide.id != NULL &&
             guide.title != NULL &&
             guide.url != NULL &&
             guide_model_list_append(&list, guide) &&
             list.len == 1 &&
             strcmp(list.items[0].id, "1") == 0;

    guide_model_list_free(&list);
    return ok;
}
