#ifndef TRADE_TARIFF_C_QUOTA_DEFINITION_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_QUOTA_DEFINITION_JSONAPI_RENDERER_H

#include "models/quota_closed_and_transferred_event_model.h"
#include "models/quota_definition_model.h"
#include "models/quota_event_model.h"

#include <yyjson.h>

yyjson_mut_val *quota_definition_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                             const QuotaDefinitionModel *definition);
int quota_definition_jsonapi_shows_balance_transfers(const QuotaDefinitionModel *definition);
yyjson_mut_val *quota_definition_jsonapi_resource(yyjson_mut_doc *doc,
                                                  const QuotaDefinitionModel *definition);
yyjson_mut_val *quota_definition_jsonapi_resource_with_events(
    yyjson_mut_doc *doc,
    const QuotaDefinitionModel *definition,
    const QuotaEventModelList *quota_events,
    const QuotaClosedAndTransferredEventModelList *events);
int quota_definition_jsonapi_renderer_self_test(void);

#endif
