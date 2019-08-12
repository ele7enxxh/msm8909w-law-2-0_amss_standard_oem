/*============================================================================
  @file rr_band_avoid.c

  @brief This module implements a state machine that manages band avoidance for
   multi-SIM

   CXM provides blacklisted frquencies which GERAN attemps to avoid by
   manipulating measurement reports to force the NW to handover to a non 
   blacklisted band.  If GERAN cannot avoid because there are no suitable 
   neighbours or the max number of measurement reports are sent a NACK
   is indicated to CXM.

                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_band_avoid.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_BAND_AVOIDANCE

#include "comdef.h"

#include "rr_task.h"
#include "rr_band_avoid.h"
#include "rr_nv.h"
#include "rr_general.h"
#include "wwan_coex_mgr.h"
#include "rr_defs.h"
#include "rr_gprs_debug.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*frequencies are defined in KHz*/
#define RR_850_UL_LOWER_FREQ 824200
#define RR_850_UL_UPPER_FREQ 848800

#define RR_850_DL_LOWER_FREQ 869200
#define RR_850_DL_UPPER_FREQ 893800

#define RR_PGSM_UL_LOWER_FREQ 890000
#define RR_PGSM_UL_UPPER_FREQ 914800

#define RR_PGSM_DL_LOWER_FREQ 935000
#define RR_PGSM_DL_UPPER_FREQ 959800

#define RR_EGSM_LOW_CHANS_UL_LOWER_FREQ 890000
#define RR_EGSM_LOW_CHANS_UL_UPPER_FREQ 914800

#define RR_EGSM_LOW_CHANS_DL_LOWER_FREQ 935000
#define RR_EGSM_LOW_CHANS_DL_UPPER_FREQ 959800

#define RR_EGSM_HIGH_CHANS_UL_LOWER_FREQ 880200
#define RR_EGSM_HIGH_CHANS_UL_UPPER_FREQ 889800

#define RR_EGSM_HIGH_CHANS_DL_LOWER_FREQ 925200
#define RR_EGSM_HIGH_CHANS_DL_UPPER_FREQ 934800

#define RR_DCS_UL_LOWER_FREQ 1710200
#define RR_DCS_UL_UPPER_FREQ 1784800

#define RR_DCS_DL_LOWER_FREQ 1805200
#define RR_DCS_DL_UPPER_FREQ 1879800

#define RR_PCS_UL_LOWER_FREQ 1850200
#define RR_PCS_UL_UPPER_FREQ 1909800

#define RR_PCS_DL_LOWER_FREQ 1930200
#define RR_PCS_DL_UPPER_FREQ 1989800

#define RR_CHAN_SPACE 200

#define GSM_850_FREQ_OFFSET 45000
#define EGSM_FREQ_OFFSET 45000
#define PGSM_FREQ_OFFSET 45000
#define DCS_FREQ_OFFSET 95000
#define PCS_FREQ_OFFSET 80000

#define GSM_850_CHAN_OFFSET 128
#define PGSM_CHAN_OFFSET 0
#define EGSM_LOW_CHAN_OFFSET 0
#define EGSM_HIGH_CHAN_OFFSET 1024
#define DCS_CHAN_OFFSET 512
#define PCS_CHAN_OFFSET 512

#define GSM_850_LOWER_ARFCN 128
#define GSM_850_UPPER_ARFCN 251

#define EGSM_LOW_RANGE_LOWER_ARFCN 0
#define EGSM_LOW_RANGE_UPPER_ARFCN 124

#define EGSM_HIGH_RANGE_LOWER_ARFCN 975
#define EGSM_HIGH_RANGE_UPPER_ARFCN 1023

#define PGSM_LOWER_ARFCN 1
#define PGSM_UPPER_ARFCN 124

#define DCS_LOWER_ARFCN 512
#define DCS_UPPER_ARFCN 885

#define PCS_LOWER_ARFCN 512
#define PCS_UPPER_ARFCN 810

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/
typedef enum
{
  RR_BAND_AVOID_STATE_IDLE,
  RR_BAND_AVOID_STATE_AVOIDING
} rr_band_avoid_state_t;

typedef struct
{
  boolean                  list_active; /*indicates if blacklist band and ARFCN are valid*/
  sys_band_T               band; /*blacklisted band to be avoided (set from blacklisted channels that match ded_freq)*/
  uint16                   entries; /*number of ARFCN entries*/
  ARFCN_T                  arfcn[1024]; /*blacklisted ARFCN's - derived from CXM band avoid blacklist*/
} rr_band_avoid_blacklist_t;

typedef struct
{
  rr_band_avoid_state_t       state;
  rr_band_avoid_state_t       old_state;  
  uint8                       meas_rpt_count; /*number of meas rpts sent in Avoiding state*/
  uint16                      blacklist_id; /*Id received from CXM*/
  frequency_list_T            ded_freq; /*TCH frequencies used during a call*/  
  rr_band_avoid_blacklist_t   blacklist; /*blacklisted ARFCN's and band*/
} rr_band_avoid_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/
static rr_band_avoid_t rr_band_avoid[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/
static void rr_band_avoid_update_blacklist(coex_ba_blist_data *blist_ptr, const gas_id_t gas_id);
static ARFCN_T rr_get_arfcn(uint32 freq, sys_band_T band, const gas_id_t gas_id);
static void rr_add_arfcns_to_blacklist(uint16 lower_arfcn, uint16 upper_arfcn, sys_band_T band, const gas_id_t gas_id);

/*!
 * \brief Returns a string of the state name
 *
 * \param state
 *
 * \return const char *
 */
static const char * rr_band_avoid_state_name(rr_band_avoid_state_t state)
{
  switch (state)
  {
    case RR_BAND_AVOID_STATE_IDLE:
      return "IDLE";
    case RR_BAND_AVOID_STATE_AVOIDING:
      return "AVOIDING";
    default:
      return "?";
  }
}

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_band_avoid_t*
 */
static rr_band_avoid_t *rr_band_avoid_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_band_avoid[as_index];
}

/*!
 * \brief Set variables to start band avoidance
 *
 * \param bl_id - Blacklist Id - CXM uses this to ensure response is matched to request
 *        gas_id
 *
 * \return void
 */
static void rr_band_avoid_start_avoidance(const gas_id_t gas_id)
{
  rr_band_avoid_t *rr_band_avoid_ptr = rr_band_avoid_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(rr_band_avoid_ptr);

  rr_band_avoid_ptr->meas_rpt_count = 0;
}

/*!
 * \brief Checks if there available NCells that are not in the blacklisted band
 *
 * \param bl_band - Blacklisted band
 *        gas_id
 *
 * \return boolean - TRUE - ncells found, FALSE - ncells not found
 */
static boolean rr_band_avoid_valid_ncells(sys_band_T bl_band, const gas_id_t gas_id)
{
  uint8 ncells_not_bl = 0;
  uint8 i;
  surrounding_measurement_T *surrounding_measurement_ptr = rr_get_surrounding_measurement_ptr(gas_id);

  if (surrounding_measurement_ptr == NULL)
  {
    MSG_GERAN_HIGH_0_G("RR_BAsurrounding_measurement_ptr == NULL");
    return FALSE;
  }

  MSG_GERAN_HIGH_2_G("RR_BA search for ncells:%d not in band:%d",surrounding_measurement_ptr->no_of_entries, bl_band);

  for (i=0; i<surrounding_measurement_ptr->no_of_entries; i++)
  {
    MSG_GERAN_MED_3_G("cell:%d band:%d Rxlev:%d",surrounding_measurement_ptr->measurement[i].BCCH_ARFCN.num,
                                         surrounding_measurement_ptr->measurement[i].BCCH_ARFCN.band,
                                         surrounding_measurement_ptr->measurement[i].RXLEV_average);

    if (surrounding_measurement_ptr->measurement[i].BCCH_ARFCN.band != bl_band &&
        surrounding_measurement_ptr->measurement[i].RXLEV_average > 
        rr_get_band_avoid_ncell_threshold(RR_GAS_ID_TO_AS_ID))
    {
      ncells_not_bl++;
    }
  }

  MSG_GERAN_HIGH_1_G("RR_BA found %d suitable ncells",ncells_not_bl);

  if (ncells_not_bl >= 2) /*at least two non blacklisted cells need to be available in order to start avoidance*/
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
 * \brief Checks if any of current TCH's are in the blacklist and returns the band that is to be avoided
 *
 * \param band - return band that is blacklisted
 *        ded_freq - TCH's of dedicated connection
 *        gas_id
 *
 * \return boolean TRUE- blacklisted TCH's found (band is valid), FALSE - blacklisted TCH's not found (band is not set)
 */
static boolean rr_band_avoid_is_ded_freq_blacklisted(sys_band_T *band, frequency_list_T *ded_freq, const gas_id_t gas_id)
{
  uint16 i, j = 0;
  boolean blacklisted = FALSE;
  rr_band_avoid_t *rr_band_avoid_ptr = rr_band_avoid_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(rr_band_avoid_ptr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(band, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(ded_freq, FALSE);

  for (i=0; i<ded_freq->no_of_items; i++)
  {
    for (j=0; j<rr_band_avoid_ptr->blacklist.entries; j++)
    {
      if (ARFCNS_EQUAL(ded_freq->channel[i], rr_band_avoid_ptr->blacklist.arfcn[j]) )
      {
        MSG_GERAN_HIGH_2_G("RR_BA Blacklisted cell found ARFCN:%d, band:%d",ded_freq->channel[i].num, ded_freq->channel[i].band);
        *band = ded_freq->channel[i].band;
        blacklisted = TRUE;
        break;
      }
    }
  }

  return blacklisted;
}

/*!
 * \brief Convert sys band class type to internal band type
 *
 * \param band
 *        gas_id
 *
 * \return sys_band_T
 */
static sys_band_T rr_band_avoid_sys_band_to_internal_band(sys_band_class_e_type band, const gas_id_t gas_id)
{
   switch(band)
   {
     case SYS_BAND_GSM_DCS_1800:
        return SYS_BAND_DCS_1800;
        break;
     case SYS_BAND_GSM_PCS_1900:
        return SYS_BAND_PCS_1900;
        break;
     case SYS_BAND_GSM_850:
        return SYS_BAND_CELL_850;
        break;
     case SYS_BAND_GSM_PGSM_900:
        return SYS_BAND_PGSM_900;
        break;
     case SYS_BAND_GSM_EGSM_900:
        return SYS_BAND_EGSM_900;
        break;
     default:
        MSG_GERAN_ERROR_1_G("RR_BA Unsupported RF band: %d", band);
        return SYS_BAND_NONE;
        break;
   }
}

/*!
 * \brief Return frequency range of requested band
 *
 * \param band
 *        *ul_lower_freq
 *        *dl_lower_freq
 *        *ul_upper_freq
 *        *dl_upper_freq
 *         band
 *
 * \return void
 */
static void rr_get_frequency_range(uint32 *ul_lower_freq, uint32 *ul_upper_freq,
                                   uint32 *dl_lower_freq, uint32 *dl_upper_freq,
                                   sys_band_T band)
{
  switch (band)
  {
    case SYS_BAND_EGSM_900:
      *ul_lower_freq = RR_EGSM_HIGH_CHANS_UL_LOWER_FREQ;
      *ul_upper_freq = RR_EGSM_LOW_CHANS_UL_UPPER_FREQ;
      *dl_lower_freq = RR_EGSM_HIGH_CHANS_DL_LOWER_FREQ;
      *dl_upper_freq = RR_EGSM_LOW_CHANS_DL_UPPER_FREQ;
    break;

    case SYS_BAND_PGSM_900:
      *ul_lower_freq = RR_PGSM_UL_LOWER_FREQ + RR_CHAN_SPACE;/*channel 0 shouldn't be used*/
      *ul_upper_freq = RR_PGSM_UL_UPPER_FREQ;
      *dl_lower_freq = RR_PGSM_DL_LOWER_FREQ + RR_CHAN_SPACE;/*channel 0 shouldn't be used*/;
      *dl_upper_freq = RR_PGSM_DL_UPPER_FREQ;
    break;

    case SYS_BAND_PCS_1900:
      *ul_lower_freq = RR_PCS_UL_LOWER_FREQ;
      *ul_upper_freq = RR_PCS_UL_UPPER_FREQ;
      *dl_lower_freq = RR_PCS_DL_LOWER_FREQ;
      *dl_upper_freq = RR_PCS_DL_UPPER_FREQ;
    break;

    case SYS_BAND_DCS_1800:
      *ul_lower_freq = RR_DCS_UL_LOWER_FREQ;
      *ul_upper_freq = RR_DCS_UL_UPPER_FREQ;
      *dl_lower_freq = RR_DCS_DL_LOWER_FREQ;
      *dl_upper_freq = RR_DCS_DL_UPPER_FREQ;
    break;

    case SYS_BAND_CELL_850:
      *ul_lower_freq = RR_850_UL_LOWER_FREQ;
      *ul_upper_freq = RR_850_UL_UPPER_FREQ;
      *dl_lower_freq = RR_850_DL_LOWER_FREQ;
      *dl_upper_freq = RR_850_DL_UPPER_FREQ;
    break;

    default:
      MSG_GERAN_ERROR_0("rr_get_frequency_range - Invalid band");
      break;
  }
}

/*!
 * \brief Checks frequency range is within the requested band. Truncates
 *        the frequency to the band limits. 
 *
 * \param band
 *        *ul_lower_freq
 *        *ul_upper_freq
 *         band
 *
 * \return void
 */
static void rr_band_avoid_check_frequencies(uint32 *lower_freq, uint32 *upper_freq, sys_band_T band, const gas_id_t gas_id)
{
  uint32 ul_lower_freq_range = 0;
  uint32 ul_upper_freq_range = 0;
  uint32 dl_lower_freq_range = 0;
  uint32 dl_upper_freq_range = 0;

  rr_get_frequency_range(&ul_lower_freq_range, &ul_upper_freq_range, &dl_lower_freq_range, &dl_upper_freq_range, band);

  /******sanity check*******/
  /*lowest frequency is greater than highest frequency in the band so the supplied frequency is completely out 
    of band*/
  if (*lower_freq > dl_upper_freq_range)
  {
    MSG_GERAN_ERROR_3_G("RR_BA lower_freq:%d out of band range:%d, band:%d", *lower_freq, dl_upper_freq_range, band);
    return;
  }

  /*highest frequency is less than lowest frequency in the band so the supplied frequency is completely out 
    of band*/
  if (*upper_freq < ul_lower_freq_range)
  {
    MSG_GERAN_ERROR_3_G("RR_BA upper_freq:%d out of band range:%d, band:%d", *lower_freq, ul_lower_freq_range, band);
    return;
  }

  /*supplied frequency range is within band offset region and is completely out of band*/
  if ( (*lower_freq > ul_upper_freq_range) &&
       (*upper_freq < dl_lower_freq_range) )
  {
    MSG_GERAN_ERROR_3_G("RR_BA lower_freq:%d upper_freq:%d, band:%d out of range", *lower_freq, *upper_freq, band);
    return;
  }

  /***** truncate frequency range to within band range******/
  /*lower frequency is less than lowest band frequency, truncate to lowest band frequency*/
  if (*lower_freq < ul_lower_freq_range)
  {
    MSG_GERAN_HIGH_2_G("RR_BA truncate lower freq:%d to %d",*lower_freq, ul_lower_freq_range);
    *lower_freq = ul_lower_freq_range;
  }

  /*upper frequency is greater than highest band frequency, truncate to highest band frequency*/
  if (*upper_freq > dl_upper_freq_range)
  {
    MSG_GERAN_HIGH_2_G("RR_BA truncate upper freq:%d to %d",*upper_freq, dl_upper_freq_range);
    *upper_freq = dl_upper_freq_range;
  }

  /*lower frequency is out of range in offset region, truncate to DL lowest frequency*/
  if ( (*lower_freq > ul_upper_freq_range) && 
       (*lower_freq < dl_lower_freq_range) )
  {
    MSG_GERAN_HIGH_2_G("RR_BA truncate lower freq:%d to %d",*lower_freq, dl_lower_freq_range);
    *lower_freq = dl_lower_freq_range;
  }

  /*upper frequency is out of range in offset region, truncate to UL highest frequency*/
  if ( (*upper_freq > ul_upper_freq_range) && 
       (*upper_freq < dl_lower_freq_range) )
  {
    MSG_GERAN_HIGH_2_G("RR_BA truncate upper freq:%d to %d",*upper_freq, ul_upper_freq_range);
    *upper_freq = ul_upper_freq_range;
  }
}

/* \brief Handle Black list indication from CXM. Convert lower, upper frequencies to ARFCN's and
 *        add entire ARFCN range to internal blacklist store.
 *
 * \param blist_ptr - blacklist from CXM
 *        gas_id
 *
 * \return void
 */
static void rr_band_avoid_update_blacklist(coex_ba_blist_data *blist_ptr, const gas_id_t gas_id)
{
  ARFCN_T lower_freq_arfcn;
  ARFCN_T upper_freq_arfcn;
  uint16 lower_arfcn, upper_arfcn = 0; 
  uint16  i;
  rr_band_avoid_t *rr_band_avoid_ptr = rr_band_avoid_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(blist_ptr);
  RR_NULL_CHECK_RETURN_VOID(rr_band_avoid_ptr);

  rr_band_avoid_ptr->blacklist.entries = 0;
  rr_band_avoid_ptr->blacklist_id = blist_ptr->blacklist_id;

  if (blist_ptr == NULL)
  {
    rr_band_avoid_ptr->blacklist.list_active = FALSE;
    return;
  }
  else
  {
    rr_band_avoid_ptr->blacklist.list_active = TRUE;
  }

  if (blist_ptr->num_entries > NELEMS(blist_ptr->freq_list))
  {
    blist_ptr->num_entries = NELEMS(blist_ptr->freq_list);
  }

  for (i=0; i < blist_ptr->num_entries; i++)
  {
    /*map the supplied band in to the internal value*/
    blist_ptr->freq_list[i].band = rr_band_avoid_sys_band_to_internal_band(blist_ptr->freq_list[i].band, gas_id);

    /*if CXM provides full frequency range: lower freq =0, upper freq = 4294967295 (max for uint32) then entire band is blacklisted*/
    if (blist_ptr->freq_list[i].freq_lower == 0 &&
        blist_ptr->freq_list[i].freq_upper == 0xffffffff)
    {
      MSG_GERAN_HIGH_1_G("RR_BA Add whole band:%d range to blacklist", blist_ptr->freq_list[i].band);

      switch (blist_ptr->freq_list[i].band)
      {
        case SYS_BAND_EGSM_900:
          rr_add_arfcns_to_blacklist(EGSM_LOW_RANGE_LOWER_ARFCN, EGSM_LOW_RANGE_UPPER_ARFCN, SYS_BAND_EGSM_900, gas_id);
          rr_add_arfcns_to_blacklist(EGSM_HIGH_RANGE_LOWER_ARFCN, EGSM_HIGH_RANGE_UPPER_ARFCN, SYS_BAND_EGSM_900, gas_id);
        break;

        case SYS_BAND_PGSM_900:
          rr_add_arfcns_to_blacklist(PGSM_LOWER_ARFCN, PGSM_UPPER_ARFCN, SYS_BAND_PGSM_900, gas_id);
        break;

        case SYS_BAND_PCS_1900:
          rr_add_arfcns_to_blacklist(PCS_LOWER_ARFCN, PCS_UPPER_ARFCN, SYS_BAND_PCS_1900, gas_id);
        break;

        case SYS_BAND_DCS_1800:
          rr_add_arfcns_to_blacklist(DCS_LOWER_ARFCN, DCS_UPPER_ARFCN, SYS_BAND_DCS_1800, gas_id);
        break;

        case SYS_BAND_CELL_850:
          rr_add_arfcns_to_blacklist(GSM_850_LOWER_ARFCN, GSM_850_UPPER_ARFCN, SYS_BAND_CELL_850, gas_id);
        break;
      }
    }
    else /*only blacklist ARFCN's within specified band*/
    {
      MSG_GERAN_HIGH_3_G("RR_BA Add band:%d subset to blacklist, lower freq:%dKHz, upper freq:%dKHz", 
                           blist_ptr->freq_list[i].band, 
                           blist_ptr->freq_list[i].freq_lower, 
                           blist_ptr->freq_list[i].freq_upper);

      /*sanitize frequencies, truncate so they are within the specified band*/
      rr_band_avoid_check_frequencies(&blist_ptr->freq_list[i].freq_lower,
                                      &blist_ptr->freq_list[i].freq_upper,
                                       blist_ptr->freq_list[i].band,
                                       gas_id);

      /*get ARFCN of lower frequency*/
      lower_freq_arfcn = rr_get_arfcn(blist_ptr->freq_list[i].freq_lower, blist_ptr->freq_list[i].band, gas_id);

      /*get ARFCN of upper frequency*/
      upper_freq_arfcn = rr_get_arfcn(blist_ptr->freq_list[i].freq_upper, blist_ptr->freq_list[i].band, gas_id);

      if (lower_freq_arfcn.band != INVALID_BAND &&
          upper_freq_arfcn.band != INVALID_BAND)
      {
        /*CXM can provide a frequency range where upper frequency can have a lower ARFCN number than the lower frequency*/
        /*order ARFCN's lower -> higher for inclusion in the blacklist*/
        lower_arfcn = MIN(lower_freq_arfcn.num,upper_freq_arfcn.num);
        upper_arfcn = MAX(lower_freq_arfcn.num,upper_freq_arfcn.num);

        if (blist_ptr->freq_list[i].band == SYS_BAND_EGSM_900)
        {
          /*EGSM is a special case because range of ARFCN's is 0..124, 975..1023*/

          /* lower and upper ARFCN are in different ARFCN ranges(0..124, 975..1023) add them separately*/
          if (lower_arfcn <= EGSM_LOW_RANGE_UPPER_ARFCN &&
              upper_arfcn >= EGSM_HIGH_RANGE_LOWER_ARFCN)
          {
            rr_add_arfcns_to_blacklist(EGSM_LOW_RANGE_LOWER_ARFCN, lower_arfcn, blist_ptr->freq_list[i].band, gas_id);
            rr_add_arfcns_to_blacklist(upper_arfcn, EGSM_HIGH_RANGE_UPPER_ARFCN, blist_ptr->freq_list[i].band, gas_id);
          }
          else
          {
            /*lower and upper ARFCN are in same range*/
            rr_add_arfcns_to_blacklist(lower_arfcn, upper_arfcn, blist_ptr->freq_list[i].band, gas_id);
          }
        }
        else /*all other bands have single range*/
        {
          rr_add_arfcns_to_blacklist(lower_arfcn, upper_arfcn, blist_ptr->freq_list[i].band, gas_id);
        }
      }
      else
      {
        MSG_GERAN_ERROR_2_G("RR_BA can't convert to ARFCN ignore, lower ARFCN band:%d, upper ARFCN band:%d", lower_freq_arfcn.band, upper_freq_arfcn.band);
      }
    }
  }
}

/* \brief Adds and ARFCN range to the blacklist. Checks for duplicates.
 *
 * \param lower_arfcn
 *        upper_arfcn
 *        band
 *        gas_id
 *
 * \return void
 */
static void rr_add_arfcns_to_blacklist(uint16 lower_arfcn, uint16 upper_arfcn, sys_band_T band, const gas_id_t gas_id)
{
  ARFCN_T  arfcn_index;
  uint16  i;
  boolean skip;
  uint16 num_arfcns_added = 0;
  rr_band_avoid_t *rr_band_avoid_ptr = rr_band_avoid_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(rr_band_avoid_ptr);

  arfcn_index.band = band;

  MSG_GERAN_HIGH_3_G("RR_BA Adding ARFCN's %d to %d, band:%d to blacklist", lower_arfcn, upper_arfcn, band);
  
  /*add ARFCN range to blacklist*/
  for (arfcn_index.num = lower_arfcn; (arfcn_index.num <= upper_arfcn) && (rr_band_avoid_ptr->blacklist.entries < NELEMS(rr_band_avoid_ptr->blacklist.arfcn)); arfcn_index.num++)
  {
    skip = FALSE;

    /*check there are no duplicates*/
    for (i=0; i<rr_band_avoid_ptr->blacklist.entries; i++)
    {
      if (ARFCNS_EQUAL(rr_band_avoid_ptr->blacklist.arfcn[i], arfcn_index))
      { 
        skip = TRUE;
      }
    }

    /*only add ARFCN if it is not in the list already*/
    if (!skip)
    {
      rr_band_avoid_ptr->blacklist.arfcn[rr_band_avoid_ptr->blacklist.entries].num = arfcn_index.num; 
      rr_band_avoid_ptr->blacklist.arfcn[rr_band_avoid_ptr->blacklist.entries].band = arfcn_index.band;
      rr_band_avoid_ptr->blacklist.entries++;
      num_arfcns_added++;
    
      if (rr_band_avoid_ptr->blacklist.entries == NELEMS(rr_band_avoid_ptr->blacklist.arfcn))
      {
        MSG_GERAN_ERROR_0_G("RR_BA band avoid blacklist full");
      }
    }
  }

  MSG_GERAN_HIGH_2_G("RR_BA num of entries: %d, num ARFCN's added:%d", rr_band_avoid_ptr->blacklist.entries,num_arfcns_added);
}

/*!
 * \brief Convert a frequency in to an ARFCN. Handles UL and DL frequencies.
 *        See ETSI TS 45.005 section 2.
 *
 * \param freq - in KHz
 *        band - band of frequency
 *        gas_id
 *
 * \return ARFCN_T
 */
static ARFCN_T rr_get_arfcn(uint32 freq, sys_band_T band, const gas_id_t gas_id)
{
  boolean freq_in_band = FALSE;
  uint32 base_freq = 0;
  ARFCN_T arfcn;
  uint16 arfcn_chan_offset = 0;
  boolean arfcn_out_of_range = FALSE;

  arfcn.band = INVALID_BAND;
  arfcn.num = 0;

  /*find band we are using*/
  switch (band)
  {
    case SYS_BAND_CELL_850:
    {
      if ( (freq >= RR_850_UL_LOWER_FREQ &&
            freq <= RR_850_UL_UPPER_FREQ) ||
           (freq >= RR_850_DL_LOWER_FREQ &&
            freq <= RR_850_DL_UPPER_FREQ) )
      {
        freq_in_band = TRUE;
        base_freq = RR_850_UL_LOWER_FREQ;
        arfcn_chan_offset = GSM_850_CHAN_OFFSET;
      }

      if (freq >= RR_850_DL_LOWER_FREQ)
      {
        freq -= GSM_850_FREQ_OFFSET;
      }
      break;
    }

    case SYS_BAND_EGSM_900:
    {
      if ( (freq >= RR_EGSM_HIGH_CHANS_UL_LOWER_FREQ &&
            freq <= RR_EGSM_LOW_CHANS_UL_UPPER_FREQ) ||
           (freq >= RR_EGSM_HIGH_CHANS_DL_LOWER_FREQ &&
            freq <= RR_EGSM_LOW_CHANS_DL_UPPER_FREQ) )
      {
        freq_in_band = TRUE;
        base_freq = RR_EGSM_LOW_CHANS_UL_LOWER_FREQ;
      }

      if (freq >= RR_EGSM_HIGH_CHANS_DL_LOWER_FREQ)
      {
        freq -= EGSM_FREQ_OFFSET;
      }

      if (freq <= RR_EGSM_HIGH_CHANS_UL_UPPER_FREQ)
      {
        arfcn_chan_offset = EGSM_HIGH_CHAN_OFFSET;
      }
      else
      {
        arfcn_chan_offset = EGSM_LOW_CHAN_OFFSET;
      }      
      break;
    }    

    case SYS_BAND_PGSM_900:
    {
      if ( (freq >= RR_PGSM_UL_LOWER_FREQ &&
            freq <= RR_PGSM_UL_UPPER_FREQ) ||
           (freq >= RR_PGSM_DL_LOWER_FREQ &&
            freq <= RR_PGSM_DL_UPPER_FREQ))
      {
        freq_in_band = TRUE;
        arfcn_chan_offset = PGSM_CHAN_OFFSET;
        base_freq = RR_PGSM_UL_LOWER_FREQ;
      }

      if (freq >= RR_PGSM_DL_LOWER_FREQ)
      {
        freq -= PGSM_FREQ_OFFSET;
      }
      break;
    }

    case SYS_BAND_DCS_1800:
    {
      if ( (freq >= RR_DCS_UL_LOWER_FREQ &&
            freq <= RR_DCS_UL_UPPER_FREQ) ||
           (freq >= RR_DCS_DL_LOWER_FREQ &&
            freq <= RR_DCS_DL_UPPER_FREQ) )
      {
        freq_in_band = TRUE;
        base_freq = RR_DCS_UL_LOWER_FREQ;
        arfcn_chan_offset = DCS_CHAN_OFFSET;
      }

      if (freq >= RR_DCS_DL_LOWER_FREQ)
      {
        freq -= DCS_FREQ_OFFSET;
      }
      break;
    }

    case SYS_BAND_PCS_1900:
    {
      if ( (freq >= RR_PCS_UL_LOWER_FREQ &&
            freq <= RR_PCS_UL_UPPER_FREQ) ||
           (freq >= RR_PCS_DL_LOWER_FREQ &&
            freq <= RR_PCS_DL_UPPER_FREQ) )
      {
        freq_in_band = TRUE;
        base_freq = RR_PCS_UL_LOWER_FREQ;
        arfcn_chan_offset = PCS_CHAN_OFFSET;
      }

      if (freq >= RR_PCS_DL_LOWER_FREQ)
      {
        freq -= PCS_FREQ_OFFSET;
      }
      break;
    }
    default:
      MSG_GERAN_ERROR_0("rr_get_arfcn - Invalid band");
      break;
  }

  if (freq_in_band)
  {
    arfcn.num = (uint16)( ( ((int32)freq - (int32)base_freq) / RR_CHAN_SPACE) + arfcn_chan_offset);

    arfcn.band = band;
  }
  
  /*sanity check ARFCN range*/
  switch (band)
  {
    case SYS_BAND_EGSM_900:
    {
      if ((arfcn.num > EGSM_LOW_RANGE_UPPER_ARFCN &&
           arfcn.num < EGSM_HIGH_RANGE_LOWER_ARFCN) ||
          arfcn.num > EGSM_HIGH_RANGE_UPPER_ARFCN)
      {
        arfcn_out_of_range = TRUE;     
      }
      break;
    }

    case SYS_BAND_PGSM_900:
    {
      if (arfcn.num < PGSM_LOWER_ARFCN ||
          arfcn.num > PGSM_UPPER_ARFCN)
      {
        arfcn_out_of_range = TRUE;     
      }
      break;
    }

    case SYS_BAND_PCS_1900:
    {
      if (arfcn.num < PCS_LOWER_ARFCN ||
          arfcn.num > PCS_UPPER_ARFCN)
      {
        arfcn_out_of_range = TRUE;     
      }
      break;
    }

    case SYS_BAND_DCS_1800:
    {
      if (arfcn.num < DCS_LOWER_ARFCN ||
          arfcn.num > DCS_UPPER_ARFCN)
      {
        arfcn_out_of_range = TRUE;     
      }
      break;
    }

    case SYS_BAND_CELL_850:
    {
      if (arfcn.num < GSM_850_LOWER_ARFCN ||
          arfcn.num > GSM_850_UPPER_ARFCN)
      {
        arfcn_out_of_range = TRUE;     
      }
      break;
    }
    default:
      MSG_GERAN_ERROR_0("rr_get_arfcn - Invalid Band");
      break;
  }

  if (arfcn_out_of_range)
  {
    MSG_GERAN_ERROR_2_G("RR_BA out of range ARFCN:%d, band:%d, invalidate",arfcn.num, arfcn.band);
    arfcn.num = 0;
    arfcn.band = INVALID_BAND;
  }

  return arfcn;
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/
/*!
 * \brief Reduce the rx lev of blacklisted neighbours(in blacklisted band) in the meas rept to at least 
 *        N below the lowest non blacklisted cell. If actively avoiding also reduce the scell to at least 
 *        N below the lowest non blacklisted cell. 
 *
 * \param *gsm_measurements - ncell measurements (modified return param)
 *        serving_cell_arfcn
 *        *rx_lev - scell rxlev (modified return param)
 *        *rx_lev_optional - scell rxlev, secondary measurement (modified return param)
 *        *rx_qual
 *        *rx_qual_optional
 *        *MEAN_BEP
 *        *CV_BEP
 *        *NBR_RCVD_BLOCKS
 *         gas_id
 * \return void
 */
void rr_band_avoid_modify_gsm_meas_rpt(
               surrounding_measurement_T * gsm_measurements,
               ARFCN_T                   serving_cell_arfcn,
               uint8                     * rx_lev,
               uint8                     * rx_lev_optional,
               uint8                     * rx_qual,
               uint8                     * rx_qual_optional,
               uint8                     * mean_bep,
               uint8                     * cv_bep,
               uint8                     * nbr_rcvd_blocks,
               const gas_id_t              gas_id)
{
  uint8 i;
  rr_band_avoid_t *rr_band_avoid_ptr = rr_band_avoid_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(gsm_measurements);
  RR_NULL_CHECK_RETURN_VOID(rx_lev);
  RR_NULL_CHECK_RETURN_VOID(rx_qual);

  if (rr_band_avoid_ptr->blacklist.list_active)
  {
    /* Reduce levels of reported neighbours and scell */
    
    if (rr_band_avoid_ptr->state == RR_BAND_AVOID_STATE_AVOIDING)
    {
      byte saved_rxlev = *rx_lev;

      for (i=0; i<gsm_measurements->no_of_entries; i++)
      {
        /*blacklisted cells are those in the blacklisted band*/
        if (gsm_measurements->measurement[i].BCCH_ARFCN.band == serving_cell_arfcn.band)
        {
          byte saved_rxlev = gsm_measurements->measurement[i].RXLEV_average;

          gsm_measurements->measurement[i].RXLEV_average = 0;
           
          MSG_GERAN_HIGH_3_G("RR_BA Reduced Ncell ARFCN %d Rxlev from %d to %d",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                              saved_rxlev,
                                                                              gsm_measurements->measurement[i].RXLEV_average);
        }
        else
        {
          MSG_GERAN_HIGH_2_G("RR_BA Ncell ARFCN %d unchanged Rxlev %d",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                       gsm_measurements->measurement[i].RXLEV_average);
        }
      }

      *rx_lev = 0;

      MSG_GERAN_HIGH_3_G("RR_BA Reduced Scell ARFCN %d Rxlev from %d to %d",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                            saved_rxlev,
                                                                           *rx_lev);

      if (rx_lev_optional != NULL)
      {
        byte saved_opt_rxlev = *rx_lev_optional;

        *rx_lev_optional = 0;

        MSG_GERAN_HIGH_3_G("RR_BA Reduced Scell ARFCN %d Rxlev opt from %d to %d",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                                  saved_opt_rxlev,
                                                                                 *rx_lev_optional);
      }

      MSG_GERAN_HIGH_2_G("RR_BA Reduced Scell ARFCN %d Rxqual from %d to 0",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                           *rx_qual);
      *rx_qual = 7;

      if (rx_qual_optional != NULL)
      {
        MSG_GERAN_HIGH_2_G("RR_BA Reduced Scell ARFCN %d Rxqual opt from %d to 0",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                                 *rx_qual_optional);
        *rx_qual_optional = 7;
      }

      if(mean_bep != NULL)
      {
        MSG_GERAN_HIGH_2_G("RR_BA Reduced Scell ARFCN %d MEAN BEP from %d to 0",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                                 *mean_bep);
        *mean_bep = 0;
      }

      if(cv_bep != NULL)
      {
        MSG_GERAN_HIGH_2_G("RR_BA Reduced Scell ARFCN %d CV BEP from %d to 4",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                                 *cv_bep);
        *cv_bep = 4;
      }

      if(mean_bep != NULL)
      {
        MSG_GERAN_HIGH_2_G("RR_BA Reduced Scell ARFCN %d NBR RCVD BLOCKS from %d to 2",gsm_measurements->measurement[i].BCCH_ARFCN.num,
                                                                                 *nbr_rcvd_blocks);
        *nbr_rcvd_blocks = 2;
      }

    }
  }
}

/*!
 * \brief Band Avoidance State Machine
 *
 * \param message
 *        gas_id
 *
 * \return void
 */
void rr_band_avoidance_control(rr_cmd_bdy_type * message, const gas_id_t gas_id)
{
  rr_band_avoid_t *rr_band_avoid_ptr = rr_band_avoid_get_data_ptr(gas_id);
  const sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  /* Validate module-global data */
  RR_NULL_CHECK_RETURN_VOID(rr_band_avoid_ptr);

  /* Validate input */
  RR_NULL_CHECK_RETURN_VOID(message);

  /****** print debug*********/
  if (message->message_header.message_set == MS_MSGR_RR &&
      (message->rr_msgr_msg.message.hdr.id == MCS_CXM_BAND_AVOID_BLIST_GSM1_IND ||
       message->rr_msgr_msg.message.hdr.id == MCS_CXM_BAND_AVOID_BLIST_GSM2_IND) )
  {
    uint8 i = 0;

    MSG_GERAN_HIGH_3_G("RR_BA Rxd MCS_CXM_BAND_AVOID_BLIST_GSM_IND blist_id:%d  serv_freq_state:%d band avoid state:%d",
                          message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.blacklist_id,
                          message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.serv_freq_state,
                          rr_band_avoid_ptr->state);

    for (i=0;i<message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.num_entries;i++)
    {
      MSG_GERAN_HIGH_3_G("RR_BA band:%d, lower freq:%d, upper freq%d",
                             message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.freq_list[i].band,
                             message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.freq_list[i].freq_lower,
                             message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.freq_list[i].freq_upper);
    }
  }

  if (message->rr.header.rr_message_set == RR_BAND_AVOID_IMSG &&
      message->message_header.message_id == RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND)
  {
    uint8 i=0;

    MSG_GERAN_HIGH_2_G("RR_BA Rxd RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND no items:%d, band avoid state:%d", 
                          message->rr.band_avoid.imsg.ded_freq_upd_ind.freq.no_of_items,
                          rr_band_avoid_ptr->state);

    for (i=0;i<message->rr.band_avoid.imsg.ded_freq_upd_ind.freq.no_of_items;i++)
    {
      MSG_GERAN_HIGH_2_G("RR_BA ARFCN num:%d band:%d",
                             message->rr.band_avoid.imsg.ded_freq_upd_ind.freq.channel[i].num,
                             message->rr.band_avoid.imsg.ded_freq_upd_ind.freq.channel[i].band);
    }

  }

  if (message->rr.header.rr_message_set == RR_BAND_AVOID_IMSG &&
      message->message_header.message_id == RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND)
  {
    MSG_GERAN_HIGH_2_G("RR_BA Rxd RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND band avoid state:%d, meas rpt counter:%d",
                             rr_band_avoid_ptr->state,
                             rr_band_avoid_ptr->meas_rpt_count);
  }

  if (rr_is_band_avoidance_enabled(as_id) == FALSE)
  {
    if (message->message_header.message_set == MS_MSGR_RR &&
        message->rr_msgr_msg.message.hdr.id == MCS_CXM_BAND_AVOID_BLIST_GSM1_IND)
    {
      MSG_GERAN_HIGH_0_G("RR_BA Band avoidance disabled send NACK"); 
      rr_band_avoid_send_cxm_blist_rsp(message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.blacklist_id, gas_id);
    }
    return;
  }

  switch (rr_band_avoid_ptr->state)
  {
    case RR_BAND_AVOID_STATE_IDLE:
    {
      switch (message->message_header.message_set)
      {
        case MS_MSGR_RR:
        {
          switch (message->rr_msgr_msg.message.hdr.id)
          {
            case MCS_CXM_BAND_AVOID_BLIST_GSM1_IND:
            case MCS_CXM_BAND_AVOID_BLIST_GSM2_IND:
            {
              if (message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.serv_freq_state == SERV_FREQ_AVOID_REQ)
              {
                rr_band_avoid_update_blacklist(&message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data, gas_id);

                if (rr_band_avoid_is_ded_freq_blacklisted(&rr_band_avoid_ptr->blacklist.band, &rr_band_avoid_ptr->ded_freq, gas_id))
                {

                  if (rr_band_avoid_valid_ncells(rr_band_avoid_ptr->blacklist.band, gas_id))
                  {
                    MSG_GERAN_HIGH_0_G("RR_BA Current TCH is in blacklist, Ncells available, start avoiding");
                    rr_band_avoid_start_avoidance(gas_id);
                    rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_AVOIDING;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("RR_BA Current TCH is in blacklist, no available Ncells");
                    /*can't avoid, no Ncells available*/
                    rr_band_avoid_send_cxm_blist_rsp(rr_band_avoid_ptr->blacklist_id, gas_id);
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("RR_BA Current TCH is not in blacklist");
                  /*current TCH is not blacklisted, send freq list*/
                  rr_band_avoid_send_cxm_freq_ind(&rr_band_avoid_ptr->ded_freq, gas_id);
                }
              }
              else if (message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.serv_freq_state == SERV_FREQ_OK)
              {
                MSG_GERAN_HIGH_0_G("RR_BA SERV_FREQ_OK - no action");
                rr_band_avoid_update_blacklist(NULL, gas_id);
              }
              break;
            }

            default:
              LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
            break;
          }
          break;
        }

        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_BAND_AVOID_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND:
                {
                  rr_band_avoid_ptr->ded_freq = message->rr.band_avoid.imsg.ded_freq_upd_ind.freq;
                  rr_band_avoid_send_cxm_freq_ind(&rr_band_avoid_ptr->ded_freq, gas_id);
                  break;
                }

                case RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND:
                { 
                  /*ignore*/
                  break;
                }

                default:
                  LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
                break;
              }
              break;
            }
            default:
              LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
            break;
          }
          break; 
        }
        default:
          LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
        break;
      }
      break;
    }

    case RR_BAND_AVOID_STATE_AVOIDING:
    {
      switch (message->message_header.message_set)
      {
        case MS_MSGR_RR:
        {
          switch (message->rr_msgr_msg.message.hdr.id)
          {
            case MCS_CXM_BAND_AVOID_BLIST_GSM1_IND:
            case MCS_CXM_BAND_AVOID_BLIST_GSM2_IND:
            {
              if (message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.serv_freq_state == SERV_FREQ_AVOID_REQ)
              {
                MSG_GERAN_HIGH_0_G("RR_BA Blist Ind received when avoiding, assess new list");

                rr_band_avoid_update_blacklist(&message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data, gas_id);

                if (rr_band_avoid_is_ded_freq_blacklisted(&rr_band_avoid_ptr->blacklist.band, &rr_band_avoid_ptr->ded_freq, gas_id))
                {
                  if (rr_band_avoid_valid_ncells(rr_band_avoid_ptr->blacklist.band, gas_id))
                  {
                    MSG_GERAN_HIGH_0_G("RR_BA Current TCH is in blacklist, Ncells available, continue avoiding");
                    rr_band_avoid_start_avoidance(gas_id);
                    rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_AVOIDING;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("RR_BA Current TCH is in blacklist, no available Ncells");
                    /*can't avoid, no Ncells available*/
                    rr_band_avoid_send_cxm_blist_rsp(rr_band_avoid_ptr->blacklist_id, gas_id);
                    rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_IDLE;
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("RR_BA Current TCH is not in blacklist");
                  /*current TCH is not blacklisted, send freq list*/
                  rr_band_avoid_send_cxm_freq_ind(&rr_band_avoid_ptr->ded_freq, gas_id);
                  rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_IDLE;
                }
              }
              else if (message->rr_msgr_msg.message.band_avoid_blacklist_ind.blacklist_data.serv_freq_state == SERV_FREQ_OK)
              {
                MSG_GERAN_HIGH_0_G("RR_BA SERV_FREQ_OK - no action");
                rr_band_avoid_update_blacklist(NULL, gas_id);
                rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_IDLE;
              }
              break;
            }

            default:
              LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
            break;
          }
          break;
        }
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_BAND_AVOID_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_BAND_AVOID_IMSG_DED_FREQ_UPD_IND:
                {
                  rr_band_avoid_ptr->ded_freq = message->rr.band_avoid.imsg.ded_freq_upd_ind.freq;
                
                  if ( rr_band_avoid_is_ded_freq_blacklisted(&rr_band_avoid_ptr->blacklist.band, &rr_band_avoid_ptr->ded_freq, gas_id) )
                  {
                    MSG_GERAN_HIGH_0_G("RR_BA Blacklisted cells in TCH after frequency update, send NACK");
                    /*avoidance didn't work, still have blacklisted cells, send NACK*/
                    rr_band_avoid_send_cxm_blist_rsp(rr_band_avoid_ptr->blacklist_id, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("RR_BA Blacklisted cells avoided after frequency update");
                    /*avoidance worked send the updated frequency list*/
                    rr_band_avoid_send_cxm_freq_ind(&rr_band_avoid_ptr->ded_freq, gas_id);
                  }
                  rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_IDLE;
                  break;
                }

                case RR_BAND_AVOID_IMSG_MEAS_RPT_SENT_IND:
                { 
                  rr_band_avoid_ptr->meas_rpt_count++;

                  if (rr_band_avoid_ptr->meas_rpt_count > rr_get_band_avoid_max_meas_rpts(as_id))
                  {
                    MSG_GERAN_HIGH_1_G("RR_BA Max number of Meas Rpts (%d) sent, send NACK", rr_band_avoid_ptr->meas_rpt_count);
                    /*NW did not HO UE to another cell, send NACK*/
                    rr_band_avoid_send_cxm_blist_rsp(rr_band_avoid_ptr->blacklist_id, gas_id);
                    rr_band_avoid_ptr->state = RR_BAND_AVOID_STATE_IDLE;
                  }
                  break; 
                }
                default:
                  LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
                break;
              }
              break;
            }
            default:
              LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
            break;
          }
          break; 
        }
        default:
          LOG_UNEXPECTED_MESSAGE(message, rr_band_avoid_state_name(rr_band_avoid_ptr->state));
        break;
      }
      break;
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(RR_BAND_AVOID_SM, 
                         EV_INPUT_MESSAGE,
                         (byte) rr_band_avoid_ptr->state,
                         message,
                         gas_id);
#endif

  if (rr_band_avoid_ptr->state != rr_band_avoid_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION("rr_band_avoid_control",
                            rr_band_avoid_ptr->old_state,
                            rr_band_avoid_ptr->state,
                            rr_band_avoid_state_name,
                            gas_id);
    
    rr_band_avoid_ptr->old_state = rr_band_avoid_ptr->state;
  }
}

/*!
 * \brief Convert an ARFCN to frequency (UL) in KHz
 *
 * \param arfcn
 *        gas_id
 *
 * \return uint32 - freq in KHz
 */
uint32 rr_band_avoid_arfcn_to_centre_freq_kHz(ARFCN_T arfcn, const gas_id_t gas_id)
{
   int32 freq = 0;

   switch ( arfcn.band )
   {
      case SYS_BAND_DCS_1800:
         freq = RR_DCS_UL_LOWER_FREQ + ( RR_CHAN_SPACE * ( arfcn.num - DCS_CHAN_OFFSET ) );
         break;
 
      case SYS_BAND_PCS_1900:
         freq = RR_PCS_UL_LOWER_FREQ + ( RR_CHAN_SPACE * ( arfcn.num - PCS_CHAN_OFFSET ) );
         break;
 
      case SYS_BAND_CELL_850:
         freq = RR_850_UL_LOWER_FREQ  + ( RR_CHAN_SPACE * ( arfcn.num - GSM_850_CHAN_OFFSET ) );
         break;

      case SYS_BAND_PGSM_900:
      case SYS_BAND_EGSM_900:
         if ( arfcn.num <= 124 )
         {
            freq = RR_PGSM_UL_LOWER_FREQ + ( RR_CHAN_SPACE * arfcn.num );
         }
         else
         {
            freq = RR_PGSM_UL_LOWER_FREQ + ( RR_CHAN_SPACE * ( arfcn.num - EGSM_HIGH_CHAN_OFFSET ) );
         }
         break;
 
     default:
         MSG_GERAN_ERROR_1_G("RR_BA Unsupported RF band: %d", arfcn.band);
         break;
   }
 
   return ( freq );
}
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
