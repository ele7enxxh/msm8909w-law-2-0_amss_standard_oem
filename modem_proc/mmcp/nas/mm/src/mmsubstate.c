/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmsubstate.c_v   1.8   08 Jan 2002 11:42:18   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmsubstate.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/01   jca     Ignore MM_STD_LIMITED_SERVICE in MM_NO_IMSI.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
09/18/01   sbk     Intertask primitives and QXDM packets are logged
11/06/01   jca     Added code to switch so ERRs not flagged on valid transitions.
11/28/01   jca     Added substate transition MM_NO_CELL_AVAILABLE -> MM_LIMITED_SERVICE.
12/19/01   jca     Added substate transition MM_ATTEMPTING_TO_UPDATE ->
                     MM_NO_CELL_AVAILABLE.
02/13/03   jca     Now handling event MM_STD_PLMN_SEARCH in substate
                   MM_PLMN_SEARCH_NORMAL_SERVICE instead of raising ERR.
06/15/04   jca     Ensure T3212 timer (re)started on all transitions to
                   MM_IDLE.NORMAL_SERVICE or MM_IDLE.ATTEMPTING_TO_UPDATE.
04/01/05   sm      Added transition from 
                   MM_ATTEMPTING_TO_UPDATE -> MM_UPDATED_NEEDED
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_v.h"
#include "err.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION MM_SUBSTATE_CONTROL

DESCRIPTION
  This function implements the state transition diagram of 2.4.1-s1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_substate_control( mm_event_T mm_sub_event )
{
   byte        next_substate;

   GS_SET_UNDEFINED( next_substate );

   if ( mm_sub_event == MM_STD_DEACTIVATE )
   {
      next_substate = MM_NULL_SUBSTATE;
   }
   else if ( mm_sub_event == MM_STD_NO_IMSI )
   {
      next_substate = MM_NO_IMSI;
   }
   else
   {

      switch ( mm_idle_substate )
      {
         case MM_NULL_SUBSTATE:
            if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            break;

         case MM_NO_IMSI:
            if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if (mm_sub_event == MM_STD_UPDATE_ATTEMPTED)
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if (mm_sub_event == MM_STD_NORMAL_SERVICE)
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if (mm_sub_event == MM_STD_LIMITED_SERVICE)
            {
               /* --------------------------------
               ** Ignore event, stay in same state
               ** -------------------------------- */
               next_substate = MM_NO_IMSI;
            }
            break;

         case MM_PLMN_SEARCH:
            if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_PENDING )
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
            else if (mm_sub_event == MM_STD_UPDATE_ATTEMPTED)
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            }
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
            break;

         case MM_LIMITED_SERVICE:
            if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_PENDING )
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
            break;

         case MM_ATTEMPTING_TO_UPDATE:
            if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_PENDING )
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
            break;

         case MM_LOCATION_UPDATE_NEEDED:
            if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if (mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            } 
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
            break;

         case MM_NO_CELL_AVAILABLE:
            if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }            
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
#ifdef FEATURE_LTE
            else if(mm_sub_event == MM_STD_UPDATE_PENDING_IRAT_FROM_LTE)
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
#endif
            break;

         case MM_PLMN_SEARCH_NORMAL_SERVICE:
            if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_PENDING )
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH_NORMAL_SERVICE;
            }
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
            break;

         case MM_NORMAL_SERVICE:
            if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH_NORMAL_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_PENDING )
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
#ifdef FEATURE_NAS_ECALL
            else if (mm_sub_event == MM_STD_ECALL_INACTIVE)
            {
               next_substate = MM_ECALL_INACTIVE;
            }
#endif
            break;
#ifdef FEATURE_NAS_ECALL
         case MM_ECALL_INACTIVE:
            if ( mm_sub_event == MM_STD_PLMN_SEARCH )
            {
               next_substate = MM_PLMN_SEARCH;
            }
            else if ( mm_sub_event == MM_STD_NO_SERVICE )
            {
               next_substate = MM_NO_CELL_AVAILABLE;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_PENDING )
            {
               next_substate = MM_LOCATION_UPDATE_NEEDED;
            }
            else if ( mm_sub_event == MM_STD_UPDATE_ATTEMPTED )
            {
               next_substate = MM_ATTEMPTING_TO_UPDATE;
            }
            else if ( mm_sub_event == MM_STD_LIMITED_SERVICE )
            {
               next_substate = MM_LIMITED_SERVICE;
            }
            else if ( mm_sub_event == MM_STD_NORMAL_SERVICE )
            {
               next_substate = MM_NORMAL_SERVICE;
            }
            break;
#endif
         default:
            MSG_ERROR_DS(MM_SUB, "=MM= Invalid mm idle substate value: %d", mm_idle_substate,0,0);
            break;
      }

   }

   if ( GS_IS_NOT_UNDEFINED( next_substate ))
   {
      MSG_HIGH_DS_3(MM_SUB, "=MM= MM Current Substate is %d, event %d next_substate %d", mm_idle_substate, mm_sub_event,next_substate);
      mm_idle_substate = next_substate;
      mmsend_mm_state_log_packet();


      if ((mm_state == MM_IDLE) &&
          ((mm_idle_substate == MM_NORMAL_SERVICE) ||
           (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)))
      {
#ifdef FEATURE_NAS_REL10
         if(mm_is_per_ms_t3212_present)
         {
             if((mm_stored_per_ms_t3212_value != 0) &&
                (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
             {

               mm_start_timer( TIMER_T3212,
                               mm_stored_per_ms_t3212_value);
             }
         }
         else
#endif
         if ((mm_system_information.T3212_timer_value != 0 ) &&
             (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
         {
             /* ------------------------------------------------------
             ** If the (T3212) timer is not already started, the timer
             ** is started each time the mobile enters the MM_IDLE
             ** substate NORMAL_SERVICE or ATTEMPTING_TO_UPDATE
             ** (24.008 section 4.2).
             ** ------------------------------------------------------ */

            mm_start_timer( TIMER_T3212,
                            mm_system_information.T3212_timer_value );
         }
      }
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Dont have transition for the mm sub-event: %d",  mm_sub_event,0,0);
   }

}
