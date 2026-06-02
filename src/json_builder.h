#ifndef TRADE_TARIFF_C_JSON_BUILDER_H
#define TRADE_TARIFF_C_JSON_BUILDER_H

#include <stddef.h>

typedef struct JsonBuilder JsonBuilder;

JsonBuilder *json_builder_new_object(void);
int json_builder_add_string(JsonBuilder *builder, const char *key, const char *value);
int json_builder_add_int(JsonBuilder *builder, const char *key, long long value);
int json_builder_add_bool(JsonBuilder *builder, const char *key, int value);
int json_builder_add_null(JsonBuilder *builder, const char *key);
char *json_builder_write(JsonBuilder *builder, size_t *len);
void json_builder_free(JsonBuilder *builder);
int json_builder_self_test(void);

#endif
