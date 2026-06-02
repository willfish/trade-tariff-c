#include "http.h"

#include "common.h"
#include "router.h"

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define REQUEST_BUFFER_SIZE 8192

static const char *reason_phrase(HttpStatus status)
{
    switch (status) {
    case HTTP_OK:
        return "OK";
    case HTTP_BAD_REQUEST:
        return "Bad Request";
    case HTTP_NOT_FOUND:
        return "Not Found";
    case HTTP_METHOD_NOT_ALLOWED:
        return "Method Not Allowed";
    case HTTP_INTERNAL_SERVER_ERROR:
        return "Internal Server Error";
    default:
        return "Error";
    }
}

void free_response(HttpResponse *response)
{
    free(response->body);
    response->body = NULL;
}

static void send_response(int client_fd, HttpResponse *response)
{
    const char *body = response->body == NULL ? "" : response->body;
    size_t body_len = strlen(body);
    char header[512];

    int header_len = snprintf(header,
                              sizeof(header),
                              "HTTP/1.1 %d %s\r\n"
                              "Content-Type: application/vnd.api+json\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: close\r\n"
                              "\r\n",
                              response->status,
                              reason_phrase(response->status),
                              body_len);

    if (header_len > 0) {
        const char *chunks[] = { header, body };
        const size_t lengths[] = { (size_t)header_len, body_len };

        for (size_t i = 0; i < 2; i++) {
            size_t written = 0;
            while (written < lengths[i]) {
                ssize_t count = write(client_fd, chunks[i] + written, lengths[i] - written);
                if (count <= 0) {
                    return;
                }
                written += (size_t)count;
            }
        }
    }
}

static void handle_client(int client_fd)
{
    char buffer[REQUEST_BUFFER_SIZE];
    ssize_t read_count = read(client_fd, buffer, sizeof(buffer) - 1);
    if (read_count <= 0) {
        return;
    }

    buffer[read_count] = '\0';

    char method[16] = {0};
    char path[2048] = {0};
    if (sscanf(buffer, "%15s %2047s", method, path) != 2) {
        HttpResponse response = { HTTP_BAD_REQUEST, strdup("{\"errors\":[{\"title\":\"Bad Request\",\"detail\":\"Invalid request line\"}]}") };
        send_response(client_fd, &response);
        free_response(&response);
        return;
    }

    char *query = strchr(path, '?');
    if (query != NULL) {
        *query = '\0';
        query++;
    }

    HttpResponse response = route_request(method, path, query);
    send_response(client_fd, &response);
    free_response(&response);
}

int serve_http(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) != 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 32) != 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    fprintf(stderr, "trade-tariff-c listening on port %d\n", port);

    for (;;) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            close(server_fd);
            return 1;
        }

        handle_client(client_fd);
        close(client_fd);
    }
}
