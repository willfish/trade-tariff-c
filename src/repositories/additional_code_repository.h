#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_REPOSITORY_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_REPOSITORY_H

#include "db.h"
#include "models/additional_code_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *additional_code_repository_sid_array_param(const char *const *additional_code_sids,
                                                 size_t sid_count);
int additional_code_repository_hydrate_row(const DbRows *rows,
                                           int row,
                                           AdditionalCodeModel *model);
int additional_code_repository_fetch_for_additional_code_sids(const CommodityRequestContext *context,
                                                              const char *const *additional_code_sids,
                                                              size_t sid_count,
                                                              AdditionalCodeModelList *additional_codes);
int additional_code_repository_self_test(void);

#endif
