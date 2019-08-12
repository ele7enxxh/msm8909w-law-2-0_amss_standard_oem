/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR System Information Module

GENERAL DESCRIPTION
   This module contains functions to handle system information message processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_sys_info.c_v   1.14   17 Jul 2002 11:50:42   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
Jul/10/12   sjw     Reinstate blocked interface detection after ignored sys info
May/26/11   sjw     Use SI4 to update RR acquisition database
Jun/09/10   sjw     Ignore dedicated SIs whilst RR state is W2GHO_PROGRESS
May/19/09   sjw     Move dedicated sys info cache into dedicated data storage
May/17/04   VT      In rr_store_system_info(), changed the ASSERT to MSG_ERROR
                      for generated_event_count >= RR_SYS_INFO_MAX_GENERATED_EVENT_COUNT
04/29/04    gc      CR42000: Fixes to handle Early Classmark Sending Restriction
01/07/04    gc      Added SI optimisation changes
02/07/03    he      Copied re-establishment allowed field to SI info db.
02/05/03    JAC     lint changes
01/27/03    JAC     Saving cell channel description to SI_cell_channel_description
12/13/02    RG      Fixed syntax error at line 1385.
12/4/02     RG      Removed code that ignored processing SI2quater in GSM only mode.
                    Added check to send 3G neighbor list to L1 only when in non-GSM only mode.
11/18/02    RG      Fixes to problems found when unit testing GSM to WCDMA reselection.
11/08/02    he      Replaced rr_sys_info_msg_processed() by rr_gprs_sys_info_post_processing()
10/24/02    JAC     Cell selection parameters not cleared if not present in SI4
10/03/02    RG     Added InterRAT Reselection ( GSM to WCDMA ) Support.
09/26/02    rmc     Add F3 message to show PLMN when SI3 is received
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/07/02    he      Determine if SI1 messages not being sent and stop waiting for it.
07/08/02    rmc     Using accessors for multiband information
07/07/02    JAC     Support for control_channel_description in pending and SI db
06/25/02    JAC     Updates BA list if it see a new freq broadcasted
06/19/02    JAC     Save control channel description in surr DB
05/30/02    rmc     Compilation warnings for MULTIBAND_SUPPORT flag
05/22/02    rmc     Changes to MM-RR ICD for dual mode
04/11/02    JAC     BA list saved only if on serving cell
04/08/02    rmc     Dual Mode Cell Selection Changes
03/28/02    rmc     Removed references to KOVU symbol
02/21/02    rmc     Changes for CR20636, RR should read BCCH/SI continuously while
                    in paging reorganization paging mode
02/11/02    rmc     QXDM Cleanup
12/21/01    RMC     Fix for notifying L1 when SysInfo 6 changes in dedicated mode
11/13/01    RMC     Fix several problems found during unit testing
12/11/00    TS      Initial Revision.

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "amssassert.h"
#include <memory.h>
#include "rr_seg_load.h"
#include "rr_sys_info.h"
#include "rr_decode_ie.h"
#include "rr_init.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "rr_control.h"
#include "rr_general.h"
#include "rr_l2.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_log.h"             // For logging
#include "rr_shared_cell_selection.h"
#include "rr_sys_info_3g.h"
#include "rr_candidate_cell_db.h"
#include "rr_inter_ho.h"
#include "rr_resel_utils.h"
#include "rr_gprs_defs.h"
#include "csn.h"
#include "rr_ded_meas_reporting.h"
#include "rr_sys_info_i.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_si.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#ifdef FEATURE_GSM_DTM
#include "rr_nv.h"
#include "rr_procedure.h"
#endif /* FEATURE_GSM_DTM */
#include "rr_acq_db.h"
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_callback.h"
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/
#include "rr_resel_nacc.h"
#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_decode_bcch_if.h"
#include "rr_femto_if.h"
#else
#include "rr_decode_bcch.h"
#endif // FEATURE_FEMTO_GSM_NL
#include "rr_if.h"
#include "rr_ded_config.h"
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
#include "rr_w2g_service_redirection.h"
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
#include "rr_multi_sim.h"
#include "rr_sys_info_cache.h"
#include "stringl/stringl.h"
#include "rr_conn_establish.h"
#include "rr_cell_selection.h"
#include "rr_select_bcch.h"
#ifdef FEATURE_FAKE_BTS_CELL_INFO
#include "rr_msgr.h"
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/
#include "rr_gprs_dsc.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */
#ifdef FEATURE_QSH_EVENT_METRIC
#include "rr_qsh_metric.h"
#endif // FEATURE_QSH_EVENT_METRIC

#define SI_NOT_EXPECTED_ON_TC0(mt) (\
((mt)==SYSTEM_INFORMATION_TYPE_2)        ||\
((mt)==SYSTEM_INFORMATION_TYPE_2_BIS)    ||\
((mt)==SYSTEM_INFORMATION_TYPE_2_TER)    ||\
((mt)==SYSTEM_INFORMATION_TYPE_2_QUATER) ||\
((mt)==SYSTEM_INFORMATION_TYPE_3)        ||\
((mt)==SYSTEM_INFORMATION_TYPE_4)        ||\
((mt)==SYSTEM_INFORMATION_TYPE_7)        ||\
((mt)==SYSTEM_INFORMATION_TYPE_8))

#define BA_LIST_NOT_CHANGED 0x0000
#define BA_LIST_HAS_CHANGED 0x0001
#define BA_IND_FLAG_CHANGED 0x0002

/* define the amount of time it takes to receive 3 full sys info TC cycles:
 * this equates to (3 * (8 * 51)) frames, which is (1224 * 4.615) milliseconds
 */
#define THREE_FULL_TC_CYCLES 5649

/* define the worst case time gaps between Si3 and Si2ter reception:
 * for these calcs, assume Si2ter is on TC=5 (it can be earlier, on TC=4).
 * When Si3 is on TC=2, the gap to the next TC=5 is 3 multiframes (3*51*4.615)
 * When Si3 is on TC=6, the gap to the next TC=5 is 7 multiframes (7*51*4.615)
 */
#define GAP_TO_SI2TER_WHEN_SI3_ON_TC2  707
#define GAP_TO_SI2TER_WHEN_SI3_ON_TC6 1648

/* CSN Decoder defines */
//#define CSN_BCCH_NORMAL    0
#define CSN_BCCH_EXTENDED  1

typedef struct
{
/**
 * When doing a handover, store BA lists from SI5 variants received from old
 * cell. After a handover, RR always sends full BA list to layer1 for which
 * it uses SI5 variants received from previous cell until all SI5s are
 * received from new cell.
 *
 * This is to stop layer1 from deleting BA list plus all sync info when it
 * receives partial dedicated BA list from RR.
 */
  BCCH_frequency_list_T    old_SI5_BA_list;
  BCCH_frequency_list_T    old_SI5bis_BA_list;
  BCCH_frequency_list_T    old_SI5ter_BA_list;
  uint8                    si5_count;
  boolean                  old_cell_si5_ba_lists_used;
} rr_sys_info_data_t;

static rr_sys_info_data_t  rr_sys_info_data[NUM_GERAN_DATA_SPACES];

static boolean rr_should_ignore_si2ter(const gas_id_t gas_id);
static boolean rr_handle_si2ter_indicator(boolean, byte, gprs_scell_info_t *, const gas_id_t gas_id);
static boolean rr_handle_si2quater_indicator(boolean, boolean, gprs_scell_info_t *, const gas_id_t gas_id);

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_sys_info_data_t*
 */
static rr_sys_info_data_t *rr_sys_info_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_sys_info_data[as_index];
}

/**
 *  FUNCTION      rr_cache_dedicated_sys_info_message
 *
 *  DESCRIPTION   Stores dedicated SIs (DL_UNIT_DATA_IND format) in a local cache
 *
 *  DEPENDENCIES  None.
 *
 *  RETURN VALUE  VOID
 *
 */
static void rr_cache_dedicated_sys_info_message(
  uint8   msg_type,
  ARFCN_T serving_ARFCN,
  dl_unit_data_ind_T *msg_ptr,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;
  uint8                cache_pos;

  switch(msg_type)
  {
  case SYSTEM_INFORMATION_TYPE_5:
    cache_pos = 0;
    break;

  case SYSTEM_INFORMATION_TYPE_5_BIS:
    cache_pos = 1;
    break;

  case SYSTEM_INFORMATION_TYPE_5_TER:
    cache_pos = 2;
    break;

  case SYSTEM_INFORMATION_TYPE_6:
    cache_pos = 3;
    break;

  default:
    /* message type is not relevant, do not store it */
    cache_pos = RR_MAX_DED_SIS;
    break;
  }

  /* obtain pointer to global dedicated mode storage */
  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

  if ((dedic_ptr != NULL) && (msg_ptr != NULL) && (cache_pos < RR_MAX_DED_SIS))
  {
    memscpy(&(dedic_ptr->sys_info_cache.msg_data[cache_pos]),
            (sizeof(dedic_ptr->sys_info_cache.msg_data)-(cache_pos*sizeof(dedic_ptr->sys_info_cache.msg_data[0]))),
            msg_ptr,
            sizeof(dl_unit_data_ind_T));    
    /* store the supplied ARFCN : not normally provided by L2 on SACCH */
    dedic_ptr->sys_info_cache.msg_data[cache_pos].channel_number = serving_ARFCN;
    dedic_ptr->sys_info_cache.msg_rcvd[cache_pos] = TRUE;
  }

  return;
}

/**
 *  FUNCTION      rr_restore_dedicated_sys_info
 *
 *  DESCRIPTION   Posts caches SIs back to the RR task queue
 *
 *  DEPENDENCIES  None.
 *
 *  RETURN VALUE  VOID
 *
 */
void rr_restore_dedicated_sys_info(
  ARFCN_T serving_ARFCN,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;
  dl_unit_data_ind_T  *msg_ptr;
  uint8                index;

  /* obtain pointer to global dedicated mode storage */
  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Unable to restore dedicated sys info");
    return;
  }

  MSG_GERAN_HIGH_1_G("Restoring dedicated sys info for ARFCN %d...",
           (int)serving_ARFCN.num);

  for (index = 0; index < RR_MAX_DED_SIS; index++)
  {
    msg_ptr = &(dedic_ptr->sys_info_cache.msg_data[index]);

    if (dedic_ptr->sys_info_cache.msg_rcvd[index] == TRUE)
    {
      /* if the ARFCN matches the request, process the cached msg */
      if (ARFCNS_EQUAL(msg_ptr->channel_number, serving_ARFCN))
      {
        /* post the cached DL_UNIT_DATA_IND back to the task queue */
        if (gs_send_message(GS_QUEUE_RR, msg_ptr, TRUE) != GS_SUCCESS)
        {
          MSG_GERAN_ERROR_0_G("Unable to restore dedicated SI in RR task queue");
          break;
        }
      }
      else
      {
        MSG_GERAN_HIGH_3_G("Cached sys info 0x%02X 0x%02X is for different ARFCN (%d)",
                 (int)msg_ptr->layer3_message[0],
                 (int)msg_ptr->layer3_message[1],
                 (int)msg_ptr->channel_number.num);
      }
    }
  }

  return;
}

/**
 *  FUNCTION      rr_clear_dedicated_sys_info_cache
 *
 *  DESCRIPTION   Reset the dedicated SI cache in this module
 *
 *  DEPENDENCIES  None.
 *
 *  RETURN VALUE  VOID
 *
 */
void rr_clear_dedicated_sys_info_cache(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;

  /* obtain pointer to global dedicated mode storage */
  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    uint8 index;

    for (index = 0; index < RR_MAX_DED_SIS; index++)
    {
      dedic_ptr->sys_info_cache.msg_data[index].channel_number.num = INVALID_ARFCN;
      dedic_ptr->sys_info_cache.msg_rcvd[index] = FALSE;
    }
  }

  return;
}

static void rr_clear_ba_list(
  BCCH_frequency_list_T *ba_list
)
{
  int i;

  ba_list->no_of_entries = 0;
  ba_list->BA_IND = 0;

  for (i = 0; i < BA_MAX_LEN; i++)
  {
    ba_list->BCCH_ARFCN[i].num = INVALID_ARFCN;
    ba_list->BCCH_ARFCN[i].band = INVALID_BAND;
  }
} /* end rr_clear_ba_list() */

/**
 *  FUNCTION      rr_reset_old_si5_ba_list()
 *
 *  DESCRIPTION   Resets requested si5 ba lists.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  None.
 *
 */

void rr_reset_old_si5_ba_list(
  uint16 ba_list_mask,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  if(ba_list_mask & MASK_SI_5)
  {
    memset(&rr_sys_info_data_ptr->old_SI5_BA_list, 0, sizeof(BCCH_frequency_list_T));
    MSG_GERAN_HIGH_0_G("Resetting old SI5 ba list");
  }
  if(ba_list_mask & MASK_SI_5_BIS)
  {
    memset(&rr_sys_info_data_ptr->old_SI5bis_BA_list, 0, sizeof(BCCH_frequency_list_T));
    MSG_GERAN_HIGH_0_G("Resetting old SI5bis ba list");
  }
  if(ba_list_mask & MASK_SI_5_TER)
  {
    memset(&rr_sys_info_data_ptr->old_SI5ter_BA_list, 0, sizeof(BCCH_frequency_list_T));
    MSG_GERAN_HIGH_0_G("Resetting old SI5ter ba list");
  }
} /* rr_reset_old_si5_ba_list()*/


/**
 * Swap the old SI5 and SI5ter BA lists.
 */
void rr_swap_old_si5_and_si5ter_ba_lists(
  const gas_id_t gas_id
)
{
  BCCH_frequency_list_T tmp;

  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  MSG_GERAN_HIGH_0_G("Swapping old SI5 and SI5ter BA lists");
  tmp = rr_sys_info_data_ptr->old_SI5_BA_list;
  rr_sys_info_data_ptr->old_SI5_BA_list = rr_sys_info_data_ptr->old_SI5ter_BA_list;
  rr_sys_info_data_ptr->old_SI5ter_BA_list = tmp;
}



/**
 *  FUNCTION      rr_update_old_si5_ba_list()
 *
 *  DESCRIPTION   Updates si5 variant ba list.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  None.
 *
 */

void rr_update_old_si5_ba_lists(
  BCCH_frequency_list_T *si5_ba_list_ptr,
  BCCH_frequency_list_T *si5bis_ba_list_ptr,
  BCCH_frequency_list_T *si5ter_ba_list_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  if(si5_ba_list_ptr != NULL)
  {
    rr_sys_info_data_ptr->old_SI5_BA_list    = *si5_ba_list_ptr;
    MSG_GERAN_HIGH_0_G("Updating old SI5 ded ba list");
  }

  if(si5bis_ba_list_ptr != NULL)
  {
    rr_sys_info_data_ptr->old_SI5bis_BA_list = *si5bis_ba_list_ptr;
    MSG_GERAN_HIGH_0_G("Updating old SI5bis ded ba list");
  }

  if(si5ter_ba_list_ptr != NULL)
  {
    rr_sys_info_data_ptr->old_SI5ter_BA_list = *si5ter_ba_list_ptr;
    MSG_GERAN_HIGH_0_G("Updating old SI5ter ded ba list");
  }

} /* rr_update_old_si5_ba_list()*/



/**
 *  FUNCTION      rr_get_old_si5_ba_list()
 *
 *  DESCRIPTION   Get si5 variant ba list.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  Requested ba list is returned.
 *
 */

BCCH_frequency_list_T rr_get_old_si5_ba_list(
  uint16 si5_variant,
  const gas_id_t gas_id
)
{
  BCCH_frequency_list_T ba_list;

  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  switch(si5_variant)
  {
    case MASK_SI_5:
    {
      MSG_GERAN_HIGH_0_G("Returning old SI5 ba list");
      ba_list = rr_sys_info_data_ptr->old_SI5_BA_list;
      break;
    }
    case MASK_SI_5_BIS:
    {
      MSG_GERAN_HIGH_0_G("Returning old SI5bis ba list");
      ba_list = rr_sys_info_data_ptr->old_SI5bis_BA_list;
      break;
    }
    case MASK_SI_5_TER:
    {
      MSG_GERAN_HIGH_0_G("Returning old SI5ter ba list");
      ba_list = rr_sys_info_data_ptr->old_SI5ter_BA_list;
      break;
    }
    default:
    {
      MSG_GERAN_HIGH_0_G("Wrong ba list type");
      rr_clear_ba_list(&ba_list);
      break;
    }
  }

  return ba_list;

} /* rr_get_old_si5_ba_list()*/



/**
 *  FUNCTION      void rr_increment_si5_count()
 *
 *  DESCRIPTION   Increments si5_count value by one.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  None.
 *
 */

void rr_increment_si5_count(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  rr_sys_info_data_ptr->si5_count++;
  MSG_GERAN_HIGH_1_G("SI5 count incremented to: %d",rr_sys_info_data_ptr->si5_count);
} /* rr_increment_si5_count()*/


/**
 *  FUNCTION      uint8 rr_get_si5_count(void)
 *
 *  DESCRIPTION   Returns si5_count value.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  si5_count.
 *
 */

uint8 rr_get_si5_count(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  MSG_GERAN_HIGH_1_G("Current SI5 count is: %d", rr_sys_info_data_ptr->si5_count);
  return rr_sys_info_data_ptr->si5_count;
}


/**
 *  FUNCTION      void rr_reset_si5_count(void)
 *
 *  DESCRIPTION   Resets si5_count to 0.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  None.
 *
 */

void rr_reset_si5_count(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  MSG_GERAN_HIGH_0_G("SI5 count is reset");
  rr_sys_info_data_ptr->si5_count = 0;
}


/**
 *  FUNCTION      void rr_set_status_of_old_cell_si5_ba_lists(boolean flag)
 *
 *  DESCRIPTION   Sets status of old cell's (cell before handover) si5 ba list
 *                status. Set to TRUE after receiving a MPH_HANDOVER_IND. Set
 *                to FALSE once RR_SI5_COUNT_BEFORE_RESET no of SI5s have been
 *                received after a handover.
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  None.
 *
 */

void rr_set_status_of_old_cell_si5_ba_lists(
  boolean flag,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  MSG_GERAN_HIGH_1_G("Old cell SI5 BA Lists status set to: %d",(int)flag);
  rr_sys_info_data_ptr->old_cell_si5_ba_lists_used = flag;
}


/**
 *  FUNCTION      boolean rr_get_status_of_old_cell_si5_ba_lists(void)
 *
 *  DESCRIPTION   Get status of old cell's (cell before handover) si5 ba lists.
 *
 *
 *  DEPENDECIES   None.
 *
 *  RETURN VALUE  TRUE/FALSE.
 *
 */

boolean rr_get_status_of_old_cell_si5_ba_lists(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_sys_info_data_t *rr_sys_info_data_ptr = rr_sys_info_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_data_ptr);

  MSG_GERAN_HIGH_3_G("Old cell SI5 BA Lists status is:%d",(int)rr_sys_info_data_ptr->old_cell_si5_ba_lists_used,
                                              0,
                                             0);
  return rr_sys_info_data_ptr->old_cell_si5_ba_lists_used;
}



/**
 * Determines if the rach control parameters have changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param RACH_control_parameters_ptr
 *                        Pointer to RACH Control Parameters IE to compare against DB
 *
 * @param sys_info_db_ptr pointer to system information database to compare against new parameters
 *
 * @param notify_L1_ptr   [out] set to TRUE if L1 should be notified, otherwise not modified
 *
 * @param notify_MM_ptr   [out] set to TRUE if MM should be notified, otherwise not modified
 */
static void rr_check_for_rach_control_parameters_change(
  RACH_control_parameters_T     *RACH_control_parameters_ptr,
  system_information_database_T *sys_info_db_ptr,
  boolean                       *notify_L1_ptr,
  boolean                       *notify_MM_ptr
)
{
  if (sys_info_db_ptr->RACH_control_parameters.REESTABLISHMENT_ALLOWED !=
      RACH_control_parameters_ptr->REESTABLISHMENT_ALLOWED)
  {
    *notify_MM_ptr = TRUE;
  }

  /* if the access class information has changed */
  if ((sys_info_db_ptr->RACH_control_parameters.access_control[0] !=
       RACH_control_parameters_ptr->access_control[0])                  ||
      (sys_info_db_ptr->RACH_control_parameters.access_control[1] !=
       RACH_control_parameters_ptr->access_control[1])
     )
  {
    /* MM needs to be notified of the change */
    *notify_MM_ptr = TRUE;
  }

  if (sys_info_db_ptr->RACH_control_parameters.tx_integer !=
      RACH_control_parameters_ptr->tx_integer)
  {
    *notify_L1_ptr = TRUE;
  }
} /* rr_check_for_rach_control_parameters_change */

/**
 * Determines if the NCC permitted values have changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param NCC_permitted   new NCC permitted values
 *
 * @param sys_info_db_ptr pointer to system information database to compare against new parameters
 *
 * @param notify_L1_ptr   [out] set to TRUE if L1 should be notified, otherwise not modified
 *
 * @param notify_MM_ptr   [out] set to TRUE if MM should be notified, otherwise not modified
 */
static void rr_check_for_ncc_permitted_change(
  byte                          NCC_permitted,
  system_information_database_T *sys_info_db_ptr,
  boolean                       *notify_L1_ptr
)
{
  if (sys_info_db_ptr->NCC_permitted != NCC_permitted)
  {
    *notify_L1_ptr = TRUE;
  }
  /*MM does not care about NCC permitted so it does not need to be notified*/

} /* rr_check_for_ncc_permitted_change */

/**
 * Determines if the control channel description parameters have changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param control_channel_description_ptr
 *                        pointer to received control channel description values
 *
 * @param sys_info_db_ptr pointer to system information database to compare against new parameters
 *
 * @param notify_L1_ptr   [out] set to TRUE if L1 should be notified, otherwise not modified
 *
 * @param notify_MM_ptr   [out] set to TRUE if MM should be notified, otherwise not modified
 */
static void rr_check_for_control_channel_description_change(
  control_channel_description_T *control_channel_description_ptr,
  system_information_database_T *sys_info_db_ptr,
  boolean                       *notify_L1_ptr,
  boolean                       *notify_MM_ptr
)
{
  if ((sys_info_db_ptr->control_channel_description.ATT != control_channel_description_ptr->ATT) ||
      (sys_info_db_ptr->control_channel_description.T3212_timeout !=
       control_channel_description_ptr->T3212_timeout)
     )
  {
    *notify_MM_ptr = TRUE;
  }

  if ((sys_info_db_ptr->control_channel_description.BS_PA_MFRMS != control_channel_description_ptr->BS_PA_MFRMS) ||
      (sys_info_db_ptr->control_channel_description.BS_AG_BLKS_RES != control_channel_description_ptr->BS_AG_BLKS_RES) ||
      (sys_info_db_ptr->control_channel_description.BS_CC_CHANS != control_channel_description_ptr->BS_CC_CHANS) ||
      (sys_info_db_ptr->control_channel_description.BS_CCCH_SDCCH_COMB != control_channel_description_ptr->BS_CCCH_SDCCH_COMB)
     )
  {
    *notify_L1_ptr = TRUE;
  }
} /* rr_check_for_control_channel_description_change */

/**
 * Determines if the cell options parameters have changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param cell_options_ptr
 *                        pointer to received cell options values
 *
 * @param sys_info_db_ptr pointer to system information database to compare against new parameters
 *
 * @param notify_L1_ptr   [out] set to TRUE if L1 should be notified, otherwise not modified
 *
 * @param notify_MM_ptr   [out] set to TRUE if MM should be notified, otherwise not modified
 */
static void rr_check_for_cell_options_change(
  cell_options_T                *cell_options_ptr,
  system_information_database_T *sys_info_db_ptr,
  boolean                       *notify_L1_ptr,
  boolean                       *notify_MM_ptr
)
{
  if ((sys_info_db_ptr->cell_options.PWRC != cell_options_ptr->PWRC))
  {
    *notify_L1_ptr = TRUE;
  }
  (void) notify_MM_ptr;
} /* rr_check_for_cell_options_change */

/**
 * Determines if the cell options parameters have changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param cell_options_ptr
 *                        pointer to received cell options values
 *
 * @param sys_info_db_ptr pointer to system information database to compare against new parameters
 *
 * @param notify_L1_ptr   [out] set to TRUE if L1 should be notified, otherwise not modified
 *
 * @param notify_MM_ptr   [out] set to TRUE if MM should be notified, otherwise not modified
 */
static void rr_check_for_cbch_parameters_change(
  channel_information_T         *cbch_param_ptr,
  system_information_database_T *sys_info_db_ptr,
  boolean                       *notify_L1_ptr,
  boolean                       *notify_MM_ptr
)
{
  if ((sys_info_db_ptr->cbch_channel_information.channel_type != cbch_param_ptr->channel_type) ||
    (sys_info_db_ptr->cbch_channel_information.hopping_flag != cbch_param_ptr->hopping_flag) ||
    (sys_info_db_ptr->cbch_channel_information.HSN != cbch_param_ptr->HSN) ||
      (sys_info_db_ptr->cbch_channel_information.MAIO != cbch_param_ptr->MAIO) ||
    (sys_info_db_ptr->cbch_channel_information.subchannel != cbch_param_ptr->subchannel) ||
    (sys_info_db_ptr->cbch_channel_information.TN != cbch_param_ptr->TN) ||
    (sys_info_db_ptr->cbch_channel_information.TSC != cbch_param_ptr->TSC)
    )
  {
    *notify_L1_ptr = TRUE;
    // *notify_MM_ptr = TRUE; TBD
  }
  (void) notify_MM_ptr;

} /* rr_check_for_cell_options_change */

/**
 * Determines if the cell options parameters have changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param cell_selection_parameters_ptr
 *                        pointer to received cell options values
 *
 * @param sys_info_db_ptr pointer to system information database to compare against new parameters
 *
 * @param notify_L1_ptr   [out] set to TRUE if L1 should be notified, otherwise not modified
 *
 * @param notify_MM_ptr   [out] set to TRUE if MM should be notified, otherwise not modified
 */
static void rr_check_for_cell_selection_parameters_change(
  cell_selection_parameters_T   *cell_selection_parameters_ptr,
  system_information_database_T *sys_info_db_ptr,
  boolean                       *notify_L1_ptr,
  boolean                       *notify_MM_ptr
)
{
  if ((sys_info_db_ptr->cell_selection_parameters.MS_TXPWR_MAX_CCH !=
       cell_selection_parameters_ptr->MS_TXPWR_MAX_CCH)
     )
  {
    *notify_L1_ptr = TRUE;
  }
  (void) notify_MM_ptr;
} /* rr_check_for_cell_selection_parameters_change */

/**
 * Determines if the cell identity has changed
 * and whether L1 or MM need to be notified of the change
 *
 * @param cell_identity     received cell identity value
 *
 * @param sys_info_db_ptr   pointer to system information database to compare against new parameters
 *
 * @return TRUE if the Cell Identity has changed; FALSE
 *         otherwise
 */
static boolean rr_check_for_cell_identity_change(
  word                          cell_identity,
  system_information_database_T *sys_info_db_ptr
)
{
  if (sys_info_db_ptr->cell_identity != cell_identity)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* rr_check_for_cell_identity_change */


/**
 * Notifies MM or L1 (if required based on flags) of a change
 * in the SI for the serving cell
 *
 * @param notify_L1 TRUE if L1 needs to be notified
 *
 * @param notify_MM TRUE if MM needs to be notified
 */
void rr_perform_MM_or_L1_notification_on_serving_SI_change(
  boolean notify_L1,
  boolean notify_MM,
  const gas_id_t gas_id
)
{
  /* if the MM should be notified of a change in SI */
  if (notify_MM == TRUE)
  {
    grr_update_mm_with_rr_service_ind(gas_id);
  }

  /* if L1 should be notified of a change in SI */
  if (notify_L1 == TRUE)
  {
    rr_gprs_update_l1_params(gas_id);
  }
} /* rr_perform_MM_or_L1_notification_on_serving_SI_change */

/**
 * Copies the cell reselection variables from 1 struct to another
 *
 * @param destination - Pointer to struct to copy to
 * @param source - Pointer to struct to copy from
 *
 */
static void rr_copy_si4_si7_si8_cell_reselection_param(
  cell_selection_parameters_T* destination,
  cell_selection_parameters_T* source)
{
  destination->CELL_RESELECT_PARAM_IND = source->CELL_RESELECT_PARAM_IND;
  destination->CELL_BAR_QUALIFY = source->CELL_BAR_QUALIFY;
  destination->CELL_RESELECT_OFFSET = source->CELL_RESELECT_OFFSET;
  destination->TEMPORARY_OFFSET = source->TEMPORARY_OFFSET;
  destination->PENALTY_TIME = source->PENALTY_TIME;
  destination->power_offset_valid = source->power_offset_valid;
  destination->POWER_OFFSET = source->POWER_OFFSET;
}


/*===========================================================================

FUNCTION decode_si1_ro

DESCRIPTION
  Extract the contents of SI1 rest octets

PARAMS
  (in) ie_ptr - pointer to encoded message IE
  (out) nch_pos_ptr - pointer to storage for NCH Position
  (out) band_ind_ptr - pointer to storage for Band Indicator

RETURN VALUE
  0 if decoding was successful (at present this is always the case)

===========================================================================*/

static void decode_si1_ro(
  uint8 *ie_ptr,
  uint8 *nch_pos_ptr,
  uint8 *band_ind_ptr,
  const gas_id_t gas_id
)
{
  si1_rest_octets_t rest_octets;

  uint8 remaining_length = 0;

  RR_ZERO_STRUCTURE(rest_octets);

  remaining_length = rr_length_to_end_of_ota_msg(
    SYSTEM_INFORMATION_TYPE_1,    // msg_type
    ie_ptr,                       // csn1_start_ie_ptr
    SI1_REST_OCTETS,              // csn_ie_type
    gas_id
  );

  (void) gcsn1_rr_decoder(
    SI1_REST_OCTETS,    // message_type
    ie_ptr,             // csn1_ie_src
    &rest_octets,       // dst
    remaining_length,   // len_to_end_of_buf
    gas_id
  );

  if (rest_octets.nch_position_flag == valid)
  {
    *nch_pos_ptr = rest_octets.nch_position;
  }
  else
  {
    *nch_pos_ptr = RESERVED_NCH_POSITION; /* reserved value, treated as "NOT PRESENT" */
  }

  *band_ind_ptr = (uint8)rest_octets.band_indicator;

} /* end decode_si1_ro() */


/**
 * Handle system information 1 messages
 *
 * @param channel_number       Channel number where the message was received
 *
 */
static void rr_handle_system_info_1(
  ARFCN_T              channel_number,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  ie_descriptor_T               *current_IE;
  boolean                        decoding_ok = TRUE;
  cell_channel_description_T     cell_allocation;
  RACH_control_parameters_T      RACH_control_parameters;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  RR_ZERO_STRUCTURE(cell_allocation);
  RR_ZERO_STRUCTURE(RACH_control_parameters);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_1, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }
  else
  {
    /***************************************/
    /* Decode the cell channel description */
    /***************************************/
    rr_decode_cell_channel_description(
      current_IE->start_IE,&cell_allocation,
      &decoding_ok,
      gas_id
    );
  }

  /**************************************/
  /* Decode the RACH control parameters */
  /**************************************/
  if (decoding_ok == TRUE)
  {
    current_IE = current_IE->next_IE;
    rr_decode_RACH_control_parameters(current_IE->start_IE,&RACH_control_parameters);
  }

  if (decoding_ok == TRUE)
  {
    uint8 nch_pos;
    uint8 band_ind = 0;

    /* extract the fields from the SI1 Rest Octets - see 3GPP 04.18 section 10.5.2.32 */
    /* NCH Position is returned in raw (uninterpreted) format, or set to 0xFF if not present */
    /* Band Indicator will be 0 for DCS 1800 or 1 for PCS 1900 */
    current_IE = current_IE->next_IE;
    decode_si1_ro(current_IE->start_IE, &nch_pos, &band_ind, gas_id);

    MSG_GERAN_HIGH_2_G("SI1(%d) band indicator = %d",(int) channel_number.num, band_ind);

    /***********************************************************************/
    /* Only store in serving SI store if camped and channel number matches */
    /***********************************************************************/
    if (rr_is_camped(gas_id) &&
       (scell_info_ptr->gsm.BCCH_ARFCN.num == channel_number.num)
       && (scell_info_ptr->gsm.BCCH_ARFCN.band == channel_number.band)
      )
    {
      boolean notify_MM = FALSE;
      boolean notify_L1 = FALSE;
      MSG_GERAN_HIGH_1_G("Store SI1(%d) for serving cell",(int) channel_number.num);

      // Log the rach control parameters if different that those on serving cell
      rr_log_diff_rach_control_parameters(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_check_for_rach_control_parameters_change(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      /* store the new information */
      scell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
      scell_info_ptr->gsm.cell_channel_description = cell_allocation;
      scell_info_ptr->gsm.SI_cell_channel_description = cell_allocation;

      /* Store R99 band indicator in SI database. If not received, stores zero which
         is the default value */
      scell_info_ptr->gsm.band_indicator = (si_psi_band_ind_T)band_ind;
      /* indicate that a valid cell allocation has been received */
      scell_info_ptr->gsm.valid_CA_received = RR_CA_RECEIVED;

      /* notify MM or L1 if required */
      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        notify_L1,
        notify_MM,
        gas_id
      );

      /* SI1 has been received for serving cell */
      scell_info_ptr->gsm.update_message_received |= MASK_SI_1;

      if (scell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_1, 
                                  channel_number,
                                  message_ptr,
                                  &scell_info_ptr->gsm.location_area_identification,
                                  &scell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_1, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }
    }
    else if (pcell_info_ptr->gsm.BCCH_ARFCN.num == channel_number.num)
    {
      /**************************************************/
      /* We have to store the system information in the */
      /* pending_channel_data store                     */
      /**************************************************/

      /* Store R99 band indicator in pending SI database. This should be done prior
         to reassigning band ind in cell allocation list since rr_get_band() uses
         pending band ind when not camped */
      pcell_info_ptr->gsm.band_indicator = (si_psi_band_ind_T)band_ind;

      MSG_GERAN_HIGH_1_G("Store SI1(%d) in pending_channel_data",(int) channel_number.num);
      pcell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
      pcell_info_ptr->gsm.cell_channel_description = cell_allocation;

      pcell_info_ptr->gsm.valid_CA_received = RR_CA_RECEIVED;

      /* Band indicator is now available.
       * Reprocess cell allocation to include
       * correct band info for each ARFCN in the list
       */
      rr_gprs_reassign_band(CELL_ALLOC_FREQ_LIST, FALSE, gas_id);

      pcell_info_ptr->gsm.SI_cell_channel_description = pcell_info_ptr->gsm.cell_channel_description;

      /* SI1 has been received */
      pcell_info_ptr->gsm.update_message_received |= MASK_SI_1;

      if (pcell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_1, 
                                  channel_number,
                                  message_ptr,
                                  &pcell_info_ptr->gsm.location_area_identification,
                                  &pcell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_1, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }
    }
    else
    {
      /***************************/
      /* Just ignore the message */
      /***************************/
      MSG_GERAN_HIGH_1_G("SI1(%d) ignored : wrong channel number",(int) channel_number.num);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Problem decoding SI1(%d)",(int) channel_number.num);
  }

} /* rr_handle_system_info_1 */

/**
 * Checks to see if the complete BA list has been received (based
 * on received SI messages). If the complete BA list has now
 * been received (and it wasn't before) indicates that L1 should be
 * notified of the change in BA list status.
 *
 * Also sets the update_messages_received bitarray based on the
 * BA messages that are expected
 *
 * @param si_db_ptr     pointer to system information database for which the determination is being performed
 *
 * @param notify_L1_ptr [out] set to TRUE if the complete BA was just received (wasn't before), otherwise it is NOT modified
 *
 * @param EXT_IND_valid Indicates if the EXT_IND value is valid or not
 *
 * @param EXT_IND       EXT_IND value received in the message, should be set to 1 if not received
 */
static void rr_sys_info_check_for_complete_idle_BA_list_received(
  system_information_database_T *si_db_ptr,
  boolean                       *notify_L1_ptr,
  boolean                       EXT_IND_valid,
  byte                          EXT_IND,
  const gas_id_t                gas_id
)
{

  /* if the mobile only supports the PGSM band */
  if (rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id) && (rr_gapi_get_supported_band_count(gas_id) == 1))
  {
    /*
     * dont expect SI_2_BIS or SI_2_TER if PGSM is only supported band
     */
    si_db_ptr->decode_SI2bis = RR_SI_NOT_REQUIRED;
    si_db_ptr->decode_SI2ter = RR_SI_NOT_REQUIRED;
  }
  else /* mobile station does not only support PGSM band */
  {
    if (EXT_IND_valid)
    {
      if (EXT_IND == 0)
      {
        /* SI2bis is not being sent by the network */
        si_db_ptr->decode_SI2bis = RR_SI_NOT_REQUIRED;
      }
      else
      {
        /* SI2 bis is expected and should be decoded */
        si_db_ptr->decode_SI2bis = RR_SI_REQUIRED;
      }
    }

    /* if only DCS_1800 band is supported */
    if (rr_gapi_is_supported_band(SYS_BAND_DCS_1800, gas_id) && (rr_gapi_get_supported_band_count(gas_id) == 1))
    {
      /* no need to wait for SI2ter if only DCS band is supported */
      si_db_ptr->decode_SI2ter = RR_SI_NOT_REQUIRED;
    }
  }

  /*****************************************************************************/
  /* set appropriate values of update_message_received based on whether SI2bis */
  /* & SI2ter messages are expected or not                                     */
  /*****************************************************************************/
  if (si_db_ptr->decode_SI2bis == RR_SI_NOT_REQUIRED)
  {
    SET_BIT(MASK_SI_2_BIS,si_db_ptr->update_message_received);
  }

  if (si_db_ptr->decode_SI2ter == RR_SI_NOT_REQUIRED)
  {
    SET_BIT(MASK_SI_2_TER,si_db_ptr->update_message_received);
  }

  /* if RR does not already have the complete BA list */
  if (!(si_db_ptr->complete_idle_BA_list_received))
  {
    /* check if the complete BA list was received */

    /* if RR knows whether the decode of SI2bis/SI2ter is required or not */
    if ((si_db_ptr->decode_SI2bis != RR_SI_REQUIRE_UNKNOWN) &&
        (si_db_ptr->decode_SI2ter != RR_SI_REQUIRE_UNKNOWN)
       )
    {
      boolean SI2bisInfoReady = (si_db_ptr->decode_SI2bis == RR_SI_NOT_REQUIRED) ||
                         (ARE_SET(MASK_SI_2_BIS,si_db_ptr->update_message_received));

      boolean SI2terInfoReady = (si_db_ptr->decode_SI2ter == RR_SI_NOT_REQUIRED) ||
                         (ARE_SET(MASK_SI_2_TER,si_db_ptr->update_message_received));

      boolean SI2InfoReady = ARE_SET(MASK_SI_2,si_db_ptr->update_message_received);

      /* if the information from the three messages is ready for processing */
      if (SI2bisInfoReady && SI2terInfoReady && SI2InfoReady)
      {
        /* the complete BA has now been received */
        si_db_ptr->complete_idle_BA_list_received = TRUE;

        /* generate the complete idle BA list once again (just to be safe) */
        rr_generate_complete_idle_BA_list(
          si_db_ptr,
          IGNORE_RCVD_SI_MASK,
          gas_id
        );

        /*************************************************************************************/
        /* need to notify L1 since this is the first time the complete_idle_BA_list_received */
        /* is being set to true since it was last set to false                               */
        /*************************************************************************************/
        *notify_L1_ptr = TRUE;
      }
      /* otherwise, need to wait for more SI before BA list is complete */
    }
    /* otherwise, still don't know whether the SI messages are required or not */
    /* cannot declare BA as complete until this is known */
  }
  /* otherwise, no need to notify L1, only notify when the complete list is first received */
} /* rr_sys_info_check_for_complete_idle_BA_list_received */

/**
 * Checks to see if the BA list information has changed and
 * stores the new information in the given SI DB and BA List.
 *
 * @param si_db_ptr     pointer to system information database in which to store new information
 *
 * @param ba_list_ptr   [in/out] BA list to compare and in which to store the new information
 *
 * @param notify_L1_ptr [out] set to TRUE if L1 needs to be notified of the change in BA
 *
 * @return TRUE if the BA has changed, FALSE otherwise
 */

static uint16 rr_sys_info_check_for_idle_BA_change_and_update(
  system_information_database_T *si_db_ptr,
  BCCH_frequency_list_T         *ba_list_ptr,
  BCCH_frequency_list_T         *BCCH_frequency_list_ptr,
  boolean                       *notify_L1_ptr,
  const gas_id_t                 gas_id
)
{
  uint16 ba_list_has_changed = BA_LIST_NOT_CHANGED;

  ASSERT(si_db_ptr != NULL);
  ASSERT(ba_list_ptr != NULL);
  ASSERT(notify_L1_ptr != NULL);
  ASSERT(BCCH_frequency_list_ptr != NULL);

  /* if RR has valid data */
  if (si_db_ptr->valid_data_flag)
  {
    /* check for a change in BA_IND value in the complete BA list */
    if (si_db_ptr->complete_BA_list.BA_IND != BCCH_frequency_list_ptr->BA_IND)
    {
      ba_list_has_changed = BA_LIST_HAS_CHANGED | BA_IND_FLAG_CHANGED;

      /*BA ind has changed must clear entire ba list*/
      MSG_GERAN_HIGH_0_G("BA_IND Changed");

      /*BA IND is changed so NC FREQUNCY list is invalid. so reverting back to BA(GPRS)*/

      rr_gprs_revert_ba_gprs(gas_id);

      /******************************************************************************/
      /* When the BA_IND changes, the complete BA list must be rebuilt from scratch */
      /******************************************************************************/
      rr_reset_idle_BA_lists(si_db_ptr);

      /* ensure that SI2ter status is reinstated after the reset */
      if (si_db_ptr->two_ti == RR_SI2TER_EXPECTED)
      {
        MSG_GERAN_MED_0_G("SI2ter decode required");
        si_db_ptr->decode_SI2ter = RR_SI_REQUIRED;
      }

      /*******************************************************/
      /* Mark SI2, SI2bis and SI2ter and SI3 as not received */
      /*******************************************************/
      CLEAR_BIT(MASK_SI_2 | MASK_SI_2_BIS | MASK_SI_2_TER | MASK_SI_3,
                si_db_ptr->update_message_received);

       /* store new BA_IND for the complete BA list */
      si_db_ptr->complete_BA_list.BA_IND = BCCH_frequency_list_ptr->BA_IND;

      /* set the BA list */
      rr_set_BA_list(ba_list_ptr,BCCH_frequency_list_ptr);
    }
    else /* BA_IND has not changed */
    {
      /* check to see if BA list has changed */
      boolean ba_change = rr_check_BA_list_change(
        BCCH_frequency_list_ptr,
        ba_list_ptr,
        gas_id
      );

      /* if BA information has changed */
      if (ba_change)
      {
        ba_list_has_changed = BA_LIST_HAS_CHANGED;

        /*BA IND is not changed but BA LIST received is changed so NC FREQUNCY list is invalid. so reverting back to BA(GPRS)*/

        rr_gprs_revert_ba_gprs(gas_id);

        if(si_db_ptr->complete_idle_BA_list_received)
        {
          /* L1 needs to be notified */
          *notify_L1_ptr = TRUE;
        }

        /* set the new BA list value */
        rr_set_BA_list(ba_list_ptr,BCCH_frequency_list_ptr);

        /* generate the complete idle BA list once again */
        rr_generate_complete_idle_BA_list(si_db_ptr, IGNORE_RCVD_SI_MASK, gas_id);
      }
      /* otherwise, no change, nothing else needs to be done */
    }
  }
  else /* system_information_database doesn't yet have complete BA list */
  {
    /* set the new idle BA list */
    rr_set_BA_list(ba_list_ptr,BCCH_frequency_list_ptr);

    /* generate the complete idle BA list with whatever information is available */
    rr_generate_complete_idle_BA_list(si_db_ptr, IGNORE_RCVD_SI_MASK, gas_id);
  }

  return(ba_list_has_changed);
} /* rr_sys_info_check_for_idle_BA_change_and_update */

static void rr_gprs_nacc_check_ba_flags(
  BCCH_frequency_list_T *ba_list_ptr,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* check the BA_IND in the supplied structure (from Si2bis or Si2ter)   */
  /* to see if it has been flagged with an extra bit by the NACC routines */
  if (ba_list_ptr->BA_IND & 0x02)
  {
    if (scell_info_ptr->gsm.complete_BA_list.BA_IND < 2)
    {
      /* BA_IND has been updated, so must have received SI2 by now */
      /* most likely that it arrived on AGCH and mask was cleared  */
      MSG_GERAN_HIGH_0_G("NACC BA_IND flag set, SI2 must have been rcvd");
      scell_info_ptr->gsm.update_message_received |= MASK_SI_2;
    }
  }

  return;
}

/**
 * Called to process a System Information 2 message received from
 * the network. SI2 is expected for the serving and pending
 * cells only
 *
 * @param channel_number       channel number where the message was received
 *
 */
static void rr_handle_system_info_2(
  ARFCN_T              channel_number,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  ie_descriptor_T           * current_IE;
  boolean                     decoding_ok = TRUE;
  byte                        NCC_permitted = 0;
  BCCH_frequency_list_T       BCCH_frequency_list;
  RACH_control_parameters_T   RACH_control_parameters;
  byte                        EXT_IND = 0;
  gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  RR_ZERO_STRUCTURE(BCCH_frequency_list);
  RR_ZERO_STRUCTURE(RACH_control_parameters);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_2, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }

  if (decoding_ok == TRUE)
  {
    /*****************************************/
    /* Decode the Neighbour Cell Description */
    /*****************************************/
    rr_decode_neighbour_cell_description(
      current_IE->start_IE,
      &EXT_IND,
      &BCCH_frequency_list,
      &decoding_ok,
      gas_id
    );
  }

  if (decoding_ok == TRUE)
  {
    current_IE = current_IE->next_IE;
    NCC_permitted = (*(current_IE->start_IE));
  }

  if (decoding_ok == TRUE)
  {
    current_IE = current_IE->next_IE;
    rr_decode_RACH_control_parameters(current_IE->start_IE,&RACH_control_parameters);
  }

  if (decoding_ok == TRUE)
  {
    /**************************************************/
    /* We have to store the system information in the */
    /* system_information_database store              */
    /**************************************************/
    if (rr_is_camped(gas_id) &&
        (scell_info_ptr->gsm.BCCH_ARFCN.num == channel_number.num)
        && (scell_info_ptr->gsm.BCCH_ARFCN.band == channel_number.band)
        )
    {
      boolean notify_MM = FALSE;
      boolean notify_L1 = FALSE;
      boolean ba_changed = FALSE;
      uint16  ba_change_flags;

      MSG_GERAN_HIGH_1_G("Store SI2(%d) for serving cell",(int) channel_number.num);

      // Log RACH params if different from SIDB
      rr_log_diff_rach_control_parameters(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_check_for_rach_control_parameters_change(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      /* store the new RACH control parameters value */
      scell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;

      /* check the BA list for change and update with the new value */
      ba_change_flags = rr_sys_info_check_for_idle_BA_change_and_update(
        &scell_info_ptr->gsm,
        &(scell_info_ptr->gsm.SI2_BA_list),
        &BCCH_frequency_list,
        &notify_L1,
        gas_id
      );

      if (ba_change_flags & BA_LIST_HAS_CHANGED)
      {
        MSG_GERAN_HIGH_0_G("SI2 BA list has changed");
        ba_changed = TRUE;
      }

      /* check for a change in NCC permitted value */
      rr_check_for_ncc_permitted_change(
        NCC_permitted,
        &scell_info_ptr->gsm,
        &notify_L1
      );

      /* store new NCC permitted value */
      scell_info_ptr->gsm.NCC_permitted = NCC_permitted;

      /* SI2 has been received */
      SET_BIT(MASK_SI_2,scell_info_ptr->gsm.update_message_received);

      if (ba_changed)
      {
        /* generate the complete idle BA list with whatever info is available */
        rr_generate_complete_idle_BA_list(&scell_info_ptr->gsm, IGNORE_RCVD_SI_MASK, gas_id);
      }

      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      {
        boolean ba_complete = FALSE;

        rr_sys_info_check_for_complete_idle_BA_list_received(
          &scell_info_ptr->gsm,
          &ba_complete,
          TRUE,
          EXT_IND,
          gas_id
        );

        notify_L1 |= ba_complete;

        /* If the GSM BA list has changed, update the BA(GPRS) list */
        /* If L1 is going to be updated anyway due to other changes, don't update here, as the new */
        /* BA list will be given to L1 with the other changes */
        if ( ba_complete || ba_changed )
        {
          rr_gprs_gsm_ba_list_updated( (boolean) ( ! notify_L1 ), gas_id );
        }
      }

      /* notify L1 and/or MM if required */
      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        notify_L1,
        notify_MM,
        gas_id
      );

      if (scell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2, 
                                  channel_number,
                                  message_ptr,
                                  &scell_info_ptr->gsm.location_area_identification,
                                  &scell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }
    } /* SI is for serving cell */
    /**************************************************/
    /* We have to store the system information in the */
    /* pending_channel_data store                     */
    /**************************************************/
    else if (channel_number.num == pcell_info_ptr->gsm.BCCH_ARFCN.num)
    {
      boolean dummy_notify_L1;
      MSG_GERAN_HIGH_1_G("Store SI2(%d) in pending_channel_data",(int) channel_number.num);

      /* store the information directly, no need to check for change for pending data */
      pcell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;

      /* store the BA_IND for the complete BA list */
      pcell_info_ptr->gsm.complete_BA_list.BA_IND = BCCH_frequency_list.BA_IND;

      /* set the new SI2 BA list */
      rr_set_BA_list(&(pcell_info_ptr->gsm.SI2_BA_list),&BCCH_frequency_list);

      /* store the NCC permitted information */
      pcell_info_ptr->gsm.NCC_permitted = NCC_permitted;

      /* SI2 has been received */
      SET_BIT(MASK_SI_2,pcell_info_ptr->gsm.update_message_received);

      /* generate the complete idle BA list with whatever info is available */
      rr_generate_complete_idle_BA_list(
        &pcell_info_ptr->gsm,
        IGNORE_RCVD_SI_MASK,
        gas_id
      );

      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      rr_sys_info_check_for_complete_idle_BA_list_received(
        &pcell_info_ptr->gsm,
        &dummy_notify_L1,
        TRUE,
        EXT_IND,
        gas_id
      );

      if (pcell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2, 
                                  channel_number,
                                  message_ptr,
                                  &pcell_info_ptr->gsm.location_area_identification,
                                  &pcell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }

    } /* pending_channel_data */
    else
    {
      MSG_GERAN_HIGH_1_G("SI2(%d) ignored : wrong channel number",(int) channel_number.num);
    }
  } /* decoding_ok */
  else
  {
    ERR("Cannot decode SI2(%d)",(int) channel_number.num,0,0);
  }

} /* rr_handle_system_info_2 */

/**
 * Handle a received system information 2bis message from
 * the network. System information 2bis contains BA list
 * extensions (cases where BA list does not fit in SI2 message alone)
 *
 * @param channel_number The BCCH ARFCN where the message was received
 */
static void rr_handle_system_info_2_bis(
  ARFCN_T              channel_number,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  ie_descriptor_T           * current_IE;
  boolean                     decoding_ok = TRUE;
  RACH_control_parameters_T   RACH_control_parameters;
  BCCH_frequency_list_T       BCCH_frequency_list;
  byte                        EXT_IND = 0;
  gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  RR_ZERO_STRUCTURE(RACH_control_parameters);
  RR_ZERO_STRUCTURE(BCCH_frequency_list);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_2_BIS, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }

  if (decoding_ok == TRUE)
  {
    /*****************************************/
    /* Decode the Neighbour Cell Description */
    /*****************************************/
    rr_decode_neighbour_cell_description(
      current_IE->start_IE,
      &EXT_IND,
      &BCCH_frequency_list,
      &decoding_ok,
      gas_id
    );
  }

  if (decoding_ok == TRUE)
  {
    current_IE = current_IE->next_IE;
    rr_decode_RACH_control_parameters(current_IE->start_IE,&RACH_control_parameters);
  }

  if (decoding_ok == TRUE)
  {
    /**************************************************/
    /* We have to store the system information in the */
    /* system_information_database store              */
    /**************************************************/

    if (rr_is_camped(gas_id) &&
        (channel_number.num == scell_info_ptr->gsm.BCCH_ARFCN.num)
        && (channel_number.band == scell_info_ptr->gsm.BCCH_ARFCN.band)
        )
    {
      boolean notify_MM  = FALSE;
      boolean notify_L1  = FALSE;
      boolean ba_changed = FALSE;
      uint16  ba_change_flags;

      MSG_GERAN_HIGH_1_G("Store SI2bis(%d) for serving cell",channel_number.num);
      rr_gprs_nacc_check_ba_flags(&(scell_info_ptr->gsm.SI2bis_BA_list), gas_id);

      // Log RACH params if different from SIDB
      rr_log_diff_rach_control_parameters(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_check_for_rach_control_parameters_change(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      /* store the new RACH control parameters value */
      scell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;

      /* check the BA list for change and update with the new value */
      ba_change_flags = rr_sys_info_check_for_idle_BA_change_and_update(
        &scell_info_ptr->gsm,
        &(scell_info_ptr->gsm.SI2bis_BA_list),
        &BCCH_frequency_list,
        &notify_L1,
        gas_id
      );

      if (ba_change_flags & BA_LIST_HAS_CHANGED)
      {
        MSG_GERAN_HIGH_0_G("SI2bis BA list has changed");
        ba_changed = TRUE;
      }

      /* if the BA_IND changes, the BA lists are reset */
      if (ba_change_flags & BA_IND_FLAG_CHANGED)
      {
        /* ensure that the newly updated SI2bis info is used later */
        ba_change_flags = MASK_SI_2_BIS;
      }
      else
      {
        ba_change_flags = IGNORE_RCVD_SI_MASK;
      }

      /* SI2bis has been received */
      SET_BIT(MASK_SI_2_BIS,scell_info_ptr->gsm.update_message_received);

      if (ba_changed)
      {
        /* generate the complete idle BA list with whatever info is known so far */
        rr_generate_complete_idle_BA_list(
          &scell_info_ptr->gsm,
          ba_change_flags,
          gas_id
        );
      }

      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      {
        boolean ba_complete = FALSE;

        rr_sys_info_check_for_complete_idle_BA_list_received(
          &scell_info_ptr->gsm,
          &ba_complete,
          TRUE,
          EXT_IND,
          gas_id
        );

        notify_L1 |= ba_complete;

        /* If the GSM BA list has changed, update the BA(GPRS) list */
        /* If L1 is going to be updated anyway due to other changes, don't update here, as the new */
        /* BA list will be given to L1 with the other changes */
        if ( ba_complete || ba_changed )
        {
          rr_gprs_gsm_ba_list_updated( (boolean) ( ! notify_L1 ), gas_id );
        }
      }

      /* notify L1 and/or MM if required */
      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        notify_L1,
        notify_MM,
        gas_id
      );

      if (scell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_BIS, 
                                  channel_number,
                                  message_ptr,
                                  &scell_info_ptr->gsm.location_area_identification,
                                  &scell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_BIS, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }

    } /* SI is for the serving cell */
    /**************************************************/
    /* We have to store the system information in the */
    /* pending_channel_data store                     */
    /**************************************************/
    else if (channel_number.num == pcell_info_ptr->gsm.BCCH_ARFCN.num)
    {
      boolean dummy_notify_L1;
      MSG_GERAN_HIGH_1_G("Store SI2bis(%d) in pending_channel_data",(int) channel_number.num);

      /* store the information directly, no need to check for change for pending data */
      pcell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;

      /* store the BA_IND for the complete BA list */
      pcell_info_ptr->gsm.complete_BA_list.BA_IND = BCCH_frequency_list.BA_IND;

      /* set the new SI2bis BA list */
      rr_set_BA_list(&(pcell_info_ptr->gsm.SI2bis_BA_list),&BCCH_frequency_list);

      /* SI2bis has been received */
      SET_BIT(MASK_SI_2_BIS,pcell_info_ptr->gsm.update_message_received);

      /* generate the complete idle BA list with whatever info is known so far */
      rr_generate_complete_idle_BA_list(
        &pcell_info_ptr->gsm,
        IGNORE_RCVD_SI_MASK,
        gas_id
      );

      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      rr_sys_info_check_for_complete_idle_BA_list_received(
        &pcell_info_ptr->gsm,
        &dummy_notify_L1,
        TRUE,
        EXT_IND,
        gas_id
      );

      if (pcell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_BIS, 
                                  channel_number,
                                  message_ptr,
                                  &pcell_info_ptr->gsm.location_area_identification,
                                  &pcell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_BIS, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }
    } /* pending_channel_data */
    else
    {
      MSG_GERAN_HIGH_1_G("SI2bis(%d) ignored : wrong channel number",(int) channel_number.num);
    }
  }  /* decoding_ok */
  else
  {
    ERR("Cannot decode SI2bis(%d)",(int) channel_number.num,0,0);
  }

} /* rr_handle_system_info_2_bis */


/****************************************************************************
Function
rr_reset_SI2q_MI_status_flags

Description
Resets received and changed flags to FALSE in the instances array.

Params
[IN]
instances_T *instances

Returns
None
*****************************************************************************/

static void rr_reset_si2ter_status_flags (si2ter_instances_T *instances)
{
  byte i;

  ASSERT (instances != NULL);

  for (i = 0; i < MAX_SI2TER_INSTANCES; ++i)
  {
    instances->instance[i].changed = FALSE;
  }
}

/**
 * Handle a received system information 2ter message from
 * the network. System information 2ter contains BA list
 * extensions for multiband
 *
 * @param channel_number The BCCH ARFCN where the message was received
 * @param expected_si_received
 *                       [OUT] Set to TRUE if the sample timer is running
 *                       and all required SI messages have been received
 */
static void rr_handle_system_info_2_ter(
  ARFCN_T              channel_number,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  system_information_database_T *sys_info_db_ptr     = NULL;
  BCCH_frequency_list_T          BCCH_frequency_list;
  si2ter_instances_T            *si2ter_inst_ptr     = NULL;
  ie_descriptor_T               *current_IE          = NULL;
  boolean                        decoding_ok         = TRUE;
  boolean                        si2ter_rest_rcvd    = FALSE;
  boolean                        process_2g_BA       = TRUE;
  byte                           multiband_reporting = 0; /* 3GPP TS 05.08 Sec: 8.4.3: initialise to NORMAL */
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  RR_ZERO_STRUCTURE(BCCH_frequency_list);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_2_TER, gas_id);

  /* obtain pointer to global SI2q instances store */
  si2ter_inst_ptr = rr_get_SI2ter_instances_ptr(gas_id);
  if ((si2ter_inst_ptr == NULL) || (current_IE == NULL))
  {
    return;
  }

  /* Find where to store the received information */
  if (rr_is_camped(gas_id) &&
      (ARFCNS_EQUAL(channel_number, scell_info_ptr->gsm.BCCH_ARFCN)))
  {
    sys_info_db_ptr = &scell_info_ptr->gsm;
  }
  else if (ARFCNS_EQUAL(channel_number, pcell_info_ptr->gsm.BCCH_ARFCN))
  {
    sys_info_db_ptr = &pcell_info_ptr->gsm;
  }
  else
  {
    MSG_GERAN_HIGH_2_G("SI2ter(%d, %d) ignored : wrong channel number",(int) channel_number.num,(int) channel_number.band);
    return;
  }

  /* SYSTEM INFORMATION TYPE 2ter message shall be sent if and only if it is
     indicated in SYSTEM INFORMATION TYPE 3 message.*/
  if (sys_info_db_ptr->two_ti == RR_SI2TER_NOT_EXPECTED)
  {
    /* SI3 indicated that SI 2ter will not be broadcast. */
    /* Don't process the SI 2ter received */
    MSG_GERAN_HIGH_2_G("SI2ter(%d) ignored : SI3 indicates no SI2ter",(int) channel_number.num, (int)channel_number.band);
    /* MASK_SI_2_TER should be already set when processing SI3 */
    SET_BIT(MASK_SI_2_TER, sys_info_db_ptr->update_message_received);
    SET_BIT(MASK_SI_2_TER_REST, sys_info_db_ptr->update_message_received);

    /* Reset 3G Neighbor list, so that if the mode changed from FALSE to TRUE,
       the old information is wiped out */
    sys_info_db_ptr->complete_idle_3G_neighbor_list_received_si2ter = TRUE;

    /* no further processing is needed */
    return;
  }

  /*****************************************/
  /* Decode the Neighbour Cell Description */
  /*****************************************/

  /*
  The neighbour cell description IE is mandatory in the SI2ter. This IE is most likely
  to be same in all the instances of SI2ter when multiple instances are sent. In order
  to save processing (of decoding this IE) time, store 16 octets of this IE from instance 0
  (or the first received instance) into a buffer.
  Note:
  This IE should be decoded and the buffer is updated only if there is any change in this IE
  in the subsequent instances. For now assuming that there will be no change in this IE
  when multiple instances are sent.
  */

  /* if this is the first received instance */
  if (rr_is_first_received_si2ter_instance(si2ter_inst_ptr))
  {
    /* preserve the encoded Extended BCCH frequency list from this first instance */
    memscpy(si2ter_inst_ptr->first_ncell_desc_2,
            sizeof(si2ter_inst_ptr->first_ncell_desc_2),
            current_IE->start_IE,
            RR_NUM_OCTETS_IN_EXT_BCCH_FREQ_LIST_SI2TER);	

    rr_decode_neighbour_cell_description_2(
      current_IE->start_IE,
      &multiband_reporting,
      &BCCH_frequency_list,
      &decoding_ok,
      gas_id
    );
  }
  else /* not the first received instance */
  {
    /* Check if different from the previous */
    if (memcmp(si2ter_inst_ptr->first_ncell_desc_2, current_IE->start_IE, RR_NUM_OCTETS_IN_EXT_BCCH_FREQ_LIST_SI2TER) != 0)
    {
      /* Display a message for now */
      MSG_GERAN_HIGH_0_G("SI2ter BCCH Freq list changed when multiple instances sent");

      rr_decode_neighbour_cell_description_2(
        current_IE->start_IE,
        &multiband_reporting,
        &BCCH_frequency_list,
        &decoding_ok,
        gas_id
      );
    }
    else /* no difference */
    {
      /* set flag to indicate not to process GSM BA */
      process_2g_BA = FALSE;
    }
  } /* first SI2ter instance received */

  if (decoding_ok == FALSE)
  {
    MSG_GERAN_ERROR_1_G("Cannot decode SI2ter(%d)",(int)channel_number.num);
    return;
  }

  current_IE = current_IE->next_IE;

  rr_decode_si2ter_rest_octets(current_IE->start_IE,
                               current_IE->IE_length,
                               si2ter_inst_ptr,
                              &si2ter_rest_rcvd,
                              &decoding_ok,
                               sys_info_db_ptr,
                               gas_id);

  if (decoding_ok == FALSE)
  {
    MSG_GERAN_ERROR_1_G("Cannot decode SI2ter(%d)",(int)channel_number.num);
    return;
  }

  if (rr_si2ter_changed(si2ter_inst_ptr))
  {
    rr_gprs_update_meas_params_from_bcch(
      channel_number,
      TRUE,
      gas_id
    );

#ifdef FEATURE_WCDMA
    /* Populate the 3G reselection list using the information received in the SI2q_MI_instances */
    rr_populate_idle_neighbor_list_si2ter(&sys_info_db_ptr->wcdma_neighbor_list_si2ter,
                                          si2ter_inst_ptr);

    rr_gprs_update_3g_nlist_from_si2ter_instances(si2ter_inst_ptr, gas_id);

    /* Note: SI 2ter is processed and stored in GSM only mode also. This is to make sure
    that the cell list to l1 is sent as soon as there is a mode change from GSM only to
    non-GSM only. But the 3G neighbor cell list should not be sent to L1 until there is a
    mode change to non-GSM only mode.  */
    if (rr_is_camped(gas_id) && !rr_gsm_only(gas_id) &&
        scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
    {

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q   
      /* if all instances of SI2ter have been received */
      if ( (si2ter_rest_rcvd == TRUE) && (scell_info_ptr->gsm.two_quater_position == SI2Q_POSITION_NONE) &&
            (scell_info_ptr->gsm.lte_neighbour_list.count == 0) )
      {
        if (rr_check_and_populate_default_irat_prior_params(gas_id) )
        {
          MSG_GERAN_HIGH_0_G("PSEUDO SI2Q Setting is a Success");
        }
      }
#endif      

    /* Send Cell_update_list_req to GL1 only for OTA instances.*/
    if (message_ptr->tc != CACHED_SYS_INFO_TC)
    {
      /* Note: If SI2quater is also received in this cell, then the following list
      will include QsearchI and FDD_Qmin from the SI2ter until these parameters
      are received from SI2quater. Once SI2quater is received the SI2quater information
      gets precedence. */
      rr_l1_update_wcdma_neighbor_list(gas_id);
    }

#ifdef FEATURE_LTE
        rr_send_imsg(RR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND, gas_id);
#endif /* FEATURE_LTE */
    } /* rr_is_camped */
#endif /* FEATURE_WCDMA */
  } /* Si2ter changed */

  /* if all instances of SI2ter have been received */
  if (si2ter_rest_rcvd == TRUE)
  {
    /* Set the SIter received bit */
    SET_BIT( MASK_SI_2_TER_REST,sys_info_db_ptr->update_message_received);

    /* set the flag in system_information_database to TRUE */
    sys_info_db_ptr->complete_idle_3G_neighbor_list_received_si2ter = TRUE;

    /* Reset the changed bits for all the received instances of SI2ter */
    rr_reset_si2ter_status_flags(si2ter_inst_ptr);

    /* Reset the Extended BCCH frequency list from the first instance */
    memset(si2ter_inst_ptr->first_ncell_desc_2, 0, RR_NUM_OCTETS_IN_EXT_BCCH_FREQ_LIST_SI2TER);
    
  }

  if (process_2g_BA == TRUE)
  {
    /**************************************************/
    /* We have to store the system information in the */
    /* system_information_database store              */
    /**************************************************/
    if (rr_is_camped(gas_id) &&
        ARFCNS_EQUAL(channel_number, scell_info_ptr->gsm.BCCH_ARFCN))
    {
      boolean notify_L1  = FALSE;
      boolean notify_MM  = FALSE;
      boolean ba_changed = FALSE;
      uint16  ba_change_flags;

      MSG_GERAN_HIGH_1_G("Store SI2ter(%d) for serving cell",(int) channel_number.num);
      rr_gprs_nacc_check_ba_flags(&(scell_info_ptr->gsm.SI2ter_BA_list), gas_id);

      scell_info_ptr->gsm.multiband_reporting = multiband_reporting;

      /* check the BA list for change and update with the new value */
      ba_change_flags = rr_sys_info_check_for_idle_BA_change_and_update(
        &scell_info_ptr->gsm,
        &(scell_info_ptr->gsm.SI2ter_BA_list),
        &BCCH_frequency_list,
        &notify_L1,
        gas_id
      );

      if (ba_change_flags & BA_LIST_HAS_CHANGED)
      {
        MSG_GERAN_HIGH_0_G("SI2ter BA list has changed");
        ba_changed = TRUE;
      }

      /* if the BA_IND changes, the BA lists are reset */
      if (ba_change_flags & BA_IND_FLAG_CHANGED)
      {
        /* ensure that the newly updated SI2ter info is used later */
        ba_change_flags = MASK_SI_2_TER;
      }
      else
      {
        ba_change_flags = IGNORE_RCVD_SI_MASK;
      }

      /* SI2ter has been received */
      SET_BIT(MASK_SI_2_TER,scell_info_ptr->gsm.update_message_received);

      if (ba_changed)
      {
        /* generate the complete idle BA list with whatever info is known so far */
        rr_generate_complete_idle_BA_list(
          &scell_info_ptr->gsm,
          ba_change_flags,
          gas_id
        );
      }

      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      {
        boolean ba_complete = FALSE;
        boolean ext_ind_valid;

        /* the EXT_IND flag is only valid if SI2bis is required */
        ext_ind_valid = (boolean)(scell_info_ptr->gsm.decode_SI2bis != RR_SI_NOT_REQUIRED);

        rr_sys_info_check_for_complete_idle_BA_list_received(
          &scell_info_ptr->gsm,
          &ba_complete,
          ext_ind_valid,
          1,
          gas_id
        );

        notify_L1 |= ba_complete;

        /* If the GSM BA list has changed, update the BA(GPRS) list */
        /* If L1 is going to be updated anyway due to other changes, don't update here, as the new */
        /* BA list will be given to L1 with the other changes */
        if ( ba_complete || ba_changed )
        {
          rr_gprs_gsm_ba_list_updated((boolean)(!notify_L1), gas_id);
        }
      }

      /* notify L1 and/or MM if required */
      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        notify_L1,
        notify_MM,
        gas_id
      );

      if (scell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_TER, 
                                  channel_number,
                                  message_ptr,
                                  &scell_info_ptr->gsm.location_area_identification,
                                  &scell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_TER, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }

    } /* SI is for serving cell */
    /**************************************************/
    /* We have to store the system information in the */
    /* pending_channel_data store                     */
    /**************************************************/
    else if (channel_number.num == pcell_info_ptr->gsm.BCCH_ARFCN.num)
    {
      boolean dummy_notify_L1;
      MSG_GERAN_HIGH_1_G("Store SI2ter(%d) in pending_channel_data",(int) channel_number.num);

      pcell_info_ptr->gsm.multiband_reporting = multiband_reporting;

      /* store the BA_IND for the complete BA list */
      pcell_info_ptr->gsm.complete_BA_list.BA_IND = BCCH_frequency_list.BA_IND;

      /* set the new SI2bis BA list */
      rr_set_BA_list(&(pcell_info_ptr->gsm.SI2ter_BA_list),&BCCH_frequency_list);

      /* SI2ter has been received */
      SET_BIT(MASK_SI_2_TER,pcell_info_ptr->gsm.update_message_received);

      /* generate the complete idle BA list with whatever info is known so far */
      rr_generate_complete_idle_BA_list(
        &pcell_info_ptr->gsm,
        IGNORE_RCVD_SI_MASK,
        gas_id
      );

      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      rr_sys_info_check_for_complete_idle_BA_list_received(
        &pcell_info_ptr->gsm,
        &dummy_notify_L1,
        FALSE,
        1,
        gas_id
      );

      if (pcell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_TER, 
                                  channel_number,
                                  message_ptr,
                                  &pcell_info_ptr->gsm.location_area_identification,
                                  &pcell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_TER, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }

    } /* SI is for pending_channel_data */
    else
    {
      MSG_GERAN_HIGH_1_G("SI2ter(%d) ignored : wrong channel number",(int) channel_number.num);
    }
  }  /* decoding_ok */
  else
  {
    MSG_GERAN_HIGH_0_G("Not decoding BA in SI2ter instance: No change");
  }

} /* rr_handle_system_info_2_ter */

/****************************************************************************
Function
rr_reset_SI2q_MI_status_flags

Description
Resets received and changed flags to FALSE in the instances array.

Params
[IN]
instances_T *instances

Returns
None
*****************************************************************************/

static void rr_reset_SI2q_MI_status_flags (instances_T *instances)
{
  byte i;

  ASSERT (instances != NULL);

  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    instances->instance[i].changed = FALSE;
  }
}


/****************************************************************************
Function
rr_handle_system_info_2_quater

Description
Handle a received system information 2quater message from
the network. System information 2quater contains the WCDMA Neighbor cell list
Params
[IN]
word channel_number
The BCCH ARFCN where the message was received

Returns
None
*****************************************************************************/
static boolean rr_handle_system_info_2_quater(
  ARFCN_T              channel_number,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  system_information_database_T *sys_info_db_ptr = NULL;
  ie_descriptor_T               *current_IE      = NULL;
  instances_T                   *si2q_inst_ptr   = NULL;
  boolean                        decoding_ok     = TRUE;
  boolean                        set_si2q_rcvd   = FALSE;
  boolean                        update_l1       = FALSE;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_2_QUATER, gas_id);

  /* obtain pointer to global SI2q instances store */
  si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE, gas_id);
  if ((si2q_inst_ptr == NULL) || (current_IE == NULL))
  {
    return(FALSE);
  }

  /* Find where to store the received information */
  if (rr_is_camped(gas_id) &&
      ARFCNS_EQUAL(channel_number, scell_info_ptr->gsm.BCCH_ARFCN))
  {
    sys_info_db_ptr = &scell_info_ptr->gsm;
  }
  else if (channel_number.num == pcell_info_ptr->gsm.BCCH_ARFCN.num)
  {
    sys_info_db_ptr = &pcell_info_ptr->gsm;
  }
  else
  {
    MSG_GERAN_HIGH_1_G("SI2quater(%d) ignored : wrong channel number",(int) channel_number.num);
    return(FALSE);
  }
  /* SI2 quater is processed and stored even when the UE is operating in the GSM only mode
     so that the stored list could be used as soon as a change to non-GSM only mode */

  /* SYSTEM INFORMATION TYPE 2quater message shall be sent if and only if it is
     indicated in SYSTEM INFORMATION TYPE 3 message.*/
  if (sys_info_db_ptr->two_quater_position == SI2Q_POSITION_NONE)
  {
    /* SI3 indicated that SI 2quater will not be broadcast. */
    /* Don't process the SI 2quater received */
    MSG_GERAN_HIGH_1_G("SI2quater(%d) ignored : SI3 indicates no SI2quater",(int) channel_number.num);
    /* Set bit to indicate SI 2quater has been received */
    SET_BIT(MASK_SI_2_QUATER, sys_info_db_ptr->update_message_received);

    /* Reset 3G Neighbor list, so that if the mode changed from FALSE to TRUE,
       the old information is wiped out */
    sys_info_db_ptr->complete_idle_3G_neighbor_list_received = TRUE;

    /* no further processing is needed */
    return(FALSE);
  }

  /*******************************************************************
    At this point, channel_number is valid and the
    SI3 rest octets indicated that SI2quater will be sent, which means
    that the message should be processed
  ********************************************************************/

  /**********************************************************************/
  /* Decode the 3G Neighbour Cell Description and store the information */
  /* in the pending_channel_data or the system_information_database     */
  /**********************************************************************/
  rr_decode_si2quater_rest_octets(
    current_IE->start_IE,
    current_IE->IE_length,
    si2q_inst_ptr,
    &set_si2q_rcvd,
    &decoding_ok,
    &channel_number,
    sys_info_db_ptr,
    gas_id
  );

  if (decoding_ok == FALSE)
  {
    MSG_GERAN_ERROR_0_G("Error decoding SI2quater Rest Octets");
    return(FALSE);
  }

  /* if a new instance has been received or updated */
  if (rr_SI2q_MI_changed(si2q_inst_ptr))
  {
    wcdma_ncell_list_data_t *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);

    /* Populate the 3G reselection list using the information received in the SI2q_MI_instances */
    rr_populate_idle_neighbor_list(
      &sys_info_db_ptr->utran_neighbor_list,
      si2q_inst_ptr
    );

    /* The 3G neighbor cell index list to be used for measurement reporting */
    rr_create_wcdma_neighbor_cell_index_list(
      si2q_inst_ptr,
      wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list,
      SOURCE_SI2_QUATER
    );

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
    rr_gprs_update_3g_nlist_from_si2quater_instances(si2q_inst_ptr, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

    /* LTE neighbour list is generated by the following function. */
    rr_gprs_update_meas_params_from_bcch(channel_number, TRUE, gas_id);

    rr_gprs_update_gprs_bsic_description_from_si2quater_instances(channel_number, gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
    /* Note 1: SI 2quater instances will be processed even after the 8 frequencies and 96 cells have
    been received. This is to handle any measurement parameter changes received in those instances after
    the storage buffers become full. */

    /* Note 2: SI 2quater is processed and stored in GSM only mode also. This is to make sure
    that the cell list to l1 is sent as soon as there is a mode change from GSM only to
    non-GSM only. But the 3G neighbor cell list should not be sent to L1 until there is a
    mode change to non-GSM only mode.  */

    /* Note 3: rr_send_mph_wcdma_cell_update_list() is called as earlier as possible
     * even haven't recevied 3g measurement parameters, default or the parameters
     * from last cell's will be used
     */
    if (rr_is_camped(gas_id) && !rr_gsm_only(gas_id))
    {
      wcdma_neighbor_list_T *p_wcdma_nlist =
        &scell_info_ptr->gsm.utran_neighbor_list;
      if ((p_wcdma_nlist->valid_data == TRUE) &&
          ((p_wcdma_nlist->wcdma_cell_list.num_of_cells != 0) ||
           (p_wcdma_nlist->wcdma_freq_only_list.num_of_frequencies != 0)) )
      {
        update_l1 = TRUE;
      }
#ifdef FEATURE_LTE
      else if (sys_info_db_ptr->lte_neighbour_list.count > 0)
      {
        update_l1 = TRUE;
      }
#endif /* FEATURE_LTE */
    }
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
  } /* rr_SI2q_MI_changed */

  /* if all instances of SI2 quater have been received:
   * Set the SI2quater received bit,
   * Set the complete_idle_3G_neighbor_list_received flag,
   * Reset the received and changed bits for all the received instances of SI2quater
   */
  if (set_si2q_rcvd == TRUE)
  {
    SET_BIT( MASK_SI_2_QUATER,sys_info_db_ptr->update_message_received);

    sys_info_db_ptr->complete_idle_3G_neighbor_list_received = TRUE;

    if (rr_SI2q_MI_changed(si2q_inst_ptr))
    {
      MSG_GERAN_HIGH_0_G("All SI2quater instances received");

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      if (rr_is_camped(gas_id) && !rr_gsm_only(gas_id))
      {
        update_l1 = TRUE;

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
        /* This is the point in code, where all the SI2Q blocks have been received by GRR */
        if (sys_info_db_ptr->lte_neighbour_list.count == 0 )
        {
          /* If all Si2Q blokcs received and NO LTE on all the SI2 Blocks */            
          if ( rr_check_and_populate_default_irat_prior_params(gas_id) )
          {
            MSG_GERAN_HIGH_0_G("PSEUDO SI2Q Setting is a Success");
          }  
        }
#endif

      }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
    }

    rr_reset_SI2q_MI_status_flags(si2q_inst_ptr);
  }
  else
  {
    if (rr_SI2q_MI_changed(si2q_inst_ptr))
    {
      /* Mark SI2quater as not received */
      CLEAR_BIT( MASK_SI_2_QUATER, sys_info_db_ptr->update_message_received);

      /* Request SI2quater acquisition */
      bsi_set_messages_required_for_acquisition(
        MASK_SI_NONE,
        MASK_SI_2_QUATER,
        FALSE,
        gas_id
      );
    }
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  if (rr_is_camped(gas_id) && (update_l1 == TRUE))
  {
    /* Send Cell_update_list_req to GL1 only for OTA instances.*/
    if (message_ptr->tc != CACHED_SYS_INFO_TC)
    {
      MSG_GERAN_HIGH_0_G("Sending WCDMA neighbor list to L1");
      rr_l1_update_wcdma_neighbor_list(gas_id);
    }

#ifdef FEATURE_LTE
    rr_send_imsg(RR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND, gas_id);
#endif /* FEATURE_LTE */
  }
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

  if (sys_info_db_ptr->cell_identity_valid_flag)
  {
    (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_QUATER, 
                              channel_number,
                              message_ptr,
                              &sys_info_db_ptr->location_area_identification,
                              &sys_info_db_ptr->cell_identity,
                              gas_id);
  }
  else
  {
    (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_2_QUATER, 
                              channel_number,
                              message_ptr,
                              NULL,
                              NULL,
                              gas_id);
  }

  return(update_l1);
} /* rr_handle_system_info_2_quater */

/**
  @brief Checks whether all SI2quater instances have been received.

  @return TRUE if all SI2quater instances have been received, FALSE
          otherwise.  Also returns FALSE if no SI2quater instances have been
          received, which means either that none are broadcast or none have
          been received.
*/
static boolean rr_mi_all_instances_received(
  instances_T * mi_instances
)
{
  RR_NULL_CHECK_RETURN_PARAM(mi_instances, FALSE);

  if (mi_instances->num_instances > 0)
  {
    if (mi_instances->instance[0].valid_data)
    {
      if (mi_instances->instance[0].common_params.COUNT ==
          (mi_instances->num_instances - 1))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

boolean rr_mi_eutran_parameters_received(
  instances_T * mi_instances
)
{
  boolean eutran_parameters_received = FALSE;
  uint32 i, j;

  RR_NULL_CHECK_RETURN_PARAM(mi_instances, FALSE);

  /* First, a quick check on whether all SI2quater instances have been
  received - this is to handle the case where Fast Acquisition is not supported
  by the network.  If this is the case, the EUTRAN_Start and EUTRAN_Stop bits
  may never be set correctly. */
  if (rr_mi_all_instances_received(mi_instances))
  {
    return TRUE;
  }

  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    /* Search for E-UTRAN Start */
    if (mi_instances->instance[i].valid_data &&
        mi_instances->instance[i].si2q_mi.mi_params.eutran_params.eutran_start)
    {
      for (j = i; j < MAX_INSTANCES; ++j)
      {
        /* Now search for E-UTRAN Stop - only if it is found before we
        encounter a missing instance can we declare that all E-UTRAN
        parameters are received. */
        if (mi_instances->instance[j].valid_data)
        {
          if (mi_instances->instance[j].si2q_mi.mi_params.eutran_params.eutran_stop)
          {
            eutran_parameters_received = TRUE;
            break;
          }
        }
        else
        {
          break;
        }
      }
      break;
    }
  }

  return eutran_parameters_received;
}

/****************************************************************************
Function
rr_handle_measurement_information

Description
Process the Measurement Information message.

Params
None

Returns
None
*****************************************************************************/
static void rr_handle_measurement_information(
  const gas_id_t gas_id
)
{
  ie_descriptor_T               *current_IE;
  instances_T                   *mi_inst_ptr;
  boolean                        decoding_ok = TRUE;
  boolean                        send_wcdma_neighbor_list_to_L1 = FALSE;
  boolean                        set_MI_received = FALSE;
  boolean                        send_lte_neighbor_list_to_L1 = FALSE;

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(MEASUREMENT_INFORMATION_TYPE, gas_id);

  /* obtain pointer to global MI instances store */
  mi_inst_ptr = rr_get_instances_ptr(MI_INSTANCE, gas_id);
  if ((mi_inst_ptr == NULL) || (current_IE == NULL))
  {
    return;
  }

  /*********************************************************************/
  /* Decode the 3G Neighbour Cell Description and store the information in
     the pending_channel_data or the system_information_database based */
  /*********************************************************************/
  rr_decode_neighbour_cell_description_MI(
    current_IE->start_IE,
    current_IE->IE_length,
    mi_inst_ptr,
    &send_wcdma_neighbor_list_to_L1,
    &set_MI_received,
    &decoding_ok
  );

  if (decoding_ok == TRUE)
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    RR_NULL_CHECK_FATAL(scell_info_ptr);

    MSG_GERAN_HIGH_0_G("Completed Decoding Measurement Information instance(s)");


#ifdef FEATURE_LTE
    if (rr_mi_eutran_parameters_received(mi_inst_ptr))
    {
      rr_dedicated_mode_process_eutran_params(
        scell_info_ptr,
        mi_inst_ptr
      );
 
      if(rr_SI2q_MI_changed(mi_inst_ptr))
      {
        send_lte_neighbor_list_to_L1 = TRUE;
      }
    }
#endif /* FEATURE_LTE */

    /* The following four possibilities are handled here:
    set_MI_received = TRUE, send_wcdma_neighbor_list_to_L1 = TRUE => Changed Information
    set_MI_received = TRUE, send_wcdma_neighbor_list_to_L1 = FALSE => Second Set with no change
    set_MI_received = FALSE, send_wcdma_neighbor_list_to_L1 = TRUE => 3G_WAIT number of instances received
    set_MI_received = FALSE, send_wcdma_neighbor_list_to_L1 = FALSE => Middle of receiving instances
    */
    /* if all instances of MI have been received */
    if ((set_MI_received == TRUE) && (send_wcdma_neighbor_list_to_L1 == TRUE))
    {
      wcdma_ncell_list_data_t *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
 
      rr_dedicated_mode_process_params(scell_info_ptr, mi_inst_ptr);

      /* Populate the 3G reselection list using the information received in the SI2q_MI_instances */
      rr_populate_ded_neighbor_list(
        &scell_info_ptr->gsm.wcdma_ded_neighbor_list,
        mi_inst_ptr,
        gas_id
      );

      /* The 3G neighbor cell index list to be used for measurement reporting */
      rr_create_wcdma_neighbor_cell_index_list(
        mi_inst_ptr,
        wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list,
        SOURCE_MEAS_INFO
      );

      /* Reset the received and changed bits for all the received instances of Measurement Information */
      rr_reset_SI2q_MI_status_flags(mi_inst_ptr);

      /* Update the bsic description structure from the BSICs received from the Measurement Information for Enhanced
      Measurement Reporting */
      rr_gsm_update_gsm_bsic_description_from_mi_instances(mi_inst_ptr, gas_id);
    }
    else if ((set_MI_received == FALSE) && (send_wcdma_neighbor_list_to_L1 == TRUE))
    {
      /* Just Populate the 3G reselection list using the information received in the 3G_WAIT number of instances */
      rr_populate_ded_neighbor_list(&scell_info_ptr->gsm.wcdma_ded_neighbor_list, mi_inst_ptr, gas_id);
    }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    if (send_wcdma_neighbor_list_to_L1 || send_lte_neighbor_list_to_L1)
    {
      /* Populate the 3G neighbor list using the information received in the SI2q_MI_instances */
      /* Note this could be done with just 3G wait number of instances received */

      /* Note 1: MI instances will be processed even after the 8 frequencies and 96 cells have
      been received. This is to handle any measurement parameter changes received in those instances after
      the storage buffers become full. The rr_send_mph_wcdma_cell_update_list is called only after 3G_WAIT
      instances of MI are received */

      if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data == TRUE ||
          scell_info_ptr->gsm.dedicated_mode_params != NULL) 
      {
#ifdef FEATURE_WCDMA
        surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);

        /* Before sending the list to L1 purge the old entries (if) present in the
        wcdma_ded_meas structure */
        rr_purge_wcdma_ded_measurements(wcdma_ded_meas_ptr, &scell_info_ptr->gsm.wcdma_ded_neighbor_list);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
        {
          rr_ded_irat_measurements_t   *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);

          rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
        }
#endif /* FEATURE_GSM_TDS */

        if (!rr_gsm_only(gas_id))
        {
          /* Send the mph_dedicated_wcdma_cell_update_list_req to L1 which has the 3g neighbor cells received in MI */
          /* L1 will start sending the mph_surround_dedicated_meas_ind after processing this message */
          rr_send_mph_dedicated_wcdma_cell_update_list_req(
            NULL,
            &scell_info_ptr->gsm.wcdma_ded_neighbor_list,
            scell_info_ptr->gsm.dedicated_mode_params,  
            gas_id
          );
        }
      }
    } /* send_wcdma_neighbor_list_to_L1 */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  }  /* decoding_ok */

} /* rr_handle_measurement_information */

/**
 * Generates a further sys info mask after the arrival of Si3
 *
 * @param cell_info_ptr pointer to cell database
 *        gas_id        access stratum identifier
 */
static rr_l1_si_type_T rr_sys_info_required_after_si3(gprs_scell_info_t *cell_info_ptr, const gas_id_t gas_id)
{
  rr_l1_si_type_T norm_mask;

  /* default mask is all sys info except Si3 */
  norm_mask = RR_L1_SI_TYPE_ALL_NORM & (~RR_L1_SI_TYPE_3);

  if (cell_info_ptr != NULL)
  {
    /* if Si2ter is not required, remove that as well */
    if (RR_SI_NOT_REQUIRED == cell_info_ptr->gsm.decode_SI2ter)
    {
      norm_mask &= ~RR_L1_SI_TYPE_2_ter;
    }

    /* if Si2quater is not required, remove that as well */
    if ((SI2Q_POSITION_NONE == cell_info_ptr->gsm.two_quater_position) ||
        (FALSE == rr_ms_3g_sys_info_mandatory(gas_id)))
    {
      norm_mask &= ~RR_L1_SI_TYPE_2_quater_NORM;
    }
  }

  return(norm_mask);
}

/**
 * Processes the SI3 message. SI3 is expected for
 * the serving cell, pending channel data, surrounding cells,
 * and candidate cells.
 *
 * @param channel_number ARFCN where the message was received
 *        tc             TC where message was received (2 or 6)
 */
static void rr_handle_system_info_3(
  ARFCN_T              channel_number,
  byte                 tc,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  ie_descriptor_T *current_IE;
  boolean decoding_ok = FALSE;
  word cell_identity = 0;
  LAI_T location_area_identification;
  control_channel_description_T control_channel_description;
  cell_options_T cell_options;
  cell_selection_parameters_T cell_selection_parameters;
  RACH_control_parameters_T RACH_control_parameters;
  boolean ecsc = FALSE;
  boolean ecsr = FALSE;
  boolean two_ti = FALSE;
  boolean si2q_pi = FALSE;
  boolean si2q_pos = FALSE;
  boolean gprs_indicator_flag = FALSE;
  gprs_indicator_t  gprs_indicator;
  sys_mcc_type mcc = 0;
  sys_mnc_type mnc = 0;
  boolean is_plmn_undefined = FALSE;
  boolean includes_pcs_digit = FALSE;
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  best_six_ncells_t             *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

  uint8 remaining_length = 0;
  boolean si_cache_updated = FALSE;

  RR_ZERO_STRUCTURE(location_area_identification);
  RR_ZERO_STRUCTURE(control_channel_description);
  RR_ZERO_STRUCTURE(cell_options);
  RR_ZERO_STRUCTURE(cell_selection_parameters);
  RR_ZERO_STRUCTURE(RACH_control_parameters);
  RR_ZERO_STRUCTURE(gprs_indicator);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_3, gas_id);

  if (current_IE)
  {
    // Mandatory IE - Neighbour Cell Identity
    if (current_IE->start_IE)
    {
      cell_identity = rr_decode_cell_identity(current_IE->start_IE);
      decoding_ok = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("NEIGHBOUR CELL IDENTITY missing");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - Location Area Identification
      if (current_IE->start_IE)
      {
        rr_decode_location_area_identification(
          current_IE->start_IE,
          &location_area_identification
        );

        /* extract the MCC/MNC from LAI */
        sys_plmn_get_mcc_mnc(
          location_area_identification.plmn_id,
          &is_plmn_undefined,
          &includes_pcs_digit,
          &mcc,
          &mnc
        );

        decoding_ok = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("LOCATION AREA IDENTIFICATION missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - Control Channel Description
      if (current_IE->start_IE)
      {
        rr_decode_control_channel_description(
          current_IE->start_IE,
          &control_channel_description,
          &decoding_ok
        );
      }
      else
      {
        MSG_GERAN_ERROR_0_G("CONTROL CHANNEL DESCRIPTION missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - Cell Options
      if (current_IE->start_IE)
      {
        rr_decode_cell_options_on_bcch(current_IE->start_IE, &cell_options);
        decoding_ok = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("CELL OPTIONS missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - Cell Selections Parameters
      if (current_IE->start_IE)
      {
        rr_decode_cell_selection_parameters(current_IE->start_IE, &cell_selection_parameters);
        decoding_ok = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("CELL SELECTION PARAMETERS missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - Cell Selections Parameters
      if (current_IE->start_IE)
      {
        rr_decode_RACH_control_parameters(current_IE->start_IE, &RACH_control_parameters);
        decoding_ok = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("RACH CONTROL PARAMETERS missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - SI3 Rest Octets
      if (current_IE->start_IE)
      {
        si3_rest_octets_t si3ro;
        csn_decoder_error_t error_code;

        RR_ZERO_STRUCTURE(si3ro);

        /* Use the CSN Decoder to decode the contents of the message */

        remaining_length = rr_length_to_end_of_ota_msg(
          SYSTEM_INFORMATION_TYPE_3,    // msg_type
          current_IE->start_IE,         // csn1_start_ie_ptr
          SI3_REST_OCTETS,              // csn_ie_type
          gas_id
        );

        error_code = gcsn1_rr_decoder(
          SI3_REST_OCTETS,                    // message_type
          current_IE->start_IE,               // csn1_ie_src
          (void *)&si3ro,remaining_length,    // len_to_end_of_buf
          gas_id
        );

        if (error_code == CSN_DECODE_NO_ERROR)
        {
          decoding_ok = TRUE;

          // Selection Parameters (optional)
          if (si3ro.selection_params_flag == valid)
          {
            cell_selection_parameters.CELL_RESELECT_PARAM_IND = TRUE;
            cell_selection_parameters.CELL_BAR_QUALIFY = si3ro.cbq;
            cell_selection_parameters.CELL_RESELECT_OFFSET = si3ro.cell_reselect_offset;
            cell_selection_parameters.TEMPORARY_OFFSET = si3ro.temporary_offset;
            cell_selection_parameters.PENALTY_TIME = si3ro.penalty_time;
          }
          else
          {
            cell_selection_parameters.CELL_RESELECT_PARAM_IND = FALSE;
          }

          // Power Offset (optional)
          if (si3ro.power_offset_flag == valid)
          {
            rr_internal_band_T  band = (rr_internal_band_T)channel_number.band;

            cell_selection_parameters.power_offset_valid = TRUE;

            /*
            See 3GPP 44.018 10.5.2.34 SI 3 Rest Octets
            ... Power Offset is used only by DCS 1800 Class 3 MSs ...
            */
            if (((band == BAND_DCS) || (band == BAND_PCS)) &&
                (rr_gapi_get_band_rf_power_class(band, gas_id) == (byte) CLASS_3))
            {
              cell_selection_parameters.POWER_OFFSET = (si3ro.power_offset * 2);
            }
            else
            {
              cell_selection_parameters.POWER_OFFSET = 0;
            }
          }
          else
          {
            cell_selection_parameters.power_offset_valid = FALSE;
          }

          // SI2ter Indicator
          two_ti = si3ro.si2ter_indicator;

          // Early Classmark Sending Control
          ecsc = si3ro.ecsc;

          // GPRS Indicator (optional)
          if (si3ro.gprs_indicator_flag == valid)
          {
            gprs_indicator_flag = TRUE;
            gprs_indicator.ra_colour = si3ro.ra_colour;

            if (si3ro.si13_position == CSN_BCCH_EXTENDED)
            {
              gprs_indicator.si13_position = BCCH_EXTENDED;
            }
            else
            {
              gprs_indicator.si13_position = BCCH_NORMAL;
            }
          }
          else
          {
            gprs_indicator_flag = FALSE;
          }

          // 3G Early Classmark Sending Restriction
          ecsr = si3ro.three_g_ecsr;

          // SI2quater Indicator
          if (si3ro.si2quater_indicator_flag == valid)
          {
            si2q_pi = TRUE;

            if (si3ro.si2quater_position == CSN_BCCH_EXTENDED)
            {
              si2q_pos = 1;  // 1=BCCH-Ext
            }
            else
            {
              si2q_pos = 0;  // 0=BCCH-Norm
            }
          }
          else
          {
            si2q_pi = FALSE;
            si2q_pos = 0;  // 0=BCCH-Norm
          }
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("SI3 REST OCTETS missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok == TRUE)
  {
    boolean channel_found = FALSE;

    /**************************************************/
    /* We have to store the system information in the */
    /* system_information_database store              */
    /**************************************************/
    if (rr_is_this_the_scell(channel_number, gas_id) && (! rr_background_plmn_search_in_progress(gas_id)))
    {
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
      boolean notify_MM = FALSE;
      boolean notify_L1 = FALSE;
      boolean log_cell_info = FALSE;
      boolean send_wcdma_cell_update = FALSE;
      boolean cell_identity_changed;
      boolean lai_changed;
      boolean bs_pa_mfrms_changed = FALSE;
#ifdef FEATURE_IDLE_DRX_SCALING
      int     old_dsc_max;
#endif /* FEATURE_IDLE_DRX_SCALING */

      MSG_GERAN_HIGH_1_G("Store SI3(%d) for serving cell",(int) channel_number.num);
      channel_found = TRUE;

      // Log any parameters that have changed

      rr_log_diff_rach_control_parameters(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_log_diff_cell_options(
        &cell_options,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_log_diff_cell_selection_parameters(
        &cell_selection_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_log_diff_control_channel_description(
        &control_channel_description,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_log_diff_gprs_indicators(
        gprs_indicator_flag,
        gprs_indicator.ra_colour,
        gprs_indicator.si13_position,
        scell_info_ptr,
        gas_id
      );

      // Check which parameters have changed and flag updates to L1 and MM

      cell_identity_changed = rr_check_for_cell_identity_change(
        cell_identity,
        &scell_info_ptr->gsm
      );

      notify_MM |= cell_identity_changed;
      log_cell_info |= cell_identity_changed;

      lai_changed = (!rr_same_lai(&location_area_identification,
                                  &(scell_info_ptr->gsm.location_area_identification)));

      notify_MM |= lai_changed;
      log_cell_info |= lai_changed;

      rr_check_for_rach_control_parameters_change(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      rr_check_for_control_channel_description_change(
        &control_channel_description,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      rr_check_for_cell_options_change(
        &cell_options,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      rr_check_for_cell_selection_parameters_change(
        &cell_selection_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      rr_gprs_check_for_cell_capability_change(
        gprs_indicator_flag,
        &scell_info_ptr->gsm
      );

      if (control_channel_description.BS_PA_MFRMS != scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS)
      {
        /* Mark change in BS_PA_MFRMS */
        bs_pa_mfrms_changed = TRUE;
#ifdef FEATURE_IDLE_DRX_SCALING
        /* Store old DSC MAX value */
        old_dsc_max = rr_gprs_get_max_dsc_value(TRUE, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }

      // Store the new values in the system information database

      scell_info_ptr->gsm.ecsc = ecsc;
      scell_info_ptr->gsm.ecsr = ecsr;
      scell_info_ptr->gsm.cell_identity = cell_identity;
      scell_info_ptr->gsm.cell_identity_valid_flag = TRUE;
      scell_info_ptr->gsm.location_area_identification = location_area_identification;
      scell_info_ptr->gsm.control_channel_description = control_channel_description;
      scell_info_ptr->gsm.control_channel_description_valid = TRUE;
      scell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
      scell_info_ptr->gsm.cell_selection_parameters = cell_selection_parameters;
      scell_info_ptr->gsm.cell_options = cell_options;
      scell_info_ptr->gsm.gprs_indicator = gprs_indicator_flag;
      scell_info_ptr->bcch_sys_info.ra_colour = gprs_indicator.ra_colour;
      scell_info_ptr->bcch_sys_info.si13_position = gprs_indicator.si13_position;

      if (bs_pa_mfrms_changed)
      {
#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_bs_pa_mfrms_changed(old_dsc_max, gas_id);
#else /* FEATURE_IDLE_DRX_SCALING */
        rr_gprs_initialise_dsc(gas_id);
#endif /* !FEATURE_IDLE_DRX_SCALING */
      }

      if (gprs_indicator_flag)
      {
        /* Check if SI13 is on BCCH-Normal or BCCH-Extended, and ensure the BSI acquisition */
        /* flags reflect this */
        if (gprs_indicator.si13_position == BCCH_EXTENDED)
        {
          /* Mark SI13 as being broadcast on BCCH-Ext */
          bsi_update_messages_being_broadcast(
            scell_info_ptr,
            MASK_SI_NONE,
            MASK_SI_13,
            MASK_SI_NONE
          );
        }
        else
        {
          /* Mark SI13 as being broadcast on BCCH-Norm */
          bsi_update_messages_being_broadcast(
            scell_info_ptr,
            MASK_SI_13,
            MASK_SI_NONE,
            MASK_SI_NONE
          );
        }
      }
      else
      {
        /* Mark SI13 as not being broadcast */
        /* Note: This also marks SI13 as received, and prevents further acquisitions */
        bsi_update_messages_being_broadcast(
          scell_info_ptr,
          MASK_SI_NONE,
          MASK_SI_NONE,
          MASK_SI_13
        );

        /*
        Note: The si13_data_valid_flag is not reset here, because it is used to compare
        against the gprs_indicator to trigger GPRS deactivation. It is reset later in
        bsi_camped_post_processing()
        */
      }

      if (log_cell_info == TRUE)
      {
        // log the change in cell information
        (void) rr_log_cell_info(&scell_info_ptr->gsm, gas_id);

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
        if (best_six_ncells_ptr != NULL)
        {
          rr_notify_cell_id_change(
            cell_identity,
            *best_six_ncells_ptr,
            scell_info_ptr->gsm.location_area_identification,
            gas_id
          );
        }
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/

       {
         gprs_scell_info_t *serving_cell_ptr = NULL;
         eng_mode_scell_info_t scell_info;

         RR_ZERO_STRUCTURE(scell_info);

         serving_cell_ptr = rr_gprs_get_scell_info(gas_id);

         scell_info = geran_eng_mode_format_scell_info(
           serving_cell_ptr,    // scell_info_ptr
           ENG_MODE_NO_CAUSE,   // cause
           gas_id               // gas_id
         );

         geran_eng_mode_data_write(
           ENG_MODE_SCELL_INFO,    // geran_eng_mode_info_type
           (void*)&scell_info,     // param_copy
           TRUE,                   // valid_flag
           gas_id                  // gas_id
         );

         rr_eng_mode_update_nw_params(gas_id);
       }

#ifdef FEATURE_QSH_EVENT_METRIC
        rr_qsh_metric_log_cell_info(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
      }

      send_wcdma_cell_update =
        rr_handle_si2ter_indicator(two_ti, tc, rr_gprs_get_scell_info(gas_id), gas_id);
      send_wcdma_cell_update |=
        rr_handle_si2quater_indicator(si2q_pi, si2q_pos, rr_gprs_get_scell_info(gas_id), gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      if (send_wcdma_cell_update)
      {
        rr_l1_update_wcdma_neighbor_list(gas_id);
      }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

      /******************************************************************/
      /* Since SI3 indicates if SI2ter is required or not, it may be    */
      /* the case that RR now has the complete idle BA list. Must check */
      /******************************************************************/
      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      rr_sys_info_check_for_complete_idle_BA_list_received(
        &scell_info_ptr->gsm,
        &notify_L1,
        FALSE,
        1,
        gas_id
      );

      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        notify_L1,
        notify_MM,
        gas_id
      );

      /* SI3 has been received */
      SET_BIT(MASK_SI_3,scell_info_ptr->gsm.update_message_received);

      /* SI3 is assumed to contain all the information in SI7 & SI8 */
      SET_BIT( MASK_SI_8 | MASK_SI_7,scell_info_ptr->gsm.update_message_received);

      if (scell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_3, 
                                  channel_number,
                                  message_ptr,
                                  &scell_info_ptr->gsm.location_area_identification,
                                  &scell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_3, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }
      si_cache_updated = TRUE;

    } /* SI is for serving cell */

    /**************************************************/
    /* We have to store the system information in the */
    /* pending_channel_data store                     */
    /**************************************************/
    else if (rr_is_this_the_pcell(channel_number, gas_id))
    {
      gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
      rr_l1_si_type_T ext_bcch_sys_info = RR_L1_SI_TYPE_NONE;
      boolean         dummy_notify_L1   = FALSE;

      MSG_GERAN_HIGH_1_G("Store SI3(%d) in pending_channel_data",(int) channel_number.num);
      channel_found = TRUE;

      pcell_info_ptr->gsm.ecsc = ecsc;
      pcell_info_ptr->gsm.ecsr = ecsr;
      pcell_info_ptr->gsm.cell_identity = cell_identity;
      pcell_info_ptr->gsm.cell_identity_valid_flag = TRUE;
      pcell_info_ptr->gsm.location_area_identification = location_area_identification;
      pcell_info_ptr->gsm.control_channel_description = control_channel_description;
      pcell_info_ptr->gsm.control_channel_description_valid = TRUE;
      pcell_info_ptr->gsm.cell_options = cell_options;
      pcell_info_ptr->gsm.cell_selection_parameters = cell_selection_parameters;
      pcell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
      pcell_info_ptr->gsm.gprs_indicator = gprs_indicator_flag;
      pcell_info_ptr->bcch_sys_info.ra_colour = gprs_indicator.ra_colour;
      pcell_info_ptr->bcch_sys_info.si13_position = gprs_indicator.si13_position;

      /**
       * If acquiring sys info following connection release, if a X2G HO had occurred then set the RR EPLMN to 
       * the current cell PLMN. This covers the case where a RAU is performed during the call (DTM) after the 
       * X2G HO and the NAS EPLMN list is deleted due to the RAU ACCEPT containing an empty EPLMN list. 
       * Note that this is not done if in LIMITED service (i.e. call was an emergency call)
       */
      if (rr_is_acquiring_si_after_conn_rel(gas_id))
      {
        if (rr_ce_conn_originated_on_other_rat(gas_id) &&
            rr_equivalent_plmn_is_undefined(gas_id) &&
            (rr_is_limited_plmn_selection_mode(gas_id) == FALSE))
        {
          rr_set_equivalent_plmn(&location_area_identification.plmn_id, gas_id);
        }
      }

      if (gprs_indicator_flag)
      {
        /* Check if SI13 is on BCCH-Normal or BCCH-Extended, and ensure the BSI acquisition */
        /* flags reflect this */
        if (gprs_indicator.si13_position == BCCH_EXTENDED)
        {
          /* Mark SI13 as being broadcast on BCCH-Ext */
          bsi_update_messages_being_broadcast(
            pcell_info_ptr,
            MASK_SI_NONE,
            MASK_SI_13,
            MASK_SI_NONE
          );

          /*lint -save -e655 :Suppress Lint Warning for Enum Bitwise operation */
          ext_bcch_sys_info |= RR_L1_SI_TYPE_13_EXT;
          /*lint -restore */
        }
        else
        {
          /* Mark SI13 as being broadcast on BCCH-Norm */
          bsi_update_messages_being_broadcast(
            pcell_info_ptr,
            MASK_SI_13,
            MASK_SI_NONE,
            MASK_SI_NONE
          );
        }

        /* If SI13 has not been received yet, setup the 'expected' and 'data valid' flags */
        if ( ARE_NO_BITS_SET( pcell_info_ptr->gsm.update_message_received, MASK_SI_13 ) )
        {
          pcell_info_ptr->bcch_sys_info.si13_expected_flag = TRUE;
          pcell_info_ptr->bcch_sys_info.si13_data_valid_flag = FALSE;
        }
      }
      else
      {
        /* Mark SI13 as not being broadcast */
        /* Note: This also marks SI13 as received, and prevents further acquisitions */
        bsi_update_messages_being_broadcast(
          pcell_info_ptr,
          MASK_SI_NONE,
          MASK_SI_NONE,
          MASK_SI_13
        );

        pcell_info_ptr->bcch_sys_info.si13_expected_flag = FALSE;
        pcell_info_ptr->bcch_sys_info.si13_data_valid_flag = FALSE;

        if (rr_gprs_nacc_pncd_in_progress(gas_id))
        {
          /* target cell has no GPRS support - NACC should not be permitted */
          MSG_GERAN_ERROR_0_G("NACC: SI3 indicates no GPRS support on target cell");
          pcell_info_ptr->gsm.update_message_received &= (~ MASK_SI_13);
        }
      }

      /* handle si2ter_indicator and SI2quater indicator, position */
      (void)rr_handle_si2ter_indicator(two_ti, tc, rr_gprs_get_pcell_info(gas_id), gas_id);
      (void)rr_handle_si2quater_indicator(si2q_pi, si2q_pos, rr_gprs_get_pcell_info(gas_id), gas_id);

      if (pcell_info_ptr->gsm.two_quater_position == SI2Q_POSITION_BCCH_Ext)
      {
        if (rr_ms_3g_sys_info_mandatory(gas_id))
        {
          /*lint -save -e655 :Suppress Lint Warning for Enum Bitwise operation */
          ext_bcch_sys_info |= RR_L1_SI_TYPE_2_quater_EXT;
          /*lint -restore */
        }
      }

      if (rr_gprs_nacc_pncd_in_progress(gas_id))
      {
        /* no need to issue an extended BCCH read request while NACC is in progress */
        ext_bcch_sys_info = RR_L1_SI_TYPE_NONE;
      }

      if (FALSE == rr_select_bcch_si_acq_in_progress(gas_id))
      {
        if ( ext_bcch_sys_info != RR_L1_SI_TYPE_NONE )
        {
          rr_l1_si_type_T norm_bcch_sys_info;

          norm_bcch_sys_info = rr_sys_info_required_after_si3(pcell_info_ptr, gas_id);

          rr_send_mph_read_sys_info_req(
            (rr_l1_si_type_T)((int)ext_bcch_sys_info | (int)norm_bcch_sys_info),
            gas_id
          );
        }
      }

      /******************************************************************/
      /* Since SI3 indicates if SI2ter is required or not, it may be    */
      /* the case that RR now has the complete idle BA list. Must check */
      /******************************************************************/
      /********************************************************************************/
      /* Check to see if RR has now received the complete BA list (with this message) */
      /********************************************************************************/
      rr_sys_info_check_for_complete_idle_BA_list_received(
        &pcell_info_ptr->gsm,
        &dummy_notify_L1,
        FALSE,
        1,
        gas_id
      );

      /* SI3 has been received */
      SET_BIT( MASK_SI_3,pcell_info_ptr->gsm.update_message_received);

      /* it is assumed that SI3 contains all the information in SI7/SI8 */
      SET_BIT( MASK_SI_8 | MASK_SI_7,pcell_info_ptr->gsm.update_message_received);

      (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_3, 
                                channel_number,
                                message_ptr,
                                &pcell_info_ptr->gsm.location_area_identification,
                                &pcell_info_ptr->gsm.cell_identity,
                                gas_id);
      si_cache_updated = TRUE;
    } /* pending channel data */

    /****************************************************************/
    /* The surrounding database and candidate cell database are     */
    /* updated independently of the system information database and */
    /* pending_channel_data stores                                  */
    /****************************************************************/

    /* Determine if this is a known neighbour cell */
    if (rr_gprs_is_this_a_surround_ncell(channel_number, gas_id))
    {
      bcch_ncell_info_t *ncell_info_ptr;

      ncell_info_ptr = rr_gprs_get_surround_ncell_sys_info(channel_number, gas_id);

      if (ncell_info_ptr)
      {
        MSG_GERAN_HIGH_1_G("Store SI3(%d) in surrounding",(int) channel_number.num);
        channel_found = TRUE;

        ncell_info_ptr->gsm.location_area_identification = location_area_identification;
        ncell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
        ncell_info_ptr->gsm.cell_selection_parameters = cell_selection_parameters;
        ncell_info_ptr->gsm.control_channel_description = control_channel_description;
        ncell_info_ptr->gsm.control_channel_description_valid = TRUE;


        ncell_info_ptr->gsm.cell_id_valid = TRUE;
        ncell_info_ptr->gsm.cell_id = cell_identity;


        if (gprs_indicator_flag)
        {
          ncell_info_ptr->gprs_supported = TRUE;
          ncell_info_ptr->gprs.ra_colour = gprs_indicator.ra_colour;
          ncell_info_ptr->gprs.si13_position = gprs_indicator.si13_position;
        }
        else
        {
          ncell_info_ptr->gprs_supported = FALSE;
        }

        /* Shared SI3/4 processing */
        rr_ncell_si3_si4_received(channel_number, ncell_info_ptr, gas_id);

        /***************************************************/
        /* The system information are complete, because we */
        /* don't need to read system information 4, and    */
        /* hence system information 7 and 8                */
        /***************************************************/
        SET_BIT(MASK_SI_3 | MASK_SI_4 | MASK_SI_7 | MASK_SI_8,
                 ncell_info_ptr->gsm.update_message_received);

        if(!si_cache_updated)
        {
          (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_3, 
                                    channel_number,
                                    message_ptr,
                                    &ncell_info_ptr->gsm.location_area_identification,
                                    (uint16*)&(ncell_info_ptr->gsm.cell_id),
                                    gas_id);
          si_cache_updated = TRUE;
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("ncell_info_ptr == NULL");
      }
    }

    /* determine if the channel is in the candidate cell database */
    if (rr_candidate_db_is_candidate_cell(channel_number, gas_id))
    {
      /* store the SI info for the candidate cell */
      rr_candidate_cell_T    *candidate_cell_ptr = NULL;
      rr_candidate_SI_info_T si_info;

      RR_ZERO_STRUCTURE(si_info);

      MSG_GERAN_HIGH_1_G("Storing SI3(%d) in candidate cell DB",(int) channel_number.num);
      channel_found = TRUE;

      si_info.CELL_BAR_ACCESS   = RACH_control_parameters.CELL_BAR_ACCESS;
      si_info.access_control[0] = RACH_control_parameters.access_control[0];
      si_info.access_control[1] = RACH_control_parameters.access_control[1];
      si_info.CELL_BAR_QUALIFY  = cell_selection_parameters.CELL_BAR_QUALIFY;
      si_info.control_channel_description_valid = TRUE;
      si_info.control_channel_description = control_channel_description;
      si_info.GPRS_indicator    = gprs_indicator_flag;
      si_info.lai               = location_area_identification;
      si_info.MS_TXPWR_MAX_CCH  = cell_selection_parameters.MS_TXPWR_MAX_CCH;
      si_info.POWER_OFFSET      = cell_selection_parameters.POWER_OFFSET;
      si_info.RXLEV_ACCESS_MIN  = cell_selection_parameters.RXLEV_ACCESS_MIN;
      si_info.REESTABLISHMENT_ALLOWED = RACH_control_parameters.REESTABLISHMENT_ALLOWED;
      si_info.cell_id_valid     = TRUE;
      si_info.cell_id           = cell_identity;

      if (si2q_pi)
      {
        if (si2q_pos)
        {
          si_info.two_quater_position = SI2Q_POSITION_BCCH_Ext;
        }
        else
        {
          si_info.two_quater_position = SI2Q_POSITION_BCCH_Norm;
        }
      }
      else
      {
        si_info.two_quater_position = SI2Q_POSITION_NONE;
      }

      if (two_ti)
      {
        si_info.two_ter_indicator = RR_SI2TER_EXPECTED;
      }
      else
      {
        si_info.two_ter_indicator = RR_SI2TER_NOT_EXPECTED;
      }

      if (gprs_indicator_flag &&
          (gprs_indicator.si13_position == BCCH_EXTENDED))
      {
        si_info.si13_on_bcch_ext = TRUE;
      }

      /* update rr_candidate_cell_db and rr_available_plmn_db */
      candidate_cell_ptr = rr_candidate_db_store_si_info(channel_number, &si_info, gas_id);
      if (candidate_cell_ptr != NULL)
      {
        rr_plmn_db_update(candidate_cell_ptr, channel_number, gas_id);
      }

      (void)rr_acq_db_add_arfcn(
        channel_number,
        location_area_identification.plmn_id,
        gas_id
      );

      if (si_info.CELL_BAR_QUALIFY != 0)
      {
        MSG_GERAN_HIGH_1_G("Cell=%d is low priority",(int) channel_number.num);
      }

      if(!si_cache_updated)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_3, 
                                  channel_number,
                                  message_ptr,
                                  &location_area_identification,
                                  &cell_identity,
                                  gas_id);
        si_cache_updated = TRUE;
      }
    }

    if (channel_found)
    {
      MSG_GERAN_MED_4_G("SI3-PLMN=%d-%d (%d), LAC=%d", (int) mcc, (int) mnc, (int) includes_pcs_digit,
                                                    (int) location_area_identification.location_area_code);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("SI3(%d) ignored - unknown channel",(int) channel_number.num);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      rr_ms_notify_sys_info_ignored(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
  } /* decoding_ok */
  else
  {
    MSG_GERAN_ERROR_1_G("Cannot decode SI3(%d)",(int) channel_number.num);
  }

} /* rr_handle_system_info_3 */

/**
 * Processes SI4 which is handled for the serving cell, the
 * pending channel data store, and the surrounding cells
 *
 * @param channel_number channel number where the message was received
 */
static void rr_handle_system_info_4(
  ARFCN_T              channel_number,
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t       gas_id
)
{
  ie_descriptor_T *current_IE, *first_IE;
  boolean decoding_ok = FALSE;
  LAI_T location_area_identification;
  cell_selection_parameters_T cell_selection_parameters;
  RACH_control_parameters_T RACH_control_parameters;
  boolean               cbch_channel_information_valid = FALSE;
  boolean               cbch_updated = FALSE; 
  channel_information_T cbch_channel_information;
  mobile_allocation_T cbch_mobile_allocation;
  int SI4_rest_length = 0;
  boolean gprs_indicator_flag = FALSE;
  gprs_indicator_t  gprs_indicator;
  sys_mcc_type mcc = 0;
  sys_mnc_type mnc = 0;
  boolean is_plmn_undefined = FALSE;
  boolean includes_pcs_digit = FALSE;
  boolean cell_identity_flag = FALSE;
  uint16  cell_identity = 0;

  uint8 remaining_length = 0;

  RR_ZERO_STRUCTURE(location_area_identification);
  RR_ZERO_STRUCTURE(cell_selection_parameters);
  RR_ZERO_STRUCTURE(RACH_control_parameters);
  RR_ZERO_STRUCTURE(cbch_channel_information);
  RR_ZERO_STRUCTURE(cbch_mobile_allocation);
  RR_ZERO_STRUCTURE(gprs_indicator);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_4, gas_id);

  /* Save the pointer to the first IE - this is needed later */
  first_IE = current_IE;

  if (current_IE)
  {
    // Mandatory IE - Location Area Identification
    if (current_IE->start_IE)
    {
      rr_decode_location_area_identification(current_IE->start_IE,
                                             &location_area_identification);

      /* extract the MCC/MNC from LAI */
      sys_plmn_get_mcc_mnc(
        location_area_identification.plmn_id,
        &is_plmn_undefined,
        &includes_pcs_digit,
        &mcc,
        &mnc
      );

      decoding_ok = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("LOCATION AREA IDENTIFICATION missing");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - Cell Selection Parameters
      if (current_IE->start_IE)
      {
        rr_decode_cell_selection_parameters(
          current_IE->start_IE,
          &cell_selection_parameters
        );

        decoding_ok = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("CELL SELECTION PARAMETERS missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatory IE - RACH Control Parameters
      if (current_IE->start_IE)
      {
        rr_decode_RACH_control_parameters(
          current_IE->start_IE,
          &RACH_control_parameters
        );

        decoding_ok = TRUE;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("RACH CONTROL PARAMETERS missing");
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Optional IE - CBCH Channel Description
      if (current_IE->start_IE)
      {
        rr_decode_channel_description(
          current_IE->start_IE,
          &cbch_channel_information,
          &decoding_ok,
          gas_id
        );

        cbch_channel_information_valid = decoding_ok;
      }
      else
      {
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Conditional IE - CBCH Mobile Allocation
      if (current_IE->start_IE)
      {
        rr_decode_mobile_allocation(current_IE->start_IE,&cbch_mobile_allocation);
      }

      decoding_ok = TRUE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok)
  {
    // Point to next IE in message
    current_IE = current_IE->next_IE;
    decoding_ok = FALSE;

    if (current_IE)
    {
      // Mandatoey IE - SI4 Reset Octets
      // N.B. This is allowed to be zero length
      if (current_IE->start_IE)
      {
        SI4_rest_length = 20 - (current_IE->start_IE - first_IE->start_IE);

        if (SI4_rest_length > 10)
        {
          MSG_GERAN_ERROR_1_G("Invalid length (%d) for SI4 Rest Octets",(int)SI4_rest_length);
          SI4_rest_length = 10;
        }

        if (SI4_rest_length > 0)
        {
          si4_rest_octets_t si4ro;
          csn_decoder_error_t error_code;

          RR_ZERO_STRUCTURE(si4ro);

          /* Use the CSN Decoder to decode the contents of the message */

          remaining_length = rr_length_to_end_of_ota_msg(
            SYSTEM_INFORMATION_TYPE_4,    // msg_type
            current_IE->start_IE,         // csn1_start_ie_ptr
            SI4_REST_OCTETS,              // csn_ie_type
            gas_id
          );

          error_code = gcsn1_rr_decoder(
            SI4_REST_OCTETS,        // message_type
            current_IE->start_IE,   // csn1_ie_src
            (void *)&si4ro,         // dst
            remaining_length,       // len_to_end_of_buf
            gas_id
          );

          if (error_code == CSN_DECODE_NO_ERROR)
          {
            decoding_ok = TRUE;

            // Selection Parameters (optional)
            if (si4ro.selection_params_flag == valid)
            {
              cell_selection_parameters.CELL_RESELECT_PARAM_IND = TRUE;
              cell_selection_parameters.CELL_BAR_QUALIFY = si4ro.cbq;
              cell_selection_parameters.CELL_RESELECT_OFFSET = si4ro.cell_reselect_offset;
              cell_selection_parameters.TEMPORARY_OFFSET = si4ro.temporary_offset;
              cell_selection_parameters.PENALTY_TIME = si4ro.penalty_time;
            }
            else
            {
              cell_selection_parameters.CELL_RESELECT_PARAM_IND = FALSE;
            }

            // Power Offset (optional)
            if (si4ro.power_offset_flag == valid)
            {
              rr_internal_band_T  band = (rr_internal_band_T)channel_number.band;

              cell_selection_parameters.power_offset_valid = TRUE;

              /*
              See 3GPP 44.018 10.5.2.34 SI 3 Rest Octets
              ... Power Offset is used only by DCS 1800 Class 3 MSs ...
              */
              if (((band == BAND_DCS) || (band == BAND_PCS)) &&
                  (rr_gapi_get_band_rf_power_class(band, gas_id) == (byte) CLASS_3))
              {
                cell_selection_parameters.POWER_OFFSET = (si4ro.power_offset * 2);
              }
              else
              {
                cell_selection_parameters.POWER_OFFSET = 0;
              }
            }
            else
            {
              cell_selection_parameters.power_offset_valid = FALSE;
            }

            // GPRS Indicator (optional)
            if (si4ro.gprs_indicator_flag == valid)
            {
              gprs_indicator_flag = TRUE;
              gprs_indicator.ra_colour = si4ro.ra_colour;

              if (si4ro.si13_position == CSN_BCCH_EXTENDED)
              {
                gprs_indicator.si13_position = BCCH_EXTENDED;
              }
              else
              {
                gprs_indicator.si13_position = BCCH_NORMAL;
              }
            }
            else
            {
              gprs_indicator_flag = FALSE;
            }

            if (si4ro.cell_identity_flag)
            {
              cell_identity_flag = TRUE;
              cell_identity = si4ro.cell_identity;
            }
          }
        }
      }
      else
      {
        MSG_GERAN_MED_0_G("SI4 Rest Octets are not present");
        SI4_rest_length = 0;
        decoding_ok = TRUE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("message_list parsing error");
    }
  }

  if (decoding_ok == TRUE)
  {
    boolean channel_found = FALSE;

    /**************************************************/
    /* We have to store the system information in the */
    /* system_information_database store              */
    /**************************************************/
    if (rr_is_this_the_scell(channel_number, gas_id) && (! rr_background_plmn_search_in_progress(gas_id)))
    {
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
      boolean notify_MM = FALSE;
      boolean notify_L1 = FALSE;
      boolean lai_changed;

      MSG_GERAN_HIGH_1_G("Store SI4(%d) for serving cell",(int) channel_number.num);
      channel_found = TRUE;

      /* See if the reselection parameters are present or should be used, if not copy over old ones before doing diff */
      if ( cell_selection_parameters.ACS==TRUE || SI4_rest_length<=0 )
      {
        /* reselection parameters not present or should not be used, copy over old values*/
        rr_copy_si4_si7_si8_cell_reselection_param(
          &cell_selection_parameters,
          &scell_info_ptr->gsm.cell_selection_parameters
        );
      }

      // Log params if different from SIDB
      rr_log_diff_rach_control_parameters(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_log_diff_cell_selection_parameters(
        &cell_selection_parameters,
        &scell_info_ptr->gsm,
        gas_id
      );

      rr_check_for_cbch_parameters_change(
        &cbch_channel_information,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      lai_changed = (!rr_same_lai(&location_area_identification,
                                  &(scell_info_ptr->gsm.location_area_identification)));

      notify_MM |= lai_changed;

      /* set the LAI value */
      scell_info_ptr->gsm.location_area_identification = location_area_identification;

      /* see if any relevant SI has changed */
      rr_check_for_rach_control_parameters_change(
        &RACH_control_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );

      rr_check_for_cell_selection_parameters_change(
        &cell_selection_parameters,
        &scell_info_ptr->gsm,
        &notify_L1,
        &notify_MM
      );


      /* store the new SI values */
      scell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
      scell_info_ptr->gsm.cell_selection_parameters = cell_selection_parameters;
      scell_info_ptr->gsm.cbch_channel_information = cbch_channel_information;
      scell_info_ptr->gsm.cbch_mobile_allocation = cbch_mobile_allocation;

      if( scell_info_ptr->gsm.cbch_channel_information_valid != cbch_channel_information_valid )
      {
        cbch_updated = cbch_channel_information_valid;
        /* store the CBCH channel information to global variable */
        scell_info_ptr->gsm.cbch_channel_information_valid = cbch_channel_information_valid;
      }

      MSG_GERAN_HIGH_1_G("CBCH channel information included in SI4= %d", (int)cbch_updated);
      if (cell_identity_flag)
      {
        scell_info_ptr->gsm.cell_identity_valid_flag = TRUE;
        scell_info_ptr->gsm.cell_identity = cell_identity;
      }

      /* Mark SI4 as received */
      SET_BIT(MASK_SI_4,scell_info_ptr->gsm.update_message_received);

      /***********************************************************/
      /* Check if we need to read the system information 7 and 8 */
      /***********************************************************/
      if (cell_selection_parameters.ACS == FALSE)
      {
        /* SI7/SI8 are not being sent */
        SET_BIT( MASK_SI_8 | MASK_SI_7,scell_info_ptr->gsm.update_message_received);
      }
      else if ((scell_info_ptr->gsm.update_message_received & (MASK_SI_8 | MASK_SI_7)) == 0)
      {
        /************************************************************************/
        /* Since this is the serving cell and SI3 is required in order to camp, */
        /* and it is assumed that SI7 and SI8 have the same information as SI3  */
        /* RR is not required to read them at this time (this may change if     */
        /* localized service area feature is implemented)                       */
        /************************************************************************/
      }

      rr_perform_MM_or_L1_notification_on_serving_SI_change(
        notify_L1,
        notify_MM,
        gas_id
      );

      if (scell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                  channel_number,
                                  message_ptr,
                                  &scell_info_ptr->gsm.location_area_identification,
                                  &scell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }

      if( cbch_updated )
      {
        rr_send_cell_change_ind(
          location_area_identification,
          scell_info_ptr->gsm.cell_identity,
          cbch_channel_information_valid,
          gas_id
        );
        rr_send_cb_info(cbch_channel_information_valid, gas_id);
      }

    } /* SI is for serving cell */
    /**************************************************/
    /* We have to store the system information in the */
    /* pending_channel_data store                     */
    /**************************************************/
    else if (rr_is_this_the_pcell(channel_number, gas_id))
    {
      gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

      MSG_GERAN_HIGH_1_G("Store SI4(%d) in pending_channel_data",(int) channel_number.num);
      channel_found = TRUE;

      pcell_info_ptr->gsm.location_area_identification = location_area_identification;
      pcell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;
      pcell_info_ptr->gsm.cbch_channel_information_valid = cbch_channel_information_valid;
      pcell_info_ptr->gsm.cbch_channel_information = cbch_channel_information;
      pcell_info_ptr->gsm.cbch_mobile_allocation = cbch_mobile_allocation;

      if ( cell_selection_parameters.ACS==TRUE || SI4_rest_length<=0 )
      {
        rr_copy_si4_si7_si8_cell_reselection_param(
          &cell_selection_parameters,
          &pcell_info_ptr->gsm.cell_selection_parameters
        );
      }

      pcell_info_ptr->gsm.cell_selection_parameters = cell_selection_parameters;

      if (cell_identity_flag)
      {
        pcell_info_ptr->gsm.cell_identity_valid_flag = TRUE;
        pcell_info_ptr->gsm.cell_identity = cell_identity;
      }

      /***********************************************************/
      /* Check if we need to read the system information 7 and 8 */
      /***********************************************************/
      if (cell_selection_parameters.ACS == FALSE)
      {
        SET_BIT(MASK_SI_8|MASK_SI_7,pcell_info_ptr->gsm.update_message_received);
      }
      else if ((pcell_info_ptr->gsm.update_message_received &( MASK_SI_8 | MASK_SI_7 )) == 0)
      {
        /************************************************************************/
        /* Since this is the serving cell and SI3 is required in order to camp, */
        /* and it is assumed that SI7 and SI8 have the same information as SI3  */
        /* RR is not required to read them at this time (this may change if     */
        /* localized service area feature is implemented)                       */
        /************************************************************************/
      }

      /* SI4 has been received */
      SET_BIT(MASK_SI_4,pcell_info_ptr->gsm.update_message_received);

      if (pcell_info_ptr->gsm.cell_identity_valid_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                  channel_number,
                                  message_ptr,
                                  &pcell_info_ptr->gsm.location_area_identification,
                                  &pcell_info_ptr->gsm.cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }

    } /* SI is for pending_channel_data store */

    /* Determine if this is a known neighbour cell */
    if (rr_gprs_is_this_a_surround_ncell(channel_number, gas_id))
    {
      bcch_ncell_info_t *ncell_info_ptr;

      ncell_info_ptr = rr_gprs_get_surround_ncell_sys_info(channel_number, gas_id);

      if (ncell_info_ptr)
      {
        MSG_GERAN_HIGH_1_G("Store SI4(%d) in surrounding",(int) channel_number.num);
        channel_found = TRUE;

        ncell_info_ptr->gsm.location_area_identification = location_area_identification;
        ncell_info_ptr->gsm.RACH_control_parameters = RACH_control_parameters;

        if (cell_identity_flag)
        {
          ncell_info_ptr->gsm.cell_id_valid = TRUE;
          ncell_info_ptr->gsm.cell_id = cell_identity;
        }

        if ( cell_selection_parameters.ACS==TRUE || SI4_rest_length<=0 )
        {
          rr_copy_si4_si7_si8_cell_reselection_param(
            &cell_selection_parameters,
            &ncell_info_ptr->gsm.cell_selection_parameters
          );
        }

        ncell_info_ptr->gsm.cell_selection_parameters = cell_selection_parameters;

        if (gprs_indicator_flag)
        {
          ncell_info_ptr->gprs_supported = TRUE;
          ncell_info_ptr->gprs.ra_colour = gprs_indicator.ra_colour;
          ncell_info_ptr->gprs.si13_position = gprs_indicator.si13_position;
        }
        else
        {
          ncell_info_ptr->gprs_supported = FALSE;
        }

        /* Shared SI3/4 processing */
        rr_ncell_si3_si4_received(channel_number, ncell_info_ptr, gas_id);

        if (cell_identity_flag)
        {
          (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                    channel_number,
                                    message_ptr,
                                    &ncell_info_ptr->gsm.location_area_identification,
                                    (uint16*)&(ncell_info_ptr->gsm.cell_id),
                                    gas_id);
        }
        else
        {
          (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                    channel_number,
                                    message_ptr,
                                    NULL,
                                    NULL,
                                    gas_id);
        }

        /***********************************************************/
        /* Check if we need to read the system information 7 and 8 */
        /* Moreover, the system information 3 doesn't need to be   */
        /* read anymore for that channel                           */
        /***********************************************************/
        if (cell_selection_parameters.ACS == FALSE)
        {
          SET_BIT(MASK_SI_3 | MASK_SI_4 | MASK_SI_7 | MASK_SI_8,
                   ncell_info_ptr->gsm.update_message_received);
        }
        else
        {
          SET_BIT(MASK_SI_3 | MASK_SI_4,ncell_info_ptr->gsm.update_message_received);

          /*******************************************************************/
          /* Note that for surrounding cells it may be necessary to read     */
          /* SI7/SI8 if SI3 has not been read because it could be blocked    */
          /* from view (by the paging channel of the serving cell). However, */
          /* RR should not request L1 to do this if it is currently storing  */
          /* SI4 in the pending channel data in preparation for camping      */
          /*******************************************************************/
          if (!rr_is_this_the_pcell(channel_number, gas_id))
          {
            gprs_surround_data_t  l1_surround_data;

            l1_surround_data.bcch_params.bcch_arfcn = channel_number;
            l1_surround_data.bcch_params.si_requested = GRR_L1_DECODE_SI7_ONLY;
            l1_surround_data.bcch_params.si13_normal_or_ext = BCCH_NORMAL;

            (void)rr_send_mph_gprs_surround_update_req(&l1_surround_data, FALSE, NULL, gas_id);
          }
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("ncell_info_ptr == NULL");
      }
    } /* SI is for surrounding cell */

    /* determine if the channel is in the candidate cell database */
    if (rr_candidate_db_is_candidate_cell(channel_number, gas_id))
    {
      rr_candidate_cell_T    *candidate_cell_ptr = NULL;
      rr_candidate_SI_info_T si_info;

      RR_ZERO_STRUCTURE(si_info);

      MSG_GERAN_HIGH_1_G("Storing SI4(%d) in candidate cell DB",(int) channel_number.num);
      channel_found = TRUE;

      si_info.CELL_BAR_ACCESS   = RACH_control_parameters.CELL_BAR_ACCESS;
      si_info.access_control[0] = RACH_control_parameters.access_control[0];
      si_info.access_control[1] = RACH_control_parameters.access_control[1];
      si_info.CELL_BAR_QUALIFY  = cell_selection_parameters.CELL_BAR_QUALIFY;
      si_info.GPRS_indicator    = gprs_indicator_flag;
      si_info.lai               = location_area_identification;
      si_info.MS_TXPWR_MAX_CCH  = cell_selection_parameters.MS_TXPWR_MAX_CCH;
      si_info.POWER_OFFSET      = cell_selection_parameters.POWER_OFFSET;
      si_info.RXLEV_ACCESS_MIN  = cell_selection_parameters.RXLEV_ACCESS_MIN;
      si_info.REESTABLISHMENT_ALLOWED = RACH_control_parameters.REESTABLISHMENT_ALLOWED;
      if (cell_identity_flag)
      {
        si_info.cell_id_valid = TRUE;
        si_info.cell_id = cell_identity;
      }

      if (gprs_indicator_flag &&
          (gprs_indicator.si13_position == BCCH_EXTENDED))
      {
        si_info.si13_on_bcch_ext = TRUE;
      }

      /* store the system information for the candidate cell */
      candidate_cell_ptr = rr_candidate_db_store_si_info(channel_number, &si_info, gas_id);
      if (candidate_cell_ptr != NULL)
      {
        rr_plmn_db_update(candidate_cell_ptr, channel_number, gas_id);
      }

      (void)rr_acq_db_add_arfcn(
        channel_number,
        location_area_identification.plmn_id,
        gas_id
      );

      if (cell_identity_flag)
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                  channel_number,
                                  message_ptr,
                                  &location_area_identification,
                                  &cell_identity,
                                  gas_id);
      }
      else
      {
        (void)rr_csi_update_entry(SYSTEM_INFORMATION_TYPE_4, 
                                  channel_number,
                                  message_ptr,
                                  NULL,
                                  NULL,
                                  gas_id);
      }
    }

    if (channel_found)
    {
      MSG_GERAN_MED_4_G("SI4-PLMN=%d-%d (%d), LAC=%d", (int) mcc, (int) mnc, (int) includes_pcs_digit,
                                                       (int) location_area_identification.location_area_code);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("SI4(%d) ignored - unknown channel",(int) channel_number.num);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      rr_ms_notify_sys_info_ignored(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
  } /* decoding_ok */
  else
  {
    MSG_GERAN_ERROR_1_G("Cannot decode SI4(%d)",(int) channel_number.num);
  }

} /* rr_handle_system_info_4 */

/**
 * Checks to see if the BA list information has changed and
 * stores the new information in the given SI DB and BA List.
 *
 * @param si_db_ptr     pointer to system information database in which to store new information
 *
 * @param ba_list_ptr   [in/out] BA list to compare and in which to store the new information
 *
 * @param BCCH_frequency_list_ptr
 *                      source BA list to compare and to use as source
 *
 * @param notify_L1_ptr [out] set to TRUE if L1 needs to be notified of the change in BA
 *
 * @return TRUE if the BA has changed, FALSE otherwise
 */

static boolean rr_sys_info_check_for_dedicated_BA_change_and_update(
  system_information_database_T *si_db_ptr,
  BCCH_frequency_list_T         *ba_list_ptr,
  BCCH_frequency_list_T         *BCCH_frequency_list_ptr,
  boolean                       *notify_L1_ptr,
  const gas_id_t                 gas_id
)
{
  boolean         ba_list_has_changed = FALSE;
  rr_ba_lists_t  *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  /* check for a change in BA_IND value in the complete BA list */
  if (rr_ba_lists_ptr->complete_BA_sacch.BA_IND != BCCH_frequency_list_ptr->BA_IND)
  {
    ba_list_has_changed = TRUE;

    /*BA ind has changed must clear entire ba list*/
    MSG_GERAN_HIGH_0_G("Dedicated BA_IND Changed");

    si_db_ptr->valid_dedicated_data_flag = FALSE;
    rr_reset_si5_count(gas_id);
    /******************************************************************************/
    /* When the BA_IND changes, the complete BA list must be rebuilt from scratch */
    /******************************************************************************/
    rr_reset_dedicated_BA_lists(si_db_ptr, gas_id);

    /*************************************************/
    /* Mark the SI5, 5 bis and ter as not received   */
    /*************************************************/
    CLEAR_BIT(MASK_SI_5 | MASK_SI_5_BIS | MASK_SI_5_TER,si_db_ptr->update_message_received);

     /* store new BA_IND for the complete BA list */
    rr_ba_lists_ptr->complete_BA_sacch.BA_IND = BCCH_frequency_list_ptr->BA_IND;

    /* set the BA list */
    rr_set_BA_list(ba_list_ptr,BCCH_frequency_list_ptr);
  }
  else /* BA_IND has not changed */
  {
    /* if RR has valid data and a complete dedicated BA list */
    if (rr_has_complete_dedicated_BA_list(si_db_ptr))
    {
      /* check to see if BA list has changed */
      boolean ba_change = rr_check_BA_list_change(
        BCCH_frequency_list_ptr,
        ba_list_ptr,
        gas_id
      );

      /* if BA information has changed */
      if (ba_change)
      {
        ba_list_has_changed = TRUE;

        /* L1 needs to be notified */
        *notify_L1_ptr = TRUE;

        /* set the new BA list value */
        rr_set_BA_list(ba_list_ptr,BCCH_frequency_list_ptr);

        /* generate the complete dedicated BA list once again */
        rr_generate_complete_dedicated_BA_list(si_db_ptr, gas_id);
      }
      /* otherwise, no change, nothing else needs to be done */
    }
    else /* system_information_database doesn't yet have complete BA list */
    {
      /* store the BA_IND for the complete BA list */
      rr_ba_lists_ptr->complete_BA_sacch.BA_IND = BCCH_frequency_list_ptr->BA_IND;

      /* set the new BA list */
      rr_set_BA_list(ba_list_ptr,BCCH_frequency_list_ptr);
    }
  }

  return ba_list_has_changed;
} /* rr_sys_info_check_for_dedicated_BA_change_and_update */

/**
 * Checks to see if the complete dedicated BA list has been received (based
 * on received SI messages). If the complete BA list has now
 * been received (and it wasn't before) indicates that L1 should be
 * notified of the change in BA list status.
 *
 * @param si_db_ptr           pointer to system information database for which the determination is being performed
 *
 * @param notify_L1_ptr       [out] set to TRUE if the complete BA was just received (wasn't before), otherwise it is NOT modified
 *
 * @param EXT_IND             EXT_IND value received in the message, should be set to 1 if not actually
 *                            received in the message
 *
 * @param EXT_IND_value_valid Indicates if the EXT_IND value is valid (known) or not
 */
static void rr_sys_info_check_for_complete_dedicated_BA_list_received(
  system_information_database_T *si_db_ptr,
  boolean                       *notify_L1_ptr,
  byte                          EXT_IND,
  boolean                       EXT_IND_value_valid,
  const gas_id_t                gas_id
)
{
  /* only perform the check if the EXT_IND value is valid */
  if (EXT_IND_value_valid)
  {
    /* if the mobile only supports the PGSM band */
    if (rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id) && rr_gapi_get_supported_band_count(gas_id) == 1)
    {
      /*
       * dont expect SI_5_BIS or SI_5_TER if PGSM is only supported band
       */
      si_db_ptr->decode_SI5bis = RR_SI_NOT_REQUIRED;
      si_db_ptr->decode_SI5ter = RR_SI_NOT_REQUIRED;
    }
    else /* mobile station does not only support PGSM band */
    {
      if (EXT_IND == 0)
      {
        /* SI5bis is not being sent by the network */
        si_db_ptr->decode_SI5bis = RR_SI_NOT_REQUIRED;
      }
      else
      {
        /* SI5 bis is expected and should be decoded */
        si_db_ptr->decode_SI5bis = RR_SI_REQUIRED;
      }
    }
  } /* EXT_IND_value_valid */

  /***************************************************************************/
  /* Note that there is no way of predicting if SI5ter is being sent or not. */
  /* Therefore, just wait for SI5 and SI5bis. If SI5ter is received, the BA  */
  /* list will be updated accordingly.                                       */
  /***************************************************************************/

  /*****************************************************************************/
  /* set appropriate values of update_message_received based on whether SI5bis */
  /* message is expected or not                                                */
  /*****************************************************************************/
  if (si_db_ptr->decode_SI5bis == RR_SI_NOT_REQUIRED)
  {
    SET_BIT(MASK_SI_5_BIS,si_db_ptr->update_message_received);
  }

  /* if RR does not already have the complete BA list */
  if (!(si_db_ptr->complete_BA_sacch_received))
  {
    /* check if the complete dedicated BA list was just received */
    /* this can be done entirely based on the messages that have been processed */
    if (ARE_SET((MASK_SI_5_BIS | MASK_SI_5),si_db_ptr->update_message_received))
    {
      /* if RR has valid dedicated data already */
      if (si_db_ptr->valid_dedicated_data_flag)
      {
        /***************************************************************/
        /* RR had the complete dedicated data at one point and it has  */
        /* just rebuilt the dedicated BA list. Notify L1 about the new */
        /* BA list just constructed                                    */
        /***************************************************************/
        *notify_L1_ptr = TRUE;
      }
      /* otherwise, wait for RR to get all the dedicated SI data before notifying L1 */

      /* the complete dedicated BA has been received */
      si_db_ptr->complete_BA_sacch_received = TRUE;

      /* generate the complete dedicated BA list */
      rr_generate_complete_dedicated_BA_list(si_db_ptr, gas_id);
    }
  }
  /* otherwise, the complete BA sacch is already received and L1 doesn't have to be notified */

} /* rr_sys_info_check_for_complete_dedicated_BA_list_received */

/**
 * Process SI5 in dedicated. Note that since SI5 is only
 * sent while in dedicated, the message can only be
 * associated with the current serving cell
 */
static void rr_handle_system_info_5(
  const gas_id_t gas_id
)
{
  ie_descriptor_T       * current_IE;
  byte                    EXT_IND = 0;
  boolean                 decoding_ok = TRUE;
  BCCH_frequency_list_T   BCCH_frequency_list;
  gprs_scell_info_t     * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t         * rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  RR_ZERO_STRUCTURE(BCCH_frequency_list);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_5, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }
  else
  {
    /**********************************/
    /* Decode the BCCH Frequency List */
    /**********************************/
    rr_decode_neighbour_cell_description(
      current_IE->start_IE,
      &EXT_IND,
      &BCCH_frequency_list,
      &decoding_ok,
      gas_id
    );
  }

  if (decoding_ok == TRUE)
  {
    /********************************************************************************/
    /* Note: this message is received often while in dedicated mode, so don't print */
    /* message indicating it has been received                                      */
    /********************************************************************************/
    boolean ba_changed = FALSE;
    boolean notify_L1  = FALSE;


    /* After a handover, full dedicated ba list is sent to layer1. However, all
       variants of SI5s may not have been received when RR sends dedicated
       update msg to layer1 for the first time after a handover. In place of
       those SI5 variants that have not been received, RR sends those
       received from old cell. This is done as long as si5 count is <=
       RR_SI5_COUNT_BEFORE_RESET in new cell. After this point, it is assumed
       that those SI5 variants that have not been received are not broadcasted
       by the new cell. Therefore, old SI5s will be deleted.

       This is particlurly used for SI5ter as there is no indication if and when
       SI5ter is received. Assumption is that it should have been received by
       the time RR_SI5_COUNT_BEFORE_RESET no of SI5s have been received , if
       broadcasted.
       */
    /* Check status. Only if list are still used (i.e. return value is TRUE)
       after handover, then increment si5 count. Otherwise set status to
       false which means old cells si5 ba lists should not be sent to layer1
       until after next handover. However, these lists get updated while
       in dedicated with latest SI5 varients received. */
    if (rr_get_status_of_old_cell_si5_ba_lists(gas_id))
    {
      if (rr_get_si5_count(gas_id) <= RR_SI5_COUNT_BEFORE_RESET)
      {
        rr_increment_si5_count(gas_id);
      }
      else
      {
        /* rr_reset_old_si5_ba_list(MASK_SI_5 | MASK_SI_5_BIS | MASK_SI_5_TER); */
        rr_reset_si5_count(gas_id);
        rr_set_status_of_old_cell_si5_ba_lists(FALSE, gas_id);
        notify_L1 = TRUE; //Other variants not received update L1.
      }
    }


    /**************************************************/
    /* We have to store the system information in the */
    /* system_information_database store              */
    /**************************************************/

    /* SI5 has been received (set before checking for change below) */
    SET_BIT(MASK_SI_5,scell_info_ptr->gsm.update_message_received);

    /* check for a change in BA information */
    ba_changed = rr_sys_info_check_for_dedicated_BA_change_and_update(
      &scell_info_ptr->gsm,
      &rr_ba_lists_ptr->SI5_BA_list,
      &BCCH_frequency_list,
      &notify_L1,
      gas_id
    );

    MSG_GERAN_HIGH_0_G("SI5 received");
    if (ba_changed)
    {
      MSG_GERAN_HIGH_0_G("SI5 BA list changed");
    }

    /* SI5 has been received (repeat again in case it was reset because of BA_IND change) */
    SET_BIT(MASK_SI_5,scell_info_ptr->gsm.update_message_received);

    /* check to see if RR has now received complete dedicated BA list (with this message) */
    rr_sys_info_check_for_complete_dedicated_BA_list_received(
      &scell_info_ptr->gsm,
      &notify_L1,
      EXT_IND,
      TRUE,
      gas_id
    );

    if (notify_L1)
    {
      rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
    }
    else
    {
      /* If L1 is not being updated because of the dedicated BA list, check if an */
      /* update is required to change the DTX indication */
      rr_dtx_update_l1_if_required(gas_id);
    }
  } /* decoding_ok */
  else
  {
    ERR("Cannot decode SI5 for serving cell",0,0,0);
  }

} /* rr_handle_system_info_5 */

/**
 * Process the SI5bis message in dedicated
 */
static void rr_handle_system_info_5_bis(
  const gas_id_t gas_id
)
{
  ie_descriptor_T       * current_IE;
  byte                    EXT_IND = 0;
  BCCH_frequency_list_T   BCCH_frequency_list;
  boolean                 decoding_ok = TRUE;
  gprs_scell_info_t     * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t         * rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  RR_ZERO_STRUCTURE(BCCH_frequency_list);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_5_BIS, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }
  else
  {
    /*****************************************/
    /* Decode the Neighbour Cell Description */
    /*****************************************/
    rr_decode_neighbour_cell_description(
      current_IE->start_IE,
      &EXT_IND,
      &BCCH_frequency_list,
      &decoding_ok,
      gas_id
    );
  }

  if (decoding_ok == TRUE)
  {
    boolean ba_changed = FALSE;
    boolean notify_L1  = FALSE;

    /* SI5bis has been received (do this before checking for dedicated_BA_change) */
    SET_BIT(MASK_SI_5_BIS,scell_info_ptr->gsm.update_message_received);

    /********************************************************************************/
    /* Note: this message is received often while in dedicated mode, so don't print */
    /* message indicating it has been received                                      */
    /********************************************************************************/
    /* check for a change in BA information */
    ba_changed = rr_sys_info_check_for_dedicated_BA_change_and_update(
      &scell_info_ptr->gsm,
      &rr_ba_lists_ptr->SI5bis_BA_list,
      &BCCH_frequency_list,
      &notify_L1,
      gas_id
    );

    MSG_GERAN_HIGH_0_G("SI5bis received");
    if (ba_changed)
    {
      MSG_GERAN_HIGH_0_G("SI5bis BA list changed");
    }

    /* SI5bis has been received (repeat again in case it was cleared due to BA change) */
    SET_BIT(MASK_SI_5_BIS,scell_info_ptr->gsm.update_message_received);

    /* check to see if RR has now received complete dedicated BA list (with this message) */
    rr_sys_info_check_for_complete_dedicated_BA_list_received(
      &scell_info_ptr->gsm,
      &notify_L1,
      EXT_IND,
      TRUE,
      gas_id
    );

    if (notify_L1)
    {
      rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
    }
    else
    {
      /* If L1 is not being updated because of the dedicated BA list, check if an */
      /* update is required to change the DTX indication */
      rr_dtx_update_l1_if_required(gas_id);
    }
  } /* decoding_ok */
  else
  {
    ERR("Cannot decode SI5bis for serving cell",0,0,0);
  }
} /* rr_handle_system_info_5_bis */

/**
 * Handles the SI5ter message received while in dedicated (only supported for dedicated mode)
 */
static void rr_handle_system_info_5_ter(
  const gas_id_t gas_id
)
{
  ie_descriptor_T       * current_IE;
  byte                    multiband_reporting = 0;
  BCCH_frequency_list_T   BCCH_frequency_list;
  boolean                 decoding_ok = TRUE;
  gprs_scell_info_t     * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t         * rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  RR_ZERO_STRUCTURE(BCCH_frequency_list);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_5_TER, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }
  else
  {
    /*****************************************/
    /* Decode the Neighbour Cell Description */
    /*****************************************/
    rr_decode_neighbour_cell_description_2(
      current_IE->start_IE,
      &multiband_reporting,
      &BCCH_frequency_list,
      &decoding_ok,
      gas_id
    );
  }

  if (decoding_ok == TRUE)
  {
    /********************************************************************************/
    /* Note: this message is received often while in dedicated mode, so don't print */
    /* message indicating it has been received                                      */
    /********************************************************************************/
    boolean notify_L1 = FALSE;
    boolean ba_changed = FALSE;

    /* since SI5ter was received, mark it as being used to generate the complete BA list */
    /* NOTE: this must be done before checking for BA change and update below, otherwise */
    /* SI5ter would not be included in the complete dedicated BA list generated          */
    scell_info_ptr->gsm.decode_SI5ter = RR_SI_REQUIRED;
    /* SI5ter has been received */
    SET_BIT(MASK_SI_5_TER,scell_info_ptr->gsm.update_message_received);

    /* check for a change in BA information */
    ba_changed = rr_sys_info_check_for_dedicated_BA_change_and_update(
      &scell_info_ptr->gsm,
      &rr_ba_lists_ptr->SI5ter_BA_list,
      &BCCH_frequency_list,
      &notify_L1,
      gas_id
    );

    MSG_GERAN_HIGH_0_G("SI5ter received");
    if (ba_changed)
    {
      MSG_GERAN_HIGH_0_G("SI5ter BA list changed");
    }

    /* check for a change in multiband reporting status */
    if (multiband_reporting != scell_info_ptr->gsm.multiband_reporting)
    {
      MSG_GERAN_HIGH_0_G("SI5ter multiband reporting change");

      /* Regenerate the complete dedicated BA list.  This must be done here
      because if SI6 has not yet been received, the complete dedicated BA list
      will not have been constructued yet and L1 will be sent an out-of-date
      list. */
      rr_generate_complete_dedicated_BA_list(&scell_info_ptr->gsm, gas_id);
      notify_L1 = TRUE;
    }

    /* Repeat setting this values in case they were cleared by a BA_IND change */
    /* since SI5ter was received, mark it as being used to generate the complete BA list */
    scell_info_ptr->gsm.decode_SI5ter = RR_SI_REQUIRED;
    /* SI5ter has been received */
    SET_BIT(MASK_SI_5_TER,scell_info_ptr->gsm.update_message_received);

    /* store the new multiband reporting value */
    scell_info_ptr->gsm.multiband_reporting = multiband_reporting;

    /* check to see if RR has now received complete dedicated BA list (with this message) */
    rr_sys_info_check_for_complete_dedicated_BA_list_received(
      &scell_info_ptr->gsm,
      &notify_L1,
      1,
      FALSE,
      gas_id
    );

    if (notify_L1)
    {
      rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
    }
    else
    {
      /* If L1 is not being updated because of the dedicated BA list, check if an */
      /* update is required to change the DTX indication */
      rr_dtx_update_l1_if_required(gas_id);
    }

    /****************************************************/
    /* Check the completeness of the system information */
    /****************************************************/

    SET_BIT( MASK_SI_5_TER,scell_info_ptr->gsm.update_message_received);
  } /* decoding_ok */
  else
  {
    ERR("Cannot decode SI5ter for serving cell",0,0,0);
  }
} /* rr_handle_system_info_5_ter */

/**
 * Process SI6 message while in dedicated mode
 */
static void rr_handle_system_info_6(
  const gas_id_t gas_id
)
{
  ie_descriptor_T   * current_IE;
  LAI_T               location_area_identification;
  cell_options_T      cell_options;
  word                cell_identity = 0;
  byte                NCC_permitted = 0;
  boolean             notify_L1 = FALSE;
#ifdef FEATURE_GSM_DTM
  /* If the band indicator changes then MM must be informed as it uses
  the band indicator to determine whether to include DCS or PCS in RAC. */
  boolean             notify_MM_band_indicator_changed = FALSE;
#endif /* FEATURE_GSM_DTM */
  si6_rest_octets_t   decoded_si6_ro;
  boolean             si6_ro_present = FALSE;
  boolean             lai_changed = FALSE;
  boolean             log_cell_info = FALSE;
  boolean             cell_identity_valid_flag = FALSE;
  boolean             decoding_ok = TRUE;
  gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  best_six_ncells_t * best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

  uint8 remaining_length = 0;

  RR_ZERO_STRUCTURE(cell_options);
  RR_ZERO_STRUCTURE(decoded_si6_ro);

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_6, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    decoding_ok = FALSE;
  }
  else
  {
    /****************************/
    /* Decode the Cell Identity */
    /****************************/
    cell_identity = rr_decode_cell_identity(current_IE->start_IE);
  }

  if (decoding_ok)
  {
    /*******************************************/
    /* Decode the Location Area Identification */
    /*******************************************/
    current_IE = current_IE->next_IE;

    if (current_IE == NULL)
    {
      decoding_ok = FALSE;
    }
    else
    {
      rr_decode_location_area_identification(current_IE->start_IE,&location_area_identification);
    }
  }

  if (decoding_ok)
  {
    /***************************/
    /* Decode the Cell Options */
    /***************************/
    current_IE = current_IE->next_IE;

    if (current_IE == NULL)
    {
      decoding_ok = FALSE;
    }
    else
    {
      rr_decode_cell_options_on_sacch(current_IE->start_IE, &cell_options);
    }
  }

  if (decoding_ok)
  {
    /****************************/
    /* Decode the NCC permitted */
    /****************************/
    current_IE = current_IE->next_IE;

    if (current_IE == NULL)
    {
      decoding_ok = FALSE;
    }
    else
    {
      NCC_permitted = *(current_IE->start_IE);
    }
  }

  if (decoding_ok)
  {
    /* Decode SI6 rest octets */
    current_IE = current_IE->next_IE;

    if (current_IE == NULL)
    {
      decoding_ok = FALSE;
    }
    else
    {
      if (current_IE->start_IE != NULL)
      {
        csn_decoder_error_t error_code;

        remaining_length = rr_length_to_end_of_ota_msg(
          SYSTEM_INFORMATION_TYPE_6,    // msg_type
          current_IE->start_IE,         // csn1_start_ie_ptr
          SI6_REST_OCTETS,              // csn_ie_type
          gas_id
        );

        error_code = gcsn1_rr_decoder(
          SI6_REST_OCTETS,            // message_type
          current_IE->start_IE,       // csn1_start_ie_ptr
          (void *)&decoded_si6_ro,    // dst
          remaining_length,           // len_to_end_of_buf
          gas_id
        );

        if (error_code == CSN_DECODE_NO_ERROR)
        {
          channel_spec_T *chan_spec_ptr;

          si6_ro_present = TRUE;

          chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);

          if (chan_spec_ptr != NULL)
          {
            MSG_GERAN_HIGH_3_G("SI6 received(SACCH=%d,BCCH=%d):BAND_IND=%d",
                     (int)chan_spec_ptr->channel_info_1_after.frequency_list.channel[0].num,
                     (int)scell_info_ptr->gsm.BCCH_ARFCN.num,
                     (int)decoded_si6_ro.band_ind);
          }
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("SI6 received (no Rest Octets)");
      }
    }
  }

  if (decoding_ok)
  {
    /* notify L1 if dedicated parameters have changed */
    if ((scell_info_ptr->gsm.cell_options.DTX != cell_options.DTX)   ||
        (scell_info_ptr->gsm.cell_options.PWRC != cell_options.PWRC) ||
        (scell_info_ptr->gsm.NCC_permitted != NCC_permitted))
    {
      notify_L1 = TRUE;
    }

    /* if the LAI has changed, invalidate the sys info database */
    if (!rr_same_lai(&location_area_identification,&(scell_info_ptr->gsm.location_area_identification)))
    {
      /* this will force a SI refresh after the dedicated connection is released */
      scell_info_ptr->gsm.valid_data_flag = FALSE;
      scell_info_ptr->gsm.invalidate_si_reason = RR_SI_LAC_CHANGED;
      lai_changed = TRUE;
    }

    /* log any parameter changes */
    rr_log_diff_cell_options(
      &cell_options,
      &scell_info_ptr->gsm,
      gas_id
    );

    cell_identity_valid_flag = scell_info_ptr->gsm.cell_identity_valid_flag;

    if ((!cell_identity_valid_flag) ||
        (scell_info_ptr->gsm.cell_identity != cell_identity) ||
        (lai_changed == TRUE))
    {
      /* cell identity or LAI has changed */
      log_cell_info = TRUE;
    }

    /**************************************************/
    /* Update the system_information_database store   */
    /**************************************************/
    scell_info_ptr->gsm.cell_identity = cell_identity;
    scell_info_ptr->gsm.cell_identity_valid_flag = TRUE;
    scell_info_ptr->gsm.location_area_identification = location_area_identification;
    scell_info_ptr->gsm.cell_options = cell_options;

    if (log_cell_info == TRUE)
    {
      /* cell info has changed - log the new data from the system info DB */
      (void)rr_log_cell_info(&scell_info_ptr->gsm, gas_id);

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
      if (best_six_ncells_ptr != NULL)
      {
        rr_notify_cell_id_change(
          cell_identity,
          *best_six_ncells_ptr,
          scell_info_ptr->gsm.location_area_identification,
          gas_id
        );
      }
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/

      {
        gprs_scell_info_t *serving_cell_ptr = NULL;
        eng_mode_scell_info_t scell_info;

        RR_ZERO_STRUCTURE(scell_info);

        serving_cell_ptr = rr_gprs_get_scell_info(gas_id);

        scell_info = geran_eng_mode_format_scell_info(
          serving_cell_ptr,    // scell_info_ptr
          ENG_MODE_NO_CAUSE,   // cause
          gas_id               // gas_id
        );

        geran_eng_mode_data_write(
          ENG_MODE_SCELL_INFO,   // geran_eng_mode_info_type
          (void*)&scell_info,    // param_copy
          TRUE,                  // valid_flag
          gas_id                 // gas_id
        );
      }

#ifdef FEATURE_QSH_EVENT_METRIC
      rr_qsh_metric_log_cell_info(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
    }

    if (!scell_info_ptr->gsm.RLT_valid)
    {
      /* Reset the S_counter to RADIO_LINK_TIMEOUT */
      rr_reset_S_counter(gas_id);
      scell_info_ptr->gsm.RLT_valid = TRUE;
    }

    scell_info_ptr->gsm.NCC_permitted = NCC_permitted;

    if (si6_ro_present == TRUE)
    {
      si_psi_band_ind_T prev_indicator;

      prev_indicator = scell_info_ptr->gsm.band_indicator;
      scell_info_ptr->gsm.band_indicator = (si_psi_band_ind_T)decoded_si6_ro.band_ind;

      if (rr_is_camped(gas_id) == FALSE)
      {
        /* If RR is not camped during dedicated mode, this implies a W2G scenario, */
        /* so calls to rr_get_band will use the indicator in pending_channel_data. */
        /* Ensure that this database is also updated with the latest NW band info. */
        pcell_info_ptr->gsm.band_indicator = (si_psi_band_ind_T)decoded_si6_ro.band_ind;
        MSG_GERAN_MED_1_G("Set band indicator in pending channel DB to %1d",(int)decoded_si6_ro.band_ind);
      }
      else if (scell_info_ptr->gsm.band_indicator != prev_indicator)
      {
        /* output a warning message if the BAND INDICATOR has changed whilst camped */
        MSG_GERAN_HIGH_0_G("WARNING: band indicator has changed during dedicated mode");
        pcell_info_ptr->gsm.band_indicator = (si_psi_band_ind_T)decoded_si6_ro.band_ind;
        rr_gprs_reassign_band(0xffff, TRUE, gas_id);
        notify_L1 = TRUE;
#ifdef FEATURE_GSM_DTM
        notify_MM_band_indicator_changed = TRUE;
#endif /* FEATURE_GSM_DTM */
      }
    }

    /* SI6 has been received */
    SET_BIT(MASK_SI_6, scell_info_ptr->gsm.update_message_received);

    if (notify_L1)
    {
      /* update the dedicated mode */
      rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
    }
    else
    {
      /* If L1 is not being updated because of the dedicated BA list, check if an */
      /* update is required to change the DTX indication */
      rr_dtx_update_l1_if_required(gas_id);
    }

#ifdef FEATURE_GSM_DTM

    /* Assume DTM support is not available - then update below */
    scell_info_ptr->dtm_sys_info_valid = FALSE;
    scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info.dtm_support = FALSE;

    if (si6_ro_present && decoded_si6_ro.dtm_supp)
    {
      scell_info_ptr->bcch_sys_info.routing_area_code = decoded_si6_ro.rac;

      scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info_flag = valid;
      scell_info_ptr->bcch_sys_info.gprs_cell_options.optional_extension_info.dtm_support = TRUE;

      scell_info_ptr->dtm_sys_info.max_LAPDm = decoded_si6_ro.max_lapdm;

      if (decoded_si6_ro.gprs_ms_tmc_flag)
      {
        scell_info_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch_valid = TRUE;
        scell_info_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch = decoded_si6_ro.gprs_ms_tmc;
      }
      else
      {
        scell_info_ptr->dtm_sys_info.gprs_ms_txpwr_max_cch_valid = FALSE;
      }

      scell_info_ptr->dtm_sys_info_valid = TRUE;

      /* The fact that DTM is supported must mean the cell supports GPRS. The */
      /* gprs_indicator would normally be set from the contents of SI3, but this */
      /* won't be received during a call */
      scell_info_ptr->gsm.gprs_indicator = TRUE;
    }

    if ((cell_identity_valid_flag == FALSE) || notify_MM_band_indicator_changed)
    {
      /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
      grr_update_mm_with_rr_service_ind(gas_id);
    }

    if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
    {
      /*
      The indication of SI6 reception is not passed directly into rr_dtm_control, but via
      rr_handle_procedures. This is because the SI6 indication (wrt to DTM) needs to be
      blocked if a RR/DTM procedure is ongoing - the DTM module should only act on the
      SI6 contents at a time when GPRS can be enabled (if DTM is indicated as supported).
      So rr_handle_procedures will only pass this onto rr_dtm_control when in
      DISP_INACTIVE_STATE.
      */
      (void) rr_handle_procedures(EV_DTM_SI6_RECEIVED, NULL, gas_id);
    }

#else

    if ((cell_identity_valid_flag == FALSE) || notify_MM_band_indicator_changed)
    {
      /* Send RR_SERVICE_IND_FROM_OTA_MSG to MM */
      grr_update_mm_with_rr_service_ind(gas_id);
    }

#endif /* FEATURE_GSM_DTM */
  }

} /* rr_handle_system_info_6 */

/**
 * Handle SI7 in idle mode. SI7 is currently only expected
 * for surrounding cells
 *
 * @param channel_number       ARFCN where the channel was received
 */
static void rr_handle_system_info_7(
  ARFCN_T channel_number,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  bcch_ncell_info_t *ncell_info_ptr = NULL;

  uint8 remaining_length = 0;

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_7, gas_id);

  if (current_IE)
  {
    if (current_IE->start_IE)
    {
      csn_decoder_error_t  error_code = UNKNOWN_MESSAGE_TYPE;
      si7_rest_octets_t  si7ro;

      RR_ZERO_STRUCTURE(si7ro);

      if (rr_gprs_is_this_a_surround_ncell(channel_number, gas_id))
      {
        ncell_info_ptr = rr_gprs_get_surround_ncell_sys_info(
          channel_number,
          gas_id
        );

        if (ncell_info_ptr)
        {
          si7ro.si4_acs_flag = ncell_info_ptr->gsm.cell_selection_parameters.ACS;

          /* Use the CSN Decoder to decode the contents of the message */

          remaining_length = rr_length_to_end_of_ota_msg(
            SYSTEM_INFORMATION_TYPE_7,    // msg_type
            current_IE->start_IE,         // csn1_start_ie_ptr
            SI7_REST_OCTETS,              // csn_ie_type
            gas_id
          );

          error_code = gcsn1_rr_decoder(
            SI7_REST_OCTETS,        // message_type
            current_IE->start_IE,   // csn1_ie_src
            (void *)&si7ro,         // dst
            remaining_length,       // len_to_end_of_buf
            gas_id
          );
        }
        else
        {
          error_code = UNKNOWN_MESSAGE_TYPE;
          MSG_GERAN_ERROR_1_G("RR SI7(%d) ncell_info_ptr == NULL",channel_number.num);
        }
      }
      else
      {
        if (!rr_gprs_nacc_pncd_in_progress(gas_id))
        {
          MSG_GERAN_ERROR_1_G("SI7(%d) ignored, wrong channel",(int) channel_number.num);
        }
        error_code = UNKNOWN_MESSAGE_TYPE;
      }

      if (error_code == CSN_DECODE_NO_ERROR)
      {
        boolean           gprs_indicator_flag;
        gprs_indicator_t  gprs_indicator;
        cell_selection_parameters_T  cell_selection_parameters;

        RR_ZERO_STRUCTURE(gprs_indicator);
        RR_ZERO_STRUCTURE(cell_selection_parameters);

        // Selection Parameters (optional)
        if (si7ro.selection_params_flag == valid)
        {
          cell_selection_parameters.CELL_RESELECT_PARAM_IND = TRUE;
          cell_selection_parameters.CELL_BAR_QUALIFY = si7ro.cbq;
          cell_selection_parameters.CELL_RESELECT_OFFSET = si7ro.cell_reselect_offset;
          cell_selection_parameters.TEMPORARY_OFFSET = si7ro.temporary_offset;
          cell_selection_parameters.PENALTY_TIME = si7ro.penalty_time;
        }
        else
        {
          cell_selection_parameters.CELL_RESELECT_PARAM_IND = FALSE;
        }

        // Power Offset (optional)
        if (si7ro.power_offset_flag == valid)
        {
          rr_internal_band_T  band = (rr_internal_band_T)channel_number.band;

          cell_selection_parameters.power_offset_valid = TRUE;

          /*
          See 3GPP 44.018 10.5.2.34 SI 3 Rest Octets
          ... Power Offset is used only by DCS 1800 Class 3 MSs ...
          */
          if (((band == BAND_DCS) || (band == BAND_PCS)) &&
              (rr_gapi_get_band_rf_power_class(band, gas_id) == (byte) CLASS_3))
          {
            cell_selection_parameters.POWER_OFFSET = (si7ro.power_offset * 2);
          }
          else
          {
            cell_selection_parameters.POWER_OFFSET = 0;
          }
        }
        else
        {
          cell_selection_parameters.power_offset_valid = FALSE;
        }

        // GPRS Indicator (optional)
        if (si7ro.gprs_indicator_flag == valid)
        {
          gprs_indicator_flag = TRUE;
          gprs_indicator.ra_colour = si7ro.ra_colour;

          if (si7ro.si13_position == CSN_BCCH_EXTENDED)
          {
            gprs_indicator.si13_position = BCCH_EXTENDED;
          }
          else
          {
            gprs_indicator.si13_position = BCCH_NORMAL;
          }
        }
        else
        {
          gprs_indicator_flag = FALSE;
        }

        MSG_GERAN_HIGH_1_G("Store SI7(%d) in surrounding",(int) channel_number.num);

        /*************************************************************************************/
        /* Note: Not all parameters in the cell selection parameters are received in SI7/SI8 */
        /* DO NOT assign the structure directly since some of the information will be lost.  */
        /* Field-by-field assignment is required here                                        */
        /*************************************************************************************/
        if (ncell_info_ptr) /* to satisfy Lint */
        {
          rr_copy_si4_si7_si8_cell_reselection_param(
            &ncell_info_ptr->gsm.cell_selection_parameters,
            &cell_selection_parameters);

          if (gprs_indicator_flag)
          {
            ncell_info_ptr->gprs_supported = TRUE;
            ncell_info_ptr->gprs.ra_colour = gprs_indicator.ra_colour;
            ncell_info_ptr->gprs.si13_position = gprs_indicator.si13_position;
          }
          else
          {
            ncell_info_ptr->gprs_supported = FALSE;
          }

          /* Shared SI7/8 processing */
          /* Note: This assumes the SI data for the ncell has been stored */
          rr_ncell_si7_si8_received(channel_number, ncell_info_ptr, gas_id);

          /* only one of SI7 or SI8 is required, not both */
          SET_BIT( MASK_SI_7 | MASK_SI_8,ncell_info_ptr->gsm.update_message_received);
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("SI7 rest octets not present");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

} /* rr_handle_system_info_7 */

/**
 * Process SI8 for a surrounding cell
 *
 * @param channel_number ARFCN where the message is received
 */
static void rr_handle_system_info_8(
  ARFCN_T channel_number,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  bcch_ncell_info_t *ncell_info_ptr = NULL;

  uint8 remaining_length = 0;

  /* Get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(SYSTEM_INFORMATION_TYPE_8, gas_id);

  if (current_IE)
  {
    if (current_IE->start_IE)
    {
      si8_rest_octets_t si8ro;
      csn_decoder_error_t error_code = UNKNOWN_MESSAGE_TYPE;

      RR_ZERO_STRUCTURE(si8ro);

      if (rr_gprs_is_this_a_surround_ncell(channel_number, gas_id))
      {
        ncell_info_ptr = rr_gprs_get_surround_ncell_sys_info(channel_number, gas_id);

        if (ncell_info_ptr)
        {
          si8ro.si4_acs_flag = ncell_info_ptr->gsm.cell_selection_parameters.ACS;

          /* Use the CSN Decoder to decode the contents of the message */

          remaining_length = rr_length_to_end_of_ota_msg(
            SYSTEM_INFORMATION_TYPE_8,    // msg_type
            current_IE->start_IE,         // csn1_start_ie_ptr
            SI8_REST_OCTETS,              // csn_ie_type
            gas_id
          );

          error_code = gcsn1_rr_decoder(
            SI8_REST_OCTETS,        // message_type
            current_IE->start_IE,   // csn1_ie_src
            (void *)&si8ro,         // dst
            remaining_length,       // len_to_end_of_buf
            gas_id
          );
        }
        else
        {
          error_code = UNKNOWN_MESSAGE_TYPE;
          MSG_GERAN_ERROR_1_G("RR SI8(%d) ncell_info_ptr == NULL",channel_number.num);
        }
      }
      else
      {
        if (!rr_gprs_nacc_pncd_in_progress(gas_id))
        {
          MSG_GERAN_ERROR_1_G("SI8(%d) ignored, wrong channel",(int) channel_number.num);
        }
        error_code = UNKNOWN_MESSAGE_TYPE;
      }

      if (error_code == CSN_DECODE_NO_ERROR)
      {
        boolean           gprs_indicator_flag;
        gprs_indicator_t  gprs_indicator;
        cell_selection_parameters_T  cell_selection_parameters;

        RR_ZERO_STRUCTURE(gprs_indicator);
        RR_ZERO_STRUCTURE(cell_selection_parameters);

        // Selection Parameters (optional)
        if (si8ro.selection_params_flag == valid)
        {
          cell_selection_parameters.CELL_RESELECT_PARAM_IND = TRUE;
          cell_selection_parameters.CELL_BAR_QUALIFY = si8ro.cbq;
          cell_selection_parameters.CELL_RESELECT_OFFSET = si8ro.cell_reselect_offset;
          cell_selection_parameters.TEMPORARY_OFFSET = si8ro.temporary_offset;
          cell_selection_parameters.PENALTY_TIME = si8ro.penalty_time;
        }
        else
        {
          cell_selection_parameters.CELL_RESELECT_PARAM_IND = FALSE;
        }

        // Power Offset (optional)
        if (si8ro.power_offset_flag == valid)
        {
          rr_internal_band_T  band = (rr_internal_band_T)channel_number.band;

          cell_selection_parameters.power_offset_valid = TRUE;

          /*
          See 3GPP 44.018 10.5.2.34 SI 3 Rest Octets
          ... Power Offset is used only by DCS 1800 Class 3 MSs ...
          */
          if (((band == BAND_DCS) || (band == BAND_PCS)) &&
              (rr_gapi_get_band_rf_power_class(band, gas_id) == (byte) CLASS_3))
          {
            cell_selection_parameters.POWER_OFFSET = (si8ro.power_offset * 2);
          }
          else
          {
            cell_selection_parameters.POWER_OFFSET = 0;
          }
        }
        else
        {
          cell_selection_parameters.power_offset_valid = FALSE;
        }

        // GPRS Indicator (optional)
        if (si8ro.gprs_indicator_flag == valid)
        {
          gprs_indicator_flag = TRUE;
          gprs_indicator.ra_colour = si8ro.ra_colour;

          if (si8ro.si13_position == CSN_BCCH_EXTENDED)
          {
            gprs_indicator.si13_position = BCCH_EXTENDED;
          }
          else
          {
            gprs_indicator.si13_position = BCCH_NORMAL;
          }
        }
        else
        {
          gprs_indicator_flag = FALSE;
        }

        MSG_GERAN_HIGH_1_G("Store SI8(%d) in surrounding",(int) channel_number.num);

        /*************************************************************************************/
        /* Note: Not all parameters in the cell selection parameters are received in SI7/SI8 */
        /* DO NOT assign the structure directly since some of the information will be lost.  */
        /* Field-by-field assignment is required here                                        */
        /*************************************************************************************/
        if (ncell_info_ptr) /* to satisfy Lint */
        {
          rr_copy_si4_si7_si8_cell_reselection_param(
            &ncell_info_ptr->gsm.cell_selection_parameters,
            &cell_selection_parameters);

          if (gprs_indicator_flag)
          {
            ncell_info_ptr->gprs_supported = TRUE;
            ncell_info_ptr->gprs.ra_colour = gprs_indicator.ra_colour;
            ncell_info_ptr->gprs.si13_position = gprs_indicator.si13_position;
          }
          else
          {
            ncell_info_ptr->gprs_supported = FALSE;
          }

          /* Shared SI7/8 processing */
          /* Note: This assumes the SI data for the ncell has been stored */
          rr_ncell_si7_si8_received(channel_number, ncell_info_ptr, gas_id);

          /* only one of SI7 or SI8 is required, not both */
          SET_BIT( MASK_SI_7 | MASK_SI_8,ncell_info_ptr->gsm.update_message_received);
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("SI8 rest octets not present");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("message_list parsing error");
  }

} /* rr_handle_system_info_8 */


/**
 * Store a system information message received in the BCCH
 *
 * @param new_message Pointer to the received message (DL_UNIT_DATA_IND)
 */
void rr_store_system_info(
  dl_unit_data_ind_T  *message_ptr,
  const gas_id_t gas_id
)
{
  l2_channel_type_T l2_channel_type;
  byte message_type;
  ARFCN_T channel_number;
  boolean message_ok;
  RR_cause_T error_type;
  information_T *surround_db_entry_ptr = NULL;

  boolean dedicated_si_message = FALSE;
  boolean is_cell_barred = FALSE ;  

  boolean short_header;

  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  /**********************************************************************************************/
  /* remember the old state of the control_channel_description_valid flag in pending data store */
  /* to see if its status changes when the message is processed                                 */
  /**********************************************************************************************/
  boolean old_pending_control_desc_valid;


  rr_si_type_t si_message_received = SI_TYPE_NONE;

  old_pending_control_desc_valid = pcell_info_ptr->gsm.control_channel_description_valid;

  ASSERT (message_ptr != NULL);

  l2_channel_type = message_ptr->l2_channel_type;
  short_header = message_ptr->short_header;
  channel_number = message_ptr->channel_number;

  /* prepare the message for parsing and check that it is OK for processing */
  if (message_ptr->l3_message_length <= N201_MAX)
  {  
    rr_check_for_protocol_errors(
      message_ptr->layer3_message,
      message_ptr->l3_message_length,
      message_ptr->l2_channel_type,
      short_header,
      &message_ok,
      &error_type,
      gas_id
    );
  }
  else
  {
    error_type = RR_SEMANTICALLY_INCORRECT_MESSAGE;
    MSG_GERAN_MED_3_G("Message length exceeds message payload size:Received message length: %d,expected:%d, cause=%d",
      message_ptr->l3_message_length,
      N201_MAX,
      (int) error_type
    );
    message_ok = FALSE;   
  }

  if (message_ok == TRUE)
  {
    /*********************************************************************/
    /* If the channel type is not SACCH, and therefore either BCCH or    */
    /* CCCH, then its a SI message, and                                  */
    /* these have an extra l2 pseudo length byte in the layer 3 message, */
    /* so the message type field is one byte further on                  */
    /*********************************************************************/
    if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
    {
      message_type = message_ptr->layer3_message[2];
    }

    else if ((l2_channel_type == SACCH) && (short_header == TRUE))
    {
      message_type = message_ptr->layer3_message[0] & 0x7C;

      /* shift right by 2 bits */
      message_type >>= 2;

      /* MEASUREMENT_INFORMATION is 0x5 which is also SYSTEM_INFORMATION_TYPE_5_BIS */
      /* So, change it to a different unused value */
      if (message_type == MEASUREMENT_INFORMATION)
      {
        message_type = MEASUREMENT_INFORMATION_TYPE;
      }
      else
      {
        /* Set to any other message type will be dropped at RR control */
        message_type = UNKNOWN_SHORT_HEADER_MESSAGE_TYPE;
      }
    }

    else
    {
      message_type = message_ptr->layer3_message[1];
    }

#ifdef FEATURE_WCDMA
    /* during a W2G handover, before reaching dedicated mode, */
    /* ignore SI5/5bis/5ter and SI6 - must be trapped here as */
    /* these System Information messages come through rr_main */
    if (get_rr_state(gas_id) == RR_W2G_INTERRAT_HANDOVER_PROGRESS)
    {
      if ((message_type == SYSTEM_INFORMATION_TYPE_5)     ||
          (message_type == SYSTEM_INFORMATION_TYPE_5_BIS) ||
          (message_type == SYSTEM_INFORMATION_TYPE_5_TER) ||
          (message_type == SYSTEM_INFORMATION_TYPE_6))
      {
        MSG_GERAN_HIGH_1_G("GRR ignoring SI type 0x%02X until dedicated mode is ready", (int)message_type);
        return;
      }
    }
#endif /* FEATURE_WCDMA */

    /* Before processing the message, determine if network sends SI1 messages */
    /* SI1 messages are not in use if other other messages are recvd on TC 0  */
    if((l2_channel_type == BCCH)                                    &&
       (channel_number.num == scell_info_ptr->gsm.BCCH_ARFCN.num)
       && (channel_number.band == scell_info_ptr->gsm.BCCH_ARFCN.band)
       && rr_is_camped(gas_id)
      )
    {
      /*******************************************************************/
      /* Determine whether SI1 is used by the network - SERVING CELL     */
      /*******************************************************************/
      if ((message_ptr->tc == 0) && SI_NOT_EXPECTED_ON_TC0(message_type))
      {
        MSG_GERAN_HIGH_3_G("Serving=%d:msg=%d,last=%d;SI1 not used",
                 (int)channel_number.num,message_type,scell_info_ptr->gsm.last_tc);
        scell_info_ptr->gsm.update_message_received |= MASK_SI_1;
      }
      scell_info_ptr->gsm.last_tc = message_ptr->tc;
    }
    else if(l2_channel_type == BCCH && (channel_number.num == pcell_info_ptr->gsm.BCCH_ARFCN.num)
          && (channel_number.band == pcell_info_ptr->gsm.BCCH_ARFCN.band)
      )
    {
      /*******************************************************************/
      /* Determine whether SI1 is used by the network - PENDING CELL     */
      /*******************************************************************/
      if ((message_ptr->tc == 0) && SI_NOT_EXPECTED_ON_TC0(message_type))
      {
        MSG_GERAN_HIGH_3_G("Pending=%d:msg=%d,last=%d;SI1 not used",
                 (int)channel_number.num,message_type,pcell_info_ptr->gsm.last_tc);
        pcell_info_ptr->gsm.update_message_received |= MASK_SI_1;
      }
      pcell_info_ptr->gsm.last_tc = message_ptr->tc;
    }

    /* switch on the message type */
    switch (message_type)
    {
      case SYSTEM_INFORMATION_TYPE_1:
      {
        rr_handle_system_info_1(channel_number, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_1;
        break;
      }

      case SYSTEM_INFORMATION_TYPE_2:
      {
        rr_handle_system_info_2(channel_number, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_2;
        break;
      }

      case SYSTEM_INFORMATION_TYPE_2_BIS:
      {
        /**********************************************************************/
        /* SI2bis message can be ignored by MSs supporting only the PGSM band */
        /**********************************************************************/
        if ((rr_gapi_get_supported_band_count(gas_id) == 1) && rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id))
        {
          MSG_GERAN_HIGH_1_G("Ignoring SI2bis(%d)",(int) channel_number.num);
        }
        else
        {
          /* message must be handled */
          rr_handle_system_info_2_bis(channel_number, message_ptr, gas_id);
          si_message_received = SI_TYPE_SI_2_BIS;
        }
        break;
      }

      case SYSTEM_INFORMATION_TYPE_2_TER:
      {
        rr_handle_system_info_2_ter(channel_number, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_2_TER;
        break;
      }

      case SYSTEM_INFORMATION_TYPE_2_QUATER:
      {
        (void)rr_handle_system_info_2_quater(channel_number, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_2_QUATER;
        break;
      }

      case MEASUREMENT_INFORMATION_TYPE:
      {
        dedicated_si_message = TRUE;
        MSG_GERAN_HIGH_1_G("Received Measurement Information Message (%d)",(int) channel_number.num);
        rr_handle_measurement_information(gas_id);
        break;
      }

      case SYSTEM_INFORMATION_TYPE_3:
      {
        rr_handle_system_info_3(channel_number, message_ptr->tc, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_3;
        /* ensure the SI cache gets a copy of the data */
        break;
      }

      case SYSTEM_INFORMATION_TYPE_4:
      {
        rr_handle_system_info_4(channel_number, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_4;
        /* ensure the SI cache gets a copy of the data */
        break;
      }

      case SYSTEM_INFORMATION_TYPE_5:
      {
        rr_handle_system_info_5(gas_id);
        dedicated_si_message = TRUE;
        break;
      }

      case SYSTEM_INFORMATION_TYPE_5_BIS:
      {
        /**********************************************************************/
        /* SI5bis message can be ignored by MSs supporting only the PGSM band */
        /**********************************************************************/
        if ((rr_gapi_get_supported_band_count(gas_id) == 1) && rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id))
        {
          MSG_GERAN_HIGH_1_G("Ignoring SI5bis(%d)",(int) channel_number.num);
        }
        else
        {
          dedicated_si_message = TRUE;
          /* message must be handled */
          rr_handle_system_info_5_bis(gas_id);
        }
        break;
      }

      case SYSTEM_INFORMATION_TYPE_5_TER:
      {
        /***********************************************************************/
        /* SI2ter messages can be ignored by MSs supporting only a single band */
        /* if the only supported band is either the PGSM or DCS1800 band       */
        /***********************************************************************/
        if ((rr_gapi_get_supported_band_count(gas_id) == 1)  &&
             (rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id) ||
               rr_gapi_is_supported_band(SYS_BAND_DCS_1800, gas_id)
             )
           )
        {
          /* message can be ignored */
          MSG_GERAN_HIGH_1_G("Ignoring SI5ter(%d)",(int) channel_number.num);
        }
        else
        {
          dedicated_si_message = TRUE;
          rr_handle_system_info_5_ter(gas_id);
        }
        break;
      }

      case SYSTEM_INFORMATION_TYPE_6:
      {
        dedicated_si_message = TRUE;
        rr_handle_system_info_6(gas_id);
        break;
      }

      case SYSTEM_INFORMATION_TYPE_7:
      {
        rr_handle_system_info_7(channel_number, gas_id);
        si_message_received = SI_TYPE_SI_7;
        break;
      }

      case SYSTEM_INFORMATION_TYPE_8:
      {
        rr_handle_system_info_8(channel_number, gas_id);
        si_message_received = SI_TYPE_SI_8;
        break;
      }

      case SYSTEM_INFORMATION_TYPE_13:
      {
        rr_process_si13_from_bcch(channel_number, message_ptr, gas_id);
        si_message_received = SI_TYPE_SI_13;
        break;
      }

      case UNKNOWN_SHORT_HEADER_MESSAGE_TYPE:
      {
        MSG_GERAN_HIGH_1_G("Unknown Short Header Message type",message_type);
        break;
      }

      default:
      {
        MSG_GERAN_ERROR_1_G("Unknown SI type (%d)",(int)message_type);
      }
    }

    /***************************************************************************************/
    /* The following sections of code may generate one or more events to be processed by   */
    /* the RR main control. They will be processed in the order they are inserted into the */
    /* event array                                                                         */
    /***************************************************************************************/

    /*******************************************************************************/
    /* SYS INFO MESSAGES RECEIVED WHILE IN IDLE                                    */
    /*  This does not include those received from the current serving cell which   */
    /*  we are camped-on - this is dealt with later.                               */
    /* This section  includes sys info messages from the pending cell which we     */
    /* want to camp on, or one of the surround neighbour cells                     */
    /*******************************************************************************/

    /* only generate the following events for idle SI messages */
    if (!dedicated_si_message)
    {
      /* only check the pending channel and candidate databases if not camped */
      if (!rr_is_camped(gas_id) && rr_is_this_the_pcell(channel_number, gas_id))
      {
        /*******************************************************************************/
        /* SYS INFO MESSAGES RECEIVED FROM THE PENDING CELL                            */
        /*  These are sys info messages received from the cell we are hoping to camp   */
        /*  onto. Reception of these generate certain events at various stages of the  */
        /*  acquisition process.                                                       */
        /*******************************************************************************/

        /* Inform RR-SELECT-BCCH module each time an SI is received to update its internal
        tracking */
        rr_select_bcch_send_imsg_bsi_received_ind(si_message_received, gas_id);

        if (pcell_info_ptr->gsm.generate_si_received_event)
        {
          uint16 si_required_mask;
          uint16 si_pending_mask;
          
          if (rr_is_fast_si_acq_enabled(NULL, gas_id))
          {
            si_required_mask = MASK_SI_3 |
                               MASK_SI_1;
          }
          else
          {
            si_required_mask = MASK_SI_8 |
                               MASK_SI_7 |
                               MASK_SI_4 |
                               MASK_SI_3 |
                               MASK_SI_2_BIS |
                               MASK_SI_2_TER |
                               MASK_SI_2 |
                               MASK_SI_1;
          }

          si_pending_mask = si_required_mask;
          CLEAR_BIT(pcell_info_ptr->gsm.update_message_received, si_pending_mask);

          MSG_GERAN_HIGH_3_G("SI Required Mask: 0x%02x, SI Received Mask: 0x%02x, SI Pending Mask: 0x%02x",
                             si_required_mask,
                             pcell_info_ptr->gsm.update_message_received,
                             si_pending_mask);

          /* if all the required SI has been read into the pending channel data */
          if (ARE_SET(si_required_mask,
                      pcell_info_ptr->gsm.update_message_received))
          {
            rr_send_priority_imsg_rr_event_ind(RR_EV_GSM_SI_RECEIVED, gas_id);

            rr_cell_acq_send_imsg_gsm_bsi_received_ind(gas_id);

            /* cancel the GSM system information timer, which is only used while waiting for pending channel data SI */
            (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);

            /* set the valid data flag to true, RR is ready to camp on this cell */
            pcell_info_ptr->gsm.valid_data_flag = TRUE;

            /* If SI13 is expected then generate an event when it arrives */
            if (pcell_info_ptr->bcch_sys_info.si13_expected_flag)
            {
              if (pcell_info_ptr->bcch_sys_info.si13_data_valid_flag == FALSE)
              {
                pcell_info_ptr->bcch_sys_info.generate_si_received_event = TRUE;
              }
            }

            /* generate the complete BA list (with whatever has been received so far) */
            rr_generate_complete_idle_BA_list(&pcell_info_ptr->gsm, IGNORE_RCVD_SI_MASK, gas_id);
          }
          else
          {
            pcell_info_ptr->bcch_sys_info.generate_si_received_event = FALSE;
          }

          /******************************************************************************/
          /* if the pending channel data's control channel description has become valid */
          /* but the data needed to fully camp is not yet valid                         */
          /******************************************************************************/
          if ((old_pending_control_desc_valid == FALSE) &&
              (pcell_info_ptr->gsm.control_channel_description_valid) &&
              (!pcell_info_ptr->gsm.valid_data_flag)
             )
          {
            rr_send_priority_imsg_rr_event_ind(RR_EV_EARLY_CAMP_SI_RECEIVED, gas_id);
            rr_cell_acq_send_imsg_early_camping_bsi_received_ind(gas_id);
          }
        }
      } /* RR is not camped */

      /*******************************************************************************/
      /* SYS INFO MESSAGES RECEIVED FROM THE CANDIDATE CELL                          */
      /*  Receiving SI3 from a candidate cell will generate an event                 */
      /*******************************************************************************/

      /* if the cell is in the candidate cell database */
      if (rr_candidate_db_is_candidate_cell(channel_number, gas_id))
      {
        /* if the received message was SI3 */
        if ((message_type == SYSTEM_INFORMATION_TYPE_3)||(message_type == SYSTEM_INFORMATION_TYPE_4))
        {

#ifdef FEATURE_FEMTO_GSM_NL
          rr_decode_bcch_send_candidate_cell_si_ind(gas_id);
#else
          rr_send_priority_imsg_rr_event_ind(RR_EV_CANDIDATE_CELL_SI_RECEIVED, gas_id);

          // Ideally, a signal would be sent here to rr_decode_bcch_list_control to indicate reception
          // of candidate cell SI. But due to the nature of how candidate cell SI is currently
          // processed in rr_plmn_list.c module, this expects rr_decode_bcch_control() to have already
          // acted on this indication by the time it processes the RR_EV_CANDIDATE_CELL_SI_RECEIVED event,
          // which it will do in this thread.
          // So until rr_plmn_list_control is given an interface that allows this to be signalled, a direct
          // call into rr_decode_bcch_list_control must be made here
          // rr_decode_bcch_send_candidate_cell_si_ind();
          rr_decode_bcch_list_control(RR_EV_CANDIDATE_CELL_SI_RECEIVED, NULL, gas_id);
#endif // FEATURE_FEMTO_GSM_NL
        }
      }

      /*******************************************************************************/
      /* SYS INFO MESSAGES FROM A SURROUND NCELL                                     */
      /*  One of SI3, SI4, SI7 or SI8 been received from a surround cell             */
      /*******************************************************************************/

      surround_db_entry_ptr = rr_gprs_get_surround_ncell_gsm_sys_info(channel_number, gas_id);

      if (surround_db_entry_ptr != NULL)
      {
        /* check to see if all the required surrounding SI have been received */
        if (ARE_SET((MASK_SI_8 |
                     MASK_SI_7 |
                     MASK_SI_4 |
                     MASK_SI_3),
                     surround_db_entry_ptr->update_message_received
                   )
           )
        {
          /* all the necessary SI have been received, mark the data as valid */
          surround_db_entry_ptr->valid_data_flag = TRUE;

          /* if the surrounding cell is requesting SI event to be generated */
          if (surround_db_entry_ptr->si_expected)
          {
            rr_cell_acq_send_imsg_surrounding_si_received_ind(gas_id);
            /* reset the flag since the event has been generated */
            surround_db_entry_ptr->si_expected = FALSE;
          }

        }

        /* otherwise, still need to wait for more SI before declaring it valid */
      } /* SI is for surrounding cell */
    } /* SI is an idle SI message */


    /*******************************************************************************/
    /* DEDICATED SYS INFO MESSAGES                                                 */
    /*  One of SI5, SI5bis or SI6 has been received                                */
    /*******************************************************************************/

    if (dedicated_si_message ||
        (rr_is_camped(gas_id) &&
        (scell_info_ptr->gsm.BCCH_ARFCN.num == channel_number.num)
        && (scell_info_ptr->gsm.BCCH_ARFCN.band == channel_number.band)
       )
       )
    {
      /* only check for barred status if the data required for camping has been received */
      /* and its not a dedicated SI message                                              */
      if (scell_info_ptr->gsm.valid_data_flag && !dedicated_si_message)
      {
        /* compute the new cell priority for the serving cell */
        rr_cell_type_T cell_priority = rr_get_cell_priority(
          scell_info_ptr->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
          scell_info_ptr->gsm.RACH_control_parameters.CELL_BAR_ACCESS,
          &scell_info_ptr->gsm.location_area_identification,
          scell_info_ptr->gsm.RACH_control_parameters.access_control,
          &scell_info_ptr->gsm.cell_test_operation_mode,
          gas_id
        );

        /* if the serving cell is barred, force a cell reselection */
        if (cell_priority == RR_CELL_BARRED)
        {
          is_cell_barred = TRUE ;        
          rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_SERVING_BARRED, gas_id);

          rr_send_priority_imsg_rr_event_ind(EV_FORCE_CELL_RESELECTION, gas_id);
        }
      }

      /* if the message is a dedicated SI message */
      if (dedicated_si_message)
      {
        /* Cache SI5, SI5bis, SI5ter and SI6 so that they can be restored
        immediately in the case of handover failure.  They are not cached
        if a handover is in progress though, as it is unclear which cell
        (old cell or target) they are received from while HO is in progress. */
        if (!rr_inter_ho_handover_in_progress(gas_id))
        {
          rr_cache_dedicated_sys_info_message(
            message_type,
            scell_info_ptr->gsm.BCCH_ARFCN,
            message_ptr,
            gas_id
          );
        }

        /* check to see if all the dedicated SI messages required have been received */
        if (ARE_SET((MASK_SI_5      |
                     MASK_SI_5_BIS  |
                     MASK_SI_6),
                     scell_info_ptr->gsm.update_message_received
                   )
           )
        {
          boolean old_dedicated_valid_flag = scell_info_ptr->gsm.valid_dedicated_data_flag;
          MSG_GERAN_HIGH_0_G("All dedicated SIs received");
          /* now have valid dedicated information */
          scell_info_ptr->gsm.valid_dedicated_data_flag = TRUE;

          /* dedicated measurement reports are no longer invalid */
          rr_ded_set_measurement_reports_invalid(FALSE, gas_id);

          /* if the RR did not yet have all the valid dedicated SI information */
          if (!old_dedicated_valid_flag)
          {
            instances_T *mi_inst_ptr;

            mi_inst_ptr = rr_get_instances_ptr(MI_INSTANCE, gas_id);
            if (mi_inst_ptr != NULL)
            {
              /* generate the complete GSM BSIC Description from any received
              Measurement Information messages. */
              rr_gsm_update_gsm_bsic_description_from_mi_instances(mi_inst_ptr, gas_id);
            }

            /* notify L1 about any possible changes */
            rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
          }
        }
        /* not all dedicated SI messages received yet, keep processing them */
      }
    } /* SI is for the serving cell */


    /*******************************************************************************/
    /* POST-PROCESSING AND ANALYSIS FOR ALL SYS INFO MESSAGES RECEIVED             */
    /*******************************************************************************/

    if (is_cell_barred == FALSE)  /*Process if Cell is not barred because cell barred itself will trigger cell reselction.*/
	{
      if (!dedicated_si_message)
      {
        rr_gprs_bcch_sys_info_post_processing(
        si_message_received,
        channel_number,
        gas_id
        );
      }
	}
  }
  else /* the message was not decoded properly */
  {
    ERR("SI(%d) Protocol Error(%d)",(int) channel_number.num,(int) error_type,0);
  }
} // rr_store_system_info


static boolean rr_should_ignore_si2ter(
  const gas_id_t gas_id
)
{
  boolean ignore = FALSE;

  /* If only the PGSM or DCS1800 band is supported, SI2ter is ignored */
  /* (see 44.018 9.1.34) */
  if ((rr_gapi_get_supported_band_count(gas_id) == 1)    &&
      (rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id) ||
       rr_gapi_is_supported_band(SYS_BAND_DCS_1800, gas_id))
     )
  {
    ignore = TRUE;
  }

  return ignore;

} /* end rr_should_ignore_si2ter() */


static boolean rr_handle_si2ter_indicator(
  boolean two_ti,
  byte tc,
  gprs_scell_info_t *cell_info_ptr,
  const gas_id_t gas_id
)
{
  boolean             send_wcdma_cell_update = FALSE;
  gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  if (cell_info_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("NULL pointer passed to handle_si2ter()");
    return FALSE;
  }

  /* Check whether RR should process SI2ter or ignore it */
  if (rr_should_ignore_si2ter(gas_id))
  {
    MSG_GERAN_MED_0_G("SI2ter indicated but will be ignored");
    two_ti = FALSE;
  }

  /***************************************************************/
  /* if System information 2ter is not expected, then set bit in */
  /* mask as if it's been received                               */
  /***************************************************************/
  if (two_ti == FALSE)
  {
    
    /* Mark that SI2ter is not being broadcast */
    /* Note: This also mark SI2ter as being received and stops acquisition requests */
    bsi_update_messages_being_broadcast(
      cell_info_ptr,
      MASK_SI_NONE,
      MASK_SI_NONE,
      MASK_SI_2_TER
    );

    rr_initialize_SI2ter_instances(gas_id);

    cell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data = FALSE;
    if ( cell_info_ptr->gsm.two_ti == RR_SI2TER_EXPECTED)
    {
      if (rr_is_camped(gas_id))
      {
        /*SI2 ter is not broadcost in the cell so initialise si2 ter ba list to null*/
        /*if we dont initialise si2 ter ba list here, when cell brings up with the same si2 ter ba list*/
        /*RR is not updating the si2 ter ba list to GL1*/
        memset(&(scell_info_ptr->gsm.SI2ter_BA_list), 0, sizeof(BCCH_frequency_list_T));
      }
      MSG_GERAN_HIGH_0_G("SI2ter was expected, now deleted");

      if ( (&cell_info_ptr->gsm == &scell_info_ptr->gsm) &&
           rr_is_camped(gas_id) && !rr_gsm_only(gas_id))
      {
        send_wcdma_cell_update = TRUE;
      }
    }

    cell_info_ptr->gsm.two_ti = RR_SI2TER_NOT_EXPECTED;
    cell_info_ptr->gsm.decode_SI2ter = RR_SI_NOT_REQUIRED;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("SI2ter required");

    /* Mark that SI2ter is being broadcast */
    /* Note: This does not require SI2ter to be acquired */
    bsi_update_messages_being_broadcast(
      cell_info_ptr,
      MASK_SI_2_TER,
      MASK_SI_NONE,
      MASK_SI_NONE
    );

    if (cell_info_ptr->gsm.two_ti == RR_SI2TER_NOT_EXPECTED)
    {
      MSG_GERAN_HIGH_0_G("SI2ter was not expected, now added");

      /* Mark SI2ter & SI2terRO as required */
      bsi_set_messages_required_for_acquisition(
        MASK_SI_2_TER,
        MASK_SI_2_TER_REST,
        FALSE,
        gas_id
      );

      if ( (&cell_info_ptr->gsm == &scell_info_ptr->gsm) &&
           rr_is_camped(gas_id) &&
           cell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data &&
           !rr_gsm_only(gas_id))
      {
        send_wcdma_cell_update = TRUE;
      }
    }

    cell_info_ptr->gsm.decode_SI2ter = RR_SI_REQUIRED;
    cell_info_ptr->gsm.two_ti = RR_SI2TER_EXPECTED;

    if (&cell_info_ptr->gsm == &pcell_info_ptr->gsm)
    {
      if (ARE_SET(MASK_SI_3,pcell_info_ptr->gsm.update_message_received) ||
          ARE_SET(MASK_SI_2_TER,pcell_info_ptr->gsm.update_message_received))
      {
        /* no need to extend the acquisition timer on this occasion */
      }
      else if (tc < 8) /* TC range is 0..7 on BCCH (TC=8 used for PACCH sys info) */
      {
        /**
         * If the RR_GSM_SYS_INFO_TIMER is running, then extend it to give enough time for SI2ter. 
         *  
         * Note: The RR_GPRS_SYS_INFO_TIMER is not checked, as if this is running, then SI13 is being 
         * acquired, and there will always be enough time to receive SI2ter while this timer is running. 
         */
        if (rr_timer_is_running(RR_GSM_SYS_INFO_TIMER, gas_id))
        {
          uint32 time_remaining_ms;

          /* this was the first reception of SI3 on the pending channel
           * ensure that we wait long enough to acquire SI2ter
           */
          uint32 max_time_to_si2ter = THREE_FULL_TC_CYCLES;

          /* Wait for SI2ter, which occurs every 4th occurrence of TC=4 or 5
           * Each TC cycle takes 8*51 frames (one GSM frame is 4.615 milliseconds)
           * SI3 can arrive on TC=2 or TC=6: see 3GPP 45.002 Section 6.3.1.3
           * So we need to wait for at least 3 complete TC cycles, plus the gap
           * to the next reception of TC=5 (worst case)
           */
          if (tc == 2)
          {
            max_time_to_si2ter += GAP_TO_SI2TER_WHEN_SI3_ON_TC2;
          }
          else
          {
            /* this SI3 was probably on TC=6: just missed TC=5, longer wait */
            max_time_to_si2ter += GAP_TO_SI2TER_WHEN_SI3_ON_TC6;
          }

          /* Find the time remaining for RR_GSM_SYS_INFO_TIMER */
          time_remaining_ms = (uint32)rr_timer_get_time_remaining(RR_GSM_SYS_INFO_TIMER, gas_id);

          /* If the time remaining is less than the extension, then extend the timer */
          if (time_remaining_ms <= max_time_to_si2ter)
          {
            uint32 extension_ms = max_time_to_si2ter - time_remaining_ms;

            (void)rr_timer_extend(RR_GSM_SYS_INFO_TIMER, extension_ms, gas_id);

            MSG_GERAN_HIGH_1_G("SI acquisition time extended for SI2ter (+%dms)",(int)extension_ms);
          }
          else
          {
            MSG_GERAN_HIGH_0_G("Sufficient time remaining for SI2ter acquisition");
          }
        }
      }
    } /* end of if (si_db_ptr == pending_channel_data_ptr) */
  }/* end of if (two_ti == FALSE) */

  return send_wcdma_cell_update;
}

/* return values indicates whether to update L1 or not */
static boolean rr_handle_si2quater_indicator(
  boolean             si2q_pi,
  boolean             si2q_pos,
  gprs_scell_info_t * cell_info_ptr,
  const gas_id_t gas_id
)
{
  boolean             send_wcdma_cell_update = FALSE;
  gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (cell_info_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("NULL pointer passed to handle_si2quater()");
    return FALSE;
  }

  /***************************************************************/
  /* if System information 2quater is not expected, then set bit */
  /* mask as if it's been received                               */
  /***************************************************************/
  if ( si2q_pi == FALSE )
  {
    
    /* Mark SI2quater as not being broadcast */
    /* Note: This also marks SI2quater as received, and prevents further acquisitions */
    bsi_update_messages_being_broadcast(
      cell_info_ptr,
      MASK_SI_NONE,
      MASK_SI_NONE,
      MASK_SI_2_QUATER
    );

    cell_info_ptr->gsm.utran_neighbor_list.valid_data = FALSE;
    rr_initialize_SI2q_instances(gas_id);

    if ((cell_info_ptr->gsm.two_quater_position == SI2Q_POSITION_BCCH_Norm) ||
        (cell_info_ptr->gsm.two_quater_position == SI2Q_POSITION_BCCH_Ext))
    {
      MSG_GERAN_HIGH_0_G("SI2quater was expected, now deleted");

      /* update L1 to delete 3G ncells from Si2quater */
      if ( (&cell_info_ptr->gsm == &scell_info_ptr->gsm) &&
           rr_is_camped(gas_id) && !rr_gsm_only(gas_id))
      {
        send_wcdma_cell_update = TRUE;
      }
    }
    cell_info_ptr->gsm.two_quater_position = SI2Q_POSITION_NONE;
  }
  else
  {
    two_quater_position_T  two_quater_position;

    /* SI3 indicates SI2quater exist, set SI2q expected
     * to ask L1 to receive SI2quater later
     */
    MSG_GERAN_HIGH_1_G("SI2quater required (si2q_pos=%d)",(int)si2q_pos);

    /* store the si2q position */
    if (si2q_pos == FALSE)
    {
       two_quater_position = SI2Q_POSITION_BCCH_Norm;

       /* Mark that SI2quater is being broadcast on BCCH-Norm */
       /* Note: This adds no requirement for acquisition */
       bsi_update_messages_being_broadcast(
         cell_info_ptr,
         MASK_SI_2_QUATER,
         MASK_SI_NONE,
         MASK_SI_NONE
       );
    }
    else
    {
       two_quater_position = SI2Q_POSITION_BCCH_Ext;

       /* Mark that SI2quater is being broadcast on BCCH-Ext */
       /* Note: This adds no requirement for acquisition */
       bsi_update_messages_being_broadcast(
         cell_info_ptr,
         MASK_SI_NONE,
         MASK_SI_2_QUATER,
         MASK_SI_NONE
       );
    }

    if (cell_info_ptr->gsm.two_quater_position == SI2Q_POSITION_NONE)
    {
      MSG_GERAN_HIGH_0_G("SI2quater was not expected, now added");

      /* Mark SI2quater as required */
      bsi_set_messages_required_for_acquisition(
        MASK_SI_NONE,
        MASK_SI_2_QUATER,
        FALSE,
        gas_id
      );

      if ( (&cell_info_ptr->gsm == &scell_info_ptr->gsm) &&
           rr_is_camped(gas_id) &&
           cell_info_ptr->gsm.utran_neighbor_list.valid_data &&
           !rr_gsm_only(gas_id))
      {
        send_wcdma_cell_update = TRUE;
      }
    }

    cell_info_ptr->gsm.two_quater_position = two_quater_position;
  }

  return send_wcdma_cell_update;
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

boolean rr_ms_3g_sys_info_mandatory(const gas_id_t gas_id)
{
#ifndef PERLUTF
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (rr_multi_sim_mode_enabled())
  {
    /* check whether this access stratum is multimode-capable */
    return(geran_is_multi_mode_gas_id(gas_id));
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(TRUE);
#else /* !PERLUTF */
  #error code not present
#endif /* !PERLUTF */
}

#ifdef FEATURE_FAKE_BTS_CELL_INFO
 void rr_handle_qmi_cell_info_req(const gas_id_t gas_id)
 {
   geran_grr_qmi_cell_info_rsp_t qmi_cell_info_rsp;
   errno_enum_type               result;
   
   rr_populate_gcell_info(&qmi_cell_info_rsp, gas_id);
  
   msgr_init_hdr(&qmi_cell_info_rsp.hdr,
                MSGR_GERAN_GRR,
                GERAN_GRR_QMI_CELL_INFO_RSP);
   result = rr_msgr_send(&qmi_cell_info_rsp.hdr, sizeof(qmi_cell_info_rsp), gas_id);

  if (result != E_SUCCESS)
  {
     MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/


/* EOF */
