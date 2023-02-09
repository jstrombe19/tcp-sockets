#include "tcp-socket.hpp"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int open_socket(int _domain, int _type, int _protocol, int* sockfd, int port, struct sockaddr_in* serv_addr) {
    int domain, type, protocol;
    if (_domain) {
        domain = _domain;
    } else {
        domain = AF_INET;
    }
    if (_type) {
        type = _type;
    } else {
        type = SOCK_STREAM;
    }
    if (_protocol) {
        protocol = _protocol;
    } else {
        protocol = 0;
    }

    *sockfd = socket(domain, type, protocol);
    if (sockfd < 0) {
        error("ERROR opening socket failed");
        return -1;
    }

    bzero((char *) serv_addr, sizeof(*serv_addr));
    serv_addr->sin_family = domain;
    serv_addr->sin_addr.s_addr = INADDR_ANY;
    serv_addr->sin_port = htons(port);

    if (bind(*sockfd, (struct sockaddr *) serv_addr, sizeof(*serv_addr)) < 0) {
        error("ERROR binding socket failed");
        return -1;
    } else {
        fprintf(stdout, "%d", serv_addr->sin_addr.s_addr);
        return 0;
    }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    portno = atoi(argv[1]);
    if (open_socket(0, 0, 0, &sockfd, portno, &serv_addr) != 0) {
        error("ERROR failed to open socket");
    }
    
    while (1) {
        listen(sockfd,MAX_NUM_CONNECTIONS);
        clilen = sizeof(cli_addr);
        newsockfd = accept(
                        sockfd, 
                        (struct sockaddr *) &cli_addr, 
                        &clilen
                    );
        if (newsockfd < 0) 
            error("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) {
            error("ERROR reading from socket");
        } else {
            int n_prime = 19 + sizeof(buffer);
            printf("Here is the message: %s\n",buffer);

            char response_buf[276];
            sprintf(response_buf, "I got your message: %s", buffer);
            n = write(newsockfd,response_buf, n_prime);
            if (n < 0) error("ERROR writing to socket");
        }
        close(newsockfd);
    }
    close(sockfd);
    return 0; 
}