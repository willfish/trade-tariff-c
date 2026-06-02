#ifndef TRADE_TARIFF_C_COMMODITY_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_COMMODITY_OPLOG_PRESENTER_H

#include "models/oplog/commodity_model.h"

const char *commodity_oplog_presenter_resource_type(void);
const char *commodity_oplog_presenter_id(const CommodityOplogModel *model);
int commodity_oplog_presenter_self_test(void);

#endif
