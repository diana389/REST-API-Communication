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

#define IP_SERVER "34.254.242.81"
#define PORT_SERVER 8080


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, "/api/v1/dummy", NULL, NULL, 0);
    send_to_server(sockfd, message);
    printf("%s", message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    close_connection(sockfd);
    printf("end task1\n");

    // Ex 1.2: POST dummy and print response from main server
    // sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    // char *form_data = "application/x-www-form-urlencoded";
    
    // char **data_buffer = malloc( 10 * sizeof(char *));
    // for(int i = 0; i < 10 ; i++ )
    //     data_buffer[i] = (char*) malloc( 100 *sizeof(char));
        
    // strcpy(data_buffer[0], "field1=value1&field2=value2");


    // message = compute_post_request(IP_SERVER, "/api/v1/dummy", form_data, data_buffer, 1, NULL, 0);
    // send_to_server(sockfd, message);
    // printf("%s", message);
    // printf("end message\n");
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // printf("end response\n");
    // close_connection(sockfd);


    // Ex 2: Login into main server
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    char *form_data = "application/x-www-form-urlencoded";
    
    char **data_buffer = malloc( 10 * sizeof(char *));
    for(int i = 0; i < 10 ; i++ )
        data_buffer[i] = (char*) malloc( 100 *sizeof(char));
        
    strcpy(data_buffer[0], "username=student&password=student");


    message = compute_post_request(IP_SERVER, "/api/v1/auth/login", form_data, data_buffer, 1, NULL, 0);
    send_to_server(sockfd, message);
    printf("%s", message);
    printf("end message\n");
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    printf("end response\n");
    close_connection(sockfd);
    // Ex 3: GET weather key from main server
    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
