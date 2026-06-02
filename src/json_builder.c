#include "json_builder.h"

#include <stdlib.h>
#include <string.h>
#include <yyjson.h>

struct JsonBuilder {
    yyjson_mut_doc *doc;
    yyjson_mut_val *root;
};

JsonBuilder *json_builder_new_object(void)
{
    JsonBuilder *builder = calloc(1, sizeof(*builder));
    if (builder == NULL) {
        return NULL;
    }

    builder->doc = yyjson_mut_doc_new(NULL);
    if (builder->doc == NULL) {
        free(builder);
        return NULL;
    }

    builder->root = yyjson_mut_obj(builder->doc);
    if (builder->root == NULL) {
        json_builder_free(builder);
        return NULL;
    }
    yyjson_mut_doc_set_root(builder->doc, builder->root);
    return builder;
}

static int add_value(JsonBuilder *builder, const char *key, yyjson_mut_val *value)
{
    if (builder == NULL || builder->doc == NULL || builder->root == NULL || key == NULL || value == NULL) {
        return 0;
    }

    yyjson_mut_val *json_key = yyjson_mut_strcpy(builder->doc, key);
    if (json_key == NULL) {
        return 0;
    }

    return yyjson_mut_obj_add(builder->root, json_key, value);
}

int json_builder_add_string(JsonBuilder *builder, const char *key, const char *value)
{
    if (builder == NULL || builder->doc == NULL) {
        return 0;
    }

    yyjson_mut_val *json_value = yyjson_mut_strcpy(builder->doc, value == NULL ? "" : value);
    return add_value(builder, key, json_value);
}

int json_builder_add_int(JsonBuilder *builder, const char *key, long long value)
{
    if (builder == NULL || builder->doc == NULL) {
        return 0;
    }

    return add_value(builder, key, yyjson_mut_int(builder->doc, value));
}

int json_builder_add_bool(JsonBuilder *builder, const char *key, int value)
{
    if (builder == NULL || builder->doc == NULL) {
        return 0;
    }

    return add_value(builder, key, yyjson_mut_bool(builder->doc, value != 0));
}

int json_builder_add_null(JsonBuilder *builder, const char *key)
{
    if (builder == NULL || builder->doc == NULL) {
        return 0;
    }

    return add_value(builder, key, yyjson_mut_null(builder->doc));
}

char *json_builder_write(JsonBuilder *builder, size_t *len)
{
    if (builder == NULL || builder->doc == NULL) {
        return NULL;
    }

    return yyjson_mut_write(builder->doc, YYJSON_WRITE_NOFLAG, len);
}

void json_builder_free(JsonBuilder *builder)
{
    if (builder == NULL) {
        return;
    }

    yyjson_mut_doc_free(builder->doc);
    free(builder);
}

int json_builder_self_test(void)
{
    JsonBuilder *builder = json_builder_new_object();
    if (builder == NULL) {
        return 0;
    }

    size_t len = 0;
    int ok = json_builder_add_string(builder, "type", "commodity") &&
             json_builder_add_int(builder, "id", 123) &&
             json_builder_add_bool(builder, "declarable", 1) &&
             json_builder_add_null(builder, "validity_end_date");
    char *json = json_builder_write(builder, &len);
    ok = ok &&
         json != NULL &&
         len > 0 &&
         strstr(json, "\"type\":\"commodity\"") != NULL &&
         strstr(json, "\"id\":123") != NULL &&
         strstr(json, "\"declarable\":true") != NULL &&
         strstr(json, "\"validity_end_date\":null") != NULL;
    free(json);
    json_builder_free(builder);
    return ok;
}
