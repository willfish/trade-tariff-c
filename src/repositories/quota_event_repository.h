#ifndef TRADE_TARIFF_C_QUOTA_EVENT_REPOSITORY_H
#define TRADE_TARIFF_C_QUOTA_EVENT_REPOSITORY_H

#include "db.h"
#include "models/quota_event_model.h"
#include "request/commodity_request_context.h"

char *quota_event_repository_text_array_param(const char *const *quota_definition_sids,
                                              size_t quota_definition_sid_count);
int quota_event_repository_hydrate_row(const DbRows *rows,
                                       int row,
                                       QuotaEventModel *model);
int quota_event_repository_fetch_latest_for_definition_sids(const CommodityRequestContext *context,
                                                            const char *const *quota_definition_sids,
                                                            size_t quota_definition_sid_count,
                                                            QuotaEventModelList *events);
int quota_event_repository_self_test(void);

#endif
