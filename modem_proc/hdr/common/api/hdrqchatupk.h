#ifndef _HDRQCHATUPK_H
#define _HDRQCHATUPK_H
/*===========================================================================
                    QCHAT PORTING KIT HDR
                           DEFINITIONS

DESCRIPTION
  This file contains the declarations for functions within the HDR Layer of
  the QChat universal porting kit.

Copyright (c) 2000-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrqchatupk.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/12   srn     Updated hdrqchatupk_amac_process_isr_sig() 
05/27/09   srf     Created.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrerrno.h"
#include "hdrmc.h"
#include "hdrmc_v.h"
                    
/*===========================================================================
                    EXTERNAL DECLARATIONS 
===========================================================================*/

/*===========================================================================

FUNCTION HDRQCHATUPK_AMAC_SET_ANI_CONN

DESCRIPTION
  QChat optimization for the HDR Access Channel MAC protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_amac_set_ani_conn(boolean enable);


/*===========================================================================

FUNCTION HDRQCHATUPK_PROCESS_ACM_MSG

DESCRIPTION
  QChat optimization for the ACM message handling.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_process_acm_msg(dsm_item_type *msg_ptr);


/*===========================================================================

FUNCTION HDRQCHATUPK_AMAC_PROCESS_ACCESS_ATTEMPT

DESCRIPTION
  QChat optimization for the HDR Access Channel MAC protocol
  access attempt.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_amac_process_access_attempt (void);


/*===========================================================================

FUNCTION HDRQCHATUPK_AMPATI_HANDLE_HDRHAI_MATI

DESCRIPTION
  QChat optimization for the HDR Address Management Protocol Access Terminal
  Identifier.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization procedure was successfully completed, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrqchatupk_ampati_handle_hdrhai_mati( uint32 ati_value );


/*===========================================================================

FUNCTION HDRQCHATUPK_BCCPREG_PROCESS_REG_LIST

DESCRIPTION
  QChat optimization for the HDR Broadcast Registration.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_bccpreg_process_reg_list(int ip_address, int port_number);


/*===========================================================================

FUNCTION HDRQCHATUPK_BCFRAME_PROCESS_ANNOUNCE_FLOW_DISABLE

DESCRIPTION
  QChat optimization for the HDR Broadcast Framing Protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization procedure was successfully completed, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrqchatupk_bcframe_process_announce_flow_disable(uint32 flow_id);


/*===========================================================================

FUNCTION HDRQCHATUPK_BCFRAME_INIT

DESCRIPTION
  QChat optimization for HDR Broadcast Framing Protocol init procedures.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_bcframe_init(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_BCFRAME_PROCESS_ANNOUNCE_FLOW_ENABLE

DESCRIPTION
  QChat optimization for the HDR Broadcast Framing Protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization procedure was successfully completed, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrqchatupk_bcframe_process_announce_flow_enable
(
  uint32 flow_id,
  void (*data_cb)(dsm_item_type *ip_pkt)
);


/*===========================================================================

FUNCTION HDRQCHATUPK_CMAC_PUT_PKT

DESCRIPTION
  QChat optimization for the HDR Control Channel MAC protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_cmac_put_pkt( hdrhai_ati_enum_type ati_type, uint32 ati_value ); 


/*===========================================================================

FUNCTION HDRQCHATUPK_CLOSE_CONNECTION

DESCRIPTION
  QChat optimization for  HDR connection management.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_close_connection(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_DOS_TX_OUTCOME_CB

DESCRIPTION
  QChat optimization for HDR Data Over Signalling Protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_dos_tx_outcome_cb
(
  void                 *outcome_data_ptr,
  hdrerrno_enum_type    status
);


/*===========================================================================

FUNCTION HDRQCHATUPK_HMPSA_START_PROCEDURE

DESCRIPTION
  QChat optimization for the HDR Messaging Protocol access attempt procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_hmpsa_start_procedure( void );


/*===========================================================================

FUNCTION HDRQCHATUPK_IDLE_PROCESS_CMD

DESCRIPTION
  QChat optimization for HDR Idle State Protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_idle_process_cmd( hdrcp_qchat_opt_req_msg_t *cmd_ptr );


/*===========================================================================

FUNCTION HDRQCHATUPK_IDLE_POWERUP_INIT

DESCRIPTION
  QChat optimization for HDR Idle State Protocol init procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_idle_powerup_init( void );


/*===========================================================================

FUNCTION HDRQCHATUPK_IDLE_PROCESS_QCHAT_TIMER

DESCRIPTION
  QChat optimization for HDR Idle State Protocol QChat timer processing.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_idle_process_qchat_timer( void );


/*===========================================================================

FUNCTION HDRQCHATUPK_IDLE_CHECK_SLEEP

DESCRIPTION
  QChat optimization for HDR Idle State Protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_idle_check_sleep( boolean *sleep_ok );


/*===========================================================================

FUNCTION HDRQCHATUPK_MC_HANDLE_IMMINENT_ORIG

DESCRIPTION
  QChat optimization for HDR Main Controller.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_mc_handle_imminent_orig( void );


/*===========================================================================

FUNCTION HDRQCHATUPK_HANDLE_DATA_OVER_SIGNALLING_MSG_ID

DESCRIPTION
  QChat optimization for HDR Packet Application Controller Data Services
  to handle data over signalling message id.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrqchatupk_handle_data_over_signalling_msg_id (void);


/*===========================================================================

FUNCTION HDRQCHATUPK_PAC_MDM_HANDLE_CASE

DESCRIPTION
  QChat optimization for HDR Packet Application Controller Data Services.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization procedure was successfully completed, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrqchatupk_pac_mdm_handle_case(uint16 msg_type, dsm_item_type* msg_ptr);


/*===========================================================================

FUNCTION HDRQCHATUPK_DOS_PROCESS_HDRMC_CMD

DESCRIPTION
  This function processes QChat optimizations received by HDRMC

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrqchatupk_dos_process_hdrmc_cmd( hdrcp_send_data_over_signaling_req_msg_t *mo_dos );


/*===========================================================================

FUNCTION HDRQCHATUPK_PROCESS_SEND_MSG

DESCRIPTION
  QChat optimization for HDR Packet Messaging Protocol.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrqchatupk_process_send_msg( rex_timer_type *dos_waiting_timer );


/*===========================================================================
FUNCTION HDRQCHATUPK_IGNORE_DOS_SEQNO

DESCRIPTION
  This function returns QChat optimization related to DOS seqno.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization is in effect, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrqchatupk_ignore_dos_seqno( void );

/*===========================================================================

FUNCTION HDRQCHATUPK_DOS_SETUP_DS_FLAGS

DESCRIPTION
  QChat optimization for DS Flags.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrqchatupk_dos_setup_ds_flags
(
  uint32 *input_flags, 
  uint32 *ds_flags
);

/*===========================================================================

FUNCTION HDRQCHATUPK_AMAC_PROCESS_ISR_SIG

DESCRIPTION
  QChat optimization for processing isr sig.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrqchatupk_amac_process_isr_sig(void);

#endif /*_HDRQCHATUPK_H*/


