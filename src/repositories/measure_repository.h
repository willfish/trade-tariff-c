#ifndef TRADE_TARIFF_C_MEASURE_REPOSITORY_H
#define TRADE_TARIFF_C_MEASURE_REPOSITORY_H

#include "db.h"
#include "models/measure_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measure_repository_sid_array_param(const char *const *goods_nomenclature_sids,
                                         size_t sid_count);
int measure_repository_hydrate_row(const DbRows *rows, int row, MeasureModel *model);
int measure_repository_fetch_for_goods_nomenclature_sids(const CommodityRequestContext *context,
                                                         const char *const *goods_nomenclature_sids,
                                                         size_t sid_count,
                                                         MeasureModelList *measures);
int measure_repository_self_test(void);

#endif
