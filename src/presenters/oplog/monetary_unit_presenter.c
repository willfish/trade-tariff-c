#include "presenters/oplog/monetary_unit_presenter.h"

#include <string.h>

const char *monetary_unit_oplog_presenter_resource_type(void)
{
    return "monetary_unit";
}

const char *monetary_unit_oplog_presenter_id(const MonetaryUnitOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int monetary_unit_oplog_presenter_self_test(void)
{
    MonetaryUnitOplogModel model = { .oid = "id" };
    return strcmp(monetary_unit_oplog_presenter_resource_type(), "monetary_unit") == 0 &&
           strcmp(monetary_unit_oplog_presenter_id(&model), "id") == 0;
}
