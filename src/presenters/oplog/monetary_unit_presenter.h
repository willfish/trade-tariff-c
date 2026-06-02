#ifndef TRADE_TARIFF_C_MONETARY_UNIT_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MONETARY_UNIT_OPLOG_PRESENTER_H

#include "models/oplog/monetary_unit_model.h"

const char *monetary_unit_oplog_presenter_resource_type(void);
const char *monetary_unit_oplog_presenter_id(const MonetaryUnitOplogModel *model);
int monetary_unit_oplog_presenter_self_test(void);

#endif
