#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_REPOSITORY_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_REPOSITORY_H

#include "db.h"
#include "models/quota_order_number_model.h"
#include "request/commodity_request_context.h"

char *quota_order_number_repository_text_array_param(const char *const *order_numbers,
                                                     size_t order_number_count);
int quota_order_number_repository_hydrate_row(const DbRows *rows,
                                              int row,
                                              QuotaOrderNumberModel *model);
int quota_order_number_repository_fetch_for_order_numbers(const CommodityRequestContext *context,
                                                          const char *const *order_numbers,
                                                          size_t order_number_count,
                                                          QuotaOrderNumberModelList *order_number_models);
int quota_order_number_repository_self_test(void);

#endif
