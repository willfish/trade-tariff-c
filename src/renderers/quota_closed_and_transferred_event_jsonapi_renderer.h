#ifndef TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_JSONAPI_RENDERER_H

#include "models/quota_closed_and_transferred_event_model.h"

#include <yyjson.h>

char *quota_closed_and_transferred_event_jsonapi_id(const QuotaClosedAndTransferredEventModel *event);
yyjson_mut_val *quota_closed_and_transferred_event_jsonapi_resource_identifier(
    yyjson_mut_doc *doc,
    const QuotaClosedAndTransferredEventModel *event);
yyjson_mut_val *quota_closed_and_transferred_event_jsonapi_relationship(
    yyjson_mut_doc *doc,
    const QuotaClosedAndTransferredEventModelList *events,
    const char *target_quota_definition_sid);
yyjson_mut_val *quota_closed_and_transferred_event_jsonapi_resource(
    yyjson_mut_doc *doc,
    const QuotaClosedAndTransferredEventModel *event);
int quota_closed_and_transferred_event_jsonapi_renderer_self_test(void);

#endif
