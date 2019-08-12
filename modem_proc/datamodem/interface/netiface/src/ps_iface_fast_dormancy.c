/******************************************************************************
  @file    ps_iface_fast_dormancy.c
  @brief   

  DESCRIPTION
  This file defines functions, structs used for fast dormancy feature implementation
  Does Fast Dormancy statistics related operations and interacts with ps sys

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_fast_dormancy.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/14   tyk     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/


#include "ps_iface.h"
#include "ps_sys_fast_dormancy.h"
#include "ps_iface_fast_dormancy.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "ds_3gpp_hdlr.h"



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* global structure to hold/cache statistics for all UMTS IFACEs required to 
check data activity for Fast Dormancy implementation*/

static ps_iface_fast_dormancy_stats_info_s* 
                                          fd_stats_tbl[MAX_SYSTEM_IFACES];

/* global to maintain present free index of stats table  */
static uint8   fd_stats_tbl_idx;

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/



/*===========================================================================

                                      FUNCTIONS

===========================================================================*/


/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_INIT_IFACE_STATS()

DESCRIPTION
  This function initializes IFACE Stats table and takes snapshot of 
  current statistics before starting Fast Dormancy Timer 

PARAMETERS
None

RETURN VALUE
0 -  Success
-1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_fast_dormancy_init_iface_stats
(
  ps_iface_name_enum_type       input_name
)
{
  uint8                                 loop = 0;
  ps_iface_type                        *iface_ptr  = NULL;
  int                                   ret_val = 0;
  uint8                                 idx = 0;
  ps_iface_fast_dormancy_stats_info_s*  stats_info_p = NULL;

  /* ------------------------------------------------------------------------ */
  idx = fd_stats_tbl_idx;

  for (loop = 0;
         loop < MAX_SYSTEM_IFACES && NULL != global_iface_ptr_array[loop];
         ++loop)
  {
    iface_ptr = global_iface_ptr_array[loop];
    if (!PS_IFACE_IS_VALID(iface_ptr))
    {
      LOG_MSG_ERROR_1("ps_iface_init_all_iface_stats(): Invalid iface 0x%x",
                                                                 iface_ptr);
      continue;
    }

    if (iface_ptr->name != input_name)
    {
      continue;
    }

    stats_info_p = (ps_iface_fast_dormancy_stats_info_s*) 
         ps_system_heap_mem_alloc(sizeof(ps_iface_fast_dormancy_stats_info_s));
    
    if (stats_info_p == NULL)
    {
      LOG_MSG_ERROR_0("ps_iface_init_all_iface_stats(): memory alloc failed");
      return DS_NOMEMORY;
    }

    memset(stats_info_p,0, sizeof(ps_iface_fast_dormancy_stats_info_s));
    stats_info_p->iface_p = iface_ptr;

    if(E_SUCCESS != ps_stat_get_iface(
                        PS_STAT_IFACE_ALL,
                        iface_ptr,
                        &(stats_info_p->stats_info),
                        sizeof(ps_stat_iface_i_s_type)))
    {
      LOG_MSG_INFO1_1("ps_iface_init_all_iface_stats():"
                                     " FAILURE for Iface_p 0x%x", iface_ptr);
      ret_val = -1;
      break;
    }

    if (idx >= MAX_SYSTEM_IFACES )
    {
      LOG_MSG_INFO1_1(" Init Stats Tbl index out of Range, Idx: %d",idx);
      return -1;
    }

    /* Store the dynamic pointer to stats tbl */
    fd_stats_tbl[idx] = stats_info_p;
    idx++;

    /* TODO this check needs to removed when this API is being used for 
       general/common purpose  presently this is limited to UMTS IFACES Only */
     
    if(input_name == UMTS_IFACE && 
                     (DS_3GPP_MAX_PS_IFACES == (idx - fd_stats_tbl_idx)))
    {
      /* All UMTS IFACES stats are cached so Break now from loop */
      break;
    }
  }
  
  /* update the idx with new index value */
  fd_stats_tbl_idx = idx;

  if(ret_val == 0)
  {
    LOG_MSG_INFO1_1(" Init SUCCESS, final idx value of stats_tbl Idx: %d",idx);
  }
  return ret_val; 
}/* ps_iface_fast_dormancy_init_iface_stats */


/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_UPDATE_3GPP_IFACE_STATS()

DESCRIPTION
  This function updates cached 3GPP IFACE Stats with current statistics
  in IFACE stats table

PARAMETERS
None

RETURN VALUE
0 -  Success
-1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_fast_dormancy_update_3gpp_iface_stats()
{
  ps_iface_type                        *iface_ptr = NULL;
  ps_iface_fast_dormancy_stats_info_s  *stats_info_p = NULL;
  uint8                                 loop;
  uint8                                 max_loop_value;
  int                                   ret_val = 0;
 /*............................................................................................................ */
  max_loop_value = fd_stats_tbl_idx;
  
  for (loop = 0;loop < max_loop_value ;loop++)
  {
    stats_info_p = fd_stats_tbl[loop];
    if(NULL == stats_info_p)
    { 
      return -1;
    }
    iface_ptr = stats_info_p->iface_p;

    if ((!PS_IFACE_IS_VALID(iface_ptr))||
        (PS_IFACEI_GET_STATE(iface_ptr) != IFACE_UP &&
         PS_IFACEI_GET_STATE(iface_ptr) != IFACE_ROUTEABLE))
    {
      continue;
    }
    if (iface_ptr->name != UMTS_IFACE)
    {
      continue;
    }
    if(E_SUCCESS == ps_stat_get_iface(
                       PS_STAT_IFACE_ALL,
                       iface_ptr,
                       &(stats_info_p->stats_info),
                       sizeof(ps_stat_iface_i_s_type)))
    {
       
      LOG_MSG_INFO1_1("ps_iface_update_all_iface_stats(): "
                                      "SUCCESS for Iface_p 0x%x", iface_ptr);
    }
    else
    {
      LOG_MSG_INFO1_1("ps_iface_update_all_iface_stats():"
                                       " FAILED for Iface_p 0x%x", iface_ptr);
      ret_val = -1;
      break;
    }
  }
  return ret_val;
}/* ps_iface_fast_dormancy_update_3gpp_iface_stats */

/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_CHECK_DATA_ACTIVITY()

DESCRIPTION
  This function checks if there is any data activity in DL/UL by
  comparing cached stats to current IFACE statistics and also
  stops/restarts fast dormancy timers based on valid data activity 
  presence

PARAMETERS
None

RETURN VALUE
0 -  Activity is present
-1 - No Data activity

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_fast_dormancy_check_data_activity
(
  int *stop_fast_dormancy
)
{
  ps_iface_type            *iface_ptr = NULL;
  ps_iface_fast_dormancy_stats_info_s       *stats_info_p = NULL;
  ps_stat_iface_i_s_type    prev_stats;
  ps_stat_iface_i_s_type    new_stats;
  uint8                     loop = 0;
  uint8                     max_loop_value;
  uint64                    delta_rx_pkt = 0;
  uint64                    delta_drpd_pkt = 0;
  uint64                    total_delta_rx = 0;
  uint64                    total_delta_dropd = 0;
  int                       dl_activity = FALSE;
  int                       ul_activity = FALSE;
 /* ------------------------------------------------------------------------ */
  max_loop_value = fd_stats_tbl_idx;
 for (loop = 0;loop < max_loop_value;loop++)
 {
   stats_info_p = fd_stats_tbl[loop];
   if(NULL == stats_info_p)
   {
     ASSERT(0);
     return 0;
   }
   /* cache existing stats before update with new stats to compare */
   prev_stats = stats_info_p->stats_info;

   iface_ptr = stats_info_p->iface_p;
   if ((!PS_IFACE_IS_VALID(iface_ptr))||
       (PS_IFACEI_GET_STATE(iface_ptr) != IFACE_UP &&
        PS_IFACEI_GET_STATE(iface_ptr) != IFACE_ROUTEABLE))
   {
     continue;
   }
   /* update with new statistics */
   if(E_SUCCESS == ps_stat_get_iface(PS_STAT_IFACE_ALL,
                                     iface_ptr,
                                     &(stats_info_p->stats_info),
                                     sizeof(ps_stat_iface_i_s_type)))
    {
      new_stats = stats_info_p->stats_info;

      LOG_MSG_INFO1_2("ps_iface_check_data_activity(): Tx_cnt: %d, "
                 "Cached_Tx_cnt: %d", new_stats.pkts_tx, prev_stats.pkts_tx );

      LOG_MSG_INFO1_2("ps_iface_check_data_activity(): Rx_cnt: %d,"
                " Cached_Rx_cnt: %d", new_stats.pkts_rx, prev_stats.pkts_rx );

      /* compare UL statistic counts and decide on activity  */
      if((new_stats.pkts_tx >0)&&
         (new_stats.pkts_tx != prev_stats.pkts_tx))
      {
        ul_activity = TRUE;
        break;
      }
      /* check for DL Statistics if any packet activity */
      else if ((new_stats.pkts_rx >0)&&
               (new_stats.pkts_rx != prev_stats.pkts_rx))
      {
        dl_activity = TRUE;
     
        /* There is DL activity but we can not break here, because all 
           these DL packets could have been spurious and dropped. so 
           cummulate all the DL and dropped differences to see  if any 
           valid packet activity is present */
        delta_rx_pkt = (new_stats.pkts_rx - prev_stats.pkts_rx);
        delta_drpd_pkt = 
           (new_stats.pkts_dropped_rx - prev_stats.pkts_dropped_rx);
 
        total_delta_rx += delta_rx_pkt;
        total_delta_dropd  += delta_drpd_pkt;

        LOG_MSG_INFO1_2("ps_iface_check_data_activity(): Rx_delta: %d,"
                         " Drpd_delta: %d", delta_rx_pkt, delta_drpd_pkt );

        if(delta_rx_pkt != delta_drpd_pkt)
        {
          /* DL packets are not equal to dropped, consumed somewhere */
          break;
        }
      }

    }	
  }

  /* check DL and UL packet activity variables to see any data activity 
             on all IFACES */
  if( TRUE == dl_activity)
  { 
    /* Ccheck if all the DL packets are dropped which means spurious data
       so restart Timer_2 to confirm no more packets are coming else could
       lead to signalling overhead */
     LOG_MSG_INFO1_2("ps_iface_check_data_activity(): Total_Rx_delta: %d,"
                                   " Total_Drpd_delta: %d", total_delta_rx,
                                                       total_delta_dropd );
     
    if((total_delta_rx == total_delta_dropd )&&
       (ps_sys_fast_dormancy_is_timer_running(PS_SYS_FAST_DORMANCY_TIMER_1)
                                                                   == TRUE))
    {
      /* there is some spurious activity happening, so dont stop
         fast dormancy and restart timer_2 */
      *stop_fast_dormancy = -1;
    }
  }

  /* return 0 if data activity is present */
  if (ul_activity || dl_activity)
  {   
    LOG_MSG_INFO1_2("ps_iface_check_data_activity(): UL_activity: %d,"
                           " DL_Activity: %d", ul_activity, dl_activity );
    return 0;
  }
  return -1;
}/* ps_iface_fast_dormancy_check_data_activity */

/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_FREE_IFACE_STATS_TBL()

DESCRIPTION
  This function frees the dynamic memory allocated for IFACE Stats table for
  fast dormancy and initializes to NULL

PARAMETERS
None

RETURN VALUE
None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_fast_dormancy_free_iface_stats_tbl()
{
  uint8                                  loop;
  ps_iface_fast_dormancy_stats_info_s   *stats_info_p = NULL;
  uint8                                  max_loop_value;
  /*---------------------------------------------------------------------- */

  max_loop_value = fd_stats_tbl_idx;
  LOG_MSG_INFO1_1("ps_iface_fast_dormancy_free_iface_stats_tbl():"
                                             "max_idx: %d", max_loop_value);

  /* Free the allocated heap memory before stopping FD	*/
  for (loop = 0;loop < max_loop_value ;loop++)
  {
    stats_info_p = fd_stats_tbl[loop];
		
    if(stats_info_p == NULL)
    {
      LOG_MSG_INFO1_0("ps_iface_free_all_iface_stats(): NULL Stat info ptr");
      continue;
    }
    PS_SYSTEM_HEAP_MEM_FREE(stats_info_p);
    fd_stats_tbl[loop] = NULL;
  }

  /* resetting the array index to 0 */
   fd_stats_tbl_idx = 0;
  
  return;
}/* ps_iface_free_iface_stats_tbl */

