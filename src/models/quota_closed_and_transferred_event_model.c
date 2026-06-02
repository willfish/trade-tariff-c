#include "models/quota_closed_and_transferred_event_model.h"

#include <stdlib.h>
#include <string.h>

void quota_closed_and_transferred_event_model_free(QuotaClosedAndTransferredEventModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->quota_definition_sid);
    free(model->occurrence_timestamp);
    free(model->target_quota_definition_sid);
    free(model->closing_date);
    free(model->transferred_amount);
    free(model->quota_definition_validity_start_date);
    free(model->quota_definition_validity_end_date);
    free(model->quota_definition_measurement_unit_description);
    free(model->quota_definition_measurement_unit_abbreviation);
    free(model->target_quota_definition_validity_start_date);
    free(model->target_quota_definition_validity_end_date);
    free(model->target_quota_definition_measurement_unit_description);
    free(model->target_quota_definition_measurement_unit_abbreviation);
    *model = (QuotaClosedAndTransferredEventModel){ 0 };
}

void quota_closed_and_transferred_event_model_list_free(QuotaClosedAndTransferredEventModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        quota_closed_and_transferred_event_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (QuotaClosedAndTransferredEventModelList){ 0 };
}

int quota_closed_and_transferred_event_model_list_append(QuotaClosedAndTransferredEventModelList *list,
                                                         QuotaClosedAndTransferredEventModel model)
{
    if (list == NULL) {
        quota_closed_and_transferred_event_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        QuotaClosedAndTransferredEventModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            quota_closed_and_transferred_event_model_free(&model);
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

int quota_closed_and_transferred_event_model_self_test(void)
{
    QuotaClosedAndTransferredEventModelList list = { 0 };
    QuotaClosedAndTransferredEventModel model = {
        .quota_definition_sid = copy_literal("100"),
        .occurrence_timestamp = copy_literal("2024-01-01T00:00:00.000Z"),
        .target_quota_definition_sid = copy_literal("200"),
    };

    int ok = model.quota_definition_sid != NULL &&
             model.occurrence_timestamp != NULL &&
             model.target_quota_definition_sid != NULL &&
             quota_closed_and_transferred_event_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].target_quota_definition_sid, "200") == 0;

    quota_closed_and_transferred_event_model_list_free(&list);
    return ok;
}
