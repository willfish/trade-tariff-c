#ifndef TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_REPOSITORY_H
#define TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_REPOSITORY_H

#include "db.h"
#include "models/quota_closed_and_transferred_event_model.h"
#include "request/commodity_request_context.h"

char *quota_closed_and_transferred_event_repository_text_array_param(const char *const *quota_definition_sids,
                                                                     size_t quota_definition_sid_count);
int quota_closed_and_transferred_event_repository_hydrate_row(const DbRows *rows,
                                                              int row,
                                                              QuotaClosedAndTransferredEventModel *model);
int quota_closed_and_transferred_event_repository_fetch_for_target_definition_sids(
    const CommodityRequestContext *context,
    const char *const *quota_definition_sids,
    size_t quota_definition_sid_count,
    QuotaClosedAndTransferredEventModelList *events);
int quota_closed_and_transferred_event_repository_self_test(void);

#endif
