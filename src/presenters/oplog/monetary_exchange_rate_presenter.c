#include "presenters/oplog/monetary_exchange_rate_presenter.h"

#include <string.h>

const char *monetary_exchange_rate_oplog_presenter_resource_type(void)
{
    return "monetary_exchange_rate";
}

const char *monetary_exchange_rate_oplog_presenter_id(const MonetaryExchangeRateOplogModel *model)
{
    return model == NULL ? NULL : model->monetary_exchange_period_sid;
}

int monetary_exchange_rate_oplog_presenter_self_test(void)
{
    MonetaryExchangeRateOplogModel model = { .monetary_exchange_period_sid = "id" };
    return strcmp(monetary_exchange_rate_oplog_presenter_resource_type(), "monetary_exchange_rate") == 0 &&
           strcmp(monetary_exchange_rate_oplog_presenter_id(&model), "id") == 0;
}
