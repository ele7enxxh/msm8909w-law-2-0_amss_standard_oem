#ifndef CBTASK_H
#define CBTASK_H
/*===========================================================================

                          CB TASK HEADER FILE

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cbtask.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
#include "cb_task_v.h"
#ifdef FEATURE_WCDMA_ETWS
#include "ms_timer_v.h"
#endif


#define CB_CMD_Q_SIG         0x0001 /* Command queue signal               */

#define CB_DOGRPT_TIMER_SIG  0x0002 /* Watchdog report signal             */

#ifdef FEATURE_UMTS_BMC
#define CB_RLC_CMD_Q_SIG     0x0004
#endif

#define PLMN_VALUE_LENGTH    3
#define LAC_VALUE_LENGTH     2
#define CELL_ID_VALUE_LENGTH 4

typedef enum cb_rat_tag
{
  CB_GSM_MODE,
  CB_UMTS_MODE
} cb_rat_type ;

typedef struct cb_command_tag
{
   q_link_type           link;

   cb_command_body_type  command;

} cb_command_type ;
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)

extern byte     search_list_len_sim [MAX_AS_IDS];
extern byte     current_plmn_sim[MAX_AS_IDS][PLMN_VALUE_LENGTH];
extern byte     current_lac_sim[MAX_AS_IDS][LAC_VALUE_LENGTH];
extern byte     current_cell_id_sim[MAX_AS_IDS][CELL_ID_VALUE_LENGTH]; 
extern cb_wms_message_id_value_type search_lst_sim[MAX_AS_IDS][CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED];

#define  search_list_len search_list_len_sim[cb_as_id]
#define  cb_current_plmn current_plmn_sim[cb_as_id]
#define  current_lac current_lac_sim[cb_as_id]
#define  current_cell_id current_cell_id_sim[cb_as_id]
#define  search_lst search_lst_sim[cb_as_id]

#else
/* Search List Length */
extern byte  search_list_len ; /* Search List is EMPTY upon Power Up */

/* Pointer to requested Search List (list of message IDs) */
extern cb_wms_message_id_value_type search_lst[] ;

extern byte cb_current_plmn[] ;
   
extern byte current_lac[] ;

extern byte current_cell_id[] ;

#endif //#ifdef FEATURE_NAS_CBS_DSDS


#ifdef FEATURE_TRIPLE_SIM
#define IS_NOT_VALID_AS_ID(x) ((x < SYS_MODEM_AS_ID_1) || (x > SYS_MODEM_AS_ID_3))
#elif defined(FEATURE_DUAL_SIM)
#define IS_NOT_VALID_AS_ID(x) ((x != SYS_MODEM_AS_ID_1) && (x != SYS_MODEM_AS_ID_2))
#endif

/*===========================================================================

FUNCTION CB_SEND_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cb_send_cmd(cb_command_type *cmd_ptr) ;

/*===========================================================================

FUNCTION CB_BMC_VERIFY_CB_MAIN_Q

DESCRIPTION
  This function reads the main CB queue and process the commands in main 
control queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_UMTS_BMC
void cb_bmc_verify_cb_main_q(void);
#endif


#endif /* #ifdef FEATURE_GSM_CB */

#endif /* #ifndef CB_TASK_H */
