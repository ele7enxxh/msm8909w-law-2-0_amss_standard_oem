#ifndef HDRPAC_API_H
#define HDRPAC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

          PACKET APPLICATION CONTROLLER DATA SERVICES EXTERNAL FILE

GENERAL DESCRIPTION
  This file contains functionality that needs interaction between HDR Call
  Processing and Data Services in a dual processor environment.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/hdrpac_api.h#1 $

when       who    what, where, why
--------   ---    -----------------------------------------------------------
11/18/11   vpk    HDR SU API cleanup
09/08/11   ash    Added support for eHRPD to HRPD fallback.
06/20/11   ps     Added hdrpac_get_sn_ppp_status_from_nv
04/12/11   sn     Removed internal header files.
01/31/11   ttv    Removed feature wrapping in api files.
09/04/09   ss     New file created for CMI SU level split.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsm.h"
#include "hdrcom_api.h"

/*===========================================================================

                        FUNCTION DEFINITIONS            

===========================================================================*/

/*===========================================================================
FUNCTION      HDRPAC_MDM_SN_INIT

DESCRIPTION   Initializes RLP for HDR SN Default Packet App. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpac_mdm_sn_init( void );

/*===========================================================================
FUNCTION      HDRPAC_MDM_SN_CLEANUP

DESCRIPTION   Cleans up the HDR RLP.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpac_mdm_sn_cleanup( void );

/*===========================================================================
FUNCTION  HDRPAC_MDM_POWERUP_INIT

DESCRIPTION
  This function does the initializations for the PAC on power up.

DEPENDENCIES
  Needs to be called after hdrstream initialization.
  Init for DS.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_mdm_powerup_init( void );

/*===========================================================================
FUNCTION HDRPAC_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-use instance.
 
  in-use messages are routed further based on the particular APP that is 
  bound to the stream on which message was received and specific protocol 
  (FCP, RLP or LUP) for which this message is meant for, in that 
  application.
  
DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================
FUNCTION HDRPAC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAPP1, HDRAPP2, HDRAPP3. Indications given 
  to PAC are processed in DS task context.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_ind_cb
(
  hdrcom_ind_name_enum_type ind_name,
  void *ind_data_ptr
);
/*===========================================================================
FUNCTION HDRPAC_GET_RTCMAC_FLOW

DESCRIPTION
  For a given stream id and sub stream id gets the RTCMAC flow id.
Substream id will be the RLP id in case of Default and Multiflow RLP.

DEPENDENCIES
  None

PARAMETERS
  stream_id
  substream_id

RETURN VALUE
  RTCMAC flow id.

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrpac_get_rtcmac_flow
(
  hdrcom_hai_stream_enum_type stream_id,
  uint8 substream_id 
);

/*===========================================================================
FUNCTION      HDRPACDSIF_POST_SMP_SESSION_CLOSED_CMD_TO_DS

DESCRIPTION   Sends a command to DS task that session closed indication was
              received by PAC.

DEPENDENCIES  None. 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_post_smp_session_closed_cmd_to_ds
( 
  void 
);

/*===========================================================================
FUNCTION      HDRPACDSIF_CLEAR_SN_PPP_STATUS

DEPENDENCIES  Needs to run in the ds task context and should be invoked only
              at power up. Clearing this NV Item later on might result in a
              state when the NV Item is not indicating the actual PPP state.
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_clear_sn_ppp_status
(
  void
);
/*===========================================================================
FUNCTION      HDRPACDSIF_IS_SN_PPP_UP

DESCRIPTION   This function provides an interface to query the SN PPP status.
              
DEPENDENCIES  None.

RETURN VALUE  
  TRUE  - SN PPP is up (hdrpacdsif registered for IFace DOWN event)
  FALSE - SN PPP is down (hdrpacdsif registered for IFace UP event)

SIDE EFFECTS  None.
===========================================================================*/
boolean hdrpacdsif_is_sn_ppp_up( void );
/*===========================================================================
FUNCTION HDRPAC_GET_SN_PPP_STATUS_FROM_NV

DESCRIPTION
  This function reads the SN PPP status from NV. It return a boolean PPP
  status. 

DEPENDENCIES
  This function should be invoked only from hdrmc context. 

PARAMETERS

RETURN VALUE
  boolean SN PPP status

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrpac_get_sn_ppp_status_from_nv
(
  void
);

/*===========================================================================
FUNCTION      HDRPACDSIF_GET_DATA_EHRPD_CAPABILITY

DESCRIPTION   This function provides an interface to query if eHRPD 
              capability is allowed or not.
              
DEPENDENCIES  None.

RETURN VALUE  
  TRUE  - eHRPD is allowed
  FALSE - eHRPD is not allowed

SIDE EFFECTS  None.
===========================================================================*/
boolean hdrpacdsif_get_data_ehrpd_capability( void );

/*===========================================================================
FUNCTION HDRPAC_APP_FLOW_IS_ENABLED

DESCRIPTION
  This function returns whether the stream for an RLP instance is in
  Open or Closed state.

  The difference between this and hdrpac_flow_is_enabled is in the parameter
  it takes as input. hdrpac_flow_is_enabled takes RLP ID, whereas this
  takes the app subtype. If the app_subtype is specified as PacketAppAny,
  then the function returns true if any of the streams have flow enabled.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If the stream corresponding to the RLP instance is Open
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrpac_app_flow_is_enabled
(
  hdrcom_hai_app_subtype_enum_type app_subtype
);

/*===========================================================================
FUNCTION HDRPAC_FLOW_ENABLE

DESCRIPTION
  This function enables flow on the current stream by commanding the Flow
  Control Protocol to send an Xon message for that stream.

DEPENDENCIES
  None

PARAMETERS
  app_subtype - The Application Subtype of the stream on which flow is to be
                enabled
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_flow_enable
(
  hdrcom_hai_app_subtype_enum_type app_subtype 
);

/*===========================================================================
FUNCTION HDRPAC_FLOW_DISABLE

DESCRIPTION
  This function disables flow on the current stream by commanding the Flow
  Control Protocol to send a Xoff message for that stream.

DEPENDENCIES
  None

PARAMETERS
  app_subtype - The Application Subtype of the stream on which flow is to be
                disabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_flow_disable
(
  hdrcom_hai_app_subtype_enum_type app_subtype 
);
#endif /* HDRPAC_API_H */
