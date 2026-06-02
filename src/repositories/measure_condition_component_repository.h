#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_REPOSITORY_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_COMPONENT_REPOSITORY_H

#include "db.h"
#include "models/measure_condition_component_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measure_condition_component_repository_sid_array_param(const char *const *measure_condition_sids,
                                                             size_t sid_count);
int measure_condition_component_repository_hydrate_row(const DbRows *rows,
                                                       int row,
                                                       MeasureConditionComponentModel *model);
int measure_condition_component_repository_fetch_for_measure_condition_sids(const CommodityRequestContext *context,
                                                                            const char *const *measure_condition_sids,
                                                                            size_t sid_count,
                                                                            MeasureConditionComponentModelList *components);
int measure_condition_component_repository_self_test(void);

#endif
