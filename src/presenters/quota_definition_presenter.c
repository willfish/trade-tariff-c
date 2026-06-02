#include "presenters/quota_definition_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *quota_definition_presenter_resource_type(void)
{
    return "quota_definition";
}

const char *quota_definition_presenter_jsonapi_resource_type(void)
{
    return "definition";
}

const char *quota_definition_presenter_id(const QuotaDefinitionModel *model)
{
    return model == NULL ? NULL : model->quota_definition_sid;
}

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static const QuotaEventModel *latest_event_for_definition(const QuotaEventModelList *events,
                                                          const char *quota_definition_sid)
{
    if (events == NULL || quota_definition_sid == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < events->len; i++) {
        if (streq(events->items[i].quota_definition_sid, quota_definition_sid)) {
            return &events->items[i];
        }
    }
    return NULL;
}

int quota_definition_presenter_shows_balance_transfers(const QuotaDefinitionModel *model)
{
    return model != NULL &&
           model->validity_start_date != NULL &&
           strcmp(model->validity_start_date, "2022-07-01") >= 0;
}

char *quota_definition_presenter_formatted_measurement_unit(const QuotaDefinitionModel *model)
{
    if (model == NULL || model->measurement_unit_description == NULL) {
        return NULL;
    }

    const char *abbreviation = model->measurement_unit_abbreviation == NULL
                                   ? model->measurement_unit_description
                                   : model->measurement_unit_abbreviation;
    size_t len = strlen(model->measurement_unit_description) + strlen(abbreviation) + strlen(" ()") + 1;
    char *formatted = malloc(len);
    if (formatted == NULL) {
        return NULL;
    }
    snprintf(formatted, len, "%s (%s)", model->measurement_unit_description, abbreviation);
    return formatted;
}

const char *quota_definition_presenter_status(const QuotaDefinitionModel *model,
                                              const QuotaEventModelList *events)
{
    if (model == NULL) {
        return "Open";
    }

    if (model->suspension_period_start_date != NULL &&
        model->suspension_period_end_date != NULL) {
        return "Suspended";
    }
    if (model->blocking_period_start_date != NULL &&
        model->blocking_period_end_date != NULL) {
        return "Blocked";
    }

    const QuotaEventModel *latest_event = latest_event_for_definition(events, model->quota_definition_sid);
    if (latest_event != NULL) {
        if (streq(latest_event->event_type, "exhaustion")) {
            return "Exhausted";
        }
        if (streq(latest_event->event_type, "critical")) {
            return "Critical";
        }
        if (streq(model->latest_critical_state, "Y")) {
            return "Critical";
        }
        return "Open";
    }

    return streq(model->critical_state, "Y") ? "Critical" : "Open";
}

int quota_definition_presenter_self_test(void)
{
    QuotaDefinitionModel model = { .quota_definition_sid = "654" };
    QuotaEventModel reopened = {
        .quota_definition_sid = "654",
        .occurrence_timestamp = "2024-02-01T00:00:00.000Z",
        .event_type = "reopening",
    };
    QuotaEventModelList events = {
        .items = &reopened,
        .len = 1,
        .cap = 1,
    };
    return strcmp(quota_definition_presenter_resource_type(), "quota_definition") == 0 &&
           strcmp(quota_definition_presenter_jsonapi_resource_type(), "definition") == 0 &&
           strcmp(quota_definition_presenter_id(&model), "654") == 0 &&
           strcmp(quota_definition_presenter_status(&model, &events), "Open") == 0;
}
