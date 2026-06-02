#include "domain/jsonapi_include_index.h"

#include <stdlib.h>
#include <string.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

static void jsonapi_include_key_free(JsonApiIncludeKey *key)
{
    if (key == NULL) {
        return;
    }
    free(key->type);
    free(key->id);
    *key = (JsonApiIncludeKey){ 0 };
}

void jsonapi_include_index_free(JsonApiIncludeIndex *index)
{
    if (index == NULL) {
        return;
    }
    for (size_t i = 0; i < index->len; i++) {
        jsonapi_include_key_free(&index->items[i]);
    }
    free(index->items);
    *index = (JsonApiIncludeIndex){ 0 };
}

int jsonapi_include_index_seen(const JsonApiIncludeIndex *index,
                               const char *type,
                               const char *id)
{
    if (index == NULL || type == NULL || id == NULL) {
        return 0;
    }

    for (size_t i = 0; i < index->len; i++) {
        if (index->items[i].type != NULL &&
            index->items[i].id != NULL &&
            strcmp(index->items[i].type, type) == 0 &&
            strcmp(index->items[i].id, id) == 0) {
            return 1;
        }
    }
    return 0;
}

static int jsonapi_include_index_remember(JsonApiIncludeIndex *index,
                                          const char *type,
                                          const char *id)
{
    if (index == NULL || type == NULL || id == NULL) {
        return 0;
    }
    if (jsonapi_include_index_seen(index, type, id)) {
        return 1;
    }

    if (index->len == index->cap) {
        size_t next_cap = index->cap == 0 ? 16 : index->cap * 2;
        JsonApiIncludeKey *next = realloc(index->items, next_cap * sizeof(*next));
        if (next == NULL) {
            return 0;
        }
        index->items = next;
        index->cap = next_cap;
    }

    JsonApiIncludeKey key = {
        .type = copy_literal(type),
        .id = copy_literal(id),
    };
    if (key.type == NULL || key.id == NULL) {
        jsonapi_include_key_free(&key);
        return 0;
    }

    index->items[index->len++] = key;
    return 1;
}

int jsonapi_include_index_append_resource(JsonApiIncludeIndex *index,
                                          yyjson_mut_val *included,
                                          const char *type,
                                          const char *id,
                                          yyjson_mut_val *resource)
{
    if (index == NULL || included == NULL || type == NULL || id == NULL || resource == NULL) {
        return 0;
    }
    if (jsonapi_include_index_seen(index, type, id)) {
        return 1;
    }
    if (!jsonapi_include_index_remember(index, type, id)) {
        return 0;
    }
    return yyjson_mut_arr_append(included, resource);
}

int jsonapi_include_index_self_test(void)
{
    JsonApiIncludeIndex index = { 0 };
    int ok = !jsonapi_include_index_seen(&index, "measure", "1") &&
             jsonapi_include_index_remember(&index, "measure", "1") &&
             jsonapi_include_index_seen(&index, "measure", "1") &&
             !jsonapi_include_index_seen(&index, "measure", "2") &&
             !jsonapi_include_index_seen(&index, "footnote", "1");
    jsonapi_include_index_free(&index);
    return ok;
}
