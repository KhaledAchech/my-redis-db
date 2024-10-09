#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>


void die(const char *msg) {
    perror(msg);  // Print the error message for the last system call
    exit(EXIT_FAILURE);  // Exit the program with failure
}

static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n<0) {
        die("read() error");
        return;
    }

    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}

int main(){
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket creation failed");
    }

    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        die("setsockopt(SO_REUSEADDR) failed");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0
    
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("bind() failed");
    }

    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen() failed");
    }

    while (true) {
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);

        if (connfd < 0) {
            continue; //error
        }
        do_something(connfd);   
        close(connfd);
    }

    close(fd);
    return 0;
}

