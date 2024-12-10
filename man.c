#include "specific_commands.h"
#include "headers.h"
#include "functions.h"
#include "input.h"

extern char init_home[];
extern char prev_dir[BUFFER_SIZE];
extern Process* bg_process[BUFFER_SIZE];
extern int num_background_processes;
extern Process* current_fg_process;


void remove_html_tags(const char *input) {
    int in_tag = 0;
    const char *ptr = input;

    while (*ptr) {
        if (*ptr == '<') {
            in_tag = 1; // Entering an HTML tag
        } else if (*ptr == '>') {
            in_tag = 0; // Exiting an HTML tag
        } else if (!in_tag) {
            putchar(*ptr); // Print character if not in a tag
        }
        ptr++;
    }
}

void fetch_man_page(const char *command) {
    // printf("command in fetch_man_page: %s\n", command);
    const char *hostname = "man.he.net";
    const char *path_prefix = "/man1/";
    int port = 80;
    
    // Create the GET request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
             "GET %s%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
             path_prefix, command, hostname);
    
    // Create socket
    int sockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR: No such host\n");
        return;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return;
    }

    // Prepare the server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        close(sockfd);
        return;
    }
    // printf("Connected to %s\n", hostname);

    // Send the request
    if (write(sockfd, request, strlen(request)) < 0) {
        perror("ERROR writing to socket");
        close(sockfd);
        return;
    }
    // printf("Sent request:%s", request);
    // printf("request end\n");

    // Read the response
    char buffer[BUFFER_SIZE];
    int n;
    int header_found = 0;
    char* header_end;

    while ((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';

        // Find the end of the HTTP headers
        if (!header_found) {
            // printf("buffer: %s\n", buffer);
            remove_html_tags(buffer);
            header_end = strstr(buffer, "\r\n\r\n");
            if (header_end) {
                header_found = 1;
                remove_html_tags(header_end + 4);
            }
        } else {
            remove_html_tags(buffer);
        }
    }

    if (n < 0) {
        perror("ERROR reading from socket");
    }
    else if(n==0){
        printf("// expected output ends\n");
    }

    // Close the socket
    close(sockfd);
}
