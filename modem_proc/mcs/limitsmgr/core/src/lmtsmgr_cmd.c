
/*!
  @file
  lmtsmgr_cmd.c

  @brief
  This file implements the core functions of LIMITS MGR module.

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014-16 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/src/lmtsmgr_cmd.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/20/15   sg      Added partial support for WCDMA2 client
01/06/15   rj      Move Translation functions in Limitsmgr to a seperate file
11/25/14   ag      Update macro state of each tech based on UL/DL registration
11/05/14   rj      Cleanup DDR related processing from Core file
11/04/14   ag      Added TTL markers for profiling 
10/27/14   rj      Cleanup SAR related processing
10/10/14   ag      Added support for coex tech state indication
09/23/14   jm      FreqId generation support in sim mode
09/19/14   ag      Added support for call in distress
09/10/14   jm      Band avoidance support for 1X/HDR/WCDMA/TDSCDMA
08/29/14   jm      Change SL TL sim to use FreqId Pointer
08/25/14   jm      Circular buffer for slot level activity tl
08/13/14   rj      Disable SVLTE+G Idle Mode Hopping
08/12/14   jm      Slot level timeline support
07/17/14   jm      Utilize proper RF API to convert GSM AFRCN to Frequency
06/23/14   sg      desense indication mechanism
07/07/14   jm      Add num frequency check in power update
04/07/14   ag      Remove hard-coding  for TDS to use Tx power based on freqId
03/27/14   jm      Added API to query UL/DL status of all techs
03/17/14   jm      WWAN coex based off of RF device assignment
01/14/14   jm      Ensure freqID is 16-bit value
01/14/14   ag      Fixed mutex leak
12/09/13   jm      DDR management phase 2 support (diversity power)
11/21/13   jm      Adding DDR management support
10/11/13   ag      Fixed allowing 24 freqs to be registered
11/23/13   rj      Handle Tech Rx Power Ind after Rx OFF indication
08/12/13   ag      Convert STX messages to WWAN_TECH for WLAN SAR
07/01/13   rj      KW error fixes
06/24/13   ag      Avoid generating new ids and running the algo if list is same
05/05/13   ag      Discard activity info once tech has stopped Tx
04/06/13   ag      Support for DSDA WWAN Coexistence 
03/24/12   ag      Fixed array out of bounds 
03/07/13   rj      Handling new MSGR for LTE BW, Rx freq and sys band to RF band mapping
02/20/13   rj      Fix CW issues
02/12/13   rj      Adding support for WLAN-SAR
09/24/12   ag      Moving to msgr interface
04/22/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "stringl.h"
#include "wwan_coex_mgr.h"
#include "wwcoex_action_iface.h"
#include "lmtsmgr_cmd.h"
#include "lmtsmgr_msgr.h"
#include "lmtsmgr_i.h"
#include "lmtsmgr_task.h"
#include "modem_mem.h"
#include "wwcoex_conflict_table.h"
#include "wwcoex_inactivity.h"
#include "lmtsmgr_diag.h"
#include "rfm_gsm.h"
#include "tcxomgr.h"
#include "sar_i.h"
#include "ddr_coex_i.h"
#include "lmtsmgr_translation.h"

/*=============================================================================

                            TYPEDEFS

=============================================================================*/

/* Default freq to be used when sending message to CXM */
#define LMTSMGR_UL_DEFAULT_FREQ     20
#define LMTSMGR_DL_DEFAULT_FREQ     30
#define LMTSMGR_ZERO_LINK_INFO_SET     0

#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
 #error code not present
#endif

/* Temporary container to hold new serving frequency information
   in desired limitsmgr format */
static lmtsmgr_tech_ba_type newServFreqList;


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  lmtsmgr_update_cxm_abt_tech_tx

=============================================================================*/
/*!
    @brief
    updates CXM regarding Tech's Tx state.
 
    @return
    None
*/
/*=============================================================================*/
#ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
static void lmtsmgr_update_cxm_abt_tech_tx 
(
  cxm_stx_tech_state_chs_ind_s* wlan_msg_ptr
)
{
  /* index for count */
  uint8 index;
  cxm_wwan_tech_state_ind_s lmtsmgr_target_tech;
  uint32 num_channels = 0;
  boolean found_gsm_victim, found_gsm_aggr;

  if ((wlan_msg_ptr->tech_id != CXM_TECH_GSM1) && (wlan_msg_ptr->tech_id != CXM_TECH_GSM2) &&
      (wlan_msg_ptr->tech_id != CXM_TECH_GSM3) )
  {
    LMTSMGR_MSG_1(LOW, "lmtsmgr_update_cxm_abt_tech_tx Tech %d is ignored %d \n", 
                  wlan_msg_ptr->tech_id);
    return;
  }

  memset(&lmtsmgr_target_tech, 0, sizeof(cxm_wwan_tech_state_ind_s));

  lmtsmgr_target_tech.tech_id = wlan_msg_ptr->tech_id;

  if (wlan_msg_ptr->num_channels > 0)
  {
    /* Tx state is ON */
    found_gsm_victim = FALSE;
    found_gsm_aggr = FALSE;
    for (index=0; index < wlan_msg_ptr->num_channels; index++)
    {

      if (num_channels >= CXM_MAX_SUPPORTED_LINK_SETS)
      { 
        LMTSMGR_MSG_2(FATAL, "Unexpected num channels %d for Tech %d freq update",
                     num_channels, wlan_msg_ptr->tech_id);
        break;
      }
        
      /* Populate only the entry that matters for SGLTE coex */
      if ( (found_gsm_victim == FALSE) &&
           (wwcoex_is_gsm_channel_victim(wlan_msg_ptr->band, wlan_msg_ptr->channels[index])) )
      {
        lmtsmgr_target_tech.tech_data.link_list[num_channels].channel = wlan_msg_ptr->channels[index];
        lmtsmgr_target_tech.tech_data.link_list[num_channels].rf_device = wlan_msg_ptr->tx_rfdev;
        lmtsmgr_target_tech.tech_data.link_list[num_channels].band = wlan_msg_ptr->band;
        lmtsmgr_target_tech.tech_data.link_list[num_channels].direction = CXM_LNK_DRCTN_UL;
        /* freq to be LMTSMGR_UL_DEFAULT_FREQ so that CXM/WLAN interpret it as Tx ON */
        lmtsmgr_target_tech.tech_data.link_list[num_channels].frequency = LMTSMGR_UL_DEFAULT_FREQ;
        lmtsmgr_target_tech.tech_data.link_list[num_channels].bandwidth = 250000;

        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].channel = wlan_msg_ptr->channels[index];
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].rf_device = wlan_msg_ptr->tx_rfdev;
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].band = wlan_msg_ptr->band;
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].direction = CXM_LNK_DRCTN_DL;
        /* freq to be LMTSMGR_UL_DEFAULT_FREQ so that CXM/WLAN interpret it as Tx ON */
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].frequency = LMTSMGR_DL_DEFAULT_FREQ;  
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].bandwidth = 250000;

           
        num_channels += 2;

        found_gsm_victim = TRUE;
      }

      if ( (found_gsm_aggr == FALSE) &&
           (wwcoex_is_gsm_channel_aggr(wlan_msg_ptr->band, wlan_msg_ptr->channels[index])) )
      {
        lmtsmgr_target_tech.tech_data.link_list[num_channels].channel = wlan_msg_ptr->channels[index];
        lmtsmgr_target_tech.tech_data.link_list[num_channels].rf_device = wlan_msg_ptr->tx_rfdev;
        lmtsmgr_target_tech.tech_data.link_list[num_channels].band = wlan_msg_ptr->band;
        lmtsmgr_target_tech.tech_data.link_list[num_channels].direction = CXM_LNK_DRCTN_UL;
        /* freq to be LMTSMGR_UL_DEFAULT_FREQ so that CXM/WLAN interpret it as Tx ON */
        lmtsmgr_target_tech.tech_data.link_list[num_channels].frequency = LMTSMGR_UL_DEFAULT_FREQ;
        lmtsmgr_target_tech.tech_data.link_list[num_channels].bandwidth = 250000;

        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].channel = wlan_msg_ptr->channels[index];
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].rf_device = wlan_msg_ptr->tx_rfdev;
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].band = wlan_msg_ptr->band;
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].direction = CXM_LNK_DRCTN_DL;
        /* freq to be LMTSMGR_UL_DEFAULT_FREQ so that CXM/WLAN interpret it as Tx ON */
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].frequency = LMTSMGR_DL_DEFAULT_FREQ;  
        lmtsmgr_target_tech.tech_data.link_list[num_channels+1].bandwidth = 250000;

           
        num_channels += 2;

        found_gsm_aggr = TRUE;
      }

      if (found_gsm_victim && found_gsm_aggr)
      {
        break;
      }
    }
    
    /* If we didnt find any SGLTE coex channel, just send one entry to indicate Tx on for SAR 
       and GPS coex */
    if (num_channels == 0)
    {
      lmtsmgr_target_tech.tech_data.link_list[0].channel = wlan_msg_ptr->channels[0];
      lmtsmgr_target_tech.tech_data.link_list[0].rf_device = wlan_msg_ptr->tx_rfdev;
      lmtsmgr_target_tech.tech_data.link_list[0].band = wlan_msg_ptr->band;
      lmtsmgr_target_tech.tech_data.link_list[0].direction = CXM_LNK_DRCTN_UL;
        /* freq to be LMTSMGR_UL_DEFAULT_FREQ so that CXM/WLAN interpret it as Tx ON */
      lmtsmgr_target_tech.tech_data.link_list[0].frequency = LMTSMGR_UL_DEFAULT_FREQ;
      lmtsmgr_target_tech.tech_data.link_list[0].bandwidth = 250000;

      lmtsmgr_target_tech.tech_data.link_list[1].channel = wlan_msg_ptr->channels[0];
      lmtsmgr_target_tech.tech_data.link_list[1].rf_device = wlan_msg_ptr->tx_rfdev;
      lmtsmgr_target_tech.tech_data.link_list[1].band = wlan_msg_ptr->band;
      lmtsmgr_target_tech.tech_data.link_list[1].direction = CXM_LNK_DRCTN_DL;
      /* freq to be LMTSMGR_UL_DEFAULT_FREQ so that CXM/WLAN interpret it as Tx ON */
      lmtsmgr_target_tech.tech_data.link_list[1].frequency = LMTSMGR_DL_DEFAULT_FREQ; 
      lmtsmgr_target_tech.tech_data.link_list[1].bandwidth = 250000;
      
      LMTSMGR_MSG_2(HIGH, "Sending WWAN_TECH msg with band %d channel %d", 
                      wlan_msg_ptr->band,
                      wlan_msg_ptr->channels[0]);
               
      num_channels = 2;
    }
    lmtsmgr_target_tech.tech_data.num_link_info_sets = num_channels;
  }
  else
  {
    /* Tx is OFF */
    lmtsmgr_target_tech.tech_data.num_link_info_sets = LMTSMGR_ZERO_LINK_INFO_SET;
  }


  LMTSMGR_MSG_1(HIGH, "lmtsmgr_update_cxm_abt_tech_tx SEND MSG TO CXM num_channels: %d \n", 
                           lmtsmgr_target_tech.tech_data.num_link_info_sets);

  lmtsmgr_msgr_send_msg(&lmtsmgr_target_tech.msg_hdr,
                        (msgr_umid_type)MCS_CXM_WWAN_TECH_STATE_IND,
                        sizeof(cxm_wwan_tech_state_ind_s));

}
#endif /* FEATURE_MCS_STX_TO_CXM_CONVERT */

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_tx_on

=============================================================================*/
/*!
    @brief
    Sets the Tx state of the tech and triggers LMTSMGR core to run.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_tx_on
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  /* void recieved message will be cast to this */
  cxm_stx_tech_state_ind_s *lmtsmgr_target_tech = NULL;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = 0;

  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  /* this will hold the details for converting to wwan_tech_st ind to 
     send this to WLAN for SAR */
  cxm_stx_tech_state_chs_ind_s wlan_msg_s;
  #endif

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
    LMTSMGR_MSG_0(ERROR, "Set Tech State was passed a NULL pointer\n");
    return;
  }

  /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_stx_tech_state_ind_s *) lmtsmgr_rcvd_msg;
  tech_id = lmtsmgr_target_tech->tech_id;


  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  /* Copy to WLAN msg struct */
  wlan_msg_s.tech_id = tech_id;
  wlan_msg_s.tx_rfdev = lmtsmgr_target_tech->tx_rfdev;
  wlan_msg_s.band = lmtsmgr_trans_rf_bands_to_sys_bands(tech_id,
                                                   lmtsmgr_target_tech->band);
  wlan_msg_s.num_channels = 1;
  wlan_msg_s.channels[0] = lmtsmgr_target_tech->channel;

  /* Update CXM regarding Tx ON state to send message to WLAN */
  lmtsmgr_update_cxm_abt_tech_tx (&wlan_msg_s);
#endif 

  LMTSMGR_MSG_1(MED, "Ignoring Tech %d STX_SET_TX_ON indication", tech_id);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_tx_on_with_chs

=============================================================================*/
/*!
    @brief
    Sets the Tx state of the tech and triggers LMTSMGR core to run. Accepts an
    array of channels
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_tx_on_with_chs
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  /* void recieved message will be cast to this */
  cxm_stx_tech_state_chs_ind_s *lmtsmgr_target_tech = NULL;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = 0;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
    LMTSMGR_MSG_0(ERROR, "Set Tech State was passed a NULL pointer\n");
    return;
  }

  /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_stx_tech_state_chs_ind_s *) lmtsmgr_rcvd_msg;
  tech_id = lmtsmgr_target_tech->tech_id;

  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  lmtsmgr_target_tech->band = lmtsmgr_trans_rf_bands_to_sys_bands(tech_id,
                                               lmtsmgr_target_tech->band);

  /* Update CXM regarding Tx ON state to send message to WLAN */
  lmtsmgr_update_cxm_abt_tech_tx (lmtsmgr_target_tech);
  #endif

  LMTSMGR_MSG_1(MED, "Ignoring Tech %d STX_SET_TX_ON_CHS indication", tech_id);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_tx_off

=============================================================================*/
/*!
    @brief
    Clears the Tx state of the tech and triggers LMTSMGR core to run.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_tx_off
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
/* void recieved message will be cast to this */
  cxm_stx_tech_txoff_ind_s *lmtsmgr_target_tech = NULL;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = 0;

  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  /* this will hold the details for converting to wwan_tech_st ind to 
     send this to WLAN for SAR */
  cxm_stx_tech_state_chs_ind_s wlan_msg_s;
  #endif

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
      LMTSMGR_MSG_0(ERROR, "Set Tech State was passed a NULL pointer\n");
      return;
  }

  /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_stx_tech_txoff_ind_s *) lmtsmgr_rcvd_msg;
  tech_id = lmtsmgr_target_tech->tech_id;

  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  /* Copy to WLAN msg struct */
  wlan_msg_s.tech_id = tech_id;
  wlan_msg_s.num_channels = 0;
  
  /* Update CXM regarding Tx OFF state to send message to WLAN */
  lmtsmgr_update_cxm_abt_tech_tx (&wlan_msg_s);
#endif 

  LMTSMGR_MSG_1(MED, "Ignoring Tech %d STX_SET_TX_OFF indication", tech_id);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_power_update

=============================================================================*/
/*!
    @brief
    Updates the power information of the tech and then figures out if anything
    needs to be transferred to the LMTSMGR Server.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_power_update
(
   void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  /* void recieved message will be cast to this */
  cxm_stx_set_pwr_ind_s *lmtsmgr_target_tech = NULL;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = 0;

  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  lmtsmgr_tech_list_type* currlist;
  uint32 i;
  #endif

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
    LMTSMGR_MSG_0(ERROR, "Set Tech Power was passed a NULL pointer\n");
    return;
  }

  /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_stx_set_pwr_ind_s *) lmtsmgr_rcvd_msg;
  tech_id = lmtsmgr_target_tech->tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* Check if the tech has Tx on. If Tx is not on, we should'nt be
     receiving power */
  if (lmtsmgr.tech_state[tech_id].currList.num_ul_entries == 0)
  {
    LMTSMGR_MSG_1(LOW, "Power received when tech %d's Tx is OFF.",tech_id);
    return;
  }
  
  /* Update the power in the tech's state info */
  switch (lmtsmgr_target_tech->valid_pwrs)
  {
    case STX_RX_VALID_ONLY:
        LMTSMGR_MSG_2(MED,"Received Rx pwr %d from tech %d", 
                             lmtsmgr_target_tech->rx_pwr, tech_id);
        break;

    case STX_TX_VALID_ONLY:
        lmtsmgr.tech_state[tech_id].pwr_info.tx_power = lmtsmgr_target_tech->tx_pwr;
        lmtsmgr.tech_state[tech_id].pwr_info.tx_pwr_valid = TRUE;
        LMTSMGR_MSG_2(MED,"Received Tx pwr %d from tech %d", 
                             lmtsmgr_target_tech->tx_pwr, tech_id);
        break;

    case STX_RX_TX_VALID_BOTH:
        lmtsmgr.tech_state[tech_id].pwr_info.tx_power = lmtsmgr_target_tech->tx_pwr;
        lmtsmgr.tech_state[tech_id].pwr_info.tx_pwr_valid = TRUE;
        LMTSMGR_MSG_3(MED,"Received Tx pwr %d Rx pwr %d from tech %d", 
                  lmtsmgr_target_tech->tx_pwr,lmtsmgr_target_tech->rx_pwr,tech_id);
        break;

    default:
        break;
  }

  /* Process SAR input */
  lmtsmgr_process_sar_input();


  #ifdef FEATURE_MCS_STX_TO_CXM_CONVERT
  if ((tech_id == CXM_TECH_GSM1) || (tech_id == CXM_TECH_GSM2) || (tech_id == CXM_TECH_GSM3))
  {
    currlist = &lmtsmgr.tech_state[tech_id].currList;
    if (currlist->num_ul_entries > 0)
    {
      /* Received power... sending coex power indication */
      for (i=0; i < currlist->num_entries; i++)
      {
        if ( (currlist->links[i].freqInfo.link_info.direction == CXM_LNK_DRCTN_UL) &&
             ( (lmtsmgr_target_tech->valid_pwrs == STX_TX_VALID_ONLY) ||
               (lmtsmgr_target_tech->valid_pwrs == STX_RX_TX_VALID_BOTH) ) )
        {
          currlist->links[i].powerInfo.tx_power = lmtsmgr_target_tech->tx_pwr;
          currlist->links[i].powerInfo.direction = CXM_LNK_DRCTN_UL;
        }
        else if ( (currlist->links[i].freqInfo.link_info.direction == CXM_LNK_DRCTN_DL) &&
                  ( (lmtsmgr_target_tech->valid_pwrs == STX_RX_VALID_ONLY) ||
                    (lmtsmgr_target_tech->valid_pwrs == STX_RX_TX_VALID_BOTH) ))
        {
          currlist->links[i].powerInfo.rx_power = lmtsmgr_target_tech->rx_pwr;
          currlist->links[i].powerInfo.direction = CXM_LNK_DRCTN_DL;
        }
      }
   
      /* Set the power change flag for the tech */   
      lmtsmgr.tech_state[tech_id].power_change = TRUE;   

      /* Log the power info */
      wwcoex_log_power(tech_id,WWCOEX_LOG_POWER_SUBCODE);

      /* Process this change in input */
      lmtsmgr_process_coex_input();
    }
  }
  #endif
}

/*=============================================================================

  FUNCTION:  lmtsmgr_gen_new_list

=============================================================================*/
/*!
    @brief
    Generate new freqids for the new list and store it in current list
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_gen_new_list
(
  cxm_tech_type   tech,
  cxm_tech_link_info_s *newList,
  uint32 num_newList_entries
)
{
  lmtsmgr_tech_state_type* tech_st;
  uint32 new_offset, i;
  boolean is_tx_active = FALSE;

  if (num_newList_entries == 0 || newList == NULL)
  {
    LMTSMGR_MSG_1(ERROR, "No entries in the new list for tech %d", tech);
    return;
  }

  TTLMARKER(LMTSMGR_GEN_NEW_LIST_IN);
  
  /* Get a pointer to the lmtsgr tech state */
  tech_st = &lmtsmgr.tech_state[tech];

  /* Compute the new offset */
  new_offset = MAX(tech_st->freqId_range_start, 
                   (tech_st->currList.freqId_offset + tech_st->currList.num_entries));

  /* Check if we will exceed the freqId range with the new list */
  if ( (new_offset + num_newList_entries)  > tech_st->freqId_range_end)
  {
    /* Looks like we will exceed the range so reset the offset to start of the range */
    new_offset = tech_st->freqId_range_start;
  }

  /* We need to start afresh so reset the counts */
  tech_st->currList.num_dl_entries = 0;
  tech_st->currList.num_ul_entries = 0;
  tech_st->currList.num_entries = 0;
  tech_st->currList.num_serving_dl_entries = 0;

  for (i=0; i < num_newList_entries; i++)
  {
    /* Copy the frequency info */
    tech_st->currList.links[i].freqInfo.link_info = newList[i];

    /* Assign new Id */
    tech_st->currList.links[i].freqInfo.freqid = new_offset + i;

    if (tech_st->currList.links[i].freqInfo.freqid & 0xFFFF0000)
    {
      MSG_FATAL("FreqId not within 16 bit range! 0x%x",
                tech_st->currList.links[i].freqInfo.freqid, 0,0);
    }

    /* Set the power to unknown */
    tech_st->currList.links[i].powerInfo.tx_power = CXM_UNKNOWN_POWER;
    tech_st->currList.links[i].powerInfo.rx_power = CXM_UNKNOWN_POWER;

    /* Set the rxtx state if there is atleast one UL freq in the list */
    if ( newList[i].direction == CXM_LNK_DRCTN_UL || 
         newList[i].direction == CXM_LNK_DRCTN_UL_AND_DL )
    {
      is_tx_active = TRUE;

      /* Add the frequency to the ulList */
      tech_st->currList.ul_freqList[tech_st->currList.num_ul_entries] = i;
      tech_st->currList.num_ul_entries++;
    }
    else
    {
      /* Add the frequency to the dlList */
      tech_st->currList.dl_freqList[tech_st->currList.num_dl_entries] = i;
      tech_st->currList.num_dl_entries++;
      
      if (tech_st->currList.links[i].freqInfo.link_info.type == 0)
      {
        tech_st->currList.num_serving_dl_entries++;
      }
    }

    /* Reset the direction in power infor to Invalid */
    tech_st->currList.links[i].powerInfo.direction = CXM_LNK_DRCTN_INVLD;
  }

  /* Store the new offset */
  tech_st->currList.freqId_offset = new_offset;

  /* Update the num entries in current list */
  tech_st->currList.num_entries = num_newList_entries;

  /* Set the flag indicating new list is ready */
  tech_st->freq_change = TRUE;
  lmtsmgr_reset_ddr_pwr_info(tech);
  if (!is_tx_active)
  {
    tech_st->currList.num_ul_entries = 0;
  }

  TTLMARKER(LMTSMGR_GEN_NEW_LIST_OUT);
  
  return;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_get_xo_error

=============================================================================*/
/*!
    @brief
    Get the XO frequency error and uncertainty from XO manager
    for the tech and store the results
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_get_xo_error
(
  cxm_tech_type   tech
)
{
  tcxomgr_client_info_struct_type  client_rgs_in;
  tcxomgr_vco_info_type  client_rgs_out;

  /* convert the tech ID and subscription ID to match TCXOMGR */
  lmtsmgr_trans_lmts_tech_to_xo_tech(tech,
                                      &client_rgs_in
                                     );
  /* Get the XO error and uncertainty for the client */
  client_rgs_out = tcxomgr_get_client_rgs(client_rgs_in);

  /* Store the XO error and uncertainty for the tech */
  lmtsmgr.tech_state[tech].currList.xo_error = client_rgs_out.rot_value;
  lmtsmgr.tech_state[tech].currList.xo_uncertainty = client_rgs_out.freq_uncertainty_ppb;
  
  return;
}


/*=============================================================================

  FUNCTION:  lmtsmgr_compare_lists

=============================================================================*/
/*!
    @brief
    Compares the received frequency list with the existing list to see if they
    exactly match
 
    @return
    TRUE - If the lists match exactly
    FALSE - otherwise
*/
/*===========================================================================*/
boolean lmtsmgr_compare_lists
(
  cxm_tech_type tech_id,
  cxm_tech_link_info_s *newList,
  uint32 num_newList_entries
)
{
  lmtsmgr_tech_state_type* tech_st;
  uint32 i;
  boolean result = TRUE;

  /* Get a pointer to the lmtsgr tech state */
  tech_st = &lmtsmgr.tech_state[tech_id];

  if ( (num_newList_entries == 0) || (newList == NULL) || 
       (num_newList_entries > CXM_MAX_SUPPORTED_LINK_SETS) || 
      (tech_st->currList.num_entries != num_newList_entries))
  {
    result = FALSE;
  }
  else
  {
     for (i=0; i<num_newList_entries; i++)
     {
        /* Check if the freq and direction are same ... */
        if ( (tech_st->currList.links[i].freqInfo.link_info.frequency != 
              newList[i].frequency) ||
             (tech_st->currList.links[i].freqInfo.link_info.bandwidth != 
              newList[i].bandwidth) ||
             (tech_st->currList.links[i].freqInfo.link_info.direction != 
              newList[i].direction) ||
             (tech_st->currList.links[i].freqInfo.link_info.channel != 
              newList[i].channel ) )
        {
          result = FALSE;
          break;
        }
     }
  }

  return result;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_comp_serv_freq_list

=============================================================================*/
/*!
    @brief
    Compares the received serving frequency list with the existing list
    to see if they
    exactly match
 
    @return
    TRUE - If the lists match exactly
    FALSE - otherwise
*/
/*===========================================================================*/
boolean lmtsmgr_comp_serv_freq_list
(
  cxm_tech_type tech_id,
  lmtsmgr_tech_ba_type *newList
)
{
  lmtsmgr_tech_ba_type* tech_ba_st;
  uint32 i, num_checks;
  boolean result = TRUE;

  /* Get a pointer to the lmtsgr band avoidance tech info */
  tech_ba_st = &lmtsmgr.band_avoidance.tech_info[tech_id];

  if ( (newList == NULL) || (newList->num_entries == 0) || 
       (newList->num_entries > CXM_MAX_SUPPORTED_LINK_SETS) || 
       (newList->num_dl_entries > CXM_MAX_SUPPORTED_LINK_SETS/2) ||
       (newList->num_ul_entries > CXM_MAX_SUPPORTED_LINK_SETS/2) ||
       (tech_ba_st->num_entries != newList->num_entries) ||
       (tech_ba_st->num_dl_entries != newList->num_dl_entries) ||
       (tech_ba_st->num_ul_entries != newList->num_ul_entries))
  {
    result = FALSE;
  }
  else
  {
     /* UL = DL entries */
     num_checks = newList->num_entries / 2;
     for (i = 0; i < num_checks; i++)
     {
       /* Power entry is a don't care */
       if ((tech_ba_st->serv_info_ul_list[i].band != newList->serv_info_ul_list[i].band) ||
           (tech_ba_st->serv_info_ul_list[i].bandwidth != newList->serv_info_ul_list[i].bandwidth) ||
           (tech_ba_st->serv_info_ul_list[i].channel != newList->serv_info_ul_list[i].channel) ||
           (tech_ba_st->serv_info_ul_list[i].frequency != newList->serv_info_ul_list[i].frequency) ||
           (tech_ba_st->serv_info_dl_list[i].band != newList->serv_info_dl_list[i].band) ||
           (tech_ba_st->serv_info_dl_list[i].bandwidth != newList->serv_info_dl_list[i].bandwidth) ||
           (tech_ba_st->serv_info_dl_list[i].channel != newList->serv_info_dl_list[i].channel) ||
           (tech_ba_st->serv_info_dl_list[i].frequency != newList->serv_info_dl_list[i].frequency)) 
       {
         result = FALSE;
         break;
       }
     }
  }
  return result;

}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_tech_state_ind

=============================================================================*/
/*!
    @brief
    Handles the Tx state and frequency indications
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_tech_state_ind
(
  void *lmtsmgr_rcvd_msg, /* struct we have recieved and are updating for */
  boolean is_sim_msg
)
{
  /* void recieved message will be cast to this */
  cxm_wwan_tech_state_ind_s *lmtsmgr_target_tech = NULL;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = CXM_TECH_DFLT_INVLD;

  uint32 prev_num_ul;

  /* Result of the compare operation on the rcvd list and the existing list 
     of frequencies */
  boolean compare_result;

  #ifdef FEATURE_MODEM_DDR_MGMT
  lmtsmgr_tech_state_type* tech_st;

  /* Used to determined LTE CA RF */
  trm_get_device_mapping_input_type input;
  uint16 i;
  #endif

  TTLMARKER(LMTSMGR_HTSI_IN);
  
  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Set Tech State was passed a NULL pointer\n");
     TTLMARKER(LMTSMGR_HTSI_ERR1);
     return;
  }

   /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_wwan_tech_state_ind_s *) lmtsmgr_rcvd_msg;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_target_tech == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "target_tech NULL pointer\n");
     TTLMARKER(LMTSMGR_HTSI_ERR2);
     return;
  }

  tech_id = lmtsmgr_target_tech->tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    TTLMARKER(LMTSMGR_HTSI_ERR3);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }
  
  /* Check if the list is the same as existing */
  compare_result = lmtsmgr_compare_lists(tech_id, 
                                         lmtsmgr_target_tech->tech_data.link_list,
                                         lmtsmgr_target_tech->tech_data.num_link_info_sets );

  /* Get XO error for the tech and add this error to center freq */
  lmtsmgr_get_xo_error(tech_id);

  if (compare_result == TRUE)
  {
    LMTSMGR_MSG_4(LOW, "Received same freq list Tech %d, Band %d, NumEntries %d, Freq0 %d ",
                   tech_id,
                   lmtsmgr_target_tech->tech_data.link_list[0].band,
                   lmtsmgr_target_tech->tech_data.num_link_info_sets,
                   lmtsmgr_target_tech->tech_data.link_list[0].frequency );

    /* send the freq id list... no need to do anything */
    if(!is_sim_msg)
    {
      lmtsmgr_send_freqId_list(tech_id);
    }

    TTLMARKER(LMTSMGR_HTSI_SAME_FREQIDS);
    
    return;
  }
  
  #ifdef FEATURE_MODEM_DDR_MGMT
  /* Enter Critical Section */
  LMTSMGR_ENTER_CRIT_SECT(lmtsmgr.crit_sect);
  #endif

  prev_num_ul = lmtsmgr.tech_state[tech_id].currList.num_ul_entries;

  if (lmtsmgr_target_tech->tech_data.num_link_info_sets == 0)
  {
    TTLMARKER(LMTSMGR_HTSI_TECH_OFF_IN);

    LMTSMGR_MSG_1(HIGH,"Received Tech %d Rx Off", tech_id );

    /* Set the state to inactive and empty the currList... */
    lmtsmgr.tech_state[tech_id].currList.num_entries = 0;
    lmtsmgr.tech_state[tech_id].currList.num_ul_entries = 0;
    lmtsmgr.tech_state[tech_id].currList.num_dl_entries = 0;

    /* Reset the macro state */
    lmtsmgr.tech_state[tech_id].macro_state = SP_BASELINE_STATE;
	
    /* Invalidate conflict tbls for this tech */
    wwcoex_invalidate_tables((uint8)tech_id);

    /* Reset the freqIdOffset */
    lmtsmgr.tech_state[tech_id].currList.freqId_offset = 
                                lmtsmgr.tech_state[tech_id].freqId_range_start;

    TTLMARKER(LMTSMGR_HTSI_TECH_OFF_OUT);
  }
  else
  {
    /* Tech is active... */
    TTLMARKER(LMTSMGR_HTSI_TECH_ON_IN);

    if (lmtsmgr_target_tech->tech_data.num_link_info_sets > 
                                     CXM_MAX_SUPPORTED_LINK_SETS)
    { 
      LMTSMGR_MSG_1(ERROR, "Incorrect number of link sets %d", 
                    lmtsmgr_target_tech->tech_data.num_link_info_sets);
      #ifdef FEATURE_MODEM_DDR_MGMT         
      /* Leave Critical Section */
      LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
      #endif
      return;
    }

    LMTSMGR_MSG_3(MED,"Received new freq list Tech %d, Band %d, NumEntries %d",
                  tech_id,
                  lmtsmgr_target_tech->tech_data.link_list[0].band,
                  lmtsmgr_target_tech->tech_data.num_link_info_sets  );

    /* Generate new freqIds for the new list */
    lmtsmgr_gen_new_list(tech_id, lmtsmgr_target_tech->tech_data.link_list,
                         lmtsmgr_target_tech->tech_data.num_link_info_sets);

    /* Log the freq info */
    wwcoex_log_power(tech_id,WWCOEX_LOG_FREQUENCY_SUBCODE);

    TTLMARKER(LMTSMGR_HTSI_TECH_ON_OUT);
  }

  #ifdef FEATURE_MODEM_DDR_MGMT

  tech_st = &lmtsmgr.tech_state[tech_id];

  /* Update Band & Channel info for the DDR */
  ddr_coex_update_tech_info(tech_id);

  /* Special Case - Need to determine if LTE CA is ON. */
  if (tech_id == CXM_TECH_LTE)
  {
    input.client = TRM_LTE_CA;
    input.resource = TRM_RX_CA_PRIMARY;

    /* Traverse through every LTE link entry */
    for (i = 0; i < (uint16) CXM_MAX_SUPPORTED_LINK_SETS; i++)
    {
      input.band = tech_st->currList.links[i].freqInfo.link_info.band;
      /* If the RF device in the entry is tied to LTE CA ..*/
      if(tech_st->currList.links[i].freqInfo.link_info.rf_device == trm_get_device_mapping(input))
      {
          LMTSMGR_MSG_0(MED, "LTE CA Frequency Entry Found!");
          /* Check if channel for LTE CA has changed. */
          if (tech_st->lte_ca_channel != tech_st->currList.links[i].freqInfo.link_info.channel)
          {
             LMTSMGR_MSG_0(MED, "LTE Channel changed!");
             tech_st->lte_ca_channel_changed = TRUE;
             tech_st->lte_ca_channel = tech_st->currList.links[i].freqInfo.link_info.channel;
          }
          else
          {
            tech_st->lte_ca_channel_changed = FALSE;
          }
          break;
      }
    }
  }
  #endif
  
  /* Process this change in input */
  lmtsmgr_process_txstate_change(tech_id, prev_num_ul, 
                          lmtsmgr.tech_state[tech_id].currList.num_ul_entries);

  #ifdef FEATURE_MODEM_DDR_MGMT
  /* Process this change in input */
  ddr_coex_process_ddr_input();

  /* Leave Critical Section */
  LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
  #endif /* FEATURE_MODEM_DDR_MGMT */

  /* Process coes input before sending freqid list to tech... */
  lmtsmgr_process_coex_input();

  /* Send the freqId list to tech... */
  if(!is_sim_msg)
  {
    lmtsmgr_send_freqId_list(tech_id);
  }

  lmtsmgr_process_sar_input();

  /* Keep this after sending the freqId */
  lmtsmgr_process_coex_tech_state_change();
  
   TTLMARKER(LMTSMGR_HTSI_OUT);
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_coex_power_update

=============================================================================*/
/*!
    @brief
    Handles the coex power indications
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_coex_power_update
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{ 
  /* void recieved message will be cast to this */
  cxm_coex_power_ind_s *lmtsmgr_target_tech = NULL;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = CXM_TECH_DFLT_INVLD;

  lmtsmgr_tech_list_type* foundList = NULL;

  uint32 i, searchId, index;

  lmtsmgr_tech_state_type* tech_st;
  
  TTLMARKER(LMTSMGR_HCPU_IN);

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Coex Power Ind was passed a NULL pointer\n");
     TTLMARKER(LMTSMGR_HCPU_ERR1);
     return;
  }

  /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_coex_power_ind_s *) lmtsmgr_rcvd_msg;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_target_tech == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "target_tech NULL pointer\n");
     TTLMARKER(LMTSMGR_HCPU_ERR2);
     return;
  }

  tech_id = lmtsmgr_target_tech->tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    TTLMARKER(LMTSMGR_HCPU_ERR3);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  if (lmtsmgr_target_tech->power_data.num_link_info_sets == 0)
  {
    LMTSMGR_MSG_1(ERROR, "No power information %d", tech_id);
    TTLMARKER(LMTSMGR_HCPU_ERR4);
    return;
  }

#ifdef FEATURE_MODEM_DDR_MGMT
  /* Enter Critical Section */
  LMTSMGR_ENTER_CRIT_SECT(lmtsmgr.crit_sect);
#endif

  /* Get the pointer to tech's state variable */
  tech_st = &lmtsmgr.tech_state[tech_id];

  if (tech_st->currList.num_entries == 0)
  {
#ifdef FEATURE_MODEM_DDR_MGMT
    /* Leave Critical Section */
    LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
#endif
    LMTSMGR_MSG_1(ERROR,"coex_power_update: tech %d sent pwr Update after Rx OFF", tech_id);
    TTLMARKER(LMTSMGR_HCPU_ERR5);
    return;
  }

  LMTSMGR_DBG_MSG_2(MED, "Received power update for tech %d num entries %d", 
                tech_id, lmtsmgr_target_tech->power_data.num_link_info_sets);

  /* Get XO error for the tech and add this error to center freq */
  lmtsmgr_get_xo_error(tech_id);

  for (i=0; i<lmtsmgr_target_tech->power_data.num_link_info_sets; i++)
  {
    searchId = lmtsmgr_target_tech->power_data.link_list[i].freqid;

    /* Check if the freqId is present in the current list */
    if (searchId >= tech_st->currList.freqId_offset && 
        searchId <= (tech_st->currList.freqId_offset + tech_st->currList.num_entries))
    {
      /* Ids are assigned sequentially so just indexing into the table should
         suffice */
      index = searchId - tech_st->currList.freqId_offset;

      foundList = &tech_st->currList;
    }
    else
    {
      /* Could not find the freqId so continue with next Id search */
      LMTSMGR_MSG_2(ERROR, "Could not find freqId %d for tech %d power update", 
                    searchId, tech_id);
      continue;
    }
    
    /* If we get here that means freqId has been found... we can update the power. */
    foundList->links[index].powerInfo = lmtsmgr_target_tech->power_data.link_list[i];
  } /* End for */

  if (foundList != NULL)
  {
    /* Set the power change flag for the tech */
    tech_st->power_change = TRUE;

#ifdef FEATURE_MODEM_DDR_MGMT
    tech_st->curr_pwr_update_time = time_get_ms_native();

    /* Process this change in input */
    ddr_coex_process_ddr_input();

    /* Leave Critical Section */
    LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
#endif

    /* Check whether Hopping needs to be disable and indicate TRM
       Should be outside the limitsmgr crit sect to avoid deadlock	*/
    lmtsmgr_update_hopping_behavior();

    /* Process this change in input */
    lmtsmgr_process_coex_input();

    /* Log the power info */
    wwcoex_log_power(tech_id,WWCOEX_LOG_POWER_SUBCODE);
  }
  else
  {
#ifdef FEATURE_MODEM_DDR_MGMT
    /* Leave Critical Section */
    LMTSMGR_LEAVE_CRIT_SECT(lmtsmgr.crit_sect);
#endif
    LMTSMGR_MSG_1(ERROR,"coex_power_update: tech %d sent pwr Update after Rx OFF", tech_id);
  }
  
  TTLMARKER(LMTSMGR_HCPU_OUT);
  return;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_activity_tl_update

=============================================================================*/
/*!
    @brief
    Handles the activity timeline indications
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_activity_tl_update
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  /* void recieved message will be cast to this */
  cxm_activity_timeline_s *lmtsmgr_target_tech = NULL;
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  wwcoex_activity_info_type   active_info;
#endif

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = CXM_TECH_DFLT_INVLD;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Activity Timline Ind was passed a NULL pointer\n");
     return;
  }

  /*-----------------------------------------------------------------------*/

  lmtsmgr_target_tech = (cxm_activity_timeline_s *) lmtsmgr_rcvd_msg;

  tech_id = lmtsmgr_target_tech->tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }
  
  /* Check if tech is transmitting */
  if (lmtsmgr.tech_state[tech_id].currList.num_ul_entries == 0)
  {
    LMTSMGR_MSG_1(ERROR, "Inactivity received when tech %d's Tx is OFF.",tech_id);
    return;
  }
  
  /* Register the activity information only if the disable mitigation mask is 0*/
  if (!(lmtsmgr.mitigation_mask & 1))
  {
    LMTSMGR_MSG_4(HIGH,"Received Tech %d Inactivity Time input: start %d, end %d, period %d",
                        tech_id,
                          lmtsmgr_target_tech->start_time,
                                      lmtsmgr_target_tech->end_time,
                                      lmtsmgr_target_tech->period );

#ifdef FEATURE_MODEM_COEXISTENCE_FW                               
    /* Register activity with wwcoex */
    active_info.start_time = lmtsmgr_target_tech->start_time;
    active_info.end_time = lmtsmgr_target_tech->end_time;
    active_info.period = lmtsmgr_target_tech->period;
    active_info.direction = lmtsmgr_target_tech->direction;

    wwcoex_register_activity(tech_id, active_info);
#endif
  }

  return;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_inactivity_reg

=============================================================================*/
/*!
    @brief
    Handles the inactivity registrations
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_inactivity_reg
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  return;
}


/*=============================================================================

  FUNCTION:  lmtsmgr_handle_slot_lvl_act_ind

=============================================================================*/
/*!
    @brief
    Handles the Tech Tx/Rx slot level activity indication from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_slot_lvl_act_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  cxm_timing_info_s* lmtsmgr_target_tech;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Slot Level Activity Ind was passed a NULL pointer");
     return;
  }

  lmtsmgr_target_tech = (cxm_timing_info_s *) lmtsmgr_rcvd_msg;

  /* copy data */
  lmtsmgr.tech_timing_info_index %= LMTSMGR_TIMING_HIST_RECS;
  memscpy(&lmtsmgr.tech_timing_info[lmtsmgr.tech_timing_info_index],
          sizeof(cxm_timing_info_s),lmtsmgr_target_tech, sizeof(cxm_timing_info_s));
  lmtsmgr.tech_timing_info_index++;

  lmtsmgr_process_slot_lvl_act_input(FALSE);
}


/*=============================================================================

  FUNCTION:  lmtsmgr_handle_slot_lvl_act_sim_ind

=============================================================================*/
/*!
    @brief
    Handles the Tech Tx/Rx slot level activity indication from simulator (FW)
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_slot_lvl_act_sim_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  cxm_sim_timing_info_s*  lmtsmgr_target_tech;
  cxm_timing_info_s*      lmtsmgr_timing_info;
  uint32 i;
#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
  #error code not present
#endif
  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
    LMTSMGR_MSG_0(ERROR, "Slot Level Activity Sim Ind was passed a NULL pointer");
    return;
  }

  lmtsmgr_target_tech = (cxm_sim_timing_info_s *) lmtsmgr_rcvd_msg;

  lmtsmgr.tech_timing_info_index %= LMTSMGR_TIMING_HIST_RECS;
  lmtsmgr_timing_info = &lmtsmgr.tech_timing_info[lmtsmgr.tech_timing_info_index];
  memset(lmtsmgr_timing_info, 0, sizeof(cxm_timing_info_s));
#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
  #error code not present
#endif

  /* copy data */
#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
  #error code not present
#endif

  lmtsmgr_timing_info->tech_id = (cxm_tech_type) lmtsmgr_target_tech->tech_id;
  lmtsmgr_timing_info->num_entries = lmtsmgr_target_tech->num_entries;

  memscpy(&lmtsmgr_timing_info->dtx_info, sizeof(msgr_hdr_struct_type),
          &lmtsmgr_target_tech->dtx_info, sizeof(msgr_hdr_struct_type));

  memscpy(&lmtsmgr_timing_info->dtx_info, sizeof(cxm_dtx_state_info),
          &lmtsmgr_target_tech->dtx_info, sizeof(cxm_sim_dtx_state_info));

  memscpy(&lmtsmgr_timing_info->tech_metrics, sizeof(cxm_tech_specific_metrics_u),
          &lmtsmgr_target_tech->tech_metrics, sizeof(cxm_sim_tech_specific_metrics_u));

    //TODO: Move feature to entire function
#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
  #error code not present
#endif /* FEATURE_LMTSMGR_SIMULATOR_SUPPORT */


  /* Generate info for Slot Level Timing */
  for(i = 0; i < lmtsmgr_target_tech->num_entries; i++)
  {
    /* FreqId is passed a pointer in sim mode */
#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
    #error code not present
#else
    if (lmtsmgr_target_tech->slot_list[i].freqid != NULL)
    {
      lmtsmgr_timing_info->slot_list[i].freqid = ((*(lmtsmgr_target_tech->slot_list[i].freqid)) & 0xFFFF);
    }
    else
    {
      lmtsmgr_timing_info->slot_list[i].freqid = CXM_UNKNOWN_FREQID;
    }
	
    lmtsmgr_timing_info->slot_list[i].frequency = lmtsmgr_target_tech->slot_list[i].frequency;
    lmtsmgr_timing_info->slot_list[i].bandwidth = lmtsmgr_target_tech->slot_list[i].bandwidth;
    lmtsmgr_timing_info->slot_list[i].band = lmtsmgr_target_tech->slot_list[i].band;
#endif /* FEATURE_LMTSMGR_SIMULATOR_SUPPORT */

    lmtsmgr_timing_info->slot_list[i].direction = 
      (cxm_tech_link_direction) lmtsmgr_target_tech->slot_list[i].direction;
    lmtsmgr_timing_info->slot_list[i].link_type = 
      (cxm_tech_link_type) lmtsmgr_target_tech->slot_list[i].link_type;

    lmtsmgr_timing_info->slot_list[i].start_time = lmtsmgr_target_tech->slot_list[i].start_time;
    lmtsmgr_timing_info->slot_list[i].end_time = lmtsmgr_target_tech->slot_list[i].end_time;
    lmtsmgr_timing_info->slot_list[i].micro_prio = lmtsmgr_target_tech->slot_list[i].micro_prio;

    lmtsmgr_timing_info->slot_list[i].wlan_prio = 
      (cxm_slot_prio_e) lmtsmgr_target_tech->slot_list[i].wlan_prio;

    lmtsmgr_timing_info->slot_list[i].pwr = lmtsmgr_target_tech->slot_list[i].pwr;
    lmtsmgr_timing_info->slot_list[i].conflict_mask = 
      lmtsmgr_target_tech->slot_list[i].conflict_mask;
  }

  lmtsmgr.tech_timing_info_index++;

  lmtsmgr_process_slot_lvl_act_input(TRUE);
}


/*=============================================================================

  FUNCTION:  lmtsmgr_reset_ddr_pwr_info

=============================================================================*/
/*!
    @brief
    Reset DDR pwr info related parameters
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_reset_ddr_pwr_info (cxm_tech_type client)
{

#ifdef FEATURE_MODEM_DDR_MGMT
  lmtsmgr_tech_state_type *tech_st;
#endif

  if (!LMTSMGR_IS_TECH_VALID(client))
  {
    return;
  }

#ifdef FEATURE_MODEM_DDR_MGMT
  tech_st = &lmtsmgr.tech_state[client];
  tech_st->pwr_thrshd_switchover_time = 0;
  tech_st->curr_pwr_update_time = 0;
  tech_st->thrshd_switchover_sign = LMTSMGR_DDR_RX_THRSHD_NEUTRAL;
  tech_st->band_changed = FALSE;
#endif
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_chain_owner_upd_ind

=============================================================================*/
/*!
    @brief
    Handles the chain owner updates
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_chain_owner_upd_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  cxm_chain_owner_msg_s* chain_owner_msg;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Chain owner update Ind was passed a NULL pointer");
     return;
  }

  /*-----------------------------------------------------------------------*/
  chain_owner_msg = (cxm_chain_owner_msg_s *) lmtsmgr_rcvd_msg;

  LMTSMGR_DBG_MSG_5(HIGH, "Rcvd chain owner update msg: ([0]=%d, [1]=%d, [2]=%d, [3]=%d, [4]=%d)",
                chain_owner_msg->owner.chain[0], 
                chain_owner_msg->owner.chain[1],
                chain_owner_msg->owner.chain[2],
                chain_owner_msg->owner.chain[3],
                chain_owner_msg->owner.chain[4]);

  memscpy(&lmtsmgr.chain_owner, TRM_MAX_CHAINS*sizeof(cxm_tech_type),
          &(chain_owner_msg->owner.chain), TRM_MAX_CHAINS*sizeof(cxm_tech_type));

  lmtsmgr_process_chain_owner_input();

}


/*============================================================================

FUNCTION CXM_GET_CONN_STATE_TBL

DESCRIPTION
  Function used by tech L1 to retrieve the UL/DL status of all techs
  
DEPENDENCIES
  None

RETURN VALUE 
  Table indicating UL/DL status of each tech 

SIDE EFFECTS
  This function is not mutex protected so results may not reflect
  the latest status

============================================================================*/
cxm_conn_tbl_type cxm_get_conn_state_tbl(void)
{
  uint32 techid;
  cxm_conn_tbl_type cxm_ul_dl_tbl;
  lmtsmgr_tech_list_type *tech_st;

  for (techid=0; techid<CXM_TECH_MAX; techid++)
  {
    /* Retrieve frequency info from tech state */
    tech_st = &lmtsmgr.tech_state[techid].currList;

    /* If UL/DL entries > 0, set UL/DL active state to TRUE
       in connection state table */
    cxm_ul_dl_tbl.conn_state[techid].is_ul_active = 
       (tech_st->num_ul_entries) ? TRUE : FALSE;

    cxm_ul_dl_tbl.conn_state[techid].is_dl_active = 
       (tech_st->num_dl_entries) ? TRUE : FALSE;
  }

  return cxm_ul_dl_tbl;

}


/*=============================================================================

  FUNCTION:  lmtsmgr_handle_band_avoidance_freq_ind

=============================================================================*/
/*!
    @brief
    Handles the serving frequency updates (for band avoidance)
 
    @details
    If the serving frequency message is invalid in any form, it will be ignored.
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_band_avoidance_freq_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  /* void recieved message will be cast to this */
  cxm_coex_ba_freq_ind_s *lmtsmgr_target_tech = NULL;
  coex_ba_serv_freq_list_s       *msgr_serv_freq_entry;

  /* this will hold the tech type to be modified, for quick access */
  cxm_tech_type tech_id = CXM_TECH_DFLT_INVLD;

  uint32 entry_pos;

  /* Result of the compare operation on the rcvd list and the existing list 
     of frequencies */
  boolean compare_result;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Serving Frequency was passed a NULL pointer");
     return;
  }

  /* if pointer is null, print error message do nothing */
  lmtsmgr_target_tech = (cxm_coex_ba_freq_ind_s *) lmtsmgr_rcvd_msg;
  if(lmtsmgr_target_tech == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "target_tech NULL pointer");
     return;
  }

  tech_id = lmtsmgr_target_tech->serv_freq_data.tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* Check if number of serving frequencies is valid */
  if(lmtsmgr_target_tech->serv_freq_data.num_entries > CXM_MAX_SUPP_FREQ_LINK_BA)
  {
    LMTSMGR_MSG_3(ERROR, "Invalid number of entries! Tech: %d Num Entries: %d (Max: %d)", 
                  tech_id, lmtsmgr_target_tech->serv_freq_data.num_entries, 
                  CXM_MAX_SUPP_FREQ_LINK_BA);
    return;
  }

  /* Convert msg info to limitsmgr ba format */
  memset(&newServFreqList, 0, sizeof(lmtsmgr_tech_ba_type));
  for (entry_pos=0; entry_pos<lmtsmgr_target_tech->serv_freq_data.num_entries; entry_pos++)
  {
    msgr_serv_freq_entry = &lmtsmgr_target_tech->serv_freq_data.freq_list[entry_pos];

    /* If direction is UL & DL, Rx frequency is based off of offset */
    if(msgr_serv_freq_entry->direction == CXM_LNK_DRCTN_UL_AND_DL)
    {
      if (newServFreqList.num_entries+2 > CXM_MAX_SUPP_FREQ_LINK_BA ||
          newServFreqList.num_ul_entries+1 > CXM_MAX_SUPP_FREQ_LINK_BA/2 ||
          newServFreqList.num_dl_entries+1 > CXM_MAX_SUPP_FREQ_LINK_BA/2)
      {
        LMTSMGR_MSG_5(ERROR, "Too many serving frequency entries! Tech: % dTotal: %d (Max: %d), UL: %d, DL: %d",
                      tech_id, newServFreqList.num_entries, CXM_MAX_SUPP_FREQ_LINK_BA, 
                      newServFreqList.num_ul_entries, newServFreqList.num_dl_entries);
        return;
      }

      /* Generate Rx serving freq info */
      switch (tech_id)
      {
        case CXM_TECH_GSM1:
        case CXM_TECH_GSM2:
        case CXM_TECH_GSM3:
          newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].frequency = 
          /* rfgsm_band_type=sys_band_class_e_type */
          rfm_gsm_convert_arfcn_to_rx_freq((rfgsm_band_type) msgr_serv_freq_entry->band,
                                          msgr_serv_freq_entry->channel);
          break;

        default:
          LMTSMGR_MSG_1(ERROR, "Unsupported tech %d for UL & DL direction!", tech_id);
      }


      if (newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].frequency == 0)
      {
        LMTSMGR_MSG_3(ERROR, "Unable to determine Rx frequency! Tech: %d Channel: %d SysBand: %d",
                      tech_id, msgr_serv_freq_entry->channel, msgr_serv_freq_entry->band);
        return;
      }
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].frequency/=1000; /* Convert to kHz */
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].band = msgr_serv_freq_entry->band;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].channel = msgr_serv_freq_entry->channel;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].bandwidth = msgr_serv_freq_entry->bandwidth;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].power = CXM_UNKNOWN_POWER;
      newServFreqList.num_entries++;
      newServFreqList.num_dl_entries++;

      /* Provided entry is TX info */
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].frequency = msgr_serv_freq_entry->frequency;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].band = msgr_serv_freq_entry->band;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].channel = msgr_serv_freq_entry->channel;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].bandwidth = msgr_serv_freq_entry->bandwidth;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].power = CXM_UNKNOWN_POWER;
      newServFreqList.num_entries++; 
      newServFreqList.num_ul_entries++;         
    }
    else if (msgr_serv_freq_entry->direction == CXM_LNK_DRCTN_UL)
    {
      if (newServFreqList.num_entries+1 > CXM_MAX_SUPP_FREQ_LINK_BA ||
           newServFreqList.num_ul_entries+1 > CXM_MAX_SUPP_FREQ_LINK_BA/2 )
      {
        LMTSMGR_MSG_2(ERROR, "Too many serving frequency entries! Total: %d DL: %d",
                      newServFreqList.num_entries, newServFreqList.num_ul_entries);
        return;
      }
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].frequency = msgr_serv_freq_entry->frequency;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].band = msgr_serv_freq_entry->band;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].channel = msgr_serv_freq_entry->channel;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].bandwidth = msgr_serv_freq_entry->bandwidth;
      newServFreqList.serv_info_ul_list[newServFreqList.num_ul_entries].power = CXM_UNKNOWN_POWER;
      newServFreqList.num_entries++; 
      newServFreqList.num_ul_entries++; 
    }
    else if (msgr_serv_freq_entry->direction == CXM_LNK_DRCTN_DL) 
    {
      if (newServFreqList.num_entries+1 > CXM_MAX_SUPP_FREQ_LINK_BA ||
           newServFreqList.num_dl_entries+1 > CXM_MAX_SUPP_FREQ_LINK_BA/2 )
      {
        LMTSMGR_MSG_2(ERROR, "Too many serving frequency entries! Total: %d DL: %d",
                      newServFreqList.num_entries, newServFreqList.num_dl_entries);
        return;
      }
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].frequency = msgr_serv_freq_entry->frequency;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].band = msgr_serv_freq_entry->band;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].channel = msgr_serv_freq_entry->channel;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].bandwidth = msgr_serv_freq_entry->bandwidth;
      newServFreqList.serv_info_dl_list[newServFreqList.num_dl_entries].power = CXM_UNKNOWN_POWER;
      newServFreqList.num_entries++; 
      newServFreqList.num_dl_entries++; 
    }
    else // Invalid
    {
      LMTSMGR_MSG_2(ERROR, "Invalid direciton %d! Pos: %d", msgr_serv_freq_entry->direction, entry_pos);
      return;
    }
  } /* End For */

  /* Verify the serving frequency contents */
  if (newServFreqList.num_entries != 0 && (newServFreqList.num_dl_entries != newServFreqList.num_ul_entries))
  {
    LMTSMGR_MSG_2(ERROR, "Uneven amount of serving frequencies! UL: %d DL: %d",
                  newServFreqList.num_ul_entries, newServFreqList.num_dl_entries);
  }

  /* If serving frequency information is the same, do nothing. */
  compare_result = lmtsmgr_comp_serv_freq_list(tech_id, &newServFreqList);
  if (compare_result)
  {
    LMTSMGR_MSG_4(MED, "Received same serv freq list Tech %d, Band %d, NumEntries %d, Freq %d ",
                   tech_id,
                   lmtsmgr_target_tech->serv_freq_data.freq_list[0].band,
                   lmtsmgr_target_tech->serv_freq_data.num_entries,
                   lmtsmgr_target_tech->serv_freq_data.freq_list[0].frequency );
    return;
  }

  /* Store the serving frequency information now that message is formatted and verified */
  if (newServFreqList.num_entries == 0)
  {
    LMTSMGR_MSG_1(HIGH, "Received Tech %d Serv Freq Off", tech_id );

    /* Set the tech serving freq to inactive and empty the serv freq list... */
    memset(&lmtsmgr.band_avoidance.tech_info[tech_id], 0, sizeof(lmtsmgr_tech_ba_type));
  }
  else
  {
    /* Tech is active... */

    LMTSMGR_MSG_3(MED,"Received new serv freq list Tech %d, Band %d, NumEntries %d",
                  tech_id,
                  lmtsmgr_target_tech->serv_freq_data.freq_list[0].band,
                  lmtsmgr_target_tech->serv_freq_data.num_entries );

    memscpy(&lmtsmgr.band_avoidance.tech_info[tech_id], sizeof(lmtsmgr_tech_ba_type),
            &newServFreqList, sizeof(lmtsmgr_tech_ba_type));

    lmtsmgr.band_avoidance.tech_info[tech_id].freq_changed = TRUE;
  }
  
  /* Log the serv freq info */
  wwcoex_log_serv_power(tech_id, WWCOEX_LOG_FREQUENCY_SUBCODE);

  lmtsmgr_process_band_avoidance_input();

  lmtsmgr.band_avoidance.tech_info[tech_id].freq_changed = FALSE;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_band_avoidance_pwr_ind

=============================================================================*/
/*!
    @brief
    Handles the serving power updates (for band avoidance)
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_band_avoidance_pwr_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  cxm_coex_ba_pwr_ind_s   *lmtsmgr_target_tech; /* Message cast to this type */
  coex_ba_serv_pwr_list_s *msgr_serv_pwr_entry; /* Points to entry in message */
  lmtsmgr_tech_ba_type    *lmtsmgr_serv_info;   /* Points to internal lmtsmgr */
  cxm_tech_type           tech_id = CXM_TECH_DFLT_INVLD;
  uint32                  i, entry_pos, num_dl_entries = 0, num_ul_entries = 0,
                          ul_pos = 0, dl_pos = 0;
  boolean                 fill_flag_enabled;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Serving Power was passed a NULL pointer");
     return;
  }

  /* if pointer is null, print error message do nothing */
  lmtsmgr_target_tech = (cxm_coex_ba_pwr_ind_s *) lmtsmgr_rcvd_msg;
  if(lmtsmgr_target_tech == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "target_tech NULL pointer");
     return;
  }

  tech_id = lmtsmgr_target_tech->serv_pwr_data.tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* Validate directions in message */
  for (entry_pos=0; entry_pos<lmtsmgr_target_tech->serv_pwr_data.num_entries; entry_pos++)
  {
    switch (lmtsmgr_target_tech->serv_pwr_data.pwr_list[entry_pos].direction)
    {
      case CXM_LNK_DRCTN_UL:
        num_ul_entries++;
        break;
      case CXM_LNK_DRCTN_DL:
        num_dl_entries++;
        break;
      default:
        LMTSMGR_MSG_3(ERROR, "Invalid direction! Tech: %d Dir: %d Pos: %d", tech_id,
                      lmtsmgr_target_tech->serv_pwr_data.pwr_list[entry_pos].direction, entry_pos);
        return;
    }
  }

  /* Validate power list:
       - If BA_PWR_LIST_FILL_ALL flag is set, there should be only one UL & DL entry
         to be used to fill the list 
       - If BA_PWR_LIST_FILL_ALL is not set, there should be a 1:1 mapping of power
         to serving frequency entries */
  fill_flag_enabled = (lmtsmgr_target_tech->serv_pwr_data.handle_flag & BA_PWR_LIST_FILL_ALL);
  if (((fill_flag_enabled) && 
       (num_ul_entries != 1 || num_dl_entries != 1)) ||
      ((!fill_flag_enabled) &&
       ((num_ul_entries != lmtsmgr.band_avoidance.tech_info[tech_id].num_ul_entries) ||
       (num_dl_entries != lmtsmgr.band_avoidance.tech_info[tech_id].num_dl_entries)))
     )
  {
    LMTSMGR_MSG_4(ERROR, "Invalid power list! Tech: %d Flag: 0x%x UL: %d DL: %d",
                  tech_id,lmtsmgr_target_tech->serv_pwr_data.handle_flag, 
                  num_ul_entries, num_dl_entries);
    return;
  }

  /* Now that serving power info is validated, add power info to lmtsmgr struct */
  lmtsmgr_serv_info = &lmtsmgr.band_avoidance.tech_info[tech_id];
  for (entry_pos=0; entry_pos<lmtsmgr_target_tech->serv_pwr_data.num_entries; entry_pos++)
  {
    /* Take an entry from the power message */
    msgr_serv_pwr_entry = &lmtsmgr_target_tech->serv_pwr_data.pwr_list[entry_pos];

    if (fill_flag_enabled)
    {
      //Fill the Rx/Tx power to all serving frequencies
      for (i = 0; i<lmtsmgr_serv_info->num_ul_entries; i++) /* num_ul_entries == num_dl_entries */
      {
        if (msgr_serv_pwr_entry->direction == CXM_LNK_DRCTN_UL) 
        {
          lmtsmgr_serv_info->serv_info_ul_list[i].power = msgr_serv_pwr_entry->tx_pwr;
        }
        else // direction is DL
        {
          lmtsmgr_serv_info->serv_info_dl_list[i].power = msgr_serv_pwr_entry->rx_pwr;
        }
      }
    }
    else // fill flag is disabled
    {
      if (msgr_serv_pwr_entry->direction == CXM_LNK_DRCTN_UL) 
      {
        lmtsmgr_serv_info->serv_info_ul_list[ul_pos].power = msgr_serv_pwr_entry->tx_pwr;
        ul_pos++;
      }
      else // direction is DL
      {
        lmtsmgr_serv_info->serv_info_dl_list[dl_pos].power = msgr_serv_pwr_entry->rx_pwr;
        dl_pos++;
      }
    }
  }

  lmtsmgr_serv_info->power_changed = TRUE;

  /* Log the serv pwr info */
  wwcoex_log_serv_power(tech_id,WWCOEX_LOG_POWER_SUBCODE);

  lmtsmgr_process_band_avoidance_input();

  lmtsmgr_serv_info->power_changed = FALSE;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_band_avoidance_blist_rsp

=============================================================================*/
/*!
    @brief
    Handles the band avoidance response from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_band_avoidance_blist_rsp
(
  void *lmtsmgr_rcvd_msg
)
{

  cxm_coex_ba_blist_res_s   *lmtsmgr_target_tech; /* Message cast to this type */
  lmtsmgr_tech_ba_type      *lmtsmgr_serv_info;   /* Points to internal lmtsmgr */
  cxm_tech_type             tech_id = CXM_TECH_DFLT_INVLD;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Blacklist Response was passed a NULL pointer");
     return;
  }

  /* if pointer is null, print error message do nothing */
  lmtsmgr_target_tech = (cxm_coex_ba_blist_res_s *) lmtsmgr_rcvd_msg;
  if(lmtsmgr_target_tech == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "target_tech NULL pointer");
     return;
  }

  tech_id = lmtsmgr_target_tech->blist_res_data.tech_id;

  /* Check if tech_id is valid */
  if (( tech_id <= CXM_TECH_DFLT_INVLD ) || ( tech_id >= CXM_TECH_MAX ))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  /* TODO: Revisit when WCDMA2 client is supported */
  if ( tech_id == CXM_TECH_WCDMA2 )
  {
    LMTSMGR_MSG_1(ERROR, "Invalid tech %d", tech_id);
    return;
  }

  if ((lmtsmgr_target_tech->blist_res_data.freq_avoid_res != FREQ_AVOID_NACK) &&
      (lmtsmgr_target_tech->blist_res_data.freq_avoid_res != FREQ_AVOID_NO_PROBLEM))
  {
    LMTSMGR_MSG_1(ERROR, "Invalid BA Response %d", 
                  lmtsmgr_target_tech->blist_res_data.freq_avoid_res);
    return;
  }

  /* Copy response data over to internal struct */
  lmtsmgr_serv_info = &lmtsmgr.band_avoidance.tech_info[tech_id];
  lmtsmgr_serv_info->blist_rsp.bl_id = 
    lmtsmgr_target_tech->blist_res_data.blacklist_id;
  lmtsmgr_serv_info->blist_rsp.response =
    lmtsmgr_target_tech->blist_res_data.freq_avoid_res;

  /* Set Blacklist Response Flag Changed to true */
  lmtsmgr_serv_info->ba_rsp_changed = TRUE;

  /* Log the tech's Blacklist Response */
  wwcoex_log_serv_power(tech_id, WWCOEX_LOG_BLIST_RSP_SUBCODE);

  lmtsmgr_process_band_avoidance_input();

}


#ifdef FEATURE_DUAL_DATA

/*==============================================================================

  FUNCTION:  lmtsmgr_add_desense_client

==============================================================================*/
/*!
    @brief
    process registration for desense indication.

    @detail
    
    @return
    None
*/
/*============================================================================*/
static void lmtsmgr_add_desense_client
(
  coex_desense_ind_reg_s  *reg_info
)
{
  uint32  i;
  uint32  *count;
  boolean is_desense;
  cxm_coex_desense_ind_s    client_ind;
  
  if (reg_info == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Desense ind req received a null pointer");
    return;
  }

  /* Get the current count */
  count = &(lmtsmgr.desense_ind.count);

  /* Check if the UMID is already registered to avoid 
     multiple registrations from the same client */
  for (i=0;i<*count;i++)
  {
    if(lmtsmgr.desense_ind.client[i].umid == reg_info->umid)
    {
      LMTSMGR_MSG_1(ERROR, "Desense ind UMID is already registered: %d", 
                    reg_info->umid);
      return;
    }
  }

  /* Check that there is enough space to add the client 
     The code should never hit this condition.*/
  if(*count >= LMTSMGR_NUM_DESENSE_CLIENTS)
  {
    LMTSMGR_MSG_0(ERROR, "Desense client buffer full");
    return;
  }

  /* Clients are stored in an array */
  lmtsmgr.desense_ind.client[*count].umid = reg_info->umid;
  *count = (*count + 1);

  /* Return the current desense status */
  is_desense = (lmtsmgr.desense_stats.status == SEVERE_DESENSE) ? TRUE:FALSE; 
  
  /* Populate the response */
  client_ind.desense_ind.is_desense = is_desense;
       
  /* Do a message router send on the clients UMID */
  lmtsmgr_msgr_send_msg(&client_ind.msg_hdr, 
                         reg_info->umid,
                         sizeof(cxm_coex_desense_ind_s));
  
  return;
}

/*==============================================================================

  FUNCTION:  lmtsmgr_remove_desense_client

==============================================================================*/
/*!
    @brief
    process deregistration for desense indication.

    @detail
    
    @return
    None
*/
/*============================================================================*/
static void lmtsmgr_remove_desense_client
(
  coex_desense_ind_reg_s  *dereg_info
)
{
  uint32   i;
  uint32   *count;
  int32    index = -1;

  /* If the input is invalid then return */
  if (dereg_info == NULL)
  {
    LMTSMGR_MSG_0(ERROR, "Desense ind req received a null pointer");
    return;
  }

  count = &(lmtsmgr.desense_ind.count);

  /* If there are no registered clients then return */
  if(*count == 0)
  {
    LMTSMGR_MSG_0(ERROR, "No clients have registered for desense indication");
    return;
  }
  else
  {
    /* If the last client in the list is deregistering 
       then decrement the count and do nothing */
    if(lmtsmgr.desense_ind.client[*count-1].umid == dereg_info->umid)
    {
      index = *count-1;
    }
    /* If the client is in between then rearrange the list */
    else
    {
      for(i=0;i<*count-1;i++)
      {
        if(lmtsmgr.desense_ind.client[i].umid == dereg_info->umid)
        {
          index = i;
          break;
        }
      }
    }
  }

  /* check if the umid was found */
  if(index == -1)
  {
    LMTSMGR_MSG_1(ERROR, "UMID not found to deregister desense indication: %d",
                  dereg_info->umid);
  }
  else
  {

    /* If the umid was found then reorder the array and decrement the count */
    for(i=index;i<*count-1;i++)
    {
      lmtsmgr.desense_ind.client[i].umid = lmtsmgr.desense_ind.client[i+1].umid;
    }
    
    *count=*count-1;
  }
 
  return;
}

#endif /* FEATURE_DUAL_DATA */


/*==============================================================================

  FUNCTION:  lmtsmgr_handle_desense_ind_req

==============================================================================*/
/*!
    @brief
    process registration for desense indication.

    @detail
    Clients could register to get updates about desense indication using MSGR.
    This API process the request from MSGR.
    When a coarse level desense is deteced MCS would update the registered clients


    @return
    None
*/
/*============================================================================*/
void lmtsmgr_handle_desense_ind_req
(
  void* lmtsmgr_rcvd_msg   /*!< pointer to the registration message */
)
{

#ifdef FEATURE_DUAL_DATA

  cxm_coex_desense_ind_req_s   *desense_req; /* Message cast to this type */

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Desense ind req received a null pointer");
     return;
  }

  /* Cast the msgr input to desense indication request type */
  desense_req = (cxm_coex_desense_ind_req_s *)lmtsmgr_rcvd_msg;

  /* Process registration request */
  if(desense_req->desense_reg.is_reg == TRUE)
  {
    lmtsmgr_add_desense_client(&(desense_req->desense_reg));
  }

  /* Process deregistration request */
  else
  {
    lmtsmgr_remove_desense_client(&(desense_req->desense_reg));
  }
  
#else /* FEATURE_DUAL_DATA */

  if(lmtsmgr_rcvd_msg)
  {
  }
  
#endif /* FEATURE_DUAL_DATA */

} 

/*=============================================================================

  FUNCTION:  lmtsmgr_handle_distress_ind

=============================================================================*/
/*!
    @brief
    Handles the distress indication from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_handle_distress_ind
(
  void *lmtsmgr_rcvd_msg /* struct we have recieved and are updating for */
)
{
  cxm_tech_distress_ind_s    *distress_msg;

  /* if pointer is null, print error message do nothing */
  if(lmtsmgr_rcvd_msg == NULL)  
  {
     LMTSMGR_MSG_0(ERROR, "Tech in distress ind req received a null pointer");
     return;
  }
  distress_msg = (cxm_tech_distress_ind_s *)lmtsmgr_rcvd_msg;

  if ( (!LMTSMGR_IS_TECH_VALID(distress_msg->tech_id)) ||       
       (distress_msg->distress_lvl > DISTRESS_HIGH) )
  {
    LMTSMGR_MSG_2(ERROR, "Invalid techid %d or distress level %d", 
                  distress_msg->tech_id,
                  distress_msg->distress_lvl);
    return;
  }

  /* Set the distress state only if it has changed... */
  if (lmtsmgr.tech_state[distress_msg->tech_id].distress_state != distress_msg->distress_lvl)
  {
    lmtsmgr.tech_state[distress_msg->tech_id].distress_state = distress_msg->distress_lvl;
    LMTSMGR_MSG_2(HIGH, " Tech %d set the distress flag to %d", 
                  distress_msg->tech_id,
                  lmtsmgr.tech_state[distress_msg->tech_id].distress_state);

    /* Check if micro priorities need to be adjusted */
    lmtsmgr_update_client_prio_table();
	
    /* Update the tech state indication if it has UL/DL */
    if (lmtsmgr.tech_state[distress_msg->tech_id].currList.num_entries > 0)
    {
      lmtsmgr_process_coex_tech_state_change();
    }
  }
}


