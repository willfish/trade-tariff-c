#ifndef TRADE_TARIFF_C_JSONAPI_INCLUDE_INDEX_H
#define TRADE_TARIFF_C_JSONAPI_INCLUDE_INDEX_H

#include <stddef.h>
#include <yyjson.h>

typedef struct {
    char *type;
    char *id;
} JsonApiIncludeKey;

typedef struct {
    JsonApiIncludeKey *items;
    size_t len;
    size_t cap;
} JsonApiIncludeIndex;

void jsonapi_include_index_free(JsonApiIncludeIndex *index);
int jsonapi_include_index_seen(const JsonApiIncludeIndex *index,
                               const char *type,
                               const char *id);
int jsonapi_include_index_append_resource(JsonApiIncludeIndex *index,
                                          yyjson_mut_val *included,
                                          const char *type,
                                          const char *id,
                                          yyjson_mut_val *resource);
int jsonapi_include_index_self_test(void);

#endif
