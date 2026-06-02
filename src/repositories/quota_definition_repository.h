#ifndef TRADE_TARIFF_C_QUOTA_DEFINITION_REPOSITORY_H
#define TRADE_TARIFF_C_QUOTA_DEFINITION_REPOSITORY_H

#include "db.h"
#include "models/quota_definition_model.h"
#include "request/commodity_request_context.h"

char *quota_definition_repository_text_array_param(const char *const *order_numbers,
                                                   size_t order_number_count);
int quota_definition_repository_hydrate_row(const DbRows *rows,
                                            int row,
                                            QuotaDefinitionModel *model);
int quota_definition_repository_fetch_for_order_numbers(const CommodityRequestContext *context,
                                                        const char *const *order_numbers,
                                                        size_t order_number_count,
                                                        QuotaDefinitionModelList *definitions);
int quota_definition_repository_self_test(void);

#endif
