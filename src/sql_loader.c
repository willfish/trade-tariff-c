#include "sql_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

static char *join_path(const char *prefix, const char *relative_path)
{
    size_t prefix_len = strlen(prefix);
    size_t path_len = strlen(relative_path);
    int needs_slash = prefix_len > 0 && prefix[prefix_len - 1] != '/';
    char *path = malloc(prefix_len + (size_t)needs_slash + path_len + 1);
    if (path == NULL) {
        return NULL;
    }

    memcpy(path, prefix, prefix_len);
    if (needs_slash) {
        path[prefix_len] = '/';
    }
    memcpy(path + prefix_len + (size_t)needs_slash, relative_path, path_len + 1);
    return path;
}

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    long len = ftell(file);
    if (len < 0 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    char *contents = malloc((size_t)len + 1);
    if (contents == NULL) {
        fclose(file);
        return NULL;
    }

    size_t read_len = fread(contents, 1, (size_t)len, file);
    fclose(file);
    if (read_len != (size_t)len) {
        free(contents);
        return NULL;
    }

    contents[read_len] = '\0';
    return contents;
}

static char *load_from_prefix(const char *prefix, const char *relative_path)
{
    char *path = join_path(prefix, relative_path);
    if (path == NULL) {
        return NULL;
    }

    char *contents = read_file(path);
    free(path);
    return contents;
}

char *sql_loader_load(const char *relative_path)
{
    const char *sql_dir = getenv("TRADE_TARIFF_C_SQL_DIR");
    if (sql_dir != NULL && *sql_dir != '\0') {
        char *contents = load_from_prefix(sql_dir, relative_path);
        if (contents != NULL) {
            return contents;
        }
    }

    static const char *prefixes[] = {
        "",
        "trade-tariff-c",
        "../trade-tariff-c",
    };

    for (size_t i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); i++) {
        char *contents = load_from_prefix(prefixes[i], relative_path);
        if (contents != NULL) {
            return contents;
        }
    }

    return NULL;
}

static char *token_for_name(const char *name)
{
    size_t name_len = strlen(name);
    char *token = malloc(name_len + 5);
    if (token == NULL) {
        return NULL;
    }

    token[0] = '{';
    token[1] = '{';
    memcpy(token + 2, name, name_len);
    token[name_len + 2] = '}';
    token[name_len + 3] = '}';
    token[name_len + 4] = '\0';
    return token;
}

static size_t occurrence_count(const char *haystack, const char *needle)
{
    size_t count = 0;
    size_t needle_len = strlen(needle);
    const char *cursor = haystack;

    while ((cursor = strstr(cursor, needle)) != NULL) {
        count++;
        cursor += needle_len;
    }

    return count;
}

static char *replace_all(const char *source, const char *token, const char *value)
{
    size_t token_len = strlen(token);
    size_t value_len = strlen(value);
    size_t count = occurrence_count(source, token);
    size_t source_len = strlen(source);
    size_t output_len = source_len - count * token_len + count * value_len;
    char *output = malloc(output_len + 1);
    if (output == NULL) {
        return NULL;
    }

    const char *cursor = source;
    char *writer = output;
    const char *match = NULL;
    while ((match = strstr(cursor, token)) != NULL) {
        size_t chunk_len = (size_t)(match - cursor);
        memcpy(writer, cursor, chunk_len);
        writer += chunk_len;
        memcpy(writer, value, value_len);
        writer += value_len;
        cursor = match + token_len;
    }

    strcpy(writer, cursor);
    return output;
}

char *sql_loader_apply_bindings(const char *template_sql,
                                const SqlTemplateBinding *bindings,
                                size_t binding_count)
{
    char *rendered = copy_string(template_sql);
    if (rendered == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < binding_count; i++) {
        char *token = token_for_name(bindings[i].name);
        if (token == NULL) {
            free(rendered);
            return NULL;
        }

        char *next = replace_all(rendered, token, bindings[i].value);
        free(token);
        free(rendered);
        if (next == NULL) {
            return NULL;
        }
        rendered = next;
    }

    return rendered;
}

int sql_loader_self_test(void)
{
    const char *template_sql = "SELECT * FROM {{SCHEMA}}.goods WHERE date <= {{ACTUAL_DATE}}";
    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", "uk" },
        { "ACTUAL_DATE", "CURRENT_DATE" },
    };

    char *rendered = sql_loader_apply_bindings(template_sql, bindings, 2);
    if (rendered == NULL) {
        return 0;
    }

    int ok = strcmp(rendered, "SELECT * FROM uk.goods WHERE date <= CURRENT_DATE") == 0;
    free(rendered);
    return ok;
}
