#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_CODE_REPOSITORY_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_CODE_REPOSITORY_H

#include "db.h"
#include "models/measure_condition_code_model.h"
#include "request/commodity_request_context.h"

char *measure_condition_code_repository_code_array_param(const char *const *codes,
                                                         size_t code_count);
int measure_condition_code_repository_hydrate_row(const DbRows *rows,
                                                  int row,
                                                  MeasureConditionCodeModel *model);
int measure_condition_code_repository_fetch_for_codes(const CommodityRequestContext *context,
                                                      const char *const *codes,
                                                      size_t code_count,
                                                      MeasureConditionCodeModelList *condition_codes);
int measure_condition_code_repository_self_test(void);

#endif
