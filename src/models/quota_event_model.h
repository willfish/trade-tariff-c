#ifndef TRADE_TARIFF_C_QUOTA_EVENT_MODEL_H
#define TRADE_TARIFF_C_QUOTA_EVENT_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *event_type;
} QuotaEventModel;

typedef struct {
    QuotaEventModel *items;
    size_t len;
    size_t cap;
} QuotaEventModelList;

void quota_event_model_free(QuotaEventModel *model);
void quota_event_model_list_free(QuotaEventModelList *list);
int quota_event_model_list_append(QuotaEventModelList *list, QuotaEventModel model);
int quota_event_model_self_test(void);

#endif
