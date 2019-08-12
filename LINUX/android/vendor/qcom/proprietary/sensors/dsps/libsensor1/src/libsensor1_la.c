/*=============================================================================
  @file libsensor1_la.c

  @brief LA-specific implementation of the sensor1 library.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <grp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <sys/system_properties.h>

#include "sensor1.h"
#include "libsensor1_osa.h"
#include "libsensor1.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/
/* Definitions for logging */
#define LIBSENSOR1_DEBUG_PROP_NAME "debug.qualcomm.sns.libsensor1"

/* Retry a standard library command if the error code is EINTR */
#define  RETRY_ON_EINTR(ret,cond) \
do {                              \
  ret = (cond);                   \
} while (ret < 0 && errno == EINTR)

#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define FD_ADD( fd, fdset, max )                \
  do {                                          \
    FD_SET((fd),(fdset));                       \
    (max) = MAX((max),(fd));                    \
  } while(0);

/* file path for sensors settings file */
/* If file exists and contains '0', then disable external sensor clients */
#define SENSORS_SETTINGS_DIR  "/persist/sensors"
#define SENSORS_SETTINGS_FILE "sensors_settings"

/*============================================================================
  Static Variable Definitions
  ============================================================================*/

/* Notifier for changes to the sensor_settings file */
static int inotify_fd;

static int settings_fd = -1;

/* Whether access to sensor1 has been enabled */
static bool sensor1_enabled;
/* Mutex is held permanently whenever sensor1_enabled is false */
static OS_EVENT *sensor1_enabled_mutex;

/*============================================================================
  Static Function Definitions
  ============================================================================*/

/**
 * Checks the settings file to see if sensors are enabled.
 */
static void
check_sensors_enabled( void )
{
  char enabled;
  uint8_t err;

  if( -1 == settings_fd ) {
    settings_fd = open( SENSORS_SETTINGS_DIR "/" SENSORS_SETTINGS_FILE, O_RDONLY | O_NOFOLLOW );
  }
  if( -1 != settings_fd ) {
    /* File exists. Set up inotify to be notified of file changes */
    inotify_add_watch( inotify_fd,
                       SENSORS_SETTINGS_DIR "/" SENSORS_SETTINGS_FILE,
                       IN_CLOSE_WRITE );

    /* Read value from settings file, and enable/disable as needed */
    lseek( settings_fd, 0, SEEK_SET );
    if( 1 == read( settings_fd, &enabled, 1 ) ) {
      if( enabled == '1' ) {
        LOG_ERROR("%s: Sensors enabled = true", __func__);
        sensor1_enabled = true;
        sns_os_mutex_post( sensor1_enabled_mutex );
        return;
      }
    } else {
      close( settings_fd );
      settings_fd = -1;
    }
  } else {
    /* File does not exist */
    LOG_ERROR("%s: open error: settings file \"%s\", errno %d", __func__,
        SENSORS_SETTINGS_DIR "/" SENSORS_SETTINGS_FILE, errno);
    if( -1 == inotify_add_watch( inotify_fd, SENSORS_SETTINGS_DIR, IN_CREATE ) ) {
      /* Error creating the watch */
      LOG_ERROR("%s: inotify error: settings path \"%s\", errno %d", __func__,
          SENSORS_SETTINGS_DIR, errno);
    }
  }
  LOG_ERROR("%s: Sensors enabled = false", __func__);
  sensor1_enabled = false;
  sns_os_mutex_pend( sensor1_enabled_mutex, 0, &err );
  sensor1_close_clients();
}

static void
inotify_thread( void *unused )
{
  UNREFERENCED_PARAMETER(unused);

  LOG_DEBUG("%s: Entered inotify thread", __func__);

  for( ;; ) {
    int sel_err;
    int max_fd = 0;
    fd_set read_fd;
    fd_set error_fd;

    FD_ZERO( &read_fd );
    FD_ZERO( &error_fd );
    FD_ADD( inotify_fd, &read_fd, max_fd );

    error_fd = read_fd;
    max_fd++;

    errno = 0;
    RETRY_ON_EINTR( sel_err, select( max_fd, &read_fd, (fd_set*)NULL, &error_fd,
                                     (struct timeval*) NULL) );
    if( sel_err < 0 ) {
      LOG_ERROR("%s: Error on select %i", __func__, sel_err);
    } else if( FD_ISSET( inotify_fd, &read_fd ) ) {
      /* Change in settings file */
      char buf[500];
      struct inotify_event *evt = (struct inotify_event*)buf;
      read(inotify_fd, evt, 500);
      if(evt->mask & IN_IGNORED) {
        /* Previous watch was removed. Nothing to do here */
      } else if( evt->len == 0 ||
                 ( (evt->mask & IN_CREATE) &&
                   (0 == strncmp( evt->name, SENSORS_SETTINGS_FILE, evt->len)))) {
        inotify_rm_watch( inotify_fd, evt->wd );
        check_sensors_enabled();
      }
    }
  }

  LOG_DEBUG("%s: Exiting inotify thread", __func__);
}

/*============================================================================
  Public Function Definitions
  ============================================================================*/

bool
sensor1_enabled_check( void )
{
  return sensor1_enabled;
}

void
sensor1_enabled_wait( void )
{
  uint8_t err;

  sns_os_mutex_pend( sensor1_enabled_mutex, 0, &err );
  sns_os_mutex_post( sensor1_enabled_mutex );
}

void
sensor1_osa_init_once( void )
{
  uint8_t err;
  int debug_prop_len;
  char debug_prop[ PROP_VALUE_MAX ];

  debug_prop_len = __system_property_get( LIBSENSOR1_DEBUG_PROP_NAME, debug_prop );
  if( 1 == debug_prop_len ) {
    switch( debug_prop[0] ) {
      case '0':
        sensor1_log_level = LOG_LEVEL_DISABLED;
        break;
      case '1':
        sensor1_log_level = LOG_LEVEL_ALL;
        break;
      case 'v':
      case 'V':
        sensor1_log_level = LOG_LEVEL_VERBOSE;
        break;
      case 'd':
      case 'D':
        sensor1_log_level = LOG_LEVEL_DEBUG;
        break;
      case 'i':
      case 'I':
        sensor1_log_level = LOG_LEVEL_INFO;
        break;
      case 'w':
      case 'W':
        sensor1_log_level = LOG_LEVEL_WARN;
        break;
      case 'e':
      case 'E':
        sensor1_log_level = LOG_LEVEL_ERROR;
        break;
      default:
        break;
    }
    LOG_ERROR("%s: Setting log level to %d", __func__, sensor1_log_level);
  } else if( debug_prop_len > 1 ) {
    LOG_ERROR("%s: Invalid value for %s: %s. Enabling all logs", __func__,
              LIBSENSOR1_DEBUG_PROP_NAME, debug_prop );
    sensor1_log_level = LOG_LEVEL_ALL;
  }

  sensor1_enabled = true;
  sensor1_enabled_mutex = sns_os_mutex_create( 0, &err );
  if( 0 != err ) {
    LOG_ERROR("%s: Cannot create mutex %i", __func__, err);
    SNS_ASSERT(false);
  }

  inotify_fd = inotify_init();
  if( -1 == inotify_fd ) {
    LOG_ERROR( "%s: inotify_init error %d", __func__, errno );
    SNS_ASSERT(false);
  }

  check_sensors_enabled();
  err = sns_os_task_create( inotify_thread, NULL, NULL, 0 );
  if( 0 != err ) {
    LOG_ERROR("%s: Error spawning WOULDBLOCK thread %i", __func__, err);
  }
}
