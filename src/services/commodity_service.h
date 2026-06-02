#ifndef TRADE_TARIFF_C_COMMODITY_SERVICE_H
#define TRADE_TARIFF_C_COMMODITY_SERVICE_H

#include "common.h"
#include "models/commodity.h"

HttpResponse commodity_service_show_json(const CommodityRequest *request);
int commodity_service_self_test(void);

#endif
