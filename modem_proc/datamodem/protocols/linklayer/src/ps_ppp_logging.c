/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ P P P _ L O G G I N G . C

GENERAL DESCRIPTION

  Contains the necessary routines to log framed or unframed PPP frames.
   
EXTERNALIZED FUNCTIONS

  ppp_log_frames_ext()
    This function logs the PPP-in-HDLC frames. The function iterates 
    through the PPP packet chain, logging each packet in succession.

Copyright (c) 2001-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_logging.c_v   1.3   13 Feb 2003 20:13:54   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_logging.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/25/12    fn     Remove usage of fatal error log messages
03/23/11    op     Data SU API cleanup
05/07/09    pp     CMI Phase-4: SU Level API Effort.
12/04/08    scb    Added PPP configuration parameters logging capability
11/10/08    scb    Fixed QTF compilation warnings.
11/01/05     jd    ppp_log_frames_ext() flavour is const to enable compile
                   time pre-computation of log code.
05/18/05    ssh    Fixed lint errors.
02/13/03     jd    removed old ppp_log_frames implementation, no longer used.
                   ppp_log_frames_ext takes device instead of iface,
                   fixed log code calculation
11/19/02     jd    for MM, include ps_ppp_logging.h instead of dsppplog.h
10/29/02    igt    turned LOG_MSG_ERROR() to LOG_MSG FATAL_ERROR() and changed text
09/19/02    igt    removed annoying MSG_ERR message.
09/12/02     vr    changed ppp_log_frames_ext to use the defines in 
                   log_codes.h
09/06/02    igt    added extended ppp logging capabilities under
                   FEATURE_DEBUG_PPP_LOG_EXT
02/26/02    sjy    Fixed QCAT bug (Log Record exceeding maximum allowed size)
                   by removing extra log_hdr_type buffer in log_alloc().
06/27/01    sjy    Included "comdef.h" to fix warning during compilation.
06/20/01    sjy    Created file.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "log_codes.h"
#include "ps_ppp.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_logging.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================
							
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_DATA_PS_PPP_LOGGING
/*---------------------------------------------------------------------------
  Payload size for optimal logging capacity at high RX speeds over 
  diagnostic interface.
---------------------------------------------------------------------------*/
#define PPP_MAX_BYTES 186

/*---------------------------------------------------------------------------
  Packet structure definition for extended PPP logging
---------------------------------------------------------------------------*/
/**/
LOG_RECORD_DEFINE(LOG_PPP_FRAMES_EXT_C)
  byte          data[ PPP_MAX_BYTES ];  /* PPP payload                     */
LOG_RECORD_END

/*---------------------------------------------------------------------------
  Log entry type for naming compatibility.  
---------------------------------------------------------------------------*/
typedef LOG_PPP_FRAMES_EXT_C_type log_ppp_frames_ext_type;



#endif /*FEATURE_DATA_PS_PPP_LOGGING*/

/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION PS_PPP_LOG_DEV_CONFIG_PARAM

DESCRIPTION

  This function logs the PPP device configuration parameters in the 
  diagnostic interface.
  The configuration parameters were passed in by the mode handler to 
  PPP via a call to ppp_start() or ppp_resync.
   
PARAMETERS
 configbuf_ptr   Pointer to PPP configuration buffer (allocated from PS mem)

  This function constructs the appropriate log packet from the 
  configbuf_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_ppp_log_dev_config_param
( 
  ppp_config_buffer_s_type      *configbuf_ptr
)
{
  ps_ppp_dev_config_param_log_pkt_type *log_ptr; /* pointer to log buffer */
  ps_ppp_dev_config_param_type          ppp_dev_config_param_ptr;
  ppp_fsm_type                         *fsm_ptr = NULL;
  ppp_dev_opts_type                    *opts_ptr = NULL;
  ppp_type                             *ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == configbuf_ptr)
  {
    DATA_ERR_FATAL("Invalid configbuf_ptr, PPP config item not initialized.");
    return;
  }
 
  memset(&ppp_dev_config_param_ptr, 0, sizeof(ps_ppp_dev_config_param_type));

  LOG_MSG_INFO2_1("ps_ppp_log_dev_config_param (dev %d)", configbuf_ptr->device);
  /*-----------------------------------------------------------------------
      Fill in the ppp_dev_config_param_ptr appropriately.
    -----------------------------------------------------------------------*/
  opts_ptr = &configbuf_ptr->dev_opts;
  ppp_dev_config_param_ptr.ppp_device_id = (uint32)configbuf_ptr->device;

  ppp_dev_config_param_ptr.ppp_setup_timeout = opts_ptr->setup_timeout;

  if( ( opts_ptr->ipcp_info.will_mask[FSM_REMOTE] & IPCP_N_PRIMARY_DNS ) &&
      ( opts_ptr->ipcp_info.want_mask[FSM_REMOTE] & IPCP_N_PRIMARY_DNS ) &&
      ( opts_ptr->ipcp_info.will_mask[FSM_LOCAL]  & IPCP_N_PRIMARY_DNS ) &&
      ( opts_ptr->ipcp_info.want_mask[FSM_LOCAL]  & IPCP_N_PRIMARY_DNS ) &&
      ( opts_ptr->ipcp_info.will_mask[FSM_REMOTE] & IPCP_N_SECONDARY_DNS ) &&
      ( opts_ptr->ipcp_info.want_mask[FSM_REMOTE] & IPCP_N_SECONDARY_DNS ) &&
      ( opts_ptr->ipcp_info.will_mask[FSM_LOCAL]  & IPCP_N_SECONDARY_DNS ) &&
      ( opts_ptr->ipcp_info.want_mask[FSM_LOCAL]  & IPCP_N_SECONDARY_DNS ) )
  {
    ppp_dev_config_param_ptr.negotiate_dns_server = TRUE;
  }

  ppp_dev_config_param_ptr.resync_dns_opts = opts_ptr->resync_dns_opts;
  ppp_dev_config_param_ptr.set_stabilize_accm_delay = 
                                     opts_ptr->set_stabilize_accm_delay;
  ppp_dev_config_param_ptr.ppp_failure_graceful_close = 
                                   opts_ptr->ppp_failure_graceful_close;

  ppp_cb_array = fsm_get_ppp_cb_array();
  fsm_ptr = &(ppp_cb_array[configbuf_ptr->device].fsm[PPP_PROTOCOL_LCP]);

  ppp_dev_config_param_ptr.lcp_creq_try_count = 
                                             fsm_ptr->pdc->try_configure;
  ppp_dev_config_param_ptr.lcp_ack_timeout = fsm_ptr->pdc->ack_timeout;

  fsm_ptr = &(ppp_cb_array[configbuf_ptr->device].fsm[PPP_PROTOCOL_IPCP]);

  ppp_dev_config_param_ptr.ipcp_creq_try_count = 
                                            fsm_ptr->pdc->try_configure;
  ppp_dev_config_param_ptr.ipcp_ack_timeout = fsm_ptr->pdc->ack_timeout;

  ppp_dev_config_param_ptr.auth_proto_mask = 
    (uint32)opts_ptr->lcp_info.want_vals[FSM_LOCAL].auth_proto_mask;
  ppp_dev_config_param_ptr.auth_retry_count = 
                      ppp_cb_array[configbuf_ptr->device].auth.retry_limit;
  ppp_dev_config_param_ptr.auth_timeout = 
                          ppp_cb_array[configbuf_ptr->device].auth.timeout;
  ppp_dev_config_param_ptr.user_id_len = opts_ptr->auth_info.user_id_len;
  ppp_dev_config_param_ptr.auth_passwd_len = opts_ptr->auth_info.passwd_len;

  if( 0 < opts_ptr->auth_info.user_id_len && 
      PPP_LOG_MAX_USER_ID_LEN > opts_ptr->auth_info.user_id_len )
  {
    memscpy(ppp_dev_config_param_ptr.user_id, 
            PPP_LOG_MAX_USER_ID_LEN,
            opts_ptr->auth_info.user_id_info, 
            opts_ptr->auth_info.user_id_len);
  }
  else
  {
    LOG_MSG_ERROR_1( "Invalid User ID length %d, PPP configuration item not"
               " properly initialized.", 
                opts_ptr->auth_info.user_id_len );
  }
  if(0 < opts_ptr->auth_info.passwd_len && 
      PPP_LOG_MAX_PASSWD_LEN > opts_ptr->auth_info.passwd_len )
  {
    memscpy(ppp_dev_config_param_ptr.auth_passwd, 
            PPP_LOG_MAX_PASSWD_LEN,
            opts_ptr->auth_info.passwd_info, 
            opts_ptr->auth_info.passwd_len);
  }
  else
  {
    LOG_MSG_ERROR_1( "Invalid Passwd length %d, PPP configuration item not"
               " properly initialized.", 
                opts_ptr->auth_info.passwd_len );
  }

  /*-----------------------------------------------------------------------
      Allocate log buffer space from diag.
    -----------------------------------------------------------------------*/

  log_ptr = (ps_ppp_dev_config_param_log_pkt_type *) 
              log_alloc( LOG_PS_PPP_DEV_CONFIG_PARAMS_C, 
                         sizeof(ps_ppp_dev_config_param_log_pkt_type ));

  if (log_ptr == NULL)
  {
    /*---------------------------------------------------------------------
      No log buffer or logging not on, so do nothing.
    ---------------------------------------------------------------------*/
    LOG_MSG_ERROR_0( "Log pointer allocation failure, PPP configuration "
               "item not logged." );
    return; 
  }
 
  log_ptr->version = PS_PPP_DEV_CONFIG_PARAM_LOG_PKT_VER;
  memset(log_ptr->reserved, 0, 3);
  memset(&(log_ptr->ppp_dev_config_param), 0, 
         sizeof( ps_ppp_dev_config_param_type ));
  
  /*-----------------------------------------------------------------------
    Copy the data from ppp_dev_config_param_ptr into log buffer.
  -----------------------------------------------------------------------*/
  memscpy( &( log_ptr->ppp_dev_config_param ),
            sizeof( ps_ppp_dev_config_param_type ) ,
            &ppp_dev_config_param_ptr, 
            sizeof( ps_ppp_dev_config_param_type ) );
  /*-----------------------------------------------------------------------
    Commit log buffer.
  -----------------------------------------------------------------------*/
  log_commit (log_ptr);

} /* ps_ppp_log_dev_config_param() */


#ifdef FEATURE_DATA_PS_PPP_LOGGING
/*===========================================================================
FUNCTION PPP_LOG_FRAMES_EXT

DESCRIPTION

  This function logs the provided PPP frame (HDLC framed or unframed) on
  the diagnostic interface.

  
  The function iterates through the input dsm chain, which may contain 
  multiple PPP, handling each packets each packet in succession.

  It may be called with framed or unframed ppp data, i.e. before or after 
  the HW/SW framing/unframing has occurred.
  
   
PARAMETERS
  item_ptr      - pointer to the dsm chain to be logged
  flavour       - The characteristics of this packet, 
                  i.e.  framed/unframed, rx/tx
  device        - the device over which the packet is passing
  
  This function constructs the appropriate log code from the ppp_data and
  device parameters.

  LOG CODE RANGE      DIRECTION      FRAMING
  --------------      ---------      -------
  275-290             Receive        HDLC
  291-306             Transmit       HDLC
  307-322             Receive        (none)
  323-338             Transmit       (none)
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ppp_log_frames_ext
( 
  dsm_item_type                  *item_ptr,
  const pppi_log_flavour_e_type  flavour,
  ppp_dev_enum_type              device
)
{
  log_ppp_frames_ext_type  *log_ptr;   /* pointer to log buffer            */
  byte                     *data_ptr;  /* ptr to buffer to copy the data   */
  uint16                   offset;     /* byte offset into PPP chain       */
  uint16                   num_bytes;  /* # of bytes copied by dsm_extract */
  uint32                   len;        /* length of log frame payload      */
  uint16                   log_code;   /* the log_code for the log packet  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (item_ptr == NULL) 
  {
    /*-----------------------------------------------------------------------
      Log item passed in is NULL, so do nothing.
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0( "NULL item ptr, frame skipped.");
    return;
  }
  
  /*-------------------------------------------------------------------------
    Calculate the log code

    The log code is the base log code (275) for the flavour of the logging 
    (rx/tx, framed/unframed) plus the interface.  The log codes are 
    arranged such that there are 16 codes for each flavour to represent 
    up to 16 interfaces.

    NOTE: no range checking is done so incoming parameters MUST be correct.
  -------------------------------------------------------------------------*/
  log_code = (uint16)(LOG_PPP_FRAMES_RESERVED_CODES_BASE_C + ((uint8)flavour * 16) +
                      (uint8)device);

  /*-------------------------------------------------------------------------
    Determine length of data packet chain.
  -------------------------------------------------------------------------*/
  offset = 0;
  len = dsm_length_packet (item_ptr);            
  do
  {
    /*-----------------------------------------------------------------------
      Allocate log buffer space from diag.
    -----------------------------------------------------------------------*/
    log_ptr = (log_ppp_frames_ext_type *)
                       log_alloc(log_code, sizeof(log_ppp_frames_ext_type));
    
    if (log_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        No log buffer or logging not on, so do nothing.
      ---------------------------------------------------------------------*/
      return; 
    }  
   
    /*-----------------------------------------------------------------------
      Extract data from item_ptr into log buffer.
    -----------------------------------------------------------------------*/
    data_ptr  = (byte *) log_ptr->data;
    num_bytes = PPP_MAX_BYTES;
    (void)dsm_seek_extract (&item_ptr, &offset, data_ptr, &num_bytes);

    offset   += num_bytes;
    len      -= num_bytes;
    
    /*-----------------------------------------------------------------------
      Set buffer length to number of bytes extracted.
    -----------------------------------------------------------------------*/
    log_shorten (log_ptr, sizeof(log_hdr_type) + num_bytes);
    log_set_length (log_ptr, sizeof(log_hdr_type) + num_bytes);

    /*-----------------------------------------------------------------------
      Commit log buffer.
    -----------------------------------------------------------------------*/
    log_commit (log_ptr);

  } while (len);

} /* ppp_log_frames_ext() */

#endif /* FEATURE_DATA_PS_PPP_LOGGING */
