#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char *compute_get_request(char *host, char *url, char *query_params,
                          char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    else
        sprintf(line, "GET %s HTTP/1.1", url);

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add the token
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookie != NULL)
    {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // Step 4: add final new line
    strcat(message, "\r\n");

    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                           int body_data_fields_count, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer;

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    JSON_Value *value = json_value_init_object();

    for (int i = 0; i < body_data_fields_count; i += 2)
        json_object_set_string(json_object(value), body_data[i], body_data[i + 1]);

    body_data_buffer = json_serialize_to_string(value);

    long int size = strlen(body_data_buffer);

    sprintf(line, "Content-Length: %ld", size);
    compute_message(message, line);

    // add the token
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 4 (optional): add cookies
    if (cookie != NULL)
    {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    strcat(message, "\r\n");

    // Step 6: add the actual payload data

    strcat(message, body_data_buffer);

    json_value_free(value);
    free(line);
    free(body_data_buffer);
    return message;
}

char *compute_delete_request(char *host, char *url, char *content_type,
                             char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add the token
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 4 (optional): add cookies
    if (cookie != NULL)
    {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    strcat(message, "\r\n");

    free(line);
    return message;
}