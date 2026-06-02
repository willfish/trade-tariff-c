#include "presenters/quota_closed_and_transferred_event_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static char *formatted_measurement_unit(const char *description, const char *abbreviation)
{
    if (description == NULL) {
        return NULL;
    }

    const char *presented_abbreviation = abbreviation == NULL ? description : abbreviation;
    size_t len = strlen(description) + strlen(presented_abbreviation) + strlen(" ()") + 1;
    char *formatted = malloc(len);
    if (formatted == NULL) {
        return NULL;
    }
    snprintf(formatted, len, "%s (%s)", description, presented_abbreviation);
    return formatted;
}

const char *quota_closed_and_transferred_event_presenter_resource_type(void)
{
    return "quota_closed_and_transferred_event";
}

char *quota_closed_and_transferred_event_presenter_id(const QuotaClosedAndTransferredEventModel *model)
{
    if (model == NULL || model->quota_definition_sid == NULL || model->occurrence_timestamp == NULL) {
        return NULL;
    }

    size_t len = strlen(model->quota_definition_sid) + strlen(model->occurrence_timestamp) + 2;
    char *id = malloc(len);
    if (id == NULL) {
        return NULL;
    }
    snprintf(id, len, "%s-%s", model->quota_definition_sid, model->occurrence_timestamp);
    return id;
}

char *quota_closed_and_transferred_event_presenter_quota_definition_measurement_unit(
    const QuotaClosedAndTransferredEventModel *model)
{
    return model == NULL
               ? NULL
               : formatted_measurement_unit(model->quota_definition_measurement_unit_description,
                                            model->quota_definition_measurement_unit_abbreviation);
}

char *quota_closed_and_transferred_event_presenter_target_quota_definition_measurement_unit(
    const QuotaClosedAndTransferredEventModel *model)
{
    return model == NULL
               ? NULL
               : formatted_measurement_unit(model->target_quota_definition_measurement_unit_description,
                                            model->target_quota_definition_measurement_unit_abbreviation);
}

const QuotaClosedAndTransferredEventModel *quota_closed_and_transferred_event_presenter_for_target_definition(
    const QuotaClosedAndTransferredEventModelList *events,
    const char *target_quota_definition_sid)
{
    if (events == NULL || target_quota_definition_sid == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < events->len; i++) {
        if (streq(events->items[i].target_quota_definition_sid, target_quota_definition_sid)) {
            return &events->items[i];
        }
    }
    return NULL;
}

int quota_closed_and_transferred_event_presenter_self_test(void)
{
    QuotaClosedAndTransferredEventModel model = {
        .quota_definition_sid = "777",
        .occurrence_timestamp = "2024-01-31T00:00:00.000Z",
        .target_quota_definition_sid = "888",
        .quota_definition_measurement_unit_description = "Kilogram",
        .quota_definition_measurement_unit_abbreviation = "kg",
    };
    char *id = quota_closed_and_transferred_event_presenter_id(&model);
    char *unit = quota_closed_and_transferred_event_presenter_quota_definition_measurement_unit(&model);
    int ok = strcmp(quota_closed_and_transferred_event_presenter_resource_type(),
                    "quota_closed_and_transferred_event") == 0 &&
             strcmp(id == NULL ? "" : id, "777-2024-01-31T00:00:00.000Z") == 0 &&
             strcmp(unit == NULL ? "" : unit, "Kilogram (kg)") == 0;
    free(id);
    free(unit);
    return ok;
}
