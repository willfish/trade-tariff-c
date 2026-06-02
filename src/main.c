#include "controllers/commodity_controller.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int port_from_env(void)
{
    const char *port = getenv("PORT");
    if (port == NULL || *port == '\0') {
        return 8080;
    }

    return atoi(port);
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) {
        return commodity_controller_self_test() ? 0 : 1;
    }

    int port = port_from_env();
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "PORT must be between 1 and 65535\n");
        return 1;
    }

    return serve_http(port);
}
