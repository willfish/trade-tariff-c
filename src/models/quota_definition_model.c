#include "models/quota_definition_model.h"

#include <stdlib.h>
#include <string.h>

void quota_definition_model_free(QuotaDefinitionModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->quota_definition_sid);
    free(model->quota_order_number_id);
    free(model->quota_order_number_sid);
    free(model->initial_volume);
    free(model->balance);
    free(model->description);
    free(model->measurement_unit_code);
    free(model->monetary_unit_code);
    free(model->measurement_unit_qualifier_code);
    free(model->last_allocation_date);
    free(model->suspension_period_start_date);
    free(model->suspension_period_end_date);
    free(model->blocking_period_start_date);
    free(model->blocking_period_end_date);
    free(model->validity_start_date);
    free(model->validity_end_date);
    free(model->measurement_unit_description);
    free(model->measurement_unit_abbreviation);
    free(model->critical_state);
    free(model->latest_critical_state);
    free(model->exhausted_event_sid);
    *model = (QuotaDefinitionModel){ 0 };
}

void quota_definition_model_list_free(QuotaDefinitionModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        quota_definition_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (QuotaDefinitionModelList){ 0 };
}

int quota_definition_model_list_append(QuotaDefinitionModelList *list,
                                       QuotaDefinitionModel model)
{
    if (list == NULL) {
        quota_definition_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        QuotaDefinitionModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            quota_definition_model_free(&model);
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

int quota_definition_model_self_test(void)
{
    QuotaDefinitionModelList list = { 0 };
    QuotaDefinitionModel model = {
        .quota_definition_sid = copy_literal("999"),
        .quota_order_number_id = copy_literal("054001"),
        .quota_order_number_sid = copy_literal("123"),
        .initial_volume = copy_literal("1000.0"),
        .balance = copy_literal("900.0"),
    };

    int ok = model.quota_definition_sid != NULL &&
             model.quota_order_number_id != NULL &&
             model.quota_order_number_sid != NULL &&
             model.initial_volume != NULL &&
             model.balance != NULL &&
             quota_definition_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].balance, "900.0") == 0;

    quota_definition_model_list_free(&list);
    return ok;
}
