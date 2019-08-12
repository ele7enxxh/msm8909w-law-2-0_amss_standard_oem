/*============================================================================
  rr_irat_overlap_detect.c

  This file contains all the functions pertaining to the feature Duplicate EARFCN and WCDMA UARFCN Frequency detection logic.

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_irat_overlap_detect.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_seg_load.h"
#include "rr_l1_send.h"
#include "ms.h"
#include "rr_general.h"
#include "rr_log.h"
#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/
#include "rr_multi_sim.h"
#ifdef FEATURE_WCDMA
#include "wcdma_rrc_api.h"
#endif /* FEATURE_WCDMA */
#include "rr_sim.h"
#include "rr_callback.h"
#include "rr_sys_info_ncell.h"
#ifdef FEATURE_LTE
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_LTE */
#include "geran_tasks.h"
#include "rr_defs.h"
#include "stringl/stringl.h"
#include "rr_process_pmo_pcco.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
#include "rr_resel_g2w.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
#include "rr_control.h"
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */
#include "grr_mutex.h"
#include "gprs_mem.h"
#include "rr_irat_overlap_detect.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
#if defined(FEATURE_LTE) 
static rr_lte_freq_info_list_t rr_lte_freq_info_list;
#endif

#if defined(FEATURE_WCDMA)
static rr_wcdma_freq_info_list_t rr_wcdma_freq_info_list;
#endif

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

#if defined(FEATURE_LTE) 
 
/* This function marks the overlapped lte earfcn or wcdma uarfcn as INVALID ARFCN in frequency priority list, i.e the found entry will be marked as INVALID
 Please note that it will be cleaned up later */

static void rr_invalidate_freq_from_l1_irat_freq_priority_list(rr_l1_irat_frequency_priority_list_t* rr_l1_irat_freq_prior_list, uint32 arfcn, rr_l1_irat_rat_e rat )
{
  uint8 index;
  
  for( index=0; ((index < rr_l1_irat_freq_prior_list->count) && (index < MAX_RR_L1_PRIORITY_ENTRIES)) ; index++ )
  {
    if ( (rr_l1_irat_freq_prior_list->entries[index].arfcn == arfcn) && (rr_l1_irat_freq_prior_list->entries[index].rat == rat) )
    {
      MSG_GERAN_HIGH_2("rr_invalidate_freq_from_l1_irat_freq_priority_list Freq %d, At Index= %d",
                        arfcn, index );
      // Mark this entry as INVALID now.
      rr_l1_irat_freq_prior_list->entries[index].arfcn = INVALID_ARFCN;
      rr_l1_irat_freq_prior_list->entries[index].priority = RR_RESEL_PRIORITY_UNDEFINED;
      rr_l1_irat_freq_prior_list->entries[index].rat = RR_L1_IRAT_RAT_UNDEFINED;
    }
  }
}

/*!
 * \brief This function removes the overlapped(marked Invalid) LTE earfcn AND/OR WCDMA uarfcn from frequncy priority list, 
 * Function scans through the List for an INvalid ARFCN and removes it
 *
 * \param L1 IRAT Frequency Priority List (in)
 * Out void
 */
 
static void rr_remove_invalid_freq_from_l1_irat_freq_prior_list(rr_l1_irat_frequency_priority_list_t* rr_l1_irat_freq_prior_list_ptr )
{

  uint8 index, prev_count;
  uint8 real_index;  
 
  RR_NULL_CHECK_FATAL(rr_l1_irat_freq_prior_list_ptr);

  if ( rr_l1_irat_freq_prior_list_ptr->count == 0 )
  {
    
    MSG_GERAN_ERROR_0("rr_remove_invalid_freq_from_l1_irat_freq_prior_list() EMPTY LIST");    
    return;
  }

  real_index=0;
  prev_count = rr_l1_irat_freq_prior_list_ptr->count;
 
  for (index=0; ((index < rr_l1_irat_freq_prior_list_ptr->count) && (index < MAX_RR_L1_PRIORITY_ENTRIES)); index++ )
  {
    if ( (rr_l1_irat_freq_prior_list_ptr->entries[index].arfcn != INVALID_ARFCN)  && (rr_l1_irat_freq_prior_list_ptr->entries[index].rat != RR_L1_IRAT_RAT_UNDEFINED) )
    { /* Only copy those ARFCN(EARFCN/UARFCN), which are NOT marked as Invalid into final list */
      rr_l1_irat_freq_prior_list_ptr->entries[real_index].arfcn  = rr_l1_irat_freq_prior_list_ptr->entries[index].arfcn;
      rr_l1_irat_freq_prior_list_ptr->entries[real_index].priority = rr_l1_irat_freq_prior_list_ptr->entries[index].priority;
      rr_l1_irat_freq_prior_list_ptr->entries[real_index].rat = rr_l1_irat_freq_prior_list_ptr->entries[index].rat;
      real_index++;
    }
  }

  /* This is the real count now */
  rr_l1_irat_freq_prior_list_ptr->count = real_index;

  /* mark the remainder of the List as Zero */
  if(  prev_count > rr_l1_irat_freq_prior_list_ptr->count )
  { 
    for ( index = rr_l1_irat_freq_prior_list_ptr->count; ((index < prev_count) && (index < MAX_RR_L1_PRIORITY_ENTRIES)); index++ )
    {
      /* It implies Pruning has been done */
      memset( (void *)&rr_l1_irat_freq_prior_list_ptr->entries[index], 0, (sizeof(rr_l1_irat_frequency_priority_t)) );      
    }
  }         
}
#endif /* defined(FEATURE_LTE) */


#ifdef FEATURE_LTE
/*!
 * \brief This fucntion gets the received LTE list and constructs an LTE to Physical Frequency map for further checking
 * of overlapped EARFCN's. For each index into LTE list, corresponding index into rr_lte_freq_info_list is populated with
 * earfcn and its corresponding LTE physical frequency.
 *
 * \param LTE List Count(in)
 * \param LTE List
 * \param gas_id (in)
 */
 
static void rr_construct_lte_earfcn_to_phys_freq_map(uint8 earfcn_count, rr_l1_eutran_frequency_t* earfcn_list, const gas_id_t gas_id )
{
  uint8 index;
  uint16 phy_lte_freq;

  RR_NULL_CHECK_FATAL(earfcn_list);

  rr_lte_freq_info_list.count = earfcn_count;
  
  for ( index=0; ((index < earfcn_count) && (index < MAX_RR_L1_EUTRAN_FREQUENCIES)) ; index++ )
  {
    phy_lte_freq = lte_rrc_get_freq_from_earfcn(earfcn_list[index].earfcn);
    rr_lte_freq_info_list.earfcn[index] = (uint32) earfcn_list[index].earfcn ;
    rr_lte_freq_info_list.lte_phy_freq[index] = phy_lte_freq;
    MSG_GERAN_HIGH_2_G("rr_construct_lte_earfcn_to_phys_freq_map LTE Freq %d, PHYSICAL FREQ= %d",
                        (uint32) earfcn_list[index].earfcn, phy_lte_freq );    
  }
}


/*!
 * \brief This fucntion flushes, i.e. mem set to zero the LTE EARFCN to Physical freq map
 */

static void rr_flush_earfcn_phys_freq_map(const gas_id_t gas_id )
{  
  memset ((void *)&rr_lte_freq_info_list, 0, sizeof(rr_lte_freq_info_list_t));
}


/*!
 * \brief This function is called for both Idle Mode and Dedicated Mode cases.
          This function removes any Invalid EARFCN(marked due to duplicate detection) from Cell Update containter (Idle or Dedicated) and tidies up the list. 
 * \param idle_msg_ptr (in), in case when function is invoked in Idle Mode
 * \param ded_msg_ptr (in),  in case when function is invoked in Dedicated Mode 
 * \param gas_id (in)          
 */

static void rr_tidy_up_invalid_earfcn_from_wcdma_cell_update_list( mph_wcdma_cell_update_list_req_T* idle_msg_ptr, 
                                                                   mph_dedicated_wcdma_cell_update_list_req_T* ded_msg_ptr, 
                                                                   const gas_id_t gas_id)
{
  uint8 index, real_index;
  uint8 initial_eutran_frequency_count;
  uint8*  eutran_frequency_count_ptr;
  rr_l1_eutran_frequency_t* eutran_freq_list;


  if ( idle_msg_ptr != NULL )
  {
    eutran_frequency_count_ptr = &idle_msg_ptr->eutran_frequency_count;
    eutran_freq_list = &idle_msg_ptr->eutran_frequencies[0];

  }
  else if ( ded_msg_ptr != NULL )
  {
    eutran_frequency_count_ptr = &ded_msg_ptr->eutran_frequency_count;
    eutran_freq_list = &ded_msg_ptr->eutran_frequencies[0];
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_tidy_up_invalid_earfcn_from_wcdma_cell_update_list() NULL POINTER"); 
    return;
  }

  initial_eutran_frequency_count = (*eutran_frequency_count_ptr);
  
  real_index=0;
  
  for (index=0; ((index < initial_eutran_frequency_count) && (index < MAX_RR_L1_EUTRAN_FREQUENCIES)); index++ )
  {
    if (eutran_freq_list[index].earfcn != INVALID_EARFCN )
    { /* Only copy those EARFCN, which are NOT marked as Invalid into final list */
      eutran_freq_list[real_index].earfcn = eutran_freq_list[index].earfcn;
      eutran_freq_list[real_index].measurement_bandwidth = eutran_freq_list[index].measurement_bandwidth;
      eutran_freq_list[real_index].type = eutran_freq_list[index].type;       
      memscpy( eutran_freq_list[real_index].not_allowed_cells, sizeof(eutran_freq_list[real_index].not_allowed_cells), eutran_freq_list[index].not_allowed_cells, RR_L1_PCID_GROUP_BITMAP_LEN );     
      real_index++;
    }
  }

  /* Final count updated in the corresponding message */
  (*eutran_frequency_count_ptr) = real_index;


  /* mark the remainder of the List as Zero */  
  if ( initial_eutran_frequency_count > real_index)
  { /* It implies that the frequency list has been pruned */
    for ( index = real_index; ((index < initial_eutran_frequency_count) && (index < MAX_RR_L1_EUTRAN_FREQUENCIES)); index++ )
    {
      memset ((void *)&eutran_freq_list[index], 0, sizeof(rr_l1_eutran_frequency_t) );
    }
  }
  
  MSG_GERAN_HIGH_1_G("rr_tidy_up_invalid_earfcn_from_wcdma_cell_update_list FINAL frequency_priority_list COUNT=  %d", real_index );

  /* Now check and remove frequencies from Freq Priority List, priority ONLY applicable in Idle Mode Configuration */
  if ( idle_msg_ptr != NULL )
  {
    if ( idle_msg_ptr->use_priority_reselection_algo && (idle_msg_ptr->frequency_priority_list.count > 0) )
    {
      rr_remove_invalid_freq_from_l1_irat_freq_prior_list( &idle_msg_ptr->frequency_priority_list);
    }
  } 
}

/*!
 * \brief This fucntion is MAIN Entry Point Function for LTE when Overlap EARFCN needs to be removed from the WCDMA List.
 *        This function is called for both Idle Mode and Dedicated Mode cases.
          This function does following in sequence: 
          (i) First generates a LTE EARFCN to Physical Freq Map and saves it in global data
          (ii) It then scans through entire LTE list to detect Duplicates and marks them INVALID when duplicate is detected.
          (iii) Then it removes/filters the INVALID EARFCN and tidy up Frequency Priority List(for idle case Only) and any other LTE List.
          (iv) Lastly flushes global data as it will be re-created at each entry of the function. 
 * \param idle_msg_ptr (in), in case when function is invoked in Idle Mode
 * \param ded_msg_ptr (in),  in case when function is invoked in Dedicated Mode 
 * \param gas_id (in)          
 */
 
void rr_detect_and_remove_overlap_earfcn(mph_wcdma_cell_update_list_req_T* idle_msg_ptr, mph_dedicated_wcdma_cell_update_list_req_T* ded_msg_ptr, const gas_id_t gas_id )
{
  uint8 index;
  uint8 inner_index;
  uint32 earfcn, overlap_earfcn;
  uint16 lte_phy_freq;
  uint8  eutran_frequency_count;
  rr_l1_eutran_frequency_t* eutran_freq_list;

  
  boolean overlap_detected = FALSE;


  if ( idle_msg_ptr != NULL )
  {
    eutran_frequency_count = idle_msg_ptr->eutran_frequency_count;
    eutran_freq_list = &idle_msg_ptr->eutran_frequencies[0];
  }
  else if ( ded_msg_ptr != NULL )
  {
    eutran_frequency_count = ded_msg_ptr->eutran_frequency_count;
    eutran_freq_list = &ded_msg_ptr->eutran_frequencies[0];
  }
  else
  {
    
    MSG_GERAN_ERROR_0_G("rr_detect_and_remove_overlap_earfcn(), invoked with NULL POINTER");     
    return;
  }

  if ( eutran_frequency_count > MAX_RR_L1_EUTRAN_FREQUENCIES )
  {
      MSG_GERAN_ERROR_0_G("rr_detect_and_remove_overlap_earfcn(), eutran_frequency_count greater than MAX");     
      return;
  }
  
  /* First generate LTE Frequency to Physical Frequency MAP */
  rr_construct_lte_earfcn_to_phys_freq_map( eutran_frequency_count, eutran_freq_list, gas_id );
  
  /* Start from the highest Index */
  
  for (index = eutran_frequency_count-1; index > 0; index-- )
  {
    earfcn = (uint32) eutran_freq_list[index].earfcn;

    if (earfcn != INVALID_EARFCN )
    {
      lte_phy_freq = rr_lte_freq_info_list.lte_phy_freq[index];
        
      for (inner_index = 0; inner_index < index; inner_index++ )
      {
        if ( (lte_phy_freq == rr_lte_freq_info_list.lte_phy_freq[inner_index]) && (eutran_freq_list[inner_index].earfcn != INVALID_EARFCN ) )
        {           
          overlap_detected = TRUE;
          overlap_earfcn =  eutran_freq_list[inner_index].earfcn;          
          eutran_freq_list[inner_index].earfcn = INVALID_EARFCN;

	      MSG_GERAN_HIGH_3_G("rr_detect_and_remove_overlap_earfcn Overlap for  Freq %d, At Index= %d, Freq Index to Retain=%d",
                        overlap_earfcn, inner_index, index );

          if ( (idle_msg_ptr!= NULL) && (idle_msg_ptr->use_priority_reselection_algo )  )
          {
            rr_invalidate_freq_from_l1_irat_freq_priority_list( &idle_msg_ptr->frequency_priority_list, overlap_earfcn, RR_L1_IRAT_RAT_LTE);          
          }
          
          /* Continue checking, if there are more frequencies that may overlap with same LTE frequency*/
        }
      }
    }
  }

  if ( overlap_detected )
  {
   /* After the completion of the For loop all the overlapped frequencies are marked as INVALID in both the list */
   /* Now clean up both the lists before sending to GL1 */
    MSG_GERAN_HIGH_0_G("rr_detect_and_remove_overlap_earfcn LTE OVERLAP DETECTED");     
    rr_tidy_up_invalid_earfcn_from_wcdma_cell_update_list(idle_msg_ptr, ded_msg_ptr, gas_id);
  }

  /* Job Done, Flush the map created at the entry of this function */
  rr_flush_earfcn_phys_freq_map(gas_id);
}

#endif /* FEATURE_LTE */


#ifdef FEATURE_WCDMA

/*!
 * \brief This function marks the given WCDMA UARFCN in the wcdma_cell_list as INVALID
 * of overlapped UARFCN's's
 *
 * \param wcdma_cell_list (in)
 * \param uarfcn (in)
 * \param gas_id (in)
 */

static void rr_invalidate_wcdma_cells_from_cell_list(wcdma_cell_list_T* wcdma_cell_list, uint32 uarfcn, const gas_id_t gas_id )
{
  uint8 index;

  RR_NULL_CHECK_FATAL(wcdma_cell_list);  
  
  for ( index=0; ((index < wcdma_cell_list->num_of_cells) && (index < MAX_RR_MPH_CELL_UPDATE_LIST)); index++ )
  {
    if ( wcdma_cell_list->cell_list[index].uarfcn == uarfcn )
    {
      MSG_GERAN_HIGH_2_G("rr_invalidate_wcdma_cells_from_cell_list Freq %d, at Index= %d",
                        uarfcn, index );
      wcdma_cell_list->cell_list[index].uarfcn = (word) INVALID_UARFCN;
      wcdma_cell_list->cell_list[index].scrambling_code = 0xFF;      
    }
  }
}


/*!
 * \brief This function gets the received UARFCN list and constructs an WCDMA UARFCN to Physical Frequency map for further checking
 * of overlapped UARFCN's's
 *
 * \param wcdma freq list (in)
 * \param gas_id (in)
 */
 
static void rr_construct_wcdma_uarfcn_to_phys_freq_map(wcdma_reselection_freq_list_T* wcdma_reselection_freq_list,const gas_id_t gas_id )
{
  uint16 wcdma_phy_freq;
  uint8 index;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  
  RR_NULL_CHECK_FATAL(wcdma_reselection_freq_list);  
  RR_NULL_CHECK_RETURN_VOID(scell_info_ptr);
  
  rr_wcdma_freq_info_list.count = wcdma_reselection_freq_list->num_of_frequencies;
  for ( index = 0; ((index < wcdma_reselection_freq_list->num_of_frequencies) && (index < MAX_RR_MPH_FREQ_UPDATE_LIST)); index++ )
  {
    if ( E_SUCCESS == wcdma_derive_freq_from_uarfcn(wcdma_reselection_freq_list->uarfcn[index], scell_info_ptr->gsm.location_area_identification.plmn_id,  &wcdma_phy_freq) )
    {
      rr_wcdma_freq_info_list.uarfcn[index] = wcdma_reselection_freq_list->uarfcn[index];
      rr_wcdma_freq_info_list.wcdma_phy_freq[index] = wcdma_phy_freq;       
    }
    else
    {
      MSG_GERAN_ERROR_0_G("rr_construct_wcdma_uarfcn_to_phys_freq_map(), WCDMA Physical Freq Derivation Failed, Quit Overlap Detection"); 
      rr_wcdma_freq_info_list.count = 0;
      break;
    }
  }
}

/*!
 * \brief This fucntion flushes, i.e. mem set to zero the WCDMA UARFCN to Physical freq map
 */

static void rr_flush_uarfcn_phys_freq_map(const gas_id_t gas_id )
{  
  memset ((void *)&rr_wcdma_freq_info_list, 0, sizeof(rr_wcdma_freq_info_list_t));
}


/*!
 * \brief This function scans through the Applicable WCDMA Cell Lists and removes any INVALID UARFCN, which may have been
          detected (due to been an Overlapped frequency) and sets the count accordingly.
 * \param wcdma_cell_list_ptr (in), WCDMA Cell List Pointer
 * \param gas_id (in)             
 */
static void rr_tidy_up_invalid_uarfcn_from_wcdma_cell_list ( wcdma_cell_list_T* wcdma_cell_list_ptr, const gas_id_t gas_id )
{
  uint8 index, real_index;
  uint8 prev_cell_count;
  
              
  if ( wcdma_cell_list_ptr->num_of_cells > 0 )
  {
    real_index=0;
    
    prev_cell_count = wcdma_cell_list_ptr->num_of_cells ;
    
    for (index=0; ((index < wcdma_cell_list_ptr->num_of_cells) && (index < MAX_RR_MPH_CELL_UPDATE_LIST)); index++ )
    { /* Only copy those Cells, whose ARFCN are NOT marked as Invalid into final list */
      if ( wcdma_cell_list_ptr->cell_list[index].uarfcn != (word) INVALID_UARFCN )
      {
        wcdma_cell_list_ptr->cell_list[real_index].uarfcn = wcdma_cell_list_ptr->cell_list[index].uarfcn;
        wcdma_cell_list_ptr->cell_list[real_index].scrambling_code = wcdma_cell_list_ptr->cell_list[index].scrambling_code ;
        wcdma_cell_list_ptr->cell_list[real_index].diversity = wcdma_cell_list_ptr->cell_list[index].diversity;
        real_index++;
      }
    }
    wcdma_cell_list_ptr->num_of_cells = real_index;

    if ( prev_cell_count > wcdma_cell_list_ptr->num_of_cells  )
    {
      /* It implies Filtering has been done */  
      for ( index = wcdma_cell_list_ptr->num_of_cells; ((index < prev_cell_count) && (index < MAX_RR_MPH_CELL_UPDATE_LIST) ); index++ )
      {
        memset ((void *)&wcdma_cell_list_ptr->cell_list[index], 0, sizeof(wcdma_cell_T) );
      }
    }
  }
}

/*!
 * \brief This fucntion scans through all the Applicable WCDMA Lists in IDLE Mode and removes any INVALID UARFCN, which may have been
          detected (due to been an Overlapped frequency) and sets the count accordingly.
 * \param idle_msg_ptr (in), Idle  Mode configuration Pointer
 * \param gas_id (in)             
 */

static void rr_tidy_up_invalid_uarfcn_from_wcdma_cell_update_list_idle( mph_wcdma_cell_update_list_req_T* idle_msg_ptr, const gas_id_t gas_id)
{

  uint8 index, real_index;
  uint8 prev_count;

  prev_count=0;
  real_index=0;

  /* First the WCDMA Frequency List */
  prev_count = idle_msg_ptr->wcdma_reselection_freq_list.num_of_frequencies;
  
  for (index=0; ((index < idle_msg_ptr->wcdma_reselection_freq_list.num_of_frequencies) && (index < MAX_RR_MPH_FREQ_UPDATE_LIST)); index++ )
  {
    if (idle_msg_ptr->wcdma_reselection_freq_list.uarfcn[index] != (word) INVALID_UARFCN )
    { /* Only copy those UARFCN, which are NOT marked as Invalid into final list */
      idle_msg_ptr->wcdma_reselection_freq_list.uarfcn[real_index] = idle_msg_ptr->wcdma_reselection_freq_list.uarfcn[index];
      real_index++;
    }
  }
  idle_msg_ptr->wcdma_reselection_freq_list.num_of_frequencies = real_index;

  if(  prev_count > idle_msg_ptr->wcdma_reselection_freq_list.num_of_frequencies )
  {
    for ( index = idle_msg_ptr->wcdma_reselection_freq_list.num_of_frequencies; ((index < prev_count) && (index < MAX_RR_MPH_FREQ_UPDATE_LIST)); index++ )
    {
      /* It implies Filtering has been done, reset the moved data to Zero */  
      memset( (void *)&idle_msg_ptr->wcdma_reselection_freq_list.uarfcn[index], 0, sizeof(word));
    }
  }

#ifdef FEATURE_LTE
  /* Next the Priority List, if Priority Algorithm used */
  if ( idle_msg_ptr->use_priority_reselection_algo && (idle_msg_ptr->frequency_priority_list.count > 0) )
  {
    rr_remove_invalid_freq_from_l1_irat_freq_prior_list(&idle_msg_ptr->frequency_priority_list);
  }
#endif

  /* Lastly the WCDMA Cell List */
  rr_tidy_up_invalid_uarfcn_from_wcdma_cell_list(&idle_msg_ptr->wcdma_cell_list, gas_id );
}


/*!
 * \brief This fucntion constructs a Unique WCDMA Frequency List from a given WCDMA Cell List
          This funciton is applicable ONLY in dedicated mode where only WCDMA Cell List is provided.
 * \param wcdma_cell_list (in), Given WCDMA Cell List Pointer
 * \param wcdma_reselection_freq_list_ptr (out) WCDMA Reselection Frequency List
 * \param gas_id (in)             
 */
 
static void rr_construct_wcdma_reselection_freq_list( wcdma_reselection_freq_list_T* wcdma_reselection_freq_list_ptr, wcdma_cell_list_T* wcdma_cell_list, const gas_id_t gas_id )
{

  uint8 index, inner_index;
  uint16 uarfcn;

  wcdma_reselection_freq_list_ptr->uarfcn[0] = wcdma_cell_list->cell_list[0].uarfcn ;
  wcdma_reselection_freq_list_ptr->num_of_frequencies++;

  /* In dedicated mode, as there is no wcdma_reselection_freq_list, only Wcdma Cell List, create Unique frequency list */
  for ( index=1; ((index < wcdma_cell_list->num_of_cells) && (index < MAX_RR_MPH_CELL_UPDATE_LIST)); index++ )
  {
    boolean found=FALSE;

    uarfcn = wcdma_cell_list->cell_list[index].uarfcn;
  
    for ( inner_index=0; inner_index < wcdma_reselection_freq_list_ptr->num_of_frequencies; inner_index++ )
    {
      if ( wcdma_reselection_freq_list_ptr->uarfcn[inner_index] == uarfcn )
      {
        MSG_GERAN_HIGH_2_G("rr_construct_wcdma_reselection_freq_list UARFCN %d Present at Index= %d", uarfcn, index);  
        found=TRUE;
        break; 
      }
    }       

    if ( (!found) && (wcdma_reselection_freq_list_ptr->num_of_frequencies < MAX_RR_MPH_FREQ_UPDATE_LIST) )
    {
      wcdma_reselection_freq_list_ptr->uarfcn[wcdma_reselection_freq_list_ptr->num_of_frequencies] = uarfcn;
      wcdma_reselection_freq_list_ptr->num_of_frequencies++;
    }
  }
}


/*!
 * \brief This fucntion is MAIN Entry Point Function for WCDMA when Overlap UARFCN needs to be removed from the WCDMA List.
 *        It is called ONLY for Idle Mode.
          This function does following in sequence: 
          (i) First generates a UARFCN to Physical Freq Map and saves it in global data
          (ii) It then scans through entire UARFCN list to detect Duplicates and marks them INVALID when duplicate is detected.
          (iii) Then it removes/filters the INVALID UARFCN and tidy up Frequency Priority List, WCDMA Freq Resel List and WCDMA Cell List
          (iv) Lastly flushes global data as it will be re-created at each entry of the function. 
 * \param idle_msg_ptr (in), Idle  Mode configuration Pointer
 * \param gas_id (in)          
 */

void rr_detect_and_remove_overlap_uarfcn_in_idle( mph_wcdma_cell_update_list_req_T* idle_msg_ptr, const gas_id_t gas_id)
{
  uint8 index;
  uint8 inner_index;
  uint32 uarfcn, overlap_uarfcn;
  uint16 utra_phy_freq;
  boolean overlap_detected;

  wcdma_reselection_freq_list_T*  wcdma_reselection_freq_list_ptr;
  wcdma_cell_list_T* wcdma_cell_list_ptr;
  
  wcdma_reselection_freq_list_ptr = NULL;
  wcdma_cell_list_ptr = NULL;
  overlap_detected = FALSE;

  /* If IDLE Mode list the wcdma_reselection_freq_list is supplied from input, i.e. idle_msg_ptr */
  if ( idle_msg_ptr != NULL )
  {
    wcdma_reselection_freq_list_ptr = &idle_msg_ptr->wcdma_reselection_freq_list;
    wcdma_cell_list_ptr = &idle_msg_ptr->wcdma_cell_list;
  }
  else
  {
    
    MSG_GERAN_ERROR_0_G("rr_detect_and_remove_overlap_uarfcn_in_idle(), Invoked with NULL Pointer");     
    return;
  }
      
  rr_construct_wcdma_uarfcn_to_phys_freq_map( wcdma_reselection_freq_list_ptr, gas_id );

  for(index = wcdma_reselection_freq_list_ptr->num_of_frequencies-1; index > 0; index-- )
  {
    uarfcn = wcdma_reselection_freq_list_ptr->uarfcn[index];
    utra_phy_freq = rr_wcdma_freq_info_list.wcdma_phy_freq[index];

    overlap_uarfcn = INVALID_UARFCN;
    if ( uarfcn != (word) INVALID_UARFCN )
	{
      for (inner_index = 0; inner_index < index; inner_index++ )
      {
        if ( (utra_phy_freq == rr_wcdma_freq_info_list.wcdma_phy_freq[inner_index]) && (wcdma_reselection_freq_list_ptr->uarfcn[inner_index] != (word)INVALID_UARFCN) )
        {
          overlap_detected = TRUE;
          overlap_uarfcn = rr_wcdma_freq_info_list.uarfcn[inner_index];
          MSG_GERAN_HIGH_2_G("rr_detect_and_remove_overlap_uarfcn_in_idle Overlap detected for UARFCN %d at Index= %d", overlap_uarfcn, inner_index);      

#ifdef FEATURE_LTE          
          if ( idle_msg_ptr->use_priority_reselection_algo )
          {
            rr_invalidate_freq_from_l1_irat_freq_priority_list( &idle_msg_ptr->frequency_priority_list, overlap_uarfcn, RR_L1_IRAT_RAT_WCDMA);
          }
#endif
          if ( wcdma_cell_list_ptr->num_of_cells > 0 )
          {
            /* MARK Cells corrsponding to Duplicate UARFCN as INVALID */
            rr_invalidate_wcdma_cells_from_cell_list( wcdma_cell_list_ptr, overlap_uarfcn, gas_id );        
          }
          wcdma_reselection_freq_list_ptr->uarfcn[inner_index] = (word) INVALID_UARFCN;
        }
      }
    }
  }

  if ( overlap_detected)
  {
    /* After the completion of the For loop all the overlapped frequencies are marked as INVALID in all the list */
    /* Now clean up all the UARFCN lists before sending to GL1 */
    rr_tidy_up_invalid_uarfcn_from_wcdma_cell_update_list_idle(idle_msg_ptr, gas_id);
  }

  /* Now the job is DONE, flush the map as it will be recreated at each time fitlering of UARFCN, is required */
  rr_flush_uarfcn_phys_freq_map(gas_id);
}


/*!
 * \brief This fucntion is MAIN Entry Point Function for WCDMA when Overlap UARFCN needs to be removed from the WCDMA List.
 *       It is called ONLY for Dedicated Mode.
 *        This function does following in sequence: 
 *        (i) It first constructs a Unique Frequency List from WCDMA Cell List in Dedicated Mode.
 *        (ii) From Unique Frequency List generate a UARFCN to Physical Freq Map and saves it in global data
 *       (ii) Scans through entire UARFCN list to detect Duplicates and marks them INVALID when duplicate is detected.
 *       (iii) Then it removes/filters the INVALID UARFCN and tidy up WCDMA Cell List
 *       (iv) Lastly flushes global data as it will be re-created at each entry of the function. 
 * \param ded_msg_ptr (in), Dedicated  Mode configuration Pointer
 * \param gas_id (in)          
 */

void rr_detect_and_remove_overlap_uarfcn_in_dedicated( mph_dedicated_wcdma_cell_update_list_req_T* ded_msg_ptr, const gas_id_t gas_id)
{
  uint8 index;
  uint8 inner_index;
  uint32 uarfcn, overlap_uarfcn;
  uint16 utra_phy_freq;
  boolean overlap_detected;
    
  wcdma_reselection_freq_list_T*  wcdma_reselection_freq_list_ptr;
  wcdma_cell_list_T* wcdma_cell_list_ptr;
    
  wcdma_reselection_freq_list_ptr = NULL;
  wcdma_cell_list_ptr = NULL;
  overlap_detected = FALSE;

  if ( ded_msg_ptr == NULL )
  {
    MSG_GERAN_ERROR_0_G("rr_detect_and_remove_overlap_uarfcn_in_dedicated(), Invoked with NULL DED MSG Pointer "); 
    return;
  }
  
  /* In DEDICATED Mode the wcdma_reselection_freq_list is constructed from supplied WCDMA CELL LIST, i.e. from ded_msg_ptr */
  
  wcdma_reselection_freq_list_ptr = GPRS_MEM_CALLOC(sizeof (wcdma_reselection_freq_list_T));
  if( wcdma_reselection_freq_list_ptr == NULL )
  {
    MSG_GERAN_ERROR_0_G("rr_detect_and_remove_overlap_uarfcn_in_dedicated(), MEM ALLOC Failed for wcdma_reselection_freq_list_ptr"); 
    return;
  }

  wcdma_cell_list_ptr = &ded_msg_ptr->wcdma_cell_list; 
  rr_construct_wcdma_reselection_freq_list(wcdma_reselection_freq_list_ptr, &ded_msg_ptr->wcdma_cell_list,  gas_id );

  rr_construct_wcdma_uarfcn_to_phys_freq_map( wcdma_reselection_freq_list_ptr, gas_id );

  for(index = wcdma_reselection_freq_list_ptr->num_of_frequencies-1; index > 0; index-- )
  {
    uarfcn = wcdma_reselection_freq_list_ptr->uarfcn[index];
    utra_phy_freq = rr_wcdma_freq_info_list.wcdma_phy_freq[index];

    overlap_uarfcn = INVALID_UARFCN;

    if ( uarfcn != (word) INVALID_UARFCN )
	{  
      for (inner_index = 0; inner_index < index; inner_index++ )
      {
        if ( (utra_phy_freq == rr_wcdma_freq_info_list.wcdma_phy_freq[inner_index]) && (wcdma_reselection_freq_list_ptr->uarfcn[inner_index] != (word)INVALID_UARFCN) )
        {
          overlap_detected = TRUE;
          overlap_uarfcn = rr_wcdma_freq_info_list.uarfcn[inner_index];
          MSG_GERAN_HIGH_2_G("rr_detect_and_remove_overlap_uarfcn_in_dedicated Overlap detected for UARFCN %d at Index= %d", overlap_uarfcn, inner_index);  

          /* MARK Cells corrsponding to Duplicate UARFCN as INVALID */      
          if ( wcdma_cell_list_ptr->num_of_cells > 0 )
          {
            rr_invalidate_wcdma_cells_from_cell_list( wcdma_cell_list_ptr, overlap_uarfcn, gas_id );        
          }

          wcdma_reselection_freq_list_ptr->uarfcn[inner_index]= (word) INVALID_UARFCN;
        }
      }
    }
  }

  if ( overlap_detected)
  {
    rr_tidy_up_invalid_uarfcn_from_wcdma_cell_list(wcdma_cell_list_ptr, gas_id);
  }

  /* Now the job is DONE, flush the map as it will be recreated at each time fitlering of UARFCN, is required */
  rr_flush_uarfcn_phys_freq_map(gas_id);
  GPRS_MEM_FREE(wcdma_reselection_freq_list_ptr);
  
}
#endif /* FEATURE_WCDMA */


/* EOF */

