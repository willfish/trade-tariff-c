#ifndef TRADE_TARIFF_C_COMMODITY_TREE_SLICE_RENDERER_H
#define TRADE_TARIFF_C_COMMODITY_TREE_SLICE_RENDERER_H

#include "domain/commodity_aggregate.h"
#include "request/commodity_include_plan.h"

char *commodity_tree_slice_render_jsonapi(const CommodityAggregate *aggregate,
                                          const char *service);
char *commodity_tree_slice_render_jsonapi_with_plan(const CommodityAggregate *aggregate,
                                                    const char *service,
                                                    const CommodityIncludePlan *plan);
int commodity_tree_slice_renderer_self_test(void);

#endif
