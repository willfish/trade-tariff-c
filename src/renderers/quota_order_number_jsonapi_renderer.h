#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_JSONAPI_RENDERER_H

#include "models/quota_definition_model.h"
#include "models/quota_order_number_model.h"

#include <yyjson.h>

yyjson_mut_val *quota_order_number_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                               const QuotaOrderNumberModel *order_number);
yyjson_mut_val *quota_order_number_jsonapi_resource(yyjson_mut_doc *doc,
                                                    const QuotaOrderNumberModel *order_number,
                                                    const QuotaDefinitionModelList *definitions);
int quota_order_number_jsonapi_renderer_self_test(void);

#endif
