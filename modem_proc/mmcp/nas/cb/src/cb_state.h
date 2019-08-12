#ifndef CB_STATE_H
#define CB_STATE_H
/*===========================================================================

               CB TASK STATE DEFINITION HEADER FILE (CB_STATE.H)

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_state.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include "comdef.h"
#include "cb_task.h"
#include "cbtask.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef enum cb_state_id_tag
{
  CB_IDLE_STATE ,
  CB_ACTIVE_STATE
} cb_state_id_type ;

typedef enum cb_idle_substate_id_tag
{
  CB_INACTIVE_SUBSTATE ,
  CB_CBCH_DETECTED_SUBSTATE ,
  CB_COLLECTION_INITIATED_SUBSTATE
#ifdef FEATURE_NAS_CBS_DSDS
  ,CB_ACTIVATION_PENDING_SUBSTATE
#endif
} cb_idle_substate_id_type ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
typedef enum cb_etws_idle_substate_id_tag
{
  CB_ETWS_IDLE_INACTIVE,
  CB_ETWS_IDLE_COLLECTION_INITIATED,
  CB_ETWS_IDLE_WAITING_FOR_CTCH
} cb_etws_idle_substate_id_type ;

typedef struct cb_etws_state_tag
{
  cb_state_id_type              state ;
  cb_etws_idle_substate_id_type idle_substate ;
} cb_etws_state_type ;

#endif
typedef enum cb_active_substate_id_tag
{
  CB_ACTIVE_NULL,
  CB_ACTIVE_NON_DRX,
  CB_ACTIVE_DRX,
  CB_ACTIVE_NON_DRX_PERIODICITY,
  CB_ACTIVE_PSEUDO_DRX
} cb_active_substate_id_type ;

typedef struct cb_state_tag
{
  cb_state_id_type         state ;
  cb_idle_substate_id_type idle_substate ;
} cb_state_type ;

extern void cb_main_state_machine_handler
(
  cb_command_type *cb_incoming_primitive
) ;

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
void cb_etws_state_control(
  cb_state_id_type next_etws_state,
  cb_state_type    next_cb_state
);
#endif

/*
  Defining local AS ID,The incoming SUB ID in each
  message will be converted to below SUB ID type,
  so that we have only SUB IDs. 
*/

typedef enum {
  /**< Subscription ID 1 */
  CB_AS_ID_1 = 0,
  /**< Subscription ID 2 */    
  CB_AS_ID_2      
} cb_as_id_e_type;



#ifdef FEATURE_UMTS_BMC
extern void bmc_process_dsm_data_cb
(
   byte rlc_id,
   byte no_pdus,
   void *instance
);

/*===========================================================================

FUNCTION    BMC_PARSE_RLC_PARAMS

DESCRIPTION
  This function parse the RLC params from the app_ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean bmc_parse_rlc_params
(
  dsm_item_type *msg_ptr,
  boolean *start_pdu_ptr,
  byte *no_li_len_ptr,
  dword *received_sfn_ptr,
  dword *received_end_sfn_ptr
);

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_RLC_PDU

DESCRIPTION
  This function processes incoming RLC PDU

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_process_rlc_pdu
(
  dword    received_sfn,
  dword    received_end_sfn,
  byte     li_length,
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  boolean  first_pdu_indicator,
#endif
  word     pdu_length,
  byte    *rlc_pdu_ptr
);

#ifdef FEATURE_WCDMA_ETWS
/*===========================================================================

FUNCTION    CB_BMC_PROCESS_ETWS_PRIM_NOTIFICATION_IND

DESCRIPTION
  This function processes incoming primary notification ind

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_process_etws_prim_notification_ind
(
  const rrc_etws_prim_notification_ind_type* rrc_etws_prim_notification_ind,
  const cb_state_type* cur_state 
);
#endif
#endif

#endif /* #ifdef FEATURE_GSM_CB */

#endif /* #ifndef CB_STATE_H */
