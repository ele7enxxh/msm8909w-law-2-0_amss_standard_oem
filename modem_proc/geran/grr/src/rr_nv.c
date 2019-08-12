/*! \file rr_nv.c 
 
  This module contains access functions for NV data.
 
                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_nv.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "rr_seg_load.h"
#include "geran_variation.h"
#include "rr_nv.h"
#include "rr_nv_g.h"
#include "rr_channel_mode.h"
#include "rr_gprs_defs.h"
#include "fs_public.h"
#include "rr_msc.h"
#include "geran_multi_sim.h"
#include "sys.h"
#include "geran_nv.h"
#include "gprs_mem.h" 
#include "rr_sim.h"
#ifdef FEATURE_GSM_TDS
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "rr_if.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define RR_SCELL_RESYNC_TIME_DEFAULT (255) /* 255 seconds */
#define RR_PSCAN_STALENESS_DEFAULT   (4)   /*   4 seconds */

#ifdef FEATURE_IDLE_DRX_SCALING
/* Default timeout of 10 seconds will be used to suspend DRX scaling after (re)selection */
#define RR_IDLE_DRX_SCALING_SUSPEND_TIMEOUT_DEFAULT 10
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
/*----------------------------------------------------------------------------
 * Default GERAN, LTE and TDS Parameters
 * -------------------------------------------------------------------------*/
 #define RR_DEFAULT_GERAN_PRIO 1
 #define RR_GERAN_DEF_H_PRIO   0  /* This implies 06 DB */
 #define RR_GERAN_DEF_THRESH_GSM_LOW 15 /* This implies GRR will always measure low Priority IRAT neighbors */
 #define RR_GERAN_DEF_THRESH_PRIO_SRCH 15
 #define RR_GERAN_DEF_T_RESEL 0  /* Zero implies 05 seconds */
 
 #define RR_LTE_DEF_PRIO      7
 #define RR_LTE_DEF_MEAS_BW   0
 #define RR_LTE_DEF_QRXLEV_MIN 0
 #define RR_LTE_DEF_THRESH_HIGH  3
 #define RR_LTE_DEF_THRESH_LOW  0

 #define RR_UTRAN_DEF_PRIO   3
 #define RR_UTRAN_DEF_QRXLEV_MIN 0
 #define RR_UTRAN_DEF_THRESH_HIGH 0
 #define RR_UTRAN_DEF_THRESH_LOW  0
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  uint8    split_page_cycle_enc;
  uint16   split_page_cycle;
  uint8    non_drx_timer_enc;
  uint8    non_drx_timer;
  uint8    multislot_class;
  uint8    anite_gcf_flag;
  boolean  egprs_enabled;
  uint8    egprs_multislot_class;
  boolean  egprs_8psk_ul_enabled;
  uint8    egprs_8psk_power_capability;
  boolean  geran_feature_pack_1_enabled;
  uint8    interrat_nacc_support;
#ifdef FEATURE_GSM_DTM
  boolean  dtm_enabled;
  uint8    dtm_egprs_multislot_subclass;
#ifdef FEATURE_GSM_EDTM
  boolean  edtm_enabled;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  uint8    gea_supported;
#ifdef FEATURE_GSM_GPRS_MSC33
  uint8    hmsc_gprs_coded_ms_class;
  uint8    hmsc_egprs_coded_ms_class;
#endif /* FEATURE_GSM_GPRS_MSC33 */
} rr_nv_data_t;

// Maximum number of PLMNs in EFS white list
#define MAX_NUM_OF_PLMNS 20

// Number of bytes storage per PLMN id
#define BYTES_PER_PLMN 3

typedef struct
{
  uint8 num_of_plmns;
  sys_plmn_id_s_type *plmn_list_ptr;
} rr_plmn_white_list_t;

typedef struct
{
  boolean x2g_fast_si_acq_white_list_present;
  rr_plmn_white_list_t x2g_fast_si_acq_white_list;
} rr_efs_x2g_fast_si_acq_info_t;

#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

typedef PACK(struct)
{
  boolean g2w_enabled;
  boolean g2t_enabled;
  boolean g2l_enabled;
} rr_efs_g2x_measurement_control_t;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

typedef struct
{
#ifdef FEATURE_VAMOS
  boolean vamos_enabled;
#endif /* FEATURE_VAMOS */
  boolean fast_si_acq_during_csfb_enabled;
  boolean fast_si_acq_when_cs_only_enabled;
  rr_plmn_white_list_t negative_c1_white_list;
  uint8 pscan_results_reuse_time_secs;
  boolean plmn_search_freq_pruning_enabled;
  rr_efs_x2g_fast_si_acq_info_t x2g_fast_si_acq_info;
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
  boolean sglte_g2x_conn_mode_resel_enabled;
#endif
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  boolean fast_g2l_using_pseudo_si2q_enabled;
  rr_efs_pseudo_si2q_def_data_t rr_efs_pseudo_si2q_def_data;
#endif
  int8    add_crh_db;
  int8    add_crh_scell_thresh_db;
  boolean sys_info_cache_enabled;
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  boolean band_avoid_enabled;
  uint8 band_avoid_ncell_threshold;
  uint8 band_avoid_max_meas_rpts;
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
  rr_plmn_white_list_t read_pch_during_transfer_whitelist;
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
  boolean iterative_si_acq_enabled;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  uint8   scell_resync_time_secs;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  boolean eng_mode_debug_enabled;
#ifdef FEATURE_LTE
  /* Indicates whether G2L blind redirection is enabled.  If enabled, after
  a CS call (for voice, supplementary services or periodic LAU) is released
  RR will initiate a blind redirection to LTE. */
  boolean g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled;

  /* Indicates whether G2L blind redirection after CSFB is enabled.  If
  enabled, after a CS call resulting from a CSFB procedure is released, RR
  will initiate a blind redirection to LTE.  This is based on RR_EST_REQ
  indicating whether the call is being set up as a result of CSFB. */
  boolean g2l_blind_redir_after_csfb_enabled;
  boolean g2l_blind_redir_after_srvcc_enabled;
#endif /* FEATURE_LTE */
  rr_efs_g2x_measurement_control_t g2x_measurement_control;
#ifdef FEATURE_LTE
  boolean depriotize_gsm_cell;
#endif /*FEATURE_LTE*/
#ifdef FEATURE_IDLE_DRX_SCALING
  uint8   idle_drx_scaling_suspend_time_secs;
#endif /* FEATURE_IDLE_DRX_SCALING */
#ifdef FEATURE_GERAN_HST_OPT
  rr_plmn_white_list_t hst_opt_white_list; /*High Speed Train Optimization*/
#endif /*FEATURE_GERAN_HST_OPT*/
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  uint16 multisim_si_refresh_duration_secs;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  boolean fake_bs_detection_enabled;
} rr_efs_nv_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// NV data (Note: per sub)
static rr_nv_data_t rr_nv_data[NUM_GERAN_DATA_SPACES];

// EFS-NV data (Note: per sub)
static rr_efs_nv_data_t rr_efs_nv_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the data-space index appropriate for the given AS-ID.
 *
 *  Note: For the NV data, this index is stored according to AS-ID, not GAS-ID. This is to allow the
 *  AS-ID to GAS-ID mapping to be changed without the NV data needing to be re-sent
 *
 * \param as_id
 *
 * \return uint8
 */
static uint8 rr_nv_get_data_space_index(const sys_modem_as_id_e_type as_id)
{
#ifdef FEATURE_TRIPLE_SIM
  if (as_id == SYS_MODEM_AS_ID_3)
  {
    return GERAN_DATA_SPACE_INDEX_3;
  }
#endif // FEATURE_TRIPLE_SIM

#ifdef FEATURE_DUAL_SIM
  if (as_id == SYS_MODEM_AS_ID_2)
  {
    return GERAN_DATA_SPACE_INDEX_2;
  }
#endif // FEATURE_DUAL_SIM

  return GERAN_DATA_SPACE_INDEX_1;
}

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param as_id
 *
 * \return rr_nv_data_t*
 */
static rr_nv_data_t *rr_nv_get_data_ptr(const sys_modem_as_id_e_type as_id)
{
  uint8 as_index = rr_nv_get_data_space_index(as_id);

  return &rr_nv_data[as_index];
}

/*!
 * \brief Returns a pointer to the EFS data.
 *
 * \param as_id
 *
 * \return rr_nv_data_t*
 */
static rr_efs_nv_data_t *rr_nv_get_efs_data_ptr(const sys_modem_as_id_e_type as_id)
{
  uint8 as_index = rr_nv_get_data_space_index(as_id);

  return &rr_efs_nv_data[as_index];
}

/*!
 * \brief Takes the encoded split page cycle code and returns the decoded value
 * 
 * \param split_page_cycle_enc (in)
 * 
 * \return uint16 
 */
static uint16 decode_split_page_cycle(uint8 split_page_cycle_enc)
{
/* Translation of SPLIT PAGE CYCLE (encoded) to SPLIT PAGE CYCLE value for values 65 -> 98 */
  static const uint16 spc_code_to_val[] =
  {
    71, 72, 74, 75, 77, 79, 80, 83, 86, 88, /* indices 0..9   */
    90, 92, 96,101,103,107,112,116,118,128, /* indices 10..19 */
   141,144,150,160,171,176,192,214,224,235, /* indices 20..29 */
   256, 288, 320, 352                       /* indices 30..33 */
  };

  uint16 split_page_cycle;

/*
 SPLIT PAGE CYCLE values - see GSM 04.08 10.5.5.6

 SPLIT PAGE CYCLE CODE   SPLIT PAGE CYCLE value
 0                       704
 1 -> 64                 1 -> 64
 65 -> 98                as constant array above
 all others              1
*/

  if ( split_page_cycle_enc == 0 )
  {
    split_page_cycle = 704;
  }
  else
  if ( ( split_page_cycle_enc >= 1 ) && ( split_page_cycle_enc <= 64 ) )
  {
    split_page_cycle = split_page_cycle_enc;
  }
  else
  if ( ( split_page_cycle_enc >= 65 ) && ( split_page_cycle_enc <= 98 ) )
  {
    split_page_cycle = spc_code_to_val[split_page_cycle_enc - 65];
  }
  else
  {
    split_page_cycle = 1;
  }

  return split_page_cycle;

} /* end decode_split_page_cycle() */

/*!
 * \brief Reads carrier-specific EFS items and populates the local EFS data store.
 */
static void rr_read_carrier_specific_efs_nv_items(const sys_modem_as_id_e_type as_id)
{
  // Obtain a pointer to the EFS data
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

#ifdef FEATURE_LTE
  // Read /nv/item_files/modem/geran/grr/g2l_blind_redir_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_G2L_BLIND_REDIR_CONTROL,
          &efs_val,
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled is ENABLED");
      efs_data_ptr->g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled is DISABLED");
      efs_data_ptr->g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/grr/g2l_blind_redir_after_csfb_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_G2L_BLIND_REDIR_AFTER_CSFB_CONTROL,
          &efs_val,
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_csfb_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_csfb_enabled is ENABLED");
      efs_data_ptr->g2l_blind_redir_after_csfb_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_csfb_enabled is DISABLED");
      efs_data_ptr->g2l_blind_redir_after_csfb_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/grr/g2l_blind_redir_after_srvcc_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_G2L_BLIND_REDIR_AFTER_SRVCC_CONTROL,
          &efs_val,
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_srvcc_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_srvcc_enabled is ENABLED");
      efs_data_ptr->g2l_blind_redir_after_srvcc_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("g2l_blind_redir_after_srvcc_enabled is DISABLED");
      efs_data_ptr->g2l_blind_redir_after_srvcc_enabled = FALSE;
    }
  }
#endif /* FEATURE_LTE */

  // Read /nv/item_files/modem/geran/grr/fast_si_acq_during_csfb_control
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_FAST_SI_ACQ_DURING_CSFB_CONTROL,
          &efs_val,
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("fast_si_acq_during_csfb_control not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("fast_si_acq_during_csfb_enabled is ENABLED");
      efs_data_ptr->fast_si_acq_during_csfb_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("fast_si_acq_during_csfb_enabled is DISABLED");
      efs_data_ptr->fast_si_acq_during_csfb_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/grr/fast_si_acq_when_cs_only_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_FAST_SI_ACQ_WHEN_CS_ONLY_ENABLED,
          &efs_val,
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV fast_si_acq_when_cs_only_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Fast SI Acq when CS-only is ENABLED");
      efs_data_ptr->fast_si_acq_when_cs_only_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Fast SI Acq when CS-only is DISABLED");
      efs_data_ptr->fast_si_acq_when_cs_only_enabled = FALSE;
    }
  }

  // Read /nv/item_files/modem/geran/x2g_fast_si_acq_white_list
  {
    struct fs_stat file_status;

    efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list_present = FALSE;

    if (geran_efs_stat_per_sub(
          GERAN_EFS_X2G_FAST_SI_ACQ_WHITE_LIST,
          &file_status,
          as_id))
    {
      // Number of bytes storage per PLMN id
      #define BYTES_PER_PLMN 3

      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_X2G_FAST_SI_ACQ_WHITE_LIST,
              efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr,
              alloc_size_bytes,
              as_id
            ) == alloc_size_bytes)
        {
          efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.num_of_plmns = num_of_plmns;
          efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list_present = TRUE;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for x2g_fast_si_acq_white_list", alloc_size_bytes);
      }
    }
  }

  // Read /nv/item_files/modem/geran/negative_c1_white_list
  {
    struct fs_stat file_status;

    efs_data_ptr->negative_c1_white_list.num_of_plmns = 0;

    if (geran_efs_stat_per_sub(
          GERAN_EFS_NEGATIVE_C1_WHITE_LIST,
          &file_status,
          as_id))
    {
      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->negative_c1_white_list.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->negative_c1_white_list.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_NEGATIVE_C1_WHITE_LIST,
              efs_data_ptr->negative_c1_white_list.plmn_list_ptr,
              alloc_size_bytes,
              as_id
            ) == alloc_size_bytes)
        {
          efs_data_ptr->negative_c1_white_list.num_of_plmns = num_of_plmns;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for negative C1 whitelist", alloc_size_bytes);
      }
    }
  }

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
  // Read /nv/item_files/modem/geran/read_pch_during_transfer_whitelist
  {
    struct fs_stat file_status;

    efs_data_ptr->read_pch_during_transfer_whitelist.num_of_plmns = 0;   

    if (geran_efs_stat_per_sub(
          GERAN_EFS_READ_PCH_DURING_TRANSFER_WHITELIST,
          &file_status,
          as_id))
    {
      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_READ_PCH_DURING_TRANSFER_WHITELIST,
              efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr,
              alloc_size_bytes,
              as_id
            ) == alloc_size_bytes)
        {
          efs_data_ptr->read_pch_during_transfer_whitelist.num_of_plmns = num_of_plmns;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for read_pch_during_transfer_whitelist", alloc_size_bytes);
      }
    }
  }
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

  // Read /nv/item_files/modem/geran/plmn_search_freq_pruning_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_PLMN_SEARCH_FREQ_PRUNING_ENABLED,
          &efs_val,
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV plmn_search_freq_pruning_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("PLMN search freq pruning is ENABLED");
      efs_data_ptr->plmn_search_freq_pruning_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("PLMN search freq pruning is DISABLED");
      efs_data_ptr->plmn_search_freq_pruning_enabled = FALSE;
    }
  }

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  // Read /nv/item_files/modem/geran/fast_g2l_using_pseudo_si2q_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_FAST_G2L_USING_PSEUDO_SI2Q_ENABLED,
          &efs_val, 
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("fast_g2l_using_pseudo_si2q_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("fast_g2l_using_pseudo_si2q_enabled is ENABLED");
      efs_data_ptr->fast_g2l_using_pseudo_si2q_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("fast_g2l_using_pseudo_si2q_enabled is DISABLED");
      efs_data_ptr->fast_g2l_using_pseudo_si2q_enabled = FALSE;
    }
  }

  if (efs_data_ptr->fast_g2l_using_pseudo_si2q_enabled)
  {
    MSG_GERAN_HIGH_0("FEATURE_FAST_G2L_PSEUDO_SI2Q IS ENABLED, in NV ");

    /* Set the Default Value */
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.pseudo_si2q_def_data_valid = TRUE;
  
    /* Set Default Params Here */
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.geran_priority = RR_DEFAULT_GERAN_PRIO; 
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.h_prio = RR_GERAN_DEF_H_PRIO;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.thresh_gsm_low = RR_GERAN_DEF_THRESH_GSM_LOW;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.thresh_priority_search = RR_GERAN_DEF_THRESH_PRIO_SRCH;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_geran_def_data.t_reselection = RR_GERAN_DEF_T_RESEL;

    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.priority = RR_LTE_DEF_PRIO;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.meas_bandwidth = RR_LTE_DEF_MEAS_BW;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.qrxlevmin = RR_LTE_DEF_QRXLEV_MIN;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.thresh_high = RR_LTE_DEF_THRESH_HIGH;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_lte_def_data.thresh_low = RR_LTE_DEF_THRESH_LOW;
  
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.priority = RR_UTRAN_DEF_PRIO;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.qrxlevmin = RR_UTRAN_DEF_QRXLEV_MIN;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.thresh_high = RR_UTRAN_DEF_THRESH_HIGH;
    efs_data_ptr->rr_efs_pseudo_si2q_def_data.rr_utran_def_data.thresh_low = RR_UTRAN_DEF_THRESH_LOW;
  }
#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
  // Read /nv/item_files/modem/geran/sglte_g2x_conn_mode_resel_enabled
  {
    int8 efs_val;

    if (geran_efs_read_per_sub(
          GERAN_EFS_SGLTE_G2X_CONN_MODE_RESEL_ENABLED,
          &efs_val, 
          sizeof(efs_val),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("sglte_g2x_conn_mode_resel_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("sglte_g2x_conn_mode_resel_enabled is ENABLED");
      efs_data_ptr->sglte_g2x_conn_mode_resel_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("sglte_g2x_conn_mode_resel_enabled is DISABLED");
      efs_data_ptr->sglte_g2x_conn_mode_resel_enabled = FALSE;
    }
  }
#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */

#ifdef FEATURE_LTE
  // Read /nv/item_files/modem/geran/grr/depriotize_gsm_cell
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_CSFB_DEPRIOTIZE_GSM_CELL,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
     efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

   // Update the module store
   if (efs_val == 1)
   {
     MSG_GERAN_HIGH_0("De-Prioritizing GSM cell  is ENABLED");
     efs_data_ptr->depriotize_gsm_cell = TRUE;
   }
   else
   {
     MSG_GERAN_HIGH_0("De-Prioritizing GSM cell is DISABLED");
     efs_data_ptr->depriotize_gsm_cell = FALSE;
   }
 }

#endif /*FEATURE_LTE*/

#ifdef FEATURE_GERAN_HST_OPT
  // Read /nv/item_files/modem/geran/hst_opt_white_list
  {
    struct fs_stat file_status;

    efs_data_ptr->hst_opt_white_list.num_of_plmns = 0;

    if (geran_efs_stat_per_sub(
          GERAN_EFS_HIGH_SPEED_TRAIN_OPT_WHITE_LIST,
          &file_status,
          as_id))
    {
      // EFS file size in bytes
      fs_size_t file_size_bytes = file_status.st_size;

      // Number of PLMNs that can be represented by this file size, capped at MAX_NUM_OF_PLMNS
      int num_of_plmns = MIN((file_size_bytes / BYTES_PER_PLMN), MAX_NUM_OF_PLMNS);

      // Number of bytes to allocate to store the PLMN list
      int alloc_size_bytes = num_of_plmns * BYTES_PER_PLMN;

      efs_data_ptr->hst_opt_white_list.plmn_list_ptr = GPRS_MEM_MALLOC(alloc_size_bytes);

      if (efs_data_ptr->hst_opt_white_list.plmn_list_ptr != NULL)
      {
        if (geran_efs_read_per_sub(
              GERAN_EFS_HIGH_SPEED_TRAIN_OPT_WHITE_LIST,
              efs_data_ptr->hst_opt_white_list.plmn_list_ptr,
              alloc_size_bytes,
              as_id
            ) == alloc_size_bytes)
        {
          efs_data_ptr->hst_opt_white_list.num_of_plmns = num_of_plmns;
        }
      }
      else
      {
        MSG_GERAN_ERROR_1("Failed to allocate %d bytes for HST opt whitelist", alloc_size_bytes);
      }
    }
  }
#endif /*FEATURE_GERAN_HST_OPT*/

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* Read "/nv/item_files/modem/geran/grr/multisim_si_refresh_duration_secs" */
  /* A duration of 0ms disables the timer */  
  {
    if (geran_efs_read_per_sub(
          GERAN_EFS_MULTISIM_SI_REFRESH_DURATION_SECS,
          &efs_data_ptr->multisim_si_refresh_duration_secs,
          sizeof(efs_data_ptr->multisim_si_refresh_duration_secs),
          as_id) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV multisim_si_refresh_duration not present, use default 300 seconds");
      efs_data_ptr->multisim_si_refresh_duration_secs = 300; /*5 minutes*/
    }
    else
    {
      MSG_GERAN_HIGH_1("EFS-NV multisim_si_refresh_duration is %ds", efs_data_ptr->multisim_si_refresh_duration_secs);
    }
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  // Read /nv/item_files/modem/geran/grr/fake_bs_detection_enabled
  {
    int8 efs_val; 
    if (geran_efs_read_per_sub(GERAN_EFS_FAKE_BS_DETECTION_ENABLED,
                                 &efs_val,
                                 sizeof(efs_val),
                                 as_id) < 0)
  
    {
      MSG_GERAN_HIGH_0("EFS-NV fake_bs_detection_enabled not present");
        efs_val = -1;
    }
   
    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }
   
    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("fake bs detection is ENABLED");
      efs_data_ptr->fake_bs_detection_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("fake bs detection is DISABLED");
      efs_data_ptr->fake_bs_detection_enabled = FALSE;
    }
  }
}

/*!
 * \brief Reads EFS-NV items and populates the local NV data.
 */
static void rr_read_efs_nv_items(const sys_modem_as_id_e_type as_id)
{
  // Obtain a pointer to the EFS data
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  MSG_GERAN_HIGH_1("Reading EFS data... (as_id=%d)", (int)as_id);

  /* I. Read device-specific EFS items first */

#ifdef FEATURE_VAMOS
  // Read /nv/item_files/modem/geran/vamos_support
  {
    uint8 vamos_support;

    if (geran_efs_read_primary(GERAN_EFS_VAMOS_SUPPORT,
                               &vamos_support,
                               sizeof(vamos_support)) < 0)
    {
      MSG_GERAN_HIGH_0("VAMOS-I enabled (default)");
      vamos_support = 1;
    }

    // vamos_support: 0=OFF 1=VAMOS-I 2=VAMOS-II
    if (vamos_support != 0)
    {
      efs_data_ptr->vamos_enabled = TRUE;
    }
    else
    {
      efs_data_ptr->vamos_enabled = FALSE;
    }
  }
#endif /* FEATURE_VAMOS */

  // Read /nv/item_files/modem/geran/pscan_results_reuse_time_secs

  if (geran_efs_read_primary(GERAN_EFS_PSCAN_RESULTS_REUSE_TIME_SECS,
                             &efs_data_ptr->pscan_results_reuse_time_secs,
                             sizeof(efs_data_ptr->pscan_results_reuse_time_secs)) < 0)
  {
    MSG_GERAN_HIGH_1("pscan_results_reuse_time_secs not present - default=%ds",
                     RR_PSCAN_STALENESS_DEFAULT);
    efs_data_ptr->pscan_results_reuse_time_secs = RR_PSCAN_STALENESS_DEFAULT;
  }
  else
  {
    MSG_GERAN_HIGH_1("pscan_results_reuse_time_secs=%d", efs_data_ptr->pscan_results_reuse_time_secs);
  }

  // Read /nv/item_files/modem/geran/add_crh_db

  if (geran_efs_read_primary(GERAN_EFS_ADD_CRH_DB,
                             &efs_data_ptr->add_crh_db, 
                             sizeof(efs_data_ptr->add_crh_db)) < 0)
  {
    // If the value if not configured (file not present), the default value is 0
    efs_data_ptr->add_crh_db = 0;
  }

  // Read /nv/item_files/modem/geran/add_crh_scell_thresh_db

  if (efs_data_ptr->add_crh_db != 0)
  {
    if (geran_efs_read_primary(GERAN_EFS_ADD_CRH_SCELL_THRESH_DB,
                               &efs_data_ptr->add_crh_scell_thresh_db, 
                               sizeof(efs_data_ptr->add_crh_scell_thresh_db)) < 0)
    {
      // If the value if not configured (file not present), the default value is -70
      efs_data_ptr->add_crh_scell_thresh_db = -70;
    }
  }

  MSG_GERAN_HIGH_2("add_crh_db=%ddB add_crh_scell_thresh_db=%ddB",
                   efs_data_ptr->add_crh_db, efs_data_ptr->add_crh_scell_thresh_db);

  // Read /nv/item_files/modem/geran/grr/sys_info_cache_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_SYS_INFO_CACHE_ENABLED,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV sys_info_cache_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Sys info cache is ENABLED");
      efs_data_ptr->sys_info_cache_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Sys info cache is DISABLED");
      efs_data_ptr->sys_info_cache_enabled = FALSE;
    }
  }

#ifdef FEATURE_GSM_BAND_AVOIDANCE
  if (geran_efs_read_primary(GERAN_EFS_BAND_AVOID_ENABLED, 
                             &efs_data_ptr->band_avoid_enabled, 
                             sizeof(efs_data_ptr->band_avoid_enabled)) < 0)
  {
    MSG_GERAN_HIGH_0("Band Avoid enabled (default)");
    efs_data_ptr->band_avoid_enabled = TRUE;
  }
  MSG_GERAN_HIGH_1("Band Avoid enabled: %d", efs_data_ptr->band_avoid_enabled);

  if (geran_efs_read_primary(GERAN_EFS_BAND_AVOID_NCELL_THRESHOLD, 
                             &efs_data_ptr->band_avoid_ncell_threshold, 
                             sizeof(efs_data_ptr->band_avoid_ncell_threshold)) < 0)
  {   
    MSG_GERAN_HIGH_0("Band Avoid Ncell Threshold (default)"); 
    efs_data_ptr->band_avoid_ncell_threshold = 21; /*rxlev = -90dBm*/
  }
  MSG_GERAN_HIGH_1("Band Avoid Ncell Threshold: %d", efs_data_ptr->band_avoid_ncell_threshold);

  if (geran_efs_read_primary(GERAN_EFS_BAND_AVOID_MAX_MEAS_RPTS, 
                             &efs_data_ptr->band_avoid_max_meas_rpts, 
                             sizeof(efs_data_ptr->band_avoid_max_meas_rpts)) < 0)
  {   
    MSG_GERAN_HIGH_0("Band Avoid Max Meas Rpts (default)"); 
    efs_data_ptr->band_avoid_max_meas_rpts = 4;
  }
  MSG_GERAN_HIGH_1("Band Avoid Max Meas Rpts: %d", efs_data_ptr->band_avoid_max_meas_rpts);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

  // Read /nv/item_files/modem/geran/grr/iterative_si_acq_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_ITERATIVE_SI_ACQ_ENABLED,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV iterative_si_acq_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 1;  // Default is ENABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Iterative SI acq is ENABLED");
      efs_data_ptr->iterative_si_acq_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Iterative SI acq is DISABLED");
      efs_data_ptr->iterative_si_acq_enabled = FALSE;
    }
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  // Read /nv/item_files/modem/geran/scell_resync_time_secs
  if (geran_efs_read_primary(GERAN_EFS_SCELL_RESYNC_TIME_SECS,
                             &efs_data_ptr->scell_resync_time_secs,
                             sizeof(efs_data_ptr->scell_resync_time_secs)) < 0)
  {
    MSG_GERAN_HIGH_1("scell_resync_time_secs not present - default=%ds",
                     RR_SCELL_RESYNC_TIME_DEFAULT);
    efs_data_ptr->scell_resync_time_secs = RR_SCELL_RESYNC_TIME_DEFAULT;
  }
  else
  {
    MSG_GERAN_HIGH_1("scell_resync_time_secs=%d", efs_data_ptr->scell_resync_time_secs);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  // Read /nv/item_files/modem/geran/eng_mode_debug_enabled
  {
    int8 efs_val;

    if (geran_efs_read_primary(GERAN_EFS_ENG_MODE_DEBUG_ENABLED,
                               &efs_val,
                               sizeof(efs_val)) < 0)
    {
      MSG_GERAN_HIGH_0("EFS-NV eng_mode_debug_enabled not present");
      efs_val = -1;
    }

    // Sanity-check the result is 0 or 1
    if ((efs_val != 0) && (efs_val != 1))
    {
      efs_val = 0;  // Default is DISABLED for this EFS-NV item
    }

    // Update the module store
    if (efs_val == 1)
    {
      MSG_GERAN_HIGH_0("Eng Mode Debug is ENABLED");
      efs_data_ptr->eng_mode_debug_enabled = TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_0("Eng Mode Debug is DISABLED");
      efs_data_ptr->eng_mode_debug_enabled = FALSE;
    }
  }

  if (geran_efs_read_primary(GERAN_EFS_G2X_MEASUREMENT_CONTROL,
                             &efs_data_ptr->g2x_measurement_control,
                             sizeof(efs_data_ptr->g2x_measurement_control)) < 0)
  {
    MSG_GERAN_HIGH_0("g2x_measurement_control not present (default=ENABLED)");

    efs_data_ptr->g2x_measurement_control.g2w_enabled = TRUE;
    efs_data_ptr->g2x_measurement_control.g2t_enabled = TRUE;
    efs_data_ptr->g2x_measurement_control.g2l_enabled = TRUE;
  }
  else
  {
    if ((efs_data_ptr->g2x_measurement_control.g2w_enabled != TRUE &&
         efs_data_ptr->g2x_measurement_control.g2w_enabled != FALSE) ||
        (efs_data_ptr->g2x_measurement_control.g2t_enabled != TRUE &&
         efs_data_ptr->g2x_measurement_control.g2t_enabled != FALSE) ||
        (efs_data_ptr->g2x_measurement_control.g2l_enabled != TRUE &&
         efs_data_ptr->g2x_measurement_control.g2l_enabled != FALSE))
    {
      MSG_GERAN_HIGH_0("g2x_measurement_control inconsistent (default=ENABLED)");

      efs_data_ptr->g2x_measurement_control.g2w_enabled = TRUE;
      efs_data_ptr->g2x_measurement_control.g2t_enabled = TRUE;
      efs_data_ptr->g2x_measurement_control.g2l_enabled = TRUE;
    }
  }

  MSG_GERAN_HIGH_3("g2x_measurement_control(g2w_enabled:%d, g2t_enabled:%d, g2l_enabled:%d)",
                   efs_data_ptr->g2x_measurement_control.g2w_enabled,
                   efs_data_ptr->g2x_measurement_control.g2t_enabled,
                   efs_data_ptr->g2x_measurement_control.g2l_enabled);

#ifdef FEATURE_IDLE_DRX_SCALING
  // Read /nv/item_files/modem/geran/idle_drx_scaling_suspend_time_secs
  if (geran_efs_read_primary(GERAN_EFS_IDLE_DRX_SCALING_SUSPEND_TIME_SECS,
                             &efs_data_ptr->idle_drx_scaling_suspend_time_secs,
                             sizeof(efs_data_ptr->idle_drx_scaling_suspend_time_secs)) < 0)
  {
    MSG_GERAN_HIGH_1("idle_idle_drx_scaling_suspend_time_secs not present - default=%ds",
                     RR_IDLE_DRX_SCALING_SUSPEND_TIMEOUT_DEFAULT);
    efs_data_ptr->idle_drx_scaling_suspend_time_secs = RR_IDLE_DRX_SCALING_SUSPEND_TIMEOUT_DEFAULT;
  }
  else
  {
    MSG_GERAN_HIGH_1("idle_drx_scaling_suspend_time_secs=%d", efs_data_ptr->idle_drx_scaling_suspend_time_secs);
  }
#endif /* FEATURE_IDLE_DRX_SCALING */

  /* II. Read sub-specific EFS items next */
  rr_read_carrier_specific_efs_nv_items(as_id);

  return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the value of SPLIT_PAGE_CYCLE stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint16 
 */
uint16 rr_get_nv_split_page_cycle(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->split_page_cycle;

} /* end rr_get_nv_split_page_cycle() */

/*!
 * \brief Returns the value of NON_DRX_TIMER.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
uint8 rr_get_nv_non_drx_timer(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->non_drx_timer;

} /* end rr_get_nv_non_drx_timer() */

/*!
 * \brief Returns the value of MULTISLOT_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
uint8 rr_get_nv_multislot_class(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->multislot_class;

} /* end rr_get_nv_multislot_class() */

/*!
 * \brief Returns the value of ANITE_GCF_FLAG stored in NV. 
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_get_nv_anite_gcf_flag(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->anite_gcf_flag;

} /* end rr_get_nv_anite_gcf_flag() */

/*!
 * \brief Returns TRUE if EGPRS is enabled in NV; FALSE otherwise.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_egprs_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->egprs_enabled;

} /* end rr_get_nv_egprs_enabled() */

/*!
 * \brief Returns the value of EGPRS_MULTISLOT_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8
 */
uint8 rr_get_nv_egprs_multislot_class(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->egprs_multislot_class;

} /* end rr_get_nv_egprs_multislot_class() */

/*!
 * \brief Returns TRUE if the NV parameter EGPRS_8PSK_POWER_CLASS is non-zero; FALSE if zero.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_egprs_8psk_ul_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->egprs_8psk_ul_enabled;

} /* end rr_get_nv_egprs_8psk_ul_enabled() */

/*!
 * \brief Returns the egprs 8psk power capability.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
uint8 rr_get_nv_egprs_8psk_power_capability(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->egprs_8psk_power_capability;

} /* end rr_get_nv_egprs_8psk_power_capability() */

/*!
 * \brief Returns TRUE if GERAN FEATURE PACK 1 is enabled in NV; FALSE otherwise.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_geran_feature_pack_1_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->geran_feature_pack_1_enabled;

} /* end rr_get_nv_geran_feature_pack_1_enabled() */

/*!
 * \brief Returns the current mask from NV that indicates support for inter-RAT NACC.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 - 0x01 indicates W2G, 0x02 indicates G2W, 0x03 indicates both
 */
uint8 rr_get_nv_interrat_nacc_support(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->interrat_nacc_support;

} /* end rr_get_nv_interrat_nacc_support() */


#ifdef FEATURE_GSM_DTM

/*!
 * \brief Indicates if DTM is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_dtm_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->dtm_enabled;

} /* end rr_get_nv_dtm_enabled() */

/*!
 * \brief Returns the value of DTM_EGPRS_MULTISLOT_SUBCLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
uint8 rr_get_nv_dtm_egprs_multislot_subclass(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->dtm_egprs_multislot_subclass;

} /* end rr_get_nv_dtm_egprs_multislot_subclass() */

/*!
 * \brief Returns the value of DTM_MULTISLOT_CLASS, derived from DTM_EGPRS_MULTISLOT_SUBCLASS.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
uint8 rr_get_nv_dtm_multislot_class(const sys_modem_as_id_e_type as_id)
{
  uint8 dtm_multislot_subclass = rr_get_nv_dtm_egprs_multislot_subclass(as_id);

  return rr_msc_conv_dtm_multislot_subclass_to_class(dtm_multislot_subclass);
}

#ifdef FEATURE_GSM_EDTM
/*!
 * \brief Indicates if EDTM is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
boolean rr_get_nv_edtm_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->edtm_enabled;
}
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

/*!
 * \brief Returns the value of GEA_SUPPORTED stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8
 */
uint8 rr_get_nv_gea_supported(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->gea_supported;

} /* end rr_get_nv_gea_supported() */

#ifdef FEATURE_GSM_GPRS_MSC33

/*!
 * \brief Returns the value of HMSC_GPRS_CODED_MS_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8
 */
uint8 rr_get_nv_hmsc_gprs_coded_ms_class(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->hmsc_gprs_coded_ms_class;

} /* end rr_get_nv_hmsc_gprs_coded_ms_class() */

/*!
 * \brief Returns the value of HMSC_EGPRS_CODED_MS_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8
 */
uint8 rr_get_nv_hmsc_egprs_coded_ms_class(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_data_ptr(as_id)->hmsc_egprs_coded_ms_class;

} /* end rr_get_nv_hmsc_egprs_coded_ms_class() */

#endif /* FEATURE_GSM_GPRS_MCS33 */

#ifdef FEATURE_VAMOS
/*!
 * \brief Indicates if VAMOS is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE is VAMOS is enabled in NV (VAMOS-I or VAMOS-II); FALSE otherwise
 */
boolean rr_get_nv_vamos_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->vamos_enabled;
}
#endif /* FEATURE_VAMOS */

/*!
 * \brief Indicates if Fast SI Acq during CSFB is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if Fast SI Acq during CSFB is enabled, FALSE otherwise
 */
boolean rr_get_nv_fast_si_acq_during_csfb_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->fast_si_acq_during_csfb_enabled;
}


boolean rr_nv_x2g_fast_si_acq_feature_is_enabled(const sys_modem_as_id_e_type as_id)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  if ((efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list_present) &&
      (efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.num_of_plmns > 0))
  {
    MSG_GERAN_HIGH_0("X2G SKIP SI13 enabled");
    return TRUE;
  }
  else
  {
    MSG_GERAN_HIGH_0("X2G SKIP SI13 NOT enabled");
    return FALSE;
  }	
}

boolean rr_nv_x2g_fast_si_acq_is_plmn_match(
  sys_plmn_id_s_type plmn,
  const sys_modem_as_id_e_type as_id
)
{
  int i;

  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  // Point to the first PLMN in the list
  sys_plmn_id_s_type *plmn_id_ptr = efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.plmn_list_ptr;

  if (plmn_id_ptr != NULL)
  {
    sys_mcc_type mcc;
    sys_mnc_type mnc;
    boolean is_undefined;
    boolean includes_pcsd;

    sys_plmn_get_mcc_mnc(
      plmn,
      &is_undefined,
      &includes_pcsd,
      &mcc,
      &mnc
    );

    MSG_GERAN_HIGH_2(
      "X2G SKIP SI13 compare PLMN=(%d,%d)",
      mcc,
      mnc
    );

    for (i = 0; ((i < efs_data_ptr->x2g_fast_si_acq_info.x2g_fast_si_acq_white_list.num_of_plmns)&&(i < MAX_NUM_OF_PLMNS)); i++)
    {
      sys_plmn_get_mcc_mnc(
        *plmn_id_ptr,
        &is_undefined,
        &includes_pcsd,
        &mcc,
        &mnc
      );

      MSG_GERAN_HIGH_2(
        "x2g_fast_si_acq_white_list PLMN=(%d,%d)",
        mcc,
        mnc
      );

      if (sys_plmn_match(plmn, *plmn_id_ptr))
      {
        return TRUE;
      }

      // Point to the next PLMN in the list
      ++plmn_id_ptr;
    }
  }

  return FALSE;
}

/*!
 * \brief Indicates if Fast SI Acq when CS-only is enabled in NV.
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_fast_si_acq_when_cs_only_is_enabled(const sys_modem_as_id_e_type as_id)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr(as_id)->fast_si_acq_when_cs_only_enabled;
}

/*!
 * \brief Indicates if the supplied PLMN is present in the C1 relaxation white-list.
 * 
 * \return boolean - TRUE if present, FALSE otherwise
 */
static boolean rr_nv_check_plmn_in_whitelist(sys_plmn_id_s_type *plmn_id_ptr, int num_list_entries, sys_plmn_id_s_type plmn)
{
  int i;

  MSG_GERAN_HIGH_1("Num of PLMN %d ", num_list_entries);
  
  if (plmn_id_ptr != NULL)
  {
    sys_mcc_type mcc;
    sys_mnc_type mnc;
    boolean is_undefined;
    boolean includes_pcsd;

    sys_plmn_get_mcc_mnc(
      plmn,
      &is_undefined,
      &includes_pcsd,
      &mcc,
      &mnc
    );
    MSG_GERAN_HIGH_2("Current  PLMN=(%d,%d)",mcc,mnc);

    for (i = 0; ((i < num_list_entries) && (i < MAX_NUM_OF_PLMNS)); i++)
    {
      sys_plmn_get_mcc_mnc(
        *plmn_id_ptr,
        &is_undefined,
        &includes_pcsd,
        &mcc,
        &mnc
      );

      MSG_GERAN_HIGH_2("White list PLMN=(%d,%d)",mcc,mnc);

      if (sys_plmn_match(plmn, *plmn_id_ptr))
      {
        return TRUE;
      }

      // Point to the next PLMN in the list
      ++plmn_id_ptr;
    }
  }

  return FALSE;
}

boolean rr_get_fake_bs_detection_enabled(const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  return rr_nv_get_efs_data_ptr(as_id)->fake_bs_detection_enabled;
}

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
boolean rr_nv_read_pch_during_transfer_is_plmn_match(sys_plmn_id_s_type plmn, const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    MSG_GERAN_HIGH_0_G("Read PCH during xfer disabled (GCF is ON)");  
    return FALSE;
  }
 
  return (rr_nv_check_plmn_in_whitelist(efs_data_ptr->read_pch_during_transfer_whitelist.plmn_list_ptr,
                                        efs_data_ptr->read_pch_during_transfer_whitelist.num_of_plmns,
                                        plmn));
}

#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

boolean rr_nv_c1_relaxation_is_plmn_match(sys_plmn_id_s_type plmn, const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    MSG_GERAN_HIGH_0_G("C1 relaxation disabled (GCF is ON)");  
    return FALSE;
  }

  return (rr_nv_check_plmn_in_whitelist(efs_data_ptr->negative_c1_white_list.plmn_list_ptr,
                                        efs_data_ptr->negative_c1_white_list.num_of_plmns,
                                        plmn));
}

/*!
 * \brief Returns the value of the power-scan re-use timer (0=disabled)
 * 
 * \return rex_timer_cnt_type - timer vaue in milliseconds
 */
rex_timer_cnt_type rr_nv_get_pscan_results_reuse_time_millisecs(const sys_modem_as_id_e_type as_id)
{
  rex_timer_cnt_type timeout_secs = rr_nv_get_efs_data_ptr(as_id)->pscan_results_reuse_time_secs;

  return (CONVERT_SECONDS_TO_MILLISECONDS(timeout_secs));
}

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
/*!
 * \brief Indicates if SGLTE Connected Mode Reselection feature is enabled in NV.
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_get_nv_sglte_g2x_conn_mode_resel_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->sglte_g2x_conn_mode_resel_enabled;
}
#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q

boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->fast_g2l_using_pseudo_si2q_enabled;
}

boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_data_valid(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->rr_efs_pseudo_si2q_def_data.pseudo_si2q_def_data_valid;

}

rr_efs_pseudo_si2q_def_data_t* rr_get_efs_nv_fast_g2l_pseudo_si2q_data(const sys_modem_as_id_e_type as_id)
{
  return(&rr_nv_get_efs_data_ptr(as_id)->rr_efs_pseudo_si2q_def_data);
}

#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

/*!
 * \brief Returns the additional C2 cell reselection hysteresis values.
 * 
 * \param add_crh_db_ptr (out)
 * \param add_crh_scell_thresh_db_ptr (out)
 * 
 * \return boolean - TRUE if configured, FALSE otherwise
 */
boolean rr_nv_get_add_crh_enabled(
  int8 *add_crh_db_ptr,
  int8 *add_crh_scell_thresh_db_ptr,
  const sys_modem_as_id_e_type as_id
)
{
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  if (efs_data_ptr->add_crh_db != 0)
  {
    *add_crh_db_ptr = efs_data_ptr->add_crh_db;
    *add_crh_scell_thresh_db_ptr = efs_data_ptr->add_crh_scell_thresh_db;

    return TRUE;
  }

  return FALSE;
}

/*!
 * \brief Returns the value of the plmn_search_freq_pruning_enabled. 
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_get_plmn_search_freq_pruning_enabled(const sys_modem_as_id_e_type as_id)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr(as_id)->plmn_search_freq_pruning_enabled;
}

/*!
 * \brief Indicates if the sys info cache functionality is enabled.
 *        Note: Feature is disabled for a Type Approval SIM. 
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_sys_info_cache_is_enabled(const sys_modem_as_id_e_type as_id)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr(as_id)->sys_info_cache_enabled;
}

/*!
 * \brief Indicates if the iterative updating during SI acq feature is enabled.
 *        Note: Feature is not disabled for a Type Approval SIM.
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_iterative_si_acq_is_enabled(const sys_modem_as_id_e_type as_id)
{
  // Note: Feature is not disabled for a Type Approval SIM

  return rr_nv_get_efs_data_ptr(as_id)->iterative_si_acq_enabled;
}

/*!
 * \brief Indicates if eng mode debug is enabled.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
boolean rr_nv_eng_mode_debug_is_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->eng_mode_debug_enabled;
}

#ifdef FEATURE_GSM_BAND_AVOIDANCE
/**
  @brief Returns if band avoidance is enabled
*/
boolean rr_is_band_avoidance_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->band_avoid_enabled;
}

/**
  @brief Returns the threshold for band avoidance ncells as 
         rxlev (0.63)
*/
uint8 rr_get_band_avoid_ncell_threshold(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->band_avoid_ncell_threshold;
}

/**
  @brief Returns the max number of meas repts that can be sent 
  when doing band avoidance. 
*/
uint8 rr_get_band_avoid_max_meas_rpts(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->band_avoid_max_meas_rpts;
}
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_LTE

boolean rr_nv_get_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(const sys_modem_as_id_e_type as_id)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr(as_id)->g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled;
}

boolean rr_nv_get_g2l_blind_redir_after_csfb_enabled(const sys_modem_as_id_e_type as_id)
{
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr(as_id)->g2l_blind_redir_after_csfb_enabled;
}

boolean rr_nv_get_g2l_blind_redir_after_srvcc_enabled(const sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->g2l_blind_redir_after_srvcc_enabled;
}

#endif /* FEATURE_LTE */

boolean rr_nv_get_g2w_measurements_enabled(sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->g2x_measurement_control.g2w_enabled;
}

boolean rr_nv_get_g2t_measurements_enabled(sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->g2x_measurement_control.g2t_enabled;
}

boolean rr_nv_get_g2l_measurements_enabled(sys_modem_as_id_e_type as_id)
{
  return rr_nv_get_efs_data_ptr(as_id)->g2x_measurement_control.g2l_enabled;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
uint32 rr_nv_get_scell_resync_time_millisecs(const sys_modem_as_id_e_type as_id)
{
  uint32 timeout_secs = (uint32)(rr_nv_get_efs_data_ptr(as_id)->scell_resync_time_secs);

  if (RR_SCELL_RESYNC_TIME_DEFAULT == timeout_secs)
  {
    return(0xFFFFFFFF);
  }

  return(CONVERT_SECONDS_TO_MILLISECONDS(timeout_secs));
}

uint32 rr_nv_get_multisim_si_refresh_duration_milliseconds(sys_modem_as_id_e_type as_id)
{
  uint32 duration_secs = (uint32)(rr_nv_get_efs_data_ptr(as_id)->multisim_si_refresh_duration_secs);
  
  return(CONVERT_SECONDS_TO_MILLISECONDS(duration_secs)); 
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_IDLE_DRX_SCALING
uint32 rr_nv_get_idle_drx_scaling_suspend_time_millisecs(const sys_modem_as_id_e_type as_id)
{
  uint32 timeout_secs = rr_nv_get_efs_data_ptr(as_id)->idle_drx_scaling_suspend_time_secs;

  return(CONVERT_SECONDS_TO_MILLISECONDS(timeout_secs));
}
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_LTE
/*!
 * \brief Indicates if De-priotizing GSM cell  is enabled or not.
 * 
 * \param as_id (in)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise.
 */
boolean rr_nv_depriotize_gsm_cell_is_enabled(const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  // Feature is disabled for a Type Approval SIM
  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;
  }

  return rr_nv_get_efs_data_ptr(as_id)->depriotize_gsm_cell;
}
#endif /*FEATURE_LTE*/

#ifdef FEATURE_GERAN_HST_OPT
/*!
 * \brief Indicates if High Speed Train Optimization can be used with the supplied PLMN.
 * 
 * \param plmn (in), gas_id (in)
 * 
 * \return boolean - TRUE if PLMN matches, FALSE otherwise.
 */
boolean rr_nv_hst_opt_is_plmn_match(sys_plmn_id_s_type plmn, const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  rr_efs_nv_data_t *efs_data_ptr = rr_nv_get_efs_data_ptr(as_id);

  if (rr_get_nv_anite_gcf_flag(as_id))
  {
    return FALSE;    
  }

  return (rr_nv_check_plmn_in_whitelist(efs_data_ptr->hst_opt_white_list.plmn_list_ptr,
                                        efs_data_ptr->hst_opt_white_list.num_of_plmns,
                                        plmn));
}
#endif /*FEATURE_GERAN_HST_OPT*/
/*!
 * \brief Called when the NV data is received from NAS.
 *  
 * The NV data is copied into local storage. The Public Store is updated later when PS access is enabled. 
 * Note: the NV data is stored according to AS-ID in this message, not GAS-ID
 * 
 * \param msg_ptr (in) - pointer to the RR_GMM_GPRS_NV_PARAMS_IND message
 */
void rr_nv_process_nv_params_ind(rr_gmm_gprs_nv_params_ind_t *msg_ptr)
{
  rr_nv_data_t *rr_nv_data_ptr;

  // Sanity-check the input
  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_nv_data_ptr = rr_nv_get_data_ptr(msg_ptr->as_id);

  rr_nv_data_ptr->multislot_class              = msg_ptr->nv_multislot_class;
  rr_nv_data_ptr->non_drx_timer_enc            = msg_ptr->nv_non_drx_timer;
  rr_nv_data_ptr->split_page_cycle_enc         = msg_ptr->nv_split_paging_cycle;
  rr_nv_data_ptr->anite_gcf_flag               = msg_ptr->nv_anite_gcf_flag;
  rr_nv_data_ptr->egprs_enabled                = msg_ptr->nv_edge_feature_support;
  rr_nv_data_ptr->egprs_multislot_class        = msg_ptr->nv_edge_multislot_class;
  rr_nv_data_ptr->egprs_8psk_ul_enabled        = msg_ptr->nv_edge_8psk_power_class != 0;
  rr_nv_data_ptr->egprs_8psk_power_capability  = msg_ptr->nv_edge_8psk_power_capability;
  rr_nv_data_ptr->geran_feature_pack_1_enabled = msg_ptr->nv_edge_nw_ass_cell_change;
  rr_nv_data_ptr->interrat_nacc_support        = msg_ptr->nv_interrat_nacc_support;
#ifdef FEATURE_GSM_DTM
  rr_nv_data_ptr->dtm_enabled                  = msg_ptr->nv_dtm_feature_support;
  rr_nv_data_ptr->dtm_egprs_multislot_subclass = msg_ptr->nv_dtm_multislot_subclass;
#ifdef FEATURE_GSM_EDTM
  rr_nv_data_ptr->edtm_enabled                 = msg_ptr->nv_edtm_feature_support;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  rr_nv_data_ptr->gea_supported                = msg_ptr->nv_gea_supported;
#ifdef FEATURE_GSM_GPRS_MSC33
  rr_nv_data_ptr->hmsc_gprs_coded_ms_class     = msg_ptr->nv_hmsc_gprs_coded_ms_class;
  rr_nv_data_ptr->hmsc_egprs_coded_ms_class    = msg_ptr->nv_hmsc_egprs_coded_ms_class;
#endif /* FEATURE_GSM_GPRS_MSC33 */

#if defined(NV_DISABLE_VOICE_SUPPORT_MASK) || defined(NV_DISABLE_CSDATA_SUPPORT_MASK)
  {
    // Assume all call types are allowed
    boolean voice_calls_supported = TRUE;
    boolean csdata_calls_supported = TRUE;

    /* NAS will provide the mask in the RR_GMM_GPRS_NV_PARAMS_IND primitive */
#ifdef NV_DISABLE_VOICE_SUPPORT_MASK
    if ((msg_ptr->nv_disable_call_type & NV_DISABLE_VOICE_SUPPORT_MASK) != 0)
    {
      MSG_GERAN_HIGH_1("ds%d:Voice calls are disabled by NV",(int)msg_ptr->as_id+1);
      voice_calls_supported = FALSE;
    }
#endif /* NV_DISABLE_VOICE_SUPPORT_MASK */

#ifdef NV_DISABLE_CSDATA_SUPPORT_MASK
    if ((msg_ptr->nv_disable_call_type & NV_DISABLE_CSDATA_SUPPORT_MASK) != 0)
    {
      MSG_GERAN_HIGH_1("ds%d:CS data calls are disabled by NV",(int)msg_ptr->as_id+1);
      csdata_calls_supported = FALSE;
    }
#endif /* NV_DISABLE_CSDATA_SUPPORT_MASK */

    rr_channel_mode_set_supported(
      voice_calls_supported,    // voice_calls_supported
      csdata_calls_supported,   // csdata_calls_supported
      msg_ptr->as_id            // as_id
    );
  }
#endif /* NV_DISABLE_VOICE_SUPPORT_MASK | NV_DISABLE_CSDATA_SUPPORT_MASK */

  MSG_GERAN_HIGH_0("NV params from MM");
  MSG_GERAN_HIGH_3("multislot:%d non_drx:%d, spc:%d",
                   (int)rr_nv_data_ptr->multislot_class,
                   (int)rr_nv_data_ptr->non_drx_timer_enc,
                   (int)rr_nv_data_ptr->split_page_cycle_enc );

  MSG_GERAN_HIGH_3("egprs:%d, egprs_multislot:%d, 8psk_ul:%d",
                   (int)rr_nv_data_ptr->egprs_enabled,
                   (int)rr_nv_data_ptr->egprs_multislot_class,
                   (int)rr_nv_data_ptr->egprs_8psk_ul_enabled );

#ifdef FEATURE_GSM_DTM
  {
    int edtm_support;

#ifdef FEATURE_GSM_EDTM
    edtm_support = (int)rr_nv_data_ptr->edtm_enabled;
#else
    edtm_support = 0xFF;
#endif /*  FEATURE_GSM_EDTM */

    MSG_GERAN_HIGH_3("DTM:%1d MSC:%d (EDTM:%02X)",
                     (int)rr_nv_data_ptr->dtm_enabled,
                     (int)rr_nv_data_ptr->dtm_egprs_multislot_subclass,
                     edtm_support);
  }
#endif /* FEATURE_GSM_DTM */

  MSG_GERAN_HIGH_2("geran_feature_pack_1:%d, interrat_nacc 0x%02X",
                   (int)rr_nv_data_ptr->geran_feature_pack_1_enabled,
                   (int)rr_nv_data_ptr->interrat_nacc_support);

  /* Calculate value for split_page_cycle based on encoded value */
  rr_nv_data_ptr->split_page_cycle = decode_split_page_cycle(rr_nv_data_ptr->split_page_cycle_enc);

  /* Calculate value for non_drx_timer based on encoded value */
  rr_nv_data_ptr->non_drx_timer = rr_gprs_decode_drx_timer(rr_nv_data_ptr->non_drx_timer_enc);

  if (GERAN_DATA_SPACE_INDEX_1 == rr_nv_get_data_space_index(msg_ptr->as_id))
  {
    // Use the reception of the first NV message to read the GERAN EFS-NV items
    geran_read_efs_nv_items();
  }

  /* Only GRR EFS items are carrier-specific and should be read per sub */
  rr_read_efs_nv_items(msg_ptr->as_id);

  /* Inform the Multi-Slot Class module that NV data is available */
  rr_msc_nv_ready(msg_ptr->as_id);

} /* end rr_nv_process_nv_params_ind() */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief Called from GCOMMON to post GERAN_NV_REFRESH_IND to GRR queue.
 *  
 * \param gas_id - GAS ID
 */
void rr_nv_send_nv_refresh_ind(const gas_id_t gas_id)
{
  MSG_GERAN_HIGH_1_G("Send GERAN_NV_REFRESH_IND to GRR (gas_id=%d)", gas_id);
  rr_send_imsg_geran_nv_refresh_ind(gas_id);
}

/*!
 * \brief Called when the NV refresh ind received from MCFG.
 *  
 * Re-reads carrier specific EFS/NV items. 
 * 
 * \param gas_id - GAS ID
 */
void rr_nv_process_nv_refresh_ind(const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  MSG_GERAN_HIGH_1("Refreshing EFS data... (as_id=%d)", (int)as_id);

  /* Re-read carrier specific EFS items */
  rr_read_carrier_specific_efs_nv_items(as_id);
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef PERLUTF
#error code not present
#endif // PERLUTF


/* EOF */

