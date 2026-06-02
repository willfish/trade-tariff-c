#ifndef TRADE_TARIFF_C_QUOTA_BALANCE_EVENT_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_QUOTA_BALANCE_EVENT_OPLOG_PRESENTER_H

#include "models/oplog/quota_balance_event_model.h"

const char *quota_balance_event_oplog_presenter_resource_type(void);
const char *quota_balance_event_oplog_presenter_id(const QuotaBalanceEventOplogModel *model);
int quota_balance_event_oplog_presenter_self_test(void);

#endif
