#include "renderers/quota_definition_jsonapi_renderer.h"

#include "presenters/quota_definition_presenter.h"
#include "renderers/quota_closed_and_transferred_event_jsonapi_renderer.h"
#include "renderers/jsonapi_relationship.h"

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

yyjson_mut_val *quota_definition_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                             const QuotaDefinitionModel *definition)
{
    const char *id = quota_definition_presenter_id(definition);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc, quota_definition_presenter_jsonapi_resource_type(), id);
}

int quota_definition_jsonapi_shows_balance_transfers(const QuotaDefinitionModel *definition)
{
    return quota_definition_presenter_shows_balance_transfers(definition);
}

static yyjson_mut_val *quota_definition_attributes(yyjson_mut_doc *doc,
                                                   const QuotaDefinitionModel *definition,
                                                   const QuotaEventModelList *quota_events)
{
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *measurement_unit = quota_definition_presenter_formatted_measurement_unit(definition);
    if (attributes == NULL || definition == NULL) {
        free(measurement_unit);
        return NULL;
    }

    int ok =
        add_string_or_null(doc, attributes, "initial_volume", definition->initial_volume) &&
        add_string_or_null(doc, attributes, "validity_start_date", definition->validity_start_date) &&
        add_string_or_null(doc, attributes, "validity_end_date", definition->validity_end_date) &&
        add_string_or_null(doc, attributes, "status", quota_definition_presenter_status(definition, quota_events)) &&
        add_string_or_null(doc, attributes, "description", definition->description) &&
        add_string_or_null(doc, attributes, "balance", definition->balance) &&
        add_string_or_null(doc, attributes, "measurement_unit", measurement_unit) &&
        add_string_or_null(doc, attributes, "monetary_unit", definition->monetary_unit_code) &&
        add_string_or_null(doc, attributes, "measurement_unit_qualifier", definition->measurement_unit_qualifier_code) &&
        add_string_or_null(doc, attributes, "last_allocation_date", definition->last_allocation_date) &&
        add_string_or_null(doc, attributes, "suspension_period_start_date", definition->suspension_period_start_date) &&
        add_string_or_null(doc, attributes, "suspension_period_end_date", definition->suspension_period_end_date) &&
        add_string_or_null(doc, attributes, "blocking_period_start_date", definition->blocking_period_start_date) &&
        add_string_or_null(doc, attributes, "blocking_period_end_date", definition->blocking_period_end_date);

    free(measurement_unit);
    return ok ? attributes : NULL;
}

yyjson_mut_val *quota_definition_jsonapi_resource_with_events(
    yyjson_mut_doc *doc,
    const QuotaDefinitionModel *definition,
    const QuotaEventModelList *quota_events,
    const QuotaClosedAndTransferredEventModelList *events)
{
    const char *id = quota_definition_presenter_id(definition);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       quota_definition_presenter_jsonapi_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = quota_definition_attributes(doc, definition, quota_events);
    if (resource == NULL ||
        attributes == NULL ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        return NULL;
    }

    if (quota_definition_jsonapi_shows_balance_transfers(definition)) {
        yyjson_mut_val *relationships = yyjson_mut_obj(doc);
        yyjson_mut_val *incoming_event = quota_closed_and_transferred_event_jsonapi_relationship(
            doc,
            events,
            definition->quota_definition_sid);
        if (relationships == NULL ||
            incoming_event == NULL ||
            !yyjson_mut_obj_add_val(doc,
                                    relationships,
                                    "incoming_quota_closed_and_transferred_event",
                                    incoming_event) ||
            !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
            return NULL;
        }
    }

    return resource;
}

yyjson_mut_val *quota_definition_jsonapi_resource(yyjson_mut_doc *doc,
                                                  const QuotaDefinitionModel *definition)
{
    return quota_definition_jsonapi_resource_with_events(doc, definition, NULL, NULL);
}

int quota_definition_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .quota_order_number_id = "060004",
        .initial_volume = "1000.0",
        .validity_start_date = "2024-01-01T00:00:00.000Z",
        .balance = "900.0",
        .measurement_unit_description = "Kilogram",
        .measurement_unit_abbreviation = "kg",
    };
    yyjson_mut_val *resource = quota_definition_jsonapi_resource(doc, &definition);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
