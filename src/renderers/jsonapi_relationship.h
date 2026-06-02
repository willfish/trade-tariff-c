#ifndef TRADE_TARIFF_C_JSONAPI_RELATIONSHIP_H
#define TRADE_TARIFF_C_JSONAPI_RELATIONSHIP_H

#include <yyjson.h>

yyjson_mut_val *jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                            const char *type,
                                            const char *id);
yyjson_mut_val *jsonapi_resource_object(yyjson_mut_doc *doc,
                                        const char *type,
                                        const char *id);
yyjson_mut_val *jsonapi_data_relationship(yyjson_mut_doc *doc,
                                          yyjson_mut_val *data);
int jsonapi_add_data_relationship(yyjson_mut_doc *doc,
                                  yyjson_mut_val *relationships,
                                  const char *name,
                                  yyjson_mut_val *data);
yyjson_mut_val *jsonapi_empty_array_relationship(yyjson_mut_doc *doc);
yyjson_mut_val *jsonapi_single_relationship(yyjson_mut_doc *doc,
                                            const char *type,
                                            const char *id);
yyjson_mut_val *jsonapi_nullable_relationship(yyjson_mut_doc *doc,
                                              const char *type,
                                              const char *id);

int jsonapi_relationship_self_test(void);

#endif
