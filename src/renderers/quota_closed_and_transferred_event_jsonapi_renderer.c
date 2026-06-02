#include "renderers/quota_closed_and_transferred_event_jsonapi_renderer.h"

#include "presenters/quota_closed_and_transferred_event_presenter.h"
#include "renderers/jsonapi_relationship.h"

#include <errno.h>
#include <stdlib.h>

static int add_string_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
}

static int add_number_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }

    char *end = NULL;
    errno = 0;
    double parsed = strtod(value, &end);
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return 0;
    }
    return yyjson_mut_obj_add_real(doc, obj, key, parsed);
}

char *quota_closed_and_transferred_event_jsonapi_id(const QuotaClosedAndTransferredEventModel *event)
{
    return quota_closed_and_transferred_event_presenter_id(event);
}

yyjson_mut_val *quota_closed_and_transferred_event_jsonapi_resource_identifier(
    yyjson_mut_doc *doc,
    const QuotaClosedAndTransferredEventModel *event)
{
    char *id = quota_closed_and_transferred_event_jsonapi_id(event);
    yyjson_mut_val *ref = id == NULL ? NULL : jsonapi_resource_identifier(doc,
                                                                           quota_closed_and_transferred_event_presenter_resource_type(),
                                                                           id);
    free(id);
    return ref;
}

yyjson_mut_val *quota_closed_and_transferred_event_jsonapi_relationship(
    yyjson_mut_doc *doc,
    const QuotaClosedAndTransferredEventModelList *events,
    const char *target_quota_definition_sid)
{
    const QuotaClosedAndTransferredEventModel *event =
        quota_closed_and_transferred_event_presenter_for_target_definition(events,
                                                                          target_quota_definition_sid);
    yyjson_mut_val *data = event == NULL
                               ? yyjson_mut_null(doc)
                               : quota_closed_and_transferred_event_jsonapi_resource_identifier(doc, event);
    return jsonapi_data_relationship(doc, data);
}

yyjson_mut_val *quota_closed_and_transferred_event_jsonapi_resource(
    yyjson_mut_doc *doc,
    const QuotaClosedAndTransferredEventModel *event)
{
    if (doc == NULL || event == NULL) {
        return NULL;
    }

    char *id = quota_closed_and_transferred_event_jsonapi_id(event);
    char *source_unit = quota_closed_and_transferred_event_presenter_quota_definition_measurement_unit(event);
    char *target_unit = quota_closed_and_transferred_event_presenter_target_quota_definition_measurement_unit(event);
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       quota_closed_and_transferred_event_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    int ok = id != NULL &&
             resource != NULL &&
             attributes != NULL &&
             add_string_or_null(doc, attributes, "closing_date", event->closing_date) &&
             add_number_or_null(doc, attributes, "transferred_amount", event->transferred_amount) &&
             add_string_or_null(doc, attributes, "target_quota_definition_validity_start_date", event->target_quota_definition_validity_start_date) &&
             add_string_or_null(doc, attributes, "target_quota_definition_validity_end_date", event->target_quota_definition_validity_end_date) &&
             add_string_or_null(doc, attributes, "target_quota_definition_measurement_unit", target_unit) &&
             add_string_or_null(doc, attributes, "quota_definition_validity_start_date", event->quota_definition_validity_start_date) &&
             add_string_or_null(doc, attributes, "quota_definition_validity_end_date", event->quota_definition_validity_end_date) &&
             add_string_or_null(doc, attributes, "quota_definition_measurement_unit", source_unit) &&
             yyjson_mut_obj_add_val(doc, resource, "attributes", attributes);

    free(id);
    free(source_unit);
    free(target_unit);
    return ok ? resource : NULL;
}

int quota_closed_and_transferred_event_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    QuotaClosedAndTransferredEventModel event = {
        .quota_definition_sid = "100",
        .occurrence_timestamp = "2024-01-01T00:00:00.000Z",
        .target_quota_definition_sid = "200",
        .closing_date = "2024-01-31",
        .transferred_amount = "10.5",
    };
    yyjson_mut_val *resource = quota_closed_and_transferred_event_jsonapi_resource(doc, &event);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
