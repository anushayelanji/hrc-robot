// server.c

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <math.h>
#include <pthread.h>

#define PORT 9999
#define LISTENQ 5
#define MAXLINE 80

// -------------- Wrapper Functions ---------------------

ssize_t
read1(int fd, char* buf, size_t nbytes)
{
    ssize_t res;
    res = read(fd, (void *) buf, nbytes);
    // if (DEBUG)
    //     printf("C> %s\n", buf);
    return res;
}

ssize_t
write1(int fd, char* buf, size_t n)
{
    // if (DEBUG)
    //     printf("S> %s\n", buf);
    return write(fd, (void *) buf, n);
}

FILE*
fopen1(char* path, char* modes)
{
    char lockpath[100];
    memset(lockpath, 0, 100);
    strcpy(lockpath, path);
    strcat(lockpath, ".lock");
    FILE* f;
    f = fopen(lockpath, "w");
    FILE* res = fopen(path, modes);
    fclose(f);
    return res;
}

int
fclose1(FILE* fp, char* path)
{
    char lockpath[100];
    memset(lockpath, 0, 100);
    strcpy(lockpath, path);
    strcat(lockpath, ".lock");
    unlink(lockpath);
    return fclose(fp);
}

// ------------------------------------------------------


// ---------------- Socket ------------------------------

int
open_listenfd(struct sockaddr* address)
{
    int client_socket, server_socket;
    int opt = 1;
    int addrlen = sizeof(address);
    // Creating socket file descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int socket_status = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt));
    if (socket_status) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    int bind_status = bind(server_socket, address, sizeof(*address));
    if (bind_status < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int listen_status = listen(server_socket, LISTENQ);
    if (listen_status < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

// ------------------------------------------------------


// ------------------ Server Functions ------------------

int
is_locked(char* filepath)
{
    char lockpath[100];
    memset(lockpath, 0, 100);
    strcpy(lockpath, filepath);
    strcat(lockpath, ".lock");
    if (access(lockpath, R_OK) == -1)
        return 0;
    else
        return 1;
}

void
append(int connfd)
{
    printf("1");
}

void
download(int connfd)
{
    printf("1");
    return;
}

void
upload(int connfd)
{
    printf("1");

    return;
}

void
delete(int connfd)
{
    printf("1");
    return;
}

void
syncheck(int connfd)
{
    printf("1");

    
    return;
}

/*
void
*thread(void *vargp)
{
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());

    char line[MAXLINE];
    while(1)
    {
        memset(line, 0, MAXLINE);
        int res = read1(connfd, line, MAXLINE);
        if (res == 0)
            break;
        else if (strcmp(line, "append") == 0)
            append(connfd);
        else if (strcmp(line, "upload") == 0)
            download(connfd);
        else if (strcmp(line, "download") == 0)
            upload(connfd);
        else if (strcmp(line, "delete") == 0)
            delete(connfd);
        else if (strcmp(line, "syncheck") == 0)
            syncheck(connfd);
    }
    close(connfd);
    free(vargp);
}
*/
// ------------------------------------------------------


int main(int argc, char *argv[])
{
    //char *host = argv[1];
    int valread;
    char buffer[1024] = { 0 };

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int listenfd;
    socklen_t addrlen;
    addrlen = sizeof(address);
    
    listenfd = open_listenfd((struct sockaddr*)&address);

    chdir("Remote Directory");

    //while (1)
    //{
        int connfd;
        if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    
    //}
    valread = read(connfd, buffer, 1024);
    printf("%s\n", buffer);
	exit(EXIT_SUCCESS);
    
}