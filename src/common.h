#ifndef TRADE_TARIFF_C_COMMON_H
#define TRADE_TARIFF_C_COMMON_H

#include <stddef.h>

#define MAX_SERVICE_LEN 2
#define MAX_COMMODITY_ID_LEN 10

typedef enum {
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_INTERNAL_SERVER_ERROR = 500
} HttpStatus;

typedef struct {
    HttpStatus status;
    char *body;
} HttpResponse;

void free_response(HttpResponse *response);

#endif
