#ifndef TRADE_TARIFF_C_ROUTER_H
#define TRADE_TARIFF_C_ROUTER_H

#include "common.h"

HttpResponse route_request(const char *method, const char *path, const char *query);

#endif
