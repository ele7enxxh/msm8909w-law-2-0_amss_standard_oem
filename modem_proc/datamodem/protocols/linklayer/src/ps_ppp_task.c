/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ P P P _ T A S K . C

GENERAL DESCRIPTION
  This is the file that defines all of the PPP functions that are tied to
  executing PPP in some task context.

EXTERNALIZED FUNCTIONS
  ppp_task_powerup_init()
    The fucntion that must be called from the executing tasks powerup
    initialization function to initialize PPP and register all of the
    callbacks etc...

Copyright (c) 2002-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_task.c#1 $ 
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
07/08/14    cx     Fixed the error when reading NV_PPP_CONFIG_DATA_I NV item  
12/14/12    fn     Remove default NV item writes when NV read fails
11/05/12    fn     Optimizing the read for PS PPP CONFIG NV item 
05/01/12    sb     Always On UIM Feature Cleanup
10/17/11    ssh    eHRPD Always On support
06/06/11    ash    Added support for PPP Partial Context
03/23/11    op     Data SU API cleanup
01/28/11    jee    JCDMA feature cleanup
12/05/10    ssh    Added ppp_ncp_abort()
11/18/10    ssh    Added ppp_ncp_resync()
08/31/09    mga    Merged from eHRPD branch
06/09/09    mga    Merged Compiler warnings fixes
05/07/09    pp     CMI Phase-4: SU Level API Effort.
11/10/08    scb    Fixed QTF compilation warnings.
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/06/07    scb    Swapped the auth_retry and auth_timeout variables in the
                   ppp_task_init() function for consistency.
08/18/07    scb    Featurized the MMGSDI interaction of RUIM code using the
                   FEATURE_RUIM_FUNC_INTERFACE feature.
08/16/07    scb    Added PPP Authentication initialization function to 
                   ppp_task_init()
05/14/07    scb    Changed variable names to avoid compilation error for
                   JCDMA_1X builds
04/27/07    scb    Added code to populate the default values for constants
                   such as LCP/IPCP/AUTH and IPv6CP timeout and retry values
                   during ppp_task_init() under FEATURE_JCDMA_1X
04/02/07    ssh    New, PPP_VENDOR_SEND_CMD. Error checks. More F3 messages.
02/10/05    ifk    Change LCP term timeouts to 3 sec by default.
10/31/04    ifk    Added reading of ppp_ccp_reject NV item
10/14/04    ifk    Added meta info ** to pppi_receive() call
08/18/04    mvl    Changed misspelling in NV item for PPP configuration.
07/16/04    ifk    Added reading of NV_AUTH_REQUIRE_PASSWORD_ENCRYPTION_I
                   nv_item.  Added processing of PPP resync command
04/13/04    ifk    Uncommented setting of IPV6CP config parameters and 
                   featurized under FEATURE_DATA_PS_IPV6
01/30/04    vp     Write back the default values for ppp_config_data to the
                   NV if not already set/unable to read in ppp_task_init()
11/11/03    ifk    Included ps_ppp_auth.h to get the prototype of
                   ppp_auth_set_device_constants()
10/27/03    ifk    Added ppp_task_init()
08/13/03    ifk    PPP cleanup for IPv6.
04/18/03    vsk    enode the right dev & prot fields in 
                   ppp_task_process_inject_pkt()
04/07/03    usb    Added PPP_INJECT_PACKET_CMD processing to replay an out 
                   of order PPP packet for PPP receive processing.
01/27/03    usb    Added PPP_SET_MODE_CMD processing to set PPP mode in PS 
                   context only 
09/23/02    mvl    Modified signal usage.
09/12/02    mvl    removed unused variable. Removed netmodel featurization.
09/02/02    mvl    Removed the PPP rx callback, and registration as this is
                   done in the start command.
08/13/02    mvl    Session handle support.
07/25/02    mvl    Updates for PPP renaming.
07/24/02    mvl    Now ensuring that is PPP mode is NONE discard before
                   sending to any of the PPP unframing functions or input.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Make the file more independent of PS via MACROs defined in
                   header file
04/01/02    mvl    Created Module
===========================================================================*/



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          INCLUDE FILES FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
/*---------------------------------------------------------------------------
  The following include is the task include that specifies the support
  functions PPP uses to set itself up as a service executing in that task
---------------------------------------------------------------------------*/
#include "ps_svc.h"
#include "ps_ppp_defs.h"

/*---------------------------------------------------------------------------
  The general includes
---------------------------------------------------------------------------*/
#include "amssassert.h"
#include "dsm.h"
#include "ps_ppp_task.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_ppp.h"
#include "ps_ppp_config.h"
#include "ps_pppi.h"
#include "ps_utils.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_pppi_vend.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_ppp_ncp.h"
#include "ps_ppp_ncpi.h"
#endif /* FEATURE_DATA_PS_EHRPD */
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_PS_PPP_EAP
#include "ps_eap_aka.h"
#endif /* FEATURE_DATA_PS_PPP_EAP */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         INTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  NV item path & the variable to store JCDMA mode info
---------------------------------------------------------------------------*/
#define PS_PPP_JCDMA_MODE_FILE_NAME  "/nv/item_files/jcdma/jcdma_mode"

static boolean jcdma_mode = FALSE;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
void ppp_task_process_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_resync
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
);

void ppp_task_process_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_abort
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_set_mode
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_inject_pkt
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_vendor_send
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

#ifdef FEATURE_DATA_PS_EHRPD
void ppp_task_process_ncp_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_ncp_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_ncp_resync
(
  ps_cmd_enum_type  cmd,
  void *data_ptr
);

void ppp_task_process_ncp_abort
(
  ps_cmd_enum_type  cmd,
  void *data_ptr
);

void ppp_task_process_device_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_trigger_lsd
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
);
#endif /* FEATURE_DATA_PS_EHRPD */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            EXTERNAL DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PPP_TASK_POWERUP_INIT()

DESCRIPTION
  This function does the powerup initialization for the PPP module.  This
  includes registering handlers with PS (the task executing it) for the PPP
  RX signals.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_powerup_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    register the handlers for the various PPP commands that must be handled
    in the task context.
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler(PPP_START_CMD, ppp_task_process_start);
  (void)ps_set_cmd_handler(PPP_RESYNC_CMD, ppp_task_process_resync);
  (void)ps_set_cmd_handler(PPP_STOP_CMD, ppp_task_process_stop);
  (void)ps_set_cmd_handler(PPP_ABORT_CMD, ppp_task_process_abort);
  (void)ps_set_cmd_handler(PPP_SET_MODE_CMD, ppp_task_process_set_mode);
  (void)ps_set_cmd_handler(PPP_INJECT_PKT_CMD, ppp_task_process_inject_pkt);
  (void)ps_set_cmd_handler(PPP_VENDOR_SEND_CMD, ppp_task_process_vendor_send);

#ifdef FEATURE_DATA_PS_EHRPD
  (void)ps_set_cmd_handler(PPP_NCP_START_CMD, ppp_task_process_ncp_start);
  (void)ps_set_cmd_handler(PPP_NCP_STOP_CMD, ppp_task_process_ncp_stop);
  (void)ps_set_cmd_handler(PPP_NCP_RESYNC_CMD, ppp_task_process_ncp_resync);
  (void)ps_set_cmd_handler(PPP_NCP_ABORT_CMD, ppp_task_process_ncp_abort);
  (void)ps_set_cmd_handler(PPP_DEVICE_STOP_CMD, ppp_task_process_device_stop);
  (void)ps_set_cmd_handler(PPP_TRIGGER_LSD_CMD, ppp_task_process_trigger_lsd);
#endif /* FEATURE_DATA_PS_EHRPD */
  
  /*-------------------------------------------------------------------------
    Call the PPP module initialization funtion
  -------------------------------------------------------------------------*/
  pppi_powerup_init();
  
} /* ppp_task_powerup_init() */

/*===========================================================================
FUNCTION  PPP_IS_CURR_MODE_JCDMA

DESCRIPTION   Called to check if it is in JCDMA mode. 

DEPENDENCIES  This parameter will be set based on UIM API

RETURN VALUE  TRUE  - JCDMA mode enabled
              FALSE - JCDMA mode disabled
  
SIDE EFFECTS
===========================================================================*/
boolean ppp_is_curr_mode_jcdma(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    JCDMA mode is enabled if both the JCDMA feature is defined & the NV item 
    is set to TRUE
  -------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_1X
  if(TRUE == jcdma_mode)
  {
    return TRUE;
  }
#endif

  return FALSE;
} /* ppp_is_curr_mode_jcdma() */

/*===========================================================================
FUNCTION PPP_TASK_INIT()

DESCRIPTION
  This function does the startup initialization for the PPP module.  This
  includes configuring the PPP configurable parameters

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_init( void )
{
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  int  init_check = 0;
#endif /* FEATURE_RUIM_FUNC_INTERFACE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_task_nv_init();

  /*-----------------------------------------------------------------------
    Initialize the PPP authentication framework.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  init_check = ppp_auth_init();
  if( 0 != init_check)
  {
    DATA_ERR_FATAL("Unable to initialize authentication module for PPP.");
    return;
  } /* Unable to initialize authentication framework */
#endif /* FEATURE_RUIM_FUNC_INTERFACE  */

  /* Initialize EAP_AKA, makes usage faster */
#ifdef FEATURE_DATA_PS_PPP_EAP
  eap_aka_powerup_init();  
#endif /*FEATURE_DATA_PS_PPP_EAP*/

  return;
} /* ppp_task_init() */

/*===========================================================================
FUNCTION PPP_TASK_NV_INIT()

DESCRIPTION
  This function configures the PPP configurable parameters

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Sets configurable protocol specific device constants
===========================================================================*/
void ppp_task_nv_init
(
  void
)
{
  ppp_dev_enum_type     device;
  nv_item_type          ppp_nv_item;
  boolean               require_encrypted_passwd = FALSE;
  ppp_ccp_config_e_type ccp_config = PPP_CCP_REJECT;
  ppp_type             *ppp_cb_array = NULL;
  char                  item_file_path[] = PS_PPP_JCDMA_MODE_FILE_NAME;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Configure CCP
  -----------------------------------------------------------------------*/
  if( NV_DONE_S == ps_get_nv_item( NV_PPP_CCP_REJECT_I,
                                   &ppp_nv_item ) )
  {
    ccp_config = (ppp_ccp_config_e_type)(ppp_nv_item.ppp_ccp_reject);
  }
  else
  {
    ppp_nv_item.ppp_ccp_reject = (uint8)ccp_config;
  } /* end else */

  /*-----------------------------------------------------------------------
    Check if encrypted passwords required
  -----------------------------------------------------------------------*/
  if( NV_DONE_S == ps_get_nv_item( NV_AUTH_REQUIRE_PASSWORD_ENCRYPTION_I,
                                   &ppp_nv_item ) )
  {
    require_encrypted_passwd = 
      ppp_nv_item.auth_require_password_encryption;
  }
  else
  {
    ppp_nv_item.auth_require_password_encryption = 
      require_encrypted_passwd;
  } /* end else */

  /*-------------------------------------------------------------------------
    Read the NV item to know if JCDMA feature is enabled or not
  -------------------------------------------------------------------------*/  
  jcdma_mode = FALSE;
  if (0 != ps_read_efs_nv(item_file_path, &jcdma_mode, sizeof(jcdma_mode)))
  {
    LOG_MSG_ERROR_0("Unable to read JCDMA mode NV item, using the default value");
  }

  /*-----------------------------------------------------------------------
    Retrieve PPP_CONFIG_DATA from NV, NV_PPP_CONFIG_DATA_I
  -----------------------------------------------------------------------*/
  for(device = PPP_MIN_DEV; device < PPP_MAX_DEV; device++)
  {
    memset(&ppp_nv_item, 0, sizeof(ppp_nv_item));

    /*---------------------------------------------------------------------
       Specify the index to get the correspoding parameters from NV item
    ---------------------------------------------------------------------*/
    ppp_nv_item.ppp_config_data.index = (uint8)device;

    if( NV_DONE_S != ps_get_nv_item( NV_PPP_CONFIG_DATA_I, &ppp_nv_item ) )
    {
      /*---------------------------------------------------------------------
        Set the NV item to default values for all its components if unable
        to read from NV_PPP_CONFIG_DATA_I
      ---------------------------------------------------------------------*/
      if(TRUE == ppp_is_curr_mode_jcdma())
      {
        ppp_nv_item.ppp_config_data.lcp_term_timeout = LCP_TERM_TO;
      }
      else
      {
        ppp_nv_item.ppp_config_data.lcp_term_timeout = LCP_TIMEOUT *
                                                       LCP_TERM_MULT;
      }
  
      ppp_nv_item.ppp_config_data.lcp_ack_timeout  = LCP_TIMEOUT;
      ppp_nv_item.ppp_config_data.lcp_req_try      = LCP_REQ_TRY;
      ppp_nv_item.ppp_config_data.lcp_nak_try      = LCP_NAK_TRY;
      ppp_nv_item.ppp_config_data.lcp_term_try     = LCP_TERM_TRY;
  
      ppp_nv_item.ppp_config_data.auth_retry   = PPP_AUTH_RETRY_COUNTER_VAL;
      ppp_nv_item.ppp_config_data.auth_timeout = PPP_AUTH_TIMER_VAL;
  
      ppp_nv_item.ppp_config_data.ipcp_term_timeout       = IPCP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipcp_ack_timeout        = IPCP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipcp_req_try            = IPCP_REQ_TRY;
      ppp_nv_item.ppp_config_data.ipcp_nak_try            = IPCP_NAK_TRY;
      ppp_nv_item.ppp_config_data.ipcp_term_try           = IPCP_TERM_TRY;
      ppp_nv_item.ppp_config_data.ipcp_compression_enable = 
          (uint8)PPP_NCP_COMPRESS_IGNORE;
  
      ppp_nv_item.ppp_config_data.ipv6cp_term_timeout     = IPV6CP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipv6cp_ack_timeout      = IPV6CP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipv6cp_req_try            = IPV6CP_REQ_TRY;
      ppp_nv_item.ppp_config_data.ipv6cp_nak_try            = IPV6CP_NAK_TRY;
      ppp_nv_item.ppp_config_data.ipv6cp_term_try         = IPV6CP_TERM_TRY;
      ppp_nv_item.ppp_config_data.ipv6cp_compression_enable =
          (uint8)PPP_NCP_COMPRESS_IGNORE;
    }

    /*-----------------------------------------------------------------------
      Initialize LCP and IPCP device constants
    -----------------------------------------------------------------------*/
    lcp_set_device_constants(device, &ppp_nv_item.ppp_config_data);
    ipcp_set_device_constants(device, &ppp_nv_item.ppp_config_data);
#ifdef FEATURE_DATA_PS_IPV6
    ipv6cp_set_device_constants(device, &ppp_nv_item.ppp_config_data);
#endif /* FEATURE_DATA_PS_IPV6 */
    ppp_auth_set_device_constants(device, &ppp_nv_item.ppp_config_data);

    /*-----------------------------------------------------------------------
      Populate the device constants structure from the NV item
    -----------------------------------------------------------------------*/
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_LCP_ACK_TIMEOUT,
                               ppp_nv_item.ppp_config_data.lcp_ack_timeout))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d", 
                 device, DEFAULT_LCP_ACK_TIMEOUT);
      ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_LCP_REQUEST_TRY,
                                   ppp_nv_item.ppp_config_data.lcp_req_try))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d", 
                 device, DEFAULT_LCP_REQUEST_TRY);
      ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_IPCP_ACK_TIMEOUT,
                              ppp_nv_item.ppp_config_data.ipcp_ack_timeout))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d", 
                 device, DEFAULT_IPCP_ACK_TIMEOUT);
      ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_IPCP_REQUEST_TRY,
                                   ppp_nv_item.ppp_config_data.ipcp_req_try))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d", 
                 device, DEFAULT_IPCP_REQUEST_TRY);
      ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_AUTH_TIMEOUT,
                                   ppp_nv_item.ppp_config_data.auth_timeout))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d", 
                 device, DEFAULT_AUTH_TIMEOUT);
      ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_AUTH_RETRY,
                                   ppp_nv_item.ppp_config_data.auth_retry))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d", 
                 device, DEFAULT_AUTH_RETRY);
      ASSERT(0);
      return;
    }

#ifdef FEATURE_DATA_PS_IPV6
    if(0 > ppp_set_device_constant(
               device,
               DEFAULT_IPV6CP_ACK_TIMEOUT,
               ppp_nv_item.ppp_config_data.ipv6cp_ack_timeout)
    )
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d",
                 device, DEFAULT_IPV6CP_ACK_TIMEOUT);
      ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                DEFAULT_IPV6CP_REQUEST_TRY,
                                ppp_nv_item.ppp_config_data.ipv6cp_req_try))
    {
      LOG_MSG_ERROR_2( "ppp_task_init: Failed to set default constant values"
                 "  for device %d constant %d",
                 device, DEFAULT_IPV6CP_REQUEST_TRY);
      ASSERT(0);
      return;
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Initialize password encryption
    -----------------------------------------------------------------------*/
    ppp_cb_array = fsm_get_ppp_cb_array();
    ppp_cb_array[device].auth.require_encrypted_passwd = 
      require_encrypted_passwd;

    /*-----------------------------------------------------------------------
        Initialize CCP
    -----------------------------------------------------------------------*/
    ppp_cb_array[device].ccp_config = ccp_config;
  } /* for(all PPP devices) */

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    Initialize VSNCP constants
  -------------------------------------------------------------------------*/
  
  /*-----------------------------------------------------------------------
   If NV item is unavaliable then device contains default values 
   from vsncp_3gpp2i_default_constants 
  -----------------------------------------------------------------------*/
  if( NV_DONE_S == ps_get_nv_item( NV_PPP_VSNCP_CONFIG_DATA_I, &ppp_nv_item ) )
  {
    if(0 > ppp_set_device_constant
        ( PPP_UM_SN_DEV, /* ignored for VSNCP */
          VSNCP_3GPP2_REQ_TRY,
          ppp_nv_item.ppp_vsncp_config_data.vsncp_req_try ) )
    {
      DATA_ERR_FATAL("ppp_task_init: failed to set default");
      return;
    }

    if(0 > ppp_set_device_constant
        ( PPP_UM_SN_DEV, /* ignored for VSNCP */
          VSNCP_3GPP2_TERM_TRY,
          ppp_nv_item.ppp_vsncp_config_data.vsncp_term_try ) )
    {
      DATA_ERR_FATAL("ppp_task_init: failed to set default");
      return;
    }

    if(0 > ppp_set_device_constant
        ( PPP_UM_SN_DEV, /* ignored for VSNCP */
          VSNCP_3GPP2_VSNCP_ACK_TIMEOUT,
          ppp_nv_item.ppp_vsncp_config_data.vsncp_ack_timeout ) )
    {
      DATA_ERR_FATAL("ppp_task_init: failed to set default");
      return;
    }

    if(0 > ppp_set_device_constant
        ( PPP_UM_SN_DEV, /* ignored for VSNCP */
          VSNCP_3GPP2_TERM_TIMEOUT,
          ppp_nv_item.ppp_vsncp_config_data.vsncp_term_timeout ) )
    {
      DATA_ERR_FATAL("ppp_task_init: failed to set default");
      return;
    }

  }
#endif /* FEATURE_DATA_PS_EHRPD */
} /* ppp_task_nv_init() */

/*===========================================================================
FUNCTION PPP_TASK_PROCESS_START()

DESCRIPTION
  This function handles the ppp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_start
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_START_CMD)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing START cmd");
  pppi_start((ppp_config_buffer_s_type *)data_ptr);

} /* ppp_task_process_start() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_RESYNC()

DESCRIPTION
  This function handles the ppp_resync command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_resync
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_RESYNC_CMD)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing RESYNC cmd");
  pppi_resync((ppp_config_buffer_s_type *)data_ptr);

} /* ppp_task_process_resync() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_ABORT()

DESCRIPTION
  This function handles the ppp_abort command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_abort
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  pppi_abort_info_type abort_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_ABORT_CMD)
  {
    DATA_ERR_FATAL("Invalid Arguments");
    return;
  }

  LOG_MSG_INFO2_0("PPP processing ABORT cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_abort()
    abort_info:
      device         : extract device from high 16-bits 
      session_handle : extract session_handle from low 16-bits 
  -------------------------------------------------------------------------*/
  abort_info.device         = 
    (uint16)(((uint32)(data_ptr) & 0xffff0000) >> 16 ); 
  abort_info.session_handle = (uint16)((uint32)(data_ptr) & 0x0000ffff);

  pppi_abort(abort_info);

} /* ppp_task_process_abort() */



/*===========================================================================
FUNCTION PPP_TASK_PROCESS_STOP()

DESCRIPTION
  This function handles the ppp_close command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_stop
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
  pppi_stop_info_type stop_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_STOP_CMD)
  {
    DATA_ERR_FATAL("Invalid Arguments");
    return;
  }

  LOG_MSG_INFO2_0("PPP processing STOP cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_stop()
    stop_info:
      device:   extract device from high 16-bits 
      protocol: extract protocol from low 16-bits 
  -------------------------------------------------------------------------*/
  stop_info.device = 
    (uint16)(((uint32)(data_ptr)& 0xffff0000) >> 16 );
  stop_info.protocol = (uint16)((uint32)(data_ptr)&(0x0000ffff));
  pppi_stop(stop_info);

} /* ppp_task_process_stop() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_SET_MODE()

DESCRIPTION
  This function handles the ppp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_set_mode
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  pppi_mode_info_type mode_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_SET_MODE_CMD)
  {
    DATA_ERR_FATAL("Invalid Arguments");
    return;
  }

  LOG_MSG_INFO2_0("PPP processing SET_MODE cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_set_mode() 
    mode_info:
      device : extract device from high 16-bits 
      mode   : extract mode from low 16-bits 
  -------------------------------------------------------------------------*/
  mode_info.device   = 
    (uint16)(((uint32)(data_ptr) & 0xffff0000) >> 16 );
  mode_info.mode = (uint16)((uint32)(data_ptr) & 0x0000ffff);

  pppi_set_mode(mode_info);

} /* ppp_task_process_set_mode() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_INJECT_PKT()

DESCRIPTION
  This function handles the ppp_inject_pkt command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_inject_pkt
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  ppp_dev_enum_type device;
  uint16 protocol;
  dsm_item_type *pkt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_INJECT_PKT_CMD)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing INJECT_PKT cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_receive()
  -------------------------------------------------------------------------*/
  pkt_ptr = (dsm_item_type*) data_ptr;
  device = (ppp_dev_enum_type)(pkt_ptr->app_field & 0xFFFF);
  protocol = (uint16)((pkt_ptr->app_field >> 16) & 0xFFFF);

  pppi_receive(device, protocol, &pkt_ptr, NULL);

} /* ppp_task_process_inject_pkt() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_VENDOR_SEND()

DESCRIPTION
  This function handles the ppp_vendor_send command.

PARAMETERS
  data_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_vendor_send
(
  ps_cmd_enum_type  cmd,
  void            * data_ptr
)
{
  ppp_vendor_pkt_info_s_type * pkt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if( ( cmd != PPP_VENDOR_SEND_CMD ) || ( data_ptr == NULL ) )
  {
    DATA_ERR_FATAL("Invalid params in ppp_task_process_vendor_send");
    return;
  }

  LOG_MSG_INFO2_0("PPP processing VENDOR_SEND cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_vendor_send()
  -------------------------------------------------------------------------*/
  pkt_info_ptr = (ppp_vendor_pkt_info_s_type *)data_ptr;
  pppi_vendor_send( pkt_info_ptr->device,
                    pkt_info_ptr->protocol,
                    pkt_info_ptr->id, 
                    pkt_info_ptr->pkt_ptr );
  
  /*-------------------------------------------------------------------------
    Free configuration memory
  -------------------------------------------------------------------------*/
  PS_MEM_FREE(pkt_info_ptr);

} /* ppp_task_process_vendor_send() */


#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPP_TASK_PROCESS_NCP_START()

DESCRIPTION
  This function handles the ppp_ncp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_ncp_start
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd != PPP_NCP_START_CMD ) || ( data_ptr == NULL ) )
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing NCP_START cmd");
  ncpi_start((ppp_ncp_handle_type)data_ptr);

} /* ppp_task_process_ncp_start() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_NCP_STOP()

DESCRIPTION
  This function handles the ppp_ncp_stop command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_ncp_stop
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd != PPP_NCP_STOP_CMD ) || ( data_ptr == NULL ) )
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing NCP_STOP cmd");
  ncpi_stop((ppp_ncp_handle_type)data_ptr);

} /* ppp_task_process_ncp_stop() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_NCP_RESYNC()

DESCRIPTION
  This function handles the ppp_ncp_resync command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_ncp_resync
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd != PPP_NCP_RESYNC_CMD ) || ( data_ptr == NULL ) )
  {  
    LOG_MSG_ERROR_0("Invalid args in NCP resync");  
    return;
  }

  LOG_MSG_INFO2_0( "PPP processing NCP_RESYNC cmd");
  ncpi_resync((ppp_ncp_handle_type)data_ptr);

} /* ppp_task_process_ncp_resync() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_NCP_ABORT()

DESCRIPTION
  This function handles the ppp_ncp_abort command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_ncp_abort
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd != PPP_NCP_ABORT_CMD ) || ( data_ptr == NULL ) )
  {  
    LOG_MSG_ERROR_2("Invalid args in NCP abort, cmd 0x%x, data_ptr %p", 
                  cmd, data_ptr);  
    return;
  }

  LOG_MSG_INFO2_0( "PPP processing NCP_ABORT cmd");
  ncpi_abort((ppp_ncp_handle_type)data_ptr);

} /* ppp_task_process_ncp_abort() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_DEVICE_STOP()

DESCRIPTION
  This function handles the ppp_device_stop command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_device_stop
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
  ppp_dev_enum_type device;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_DEVICE_STOP_CMD)
  {
    DATA_ERR_FATAL("Invalid Arguments");
    return;
  }

  LOG_MSG_INFO2_0("PPP processing DEVICE_STOP cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_device_stop()
  -------------------------------------------------------------------------*/
  device = ( ppp_dev_enum_type )(( uint32 ) data_ptr );

  pppi_device_stop(device);

} /* ppp_task_process_device_stop() */

/*===========================================================================
FUNCTION PPP_TASK_PROCESS_TRIGGER_LSD()

DESCRIPTION
  This function handles the ppp_trigger_lsd command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_trigger_lsd
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_TRIGGER_LSD_CMD)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing TRIGGER_LSD cmd");
  pppi_trigger_lsd((ppp_trigger_lsd_info_s_type *)data_ptr);
} /* ppp_task_process_trigger_lsd() */
#endif /* FEATURE_DATA_PS_EHRPD */
