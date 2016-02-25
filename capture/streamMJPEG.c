//
// Created by manu on 2/18/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

int main(void) {
    int fd;
    if ((fd = open("/dev/video0", O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }
    struct v4l2_capability capabilities;
    if (ioctl(fd, VIDIOC_QUERYCAP, &capabilities) < 0) {
        perror("VIDIOC_QUERYCAP");
        exit(1);
    }

    struct timeval tv;
    gettimeofday(&tv,NULL);


    long sec = tv.tv_sec;

    long micro = tv.tv_usec;

    //  salut
    //
    if (!(capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "The device does not handle single-planar video capture.\n");
        exit(1);
    }

    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    format.fmt.pix.width = 1920;
    format.fmt.pix.height = 1080;

    if (ioctl(fd, VIDIOC_S_FMT, &format) < 0) {
        perror("VIDIOC_S_FMT");
        exit(1);
    }

    struct v4l2_requestbuffers bufrequest;
    bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufrequest.memory = V4L2_MEMORY_MMAP;
    bufrequest.count = 1;

    if (ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0) {
        perror("VIDIOC_REQBUFS");
        exit(1);
    }

    struct v4l2_buffer bufferinfo;
    memset(&bufferinfo, 0, sizeof(bufferinfo));

    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0;

    if (ioctl(fd, VIDIOC_QUERYBUF, &bufferinfo) < 0) {
        perror("VIDIOC_QUERYBUF");
        exit(1);
    }

    char *buffer_start = mmap(
            NULL,
            bufferinfo.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd,
            bufferinfo.m.offset
    );

    printf("Mapped size %d\n", bufferinfo.length);


    if (buffer_start == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    memset(buffer_start, 0, bufferinfo.length);
    //struct v4l2_buffer bufferinfo;
    memset(&bufferinfo, 0, sizeof(bufferinfo));

    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0; /* Queueing buffer index 0. */

// Activate streaming
    int type = bufferinfo.type;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        exit(1);
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    // Get the screen's surface.
    SDL_Surface *screen = SDL_SetVideoMode(
            format.fmt.pix.width,
            format.fmt.pix.height,
            32, SDL_HWSURFACE
    );

    SDL_Rect position = {.x = 0, .y = 0};


    int i;
    for (i = 0; i < 300; i++) {
/* Here is where you typically start two loops:
 * - One which runs for as long as you want to
 *   capture frames (shoot the video).
 * - One which iterates over your buffers everytime. */


        gettimeofday(&tv,NULL);


        long newMicro = (tv.tv_sec - sec)*1000000+tv.tv_usec;


        printf("Elapsed %ld\n",(newMicro - micro));
        micro = newMicro;

// Put the buffer in the incoming queue.
        if (ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0) {
            perror("VIDIOC_QBUF");
            exit(1);
        }

// The buffer's waiting in the outgoing queue.
        if (ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0) {
            perror("VIDIOC_QBUF");
            exit(1);
        }

        //printf("Dequeue %d\n", bufferinfo.bytesused);

/* Your loops end here. */



// Create a stream based on our buffer.
        SDL_RWops *buffer_stream = SDL_RWFromMem(buffer_start, bufferinfo.bytesused);

       // printf("FromMem\n");

// Create a surface using the data coming out of the above stream.
        SDL_Surface *picture = IMG_Load_RW(buffer_stream, 1);
      //  printf("Load\n");

// Blit the surface and flip the screen.
        SDL_BlitSurface(picture, NULL, screen, &position);
       // printf("Blit\n");

        SDL_FreeSurface(picture);
       // printf("Free\n");


        SDL_Flip(screen);
       // printf("Flip\n");

// Free everything, and unload SDL & Co.
     //   SDL_RWclose(buffer_stream);
      //  printf("Close\n");


    }
// Deactivate streaming
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("VIDIOC_STREAMOFF");
        exit(1);
    }


    IMG_Quit();
    SDL_Quit();
    printf("Hourra!!");
    close(fd);
    return EXIT_SUCCESS;
}