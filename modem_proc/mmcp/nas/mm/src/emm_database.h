
/*!
  @file
  emm_database.h

  @brief
  EMM internal data structure definitions.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_database.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
05/19/10   zren    Added support for GSTK EMM REGISTERED EVT and NW REJ EVT
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/07/10   zren    Added temp FPLMN feature
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/22/10   RI      Updated the NAS secure exchange flag in the macros.
12/18/09   RI      Added a flag to indicate storage of EPS info in USIM
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/04/09   MNK     Preserve the security context upon i-RAT & Cleanup emm_ctrl_data as necessary
10/20/09   RI      Added AMF information.
10/05/09   hnam    Added support for GUTI in EMM STATE log packet
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/05/09   zren    Updated code to log EMM state after EMM state is set
07/29/09   zren    Added support to use global cell_identity
07/21/09   zren    Added support to set high priority flag in RRC_CONN_EST_REQ
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
06/13/09   RI      Added support for NAS security context.
06/02/09   zren    Added SEMAPHORE to protect access_barring_flag
05/29/09   zren    Added support for TAU feature
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/02/09   zren    Added support for new trnsaction ID format 
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Added support for service request and paging feature
02/26/09   vdr     Added EMM_SYS_BAND_MASK_LTE_ALL_BANDS macro
02/23/09   vdr     Fixed Lint errors & warnings
01/28/09   vdr     Changed definition of the access_class field from byte to word
01/22/09   vdr     Fixed EMM_MOVES_TO_ABORTING_RRC_CONNECTION_STATE() macro
04/14/09   RI      Added LTE security parameters
===========================================================================*/

#ifndef _EMM_DATABASE_H_
#define _EMM_DATABASE_H_

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "sys.h"
#include "sys_plmn_selection_v.h"
#include "emm_connection_type.h"
#include "emm_state.h"
#include "lte_nas_emm_message_description.h"
#include "lte_nas_esm_message_type.h"
#include "reg_mm_v.h"
#include "mm_v.h"
#include "pthread.h"
#include "lte_nas_log.h"
#include "emm_security.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

extern boolean is_nas_emm_attach_tau_timeout;
extern emm_mo_detach_type nas_emm_power_off_detach_type;

#define EMM_MAX_NUMBER_OF_CLIENTS      1
#define EMM_MAX_MESSAGE_LENGTH         2048

#define EMM_FIRST_MESSAGE              0

/*===========================================================================

FUNCTION    EMM_SET_CONNECTION_STATE

DESCRIPTION
  Sets Connection state to the passed value to this function.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_SET_CONNECTION_STATE
(
  emm_connection_state_type emm_connection_state
);
/*===========================================================================

FUNCTION    EMM_MOVES_TO_IDLE_STATE

DESCRIPTION
  Sets Connection State to IDLE

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_MOVES_TO_IDLE_STATE
(
  void
);
/*===========================================================================

FUNCTION    EMM_MOVES_TO_WAITING_FOR_RRC_CONFIRMATION_STATE

DESCRIPTION
  Sets Connection state to 'EMM_WAITING_FOR_RRC_CONFIRMATION_STATE'.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void  EMM_MOVES_TO_WAITING_FOR_RRC_CONFIRMATION_STATE
(
  void
);
/*===========================================================================

FUNCTION    EMM_MOVES_TO_CONNECTED_STATE_STATE

DESCRIPTION
  Sets Connection state to 'EMM_CONNECTED_STATE'.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void  EMM_MOVES_TO_CONNECTED_STATE_STATE
(
  void 
) ;
/*===========================================================================

FUNCTION    EMM_MOVES_TO_RELEASING_RRC_CONNECTION_STATE

DESCRIPTION
  Sets Connection state to EMM_MOVES_TO_RELEASING_RRC_CONNECTION_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void  EMM_MOVES_TO_RELEASING_RRC_CONNECTION_STATE
(
  void
) ;
/*===========================================================================

FUNCTION    EMM_GET_CONNECTION_STATE

DESCRIPTION
  Returns the current connection state.

DEPENDENCIES
  None

RETURN VALUE
  Current connection state

SIDE EFFECTS
  None
===========================================================================*/

emm_connection_state_type  EMM_GET_CONNECTION_STATE
(
  void
) ;
/*===========================================================================

FUNCTION    EMM_SET_STATE

DESCRIPTION
  Sets EMM state to the passed value to this function.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_SET_STATE
(
  emm_state_type emm_state
) ;
/*===========================================================================

FUNCTION    EMM_SET_SUBSTATE

DESCRIPTION
  Sets EMM Substate state to the passed value to this function.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_SET_SUBSTATE
(
  emm_substate_type emm_substate
) ;
/*===========================================================================

FUNCTION    EMM_MOVES_TO_NULL_STATE

DESCRIPTION
  Sets EMM State to EMM_NULL

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void  EMM_MOVES_TO_NULL_STATE
(
  void
);
/*===========================================================================

FUNCTION    EMM_MOVES_TO_DEREGISTERED_STATE

DESCRIPTION
  Sets EMM State to EMM_DEREGISTERED

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_MOVES_TO_DEREGISTERED_STATE
(
  emm_substate_type emm_substate
);
/*===========================================================================

FUNCTION    EMM_MOVES_TO_REGISTERED_INITIATED_STATE

DESCRIPTION
  Sets EMM State to EMM_REGISTERED_INITIATED

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_MOVES_TO_REGISTERED_INITIATED_STATE
(
  emm_substate_type emm_substate
) ;
/*===========================================================================

FUNCTION    EMM_MOVES_TO_REGISTERED_STATE

DESCRIPTION
  Sets EMM State to EMM_REGISTERED

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_MOVES_TO_REGISTERED_STATE
(
  emm_substate_type emm_substate
) ;
/*===========================================================================

FUNCTION    EMM_REGISTERED_CHANGE_SUB_STATE

DESCRIPTION
 In EMM_REGISTERED state, this function changes the substate to the passed value.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_REGISTERED_CHANGE_SUB_STATE
(
  emm_substate_type emm_substate
); 
/*===========================================================================

FUNCTION    EMM_MOVES_TO_TRACKING_AREA_UPDATING_INITIATED_STATE

DESCRIPTION
 Sets EMM State to EMM_TRACKING_AREA_UPDATING_INITIATED.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void EMM_MOVES_TO_TRACKING_AREA_UPDATING_INITIATED_STATE
( 
  void 
); 
/*===========================================================================

FUNCTION    EMM_MOVES_TO_SERVICE_REQUEST_INITIATED_STATE

DESCRIPTION
  Sets EMM state to EMM_SERVICE_REQUEST_INITIATED.

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void  EMM_MOVES_TO_SERVICE_REQUEST_INITIATED_STATE
(
  void
) ;
/*===========================================================================

FUNCTION    EMM_MOVES_TO_DEREGISTERED_INITIATED_STATE

DESCRIPTION
  Sets EMM state to EMM_DEREGISTERED_INITIATED

DEPENDENCIES
  None

RETURN VALUE
  None*

SIDE EFFECTS
  None
===========================================================================*/

void  EMM_MOVES_TO_DEREGISTERED_INITIATED_STATE
(
  void
) ;
/*===========================================================================

FUNCTION    EMM_GET_STATE

DESCRIPTION
  Returns current EMM State.

DEPENDENCIES
  None

RETURN VALUE
  current EMM state

SIDE EFFECTS
  None
===========================================================================*/ 

emm_state_type EMM_GET_STATE
(
  void
);
/*===========================================================================

FUNCTION    EMM_GET_SUBSTATE

DESCRIPTION
  Returns current EMM substate.

DEPENDENCIES
  None

RETURN VALUE
  current EMM substate

SIDE EFFECTS
  None
===========================================================================*/

emm_substate_type EMM_GET_SUBSTATE
(
  void
) ;

#define EMM_SYS_BAND_MASK_LTE_ALL_BANDS   (sys_band_mask_type)( SYS_BAND_MASK_LTE_BAND1  |\
                                                                SYS_BAND_MASK_LTE_BAND2  |\
                                                                SYS_BAND_MASK_LTE_BAND3  |\
                                                                SYS_BAND_MASK_LTE_BAND4  |\
                                                                SYS_BAND_MASK_LTE_BAND5  |\
                                                                SYS_BAND_MASK_LTE_BAND6  |\
                                                                SYS_BAND_MASK_LTE_BAND7  |\
                                                                SYS_BAND_MASK_LTE_BAND8  |\
                                                                SYS_BAND_MASK_LTE_BAND9  |\
                                                                SYS_BAND_MASK_LTE_BAND10 |\
                                                                SYS_BAND_MASK_LTE_BAND11 |\
                                                                SYS_BAND_MASK_LTE_BAND12 |\
                                                                SYS_BAND_MASK_LTE_BAND13 |\
                                                                SYS_BAND_MASK_LTE_BAND14 |\
                                                                SYS_BAND_MASK_LTE_BAND17 |\
                                                                SYS_BAND_MASK_LTE_BAND33 |\
                                                                SYS_BAND_MASK_LTE_BAND34 |\
                                                                SYS_BAND_MASK_LTE_BAND35 |\
                                                                SYS_BAND_MASK_LTE_BAND36 |\
                                                                SYS_BAND_MASK_LTE_BAND37 |\
                                                                SYS_BAND_MASK_LTE_BAND38 |\
                                                                SYS_BAND_MASK_LTE_BAND39 |\
                                                                SYS_BAND_MASK_LTE_BAND40 )


typedef enum
{
  EMM_EMERGENCY_BARRED               = 1 ,
  EMM_MT_ACCESS_BARRED               = 4 ,
  EMM_MO_SIGNALING_BARRED            = 8 ,
  EMM_MO_DATA_BARRED                 = 16 ,
  EMM_MO_CSFB_BARRED                 = 32,
  EMM_DELAY_TOLERANT_BARRED          = 64
} emm_access_barring_mask_type ;

#define EMM_EAB_BARRING_BITMASK  (dword)( EMM_MO_SIGNALING_BARRED   |\
                                          EMM_MO_DATA_BARRED        |\
                                          EMM_MO_CSFB_BARRED        |\
                                          EMM_DELAY_TOLERANT_BARRED )

#define  EMM_ACCESS_CLASS_11    0x800
#define  EMM_ACCESS_CLASS_12    0x1000
#define  EMM_ACCESS_CLASS_13    0x2000
#define  EMM_ACCESS_CLASS_14    0x4000
#define  EMM_ACCESS_CLASS_15    0x8000

#define TOTAL_NUMBER_OF_CALL_TYPES 7

/* Invalid ESM transaction ID for PDN Connectivity Req message */
#define EMM_INVALID_PDN_CONN_REQ_TRANS_ID      0

/* Invalid ESM_SERVICE_REQ transaction ID */
#define EMM_INVALID_ESM_SRV_REQ_TRANS_ID      0

/* Temp forbidden PLMN list feature */
#define EMM_TEMP_FPLMN_DISABLED    0
#define EMM_TEMP_FPLMN_USE_T3402   0xFFFFFFFF

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

typedef
void (*emm_reg_service_report_type)
(
  mmr_cause_e_type    mmr_status,
  boolean             only_for_reg,
  struct emm_ctrl_data_tag *emm_ctrl_data_ptr
) ; /* end of reg_service_report_type() */ 

/*===========================================================================

                        ADDITIONAL MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/
extern emm_ctrl_data_type  *emm_ctrl_data_ptr;


extern emm_reg_service_report_type reg_service_report ; 

extern void emm_db_alloc_memory(void);
 extern emm_ctrl_data_type *emm_db_get_ctrl_data_per_subs(mm_as_id_e_type as_id);
emm_ctrl_data_type *emm_db_get_ctrl_data(void);

#endif /* _EMM_DATABASE_H_ */

#endif /* FEATURE_LTE */

