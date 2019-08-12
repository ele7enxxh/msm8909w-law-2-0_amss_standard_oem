/******************************************************************************
  @file    dblayer.h
  @brief   nheader  file to database of files

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

/******************************************************************************
  DESCRIPTION

     pkg_file_tbl               pkg_tbl
  |-----------------|      |-----------------|
  |  pkg_name       |      |  pkg_name       |
  |  file_name      |      |-----------------|
  |-----------------|      | pkg_use_time    |
  |                 |      | num_of_launches |
  | file_use_ctr    |      |-----------------|
  | file_time_stamp |
  | file_size       |
  | mark_for_delete |
  |-----------------|
******************************************************************************/
#ifndef __PREFETCHER_DBLAYER_H__
#define __PREFETCHER_DBLAYER_H__

#include <time.h>

#define FILE_NAME_LEN 4096
#define PKG_NAME_LEN 2048

/* data structure */
typedef struct _file_details
{
    char file_name[FILE_NAME_LEN];
    int file_use_counter;
    int filesize; // size in KB
    int file_time_stamp; // time in ms when this file open during launch
}file_details;

typedef struct _pkg_details {
    char pkg_name[PKG_NAME_LEN];
    time_t last_time_launched;
    int num_launches;
} pkg_details;

/* Public API */
int get_file_list(char * pkg_name, file_details *fl_list,int fl_list_size);
int get_pkg_list(pkg_details *pkg_list_ptr,int size);
int remove_pkg(char * pkg_name);
int remove_file(char * pkg_name,char * file_name);
int mark_for_delete(char * pkg_name,char * file_name);
int delete_mark_files();
int update_file_details(char * pkg_name,file_details *file_info[], int size);
int update_pkg_details(pkg_details pkg_info);
int get_total_pkgs();
int get_total_file(char *pkg_name);
int get_file(char *pkg_name, char *file_name, file_details file_info);
int get_package(char *pkg_name, pkg_details pkg_info);
int clean_db();
int create_database();
#endif //__PREFETCHER_DBLAYER_H__
