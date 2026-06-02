#include "presenters/quota_order_number_presenter.h"

#include <string.h>

const char *quota_order_number_presenter_resource_type(void)
{
    return "order_number";
}

const char *quota_order_number_presenter_id(const QuotaOrderNumberModel *model)
{
    return model == NULL ? NULL : model->quota_order_number_id;
}

const char *quota_order_number_presenter_number(const QuotaOrderNumberModel *model)
{
    return quota_order_number_presenter_id(model);
}

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

const QuotaDefinitionModel *quota_order_number_presenter_definition(
    const QuotaDefinitionModelList *definitions,
    const QuotaOrderNumberModel *model)
{
    const char *order_number_id = quota_order_number_presenter_id(model);
    if (definitions == NULL || order_number_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < definitions->len; i++) {
        if (streq(definitions->items[i].quota_order_number_id, order_number_id)) {
            return &definitions->items[i];
        }
    }
    return NULL;
}

int quota_order_number_presenter_self_test(void)
{
    QuotaOrderNumberModel order_number = {
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModel definition = {
        .quota_definition_sid = "123",
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModelList definitions = {
        .items = &definition,
        .len = 1,
        .cap = 1,
    };
    return strcmp(quota_order_number_presenter_resource_type(), "order_number") == 0 &&
           strcmp(quota_order_number_presenter_id(&order_number), "060004") == 0 &&
           quota_order_number_presenter_definition(&definitions, &order_number) == &definition;
}
