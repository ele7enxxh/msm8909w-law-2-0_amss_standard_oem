#ifndef QCHATUPK_DATA1X
#define QCHATUPK_DATA1X
/*===========================================================================
                    QCHAT PORTING KIT DATA1X
                           DEFINITIONS

DESCRIPTION
  This file contains the declarations for functions within the DATA Layer of
  the QChat universal porting kit.

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

QUALCOMM does not grant to you in this Agreement any rights in the OEM 
QChat Porting Kit including the accompanying associated documentation 
(the Software) for any purpose whatsoever, including without limitation 
any rights to copy, use or distribute the Software. Your rights to use the 
Software shall be only as set forth in any separate license between you and 
QUALCOMM which grants to you a license in the Software, and such license  
shall be under the terms and for the limited purposes set forth in that  
agreement.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qchat/api/data1x_qchatupk.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   srf     Created.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "ds707_sdb.h"
#include "sys.h"
                    
/*===========================================================================
                    EXTERNAL DECLARATIONS 
===========================================================================*/
/*===========================================================================

FUNCTION QCHATUPK_DS707_EPZID_HYST_DORMANT_SET_SCI

DESCRIPTION
  QChat optimization for the DS707 Hysteresis engine.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void qchatupk_ds707_epzid_hyst_dormant_set_sci( void );

/*===========================================================================

FUNCTION QCHATUPK_CHECK_FLAGS

DESCRIPTION
  QChat optimization for the DS707 Short Data Burst engine.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean qchatupk_check_flags( uint32 flags );

/*===========================================================================

FUNCTION QCHATUPK_GET_FLAGS

DESCRIPTION
  QChat optimization for the DS707 Short Data Burst engine.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void qchatupk_get_flags( uint32* flags );

/*===========================================================================

FUNCTION QCHATUPK_DS707_SDB_SET_RADIO_TRANSPORT

DESCRIPTION
  QChat optimization for the DS707 short data burst radio transport 
  protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Returns radio transport type preferred by QChat

SIDE EFFECTS
  None.

===========================================================================*/
extern ds707_radio_transport_in_dorm_type qchatupk_ds707_sdb_set_radio_transport
(
  ds707_radio_transport_in_dorm_type radio_transport, 
  uint32 item_ptr_length 
);

/*===========================================================================

FUNCTION QCHATUPK_HANDLE_MATI_FROM_IP_PORT 

DESCRIPTION
  QChat optimization for handling of MATI.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void qchatupk_handle_mati_from_ip_port ( int ip_address, int port_number );


/*===========================================================================

FUNCTION QCHATUPK_DOS_SETUP_CM_FLAGS

DESCRIPTION
  This function processes packet meta info flags and sets up call manager
  flags.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if QChat flags were set, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_dos_setup_cm_flags( uint32 *input_flags );

/*===========================================================================

FUNCTION QCHATUPK_DS707_SDB_MDM_RX_HANDLER 

DESCRIPTION
  QChat optimization for DS707 short data burst modem side support.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void qchatupk_ds707_sdb_mdm_rx_handler
(
  uint8 *dsm_payload_ptr,
  uint32 rlp_flow,
  byte   route
);

/*===========================================================================

FUNCTION QCHATUPK_DSS_SENDTO_FLAGS

DESCRIPTION
  QChat optimization for dss_sendto().

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void qchatupk_dss_sendto_flags( uint32* ds_flags );

/*===========================================================================

FUNCTION QCHATUPK_UPDATE_LOCATION

DESCRIPTION
  QChat optimization for updating location information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void qchatupk_update_location(sys_sys_mode_e_type tech_type);

/*===========================================================================

FUNCTION qchatupk_override_meta_info_flags

DESCRIPTION
  Override meta_info flags in ds707_pkt_tx_data for QChat if needed

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean qchatupk_override_meta_info_flags(ps_flow_type *ps_flow_ptr, 
                                          dsm_item_type **dsm_item_ptr_ptr, 
                                          void *ps_meta_info_ptr);

/*===========================================================================

FUNCTION  qchatupk_inform_fa_change()

Description: Notify QChat when there is FA change.

Parameters: None

Return Value: None

Comments: None

Version: NONE

==============================================================================*/
void qchatupk_inform_fa_change(void);

#endif /*QCHATUPK_DATA1X*/

