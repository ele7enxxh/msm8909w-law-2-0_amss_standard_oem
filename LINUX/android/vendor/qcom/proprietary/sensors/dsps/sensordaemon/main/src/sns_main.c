/*============================================================================
  @file sns_main.c

  @brief Implements the "main" function for the Linux Android sensors library.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#include "sns_smgr_api_v01.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_common.h"
#include "sns_main.h"
#include "sns_init.h"

#include "qmi_client.h"
#include "qmi_idl_lib.h"

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <inttypes.h>
#include <pthread.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <utime.h>
#include <cutils/properties.h>

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/

/* Timeout in ms for SMRG service */
#define SNS_SMGR_SVC_TIMEOUT_MS 10000

/* Android defines PRIxPTR incorrectly. Fix it here */
#ifdef SNS_LA
#  undef PRIxPTR
#  define PRIxPTR "x"
#endif /* SNS_LA */

/*============================================================================
  Type Declarations
  ============================================================================*/

typedef struct sns_main_data_s {
  /** If set to false, do not accept incoming connecitons, and close all existing
   * client connections */
  bool enabled;
} sns_main_data_s;

/*============================================================================
 * Global Data Definitions
 ============================================================================*/

/*============================================================================
  Static Variable Definitions
  ============================================================================*/
static sem_t            sns_daemonctrl_sem;

//init sensor debug level
uint32_t sns_debug_level = 0;

/*============================================================================
  Function Definitions
  ============================================================================*/

/*===========================================================================

  FUNCTION:   sns_main_daemonize

  ===========================================================================*/
/*!
  @brief Turns the current process into a background daemon.

  @return
  None.

  @sideeffects
  Many associated with becoming a daemon
  - Forks off a child process (the child returns, the parent exits)
  - Sets the umask to 0
  - Creates a new SID
  - Changes the path to root
  - Closes all of the stdio handles.

*/
/*=========================================================================*/
static void
sns_main_daemonize( void )
{
  pid_t pid;
  pid_t sid;
  int error;

  ATRACE_BEGIN("SensorDaemon:sns_main_daemonize");
  error = getppid();
  if( 1 == error ) {
    /* Parent is init; already a daemon */
    ATRACE_END();
    return;
  }

  /* Fork off the daemon process */
  pid = fork();
  if( 0 > pid ) {
    /* uh oh... */
    ATRACE_END();
    exit( 1 );
  } else if( pid > 0 ) {
    /* This is the parent process. Exit here, and let the child continue */
    printf("Background daemon process starting... PID %d\n", pid);
    ATRACE_END();
    exit( 0 );
  }

  /* Set the file mask */
  umask(0);

  /* Create a SID to disassociate ourselves from the parent process &
     terminal */
  sid = setsid();
  if( sid < 0 ) {
    ATRACE_END();
    exit(2);
  }

  /* Set the cwd to root, so we don't keep subdirs locked */
  error = chdir("/");
  if( 0 > error ) {
    ATRACE_END();
    exit(3);
  }

  /* Redirect stdio to /dev/null */
  freopen( "/dev/null", "r", stdin);
  freopen( "/dev/null", "w", stdout);
  freopen( "/dev/null", "w", stderr);
  ATRACE_END();
}

/*===========================================================================

  FUNCTION:   sns_main_get_usr

  ===========================================================================*/
/*!
  @return The passwd struct corresponding to either the sensors user or nobody.
*/
/*=========================================================================*/
static struct passwd*
sns_main_get_usr()
{
  static struct passwd *sensors_pwent = NULL;

  ATRACE_BEGIN("SensorDaemon:sns_main_get_usr");
  if( NULL != sensors_pwent )
  {
    ATRACE_END();
    return sensors_pwent;
  }
  else
  {
    sensors_pwent = getpwnam( SNS_USERNAME );
  }

  if( NULL == sensors_pwent ) /* Can't find sensors user; try 'nobody' instead */
  {
    sensors_pwent = getpwnam( USERNAME_NOBODY );

    if( NULL == sensors_pwent ) {
      SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
          "Unable to get user entry for 'sensors'" );
    } else {
      SNS_PRINTF_STRING_HIGH_0( SNS_DBG_MOD_APPS_MAIN,
         "Usr 'sensors' does not exist. Switching to 'nobody' instead." );
    }
  }
  ATRACE_END();
  return sensors_pwent;
}

/*===========================================================================

  FUNCTION:   sns_main_get_grp

  ===========================================================================*/
/*!
  @return The group struct corresponding to either the sensors group or nobody.
*/
/*=========================================================================*/
static struct group*
sns_main_get_grp()
{
  static struct group *sensors_grent = NULL;

  ATRACE_BEGIN("SensorDaemon:sns_main_get_grp");
  if( NULL != sensors_grent )
  {
    ATRACE_END();
    return sensors_grent;
  }
  else
  {
    sensors_grent = getgrnam( SNS_GROUPNAME );
  }

  if( NULL == sensors_grent ) /* Can't find sensors group; try 'nobody' instead */
  {
    sensors_grent = getgrnam( GROUPNAME_NOBODY );

    if( NULL == sensors_grent )
    {
      SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
          "Unable to get group entry for 'sensors' or 'nobody'" );
      sensors_grent = NULL;
    }
    else
    {
      SNS_PRINTF_STRING_HIGH_0( SNS_DBG_MOD_APPS_MAIN,
          "Grp 'sensors' does not exist. Switching to 'nobody' instead." );
    }
  }

  ATRACE_END();
  return sensors_grent;
}

/*===========================================================================

  FUNCTION:   sns_main_drop_root

  ===========================================================================*/
/*!
  @brief Drops root permissions, and sets uid/gid to SNS_USERNAME/SNS_GROUPNAME

  @return
  TRUE: Error
  FALSE: No error

*/
/*=========================================================================*/
static bool
sns_main_drop_root( void )
{
  struct passwd *sensors_pwent; /* Sensors password entry */
  struct group *sensors_grent; /* Sensors group entry */

  bool rv = false; /* return value */
  int error;

  ATRACE_BEGIN("SensorDaemon:sns_main_drop_root");
  if( getuid() != 0 ) {
    /* Not root. No need to drop root privs */
    rv = false;
  } else {
    sensors_pwent = sns_main_get_usr();
    sensors_grent = sns_main_get_grp();

    if( NULL == sensors_pwent || NULL == sensors_grent ) {
      SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
          "Exiting! Unable to get user or group entry for 'sensors' or 'nobody'" );
      rv = true;
      ATRACE_END();
      return rv;
    }

    error = setresgid( sensors_grent->gr_gid,
                       sensors_grent->gr_gid,
                       sensors_grent->gr_gid );
    if( -1 == error ) {
      SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                                 "Exiting! Unable to set gid to %d",
                                 sensors_grent->gr_gid );
      rv = true;
    }
    error = setresuid( sensors_pwent->pw_uid,
                       sensors_pwent->pw_uid,
                       sensors_pwent->pw_uid );
    if( -1 == error ) {
      SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                                 "Exiting! Unable to set uid to %d",
                                 sensors_pwent->pw_uid );
      rv = true;
    }
  }
  ATRACE_END();
  return rv;
}


/*===========================================================================

  FUNCTION:   sns_main_setup

  ===========================================================================*/
/*!
  @brief Initializes sensor daemon.

  Creates the main server socket to listen for incomming connections.

  @return
  Pointer to the main data structure representing all clients.

*/
/*=========================================================================*/
static sns_main_data_s*
sns_main_setup( void )
{
  sns_main_data_s *srv_data;

  ATRACE_BEGIN("SensorDaemon:sns_main_setup");
  srv_data = malloc( sizeof(*srv_data) );
  if( NULL == srv_data ) {
    SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN, "malloc error" );
    ATRACE_END();
    return NULL;
  }

  memset( srv_data, 0, sizeof(*srv_data) );

  srv_data->enabled = true;

  ATRACE_END();
  return srv_data;
}

/*===========================================================================

  FUNCTION:   sns_main_exit

  ===========================================================================*/
/*!
  @brief Causes the main thread to exit.
*/
/*=========================================================================*/
void sns_main_exit( void )
{
  SNS_PRINTF_STRING_FATAL_0( SNS_DBG_MOD_APPS_MAIN, "sns_main_exit called unlock daemon to exit");
  sem_post( &sns_daemonctrl_sem );
  return ;
}

/*===========================================================================

  FUNCTION:   sns_smgr_svc_error_cb

  ===========================================================================*/
/*!
  @brief Handles the QCCI error events, this is called by the
  QCCI infrastructure when the service is no longer available.

  Notified when SMGR service disconnected to detect
  ADSP going down. Exit the Sensors Daemon upon ADSP reset

  @param[i] user_handle: Unused
  @param[i] error: Unused
  @param[i] err_cb_data: Unused

  @return void
*/
/*=========================================================================*/
static void
sns_smgr_svc_error_cb
(
  qmi_client_type user_handle,
  qmi_client_error_type error,
  void *err_cb_data
)
{
  UNREFERENCED_PARAMETER(user_handle);
  UNREFERENCED_PARAMETER(err_cb_data);

  SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                             "SMGR service error %d received",
                             error );

  if( error == QMI_SERVICE_ERR )
  {
    SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
                             "ADSP reset. Exit Sensors Daemon!!!" );
    sns_main_exit();
  }
}

/*===========================================================================

  FUNCTION:   sns_monitor_smgr_restart

  ===========================================================================*/
/*!
  @brief Handles SMGR restart for SSR.

  @return void
*/
/*=========================================================================*/
static void
sns_monitor_smgr_restart( void )
{
  qmi_client_error_type qmi_err;
  qmi_service_info service_info;
  qmi_client_type smgr_client;
  qmi_idl_service_object_type service;
  qmi_client_type notifier_handle;
  qmi_cci_os_signal_type os_params;

  SNS_PRINTF_STRING_MEDIUM_0( SNS_DBG_MOD_APPS_MAIN,
                              "Register for SMRG service");
  service = SNS_SMGR_SVC_get_service_object_v01();
  if( service == NULL )
  {
    SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
                               "Error: SMGR service is NULL" );
    return ;
  }

  qmi_err = qmi_client_notifier_init( service, &os_params, &notifier_handle );
  if( QMI_NO_ERR != qmi_err )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                               "qmi_client_notifier_init for SMGR failed, err=%d",
                               qmi_err );
    return ;
  }
  SNS_PRINTF_STRING_MEDIUM_0( SNS_DBG_MOD_APPS_MAIN,
                              "Waiting for SMGR service up" );
  QMI_CCI_OS_SIGNAL_WAIT( &os_params, SNS_SMGR_SVC_TIMEOUT_MS );
  if( QMI_CCI_OS_SIGNAL_TIMED_OUT(&os_params) )
  {
    SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
                               "Timeout waiting for SMGR service. Exit sensors daemon!" );
    sns_main_exit();
  }
  else
  {
    SNS_PRINTF_STRING_MEDIUM_0( SNS_DBG_MOD_APPS_MAIN,
                                "Get SMGR servive info" );
    qmi_err = qmi_client_get_any_service( service, &service_info );
    if( qmi_err != QMI_NO_ERR )
    {
      SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                                 "qmi_client_get_any_service for SMGR failed, err=%d",
                                 qmi_err );
    }
    else
    {
      SNS_PRINTF_STRING_MEDIUM_0( SNS_DBG_MOD_APPS_MAIN,
                                  "Initialize client for SMRG" );
      qmi_err = qmi_client_init( &service_info, service, NULL, NULL, NULL, &smgr_client );
      if( QMI_NO_ERR != qmi_err )
      {
        SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                                   "qmi_client_init for SMGR failed, err=%d",
                                   qmi_err);
      }
      else
      {
        SNS_PRINTF_STRING_MEDIUM_0( SNS_DBG_MOD_APPS_MAIN,
                                    "Register for SMGR error notification" );
        qmi_err = qmi_client_register_error_cb( smgr_client, sns_smgr_svc_error_cb, NULL );
        if( QMI_NO_ERR != qmi_err )
        {
          SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN,
                                     "qmi_client_register_error_cb for SMGR failed, err=%d",
                                     qmi_err );
          qmi_client_release( smgr_client );
        }
      }
    }
  }

  qmi_client_release( notifier_handle );
}

/*===========================================================================

  FUNCTION:   main

  ===========================================================================*/
/*!
  @brief "Main" function for the sensor daemon process. Handles starting
  the process, initializing sockets and sensor1 API, and blocking for
  incoming data on sockets.

  @param[i] argc: Count of arguments on the command line.

  @param[i] argv: Array of strings contaning command line arguments.

  @return
  0 - no error
*/
/*=========================================================================*/
int
main( int argc, char *argv[] )
{
  sns_main_data_s *srv_data;
  bool error = FALSE;

  if( 'd' == getopt( argc, argv, "d" ) ) {
    /* Put ourselves in a background daemon process */
    sns_main_daemonize();
  } else {
    printf("Use option \"-d\" to run as background process.\n");
    printf("Running in foreground...\n");
  }

  char property_val[PROPERTY_VALUE_MAX] = {0};
  property_get("debug.sns.daemon.ftrace", property_val, "0");
  sns_debug_level = atoi(property_val);
  property_val[0] = '\0';

  sns_init();

  (void)sem_init( &sns_daemonctrl_sem, 0, 0 );

  /* Initialize server */
  srv_data = sns_main_setup();

  if( NULL == srv_data ) {
    SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
                               "Exiting! Can't setup server socket" );
    (void)sem_destroy( &sns_daemonctrl_sem );
    exit(4);
  }

  sns_monitor_smgr_restart();

  /* Drop all root privs */
  error = sns_main_drop_root();
  if( error != false ) {
    SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_APPS_MAIN,
                               "Error dropping root privs. Exiting" );
    (void)sem_destroy( &sns_daemonctrl_sem );
    exit(5);
  }

  for( ;; ) {
    SNS_PRINTF_STRING_HIGH_0( SNS_DBG_MOD_APPS_MAIN, "sns_daemonctrl_sem! waiting %d");
    sem_wait( &sns_daemonctrl_sem );
    SNS_PRINTF_STRING_HIGH_0( SNS_DBG_MOD_APPS_MAIN, "sns_daemonctrl_sem! Done %d");
    break;
  }

  SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_APPS_MAIN, "Exiting! Err %d",
                             error );
  (void)sem_destroy( &sns_daemonctrl_sem );

  return 0;
}
