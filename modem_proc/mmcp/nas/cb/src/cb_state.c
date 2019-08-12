/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     CB STATE MACHINE (CB_STATE.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_state.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/22/12   jbk     Read CB NV items irrespective of BMC feature flag 
08/09/11   ss      Update the rat info even though CTCH is false.
08/03/11   ss      Process the CB command queue after updating the cb_state global context. 
01/21/08   ab      Removed compiler warning
04/15/06   kvk     Renamed cb_free and cb_alloc to cb_mem_free and cb_mem_alloc 
                   to make it compatible with L4 builds
08/03/05   kvk     Added debug print to print SFN stamped with every PDU.
07/25/05   kvk     Fixed global varibale PDU counter getting reset while processing
                   RLC data indication.
06/14/05   kvk     Removed cell id stamping on each PDU. Now RLC PDUs received 
                   on the old cell will be completely discarded when Cell change happens.
06/06/05   kvk     Added recover mechanism for BMC in case rlc pdu counter is
                   getting incremented indefinetely and CB task is starving for
                   a long time and resources will be held up by other high priority tasks.
                   BMC will be temporarily suspended till RLC queue is cleaned up completely
                   and then starts reading the Downlink messages.
05/25/05   kvk     If RLC PDU is received in IDLE state then trash the PDU without
                   processing it.
04/19/05   kvk     Added flipping of octets in RLC PDU before processing.
03/30/05   kvk     Added discarding outdated PDUs received on the old cell.
03/29/05   kvk     Now First PDU is detected based on the Start SFN in case of
                   DRX mode.
03/28/05   kvk     Added support for UMTS BMC.
08/02/04   vdr     Added processing of the CB_DELETE_PAGE_REQ
07/20/04   vdr     Changed Cell Change indication processing in order to 
                   distinguish between NO SERVICE an Cell which does not support 
                   CBCH 
04/14/04   vdr     Added processing of the L1 Missing Block indication
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
07/03/03   jca     Initial version

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include <string.h>
#include <stringl/stringl.h>
#include "comdef.h"
#include "cb_state.h"
#include "cb_task.h"
#include "cbtask.h"
#include "gs_v.h"
#include "msg.h"
#include "err.h"
#include "cb_schedule.h"
#include "cbmsg.h"
#include "cb_rr.h"
#include "cb_l2.h"
#ifdef FEATURE_UMTS_BMC
#include "rlcbmcif.h"
#include "rrcbmcif.h"
#include "sys.h"
#include "sys_v.h"
#endif/*FEATURE_UMTS_BMC*/
#include "ms.h"
#include "cb_utils.h"
#include "cb_mem.h"

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean mm_dual_standby_mode;
#endif

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
cb_state_type cb_state_sim[MAX_AS_IDS] ;
#define cb_state cb_state_sim[cb_as_id]

cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];
#define   bmc_active_rat bmc_active_rat_sim[cb_as_id]



static cb_active_substate_id_type cb_active_substate_sim[MAX_AS_IDS];
#define   cb_active_substate cb_active_substate_sim[cb_as_id]

boolean cb_supported_sim[MAX_AS_IDS] ;
#define cb_supported cb_supported_sim[cb_as_id]

#ifdef FEATURE_NO_CBS_LTD 
static boolean cb_service_sim[MAX_AS_IDS];
#define   cb_service cb_service_sim[cb_as_id]
#endif/*FEATURE_NO_CBS_LTD*/

#else

cb_state_type cb_state  ;

cb_rat_type bmc_active_rat ;

static cb_active_substate_id_type cb_active_substate;

boolean cb_supported ;

#ifdef FEATURE_NO_CBS_LTD 
static boolean cb_service      = FALSE;
#endif/*FEATURE_NO_CBS_LTD*/

#endif


#define CB_ACTION_0     0
#define CB_ACTION_1     1
#define CB_ACTION_2     2
#define CB_ACTION_3     3
#define CB_ACTION_4     4
#define CB_ACTION_5     5

typedef struct substate_event_tag
{
   cb_active_substate_id_type        current_substate;
   cb_active_substate_id_type        next_substate;
   cb_event_T                        event;
   byte                              action_code;
} substate_event_T;


static substate_event_T cb_substate_event_table[] =
{
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NULL,               CB_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NON_DRX,            CB_STD_NON_DRX_ACTIVATED,    CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_DRX,                CB_STD_DRX_ACTIVATED,        CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NULL,               CB_STD_DEACTIVATED,          CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NON_DRX_PERIODICITY,CB_STD_NON_DRX_P_ACTIVATED,  CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_PSEUDO_DRX,         CB_STD_PSEUDO_DRX_ACTIVATED, CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NULL,               CB_STD_DEACTIVATED,          CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NULL,               CB_STD_RLC_PDU_DROPPED,      CB_ACTION_0  },
   {  CB_ACTIVE_NULL,    CB_ACTIVE_NULL,               CB_STD_DATA_PAGE_RECD,       CB_ACTION_0  },

   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_NON_DRX,            CB_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_NON_DRX,            CB_STD_NON_DRX_ACTIVATED,    CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_DRX,                CB_STD_DRX_ACTIVATED,        CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_NON_DRX_PERIODICITY,CB_STD_NON_DRX_P_ACTIVATED,  CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_NULL,               CB_STD_DEACTIVATED,          CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_PSEUDO_DRX,         CB_STD_PSEUDO_DRX_ACTIVATED, CB_ACTION_4  }, 
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_NON_DRX,            CB_STD_RLC_PDU_DROPPED,      CB_ACTION_2  },
   {  CB_ACTIVE_NON_DRX, CB_ACTIVE_NON_DRX,            CB_STD_DATA_PAGE_RECD,       CB_ACTION_0  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_DRX,                CB_STD_PSEUDO_DRX_ACTIVATED, CB_ACTION_0  },

   {  CB_ACTIVE_DRX,     CB_ACTIVE_DRX,                CB_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_NON_DRX,            CB_STD_NON_DRX_ACTIVATED,    CB_ACTION_3  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_DRX,                CB_STD_DRX_ACTIVATED,        CB_ACTION_0  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_NON_DRX_PERIODICITY,CB_STD_NON_DRX_P_ACTIVATED,  CB_ACTION_0  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_NULL,               CB_STD_DEACTIVATED,          CB_ACTION_3  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_DRX,                CB_STD_RLC_PDU_DROPPED,      CB_ACTION_2  },
   {  CB_ACTIVE_DRX,     CB_ACTIVE_DRX,                CB_STD_DATA_PAGE_RECD,       CB_ACTION_0  },

   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_NON_DRX_PERIODICITY, CB_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_NON_DRX,             CB_STD_NON_DRX_ACTIVATED,    CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_DRX,                 CB_STD_DRX_ACTIVATED,        CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_NON_DRX_PERIODICITY, CB_STD_NON_DRX_P_ACTIVATED,  CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_NULL,                CB_STD_DEACTIVATED,          CB_ACTION_0  },
   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_NON_DRX_PERIODICITY, CB_STD_RLC_PDU_DROPPED,      CB_ACTION_1  },
   {  CB_ACTIVE_NON_DRX_PERIODICITY, CB_ACTIVE_NON_DRX_PERIODICITY, CB_STD_DATA_PAGE_RECD,       CB_ACTION_1  },
   {  CB_ACTIVE_PSEUDO_DRX, CB_ACTIVE_PSEUDO_DRX, CB_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ACTIVE_PSEUDO_DRX, CB_ACTIVE_NON_DRX,    CB_STD_NON_DRX_ACTIVATED,    CB_ACTION_0  },
   {  CB_ACTIVE_PSEUDO_DRX, CB_ACTIVE_DRX,        CB_STD_DRX_ACTIVATED,        CB_ACTION_5  },
   {  CB_ACTIVE_PSEUDO_DRX, CB_ACTIVE_PSEUDO_DRX, CB_STD_PSEUDO_DRX_ACTIVATED, CB_ACTION_4  },

   {  CB_ACTIVE_NULL,    CB_ACTIVE_NULL,               CB_STD_INVALID_EVENT,        CB_ACTION_0  }

};

#if defined( FEATURE_WCDMA_ETWS) && defined (FEATURE_UMTS_BMC)

typedef struct etws_substate_event_tag
{
   cb_etws_idle_substate_id_type     current_substate;
   cb_etws_idle_substate_id_type     next_substate;
   cb_event_T                        event;
   byte                              action_code;
} etws_substate_event_T;

static cb_etws_idle_substate_id_type cb_etws_idle_substate;

static etws_substate_event_T cb_etws_idle_substate_event_table[] =
{
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_NULL_EVENT,           CB_ACTION_0  },

   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_COLLECTION_INITIATED,   CB_ETWS_STD_ETWS_MESSAGE_IDS_ENABLED,    CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_PRIMARY_NOTIFICATION_RECD,        CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_CTCH_TIMER_EXPIRED,        CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_CTCH_SUPPORTED,  CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_ETWS_MESSAGE_IDS_DISABLED,          CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_CTCH_DEACTIVATED,      CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_DRX_ACTIVATED,       CB_ACTION_0  },
   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_NDRX_ACTIVATED_AFTR_L1_SCHD_EXPIRY,      CB_ACTION_0  },

   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,            CB_ETWS_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,            CB_ETWS_STD_ETWS_MESSAGE_IDS_ENABLED,    CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_WAITING_FOR_CTCH,                CB_ETWS_STD_PRIMARY_NOTIFICATION_RECD,        CB_ACTION_1  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,    CB_ETWS_STD_CTCH_TIMER_EXPIRED, CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,CB_ETWS_STD_CTCH_SUPPORTED,  CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_ETWS_MESSAGE_IDS_DISABLED,          CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,            CB_ETWS_STD_CTCH_DEACTIVATED,      CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,            CB_ETWS_STD_DRX_ACTIVATED,   CB_ACTION_0  },
   {  CB_ETWS_IDLE_COLLECTION_INITIATED, CB_ETWS_IDLE_COLLECTION_INITIATED,            CB_ETWS_STD_NDRX_ACTIVATED_AFTR_L1_SCHD_EXPIRY,   CB_ACTION_0  },

   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,                CB_ETWS_STD_NULL_EVENT,           CB_ACTION_0  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,            CB_ETWS_STD_ETWS_MESSAGE_IDS_ENABLED,    CB_ACTION_0  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,                CB_ETWS_STD_PRIMARY_NOTIFICATION_RECD,        CB_ACTION_0  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_COLLECTION_INITIATED,    CB_ETWS_STD_CTCH_TIMER_EXPIRED, CB_ACTION_0  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,CB_ETWS_STD_CTCH_SUPPORTED,  CB_ACTION_3  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_INACTIVE,               CB_ETWS_STD_ETWS_MESSAGE_IDS_DISABLED,          CB_ACTION_4  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,                CB_ETWS_STD_CTCH_DEACTIVATED,      CB_ACTION_0  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,    CB_ETWS_STD_DRX_ACTIVATED,   CB_ACTION_0  },
   {  CB_ETWS_IDLE_WAITING_FOR_CTCH,     CB_ETWS_IDLE_WAITING_FOR_CTCH,    CB_ETWS_STD_NDRX_ACTIVATED_AFTR_L1_SCHD_EXPIRY,   CB_ACTION_0  },

   {  CB_ETWS_IDLE_INACTIVE,    CB_ETWS_IDLE_INACTIVE,               CB_STD_INVALID_EVENT,        CB_ACTION_0  }

};
static cb_etws_state_type cb_etws_state = {CB_IDLE_STATE, CB_ETWS_IDLE_INACTIVE };
static boolean cb_etws_ctch_drx_timer_running = FALSE;

static cb_state_id_type cb_etws_idle_substate_control
(
  cb_event_T cb_event
);
cb_etws_substate_control_type cb_etws_substate_control = cb_etws_idle_substate_control;

#endif


/* Periodicity of BMC Schedule Message */
word cb_bmc_schedule_msg_periodicity = 0;

#ifdef FEATURE_UMTS_BMC
boolean start_pdu_recd_flag = FALSE;
#endif/*FEATURE_UMTS_BMC*/



#ifdef FEATURE_MODEM_HEAP
extern void cb_free_dynamic_pool_NDRX(void);
extern void cb_free_skip_pool_if_needed(void);

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
extern void cb_free_dynamic_pool_DRX(void);
#endif
#endif

#ifdef FEATURE_DUAL_SIM
cb_as_id_e_type cb_as_id  = CB_AS_ID_1;
#endif //#ifdef FEATURE_DUAL_SIM
boolean cb_multisim_standby = FALSE;
extern void cb_process_mode_change_indication(byte val);

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
sys_modem_as_id_e_type bmc_as_id = SYS_MODEM_AS_ID_NONE;
extern dword cb_dup_timer_value_sim[MAX_AS_IDS];
#define   cb_dup_timer_value cb_dup_timer_value_sim[cb_as_id]
#else
extern dword cb_dup_timer_value;
#endif //FEATURE_NAS_CBS_DSDS

extern boolean cb_gsm_feature_disabled;

extern void cb_ctch_pdu_decode_error(dword pdu_sfn);

byte                cb_mcc_JAPAN1[3] = {0x04,0x04,0x00};
byte                cb_mcc_JAPAN2[3] = {0x04,0x04,0x01};
extern dword cb_calculate_block_fn(dword reference_fn,byte  block_number);
extern dword cb_calculate_slot_fn(dword reference_fn,byte  slot_number);
extern void cb_ctch_pdu_dropped(dword pdu_sfn);
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
/*===========================================================================

FUNCTION CB_ETWS_IDLE_SUBSTATE_CONTROL

DESCRIPTION
  This function updates the ETWS substate and based on the events
  send the new ETWS state value to cb_etws_state_control function

DEPENDENCIES
  None

RETURN VALUE
  Next ETWS state 

SIDE EFFECTS
  None

===========================================================================*/

static cb_state_id_type cb_etws_idle_substate_control
(
  cb_event_T cb_event
)
{
   /* Set action to a non-existing action as a default*/
   byte                         action        = 0xFF;
   boolean                      entry_found   = FALSE;
   byte                         entry         = 0;
   cb_state_id_type             next_state = cb_etws_state.state;
   cb_etws_idle_substate_id_type   next_substate = cb_etws_idle_substate;
   byte           table_size = (sizeof(cb_etws_idle_substate_event_table)/sizeof(substate_event_T));/*Klocwork fix*/


/*
 * FOR all entries in table
 */

   while (cb_etws_idle_substate_event_table[entry].event != CB_STD_INVALID_EVENT)
   {
      if ( cb_etws_idle_substate_event_table[entry].current_substate == cb_etws_idle_substate )
      {
         if (cb_etws_idle_substate_event_table[entry].event ==  cb_event )
         {
            next_substate = cb_etws_idle_substate_event_table[entry].next_substate;

            action = cb_etws_idle_substate_event_table[entry].action_code;
            entry_found = TRUE;
            break;
         }
      }
      entry++;
      if(entry >= table_size) /*Klocwork fix to ensure array is accessed with in the bounds*/
      {
         MSG_ERROR_DS_0(cb_as_id,"Index out of bounds and buffer overflow");
         break;
      }
   }

   if ((entry_found) &&
       ((next_substate != cb_etws_idle_substate) ||
        (action != CB_ACTION_0)))
   {
      MSG_HIGH_DS_3(cb_as_id, "CB ETWS IDLE substate change from %d to %d, action:%d", cb_etws_idle_substate, next_substate, action );

      cb_etws_idle_substate = next_substate;

      switch (action)
      {
         case CB_ACTION_0:      /* NULL action, do nothing, go to next_substate */
            break;

         case CB_ACTION_1:     /* waiting for CTCH activation */
#ifdef FEATURE_NO_CBS_LTD
           if (cb_supported && cb_service)
#else
           if (cb_supported)
#endif
           {
             /* start disable CTCH timer with 2 mins time out*/
             if (cb_timer_status[CB_ETWS_PRIM_DISABLE_CTCH_TIMER-1] == TIMER_STOPPED)
             {
              cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE);
             }
             next_state = CB_ACTIVE_STATE;
           }
           else
           {
             /* start disable CTCH timer with 2 mins time out*/
             if (cb_timer_status[CB_ETWS_PRIM_DISABLE_CTCH_TIMER-1] == TIMER_STOPPED)
             {
               cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE);
             }
             next_state = CB_IDLE_STATE;
           }
           break;

         case CB_ACTION_2:     /* moving to active */
           /* start disable CTCH timer with 2 mins time out*/
           if (cb_timer_status[CB_ETWS_PRIM_DISABLE_CTCH_TIMER-1] == TIMER_STOPPED)
           {
             cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE);
           }
           next_state = CB_ACTIVE_STATE;
           break;

         case CB_ACTION_3:     /* restart timer and activate CTCH if CB is supported */
#ifdef FEATURE_NO_CBS_LTD
           if (cb_supported && cb_service)
#else
           if (cb_supported)
#endif
           {
             cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
             if (cb_timer_status[CB_ETWS_PRIM_DISABLE_CTCH_TIMER-1] == TIMER_STOPPED)
             {
               cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE);
             }
             next_state = CB_ACTIVE_STATE;
           }
           else
           {
             next_state = CB_IDLE_STATE;
             cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
           }
           break;

         case CB_ACTION_4:     /* stop CTCH timer */
           cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
           break;

      }
   }

  return next_state;
}

/*===========================================================================

FUNCTION CB_ETWS_ACTIVE_SUBSTATE_CONTROL

DESCRIPTION
  This function updates the ETWS substate and based on the events
  send the new ETWS state value to cb_etws_state_control function

DEPENDENCIES
  None

RETURN VALUE
  Next ETWS state 

SIDE EFFECTS
  None

===========================================================================*/

static cb_state_id_type cb_etws_active_substate_control
(
  cb_event_T cb_event
)
{
  cb_state_id_type             next_state = cb_etws_state.state;

  MSG_HIGH_DS_1(cb_as_id, "CB ETWS ACTIVE event:%d", cb_event);

  switch (cb_event)
  {
     case CB_ETWS_STD_PRIMARY_NOTIFICATION_RECD: /* restart the timer a new primary has arrived */
        cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
        if(cb_etws_ctch_drx_timer_running)
        {
          cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_DRX_TIMEOUT_VALUE);
          cb_etws_ctch_drx_timer_running = TRUE;
        }
        {
          cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE);
        }
        break;

     case CB_ETWS_STD_ETWS_MESSAGE_IDS_DISABLED: /* idle substate control will move back to inactive */
        cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
        next_state = CB_IDLE_STATE;
       break;

     case CB_ETWS_STD_CTCH_DEACTIVATED:     /* dont stop the timer as CTCH may resume  */
        next_state = CB_IDLE_STATE;
       break;

     case CB_ETWS_STD_CTCH_TIMER_EXPIRED:   /* idle substate control will move back to collection init */
        next_state = CB_IDLE_STATE;
       break;

     case CB_ETWS_STD_DRX_ACTIVATED:
        /* 
        ** CB is active for ETWS and moved DRX mode 
        ** Need to restart the ETWS PRIM DISABLE CTCH TIMER with 30 mins time out
        */
        if(!cb_etws_ctch_drx_timer_running)
        {
          cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
          cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_DRX_TIMEOUT_VALUE);
          cb_etws_ctch_drx_timer_running = TRUE;
        }
        else
        {
          MSG_HIGH_DS_0(cb_as_id,"CB ETWS DRX timer is not started as its already running ");
        }
       break;

     case CB_ETWS_STD_NDRX_ACTIVATED_AFTR_L1_SCHD_EXPIRY:
        /* 
        ** CB is active for ETWS and moving from DRX mode to Non DRX mode 
        ** lets not restart ETWS PRIM DISABLE CTCH TIMER with 2 mins time out
        ** since NW is in DRX mode and UE will probably move to DRX soon
        if((cb_timer_status[CB_ETWS_PRIM_DISABLE_CTCH_TIMER-1] == TIMER_ACTIVE))
        {
          cb_stop_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER);
          cb_start_etws_timer(CB_ETWS_PRIM_DISABLE_CTCH_TIMER,CB_ETWS_PRIM_DISABLE_CTCH_NDRX_TIMEOUT_VALUE);
          cb_etws_ctch_drx_timer_running = FALSE;
        }
        */
       break;
     default :
       break;
  }

  if(next_state != cb_etws_state.state)
  {
    /* ignore the state returned as state is already changed */
    (void) cb_etws_idle_substate_control(cb_event);
  }
  return next_state;
}

/*===========================================================================

FUNCTION CB_ETWS_STATE_CONTROL

DESCRIPTION
  This function updates ETWS new state
  Perfroms activation and deactivation of  lower layers if is not already taken care
  in the BMC state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_etws_state_control(
  cb_state_id_type next_etws_state,
  cb_state_type    next_cb_state
)
{
  boolean etws_activate = (cb_etws_state.state == CB_IDLE_STATE) &&
                          (next_etws_state == CB_ACTIVE_STATE);
  boolean etws_deactivate = (cb_etws_state.state == CB_ACTIVE_STATE) &&
                            (next_etws_state == CB_IDLE_STATE);

  if(etws_activate &&
     (next_cb_state.state != CB_ACTIVE_STATE))
  {
    cb_start_non_drx_mode(bmc_active_rat) ;
  }
  else if(etws_deactivate &&
          ((cb_state.state != CB_ACTIVE_STATE) &&
             (next_cb_state.state != CB_ACTIVE_STATE)))
  {
    
    if (bmc_active_rat == CB_UMTS_MODE && bmc_rx_suspended)
    {
      MSG_HIGH_DS_0(cb_as_id,"BMC: BMC is already suspended no need to deactivate the WL1");
      bmc_rx_suspended = FALSE;
    }
    else
    {
      cb_deactivate_bmc_monitoring(bmc_active_rat) ;
    }
  }


  if((cb_etws_ctch_drx_timer_running) && 
     (next_etws_state == CB_IDLE_STATE))
  {
    /* 
    **Moving to idle state and if the flag is set to TRUE
    **mark it as FALSE
    */
    cb_etws_ctch_drx_timer_running = FALSE;
  }

  if(cb_etws_state.state != next_etws_state)
  {
	MSG_HIGH_DS_2(cb_as_id,"CB ETWS state change from %d to %d",cb_etws_state.state,next_etws_state);
    if(next_etws_state == CB_IDLE_STATE)
    {
      cb_etws_substate_control = cb_etws_idle_substate_control;
    }
    else
    {
      cb_etws_substate_control = cb_etws_active_substate_control;
    }
  }

  /* update new ETWS state here*/
  cb_etws_state.state = next_etws_state;
}
#endif

#ifdef FEATURE_NAS_CBS_DSDS
/*************************************************************
FUNCTION cb_get_as_id

DESCRIPTION
  This function will extract SIM_ID from all incoming primitives and enables  processing of  the primitive 
  in the context of extracted SIM_ID

DEPENDENCIES
  None

RETURN VALUE
  TRUE if AS ID is valid

SIDE EFFECTS
  None
 *************************************************************/
boolean cb_get_as_id(cb_command_type *cb_incoming_primitive)
{
  boolean status = TRUE;
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  switch (cb_incoming_primitive->command.message_header.message_set)
  {  
    case MS_WMS_CB :
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case CB_SEARCH_LIST_REQ :
          as_id = cb_incoming_primitive->command.search_list_req.as_id;
          break;
        case CB_DELETE_PAGE_REQ :
          as_id = cb_incoming_primitive->command.delete_page_req.as_id;
          break;
        case CB_DELETE_ALL_REQ :
          as_id = cb_incoming_primitive->command.delete_all_req.as_id;
          break;
        case CB_DUAL_STANDBY_STATUS_IND :
          return FALSE;
        default :
          MSG_FATAL_DS( cb_as_id,"=CB= Unexpected WMS_CB cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 );
      }
      break;
    case MS_CB_RR:
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case RR_CELL_CHANGE_IND :
          as_id = cb_incoming_primitive->command.cell_change_ind.as_id;
          break;
        default :
          MSG_FATAL_DS( cb_as_id,"=CB= Unexpected CB RR cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 );
      }
      break;
#ifdef FEATURE_UMTS_BMC
    case MS_RRC_BMC :  
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case RRC_CELL_CHANGE_IND :
          as_id = cb_incoming_primitive->command.rrc_cell_change_ind.as_id;
          break;
        case RRC_ETWS_PRIM_NOTIFICATION_IND:
          as_id =cb_incoming_primitive->command.rrc_etws_prim_notification_ind.as_id; 
          break;  
        default :
          MSG_FATAL_DS( cb_as_id,"=CB= Unexpected RRC BMC cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 );
      }
      break;

    case MS_RLC_BMC :
      /*The cell change indication should have already 
        received. So just use that as_id to handle this msg
      */
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
      as_id = bmc_as_id;
#else
      as_id = SYS_MODEM_AS_ID_NONE;
#endif
      break;
    case MS_BMC_L1  :
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case L1_BMC_SCHED_EXPIRY_IND :
          as_id = cb_incoming_primitive->command.l1_bmc_sched_expiry_ind.as_id;
          break;
        case L1_BMC_SCHEDULING_CNF :
          as_id = cb_incoming_primitive->command.l1_bmc_scheduling_cnf.as_id;
          break;
        case L1_BMC_SKIP_CNF:
          as_id = cb_incoming_primitive->command.l1_bmc_skip_cnf.as_id;
          break;
        case L1_BMC_DROPPED_PDU_IND:
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
          as_id = bmc_as_id;
#else
          as_id = SYS_MODEM_AS_ID_NONE;
#endif
          break;
        default :
          MSG_FATAL_DS( cb_as_id,"=CB= Unexpected L1_BMC cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 );
      }
      break;
#endif/*FEATURE_UMTS_BMC*/

    case MS_TIMER:
        switch(cb_incoming_primitive->command.message_header.message_id)
        {
          case MID_TIMER_EXPIRY:
            as_id=(sys_modem_as_id_e_type)((cb_incoming_primitive->command.timer_expiry.timer_id >> (CB_TIMER_ID_SIM_POS)) & 0x11);
            cb_incoming_primitive->command.timer_expiry.timer_id &=  ~(0x11 << (CB_TIMER_ID_SIM_POS));
            break;
          default:
            MSG_FATAL_DS( cb_as_id,"=CB= Unexpected Timer cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 );
        }
       break;

    case MS_CB_L2   : 
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case DL_CB_BLOCK_IND :
          as_id = cb_incoming_primitive->command.cb_block_ind.as_id;
          break;
        default :
          MSG_FATAL_DS( cb_as_id,"=CB= Unexpected CB_L2 cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 );
      }
      break;
    case MS_CB_L1   :
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case L1_CB_MISSING_BLOCK_IND :
          as_id = cb_incoming_primitive->command.cb_missing_block.as_id;
          break;
        case L1_CB_SKIP_CNF :
          as_id = cb_incoming_primitive->command.cb_skip_cnf.as_id;
          break;
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
        case L1_CB_SCHED_EXPIRY_IND :
          as_id = cb_incoming_primitive->command.cb_schedule_expired_ind.as_id;
          break;
        case L1_CB_SCHEDULING_CNF:
          as_id = cb_incoming_primitive->command.cb_scheduling_cnf.as_id;
          break;
#endif
        default :
          MSG_FATAL_DS( cb_as_id,"=CB= Unexpected CB_L1 cmd %d", cb_incoming_primitive->command.message_header.message_id, 0,0 ); 
      }
      break;
      case MS_REG_CB:
      switch(cb_incoming_primitive->command.message_header.message_id)
      {
        case REG_MODE_CHANGE_STATE_IND:
             return TRUE;  
        case REG_SERVICE_STATE_IND:
          as_id=cb_incoming_primitive->command.service_state_ind.as_id;
          break;
      }
      break;
    default :
      MSG_HIGH_DS_1(cb_as_id,"=CB= Unexpected message set %d", cb_incoming_primitive->command.message_header.message_set); 
      break;
  }

  if(IS_NOT_VALID_AS_ID(as_id))
  {
    MSG_HIGH_DS_3(cb_as_id,"=CB= Invalid AS ID:%d - Ignoring message MS: %d   MSG_ID: %d", 
            as_id,cb_incoming_primitive->command.message_header.message_set,
                     cb_incoming_primitive->command.message_header.message_id);
    status = FALSE;

    /* leave SUB ID with a valid value */
    cb_as_id = CB_AS_ID_1;
  }
  else
  {
    MSG_HIGH_DS_3(cb_as_id,"=CB= MS: %d   MSG_ID: %d AS ID: %d ", cb_incoming_primitive->command.message_header.message_set
                                        , cb_incoming_primitive->command.message_header.message_id, as_id);
    cb_as_id = (cb_as_id_e_type)as_id;
  }
  return status;
}
#endif //#ifdef FEATURE_NAS_CBS_DSDS

/*===========================================================================

FUNCTION CB_PROCESS_WMS_MESSAGE

DESCRIPTION
  This function processes incoming WMS Search List request

DEPENDENCIES
  None

RETURN VALUE
  Next CB state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_wms_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state;

  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  if (cb_incoming_primitive->command.message_header.message_id 
                                                         == (int)CB_SEARCH_LIST_REQ)
  {
    byte cb_search_list_length;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
    byte etws_msg_count=0;
#endif

    cb_bmc_reading_advised = 
               cb_incoming_primitive->command.search_list_req.reading_advised ;

    cb_bmc_reading_optional = 
              cb_incoming_primitive->command.search_list_req.reading_optional ;

    cb_process_wms_search_list(
                             &cb_incoming_primitive->command.search_list_req) ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
    etws_msg_count = number_of_etws_ids_in_search_list();
    cb_search_list_length = search_list_len - etws_msg_count;
#else
    cb_search_list_length = search_list_len;
#endif

#if defined (FEATURE_DUAL_SIM) && !defined(FEATURE_NAS_CBS_DSDS) 
    if (search_list_len != 0)
    {
      if(!IS_NOT_VALID_AS_ID(cb_incoming_primitive->command.search_list_req.as_id))
      {
        cb_as_id = (cb_as_id_e_type)cb_incoming_primitive->command.search_list_req.as_id;
      }
      else
      {
        MSG_ERROR_DS_1(cb_as_id,"=CB= Invalid SUB ID:%d in search list req", 
                   cb_incoming_primitive->command.search_list_req.as_id);
      }
    }
#endif
    switch (cur_state->state)
    {
      case CB_IDLE_STATE :

        switch (cur_state->idle_substate) 
        {
          case CB_INACTIVE_SUBSTATE :
            /* If Search List is empty we gotta stay were we are */
            if (cb_search_list_length == 0)
            {
              break;
            }

            new_state.idle_substate = CB_COLLECTION_INITIATED_SUBSTATE ;

            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into COLLECTION INITIATED substate") ;
            break ;
          case CB_CBCH_DETECTED_SUBSTATE :
            /* If Search List is empty we gotta stay were we are */
            if (cb_search_list_length == 0)
            {
              break;
            }
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
            if(mm_dual_standby_mode && (CB_UMTS_MODE == bmc_active_rat))
            {
              MSG_HIGH_DS_0(cb_as_id,"CB: Moved into ACTIVATION PENDING SUBSTATE") ;
              /*Dual standby mode is TRUE , so can't activate W
                Move to pending state, so that the info can be used
                later
              */
              new_state.idle_substate = CB_ACTIVATION_PENDING_SUBSTATE;
            }
            else
#endif //ifdef FEATURE_NAS_CBS_DSDS
            {
              new_state.state = CB_ACTIVE_STATE ;

              MSG_HIGH_DS_0(cb_as_id,"CB: Moved into ACTIVE state") ;

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
            /* Activate L1 if not already activated by ETWS*/
              if(cb_etws_state.state != CB_ACTIVE_STATE)
#endif
              {
                cb_start_non_drx_mode(bmc_active_rat) ;
              }
            }
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED

            /**********************************************/
            /*     PROCESS REMAINING SCHEDULING PERIOD    */
            /*                    IF ANY                  */
            /**********************************************/

#endif /* #ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED */
            break ;
          case CB_COLLECTION_INITIATED_SUBSTATE :
            if (cb_search_list_length == 0)
            {
              new_state.idle_substate = CB_INACTIVE_SUBSTATE ;

              MSG_HIGH_DS_0(cb_as_id,"CB: Moved into INACTIVE substate") ;
            }

            break ;
#ifdef FEATURE_NAS_CBS_DSDS
        /* We are in activation pending state, it means
           the CBCH detection and search list both are vailable
           Due to dual standby mode W is not active,In this
           case if search list comes as NULL, the move to
           detected substate
        */
        case CB_ACTIVATION_PENDING_SUBSTATE:
          if (cb_search_list_length == 0)
          {
            new_state.state = CB_IDLE_STATE ;

            new_state.idle_substate = CB_CBCH_DETECTED_SUBSTATE ;

            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into IDLE, CBCH DETECTED substate") ;
          }
          break;
#endif //ifdef FEATURE_NAS_CBS_DSDS
          default:
            MSG_FATAL_DS( cb_as_id,"Illigal CB IDLE substate ID",0,0,0) ;
        }

        break ;
      case CB_ACTIVE_STATE :
        if (cb_search_list_length == 0)
        {
          new_state.state = CB_IDLE_STATE ;

          new_state.idle_substate = CB_CBCH_DETECTED_SUBSTATE ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
          if(cb_timer_status[CB_ETWS_PRIM_DISABLE_CTCH_TIMER-1] != TIMER_ACTIVE  ||
                            etws_msg_count == 0)
#endif
          {
            /*
            ** send deactivation only when CTCH timer is not running
            ** other wise deactivation is from WMS State transition
            ** at timer expiry
            */
#ifdef FEATURE_UMTS_BMC
            if (bmc_active_rat == CB_UMTS_MODE && bmc_rx_suspended)
            {
              MSG_HIGH_DS_0(cb_as_id,"BMC: BMC is already suspended no need to deactivate the WL1");
              bmc_rx_suspended = FALSE;            
            }
            else
#endif
            {
              cb_deactivate_bmc_monitoring(bmc_active_rat) ;
            }
          }

          MSG_HIGH_DS_0(cb_as_id,"CB: Moved into IDLE, CBCH DETECTED substate") ;

        }

        break ;
      default:
        MSG_FATAL_DS( cb_as_id,"Illigal CB state ID",0,0,0) ;
    } /* switch (cur_state->state) */
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
    if((search_list_len !=0)  &&
       (etws_msg_count > 0))
    {
      cb_etws_state_control
      (
        cb_etws_substate_control(CB_ETWS_STD_ETWS_MESSAGE_IDS_ENABLED),
        new_state
      );
    }
    else
    {
      cb_etws_state_control
      (
        cb_etws_substate_control(CB_ETWS_STD_ETWS_MESSAGE_IDS_DISABLED),
        new_state
      );
    }
#endif

  }
  else if (cb_incoming_primitive->command.message_header.message_id 
                                                         == (int)CB_DELETE_PAGE_REQ)
  {

    MSG_HIGH_DS_0(cb_as_id,"CB: Received CB_DELETE_PAGE_REQ") ;

    cb_delete_required_page(
                  cb_incoming_primitive->command.delete_page_req.message_id,
                  cb_incoming_primitive->command.delete_page_req.message_code,
                  cb_incoming_primitive->command.delete_page_req.page_number) ;
  }
  else if (cb_incoming_primitive->command.message_header.message_id
                                                         == (int)CB_DELETE_ALL_REQ)  
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Received CB_DELETE_ALL_REQ") ;

    /* Delete all collected pages */
    cb_delete_all_pages() ;
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"Unexpected WMS message is received") ;
  }

  return new_state ;
} /* end cb_process_wms_message() */

/*===========================================================================

  
  FUNCTION CB_CBCH_SERVICE_STATE_HANDLER
  
  DESCRIPTION
    This function handles the state transitions based on service state and cb supported if FEATURE_NO_CBS_LTD
    is enabled and otherwise based on cb supported
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Next CB State
  
  SIDE EFFECTS
    None
  
  ===========================================================================*/
cb_state_type cb_cbch_service_state_handler
(
cb_state_type *cur_state,
cb_rat_type   current_rat
)
{
cb_state_type new_state = *cur_state ;
#ifdef FEATURE_NO_CBS_LTD
  switch (cur_state->state)
  {
    case CB_IDLE_STATE :
      switch (cur_state->idle_substate)
      {
        case CB_INACTIVE_SUBSTATE :
          if (cb_supported && cb_service)
          {
            new_state.idle_substate = CB_CBCH_DETECTED_SUBSTATE ;

            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into CBCH DETECTED substate") ;
          }
          break ;
        case CB_CBCH_DETECTED_SUBSTATE :
          if ((!cb_supported) || (!cb_service))
          {
            new_state.idle_substate = CB_INACTIVE_SUBSTATE ;

            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into INACTIVE substate") ;
          }
          break ;
        case CB_COLLECTION_INITIATED_SUBSTATE :
          if (cb_supported && cb_service)
          {
            new_state.state = CB_ACTIVE_STATE ;

            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into ACTIVE state") ;

            cb_start_non_drx_mode(current_rat) ;

          }
          break ;
        default:
          MSG_FATAL_DS( cb_as_id,"Illigal CB IDLE substate ID",0,0,0) ;
      }
      break ;
    case CB_ACTIVE_STATE :
      if ((!cb_supported) || (!cb_service))
      {
        new_state.state = CB_IDLE_STATE ;

        new_state.idle_substate = CB_COLLECTION_INITIATED_SUBSTATE ;

        MSG_HIGH_DS_0(cb_as_id,"CB: Moved into IDLE, COLLECTION INITIATED substate") ;
#ifdef FEATURE_UMTS_BMC
        if (current_rat == CB_UMTS_MODE && bmc_rx_suspended)
        {
          MSG_HIGH_DS_0(cb_as_id,"BMC: BMC is already suspended no need to deactivate the WL1");
          bmc_rx_suspended = FALSE;
        }
        else
#endif
        {
          cb_deactivate_bmc_monitoring(current_rat) ;
        }
      }
      else
      {
 #ifdef FEATURE_UMTS_BMC
        /*If BMC is already suspended resume it*/
        bmc_rx_suspended = FALSE;
#endif
        /*Start Non DRX mode of operation*/
        cb_start_non_drx_mode(current_rat) ;
        
      }
      break ;
    default:
      MSG_FATAL_DS( cb_as_id,"Illigal CB state ID",0,0,0) ;
  }

#else
  switch (cur_state->state)
  {
    case CB_IDLE_STATE :
      switch (cur_state->idle_substate)
       {
        case CB_INACTIVE_SUBSTATE :
          if (cb_supported)
          {
            new_state.idle_substate = CB_CBCH_DETECTED_SUBSTATE ;
            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into CBCH DETECTED substate") ;
          }
          break ;
        case CB_CBCH_DETECTED_SUBSTATE :
          if (!cb_supported)
          {
            new_state.idle_substate = CB_INACTIVE_SUBSTATE ;
            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into INACTIVE substate") ;
          }
        break ;
        case CB_COLLECTION_INITIATED_SUBSTATE :
          if (cb_supported)
          {
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
            /*If search list is already present and it's a dual 
              standby mode, then W is not allowed. So move to
              activation pending state
            */
            if(mm_dual_standby_mode && (CB_UMTS_MODE == bmc_active_rat))
            {
              MSG_HIGH_DS_0(cb_as_id,"CB: Moved into ACTIVATION PENDING SUBSTATE") ;
              new_state.idle_substate = CB_ACTIVATION_PENDING_SUBSTATE;
            }
            else
#endif //#ifdef FEATURE_NAS_CBS_DSDS
            {
              new_state.state = CB_ACTIVE_STATE ;

              MSG_HIGH_DS_0(cb_as_id,"CB: Moved into ACTIVE state") ;

              cb_start_non_drx_mode(current_rat) ;
            }
          }
          break ;
#ifdef FEATURE_NAS_CBS_DSDS
        case CB_ACTIVATION_PENDING_SUBSTATE:
          /* We are in activation pending state, it means
             the CBCH detection and search list both are vailable
             Due to dual standby mode, W is not active,In this
             case if CB support comes as falseL, the move to
             collection initiated substate
          */
          if (!cb_supported)
          {
            new_state.state = CB_IDLE_STATE ;

            new_state.idle_substate = CB_COLLECTION_INITIATED_SUBSTATE ;

            MSG_HIGH_DS_0(cb_as_id,"CB: Moved into IDLE, COLLECTION INITIATED substate") ;
          }
          break;
#endif //ifdef FEATURE_NAS_CBS_DSDS
        default:
          MSG_FATAL_DS( cb_as_id,"Illigal CB IDLE substate ID",0,0,0) ;
       }
      break ;
    case CB_ACTIVE_STATE :
      if (!cb_supported)
      {
        new_state.state = CB_IDLE_STATE ;
        new_state.idle_substate = CB_COLLECTION_INITIATED_SUBSTATE ;
        MSG_HIGH_DS_0(cb_as_id,"CB: Moved into IDLE, COLLECTION INITIATED substate") ;
#ifdef FEATURE_UMTS_BMC
        if (current_rat == CB_UMTS_MODE && bmc_rx_suspended)
        {
          MSG_HIGH_DS_0(cb_as_id,"BMC: BMC is already suspended no need to deactivate the WL1");
          bmc_rx_suspended = FALSE;
        }
        else
#endif
        {
          cb_deactivate_bmc_monitoring(current_rat) ;
        }
      }
      else
      {
#ifdef FEATURE_UMTS_BMC
        /*If BMC is already suspended resume it*/
        bmc_rx_suspended = FALSE;
#endif
        /*Start Non DRX mode of operation*/
        cb_start_non_drx_mode(current_rat) ;
      }
    break ;
    default:
      MSG_FATAL_DS( cb_as_id,"Illigal CB state ID",0,0,0) ;
  }
#endif
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
#ifdef FEATURE_NO_CBS_LTD
      if (cb_supported && cb_service)
#else
      if (cb_supported)
#endif
      {
        cb_etws_state_control
        (
          (cb_state_id_type)cb_etws_substate_control(CB_ETWS_STD_CTCH_SUPPORTED),
           new_state
        );
      }
      else
      {
        cb_etws_state_control
        (
          (cb_state_id_type)cb_etws_substate_control(CB_ETWS_STD_CTCH_DEACTIVATED),
          new_state
         );
      }

#endif

return new_state;
}


/*===========================================================================

FUNCTION CB_PROCESS_CELL_CHANGE_INDICATION

DESCRIPTION
  This function processes incoming RR Cell Indication

DEPENDENCIES
  None

RETURN VALUE
  Next CB state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_cell_change_indication
(
  boolean            delete_page_buffers,
  cb_rat_type        current_rat,
  byte              *incoming_plmn,
  byte              *incoming_lac,
  byte              *incoming_cell_id,
  cb_state_type   *cur_state
)
{
  byte etalon_cell_id[CELL_ID_VALUE_LENGTH] = {0x00, 0x00, 0x00, 0x00} ;
  boolean filter_out_of_date_messages = FALSE ;
  cb_state_type new_state ;
#ifdef FEATURE_UMTS_BMC  
  cb_rat_type rat_tobe_deleted ;
#endif /* #ifdef FEATURE_UMTS_BMC */  

  if(cur_state == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  new_state = *cur_state ;
  if (cb_supported)
  {

    if (memcmp((void *)cb_current_plmn,(void *)incoming_plmn, PLMN_VALUE_LENGTH) ||
        memcmp((void *)current_lac,(void *)incoming_lac, LAC_VALUE_LENGTH) ||
        memcmp((void *)current_cell_id,(void *)incoming_cell_id, CELL_ID_VALUE_LENGTH))
    {
      /* Copy current location */
      memscpy((void *)cb_current_plmn, PLMN_VALUE_LENGTH,(void *)incoming_plmn, PLMN_VALUE_LENGTH) ;
   
      memscpy((void *)current_lac, LAC_VALUE_LENGTH,(void *)incoming_lac, LAC_VALUE_LENGTH) ;

      memscpy((void *)current_cell_id, CELL_ID_VALUE_LENGTH,(void *)incoming_cell_id, CELL_ID_VALUE_LENGTH) ;

      /* Trigger message filtering */
      filter_out_of_date_messages = TRUE ;

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
      /* reset pseudo drx if there is a cell change */
      cb_pseudo_drx_start_fn = BMC_INVALID_SFN;
      cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
      cb_pseudo_drx_offset = 0;
#endif

    }

    MSG_HIGH_DS_3(cb_as_id,"BMC: PLMN %X %X %X",incoming_plmn[0],incoming_plmn[1],incoming_plmn[2]) ;

    MSG_HIGH_DS_2(cb_as_id,"BMC: LAC %X %X",incoming_lac[0],incoming_lac[1]) ;

    MSG_HIGH_DS_1(cb_as_id,"BMC: CELL ID %X",*(dword *)incoming_cell_id) ;

    bmc_active_rat = current_rat ;

    MSG_HIGH_DS_3(cb_as_id,"CB Service ID list:%d  ID-0:%d ID-1:%d", (search_list_len!=0),
                                                       cb_message_is_requested(0),
                                                       cb_message_is_requested(1));
  }
  else
  {

    if (memcmp((void *)incoming_cell_id,(void *)etalon_cell_id,CELL_ID_VALUE_LENGTH) == 0)
    {
      /* Invalid cell id represents NO SERVICE */
      filter_out_of_date_messages = FALSE ;
    }
    else
    {
      if (memcmp((void *)cb_current_plmn,(void *)incoming_plmn, PLMN_VALUE_LENGTH) ||
          memcmp((void *)current_lac,(void *)incoming_lac, LAC_VALUE_LENGTH) ||
          memcmp((void *)current_cell_id,(void *)incoming_cell_id, CELL_ID_VALUE_LENGTH))
      {
        /* Copy current location */
        memscpy((void *)cb_current_plmn, PLMN_VALUE_LENGTH,(void *)incoming_plmn, PLMN_VALUE_LENGTH) ;
   
        memscpy((void *)current_lac, LAC_VALUE_LENGTH,(void *)incoming_lac, LAC_VALUE_LENGTH) ;

        memscpy((void *)current_cell_id, CELL_ID_VALUE_LENGTH,(void *)incoming_cell_id, CELL_ID_VALUE_LENGTH) ;

        /* Trigger message filtering */
        filter_out_of_date_messages = TRUE ;
      }
#ifdef FEATURE_GSM_CB_PSEUDO_DRX
      /* reset pseudo drx if there is a cell change */
      cb_pseudo_drx_start_fn = BMC_INVALID_SFN;
      cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
      cb_pseudo_drx_offset = 0;
#endif
      MSG_HIGH_DS_3(cb_as_id,"BMC: PLMN %X %X %X",incoming_plmn[0],incoming_plmn[1],incoming_plmn[2]) ;

      MSG_HIGH_DS_2(cb_as_id,"BMC: LAC %X %X",incoming_lac[0],incoming_lac[1]) ;

      MSG_HIGH_DS_1(cb_as_id,"BMC: CELL ID %X",*(dword *)incoming_cell_id) ;
    }
    bmc_active_rat = current_rat ;
  }

  new_state = cb_cbch_service_state_handler (&new_state,current_rat);
  if (filter_out_of_date_messages)
  {
    if (cb_extended_cbch_supported)
    {
      cb_filters_out_of_date_messages(CBCH_ECBCH) ;
    }
    else
    {
      cb_filters_out_of_date_messages(CBCH) ;
    }
  }

#ifdef FEATURE_UMTS_BMC

  if (delete_page_buffers)
  {
    if (current_rat == CB_GSM_MODE)
    {
      rat_tobe_deleted = CB_UMTS_MODE ;
    }
    else
    {
      rat_tobe_deleted = CB_GSM_MODE ;
    }

    cb_bmc_clean_up_obsolete_page_from_list(
                                rat_tobe_deleted,basic_cbch_list_entry_point) ;
  }

#endif /* #ifdef FEATURE_UMTS_BMC */  

  return new_state ;
} /* end cb_process_cell_change_indication() */

/*===========================================================================

FUNCTION CB_PROCESS_RR_DATA_INDICATION

DESCRIPTION
  This function processes incoming paging block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_rr_data_indication
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  switch (cur_state->state)
  {
    case CB_IDLE_STATE :
      
      MSG_HIGH_DS_0(cb_as_id,"CB: Can not process DL_CB_BLOCK_IND in IDLE state") ;

      break ;
    case CB_ACTIVE_STATE :

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
      cb_current_sfn = cb_incoming_primitive->command.cb_block_ind.Frame_no;
      if (cb_pseudo_drx_supported &&
          (cb_active_substate != CB_ACTIVE_DRX))
      {
        /* first message slot after camping, store FN where infrequent CBCH decoding will start */
        if ((cb_pseudo_drx_start_fn == BMC_INVALID_SFN) &&
            ((CB_CALCULATE_PAGING_SLOT(cb_current_sfn) == CB_FIRST_BLOCK) ||
              (CB_CALCULATE_PAGING_SLOT(cb_current_sfn) == CB_FIFTH_BLOCK)))
        {
          cb_pseudo_drx_prev_start_fn = cb_current_sfn;
          cb_pseudo_drx_start_fn = cb_calculate_slot_fn(cb_current_sfn,cb_pre_pseudo_drx_scan_length);
          cb_is_second_pseudo_mode = FALSE;

          MSG_HIGH_DS_2(cb_as_id,"CB PSEUDO DRX: Aggressive scan for %d slots till FN:%d",
                    cb_pre_pseudo_drx_scan_length, cb_pseudo_drx_start_fn) ;
        }
        /* check if  the previous cycle has expired and GL1 failed to intimate us */
        else if ((cb_pseudo_drx_start_fn != BMC_INVALID_SFN) && 
                 (cb_pseudo_drx_prev_start_fn != BMC_INVALID_SFN) && 
                 ((cb_current_sfn == cb_pseudo_drx_start_fn) ||
                  (!cb_bmc_check_sfn_within_sch_prd(cb_current_sfn,cb_pseudo_drx_prev_start_fn,
                                                   cb_pseudo_drx_start_fn))))
        {
          cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
          cb_active_substate_control(CB_STD_PSEUDO_DRX_ACTIVATED);
        }
        /* check if next FN is where pseudo DRX is supposed to start */
        else if ((cb_pseudo_drx_start_fn != BMC_INVALID_SFN) && 
                 (cb_pseudo_drx_prev_start_fn != BMC_INVALID_SFN)&& 
                 (cb_calculate_block_fn(cb_current_sfn,CB_CHANNEL_CYCLE-CB_FOURTH_BLOCK)
                     == cb_pseudo_drx_start_fn))
        {
          cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
          cb_active_substate_control(CB_STD_PSEUDO_DRX_ACTIVATED);
        }
      }

#endif

      if (!cb_validate_paging_block(
                  cb_incoming_primitive->command.cb_block_ind.cb_block,
                         cb_incoming_primitive->command.cb_block_ind.Frame_no))
      {
        byte sequence_number = CB_FIRST_PAGING_BLOCK ;
        dword received_fn = 
                         cb_incoming_primitive->command.cb_block_ind.Frame_no ;

        MSG_HIGH_DS_0(cb_as_id,"CB: Received invalid CB BLOCK") ;

        switch (CB_CALCULATE_PAGING_SLOT(received_fn))
        {
          case CB_FIRST_BLOCK:
          case CB_FIFTH_BLOCK:
            sequence_number = CB_FIRST_PAGING_BLOCK ;
            break ;
          case CB_SECOND_BLOCK:
          case CB_SIXTH_BLOCK:
            sequence_number = CB_SECOND_PAGING_BLOCK ;
            break ;
          case CB_THIRD_BLOCK:
          case CB_SEVENTH_BLOCK:
            sequence_number = CB_THIRD_PAGING_BLOCK ;
            break ;
          case CB_FOURTH_BLOCK:
          case CB_EIGHTH_BLOCK:
            sequence_number = CB_FOURTH_PAGING_BLOCK ;
            break ;
          default: /* Nothing to do */
            break ;
        }

        cb_skip_rest_of_page(
           cb_calculate_base_fn(received_fn,sequence_number),sequence_number) ;

        return ;
      }

      /* Process valid paging block */
      cb_process_paging_block(
         cb_incoming_primitive->command.cb_block_ind.cb_block,
                cb_incoming_primitive->command.cb_block_ind.Frame_no) ;
      break ;

    default:
      MSG_FATAL_DS( cb_as_id,"Illigal CB state ID",0,0,0) ;
  }
} /* end cb_process_rr_data_indication() */

#ifdef FEATURE_UMTS_BMC
/*===========================================================================

FUNCTION BMC_PROCESS_WCDMA_EXPIRY_IND

DESCRIPTION
  This function processes WCDMA L1 expiry indication

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
cb_state_type bmc_process_wcdma_expiry_ind
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state ;

  if(cur_state == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  if((cur_state->state == CB_IDLE_STATE)
#ifdef FEATURE_WCDMA_ETWS
     && (cb_etws_state.state == CB_IDLE_STATE)
#endif
     )
  {
      MSG_HIGH_DS_0(cb_as_id,"WCDMA L1 Schedule Expiry indication received in IDLE state") ;
  }
  else if((cur_state->state == CB_ACTIVE_STATE)
#ifdef FEATURE_WCDMA_ETWS
          || (cb_etws_state.state == CB_ACTIVE_STATE)
#endif
          )
  {
      cb_start_non_drx_mode(CB_UMTS_MODE) ;
#ifdef FEATURE_MODEM_HEAP
      /*clear all the schedule DRX memory pools*/
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
      cb_free_dynamic_pool_DRX();
#endif
#endif
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"WCDMA L1 Schedule Expiry indication ignored in Improper state") ;
  }

  return new_state ;
} /* end bmc_process_wcdma_expiry_ind() */

#endif
/*lint +e715*/
/*===========================================================================

FUNCTION CB_PROCESS_L2_MESSAGE

DESCRIPTION
  This function processes L2 messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_l2_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state ;

  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  switch (cb_incoming_primitive->command.message_header.message_id)
  {
    case DL_CB_BLOCK_IND :

      MSG_HIGH_DS_1(cb_as_id,"CB: Received DL_CB_BLOCK_IND FN:%d",cb_incoming_primitive->command.cb_block_ind.Frame_no) ;

      /* Process L2 data indication */
      cb_process_rr_data_indication(cb_incoming_primitive,&new_state) ;

      /* Filter out obsolete Skip Request history */
      cb_process_skip_archive(
                        cb_incoming_primitive->command.cb_block_ind.Frame_no) ;

      break ;
    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected RR message is received") ;
      break ;
  }

  return new_state ;
} /* end cb_process_l2_message() */

/*===========================================================================

FUNCTION CB_PROCESS_RR_MESSAGE

DESCRIPTION
  This function processes RR messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_rr_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state ;
  boolean rlc_pdu_size_changed = FALSE ;
  byte              *incoming_plmn ;
  byte              *incoming_lac ;
  byte               incoming_cell_id[4] = {0x00, 0x00, 0x00, 0x00};
  byte               cb_mcc[3];

  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;
  cb_supported = cb_incoming_primitive->command.cell_change_ind.CBCH_present ;
  incoming_plmn = cb_incoming_primitive->command.cell_change_ind.plmn_id ;
  incoming_lac = cb_incoming_primitive->command.cell_change_ind.lac ;

  /*Extracting MCC form plmn_id*/
  cb_mcc[0] = incoming_plmn[0] & 0x0F;
  cb_mcc[1] = (incoming_plmn[0] & 0xF0) >> 4;
  cb_mcc[2] = incoming_plmn[1] & 0x0F;
  
  /* Setting cb_dup_detection_timer_value with respect to MCC */
  if(memcmp(cb_mcc,cb_mcc_JAPAN1,3) == 0 || memcmp(cb_mcc,cb_mcc_JAPAN2,3) == 0)
  {
    cb_dup_timer_value = CB_DUP_TIMER_VALUE_1;
  }
  else
  {
    cb_dup_timer_value = CB_DUP_TIMER_VALUE_2;
  }

  memscpy((void *)&incoming_cell_id[2], sizeof(incoming_cell_id)-2,
           (void *)cb_incoming_primitive->command.cell_change_ind.cell_id, 2) ;


  switch (cb_incoming_primitive->command.message_header.message_id)
  {
    case RR_CELL_CHANGE_IND :

      MSG_HIGH_DS_0(cb_as_id,"CB: Received RR_CELL_CHANGE_IND") ;

      if(TRUE == cb_gsm_feature_disabled)
      {
        MSG_HIGH_DS_0(cb_as_id,"CB: GSM CB feature is disable") ;
        return new_state;
      }

      /* Notify WMS task that cell has been changed */
      cb_send_wms_cell_change_ind(incoming_plmn, incoming_lac, incoming_cell_id) ;

      if (cb_bmc_rlc_pdu_size != CB_PAGING_BLOCK_DATA_LENGTH)
      {
          /* RLC pdu size */
        cb_bmc_rlc_pdu_size = CB_PAGING_BLOCK_DATA_LENGTH ;

        rlc_pdu_size_changed = TRUE ;
      }


      /* Process RR message */
      new_state = cb_process_cell_change_indication(
              rlc_pdu_size_changed,CB_GSM_MODE,
                      incoming_plmn,incoming_lac,incoming_cell_id,&new_state) ;

      break ;
    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected RR message is received") ;
      break ;
  }

  return new_state ;
} /* end cb_process_rr_message() */

#ifdef FEATURE_UMTS_BMC

/*===========================================================================

FUNCTION CB_PROCESS_RRC_MESSAGE

DESCRIPTION
  This function processes RRC messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_rrc_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  boolean rlc_pdu_size_changed = FALSE ;
  sys_plmn_id_s_type  incoming_plmn ;    
  cb_state_type new_state ;
  byte              *incoming_lac ;
  byte              *incoming_cell_id ;

  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  new_state = *cur_state ;

  switch (cb_incoming_primitive->command.message_header.message_id)
  {
    case RRC_CELL_CHANGE_IND :

      cb_supported = cb_incoming_primitive->command.
                         rrc_cell_change_ind.ctch_is_supported ;
      incoming_lac = cb_incoming_primitive-> command.rrc_cell_change_ind.lac ;
      incoming_cell_id = (byte *)&cb_incoming_primitive->
                          command.rrc_cell_change_ind.cell_id ;
      incoming_plmn = cb_convert_rrc_plmn_id_to_sys_plmn_id(
      cb_incoming_primitive->command.rrc_cell_change_ind.plmn_id) ;

      /* Setting cb_dup_detection_timer_value with respect to MCC */
      if(memcmp(cb_incoming_primitive->command.rrc_cell_change_ind.plmn_id.mcc,cb_mcc_JAPAN1,3) == 0 || memcmp(cb_incoming_primitive->command.rrc_cell_change_ind.plmn_id.mcc,cb_mcc_JAPAN2,3) == 0)
      {
        cb_dup_timer_value = CB_DUP_TIMER_VALUE_1;
      }
      else
      {
        cb_dup_timer_value = CB_DUP_TIMER_VALUE_2;
      }
      
      MSG_HIGH_DS_2(cb_as_id,"CB: Received RRC_CELL_CHANGE_IND with N = %d, K = %d ",
      cb_incoming_primitive->command.rrc_cell_change_ind.rrcbmc_ctch_info.ctch_allocation_period,
      cb_incoming_primitive->command.rrc_cell_change_ind.rrcbmc_ctch_info.cbs_frame_offset) ;

      /* Notify WMS task that cell has been changed */
      cb_send_wms_cell_change_ind((byte *)&incoming_plmn, incoming_lac, incoming_cell_id) ;

      if (cb_supported)
      {
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
        /*store the SUB ID, which will be 
          used in other operation of BMC
        */
        bmc_as_id = cb_incoming_primitive->command.rrc_cell_change_ind.as_id;

        if(cb_multisim_standby==TRUE)
        {
          /*State change will be automatically taken care by cb_process_cell_change_indication*/
		    cb_incoming_primitive->command.rrc_cell_change_ind.ctch_is_supported = FALSE;
            cb_supported = FALSE;
        }
#endif /*#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)*/
        /* Number of radio frame per TTI */
        cb_bmc_ctch_rf_per_tti = cb_incoming_primitive->
                 command.rrc_cell_change_ind.rrcbmc_ctch_info.ctch_rf_per_tti ;

        /* Period of CTCH allocation ( N ) */
        cb_bmc_ctch_allocation_period = cb_incoming_primitive->
          command.rrc_cell_change_ind.rrcbmc_ctch_info.ctch_allocation_period ;

        /* CBS frame offset ( K ) */
        cb_bmc_cbs_frame_offset = cb_incoming_primitive->
                command.rrc_cell_change_ind.rrcbmc_ctch_info.cbs_frame_offset ;

        if (cb_bmc_rlc_pdu_size != ((byte)cb_incoming_primitive->
                      command.rrc_cell_change_ind.rrcbmc_ctch_info.
                                                         rlc_pdu_size / 8 - 1))
        {
          /* RLC pdu size */
          cb_bmc_rlc_pdu_size = (byte)(cb_incoming_primitive->
           command.rrc_cell_change_ind.rrcbmc_ctch_info.rlc_pdu_size / 8 - 1) ;

          rlc_pdu_size_changed = TRUE ;
        }

#if defined FEATURE_UMTS_BMC_DRX_SUPPORTED
        cb_bmc_schedule_msg_periodicity = cb_incoming_primitive->command.
             rrc_cell_change_ind.rrcbmc_ctch_info.bmc_schedul_msg_period;

        MSG_HIGH_DS_1(cb_as_id,"BMC: Schedule Message Periodicity: %d",cb_bmc_schedule_msg_periodicity);

        if((cb_bmc_schedule_msg_periodicity != 8) &&
           (cb_bmc_schedule_msg_periodicity != 16) &&
           (cb_bmc_schedule_msg_periodicity != 32) &&
           (cb_bmc_schedule_msg_periodicity != 64) &&
           (cb_bmc_schedule_msg_periodicity != 128) &&
           (cb_bmc_schedule_msg_periodicity != 256))
        {
          cb_bmc_schedule_msg_periodicity = 0;
        }
#endif


        /* Register BMC task for CTCH data stream */
        bmc_register_rlc_wm(cb_incoming_primitive->
                      command.rrc_cell_change_ind.rrcbmc_ctch_info.rlc_lc_id) ;
      }

      /* Process RRC message */
      new_state = cb_process_cell_change_indication(rlc_pdu_size_changed,
                                                    CB_UMTS_MODE,
                                                    (byte *)&incoming_plmn,
                                                    incoming_lac,
                                                    incoming_cell_id,
                                                    &new_state) ;
       break ;
#ifdef FEATURE_WCDMA_ETWS
      case RRC_ETWS_PRIM_NOTIFICATION_IND:
        MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received RRC ETWS Primary Indication");
        cb_bmc_process_etws_prim_notification_ind(&cb_incoming_primitive->command.rrc_etws_prim_notification_ind,&new_state);
        break;
#endif
    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected RRC message is received") ;
      break ;
  }
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
  /*Reset the SUB ID , in case the CBCH support is not present.
    This has to be done at the end after all the operation as
    the other operations might be using this SUB ID*/
  if((RRC_CELL_CHANGE_IND == cb_incoming_primitive->command.message_header.message_id) &&
     (!cb_supported) )
  {
    bmc_as_id = SYS_MODEM_AS_ID_NONE;
  }
#endif //#ifdef FEATURE_NAS_CBS_DSDS
  return new_state ;
} /* end cb_process_rrc_message() */

#endif /* #ifdef FEATURE_UMTS_BMC */
/*===========================================================================

FUNCTION CB_PROCESS_TIMER_EXPIRY

DESCRIPTION
  This function handles the CB timer expiry events


DEPENDENCIES
  None

RETURN VALUE
 CB state type

SIDE EFFECTS
  None

===========================================================================*/

static cb_state_type cb_process_timer_expiry
(
  timer_id_T timer_id,
  const cb_state_type* cur_state
)
{
  cb_state_type new_state = *cur_state;

  gs_clear_timer(GS_QUEUE_CB, timer_id);

  switch(timer_id)
  {
    case CB_TIMER_DUP_DET:
        MSG_HIGH_DS_0(cb_as_id,"CB: Timer CB_TIMER_DUP_DET expired");
        cb_update_cache_timer_offsets(0x00);
        break;

#if defined (FEATURE_WCDMA_ETWS) && defined (FEATURE_UMTS_BMC)
    case CB_ETWS_PRIM_DISABLE_CTCH_TIMER:
        MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Timer CB_ETWS_PRIM_DISABLE_CTCH_TIMER expired");
        if (new_state.state == CB_ACTIVE_STATE)
        {
          MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Timer expiry is ignored as non-ETWS CB is Active");
        }
        if(cb_etws_ctch_drx_timer_running)
        {
          /* Timer expired resetting DRX flag to FALSE */
          cb_etws_ctch_drx_timer_running = FALSE;
         }
        cb_etws_state_control
        (
           (cb_state_id_type)cb_etws_substate_control(CB_ETWS_STD_CTCH_TIMER_EXPIRED),
             new_state
        );
        break;
#endif
    default :
          MSG_FATAL_DS( cb_as_id,"CB: Unknown timer id expired",0,0,0);
  }    
  
  return new_state;
}

/*===========================================================================

FUNCTION BMC_PROCESS_REG_MESSAGE

DESCRIPTION
  This function processes REG.messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_reg_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state = *cur_state ;
  #ifdef FEATURE_NO_CBS_LTD   
  cb_rat_type active_rat;
  #endif
  if(cb_incoming_primitive == NULL)
  {
  MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  switch (cb_incoming_primitive->command.message_header.message_id)
    {
#ifdef FEATURE_NO_CBS_LTD       
     case REG_SERVICE_STATE_IND :
      if(cb_incoming_primitive->command.service_state_ind.gsm_active_rat)
      {
        active_rat=CB_GSM_MODE;//TRUE for GSM
      }
      else
      {
        active_rat=CB_UMTS_MODE;//FALSE for UMTS
      }
      if(bmc_active_rat == active_rat)
      {
          if(cb_service == cb_incoming_primitive->command.service_state_ind.service_state)
          {
            MSG_HIGH_DS_0(cb_as_id,"CB: Received Duplicate Request from REG");
          }
          else
          {
            cb_service   = cb_incoming_primitive->command.service_state_ind.service_state;
            MSG_HIGH_DS_0(cb_as_id,"CB: Received REG_SERVICE_STATE_IND") ;
            new_state = cb_cbch_service_state_handler(&new_state,active_rat);
          }
      } 
          break ;
#endif
      case REG_MODE_CHANGE_STATE_IND :
         MSG_HIGH_DS_0(cb_as_id,"CB: Receive REG_MODE_CHANGE_STATE_IND ");
         cb_process_mode_change_indication((byte)cb_incoming_primitive->command. multisim_status_ind.active_subs);
          break ;
        default:
            MSG_HIGH_DS_0(cb_as_id,"Unexpected REG message is received") ;
          break ;
    }
    
  return new_state;
}






/*===========================================================================

FUNCTION CB_PROCESS_L1_MISSING_BLOCK_IND

DESCRIPTION
  This function processes L1 missing block indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_l1_missing_block_ind
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  switch (cur_state->state)
  {
    case CB_IDLE_STATE :
      MSG_HIGH_DS_0(cb_as_id,"L1 Missing Page indication received in IDLE state") ;
      break ;
    case CB_ACTIVE_STATE :

    cb_ctch_pdu_dropped(cb_incoming_primitive->command.cb_missing_block.fn);

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
      cb_current_sfn = cb_incoming_primitive->command.cb_missing_block.fn;
      if (cb_pseudo_drx_supported &&
          (cb_active_substate != CB_ACTIVE_DRX))
      {
        /* first message slot after camping, store FN where infrequent CBCH decoding will start */
        if ((cb_pseudo_drx_start_fn == BMC_INVALID_SFN) &&
            ((CB_CALCULATE_PAGING_SLOT(cb_current_sfn) == CB_FIRST_BLOCK) ||
              (CB_CALCULATE_PAGING_SLOT(cb_current_sfn) == CB_FIFTH_BLOCK)))
        {
          cb_pseudo_drx_prev_start_fn = cb_current_sfn;
          cb_pseudo_drx_start_fn = cb_calculate_slot_fn(cb_current_sfn,cb_pre_pseudo_drx_scan_length);
          cb_is_second_pseudo_mode = FALSE;

          MSG_HIGH_DS_2(cb_as_id,"CB PSEUDO DRX: Aggressive scan for %d slots till FN:%d",
                    cb_pre_pseudo_drx_scan_length, cb_pseudo_drx_start_fn) ;
        }
        /* check if  the previous cycle has expired and GL1 failed to intimate us */
        else if ((cb_pseudo_drx_start_fn != BMC_INVALID_SFN) && 
                 (cb_pseudo_drx_prev_start_fn != BMC_INVALID_SFN) && 
                 ((cb_current_sfn == cb_pseudo_drx_start_fn) ||
                  (!cb_bmc_check_sfn_within_sch_prd(cb_current_sfn,cb_pseudo_drx_prev_start_fn,
                                                   cb_pseudo_drx_start_fn))))
        {
          cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
          cb_active_substate_control(CB_STD_PSEUDO_DRX_ACTIVATED);
        }
        /* check if next FN is where pseudo DRX is supposed to start */
        else if ((cb_pseudo_drx_start_fn != BMC_INVALID_SFN) && 
                 (cb_pseudo_drx_prev_start_fn != BMC_INVALID_SFN)&& 
                 (cb_calculate_block_fn(cb_current_sfn,CB_CHANNEL_CYCLE-CB_FOURTH_BLOCK)
                     == cb_pseudo_drx_start_fn))
        {
          cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
          cb_active_substate_control(CB_STD_PSEUDO_DRX_ACTIVATED);
        }
      }
#endif

      cb_asm_state_handler(cb_incoming_primitive->command.cb_missing_block.fn) ;

      break ;

    default:
      MSG_FATAL_DS( cb_as_id,"Illigal CB state ID",0,0,0) ;
  }

} /* end cb_process_l1_missing_block_ind() */
    
/*===========================================================================

FUNCTION CB_PROCESS_L1_SKIP_CNF

DESCRIPTION
  This function processes L1 skip confirmation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_l1_skip_cnf
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if(cb_incoming_primitive->command.cb_skip_cnf.scheduling_period_fn_list != NULL)
  {
    cb_mem_free(cb_incoming_primitive->command.cb_skip_cnf.scheduling_period_fn_list) ;
#ifdef FEATURE_MODEM_HEAP
    if (cb_state.state == CB_IDLE_STATE)
    {
      cb_free_skip_pool_if_needed();
    }
#endif
  }

} /* end cb_process_l1_skip_cnf() */

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
/*===========================================================================

FUNCTION CB_PROCESS_L1_SCHEDULE_EXPIRED_IND

DESCRIPTION
  This function processes L1 expiry indication

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_l1_schedule_expired_ind
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state = *cur_state ;

  switch (cur_state->state)
  {
    case CB_IDLE_STATE :
      MSG_HIGH_DS_0(cb_as_id,"L1 Schedule Expired indication received in IDLE state") ;
      break ;
    case CB_ACTIVE_STATE :

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
      if (cb_pseudo_drx_supported && 
          (cb_pseudo_drx_start_fn != BMC_INVALID_SFN))
      {
        (void) cb_start_gsm_pseudo_drx_mode(TRUE);
      }
      else
#endif
      {
        switch (cb_incoming_primitive->
                                     command.cb_schedule_expired_ind.channel_ind)
        {
          case CBCH:
            cb_start_gsm_non_drx_mode(CBCH) ;
            break ;
          case ECBCH:
            cb_start_gsm_non_drx_mode(ECBCH) ;
            break ;
          case CBCH_ECBCH:
            cb_start_gsm_non_drx_mode(CBCH_ECBCH) ;
            break ;
          default:
            MSG_ERROR_DS_0(cb_as_id,"Unknown channel identifier") ;
            cb_start_gsm_non_drx_mode(CBCH_ECBCH) ;
            break ;
        }
      }
#if (defined FEATURE_MODEM_HEAP) && (defined FEATURE_GSM_CB_DRX_SUPPORTED )
      cb_free_dynamic_pool_DRX();
#endif
      break ;

    default:
      MSG_FATAL_DS( cb_as_id,"Illigal CB state ID",0,0,0) ;
  }

  return new_state ;
} /* end cb_process_l1_schedule_expired_ind() */

/*===========================================================================

FUNCTION CB_PROCESS_L1_SCHEDULING_CNF

DESCRIPTION
  This function processes L1 scheduling confirmation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_l1_scheduling_cnf
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if(cb_incoming_primitive->command.cb_scheduling_cnf.scheduling_period_fn_list != NULL)
  {
    cb_mem_free(cb_incoming_primitive->command.cb_scheduling_cnf.scheduling_period_fn_list) ;
#ifdef FEATURE_MODEM_HEAP
    if (cb_state.state == CB_IDLE_STATE)
    {
      cb_free_skip_pool_if_needed();
    }
#endif
  }

} /* end cb_process_l1_scheduling_cnf() */
#endif

/*===========================================================================

FUNCTION CB_PROCESS_L1_MESSAGE

DESCRIPTION
  This function processes L1 messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type cb_process_l1_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state ;

  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  switch (cb_incoming_primitive->command.message_header.message_id)
  {
    case L1_CB_MISSING_BLOCK_IND:
      
      MSG_HIGH_DS_1(cb_as_id,"CB: Received L1_CB_MISSING_BLOCK_IND FN:%d",
         cb_incoming_primitive->command.cb_missing_block.fn) ;

      cb_process_l1_missing_block_ind(cb_incoming_primitive,&new_state) ;

      break ;

    case L1_CB_SKIP_CNF:

      MSG_HIGH_DS_0(cb_as_id,"CB: Received L1_CB_SKIP_CNF ") ;

      cb_process_l1_skip_cnf(cb_incoming_primitive,&new_state) ;
      
      break ;

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
    case L1_CB_SCHED_EXPIRY_IND :

      MSG_HIGH_DS_0(cb_as_id,"CB DRX: Received CB_SCHEDULE_EXPIRED_IND ") ;

      new_state = 
         cb_process_l1_schedule_expired_ind(cb_incoming_primitive,&new_state) ;
      break ;

    case L1_CB_SCHEDULING_CNF:

      MSG_HIGH_DS_0(cb_as_id,"CB DRX: Received L1_CB_SCHEDULING_CNF ") ;

      cb_process_l1_scheduling_cnf(cb_incoming_primitive,&new_state) ;

      break ;
#endif

    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected RR message is received") ;
      break ;
  }

  return new_state ;
} /* end cb_process_rr_message() */

#ifdef FEATURE_UMTS_BMC
/*===========================================================================

FUNCTION BMC_PROCESS_RLC_DATA_INDICATION

DESCRIPTION
  This function processes RLC data block indication.

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
cb_state_type bmc_process_rlc_data_indication
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state,
  word            pdu_cnt_local
)
{
  cb_state_type new_state;
  dsm_item_type *item_ptr;
  boolean       start_pdu;
  word          rlc_pdu_len;
  byte          *rlc_pdu_ptr;
  byte          li_length;
  dword         received_sfn;
  dword         received_end_sfn;
  boolean      status;
  word         pdu_index;

  if(cur_state == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  if((cur_state->state == CB_IDLE_STATE)
#ifdef FEATURE_WCDMA_ETWS
     && (cb_etws_state.state == CB_IDLE_STATE)
#endif
    )
  {
      MSG_ERROR_DS_0(cb_as_id,"CB: Can not process DL_RLC_BLOCK_IND in IDLE state") ;
      /*Get the DSM item and trash it*/
      item_ptr = bmc_get_dsm_item();
      if (item_ptr != NULL)
      {
        dsm_free_packet(&item_ptr);
      }
  }
  else if((cur_state->state == CB_ACTIVE_STATE)
#ifdef FEATURE_WCDMA_ETWS
          || (cb_etws_state.state == CB_ACTIVE_STATE)
#endif
     )
  {
      /*Get the DSM item from the chain*/

      MSG_HIGH_DS_1(cb_as_id,"BMC: Received %d new DSM chain(s) ",pdu_cnt_local) ;

      for (pdu_index = 0; pdu_index < pdu_cnt_local; pdu_index++)
      {
        item_ptr = bmc_get_dsm_item();


        MSG_HIGH_DS_1(cb_as_id,"BMC: Received new DSM chain 0x%X",item_ptr) ;

        if (item_ptr != NULL)
        {
          /*-----------------------------------------------------------------------
          The RLC wm item has BMC PDU queued up. De-queue the first PDU and 
          decode the SFN,LI bits used and Start PDU
          -----------------------------------------------------------------------*/
          status = bmc_parse_rlc_params(item_ptr,&start_pdu,&li_length,&received_sfn,&received_end_sfn);

          if (status == FALSE)
          {
            MSG_HIGH_DS_0(cb_as_id,"CB: Error decoding the RLC parameters");

            /*Free the DSM packet chain*/
            dsm_free_packet(&item_ptr);

            return new_state ;
          }

          if (!cb_bmc_validate_pdu_sfn(received_sfn))
          {
            MSG_HIGH_DS_1(cb_as_id,"CB: Received invalid RLC PDU %d",received_sfn) ;

            /*Free the DSM packet chain*/
            dsm_free_packet(&item_ptr);

            return new_state ;
          }

          /*Get the PDU lenght*/
          rlc_pdu_len = bmc_compute_rlc_pdu_len(item_ptr);

          MSG_HIGH_DS_3(cb_as_id,"BMC: Expected RLC PDU length is %d at SFN %d and End_SFN = %d ",rlc_pdu_len,received_sfn,received_end_sfn) ;

          /*Allocate the memory to store the PDU*/
#ifndef FEATURE_MODEM_HEAP
          if ((rlc_pdu_ptr = (byte *)gs_alloc(rlc_pdu_len)) != NULL)
#else
          if ((rlc_pdu_ptr = (byte *)modem_mem_calloc(1,rlc_pdu_len, MODEM_MEM_CLIENT_NAS)) != NULL)
#endif
          {
            /*copy the PDu in to the buffer*/
            cb_umts_get_rlc_pdu_data_ptr(rlc_pdu_ptr, item_ptr, rlc_pdu_len);

            /*Free the DSM packet chain*/
            dsm_free_packet(&item_ptr);
  
                 /*Flip the octet bits*/
            cb_bmc_flip_octet_bits_rlc_pdu(rlc_pdu_ptr, rlc_pdu_len);
  
  
            /*Now process the RLC PDU*/
#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
            cb_bmc_process_rlc_pdu(received_sfn, received_end_sfn, li_length, rlc_pdu_len, rlc_pdu_ptr);
#else
            cb_bmc_process_rlc_pdu(received_sfn, received_end_sfn, li_length, start_pdu, rlc_pdu_len, rlc_pdu_ptr);
#endif

            /* Free tmp BMC buffer */
#ifndef FEATURE_MODEM_HEAP
            gs_free(rlc_pdu_ptr) ;
#else
            modem_mem_free(rlc_pdu_ptr, MODEM_MEM_CLIENT_NAS) ;
#endif
          }
          else
          {
            /*Free the DSM packet chain*/
            dsm_free_packet(&item_ptr);

            MSG_ERROR_DS_0(cb_as_id,"Not enough memory to process RLC PDU");
          }
        }
        else
        {
          MSG_ERROR_DS_0(cb_as_id,"Unable to get DSM item");
        }
      }
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"Ignored DL_RLC_BLOCK_IND in improper state");
  }
  return new_state;
}
/*lint +e715*/
/*===========================================================================

FUNCTION BMC_PROCESS_RLC_MESSAGE

DESCRIPTION
  This function processes RLC messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type bmc_process_rlc_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  word no_pdus;
  cb_state_type new_state;
  
  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  switch (cb_incoming_primitive->command.message_header.message_id)
  {
    case DL_BMC_BLOCK_IND:

      MSG_HIGH_DS_0(cb_as_id,"BMC: Received DL_BMC_BLOCK_IND") ;
      
      no_pdus = cb_incoming_primitive->command.bmc_block_ind.pdu_cnt;
      /* Process RLC data indication */
      (void)bmc_process_rlc_data_indication(cb_incoming_primitive, &new_state, no_pdus);
      
      break ;
    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected RLC message is received") ;
      break ;
  }
  /*If the queue is empty and BMC is suspended then retreive the 
    PDUs from the DSM chain*/
  if (bmc_rx_suspended && gs_count_message(GS_QUEUE_RLC_CB) == 0)
  {
    word index = 0;
    for (index = pdu_cnt; index > 1; index--)
    {
      /* Process RLC data indication */
      (void)bmc_process_rlc_data_indication(cb_incoming_primitive, &new_state, 1);
    }

    MSG_HIGH_DS_0(cb_as_id,"BMC: Recovered from the suspend state");
    pdu_cnt = 1;
    /*Now resume the BMC and start Non DRX processing*/
    bmc_rx_suspended = FALSE;
    cb_start_non_drx_mode(CB_UMTS_MODE);
  }
  
  return new_state;
}

/*===========================================================================

FUNCTION BMC_PROCESS_WCDMA_L1_MESSAGE

DESCRIPTION
  This function processes WCDMA L1 messages

DEPENDENCIES
  None

RETURN VALUE
  Next CB task's state

SIDE EFFECTS
  None

===========================================================================*/
cb_state_type bmc_process_wcdma_l1_message
(
  cb_command_type *cb_incoming_primitive,
  cb_state_type   *cur_state
)
{
  cb_state_type new_state;

  if(cur_state == NULL || cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  new_state = *cur_state ;

  switch (cb_incoming_primitive->command.message_header.message_id)
  {
    case L1_BMC_SCHED_EXPIRY_IND:

      MSG_HIGH_DS_0(cb_as_id,"BMC: Received L1_BMC_SCHED_EXPIRY_IND") ;

      new_state = 
               bmc_process_wcdma_expiry_ind(cb_incoming_primitive,&new_state) ;
  
      break ;
    case L1_BMC_SKIP_CNF:

      MSG_HIGH_DS_0(cb_as_id,"BMC: Received L1_BMC_SKIP_CNF") ;

      if(cb_incoming_primitive->command.l1_bmc_skip_cnf.scheduling_period_sfn_list != NULL)
      {
        cb_mem_free(cb_incoming_primitive->
                          command.l1_bmc_skip_cnf.scheduling_period_sfn_list) ;
#ifdef FEATURE_MODEM_HEAP
        if (cb_state.state == CB_IDLE_STATE)
        {
          cb_free_skip_pool_if_needed();
        }
#endif
      }
      break ;
    case L1_BMC_SCHEDULING_CNF:

      MSG_HIGH_DS_0(cb_as_id,"BMC: Received L1_BMC_SCHEDULING_CNF") ;

      if(cb_incoming_primitive->command.l1_bmc_scheduling_cnf.scheduling_period_sfn_list != NULL)
      {
        cb_mem_free(cb_incoming_primitive->
                    command.l1_bmc_scheduling_cnf.scheduling_period_sfn_list) ;
#ifdef FEATURE_MODEM_HEAP
        if (cb_state.state == CB_IDLE_STATE)
        {
          cb_free_skip_pool_if_needed();
        }
#endif
      }
      break ;
      case L1_BMC_DROPPED_PDU_IND:

      MSG_HIGH_DS_0(cb_as_id,"BMC: Received L1_BMC_DROPPED_PDU_IND") ;
      cb_ctch_pdu_dropped(cb_incoming_primitive->command.l1_bmc_dropped_pdu_ind.pdu_sfn);

      break ;
    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected WL1 message is received") ;
      break ;
  }

  return new_state;
} /* end of bmc_process_wcdma_l1_message() */


#endif/*FEATURE_UMTS_BMC*/

/*===========================================================================

FUNCTION CB_MAIN_STATE_MACHINE_HANDLER

DESCRIPTION
  Main Cell Broadcast task state machine handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Change state & substate

===========================================================================*/
void cb_main_state_machine_handler
(
  cb_command_type *cb_incoming_primitive
)
{
  cb_state_type new_state = cb_state ;


  if(cb_incoming_primitive == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

#if defined (FEATURE_DUAL_SIM) && !defined(FEATURE_NAS_CBS_DSDS)
  if(mm_dual_standby_mode)
  {
    if (!((cb_incoming_primitive->command.message_header.message_set == MS_WMS_CB) &&
           (cb_incoming_primitive->command.message_header.message_id 
                                                         == (int)CB_SEARCH_LIST_REQ)))
    {
      return;
    }
  }
#elif defined (FEATURE_NAS_CBS_DSDS)

  if (!cb_get_as_id(cb_incoming_primitive))
  {
    return;
  }
#endif
  switch (cb_incoming_primitive->command.message_header.message_set)
  {
    case MS_WMS_CB:

      /* Process WMS message */

      new_state = cb_process_wms_message(cb_incoming_primitive,&cb_state) ;
      break ;
    case MS_CB_RR :

      /* Process RR message */

      new_state = cb_process_rr_message(cb_incoming_primitive,&cb_state);
      break ;

#ifdef FEATURE_UMTS_BMC
    case MS_RRC_BMC :

      /* Process RRC message */

      new_state = cb_process_rrc_message(cb_incoming_primitive,&cb_state);
      break ;

    case MS_RLC_BMC:
      /*Process RLC message*/

      new_state = bmc_process_rlc_message(cb_incoming_primitive, &cb_state);
      break;
    case MS_BMC_L1:
      /*Process RLC message*/

      new_state = bmc_process_wcdma_l1_message(cb_incoming_primitive, &cb_state);
      break;

#endif/*FEATURE_UMTS_BMC*/

    case MS_TIMER :
        /* Process timer expiry*/

        new_state = cb_process_timer_expiry(cb_incoming_primitive->command.timer_expiry.timer_id,&cb_state);
        break;

    case MS_REG_CB :
      /* Process REG message */
      new_state = cb_process_reg_message(cb_incoming_primitive,&cb_state);
      break ;

    case MS_CB_L2:

      /* Process L2 message */

      new_state = cb_process_l2_message(cb_incoming_primitive,&cb_state) ;
      break ;
    case MS_CB_L1:

      /* Process L1 message */

      new_state = cb_process_l1_message(cb_incoming_primitive,&cb_state) ;
      break ;
    default:
      MSG_HIGH_DS_0(cb_as_id,"Unexpected message set") ;
      break ;
  }

#ifdef FEATURE_MODEM_HEAP
  /*dynamic memory is freed
     *during transition from Active to Idle but not in Collection initiated substate 
     *since CB has to retain cache even after moving CB disabled cell.*/
  if(((cb_state.state != new_state.state) ||
      (cb_state.idle_substate != new_state.idle_substate)) && 
     (new_state.state == CB_IDLE_STATE))
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Dynamic memory is freed now");
    if (new_state.idle_substate != CB_COLLECTION_INITIATED_SUBSTATE)
    {
      cb_free_dynamic_pool_NDRX();
      current_message = basic_cbch_list = basic_cbch_list_entry_point = NULL ;
    }
    cb_free_skip_pool_if_needed();
#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
    cb_free_dynamic_pool_DRX();
#endif
  }
#endif
  /* Change CB task's state */

  cb_state = new_state ;

#ifdef FEATURE_UMTS_BMC
  /*Process any pending CB command , while processing RLC message*/
  if(MS_RLC_BMC == cb_incoming_primitive->command.message_header.message_set)
  {
    /*Go and check the control queue to verify any pending control commands*/
    cb_bmc_verify_cb_main_q();
  }
#endif
} /* end cb_main_state_machine_handler() */

void cb_active_substate_control( cb_event_T cb_event )
{
   /* Set action to a non-existing action as a default*/
   byte                         action        = 0xFF;
   boolean                      entry_found   = FALSE;
   byte                         entry         = 0;
   cb_active_substate_id_type   next_substate = cb_active_substate;
   byte           table_size = (sizeof(cb_substate_event_table)/sizeof(substate_event_T));/*Klocwork fix*/

/*
 * FOR all entries in table
 */

   while (cb_substate_event_table[entry].event != CB_STD_INVALID_EVENT)
   {
      if ( cb_substate_event_table[entry].current_substate == cb_active_substate )
      {
         if (cb_substate_event_table[entry].event ==  cb_event )
         {
            next_substate = cb_substate_event_table[entry].next_substate;

            action = cb_substate_event_table[entry].action_code;
            entry_found = TRUE;
            break;
         }
      }
      entry++;
      if(entry >= table_size) /*Klocwork fix to ensure array is accessed with in the bounds*/
      {
         MSG_ERROR_DS_0(cb_as_id,"Index out of bounds and buffer overflow");
         break;
      }
   }

   if ((entry_found) &&
       ((next_substate != cb_active_substate) ||
        (action != CB_ACTION_0)))
   {
      MSG_HIGH_DS_3(cb_as_id, "CB substate change from %d to %d, action:%d", cb_active_substate, next_substate, action );

      cb_active_substate = next_substate;

      switch (action)
      {
         case CB_ACTION_0:      /* NULL action, do nothing, go to next_substate */
            break;

#ifdef FEATURE_UMTS_BMC
         case CB_ACTION_1:     /* In NON-DRXP, resend SCHED REQ to WL1*/
            if((bmc_active_rat == CB_UMTS_MODE) &&
               (cb_state.state == CB_ACTIVE_STATE) &&
               (cb_bmc_schedule_msg_periodicity != 0))
            {
              cb_start_umts_non_drx_mode();
            }
            break;
#endif

         case CB_ACTION_2:     /* In DRX, skip the rest of msg */
            break;

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
         case CB_ACTION_3:      /* clear DRX */
            cb_bmc_cancel_scheduling();
            break;
#endif

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
         case CB_ACTION_4:      /* Pseudo DRX */
            if (cb_pseudo_drx_supported)
            {
              (void) cb_start_gsm_pseudo_drx_mode(FALSE);
            }
            break;
         case CB_ACTION_5:
            if (cb_pseudo_drx_supported)
            {
              (void) cb_send_skip_request(CB_ERASE_LAST_SCHEDULING,0,NULL);
            } 
            break;
#endif

         default:
            MSG_HIGH_DS_1(cb_as_id, "CB: Action %d not supported", action);
            break;
      }
   }
   
}

#endif /* #ifdef FEATURE_GSM_CB */

