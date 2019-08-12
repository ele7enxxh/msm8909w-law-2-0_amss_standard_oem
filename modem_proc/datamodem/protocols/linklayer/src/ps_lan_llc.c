/*===========================================================================
                                 PS_LAN_LLC.C


DESCRIPTION

    This file contains implementation of the PS LAN LLC public interface.

EXTERNALIZED FUNCTIONS
    lan_llc_init()
    lan_llc_start()
    lan_llc_stop()
    lan_llc_set_input_handler()
    lan_llc_is_started()
    lan_llc_deinit()

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

           $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $


when       who     what, where, why
-------   ---     ----------------------------------------------------------
08/17/14   cx     Added API to de-init llc module
06/27/13   cx     Replace memcpy and memmove with memscpy and memsmove
12/06/12   at     Fix for the USB TE-A5 data path issue in ETHERNET mode.
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
10/07/10   am      Rm split changes for LLC in SoftAp mode.
09/24/09   ss      Fixed KW warnings.
08/19/09   pg      Added support for LAN_ETHERTYPE_WAPI.
08/11/09   am      Modified locks usage. Allowing tandem start->stop->start 
                   calls in caller context.
11/16/06   ks      High Lint error fixes.
03/07/06   jd      Added reconfigure api to change tx function after started
02/06/06   ks      Replaced tasklocks with critical sections.
07/08/05   hba     Fixed error in type passed in sizeof when procesing START
                   command.
06/08/05   hba     Added support for 802.1X packets types.
10/07/04   hba     Creation.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "ps_lan_llc.h"
#include "ps_lan_llci.h"
#include "ps_arp.h"
#include "ps_mem.h"
#include "ps_crit_sect.h"
#include "ps_utils.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_rm_svc.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
const byte LAN_IEEE_MAC_BROADCAST_ADDR [LAN_IEEE_MAC_ADDR_LEN] =
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*---------------------------------------------------------------------------
  Tuning the number of ps lan llc buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_LAN_LLC_BUF_SIZE  ((sizeof(lan_lle_config_type) + 3) & ~3)

#define PS_LAN_LLC_BUF_NUM       (LAN_LLE_MAX + 1)
#define PS_LAN_LLC_BUF_HIGH_WM   LAN_LLE_MAX
#define PS_LAN_LLC_BUF_LOW_WM    1

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_lan_llc along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_lan_llc_buf_mem[PS_MEM_GET_TOT_SIZE(PS_LAN_LLC_BUF_NUM,
                                                  PS_LAN_LLC_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debuggine.  The first one points
  to the ps_mem header and the latter points to ps_lan_llc_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ps_lan_llc_buf_hdr[PS_LAN_LLC_BUF_NUM];
static lan_lle_config_type * ps_lan_llc_buf_ptr[PS_LAN_LLC_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
 Check Configuration information
---------------------------------------------------------------------------*/
STATIC int lan_llc_check_start_info
(
  lan_llc_start_info_type *config_ptr
);

/*---------------------------------------------------------------------------
 LAN_LLC_START_CMD handler
---------------------------------------------------------------------------*/
STATIC void lan_llc_process_start
(
  ps_cmd_enum_type cmd,
  void            *user_data
);

/*---------------------------------------------------------------------------
 LAN_LLC_RECONFIG_CMD handler
---------------------------------------------------------------------------*/
STATIC void lan_llc_process_reconfig
(
  ps_cmd_enum_type cmd,
  void             *user_data
);


/*---------------------------------------------------------------------------
 LAN_LLC_STOP_CMD handler
---------------------------------------------------------------------------*/
STATIC void lan_llc_process_stop
(
  ps_cmd_enum_type cmd,
  void            *user_data
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION LAN_LLC_INIT()

DESCRIPTION
  This function initializes the LLC module main data structures. It should
  be called only one, at power up, before any LLC service is invoked. It also
  called indirectly the initialization function of the ARP module.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void lan_llc_init
(
  void
)
{
  static boolean    init_flag = FALSE;
  uint8             i;
  lan_lle_cb_type * ptr_lle_cb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( TRUE == init_flag)
  {
    DATA_ERR_FATAL("PS LAN LLC Module already initialized");
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize Pool 
  -------------------------------------------------------------------------*/
  if (ps_mem_pool_init(PS_MEM_LAN_LLC_CFG_BUFFER_TYPE,
                       ps_lan_llc_buf_mem,
                       PS_LAN_LLC_BUF_SIZE, 
                       PS_LAN_LLC_BUF_NUM,
                       PS_LAN_LLC_BUF_HIGH_WM,
                       PS_LAN_LLC_BUF_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG                   
                       (int *) ps_lan_llc_buf_hdr,
                       (int *) ps_lan_llc_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    DATA_ERR_FATAL("Can't init the module");
    return;
  }

  /*-------------------------------------------------------------------------
   Initialization of all LLE control blocks
  -------------------------------------------------------------------------*/
  memset( lle_array, 0, LAN_LLE_MAX * sizeof(lan_lle_cb_type));

  for ( i = 0; i < LAN_LLE_MAX; i++ )
  {
    PS_INIT_CRIT_SECTION(&(lle_array[i].lan_lle_crit_section));
    ptr_lle_cb = & lle_array[i];
    /*-------------------------------------------------------------------------
      No need to Enter critical section here as init() is only called once at
      powerup.
    -------------------------------------------------------------------------*/
    ptr_lle_cb->start_lock = FALSE;
    ptr_lle_cb->started    = FALSE;
    ptr_lle_cb->softap_mode = FALSE;
  }

  /*-------------------------------------------------------------------------
   Register the handlers for the LLC commands that must be handled in the PS
   task context.
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler(PS_LAN_LLC_START_CMD, lan_llc_process_start);
  (void)ps_set_cmd_handler(PS_LAN_LLC_RECONFIG_CMD, lan_llc_process_reconfig);
  (void)ps_set_cmd_handler(PS_LAN_LLC_STOP_CMD , lan_llc_process_stop );

  /*-------------------------------------------------------------------------
  Initialization of the ARP module
  -------------------------------------------------------------------------*/
  arp_init();

  init_flag = TRUE;

} /* lan_llc_init() */

/*===========================================================================
FUNCTION LAN_LLC_START()

DESCRIPTION
  This function posts a START command to the PS task for a particular LLE
  instance. It is called by the specific Mode Controller.

PARAMETERS
  lle_instance: the LLE instance that should be started
  config_ptr: pointer to configuration information for LLE to start. This
              memory belongs to the caller and will not be freed by PS LAN
              LLC

RETURN VALUE
  -1 in case of error
   0 in the command has been posted successfully.

DEPENDENCIES
  lan_llc_init should have been called.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llc_start
(
  lan_lle_enum_type        lle_instance,
  lan_llc_start_info_type* config_ptr
)
{
  lan_lle_cb_type*      lle_cb_ptr;
  lan_lle_config_type*  cmd_lle_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( ( lle_instance <= LAN_LLE_MIN ) || ( lle_instance >= LAN_LLE_MAX ) || 
       ( NULL == config_ptr ) )
  {
    LOG_MSG_ERROR_1("BAD LLE instance (%d) passed in lan_llc_start",
                    lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    LLE instance seems good and config_ptr does contain something!
    Let's do some sanity check on config_ptr!
  -------------------------------------------------------------------------*/
  if ( lan_llc_check_start_info(config_ptr) < 0 )
  {
    DATA_ERR_FATAL("Invalid Configuration for LLE start up");
    return -1;
  }

  lle_cb_ptr = &lle_array[lle_instance];

  /*-------------------------------------------------------------------------
    Enter here a critical section to avoid multiple tasks operating on the
    same data...
    In the future use rex_enter_crit_sect: it is lighter weigh than TASKLOCK
  -------------------------------------------------------------------------*/

  PS_ENTER_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));

  /*-------------------------------------------------------------------------
    Check if this is not a duplicate start
  -------------------------------------------------------------------------*/
  if ( TRUE == lle_cb_ptr->start_lock)
  {
    LOG_MSG_ERROR_0("LLE already started");
    PS_BRANCH_LEAVE_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));
    return -1;
  }

  lle_cb_ptr->start_lock = TRUE;

  memscpy(&lle_cb_ptr->start_info,
          sizeof(lan_llc_start_info_type),
          config_ptr,
          sizeof(lan_llc_start_info_type) );

  PS_LEAVE_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));

  /*-------------------------------------------------------------------------
    Get a buffer from PS MEM and populate it
  -------------------------------------------------------------------------*/
  cmd_lle_cb_ptr = (lan_lle_config_type*)
                      ps_mem_get_buf(PS_MEM_LAN_LLC_CFG_BUFFER_TYPE);

  if ( NULL == cmd_lle_cb_ptr )
  {
    LOG_MSG_ERROR_0("Could not get buffer to store config info");
    return -1;
  }

  /*-------------------------------------------------------------------------
    All parameters are perfect... We can proceed on...
  -------------------------------------------------------------------------*/

  cmd_lle_cb_ptr->lle_instance = lle_instance;

  memscpy(&cmd_lle_cb_ptr->start_info,
          sizeof(lan_llc_start_info_type), 
          config_ptr,
          sizeof(lan_llc_start_info_type) );

  /*-------------------------------------------------------------------------
    Prepare info to post command to PS task
  -------------------------------------------------------------------------*/

  LAN_LLC_SEND_CMD( PS_LAN_LLC_START_CMD, (void*)cmd_lle_cb_ptr);

  return 0;

} /* lan_llc_start() */


/*===========================================================================
FUNCTION LAN_LLC_RECONFIG()

DESCRIPTION
  Change the configuration of a lan_llc instance after it was started.

PARAMETERS
  lle_instance:  Which LLC instance to reconfigure
  config_ptr:    The new configuration for this LLE instance

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lan_llc_reconfig
(
  lan_lle_enum_type        lle_instance,
  lan_llc_start_info_type* config_ptr
)
{
  lan_lle_config_type*  cmd_lle_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( lle_instance >= LAN_LLE_MAX || NULL == config_ptr )
  {
    LOG_MSG_ERROR_1("BAD LLE instance (%d) passed in lan_llc_start",
                    lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    LLE instance seems good and config_ptr does contain something!
    Let's do some sanity check on config_ptr!
  -------------------------------------------------------------------------*/
  if ( config_ptr->tx_f_ptr == NULL )
  {
    DATA_ERR_FATAL("Invalid Configuration for LLE reconfig");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Enter here a critical section to avoid multiple tasks operating on the
    same data...
    In the future use rex_enter_crit_sect: it is lighter weigh than TASKLOCK

    forget about this for now.  Only changing tx function - this will happen
    during command processing, so won't interfere with lan_llc operation.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Get a buffer from PS MEM and populate it
  -------------------------------------------------------------------------*/
  cmd_lle_cb_ptr = (lan_lle_config_type*)
                      ps_mem_get_buf(PS_MEM_LAN_LLC_CFG_BUFFER_TYPE);



  if ( NULL == cmd_lle_cb_ptr )
  {
    LOG_MSG_ERROR_0("Could not get buffer to store config info");
    return -1;
  }

  /*-------------------------------------------------------------------------
    All parameters are perfect... We can proceed on...
  -------------------------------------------------------------------------*/

  cmd_lle_cb_ptr->lle_instance = lle_instance;

  memscpy(&cmd_lle_cb_ptr->start_info,
          sizeof(lan_llc_start_info_type),
          config_ptr,
          sizeof(lan_llc_start_info_type) );
 
  /*-------------------------------------------------------------------------
    Prepare info to post command to PS task
  -------------------------------------------------------------------------*/

  LAN_LLC_SEND_CMD( PS_LAN_LLC_RECONFIG_CMD, (void*)cmd_lle_cb_ptr);

  return 0;

} /* lan_llc_reconfig() */


/*===========================================================================
FUNCTION LAN_LLC_STOP()

DESCRIPTION
  This function posts a STOP command to the PS task for a particular LLE. It
  is called by the specific Mode controller (not running in PS task context).

PARAMETERS
  lle_instance: the LLE instance to stop.

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  lan_llc_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llc_stop
(
  lan_lle_enum_type  lle_instance
)
{
  lan_lle_cb_type*  lle_cb_ptr;
  int stop_info;       /*To be passed as user data with PS task cmd buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Sanity Check
  -------------------------------------------------------------------------*/
  if ( ( lle_instance <= LAN_LLE_MIN ) || ( lle_instance >= LAN_LLE_MAX ) )
  {
    LOG_MSG_ERROR_1("Bad LLE instance(%d) passed in lan_llc_stop",
                    lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
   Was this LLE started?
  -------------------------------------------------------------------------*/
  lle_cb_ptr = &lle_array[lle_instance];

  PS_ENTER_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));

  if (TRUE != lle_cb_ptr->start_lock)
  {
    LOG_MSG_INFO2_1("LLE (%d) is not started\n", lle_instance);
    PS_BRANCH_LEAVE_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));
    return -1;
  }

  /*-------------------------------------------------------------------------
   Allow start calls in caller context now
  -------------------------------------------------------------------------*/
  lle_cb_ptr->start_lock = FALSE;

  PS_LEAVE_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));

  /*-------------------------------------------------------------------------
   Post an LLE stop command to PS task
  -------------------------------------------------------------------------*/
  stop_info = lle_instance;

  LAN_LLC_SEND_CMD(PS_LAN_LLC_STOP_CMD, (void*)stop_info);

  return 0;

} /* lan_llc_stop() */


/*===========================================================================
FUNCTION LAN_LLC_SET_INPUT_HANDLER()

DESCRIPTION
  This function registers with a particular LLE instance the handler for
  packets of a particular ethertype. Note that LLC support natively handler
  for IP and ARP packets and those handlers does not have to be registered
  explicitly.

PARAMETERS
  lle_instance: the LLE instance to stop.

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  lan_llc_init should have been called.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llc_set_input_handler
(
  lan_lle_enum_type        lle_instance,
  uint16                   ethertype,
  lan_llc_input_f_ptr_type lle_input_f_ptr
)
{
  lan_lle_cb_type*      lle_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( ( lle_instance <= LAN_LLE_MIN) || ( lle_instance >= LAN_LLE_MAX) )
  {
    LOG_MSG_ERROR_1("BAD LLE instance (%d) passed in lan_llc_set_input_handler",
                    lle_instance);
    ASSERT(0);
    return -1;
  }

  lle_cb_ptr = &lle_array[lle_instance];

  switch(ethertype)
  {
    case LAN_ETHERTYPE_802_1X:
      lle_cb_ptr->lan_1x_input_f_ptr = lle_input_f_ptr;
      break;

    case LAN_ETHERTYPE_802_1X_PREAUTH:
      lle_cb_ptr->lan_1x_preauth_input_f_ptr = lle_input_f_ptr;
      break;

    case LAN_ETHERTYPE_WAPI:
      lle_cb_ptr->lan_wapi_input_f_ptr = lle_input_f_ptr;
      break;

    default:
     DATA_ERR_FATAL("Registering unsupported ethertype by LLC");
     return -1;
  }

  return 0;

} /* ln_llc_set_input_handler() */


/*===========================================================================
FUNCTION LAN_LLC_IS_STARTED()

DESCRIPTION
  This function queries whether a paticular LLC instance has been started.

PARAMETERS
  lle_instance: the LLE instance

RETURN VALUE
  TRUE:  The LLC has been started
  FALSE: The instance is stopped

DEPENDENCIES
  lan_llc_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
boolean lan_llc_is_started
(
  lan_lle_enum_type        lle_instance
)
{
  lan_lle_cb_type*      lle_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( ( lle_instance <= LAN_LLE_MIN ) || ( lle_instance >= LAN_LLE_MAX ) )
  {
    LOG_MSG_ERROR_1("BAD LLE instance (%d) passed in lan_llc_set_input_handler",
                    lle_instance);
    ASSERT(0);
    return FALSE;
  }

  lle_cb_ptr = &lle_array[lle_instance];

  return lle_cb_ptr->started;

} /* lan_llc_is_started() */

/*===========================================================================
FUNCTION LAN_LLC_SOFTAP_START()

DESCRIPTION
  This function posts a START command to the PS_RM task for a particular LLE
  instance. It is called by the WLAN softap specific Mode Controller only.

PARAMETERS
  lle_instance: the LLE instance that should be started
  config_ptr: pointer to configuration information for LLE to start. This
              memory belongs to the caller and will not be freed by PS LAN
              LLC

RETURN VALUE
  -1 in case of error
   0 in the command has been posted successfully.

DEPENDENCIES
  lan_llc_init should have been called.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llc_softap_start
(
  lan_lle_enum_type        lle_instance,
  lan_llc_start_info_type* config_ptr
)
{
  lan_lle_cb_type*      lle_cb_ptr;
  lan_lle_config_type*  cmd_lle_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( ( lle_instance <= LAN_LLE_MIN ) || ( lle_instance >= LAN_LLE_MAX ) || 
       ( NULL == config_ptr ) )
  {
    LOG_MSG_ERROR_1("BAD LLE instance (%d) passed in lan_llc_start",
                    lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    LLE instance seems good and config_ptr does contain something!
    Let's do some sanity check on config_ptr!
  -------------------------------------------------------------------------*/
  if ( lan_llc_check_start_info(config_ptr) < 0 )
  {
    DATA_ERR_FATAL("Invalid Configuration for LLE start up");
    return -1;
  }

  lle_cb_ptr = &lle_array[lle_instance];
  config_ptr->lle_rx_sig = PS_RM_LLC_RX_DATA_Q_SIGNAL;

  /*-------------------------------------------------------------------------
    Enter here a critical section to avoid multiple tasks operating on the
    same data...
    In the future use rex_enter_crit_sect: it is lighter weigh than TASKLOCK
  -------------------------------------------------------------------------*/

  PS_ENTER_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));

  /*-------------------------------------------------------------------------
    Check if this is not a duplicate start
  -------------------------------------------------------------------------*/
  if ( TRUE == lle_cb_ptr->start_lock)
  {
    LOG_MSG_ERROR_0("LLE already started");
    PS_BRANCH_LEAVE_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));
    return -1;
  }

  lle_cb_ptr->start_lock = TRUE;
  //Set softap mode
  lle_cb_ptr->softap_mode = TRUE;

  PS_LEAVE_CRIT_SECTION(&(lle_cb_ptr->lan_lle_crit_section));

  /*-------------------------------------------------------------------------
    Get a buffer from PS MEM and populate it
  -------------------------------------------------------------------------*/
  cmd_lle_cb_ptr = (lan_lle_config_type*)
                      ps_mem_get_buf(PS_MEM_LAN_LLC_CFG_BUFFER_TYPE);

  if ( NULL == cmd_lle_cb_ptr )
  {
    LOG_MSG_ERROR_0("Could not get buffer to store config info");
    return -1;
  }

  /*-------------------------------------------------------------------------
    All parameters are perfect... We can proceed on...
  -------------------------------------------------------------------------*/

  cmd_lle_cb_ptr->lle_instance = lle_instance;

  memscpy(&cmd_lle_cb_ptr->start_info,
          sizeof(lan_llc_start_info_type),
          config_ptr,
          sizeof(lan_llc_start_info_type) );
 
  memscpy(&lle_array[lle_instance].start_info,
          sizeof(lan_llc_start_info_type), 
          config_ptr,
          sizeof(lan_llc_start_info_type) );

  /*-------------------------------------------------------------------------
    Prepare info to post command to PS task
  -------------------------------------------------------------------------*/

  LAN_LLC_SEND_CMD( PS_LAN_LLC_START_CMD, (void*)cmd_lle_cb_ptr);

  return 0;

} /* lan_llc_softap_start() */

/*===========================================================================
FUNCTION LAN_LLC_SET_SIGNAL()

DESCRIPTION
  This function is called to set the appropiate protocol task Rx signal for
  the corresponding LLE instance.

PARAMETERS
  lle_instance: the LLE instance to stop.

RETURN VALUE
  -1 in case of error
   0 if successful

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llc_set_signal
(
  lan_lle_enum_type instance
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_RM_SET_SIGNAL(lle_array[instance].start_info.lle_rx_sig);

  return 0;
}

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        LOCAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION LAN_LLC_CHECK_START_INFO()

DESCRIPTION
  This function chechs the validity of the configuration element passed when
  starting up an LLE

PARAMETERS
  config_ptr: Pointer to the configuration pointer.

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  lan_llc_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
STATIC int lan_llc_check_start_info
(
  lan_llc_start_info_type * config_ptr
)
{
  uint8 lle_mode = config_ptr->lle_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check validity of PS_IFACE
  -------------------------------------------------------------------------*/
  if ( !PS_IFACE_IS_VALID(config_ptr->iface_ptr) )
  {
    DATA_ERR_FATAL("Invalid PS_IFACE pointer in LLE start info");
    return -1;
  }

  if (lle_mode >= LAN_LLC_MODE_MAX)
  {
    LOG_MSG_INFO2_1("Invalid LLC mode (%d) supply in LLE start info",
                    lle_mode);
    ASSERT(0);
    return -1;
  }

#if 0
  if ( NULL == config_ptr->rx_f_ptr || NULL == config_ptr->tx_f_ptr )
  {
    DATA_ERR_FATAL("Invalid Rx Func Ptr or Tx Func Ptr in LLE Start info");
    return -1;
  }
#endif

  if (NULL == config_ptr->tx_f_ptr )
  {
    DATA_ERR_FATAL("Invalid Tx Func Ptr in LLE Start info");
    return -1;
  }

  return 0;
} /* lan_llc_check_start_info() */


/*===========================================================================
FUNCTION LAN_LLC_PROCESS_START()

DESCRIPTION
  This function handles the lan_llc_start command.

PARAMETERS
  cmd: Command identifier
  user_data: pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void lan_llc_process_start
(
  ps_cmd_enum_type cmd,
  void             *user_data
)
{
  lan_lle_config_type*  lle_cfg_ptr;
  lan_lle_enum_type start_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd != PS_LAN_LLC_START_CMD)
  {
    LOG_MSG_ERROR_1("LAN_LLC_START_CMD handler triggered by wrong cmd (%d)",
                    cmd);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
  Recover the LLE instance!
  -------------------------------------------------------------------------*/
  lle_cfg_ptr = (lan_lle_config_type*) user_data;

  /*-------------------------------------------------------------------------
    Checking lle_instance bounds
  -------------------------------------------------------------------------*/
  if (NULL != lle_cfg_ptr)
  {
    if ( ( lle_cfg_ptr->lle_instance <= LAN_LLE_MIN) || 
         ( lle_cfg_ptr->lle_instance >= LAN_LLE_MAX) )
    {
      LOG_MSG_ERROR_1("BAD LLE instance (%d)", lle_cfg_ptr->lle_instance);
      ASSERT(0);
      return;
    }
  }
  else
  {
    DATA_ERR_FATAL("NULL user_data ptr");
    return;
  }

  lle_array[lle_cfg_ptr->lle_instance].lle_instance =
    lle_cfg_ptr->lle_instance;

  memscpy(&lle_array[lle_cfg_ptr->lle_instance].start_info,
          sizeof(lan_llc_start_info_type) ,
          &lle_cfg_ptr->start_info,
          sizeof(lan_llc_start_info_type) );

  start_info = lle_cfg_ptr->lle_instance;

  PS_MEM_FREE(user_data);

  lan_llci_start( start_info );

} /* lan_llc_process_start() */


/*===========================================================================
FUNCTION LAN_LLC_PROCESS_RECONFIG()

DESCRIPTION
  This function handles the lan_llc_reconfig command.

PARAMETERS
  cmd:       Command identifier
  user_data: pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void lan_llc_process_reconfig
(
  ps_cmd_enum_type cmd,
  void             *user_data
)
{
  lan_lle_config_type*  reconfig_ptr;
  lan_lle_cb_type*      lle_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd != PS_LAN_LLC_RECONFIG_CMD)
  {
    LOG_MSG_ERROR_1("LAN_LLC_RECONFIG_CMD handler triggered by wrong cmd (%d)",
                    cmd);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Recover the LLE instance!
  -------------------------------------------------------------------------*/
  reconfig_ptr = (lan_lle_config_type*) user_data;

  /*-------------------------------------------------------------------------
    Checking lle_instance bounds
  -------------------------------------------------------------------------*/
  if (NULL != reconfig_ptr)
  {
    if ( ( reconfig_ptr->lle_instance <= LAN_LLE_MIN) || 
         ( reconfig_ptr->lle_instance >= LAN_LLE_MAX) )
    {
      LOG_MSG_ERROR_1("Bad lle_instance %d passed",
                      reconfig_ptr->lle_instance);
      ASSERT(0);
      return;
    }
  }
  else
  {
    DATA_ERR_FATAL("NULL user_data ptr");
    return;
  }

  lle_ptr = &lle_array[reconfig_ptr->lle_instance];

  if (lle_ptr->started == FALSE)
  {
    LOG_MSG_ERROR_0("attempt to reconfig llc instance that isn't started!");
    PS_MEM_FREE(user_data);
    return;
  }

  lle_ptr->start_info.tx_f_ptr       = reconfig_ptr->start_info.tx_f_ptr;
  lle_ptr->start_info.tx_f_user_data = reconfig_ptr->start_info.tx_f_user_data;

  PS_MEM_FREE(user_data);

} /* lan_llc_process_reconfig() */


/*===========================================================================
FUNCTION LAN_LLC_PROCESS_STOP()

DESCRIPTION
  This function handles the lan_llc_stop command.

PARAMETERS
  cmd: Command identifier
  user_data: pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void lan_llc_process_stop
(
  ps_cmd_enum_type cmd,
  void *user_data
)
{
  lan_lle_enum_type stop_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd != PS_LAN_LLC_STOP_CMD)
  {
    LOG_MSG_ERROR_1("LAN_LLC_STOP_CMD handler triggered by wrong cmd (%d)",
                    cmd);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
   Recover the LLE instance: user_data was a fake pointer. Actually the value
   it does contain is the actual LLE instance.
  -------------------------------------------------------------------------*/
  stop_info = (lan_lle_enum_type)(uint32)user_data;

  if( stop_info <= LAN_LLE_MIN ||
      stop_info >= LAN_LLE_MAX )
  {
    LOG_MSG_ERROR_1("Invalid LLE, value=%d", stop_info);
    ASSERT(0);
    return;
  }
  (void) lan_llci_stop( stop_info );

} /* lan_llc_process_stop() */

/*===========================================================================
FUNCTION LAN_LLC_DEINIT()

DESCRIPTION
  This function free the resources allocated in init function

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void lan_llc_deinit
(
  void
)
{
  uint8 i;
  
  for ( i = 0; i < LAN_LLE_MAX; i++ )
  {
    PS_DESTROY_CRIT_SECTION(&(lle_array[i].lan_lle_crit_section));
  }
}

