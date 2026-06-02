#ifndef TRADE_TARIFF_C_QUOTA_DEFINITION_PRESENTER_H
#define TRADE_TARIFF_C_QUOTA_DEFINITION_PRESENTER_H

#include "models/quota_definition_model.h"
#include "models/quota_event_model.h"

const char *quota_definition_presenter_resource_type(void);
const char *quota_definition_presenter_jsonapi_resource_type(void);
const char *quota_definition_presenter_id(const QuotaDefinitionModel *model);
int quota_definition_presenter_shows_balance_transfers(const QuotaDefinitionModel *model);
char *quota_definition_presenter_formatted_measurement_unit(const QuotaDefinitionModel *model);
const char *quota_definition_presenter_status(const QuotaDefinitionModel *model,
                                              const QuotaEventModelList *events);
int quota_definition_presenter_self_test(void);

#endif
