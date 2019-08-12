#ifndef PS_PPP_LOGGING_H
#define PS_PPP_LOGGING_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ P P P _ L O G G I N G . H

GENERAL DESCRIPTION

  Contains the necessary routines to log framed or unframed PPP frames.

EXTERNALIZED FUNCTIONS

  ppp_log_frames_ext()
    This function logs the PPP-in-HDLC frames. The function iterates
    through the PPP packet chain, logging each packet in succession.

Copyright (c) 2001-2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_logging.h_v   1.1   13 Feb 2003 20:14:10   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_logging.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/08    scb    Added PPP configuration parameters logging capability
11/01/05    jd     Added macro wrapper to ppp_log_frames_ext that checks log
                   status before calling the function to lessen CPU loading.
02/13/03    jd     removed old ppp_log_frames prototype, ppp_log_frames_ext 
                   prototype updated to use device instead of iface
02/13/03    jd     removed old ppp_log_frames implementation, no longer used
09/12/02    vr     removed redundant #defines
09/06/02   igt     added extended ppp logging capabilities under
                   FEATURE_DEBUG_PPP_LOG_EXT
02/26/02   sjy     Wrapped header file with FEATURE_DS_PPP_LOGGING.
06/20/01   sjy     Created file.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "log.h"
#include "ps_ppp_defs.h"
#include "ps_pppi.h"

#ifdef FEATURE_DATA_PS_PPP_LOGGING
#include "dsm.h"
#endif /*FEATURE_DATA_PS_PPP_LOGGING*/

/*===========================================================================

                      EXPORTED DATA TYPES FOR MODULE

===========================================================================*/
#ifdef FEATURE_DATA_PS_PPP_LOGGING
/*-------------------------------------------------------------------------
  These are to help the caller of the extended ppp logging to describe
  the nature of ppp data being passed in. 

  The log code for the diagnostic packet is generated based on these
  and the interface values.
-------------------------------------------------------------------------*/
typedef enum
{
  PPP_LOG_FRAMED_RX = 0,
  PPP_LOG_FRAMED_TX = 1,
  PPP_LOG_UNFRAMED_RX = 2,
  PPP_LOG_UNFRAMED_TX = 3
} pppi_log_flavour_e_type;
#endif /*FEATURE_DATA_PS_PPP_LOGGING*/

#define PPP_LOG_MAX_PASSWD_LEN (PPP_MAX_PASSWD_LEN + 1)

#define PPP_LOG_MAX_USER_ID_LEN (PPP_MAX_USER_ID_LEN + 1)

#define PS_PPP_DEV_CONFIG_PARAM_LOG_PKT_VER    1

/*---------------------------------------------------------------------------
  Structure for PS_PPP_DEV_CONFIG_PARAM_TYPE

  NOTE: Diag requires log structures to be 4-byte aligned.

  Fields are:

  PPP_DEVICE_ID: 	      PPP device on which the PPP session is 
                              requested.
  PPP_SETUP_TIMEOUT:          PPP setup timer timeout value (msecs).
  NEGOTIATE_DNS_SERVER:       Indicates whether DNS options should be 
                              negotiated for the current PPP session.
  RESYNC_DNS_OPTS:	      Indicates whether DNS options should be 
                              negotiated on a resync.
  SET_STABILIZE_ACCM_DELAY:   Indicates whether an LCP discard 
                              message has to be sent or not.
  PPP_FAILURE_GRACEFUL_CLOSE: Indicates whether PPP should send 
                              a T REQ on a call tear down in adverse 
                              conditions such as TOs etc.
  LCP_CREQ_TRY_COUNT:         Number of LCP C_REQ tries.
  LCP_ACK_TIMEOUT: 	      LCP C_REQ timer timeout value i.e time to 
                              wait for LCP C_ACK (msecs).
  IPCP_CREQ_TRY_COUNT:	      Number of IPCP C_REQ tries.
  IPCP_ACK_TIMEOUT:	      IPCP C_REQ timer timeout value i.e time to 
                              wait for IPCP C_ACK (msecs).
  AUTH_PROTO_MASK:            Auth protocol(s) supported by current PPP 
                              session i.e either PAP or CHAP or both.
  AUTH_RETRY_COUNT: 	      Number of AUTH retries.
  AUTH_TIMEOUT: 	      AUTH timer timeout value (msecs).
  USER_ID_LEN:	              Length of user_id for PPP negotiation in the 
                              authentication phase.
  AUTH_PASSWD_LEN:	      Length of password PPP negotiation in the 
                              authentication phase.
  USER_ID:	              User id used by PPP during negotiation in the 
                              authentication phase.
  AUTH_PASSWD:  	      Password used by PPP during negotiation in the 
                              authentication phase.
---------------------------------------------------------------------------*/
typedef struct ps_ppp_dev_config_param_s
{
  uint32                         ppp_device_id;
  uint32                         ppp_setup_timeout;
  uint32                         negotiate_dns_server;
  uint32                         resync_dns_opts;
  uint32                         set_stabilize_accm_delay;
  uint32                         ppp_failure_graceful_close; 
  uint32                         lcp_creq_try_count;
  uint32                         lcp_ack_timeout;
  uint32                         ipcp_creq_try_count;
  uint32                         ipcp_ack_timeout;
  uint32                         auth_proto_mask;
  uint32                         auth_retry_count;
  uint32                         auth_timeout;
  uint32                         user_id_len;
  uint32                         auth_passwd_len;
  uint8                          user_id[PPP_LOG_MAX_USER_ID_LEN];
  uint8                          auth_passwd[PPP_LOG_MAX_PASSWD_LEN];
} ps_ppp_dev_config_param_type;

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_PPP_DEV_CONFIG_PARAM_C

  NOTE: Diag requires log structures to be 4-byte aligned.

  Fields are
  hdr                          : Diag header (DONT MESS WITH THIS FIELD)
  version                      : version of this log pkt's definition
  reserved                     : defined so as to make structure word 
                                 aligned DONT CHANGE THIS as Diag 
                                 requires log structures to be 4-byte aligned
  ps_ppp_dev_config_params_type: PPP device configuration parameters
                                 structure
---------------------------------------------------------------------------*/
typedef struct
{
  log_hdr_type                  hdr;
  uint8                         version;
  uint8                         reserved[3];
  ps_ppp_dev_config_param_type  ppp_dev_config_param;
} ps_ppp_dev_config_param_log_pkt_type;

/*===========================================================================

                      EXPORTED FUNCTION DEFINITIONS

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
  ppp_config_buffer_s_type  *configbuf_ptr
);

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
  device        - the interface over which the packet is passing

  This function constructs the appropriate log code from the ppp_data and
  iface parameters.

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
extern void ppp_log_frames_ext
(
  dsm_item_type                  *item_ptr,
  const pppi_log_flavour_e_type  flavour,
  ppp_dev_enum_type              device
);  
#endif /*FEATURE_DATA_PS_PPP_LOGGING*/

#ifdef FEATURE_DATA_PS_PPP_LOGGING
/*===========================================================================
  MACRO PPP_LOG_FRAMES_EXT

  Wrapper to ppp_log_frames_ext - checks log_status before calling

    Calculate the log code

    The log code is the base log code (275) for the flavour of the logging
    (rx/tx, framed/unframed) plus the interface.  The log codes are
    arranged such that there are 16 codes for each flavour to represent
    up to 16 interfaces.

    NOTE: no range checking is done so incoming parameters MUST be correct.

PARAMETERS
  p - pointer to the dsm chain to be logged
  f - The characteristics of this packet, i.e.  framed/unframed, rx/tx
  d - the interface over which the packet is passing

  See ppp_log_frames_ext description above for more info.
===========================================================================*/
#define PPP_LOG_FRAMES_EXT(p,f,d) \
          if( log_status( (uint16)(LOG_PPP_FRAMES_RESERVED_CODES_BASE_C + \
                                   ((uint8)f * 16) + (uint8)d) ) ) \
          { ppp_log_frames_ext( p, f, d ); }


#endif /* FEATURE_DATA_PS_PPP_LOGGING */

#endif /* PS_PPP_LOGGING_H */
