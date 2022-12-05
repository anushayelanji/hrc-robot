// server.cpp
//g++ -g `pkg-config --cflags opencv4` server.cpp `pkg-config --libs opencv4`

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


#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/opencv.hpp>

#include <opencv2/imgcodecs.hpp>
#include "opencv2/highgui.hpp"


#define PORT 9999
#define LISTENQ 5
using namespace cv;


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



int main(int argc, char *argv[])
{
    int valread;
    int connfd;
    char buffer[1080] = { 0 };

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int listenfd;
    socklen_t addrlen;
    addrlen = sizeof(address);
    
    listenfd = open_listenfd((struct sockaddr*)&address);


    while (1)
    {
        
        if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }else{

            //used to check client server connection - prints "Hello from Client"
            valread = recv(connfd, buffer, 1080, 0);
            printf("%s\n", buffer);

            //some attempts
            /*
            cv::Mat img = cv::Mat::zeros(1080, 1920, CV_8UC4);
            int imgSize = img.total()*img.elemSize(); //8294400
            uchar sockData[imgSize];

            int bytes = 0;
        
            for (int i = 0; i < imgSize; i += bytes) {
                if ((bytes = recv(connfd, sockData +i, imgSize  - i, 0)) == -1)  {
                    exit(1);
                }
            } 

            int ptr=0;
            for (int i = 0;  i < img.rows; i++) {
                for (int j = 0; j < img.cols; j++) {                                     
                    img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
                    ptr=ptr+3;
                }
            }

            cv::Mat other(1080, 1920, CV_8UC4, sockData);
            imshow("other", img);

            */
            
         
            
        }

        close(connfd);
    
    }
    
    
    exit(EXIT_SUCCESS);
        
    
}