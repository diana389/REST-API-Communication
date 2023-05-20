#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

/* info about server*/
#define IP_SERVER "34.254.242.81"
#define PORT_SERVER 8080

char *message, *response;
char *cookie = NULL, *token = NULL, *url;
int sockfd;

char buffer[100];

char username[100];
char password[100];
int id;

char title[100], author[100], genre[100], publisher[100];
int page_count;

/* extract cookie */
char *extract_cookie(char *response, char *cookie)
{
    cookie = strstr(response, "Set-Cookie:");
    cookie += 12;

    cookie = strtok(cookie, ";");

    // printf("\nCOOKIE: %s\n", cookie);
    return cookie;
}

/* extract token */
char *extract_token(char *response, char *token)
{
    token = strstr(response, "\"token\":");
    token += 9;
    token[strlen(token) - 2] = '\0';

    // printf("\nTOKEN : %s\n", token);
    return token;
}

void reg()
{
    /* set data type */
    char *form_data = "application/json";
    /* set username & password */
    char **data_buffer = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++)
        data_buffer[i] = malloc(200 * sizeof(char));

    strcpy(data_buffer[0], "{\"username\": \"dianaaaa\", \"password\": \"pass\"}");

    url = "/api/v1/tema/auth/register";

    /* compute message */
    message = compute_post_request(IP_SERVER, url, form_data, data_buffer, 1, NULL, 0, NULL);
    /* send message */
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    char *line = strtok(response, "\n");
    if (strcmp(line, "HTTP/1.1 400 Bad Request") == 0)
        printf("\n!!! The username is taken!\n");

    printf("\n!!! 201 - Created - User registered successfully!\n");
}

void login()
{
    /* set data type */
    char *form_data = "application/json";
    /* set username & password */
    char **data_buffer = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++)
        data_buffer[i] = malloc(200 * sizeof(char));

    strcpy(data_buffer[0], "{\"username\": \"dianaa\", \"password\": \"pass\"}");
    url = "/api/v1/tema/auth/login";

    /* compute message */
    message = compute_post_request(IP_SERVER, url, form_data, data_buffer, 1, NULL, 0, NULL);
    /* send message */
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! The credentials do not match!\n");
        return;
    }

    printf("200 - OK - Welcome!");

    /* extract cookie */
    cookie = malloc(200 * sizeof(char));
    cookie = extract_cookie(response, cookie);
}

void enter_library()
{
    url = "/api/v1/tema/library/access";
    message = compute_get_request(IP_SERVER, url, NULL, &cookie, 1, NULL);
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 401 Unauthorized", 25) == 0)
    {
        printf("\n!!! You are not logged in!\n");
        return;
    }

    printf("200 - OK - User entered library!");

    /* extract token */
    token = malloc(1000 * sizeof(char));
    memset(token, 0, 1000);
    token = extract_token(response, token);

    // printf("\nTOKEN : %s\n", token);
}

void get_books()
{
    // printf("\nTOKEN : %s\n", token);

    url = "/api/v1/tema/library/books";

    message = compute_get_request(IP_SERVER, url, NULL, NULL, 0, token);
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n");
        return;
    }

    printf("\n%s\n", basic_extract_json_response(response));
}

void get_book()
{
    // printf("\nTOKEN : %s\n", token);

    url = malloc(200 * sizeof(char));
    strcpy(url, "/api/v1/tema/library/books/");

    char id[50];
    sprintf(id, "%d", 9997);

    strcat(url, id);

    message = compute_get_request(IP_SERVER, url, NULL, NULL, 0, token);
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n");
        return;
    }

    if (strncmp(response, "HTTP/1.1 404 Not Found", 22) == 0)
    {
        printf("\n!!! Invalid ID!\n");
        return;
    }

    printf("\n%s\n", basic_extract_json_response(response));
}

void add_book()
{
    /* set data type */
    char *form_data = "application/json";
    /* set username & password */
    char **data_buffer = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++)
        data_buffer[i] = malloc(500 * sizeof(char));

    JSON_Value *value = json_value_init_object();
    json_object_set_string(json_object(value), "title", "Ion");
    json_object_set_string(json_object(value), "author", "Liviu");
    json_object_set_string(json_object(value), "genre", "Comedie");
    json_object_set_number(json_object(value), "page_count", 95);
    json_object_set_string(json_object(value), "publisher", "Humanitas");
    data_buffer[0] = json_serialize_to_string(value);

    url = "/api/v1/tema/library/books";

    /* compute message */
    message = compute_post_request(IP_SERVER, url, form_data, data_buffer, 1, NULL, 0, token);
    /* send message */
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n");
        return;
    }

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! Bad format!\n");
        return;
    }

    printf("200 - OK - Book added!");
}

void delete_book()
{
    /* set data type */
    char *form_data = "application/json";
    /* set username & password */
    char **data_buffer = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++)
        data_buffer[i] = malloc(500 * sizeof(char));

    url = malloc(200 * sizeof(char));
    strcpy(url, "/api/v1/tema/library/books/");

    char id[50];
    sprintf(id, "%d", 9942);

    strcat(url, id);

    /* compute message */
    message = compute_delete_request(IP_SERVER, url, form_data, data_buffer, 1, NULL, 0, token);
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n");
        return;
    }

    if (strncmp(response, "HTTP/1.1 404 Not Found", 22) == 0)
    {
        printf("\n!!! Invalid ID!\n");
        return;
    }

    printf("200 - OK - Book deleted!");
}

void logout()
{
    url = "/api/v1/tema/auth/logout";
    message = compute_get_request(IP_SERVER, url, NULL, &cookie, 1, NULL);
    send_to_server(sockfd, message);
    // printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    // printf("Response:\n%s", response);

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! You are not logged in!\n");
        return;
    }

    printf("200 - OK - Goodbye!");
}

int main(int argc, char *argv[])
{

    while (fgets(buffer, 100, stdin))
    {
        if (strcmp(buffer, "exit\n") == 0)
            return 0;

        /* open connection */
        sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

        if (strcmp(buffer, "register\n") == 0)
        {
            printf("\n-------- START REGISTER --------\n");

            reg();

            printf("\n-------- END REGISTER ----------\n");
        }

        if (strcmp(buffer, "login\n") == 0)
        {
            printf("\n-------- START LOGIN --------\n");

            login();

            printf("\n-------- END LOGIN ----------\n");
        }

        if (strcmp(buffer, "enter_library\n") == 0)
        {
            printf("\n-------- ENTER_LIBRARY --------\n");

            enter_library();

            printf("\n-------- END ENTER_LIBRARY ----------\n");
        }

        if (strcmp(buffer, "get_books\n") == 0)
        {
            printf("\n-------- GET_BOOKS --------\n");

            get_books();

            printf("\n-------- END GET_BOOKS ----------\n");
        }

        if (strcmp(buffer, "get_book\n") == 0)
        {
            printf("\n-------- GET_BOOK --------\n");

            get_book();

            printf("\n-------- END GET_BOOK ----------\n");
        }

        if (strcmp(buffer, "add_book\n") == 0)
        {
            printf("\n-------- START ADD_BOOK --------\n");

            add_book();

            printf("\n-------- END ADD_BOOK ----------\n");
        }

        if (strcmp(buffer, "delete_book\n") == 0)
        {
            printf("\n-------- DELETE_BOOK --------\n");

            delete_book();

            printf("\n-------- END DELETE_BOOK ----------\n");
        }

        if (strcmp(buffer, "logout\n") == 0)
        {
            printf("\n-------- LOGOUT --------\n");

            logout();

            printf("\n-------- END LOGOUT ----------\n");
        }

        close_connection(sockfd);
    }
}
