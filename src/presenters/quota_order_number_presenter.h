#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_PRESENTER_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_PRESENTER_H

#include "models/quota_definition_model.h"
#include "models/quota_order_number_model.h"

const char *quota_order_number_presenter_resource_type(void);
const char *quota_order_number_presenter_id(const QuotaOrderNumberModel *model);
const char *quota_order_number_presenter_number(const QuotaOrderNumberModel *model);
const QuotaDefinitionModel *quota_order_number_presenter_definition(
    const QuotaDefinitionModelList *definitions,
    const QuotaOrderNumberModel *model);
int quota_order_number_presenter_self_test(void);

#endif
