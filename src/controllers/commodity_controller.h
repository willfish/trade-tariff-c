#ifndef TRADE_TARIFF_C_COMMODITY_CONTROLLER_H
#define TRADE_TARIFF_C_COMMODITY_CONTROLLER_H

#include "common.h"

HttpResponse commodity_controller_show(const char *service, const char *commodity_id, const char *query);
int commodity_controller_self_test(void);

#endif
