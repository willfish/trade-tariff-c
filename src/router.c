#include "router.h"

#include "controllers/commodity_controller.h"
#include "json.h"

#include <stdlib.h>
#include <string.h>

static const char *service_from_env(void)
{
    const char *service = getenv("SERVICE");
    if (service == NULL || *service == '\0') {
        return "uk";
    }

    return service;
}

static int parse_commodity_path(const char *path, const char **service, const char **commodity_id)
{
    static const char api_prefix[] = "/api/commodities/";
    static const char uk_prefix[] = "/uk/api/commodities/";
    static const char xi_prefix[] = "/xi/api/commodities/";

    if (strncmp(path, uk_prefix, strlen(uk_prefix)) == 0) {
        *service = "uk";
        *commodity_id = path + strlen(uk_prefix);
        return 1;
    }

    if (strncmp(path, xi_prefix, strlen(xi_prefix)) == 0) {
        *service = "xi";
        *commodity_id = path + strlen(xi_prefix);
        return 1;
    }

    if (strncmp(path, api_prefix, strlen(api_prefix)) == 0) {
        *service = service_from_env();
        *commodity_id = path + strlen(api_prefix);
        return 1;
    }

    return 0;
}

HttpResponse route_request(const char *method, const char *path, const char *query)
{
    if (strcmp(method, "GET") != 0) {
        return (HttpResponse){ HTTP_METHOD_NOT_ALLOWED, json_error("Method Not Allowed", "Only GET is supported") };
    }

    if (strcmp(path, "/health") == 0) {
        char *body = malloc(16);
        if (body == NULL) {
            return (HttpResponse){ HTTP_INTERNAL_SERVER_ERROR, json_internal_error() };
        }
        strcpy(body, "{\"status\":\"ok\"}");
        return (HttpResponse){ HTTP_OK, body };
    }

    const char *service = NULL;
    const char *commodity_id = NULL;
    if (!parse_commodity_path(path, &service, &commodity_id)) {
        return (HttpResponse){ HTTP_NOT_FOUND, json_error("Not Found", "Route was not found") };
    }

    return commodity_controller_show(service, commodity_id, query);
}
