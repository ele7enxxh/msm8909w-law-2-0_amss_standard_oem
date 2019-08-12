/******************************************************************************
  @file    list_capture.c
  @brief   Source file to capture list of files

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <cutils/trace.h>
#include "list_capture.h"
#include <cutils/log.h>

#undef LOG_TAG
#define LOG_TAG           "ANDR-IOP_CAP"


#if QC_DEBUG
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#define QLOGE(...)
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#endif

#define CAPTURE_MAX_DURATION 3000
#define MAX_NUM_OF_FILES 1024

typedef struct _capture_thread_arg {
    pid_t pid;
    char *pkg_name;
} capture_thread_arg;

file_details     * file_list[MAX_NUM_OF_FILES];
static int       total_files = 0;
static long      start_capture_time;
static time_t    start_capture_time_s;
static pthread_t capture_pthread_id;
volatile static int       halt_thread = 1;
static long      read_fd_interval_ms;
static char      list_pkg_name[PKG_NAME_LEN];

static double now_ms(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (double)1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}

static int find_insert_into_list(char *filename, long long size, long file_capture_time) {
    int index;
    if(MAX_NUM_OF_FILES <=  total_files)
    {
         QLOGE("MAX Num of file reached, can not add more files");
         return -1;
    }
    if(NULL == filename)
    {
        QLOGE("Invalid argument");
        return -1;
    }

    for(index = 0; index < total_files; index++) {
        if(NULL == file_list[index])
        {
            QLOGE("NULL File Name");
            continue;
        }
        if(strcmp(filename, file_list[index]->file_name) == 0) {
        break;
    }
    }

    if (index == total_files) {
        file_list[index] = (file_details *) malloc (sizeof(file_details));
        if(NULL == file_list[index])
        {
            QLOGE("Fail to allocate memory");
            return -1;
        }
        strlcpy(file_list[index]->file_name, filename,FILE_NAME_LEN);
        file_list[index]->file_time_stamp = file_capture_time;
        file_list[index]->filesize = size;
        total_files++;
    }
    return 0;
}

 char filter_list[][1024]= {"/dev/","/core-libart.jar","/selinux_version","debug","/seapp_contexts"\
                           ,"boot.art","mp4","m4v","/proc/","fuse","mmcblk0p","media"\
                           ,"resource-cache","shared_relro","wallpaper","keychain"\
                           ,"cache/http","event-log-tags","/system/framework/"\
                           ,"libvoH264Dec_v7_OSMP","libvossl_OSMP","libTango.so"\
                           };

static int file_need_to_filter(char * file_name)
{
    int need_to_filter = 0;
    unsigned int ittr = 0;

    //Check base.apk belong to the same package or not
    if(NULL != strstr(file_name,"base.apk") &&
            NULL == strstr(file_name, list_pkg_name))
    {
        need_to_filter = 1;
    }
    else
    {
        for(ittr = 0; ittr < sizeof(filter_list)/1024;ittr++)
        {
            if(NULL != strstr(file_name,filter_list[ittr]))
            {
                need_to_filter = 1;
                break;
            }
        }
    }
    return need_to_filter;
}



int get_snapshot(pid_t pid) {

    struct stat sb_link, sb;
    char dirname[PATH_MAX + 1];
    char pathname[PATH_MAX + 1];
    char linkname[PATH_MAX + 1];
    ssize_t r;
    int need_to_filter = 0;

    DIR *dp;
    struct dirent *ep;
    long file_capture_time;

    snprintf(dirname,PATH_MAX,"/proc/%d/fd", pid);
    QLOGI("Dirname = %s MAX_PATH = %d\n", dirname, PATH_MAX);
    dp = opendir(dirname);
    QLOGI("get_snapshot Enter dp = %p",dp);
    QLOGI("dp = %d",dp);
    if (dp != NULL) {
        int i = 0;
        while ((ep = readdir(dp)))
        {
            snprintf(pathname, PATH_MAX , "%s/%s", dirname, ep->d_name);
            int ret = lstat(pathname, &sb_link);
            if (ret == -1) {
                continue;
            }
            file_capture_time = (long) now_ms() - start_capture_time;
            r = readlink(pathname, linkname, PATH_MAX + 1);
            if (r < 0) {
                continue;
            }
            linkname[r] = '\0';
            need_to_filter = file_need_to_filter(linkname);

            if(need_to_filter)
            {
                need_to_filter = 0;
                QLOGI("file is in filter list %s",linkname);
                continue; // goto  next file
            }

            if (stat(linkname, &sb) == -1) {
                continue;
            }

            if(memcmp("/storage/emulated/0/",linkname,20) == 0) {
                for(i = r; i >= 19; i--) {
                    linkname[i+5] = linkname[i];
                }
                memcpy((void *)&linkname[18],(void *)"legacy", 6);
            }
            if (stat(linkname, &sb) == -1) {
                continue;
            }

            if (S_ISREG(sb.st_mode)) {
                find_insert_into_list(linkname, sb.st_size, file_capture_time);
            }
        }
        QLOGI("get_snapshot Exit dp = %p",dp);
        closedir(dp);
    }

    return 0;
}

//start_capture_time
int capture_pkg_file(DIR *dp,char *dir_path)
{
    struct dirent *ep;
    char pathname[PATH_MAX + 1];
    DIR *new_dp = NULL;
    int fd = -1;
    struct stat f_st;
    int need_to_filter = 0;

    if(dp == NULL)
    {
        return -1;
    }
    while ((ep = readdir(dp))) {
        if(strcmp(ep->d_name,".") == 0 || strcmp(ep->d_name,"..") == 0)
        {
            QLOGI("current or previous dir IGNORE");
            continue;
        }

        snprintf(pathname, PATH_MAX,"%s/%s", dir_path, ep->d_name);
        if(ep->d_type == DT_DIR)
        {
            new_dp = opendir(pathname);
            QLOGI("Opening dir new_dp = %s",pathname);
            if (new_dp != NULL) {
                capture_pkg_file(new_dp,pathname);
                closedir(new_dp);
            }
            continue;
        }
        if(ep->d_type != DT_REG)
        {
            continue;
        }

        QLOGI("Opening file = %s",pathname);
        fd = open(pathname, O_RDONLY);
        if(fd < 0)
        {
            QLOGI("fail to open file");
            continue;
        }

        if ( stat( pathname, &f_st ) < 0 ) {
            QLOGI("fail to get file stat");
            close(fd);
            continue;
        }

        if(4096 > f_st.st_size)
        {
            QLOGI("File size is smaller then 4k");
            close(fd);
            continue;
        }
        need_to_filter = file_need_to_filter(pathname);

        if(need_to_filter)
        {
            need_to_filter = 0;
            close(fd);
            QLOGI("file is in filter list %s",pathname);
            continue; // goto  next file
        }
        QLOGI("curTime = %lu, start_capture_time_s = %lu,f_st.st_atime = %lu",time(NULL)
                                ,start_capture_time_s,f_st.st_atime);

        QLOGI("File is accessed %s ",ep->d_name);
        if (S_ISREG(f_st.st_mode))
        {
            QLOGI("inserting pathname = %s,f_st.st_size = %d, a_time = %d"
                                                    ,pathname,f_st.st_size,1010);

            find_insert_into_list(pathname, f_st.st_size, CAPTURE_MAX_DURATION);
        }

        close(fd);
    }

    return 0;
}

void get_priv_files(capture_thread_arg * arg_bundle)
{
    char f_name[FILE_NAME_LEN];
    DIR *dp;

    //Call the function for private dirs
    snprintf(f_name,FILE_NAME_LEN,"/data/app/%s-1",arg_bundle->pkg_name);
    QLOGI("Dir = %s",f_name);
    dp = opendir(f_name);
    QLOGI("First dp = %d",dp);
    if (dp != NULL) {
        capture_pkg_file(dp,f_name);
        closedir(dp);
    }
    else
    {
        QLOGI("dp is NULL");
    }

    snprintf(f_name,FILE_NAME_LEN,"/data/app/%s-2",arg_bundle->pkg_name);
    QLOGI("Dir = %s",f_name);
    dp = opendir(f_name);
    QLOGI("First dp = %d",dp);
    if (dp != NULL) {
        capture_pkg_file(dp,f_name);
        closedir(dp);
    }
    else
    {
        QLOGI("dp is NULL");
    }

    snprintf(f_name,FILE_NAME_LEN,"/data/user/0/%s",arg_bundle->pkg_name);
    QLOGI("Dir = %s",f_name);
    dp = opendir(f_name);
    QLOGI("First dp = %d",dp);
    if (dp != NULL) {
        capture_pkg_file(dp,f_name);
        closedir(dp);
    }
    else
    {
        QLOGI("dp is NULL");
    }


    snprintf(f_name,FILE_NAME_LEN,"/data/data/%s",arg_bundle->pkg_name);
    QLOGI("Dir = %s",f_name);
    dp = opendir(f_name);
    QLOGI("First dp = %d",dp);

    if (dp != NULL) {
        capture_pkg_file(dp,f_name);
        closedir(dp);
    }
    else
    {
        QLOGI("dp is NULL");
    }
}

void * capture_thread(void * arg) {
    int i = 0;
    int pkg_len = 0;
    int duration_counter = 0;
    pkg_details pkg_info;
    int halt_counter = CAPTURE_MAX_DURATION/read_fd_interval_ms;

    ATRACE_BEGIN("capture_thread");
    capture_thread_arg * arg_bundle = (capture_thread_arg *)arg;

    //Copy the package to same string to remove ':'
    pkg_len = strlen(arg_bundle->pkg_name);
    i = 0;
    while(i < pkg_len && arg_bundle->pkg_name[i] != ':')
    {
        i++;
    }
    arg_bundle->pkg_name[i] = '\0';
    strlcpy(list_pkg_name,arg_bundle->pkg_name,PKG_NAME_LEN);

    QLOGI("pkg_name  = %s",arg_bundle->pkg_name);

    while (duration_counter < halt_counter) {
        if (halt_thread) goto cleanup;
        QLOGI("Getting snapshot %d\n", arg_bundle->pid);
        get_snapshot(arg_bundle->pid);
        duration_counter++;
        usleep(read_fd_interval_ms * 1000);
    }

    get_priv_files(arg_bundle);

    QLOGI("pkg_name = %s total_files = %d ",arg_bundle->pkg_name,total_files);

    // Insert package into the table
    strlcpy(pkg_info.pkg_name,arg_bundle->pkg_name,PKG_NAME_LEN);
    time(&pkg_info.last_time_launched);
    update_pkg_details(pkg_info);
    update_file_details(arg_bundle->pkg_name, file_list, total_files);
    delete_mark_files();
cleanup:
    for(i = 0; i < total_files; i++) {
        QLOGE("%d. Final entry : %s %s %d %d\n",i,arg_bundle->pkg_name,file_list[i]->file_name
                                , file_list[i]->file_time_stamp, file_list[i]->filesize);
        free(file_list[i]);
    }
    ATRACE_END();

    free(arg_bundle->pkg_name);
    free(arg_bundle);
    QLOGI("Exit capture_thread");
    return NULL;
}

int start_capture(pid_t pid, char *pkg_name) {
    char property[PROPERTY_VALUE_MAX];
    capture_thread_arg *arg_bundle = NULL;
    int len = 0;

    if(NULL == pkg_name)
        return -1;

    len = strlen(pkg_name);
    if ( len <= 0)
    {
        QLOGW("incorrect length for pkg_name");
        return -1;
    }

    arg_bundle = (capture_thread_arg *)malloc(sizeof(capture_thread_arg));
    if (NULL == arg_bundle)
        return -1;

    total_files = 0;
    QLOGE(" start_capture ENTER ");

    arg_bundle->pid = pid;
    arg_bundle->pkg_name = (char *) malloc(len+1);
    if (NULL == arg_bundle->pkg_name) {
        free(arg_bundle);
        return -1;
    }

    QLOGI("%d %s %d\n", pid, pkg_name,strlen(pkg_name),PKG_NAME_LEN);
    strlcpy(arg_bundle->pkg_name, pkg_name,len+1);
    QLOGI("%d %s %d\n", pid, arg_bundle->pkg_name,strlen(pkg_name));

    halt_thread = 0;
    start_capture_time = (long) now_ms();

    time(&start_capture_time_s);

    property_get("read_fd_interval_ms", property, "50");
    read_fd_interval_ms = atoi(property);
    if(read_fd_interval_ms <= 0) {
        QLOGI("read_fd_interval_ms is set to a non-positive value %d. Resetting to default value 50\n", read_fd_interval_ms);
        property_set("read_fd_interval_ms", "50");
        read_fd_interval_ms = 50;
    }
    QLOGI("property = %d\n", read_fd_interval_ms);
    //TBD must remove comments
    if(pthread_create(&capture_pthread_id, NULL, capture_thread, arg_bundle)) {
        return -1;
    }
   QLOGI("start_capture EXIT");
    return 0;
}

int stop_capture() {
    halt_thread = 1;
    QLOGE(" stop_capture ENTER ");
    pthread_join(capture_pthread_id, (void **) NULL);
    QLOGE(" stop_capture EXIT ");
    return 0;
}
