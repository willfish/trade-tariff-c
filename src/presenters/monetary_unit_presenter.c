#include "presenters/monetary_unit_presenter.h"

#include <string.h>

const char *monetary_unit_presenter_resource_type(void)
{
    return "monetary_unit";
}

const char *monetary_unit_presenter_id(const MonetaryUnitModel *model)
{
    return model == NULL ? NULL : model->monetary_unit_code;
}

const char *monetary_unit_presenter_abbreviation_for_code(const char *monetary_unit_code)
{
    if (monetary_unit_code != NULL && strcmp(monetary_unit_code, "EUC") == 0) {
        return "EUR (EUC)";
    }
    return NULL;
}

int monetary_unit_presenter_self_test(void)
{
    MonetaryUnitModel model = { .monetary_unit_code = "GBP" };
    return strcmp(monetary_unit_presenter_resource_type(), "monetary_unit") == 0 &&
           strcmp(monetary_unit_presenter_id(&model), "GBP") == 0 &&
           strcmp(monetary_unit_presenter_abbreviation_for_code("EUC"), "EUR (EUC)") == 0 &&
           monetary_unit_presenter_abbreviation_for_code("GBP") == NULL;
}
