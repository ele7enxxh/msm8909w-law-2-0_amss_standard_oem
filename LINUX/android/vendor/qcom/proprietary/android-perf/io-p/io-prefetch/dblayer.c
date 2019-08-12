/******************************************************************************
  @file    dblayer.c
  @brief   Source file to database of files

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

/******************************************************************************
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

#define ATRACE_TAG ATRACE_TAG_ALWAYS

#include "dblayer.h"
#include "string.h"
#include <cutils/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "sqlite3.h"
#include "maintain_db.h"

#undef LOG_TAG
#define LOG_TAG           "ANDR-IOP_DB"

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

#define _SQLITE_CHECK(stmt, rt) \
    sem_wait(&mutex); \
    if (rt != stmt) { \
        QLOGE("\n %s:%d: reason:%s", __FILE__, __LINE__, sqlite3_errmsg(db_conn)); \
        QLOGE(" \n in error code \n");\
        return 0; \
    }\
    sem_post(&mutex); 

#define _SQLITE_TRANSACTION_CHECK(stmt, rt) \
    sem_wait(&mutex); \
    if (rt != stmt) { \
        QLOGE("\n %s:%d: reason:%s", __FILE__, __LINE__, sqlite3_errmsg(db_conn)); \
        sqlite3_exec(db_conn, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);\
        return 0; \
    }\
    sem_post(&mutex); \

#define _SQLITE_CHECK_DONE(stmt) _SQLITE_CHECK(stmt, SQLITE_DONE)
#define _SQLITE_CHECK_OK(stmt) _SQLITE_CHECK(stmt, SQLITE_OK)
#define _SQLITE_CHECK_TRANSACTION_OK(stmt) _SQLITE_TRANSACTION_CHECK(stmt, SQLITE_OK)

/* database queries */


#define IO_PREFETCHER_QUERY_CREATE_TBL_FILE_LIST "CREATE TABLE IF NOT EXISTS io_pkg_file_tbl( \
pkg_name TEXT , \
file_name TEXT,\
file_use_counter INTEGER,\
file_time_stamp INTEGER,\
file_size INTEGER,\
mark_for_delete INTEGER,\
PRIMARY KEY(pkg_name,file_name)\
)"

#define IO_PREFETCHER_QUERY_CREATE_TBL_PKG_LIST  "CREATE TABLE IF NOT EXISTS io_pkg_tbl( \
 pkg_name TEXT PRIMARY KEY, \
 pkg_last_use DATETIME, \
 pkg_use_count INTEGER \
 )"

#define IO_PREFETCHER_QUERY_REMOVE_PACKAGE_PKG_TBL   "DELETE from io_pkg_tbl WHERE pkg_name = '%s';\
 DELETE from io_pkg_file_tbl\
 WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_REMOVE_PACKAGE_FILE_TBL  "DELETE from io_pkg_file_tbl \
 WHERE pkg_name = '%s' AND \
 file_name = '%s'"

#define IO_PREFETCHER_QUERY_MARK_FOR_DELETE      "UPDATE io_pkg_file_tbl \
  SET mark_for_delete = 1\
  WHERE pkg_name = '%s'\
  AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_DELETE_MARK_FILE     "DELETE from io_pkg_file_tbl \
WHERE mark_for_delete = 1"

#define IO_PREFETCHER_QUERY_TOTAL_PKG            "SELECT COUNT(pkg_name) \
AS num_of_pkg \
FROM io_pkg_tbl"

#define IO_PREFETCHER_QUERY_TOTAL_FILE "SELECT COUNT(file_name) \
 AS num_of_file \
 FROM io_pkg_file_tbl \
 WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_GET_FILE_LIST "SELECT * FROM io_pkg_file_tbl \
  WHERE pkg_name = '%s' \
  ORDER BY file_time_stamp ASC"

#define IO_PREFETCHER_QUERY_GET_PKG_LIST "SELECT * FROM io_pkg_tbl ORDER BY pkg_last_use"

#define IO_PREFETCHER_QUERY_GET_FILE "select * from io_pkg_file_tbl where pkg_name='%s'\
  AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_UPDATE_FILE_DETAILS  "UPDATE io_pkg_file_tbl \
 SET \
 file_use_counter = file_use_counter+1\
 ,file_time_stamp = %d\
 ,file_size = %d\
 WHERE pkg_name = '%s' AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_INSER_FILE "INSERT INTO io_pkg_file_tbl (pkg_name,file_name\
,file_use_counter\
,file_time_stamp\
,file_size\
,mark_for_delete)\
VALUES ('%s','%s',1,%d,%d,0)"

#define IO_PREFETCHER_QUERY_GET_PKG "SELECT * FROM io_pkg_tbl WHERE pkg_name='%s'"

#define IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS  "UPDATE io_pkg_tbl \
SET pkg_last_use = %lu\
,pkg_use_count = pkg_use_count+1 \
WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_INSERT_PKG "INSERT INTO io_pkg_tbl (pkg_name,\
  pkg_last_use,\
  pkg_use_count)\
  VALUES ('%s',%lu,1)"

sqlite3 *db_conn = NULL;     // database connection object
sem_t mutex;
char    _dbpath[] = "/data/misc/iop/io-prefetcher.db"; // database path

/* Private API */
int create_database();
static int open_db();
static int close_database();

typedef struct _wrapper_file_list
{
    file_details *file_list ;
    int          size;
    int         counter;

}wrapper_file_list;

typedef struct _wrapper_pkg_list
{
    pkg_details *pkg_list ;
    int          size;
    int          counter;
}wrapper_pkg_list;

//Open database connection
static int open_db()
{
    // don't allow opening up more than one database connection

    if( db_conn != NULL )
    {
      QLOGI( "Already Opened database connection");
      return 1;
    }
    // open the database connection
    int rc = sqlite3_open(_dbpath, &db_conn );
    QLOGI("sqlite3_open db_conn = %p rc = %d \n",db_conn,rc);

    if( db_conn == NULL || rc != SQLITE_OK)
    {
        QLOGI("unable to open database at this path=\"%s\" with sql err_msg=%s"
                     " sql ret code=%d", _dbpath, sqlite3_errmsg(db_conn),rc);

      return 0;
    }
    QLOGI( "db=%s has been opened successful", _dbpath);
    return 1;
}

int create_database()
{
    int fd = -1;
    fd = open(_dbpath, O_CREAT,644);

    if(fd < 0)
    {
        QLOGI("fail to create database");
        return -1;
    }

    close(fd);

    int conn_status = open_db();
    QLOGI("\n open DB %d \n", conn_status);

    if(conn_status != 1)
    {
        ALOGI("fail to open DB");
        return -1;
    }
    else
    {
        if( db_conn == NULL )
        {
          QLOGI("fail to create DB");
            return -1;
        }
        sem_init(&mutex, 0, 1);
        //create table
        QLOGI("executing Query");
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, IO_PREFETCHER_QUERY_CREATE_TBL_FILE_LIST, NULL,NULL,NULL));
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, IO_PREFETCHER_QUERY_CREATE_TBL_PKG_LIST, NULL,NULL,NULL));
        close_database();
    }
    db_maintainer_init();
    return 0;
}

// Close data base connection
static int close_database()
{
  // db already closed?
  if( db_conn == NULL ) return -1;

  int rc = sqlite3_close(db_conn);
  if( rc != SQLITE_OK )
  {
    QLOGI("unable to close database=%s with sql err_msg=%s sql ret code=%d",
                   _dbpath, sqlite3_errmsg(db_conn),rc);
    return -1;
  }
  db_conn = NULL;

  QLOGI( "db=%s has been closed successfully", _dbpath);
  return 0;
}

int clean_db()
{
    //TBD
    // This function is not needed so far
    // may need to revisit in V2
    return 0;
}

//=============================================================================
// Get list of file for given package
// Callback function for get_file_list
int get_file_list_cb(void * fl_list, int num_col, char **col_value, char **col_name)
{
    wrapper_file_list *file_info = (wrapper_file_list *)fl_list;
    if(file_info->counter < file_info->size)
    {
        QLOGI("get_file_list_cb filename = %s \n",col_value[1]);
        strlcpy(file_info->file_list[file_info->counter].file_name, col_value[1], FILE_NAME_LEN);
        file_info->file_list[file_info->counter].file_use_counter = strtod(col_value[2],NULL);
        file_info->file_list[file_info->counter].filesize  = strtod(col_value[3],NULL);
        file_info->file_list[file_info->counter].file_time_stamp = strtod(col_value[4],NULL);

        QLOGI("file_info->file_list[file_info->counter].file_name filename = %s \n",file_info->file_list[file_info->counter].file_name);

        file_info->counter++;
        return 0;
    }
    else
    {
        return -1;
    }
}

// Get list of file for given package
int get_file_list(char * pkg_name, file_details *file_list,int size)
{
    wrapper_file_list file_list_param;
    file_list_param.file_list = file_list;
    file_list_param.size = size;
    file_list_param.counter = 0;
    char query_str[1024] = IO_PREFETCHER_QUERY_GET_FILE_LIST ;

    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_GET_FILE_LIST,pkg_name);

    QLOGI("\n query = %s\n", query_str);
    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str,get_file_list_cb ,&file_list_param,NULL));
    return file_list_param.counter;
}

//===========
// Callback function for get_total_file
int get_total_file_cb(void * count, int num_col, char **col_value, char **col_name)
{
    int *cnt = (int *) count;
    *cnt = strtod(col_value[0],NULL);
    return 0;
}

// Get total number of file
int get_total_file(char * pkg_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_TOTAL_FILE;
    int num_files;
    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_TOTAL_FILE,pkg_name);

    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str,get_total_file_cb ,&num_files,NULL));

    return num_files;
}

//===========
// Callback function for get_total_pkgs
int get_total_pkg_cb(void * count, int num_col, char **col_value, char **col_name)
{
    int *cnt  = (int *) count;
    *cnt = strtoul(col_value[0],NULL,10);
    return 0;
}

// Get total numver of packages
int get_total_pkgs()
{
    char query_str[1024] = IO_PREFETCHER_QUERY_TOTAL_PKG;
    int num_files;

    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str,get_total_file_cb ,&num_files,NULL));
    return num_files;
}
//=============================================================================
// Callback function for get_pkg_list
int get_pkg_list_cb(void * pkg_list_arg, int num_col, char **col_value, char **col_name)
{
    wrapper_pkg_list *pkg_info = (wrapper_pkg_list *)pkg_list_arg;
    QLOGI("pkg_name = %s \n",col_value[0]);
    if(pkg_info->counter < pkg_info->size)
    {
        strlcpy(pkg_info->pkg_list[pkg_info->counter].pkg_name, col_value[0],PKG_NAME_LEN);
        pkg_info->pkg_list[pkg_info->counter].last_time_launched = strtod(col_value[1],NULL);
        pkg_info->pkg_list[pkg_info->counter].num_launches = strtod(col_value[2],NULL);
        pkg_info->counter++;
        return 0;
    }
    else
    {
        return -1;
    }
}

// to get the list of packages
int get_pkg_list(pkg_details *pkg_list_ptr,int size)
{
    wrapper_pkg_list pkg_list_param;
    char query_str[1024] = IO_PREFETCHER_QUERY_GET_PKG_LIST;
    pkg_list_param.pkg_list = pkg_list_ptr;
    pkg_list_param.size = size;
    pkg_list_param.counter = 0;

    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str,get_pkg_list_cb ,&pkg_list_param,NULL));
    return pkg_list_param.counter;
}

//==========================
// Remove package from databae
int remove_pkg(char *pkg_name)
{
    char pkg_query_str[2048] = IO_PREFETCHER_QUERY_REMOVE_PACKAGE_PKG_TBL;
    snprintf(pkg_query_str,sizeof(pkg_query_str),IO_PREFETCHER_QUERY_REMOVE_PACKAGE_PKG_TBL,pkg_name,pkg_name);

    QLOGI("queries = %s\n",pkg_query_str);
    if(NULL == db_conn)
        open_db();
    QLOGI("queries = %s\n",pkg_query_str);

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, pkg_query_str, NULL,NULL,NULL));

    return 0;
}

//==========================
// Remove file from databae
int remove_file(char *pkg_name,char* file_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_REMOVE_PACKAGE_FILE_TBL;
    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_REMOVE_PACKAGE_FILE_TBL,pkg_name,file_name);
      printf("queries = %s\n",query_str);
    if(NULL == db_conn)
        open_db();
    QLOGI("queries = %s\n",query_str);

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str, NULL,NULL,NULL));
    return 0;
}

//=====================
// Mark any file for delete so can be cleanup in next ittr
int mark_for_delete(char * pkg_name,char * file_name)
{
     char query_str[1024] = IO_PREFETCHER_QUERY_MARK_FOR_DELETE;

     snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_MARK_FOR_DELETE
                                        ,pkg_name
                                        ,file_name);
     if(NULL == db_conn)
        open_db();

    QLOGI("\n Query = %s\n",query_str);
    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str, NULL,NULL,NULL));

    return 0;
}

//=====================
// Delete all the marked file for deletion
int delete_mark_files()
{
    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, IO_PREFETCHER_QUERY_DELETE_MARK_FILE, NULL,NULL,NULL));
    return 0;
}

// update detail for file with provided attibutes
int update_file_details(char * pkg_name,file_details *file_info[], int size)
{
    int i = 0;
    char query_str[2048] = IO_PREFETCHER_QUERY_UPDATE_FILE_DETAILS;
    _SQLITE_CHECK_TRANSACTION_OK(sqlite3_exec(db_conn, "BEGIN TRANSACTION;", NULL,NULL,NULL));
    for(i=0;i<size;i++)
    {

        file_details temp_file_info;
        temp_file_info.file_name[0]=0;
        int is_file_present  = get_file(pkg_name, file_info[i]->file_name, temp_file_info);

        if(is_file_present)
        {
            //Update

            snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_UPDATE_FILE_DETAILS
                                                ,file_info[i]->file_time_stamp
                                                ,file_info[i]->filesize
                                                ,pkg_name
                                                ,file_info[i]->file_name);
            QLOGI("\nQuery = %s \n",query_str);

            _SQLITE_CHECK_TRANSACTION_OK(sqlite3_exec(db_conn, query_str, NULL,NULL,NULL));
        }
        else
        {
            //Insert

            snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_INSER_FILE
                                                ,pkg_name,file_info[i]->file_name
                                                ,file_info[i]->file_time_stamp
                                                ,file_info[i]->filesize
                                                );
            QLOGI("\nQuery = %s \n",query_str);

            _SQLITE_CHECK_TRANSACTION_OK(sqlite3_exec(db_conn, query_str, NULL,NULL,NULL));
        }
    }
    _SQLITE_CHECK_TRANSACTION_OK(sqlite3_exec(db_conn, "COMMIT TRANSACTION;", NULL,NULL,NULL));
     return 0;
}

//Update package deails with provided attributes
int update_pkg_details(pkg_details pkg_info)
{
    char query_str[2048] = IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS;
    pkg_details temp_pkg_info;
    temp_pkg_info.pkg_name[0] = 0;
    int is_file_present  =  get_package(pkg_info.pkg_name, temp_pkg_info);
    QLOGI("update_pkg_details");

    if(NULL == db_conn)
        open_db();

    if(is_file_present)
    {
        //Update
        snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS
                                            ,pkg_info.last_time_launched
                                            ,pkg_info.pkg_name);
        QLOGI("\nQuery = %s \n",query_str);
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str, NULL,NULL,NULL));
    }
    else
    {
        //Insert
        snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_INSERT_PKG
                                            ,pkg_info.pkg_name
                                            ,pkg_info.last_time_launched
                                            );

        QLOGI("\nQuery = %s \n",query_str);
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str, NULL,NULL,NULL));
    }
    return 0;
}

//=============================================================================
// SQL Callback for get_file
int get_file_cb(void * file_list_arg, int num_col, char **col_value, char **col_name)
{

    wrapper_file_list *file_info = (wrapper_file_list *)file_list_arg;
    QLOGI("pkg_name = %s \n",col_value[0]);
    if(file_info->counter < file_info->size)
    {
        QLOGI("get_file_cb filename = %s \n",col_value[1]);
        strlcpy(file_info->file_list[file_info->counter].file_name, col_value[1], FILE_NAME_LEN);
        file_info->file_list[file_info->counter].file_use_counter = strtod(col_value[2],NULL);
        file_info->file_list[file_info->counter].filesize  = strtod(col_value[3],NULL);
        file_info->file_list[file_info->counter].file_time_stamp = strtod(col_value[4],NULL);
        file_info->counter++;
        return 0;
    }
    else
    {
        return -1;
    }
}

int get_file(char *pkg_name, char *file_name, file_details file_info)
{
    char query_str[2048] = IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS;
    wrapper_file_list file_list_param;
    file_list_param.file_list = &file_info;
    file_list_param.size = 1;
    file_list_param.counter = 0;

    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_GET_FILE,pkg_name,file_name);
    QLOGI("\n get_file %s \n",query_str);
    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str, get_file_cb,&file_list_param,NULL));
    return file_list_param.counter;
}

//=============================================================================
// SQL callback for get-package
int get_package_cb(void * pkg_list_arg, int num_col, char **col_value, char **col_name)
{
    wrapper_pkg_list *pkg_info = (wrapper_pkg_list *)pkg_list_arg;
    QLOGI("pkg_name = %s \n",col_value[0]);
    if(pkg_info->counter < pkg_info->size)
    {
        strlcpy(pkg_info->pkg_list[pkg_info->counter].pkg_name, col_value[0],PKG_NAME_LEN);
        pkg_info->pkg_list[pkg_info->counter].last_time_launched = strtod(col_value[1],NULL);
        pkg_info->pkg_list[pkg_info->counter].num_launches = strtod(col_value[2],NULL);
        pkg_info->counter++;
        return 0;
    }
    else
    {
        return -1;
    }
}

int get_package(char *pkg_name, pkg_details pkg_info)
{

    wrapper_pkg_list pkg_list_param;
    char query_str[1024] = IO_PREFETCHER_QUERY_GET_PKG;
    pkg_list_param.pkg_list = &pkg_info;
    pkg_list_param.size = 1;
    pkg_list_param.counter = 0;

    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_GET_PKG,pkg_name);
    QLOGI("\n get_file %s \n",query_str);
    if(NULL == db_conn)
        open_db();

    _SQLITE_CHECK_OK(sqlite3_exec(db_conn, query_str, get_package_cb,&pkg_list_param,NULL));
    return pkg_list_param.counter;
}
