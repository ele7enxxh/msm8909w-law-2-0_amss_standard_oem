/*====*====*====*====*====*===*====*====*====*====*====*====*====*====*====*/
/*
       R U N T I M E   D E V I C E   M A P P E R   S E R V I C E S


GENERAL DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Runtime Device Mapper (RDM).

EXTERNALIZED FUNCTIONS

  rdm_init()
    Initializes the Runtime Device Mapper Services. This function must
    be called before other functions of this module.

  rdm_get_prev_ports()
    Read in the port mappings last know by RDM from EFS.

  rdm_issue_opens()
    RDM issues opens to SRVCs based on last mapping before power down. If there
    is no EFS, the default mapping of DS->U1 and DIAG->U2 (if U2 defined) is
    performed.

  rdm_assign_port()
    Request RDM assign a specified application to the specified device.

  rdm_assign_port_tmp()
    Same function as rdm_assign_port(), except the change is non persistant.
   Restarting the phone will cause the port mapping to set to last known value.

  rdm_close_device()
    This function will close a device used by a service.

  rdm_notify()
    Notify RDM of request completion/status.

  rdm_get_device()
    Query RDM for the device that is currently assigned to the specified
    port (if any).

  rdm_get_multi_dev()
    Query the Runtime Device Mapper for the ports/devices currently being
    used by the specified application/service.

  rdm_get_service()
    Query the Runtime Device Mapper for the application/service that is
    currently assigned to the specified port/device (if any).

  rdm_get_valid_ports()
    Query the RDM for the valid devices the specified application can map to.

  rdm_register_open_func()
    Function for Service Tasks to register their open function. This open
    function is what RDM calls to instruct a Service to open a specified port.

  rdm_register_close_func()
    Function for Service Tasks to register their close function. This close
    function is what RDM calls to instruct a Service to close it's open port.

  rdm_register_func_multi_dev()
    Function for Service Tasks to register their open/close functions.  The
    functions are what RDM calls to instruct a Service to open/close device.

  rdm_get_device_str()
    Query the RDM for the ASCII string that corresponds to the
    rdm_device_enum_type specified.

  rdm_is_srv_holding_dev()
    Test if a service is using a device.

  rdm_set_srv_compat_mask()
    Set device compatible mask for the service from
    (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which
    device this service can use.

  rdm_is_srv_dev_compat()
    Test if a service and a device is compatible. That is whether the service
    can use the device.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  rdm_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 2000-2012,2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/rdevmap.c_v   1.37.1.2   16 May 2003 14:33:04   wxie  $
  $Header: //components/rel/core.mpss/3.5.c12.3/wiredconnectivity/rdevmap/src/rdevmap.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/12   tl      Added USB_CSVT
12/06/11   tl      Added USB_SER4
10/28/11   tl      Added A2 mux rmnet ports 8 and 9
01/10/11   amv     Added RMNET5 port.
12/20/10   amv     Added safe string copy.
06/30/10   amv     Replaced INTLOCK/TASKLOCK with rex critical section
04/14/09   amv     Added SDIO MUX Rmnet devices
02/10/09   amv     Added dynamic portmapping with text file, removed RDM 2.0 dead code
01/21/09   amv     Added synchronization for remote opens and rdm_get_prev_opens
                   for persistency
12/15/09   amv     removed RDM_SKIP_FIRST_ISSUE_OPEN feature to sync. processor,
                   Added seperate issue_opens for remote and local services
                   Closed RDM file handle after modification.
12/1009    amv     Added A2 device registration
11/04/09   amv     Made QMI device mapping persistent over each boot up
10/30/09   amv     Added QMI devices for WWAN service, enabled out of order
                   registration for devices and services
10/01/09   amv     Added MUX physical device and Rmnet1,2 devices
09/02/09   amv     Moved from EFS1 to EFS2 apis
08/27/09   amv     Added RDM 3.0 feature to support more than 32 devices
06/23/09   amv     mainline 8200 target specific changes
05/14/08   rh      Klocwork cleanup2
05/12/08   rh      Include tramp.h
04/30/08   rh      Klocwork cleanup
02/20/08   jmm     Added messages to help debug failure return.
10/31/07   rh      FEATURE_BT_EXT_SOC_UART_TL has been replaced by
                   FEATURE_BT_SOC_UART_TL FEATURE_BT_EXT_HC_SOC has been
                   replaced by FEATURE_BT_SOC
10/08/07   ar      Added support for RDM_SIO_SMD_DATA11.
10/05/07   rh      Add support for modem restart
11/30/06   dk      Modified rdm_issue_opens() to work with modem only build.
11/15/06   TMR     Removing some RMNET featured code as per Jeff D.
10/11/06   rh      Adding support for FEATURE_HS_USB
08/29/06   ih      Default Diag to UART1 for WinMob build with
                   FEATURE_DIAG_MP_MODEM_ONLY defined
08/23/06   sg      Purge saved port map for RDM_BREW_EXTENDED_SRVC on bootup.
06/26/06   gd      Removed MMC feature hack and cleaned up some code
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support.
06/19/06   sb      Added the function to control clk and modem ISR for USB
27/06/06   rh      BT support for diag is now controled by
                   FEATURE_DIAG_SUPPORT_WIRELESS
05/01/06   vp      Changes for enabling multi ports for data
06/12/06   dsn     Support to use UART1DM for BT SoC support
25/05/06   rh      Add rdm_assign_port_tmp function for non-persistant port
                   assignment.  Code merged from toddb.
01/04/06   ptm     Restore SMEM code, removed RPCMP and DIAG_MP services and
                   devices. Fixed RVCT warnings.
11/18/05   vs      Removed / supressed lint warnings
08/19/05   cp      Support for APS virtual port
06/05/05   TMR     Merged in MSM7500 code
05/09/05   cc      Updated rdm_assign_port() and rdm_close_device() to call
                   the callback with device, as opposite to RDM_NULL_DEV,
                   once found RDM state machine is busy.
04/15/05   tmr     Changed name of SMEM data device to APPS uP
04/06/05   cc      Corprate code-review comments.
                   Change LOCAL to static
                   Prevent OEM services request for non-existing NET_WWAN or
                   USB_SER2 port.
                   Mark NET_WWAN unavailable if FEATURE_DATA_RM_NET not defined.
03/08/05   cc      Fixed Typo of FEATURE_DATA_RM_NET in one place.
03/08/05   cc      Merged in OSIFDev_Dec04 branck.
2/11/05     nrh    Added conditional port map file name depending on build (apps only,
                   modem only, dual processor)
01/11/05   cc      Fixed function rdm_update_srv_dev_compat_table().
12/23/04   cc      Rename USB_CDC, USB_DIAG, USB_NMEA device to be USB_MDM, USB_SER1
                   and USB_SER2 respectively.
                   Added FEATURE_DATA_RM_NET support.
11/24/04   cc      Added support for RDM_USB_NET_WWAN_DEV and RDM_DATA_WWAN_SRVC.
01/25/05   cc      Purge saved Part map for BREW_SRVC.
10/20/04   ptm     Added default connection for RPC SM service and cleaned up
                   some white space and comments.
10/20/04   adm     Added support for featurization of UART1.
                   Added support for multiple DS pipes.
10/05/04   jtl     Allow Bluetooth HCI mode to use USB
06/15/04   cc      Added FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB
04/20/04   pn      Added support for BT_SPP port.
08/15/03   cc      Disallow any service to be mapped to RDM_MUSB_MMC_DEV at
                   boot up, i.e. rdm_get_prev_ports().
07/01/03   cc      Set RDM_NULL_DEV_BM as default value to rdm_srv_dev_compat_tab[]
                   for all OEM services.
04/08/02   wxie    Protect RDM state in critical section, add capability
                   to open multiple ports for a service, add new service
                   easily, and other internal RDM changes.
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support.
12/03/02   cc      Added back accidently ommitted callback calls in
                   rdm_adssign_port() when return(TRUE).
11/22/02   ltan    Replaced FEATURE_MMGPS with FEATURE_MMGPS_CSD, and added
                   sanity check for RDM_LSM_CSD_DEV in rdm_get_prev_ports().
10/11/02   cc      Changed rdm_get_prev_ports() to preserv DIAG on USB_DIAG.
                   This fixes CR25022.
08/24/02   ry      Updated NMEA featurization by adding FEATURE_MMGPS
07/15/02   ry      Added support for the LSM_CSD port
06/17/02   cc      Removes feature wrapping around RDM_RPC_SRVC in all tables
                   and functions.
                   Check rdm_configuration_table[][] see if each service is
                   available when read port mapping from EFS.
                   Added funciton rdm_remove_dup_mappings() and call this
                   function from rdm_issue_opens().
                   All above is to fix CR23030
03/26/02   cc      rdm_cinfiguration_table[RPC][USB_CDC] define is dependent on
                   if BT mode or USB mode is used.  Cofigure ...[RPC][..] in
                   both sections of rdm_set_bt_mode().
                   Also Added rdm_configuration_table[RPC][UART3] settings.
03/08/02   cc      function rdm_data_got_atqcdmg() returns immediately
                   as long as #FEATURE_USB_DIAG is defined.
03/05/02   cc      function rdm_data_got_atqcdmg() returns immediately
                   if DIAG is on USB_DIAG port.
02/06/02   cc      Add third UART supporting.
01/09/02   cc      Add ONCRPC service supporting.
                   In rdm_set_bt_mode(), configuration for NMEA and ONCRPC
                   services are independent of bt_mode.
12/04/01   cc      Change the function name rdm_data_got_at$qcdmg()
                   to rdm_data_got_atqcdmg() to avoid future headach.
                   Update EFS when DS and DIAG getting into dance
                   or getting out of dance completed.
                   Preserve the DIAG on USB_CDC info in rdm_get_prev_ports.
10/16/01   cc      Merge in mods of SIMBA from MSM5200_SIO.00.00.17, includes
                   following 1 item.
08/13/01   cc      Add support for FEATURE_DATA.
08/09/01   cc      Removed function rdm_mc_direct_opens() which is not used.
08/06/01   cc      Merge gpsOne rdevmap.c -r1.3 to add NMEA support.
07/23/01   dwp     Don't save RDM_MMC_SRVC port mappings to EFS.
06/15/01   dwp     Special handling of when DS & DIAG dance over the USB CDC
                   endpoints. Make a call to sio_usb_diag_over_data() when
                   going into and coming out of the dance.
05/08/01   dwp     Implement smarter to_data_mode logic. Also, don't save port
                   mappings to EFS when in the dance.
03/30/01   dwp     Make rdm_get_prev_ports() more robust for devices coming &
                   going by build features compared to what's read from EFS.
03/28/01   dwp     Cleaned up rdm_get_device_str().
03/26/01   dl      Add support for SMEM_DS port.
03/26/01   dwp     Fix some intializations for HCI and DS in rdm_set_bt_mode.
03/22/01   dwp     FEATURE_DS_MAPPABLE_TO_UART2 is now required to be able to
                   have DS mappable to U2. This is in addition to
                   FEATURE_SECOND_UART.
03/16/01   dwp     Removed restriction of DIAG not being able to boot to NULL.
03/09/01   dwp     Added logic in rdm_get_prev_ports() to not let DIAG
                   boot to NULL. Also DIAG can't boot to USB CDC ACM.
03/08/01   dwp     Added logic in rdm_get_valid_ports() to show if DIAG is
                   on USB when DS & DIAG are in the dance on CDC ACM.
02/28/01   dwp     Merge in Bama mods from MSM5105_SIO.00.00.09.
02/09/01   dwp     Add logic to handle port mappings read from EFS to devices
                   that may not be allowed in the current build.
01/31/01   dwp     Add call to rdm_update_efs() in rdm_issue_opens().
12/21/00   dwp     Add portmap persistence across resets using EFS.
12/14/00   dwp     Need to display HCI mapping options even in USB mode.
12/13/00   dwp     RDM wasn't recording the callback pointer when a service
                   was assigned to the NULL device.
12/12/00   dwp     RDM must handle to_data_mode calls when not in the dance.
12/11/00   dwp     Add test for FEATURE_SECOND_UART in rdm_init().
12/06/00   dwp     Add call to rdm_reset_state_machine() in rdm_init().
11/27/00   dwp     Change SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_NA. Mapped
                   BT_HCI_SRVC to U1,U2 only. Use some up front #defines for
                   devices to make rdm_set_bt_mode() easier to read/maintain.
11/16/00   dwp     Added UI interfaces and updated comments.
11/14/00   dwp     Created module.

===========================================================================*/



/*===========================================================================

                            INCLUDE FILES


===========================================================================*/
#include "rdevmap.h"
#include "rdevmap_v.h"
#include <stringl/stringl.h>
#include "stdio.h"
#include "comdef.h"

#include "err.h"
#include "string.h"
#include "assert.h"
#include "rex.h"

#include "fs_public.h"            /* EFS2 file support*/
#include "fs_errno.h"

#if defined (FEATURE_USB_ON_APPS_PROC) && !defined (RDM_SERVER_ON_ALL_PROC)
#include "oncrpc.h"
#include "oncrpc_proxy.h"
#include "remote_apis.h"
#include "clkregim.h"
#include "tramp.h"
#endif /*FEATURE_USB_ON_APPS_PROC*/

#include "DALFramework.h"         /* DALSys_Malloc buffer for RDM config item */


/*===========================================================================
  Debugging support for this file
===========================================================================*/
#ifdef DEBUG_RDM
#define DEBUG( x )  { x }
#else
#define DEBUG( x )
#endif /* DEBUG_RDM */

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/

/*===========================================================================
                                Defines/Constants
===========================================================================*/

#if defined(FEATURE_USB) || defined(FEATURE_HS_USB) || (defined(T_MDM8200) && defined(IMAGE_QDSP6_PROC))
#define RDM_USES_FSHS_USB
#endif

#define RDM_DEV_ALL (-1)                ///< used to specify all RDM devices

#define MAX_DEVICE_NAME_LEN 13          ///< Device name string length
#define RDM_CONFIG_ITEM_FILE_SIZE  2048

/*-------------------------RDM FiLe parameters--------------------------*/
#define RDM_FILE_NAME "rdm_config.csv"  ///< A name of RDM port map file on EFS file system
#define RDM_FILE_FORMAT 2               ///< A version number for file format
                                        /// Version 2 relies on commas only, no spaces, for separation of data values.
                                        /// Version 2 also does not store a mapping fm rdm_map unless it is persistent
                                        /// and has one of default/current services set to other than RDM_NULL_SRVC.
#define RDM_FILE_MAX_COL 4              ///< Maximum columns supported in given file format

#define RDM_COL0_ID    0                ///< Column 0 ID
#define RDM_COL0_NAME "ID"              ///< Column 0 title
#define RDM_COL0_WIDTH 4                ///< Column 0 Width in characters

#define RDM_COL1_ID    1                ///< Column 1 ID
#define RDM_COL1_NAME "Device Name"     ///< Column 1 title
#define RDM_COL1_WIDTH 40               ///< Column 1 Width in characters,
                                        ///< Max device enum name length

#define RDM_COL2_ID    2                ///< Column 2 ID
#define RDM_COL2_NAME "Mapped Service"  ///< Column 2 title
#define RDM_COL2_WIDTH 32               ///< Column 2 ID  Width in characters,
                                        ///< Max service enum name length

#define RDM_COL3_ID    3                ///< Column 3 ID
#define RDM_COL3_NAME "Default Service" ///< Column 3 title,
#define RDM_COL3_WIDTH 32               ///< Column 3 Width in characters,
                                        ///< Max service enum name length

#define RDM_ROW_WIDTH 200               ///< Max width of the line

#define RDM_ENABLED  "YES"
#define RDM_DISABLED "NO"

/*-------------------------RDM Compatiblity --------------------------*/

#define RDM_DEV_COMPAT_WORD_SIZE   32           ///< No. of bits in uint32
#define RDM_DEV_COMPAT_INDEX_SHIFT 5            ///< Shift value for uint32
#define RDM_DEV_COMPAT_ALL         0xffffffff   ///< Compat mask for null service

/*===========================================================================
                          Local Typedefs
===========================================================================*/
/**------------------------------------------------------------------------
  Type of DEVMAP State
--------------------------------------------------------------------------*/
typedef enum
{
  RDM_IDLE_STATE,               /**< DEVMAP Idle, nothing in process       */
  RDM_CLOSING_STATE,            /**< DEVMAP Closing a serial port          */
  RDM_OPENING_STATE,            /**< DEVMAP Opening a serial port          */
  RDM_INIT_STATE,               /**< DEVMAP restoring the previous setting */
  RDM_LOCKED_STATE              /**< DEVMAP has been locked by client req  */
} rdm_state_enum_type;

/**------------------------------------------------------------------------
  Type of DEVMAP state and related variable of its State Machine
--------------------------------------------------------------------------*/
typedef struct {
  /** Current state variable for RDM state machine.      */
  rdm_state_enum_type   state;

  /** All other fields (except state) are meaningful only when
  *  rdm_state is in RDM_CLOSING_STATE or RDM_CLOSING_STATE
  *   */

  /** this is the service requesting the desired_device
     Its init value is RDM_NULL_SRVC                    */
  rdm_service_enum_type service_requester;

  /** this is the device the service requester desires
     Its init value is RDM_DEV_MAX                      */
  rdm_device_enum_type  desired_device;

  /** this the service that taking current action
     Its init value is RDM_NULL_SRVC                    */
  rdm_service_enum_type inprocess_service;

  /** this is the device current action aims
     Its init value is RDM_DEV_MAX                      */
  rdm_device_enum_type inprocess_device;

  /** assign_port call back function to inform
     service_requester of the assign_port status        */
  rdm_assign_port_cb_type cbptr;

  /** whether the transition should be persistent across
     reboots or not                                     */
  boolean is_persistent;

} rdm_state_type;

/**-------------------------------------------------------------------------
  Type of storage to keep the Services registered Open and Close routines.
---------------------------------------------------------------------------*/
typedef union {
  /** open routine support only one device for one service       */
  rdm_service_open_func_ptr_type        single_dev_p;

  /** open routine support multiple devices for one service     */
  rdm_srv_func_ptr_multi_dev_type       multi_dev_p;
} rdm_open_func_u_type;

typedef union {
  /** close routine support only one device for one service     */
  rdm_service_close_func_ptr_type       single_dev_p;

  /** close routine support multiple devices for one service    */
  rdm_srv_func_ptr_multi_dev_type       multi_dev_p;
} rdm_close_func_u_type;

typedef struct {
  /** test if the service cb functions are supporting multi
     devices for the service                                            */
  boolean                   multi_dev;

  /** when multi_dev is TRUE, open_func union takes multi_dev_p
     when multi_dev is FALSE, open_func union takes single_dev_p        */
  rdm_open_func_u_type      open_func;

  /** when multi_dev is TRUE, close_func union takes multi_dev_p
     when multi_dev is FALSE, close_func union takes single_dev_p       */
  rdm_close_func_u_type     close_func;

  /** a user data, service register it to the RDM with open/close function
     pointers. RDM passes this pointer back to the service when calling
     open or closing function.*/
  void *                data;
} rdm_srv_cb_type;

/**-------------------------------------------------------------------------
  Type of RDM device map
--------------------------------------------------------------------------*/
typedef struct rdm_map_type
{
    /** current service mapped on a device */
    rdm_service_enum_type current_srvc_id;

    /** default service for a device */
    rdm_service_enum_type default_srvc_id;

    /** when is_persistnet is TRUE, port mappings are persistent
        when is_persistent is FALSE, port mappings are not
        persistent across power cycle                             */
    boolean is_persistent;
}rdm_map_type;

/*===========================================================================
                    Local Function Prototypes
===========================================================================*/
LOCAL boolean rdm_assign_port_common
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr,

  /* Whether this transition should persist across      */
  /* device resets.                                     */
  boolean                       is_persistent
);

static boolean rdm_start_open_device
(
  /* Service Task desiring a new port    */
  rdm_service_enum_type         service,

  /* Device port to give to Service task */
  rdm_device_enum_type          device,

  /* callback function         */
  rdm_assign_port_cb_type       cbptr,

  /* is this transition persistent? */
  boolean                       is_persistent
);

static void rdm_reset_state_machine (void);

static boolean rdm_send_service_cmd
(
  /* the service executing the cmd              */
  rdm_service_enum_type  service,

  /* the cmd                                    */
  rdm_command_enum_type  port_cmd,

  /* the device which the cmd act on            */
  rdm_device_enum_type   device
);

LOCAL void rdm_update_efs_entry(int device_index);

static boolean rdm_notify_close
(
  /* Service Task desiring a new port                           */
  rdm_service_enum_type  service,

  /* status of requested task change                            */
  rdm_assign_status_type status,

  /* what to notify the service_requester if the port assign
     is done or aborted                                         */
  rdm_assign_status_type * notify_ptr
);

static void rdm_remove_dup_mappings(boolean is_remote);

static boolean rdm_start_close_device
(
  /* this service whoes device is to be closed          */
  rdm_service_enum_type         service,

  /* device to be closed                                */
  rdm_device_enum_type          device,

  /* call back function                                 */
  rdm_assign_port_cb_type       cbptr
);

static boolean rdm_lock_state ( void );

static void rdm_remove_unsupported_mappings(boolean is_remote);

static void rdm_register_all_services(void);

static void rdm_register_all_devices(void);

static void rdm_issue_opens_common(boolean is_remote);

static void rdm_enum_to_string(void);

static rdm_device_enum_type rdm_get_device_id(char *ID);

static rdm_service_enum_type rdm_get_service_id(char *Service);

static boolean rdm_restore_port_mapping(void);
#ifndef FEATURE_RDM_NO_MISC_API
    #if defined (FEATURE_USB_ON_APPS_PROC) && !defined (RDM_SERVER_ON_ALL_PROC)
    typedef void (*usbotg_remote_vv_f_type) (void);
    extern void usbotg_turn_on_apps_isr_control(void);
    #endif /* FEATURE_USB_ON_APPS_PROC */
#endif /*FEATURE_RDM_NO_MISC_API*/

/*===========================================================================
                                Globals
===========================================================================*/
/**-------------------------------------------------------------------------
  Current state variable for RDM state machine.
  local variables that stores the Service task ID, in progress Service
  task, and inprogress Device, and the final desired device during DEVMAP
  state machine operation.
---------------------------------------------------------------------------*/
static rdm_state_type rdm_state = {
  RDM_IDLE_STATE,       /**< state                */
  RDM_NULL_SRVC,        /**< service_requester    */
  RDM_DEV_MAX,          /**< desired_device       */
  RDM_NULL_SRVC,        /**< inprocess_service    */
  RDM_DEV_MAX,          /**< inprocess_device     */
  NULL                  /**< cbptr                */
};

/**---------------------------------------------------------------------------
  number of requests to the services to open previously opened devices
  at the powerup time
---------------------------------------------------------------------------*/
static int rdm_req_init_num;
static int rdm_remote_req_init_num;

/**--------------------------------------------------------------------------
   Flags to synchronize dual processor initialization...
---------------------------------------------------------------------------*/

static volatile boolean rdm_remote_init_done;
static volatile boolean rdm_remote_issue_pending;
static volatile boolean rdm_get_prev_done;

/**--------------------------------------------------------------------------
   Flags for RDM persistency feature
---------------------------------------------------------------------------*/
static volatile boolean rdm_non_persistent;

/**-------------------------------------------------------------------------
  Where and what in EFS to keep port mappings across MSM reset's.
---------------------------------------------------------------------------*/
/** rdm_port_map_file name on EFS file system */
static const char  *rdm_efs_port_map_file = "rdm_config.csv";

/** Rdm service info: Holds information about each service
    parameters and it's registered callback                               */
static rdm_service_info_type rdm_service_info[RDM_SRVC_MAX];

/** Rdm dervice info: Holds information for each dervice, it's
    parameters and it's availablity                                       */
static rdm_device_info_type  rdm_device_info[RDM_DEV_MAX];

/** Look up table for service ID to service name transation               */
static char rdm_service_names[RDM_SRVC_MAX][RDM_COL2_WIDTH];

/** Look up table for service ID to service name transation               */
static char rdm_device_names[RDM_DEV_MAX][RDM_COL1_WIDTH];

/** RDM device and  service map table                                     */
rdm_map_type rdm_map[RDM_DEV_MAX];

/** rdm_fversion: holds the rdm version string, if version
 * string of the efs file doesn't match with rdm_fversion then
 * new file is created from default configuration and existing
 * file is discarded.
 * Where,
 * RDM VERSION = RDM_DEV_MAX . RDM_FILE_MAX_COL . RDM_FILE_FORMAT        */
char rdm_fversion[RDM_ROW_WIDTH];

/** Critical section for RDM */
rex_crit_sect_type rdm_crit_sect;
#define rdm_enter_crit_section() rex_enter_crit_sect(&rdm_crit_sect)
#define rdm_leave_crit_section() rex_leave_crit_sect(&rdm_crit_sect)
#define rdm_init_crit_section()  rex_init_crit_sect(&rdm_crit_sect)
/*===========================================================================
                                Macros
===========================================================================*/

/** It returns the index associated to Device ID for device
 *  compatiblity arrray                                                    */
#define RDM_DEV_COMPAT_INDEX(dev) (dev >> RDM_DEV_COMPAT_INDEX_SHIFT)

/** It returns the shift valude for Device ID                              */
#define RDM_DEV_COMPAT_SHIFT(dev) (dev & (RDM_DEV_COMPAT_WORD_SIZE-1))

/** It returns TRUE if dev and srv are compatible otherwise it
 *  returns FALSE. Make sure srv is <= RDM_SRVC_MAX before
 *  calling this macro                                                      */
#define RDM_SD_COMPAT(srv, dev)     rdm_is_srv_dev_compat(srv, dev)

/** It returns TRUE if srv is mapped on dev, otherwise FALSE                */
#define RDM_SRV_USE_DEV(srv, dev)    rdm_is_srv_holding_dev(srv, dev)

/** It copies device name (str) to dev_ptr                                  */
#define RDM_SET_DEV_NAME(dev_ptr, str)  strlcpy(dev_ptr, str, RDM_MAX_DEV_NAME_LEN);

/** It converts ENUM symbol to string and copies into
 *  rdm_device_name[] array                                                 */
#define RDM_DEV_NAME(x) { \
                         int nbytes; \
                         nbytes = snprintf(rdm_device_names[x], RDM_COL1_WIDTH ,#x); \
                         ASSERT(nbytes < RDM_COL1_WIDTH); \
                        }

/** It converts ENUM symbol to string and copies into
 *  rdm_service_name[] array                                                */
#define RDM_SVC_NAME(x) { \
                         int nbytes; \
                         nbytes = snprintf(rdm_service_names[x], RDM_COL2_WIDTH,#x); \
                         ASSERT(nbytes < RDM_COL2_WIDTH); \
                        }

/** It returns the string name associated with Service ID,
 *  For RDM_NULL_SRVC it returns blank string                                */
#define RDM_GET_SVC_NAME(x) (( ((x) == RDM_NULL_SRVC) || ((x) >= RDM_SRVC_MAX) ) ? " " : rdm_service_names[(x)])



/*===========================================================================

                           PUBLIC FUNCTIONS

===========================================================================*/

/**===========================================================================

FUNCTION RDM_INIT                                     EXTERNALIZED FUNCTION

@brief
  Initializes the Runtime Device Mapper Services. This function must
  be called before other functions of this module.

@dependencies
  Should be called just once.

@return
  None

@sideeffects
  None

===========================================================================*/
/*lint -save -e641 -e539 -e64*/
void rdm_init(void)
{
  /* device index to traverse valid rdm_device_enum_type        */
  rdm_device_enum_type          dev_index;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_init()", 0, 0, 0););
  rdm_enum_to_string();

  rdm_init_crit_section(); /* Initialize ciritcal section */

  rdm_register_all_devices();  /* Set devices */
  rdm_register_all_services(); /* Set services */

  /* Set RDM state to INIT */
  rdm_state.state = RDM_INIT_STATE;
  rdm_remote_init_done = FALSE;
  rdm_get_prev_done = FALSE;
  rdm_remote_issue_pending = FALSE;
#ifdef FEATURE_RDM_ALWAYS_NON_PERSISTENT
  rdm_non_persistent =  TRUE;
#else
  rdm_non_persistent = FALSE;
#endif

  /* init rdm_map                                    */
  for(dev_index = RDM_FIRST_DEV; dev_index < RDM_DEV_MAX; dev_index++) /*lint !e539*/
  {
    rdm_map[dev_index].default_srvc_id = RDM_NULL_SRVC;
    rdm_map[dev_index].current_srvc_id = RDM_NULL_SRVC;
    rdm_map[dev_index].is_persistent = 1;
  }

#ifdef FEATURE_DIAG_MP_MODEM_ONLY
#ifdef RDM_MPO_DEFAULT_DIAG_PORT
  #error "Outdated feature usage, please use RDM_xxx_INIT_SRVC instead"
#endif
#endif

#ifdef RDM_USB_MDM_INIT_SRVC
  rdm_map[RDM_USB_MDM_DEV].default_srvc_id   = RDM_USB_MDM_INIT_SRVC;
#endif

#ifdef RDM_USB_SER1_INIT_SRVC
  rdm_map[RDM_USB_SER1_DEV].default_srvc_id  = RDM_USB_SER1_INIT_SRVC;
#endif

#ifdef RDM_UART1_INIT_SRVC
  rdm_map[RDM_UART1_DEV].default_srvc_id     = RDM_UART1_INIT_SRVC;
#endif

#ifdef RDM_UART2_INIT_SRVC
  rdm_map[RDM_UART2_DEV].default_srvc_id     = RDM_UART2_INIT_SRVC;
#endif

#ifdef RDM_UART3_INIT_SRVC
  rdm_map[RDM_UART3_DEV].default_srvc_id     = RDM_UART3_INIT_SRVC;
#endif

#ifdef RDM_SMD_DATA_INIT_SRVC
  rdm_map[RDM_SMD_DATA1_DEV].default_srvc_id  = RDM_SMD_DATA_INIT_SRVC;
  rdm_map[RDM_SMD_DATA2_DEV].default_srvc_id  = RDM_SMD_DATA_INIT_SRVC;
  rdm_map[RDM_SMD_DATA3_DEV].default_srvc_id  = RDM_SMD_DATA_INIT_SRVC;
  rdm_map[RDM_SMD_DATA4_DEV].default_srvc_id  = RDM_SMD_DATA_INIT_SRVC;
  rdm_map[RDM_SMD_DATA11_DEV].default_srvc_id = RDM_SMD_DATA_INIT_SRVC;
#endif

#ifdef FEATURE_8X60_CSFB_CSVT
#ifdef RDM_SDIO_CSVT_DATA_INIT_SRVC
  rdm_map[RDM_SDIO_MUX_A2_CSVT_DEV].default_srvc_id =
                                    RDM_SDIO_CSVT_DATA_INIT_SRVC;
#endif /* RDM_SDIO_DATA_INIT_SRVC */
#endif /* FEATURE_8X60_CSFB_CSVT */

#ifdef RDM_SMD_DS_INIT_SRVC
  rdm_map[RDM_SMD_DS_DEV].default_srvc_id       = RDM_SMD_DS_INIT_SRVC;
#endif

#ifdef RDM_APS_VSP_INIT_SRVC
  rdm_map[RDM_APS_SERIAL_VSP_DEV].default_srvc_id = RDM_APS_VSP_INIT_SRVC;
#endif

#ifdef FEATURE_HS_USB_SER3_PORT
 rdm_map[RDM_USB_SER3_DEV].default_srvc_id   = RDM_DATA_SRVC;
#endif

#ifdef FEATURE_HS_USB_SMCT_PORT
  rdm_map[RDM_USB_SMCT_DEV].default_srvc_id   = RDM_DATA_SRVC;
#endif

  #ifdef RDM_SMUX_0_INIT_SRVC
     rdm_map[RDM_SMUX_0_DEV].default_srvc_id = RDM_SMUX_0_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_1_INIT_SRVC
     rdm_map[RDM_SMUX_1_DEV].default_srvc_id = RDM_SMUX_1_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_2_INIT_SRVC
     rdm_map[RDM_SMUX_2_DEV].default_srvc_id = RDM_SMUX_2_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_3_INIT_SRVC
     rdm_map[RDM_SMUX_3_DEV].default_srvc_id = RDM_SMUX_3_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_4_INIT_SRVC
     rdm_map[RDM_SMUX_4_DEV].default_srvc_id = RDM_SMUX_4_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_5_INIT_SRVC
     rdm_map[RDM_SMUX_5_DEV].default_srvc_id = RDM_SMUX_5_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_6_INIT_SRVC
     rdm_map[RDM_SMUX_6_DEV].default_srvc_id = RDM_SMUX_6_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_7_INIT_SRVC
     rdm_map[RDM_SMUX_7_DEV].default_srvc_id = RDM_SMUX_7_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_8_INIT_SRVC
     rdm_map[RDM_SMUX_8_DEV].default_srvc_id = RDM_SMUX_8_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_9_INIT_SRVC
     rdm_map[RDM_SMUX_9_DEV].default_srvc_id = RDM_SMUX_9_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_10_INIT_SRVC
     rdm_map[RDM_SMUX_10_DEV].default_srvc_id = RDM_SMUX_10_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_11_INIT_SRVC
     rdm_map[RDM_SMUX_11_DEV].default_srvc_id = RDM_SMUX_11_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_12_INIT_SRVC
     rdm_map[RDM_SMUX_12_DEV].default_srvc_id = RDM_SMUX_12_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_13_INIT_SRVC
     rdm_map[RDM_SMUX_13_DEV].default_srvc_id = RDM_SMUX_13_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_14_INIT_SRVC
     rdm_map[RDM_SMUX_14_DEV].default_srvc_id = RDM_SMUX_14_INIT_SRVC;
  #endif

  #ifdef RDM_SMUX_15_INIT_SRVC
     rdm_map[RDM_SMUX_15_DEV].default_srvc_id = RDM_SMUX_15_INIT_SRVC;
  #endif

  rdm_map[RDM_MUX_PHYSICAL_DEV].default_srvc_id = RDM_DATA_MUX_SRVC;

  /* map default mapping to current                                    */
  for(dev_index = RDM_FIRST_DEV; dev_index < RDM_DEV_MAX; dev_index++) /*lint !e539*/
  {
    rdm_map[dev_index].current_srvc_id = rdm_map[dev_index].default_srvc_id;
  }

} /* rdm_init() */
/*lint restore*/

/**==========================================================================

FUNCTION  RDM_GET_PREV_PORTS                            EXTERNALIZED FUNCTION

@brief
  Read in the port mappings last know by RDM from EFS.

  It is here that the compile time defaults for port mapping may be
  overwritten by what is read from EFS at power up.

@dependencies
  None

@return
  None

@sideeffects
  Port mappings before the last reset are restored at boot.

===========================================================================*/
void rdm_get_prev_ports(void)
{
/*------------------------------------------------------------------*/
  if(!rdm_non_persistent)
  {
    DEBUG( MSG_MED("rdm_get_prev_ports()",0,0,0); );

    if(!rdm_restore_port_mapping())
    {
       /* Port mapping was not restored successfully likely due to no prior file */
       /* Create a file and save current mapping to it */
       rdm_update_efs_entry(RDM_DEV_ALL);
    }

    rdm_get_prev_done = TRUE;
  }
} /* rdm_get_prev_ports() */


/**===========================================================================

FUNCTION  RDM_UPDATE_CURRENT_DEVICE_MAP                      INTERNAL FUNCTION

@brief
  update rdm_map according to network_device_flag.

  If USB is NETWORK device, USB_SER2 is mapped to NULL service.
  If USB is MODEM device, USB_NET_WWAN is mapped to NULL service.

@dependencies
  None

@return
  None

@sideeffects
  rdm_map is modified.
=============================================================================*/
#ifdef FEATURE_DATA_RM_NET
void  rdm_update_current_device_map(boolean network_device_flag)
{
   if (network_device_flag)
   {
     rdm_map[RDM_USB_SER2_DEV].current_srvc_id = RDM_NULL_SRVC;
     rdm_map[RDM_USB_NET_WWAN_DEV].current_srvc_id = RDM_DATA_WWAN_SRVC;
   }
   else
   {
      rdm_map[RDM_USB_NET_WWAN_DEV].current_srvc_id = RDM_NULL_SRVC;
   }
}
#endif

/**==========================================================================

FUNCTION RDM_ASSIGN_PORT                                EXTERNALIZED FUNCTION

@brief
  This function initiates the DEVMAP state machine to change the current
  serial port configuration to give the specified serial port to the
  specified Service task. This function will perform checks to ensure that
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using.
  Please use rdm_close_device if you want to specify which port to close.

  This function records the transition to the EFS so that the assignment
  persists across power cycles.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.

@dependencies
  None

@return
  Boolean
    True : if the mapping is valid and will be attempted.
           cbptr is called immediatly before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status
           otherwise.
    False: if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.

@sideeffects
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_assign_port
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr
)
{
   return rdm_assign_port_common(service,device,cbptr,TRUE);
}

/**==========================================================================

FUNCTION RDM_ASSIGN_PORT_TMP                            EXTERNALIZED FUNCTION

@brief
  This function initiates the DEVMAP state machine to change the current
  serial port configuration to give the specified serial port to the
  specified Service task. This function will perform checks to ensure that
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using.
  Please use rdm_close_device if you want to specify which port to close.

  This function does not record the port transition to the EFS.  It is
  thus a non-persistent assignment.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.

@dependencies
  None

@return
  Boolean
    True : if the mapping is valid and will be attempted.
           cbptr is called immediatly before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status
           otherwise.
    False: if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.

@sideeffects
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_assign_port_tmp
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr
)
{
   return rdm_assign_port_common(service,device,cbptr,FALSE);
}

/**==========================================================================

FUNCTION RDM_ASSIGN_PORT_COMMON

@brief
  Port assignment code shared between persistent and non-persistent forms
  of the function.

@dependencies
  None

@return
  TRUE if the operation succeeded
  FALSE otherwise

@sideeffects
  None
===========================================================================*/
LOCAL boolean rdm_assign_port_common
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of
     the assign action                                  */
  rdm_assign_port_cb_type       cbptr,

  /* Whether this transition should persist across      */
  /* device resets.                                     */
  boolean                       is_persistent
)
{
  /* result of the function return              */
  boolean               result;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_assign_port(SRVC=%d, DEV=%d, CB=%x)",
                service, device, cbptr););

  /* Make sure we are not called in ISR         */
  if (rex_is_in_irq_mode())
  {
    ERR("rdm_assign_port called in ISR",0,0,0);
    if(cbptr)
    {
      cbptr(RDM_RESTRICTED_S, service, device);
    }
    return FALSE;
  }

  /* Check if service & device are w/i range    */
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX ||
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }
    return FALSE;
  }

  /* Make sure the requested SRVC/DEV combo
     is allowed                                 */
  if(!RDM_SD_COMPAT(service, device) )
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }

    return FALSE;
  }


  /* Remote service needs to wait until remote service initialization is done */
  if(rdm_service_info[service].is_remote == TRUE && rdm_remote_init_done == FALSE)
  {
    if(cbptr)
    {
      cbptr(RDM_DEVMAP_BUSY_S, service, device);
    }
    return FALSE;
  }

  /* check if the rdm is busy                    */
  if(!rdm_lock_state())
  {
    /* DEVMAP state machine in process, inform device_requester that
       we are busy                                                      */
    if(cbptr)
    {
      cbptr(RDM_DEVMAP_BUSY_S, service, device);
    }

    return FALSE;
  }

  /*------------------------------------------------------------------
    we passed the parameter and state checking
    ------------------------------------------------------------------*/

  if(device == RDM_NULL_DEV)
  {
    /* Special case, the request is to just close down a service's port */
    rdm_device_enum_type        first_dev;

    first_dev   = rdm_get_device(service);

    if(first_dev == RDM_NULL_DEV)
    {
      /* we are ready to service next request           */
      rdm_reset_state_machine();

      /* The service already has no port/device open... */
      if(cbptr)
      {
        cbptr(RDM_DONE_S, service, RDM_NULL_DEV);
      }

      result = TRUE;
    }
    else
    {
      rdm_state.is_persistent           = is_persistent;
      result = rdm_start_close_device(service, first_dev, cbptr);
    }
  }
  else
  {
    /* device which the service is using        */
    rdm_device_enum_type          srv_dev;

    /* if service support only one device openning, we need to close
       its old port before openning a new one   */
    srv_dev = rdm_get_device(service);

    if( (srv_dev != RDM_NULL_DEV) && (srv_dev != device) &&
        rdm_service_info[service].service_params.device_support == RDM_SINGLE_DEV_SUPPORT
        )
    {

      /* the service is using a device which is not the reqested
         device to be opened and the service is not able to open
         multiple device.  RDM will close the service's current
         device then request opening of the new device          */

      rdm_state.state                   = RDM_CLOSING_STATE;
      rdm_state.service_requester       = service;
      rdm_state.desired_device          = device;
      rdm_state.inprocess_service       = service;
      rdm_state.inprocess_device        = srv_dev;

      rdm_state.cbptr                   = cbptr;
      rdm_state.is_persistent           = is_persistent;

      /* close the device the service holds */
      if(rdm_send_service_cmd(rdm_state.inprocess_service,
                              RDM_CLOSE_PORT, rdm_state.inprocess_device))
      {
        result = TRUE;
      }
      else
      {
        rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;

        /* we are ready to service next request           */
        rdm_reset_state_machine();

        /* inform the requester of the failure */
        if(notify_cb)
        {
          notify_cb(RDM_NOT_ALLOWED_S, service, RDM_NULL_DEV);
        }

        result = FALSE;
      }
    }
    else
    {
      /* open the device for the service */
      result = rdm_start_open_device(service, device, cbptr, is_persistent);
    }
  } /* else (device == RDM_NULL_DEV) */

  return result;
} /* rdm_assign_port */

/**==========================================================================

FUNCTION rdm_close_device                               EXTERNALIZED FUNCTION

@brief
  This function will close a device used by a service and map the device
  to NULL service.

@dependencies
  None

@return
  Boolean
    True : the closing action is made
    False: If the device or service is not valid or
           If the service is not using the device or
           If the function is call in an ISR

@sideeffects
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_close_device
(
  /* Service Task request closing of the device         */
  rdm_service_enum_type         service,

  /* Device/port to be closed                           */
  rdm_device_enum_type          device,

  /* Optional callback function to inform the result of
     the closing action                                 */
  rdm_assign_port_cb_type       cbptr
)
{
  /* result of the function return              */
  boolean               result;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DEBUG(MSG_MED("rdm_close_device(SRVC=%d, DEV=%d, CB=%x)",
                service, device, cbptr););

  /* Make sure we are not called in an ISR      */
  if (rex_is_in_irq_mode())
  {
    ERR("rdm_close_device called in ISR",0,0,0);
    if(cbptr)
    {
      cbptr(RDM_RESTRICTED_S, service, device);
    }
    return FALSE;
  }

  /* Check if service & device are w/i range    */
  if(device <= RDM_FIRST_DEV || device >= RDM_DEV_MAX ||
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }
    return FALSE;
  }

  /* check if service is using the device       */
  if ( !RDM_SRV_USE_DEV(service, device) )
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }
    return FALSE;
  }

  /* check if the rdm is busy                   */
  if(!rdm_lock_state())
  {
    /* DEVMAP state machine in process, inform device_requester that
       we are busy                                                      */
    if(cbptr)
    {
      cbptr(RDM_DEVMAP_BUSY_S, service, device);
    }

    return FALSE;
  }

  /*------------------------------------------------------------------
    we passed the parameter and state checking
    ------------------------------------------------------------------*/
  result = rdm_start_close_device(service, device, cbptr);

  return result;
} /* rdm_close_device */

#if defined (FEATURE_USB_ON_APPS_PROC) && !defined (RDM_SERVER_ON_ALL_PROC)
/**==========================================================================

FUNCTION rdm_notify_cb_helper_f

@brief
  This function calls the pm_app_otg_set_remote_a_dev_info
  which is the real callback function registered from the applications
  processor.  It uses the parameters sent with the message

@dependencies
  None.

@return
  None.

@sideeffects
  Makes an RPC call to the applications processor.
===========================================================================*/
void
rdm_notify_cb_helper_f
(
  /* ONCRPC Proxy command message */
  oncrpc_proxy_cmd_client_call_type *msg
)
{
  rdm_assign_port_cb_type notify_cb = (rdm_assign_port_cb_type)msg->data[0];
  rdm_assign_status_type notify_s = (rdm_assign_status_type)msg->data[1];
  rdm_service_enum_type   srv_req = (rdm_service_enum_type)msg->data[2];
  rdm_device_enum_type    dev_req = (rdm_device_enum_type)msg->data[3];

  /* Make call to callback with parameter data */
  notify_cb(notify_s, srv_req, dev_req);

} /* usbsdms_rdm_notify_helper_f */



/**==========================================================================

FUNCTION rdm_notify_cb_handler

@brief
  This function is the callback that is registered locally with the
  pm_app_otg_set_remote_a_dev_info() function.  When called, this function
  will save parameter information and queue a command with the RPC
  proxy task to call the real callback from the applications processor.
  This is necessary because this callback is called from interrrupt space
  and we cannot make RPC calls from interrupt space.

@dependencies
  None.

@return
  None.

@sideeffects
  Sends a message to RPC proxy task.
===========================================================================*/
void
rdm_notify_cb_handler
(
  rdm_assign_port_cb_type notify_cb,
  rdm_assign_status_type notify_s,
  rdm_service_enum_type   srv_req,
  rdm_device_enum_type    dev_req
)
{
  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type *rdm_notify_cb_msg_ptr;

  rdm_notify_cb_msg_ptr = remote_apis_client_call_get();

  /* Check that the message pointer is valid */
  ASSERT (rdm_notify_cb_msg_ptr != NULL);

  if( rdm_notify_cb_msg_ptr != NULL )
  {

    /* Fill up the pointer data */
    rdm_notify_cb_msg_ptr->client_call =
      (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
        rdm_notify_cb_helper_f;

    /* Send parameter data with message */
    rdm_notify_cb_msg_ptr->data[0] = (uint32)notify_cb;
    rdm_notify_cb_msg_ptr->data[1] = (uint32)notify_s;
    rdm_notify_cb_msg_ptr->data[2] = (uint32)srv_req;
    rdm_notify_cb_msg_ptr->data[3] = (uint32)dev_req;

    /* Queue up the message */
    oncprc_proxy_client_call_send( rdm_notify_cb_msg_ptr );

    /* Explicitly set to NULL */
    rdm_notify_cb_msg_ptr = NULL;
  }
} /* rdm_notify_cb_handler */
#endif /*FEATURE_USB_ON_APPS_PROC*/

/**==========================================================================

FUNCTION RDM_NOTIFY                                     EXTERNALIZED FUNCTION

@brief
  This function will notify to the DEVMAP state machine the specified result
  of a requested serial port change. This function performs a portion of the
  DEVMAP state machine.

@dependencies
  Should not be called in ISR.

@return
  None

@sideeffects
  None
===========================================================================*/
void rdm_notify
(
  rdm_service_enum_type  service,    /* Service Task desiring a new port    */
  rdm_assign_status_type status      /* status of requested task change     */
)
{

  /* determine if RDM is done                   */
  boolean       port_mapping_done = FALSE;

  /* the status to notify the requester, init it to be status,
     it is meaningful only when port_mapping_done is TRUE */
  rdm_assign_status_type notify_s = status;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DEBUG(MSG_MED("rdm_notify(SRVC=%d, STATUS=%d)", service, status, 0););

  if(rdm_remote_init_done == FALSE && rdm_service_info[service].is_remote == TRUE)
  {
      /* we have one opening done at power up */
      if( status != RDM_DONE_S)
      {
        rdm_device_enum_type device;
        /* Remove the port mapping of the device fail to open */
        device = rdm_get_device(service);
        rdm_map[device].current_srvc_id = RDM_NULL_SRVC;
        MSG_HIGH("srv: %d fail to open port", service, 0,0);
      }

      /* set/test in atomity */
      rdm_enter_crit_section();

      if(--rdm_remote_req_init_num == 0)
      {
        /* all the open of previous port has been done,
           update the port mapping                      */

        rdm_remote_init_done = TRUE;
      }

      rdm_leave_crit_section();

      if(rdm_remote_init_done)
      {
        rdm_update_efs_entry(RDM_DEV_ALL);
      }
      return;
  }

  /* make sure the function is called in correct state and is called
     by right task */
  ASSERT( (rdm_state.state == RDM_INIT_STATE) ||
          ( (rdm_state.state == RDM_CLOSING_STATE ||
             rdm_state.state == RDM_OPENING_STATE) &&
            (rdm_state.inprocess_service == service)) );

  /* Make sure we are not called in ISR */
  if (rex_is_in_irq_mode())
  {
    ERR("rdm_notify called in ISR",0,0,0);
  }

  /*----------------------------------------------------
    This is the scheduled notification operation. Proceed
    to continue DEVMAP state machine processing
    ----------------------------------------------------*/
  switch(rdm_state.state)
  {
    case RDM_CLOSING_STATE:
      port_mapping_done = rdm_notify_close(service, status, &notify_s);
      break;

    case RDM_OPENING_STATE:
      if( status == RDM_DONE_S )
      {
        ASSERT( (rdm_state.inprocess_device > RDM_FIRST_DEV) &&
                (rdm_state.inprocess_device < RDM_DEV_MAX));
        /*-------------------------------------------------------
          Update the current config table, if opened, the service
          will be using the new port.
        -------------------------------------------------------*/
        rdm_map[rdm_state.inprocess_device].current_srvc_id =
          rdm_state.inprocess_service;
        rdm_map[rdm_state.inprocess_device].is_persistent =
          rdm_state.is_persistent;

        if (rdm_map[rdm_state.inprocess_device].is_persistent)
        {
           /* inprocess_service opened inprocess_device, update the maps */
           rdm_update_efs_entry(rdm_state.inprocess_device);
        }
      }

      /*----------------------------------------------------------------
      Attempt made at opening Desired port now done.
      ----------------------------------------------------------------*/
      port_mapping_done = TRUE;
      break;

    case RDM_INIT_STATE:
      /* we have one opening done at power up */
      if( status != RDM_DONE_S)
      {
        rdm_device_enum_type device;
        /* Remove the port mapping of the device fail to open */
        device = rdm_get_device(service);
        rdm_map[device].current_srvc_id = RDM_NULL_SRVC;
        MSG_HIGH("srv: %d fail to open port", service, 0,0);
      }

      /* set/test in atomity */
      rdm_enter_crit_section();

      if(--rdm_req_init_num == 0)
      {
        /* all the open of previous port has been done,
           update the port mapping                      */

        port_mapping_done = TRUE;
      }

      rdm_leave_crit_section();

      /* If we created a port map file in EFS for the first time, we need to
         write out the current mapping. If we did not, the file will be there
         next time, but empty. This will assign all the devices to NULL on the
         next boot (unless the UI was used for port mapping a SRVC).
      */

      if(port_mapping_done)
      {
        rdm_update_efs_entry(RDM_DEV_ALL);
      }
      break;

    default:
      ERR( "Invalid DEVMAP State: %d", rdm_state.state, 0, 0);
      break;
  }

  /* If we are done mapping the port for whatever reason,
     notify the requester */
  if (port_mapping_done)
  {
    rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;
    rdm_service_enum_type   srv_req   = rdm_state.service_requester;
    rdm_device_enum_type    dev_req   = rdm_state.desired_device;

    /* we are ready to service next request           */
    rdm_reset_state_machine();

    if(notify_cb)
    {
#if (!defined(FEATURE_DIAG_MP_MASTER_MODEM) && defined(FEATURE_MULTIPROCESSOR)) && \
    (!defined(FEATURE_STANDALONE_MODEM)) && (defined(FEATURE_USB_ON_APPS_PROC) && !defined(RDM_SERVER_ON_ALL_PROC))
      rdm_notify_cb_handler(notify_cb,notify_s, srv_req, dev_req);
#else
      notify_cb(notify_s, srv_req, dev_req);
#endif
    }

  }
} /* rdm_notify */


/**===========================================================================

FUNCTION RDM_GET_DEVICE                                 EXTERNALIZED FUNCTION

@brief
  Query the Runtime Device Mapper for the port/device currently being used
  by the specified application/service.

  It returns the first device a service owns. This is good enough for service
  which can hold only one device.

  Please use rdm_get_multi_dev to get all the device a service holds.

@dependencies
  None

@return
  rdm_device_enum_type : the device which the service holds.

@sideeffects
  None

===========================================================================*/
rdm_device_enum_type rdm_get_device
(
  /* Service to query what device it may be using, if any */
  rdm_service_enum_type service
)
{
  /* device index to traverse valid devices                     */
  rdm_device_enum_type  dev_index;

/*------------------------------------------------------------------*/

  if(service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    ERR("wrong srv: %d", service,0,0);
    return RDM_NULL_DEV;
  }

  /* Test devices from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to
     RDM_LSM_CSD_DEV (RDM_DEV_MAX - 1)                          */
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
    if(rdm_map[dev_index].current_srvc_id == service)
    {
      return dev_index;
    }
  }
  return RDM_NULL_DEV;

} /* rdm_get_device */


/**==========================================================================

FUNCTION RDM_GET_MULTI_DEV                              EXTERNALIZED FUNCTION

@brief
  Query the Runtime Device Mapper for the port/devices currently being used
  by the specified application/service.

  It returns a device mask for the devices the service holds. The caller can
  test if the service is using a dev (for example, RDM_USB_SER1_DEV) by
  if (dev_mask & (1 << RDM_USB_SER1_DEV)).

  NOTE: This function can not be supported for on RDM 3.0 with more than
        32 devices

@dependencies
  None

@return
  device mask.

@sideeffects
  None

===========================================================================*/
uint32 rdm_get_multi_dev
(
  /* Service to query what devices it may be using, if any */
  rdm_service_enum_type service
)
{
  /* index to traverse the valid devices                */
  rdm_device_enum_type  index;

  /* device mask to return                              */
  uint32                dev_mask = 0;

/*------------------------------------------------------------------*/

  if(service >= RDM_FIRST_SRVC && service < RDM_SRVC_MAX)
  {
    /* Then we test the devices by traverving from
       RDM_UART1_DEV (RDM_FIRST_DEV + 1) to (RDM_DEV_MAX - 1) */
    for(index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
        index < RDM_DEV_MAX; index++)
    {
      if(rdm_map[index].current_srvc_id == service)
      {
        dev_mask |= (1 << index);
      }
    }
  }

  return dev_mask;
} /* rdm_get_multi_dev */


/**==========================================================================

FUNCTION RDM_GET_SERVICE                                EXTERNALIZED FUNCTION

@brief
  Query the Runtime Device Mapper for the application/service that is
  currently holding the specified port/device (if any).

@dependencies
  None

@return
  rdm_service_enum_type

@sideeffects
  None

===========================================================================*/
rdm_service_enum_type rdm_get_service
(
  /* Device to see what service is using it, if any */
  rdm_device_enum_type device
)
{
  if(device <= RDM_FIRST_DEV || device >= RDM_DEV_MAX )
  {
    ERR("wrong device: %d", device,0,0);
    return RDM_NULL_SRVC;
  }
  else
  {
    return rdm_map[device].current_srvc_id;
  }

} /* rdm_get_service */


/**==========================================================================

FUNCTION  RDM_GET_VALID_PORTS                           EXTERNALIZED FUNCTION

@brief
  Query the RDM for the list of valid ports for the specified service.

@dependencies
  The caller must have allocated space of RDM_DEV_MAX to the
  dev_displays array.

@return
  Boolean TRUE if the service is valid, else FALSE.

@sideeffects
  The contents of dev_displays[] will be written to.
===========================================================================*/
boolean rdm_get_valid_ports
(
  /* The service that getting the valid ports           */
  rdm_service_enum_type    service,

  /* array of rdm_service_display of length RDM_DEV_MAX */
  rdm_service_display_type dev_displays[]
)
{
  /* index to traverse rdm_device_enum_type     */
  rdm_device_enum_type  dev_index;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_get_valid_ports: %d",service,0,0););

  if(service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    ERR("wrong service: %df",service,0,0);
    return FALSE;
  }

  rdm_enter_crit_section();

  /*----------------------------------------------------------------------
    Opt to build this table on the fly versus keeping them all in memory
    and having to update them often
  ----------------------------------------------------------------------*/
  for(dev_index = RDM_FIRST_DEV; dev_index < RDM_DEV_MAX; dev_index++)
  {
    /* set device for dev_displays[dev_index]   */
    if( RDM_SD_COMPAT(service, dev_index) )
    {
      dev_displays[dev_index].device = dev_index;
    }
    else
    {
      dev_displays[dev_index].device  = RDM_SRVC_NOT_ALLOWED;
      dev_displays[dev_index].display = FALSE;
      dev_displays[dev_index].current = FALSE;
      continue;
    }

    /* set display for dev_displays[dev_index]  */
    if( (dev_displays[dev_index].device == RDM_USB_MDM_DEV) ||
        (dev_displays[dev_index].device == RDM_BT_NA_DEV) )
    {
      /*------------------------------------------------
        Let UI show & therefore map DS to either BT or
        USB which ever mode we're in ...
      ------------------------------------------------*/
      if(service == RDM_DATA_SRVC)
      {
        dev_displays[dev_index].display = TRUE;
      }
      else
      {
        dev_displays[dev_index].display = TRUE;
      }
    }
    else if( (dev_displays[dev_index].device == RDM_USB_MMC_DEV) ||
             (dev_displays[dev_index].device == RDM_USB_SER1_DEV) )
    {
      dev_displays[dev_index].display = TRUE;
    }
#if defined(FEATURE_SMD) && defined(IMAGE_MODEM_PROC)
    else if( (dev_displays[dev_index].device >= RDM_SMD_DS_DEV) &&
             (dev_displays[dev_index].device <= RDM_SMD_DATA4_DEV) )
    {
      dev_displays[dev_index].display = FALSE;
    }
#endif /* defined(FEATURE_SMD)... */
    else
    {
      dev_displays[dev_index].display = TRUE;
    }

    /* set current for dev_displays[dev_index]. Current is TRUE when
        dev_displays[dev_index]'s device is not NULL and the service
        is using it                                                     */
    dev_displays[dev_index].current = rdm_is_srv_holding_dev(service, dev_index);
  } /* end for loop */


  rdm_leave_crit_section();
  return TRUE;
} /* rdm_get_valid_ports() */

/**==========================================================================

FUNCTION rdm_set_srv_compat_mask                        EXTERNALIZED FUNCTION

@brief
  Set device compatible mask for the service from
  (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which
  device this service can use.

  It provides an interface to dynamically set the compatiblity mask. Don't
  use this function for services of from RDM_DIAG_SRVC to
  RDM_LAST_KNOWN_SRVC (RDM_RPC_SRVC). The device compatibility table
  are set for those serivce at compile time.

  The caller can set dev_mask as this: if a service can use devices
  RDM_UART1_DEV and RDM_USB_SER1_DEV.  The dev_mask is
  (1 << RDM_NULL_DEV) | (1 << RDM_UART1_DEV) | (1 << RDM_USB_SER1_DEV).
  RDM_NULL_DEV is here since RDM can always map NULL DEV to the service,
  meaning closing the device of the service.

  NOTE: This function can only support devices with value < 32

@dependencies
  None

@return
  Boolean
    TRUE successfully set
    FALSE if the service is incorrect.

@sideeffects
  None

===========================================================================*/
boolean rdm_set_srv_compat_mask
(
  rdm_service_enum_type service,        /* service to set       */
  uint32                dev_mask        /* dev mask to set      */
)
{
  /* This function is for the services from
     (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1)   */
  if (service <= RDM_LAST_KNOWN_SRVC  || service >= RDM_SRVC_MAX)
  {
    ERR("Invalid serivce %d", service,0,0);
    return FALSE;
  }

  /* For RDM 3.0 : Legacy Support */
  rdm_service_info[service].dev_compat_array[0] = dev_mask;
  return TRUE;
} /* rdm_set_srv_compat_mask */


/**==========================================================================

FUNCTION RDM_IS_SRV_DEV_COMPAT                          EXTERNALIZED FUNCTION

@brief
  Test if a service and a device is compatible. That is whether the service
  can use the device.

@dependencies
  None

@return
  Boolean
    TRUE if the service and the device is compatible,
    FALSE if the service and the device is not compatible or
          if the parameters are not right

@sideeffects
  None

===========================================================================*/
boolean rdm_is_srv_dev_compat
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
)
{

  /* Check if service & device are w/i range    */
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX ||
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    return FALSE;
  }

  if(rdm_device_info[device].available == FALSE)
  {
    return FALSE;
  }

  if(((rdm_service_info[service].dev_compat_array[RDM_DEV_COMPAT_INDEX(device)]) & (1 << RDM_DEV_COMPAT_SHIFT(device))))
  {
    return TRUE;
  }

  return FALSE;
} /* rdm_is_srv_dev_compat */

/*==========================================================================
                           LOCAL FUNCTIONS
===========================================================================*/


/**==========================================================================

FUNCTION RDM_RESET_STATE_MACHINE

@brief
  Cleanup/reset RDM state varaibles

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/
static void rdm_reset_state_machine (void)
{

  DEBUG(MSG_MED("rdm_reset_state_machine()", 0, 0, 0););

  rdm_state.service_requester   = RDM_NULL_SRVC;
  rdm_state.desired_device      = RDM_DEV_MAX;
  rdm_state.inprocess_service   = RDM_NULL_SRVC;
  rdm_state.inprocess_device    = RDM_DEV_MAX;

  rdm_state.cbptr               = NULL;

  rdm_state.is_persistent       = FALSE;

  /* reset rdm_state.state at the end */
  rdm_state.state               = RDM_IDLE_STATE;

} /* rdm_reset_state_machine() */

/**===========================================================================

FUNCTION  RDM_UPDATE_EFS_ENTRY

@brief
  Used by RDM to update port mapping information in EFS. This is called after
  a successful SRVC to DEV mapping.

  If called with RDM_DEV_ALL as an argument will unconditionally write the
  entire file to EFS.  This is useful to generate the file initially.

@dependencies
  None

@return
  None

@sideeffects
  The port mapping is written to EFS.

===========================================================================*/
LOCAL void rdm_update_efs_entry(int device_index)
{
/*------------------------------------------------------------------*/
  if(!rdm_non_persistent)
  {
    char *config_item_buffer, *fline;
    int buffer_remaining = RDM_CONFIG_ITEM_FILE_SIZE;
    fs_ssize_t nbytes;
    rdm_device_enum_type dev;

    /* Prior version and this one ignore device_index parm and re-write entire file.
       Correcting this might allow both modem and app-side RDM servers to share and update the same file. */

    if (DAL_SUCCESS == DALSYS_Malloc(RDM_CONFIG_ITEM_FILE_SIZE, (void **)(&config_item_buffer)))
    {

      memset(config_item_buffer, 0, RDM_CONFIG_ITEM_FILE_SIZE);
      fline = config_item_buffer;

      /* PRINT HEADER: Use '#' to add comments in config file */
      /* The file is of fixed size due to EFS restrictions for efs_get() that the exact size of the item requested
         must be given or API fails. RDM_CONFIG_ITEM_FILE_SIZE of 2048 bytes - sizeof(rdm_fversion) gives 2025 bytes.
         This allows room, after the optional header is removed from file, for about 38 persistent mappings. */
      nbytes = snprintf(fline, buffer_remaining, "%s%s%s# Total size for file MUST remain exactly %d bytes!\n%s%s%s%s",
                        rdm_fversion,
                        "#------HEADER: May be removed------\n",
                        "# Do NOT modify Format Version above or file will be regenerated!\n",
                        RDM_CONFIG_ITEM_FILE_SIZE,
                        "# Required line format: \"ID,Device_Name,Mapped_Service,Default_Service,\"\n",
                        "# \"ID,Device_Name\" pair must be valid RDM device ID coupled with correct name!\n",
                        "# \"Mapped_Service\", and \"Default_Service\" if present, must be valid RDM services!\n",
                        "#------HEADER: May be removed------\n");
      fline += nbytes;
      if ((buffer_remaining -= nbytes) > 0)
      {
        /* Update entire file */
        for(dev = RDM_UART1_DEV; dev < RDM_DEV_MAX; dev++ )
        {
          if ( rdm_map[dev].is_persistent &&
               (rdm_map[dev].current_srvc_id != RDM_NULL_SRVC || rdm_map[dev].default_srvc_id != RDM_NULL_SRVC) )
          {
            nbytes = snprintf(fline, buffer_remaining,"%d,%s,%s,%s,\n",
                              dev,
                              rdm_device_names[dev],
                              RDM_GET_SVC_NAME(rdm_map[dev].current_srvc_id),
                              RDM_GET_SVC_NAME(rdm_map[dev].default_srvc_id));
            fline += nbytes;
            if ((buffer_remaining -= nbytes) <= 0)
            {
              ERR("Config buffer exceeded", 0, 0, 0);
              goto BUFFER_FREE;
            }
          }
        }

        // From either remote apps proc or modem we interface to this file using efs_put/efs_get as if it were an item
        // file, but under hood EFS stores it as a regular file because of its large size.
        if (efs_put(rdm_efs_port_map_file,
                    (void *)config_item_buffer, (fs_size_t)RDM_CONFIG_ITEM_FILE_SIZE,
                    O_CREAT, S_IRUSR | S_IWUSR) < 0)
        {
            /* Handle errors if any */
            ERR("efs_put: errcode %d", efs_errno, 0, 0);
        }
      }
      else
        ERR("Config buffer exceeded", 0, 0, 0);

BUFFER_FREE:
      DALSYS_Free((void *)config_item_buffer);
    }
    else
       ERR("rdm_update_efs_entry: failed to allocate config item buffer", 0, 0, 0);
  }
} /* rdm_update_efs_entry() */


/**===========================================================================

FUNCTION RDM_NOTIFY_CLOSE

@brief
  This function process rdm_notify status when RDM is in RDM_CLOSING_STATE.

@dependencies
  None

@return
  boolean
    TRUE : if the port mapping ended, the status is in notify_ptr
    FALSE: if the port mapping hasnot been completed

@sideeffects
  None
===========================================================================*/
static boolean rdm_notify_close
(
  /* Service Task desiring a new port                           */
  rdm_service_enum_type  service,

  /* status of requested task change                            */
  rdm_assign_status_type status,

  /* what to notify the service_requester if the port assign
     is done or aborted                                         */
  rdm_assign_status_type * notify_ptr
)
{
  /* function return            */
  boolean result;

/*------------------------------------------------------------------*/
  DEBUG(MSG_MED("notify(srv: %d, status: %d, state: %d",
                service, status, rdm_state.state););

  ASSERT( (rdm_state.state == RDM_CLOSING_STATE) &&
          (rdm_state.inprocess_service == service) );
  ASSERT( (rdm_state.inprocess_device > RDM_FIRST_DEV) &&
          (rdm_state.inprocess_device < RDM_DEV_MAX));

  /*----------------------------------------------------
      The Service that has Closed the desired port has
      now completed the Close. Proceed to ask
      the proper Service to Open the desired port. Note
      the order is important because informing the service
      task (queuing a command to another task) can cause a
      context switch and a potential invocation of
      "rdm_notify")
      ----------------------------------------------------*/
  if( status == RDM_DONE_S )
  {
    /* Update the current config table. */
    rdm_map[rdm_state.inprocess_device].current_srvc_id = RDM_NULL_SRVC;
    rdm_map[rdm_state.inprocess_device].is_persistent = rdm_state.is_persistent;

    if (rdm_map[rdm_state.inprocess_device].is_persistent)
    {
       /* a port has been close, update the port map */
       rdm_update_efs_entry(rdm_state.inprocess_device);
    }

    /*----------------------------------------------------
      if the service was only told to close (assign(srvc, NULL))
      versus a goto type command, then there is no open to issue,
      and we are done here. Call callback if one provided.
      ----------------------------------------------------*/
    if(rdm_state.desired_device == RDM_NULL_DEV)
    {
      result            = TRUE;
      *notify_ptr       = status;
    }
    else
    {
      /* service holding desired_device                         */
      rdm_service_enum_type     srv_using_desired_device;

      /* command to take                                        */
      rdm_command_enum_type     srv_cmd;


      srv_using_desired_device = rdm_get_service(rdm_state.desired_device);

      if(srv_using_desired_device != RDM_NULL_SRVC)
      {
        /* rdm closed the device the service_requester holds, continue to
           close desired_device so that it can be opened for
           service_requester                                              */
        rdm_state.state                 = RDM_CLOSING_STATE;
        rdm_state.inprocess_service     = srv_using_desired_device;

        srv_cmd                         = RDM_CLOSE_PORT;
      }
      else
      {
        /* we need continue to open the desired_device for service_requester */
        rdm_state.state                 = RDM_OPENING_STATE;
        rdm_state.inprocess_service     = rdm_state.service_requester;

        srv_cmd                         = RDM_OPEN_PORT;
      }

      rdm_state.inprocess_device        = rdm_state.desired_device;

      if(rdm_send_service_cmd(rdm_state.inprocess_service, srv_cmd,
                              rdm_state.inprocess_device))
      {
        /* port mapping continues               */
        result = FALSE;
      }
      else
      {
        /* port mapping aborts                  */
        result          = TRUE;
        *notify_ptr     = RDM_NOT_ALLOWED_S;
      }
    } /* else (rdm_state.desired_device == RDM_NULL_SRVC) */

  } /* if( status == RDM_DONE_S ) */
  else
  {
    /* Close did not occur, reset DEVMAP state machine, we are done */
    result = TRUE;
    *notify_ptr = status;
  }

  return result;
} /* rdm_notify_close */


/**=======================================================================
FUNCTION RDM_REMOVE_DUP_MAPPINGS

@brief:
  If a service maps to multiple ports but the service does not support
  multiple device openning, give only the first port to the service.

@dependencies
  None

@return
  None

@sideeffects
  Some default port mapping or port mapping read from EFS could be
  changed to RDM_NULL_SRVC.
=======================================================================*/
static void rdm_remove_dup_mappings(boolean is_remote)
{
  /* index to traverse rdm_device_enum_type             */
  rdm_device_enum_type  dev_index;

  /* service mapped to dev_index                        */
  rdm_service_enum_type srv;

  /* first device held by srv                           */
  rdm_device_enum_type  first_dev;

/*------------------------------------------------------------------*/

  /* Traverve device from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to
     (RDM_DEV_MAX - 1) to search for duplicates                 */
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
    /* get the service mapped to dev_index              */
    srv         = rdm_get_service(dev_index);

    if(srv != RDM_NULL_SRVC && rdm_service_info[srv].is_remote == is_remote)
    {
      /* get the first device held by srv                 */
      ASSERT( (srv >= RDM_FIRST_SRVC) && (srv < RDM_SRVC_MAX) );
      first_dev   = rdm_get_device(srv);

      if(
         /* srv holds both first_dev and dev_index */
         (first_dev < dev_index) &&
         /* but srv support only one device */
          (rdm_service_info[srv].service_params.device_support !=
          RDM_MULTIPLE_DEV_SUPPORT)
         )
      {
        rdm_map[dev_index].current_srvc_id = RDM_NULL_SRVC;
      }
    }
  }

} /* rdm_remove_dup_mappings() */


/**=======================================================================
FUNCTION RDM_START_CLOSE_DEVICE

@brief:
  Close a device being used by a service.

@dependencies
  The device is not a null device.
  The service is using the device.

@return
  boolean
    TRUE : the closing action is made
    FALSE: the closing request is rejected when
           1) rdm is busy, not in Idle state

@sideeffects
  None

=======================================================================*/
static boolean rdm_start_close_device
(
  /* this service whose device is to be closed          */
  rdm_service_enum_type         service,

  /* device to be closed                                */
  rdm_device_enum_type          device,

  /* call back function                                 */
  rdm_assign_port_cb_type       cbptr
)
{
  /* function return                            */
  boolean       result;

/*------------------------------------------------------------------*/

  /* check parameter                            */
  ASSERT(device > RDM_FIRST_DEV && device < RDM_DEV_MAX);
  ASSERT(service >= RDM_FIRST_SRVC && service < RDM_SRVC_MAX);
  /* service should be using device             */
  ASSERT(RDM_SRV_USE_DEV(service, device) );

  /*-------------------------------------------------------------
    If we made it here, we just request the service to close it's
    port. We setup RDM state variables such that when the notify
    comes in for this close, we do not then issue an open to any
    service.
    -------------------------------------------------------------*/
  rdm_state.state               = RDM_CLOSING_STATE;

  rdm_state.service_requester   = service;
  rdm_state.desired_device      = RDM_NULL_DEV;
  rdm_state.inprocess_service   = service;
  rdm_state.inprocess_device    = device;

  rdm_state.cbptr               = cbptr;

  /* close the device the service holds */
  if(rdm_send_service_cmd(rdm_state.inprocess_service,
                          RDM_CLOSE_PORT, rdm_state.inprocess_device))
  {
    result = TRUE;
  }
  else
  {
    rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;

    /* we are ready to service next request           */
    rdm_reset_state_machine();

    /* inform the requester of the failure */
    if(notify_cb)
    {
      notify_cb(RDM_NOT_ALLOWED_S, service, RDM_NULL_DEV);
    }

    result = FALSE;
  }

  return result;
} /* rdm_start_close_device() */



/**=======================================================================
FUNCTION RDM_START_OPEN_DEVICE

@brief:
  Open a device and assign it to a service.

@dependencies
  None

@return
  boolean
    TRUE : open action is made.
    FALSE: open action is rejected.

@sideeffects
  None

=======================================================================*/
static boolean rdm_start_open_device
(
  /* Service Task desiring a new port    */
  rdm_service_enum_type         service,

  /* Device port to give to Service task */
  rdm_device_enum_type          device,

  /* callback function         */
  rdm_assign_port_cb_type       cbptr,

  /* is this transition persistent? */
  boolean                       is_persistent
)
{
  /* function return                    */
  boolean                       result;

  /* the service who is using device    */
  rdm_service_enum_type         dev_owner;

  /* action to take                     */
  rdm_command_enum_type         srv_cmd;

/*------------------------------------------------------------------*/

  /* check parameters                                           */
  ASSERT( (device > RDM_FIRST_DEV) && (device < RDM_DEV_MAX) );
  ASSERT( (service >= RDM_FIRST_SRVC) && (service < RDM_SRVC_MAX) );

  /* get who is using the device                                */
  dev_owner = rdm_get_service(device);

  /* if the service already holds the device, we are done       */
  if(dev_owner == service)
  {
    /* we are ready to serve the next request,
       unlock the state machine                                 */
    rdm_reset_state_machine();

    if(cbptr)
    {
      cbptr(RDM_DONE_S, service, device);
    }

    result = TRUE;
  }
  else
  {
    if( dev_owner != RDM_NULL_SRVC)
    {
      /*------------------------------------------------------
        The desired port is now in use. Set the DEVMAP State
        and set the in-process Service indicator then prepare
        to ask the correct Service task to close the
        specified port.
        ------------------------------------------------------*/
      rdm_state.state                 = RDM_CLOSING_STATE;
      srv_cmd                         = RDM_CLOSE_PORT;
      rdm_state.inprocess_service     = dev_owner;
    }
    else /* the device is free, no need to close it */
    {
      /*------------------------------------------------------
        Skipping Closing State. Request specified Service
        task to Open specified port. Note the order is
        important because informing the service task (queuing
        a command to another task) can cause a context switch
        and a potential invocation of "rdm_notify")
        ------------------------------------------------------*/
      rdm_state.state                 = RDM_OPENING_STATE;
      srv_cmd                         = RDM_OPEN_PORT;
      rdm_state.inprocess_service     = service;
    }

    rdm_state.service_requester       = service;
    rdm_state.desired_device          = device;
    rdm_state.inprocess_device        = device;
    /* record the callback function */
    rdm_state.cbptr = cbptr;
    rdm_state.is_persistent           = is_persistent;

    if(rdm_send_service_cmd(rdm_state.inprocess_service,
                            srv_cmd, rdm_state.inprocess_device))
    {
      result = TRUE;
    }
    else
    {
      rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;

      /* we are ready to service next request           */
      rdm_reset_state_machine();

      /*------------------------------------------------------
        If rdm_send_service_cmd() returns FALSE, that means the
        service did not register a close routine. This means
        that service cannot use the RDM utility.
        ------------------------------------------------------*/
      if(notify_cb)
      {
        notify_cb(RDM_NOT_ALLOWED_S, service, device);
      }

      /* the request aborts           */
      result = FALSE;
    }
  } /* else (dev_owner == service) */

  return result;

} /* rdm_start_open_device */


/**=======================================================================
FUNCTION RDM_LOCK_STATE

@brief:
  Check if RDM is in IDLE state.  If it is, change it to non-idle state
  so that the RDM will not accept any other port mapping request until
  it finishes its current task and goes back to IDLE.

@dependencies
  Don't call this in ISR context.

@return
  boolean
    TRUE : RDM is in IDLE and change to non-idle.
    FALSE: RDM is not in IDLE state.

@sideeffects
  None

=======================================================================*/
static boolean rdm_lock_state ( void )
{
  /* if RDM can serve the request               */
  boolean       rdm_locked = FALSE;

/*------------------------------------------------------------------*/

  if((rex_is_in_irq_mode()))
  {
    ASSERT(!rex_is_in_irq_mode());
    return rdm_locked;
  }

  /* guarantee atomity of test/set rdm_state

     Enter critical section for test/set rdm_state.
  */
  rdm_enter_crit_section();

  if(rdm_state.state == RDM_IDLE_STATE)
  {
    /* set the state to be locked so that we will reject any other
       request to map the port.                                         */
    rdm_state.state     = RDM_LOCKED_STATE;

    rdm_locked          = TRUE;
  }

  /* Leave critical section */
  rdm_leave_crit_section();

  return rdm_locked;

} /* rdm_lock_state */

#ifndef FEATURE_RDM_NO_MISC_API
#if defined (FEATURE_USB_ON_APPS_PROC) && !defined (RDM_SERVER_ON_ALL_PROC)

/**===========================================================================

FUNCTION usbotg_remote_vv_cb_helper_f

@brief
  This function calls the pm_app_otg_ctrlr_remote_dev_resumed_cb
  which is the real callback function registered from the applications
  processor.  It uses the parameters sent with the message

@dependencies
  None.

@return
  None.

@sideeffects
  Makes an RPC call to the applications processor.
===========================================================================*/
void
usbotg_remote_vv_cb_helper_f
(
  /* ONCRPC Proxy command message */
  oncrpc_proxy_cmd_client_call_type *msg
)
{
  usbotg_remote_vv_f_type cb_func  =
                (usbotg_remote_vv_f_type)msg->data[0];

  /* Make call to callback with parameter data */
  if (cb_func != NULL)
  {
    cb_func();
  }

} /* usbotg_remote_vv_cb_helper_f */



/**===========================================================================

FUNCTION pm_app_otg_remote_vv_cb_handler

@brief
  This function is the callback that is registered locally with the
  pm_app_otg_ctrlr_remote_dev_resumed_cb_handler() function.  When called, this function
  will save parameter information and queue a command with the RPC
  proxy task to call the real callback from the applications processor.
  This is necessary because this callback is called from interrrupt space
  and we cannot make RPC calls from interrupt space.

@dependencies
  None.

@return
  None.

@sideeffects
  Sends a message to RPC proxy task.
===========================================================================*/
void
usbotg_remote_vv_handler
(
  usbotg_remote_vv_f_type cb_func
)
{
  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type *usbotg_remote_vv_cb_msg_ptr;

  usbotg_remote_vv_cb_msg_ptr = remote_apis_client_call_get();

  /* Check that the message pointer is valid */
  ASSERT (usbotg_remote_vv_cb_msg_ptr != NULL);

  if( usbotg_remote_vv_cb_msg_ptr != NULL )
  {

    /* Fill up the pointer data */
    usbotg_remote_vv_cb_msg_ptr->client_call =
      (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
        usbotg_remote_vv_cb_helper_f;

    /* Send parameter data with message */
    usbotg_remote_vv_cb_msg_ptr->data[0] = (uint32) cb_func;

    /* Queue up the message */
    oncprc_proxy_client_call_send( usbotg_remote_vv_cb_msg_ptr );

    /* Explicitly set to NULL */
    usbotg_remote_vv_cb_msg_ptr = NULL;
  }
} /* usbotg_remote_vv_handler */

/**===========================================================================

FUNCTION usbotg_turn_off_modem_isr_control

@brief
  This function call USB clock enable function and gives the interrupt control
  to apps.

@dependencies
  None.

@return
  None.

@sideeffects
  Sends a message to RPC proxy task.
===========================================================================*/

void usbotg_turn_off_modem_isr_control(void)
{
  /* Called based on signal from usb_modem_isr */
  clk_regime_usb_xtal_on();
  if(rex_is_in_irq_mode())
  {
    usbotg_remote_vv_handler(usbotg_turn_on_apps_isr_control);/*RPC to ARM11*/
  }
  else
  {
    usbotg_turn_on_apps_isr_control();
  }
} /* usbotg_turn_off_modem_isr_control */

/**===========================================================================

FUNCTION usbotg_modem_isr

@brief
  This function sets modem usb ISR to NULL and calls for turn off modem
  interrupt control.

@dependencies
  None.

@return
  None.

@sideeffects
  None.
===========================================================================*/

void usbotg_modem_isr(void)
{
      /* Need to disable too or we will never see user code again */
  tramp_set_isr( TRAMP_USB_ISR, NULL );
  /*signal something to be done at task context rather than interrupt context.*/
  usbotg_turn_off_modem_isr_control();
} /* usbotg_modem_isr */

/**===========================================================================

FUNCTION usbotg_turn_on_modem_isr_control

@brief
  This function sets modem usb ISR and calls USB clock disable.

@dependencies
  None.

@return
  None.

@sideeffects
  None.
===========================================================================*/
void usbotg_turn_on_modem_isr_control(void)
{
     tramp_set_isr( TRAMP_USB_ISR, usbotg_modem_isr );
     clk_regime_usb_xtal_off();

#if 0 /* commented for future purpose*/
  if( tramp_is_interrupt_pending( TRAMP_USB_ISR ) == TRUE )
  {
    /* In this case the USB interrupt already occurred and we should just turn on USB again */
    /*signal something to be done at task context rather than interrupt context.*/
      usbotg_turn_off_modem_isr_control();
  }
  else
  {
     tramp_set_isr( TRAMP_USB_ISR, usbotg_modem_isr );
     clk_regime_usb_xtal_off();
  }
#endif
} /* usbotg_turn_on_modem_isr_control */
#else
/* temporary stub, needed because the auto-generated file misc_modem_apis_svc.c
** refers to this function.  Added by <dlansky>
*/
void usbotg_turn_on_modem_isr_control(void){}
#endif /*FEATURE_USB_ON_APPS_PROC*/
#endif /*FEATURE_RDM_NO_MISC_API*/


/* -------------------------------NEW_RDM 3.0------------------------------*/

/**===========================================================================

FUNCTION  RDM_REGISTER_DEVICE                         EXTERNALIZED FUNCTION

@brief
  This function can be used by devices to register itself with RDM

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/

void rdm_register_device(rdm_device_info_type *dev_info)
{
    rdm_device_enum_type device = dev_info->device;
    ASSERT(device < RDM_DEV_MAX && device >= RDM_NULL_DEV);

    rdm_device_info[device].device = dev_info->device;
    (void) strlcpy(rdm_device_info[device].device_name, dev_info->device_name, RDM_MAX_DEV_NAME_LEN);
    rdm_device_info[device].current_service = &(rdm_map[device].current_srvc_id);
    rdm_device_info[device].available = dev_info->available;
    if(dev_info->available)
        rdm_device_info[device].sio_port = dev_info->sio_port;
    else
        rdm_device_info[device].sio_port = SIO_PORT_NULL;
}

/**===========================================================================

FUNCTION  RDM_REGISTER_SERVICE                         EXTERNALIZED FUNCTION

@brief
  This function can be used by services to register itself with RDM

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/

void rdm_register_service(rdm_service_enum_type srvc, rdm_service_params_type *srvc_params)
{
  rdm_service_params_type* service_params = &rdm_service_info[srvc].service_params;

  if(srvc_params->open_func != NULL || srvc_params->close_func != NULL)
  {
      service_params->open_func = srvc_params->open_func;
      service_params->close_func  = srvc_params->close_func;
      service_params->open_multi_func = NULL;
      service_params->close_multi_func = NULL;
      service_params->device_support = RDM_SINGLE_DEV_SUPPORT;
  }
  else if(srvc_params->open_multi_func != NULL || srvc_params->close_multi_func != NULL)
  {
      service_params->open_func = NULL;
      service_params->close_func  = NULL;
      service_params->open_multi_func = srvc_params->open_multi_func;
      service_params->close_multi_func = srvc_params->close_multi_func;
      service_params->data = NULL;
      service_params->device_support = RDM_MULTIPLE_DEV_SUPPORT;
  }
  else
  {
      /* set all parameters to NULL */
      memset(service_params, 0, sizeof(rdm_service_params_type));
  }
  rdm_service_info[srvc].service = srvc;
}

/**===========================================================================

FUNCTION  RDM_SET_COMPATIBILITY                        EXTERNALIZED FUNCTION

@brief
  This function can be used by services to set its compatiblity with device

@dependencies
  Service must register with rdm before calling this function

@return
  None

@sideeffects
  None
===========================================================================*/

void rdm_set_compatibility(rdm_service_enum_type service, rdm_device_enum_type device)
{
    ASSERT(device < RDM_DEV_MAX && service < RDM_SRVC_MAX && service > RDM_NULL_SRVC);
    rdm_service_info[service].dev_compat_array[RDM_DEV_COMPAT_INDEX(device)] |= (1 << RDM_DEV_COMPAT_SHIFT(device));
}

/*===========================================================================

FUNCTION  RDM_CLEAR_COMPATIBILITY                        EXTERNALIZED FUNCTION

@brief
  This function can be used by services to clear its compatiblity with device

@dependencies
  Service must register with rdm before calling this function

@return
  None

@sideeffects
  None
===========================================================================*/
void rdm_clear_compatibility(rdm_service_enum_type service, rdm_device_enum_type device)
{
    ASSERT(device < RDM_DEV_MAX && service < RDM_SRVC_MAX && service > RDM_NULL_SRVC);
    rdm_service_info[service].dev_compat_array[RDM_DEV_COMPAT_INDEX(device)] &= ~(1 << RDM_DEV_COMPAT_SHIFT(device));
}

/**===========================================================================

FUNCTION rdm_register_all_devices

@brief
  This function registers all the devices with RDM

@dependencies
  None.

@return
  None.

@sideeffects
  None.
===========================================================================*/

static void rdm_register_all_devices()
{
  rdm_device_info_type device_info;
  rdm_device_enum_type dev_index;

 /*----RDM_NULL DEV---*/
  device_info.device = RDM_NULL_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "NULL_DEV");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_NULL;
  rdm_register_device(&device_info);
 /*-------------------*/

  /*----Default device registration----*/
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
      device_info.device = dev_index;
      RDM_SET_DEV_NAME(device_info.device_name , "UNKNOWN_DEV");
      device_info.available = FALSE;
      device_info.sio_port = SIO_PORT_NULL;
      rdm_register_device(&device_info);
  }
 /*-------------------*/

 /*=============== UART DEVICES ================*/
 /*----RDM_UART1 DEV---*/
  device_info.device = RDM_UART1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "UART1");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_UART_MAIN;
  rdm_register_device(&device_info);
 /*--------------------*/

 /*----RDM_UART2 DEV----*/
  device_info.device = RDM_UART2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "UART2");
  #ifdef FEATURE_SECOND_UART_ON_ANY_PROC
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_UART_AUX;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*----RDM_UART3 DEV----*/
  device_info.device = RDM_UART3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "UART3");
  #ifdef FEATURE_THIRD_UART_ON_ANY_PROC
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_UART_THIRD;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*============== BLUETOOTH DEVICES ==============*/
 /*---RDM_BT_SPP_DEV---*/
  device_info.device = RDM_BT_SPP_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "BT SPP");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_BT_SPP;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_BT_NA_DEV-----*/
  device_info.device = RDM_BT_NA_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "BT NA");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_BT_NA;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*================== SMD DEVICES =================*/
 /*---RDM_SMEM_DS_DEV---*/
  device_info.device = RDM_SMEM_DS_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMEM DS");
  #if defined(FEATURE_SMEM_DS) && !defined (FEATURE_DATA_ON_APPS)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMEM_DS;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_SMD_DS_DEV---*/
  device_info.device = RDM_SMD_DS_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS uP DATA");
  #if defined(FEATURE_SMD) && !defined (FEATURE_DATA_ON_APPS)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DS;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_SMD_DATA1_DEV----*/
  device_info.device = RDM_SMD_DATA1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS uP DATA1");
  #if defined(FEATURE_SMD)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA1;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA2_DEV-*/
  device_info.device = RDM_SMD_DATA2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS uP DATA2");
  #if defined(FEATURE_SMD)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA2;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_SMD_DATA3_DEV---*/
  device_info.device = RDM_SMD_DATA3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS uP DATA3");
  #if defined(FEATURE_SMD)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA3;
  rdm_register_device(&device_info);
 /*-----------------------*/

 /*---RDM_SMD_DATA4_DEV---*/
  device_info.device = RDM_SMD_DATA4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS uP DATA4");
  #if defined(FEATURE_SMD)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA4;
  rdm_register_device(&device_info);
 /*-----------------------*/

 /*---RDM_SMD_DATA5_DEV---*/
  device_info.device = RDM_SMD_DATA5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA5");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA5;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA6_DEV---*/
  device_info.device = RDM_SMD_DATA6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA6");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA6;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA7_DEV---*/
  device_info.device = RDM_SMD_DATA7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA7");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA7;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA8_DEV---*/
  device_info.device = RDM_SMD_DATA8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA8");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA8;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA9_DEV---*/
  device_info.device = RDM_SMD_DATA9_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA9");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA9;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA11_DEV---*/
  device_info.device = RDM_SMD_DATA11_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS DATA11");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA11;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA12_DEV---*/
  device_info.device = RDM_SMD_DATA12_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA12");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA12;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA13_DEV---*/
  device_info.device = RDM_SMD_DATA13_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA13");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA13;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA14_DEV---*/
  device_info.device = RDM_SMD_DATA14_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA14");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA14;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA23_DEV---*/
  device_info.device = RDM_SMD_DATA23_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA23");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA23;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA24_DEV---*/
  device_info.device = RDM_SMD_DATA24_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA24");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA24;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA25_DEV---*/
  device_info.device = RDM_SMD_DATA25_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA25");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA25;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA26_DEV---*/
  device_info.device = RDM_SMD_DATA26_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA26");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA26;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA27_DEV---*/
  device_info.device = RDM_SMD_DATA27_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA27");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA27;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA28_DEV---*/
  device_info.device = RDM_SMD_DATA28_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA28");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA28;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA29_DEV---*/
  device_info.device = RDM_SMD_DATA29_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA29");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA29;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA30_DEV---*/
  device_info.device = RDM_SMD_DATA30_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA30");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA30;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA31_DEV---*/
  device_info.device = RDM_SMD_DATA31_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA31");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA31;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA32_DEV---*/
  device_info.device = RDM_SMD_DATA32_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA32");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA32;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA33_DEV---*/
  device_info.device = RDM_SMD_DATA33_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA33");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA33;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA34_DEV---*/
  device_info.device = RDM_SMD_DATA34_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA34");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA34;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA35_DEV---*/
  device_info.device = RDM_SMD_DATA35_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA35");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA35;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA36_DEV---*/
  device_info.device = RDM_SMD_DATA36_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA36");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA36;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA37_DEV---*/
  device_info.device = RDM_SMD_DATA37_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA37");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA37;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA38_DEV---*/
  device_info.device = RDM_SMD_DATA38_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA38");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA38;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA39_DEV---*/
  device_info.device = RDM_SMD_DATA39_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA39");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA39;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_DATA40_DEV---*/
  device_info.device = RDM_SMD_DATA40_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "SMD DATA40");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_DATA40;
  rdm_register_device(&device_info);
 /*------------------------*/

 /*---RDM_SMD_NMEA_DEV---*/
  device_info.device = RDM_SMD_NMEA_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "APPS NMEA");
  #ifdef FEATURE_SMD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SMD_GPS_NMEA;
  rdm_register_device(&device_info);
 /*----------------------*/

 /*================== USB DEVICES =================*/
 /*---RDM_USB_MDM_DEV---*/
  device_info.device = RDM_USB_MDM_DEV;
  #ifdef FEATURE_DATA_RM_NET
    RDM_SET_DEV_NAME(device_info.device_name , "USB MDM");
  #else
    RDM_SET_DEV_NAME(device_info.device_name , "USB CDC");
  #endif
  #ifdef FEATURE_USB_CDC_ACM
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_MDM;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_USB_NET_WWAN_DEV----*/
  device_info.device = RDM_USB_NET_WWAN_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB NET WWAN");
  #ifdef FEATURE_DATA_RM_NET
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_NET_WWAN;
  rdm_register_device(&device_info);
  /*---------------------------*/

  /*---RDM_USB_NET_WWAN_DEV2----*/
  device_info.device = RDM_USB_NET_WWAN2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB NET WWAN2");
  #ifdef FEATURE_USB_DATA_RMNET_MULTI
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_RMNET2;
  rdm_register_device(&device_info);
  /*---------------------------*/

  /*---RDM_USB_NET_WWAN_DEV3----*/
  device_info.device = RDM_USB_NET_WWAN3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB NET WWAN3");
  #ifdef FEATURE_USB_DATA_RMNET_MULTI
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_RMNET3;
  rdm_register_device(&device_info);
 /*---------------------------*/

 /*---RDM_USB_NET_WWAN_DEV4----*/
  device_info.device = RDM_USB_NET_WWAN4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB NET WWAN4");
  #ifdef FEATURE_USB_DATA_RMNET_MULTI
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_RMNET4;
  rdm_register_device(&device_info);
 /*---------------------------*/

  /*---RDM_USB_NET_WWAN_DEV5----*/
  device_info.device = RDM_USB_NET_WWAN5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB NET WWAN5");
  #ifdef FEATURE_USB_DATA_RMNET_MULTI
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_RMNET5;
  rdm_register_device(&device_info);
 /*---------------------------*/

 /*---RDM_USB_MMC_DEV-----*/
  device_info.device = RDM_USB_MMC_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB MMC");
  #ifdef FEATURE_USB_PERIPHERAL_MASS_STORAGE
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_MMC;
  rdm_register_device(&device_info);
 /*-----------------------*/

 /*---RDM_USB_DIAG_DEV--*/
  device_info.device = RDM_USB_DIAG_DEV;
  #ifdef FEATURE_DATA_RM_NET
    RDM_SET_DEV_NAME(device_info.device_name , "USB SER1");
  #else
    RDM_SET_DEV_NAME(device_info.device_name , "USB DIAG");
  #endif
  #if (defined(RDM_USES_FSHS_USB) && (defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA) ))
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_SER1;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_USB_NMEA_DEV----*/
  device_info.device = RDM_USB_NMEA_DEV;
  #ifdef FEATURE_DATA_RM_NET
    RDM_SET_DEV_NAME(device_info.device_name , "USB SER2");
  #else
    RDM_SET_DEV_NAME(device_info.device_name , "USB NMEA");
  #endif
  #if defined(RDM_USES_FSHS_USB) &&  defined(FEATURE_USB_DIAG_NMEA)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_SER2;
  rdm_register_device(&device_info);
 /*-----------------------*/

  /*---RDM_USB_SER3_DEV----*/
  device_info.device = RDM_USB_SER3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB SER3");
  #ifdef FEATURE_HS_USB_SER3_PORT
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_SER3;
  rdm_register_device(&device_info);
 /*-----------------------*/

  /*---RDM_USB_SMCT_DEV----*/
  device_info.device = RDM_USB_SMCT_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB SMCT");
  #ifdef FEATURE_HS_USB_SMCT_PORT
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_SMCT;
  rdm_register_device(&device_info);
 /*-----------------------*/

  /*---RDM_USB_SER4_DEV----*/
  device_info.device = RDM_USB_SER4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB SER4");
  #ifdef FEATURE_HS_USB_SER4_PORT
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_SER4;
  rdm_register_device(&device_info);
 /*-----------------------*/

  /*---RDM_USB_QDSS_DEV----*/
  device_info.device = RDM_USB_QDSS_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB QDSS");
  #ifdef FEATURE_HS_USB_QDSS_PORT
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_QDSS;
  rdm_register_device(&device_info);
 /*-----------------------*/


  /*---RDM_USB_CSVT_DEV----*/
  device_info.device = RDM_USB_CSVT_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB CSVT");
  #ifdef FEATURE_HS_USB_CSVT_PORT
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_CSVT;
  rdm_register_device(&device_info);
 /*-----------------------*/

 /*---RDM_USB_BREW_EXTENDED_DATA_DEV---*/
  device_info.device = RDM_USB_BREW_EXTENDED_DATA_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "BREW EX DATA");
  #if defined (FEATURE_USB) && defined( FEATURE_USB_BREW_EXTENDED_DEVICE)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_BREW_EXTENDED_DATA;
  rdm_register_device(&device_info);
 /*--------------------------------*/

 /*---RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV---*/
  device_info.device = RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "USB EX NOTI");
  #if defined (FEATURE_USB) && defined( FEATURE_USB_BREW_EXTENDED_DEVICE)
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_USB_BREW_EXTENDED_NOTIFICATION;
  rdm_register_device(&device_info);
 /*----------------------------------------*/

 /*================== APS DEVICES =================*/
 /*---RDM_APS_SERIAL_VSP_DEV---*/
  device_info.device = RDM_APS_SERIAL_VSP_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "3RD PARTY OS");
  #ifdef FEATURE_APS
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_APS_SERIAL_VSP;
  rdm_register_device(&device_info);
 /*----------------------------*/

 /*================== LSM DEVICES =================*/
 /*---RDM_LSM_CSD_DEV--*/
  device_info.device = RDM_LSM_CSD_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "LSM CSD");
  #ifdef FEATURE_MMGPS_CSD
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_LSM_CSD;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*================== A2  DEVICES =================*/
 /*---RDM_A2_MDM_DEV--*/
  device_info.device = RDM_A2_MDM_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 MDM");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_MDM;
  rdm_register_device(&device_info);
 /*---------------------*/

  /*---RDM_A2_RMNET_1_DEV--*/
  device_info.device = RDM_A2_RMNET_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 RMNET1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_RMNET_1;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_A2_RMNET_2_DEV--*/
  device_info.device = RDM_A2_RMNET_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 RMNET2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_RMNET_2;
  rdm_register_device(&device_info);
 /*---------------------*/

 /*---RDM_A2_RMNET_3_DEV--*/
  device_info.device = RDM_A2_RMNET_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 RMNET3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_RMNET_3;
  rdm_register_device(&device_info);
 /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_0_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_0_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET0");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_0;
  rdm_register_device(&device_info);
  /*---------------------*/

 /*---RDM_SDIO_MUX_A2_RMNET_1_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_1;
  rdm_register_device(&device_info);
  /*---------------------*/

 /*---RDM_SDIO_MUX_A2_RMNET_2_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_3_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_4_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET4");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_4;
  rdm_register_device(&device_info);
  /*---------------------*/

/*---RDM_SDIO_MUX_A2_RMNET_5_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET5");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_5;
  rdm_register_device(&device_info);
  /*---------------------*/

 /*---RDM_SDIO_MUX_A2_RMNET_6_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET6");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_7_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET7");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_7;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_8_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET8");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_8;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_9_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_9_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET9");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_9;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_10_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_10_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO RMNET10");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_10;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_RMNET_TETH_0_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_RMNET_TETH_0_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 RMNET TETH0");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_RMNET_TETH_0;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_DUN_0_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_DUN_0_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO DUN0");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_DUN_0;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CSVT_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CSVT_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CSVT");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CSVT;
  rdm_register_device(&device_info);
  /*---------------------*/


  /*---RDM_SDIO_MUX_A2_CH_0_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_0_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_0");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_0;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_1_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_2_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_3_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_4_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_4");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_5_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_5");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_5;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_6_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_6");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_CH_7_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_CH_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO CH_7");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_CH_7;
  rdm_register_device(&device_info);
  /*---------------------*/


  /*---RDM_SDIO_MUX_A2_REV_CH_0_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_0_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_0");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_0;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_1_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_2_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_3_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_4_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_4");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_5_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_5");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_5;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_6_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_6");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_7_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_7");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_7;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_CH_8_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_CH_8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV_CH_8");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_CH_8;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_0_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_0_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP0");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_0;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_1_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_2_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_3_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_4_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP4");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_5_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP5");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_5;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_6_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP6");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_7_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP7");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_7;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_SDIO_MUX_A2_REV_IP_8_DEV--*/
  device_info.device = RDM_SDIO_MUX_A2_REV_IP_8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 SDIO REV IP8");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_SDIO_MUX_A2_REV_IP_8;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_PHYS_1_DEV---*/
  device_info.device = RDM_A2_DS_PHYS_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS PHYS_1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif 
  device_info.sio_port = SIO_PORT_A2_DS_PHYS_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_PHYS_2_DEV---*/
  device_info.device = RDM_A2_DS_PHYS_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS PHYS_2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif  
  device_info.sio_port = SIO_PORT_A2_DS_PHYS_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_PHYS_3_DEV---*/
  device_info.device = RDM_A2_DS_PHYS_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS PHYS_3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif 
  device_info.sio_port = SIO_PORT_A2_DS_PHYS_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_PHYS_4_DEV---*/
  device_info.device = RDM_A2_DS_PHYS_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS PHYS_4");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_PHYS_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_1_DEV---*/
  device_info.device = RDM_A2_DS_CH_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_1");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_2_DEV---*/
  device_info.device = RDM_A2_DS_CH_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_2");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_3_DEV---*/
  device_info.device = RDM_A2_DS_CH_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_3");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_4_DEV---*/
  device_info.device = RDM_A2_DS_CH_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_4");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_5_DEV---*/
  device_info.device = RDM_A2_DS_CH_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_5");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_5;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_6_DEV---*/
  device_info.device = RDM_A2_DS_CH_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_6");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_7_DEV---*/
  device_info.device = RDM_A2_DS_CH_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_7");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_7;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_8_DEV---*/
  device_info.device = RDM_A2_DS_CH_8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_8");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_8;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_9_DEV---*/
  device_info.device = RDM_A2_DS_CH_9_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_9");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_9;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_10_DEV---*/
  device_info.device = RDM_A2_DS_CH_10_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_10");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_10;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_11_DEV---*/
  device_info.device = RDM_A2_DS_CH_11_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_11");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_11;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_12_DEV---*/
  device_info.device = RDM_A2_DS_CH_12_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_12");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_12;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_13_DEV---*/
  device_info.device = RDM_A2_DS_CH_13_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_13");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_13;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_14_DEV---*/
  device_info.device = RDM_A2_DS_CH_14_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_14");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_14;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_15_DEV---*/
  device_info.device = RDM_A2_DS_CH_15_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_15");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_15;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_16_DEV---*/
  device_info.device = RDM_A2_DS_CH_16_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_16");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_16;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_17_DEV---*/
  device_info.device = RDM_A2_DS_CH_17_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_17");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_17;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_18_DEV---*/
  device_info.device = RDM_A2_DS_CH_18_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_18");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_18;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_19_DEV---*/
  device_info.device = RDM_A2_DS_CH_19_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_19");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_19;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_A2_DS_CH_20_DEV---*/
  device_info.device = RDM_A2_DS_CH_20_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "A2 DS CH_20");
  #ifdef FEATURE_A2
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_A2_DS_CH_20;
  rdm_register_device(&device_info);
  /*---------------------*/
  
  /*============= ONDEVICE DIAG  DEVICES ============*/
  /*---RDM_ONDEVICE_DIAG_DEV---*/
  device_info.device =  RDM_ONDEVICE_DIAG_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "OnDevice SD");
  #ifdef FEATURE_ONDEVICE_DIAG
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_ONDEVICE_DIAG;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*============= ONDEVICE DIAG USB AUTO DEVICES ============*/
  /*---RDM_ONDEVICE_DIAG_USB_AUTO_DEV---*/
  device_info.device =  RDM_ONDEVICE_DIAG_USB_AUTO_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "Ondevice Auto");
  #ifdef FEATURE_ONDEVICE_DIAG_AUTO
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_ONDEVICE_DIAG_USB_AUTO;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*============= QBI DIAG USB DEVICE ============*/
  /*---RDM_QBI_DIAG_DEV---*/
  device_info.device = RDM_QBI_DIAG_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "QBI DIAG");
  #ifdef FEATURE_QBI
    device_info.available = TRUE;
  #else
    device_info.available = FALSE;
  #endif
  device_info.sio_port = SIO_PORT_QBI_DIAG;
  rdm_register_device(&device_info);
  /*---------------------*/
  #ifdef FEATURE_SIO_IPA_SUPPORT
/*============= IPA DEVICES  ============*/
  // Removing the featurization on IPA devices. Don't integrate these devices without request.
  /*---RDM_IPA_DS_PHYS_1_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_1");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_2_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_2");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_3_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_3");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_4_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_4");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_5_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_5");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_5;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_6_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_6");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_7_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_7");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_7;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_PHYS_8_DEV---*/
  device_info.device = RDM_IPA_DS_PHYS_8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS PHYS_8");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_PHYS_8;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_1_DEV---*/
  device_info.device = RDM_IPA_DS_CH_1_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_1");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_1;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_2_DEV---*/
  device_info.device = RDM_IPA_DS_CH_2_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_2");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_2;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_3_DEV---*/
  device_info.device = RDM_IPA_DS_CH_3_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_3");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_3;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_4_DEV---*/
  device_info.device = RDM_IPA_DS_CH_4_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_4");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_4;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_5_DEV---*/
  device_info.device = RDM_IPA_DS_CH_5_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_5");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_5;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_6_DEV---*/
  device_info.device = RDM_IPA_DS_CH_6_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_6");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_6;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_7_DEV---*/
  device_info.device = RDM_IPA_DS_CH_7_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_7");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_7;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_8_DEV---*/
  device_info.device = RDM_IPA_DS_CH_8_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_8");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_8;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_9_DEV---*/
  device_info.device = RDM_IPA_DS_CH_9_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_9");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_9;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_10_DEV---*/
  device_info.device = RDM_IPA_DS_CH_10_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_10");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_10;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_11_DEV---*/
  device_info.device = RDM_IPA_DS_CH_11_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_11");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_11;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_12_DEV---*/
  device_info.device = RDM_IPA_DS_CH_12_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_12");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_12;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_13_DEV---*/
  device_info.device = RDM_IPA_DS_CH_13_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_13");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_13;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_14_DEV---*/
  device_info.device = RDM_IPA_DS_CH_14_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_14");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_14;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_15_DEV---*/
  device_info.device = RDM_IPA_DS_CH_15_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_15");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_15;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_16_DEV---*/
  device_info.device = RDM_IPA_DS_CH_16_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_16");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_16;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_17_DEV---*/
  device_info.device = RDM_IPA_DS_CH_17_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_17");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_17;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_18_DEV---*/
  device_info.device = RDM_IPA_DS_CH_18_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_18");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_18;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_19_DEV---*/
  device_info.device = RDM_IPA_DS_CH_19_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_19");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_19;
  rdm_register_device(&device_info);
  /*---------------------*/

  /*---RDM_IPA_DS_CH_20_DEV---*/
  device_info.device = RDM_IPA_DS_CH_20_DEV;
  RDM_SET_DEV_NAME(device_info.device_name , "IPA DS CH_20");
  device_info.available = TRUE;
  device_info.sio_port = SIO_PORT_IPA_DS_CH_20;
  rdm_register_device(&device_info);
  /*---------------------*/
  #endif
}

/**===========================================================================

FUNCTION rdm_register_all_services

@brief
  This function registers all the devices with RDM

@dependencies
  None.

@return
  None.

@sideeffects
  None.
===========================================================================*/

static void rdm_register_all_services()
{
  rdm_service_enum_type srv_index;

  /* All of the following services are using legacy apis
     for service function registration, only compatiblity
     for these services are set in RDM.
     Newer services needs to set its compatiblity directly */
   memset(rdm_service_info, 0, RDM_SRVC_MAX * sizeof(rdm_service_info_type));

  /*---RDM_DIAG_SRVC---*/
   #ifndef FEATURE_BT_SOC_UART_TL
        rdm_set_compatibility(RDM_DIAG_SRVC, RDM_UART1_DEV);
   #endif
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_UART2_DEV);
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_UART3_DEV);
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_USB_MDM_DEV);
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_USB_DIAG_DEV);
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMEM_DS_DEV);
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_ONDEVICE_DIAG_DEV);
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_ONDEVICE_DIAG_USB_AUTO_DEV);
   #ifdef FEATURE_QBI
   rdm_set_compatibility(RDM_DIAG_SRVC, RDM_QBI_DIAG_DEV);
   #endif
   #ifdef FEATURE_DIAG_SUPPORT_WIRELESS
        rdm_set_compatibility(RDM_DIAG_SRVC, RDM_BT_SPP_DEV);
   #endif
  /*-------------------*/

  /*---RDM_DATA_SRVC---*/
    #if !((defined FEATURE_BT_SOC_UART_TL) || (defined SIORS_UART1_USES_4WIRE_ONLY))
        rdm_set_compatibility(RDM_DATA_SRVC, RDM_UART1_DEV);
    #endif
    #ifdef FEATURE_DS_MAPPABLE_TO_UART2
        rdm_set_compatibility(RDM_DATA_SRVC, RDM_UART2_DEV);
    #endif
    #ifdef FEATURE_DS_MAPPABLE_TO_UART3
        rdm_set_compatibility(RDM_DATA_SRVC, RDM_UART3_DEV);
    #endif
    #if defined (FEATURE_DS) || defined (FEATURE_DATA)
        rdm_set_compatibility(RDM_DATA_SRVC, RDM_USB_MDM_DEV);
    #endif
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_BT_NA_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMEM_DS_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_LSM_CSD_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMD_DS_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMD_DATA1_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMD_DATA2_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMD_DATA3_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMD_DATA4_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMD_DATA11_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_USB_SER3_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_SDIO_MUX_A2_CSVT_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_USB_SMCT_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_USB_SER4_DEV);
    rdm_set_compatibility(RDM_DATA_SRVC, RDM_USB_CSVT_DEV);

    #ifdef FEATURE_SMUX
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_0_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_1_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_2_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_3_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_4_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_5_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_6_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_7_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_8_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_9_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_10_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_11_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_12_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_13_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_14_DEV);
       rdm_set_compatibility(RDM_DATA_SRVC, RDM_SMUX_15_DEV);

       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_0_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_1_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_2_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_3_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_4_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_5_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_6_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_7_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_8_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_9_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_10_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_11_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_12_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_13_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_14_DEV);
       rdm_set_compatibility(RDM_DIAG_SRVC, RDM_SMUX_15_DEV);
    #endif

  /*-------------------*/

  /*---RDM_BT_HCI_SRVC---*/
    #ifndef FEATURE_BT_SOC_UART_TL
        rdm_set_compatibility(RDM_BT_HCI_SRVC, RDM_UART1_DEV);
    #endif
    rdm_set_compatibility(RDM_BT_HCI_SRVC, RDM_UART2_DEV);
    rdm_set_compatibility(RDM_BT_HCI_SRVC, RDM_UART3_DEV);
    rdm_set_compatibility(RDM_BT_HCI_SRVC, RDM_USB_MDM_DEV);

  /*---------------------*/

  /*---RDM_BT_SOC_SRVC---*/
    #if defined (FEATURE_BT_SOC) && defined (FEATURE_BT_SOC_UART_TL)
        rdm_set_compatibility(RDM_BT_SOC_SRVC, RDM_UART1_DEV);
    #endif
  /*---------------------*/

  /*---RDM_MMC_SRVC---*/
   rdm_set_compatibility(RDM_MMC_SRVC, RDM_NULL_DEV);
  /*------------------*/

  /*---RDM_NMEA_SRVC---*/
   #if defined (FEATURE_GPSONE_PE) || defined (FEATURE_MMGPS) || defined (FEATURE_CGPS)
       #ifndef FEATURE_BT_SOC_UART_TL
            rdm_set_compatibility(RDM_NMEA_SRVC, RDM_UART1_DEV);
       #endif
       rdm_set_compatibility(RDM_NMEA_SRVC, RDM_UART2_DEV);
       rdm_set_compatibility(RDM_NMEA_SRVC, RDM_UART3_DEV);
       #ifdef FEATURE_SMD
            rdm_set_compatibility(RDM_NMEA_SRVC, RDM_SMEM_DS_DEV);
       #endif
   #endif
   rdm_set_compatibility(RDM_NMEA_SRVC, RDM_USB_NMEA_DEV);
  /*-------------------*/

  /*---RDM_RPC_SRVC---*/
   #ifdef FEATURE_ONCRPC
        #ifndef FEATURE_BT_SOC_UART_TL
            rdm_set_compatibility(RDM_RPC_SRVC, RDM_UART1_DEV);
        #endif
        #ifdef FEATURE_DS_MAPPABLE_TO_UART2
            rdm_set_compatibility(RDM_RPC_SRVC, RDM_UART2_DEV);
        #endif
        #ifdef FEATURE_DS_MAPPABLE_TO_UART3
            rdm_set_compatibility(RDM_RPC_SRVC, RDM_UART3_DEV);
        #endif
         rdm_set_compatibility(RDM_RPC_SRVC, RDM_USB_MDM_DEV);
   #endif
  /*------------------*/

  /*---RDM_DATA_WWAN_SRVC---*/
   rdm_set_compatibility(RDM_DATA_WWAN_SRVC, RDM_USB_NET_WWAN_DEV);
  /*------------------------*/

  /*---SET RDM_NULL_DEV for all the services---*/
   for(srv_index=RDM_FIRST_SRVC; srv_index < RDM_SRVC_MAX; srv_index++)
       rdm_set_compatibility(srv_index, RDM_NULL_DEV);
}

/**===========================================================================

FUNCTION RDM_SEND_SERVICE_CMD

@brief
  This function will call the registered open or close routine provided by
  the service task. If the service task did not register both an open and
  close routine, then it cannot use the RDM services.

@dependencies
  None

@return
  Boolean - False means the Service has not registered an open or close
  routine.

@sideeffects
  None
===========================================================================*/
static boolean rdm_send_service_cmd
(
  /* the service executing the cmd              */
  rdm_service_enum_type  service,

  /* the cmd                                    */
  rdm_command_enum_type  port_cmd,

  /* the device which the cmd act on            */
  rdm_device_enum_type   device
)
{
    /* function return                                     */
  boolean result;
  /* sio port type corresponding to device                  */
  sio_port_id_type sio_port;
  /*  service parameters that holds open and close func ptr */
  rdm_service_params_type *service_params;
/*------------------------------------------------------------------*/
  DEBUG(MSG_MED("rdm_send_service_cmd(SRV=%d, CMD=%d, DEV=%d)",
                service, port_cmd, device););

  /* the parameters should be correct   */
  ASSERT( (device > RDM_FIRST_DEV) && (device < RDM_DEV_MAX));
  ASSERT( (service >= RDM_FIRST_SRVC) && (service < RDM_SRVC_MAX));

  sio_port = rdm_device_info[device].sio_port;
  service_params = &(rdm_service_info[service].service_params);

  if(service_params->device_support == RDM_SINGLE_DEV_SUPPORT)
  {
      switch(port_cmd)
      {
      case RDM_OPEN_PORT:
          ASSERT(rdm_state.state == RDM_OPENING_STATE || rdm_state.state == RDM_INIT_STATE || rdm_remote_init_done == FALSE);
          (service_params->open_func)(sio_port);
          result = TRUE;
          break;
      case RDM_CLOSE_PORT:
          ASSERT(rdm_state.state == RDM_CLOSING_STATE || rdm_state.state == RDM_INIT_STATE || rdm_remote_init_done == FALSE);
          (service_params->close_func)();
          result = TRUE;
          break;
      default:
          ERR("Invalid cmd: %d", port_cmd, 0, 0);
          result = FALSE;
          break;
      }
  }
  else if(service_params->device_support == RDM_MULTIPLE_DEV_SUPPORT)
  {
      switch(port_cmd)
      {
      case RDM_OPEN_PORT:
          ASSERT(rdm_state.state == RDM_OPENING_STATE || rdm_state.state == RDM_INIT_STATE || rdm_remote_init_done == FALSE);
          (service_params->open_multi_func)(sio_port, service_params->data);
          result = TRUE;
          break;
      case RDM_CLOSE_PORT:
          ASSERT(rdm_state.state == RDM_CLOSING_STATE || rdm_state.state == RDM_INIT_STATE || rdm_remote_init_done == FALSE);
          (service_params->close_multi_func)(sio_port, service_params->data);
          result = TRUE;
          break;
      default:
          ERR("Invalid cmd: %d", port_cmd, 0, 0);
          result = FALSE;
          break;
      }
  }
  else
  {
      MSG_ERROR("Open and close callbacks not registered",0,0,0);
      return FALSE;
  }

  return result;
}

/**===========================================================================

FUNCTION RDM_GET_DEVICE_STR                             EXTERNALIZED FUNCTION

@brief
  Query the RDM for the ASCII string that corresponds to the
  rdm_device_enum_type specified.

  Note:This is used by the UI, and only 13 bytes of storage will be
  available in *str. One byte is for the NULL.

@dependencies
  The device must be in rdm_device_enum_type. The str has pre-allocated
  space of at least 13 chars including null-terminator.

@return
  Boolean TRUE if the device is valid, else FALSE.

@sideeffects
  The contents of pointer str will be written to. If the specified device
  is not valid, str is set to NULL.
===========================================================================*/
boolean rdm_get_device_str
(
  rdm_device_enum_type  device, /* device to look up            */
  char                  *str    /* where to write the string    */
)
{
  boolean result;
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX)
  {
    str[0] = '\0';
    result = FALSE;
  }
  else
  {
    (void) strlcpy(str, rdm_device_info[device].device_name, RDM_MAX_DEV_NAME_LEN);
    result = TRUE;
  }

  return result;
}

/**============================================================================

FUNCTION  RDM_UPDATE_SRV_DEV_COMPAT_TABLE                    INTERNAL FUNCTION

@brief
  update rdm_srv_dev_compat_table according to network_device_flag.

  If USB is NETWORK device, USB_SER2 port/interface is not available to any service.
  If USB is MODEM device, USB_NET_WWAN interface is not available to any service.

@dependencies
  None

@return
  None

@sideeffects
  rdm_srv_dev_compat_table can be modified
=============================================================================*/
void   rdm_update_srv_dev_compat_table(boolean network_device_flag)
{
  int  i;
  rdm_device_enum_type device;
  device = (network_device_flag) ? RDM_USB_SER2_DEV : RDM_USB_NET_WWAN_DEV;
  for (i = RDM_FIRST_SRVC; i<= RDM_LAST_KNOWN_SRVC; i++ )
  {
    rdm_clear_compatibility((rdm_service_enum_type)i, device);
  }
}

/**===========================================================================

FUNCTION RDM_IS_SRV_HOLDING_DEV                         EXTERNALIZED FUNCTION

@brief
  Test if a service is using a device.

@dependencies
  None

@return
  Boolean
    TRUE if the service is holding the device or
         if the service does not hold any device and the second
            parameter is RDM_NULL_DEV
    FALSE if the service is not holding the device or
          if the parameters are not right

@sideeffects
  None

===========================================================================*/
boolean rdm_is_srv_holding_dev
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
)
{
  /* function return                                    */
  boolean       result;
  boolean       srvc_busy;

  /* index for device */
  rdm_device_enum_type  dev_index;

/*------------------------------------------------------------------*/

  /* Check if service & device are w/i range    */
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX ||
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    return FALSE;
  }

  result = FALSE;
  srvc_busy = FALSE;
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
      if(rdm_get_service(dev_index) == service){
          srvc_busy = TRUE;
          if(dev_index == device)
          {
            result = TRUE;
            break;
          }
      }
  }

  /* the service does not hold any device and the second
     parameter is RDM_NULL_DEV                                  */
  if(srvc_busy == FALSE && device == RDM_NULL_DEV)
  {
      result = TRUE;
  }

  return result;
} /* rdm_is_srv_holding_dev */

/**===========================================================================

FUNCTION  RDM_ISSUE_OPENS_COMMON

@brief
  RDM issues opens to SRVCs at the power up time based on last mapping
  before power down.

  Services using the RDM should not just open a port when the MSM boots.
  RDM will call the registered open routines for the SRVC's if they should
  open a port at boot.

  If EFS is being used, whatever the previous mapping was will be reinstated.
  If there was no previous mapping, DS will open UART1, and DIAG will open
  UART2 if it is defined.

@dependencies
  None

@return
  None

@sideeffects
  Instruct tasks to open specific shared serial devices.
===========================================================================*/
static void rdm_issue_opens_common(boolean is_remote)
{
  int *rdm_init_num;

    /* index to traverse rdm_device_enum_type             */
  rdm_device_enum_type dev_index;

  /* service mapped to dev_index                        */
  rdm_service_enum_type srv;

/*------------------------------------------------------------------*/
  rdm_get_prev_done = TRUE;
  if(rdm_remote_issue_pending)
  {
      rdm_remote_issue_pending = FALSE;
      /* Issue opens for remote processor */
      rdm_issue_opens_common(TRUE);
  }

  DEBUG(MSG_MED("rdm_issue_opens() - is_remote = %d",is_remote,0,0););

  /* Remove mapping for unsupported devices on current build */
  rdm_remove_unsupported_mappings(is_remote);
  /* Remove duplicate service mapping for services that don't
     support multiple devices                                */
  rdm_remove_dup_mappings(is_remote);

  /* init rdm_req_init_num          */
  if(is_remote)
  {
      rdm_init_num = &rdm_remote_req_init_num;
      *rdm_init_num = 0;
      rdm_remote_init_done = FALSE;
  }
  else
  {
      rdm_init_num = &rdm_req_init_num;
      *rdm_init_num = 0;
  }

  /* we will decides how many openings will be issued by traverving
     from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to (RDM_DEV_MAX - 1)        */
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
    srv = rdm_get_service(dev_index);

    if(srv != RDM_NULL_SRVC && rdm_service_info[srv].is_remote == is_remote)
    {
      /* see how many requests we are going to make     */
      ASSERT( (srv >= RDM_FIRST_SRVC) && (srv < RDM_SRVC_MAX) );

      /* check if the service has registered open/close functions */
      if(rdm_service_info[srv].service_params.device_support != RDM_NO_DEV_SUPPORT)
      {
          (*rdm_init_num)++;
      }
    }
  }

  if(*rdm_init_num > 0)
  {
      for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
          dev_index < RDM_DEV_MAX; dev_index++)
      {
          srv = rdm_get_service(dev_index);
          if(srv != RDM_NULL_SRVC &&
             rdm_service_info[srv].is_remote == is_remote &&
             rdm_service_info[srv].service_params.device_support != RDM_NO_DEV_SUPPORT)
          {
              (void) rdm_send_service_cmd(srv, RDM_OPEN_PORT, dev_index);
          }
      }
  }
  else
  {
      if(!is_remote)
      {
         /* we are ready to service next request           */
         rdm_reset_state_machine();
      }
      else
      {
        rdm_remote_init_done = TRUE;
      }
  }
}


/**===========================================================================

FUNCTION  RDM_ISSUE_OPENS                               EXTERNALIZED FUNCTION

@brief
  RDM issues opens to SRVCs at the power up time based on last mapping
  before power down.

  Services using the RDM should not just open a port when the MSM boots.
  RDM will call the registered open routines for the SRVC's if they should
  open a port at boot.

  If EFS is being used, whatever the previous mapping was will be reinstated.
  If there was no previous mapping, DS will open UART1, and DIAG will open
  UART2 if it is defined.

@dependencies
  None

@return
  None

@sideeffects
  Instruct tasks to open specific shared serial devices.
===========================================================================*/
void rdm_issue_opens(void)
{
    boolean is_remote = FALSE;
    rdm_issue_opens_common(is_remote);
} /* rdm_issue_opens() */

/**===========================================================================

FUNCTION  RDM_REMOVE_UNSUPPORTED_MAPPINGS             EXTERNALIZED FUNCTION

@brief
  Removes service mappings for the device that are not available

@dependencies
  None

@return
  None

@sideeffects
  Port mappings of previous build may not be applicable

===========================================================================*/
static void rdm_remove_unsupported_mappings(boolean is_remote)
{
  /* index to traverse rdm_device_enum_type                           */
  rdm_device_enum_type dev_index;
  rdm_service_enum_type prev_srvc;
  /*------------------------------------------------------------------*/

  /* Persistant default mappings */
  /* map Data service on SMD devices if not present */
  rdm_map[RDM_SMD_DS_DEV].current_srvc_id     = RDM_DATA_SRVC;
  rdm_map[RDM_SMD_DATA1_DEV].current_srvc_id  = RDM_DATA_SRVC;
  rdm_map[RDM_SMD_DATA2_DEV].current_srvc_id  = RDM_DATA_SRVC;
  rdm_map[RDM_SMD_DATA3_DEV].current_srvc_id  = RDM_DATA_SRVC;
  rdm_map[RDM_SMD_DATA4_DEV].current_srvc_id  = RDM_DATA_SRVC;
  rdm_map[RDM_SMD_DATA11_DEV].current_srvc_id = RDM_DATA_SRVC;

  rdm_map[RDM_MUX_PHYSICAL_DEV].current_srvc_id = RDM_DATA_MUX_SRVC;

  /* USB_MMC_DEV is not allowed to use at boot up */
  rdm_map[RDM_USB_MMC_DEV].current_srvc_id = RDM_NULL_SRVC;

  /*-------------------------------------------------------------
     Check if previous port map for each port is allowed now.
     A service availabe in an old build maybe incompatible in a new
     build or the dvice may not be available in new build.
     In this case, it is not allowed to map to any device.
   --------------------------------------------------------------*/
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
      prev_srvc = rdm_map[dev_index].current_srvc_id;
      if (!(rdm_device_info[dev_index].available) ||
           (prev_srvc < RDM_NULL_SRVC || prev_srvc >= RDM_SRVC_MAX) ||
           (prev_srvc != RDM_NULL_SRVC && !RDM_SD_COMPAT(prev_srvc, dev_index)) ||
           ((prev_srvc == RDM_BREW_SRVC || prev_srvc == RDM_BREW_EXTENDED_SRVC) &&
            rdm_service_info[prev_srvc].is_remote == is_remote)
         )
      {
          rdm_map[dev_index].current_srvc_id = RDM_NULL_SRVC;
      }

  }
}

/*--------------------------------------------------------------------------
NOTE: Following functions provide backward compatibility for RDM 2.0 Clients
      RDM 3.0 clients (services) needs to register its service params using
      new service registration apis.
--------------------------------------------------------------------------*/

/**===========================================================================

FUNCTION RDM_REGISTER_OPEN_FUNC                         EXTERNALIZED FUNCTION

@brief
  Function for Service Tasks to register their open function. This open
  function is what RDM calls to instruct a Service to open a specified port.

  The function is used in conjunction with rdm_register_close_func.

  The service can only hold one device at a time if it registers
  its open function by this function. If the service holds dev1 and requests
  RDM to map dev2 to it, RDM will take dev1 from the service and then assign
  dev2 to the service.

  Please use rdm_register_func_multi_dev if you want multi-dev support.

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/
void rdm_register_open_func
(
  /* What Service is registering the function */
  rdm_service_enum_type             service,

  /* the routine to be called for opening */
  rdm_service_open_func_ptr_type    open_func
)
{
  DEBUG(MSG_MED("SRVC: %d, open: %x)", service, open_func, 0););

  /* check the parameter                                        */
  if(service >= RDM_SRVC_MAX || service < RDM_FIRST_SRVC)
  {
    ERR("wrong service: %d", service,0,0);
    return;
  }

  if(open_func == NULL)
  {
    ERR("null open_func",0,0,0);
    return;
  }

  rdm_service_info[service].service_params.device_support = RDM_SINGLE_DEV_SUPPORT;
  rdm_service_info[service].service_params.open_func      = open_func;
  rdm_service_info[service].service                       = service;

} /* rdm_register_open_func() */


/**===========================================================================

FUNCTION RDM_REGISTER_CLOSE_FUNC                        EXTERNALIZED FUNCTION

@brief
  Function for Service Tasks to register their close function. This close
  function is what RDM calls to instruct a Service to close it's open port.

  The function is used in conjunction with rdm_register_open_func.

  By using this function, a service can have only one port open, hence
  no sio_port_id_type argument in the close function.

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/
void rdm_register_close_func
(
  /* What Service is registering the function */
  rdm_service_enum_type              service,

  /* the routine to be called for closing */
  rdm_service_close_func_ptr_type    close_func
)
{
  DEBUG(MSG_MED("SRVC: %d, close: %x)", service, close_func, 0););

  /* check the parameter                                        */
  if(service >= RDM_SRVC_MAX || service < RDM_FIRST_SRVC)
  {
    ERR("wrong service: %d", service,0,0);
    return;
  }

  if(close_func == NULL)
  {
    ERR("null close_func",0,0,0);
    return;
  }

  rdm_service_info[service].service_params.device_support = RDM_SINGLE_DEV_SUPPORT;
  rdm_service_info[service].service_params.close_func     = close_func;

} /* rdm_register_close_func() */


/**===========================================================================

FUNCTION RDM_REGISTER_FUNC_MULTI_DEV                    EXTERNALIZED FUNCTION

@brief
  Function for Service Tasks to register their open/close functions.  The
  functions are what RDM calls to instruct a Service to open/close device.

  This function supports multiple ports functionality, ie a service can
  open more than one device at a time.

  It is newer than rdm_register_open_func and rdm_register_close_func,
  which register the opening/closing function when RDM only supports one
  port for one service. rdm_register_open_func and rdm_register_close_func
  are maintained for back compatibility.

@dependencies
  None

@return
  boolean
    TRUE : the registration succeeds
    FALSE: the registration fails. The following reasons could cause failure.
           1) open or close function is NULL
           2) service is out of range

@sideeffects
  None
===========================================================================*/
boolean rdm_register_func_multi_dev
(
  /* the service who registers the open and close function      */
  rdm_service_enum_type                 service,

  /* function for opening device                                */
  rdm_srv_func_ptr_multi_dev_type       open_func,

  /* function for closing device                                */
  rdm_srv_func_ptr_multi_dev_type       close_func,

  /* user data, the pointer will be passed back to the service
     It can be set to NULL.                                     */
  void *                                data
)
{
  DEBUG(MSG_MED("srv: %d, open: %p, close: %p", service, open_func,
                close_func );
        MSG_MED("data: %p", data,0,0);
        );

  /* check parameters                                           */
  if(service >= RDM_SRVC_MAX || service < RDM_FIRST_SRVC)
  {
    ERR("wrong service: %d", service,0,0);
    return FALSE;
  }

  if(open_func == NULL || close_func == NULL)
  {
    ERR("null open func and close function",0,0,0);
    return FALSE;
  }

  rdm_service_info[service].service_params.device_support      = RDM_MULTIPLE_DEV_SUPPORT;
  rdm_service_info[service].service_params.open_multi_func     = open_func;
  rdm_service_info[service].service_params.close_multi_func    = close_func;
  rdm_service_info[service].service_params.data                = data;
  rdm_service_info[service].service                            = service;

  return TRUE;

} /* rdm_register_func_multi_dev */

/**===========================================================================

FUNCTION  RDM_ISSUE_REMOTE_OPENS                        EXTERNALIZED FUNCTION

@brief
  RDM issues opens to SRVCs at the power up time based on last mapping
  before power down.

  Services using the RDM should not just open a port when the MSM boots.
  RDM will call the registered open routines for the SRVC's if they should
  open a port at boot.

  If EFS is being used, whatever the previous mapping was will be reinstated.
  If there was no previous mapping, DS will open UART1, and DIAG will open
  UART2 if it is defined.

  NOTE: when tmc calls rdm_issue_opens() from remote processor, rdm server
        will execute rdm_issue_opens_remote() to only issue opens for
        services on remote processor.

@dependencies
  None

@return
  None

@sideeffects
  If EFS is defined, the last valid port mapping prior to reset is
  re-instated
===========================================================================*/
void rdm_issue_remote_opens(void)
{
    boolean is_remote = TRUE;
    if(!rdm_get_prev_done && !rdm_non_persistent)
    {
        /* Can not issue remote opens without restoring port mappings
           Set the flag, and remote opens will be issued when local
           rdm_issue_open is called.
           Local rdm_issue_open is called after rdm_get_prev_ports()
        */
        rdm_remote_issue_pending = TRUE;
        return;
    }
    /* Issue remote opens as previous port mappings are restored */
    rdm_issue_opens_common(is_remote);

} /* rdm_issue_remote_opens() */

/**===========================================================================

FUNCTION RDM_REGISTER_REMOTE_OPEN_FUNC                  EXTERNALIZED FUNCTION

@brief
  Function for Service Tasks to register their open function. This open
  function is what RDM calls to instruct a Service to open a specified port.

  The function is used in conjunction with rdm_register_close_func.

  The service can only hold one device at a time if it registers
  its open function by this function. If the service holds dev1 and requests
  RDM to map dev2 to it, RDM will take dev1 from the service and then assign
  dev2 to the service.

  Please use rdm_register_func_multi_dev if you want multi-dev support.

  NOTE: when service on remote processor calls rdm_register_open_func(),
        RDM server executes rdm_register_remote_open_func() on local proc.

@dependencies
  None

@return
  None

@sideeffects
  None

===========================================================================*/
void rdm_register_remote_open_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_open_func_ptr_type open_func     /* the routine to be called */
)
{
    rdm_register_open_func(service, open_func);
    rdm_service_info[service].is_remote = TRUE;  /* mark service as remote */
}

/**===========================================================================

FUNCTION RDM_REGISTER_REMOTE_CLOSE_FUNC                 EXTERNALIZED FUNCTION

@brief
  Function for Service Tasks to register their close function. This close
  function is what RDM calls to instruct a Service to close it's open port.

  The function is used in conjunction with rdm_register_open_func.

  By using this function, a service can have only one port open, hence
  no sio_port_id_type argument in the close function.

  NOTE: when service on remote processor calls rdm_register_close_func(),
        RDM server executes rdm_register_remote_close_func() on local proc.

@dependencies
  None

@return
  None

@sideeffects
  None

===========================================================================*/
void rdm_register_remote_close_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_close_func_ptr_type  close_func  /* the routine to be called */
)
{
    rdm_register_close_func(service, close_func);
    rdm_service_info[service].is_remote = TRUE;  /* mark service as remote */
}

/**===========================================================================

FUNCTION RDM_REGISTER_REMOTE_FUNC_MULTI_DEV             EXTERNALIZED FUNCTION

@brief
  Function for Service Tasks to register their open/close functions.  The
  functions are what RDM calls to instruct a Service to open/close device.

  This function supports multiple ports functionality, ie a service can
  open more than one device at a time.

  It is newer than rdm_register_open_func and rdm_register_close_func,
  which register the opening/closing function when RDM only supports one
  port for one service. rdm_register_open_func and rdm_register_close_func
  are maintained for back compatibility.

 NOTE: when service on remote processor calls rdm_register_func_multi_dev(),
        RDM server executes rdm_register_remote_func_multi_dev() on local proc.


@dependencies
  None

@return
  boolean
    TRUE : the registration succeeds
    FALSE: the registration fails. The following reasons could cause failure.
           1) open or close function is NULL
           2) service is out of range

@sideeffects
  None
===========================================================================*/
boolean rdm_register_remote_func_multi_dev
(
  /* the service who registers the open and close function      */
  rdm_service_enum_type                 service,

  /* function for opening device                                */
  rdm_srv_func_ptr_multi_dev_type       open_func,

  /* function for closing device                                */
  rdm_srv_func_ptr_multi_dev_type       close_func,

  /* user data, the pointer will be passed back to the service
     It can be set to NULL.                                     */
  void *                                data
)
{
    boolean ret_val;
    ret_val = rdm_register_func_multi_dev(service,open_func,close_func,data);
    rdm_service_info[service].is_remote = TRUE;  /* mark service as remote */
    return ret_val;
}


/**===========================================================================

FUNCTION RDM_ENUM_TO_STRING

@brief
  rdm_enum_to_string() function converts enum symbols to string and initializes
  services_names and devices_names array for runtime enum to string translation.

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/

static void rdm_enum_to_string(void)
{
  int nbytes;
    /* RDM_DEVICES: Add all the rdm_device_enum_type */
  RDM_DEV_NAME(RDM_NULL_DEV);
  RDM_DEV_NAME(RDM_UART1_DEV);
  RDM_DEV_NAME(RDM_UART2_DEV);
  RDM_DEV_NAME(RDM_UART3_DEV);
  RDM_DEV_NAME(RDM_USB_MDM_DEV);
  RDM_DEV_NAME(RDM_USB_MMC_DEV);
  RDM_DEV_NAME(RDM_USB_SER1_DEV);
  RDM_DEV_NAME(RDM_BT_NA_DEV);
  RDM_DEV_NAME(RDM_SMEM_DS_DEV);
  RDM_DEV_NAME(RDM_LSM_CSD_DEV);
  RDM_DEV_NAME(RDM_USB_SER2_DEV);
  RDM_DEV_NAME(RDM_BT_SPP_DEV);
  RDM_DEV_NAME(RDM_USB_NET_WWAN_DEV);
  RDM_DEV_NAME(RDM_SMD_DS_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA1_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA2_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA3_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA4_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA11_DEV);
  RDM_DEV_NAME(RDM_APS_SERIAL_VSP_DEV);
  RDM_DEV_NAME(RDM_SMD_NMEA_DEV);
  RDM_DEV_NAME(RDM_USB_BREW_EXTENDED_DATA_DEV);
  RDM_DEV_NAME(RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV);
  RDM_DEV_NAME(RDM_MUX_PHYSICAL_DEV);
  RDM_DEV_NAME(RDM_MUX_DEV_1);
  RDM_DEV_NAME(RDM_MUX_DEV_2);
  RDM_DEV_NAME(RDM_MUX_DEV_3);
  RDM_DEV_NAME(RDM_MUX_DEV_4);
  RDM_DEV_NAME(RDM_MUX_DEV_5);
  RDM_DEV_NAME(RDM_MUX_DEV_6);
  RDM_DEV_NAME(RDM_MUX_DEV_7);
  RDM_DEV_NAME(RDM_MUX_DEV_8);
  RDM_DEV_NAME(RDM_MUX_DEV_9);
  RDM_DEV_NAME(RDM_MUX_DEV_10);
  RDM_DEV_NAME(RDM_MUX_DEV_11);
  RDM_DEV_NAME(RDM_MUX_DEV_12);
  RDM_DEV_NAME(RDM_MUX_DEV_13);
  RDM_DEV_NAME(RDM_MUX_DEV_14);
  RDM_DEV_NAME(RDM_MUX_DEV_15);
  RDM_DEV_NAME(RDM_MUX_DEV_16);
  RDM_DEV_NAME(RDM_USB_NET_WWAN2_DEV);
  RDM_DEV_NAME(RDM_USB_NET_WWAN3_DEV);
  RDM_DEV_NAME(RDM_USB_NET_WWAN4_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA5_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA6_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA7_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA8_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA9_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA12_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA13_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA14_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA23_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA24_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA25_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA26_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA27_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA28_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA29_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA30_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA31_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA32_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA33_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA34_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA35_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA36_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA37_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA38_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA39_DEV);
  RDM_DEV_NAME(RDM_SMD_DATA40_DEV);
  RDM_DEV_NAME(RDM_A2_MDM_DEV);
  RDM_DEV_NAME(RDM_A2_RMNET_1_DEV);
  RDM_DEV_NAME(RDM_A2_RMNET_2_DEV);
  RDM_DEV_NAME(RDM_A2_RMNET_3_DEV);
  RDM_DEV_NAME(RDM_ONDEVICE_DIAG_DEV);
  RDM_DEV_NAME(RDM_ONDEVICE_DIAG_USB_AUTO_DEV);
  RDM_DEV_NAME(RDM_USB_SER3_DEV);
  RDM_DEV_NAME(RDM_USB_SMCT_DEV);
  RDM_DEV_NAME(RDM_USB_SER4_DEV);
  RDM_DEV_NAME(RDM_USB_QDSS_DEV);
  RDM_DEV_NAME(RDM_USB_CSVT_DEV);
  RDM_DEV_NAME(RDM_SMUX_0_DEV);
  RDM_DEV_NAME(RDM_SMUX_1_DEV);
  RDM_DEV_NAME(RDM_SMUX_2_DEV);
  RDM_DEV_NAME(RDM_SMUX_3_DEV);
  RDM_DEV_NAME(RDM_SMUX_4_DEV);
  RDM_DEV_NAME(RDM_SMUX_5_DEV);
  RDM_DEV_NAME(RDM_SMUX_6_DEV);
  RDM_DEV_NAME(RDM_SMUX_7_DEV);
  RDM_DEV_NAME(RDM_SMUX_8_DEV);
  RDM_DEV_NAME(RDM_SMUX_9_DEV);
  RDM_DEV_NAME(RDM_SMUX_10_DEV);
  RDM_DEV_NAME(RDM_SMUX_11_DEV);
  RDM_DEV_NAME(RDM_SMUX_12_DEV);
  RDM_DEV_NAME(RDM_SMUX_13_DEV);
  RDM_DEV_NAME(RDM_SMUX_14_DEV);
  RDM_DEV_NAME(RDM_SMUX_15_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_0_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_1_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_2_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_3_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_4_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_5_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_6_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_7_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_8_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_9_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_10_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_RMNET_TETH_0_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_DUN_0_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CSVT_DEV);

  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_0_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_1_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_2_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_3_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_4_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_5_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_6_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_CH_7_DEV);

  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_0_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_1_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_2_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_3_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_4_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_5_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_6_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_7_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_CH_8_DEV);

  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_0_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_1_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_2_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_3_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_4_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_5_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_6_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_7_DEV);
  RDM_DEV_NAME(RDM_SDIO_MUX_A2_REV_IP_8_DEV);

  RDM_DEV_NAME(RDM_USB_NET_WWAN5_DEV);
  RDM_DEV_NAME(RDM_QBI_DIAG_DEV);

  RDM_DEV_NAME(RDM_A2_DS_PHYS_1_DEV);
  RDM_DEV_NAME(RDM_A2_DS_PHYS_2_DEV);
  RDM_DEV_NAME(RDM_A2_DS_PHYS_3_DEV);
  RDM_DEV_NAME(RDM_A2_DS_PHYS_4_DEV);
  
  RDM_DEV_NAME(RDM_A2_DS_CH_1_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_2_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_3_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_4_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_5_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_6_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_7_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_8_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_9_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_10_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_11_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_12_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_13_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_14_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_15_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_16_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_17_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_18_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_19_DEV);
  RDM_DEV_NAME(RDM_A2_DS_CH_20_DEV);

 #ifdef FEATURE_SIO_IPA_SUPPORT
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_1_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_2_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_3_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_4_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_5_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_6_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_7_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_PHYS_8_DEV);

  RDM_DEV_NAME(RDM_IPA_DS_CH_1_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_2_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_3_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_4_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_5_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_6_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_7_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_8_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_9_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_10_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_11_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_12_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_13_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_14_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_15_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_16_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_17_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_18_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_19_DEV);
  RDM_DEV_NAME(RDM_IPA_DS_CH_20_DEV);
  
  #endif //#ifdef FEATURE_SIO_IPA_SUPPORT
  /* RDM_DEVICES: Add all the rdm_device_enum_type */
  RDM_SVC_NAME(RDM_DIAG_SRVC);
  RDM_SVC_NAME(RDM_DATA_SRVC);
  RDM_SVC_NAME(RDM_BT_HCI_SRVC);
  RDM_SVC_NAME(RDM_BT_SOC_SRVC);
  RDM_SVC_NAME(RDM_MMC_SRVC);
  RDM_SVC_NAME(RDM_NMEA_SRVC);
  RDM_SVC_NAME(RDM_RPC_SRVC);
  RDM_SVC_NAME(RDM_DATA_WWAN_SRVC);
  RDM_SVC_NAME(RDM_DATA_MUX_SRVC);
  RDM_SVC_NAME(RDM_BREW_SRVC);
  RDM_SVC_NAME(RDM_BREW_EXTENDED_SRVC);
  RDM_SVC_NAME(RDM_STRIDE_SRVC);

  nbytes = snprintf(rdm_fversion, RDM_ROW_WIDTH, "#Format Version:%d.%d.%d\r\n", RDM_DEV_MAX, RDM_FILE_MAX_COL, RDM_FILE_FORMAT);
  ASSERT(nbytes < RDM_ROW_WIDTH);
}

/**===========================================================================

FUNCTION RDM_GET_DEVICE_ID

@brief
  rdm_get_device_id (i.e. atoi function) convert device ID string to
  rdm_device_enum_type.

@dependencies
  None

@return
  rdm_device_enum_type associated with dev ID.

@sideeffects
  None
===========================================================================*/

static rdm_device_enum_type rdm_get_device_id(char *ID)
{
    int i;
    /* Convert DEVICE ID (string) to int */
    for(i=0;*ID;ID++)
    {
        if(*ID >= '0' && *ID <= '9')
             i = (i * 10) + (*ID -'0');
    }
    return (rdm_device_enum_type)i;
}


/**===========================================================================

FUNCTION RDM_GET_SERVICE_ID

@brief
  rdm_get_service_id() takes string name (service name) as input and returns
  rdm_service_enum_type associated with the service name.

@dependencies
  None

@return
  None

@sideeffects
  None
===========================================================================*/

static rdm_service_enum_type rdm_get_service_id(char *Service)
{
    rdm_service_enum_type srvc_id;

    if (*Service && (strlen(Service) < RDM_COL2_WIDTH))
    {
       for(srvc_id = RDM_FIRST_SRVC; srvc_id < RDM_SRVC_MAX; srvc_id = (rdm_service_enum_type) (srvc_id + 1))
       {
           if(strcmp(rdm_service_names[srvc_id], Service) == 0)
           {
               return srvc_id;
           }
       }
    }
    return RDM_NULL_SRVC;
}

/**===========================================================================

FUNCTION  RDM_RESTORE_PORT_MAPPING

@brief
  Make QMI calls to remote RDM entity to read in the port mappings last knowm
  by RDM from EFS.

  It is here that the compile time defaults for port mapping may be
  overwritten by what is read from EFS at power up.

@dependencies
  None

@return
  TRUE: If port mappings is restored successfully
  FALSE: If error occurs

@sideeffects
  Port mappings before the last reset are restored at boot.

===========================================================================*/
static boolean rdm_restore_port_mapping(void)
{
  /* get device-service map from efs and save it in this map  */
  rdm_map_type user_dev_map;
  rdm_device_enum_type device;
  char *dst, *config_item_buffer, *buffer_end, *line;
  uint32 col_num=0;
  char rdm_config[RDM_FILE_MAX_COL][RDM_COL1_WIDTH]; /* MAx col width */
  int32 nbytes;
  boolean retval = 1;

  if(DAL_SUCCESS == DALSYS_Malloc(RDM_CONFIG_ITEM_FILE_SIZE, (void **)(&config_item_buffer)))
  {
    memset(config_item_buffer, 0, RDM_CONFIG_ITEM_FILE_SIZE);

    // From either remote apps proc or modem we interface to this file using efs_put/efs_get as if it were an item
    // file, but under hood EFS stores it as a regular file because of its large size.
    if( (nbytes = efs_get(rdm_efs_port_map_file,
                          (void *)config_item_buffer, (fs_size_t)RDM_CONFIG_ITEM_FILE_SIZE)) > 0 )
    {
       if(strncmp(config_item_buffer,rdm_fversion,strlen(rdm_fversion)) == 0)
       {
         DEBUG( MSG_MED("rdm_restore_port_mapping: version verified",0,0,0); );

         // get string at a time from buffer, parsing string into rdm_map, until null string
         buffer_end = config_item_buffer + nbytes;
         line = config_item_buffer;
         while(line < buffer_end)
         {
           if(*line != '#')
           {
             for(col_num = 0; col_num < RDM_FILE_MAX_COL; col_num++)
             {
               dst=&rdm_config[col_num][0];

               for(; (line < buffer_end) &&                 /* For leading spaces, tabs or NULL padding after last line */
                     (*line == ' ' || *line == 0x09 || *line == 0x00);   line++);
               for(; (line < buffer_end) && (*line != ',') && (*line != ' ') &&                      /* parse .csv file */
                     (*line != 0x09) && (*line != 0x0A);  *dst++ = *line++);
               for(; (line < buffer_end) && ((*line == ',') || (*line == 0x0A)); line++); /* For trailing comma, spaces */

               *dst = '\0';

//               if(*line)
//                 line++;
             }

             device = rdm_get_device_id(rdm_config[RDM_COL0_ID]);
             user_dev_map.current_srvc_id = rdm_get_service_id(rdm_config[RDM_COL2_ID]);
             user_dev_map.default_srvc_id = rdm_get_service_id(rdm_config[RDM_COL3_ID]);
             user_dev_map.is_persistent = 1;

             if(device < RDM_DEV_MAX && device > RDM_NULL_DEV)
             {
               rdm_map[device].default_srvc_id = user_dev_map.default_srvc_id;
               rdm_map[device].is_persistent = user_dev_map.is_persistent;

               if(rdm_map[device].is_persistent)
                 rdm_map[device].current_srvc_id = user_dev_map.current_srvc_id;
               else
                 rdm_map[device].current_srvc_id = user_dev_map.default_srvc_id;
             }
           }
           else
           {
              // skip a comment line
              for(; (line < buffer_end) && (*line != 0x0A); line++);
              if(*line == 0x0A)
                line++;
           }
         }
       }
       else
       {
         ERR("rdm_restore_port_mapping: Incorrect file version, generate new one",0,0,0);
         retval = 0;
       }
    }
    else
    {
      // efs_get returned negative number or 0, assume this always means that file doesn't exist
      ERR("efs_get: errcode %d", efs_errno, 0, 0);
      retval = 0;
    }

    DALSYS_Free((void *)config_item_buffer);
  }
  else
  {
     ERR("rdm_restore_port_mapping: failed to allocate config item buffer", 0, 0, 0);
     // If buffer couldn't be allocated allow true to be returned so a new file isn't created
  }

  return retval;
}

