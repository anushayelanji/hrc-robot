// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
//#define PORT 8080

#include <assert.h>
#include <array>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <vector>
#include <ctime>
#include <unistd.h>


#include <k4a/k4a.hpp>
#include <k4abt.hpp>
#include <k4arecord/record.hpp>
#include <k4a/k4a.h>
#include "k4atypes.h"


#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/opencv.hpp>

#include <opencv2/imgcodecs.hpp>
#include "opencv2/highgui.hpp"
#include "MultiDeviceCapturer.h"

//#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <string.h>
#include <math.h>

#define MAXLINE    80
#define PORT       9999

int cfd;

using namespace cv;
using namespace std;

using std::cerr;
using std::cout;
using std::endl;
using std::vector;

#define VERIFY(result, error)                                                                            \
    if(result != K4A_RESULT_SUCCEEDED)                                                                   \
    {                                                                                                    \
        printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__); \
        exit(1);                                                                                         \
    }                                                                                                    \

#define FOURCC(cc) ((cc)[0] | (cc)[1] << 8 | (cc)[2] << 16 | (cc)[3] << 24)

// Codec context struct for Codec ID: "V_MS/VFW/FOURCC"
// See https://docs.microsoft.com/en-us/windows/desktop/wmdm/-bitmapinfoheader
typedef struct
{
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

void fill_bitmap_header(uint32_t width, uint32_t height, BITMAPINFOHEADER *out);
void fill_bitmap_header(uint32_t width, uint32_t height, BITMAPINFOHEADER *out)
{
    out->biSize = sizeof(BITMAPINFOHEADER);
    out->biWidth = width;
    out->biHeight = height;
    out->biPlanes = 1;
    out->biBitCount = 16;
    out->biXPelsPerMeter = 0;
    out->biYPelsPerMeter = 0;
    out->biClrUsed = 0;
    out->biClrImportant = 0;
}

constexpr uint32_t MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC = 160;
static k4a_device_configuration_t get_master_config();
static k4a_device_configuration_t get_subordinate_config();

k4a::record record_main;
k4a::record record_sub;
//queue<k4a::record> to_send; a queue if needed


// ---------------- Socket ------------------------------

int
open_clientfd()
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    //serv_addr.sin_addr.s_addr = inet_addr(hostname);

     if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    


    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    return sock;
    
}

// ------------------------------------------------------

int main()
{

    cout<<"KEY"<<endl;
    cout<<"r to record"<<endl;
    cout<<"s to stop"<<endl;
    cout<<"q to quit \n"<<endl;

    //trial client code

    printf("I am the client.\n");
	//printf("Input file name: %s\n", argv[1]);
	//printf("My server IP address: %s\n", argv[2]);

	printf("-----------\n");

    //char* host = '127.0.0.1';
    string port = "9999";
    
    int status;
    char line[MAXLINE];
    char buf[MAXLINE];
    //cmd cmd; 

    //file = fopen(argv[1], "r");
    cfd = open_clientfd();


    string hello = "Hello from client";
    send(cfd, "Hello from client", size(hello), 0);


    //exit(EXIT_SUCCESS);


    int32_t color_exposure_usec = 8000;  // somewhat reasonable default exposure time
    int32_t powerline_freq = 2;          // default to a 60 Hz powerline
    uint16_t depth_threshold = 1000;     // default to 1 meter
    size_t num_devices = 2;
    double calibration_timeout = 60.0; // default to timing out after 60s of trying to get calibrated

    vector<uint32_t> device_indices{ 0 }; // Set up a MultiDeviceCapturer to handle getting many synchronous captures
                                          // Note that the order of indices in device_indices is not necessarily
                                          // preserved because MultiDeviceCapturer tries to find the master device based
                                          // on which one has sync out plugged in. Start with just { 0 }, and add
                                          // another if needed

    if (num_devices == 2)
    {
        device_indices.emplace_back(1); // now device indices are { 0, 1 }
    }

    try
    {   

        MultiDeviceCapturer capturer(device_indices, color_exposure_usec, powerline_freq);

        k4a_device_configuration_t main_config = get_master_config();
        if (num_devices == 1) // no need to have a master cable if it's standalone
        {
            main_config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
        }
        k4a_device_configuration_t secondary_config = get_subordinate_config();

        // Construct all the things that we'll need whether or not we are running with 1 or 2 cameras
        k4a::calibration main_calibration = capturer.get_master_device().get_calibration(main_config.depth_mode,
                                                                                     main_config.color_resolution);

        // Set up a transformation. DO THIS OUTSIDE OF YOUR MAIN LOOP! Constructing transformations involves time-intensive
        // hardware setup and should not change once you have a rigid setup, so only call it once or it will run very
        // slowly.
         //k4a::transformation main_depth_to_main_color(main_calibration);

        capturer.start_devices(main_config, secondary_config);
        // get an image to be the background
        vector<k4a::capture> background_captures = capturer.get_synchronized_captures(secondary_config);

        
        int frame_count = 0;
        bool record_flag = 0;

        while (1)
        {

            vector<k4a::capture> captures;
            captures = capturer.get_synchronized_captures(secondary_config, true);
            k4a::image depth_image = captures[0].get_depth_image();
            uint8_t* buffer = depth_image.get_buffer();
            int rows = depth_image.get_height_pixels();
            int cols = depth_image.get_width_pixels();

            int buf_num = 0;
            int i = 0;

            //cout<<  rows << ", " << *(buffer)<<endl;
/*
            while(&(buffer[0]) != nullptr){
                //cout<<buffer[0];
                buffer++;

            }
            */
            //buffer++;
            //cout<<  rows << ", " << cols << ", " << (int)*buffer <<endl;

            //the line that works
            cv::Mat depthMat(rows, cols, CV_16U, (void*)buffer, cv::Mat::AUTO_STEP); 
            Mat dst;
            threshold(depthMat, dst, 1500, 65535, THRESH_BINARY);


            //cv::Mat depthMat(rows, cols, CV_16U, buffer);
            //cv::Mat3b rgb_image(rows,cols, CV_32F, buffer);

            //cv::Mat3b depthMat(rows, cols, CV_8UC4, (void*)buffer, cv::Mat::AUTO_STEP);
            //cv::Mat depthMat(rows, cols, CV_8UC3, (void*)buffer, cv::Mat::AUTO_STEP);
           
            imshow("Master", dst);
            

            int key = (waitKey(15) & 0xFF);

        

            if (key == 'r')
            {

                std::time_t timestamp = std::time(0);  // t is an integer type
                std::stringstream s_timestamp;
                s_timestamp << timestamp;
                
                string file_path_1 = "/home/k8s/Azure-Kinect-Samples/Recorded_Vids/record-main";
                string file_path_2 = "/home/k8s/Azure-Kinect-Samples/Recorded_Vids/record-sub";
                file_path_1 += '-' + s_timestamp.str() + ".mkv";
                file_path_2 += '-' + s_timestamp.str() + ".mkv";
                const char * c_1 = file_path_1.c_str();
                const char * c_2 = file_path_2.c_str();

                record_main = k4a::record::create(c_1, 
                    capturer.get_master_device(), main_config);
                record_main.write_header();
                
                record_sub = k4a::record::create(c_2, 
                    capturer.get_subordinate_device_by_index(0), secondary_config);
                record_sub.write_header();


                record_flag = 1;    
            }
            else if (key == 's')
            {
                cout<<"Recording Done"<<endl;
                record_flag = 0;
                record_main.flush();
                record_main.close();
                record_sub.flush();
                record_sub.close();
            }
                        else if (key == 'q')
            {
                capturer.close_devices();
                exit(1);
            }

            if (record_flag)
            {   
                if (record_main.is_valid() && record_sub.is_valid())
                {
                    cout<<"recording....."<<frame_count<<endl;
                    record_main.write_capture(captures[0]);
                    record_sub.write_capture(captures[1]);
                }
                else
                {
                    cout<<"Invalid recorder!!"<<endl;
                }
                
                
            }
            

            frame_count++;
            captures[0].reset();
            captures[1].reset();


           

        }
        //std::cout << "Finished body tracking processing!" << std::endl;

        capturer.close_devices();

    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed with exception:" << std::endl
            << "    " << e.what() << std::endl;
        return 1;
    }

    
    return 0;
}



// The following functions provide the configurations that should be used for each camera.
// NOTE: For best results both cameras should have the same configuration (framerate, resolution, color and depth
// modes). Additionally the both master and subordinate should have the same exposure and power line settings. Exposure
// settings can be different but the subordinate must have a longer exposure from master. To synchronize a master and
// subordinate with different exposures the user should set `subordinate_delay_off_master_usec = ((subordinate exposure
// time) - (master exposure time))/2`.
//
static k4a_device_configuration_t get_default_config()
{
    k4a_device_configuration_t camera_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    // camera_config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    camera_config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
    camera_config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED; // No need for depth during calibration
    camera_config.camera_fps = K4A_FRAMES_PER_SECOND_30;     // Don't use all USB bandwidth
    camera_config.subordinate_delay_off_master_usec = 0;     // Must be zero for master
    camera_config.synchronized_images_only = true;
    return camera_config;
}

// Master customizable settings
static k4a_device_configuration_t get_master_config()
{
    k4a_device_configuration_t camera_config = get_default_config();
    camera_config.wired_sync_mode = K4A_WIRED_SYNC_MODE_MASTER;

    // Two depth images should be seperated by MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC to ensure the depth imaging
    // sensor doesn't interfere with the other. To accomplish this the master depth image captures
    // (MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC / 2) before the color image, and the subordinate camera captures its
    // depth image (MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC / 2) after the color image. This gives us two depth
    // images centered around the color image as closely as possible.
    camera_config.depth_delay_off_color_usec = -static_cast<int32_t>(MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC / 2);
    camera_config.synchronized_images_only = true;
    //camera_config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
    return camera_config;
}

// Subordinate customizable settings
static k4a_device_configuration_t get_subordinate_config()
{
    k4a_device_configuration_t camera_config = get_default_config();
    camera_config.wired_sync_mode = K4A_WIRED_SYNC_MODE_SUBORDINATE;

    // Two depth images should be seperated by MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC to ensure the depth imaging
    // sensor doesn't interfere with the other. To accomplish this the master depth image captures
    // (MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC / 2) before the color image, and the subordinate camera captures its
    // depth image (MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC / 2) after the color image. This gives us two depth
    // images centered around the color image as closely as possible.
    camera_config.depth_delay_off_color_usec = MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC / 2;
    
    return camera_config;
}
