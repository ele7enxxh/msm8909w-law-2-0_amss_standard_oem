#ifndef _DS_QMI_SVC_EXT_H
#define _DS_QMI_SVC_EXT_H
/*===========================================================================

                         D S _ Q M I _ S V C _ E X T . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2004-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmi_svc_ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
09/18/11    wc     Nikel and MPSS.NI.1.1 port configuration
05/02/11    ua     Enabling Autoconnect to working with first instance across
                   all targets. 
02/09/11    ua     Added support for LTE Data rates.
10/23/09    ar     Added IS_SERVICE_VALID() test marco.
10/22/09    kk     Windows7 compliance - WDS changes and BER/PER/FER support.
12/12/06    ks     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsm.h"

#ifdef FEATURE_DATA_QMI

#include "sys.h"
#include "qmi_svc_defs.h"

/*---------------------------------------------------------------------------
  NOTE - Any constants or function prototypes in this file should NOT be
         changed by the customer
---------------------------------------------------------------------------*/


/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Vendor specific services need to use these constants to declare the common
  members of their service state and client state respectively. 

  ********************   DO NOT CHANGE THESE VALUES   *********************
---------------------------------------------------------------------------*/
#define QMI_COMMON_SVC_STATE_SIZE     (sizeof(qmi_common_svc_state_type))
#define QMI_COMMON_CLIENT_STATE_SIZE  (sizeof(qmi_common_client_state_type))

/*===========================================================================

                               DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Multiple QMI links may exist between the MSM   and a tethered device(s).  
  Each link is identified by the Um interface it is intended to control 
  (i.e. bound to).

  NOTE: This enum should be in line with the enum 'rmnet_instance_e_type'.
        Editing this enum independently of the other enum can cause boot-up
        crashes.
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_INSTANCE_MIN     = 0, 
  QMI_INSTANCE_RMNET1  = QMI_INSTANCE_MIN,
  QMI_INSTANCE_SHIM    = QMI_INSTANCE_MIN,
  QMI_INSTANCE_RMNET5  = 4,
  QMI_INSTANCE_MAX
} qmi_instance_e_type;

/*---------------------------------------------------------------------------
  Enum for the channel type supported.
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CHANNEL_SUPP_MIN      = 0,
  QMI_CHANNEL_SUPP_CDMA     = 1,
  QMI_CHANNEL_SUPP_HDR_REV0 = 2,
  QMI_CHANNEL_SUPP_GPRS     = 3,
  QMI_CHANNEL_SUPP_EDGE     = 4,
  QMI_CHANNEL_SUPP_WCDMA    = 5,
  QMI_CHANNEL_SUPP_LTE      = 6,
  QMI_CHANNEL_SUPP_HDR_REVA = 7,
  QMI_CHANNEL_SUPP_TDSCDMA  = 8,
  QMI_CHANNEL_SUPP_MAX
} qmi_channel_supp_e_type;

typedef struct qmi_channel_info_s
{
  qmi_channel_supp_e_type supp_channel;

  /* extended channel type information */
  union
  {
    /* GPRS/EDGE */
    struct
    {
      uint8 multislot_class;
    } gprs_edge;
    
    /* HSDPA/HSUPA */
    struct
    {
      uint8 hsdpa_cat;
      uint8 hsupa_cat;
    } wcdma;

    /* HDR */
    struct
    {
      sys_active_prot_e_type active_protcol;
    } hdr;
  } extend;
} qmi_channel_info_s_type;

extern char * qmi_device_mfr_str;

/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
extern void qmi_svc_ext_init
(
  qmi_instance_e_type i
);


/*===========================================================================
  FUNCTION QMI_SVC_COMMON_SVC_POWERUP_INIT()

  DESCRIPTION
   Powerup initialization of QMI SVC module. Services need not call this
   method explicitly.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_common_svc_powerup_init
(
  void
);


/*===========================================================================
  FUNCTION QMI_SVC_COMMON_SVC_DEINIT()

  DESCRIPTION
    De-initialization of QMI SVC module. 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_common_svc_deinit
(
  void
);


/*===========================================================================
  FUNCTION QMI_SVC_COMMON_SVC_INIT()

  DESCRIPTION
    Common Service initialization

  PARAMETERS
    sp          :  service provided state pointer (user data)
    qmux_svc_handle :  qmux service handle 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_common_svc_init
(
  void *   sp,
  void *   qmux_svc_handle
);

/*===========================================================================
  FUNCTION QMI_SVC_COMMON_CL_INIT()

  DESCRIPTION
    Common client initialization

  PARAMETERS
    sp :  service provided state pointer (user data)
    client_sp  :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_common_cl_init
(
  void * sp,
  void * client_sp
);

/*===========================================================================
  FUNCTION QMI_SVC_COMMON_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    ptr to response
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_common_reset_client
(
  void * cl_sp_in
);

/*===========================================================================
  FUNCTION QMI_SVC_SEND_RESPONSE()

  DESCRIPTION
    Send the message to the QMUX layer

  PARAMETERS
    sp    : service provided state pointer (user data)
    cmd_buf_p : cmd_buf pointer for the command that the response is for
    msg_ptr   : response to send out

  RETURN VALUE
    TRUE if command was sent (or is queued in transaction for later send)
    FALSE if something was bad

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_send_response
(
  void *           sp,
  void *           cmd_buf_p_in,
  dsm_item_type *  msg_ptr
);

/*===========================================================================
  FUNCTION QMI_SVC_SEND_IND()

  DESCRIPTION
    Sends Indications. For broadcast indications, clid = 255
    
  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    cmd_type      : cmd type
    ind           : indication to send out

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_send_ind   
(
  void *           sp,
  uint8            clid,
  uint16           cmd_type,
  dsm_item_type *  ind
);

/*===========================================================================
  FUNCTION qmi_svc_get_clid_from_client_sp()

  DESCRIPTION
    Retrieve clid from the client state pointer

  PARAMETERS
    cl_sp  :  client state pointer

  RETURN VALUE
    clid   :  client id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern uint8 qmi_svc_get_clid_from_client_sp
(
  void * cl_sp
);

/*===========================================================================
  FUNCTION QMI_SVC_GET_CLIENT_SP_BY_CMD_BUF()

  DESCRIPTION
    Retrieve client sp from the command buffer

  PARAMETERS
    cmd_buf_p_in  :  command buffer

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void * qmi_svc_get_client_sp_by_cmd_buf
(
  void * cmd_buf_p_in
);

/*===========================================================================
  FUNCTION QMI_SVC_EXT_GET_DEFAULT_CHANNEL_RATES()

  DESCRIPTION
    Get the current serving system's default MAX data channel rate values
    given the channel type supported.
    These rates are reported by QMI as a fallback mechanism when 
    not in a call or for cases when lower layers do not return the correct 
    current rates

  PARAMETERS
    supp_channel : Type of channel supported
    tx_rate      : Tx rate to be reported
    rx_rate      : Rx rate to be reported

  RETURN VALUE
    none

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_ext_get_default_channel_rates
( 
  qmi_channel_info_s_type supp_channel_info,
  uint32 *                tx_rate, 
  uint32 *                rx_rate 
);

/*===========================================================================
  FUNCTION QMI_SVC_EXT_ALLOW_AUTO_CONNECT()

  DESCRIPTION
    Determines whether an auto-connect request should be allowed to be serviced

  PARAMETERS
    none

  RETURN VALUE
    TRUE  - request allowed
    FALSE - request denied

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_ext_allow_auto_connect
( 
  void
);
#endif /* FEATURE_DATA_QMI */
#endif /* _DS_QMI_SVC_EXT_H */
