#ifndef TRADE_TARIFF_C_MEASURE_TYPE_REPOSITORY_H
#define TRADE_TARIFF_C_MEASURE_TYPE_REPOSITORY_H

#include "db.h"
#include "models/measure_type_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measure_type_repository_id_array_param(const char *const *ids, size_t id_count);
int measure_type_repository_hydrate_row(const DbRows *rows, int row, MeasureTypeModel *model);
int measure_type_repository_fetch_for_ids(const CommodityRequestContext *context,
                                          const char *const *ids,
                                          size_t id_count,
                                          MeasureTypeModelList *measure_types);
int measure_type_repository_self_test(void);

#endif
