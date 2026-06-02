#include "renderers/jsonapi_attribute.h"

#include <errno.h>
#include <stdlib.h>

int jsonapi_add_string_or_null(yyjson_mut_doc *doc,
                               yyjson_mut_val *obj,
                               const char *key,
                               const char *value)
{
    if (doc == NULL || obj == NULL || key == NULL) {
        return 0;
    }

    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
}

int jsonapi_add_number_string_or_null(yyjson_mut_doc *doc,
                                      yyjson_mut_val *obj,
                                      const char *key,
                                      const char *value)
{
    if (doc == NULL || obj == NULL || key == NULL) {
        return 0;
    }

    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }

    char *end = NULL;
    errno = 0;
    double number = strtod(value, &end);
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
    }
    return yyjson_mut_obj_add_real(doc, obj, key, number);
}
