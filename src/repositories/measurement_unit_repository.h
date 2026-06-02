#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_REPOSITORY_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_REPOSITORY_H

#include "db.h"
#include "models/measurement_unit_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measurement_unit_repository_code_array_param(const char *const *codes,
                                                   size_t code_count);
int measurement_unit_repository_hydrate_row(const DbRows *rows,
                                            int row,
                                            MeasurementUnitModel *model);
int measurement_unit_repository_fetch_for_codes(const CommodityRequestContext *context,
                                                const char *const *codes,
                                                size_t code_count,
                                                MeasurementUnitModelList *units);
int measurement_unit_repository_self_test(void);

#endif
