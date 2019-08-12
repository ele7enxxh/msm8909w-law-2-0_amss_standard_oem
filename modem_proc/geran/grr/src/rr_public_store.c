/*! \file rr_public_store.c

  This module contains the public store and functions to access the data.
  The public store is a shared data area, which stores data required by
  MAC/RLC/LLC which is not passed by message exchange.
  Only RR directly updates the data; All can read from it.

                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_public_store.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"  /* Includes FEATURE_... defines */
#include "comdef.h"
#include <string.h>
#include "rr_seg_load.h"
#include "rr_public_store.h"
#include "rr_sys_info_pacch.h"
#ifndef DEBUG_NO_RR
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "mac_grr.h"
#include "rr_decode_ie.h"
#include "rex.h" /* for critical section support */
#else
#error code not present
#endif
#include "rr_multi_sim.h"
#include "gprs_mem.h"
#include "rr_nv.h"
#include "rr_sim.h"
#include "stringl/stringl.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#ifndef DEBUG_NO_RR
// Define macros for mutex control
#define GRR_LOCK_PUBLIC_STORE() rex_enter_crit_sect(&grr_public_store_crit_sect[gas_id])
#define GRR_UNLOCK_PUBLIC_STORE() rex_leave_crit_sect(&grr_public_store_crit_sect[gas_id])
#else
#error code not present
#endif /* DEBUG_NO_RR */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  rrps_change_mark_t               change_mark_si13;
  rrps_change_mark_t               change_mark_psi2;
  rrps_ma_bitmap_t                 ma_valid_bitmap;
  rrps_gprs_cell_options_t         gprs_cell_options;
  rrps_global_power_ctrl_params_t  power_control_params;
  rrps_sim_data_t                  sim_data;
  rrps_gmm_data_t                  gmm_data;
  rrps_ms_data_t                   ms_data;
  uint8                            priority_access_thr;
  uint8                            nw_band_ind;
} rrps_store_data_t;

typedef struct
{
  rrps_store_data_t rrps_store_data;
} rr_public_store_data_t;

/* When DUAL_SIM is defined the public store will support two data spaces.
   RR public store is used for PS access so only one of stores will be
   required at a time.  But it is necessary to maintain both stores because
   SIM data is distinct between them. Any reads from the store are
   default to the PS gas_id.  Any writes to the store rely on the data space
   switching to the required gas_id in the current thread.  Switching PS from
   one SIM to another should be ok as the NV items will be copied in to both
   data spaces at startup and SIM data will be copied in to its respective data space.*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_public_store_data_t  rr_public_store_data[NUM_GERAN_DATA_SPACES];

#ifndef DEBUG_NO_RR
// Public Store access requires mutex control
rex_crit_sect_type grr_public_store_crit_sect[NUM_GERAN_DATA_SPACES];
#endif /* DEBUG_NO_RR */

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_public_store_data_t*
 */
static rr_public_store_data_t *rr_public_store_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_public_store_data[as_index];
}

/*!
 * \brief This function is used to access parameters in the Public Store.
 *
 * Note: This may call itself, so does not utitise a mutex lock (see rr_read_public_store()).
 *
 * \param param_id (in) - parameter identifier
 * \param param_copy (out) - updated with the value of the parameter
 * \param gas_id
 */
static void rr_read_public_store_internal(
  rr_public_store_id_t param_id,
  void *param_copy,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_public_store_data_t *rr_public_store_data_ptr = rr_public_store_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_public_store_data_ptr);

  if ( param_id < OVERFLOW_CHECK_FOR_RRPS_ID )
  {
    switch ( param_id )
    {
     /*
      * Start with the parameters grouped together in a structure.
      * Call this function recursively to load up individual elements
      */
      case RRPS_CELL_OPTIONS:
      {
        rrps_gprs_cell_options_t *cell_options = (rrps_gprs_cell_options_t *) param_copy;

        rr_read_public_store_internal(RRPS_CELL_OPTIONS_T3168, &cell_options->t3168, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_T3192, &cell_options->t3192, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_BS_CV_MAX, &cell_options->bs_cv_max, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_PAN_DEC, &cell_options->pan_dec, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_PAN_INC, &cell_options->pan_inc, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_PAN_MAX, &cell_options->pan_max, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE, &cell_options->access_burst_type, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE, &cell_options->control_ack_type, gas_id);

        rr_read_public_store_internal(RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &cell_options->egprs_capable_cell, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ, &cell_options->egprs_pkt_chan_req, gas_id);

        rr_read_public_store_internal(RRPS_CELL_OPTIONS_NW_EXT_UL_TBF, &cell_options->nw_ext_ul_tbf, gas_id);
        rr_read_public_store_internal(RRPS_CELL_OPTIONS_EXT_UTBF_NODATA, &cell_options->ext_utbf_nodata, gas_id);

        rr_read_public_store_internal(RRPS_CELL_OPTIONS_PFC_FEATURE_MODE, &cell_options->pfc_feature_mode, gas_id);
        break;
      }

      case RRPS_POWER_CTRL:
      {
        rrps_global_power_ctrl_params_t *pwr_control = (rrps_global_power_ctrl_params_t *) param_copy;

        rr_read_public_store_internal(RRPS_POWER_CTRL_ALPHA, &pwr_control->alpha, gas_id);
        rr_read_public_store_internal(RRPS_POWER_CTRL_T_AVG_W, &pwr_control->t_avg_w, gas_id);
        rr_read_public_store_internal(RRPS_POWER_CTRL_T_AVG_T, &pwr_control->t_avg_t, gas_id);
        rr_read_public_store_internal(RRPS_POWER_CTRL_PB, &pwr_control->pb, gas_id);
        rr_read_public_store_internal(RRPS_POWER_CTRL_PC_MEAS_CHAN, &pwr_control->pc_meas_chan, gas_id);
        rr_read_public_store_internal(RRPS_POWER_CTRL_INT_MEAS_CHAN_LIST_AV, &pwr_control->int_meas_channel_list_avail, gas_id);
        rr_read_public_store_internal(RRPS_POWER_CTRL_N_AVG_I, &pwr_control->n_avg_i, gas_id);

        break;
      }

      case RRPS_SIM_DATA:
      {
        rrps_sim_data_t *sim_data = (rrps_sim_data_t *) param_copy;

        rr_read_public_store_internal(RRPS_SIM_IMSI, &sim_data->imsi[0], gas_id);

        break;
      }

      case RRPS_GMM_DATA:
      {
        rrps_gmm_data_t *gmm_data = (rrps_gmm_data_t *) param_copy;

        rr_read_public_store_internal(RRPS_GMM_STATE, &gmm_data->gmm_state, gas_id);
        rr_read_public_store_internal(RRPS_READY_TIMER_FORCE_TO_STANDBY, &gmm_data->ready_timer_force_to_standby, gas_id);
        rr_read_public_store_internal(RRPS_PTMSI, &gmm_data->ptmsi, gas_id);
        rr_read_public_store_internal(RRPS_TMSI, &gmm_data->tmsi, gas_id);
        rr_read_public_store_internal(RRPS_TLLI, &gmm_data->tlli, gas_id);
        rr_read_public_store_internal(RRPS_TLLI_OLD, &gmm_data->tlli_old, gas_id);
        rr_read_public_store_internal(RRPS_MS_RA_CAP_IE, &gmm_data->ms_ra_cap_ie, gas_id);

        break;
      }

      case RRPS_MS_DATA:
      {
        rrps_ms_data_t *ms_data = (rrps_ms_data_t *) param_copy;

        rr_read_public_store_internal(RRPS_MS_NON_DRX_TIMER, &ms_data->non_drx_timer, gas_id);
        rr_read_public_store_internal(RRPS_MS_SPLIT_PAGE_CYCLE, &ms_data->split_page_cycle, gas_id);
        rr_read_public_store_internal(RRPS_MS_BCCH_BAND,&ms_data->ms_bcch_band, gas_id);
        rr_read_public_store_internal(RRPS_MS_EGPRS_MULTISLOT_CLASS,&ms_data->egprs_multislot_class, gas_id);
        rr_read_public_store_internal(RRPS_MS_EGPRS_8PSK_UL_ENABLED,&ms_data->egprs_8psk_ul_enabled, gas_id);
        rr_read_public_store_internal(RRPS_MS_EGPRS_8PSK_POWER_CAP,&ms_data->egprs_8psk_power_capability, gas_id);

#ifdef FEATURE_GSM_DTM
        rr_read_public_store_internal(RRPS_MS_DTM_ENABLED,&ms_data->dtm_enabled, gas_id);
        rr_read_public_store_internal(RRPS_MS_DTM_EGPRS_MULTISLOT_SUBCLASS,&ms_data->dtm_egprs_multislot_subclass, gas_id);
#ifdef FEATURE_GSM_EDTM
        rr_read_public_store_internal(RRPS_MS_EDTM_ENABLED,&ms_data->edtm_enabled, gas_id);
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */

        rr_read_public_store_internal(RRPS_MS_GPRS_GEA_SUPPORT,&ms_data->gea_supported, gas_id);
#ifdef FEATURE_GSM_GPRS_MSC33
        rr_read_public_store_internal(RRPS_MS_HMSC_GPRS_CODED_MS_CLASS,&ms_data->hmsc_gprs_coded_ms_class, gas_id);
        rr_read_public_store_internal(RRPS_MS_HMSC_EGPRS_CODED_MS_CLASS,&ms_data->hmsc_egprs_coded_ms_class, gas_id);
#endif /* FEATURE_GSM_GPRS_MSC33 */
        break;
      }

      /* Individual parameters */
      case RRPS_CHANGE_MARK_SI13:
      {
        *((rrps_change_mark_t *) param_copy) = rr_public_store_data_ptr->rrps_store_data.change_mark_si13;

        break;
      }

      case RRPS_VALID_MA_BIT_MAP:
      {
        *((rrps_ma_bitmap_t *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ma_valid_bitmap;

        break;
      }

      case RRPS_CELL_OPTIONS_T3168:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.t3168;

        break;
      }

      case RRPS_CELL_OPTIONS_T3192:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.t3192;

        break;
      }

      case RRPS_CELL_OPTIONS_BS_CV_MAX:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.bs_cv_max;

        break;
      }

      case RRPS_CELL_OPTIONS_PAN_DEC:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pan_dec;

        break;
      }

      case RRPS_CELL_OPTIONS_PAN_INC:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pan_inc;

        break;
      }

      case RRPS_CELL_OPTIONS_PAN_MAX:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pan_max;

        break;
      }

      case RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE:
      {
#ifndef DEBUG_NO_RR
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.access_burst_type;
#else
        #error code not present
#endif /* !DEBUG_NO_RR */

        break;
      }

      case RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE:
      {
#ifndef DEBUG_NO_RR
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.control_ack_type;
#else
        #error code not present
#endif /* !DEBUG_NO_RR */

        break;
      }

      case RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.egprs_capable_cell;

        break;
      }

      case RRPS_CELL_OPTIONS_EGPRS_PKT_CHAN_REQ:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.egprs_pkt_chan_req;

        break;
      }

      case RRPS_CELL_OPTIONS_NW_EXT_UL_TBF:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.nw_ext_ul_tbf;

        break;
      }

      case RRPS_CELL_OPTIONS_EXT_UTBF_NODATA:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.ext_utbf_nodata;

        break;
      }

      case RRPS_CELL_OPTIONS_PFC_FEATURE_MODE:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pfc_feature_mode;

        break;
      }

      case RRPS_POWER_CTRL_ALPHA:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.alpha;

        break;
      }

      case RRPS_POWER_CTRL_T_AVG_W:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.t_avg_w;

        break;
      }

      case RRPS_POWER_CTRL_T_AVG_T:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.t_avg_t;

        break;
      }

      case RRPS_POWER_CTRL_PB:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.pb;

        break;
      }

      case RRPS_POWER_CTRL_INT_MEAS_CHAN_LIST_AV:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.int_meas_channel_list_avail;

        break;
      }

      case RRPS_POWER_CTRL_PC_MEAS_CHAN:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.pc_meas_chan;

        break;
      }

      case RRPS_POWER_CTRL_N_AVG_I:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.power_control_params.n_avg_i;

        break;
      }

      case RRPS_SIM_IMSI:
      {
        memscpy( (uint8 *) param_copy,
			    sizeof(rr_public_store_data_ptr->rrps_store_data.sim_data.imsi), 
			    rr_public_store_data_ptr->rrps_store_data.sim_data.imsi, 
			    RRPS_SIM_IMSI_SIZE 
			    );

        break;
      }

      case RRPS_GMM_STATE:
      {
        *((gmm_state_t *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.gmm_state;

        break;
      }

      case RRPS_READY_TIMER_FORCE_TO_STANDBY:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.ready_timer_force_to_standby;

        break;
      }

      case RRPS_TMSI:
      {
        *((uint32 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.tmsi;

        break;
      }

      case RRPS_PTMSI:
      {
        *((uint32 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.ptmsi;

        break;
      }

      case RRPS_TLLI:
      {
        *((uint32 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.tlli;

        break;
      }

      case RRPS_TLLI_OLD:
      {
        *((uint32 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.tlli_old;

        break;
      }

      case RRPS_MS_RA_CAP_IE:
      {
        *((rrps_ms_ra_cap_ie_t *) param_copy) = rr_public_store_data_ptr->rrps_store_data.gmm_data.ms_ra_cap_ie;

        break;
      }

      case RRPS_MS_NON_DRX_TIMER:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.non_drx_timer;

        break;
      }

      case RRPS_MS_SPLIT_PAGE_CYCLE:
      {
        *((uint16 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.split_page_cycle;

        break;
      }

      case RRPS_MS_MULTISLOT_CLASS:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.multislot_class;

        break;
      }

      case RRPS_MS_ANITE_GCF_FLAG:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.anite_gcf_flag;

        break;
      }

      case RRPS_MS_BCCH_BAND:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.ms_bcch_band;

        break;
      }

      case RRPS_MS_EGPRS_MULTISLOT_CLASS:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.egprs_multislot_class;

        break;
      }

      case RRPS_MS_EGPRS_8PSK_UL_ENABLED:
      {
        *((boolean *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.egprs_8psk_ul_enabled;

        break;
      }

      case RRPS_MS_EGPRS_8PSK_POWER_CAP:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.egprs_8psk_power_capability;

        break;
      }

#ifdef FEATURE_GSM_DTM
      case RRPS_MS_DTM_ENABLED:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.dtm_enabled;

        break;
      }

      case RRPS_MS_DTM_EGPRS_MULTISLOT_SUBCLASS:
      {
        *((uint8 *) param_copy) =
          rr_public_store_data_ptr->rrps_store_data.ms_data.dtm_egprs_multislot_subclass;

        break;
      }

#ifdef FEATURE_GSM_EDTM
      case RRPS_MS_EDTM_ENABLED:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.edtm_enabled;

         break;
      }
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

      case RRPS_MS_GPRS_GEA_SUPPORT:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.gea_supported;

        break;
      }

#ifdef FEATURE_GSM_GPRS_MSC33
      case RRPS_MS_HMSC_GPRS_CODED_MS_CLASS:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.hmsc_gprs_coded_ms_class;

        break;
      }

      case RRPS_MS_HMSC_EGPRS_CODED_MS_CLASS:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.ms_data.hmsc_egprs_coded_ms_class;

        break;
      }
#endif /* FEATURE_GSM_GPRS_MSC33 */

      case RRPS_PRIORITY_ACCESS_THR:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.priority_access_thr;

        break;
      }

      case RRPS_NW_BAND_IND:
      {
        *((uint8 *) param_copy) = rr_public_store_data_ptr->rrps_store_data.nw_band_ind;

        break;
      }

      default:
      {
        MSG_GERAN_HIGH_1_G("Unhandled Public Store read (id=%d)",param_id);
        *((uint8 *)param_copy) = 0; /* To prevent KW error */
        break;
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Public Store id out of range (id=%d)",param_id);
    *((uint8 *)param_copy) = 0; /* To prevent KW error */
  }

  return;
} /* end rr_read_public_store_internal() */

/*!
 * \brief Outputs Public Store data content to diag.
 *
 * \param gas_id
 */
void rr_public_store_dump_to_diag(
  const gas_id_t gas_id
)
{
  rrps_ms_data_t *ms_data_ptr;

  // Obtain a pointer to the module data
  rr_public_store_data_t *data_ptr = rr_public_store_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(data_ptr);

  MSG_GERAN_HIGH_0_G("GERAN public store data:");

  ms_data_ptr = &data_ptr->rrps_store_data.ms_data;

  MSG_GERAN_HIGH_1_G("split_page_cycle=%d",(int)ms_data_ptr->split_page_cycle);
  MSG_GERAN_HIGH_1_G("anite_gcf_flag=%d",(int)ms_data_ptr->anite_gcf_flag);
  MSG_GERAN_HIGH_1_G("multislot_class=%d",(int)ms_data_ptr->multislot_class);
  MSG_GERAN_HIGH_3_G("egprs_multislot_class=%d egprs_8psk_ul_enabled=%d egprs_8psk_power_capability=%d",
                     (int)ms_data_ptr->egprs_multislot_class,
                     (int)ms_data_ptr->egprs_8psk_ul_enabled,
                     (int)ms_data_ptr->egprs_8psk_power_capability);
#ifdef FEATURE_GSM_DTM
  MSG_GERAN_HIGH_1_G("dtm_enabled=%d",(int)ms_data_ptr->dtm_enabled);
  MSG_GERAN_HIGH_1_G("dtm_egprs_multislot_subclass=%d",(int)ms_data_ptr->dtm_egprs_multislot_subclass);
#ifdef FEATURE_GSM_EDTM
  MSG_GERAN_HIGH_1_G("edtm_enabled=%d",(int)ms_data_ptr->edtm_enabled);
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  MSG_GERAN_HIGH_1_G("gea_supported=0x0%x",(int)ms_data_ptr->gea_supported);
#ifdef FEATURE_GSM_GPRS_MSC33
  MSG_GERAN_HIGH_1_G("hmsc_gprs_coded_ms_class=%d",(int)ms_data_ptr->hmsc_gprs_coded_ms_class);
  MSG_GERAN_HIGH_1_G("hmsc_egprs_coded_ms_class=%d",(int)ms_data_ptr->hmsc_egprs_coded_ms_class);
#endif /* FEATURE_GSM_GPRS_MSC33 */

} /* end rr_public_store_dump_to_diag() */

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief Initialises REX critical section that is used for all access to the RRPS.
 *
 * This function is called once as RR task start and should initialise the mutex locks for all subs.
 */
void rr_public_store_lock_init(
  void
)
{
#ifndef DEBUG_NO_RR
  uint8 i;

  for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
  {
    rex_init_crit_sect(&grr_public_store_crit_sect[i]);
  }
#endif /* DEBUG_NO_RR */

  return;
}

/*!
 * \brief This function is used to access parameters in the Public Store.
 *
 * \param param_id (in) - parameter identifier
 * \param param_copy (out) - updated with the value of the parameter
 * \param gas_id
 */
void rr_read_public_store(
  rr_public_store_id_t param_id,
  void *param_copy,
  const gas_id_t gas_id
)
{
#ifndef DEBUG_NO_RR
  if (rex_is_in_irq_mode())
  {
    /* if ISR context is in effect, assume unrestricted access */
    rr_read_public_store_internal(param_id, param_copy, gas_id);
  }
  else
#endif /* !DEBUG_NO_RR */
  {
    /* lock the public store so that the read cannot be interrupted */
    GRR_LOCK_PUBLIC_STORE();

    rr_read_public_store_internal(param_id, param_copy, gas_id);

    /* unlock the public store now that the read has completed */
    GRR_UNLOCK_PUBLIC_STORE();
  }

  return;
} /* end rr_read_public_store */

/*!
 * \brief This function is used to access the Mobile Allocation frequency lists.
 *
 * \param ma_number (in) - MA_NUMBER_14 or MA_NUMBER_15
 * \param gas_id
 *
 * \return rrps_frequency_list_t*
 */
rrps_frequency_list_t *rr_read_public_store_freq_list(
  uint8 ma_number,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t          *scell_info = NULL;
  gprs_mobile_allocations_t  *mobile_allocs = NULL;
  rrps_frequency_list_t      *ret_ptr = NULL;
  rrps_ma_bitmap_t            rrps_ma_bitmap;
  rr_public_store_data_t     *rr_public_store_data_ptr;
  uint8                       ma_offset;

  ma_offset = MAP_MA_NUMBER_TO_OFFSET(ma_number);

  if (ma_offset >= MAX_NUM_OF_GPRS_MA)
  {
    return(NULL);
  }

  // Obtain a pointer to the module data
  rr_public_store_data_ptr = rr_public_store_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_public_store_data_ptr);

  scell_info = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info);

  mobile_allocs = &scell_info->mobile_allocations;

  /* read the MA valid bitmap from the public store */
  GRR_LOCK_PUBLIC_STORE();
  rrps_ma_bitmap = rr_public_store_data_ptr->rrps_store_data.ma_valid_bitmap;
  GRR_UNLOCK_PUBLIC_STORE();

  if (IS_BIT_NUM_SET(rrps_ma_bitmap, ma_number))
  {
    ret_ptr = &mobile_allocs->gprs_mobile_allocation[ma_offset];

    if (ret_ptr->list_length == 0)
    {
#ifndef DEBUG_NO_RR
      MSG_GERAN_ERROR_1_G("MA (%d) list returned to MAC invalid",ma_number);
#else
      #error code not present
#endif /* !DEBUG_NO_RR */
    }
  }

  return(ret_ptr);
} /* end rr_read_public_store_freq_list() */

#ifndef DEBUG_NO_RR
/*!
 * \brief This function is used to update parameters in the Public Store.
 *
 * \param update_bitmap (in) - bitmap indicating which parameters are being updated
 * \param data (in) - pointer to the RRPS update structure
 * \param gas_id
 */
void rr_write_public_store(
  rrps_store_update_bitmap_t  update_bitmap,
  rrps_store_update_data_t   *data,
  const gas_id_t              gas_id
)
{
  // Obtain a pointer to the module data
  rr_public_store_data_t *rr_public_store_data_ptr = rr_public_store_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_public_store_data_ptr);

  /* lock the public store so that the write cannot be interrupted */
  GRR_LOCK_PUBLIC_STORE();

  if ( update_bitmap & RRPS_UPD_CHANGE_MARK_SI13 )
  {
    rr_public_store_data_ptr->rrps_store_data.change_mark_si13 = (rrps_change_mark_t) ( data->change_mark_si13 );
  }

  if ( update_bitmap & RRPS_UPD_VALID_MA_BIT_MAP )
  {
    rr_public_store_data_ptr->rrps_store_data.ma_valid_bitmap = (rrps_ma_bitmap_t) ( data->ma_valid_bitmap );
  }

  if ( update_bitmap & RRPS_UPD_CELL_OPTIONS )
  {
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.access_burst_type = data->gprs_cell_options.access_burst_type;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.bs_cv_max = data->gprs_cell_options.bs_cv_max;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.control_ack_type = data->gprs_cell_options.ctrl_ack_type;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pan_dec = data->gprs_cell_options.pan_dec;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pan_inc = data->gprs_cell_options.pan_inc;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pan_max = data->gprs_cell_options.pan_max;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.t3168 = data->gprs_cell_options.t3168;
    rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.t3192 = data->gprs_cell_options.t3192;

    if ( data->gprs_cell_options.optional_extension_info.egprs_supported_flag == valid )
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.egprs_capable_cell =
        rr_get_nv_egprs_enabled(RR_GAS_ID_TO_AS_ID);
    }
    else
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.egprs_capable_cell = FALSE;
    }

    /* 3GPP 44.060 table 12.24.2 EGPRS_PACKET_CHANNEL_REQUEST 0-on, 1-off */
    if ( data->gprs_cell_options.optional_extension_info.egprs_packet_channel_request == 0 )
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.egprs_pkt_chan_req = TRUE;
    }
    else
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.egprs_pkt_chan_req = FALSE;
    }

    if ( (data->gprs_cell_options.optional_extension_info.nw_ext_tbf) &&
         (rr_get_nv_geran_feature_pack_1_enabled(RR_GAS_ID_TO_AS_ID)) )
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.nw_ext_ul_tbf = TRUE;
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.ext_utbf_nodata =
        data->gprs_cell_options.optional_extension_info.ext_utbf_no_data;
    }
    else
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.nw_ext_ul_tbf = FALSE;
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.ext_utbf_nodata = FALSE;
    }

    if (data->gprs_cell_options.optional_extension_info_flag)
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pfc_feature_mode = data->gprs_cell_options.optional_extension_info.pfc_feature_mode;
    }
    else
    {
      rr_public_store_data_ptr->rrps_store_data.gprs_cell_options.pfc_feature_mode = FALSE;
    }
  }

  if ( update_bitmap & RRPS_UPD_POWER_CTRL )
  {
    rr_public_store_data_ptr->rrps_store_data.power_control_params.alpha = data->power_control_params.alpha;
    rr_public_store_data_ptr->rrps_store_data.power_control_params.int_meas_channel_list_avail = data->power_control_params.int_meas_chan_list_avail;
    rr_public_store_data_ptr->rrps_store_data.power_control_params.n_avg_i = data->power_control_params.n_avg_i;
    rr_public_store_data_ptr->rrps_store_data.power_control_params.pb = data->power_control_params.pb;
    rr_public_store_data_ptr->rrps_store_data.power_control_params.pc_meas_chan = data->power_control_params.pc_meas_chan;
    rr_public_store_data_ptr->rrps_store_data.power_control_params.t_avg_t = data->power_control_params.t_avg_t;
    rr_public_store_data_ptr->rrps_store_data.power_control_params.t_avg_w = data->power_control_params.t_avg_w;
  }

  /* The IMSI value passed is taken from the SIM, and is an array of 9 octets, the first one being */
  /* the length. See GSM 11.11 10.3.2. MAC does not require this first octet, so it is not stored */
  if ( update_bitmap & RRPS_UPD_SIM_IMSI )
  {
   memscpy( rr_public_store_data_ptr->rrps_store_data.sim_data.imsi,sizeof(rr_public_store_data_ptr->rrps_store_data.sim_data.imsi), &data->imsi[1], RRPS_SIM_IMSI_SIZE );
  }

  if ( update_bitmap & RRPS_UPD_SIM_TMSI )
  {
    uint32 tmsi;

    /* The TMSI is a 32-bit value, communicated to RR as four octets, format see 04.18 10.5.2.42 */
    /* Octet 1   31 30 29 28 27 26 25 24
       Octet 2   23 22 21 20 19 18 17 16
       Octet 3   15 14 13 12 11 10  9  8
       Octet 4    7  6  5  4  3  2  1  0  */
    tmsi = data->tmsi[0];
    tmsi = (tmsi << 8) | data->tmsi[1];
    tmsi = (tmsi << 8) | data->tmsi[2];
    tmsi = (tmsi << 8) | data->tmsi[3];

    rr_public_store_data_ptr->rrps_store_data.gmm_data.tmsi = tmsi;
  }

  if ( update_bitmap & RRPS_UPD_SIM_PTMSI )
  {
    uint32 ptmsi;

    /* The P-TMSI is a 32-bit value, communicated to RR as four octets, format see 04.18 10.5.2.42 */
    /* Octet 1   31 30 29 28 27 26 25 24
       Octet 2   23 22 21 20 19 18 17 16
       Octet 3   15 14 13 12 11 10  9  8
       Octet 4    7  6  5  4  3  2  1  0  */
    ptmsi = data->ptmsi[0];
    ptmsi = (ptmsi << 8) | data->ptmsi[1];
    ptmsi = (ptmsi << 8) | data->ptmsi[2];
    ptmsi = (ptmsi << 8) | data->ptmsi[3];

    rr_public_store_data_ptr->rrps_store_data.gmm_data.ptmsi = ptmsi;
  }

  if ( update_bitmap & RRPS_UPD_GMM_TLLI )
  {
    rr_public_store_data_ptr->rrps_store_data.gmm_data.tlli = data->new_tlli;
    rr_public_store_data_ptr->rrps_store_data.gmm_data.tlli_old = data->old_tlli;
  }

  if ( update_bitmap & RRPS_UPD_GMM_READY_TIMER )
  {
    rr_public_store_data_ptr->rrps_store_data.gmm_data.ready_timer_force_to_standby = data->negotiated_ready_timer_zero;
  }

  if ( update_bitmap & RRPS_UPD_GMM_STATUS )
  {
    if ( data->gmm_ready_state == GMM_IDLE_STATE )
    {
      rr_public_store_data_ptr->rrps_store_data.gmm_data.gmm_state = GMM_IDLE;
    }
    else
    if ( data->gmm_ready_state == GMM_STANDBY_STATE )
    {
      rr_public_store_data_ptr->rrps_store_data.gmm_data.gmm_state = GMM_STANDBY;
    }
    else
    if ( data->gmm_ready_state == GMM_READY_STATE )
    {
      rr_public_store_data_ptr->rrps_store_data.gmm_data.gmm_state = GMM_READY;
    }
  }

  if ( update_bitmap & RRPS_UPD_GMM_MS_RA_CAP_IE )
  {
    uint16  len_bits;
    uint8   len_octets;

    #define MAX_MS_RA_CAP_IE_LEN_BITS (8 * MAX_MS_RA_CAP_IE_LEN)

    /* Check that the number of bits is not greater than 51 octets */
    if (data->ms_ra_cap_ie.length_bits <= MAX_MS_RA_CAP_IE_LEN_BITS)
    {
      len_bits = data->ms_ra_cap_ie.length_bits;
    }
    else
    {
      len_bits = MAX_MS_RA_CAP_IE_LEN_BITS;
    }

    len_octets = (uint8)(len_bits / 8);

    /* If number of bits not on octet boundary, add an octet */
    if ( len_bits & 7 ) len_octets++;

    /* Copy over number of bits */
    rr_public_store_data_ptr->rrps_store_data.gmm_data.ms_ra_cap_ie.ms_ra_cap_ie_len_bits = len_bits;

    /* Copy number of octets required from GMM data area */
	if (len_octets <= MAX_MS_RA_CAP_IE_LEN) //copy till the max length 
	{
    memscpy( rr_public_store_data_ptr->rrps_store_data.gmm_data.ms_ra_cap_ie.ms_ra_cap_ie,
		     sizeof(rr_public_store_data_ptr->rrps_store_data.gmm_data.ms_ra_cap_ie.ms_ra_cap_ie),
             data->ms_ra_cap_ie.ms_ra_cap_ie,
             len_octets );	
	}

   if((MAX_MS_RA_CAP_IE_LEN-len_octets)>0 ) //Then only set the remaining to 0.
    {
    /* Zero out remainder of MS_RA_CAP_IE */
    memset( rr_public_store_data_ptr->rrps_store_data.gmm_data.ms_ra_cap_ie.ms_ra_cap_ie + len_octets,
            0,
            MAX_MS_RA_CAP_IE_LEN - len_octets );
    }
  }

  if ( update_bitmap & RRPS_UPD_MS_NON_DRX_TIMER )
  {
    rr_public_store_data_ptr->rrps_store_data.ms_data.non_drx_timer = data->non_drx_timer;
  }

  if ( update_bitmap & RRPS_UPD_MS_BCCH_BAND )
  {
    rr_public_store_data_ptr->rrps_store_data.ms_data.ms_bcch_band = data->ms_bcch_band;
  }

  if ( update_bitmap & RRPS_UPD_PRIORITY_ACCESS_THR )
  {
    rr_public_store_data_ptr->rrps_store_data.priority_access_thr = data->priority_access_thr;
  }

  if ( update_bitmap & RRPS_UPD_NW_BAND_IND )
  {
    rr_public_store_data_ptr->rrps_store_data.nw_band_ind = data->nw_band_ind;
  }

  GRR_UNLOCK_PUBLIC_STORE();

  return;
} /* end rr_write_public_store() */

/*!
 * \brief Called when a MAC_GRR_STORE_UPDATE message is received from MAC.
 *
 * Global System Information and the Public Store are updated with the new parameters.
 *
 * \param sig (in) - pointer to MAC_GRR_STORE_UPDATE message
 * \param gas_id
 */
void rr_process_mac_grr_store_update(
  mac_grr_store_update_t *sig,
  const gas_id_t gas_id
)
{
  global_power_control_params_t  power_control_params;

  switch ( sig->id )
  {
    case RRPS_POWER_CTRL:
    {
      power_control_params.alpha = sig->update.global_power_control_params.alpha;
      power_control_params.int_meas_chan_list_avail = sig->update.global_power_control_params.int_meas_channel_list_avail;
      power_control_params.n_avg_i = sig->update.global_power_control_params.n_avg_i;
      power_control_params.pb = sig->update.global_power_control_params.pb;
      power_control_params.pc_meas_chan = sig->update.global_power_control_params.pc_meas_chan;
      power_control_params.t_avg_t = sig->update.global_power_control_params.t_avg_t;
      power_control_params.t_avg_w = sig->update.global_power_control_params.t_avg_w;

      rr_gprs_updated_sys_info_params_from_mac(
        &power_control_params,    // mac_pwoer_control_params
        gas_id                    // gas_id
      );

      break;
    }

    default: break;
  }

} /* end rr_process_mac_grr_store_update() */

/*!
 * \brief This function decodes a Mobile Allocation freq list from a packet assignment received by MAC.
 *
 * This freq list is stored and can be accessed using MA_NUMBER=15.
 * An optional comparison can be performed against the last freq list stored by this method.
 * If a comparison is not required, 'compare_ptr' can be set to NULL, or the flag it points to FALSE.
 * If a comparison is required, 'compare_ptr' should pointer to a flag set TRUE. In this case, the result
 * of the comparison is written to this flag: TRUE if the freq list is the same; FALSE if they are different.
 *
 * \param type (in) - format of the supplied data. Should be 'direct1_valid' or 'direct2_valid'
 * \param data (in) - data structure of type either 'direct1_enc_t' or 'direct2_enc_t'
 * \param compare_ptr (in) - comparison required flag / comparison result
 * \param gas_id
 *
 * \return rrps_frequency_list_t* - decoded freq list (NULL if an error occurred during decoding)
 */
rrps_frequency_list_t *rr_packet_assignment_mobile_alloc(
  encoding_type_e type,
  void *data,
  boolean *compare_ptr,
  const gas_id_t gas_id
)
{
  gprs_mobile_allocation_t *decoded_mobile_alloc_ptr;
  rrps_frequency_list_t    *freq_list_p = NULL;
  boolean                   decode_ok = TRUE;
  gprs_scell_info_t        *scell_info = NULL;
  cell_channel_description_T *cell_allocation_ptr;

  scell_info = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info);

  // The Cell Allocation list is used to decode the Mobile Allocation

  // The Cell Allocation list is required to have ARFCN 0 at the end.
  // This is done by taking a copy of the Cell Allocation list from system information and moving
  // ARFCN 0 (if present).

  // First allocate a new Cell Allocation list from heap
  cell_allocation_ptr = GPRS_MEM_CALLOC(sizeof(cell_channel_description_T));
  RR_NULL_CHECK_FATAL(cell_allocation_ptr);

  // Truncate the Cell Allocation length to the maximum number of entries for a Mobile Allocation
  cell_allocation_ptr->no_of_entries =
    MIN(scell_info->gsm.cell_channel_description.no_of_entries,CA_MAX_LEN);

  // Now copy the Cell Allocation list and move ARFCN 0 (if present)
  rr_move_zero_in_frequency_list(
    scell_info->gsm.cell_channel_description.channel_number,   // original_list_ptr
    cell_allocation_ptr->no_of_entries,                        // length
    cell_allocation_ptr->channel_number                        // modified_list_ptr
  );

  // Now the Mobile Allocation can be decoded

  decoded_mobile_alloc_ptr = GPRS_MEM_MALLOC(sizeof(gprs_mobile_allocation_t));
  RR_NULL_CHECK_FATAL(decoded_mobile_alloc_ptr);

  /* From either encoding method, extract the following: */
  /* 1. A frequency list (ARFCNs + length) */
  /* 2. A HSN */
  /* Then store this in the Public Store as MA_NUMBER=15 */

  switch ( type )
  {
    case direct1_valid:
    {
      direct_enc1_t  * enc1 = (direct_enc1_t *) data;
      gprs_mobile_allocation_ie_t temp_ma_store;

      rr_gprs_decode_mobile_alloc(
        &enc1->gprs_mobile_alloc,
        &temp_ma_store,
        cell_allocation_ptr,
        decoded_mobile_alloc_ptr
      );

      decoded_mobile_alloc_ptr->hsn = enc1->gprs_mobile_alloc.hsn;
      break;
    }

    case direct2_valid:
    {
      #define CSN_MA_FREQ_LIST_DATA_LEN_MIN_VAL_OFFSET 3

      direct_enc2_t  * enc2 = (direct_enc2_t *) data;
      uint8 len_ma_freq_list = (uint8)(enc2->len_ma_freq_list + CSN_MA_FREQ_LIST_DATA_LEN_MIN_VAL_OFFSET);
      
      /* whichever is the minimum used as ma_freq_list for KW*/
      len_ma_freq_list = MIN(len_ma_freq_list,sizeof(enc2->ma_freq_list));

      rr_gprs_decode_mobile_alloc_freq_list(
        enc2->ma_freq_list,
        len_ma_freq_list,
        decoded_mobile_alloc_ptr,
        gas_id
      );

      decoded_mobile_alloc_ptr->hsn = enc2->hsn;
      break;
    }

    default:

      decode_ok = FALSE;
      break;
  }

  if ( decode_ok )
  {
    /* Now store this mobile allocation in the mobile allocation list at MA_NUMBER=15 */
    rr_gprs_store_mobile_alloc(
      MA_NUMBER_15,               // ma_number
      decoded_mobile_alloc_ptr,   // new_mobile_alloc
      scell_info,                 // cell_info
      gas_id                      // gas_id
    );

    freq_list_p = rr_read_public_store_freq_list(
      MA_NUMBER_15,               // ma_number
      gas_id                      // gas_id
    );
  }

  // Free all allocated data
  GPRS_MEM_FREE(decoded_mobile_alloc_ptr);
  GPRS_MEM_FREE(cell_allocation_ptr);

  (* compare_ptr) = FALSE;

  return freq_list_p;

} /* end rr_packet_assignment_mobile_alloc() */

#else
#error code not present
#endif // DEBUG_NO_RR

#if defined(TEST_FRAMEWORK) || defined (PERLUTF)
#error code not present
#endif /* TEST_FRAMEWORK || PERLUTF*/

/*!
 * \brief Updates the multislot class parameters.
 *
 * \param egprs_mulislot_class (in)
 * \param dtm_egprs_multislot_subclass (in)
 * \param gprs_multislot_class (in)
 * \param gas_id
 */
void rr_public_store_update_multislot_class(
  uint8 egprs_mulislot_class,
#ifdef FEATURE_GSM_DTM
  uint8 dtm_egprs_multislot_subclass,
#endif
  uint8 gprs_multislot_class,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_public_store_data_t *data_ptr = rr_public_store_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(data_ptr);

  GRR_LOCK_PUBLIC_STORE();

  {
    rrps_ms_data_t *rrps_ms_data_ptr = &(data_ptr->rrps_store_data.ms_data);

    rrps_ms_data_ptr->multislot_class = gprs_multislot_class;
    rrps_ms_data_ptr->egprs_multislot_class = egprs_mulislot_class;
#ifdef FEATURE_GSM_DTM
    rrps_ms_data_ptr->dtm_egprs_multislot_subclass = dtm_egprs_multislot_subclass;
#endif
  }

  GRR_UNLOCK_PUBLIC_STORE();

} /* rr_public_store_update_multislot_class() */

/*!
 * \brief Updates the NV data in the Public Store.
 *
 * \param gas_id
 */
void rr_public_store_update_nv_data(
  const gas_id_t gas_id
)
{
  rrps_ms_data_t *ms_data_ptr;

  // All NV data is accessed by AS-ID, not GAS-ID
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  // Obtain a pointer to the module data
  rr_public_store_data_t *data_ptr = rr_public_store_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(data_ptr);

  MSG_GERAN_HIGH_0_G("Updating NV data in public store");

  ms_data_ptr = &data_ptr->rrps_store_data.ms_data;

  GRR_LOCK_PUBLIC_STORE();

  ms_data_ptr->split_page_cycle = rr_get_nv_split_page_cycle(as_id);
  ms_data_ptr->multislot_class = rr_get_nv_multislot_class(as_id);
  ms_data_ptr->anite_gcf_flag = rr_get_nv_anite_gcf_flag(as_id);
  ms_data_ptr->egprs_multislot_class = rr_get_nv_egprs_multislot_class(as_id);
  ms_data_ptr->egprs_8psk_ul_enabled = rr_get_nv_egprs_8psk_ul_enabled(as_id);
  ms_data_ptr->egprs_8psk_power_capability = rr_get_nv_egprs_8psk_power_capability(as_id);
#ifdef FEATURE_GSM_DTM
  ms_data_ptr->dtm_enabled = rr_get_nv_dtm_enabled(as_id);
  ms_data_ptr->dtm_egprs_multislot_subclass = rr_get_nv_dtm_egprs_multislot_subclass(as_id);
#ifdef FEATURE_GSM_EDTM
  ms_data_ptr->edtm_enabled = rr_get_nv_edtm_enabled(as_id);
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  ms_data_ptr->gea_supported = rr_get_nv_gea_supported(as_id);
#ifdef FEATURE_GSM_GPRS_MSC33
  ms_data_ptr->hmsc_gprs_coded_ms_class = rr_get_nv_hmsc_gprs_coded_ms_class(as_id);
  ms_data_ptr->hmsc_egprs_coded_ms_class = rr_get_nv_hmsc_egprs_coded_ms_class(as_id);
#endif /* FEATURE_GSM_GPRS_MSC33 */

  GRR_UNLOCK_PUBLIC_STORE();

  rr_public_store_dump_to_diag(gas_id);

} /* end rr_public_store_update_nv_data() */

/*!
 * \brief Module initialisation function called at task start.
 */
void rr_public_store_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_public_store_data, 0, sizeof(rr_public_store_data));

  // Setup any specific non-zero values
  {
    rr_public_store_data_t *data_ptr;
    uint8 i;

    for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
    {
      data_ptr = &rr_public_store_data[i];

      data_ptr->rrps_store_data.change_mark_si13 = RRPS_CHANGE_MARK_NOT_VALID;
      data_ptr->rrps_store_data.change_mark_psi2 = RRPS_CHANGE_MARK_NOT_VALID;

      data_ptr->rrps_store_data.gmm_data.gmm_state = GMM_IDLE;
      data_ptr->rrps_store_data.gmm_data.ptmsi = TMSI_PTMSI_INVALID;
      data_ptr->rrps_store_data.gmm_data.tmsi = TMSI_PTMSI_INVALID;
      data_ptr->rrps_store_data.gmm_data.tlli = TMSI_PTMSI_INVALID;
      data_ptr->rrps_store_data.gmm_data.tlli_old = TMSI_PTMSI_INVALID;
    }
  }

} /* end rr_public_store_task_start_init() */

/* EOF */