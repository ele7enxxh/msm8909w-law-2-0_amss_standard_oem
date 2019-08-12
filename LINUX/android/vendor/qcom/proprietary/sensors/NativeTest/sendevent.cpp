/*============================================================================
  @file sendevent.cpp

  @brief
    Loop back test for inserting sensor events which can be received back at sensor HAL.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
  ==========================================================================*/



#include "file_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>
#include <linux/uinput.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <dirent.h>
#include <cutils/log.h>


#define MODE_GET_STATUS "getstatus"
#define MODE_SET_STATUS "setstatus"
#define MODE_INJECT_DATA "inject"

#define M_PI 3.14159265358979323846
#define GRAVITY_EARTH           (9.80665f)

#define	EVENT_TYPE_ACCEL_X ABS_X
#define	EVENT_TYPE_ACCEL_Y ABS_Y
#define	EVENT_TYPE_ACCEL_Z ABS_Z

#define ACCEL_CONVERT  ((GRAVITY_EARTH) / 1024)
#define CONVERT_ACCEL_X ACCEL_CONVERT
#define CONVERT_ACCEL_Y ACCEL_CONVERT
#define CONVERT_ACCEL_Z ACCEL_CONVERT

#define GYROSCOPE_CONVERT (M_PI / (180 * 16.4))

#define	EVENT_TYPE_GYRO_X ABS_RX
#define	EVENT_TYPE_GYRO_Y ABS_RY
#define	EVENT_TYPE_GYRO_Z ABS_RZ

#define MAX_SYSFS_LENGTH 100
#define SYSFS_ENABLE "/enable"


int getstatus(char *input_sysfs_path) {
    int fd;
    fd = open(input_sysfs_path, O_RDONLY);
    if(fd<0) {
        ALOGE("(%s)failed to open %s",
            __FUNCTION__,input_sysfs_path);
        return -1;
    }
    if (fd >= 0) {
    char buf[2];
        if(!read(fd, buf, sizeof(buf))){
            ALOGE("get status read failed");
            close(fd);
            return -1;
        }
        ALOGE("(%s) status %d",__FUNCTION__,atoi(&buf[0]));
        return atoi(&buf[0]);
    }
    return -1;
}

int setstatus(char *input_sysfs_path,int status) {
    int fd;
    char buf[2] = {'0','\0'};
    ALOGE("(%s) status %d",__FUNCTION__,status);
    fd = open(input_sysfs_path, O_WRONLY);
    if(fd<0) {
        ALOGE("(%s)failed to open %s",
            __FUNCTION__,input_sysfs_path);
        return -1;
    }
    if (fd >= 0) {
        if(status) buf[0] = '1';
        if(!write(fd, buf, sizeof(buf))){
            ALOGE("(%s)write failed",__FUNCTION__);
            close(fd);
            return -1;
        }
    }
    return 0;
}

int ParseInputForfd(const char *name,char *sysfspath) {
    ALOGE("scan for the device %s",name);
    char inputName[50] = "";
    strlcpy(inputName,name,50);
    int fd = -1;
    const char *dirname = "/dev/input";
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL) {
        ALOGE("sensors:failed to open dir");
        return -1;
    }
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
                (de->d_name[1] == '\0' ||
                    (de->d_name[1] == '.' && de->d_name[2] == '\0')))
        continue;
        strcpy(filename, de->d_name);
        fd = open(devname, O_RDWR);
        if (fd>=0) {
            char name[80];
            if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
                name[0] = '\0';
            }
            if (!strcmp(name, inputName)) {
                ALOGE("SUCCESS: '%s' input device @ %s and fd %d filename %s",
                                inputName,devname,fd,filename);
                break;
            } else {
                ALOGE("SEARCHING: %s got %s and device name %s",
                        inputName,name,devname);
                close(fd);
                fd = -1;
            }
        }
    }
    closedir(dir);
#ifndef LOG
    ALOGE_IF(fd<0, "couldn't find '%s' input device", inputName);
#else
    if(fd < 0)
    {
        printf("couldn't find '%s' input device", inputName);
    }
#endif
    if(fd>=0) {
        ALOGE("Return the loopback device data fd %d",fd);
    }
    return fd;
}

int inject_accel_data(int fd, int *value)
{
    struct input_event event;
    int bytes_written = 0;

    ALOGE("feeding ACCEL data");
    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = EVENT_TYPE_ACCEL_X;
    event.value = value[0]/ACCEL_CONVERT;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = EVENT_TYPE_ACCEL_Y;
    event.value = value[1]/ACCEL_CONVERT;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = EVENT_TYPE_ACCEL_Z;
    event.value = value[2]/ACCEL_CONVERT;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    return bytes_written;
}

int inject_gyro_data(int fd, int *value)
{
    struct input_event event;
    int bytes_written = 0;
    ALOGE("feeding GYRO data");
    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = EVENT_TYPE_GYRO_X;
    event.value = value[0]/GYROSCOPE_CONVERT;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = EVENT_TYPE_GYRO_X;
    event.value = value[1]/GYROSCOPE_CONVERT;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = EVENT_TYPE_GYRO_X;
    event.value = value[2]/GYROSCOPE_CONVERT;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    return bytes_written;
}

int inject_light_data(int fd, int value)
{
    struct input_event event;
    int bytes_written = 0;
    ALOGE("feeding LIGHT data");

    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = ABS_MISC;
    event.value = value;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    return bytes_written;
}

int inject_proxy_data(int fd, int value)
{
    struct input_event event;
    int bytes_written = 0;
    ALOGE("feeding PROXY data");
    memset(&event, 0, sizeof(struct input_event));
    event.type  = EV_ABS;
    event.code  = ABS_DISTANCE;
    event.value = value;
    bytes_written = write(fd, &event, sizeof(struct input_event));
    if (bytes_written != sizeof(struct input_event)) {
        printf("%s fd %d error %d",__FUNCTION__,fd,bytes_written);
        return -1;
    }

    return bytes_written;
}

const char *getsysfspath(char *sensor_type)
{
    if(!strcmp(sensor_type,sensorNames[ACCEL_DEVICE_ID]))
        return "/sys/class/sensors/MPU6050-accel/enable";
    else if(!strcmp(sensor_type,sensorNames[PROXIMITY_DEVICE_ID]))
        return "/sys/class/sensors/apds9930-proximity/enable";
    else if(!strcmp(sensor_type,sensorNames[LIGHT_DEVICE_ID]))
        return "/sys/class/sensors/apds9930-light/enable";
    else if(!strcmp(sensor_type,sensorNames[GYROSCOPE_DEVICE_ID]))
        return "/sys/class/sensors/MPU6050-gyro/enable";
    else
        return NULL;
}

int openFds(int *fd)
{
    int version;
    int id;
    for(id = 0; id < TOTAL_NUM_OF_SENSORS; id++)
    {
        fd[id] =ParseInputForfd(sensorNames[id], NULL);
        if(fd[id] < 0) {
            fprintf(stderr, "could not open %s, %s\n", sensorNames[id], strerror(errno));
            return -1;
        }
        if (ioctl(fd[id], EVIOCGVERSION, &version)) {
            fprintf(stderr, "could not get driver version for %s, %s\n", sensorNames[id], strerror(errno));
            return -1;
        }
    }
    return 0;
}

void closeFds(int *fd)
{
    int id;
    for(id = 0; id < TOTAL_NUM_OF_SENSORS; id++)
    {
        if(fd[id] >= 0)
        {
            close(fd[id]);
        }
    }
}

int main(int argc, char *argv[])
{
    int fd[TOTAL_NUM_OF_SENSORS];
    int currentFd;
    int ret;
    struct input_event event;
    unsigned int bytes_written = 0;
    static int cnt = 0;
    int duration;
    int frequency;

    if(argc < 3) {
        fprintf(stderr, "use: %s sensor_name operating mode\n", argv[0]);
        return -1;
    }

    if(!strcmp(argv[2],MODE_GET_STATUS))
    {
         ALOGE("get_status");
         int fd = -1;
         fd =ParseInputForfd(argv[1],NULL);
         if(fd < 0) {
             fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
             return -1;
         }
         char sysfspath[MAX_SYSFS_LENGTH];
         strcpy(sysfspath,getsysfspath(argv[1]));
         return getstatus(sysfspath);
    }

    if(!strcmp(argv[2],MODE_SET_STATUS))
    {
         ALOGE("set_status");
         if(argc < 4) {
             fprintf(stderr, "use: %s sensor_name setstatus value\n", argv[0]);
             return -1;
         }
         int fd = -1;
         fd =ParseInputForfd(argv[1],NULL);
         if(fd < 0) {
             fprintf(stderr, "could not open %s, %s\n", argv[optind], strerror(errno));
             return -1;
         }
         char sysfspath[MAX_SYSFS_LENGTH];
         strcpy(sysfspath,getsysfspath(argv[1]));
         return setstatus(sysfspath,atoi(argv[3]));
    }

    if(!strcmp(argv[2],MODE_INJECT_DATA))
    {
        int value[MAX_DATA_IN_ROW]; /* value[0] holds the sensor id, value[1,2,3] holds the respective data for each sensor. */
        ALOGE("data_inject");
        if(argc < 5){
            ALOGE("Error: Invalid usage for inject.\r\n");
            ALOGE("Usage: ./%s <filename> inject <duration> <frequency>\r\n", argv[0]);
        }
        if(readFile(argv[1]))
        {
            ALOGE("Error: Failed [at main()]\r\n");
            return -1;
        }
        duration = atoi(argv[3]) * 60; /* converting into seconds. */
        if(duration < 1)
        {
            duration = 60;
        }
        frequency = atoi(argv[4]);
        if(frequency <= 0)
        {
            ALOGE("Error: Invalid frequency [at main()]\r\n");
            return -1;
        }
        frequency = (1000/frequency) * 1000; /* Converting into microseconds. */

        if(openFds(fd))
        {
            ALOGE("Error: Failed to open fds for sensor(s) [at main()]. \r\n");
            closeFds(fd);
            return - 1;
        }
         stopFlag = 0;
         setAlarm(duration);
         do {
             //usleep(10*1000);
                usleep(frequency);
                getData(value, cnt);
                /* This function will update the array value with data from the input file
                              only if there is any valid data read from the file.  Otherwise it will
                              update the array with cnt.*/
                cnt++;
             if(ACCEL_DEVICE_ID == value[0]) { /* value[0] contains the sensor_ID. */
                 currentFd = fd[ACCEL_DEVICE_ID];
                 ret = inject_accel_data(currentFd, &value[3]);
                 if(ret<=0) {
                     ALOGE("FAILED for inject_accel_data err: %d",ret);
                 }
             }
             if(GYROSCOPE_DEVICE_ID == value[0]) {/* value[0] contains the sensor_ID. */
                 currentFd = fd[GYROSCOPE_DEVICE_ID];
                 ret = inject_gyro_data(currentFd, &value[3]);
                 if(ret<=0) {
                     ALOGE("FAILED for inject_gyro_data err: %d",ret);
                 }
             }
             if(PROXIMITY_DEVICE_ID == value[0]) {/* value[0] contains the sensor_ID. */
                 currentFd = fd[PROXIMITY_DEVICE_ID];
                 ret = inject_proxy_data(currentFd, value[3]%2);
                 if(ret<=0) {
                     ALOGE("FAILED for inject_proxymity_data err: %d",ret);
                 }
             }
             if(LIGHT_DEVICE_ID == value[0]) {/* value[0] contains the sensor_ID. */
                 currentFd = fd[LIGHT_DEVICE_ID];
                 ret = inject_light_data(currentFd, value[3]);
                 if(ret<=0) {
                     ALOGE("FAILED for inject_proxymity_data err: %d",ret);
                 }
             }

            /*Read the TS and send via interface*/
             memset(&event, 0, sizeof(struct input_event));
             ALOGE("INJECT TIME");
             event.type  = EV_SYN;
             event.code  = SYN_TIME_SEC;

             event.value = value[1];
             bytes_written = write(currentFd, &event, sizeof(struct input_event));
             if (bytes_written != sizeof(struct input_event)) {
                 printf("%s fd %d error %d",__FUNCTION__,currentFd,bytes_written);
             }

             memset(&event, 0, sizeof(struct input_event));
             event.type  = EV_SYN;
             event.code  = SYN_TIME_NSEC;
             event.value = value[2];  /*get this value from file and use it*/
             bytes_written = write(currentFd, &event, sizeof(struct input_event));
             if (bytes_written != sizeof(struct input_event)) {
                 printf("%s fd %d error %d",__FUNCTION__,currentFd,bytes_written);
             }

             memset(&event, 0, sizeof(struct input_event));
             event.type  = EV_SYN;
             event.code  = SYN_REPORT;
             bytes_written = write(currentFd, &event, sizeof(struct input_event));
             if (bytes_written != sizeof(struct input_event)) {
                 printf("%s fd %d error %d",__FUNCTION__,currentFd,bytes_written);
             }
        }while(stopFlag == 0);
        closeFds(fd);
    }
    return -1;
}
