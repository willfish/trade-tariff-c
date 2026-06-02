#include "controllers/commodity_controller.h"

#include "json.h"
#include "models/commodity.h"
#include "request/commodity_include_plan.h"
#include "services/commodity_service.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char *actual_date(void)
{
    const char *value = getenv("ACTUAL_DATE");
    if (value != NULL && *value != '\0') {
        return value;
    }

    return NULL;
}

static int copy_query_value(const char *value,
                            size_t value_len,
                            char *out,
                            size_t out_len)
{
    if (out == NULL || out_len == 0) {
        return 1;
    }

    size_t copy_len = value_len >= out_len ? out_len - 1 : value_len;
    memcpy(out, value, copy_len);
    out[copy_len] = '\0';
    return value_len < out_len;
}

static int query_key_equals(const char *key,
                            size_t key_len,
                            const char *plain,
                            const char *encoded)
{
    return (strlen(plain) == key_len && strncmp(key, plain, key_len) == 0) ||
           (strlen(encoded) == key_len && strncmp(key, encoded, key_len) == 0);
}

static int extract_query_params(const char *query,
                                char *include_value,
                                size_t include_value_len,
                                int *has_include,
                                char *meursing_value,
                                size_t meursing_value_len,
                                int *has_meursing,
                                char *error,
                                size_t error_len)
{
    *has_include = 0;
    *has_meursing = 0;
    if (include_value != NULL && include_value_len > 0) {
        include_value[0] = '\0';
    }
    if (meursing_value != NULL && meursing_value_len > 0) {
        meursing_value[0] = '\0';
    }
    if (query == NULL || *query == '\0') {
        return 1;
    }

    const char *cursor = query;
    while (*cursor != '\0') {
        const char *pair_end = strchr(cursor, '&');
        size_t pair_len = pair_end == NULL ? strlen(cursor) : (size_t)(pair_end - cursor);
        const char *equals = memchr(cursor, '=', pair_len);
        size_t key_len = equals == NULL ? pair_len : (size_t)(equals - cursor);

        if (key_len == strlen("include") && strncmp(cursor, "include", key_len) == 0) {
            if (*has_include) {
                snprintf(error, error_len, "Duplicate include query parameter");
                return 0;
            }

            *has_include = 1;
            if (equals != NULL && include_value != NULL && include_value_len > 0) {
                size_t value_len = pair_len - key_len - 1;
                if (!copy_query_value(equals + 1, value_len, include_value, include_value_len)) {
                    snprintf(error, error_len, "include query parameter is too long");
                    return 0;
                }
            }
        } else if (query_key_equals(cursor,
                                    key_len,
                                    "filter[meursing_additional_code_id]",
                                    "filter%5Bmeursing_additional_code_id%5D")) {
            if (*has_meursing) {
                snprintf(error, error_len, "Duplicate meursing_additional_code_id query parameter");
                return 0;
            }

            *has_meursing = 1;
            if (equals != NULL && meursing_value != NULL && meursing_value_len > 0) {
                size_t value_len = pair_len - key_len - 1;
                if (!copy_query_value(equals + 1, value_len, meursing_value, meursing_value_len)) {
                    snprintf(error, error_len, "meursing_additional_code_id query parameter is too long");
                    return 0;
                }
            }
        } else {
            if (error != NULL && error_len > 0) {
                snprintf(error, error_len, "Unsupported query parameter: %.*s", (int)key_len, cursor);
            }
            return 0;
        }

        if (pair_end == NULL) {
            break;
        }
        cursor = pair_end + 1;
    }

    return 1;
}

static HttpResponse include_query_error(const char *detail)
{
    return (HttpResponse){ HTTP_BAD_REQUEST, json_error("Bad Request", detail) };
}

static int apply_query_params(CommodityRequest *request,
                              const char *query,
                              char *meursing_value,
                              size_t meursing_value_len,
                              char *error,
                              size_t error_len)
{
    char include_value[2048];
    int has_include = 0;
    int has_meursing = 0;
    if (!extract_query_params(query,
                              include_value,
                              sizeof(include_value),
                              &has_include,
                              meursing_value,
                              meursing_value_len,
                              &has_meursing,
                              error,
                              error_len)) {
        return 0;
    }
    if (has_meursing) {
        request->meursing_additional_code_id = meursing_value;
    }

    if (has_include) {
        char invalid[128];
        CommodityIncludePlan requested_plan = commodity_include_plan_empty();
        CommodityIncludeParseResult result = commodity_include_plan_parse(include_value,
                                                                          &requested_plan,
                                                                          invalid,
                                                                          sizeof(invalid));
        if (result != COMMODITY_INCLUDE_PARSE_OK) {
            snprintf(error, error_len, "Unsupported include path: %s", invalid);
            return 0;
        }

        request->include_plan = requested_plan;
    }

    return 1;
}

HttpResponse commodity_controller_show(const char *service, const char *commodity_id, const char *query)
{
    CommodityRequest request = commodity_request_new(service, commodity_id);
    request.actual_date = actual_date();
    CommodityRequestValidation validation = commodity_request_validate(&request);
    if (validation != COMMODITY_REQUEST_VALID) {
        return (HttpResponse){ HTTP_BAD_REQUEST, json_error("Bad Request", commodity_request_validation_message(validation)) };
    }

    char query_error[160];
    char meursing_value[64];
    if (!apply_query_params(&request,
                            query,
                            meursing_value,
                            sizeof(meursing_value),
                            query_error,
                            sizeof(query_error))) {
        return include_query_error(query_error);
    }

    return commodity_service_show_json(&request);
}

static int commodity_controller_query_self_test(void)
{
    char error[160];
    char meursing_value[64];
    CommodityRequest request = commodity_request_new("uk", "0101210000");
    if (!apply_query_params(&request,
                            "include=section,import_measures.measure_components&filter%5Bmeursing_additional_code_id%5D=123",
                            meursing_value,
                            sizeof(meursing_value),
                            error,
                            sizeof(error)) ||
        !commodity_include_plan_enabled(&request.include_plan, COMMODITY_INCLUDE_SECTION) ||
        !commodity_include_plan_enabled(&request.include_plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS) ||
        commodity_include_plan_enabled(&request.include_plan, COMMODITY_INCLUDE_CHAPTER) ||
        request.meursing_additional_code_id == NULL ||
        strcmp(request.meursing_additional_code_id, "123") != 0) {
        return 0;
    }

    request = commodity_request_new("uk", "0101210000");
    if (apply_query_params(&request,
                           "include=section,unknown",
                           meursing_value,
                           sizeof(meursing_value),
                           error,
                           sizeof(error)) ||
        strcmp(error, "Unsupported include path: unknown") != 0) {
        return 0;
    }

    request = commodity_request_new("uk", "0101210000");
    return !apply_query_params(&request,
                               "foo=bar",
                               meursing_value,
                               sizeof(meursing_value),
                               error,
                               sizeof(error)) &&
           strcmp(error, "Unsupported query parameter: foo") == 0;
}

int commodity_controller_self_test(void)
{
    return commodity_controller_query_self_test() &&
           commodity_service_self_test();
}
