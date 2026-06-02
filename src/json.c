#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *json_escape(const char *input)
{
    size_t extra = 0;

    for (const char *p = input; *p; p++) {
        if (*p == '"' || *p == '\\' || *p == '\n' || *p == '\r' || *p == '\t') {
            extra++;
        }
    }

    size_t len = strlen(input);
    char *escaped = malloc(len + extra + 1);
    if (escaped == NULL) {
        return NULL;
    }

    char *out = escaped;
    for (const char *p = input; *p; p++) {
        if (*p == '"' || *p == '\\') {
            *out++ = '\\';
        } else if (*p == '\n') {
            *out++ = '\\';
            *out++ = 'n';
            continue;
        } else if (*p == '\r') {
            *out++ = '\\';
            *out++ = 'r';
            continue;
        } else if (*p == '\t') {
            *out++ = '\\';
            *out++ = 't';
            continue;
        }
        *out++ = *p;
    }
    *out = '\0';

    return escaped;
}

char *json_error(const char *title, const char *detail)
{
    char *escaped_title = json_escape(title);
    char *escaped_detail = json_escape(detail);
    if (escaped_title == NULL || escaped_detail == NULL) {
        free(escaped_title);
        free(escaped_detail);
        return NULL;
    }

    int needed = snprintf(NULL, 0,
                          "{\"errors\":[{\"title\":\"%s\",\"detail\":\"%s\"}]}",
                          escaped_title,
                          escaped_detail);
    if (needed < 0) {
        free(escaped_title);
        free(escaped_detail);
        return NULL;
    }

    char *json = malloc((size_t)needed + 1);
    if (json != NULL) {
        snprintf(json,
                 (size_t)needed + 1,
                 "{\"errors\":[{\"title\":\"%s\",\"detail\":\"%s\"}]}",
                 escaped_title,
                 escaped_detail);
    }

    free(escaped_title);
    free(escaped_detail);
    return json;
}

char *json_internal_error(void)
{
    return json_error("Internal Server Error", "The server could not build a response");
}
