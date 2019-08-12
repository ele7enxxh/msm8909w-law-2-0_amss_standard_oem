
/*!
  @file
  wwan_ba.c

  @brief
  This file implements the band avoidance functions of WWAN Coex.

*/

/*=============================================================================

  Copyright (c) 2014-16 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/wwan_coex/src/wwcoex_ba.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/16   jm      KW Warning Resolution
01/06/15   rj      Move Translation functions in Limitsmgr to a seperate file
06/13/14   jm      KW Warning Fix
04/28/14   jm      Initial Revision

=============================================================================*/

#include "wwcoex_ba.h"
#include "lmtsmgr_task.h"
#include "stringl.h"      /* memscpy */
#include "lmtsmgr_translation.h"

#ifndef ABS
  #define ABS(a) (((a) < 0) ? (-(a)) : (a))
#endif


/*=============================================================================

  FUNCTION:  wwcoex_ba_add_entry_to_bl

=============================================================================*/
/*!
    @brief
    Adds an blacklist entry to the blacklist
 
    @param
    [in] tech
    [in] Entry to be added
    [in/out] Blacklist
 
    @details
    - Entries that exactly match a frequency range in the blacklist
      will be aggregated as such:
        Rx Threshold: MAX between entries
        Tx Threshold: MIN between entries (Unknown power excluded)
 
    @return
    - Sorted blacklist
*/
/*===========================================================================*/
static void wwcoex_ba_add_entry_to_bl
(
  cxm_tech_type tech,
  coex_ba_list_s *entry,
  coex_ba_blist_data* blacklist
)
{
  uint32 i;
  boolean isunique = TRUE;

  if (entry == NULL || blacklist == NULL)
  {
    LMTSMGR_MSG_1(ERROR, "Unable to update BL for Tech %d: Entry and/or blacklist is NULL!", tech);
    return;
  }

  /* Search through the blacklist to see if an entry exists based on freq range */
  for (i=0; i<blacklist->num_entries; i++)
  {
    if ((entry->band == blacklist->freq_list[i].band) &&
        (entry->desense_mode == blacklist->freq_list[i].desense_mode) &&
        (entry->freq_lower == blacklist->freq_list[i].freq_lower) &&
        (entry->freq_upper == blacklist->freq_list[i].freq_upper))
    {
      isunique = FALSE;
      /* Select the 'worst' threshold  between the two */
      blacklist->freq_list[i].rxpwr_thresh =
        MAX(blacklist->freq_list[i].rxpwr_thresh, entry->rxpwr_thresh);

      /* Minumum TX taken (unknown power should be ignored) */
      if ((blacklist->freq_list[i].txpwr_thresh == CXM_UNKNOWN_POWER) &&
          (entry->txpwr_thresh == CXM_UNKNOWN_POWER))
      {
          blacklist->freq_list[i].txpwr_thresh = CXM_UNKNOWN_POWER;
      }
      else
      {
        /* Adjust unknown power for Tx threshold comparison */
        blacklist->freq_list[i].txpwr_thresh = 
          MIN(((blacklist->freq_list[i].txpwr_thresh == CXM_UNKNOWN_POWER) ? 
                 ABS(CXM_UNKNOWN_POWER) : blacklist->freq_list[i].txpwr_thresh),
              ((entry->txpwr_thresh == CXM_UNKNOWN_POWER) ? 
                 ABS(CXM_UNKNOWN_POWER) : entry->txpwr_thresh));
      }
      break;
    }
  }

  if (isunique)
  {
    if (blacklist->num_entries >= CXM_MAX_SUPP_BL_ENTRIES)
    {
      LMTSMGR_MSG_6(ERROR, "Blacklist Full! Tech: %d Band: %d Desense Mode: %d Freq Range: %d-%d Threshold: %d",
                    tech, entry->band, entry->desense_mode, entry->freq_lower, entry->freq_upper,
                    entry->desense_mode == DESENSE_VICTIM ? entry->rxpwr_thresh : entry->txpwr_thresh);
    }
    else
    {
      memscpy(&blacklist->freq_list[blacklist->num_entries],sizeof(coex_ba_list_s), entry, sizeof(coex_ba_list_s));
      blacklist->num_entries++;
    }
  }

  return;
}


/*=============================================================================

  FUNCTION:  wwcoex_ba_sort_blacklist

=============================================================================*/
/*!
    @brief
    Sorts the blacklist
 
    @param
    [in/out] - blist: The blacklist to be sorted
 
    @details
    Will be sorted in the following order:
        1. Aggressor entries
        2. Tx Desense Threshold (low to high)
        3. Victim entries
        4. Rx Desense Threshold (high to low)
 
    @return
    - Sorted blacklist
*/
/*===========================================================================*/
static void wwcoex_ba_sort_blacklist
(
  coex_ba_blist_data* blist
)
{
  coex_ba_list_s entry;
  uint8 i, j;

  if (blist == NULL)
    return;

  /* Insertion Sort */
  for (i=1; i<blist->num_entries; i++)
  {
    memscpy(&entry, sizeof(coex_ba_list_s), 
            &blist->freq_list[i], sizeof(coex_ba_list_s));
    j = i;

    /* Shift the entry to the right if the following is true:
        - Tx Threshold & Entry Tx are not unknown and Tx Threshold > Entry Tx Thd
        - Entry Tx Threshold is not unknown and Tx threshold is unknown
        - Both Tx thresholds are unknown and Rx Threshold < Entry Rx Threshold */
    while (j > 0 &&
          (((blist->freq_list[j-1].txpwr_thresh != CXM_UNKNOWN_POWER) &&
             (entry.txpwr_thresh != CXM_UNKNOWN_POWER) &&
             (blist->freq_list[j-1].txpwr_thresh > entry.txpwr_thresh))
           ||
             ((entry.txpwr_thresh != CXM_UNKNOWN_POWER) &&
              (blist->freq_list[j-1].txpwr_thresh == CXM_UNKNOWN_POWER))
           ||
            ((blist->freq_list[j-1].txpwr_thresh == CXM_UNKNOWN_POWER) &&
             (entry.txpwr_thresh == CXM_UNKNOWN_POWER) &&
             (blist->freq_list[j-1].rxpwr_thresh < entry.rxpwr_thresh))))
    {
      memscpy(&blist->freq_list[j], sizeof(coex_ba_list_s), 
              &blist->freq_list[j-1], sizeof(coex_ba_list_s));
      j--;
    }
     memscpy(&blist->freq_list[j], sizeof(coex_ba_list_s), 
             &entry, sizeof(coex_ba_list_s));
  }


}


/*=============================================================================

  FUNCTION:  wwcoex_ba_check_with_dl

=============================================================================*/
/*!
    @brief
    Compares a single serving tx frequency with all serving rx frequency of
    other tech
 
    @return
    - newBlacklist will be modified with the new blacklist data (unsorted)
    - Value will return number of serving frequency pairs which have desense issue (w/
      threshold crossed)
*/
/*===========================================================================*/
static uint32 wwcoex_ba_check_with_dl
(
  cxm_tech_type aTech,
  cxm_tech_type vTech,
  lmtsmgr_tech_ba_type* aTech_list,
  lmtsmgr_tech_ba_type* vTech_list,
  coex_ba_blist_data* newBlacklist,
  wwan_coex_desense_mode entry_type
)
{
   uint32 counter = 0, aggr_pos, vctm_pos, desense_mask = 0;
   int16 txpwr_thresh = CXM_UNKNOWN_POWER, rxpwr_thresh = CXM_UNKNOWN_POWER;
   coex_freqid_link_s aul_freq, vul_freq, vdl_freq;
   wwan_coex_desense_type i;
   coex_ba_list_s bListentry;

   /* Non-IM Freq compare argument structures */
   wwcoex_nonim_freqcompare_in  nonim_comp_in;
   wwcoex_nonim_freqcompare_out  nonim_comp_out;

    /* IM Freq compare argument structures */
   wwcoex_im_freqcompare_in      im_comp_in;
   wwcoex_im_freqcompare_out     im_comp_out;

   wwcoex_blank_efs_lookup_in lookup_in;

   /* For every agressor entry.. */
   for (aggr_pos=0; aggr_pos<aTech_list->num_ul_entries; aggr_pos++)
   {
      /* Construct Agressor UL frequency information to wwcoex format
      Assumption: wwan_coex_list does not utilize frequency ID */
      memset(&aul_freq, 0, sizeof(coex_freqid_link_s));
      aul_freq.link_info.band = aTech_list->serv_info_ul_list[aggr_pos].band;
      aul_freq.link_info.bandwidth = aTech_list->serv_info_ul_list[aggr_pos].bandwidth;
      aul_freq.link_info.channel = aTech_list->serv_info_ul_list[aggr_pos].channel;
      aul_freq.link_info.frequency = aTech_list->serv_info_ul_list[aggr_pos].frequency;
      aul_freq.link_info.direction = CXM_LNK_DRCTN_UL;
      /* RF Device & Link Type are Don't Cares */

      /* Compare against every victim entry.. */
      for (vctm_pos=0; vctm_pos<vTech_list->num_dl_entries; vctm_pos++)
      {  
        /* Construct Victim UL/DL frequency information to wwcoex format
           Assumption: wwan_coex_list does not utilize frequency ID */
        memset(&vul_freq, 0, sizeof(coex_freqid_link_s));
        vul_freq.link_info.band = vTech_list->serv_info_ul_list[vctm_pos].band;
        vul_freq.link_info.bandwidth = vTech_list->serv_info_ul_list[vctm_pos].bandwidth;
        vul_freq.link_info.channel = vTech_list->serv_info_ul_list[vctm_pos].channel;
        vul_freq.link_info.frequency = vTech_list->serv_info_ul_list[vctm_pos].frequency;
        vul_freq.link_info.direction = CXM_LNK_DRCTN_UL;
        /* RF Device & Link Type are Don't Cares */

        memset(&vdl_freq, 0, sizeof(coex_freqid_link_s));
        vdl_freq.link_info.band = vTech_list->serv_info_dl_list[vctm_pos].band;
        vdl_freq.link_info.bandwidth = vTech_list->serv_info_dl_list[vctm_pos].bandwidth;
        vdl_freq.link_info.channel = vTech_list->serv_info_dl_list[vctm_pos].channel;
        vdl_freq.link_info.frequency = vTech_list->serv_info_dl_list[vctm_pos].frequency;
        vdl_freq.link_info.direction = CXM_LNK_DRCTN_DL;
        /* RF Device & Link Type are Don't Cares */

        nonim_comp_in.atech = aTech;
        nonim_comp_in.vtech = vTech;
        nonim_comp_in.atulfreqinfo = aul_freq;
        nonim_comp_in.vtdlfreqinfo = vdl_freq;

        im_comp_in.atech = aTech;
        im_comp_in.vtech = vTech;
        im_comp_in.atulfreqinfo = aul_freq;
        im_comp_in.vtulfreqinfo = vul_freq;
        im_comp_in.vtdlfreqinfo = vdl_freq;
         
        nonim_comp_out.desense_mask = NO_DESENSE;
        im_comp_out.desense_mask = NO_DESENSE;
        desense_mask = NO_DESENSE;
         
        wwcoex_nonim_compare_freqs(&nonim_comp_in, &nonim_comp_out);
        desense_mask |= nonim_comp_out.desense_mask;

        wwcoex_im_compare_freqs(&im_comp_in, &im_comp_out);
        desense_mask |= im_comp_out.desense_mask;

        if (desense_mask == NO_DESENSE)
        {
          /* No interference between these frequencies */
          continue;
        }

        for (i = NO_DESENSE; i < MAX_DESENSE_TYPES; i++)
        {
          if ((1<<i) & desense_mask)
          {
            /* Generate blacklist entry */
            bListentry.desense_mode = entry_type;
            switch (bListentry.desense_mode)
            {
              case DESENSE_VICTIM:
                bListentry.band = vdl_freq.link_info.band;
                break;

              case DESENSE_AGGR:
                bListentry.band = aul_freq.link_info.band;
                break;

              default:
                LMTSMGR_MSG_1(ERROR, "Invalid desense entry type %d!", bListentry.desense_mode);
                continue;
            }

            wwcoex_gen_freq_range(aTech, vTech, &aTech_list->serv_info_ul_list[aggr_pos], 
                                  &vTech_list->serv_info_ul_list[vctm_pos],
                                  &vTech_list->serv_info_dl_list[vctm_pos], i, 
                                  bListentry.desense_mode, 
                                  &bListentry.freq_lower, &bListentry.freq_upper);

            lookup_in.vtech = vTech;
            lookup_in.vband = lmtsmgr_trans_sys_bands_to_rf_bands(
                                         vdl_freq.link_info.band , 
                                          vTech);
            lookup_in.atech = aTech;
            lookup_in.aband = lmtsmgr_trans_sys_bands_to_rf_bands(
                                                aul_freq.link_info.band ,
                                                aTech);

            lookup_in.victim_rxpower = vTech_list->serv_info_dl_list[vctm_pos].power;
            lookup_in.victim_txpower = vTech_list->serv_info_ul_list[vctm_pos].power;
            lookup_in.aggr_txpower = aTech_list->serv_info_ul_list[aggr_pos].power;

            lookup_in.desense = i;

            wwcoex_blank_efs_lookup_get_thresholds(&lookup_in, entry_type, 
                                                   &bListentry.txpwr_thresh,
                                                   &bListentry.rxpwr_thresh);

            LMTSMGR_MSG_5(LOW, "Desense: %d, Freq Lower: %u, Freq Upper: %u Tx Thresh: %d Rx Thresh: %d",
                                i, bListentry.freq_lower, bListentry.freq_upper,
                                bListentry.txpwr_thresh, bListentry.rxpwr_thresh);

            if ((newBlacklist->serv_freq_mode == DESENSE_AGGR_VICTIM) ||
                (entry_type == DESENSE_VICTIM && newBlacklist->serv_freq_mode == DESENSE_AGGR) ||
                (entry_type == DESENSE_AGGR && newBlacklist->serv_freq_mode == DESENSE_VICTIM))
            {
              newBlacklist->serv_freq_mode = DESENSE_AGGR_VICTIM;
            }
            else if (entry_type == DESENSE_AGGR)
            {
              newBlacklist->serv_freq_mode = DESENSE_AGGR;
            }
            else if (entry_type == DESENSE_VICTIM)
            {
              newBlacklist->serv_freq_mode = DESENSE_VICTIM;
            }


            /* if threshold set, then implicitly means power has crossed threshold so need to check
               other threshold */
            if ((entry_type == DESENSE_VICTIM && bListentry.rxpwr_thresh != CXM_UNKNOWN_POWER) ||
                (entry_type == DESENSE_AGGR && bListentry.txpwr_thresh != CXM_UNKNOWN_POWER)) 
            {

              wwcoex_blank_efs_lookup_get_thresholds(&lookup_in, ((entry_type == DESENSE_VICTIM) ? 
                                                     DESENSE_AGGR : DESENSE_VICTIM), 
                                                     &txpwr_thresh, &rxpwr_thresh);

              /* if other threshold set, then implicity means power has crossed threshold.
                 Serving Frequency is therefore bad. */
              if ((entry_type == DESENSE_VICTIM && txpwr_thresh != CXM_UNKNOWN_POWER) ||
                  (entry_type == DESENSE_AGGR && rxpwr_thresh != CXM_UNKNOWN_POWER))
              { 
                counter++;
                //TODO: Double check this counter.. you can have multiple desense for one pair.
              }
            }

            /* Attempt to add entry to list */
            wwcoex_ba_add_entry_to_bl((entry_type == DESENSE_VICTIM) ? vTech : aTech, &bListentry, newBlacklist);

          }   // End if            

        } // End for (Desense mask)

      } // End for (Victim DL)

   } // End for (Aggr UL)
   return counter;
}

/*=============================================================================

  FUNCTION:  wwcoex_ba_gen_blacklist

=============================================================================*/
/*!
    @brief
    Generate blacklist based off of two techs
 
    @details
    This should only be called when serving frequency, serving power,
    or a blacklist response has been sent from Tech L1
 
    @return
    newBlacklist will be modified with the new blacklist data
*/
/*===========================================================================*/
void wwcoex_ba_gen_blacklist
(
  cxm_tech_type bl_tech,              /* Tech that will be sent blacklist */
  lmtsmgr_tech_ba_type* bl_tech_info, /* Freq/Power info of blacklisted tech */
  cxm_tech_type st_tech,              /* Tech that will remain static */
  lmtsmgr_tech_ba_type* st_tech_info, /* Freq/Power info of static tech */
  coex_ba_blist_data* newBlacklist    /* Blacklist to be updated */
)
{
   lmtsmgr_ba_serv_info_type non_serv_freq_info;
   boolean tables_exist = FALSE;
   uint32 index, st_index;
   wwan_coex_desense_type desense_type;
   coex_ba_list_s bListentry;
   wwcoex_blank_efs_lookup_in lookup_in;
   wwcoex_comb_lookup_out output;
   uint32 counter_aggr=0, counter_vict=0;
   uint32 total_pairs, min_serv_freq_pct;
   
   if ((bl_tech_info == NULL) || (st_tech_info == NULL) ||
       (newBlacklist == NULL))
   {
     LMTSMGR_MSG_0(ERROR, "Unable to make blacklist due to NULL pointers!");
     return;
   }

   newBlacklist->serv_freq_state = SERV_FREQ_OK;
   newBlacklist->serv_freq_mode = DESENSE_NONE;

   if (bl_tech_info->num_entries < 2 || st_tech_info->num_entries < 2)
   {
     /* Invalid number of serving frequencies */
     return;
   }

   /* Check blacklisted tech as agressor */
   counter_aggr = wwcoex_ba_check_with_dl(bl_tech, st_tech, bl_tech_info, st_tech_info, newBlacklist, DESENSE_AGGR);

   /* Check blacklisted tech as victim */
   counter_vict = wwcoex_ba_check_with_dl(st_tech, bl_tech, st_tech_info, bl_tech_info, newBlacklist, DESENSE_VICTIM);

   //TODO: Frequency Percentage EFS
   min_serv_freq_pct = WWCOEX_BA_DEFAULT_MIN_PRCT;

   /* If the percentage of coexistence issues with the serving frequencies have been met (EFS based),
      serving frequency must be avoided */
   total_pairs = bl_tech_info->num_ul_entries*st_tech_info->num_ul_entries;
   newBlacklist->serv_freq_state = ((((uint64)(counter_vict + counter_aggr)/total_pairs)*100) >= min_serv_freq_pct) 
                                    ? SERV_FREQ_AVOID_REQ : SERV_FREQ_OK;
                                    

   memset(&output, 0, sizeof(wwcoex_comb_lookup_out));

   /* Find all bands of the blacklisted tech which has desense issues with non-blacklisted tech 
      Assumption - All serving frequencies will have the same band */
   wwcoex_get_all_desense_combs(bl_tech, st_tech, st_tech_info->serv_info_ul_list[0].band, &output);

   // For every (non-blacklisted) serving frequency pair.. (ul_entries == dl_entries)
   for (st_index = 0; st_index<st_tech_info->num_ul_entries; st_index++)
   {
     /* Generate a blacklist etnry for all blacklisted bands */
     for (index = 0; index<output.num_entries; index++)
     {
       LMTSMGR_MSG_3(LOW, "Combo found for band %d, mode: %d desense mask 0x%x", 
                     output.band_list[index].bl_band, output.band_list[index].mode,
                     output.band_list[index].desense_mask);

       if (output.band_list[index].bl_band == bl_tech_info->serv_info_ul_list[0].band)
       {
         /* Skip the current serving band since the blacklist entry
            is already covered above */
         continue;
       }
       
       /* Generate a blacklist entry for all desense types */
       for (desense_type = NO_DESENSE; desense_type < MAX_DESENSE_TYPES; desense_type++)
       {
         if ((1<<desense_type) & output.band_list[index].desense_mask)
         {
           bListentry.desense_mode = output.band_list[index].mode;
           bListentry.band = output.band_list[index].bl_band;

           non_serv_freq_info.band = bListentry.band;
           non_serv_freq_info.power = CXM_UNKNOWN_POWER;

           /* Determine the frequency range and power with the associated blacklisted band */
           if (bListentry.desense_mode == DESENSE_VICTIM)
           {
             wwcoex_gen_freq_range(st_tech, bl_tech, &st_tech_info->serv_info_ul_list[st_index], 
                                   NULL, &non_serv_freq_info, desense_type,
                                   bListentry.desense_mode, 
                                   &bListentry.freq_lower, &bListentry.freq_upper);

             lookup_in.vtech = bl_tech;
             lookup_in.vband = lmtsmgr_trans_sys_bands_to_rf_bands(bListentry.band, bl_tech);

             lookup_in.atech = st_tech;
             lookup_in.aband = lmtsmgr_trans_sys_bands_to_rf_bands(
                                    st_tech_info->serv_info_ul_list[st_index].band, st_tech);

             lookup_in.victim_rxpower = CXM_UNKNOWN_POWER;
             lookup_in.victim_txpower = CXM_UNKNOWN_POWER;
             lookup_in.aggr_txpower = st_tech_info->serv_info_ul_list[st_index].power;

             lookup_in.desense = desense_type;

             tables_exist = wwcoex_blank_efs_lookup_get_thresholds(&lookup_in, bListentry.desense_mode, 
                                                    &bListentry.txpwr_thresh,
                                                    &bListentry.rxpwr_thresh);
           }
           else if(bListentry.desense_mode == DESENSE_AGGR)
           {
             wwcoex_gen_freq_range(bl_tech, st_tech, &non_serv_freq_info, 
                                   &st_tech_info->serv_info_ul_list[st_index], 
                                   &st_tech_info->serv_info_dl_list[st_index], desense_type,
                                   bListentry.desense_mode, 
                                   &bListentry.freq_lower, &bListentry.freq_upper);

             lookup_in.vtech = st_tech;
             lookup_in.vband = lmtsmgr_trans_sys_bands_to_rf_bands(
                                    st_tech_info->serv_info_ul_list[st_index].band, st_tech);

             lookup_in.atech = bl_tech;
             lookup_in.aband = lmtsmgr_trans_sys_bands_to_rf_bands(bListentry.band, bl_tech);

             lookup_in.victim_rxpower = st_tech_info->serv_info_dl_list[st_index].power;
             lookup_in.victim_txpower = st_tech_info->serv_info_ul_list[st_index].power;
             lookup_in.aggr_txpower = CXM_UNKNOWN_POWER;

             lookup_in.desense = desense_type;

             tables_exist = wwcoex_blank_efs_lookup_get_thresholds(&lookup_in, bListentry.desense_mode, 
                                                    &bListentry.txpwr_thresh,
                                                    &bListentry.rxpwr_thresh);
           }

           /* Attempt to add entry to list if threshold tables exist for this combination */
           if(tables_exist)
           {
             wwcoex_ba_add_entry_to_bl(bl_tech, &bListentry, newBlacklist);
             LMTSMGR_MSG_6(MED, "Blacklist Band: %d Desense: %d, Freq Lower: %u, Freq Upper: %u Tx Thresh: %d Rx Thresh: %d",
                           bListentry.band, desense_type, bListentry.freq_lower, bListentry.freq_upper,
                           bListentry.txpwr_thresh, bListentry.rxpwr_thresh);
           }

         }   // End if            
       } /* End for - All desense types from blacklisted band */
     } /* End for - Blacklisted Band */
   } /* End for - Serving Frequency */

   wwcoex_ba_sort_blacklist(newBlacklist);
}


/*=============================================================================

  FUNCTION:  wwcoex_ba_comp_blacklists

=============================================================================*/
/*!
    @brief
    Compares the contents of the blacklists and determines if the data is
    the same.
 
    @details
    All fields are compared except for the blacklist id
    Assumption is that blacklists have already been sorted
 
    @return
    TRUE  - both blacklists have the same data
    FALSE - blacklists have different data
*/
/*===========================================================================*/
boolean wwcoex_ba_comp_blacklists
(
  coex_ba_blist_data *blist1,
  coex_ba_blist_data *blist2
)
{
  uint8 index;
  boolean result = TRUE;

  /* Blacklist id is ignored in the comparison */
  if ((blist1->num_entries != blist2->num_entries) ||
      (blist1->serv_freq_mode != blist2->serv_freq_mode) ||
      (blist1->serv_freq_state != blist2->serv_freq_state))
  {
    result = FALSE;
  }
  else
  {
    /* Compare every valid entry */
    for (index = 0; index<blist1->num_entries; index++)
    {
      /* Compare all fields in the entry */
      if ((blist1->freq_list[index].band != blist2->freq_list[index].band) ||
          (blist1->freq_list[index].desense_mode != blist2->freq_list[index].desense_mode) ||
          (blist1->freq_list[index].freq_lower != blist2->freq_list[index].freq_lower) ||
          (blist1->freq_list[index].freq_upper != blist2->freq_list[index].freq_upper) ||
          (blist1->freq_list[index].rxpwr_thresh != blist2->freq_list[index].rxpwr_thresh) ||
          (blist1->freq_list[index].txpwr_thresh != blist2->freq_list[index].txpwr_thresh))
      {
        result = FALSE;
        break;
      }
    }
  }

  return result;
}
