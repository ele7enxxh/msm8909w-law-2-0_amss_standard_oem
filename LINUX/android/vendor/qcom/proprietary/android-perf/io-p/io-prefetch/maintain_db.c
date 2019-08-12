/******************************************************************************
  @file    maintain_db.c
  @brief   Source file to maintain database of files

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <cutils/properties.h>
#include <pthread.h>
#include "maintain_db.h"

static int pkg_retention_threshold = -1;
static int pkg_cleanup_interval = -1;
static pthread_t db_cleanup_thread;

/* return current time in milliseconds */
static double now_secs(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (double)1.0 * res.tv_sec + (double) res.tv_nsec / 1e9;
}

static int db_maintainer_cleanup_schedule()
{
    pkg_details *cur_list;
    int total_pkgs = 0, num_pkgs = 0, index;
    long cur_time = (long) now_secs();

    total_pkgs = get_total_pkgs();
    cur_list = (pkg_details *) malloc(total_pkgs * sizeof(pkg_details));

    if(cur_list == NULL)
    {
        return -1;
    }

    num_pkgs = get_pkg_list(cur_list, total_pkgs);

    if (num_pkgs == -1)
    {
        free(cur_list);
        return -1;
    }

    for (index = 0; index < num_pkgs; index++)
    {
        if(cur_list[index].last_time_launched - cur_time >= pkg_retention_threshold)
        {
            remove_pkg(cur_list[index].pkg_name);
        }
    }
    free(cur_list);
    return 0;
}

void * db_maintainer_cleanup_thread(void * arg) {
    int ret;
    arg = NULL;
    char property[PROPERTY_VALUE_MAX];

    while(1)
    {
        property_get("prefetcher.pkg_cleanup_intrvl",property, "2880");
        pkg_cleanup_interval = atoi(property) * 60;
        property_get("prefetcher.pkg_retention_mins",property, "21600");
        pkg_retention_threshold = atoi(property) * 60;

        ret = db_maintainer_cleanup_schedule();
        delete_mark_files();
        clean_db();

        sleep(pkg_cleanup_interval);
    }
    return NULL;
}


/*
static int db_maintainer_update_pkg(char *pkg_name)
{
    //TBD Need to fix this
    pkg_name[0] = pkg_name[0];
    return 0;
}*/

int db_maintainer_init()
{
    if(pthread_create(&db_cleanup_thread, NULL, &db_maintainer_cleanup_thread, NULL))
    {
        return -1;
    }

    return 0;
}

