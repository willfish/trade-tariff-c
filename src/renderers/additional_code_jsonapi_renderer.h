#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_JSONAPI_RENDERER_H

#include "models/additional_code_model.h"

#include <yyjson.h>

yyjson_mut_val *additional_code_jsonapi_resource(yyjson_mut_doc *doc,
                                                 const AdditionalCodeModel *additional_code);
int additional_code_jsonapi_renderer_self_test(void);

#endif
