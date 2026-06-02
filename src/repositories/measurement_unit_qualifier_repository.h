#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_REPOSITORY_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_QUALIFIER_REPOSITORY_H

#include "db.h"
#include "models/measurement_unit_qualifier_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measurement_unit_qualifier_repository_code_array_param(const char *const *codes,
                                                             size_t code_count);
int measurement_unit_qualifier_repository_hydrate_row(const DbRows *rows,
                                                      int row,
                                                      MeasurementUnitQualifierModel *model);
int measurement_unit_qualifier_repository_fetch_for_codes(const CommodityRequestContext *context,
                                                          const char *const *codes,
                                                          size_t code_count,
                                                          MeasurementUnitQualifierModelList *qualifiers);
int measurement_unit_qualifier_repository_self_test(void);

#endif
