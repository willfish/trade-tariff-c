#ifndef TRADE_TARIFF_C_CLASSIFICATION_REPOSITORY_H
#define TRADE_TARIFF_C_CLASSIFICATION_REPOSITORY_H

#include "db.h"
#include "models/classification_model.h"
#include "request/commodity_request_context.h"

int classification_repository_hydrate_row(const DbRows *rows,
                                          int row,
                                          ClassificationModel *model);
int classification_repository_fetch_for_commodity(const CommodityRequestContext *context,
                                                  ClassificationModel *model);
int classification_repository_self_test(void);

#endif
