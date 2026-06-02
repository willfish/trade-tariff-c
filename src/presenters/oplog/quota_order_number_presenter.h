#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_OPLOG_PRESENTER_H

#include "models/oplog/quota_order_number_model.h"

const char *quota_order_number_oplog_presenter_resource_type(void);
const char *quota_order_number_oplog_presenter_id(const QuotaOrderNumberOplogModel *model);
int quota_order_number_oplog_presenter_self_test(void);

#endif
