#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_REPOSITORY_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_REPOSITORY_H

#include "db.h"
#include "models/geographical_area_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *geographical_area_repository_sid_array_param(const char *const *measure_sids,
                                                   size_t sid_count);
int geographical_area_repository_hydrate_row(const DbRows *rows,
                                             int row,
                                             GeographicalAreaModel *model);
int geographical_area_repository_fetch_for_measure_sids(const CommodityRequestContext *context,
                                                        const char *const *measure_sids,
                                                        size_t sid_count,
                                                        GeographicalAreaModelList *areas);
int geographical_area_repository_self_test(void);

#endif
