#ifndef TRADE_TARIFF_C_JSONAPI_ATTRIBUTE_H
#define TRADE_TARIFF_C_JSONAPI_ATTRIBUTE_H

#include <yyjson.h>

int jsonapi_add_string_or_null(yyjson_mut_doc *doc,
                               yyjson_mut_val *obj,
                               const char *key,
                               const char *value);
int jsonapi_add_number_string_or_null(yyjson_mut_doc *doc,
                                      yyjson_mut_val *obj,
                                      const char *key,
                                      const char *value);

#endif
