#ifndef TRADE_TARIFF_C_IMPORT_TRADE_SUMMARY_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_IMPORT_TRADE_SUMMARY_JSONAPI_RENDERER_H

#include "models/import_trade_summary_model.h"

#include <yyjson.h>

yyjson_mut_val *import_trade_summary_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                 const ImportTradeSummaryModel *model);
yyjson_mut_val *import_trade_summary_jsonapi_resource(yyjson_mut_doc *doc,
                                                      const ImportTradeSummaryModel *model);
int import_trade_summary_jsonapi_renderer_self_test(void);

#endif
