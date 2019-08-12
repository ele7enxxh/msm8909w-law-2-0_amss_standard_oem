/*
@file app_crypto.c
@brief This file is a sample code that show how QSEE crypto API's can be used.

*/
/*===========================================================================
   Copyright (c) 2011 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_sfs.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_sfs.h"
#include "qsee_counter.h"
#include "qsee_timer.h"

#ifdef APP_SFS_L2_TEST
#include "tzcommon_entry.h"
#include "qsee_fs.h"
#include <string.h>
#else
#include "tzbsp_fs.h"
#endif


#define ETEST_DATA "dummy data for sfs test suite"
#define ETEST_DATA2 ". kdfjg sodj sdk gasdoi"

#define TZ_CM_MAX_NAME_LEN          256

int tz_app_counter(void)
{
  uint8 id_1[32]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16, \
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
  uint8 id_2[32]={16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, \
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
  uint8 id_3[32]={3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16, \
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
  int stat;
  uint64 value;
  unsigned long long time_1;
  unsigned long long time_2;

  do
  {
    time_1 = qsee_get_uptime();
    stat = qsee_create_counter(id_1, 0);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_create_counter(id_1, 0) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_create_counter(id_1, 0) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_create_counter time used %ld ",  time_2-time_1);

    value = 0;
    stat = qsee_query_counter(id_1, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_1) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_1, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1) );
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_query_counter time used %ld ",  time_1-time_2);

    stat = qsee_increment_counter(id_1);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_increment_counter(id_1) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_increment_counter(id_1) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_increment_counter time used %ld ",  time_2-time_1);

    value = 0;
    stat = qsee_query_counter(id_1, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_1) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_1, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_query_counter time used %ld ",  time_1-time_2);

    stat = qsee_create_counter(id_2, 0xFFFFFFFF);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_create_counter(id_2, 0xFFFFFFFF) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_create_counter(id_2, 0xFFFFFFFF) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_create_counter time used %ld ",  time_2-time_1);

    value = 0;
    stat = qsee_query_counter(id_2, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_2, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_query_counter time used %ld ",  time_1-time_2);

    stat = qsee_increment_counter(id_2);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_increment_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_increment_counter(id_2) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_increment_counter time used %ld ",  time_2-time_1);

    stat = qsee_increment_counter(id_2);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_increment_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_increment_counter(id_2) PASSED");
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_increment_counter time used %ld ",  time_1-time_2);

    value = 0;
    stat = qsee_query_counter(id_2, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_2, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_query_counter time used %ld ",  time_2-time_1);

    stat = qsee_create_counter(id_2, 1);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_create_counter(id_2) PASSED negative returned = %ld", stat);
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_create_counter(id_2) FAILED negative create");
      break;
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_create_counter time used %ld ",  time_1-time_2);

    value = 0;
    stat = qsee_query_counter(id_2, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_2, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_query_counter time used %ld ",  time_2-time_1);

    stat = qsee_increment_counter(id_2);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_increment_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_increment_counter(id_2) PASSED");
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_increment_counter time used %ld ",  time_1-time_2);

    value = 0;
    stat = qsee_query_counter(id_2, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_2, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_query_counter time used %ld ",  time_2-time_1);

    memset(&value, 0xFF, sizeof(uint64));
    stat = qsee_create_counter(id_3, value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_create_counter(id_3, 0xFFFFFFFFFFFFFFFF) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_create_counter(id_3, 0xFFFFFFFFFFFFFFFF) PASSED");
    }

    value = 0;
    stat = qsee_query_counter(id_3, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_3) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_3, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }

    stat = qsee_increment_counter(id_3);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_increment_counter(id_3) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_increment_counter(id_3) PASSED");
    }

    value = 0;
    stat = qsee_query_counter(id_3, &value);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_query_counter(id_3) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_query_counter(id_3, value[0]=0x%x, value[1]=0x%x)", *((uint32 *)(&value)), *((uint32 *)(&value)+1));
    }

    stat = qsee_delete_counter(id_1);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_delete_counter(id_1) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_delete_counter(id_1) PASSED");
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_delete_counter time used %ld ",  time_1-time_2);

    stat = qsee_delete_counter(id_2);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_delete_counter(id_2) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_delete_counter(id_2) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_delete_counter time used %ld ",  time_2-time_1);

    stat = qsee_delete_counter(id_3);
    if( stat != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, " qsee_delete_counter(id_3) FAILED! returned = %ld", stat);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, " qsee_delete_counter(id_3) PASSED");
    }

  }while(0);

  return stat;
}

int tz_app_sfs(void)
{
  int ret = 0;
  int fd = -1;
  uint8 * buf = NULL;
  uint32 len = 0;
  uint32 num_bytes = 0;
  char etest_sfs_dir[] = "/data/misc/sampleapp/sfs"; 
  char etest_sfs_file[] = "/data/misc/sampleapp/sfs/file.dat";
  char etest_sfs_file2[] = "/data/misc/sampleapp/sfs/file2.dat";
  unsigned long long time_1;
  unsigned long long time_2;
  
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- SFS UNIT TESTS ---------------------------------");
  
  
  do
  {
    len = (strlen(ETEST_DATA) + strlen(ETEST_DATA2) + 1);
    buf = (uint8*)qsee_malloc(sizeof(int8) *  len);
    if( buf == NULL )
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "Failed to create buffer! qsee_malloc() failed!");
      ret = -1;
      break;
    }
    buf[len - 1] = '\0';

    /* do not check return value becuase qsee_sfs_mkdir is deprecated */
    ret = qsee_sfs_mkdir( etest_sfs_dir );

    time_1 = qsee_get_uptime();
    fd = qsee_sfs_open( etest_sfs_file, O_RDWR | O_CREAT | O_TRUNC );
    if( fd == 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open( O_RDWR | O_CREAT | O_TRUNC ) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open time used %ld ",  time_2-time_1);

    ret = qsee_sfs_write( fd, ETEST_DATA, strlen(ETEST_DATA) );
    if( ret != strlen(ETEST_DATA) )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_write() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_write(%s) PASSED", ETEST_DATA);
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_write time used %ld ",  time_1-time_2);

    ret = qsee_sfs_read(fd, (char *)buf, 1);
    if(ret != 1)
    {
      ret = qsee_sfs_error(fd);
      if(ret == SFS_EOF)
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "SFS_EOF test PASSED errno = %d", ret);
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "SFS_EOF test FAILED errno = %d", ret);
        break;
      }
    }

    ret = qsee_sfs_close( fd );
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_close() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_close() PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_close time used %ld ",  time_2-time_1);

    fd = qsee_sfs_open( etest_sfs_file, O_RDWR | O_CREAT | O_APPEND );
    if( fd == 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open( O_RDWR | O_CREAT | O_APPEND ) PASSED");
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open time used %ld ",  time_1-time_2);

    ret = qsee_sfs_write( fd, ETEST_DATA2, strlen(ETEST_DATA2) );
    if( ret != strlen(ETEST_DATA2) )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_write() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_write(%s) PASSED", ETEST_DATA2);
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_write time used %ld ",  time_2-time_1);

    ret = qsee_sfs_close( fd );
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_close() FAILED! errno = %d ", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_close() PASSED");
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_close time used %ld ",  time_1-time_2);

    fd = qsee_sfs_open( etest_sfs_file, O_RDONLY );
    if( fd == 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open( O_RDONLY ) FAILED! errno = %d ", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open() PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open time used %ld ",  time_2-time_1);

    ret = qsee_sfs_getSize(fd, &num_bytes);
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_getSize() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      if(num_bytes != len -1)
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_getSize %ld != write %ld", num_bytes, len-1);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_getSize PASSED");
      }
    }

    ret = qsee_sfs_read( fd, (char*) buf, len-1 );
    if( ret != len-1 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_read() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_read(%s) PASSED", buf);
    }
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_read time used %ld ",  time_1-time_2);

    ret = qsee_sfs_close( fd );
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_close() FAILED! errno = %d ", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_close() PASSED");
    }

    time_1 = qsee_get_uptime();
    ret = qsee_sfs_rm( etest_sfs_file );
    if( ret != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_rm(etest_sfs_file) FAILED! returned = %ld", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_rm(etest_sfs_file) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_rm time used %ld ",  time_2-time_1);

    fd = qsee_sfs_open( etest_sfs_file2, O_RDONLY );
    if( fd == 0 )
    {
      ret = qsee_sfs_error(fd);
      if(ret == SFS_NO_FILE)
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open(%s,O_RDONLY) PASSED errno = %d", etest_sfs_file2, ret);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open(%s,O_RDONLY) FAILED! errno = %d", etest_sfs_file2, ret);
        break;
      }
    }


    fd = qsee_sfs_open( etest_sfs_file2, O_RDWR | O_CREAT | O_TRUNC );
    if( fd == 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open() FAILED! errno = ret", fd);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_open( O_RDWR | O_CREAT | O_TRUNC ) PASSED");
    }

    ret = qsee_sfs_getSize(fd, &num_bytes);
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_getSize() FAILED! errno = %d ", ret);
    }
    else
    {
      if(num_bytes != 0)
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_getSize %ld != 0", num_bytes);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_getSize PASSED");
      }
    }

    ret = qsee_sfs_write( fd, ETEST_DATA, strlen(ETEST_DATA) );
    if( ret != strlen(ETEST_DATA) )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_write() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_write(%s) PASSED", ETEST_DATA);
    }

    ret = qsee_sfs_getSize(fd, &num_bytes);
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_getSize() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      if(num_bytes != strlen(ETEST_DATA))
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_getSize %ld != %ld", num_bytes, strlen(ETEST_DATA));
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_getSize PASSED");
      }
    }

    ret = qsee_sfs_seek(fd, 0, SEEK_SET);
    if( ret == -1 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_seek SEEK_SET error = %d", ret);
      break;
    } 

    memset((void *)buf, 0, len);
    ret = qsee_sfs_read(fd, (char *)buf, 1);
    if(ret != 1)
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_read error errno = %d", ret);
      break;
    }
    
    //Compare seek results 
    ret = memcmp(buf, ETEST_DATA, 1);
    if ( ret )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_seek read error %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_seek read PASSED");
    }

    time_1 = qsee_get_uptime();
    ret = qsee_sfs_seek(fd, 0, SEEK_END);
    if( ret == -1 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_seek SEEK_END errno = %d", ret);
      break;
    } 
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_seek time used %ld ",  time_2-time_1);

    ret = qsee_sfs_seek(fd, 0, SEEK_CUR);
    if( ret == -1 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_seek SEEK_CUR errno = %d", ret);
      break;
    } 
    time_1 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_seek time used %ld ",  time_1-time_2);

    if (ret != strlen(ETEST_DATA))
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_seek Tell overall file size compare error %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_seek Tell overall file size passed");
    }


    ret = qsee_sfs_close( fd );
    if( ret != 0 )
    {
      ret = qsee_sfs_error(fd);
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_close() FAILED! errno = %d", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_close() PASSED");
    }

    time_1 = qsee_get_uptime();
    ret = qsee_sfs_rm( etest_sfs_file2 );
    if( ret != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_rm(etest_sfs_file2) FAILED! returned = %ld", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_rm(etest_sfs_file2) PASSED");
    }
    time_2 = qsee_get_uptime();
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_sfs_rm time used %ld ",  time_2-time_1);

    /* do not check return value becuase qsee_sfs_mkdir is deprecated */
    ret = qsee_sfs_rmdir( etest_sfs_dir );

  } while (0);
  
  /* Clean up */
  if (ret != 0 && fd != 0)
  {
    if (qsee_sfs_close( fd ))
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "Clean Up FAILED: Heap Memory Leaked", ret);
    }
  }
  if( buf != NULL )
  {
    qsee_free(buf);
    buf  = NULL;
  }  

  return ret;
}


#ifdef APP_SFS_L2_TEST
const char config_path[]="/persist/sfs.config";
const char anti_rollback_file[]="anti_rollback_sfs_file";
/* Note sfs.config format, a char must immediately follow data: */
/* write data:b  -> You want to write 100 bytes 'b' to anti_rollback_sfs_file */
/* read data:b  -> You expect to read 100 bytes 'b' out from anti_rollback_sfs_file */


int tz_app_sfs_anti_rollback(void)
{
  int ret = 0;
  int fd = -1;
  fs_stat  stat_info;
  char  *sfs_config_buf = NULL;
  boolean write_file = TRUE;
  char  *data;
  char  *test_buffer = NULL;
  int    len = 100;
  int    fd_sfs = -1;

  /* open, read, close test config file form HLOS file system */
  do
  {
    fd = open(config_path, O_RDONLY);
    if(fd >0)
    {
      /* get the status of the file */
      ret = lstat(config_path, &stat_info);
      if(!ret && stat_info.st_size <250)
      {
        sfs_config_buf = (char *)qsee_malloc(stat_info.st_size);
        if (!sfs_config_buf) 
        {
          QSEE_LOG(QSEE_LOG_MSG_DEBUG,"sfs_config_buf malloc failed");
          break;
        }
        (void)read(fd, (void *)sfs_config_buf, stat_info.st_size);
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_DEBUG, "/persist/sfs.config size=%d too big, ret=%d",stat_info.st_size,ret);
        break;
      }
      (void)close(fd);
      if(strstr((const char *)sfs_config_buf, "read"))
      {
        write_file = FALSE;
      }
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"No tz_app_sfs_anti_rollback test");
      break;
    }


    /* parse write, data, len*/
    data = strstr((const char *)sfs_config_buf, "data:");
    if (!data) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"data: is not set in /persist/sfs.config");
      break;
    }
    data+=5;
    test_buffer = (char *)qsee_malloc(len);
    if (!test_buffer) 
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG,"test_buffer malloc failed");
      break;
    }


    /* write result */
    if (write_file) 
    {
      memset((void *)test_buffer, *data, len);
      fd_sfs = qsee_sfs_open(anti_rollback_file, O_RDWR | O_CREAT | O_TRUNC );
      if( fd_sfs == 0 )
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open() %s for write FAILED! ",anti_rollback_file );
        break;
      }
      ret = qsee_sfs_write(fd_sfs, test_buffer, len);
      if(ret != len)
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_write() %s FAILED! ",anti_rollback_file );
        break;
      }
      else
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "%d bytes %c is written to %s for sfs anti-rollback test ",len, *data, anti_rollback_file );
        break;
      }
    }
    else    /* read result */
    {
      fd_sfs = qsee_sfs_open(anti_rollback_file, O_RDONLY );
      if( fd_sfs == 0 )
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_open() %s for read FAILED! ",anti_rollback_file );
        break;
      }
      ret = qsee_sfs_read(fd_sfs, test_buffer, len);
      if(ret != len)
      {
        QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_sfs_write() %s FAILED! ",anti_rollback_file );
        break;
      }
      else
      {
        int i;
        char *read_data = test_buffer;
        for(i = 0; i<len; i++)
        {
          if(*read_data != *data)
          {
             QSEE_LOG(QSEE_LOG_MSG_ERROR, "read data %c != %c from %s for sfs anti-rollback test ", *test_buffer, *data,  anti_rollback_file );
             break;
          }
          read_data++;
        }
        if(i == len)
          QSEE_LOG(QSEE_LOG_MSG_ERROR, "%d bytes %c is read from %s for sfs anti-rollback test ",len, *test_buffer, anti_rollback_file );
      }
    }


  }while(0);

  if(fd != -1)
    close(fd);
  if(fd_sfs != -1)
    qsee_sfs_close(fd_sfs);
  if(sfs_config_buf)
    qsee_free(sfs_config_buf);
  if(test_buffer)
    qsee_free(test_buffer);

  return 0;
}

#endif

