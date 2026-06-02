#include "renderers/quota_order_number_jsonapi_renderer.h"

#include "presenters/quota_order_number_presenter.h"
#include "renderers/jsonapi_relationship.h"
#include "renderers/quota_definition_jsonapi_renderer.h"

yyjson_mut_val *quota_order_number_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                               const QuotaOrderNumberModel *order_number)
{
    const char *id = quota_order_number_presenter_id(order_number);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc, quota_order_number_presenter_resource_type(), id);
}

static yyjson_mut_val *definition_relationship(yyjson_mut_doc *doc,
                                               const QuotaDefinitionModelList *definitions,
                                               const QuotaOrderNumberModel *order_number)
{
    const QuotaDefinitionModel *definition = quota_order_number_presenter_definition(definitions, order_number);
    yyjson_mut_val *data = definition == NULL
                               ? yyjson_mut_null(doc)
                               : quota_definition_jsonapi_resource_identifier(doc, definition);
    return jsonapi_data_relationship(doc, data);
}

yyjson_mut_val *quota_order_number_jsonapi_resource(yyjson_mut_doc *doc,
                                                    const QuotaOrderNumberModel *order_number,
                                                    const QuotaDefinitionModelList *definitions)
{
    const char *id = quota_order_number_presenter_id(order_number);
    const char *number = quota_order_number_presenter_number(order_number);
    if (doc == NULL || id == NULL || number == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       quota_order_number_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *definition = definition_relationship(doc, definitions, order_number);
    if (resource == NULL ||
        attributes == NULL ||
        relationships == NULL ||
        definition == NULL ||
        !yyjson_mut_obj_add_strcpy(doc, attributes, "number", number) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ||
        !yyjson_mut_obj_add_val(doc, relationships, "definition", definition) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        return NULL;
    }

    return resource;
}

int quota_order_number_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    QuotaOrderNumberModel order_number = {
        .quota_order_number_sid = "777",
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModelList definitions = {
        .items = &definition,
        .len = 1,
        .cap = 1,
    };
    yyjson_mut_val *resource = quota_order_number_jsonapi_resource(doc, &order_number, &definitions);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
