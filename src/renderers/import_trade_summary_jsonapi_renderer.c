#include "renderers/import_trade_summary_jsonapi_renderer.h"

#include "presenters/import_trade_summary_presenter.h"
#include "renderers/jsonapi_relationship.h"

#include <string.h>

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

yyjson_mut_val *import_trade_summary_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                 const ImportTradeSummaryModel *model)
{
    if (doc == NULL || model == NULL || model->id == NULL) {
        return NULL;
    }
    const char *id = import_trade_summary_presenter_id(model);

    return jsonapi_resource_identifier(doc,
                                       import_trade_summary_presenter_resource_type(),
                                       id);
}

yyjson_mut_val *import_trade_summary_jsonapi_resource(yyjson_mut_doc *doc,
                                                      const ImportTradeSummaryModel *model)
{
    if (doc == NULL || model == NULL || model->id == NULL) {
        return NULL;
    }
    const char *id = import_trade_summary_presenter_id(model);

    yyjson_mut_val *resource = jsonapi_resource_object(doc, import_trade_summary_presenter_resource_type(), id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (resource == NULL || attributes == NULL ||
        !add_string_or_null(doc, attributes, "basic_third_country_duty", model->basic_third_country_duty) ||
        !add_string_or_null(doc, attributes, "preferential_tariff_duty", model->preferential_tariff_duty) ||
        !add_string_or_null(doc, attributes, "preferential_quota_duty", model->preferential_quota_duty) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        return NULL;
    }
    return resource;
}

int import_trade_summary_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    ImportTradeSummaryModel model = {
        .id = "abc",
        .basic_third_country_duty = "8.5 %",
    };
    yyjson_mut_val *resource = import_trade_summary_jsonapi_resource(doc, &model);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
