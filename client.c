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
char **data_buffer;
char *cookie = NULL, *token = NULL, url[200];
int sockfd;
int data_buffer_size;

char buffer[100];

char username[100];
char password[100];
int id;

char title[100], author[100], genre[100], publisher[100];
char page_count_string[100];
int page_count;

/* extract cookie */
char *get_cookie(char *response, char *cookie)
{
    cookie = calloc(100, sizeof(char));

    char *p = strstr(response, "Set-Cookie:");
    p += 12;
    p = strtok(p, ";");

    strcpy(cookie, p);
    return cookie;
}

/* extract token */
char *get_token(char *response, char *token)
{
    token = calloc(1000, sizeof(char));

    char *p = strstr(response, "\"token\":");
    p += 9;
    p[strlen(p) - 2] = '\0';

    strcpy(token, p);
    return token;
}

void free_memory()
{
    free(message);
    free(response);

    if (data_buffer != NULL)
    {
        for (int i = 0; i < data_buffer_size; i++)
            free(data_buffer[i]);
        free(data_buffer);

        data_buffer = NULL;
    }
}

void reg()
{
    char *data_type = "application/json";
    strcpy(url, "/api/v1/tema/auth/register");

    // complete the data buffer with the necessary data

    data_buffer_size = 4;

    data_buffer = malloc(data_buffer_size * sizeof(char *));
    for (int i = 0; i < data_buffer_size; i++)
        data_buffer[i] = malloc(200 * sizeof(char));

    strcpy(data_buffer[0], "username");
    strcpy(data_buffer[1], username);
    strcpy(data_buffer[2], "password");
    strcpy(data_buffer[3], password);

    // send the request and receive the response

    message = compute_post_request(IP_SERVER, url, data_type, data_buffer, data_buffer_size, NULL, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! The username is taken!\n\n");
        free_memory();
        return;
    }

    printf("\n201 - Created - User registered successfully!\n\n");
    free_memory();
}

void login()
{
    char *data_type = "application/json";
    strcpy(url, "/api/v1/tema/auth/login");

    // complete the data buffer with the necessary data

    data_buffer_size = 4;

    data_buffer = malloc(data_buffer_size * sizeof(char *));
    for (int i = 0; i < data_buffer_size; i++)
        data_buffer[i] = malloc(200 * sizeof(char));

    strcpy(data_buffer[0], "username");
    strcpy(data_buffer[1], username);
    strcpy(data_buffer[2], "password");
    strcpy(data_buffer[3], password);

    // send the request and receive the response

    message = compute_post_request(IP_SERVER, url, data_type, data_buffer, 4, NULL, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! The credentials do not match!\n\n");
        free_memory();
        return;
    }

    // get cookie

    if (cookie == NULL)
        cookie = get_cookie(response, cookie);

    printf("\n200 - OK - Welcome!\n\n");
    free_memory();
}

void enter_library()
{
    strcpy(url, "/api/v1/tema/library/access");

    // send the request and receive the response

    message = compute_get_request(IP_SERVER, url, NULL, cookie, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 401 Unauthorized", 25) == 0)
    {
        printf("\n!!! You are not logged in!\n\n");
        free_memory();
        return;
    }

    // get token

    if (token == NULL)
        token = get_token(response, token);

    printf("\n200 - OK - User entered library!\n\n");
    free_memory();
}

void get_books()
{
    strcpy(url, "/api/v1/tema/library/books");

    // send the request and receive the response

    message = compute_get_request(IP_SERVER, url, NULL, NULL, token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n\n");
        free_memory();
        return;
    }

    // print the books

    if (basic_extract_json_response(response) != NULL)
        printf("\nBOOKS: [%s\n\n", basic_extract_json_response(response));
    else
        printf("\nBOOKS: -\n\n");

    free_memory();
}

void get_book()
{
    // form url

    strcpy(url, "/api/v1/tema/library/books/");
    char id_string[50];
    sprintf(id_string, "%d", id);
    strcat(url, id_string);

    // send the request and receive the response

    message = compute_get_request(IP_SERVER, url, NULL, NULL, token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n\n");
        free_memory();
        return;
    }

    if (strncmp(response, "HTTP/1.1 404 Not Found", 22) == 0)
    {
        printf("\n!!! Invalid ID!\n\n");
        free_memory();
        return;
    }

    // print the book

    printf("\n%s\n\n", basic_extract_json_response(response));
    free_memory();
}

void add_book()
{
    char *data_type = "application/json";
    strcpy(url, "/api/v1/tema/library/books");

    // complete the data buffer with the necessary data

    data_buffer_size = 10;

    data_buffer = malloc(data_buffer_size * sizeof(char *));
    for (int i = 0; i < data_buffer_size; i++)
        data_buffer[i] = malloc(200 * sizeof(char));

    strcpy(data_buffer[0], "title");
    strcpy(data_buffer[1], title);
    strcpy(data_buffer[2], "author");
    strcpy(data_buffer[3], author);
    strcpy(data_buffer[4], "genre");
    strcpy(data_buffer[5], genre);
    strcpy(data_buffer[6], "page_count");
    strcpy(data_buffer[7], page_count_string);
    strcpy(data_buffer[8], "publisher");
    strcpy(data_buffer[9], publisher);

    // send the request and receive the response

    message = compute_post_request(IP_SERVER, url, data_type, data_buffer, 10, NULL, token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n\n");
        free_memory();
        return;
    }

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! Bad format!\n\n");
        free_memory();
        return;
    }

    printf("\n200 - OK - Book added!\n\n");
    free_memory();
}

void delete_book()
{
    char *data_type = "application/json";

    // form url 

    strcpy(url, "/api/v1/tema/library/books/");
    char id_string[50];
    sprintf(id_string, "%d", id);
    strcat(url, id_string);

    // send the request and receive the response

    message = compute_delete_request(IP_SERVER, url, data_type, NULL, token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // check the response

    if (strncmp(response, "HTTP/1.1 403 Forbidden", 22) == 0)
    {
        printf("\n!!! You do not have access to library!\n\n");
        free_memory();
        return;
    }

    if (strncmp(response, "HTTP/1.1 404 Not Found", 22) == 0)
    {
        printf("\n!!! Invalid ID!\n\n");
        free_memory();
        return;
    }

    printf("\n200 - OK - Book deleted!\n\n");
    free_memory();
}

void logout()
{
    strcpy(url, "/api/v1/tema/auth/logout");

    // send the request and receive the response

    message = compute_get_request(IP_SERVER, url, NULL, cookie, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // remove cookie

    if (cookie != NULL)
    {
        free(cookie);
        cookie = NULL;
    }

    // remove token

    if (token != NULL)
    {
        free(token);
        token = NULL;
    }

    // check the response

    if (strncmp(response, "HTTP/1.1 400 Bad Request", 24) == 0)
    {
        printf("\n!!! You are not logged in!\n\n");
        free_memory();
        return;
    }

    printf("\n200 - OK - Goodbye!\n\n");
    free_memory();
}

int main(int argc, char *argv[])
{
    while (fgets(buffer, 100, stdin))
    {
        if (strcmp(buffer, "exit\n") == 0)
            return 0;

        // open connection
        sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

        if (strcmp(buffer, "register\n") == 0)
        {
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            reg();
            continue;
        }

        if (strcmp(buffer, "login\n") == 0)
        {
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            login();
            continue;
        }

        if (strcmp(buffer, "enter_library\n") == 0)
        {
            enter_library();
            continue;
        }

        if (strcmp(buffer, "get_books\n") == 0)
        {
            get_books();
            continue;
        }

        if (strcmp(buffer, "get_book\n") == 0)
        {
            printf("id=");
            scanf("%d", &id);

            get_book();
            continue;
        }

        if (strcmp(buffer, "add_book\n") == 0)
        {
            // read book data

            printf("title=");
            fgets(title, 100, stdin);
            title[strlen(title) - 1] = '\0';

            printf("author=");
            fgets(author, 100, stdin);
            author[strlen(author) - 1] = '\0';

            printf("genre=");
            fgets(genre, 100, stdin);
            genre[strlen(genre) - 1] = '\0';

            printf("publisher=");
            fgets(publisher, 100, stdin);
            publisher[strlen(publisher) - 1] = '\0';

            printf("page_count=");

            // check page count
            if (scanf("%d", &page_count) <= 0)
            {
                printf("\n!!! Invalid data!\n\n");
                continue;
            }

            sprintf(page_count_string, "%d", page_count);

            add_book();
            continue;
        }

        if (strcmp(buffer, "delete_book\n") == 0)
        {
            printf("id=");
            scanf("%d", &id);

            delete_book();
            continue;
        }

        if (strcmp(buffer, "logout\n") == 0)
        {
            logout();
            continue;
        }

        // close connection
        close_connection(sockfd);
    }
}
