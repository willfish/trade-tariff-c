#include "presenters/oplog/monetary_unit_description_presenter.h"

#include <string.h>

const char *monetary_unit_description_oplog_presenter_resource_type(void)
{
    return "monetary_unit_description";
}

const char *monetary_unit_description_oplog_presenter_id(const MonetaryUnitDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->language_id;
}

int monetary_unit_description_oplog_presenter_self_test(void)
{
    MonetaryUnitDescriptionOplogModel model = { .language_id = "id" };
    return strcmp(monetary_unit_description_oplog_presenter_resource_type(), "monetary_unit_description") == 0 &&
           strcmp(monetary_unit_description_oplog_presenter_id(&model), "id") == 0;
}
