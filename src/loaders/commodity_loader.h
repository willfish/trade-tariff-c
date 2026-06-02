#ifndef TRADE_TARIFF_C_COMMODITY_LOADER_H
#define TRADE_TARIFF_C_COMMODITY_LOADER_H

#include "domain/commodity_aggregate.h"
#include "request/commodity_include_plan.h"
#include "request/commodity_request_context.h"

int commodity_loader_load(const CommodityRequestContext *context,
                          CommodityAggregate *aggregate);
int commodity_loader_load_with_plan(const CommodityRequestContext *context,
                                    const CommodityIncludePlan *plan,
                                    CommodityAggregate *aggregate);
int commodity_loader_self_test(void);

#endif
