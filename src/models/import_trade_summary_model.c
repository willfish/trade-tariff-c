#include "models/import_trade_summary_model.h"

#include <stdlib.h>
#include <string.h>

void import_trade_summary_model_free(ImportTradeSummaryModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->id);
    free(model->basic_third_country_duty);
    free(model->preferential_tariff_duty);
    free(model->preferential_quota_duty);
    *model = (ImportTradeSummaryModel){ 0 };
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

int import_trade_summary_model_self_test(void)
{
    ImportTradeSummaryModel model = {
        .id = copy_literal("id"),
        .basic_third_country_duty = copy_literal("8.5%"),
    };
    int ok = model.id != NULL &&
             model.basic_third_country_duty != NULL &&
             strcmp(model.id, "id") == 0 &&
             strcmp(model.basic_third_country_duty, "8.5%") == 0;
    import_trade_summary_model_free(&model);
    return ok;
}
