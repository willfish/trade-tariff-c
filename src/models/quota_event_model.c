#include "models/quota_event_model.h"

#include <stdlib.h>
#include <string.h>

void quota_event_model_free(QuotaEventModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->quota_definition_sid);
    free(model->occurrence_timestamp);
    free(model->event_type);
    *model = (QuotaEventModel){ 0 };
}

void quota_event_model_list_free(QuotaEventModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        quota_event_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (QuotaEventModelList){ 0 };
}

int quota_event_model_list_append(QuotaEventModelList *list, QuotaEventModel model)
{
    if (list == NULL) {
        quota_event_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        QuotaEventModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            quota_event_model_free(&model);
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

int quota_event_model_self_test(void)
{
    QuotaEventModelList list = { 0 };
    QuotaEventModel event = {
        .quota_definition_sid = copy_literal("888"),
        .occurrence_timestamp = copy_literal("2024-01-01T00:00:00.000Z"),
        .event_type = copy_literal("balance"),
    };

    int ok = event.quota_definition_sid != NULL &&
             event.occurrence_timestamp != NULL &&
             event.event_type != NULL &&
             quota_event_model_list_append(&list, event) &&
             list.len == 1 &&
             strcmp(list.items[0].event_type, "balance") == 0;

    quota_event_model_list_free(&list);
    return ok;
}
