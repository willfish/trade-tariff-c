#ifndef TRADE_TARIFF_C_MEASURE_COMPONENT_REPOSITORY_H
#define TRADE_TARIFF_C_MEASURE_COMPONENT_REPOSITORY_H

#include "db.h"
#include "models/measure_component_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measure_component_repository_sid_array_param(const char *const *measure_sids,
                                                   size_t sid_count);
int measure_component_repository_hydrate_row(const DbRows *rows,
                                             int row,
                                             MeasureComponentModel *model);
int measure_component_repository_fetch_for_measure_sids(const CommodityRequestContext *context,
                                                        const char *const *measure_sids,
                                                        size_t sid_count,
                                                        MeasureComponentModelList *components);
int measure_component_repository_fetch_resolved_meursing_for_measure_sids(const CommodityRequestContext *context,
                                                                          const char *const *measure_sids,
                                                                          size_t sid_count,
                                                                          MeasureComponentModelList *components);
int measure_component_repository_self_test(void);

#endif
