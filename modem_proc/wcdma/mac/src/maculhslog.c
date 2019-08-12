/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         U P L I N K    M A C   H S

GENERAL DESCRIPTION

    This module contains the data definitions and procedures for the performing
    the following functions

    1. HSUPA Configuration and setup


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2005 by Qualcomm Technologies, Inc. All Rights Reserved.
Copyright (c) 2007-2009 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                     EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maculhslog.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
11/06/14   aa       Replaced 0x4321 log packet with enhanced version 0x41C2
07/26/14   rkmk     Reduce image size by using right macro depending upon number of parameters
05/13/14   ymu      Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/04/14   ts       Removal of redundant f3s / putting them under right debug flags, fix for NS BO logging issue
02/24/14   ts       Changes for 0x430E log packet redesign
11/20/13   kv       Moved mask for MSG_4to9 from MSG_SSID_DIAG to MSG_SSID_WCDMA_MAC
07/17/13   kv       Fixed loggin issue for ETFCI and Happy Bit       
05/22/13   kc       Added DC-HSUPA changes
03/01/13   mrg      0x4309 ETFCI reason filed, populate this value correctly
09/14/12   ts       Fixed KW errors
10/12/12   kc       HS_RACH MAC F3's moved under debug variable and x4314 log packet issue
08/23/12   mrg      MAC-iis: Special HE functionality with LI 7 configured and 
                    RLC PDU size greater than 126 octets
08/22/12   kc       New log packet 0x4314 for HS RACH MAC timer info
08/13/12   mrg      HS-RACH & MAC-I Feature updated code check-in.
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12   mrg      HS-RACH & MAC-I Feature code integrated
03/15/11   ssg      Mainlining FEATURE_WCDMA_HSUPA_2MS_TTI.
02/28/11   mrg      Guard check added for number of samples in a log packet
04/13/10   ssg      Allocate memory for "etfci_num_samples" for 0x4323.
03/29/10   mrg      reverting double buffer log commit changes
02/23/10   mrg      Lint error fixed
02/19/10   mrg      log_commit both the buffers when buffers had valid log samples (ex: at reconifg times)
01/07/10   mrg      Request log_alloc() for MAX_NUM_SAMPLES of 0x4322
05/29/09   mrg      Due to overwritting of log smaples in ISR ,there is a mismatch 
                    of the log smaple values before log_commit.So dirt_bit concept is added.
05/07/09   ssg      Updated Copyright Information
04/29/09   av       Fixed compiler warnings in L2 
02/05/09   mrg      In the MAC EUL logging for the log sampels,valid checks are added.
11/12/08   mrg      Revert back to old version#3. Log samples are copied into
                    MAC internal DS and sending them to diag.
10/20/08   mrg      Num_samples are reset to zero after committing the
                    EUL MAC log packets.
09/12/08   mrg      log_status() check is modified avoid unnecessary memory 
                    allocation and log packet preperation.
07/29/08   mrg      MAC-EUL log_packet data structures are taken out from the
                    l2_ul_cmd_data_type and maintained interanally to MAC.
03/30/07   sk       Remove unwanted F3's
03/22/07   sk       Fix for the null pointer access.
03/19/07   sk       Log pkt fixes
02/10/07   sk       Logging support for the internal log packet - 0x4323
01/31/07   sk       Logging support for log code 0x4321, 0x4322
12/12/05   sk      Initial revision

===========================================================================*/


/*===========================================================================

                  INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_WCDMA_HSUPA

#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
#include "task.h"
#include "err.h"
#include "macrrcif_v.h"
#include "macul.h"
#include "maculhs.h"
#include "maculhslog.h"
#include "maculsubtask.h"
#include "maculhssubtask.h"


/*===========================================================================

              DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                         GLOBAL DATA
---------------------------------------------------------------------------*/
extern   mac_hs_ul_log_header_uncompressed_pkt_type   mac_hs_ul_header_log_info,
                                                      mac_hs_ul_header_prev_log_info;

extern   mac_hs_ul_log_etfci_status_uncompressed_pkt_type  mac_hs_ul_etfci_status_log_info,
                                                           mac_hs_ul_etfci_status_prev_log_info;

extern   boolean              mac_hs_ul_current_etfci_log_buf_flag;
extern   boolean              mac_hs_ul_current_hdr_log_buf_flag;

#ifdef FEATURE_MAC_I
/* MAC-e/es or MAC-i/is PDU */
extern   mac_ul_pdu_hdr_e_type               mac_hs_ul_pdu_type;
extern  boolean                              mac_hs_ul_maci_current_log_buf_flag;
extern  mac_hs_ul_maci_logpkt_uncompressed_pkt_type          mac_hs_ul_maci_is_log_packet_info,
                                                      mac_hs_ul_maci_is_prev_log_packet_info;
boolean               maci_ss_convert_to_apex_flag = TRUE;
extern   boolean         maci_logging_debug_f3_enable;
#endif  /*  FEATURE_MAC_I  */

#ifdef FEATURE_WCDMA_DC_HSUPA
extern mac_hs_ul_sec_carrier_info_type mac_hs_ul_sec_car_info;
#endif

#ifdef FEATURE_WCDMA_HS_RACH
extern boolean hs_rach_mac_timers_cur_log_buf_flag;
extern boolean hs_rach_mac_timers_log_pkt_status;
extern mac_hs_ul_hsrach_mac_timer_log_pkt_type  hs_rach_mac_timers_log_pkt_info, hs_rach_mac_timers_prev_log_pkt_info;
extern boolean mac_hs_rach_f3_enable;
#endif

/*===========================================================================

                     GENERAL PURPOSE MACROS

===========================================================================*/

/*---------------------------------------------------------------------------

---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                         LOCAL FUNCTIONS
---------------------------------------------------------------------------*/



/*===========================================================================
FUNCTION: mac_hs_send_config_log_packet

DESCRIPTION:


DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:

===========================================================================*/
void mac_hs_ul_send_config_log_packet
(
  const mac_e_config_s_type   *hs_ul_config_ptr,
  boolean mac_e_reset_indicator
)
{
  WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1_type *mac_hs_ul_cfg_log_ptr;
  uint8                                     *log_ptr;
  
  uint16                                    i,j,k;
  
  const mac_e_ul_logch_info_s_type          *ue_logch_ptr;

  uint8                                     num_log_ch=0,
                                            num_ddi = 0,
                                            num_fixed_ch = 0,
                                            num_flex_ch = 0;
  uint32                                    pkt_len=0;
  
  //Compute the number of the DDI and Number of logical channel
  for (i=0; i<hs_ul_config_ptr->num_e_mac_d_flow; i++)
  {
    num_log_ch += hs_ul_config_ptr->e_mac_d_flow_info[i].num_logch;
    
     /* For each logical channel mapped to this MAC-d flow */
    for (j=0; j<hs_ul_config_ptr->e_mac_d_flow_info[i].num_logch; j++)
    {
       ue_logch_ptr = &hs_ul_config_ptr->e_mac_d_flow_info[i].e_ul_logch_info[j];
       num_ddi += ue_logch_ptr->num_rlc_size_ddi_info;
       num_flex_ch += (ue_logch_ptr->rlc_pdu_type == RLC_FLEXIBLE_PDU_SIZE) ? 1 : 0;
       num_fixed_ch += (ue_logch_ptr->rlc_pdu_type == RLC_FIXED_PDU_SIZE) ? 1 : 0;
    }
  }
  
  pkt_len =
  MAC_HS_UL_CHAN_CFG_INFO_LOG_PKT_V1_SIZE(hs_ul_config_ptr->num_e_mac_d_flow,num_log_ch, num_ddi, num_flex_ch, num_fixed_ch);
  
  // Debug later why the log_alloc is returning the NULL pointer
  mac_hs_ul_cfg_log_ptr = 
  (WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1_type *)log_alloc(WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1, pkt_len);
  
  if (mac_hs_ul_cfg_log_ptr != NULL)
  {
    log_ptr = (uint8*)mac_hs_ul_cfg_log_ptr;
    log_ptr += FPOS(WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1_type, ver);
    
    /*Write the version number*/
    *log_ptr++ = 1;
    
    /* 0 = 2ms TTI, 1= 10msTTI */
    *log_ptr++= ((hs_ul_config_ptr->e_tti == E_TTI_2MS) ? 0 : 1 );
    
    /*Indicates which standardized E-TFCI TB size table shall be used*/
    *log_ptr++ = hs_ul_config_ptr->e_dch_info.e_dpdch_info.e_tfci_table_index;
    
    /*Happy bit delay condition over which to evaluate the current
    serving grant. Value is reported in milliseconds. The following values are possible:
    2ms,10ms,20ms,50ms,100ms,200ms,500ms,1000ms */
    *log_ptr++ = (uint8)(hs_ul_config_ptr->e_dch_info.e_dpcch_info.happy_bit_delay_in_ms & 0x00FF);
    *log_ptr++ = (uint8)((hs_ul_config_ptr->e_dch_info.e_dpcch_info.happy_bit_delay_in_ms & 0xFF00) >> 8);
    
    /* Number of MAC-d flows configured. Range: 0..7 */
    *log_ptr++ = hs_ul_config_ptr->num_e_mac_d_flow;
    
    /* E-DCH minimum set E-TFCI.
    0xFF indicates that minimum set E-TFCI is not provided by NW.*/
    *log_ptr++ = hs_ul_config_ptr->e_dch_info.e_dpdch_info.e_tfci_min_set;
    
    /*Periodicity for Scheduling Info when no grant is available. The range is 0..1000
    is mapped as follows: 0: no periodic reporting configured 1: every TTI 4: 4ms 10: 10ms
    20: 20ms 50: 50ms 100: 100ms 200: 200ms 500: 500ms 1000: 1000ms */
    *log_ptr++ =
      (uint8)(hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.no_grant_periodicity_of_si & 0x00FF);
    *log_ptr++ =
      (uint8)((hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.no_grant_periodicity_of_si & 0xFF00) >> 8);
      
    /* Periodicity for Scheduling Info when grant is available. Same range as above.*/
    *log_ptr++ =
       (uint8)(hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.grant_periodicity_of_si & 0x00FF);
    *log_ptr++ =
       (uint8)((hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.grant_periodicity_of_si & 0xFF00) >> 8);
    
    /*Power offset in case SI only is transmitted. Range: 0..6 corresponding to 0dB..6dB*/
    *log_ptr++ =
      hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.power_offset_of_si;
      
    /*MAC-E or MAC-I*/
    #ifdef FEATURE_MAC_I
    *log_ptr++ = mac_hs_ul_pdu_type;
    #else
    *log_ptr++ = 0;
    #endif
      
    /*MAC_E RESET Indicator*/
    *log_ptr++ = mac_e_reset_indicator;

    /*TSN Size: 6 bits(no DC-HSUPA) or 14 bits(DC-HSUPA)*/
    #ifdef FEATURE_WCDMA_DC_HSUPA
    if (TRUE == mac_hs_ul_sec_car_info.tsn_field_extn_flag)
    {
      *log_ptr++ = 14;
    }
    else
    #endif
    {
      *log_ptr++ = 6;
    }
      
    #ifdef FEATURE_MAC_I
    MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                 "MAC_I_CFG: TTI %d,	ETFCI Table %d,HB dealy %d,N_flows %d,Min_ETFCI %d,Sched_HQ_Alloc %d,T_SING %d, T_SIG %d,SI_PO %d",
                 hs_ul_config_ptr->e_tti,hs_ul_config_ptr->e_dch_info.e_dpdch_info.e_tfci_table_index,
                 hs_ul_config_ptr->e_dch_info.e_dpcch_info.happy_bit_delay_in_ms, hs_ul_config_ptr->num_e_mac_d_flow,
                 hs_ul_config_ptr->e_dch_info.e_dpdch_info.e_tfci_min_set, hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_tx_config.sched_tx_grant_harq_allocation_2ms,
                 hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.no_grant_periodicity_of_si,
                 hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.grant_periodicity_of_si,
                 hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_info_config.power_offset_of_si);
    #endif /*  FEATURE_MAC_I  */
    
    /* For each E-DCH MAC-d flow */
    for (i=0;i<hs_ul_config_ptr->num_e_mac_d_flow; i++)
    {
      /*log_ptr->e_mac_d_flow_info[i].e_mac_d_flow_id = */
      *log_ptr++ =
        hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_id;
      
      /*log_ptr->e_mac_d_flow_info[i].e_mac_d_flow_power_offset = */
      *log_ptr++ =
        hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_power_offset;
      
      /*log_ptr->e_mac_d_flow_info[i].e_mac_d_flow_max_num_of_retx = */
      *log_ptr++ =
        hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_max_num_of_retx;
      
      /*log_ptr->e_mac_d_flow_info[i].e_mac_d_flow_mux_list,
        ensuring that the bit corresponding to this MAC-D flow is always set */
      *log_ptr++ =
        (hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_mux_list) |
        (128 >> (hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_id));
      
      /*log_ptr->e_mac_d_flow_info[i].e_tx_grant_info = */
      if (hs_ul_config_ptr->e_mac_d_flow_info[i].e_tx_grant_info == MAC_E_TX_GRANT_NON_SCHEDULED)
      {
        *log_ptr++ = 1;
      }
      else
      {
        *log_ptr++ = 0;
      }
      
      #ifdef FEATURE_MAC_I
      MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                  "MAC_I_CFG: macd_id %d,PO %d, max_retx %d, mux_list %d,Grant_typ %d,num_lc %d,NSG %d,NS_harq_alloc %d",
                  hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_id, hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_power_offset,
                  hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_max_num_of_retx, hs_ul_config_ptr->e_mac_d_flow_info[i].e_mac_d_flow_mux_list,
                  hs_ul_config_ptr->e_mac_d_flow_info[i].e_tx_grant_info, hs_ul_config_ptr->e_mac_d_flow_info[i].num_logch,
                  hs_ul_config_ptr->e_mac_d_flow_info[i].non_sched_tx_grant.max_mac_e_pdu_size_in_bits,
                  hs_ul_config_ptr->e_mac_d_flow_info[i].non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms, 0);
      #endif /*  FEATURE_MAC_I  */
      
      /*log_ptr->e_mac_d_flow_info[i].num_logch =*/
      *log_ptr++ =
        hs_ul_config_ptr->e_mac_d_flow_info[i].num_logch;
      
      /*log_ptr->e_mac_d_flow_info[i].max_mac_e_pdu_size_in_bits= */
      *log_ptr++ =
        (uint8)(hs_ul_config_ptr->e_mac_d_flow_info[i].non_sched_tx_grant.max_mac_e_pdu_size_in_bits & 0x00FF);
      *log_ptr++ =
        (uint8)((hs_ul_config_ptr->e_mac_d_flow_info[i].non_sched_tx_grant.max_mac_e_pdu_size_in_bits & 0xFF00) >> 8);
      
      if (hs_ul_config_ptr->e_mac_d_flow_info[i].e_tx_grant_info == MAC_E_TX_GRANT_NON_SCHEDULED)
      {
      /*log_ptr->e_mac_d_flow_info[i].non_sched_tx_grant_harq_allocation_2ms=*/
      *log_ptr++ =
        hs_ul_config_ptr->e_mac_d_flow_info[i].non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms;
      }
      else
      {
        /*Scheduled TX HARQ process allocation (2ms TTI only).
          Bit 0 (LSB) corresponds to HARQ process 0, bit 1 corresponds to HARQ process 1, …etc.
          0: Process is disabled1: Process is enabled*/
        *log_ptr++ = hs_ul_config_ptr->e_dch_info.e_dpdch_info.sched_tx_config.sched_tx_grant_harq_allocation_2ms;
      }
      
      /* For each logical channel mapped to this MAC-d flow */
      for (j=0;j<hs_ul_config_ptr->e_mac_d_flow_info[i].num_logch;j++)
      {
        ue_logch_ptr = &hs_ul_config_ptr->e_mac_d_flow_info[i].e_ul_logch_info[j];
        
        /*log_logch_ptr->rb_id */
        *log_ptr++= ue_logch_ptr->rb_id;
        
	/*log_logch_ptr->lc_id */
        *log_ptr++= ue_logch_ptr->lc_id;
        
        /*log_logch_ptr->rlc_id */
        *log_ptr++ =  ue_logch_ptr->rlc_id;
        
        /*log_logch_ptr->priority */
        *log_ptr++ = ue_logch_ptr->priority;
        
        /*log_logch_ptr->chan_type */
        if (ue_logch_ptr->chan_type == UE_LOGCHAN_DCCH)
        {
          *log_ptr++ = 0;
        }
        else if (ue_logch_ptr->chan_type == UE_LOGCHAN_DTCH)
        {
          *log_ptr++ = 1;
        }
	else if (ue_logch_ptr->chan_type == UE_LOGCHAN_CCCH)
        {
          *log_ptr++ = 2;
        }
        else
        {
          *log_ptr++ = 3;
        }
        
        if (UE_MODE_TRANSPARENT == ue_logch_ptr->rlc_mode)
        {
          *log_ptr++ = 0;
        }
        else if (UE_MODE_UNACKNOWLEDGED == ue_logch_ptr->rlc_mode)
        {
          *log_ptr++ = 1;
        }
	else if (ue_logch_ptr->rlc_mode < UE_MODE_DL_CTCH)
	{
	  *log_ptr++ = 2;
	}
        else
        {
           *log_ptr++ = 3;
        }			
        
        /*log_logch_ptr->include_in_scheduling_info */
        *log_ptr++= ue_logch_ptr->include_in_scheduling_info;
        
        #ifdef FEATURE_MAC_I
        MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                    "MAC_I_CFG: RB Id %d,LC_Id%d,P %d,LC_Type %d,RLC Mode%d, Include_in_SI %d, rlc_pdu_type %d, N-DDI %d",
                    ue_logch_ptr->rb_id, ue_logch_ptr->rlc_id, ue_logch_ptr->priority, ue_logch_ptr->chan_type,
                    ue_logch_ptr->rlc_mode, ue_logch_ptr->include_in_scheduling_info, ue_logch_ptr->rlc_pdu_type, ue_logch_ptr->num_rlc_size_ddi_info, 0);
        
        if ((mac_hs_ul_pdu_type == MAC_I_PDU_HDR) && (ue_logch_ptr->rlc_pdu_type == RLC_FLEXIBLE_PDU_SIZE))
        {
          /*PDU Size - FLEXIBLE*/
          *log_ptr++ = 1;
          
          /*Min and Max PDU size for Flexible PDU*/
          *log_ptr++= (uint8)(ue_logch_ptr->ul_flex_min_pdu_size & 0x00FF);
          *log_ptr++= (uint8)((ue_logch_ptr->ul_flex_min_pdu_size & 0xFF00) >> 8);
          *log_ptr++= (uint8)(ue_logch_ptr->ul_flex_max_pdu_size & 0x00FF);
          *log_ptr++= (uint8)((ue_logch_ptr->ul_flex_max_pdu_size & 0xFF00) >> 8);
          
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_CFG: Li_Size%d,Min_Pdu_Size%d,Max_Pdu_Size %d",
                                                     ue_logch_ptr->ul_li_size,
                                                     ue_logch_ptr->ul_flex_min_pdu_size,
                                                     ue_logch_ptr->ul_flex_max_pdu_size);
        }
        else if ((mac_hs_ul_pdu_type == MAC_E_PDU_HDR) || 
                 ((mac_hs_ul_pdu_type == MAC_I_PDU_HDR) && (ue_logch_ptr->rlc_pdu_type == RLC_FIXED_PDU_SIZE)))
        #endif /*  FEATURE_MAC_I  */
        {
          /*PDU Size - FIXED*/
	  *log_ptr++ = 0;
          
          /*log_logch_ptr->num_rlc_size_ddi_info */
          *log_ptr++=  ue_logch_ptr->num_rlc_size_ddi_info;
          
          for (k=0;k< ue_logch_ptr->num_rlc_size_ddi_info;k++)
          {
            /*log_logch_ptr->rlc_size_ddi_info[k].ddi */
            *log_ptr++  = ue_logch_ptr->rlc_size_ddi_info[k].ddi;
            /*log_logch_ptr->rlc_size_ddi_info[k].rlc_pdu_size_in_bits =*/
            *log_ptr++= (uint8)(ue_logch_ptr->rlc_size_ddi_info[k].rlc_pdu_size_in_bits & 0x00FF);
            *log_ptr++= (uint8)((ue_logch_ptr->rlc_size_ddi_info[k].rlc_pdu_size_in_bits & 0xFF00) >> 8);
            
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_CFG: DDI %d, PDU_SIZE %d",
                                                       ue_logch_ptr->rlc_size_ddi_info[k].ddi,
                                                       ue_logch_ptr->rlc_size_ddi_info[k].rlc_pdu_size_in_bits);
          
          } // num rlc size ddi info
        }
      } // num logical channels mapped to this mac-d flow
    } // num mac d flows
    
    log_commit(mac_hs_ul_cfg_log_ptr);
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC HS Logging of config pkt failed");
  }
  //log ptr != NULL
}//mac_hs_send_config_log_packet

/*===========================================================================
FUNCTION:   mac_ul_hs_process_header_log_packet

DESCRIPTION:
This fucntion is called from l2ultask.c when the status log timer expires

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_ul_process_header_log_packet(void)
{
  uint8        *log_ptr;
  uint16        idx, y, num_mac_hdrs = 0, num_si = 0, num_new_tx = 0;
  uint16        hdr_num_samples =0;
  uint32        pkt_len = 0;
  uint8         max_num_samples = 0, max_mac_e_hrds_per_sample =4;
  mac_hs_ul_log_header_info_per_tti_type            *hdr_ptr;
  mac_hs_ul_log_header_uncompressed_pkt_type        *hdr_log_ptr;
  WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET_type     *mac_hs_ul_hdr_status_ptr;
  
  if (mac_hs_ul_current_hdr_log_buf_flag == TRUE)
  {
    hdr_log_ptr = &(mac_hs_ul_header_prev_log_info);
  }
  else
  {
    hdr_log_ptr = &(mac_hs_ul_header_log_info);
  }
  
  // set the dirty_bit to TRUE so that it will not be overwritten
  hdr_log_ptr->dirty_bit = TRUE;
  
  hdr_num_samples = hdr_log_ptr->num_samples;
  
  if(((hdr_log_ptr->tti == E_TTI_10MS) && 
       (hdr_log_ptr->num_samples > MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT_10MS_TTI))
    ||((hdr_log_ptr->tti == E_TTI_2MS) && 
       (hdr_log_ptr->num_samples > MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT)))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Num log samples exceeds count,num_samples = %d ",
                                                hdr_log_ptr->num_samples);
    // clear the dirty_bit and num_samples
    hdr_log_ptr->num_samples = 0;
    hdr_log_ptr->dirty_bit = FALSE;
    return;
  }
  
  if (hdr_log_ptr->num_samples > 0)
  {
    for (idx = 0 ; idx < hdr_num_samples; idx++)
    {
       if (hdr_log_ptr->hdr_info[idx].ctrl_info.tx_type==0)
       {
         num_new_tx++ ;
         if (hdr_log_ptr->hdr_info[idx].ctrl_info.si_present)
         {
           num_si++ ;
         }
         num_mac_hdrs += hdr_log_ptr->hdr_info[idx].num_mac_es_hdrs;
       }
    }
    
    if (hdr_log_ptr->tti == E_TTI_2MS)
    {
       max_num_samples = MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT;
    }
    else
    {
       max_num_samples = MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT_10MS_TTI;
    }
    
    pkt_len =
      MAC_HS_UL_HDR_INFO_LOG_PKT_SIZE(max_num_samples,(max_num_samples*max_mac_e_hrds_per_sample), max_num_samples, max_num_samples);
  
    mac_hs_ul_hdr_status_ptr =
    (WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET_type *)log_alloc(
          WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET,pkt_len);
  
    log_ptr = (uint8 *)mac_hs_ul_hdr_status_ptr;
  
    if (log_ptr != NULL)
    {
      log_ptr += FPOS(WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET_type, nsamp_tti_etfci_tbl);
      // Assign the number of samples
      //    log_ptr->nsamp_tti_etfci_tbl
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, NUM_SAMPLES, hdr_num_samples);
      // Mark if it is 2ms or 10ms tti
      if (hdr_log_ptr->tti == E_TTI_2MS)
      {
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, TTI, 0);
      }
      else
      {
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, TTI, 1);
      }
  
      //Mark the ETFCI Table
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, ETFCI_TABLE,
                                      hdr_log_ptr->etfci_table_idx);
  
      // Increment to the next byte
      log_ptr++;
  
      // Mark if it is 2ms or 10ms tti
      if (hdr_log_ptr->tti == E_TTI_2MS)
      {
        *log_ptr++ = hdr_log_ptr->start_fn & 0xFF;
        *log_ptr = (hdr_log_ptr->start_fn & 0xF00) >> 8;
      }
      else
      {
        *log_ptr++ = hdr_log_ptr->start_fn;
        *log_ptr = 0;
      }
  
      // Store the start harq id
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, HARQ_ID,
                                      hdr_log_ptr->harq_id);
  
      log_ptr++;
  
      for (idx = 0 ; idx < hdr_num_samples; idx++)
      {
        hdr_ptr = &hdr_log_ptr->hdr_info[idx];
  
        // Store the happy bit tx type
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                  HDR_LOG, TX_TYPE, hdr_ptr->ctrl_info.tx_type);
  
        // Store the happy bit
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                  HDR_LOG, HB, hdr_ptr->ctrl_info.happy_bit);
  
        //Go to the next byte
        log_ptr++;
  
        if (hdr_ptr->ctrl_info.tx_type == 0 )
        {
  
          // Store the ETFCI value
          *log_ptr++ = hdr_ptr->ctrl_info.etfci;
  
          //log_hdr_status_ptr->num_mac_hdrs_si_present
  
          // Store the number of mac-es headers and the si present bitmask
          MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                    HDR_LOG, MAC_ES, hdr_ptr->num_mac_es_hdrs);
  
          // Store the number of mac-es headers and the si present bitmask
          MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                    HDR_LOG, SI_PRESENT, hdr_ptr->ctrl_info.si_present);
  
          // GO to the next byte
          log_ptr++;
  
          if (hdr_ptr->ctrl_info.si_present)
          {
  
            //log_hdr_status_ptr->si_content[0]
            // Set the HLID
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                  HDR_LOG, HLID, hdr_ptr->ctrl_info.hlid);
  
            // Set HLBS value
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, HLBS,hdr_ptr->ctrl_info.hlbs);
  
            //Go to the next byte
            log_ptr++;
  
  
            //Set TEBS value - TBD
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, TEBS,hdr_ptr->ctrl_info.tebs);
  
            // Set SI triggered
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, SI_TRIG,hdr_ptr->ctrl_info.si_triggered);
  
            // Set SI padded
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, SI_PAD,hdr_ptr->ctrl_info.si_padding);
  
            //Go to the next byte
            log_ptr++;
  
  
            //log_hdr_status_ptr->si_content[1]
  
            // Set UPH value - TBD
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, UPH,hdr_ptr->ctrl_info.uph);
  
            // Go to the next byte
            log_ptr++;
  
            //decrement the num_si count
            num_si--;
  
          }
  
          // Fill the DDI,N and TSN field.
          for (y=0 ; y < ( MIN(hdr_ptr->num_mac_es_hdrs, max_mac_e_hrds_per_sample)); y++)
          {
            *log_ptr++ = hdr_ptr->hdr_data[y].ddi;
            *log_ptr++ = hdr_ptr->hdr_data[y].tsn;
            *log_ptr++ = (uint8)hdr_ptr->hdr_data[y].npdus;
          }
          
          //decrement the num_mac_es_hdrs count
          num_mac_hdrs = num_mac_hdrs - hdr_ptr->num_mac_es_hdrs;
          //decrement the num_new_tx count
          num_new_tx--;
          
        } // Tx type is new transmission
      }
      
      if ((num_new_tx != 0) || (num_si != 0) || (num_mac_hdrs != 0))
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "num_new_tx %d, num_si %d, num_mac_hdrs %d",
                                                   num_new_tx,
                                                   num_si,
                                                   num_mac_hdrs);
      }
  
      log_commit(mac_hs_ul_hdr_status_ptr);
      
      // clear the dirty_bit and num_samples
      hdr_log_ptr->num_samples = 0;
      hdr_log_ptr->dirty_bit = FALSE;
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Unable to allocate memory for the log_alloc ");
    }
  }
}

/*===========================================================================
FUNCTION:   mac_ul_hs_process_etfci_status_log_packet

DESCRIPTION:
This fucntion is called from l2ultask.c when the status log timer expires

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_ul_process_etfci_status_log_packet(void)
{
  uint8          *log_ptr;
  uint16         idx, num_new_tx = 0, etfci_num_samples =0;
  uint32         pkt_len = 0;
  WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET_type       *mac_hs_ul_etfci_status_ptr;
  mac_hs_ul_log_etfci_status_uncompressed_pkt_type   *etfci_log_ptr;
  mac_hs_ul_log_etfci_status_ctrl_info_per_tti_type  *ctrl_ptr;
  mac_hs_ul_log_etfci_status_pkt_per_tti_type        *pkt_ptr;


  if (mac_hs_ul_current_etfci_log_buf_flag == TRUE)
  {
    etfci_log_ptr = &(mac_hs_ul_etfci_status_prev_log_info);
  }
  else
  {
    etfci_log_ptr = &(mac_hs_ul_etfci_status_log_info);
  }

  // set the dirty_bit to TRUE so that it will not be overwritten
  etfci_log_ptr->dirty_bit = TRUE;
  
  etfci_num_samples = etfci_log_ptr->num_samples;

  if(((etfci_log_ptr->num_samples > MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT_10MS_TTI)
      && (etfci_log_ptr->tti == E_TTI_10MS)) ||((etfci_log_ptr->tti == E_TTI_2MS) && 
          (etfci_log_ptr->num_samples > MAC_HS_UL_NUM_REPORTS_IN_STATUS_LOG_PKT)))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Num log samples exceeds count,num_samples = %d ",
                                                etfci_log_ptr->num_samples);
    etfci_log_ptr->num_samples = 0;
    etfci_log_ptr->dirty_bit = FALSE;
    return;
  }
  
  if (etfci_log_ptr->num_samples > 0)
  {
    for (idx = 0 ; idx < etfci_num_samples; idx++)
    {
       if (etfci_log_ptr->ctrl_info[idx].tx_type==0)
       {
           num_new_tx++ ;
       }
    }
  
    pkt_len =
      MAC_HS_UL_ETFCI_STATUS_INFO_LOG_PKT_SIZE(etfci_num_samples, etfci_num_samples);
  
    mac_hs_ul_etfci_status_ptr =
    (WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET_type *)log_alloc(
          WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET,pkt_len);
  
    log_ptr = (uint8 *)mac_hs_ul_etfci_status_ptr;
  
    if (log_ptr != NULL)
    {
      log_ptr += FPOS(WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET_type, nsamp_tti_etfci_tbl);

      // Assign the number of samples
      //    log_ptr->nsamp_tti_etfci_tbl
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, NUM_SAMPLES, etfci_num_samples);
      // Mark if it is 2ms or 10ms tti
      if (etfci_log_ptr->tti == E_TTI_2MS)
      {
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, TTI, 0);
      }
      else
      {
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, TTI, 1);
      }
  
      //Mark the ETFCI Table
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,HDR_LOG, ETFCI_TABLE,
                                      etfci_log_ptr->etfci_table_idx);
  
      // Increment to the next byte
      log_ptr++;
  
      // Mark if it is 2ms or 10ms tti
      if (etfci_log_ptr->tti == E_TTI_2MS)
      {
        *log_ptr++ = etfci_log_ptr->start_fn & 0xFF;
        *log_ptr = (etfci_log_ptr->start_fn & 0xF00) >> 8;
      }
      else
      {
        *log_ptr++ = etfci_log_ptr->start_fn;
        *log_ptr = 0;
      }
  
      // Store the start harq id
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr,
                                      HDR_LOG, HARQ_ID,
                                      etfci_log_ptr->harq_id);
  
      log_ptr++;
  
      for (idx = 0 ; idx < etfci_num_samples; idx++)
      {
        ctrl_ptr = &etfci_log_ptr->ctrl_info[idx];
        pkt_ptr = &etfci_log_ptr->etfci_status_info[idx];
  
        /*SI_TRIG     Whether SI was triggered by buffer status or periodic trigger
        0: SI was not triggered by buffer status or periodic trigger
        1: SI was triggered due to either buffer status or periodic trigger*/
        *log_ptr = ctrl_ptr->si_trigger & 0x1;
  
        /*SI_PAD      Whether SI was added due to available padding
        0: SI was not appended due to available
        1: SI was appended due to available*/
        *log_ptr |= ((ctrl_ptr->si_pad & 0x1 ) << 1);
  
        /*HAPPY_BIT       Whether mobile is happy (1) or unhappy (0) with the current SG */
        *log_ptr |=  ((ctrl_ptr->happy_bit & 0x1) << 2);
  
        /*TX_TYPE     Indicates the type of transmission
          0: new transmission
          1: retransmission
          2: no transmission */
        *log_ptr |= ((ctrl_ptr->tx_type & 0x3) << 3);
  
  
        //Go to the next byte
        log_ptr++;
  
        if ((ctrl_ptr->tx_type == 0) || (ctrl_ptr->tx_type == 1))
        {
          *log_ptr = ctrl_ptr->etfci;
        }
        else
        {
          *log_ptr = 0;
        }
  
        log_ptr++;
  
        // Store the Sched buf status
        *log_ptr++ = (uint8)(ctrl_ptr->sched_buf_status & 0x00FF);
        *log_ptr++ = (uint8)((ctrl_ptr->sched_buf_status & 0xFF00) >> 8);
  
        // Store the non_sched_buf status
        *log_ptr++ = (uint8)(ctrl_ptr->non_sched_buf_status & 0x00FF);
        *log_ptr++ = (uint8)((ctrl_ptr->non_sched_buf_status & 0xFF00) >> 8);
  
  
        if (ctrl_ptr->tx_type == 0 )
        {
          // Store the L1 supported payload
          *log_ptr++ = (uint8)(pkt_ptr->l1_sup_payload & 0x00FF);
          *log_ptr++ = (uint8)((pkt_ptr->l1_sup_payload & 0xFF00) >> 8);
  
          // Store the SG payload
          *log_ptr++ = (uint8)(pkt_ptr->l1_sg_payload & 0x00FF);
          *log_ptr++ = (uint8)((pkt_ptr->l1_sg_payload & 0xFF00) >> 8);
  
          // Store the Sched payload txed after etfci selecltion
          *log_ptr++ = (uint8)(pkt_ptr->sched_payload & 0x00FF);
          *log_ptr++ = (uint8)((pkt_ptr->sched_payload & 0xFF00) >> 8);
  
          //Store the NonSched payload txed after etfci selection
          *log_ptr++ = (uint8)(pkt_ptr->ns_payload & 0x00FF);
          *log_ptr++ = (uint8)((pkt_ptr->ns_payload & 0xFF00) >> 8);
  
          //decrement the num_new_tx count
          num_new_tx--;
          
        }
      }
      
      if (num_new_tx != 0)
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "num_new_tx %d, etfci_num_samples %d",
                                                   num_new_tx,
                                                   etfci_num_samples);
      }
      
      log_commit(mac_hs_ul_etfci_status_ptr);
      
      // Clear the num_samples and dirty_bit 
      etfci_log_ptr->num_samples = 0;
      etfci_log_ptr->dirty_bit = FALSE;
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Unable to allocate memory for the log_alloc ");
    }
  }
}


#ifdef FEATURE_MAC_I

#ifdef FEATURE_WCDMA_DC_HSUPA

void mac_hs_ul_process_maci_is_log_packet(void)
{
  uint8          *log_ptr, num_si_present, continue_tx_type_indx, log_ch_hdr_idx,tti_idx;
  uint16         idx, num_new_tx, num_samples =0, num_re_tx, num_continue_tx, num_dtx, per_tti_idx = 0;
  uint32         pkt_len;
  
  WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type       *mac_hs_ul_maci_is_log_pkt_type_ptr;
  mac_hs_ul_maci_logpkt_uncompressed_pkt_type   *maci_is_log_ptr;
  mac_hs_ul_maci_hdr_logch_type      *maci_logch_hdr_ptr = NULL;
  mac_hs_ul_maci_pkt_per_tti_type    *maci_per_tti_ptr;
  
  idx = num_new_tx = num_re_tx = num_continue_tx = num_samples = pkt_len = num_dtx = tti_idx = 0;
  num_si_present = continue_tx_type_indx = log_ch_hdr_idx = 0;
  
  if (mac_hs_ul_maci_current_log_buf_flag == TRUE)
  {
    maci_is_log_ptr = &(mac_hs_ul_maci_is_prev_log_packet_info);
  }
  else
  {
    maci_is_log_ptr = &(mac_hs_ul_maci_is_log_packet_info);
  }
  
  // set the dirty_bit to TRUE so that it will not be overwritten
  maci_is_log_ptr->dirty_bit = TRUE;
  
  num_samples = maci_is_log_ptr->log_pkt_num_samples;
  
  if(maci_is_log_ptr->log_pkt_num_samples > MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT)
   {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Num log samples exceeds count,num_samples = %d ",
                                                maci_is_log_ptr->log_pkt_num_samples);
    maci_is_log_ptr->log_pkt_num_samples = 0;
    maci_is_log_ptr->dirty_bit = FALSE;
    maci_is_log_ptr->num_tti_logged = 0;
    return;
  }

  if(maci_is_log_ptr->num_tti_logged > MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT)
  { 
     MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: mismatch in MAX-40 allowed and num TTI %d and nsamples %d ",
                                                maci_is_log_ptr->num_tti_logged,
                                                maci_is_log_ptr->log_pkt_num_samples);
  }
  
  if(maci_is_log_ptr->num_tti_logged > 0)
  {
    for(idx = 0; idx < num_samples; idx++)
    {
      maci_per_tti_ptr = &(maci_is_log_ptr->maci_per_tti_info[per_tti_idx++]);
      if (maci_per_tti_ptr->ctrl_info.tx_type  == 0)
      {
        num_new_tx++;
        if (maci_per_tti_ptr->num_mac_i_hdrs > 0)
        {
          num_continue_tx += (maci_per_tti_ptr->num_mac_i_hdrs - 1);
          idx += (maci_per_tti_ptr->num_mac_i_hdrs - 1);
        }
        
        if(maci_per_tti_ptr->ctrl_info.si_present)
        {
          num_si_present++;
        }
      }
      else if (maci_per_tti_ptr->ctrl_info.tx_type  == 1)
      {
        num_re_tx++;
      }
      else if (maci_per_tti_ptr->ctrl_info.tx_type == 2)
      {
        num_dtx++;
      }
      else if(maci_per_tti_ptr->ctrl_info.tx_type == 3)
      {
        num_continue_tx++;
      }
    }

    pkt_len = MAC_HS_UL_DC_MACI_IS_INFO_LOG_PKT_SIZE(num_new_tx, num_re_tx, num_continue_tx, num_dtx);
    if (maci_logging_debug_f3_enable)
    {
    MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
          "MAC_DC_LOG: nsamples %d, new tx %d, continue tx %d, num_re_tx %d, num_dtx %d, num_si_present %d, num_tti_logged %d, pkt_len %d, per_tti_idx %d",
           maci_is_log_ptr->log_pkt_num_samples, num_new_tx, num_continue_tx,
           num_re_tx , num_dtx, num_si_present,
           maci_is_log_ptr->num_tti_logged, pkt_len, per_tti_idx);
    }

    if((num_continue_tx != maci_is_log_ptr->num_continue) && (maci_logging_debug_f3_enable))
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "number of continues do not match num_continue_tx - %d maci_is_log_ptr->num_continue - %d",
                                                 num_continue_tx,
                                                 maci_is_log_ptr->num_continue);
    }           
    mac_hs_ul_maci_is_log_pkt_type_ptr = (WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type *)log_alloc( WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET,pkt_len);
    log_ptr = (uint8 *)mac_hs_ul_maci_is_log_pkt_type_ptr;
    
    if(log_ptr != NULL)
    {
      log_ptr += FPOS(WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type, version);

      //Version 2 for DC 
      *log_ptr++ = 2;
      
      /*  num_sample_rrc_state  */
      *log_ptr = (maci_is_log_ptr->rrc_state & 0x03);
      *log_ptr |= ((maci_is_log_ptr->log_pkt_num_samples << 2) & (0xFC));
      log_ptr++;
      
      // Mark if it is 2ms or 10ms tti
      if (maci_is_log_ptr->tti == E_TTI_2MS)
      {
        *log_ptr++ = maci_is_log_ptr->start_frame_num & 0xFF;
        *log_ptr = (maci_is_log_ptr->start_frame_num & 0xF00) >> 8;
      }
      else
      {
        *log_ptr++ = maci_is_log_ptr->start_frame_num;
        *log_ptr = 0;
      }
      
      // Store the start harq id
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, HARQ_ID, maci_is_log_ptr->harq_id);
      *log_ptr |= ((maci_is_log_ptr->tti << 7) & 0x80);
      log_ptr++;
      
      for (idx = 0 ; idx < num_samples; idx++)
      {
        maci_per_tti_ptr = &(maci_is_log_ptr->maci_per_tti_info[tti_idx++]);
        
        *log_ptr = 0;
        *log_ptr = ( maci_per_tti_ptr->carrier_config_type & MAC_HS_UL_DC_CARRIER_CONFIG_MASK);
        *log_ptr |= ((maci_per_tti_ptr->carrier_index & MAC_HS_UL_DC_CARRIER_INDEX_MASK) << MAC_HS_UL_DC_CARRIER_INDEX_SHIFT);
        *log_ptr |= ((maci_per_tti_ptr->ctrl_info.tx_type & MAC_HS_UL_DC_CARRIER_TX_TYPE_MASK) << MAC_HS_UL_DC_CARRIER_TX_TYPE_SHIFT);
        *log_ptr |= ((maci_per_tti_ptr->ctrl_info.happy_bit & MAC_HS_UL_DC_CARRIER_HB_MASK) << MAC_HS_UL_DC_CARRIER_HB_SHIFT);
        log_ptr++;
        
        if((maci_per_tti_ptr->ctrl_info.tx_type == 2) || (maci_per_tti_ptr->ctrl_info.tx_type == 1))
        {
          //tx_type == 2 means DTX & - tx_type == 1 means Re-Tx 
          
          //Store ETFCI Reason
          *log_ptr = (maci_per_tti_ptr->etfci_restricted_reason & (0x0F));
          log_ptr++;
          
          // Store the Sched buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_buf_status & 0xFF00) >> 8);
          
          // Store the non_sched_buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->non_sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->non_sched_buf_status & 0xFF00) >> 8);
          
          //SUP_payload and SG_payload 
          *log_ptr++ = (uint8)(maci_per_tti_ptr->l1_sup_payload & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->l1_sup_payload & 0xFF00) >> 8);
          *log_ptr++ = (uint8)(maci_per_tti_ptr->l1_sg_payload& 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->l1_sg_payload & 0xFF00) >> 8);
          if (maci_logging_debug_f3_enable)
          {          
            MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                 "MAC_DC_LOG: RETX or DTX case  idx %d, etfci_reason %d, S buf %d, NS buf %d, sup payld %d, sg payld %d",
                  idx, (maci_per_tti_ptr->etfci_restricted_reason & (0x0F)), maci_per_tti_ptr->sched_buf_status, 
                   maci_per_tti_ptr->non_sched_buf_status, maci_per_tti_ptr->l1_sup_payload, maci_per_tti_ptr->l1_sg_payload, 
                   0,0, 0);
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_DC_LOG: N_sample %d DTX , log_ch_hdr_idx %d",
                                                         idx, log_ch_hdr_idx);
          }
          continue;
        }
        else if(maci_per_tti_ptr->ctrl_info.tx_type == 3)
        {
          //Write the TX-TYPE to CONTINUE for the Next MAC-I header.
          if (maci_logging_debug_f3_enable)
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_DC_LOG:N_sample %d CONTINUE_TX, type %d",
                                                       idx,
                                                       maci_per_tti_ptr->ctrl_info.tx_type);
          }
        }
        else if(maci_per_tti_ptr->ctrl_info.tx_type == 0)
        {
          //Store ETFCI Reason
          *log_ptr = (maci_per_tti_ptr->etfci_restricted_reason & (0x0F));
          log_ptr++;
          
          // Store the ETFCI value
          *log_ptr++ = maci_per_tti_ptr->ctrl_info.etfci;
          
          // Store the Sched buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_buf_status & 0xFF00) >> 8);
          
          // Store the non_sched_buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->non_sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->non_sched_buf_status & 0xFF00) >> 8);
          
          //SUP_payload and SG_payload 
          *log_ptr++ = (uint8)(maci_per_tti_ptr->l1_sup_payload & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->l1_sup_payload & 0xFF00) >> 8);
          *log_ptr++ = (uint8)(maci_per_tti_ptr->l1_sg_payload& 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->l1_sg_payload & 0xFF00) >> 8);
          
          // Store the Sched payload & NS_payload txed after etfci selecltion
          *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_payload & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_payload & 0xFF00) >> 8);
          *log_ptr++ = (uint8)(maci_per_tti_ptr->ns_payload & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->ns_payload & 0xFF00) >> 8);
          
          if (maci_per_tti_ptr->ctrl_info.si_present)
          {
            if(maci_per_tti_ptr->ctrl_info.si_triggered)
            {
              *log_ptr = 2;//SI_TRIG
            }
            else
            {
              *log_ptr = 1;//SI_PAD
            }
            
            *log_ptr |= (( maci_per_tti_ptr->ctrl_info.uph & MAC_HS_UL_HDR_LOG_UPH_BMASK) << 2);
            log_ptr++;
            
            //HLBS|HLID - each of 4bits
            *log_ptr = (maci_per_tti_ptr->ctrl_info.hlid & 0xF);
            *log_ptr |= ((maci_per_tti_ptr->ctrl_info.hlbs & 0xF) << 4);
            log_ptr++;
            
            //TEBS- 5bits
            *log_ptr = (maci_per_tti_ptr->ctrl_info.tebs & 0x1F);
            log_ptr++;
          }
          else
          {
            *log_ptr++ = 0;// NO-SI
            
            *log_ptr++ = 0; //HLBS | HLID

            *log_ptr++ = 0; // TEBS
          }

          if (maci_logging_debug_f3_enable)
          {
            MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                 "MAC_DC_LOG: NewTx case  idx %d, etfci %d, etfci_reason %d, S buf %d, NS buf %d, sup payld %d, sg payld %d, S payld %d, NS payld %d",
                  idx, maci_per_tti_ptr->ctrl_info.etfci, maci_per_tti_ptr->etfci_restricted_reason & (0x0F), 
                  maci_per_tti_ptr->sched_buf_status, maci_per_tti_ptr->non_sched_buf_status, maci_per_tti_ptr->l1_sup_payload,
                  maci_per_tti_ptr->l1_sg_payload, maci_per_tti_ptr->sched_payload, maci_per_tti_ptr->ns_payload);

            MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                "MAC_DC_LOG: NewTx case cont.. si_present %d, si_trigg %d, uph %d, hlid %d, hlbs %d, tebs %d",
                maci_per_tti_ptr->ctrl_info.si_present,maci_per_tti_ptr->ctrl_info.si_triggered, maci_per_tti_ptr->ctrl_info.uph,
                maci_per_tti_ptr->ctrl_info.hlid,maci_per_tti_ptr->ctrl_info.hlbs,maci_per_tti_ptr->ctrl_info.tebs,
                0,0,0);
          }
        }
        else
        {
          log_ptr++;
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_DC_LOG: Invalid TxTYPe %d, idx %d ",
                                                      idx,
                                                      maci_per_tti_ptr->ctrl_info.tx_type);
          continue;
        }
        
        for (continue_tx_type_indx = 0; 
             ((continue_tx_type_indx < maci_per_tti_ptr->num_mac_i_hdrs) && 
             (log_ch_hdr_idx < MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT)); continue_tx_type_indx++)
        {
          if(continue_tx_type_indx != 0)
          {
            //Write the TX-TYPE to CONTINUE for the Next MAC-I header.
            *log_ptr = ( maci_per_tti_ptr->carrier_config_type & MAC_HS_UL_DC_CARRIER_CONFIG_MASK);
            *log_ptr |= ((maci_per_tti_ptr->carrier_index & MAC_HS_UL_DC_CARRIER_INDEX_MASK) << MAC_HS_UL_DC_CARRIER_INDEX_SHIFT);
            *log_ptr |= ((0x03 & MAC_HS_UL_DC_CARRIER_TX_TYPE_MASK) << MAC_HS_UL_DC_CARRIER_TX_TYPE_SHIFT);
            *log_ptr |= ((maci_per_tti_ptr->ctrl_info.happy_bit & MAC_HS_UL_DC_CARRIER_HB_MASK) << MAC_HS_UL_DC_CARRIER_HB_SHIFT);
            log_ptr++;
            
            // Increment the index to jump to the next Continue sample					
            idx++;
            if (maci_logging_debug_f3_enable)
            {
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: Tx_Type continue, n maci_hdrs %d,idx %d ",
                                                         maci_per_tti_ptr->num_mac_i_hdrs,
                                                         idx);
            }
          }
          
          maci_logch_hdr_ptr = &(maci_is_log_ptr->maci_logch_hdr_info[log_ch_hdr_idx++]);
          
          //SS-TSN
          *log_ptr++ = (uint8) (maci_logch_hdr_ptr->tsn & 0x00FF);
          *log_ptr   = (uint8) ((maci_logch_hdr_ptr->tsn & 0x3F00) >> 8);
          *log_ptr++ |= ((maci_logch_hdr_ptr->ss & 0x03) << 6);
          
          //SDU-Size
          *log_ptr++ = (uint8)(maci_logch_hdr_ptr->maci_sdu_size & 0x00FF);
          *log_ptr = (uint8)((maci_logch_hdr_ptr->maci_sdu_size & 0x0700) >> 8);
          
          // LC-Id , ERNTI, N_SDU, F
          *log_ptr |= ((maci_logch_hdr_ptr->logch_id & 0x0F) << 3);
          *log_ptr |= ((maci_logch_hdr_ptr->ernti & 0x01) << 7);
          log_ptr++;
          *log_ptr = (maci_logch_hdr_ptr->n_sdu & 0x7F);
          *log_ptr |= ((maci_logch_hdr_ptr->flag_bit & 0x01) << 7);
          log_ptr++;
          
          if(maci_logging_debug_f3_enable)
          {          
            MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                 "continue_tx_type_indx %d, TSN %d|SS %d|SIZE %d|LCID %d|N_SDU %d|ERNTI %d|F %d|num mac headers %d",
                 continue_tx_type_indx, maci_logch_hdr_ptr->tsn, maci_logch_hdr_ptr->ss,
                 maci_logch_hdr_ptr->maci_sdu_size, maci_logch_hdr_ptr->logch_id, maci_logch_hdr_ptr->n_sdu, maci_logch_hdr_ptr->ernti,
                 maci_logch_hdr_ptr->flag_bit, maci_per_tti_ptr->num_mac_i_hdrs);
          }
          
          if (maci_logging_debug_f3_enable)
          {
            MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
              "|TX %d |ETFCI %d|S-Buf %d |SI %d|TSN %d|SS %d|SIZE %d|LCID %d|N_SDU %d|",
               maci_per_tti_ptr->ctrl_info.tx_type, maci_per_tti_ptr->ctrl_info.etfci, 
               maci_per_tti_ptr->sched_buf_status, maci_per_tti_ptr->ctrl_info.si_triggered,
               maci_logch_hdr_ptr->tsn, maci_logch_hdr_ptr->ss,
               maci_logch_hdr_ptr->maci_sdu_size, maci_logch_hdr_ptr->logch_id, maci_logch_hdr_ptr->n_sdu);
            
            MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                 "|HLBS %d|TEBS %d|TSN %d|SS %d|SIZE %d|LCID %d|N_SDU %d|ERNTI %d|F %d|",
                 maci_per_tti_ptr->ctrl_info.hlbs, maci_per_tti_ptr->ctrl_info.tebs, maci_logch_hdr_ptr->tsn, maci_logch_hdr_ptr->ss,
                 maci_logch_hdr_ptr->maci_sdu_size, maci_logch_hdr_ptr->logch_id, maci_logch_hdr_ptr->n_sdu, maci_logch_hdr_ptr->ernti,
                 maci_logch_hdr_ptr->flag_bit);
           }
        }
        //SI-only triggere case- or - NO data TX but Tx_Type is NEW(SRB delay)
        if((maci_per_tti_ptr->ctrl_info.tx_type == 0) && (maci_per_tti_ptr->num_mac_i_hdrs == 0))
        {
          if (maci_logging_debug_f3_enable)
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: SI only Triggered or No-data Tx- idx %d, maci_hdr_idx %d, ",
                                                       idx, log_ch_hdr_idx);
          }
          //SS-TSN
          *log_ptr++ = (0x40 & 0x3F);// TSN_64-Assigned
          *log_ptr++ |= ((0) << 6);
//            *log_ptr++;
          
          //SDU-Size
          *log_ptr++ = (uint8)(0);
          *log_ptr = (uint8)((0) >> 8);
          
          // LC-Id , ERNTI,
          *log_ptr |= ((0) << 3);
          *log_ptr |= ((0) << 7);
          log_ptr++;
          
          //N_SDU, Flag_bit
          *log_ptr = (0);
          *log_ptr |= ((1 & 0x01) << 7);
          log_ptr++;
        }
        
        if(maci_logging_debug_f3_enable)
        {
          MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
               "|TX %d |HB %d|ETFCI_Reason %d|ETFCI %d|S-Buf %d |NS-Buf %d|S_pyld %d|NS_pyld %d|SI %d|",
                maci_per_tti_ptr->ctrl_info.tx_type, maci_per_tti_ptr->ctrl_info.happy_bit, maci_per_tti_ptr->etfci_restricted_reason,
                maci_per_tti_ptr->ctrl_info.etfci, maci_per_tti_ptr->sched_buf_status, maci_per_tti_ptr->non_sched_buf_status,
                maci_per_tti_ptr->sched_payload, maci_per_tti_ptr->ns_payload, maci_per_tti_ptr->ctrl_info.si_triggered);
        
        }
        if((maci_logging_debug_f3_enable) && (log_ch_hdr_idx != idx))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Not on same index- log_ch_hdr_idx %d , idx %d",
                                                      log_ch_hdr_idx, idx);
        }
      }
      
      log_commit(mac_hs_ul_maci_is_log_pkt_type_ptr);
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Unable to allocate memory for Mac-i-is log_alloc");
    }
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: num samples zero ");
  }
  
  // Clear the num_samples and dirty_bit 
  maci_is_log_ptr->log_pkt_num_samples = 0;
  maci_is_log_ptr->dirty_bit = FALSE;
  maci_is_log_ptr->num_tti_logged = 0;
}

#else

void mac_hs_ul_process_maci_is_log_packet(void)
{
  uint8          *log_ptr, num_si_present, continue_tx_type_indx, log_ch_hdr_idx,tti_idx;
  uint16         idx, num_new_tx, num_samples =0, num_re_tx, num_continue_tx, num_dtx;
  uint32         pkt_len;
  
  WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type       *mac_hs_ul_maci_is_log_pkt_type_ptr;
  mac_hs_ul_maci_logpkt_uncompressed_pkt_type   *maci_is_log_ptr;
  mac_hs_ul_maci_hdr_logch_type      *maci_logch_hdr_ptr;
  mac_hs_ul_maci_pkt_per_tti_type    *maci_per_tti_ptr;
  
  idx = num_new_tx = num_re_tx = num_continue_tx = num_samples = pkt_len = num_dtx = tti_idx = 0;
  num_si_present = continue_tx_type_indx = log_ch_hdr_idx = 0;
  
  if (mac_hs_ul_maci_current_log_buf_flag == TRUE)
  {
    maci_is_log_ptr = &(mac_hs_ul_maci_is_prev_log_packet_info);
  }
  else
  {
    maci_is_log_ptr = &(mac_hs_ul_maci_is_log_packet_info);
  }
  
  // set the dirty_bit to TRUE so that it will not be overwritten
  maci_is_log_ptr->dirty_bit = TRUE;
  
  num_samples = maci_is_log_ptr->log_pkt_num_samples;
  
  if(maci_is_log_ptr->log_pkt_num_samples > MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT)
   {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Num log samples exceeds count,num_samples = %d ",
                                                maci_is_log_ptr->log_pkt_num_samples);
    maci_is_log_ptr->log_pkt_num_samples = 0;
    maci_is_log_ptr->dirty_bit = FALSE;
    maci_is_log_ptr->num_tti_logged = 0;
    return;
  }

  if((maci_logging_debug_f3_enable) &&
     ((maci_is_log_ptr->num_tti_logged > 40) || (maci_is_log_ptr->log_pkt_num_samples > 40)))
  { 
     MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: mismatch in MAX-40 allowed and num TTI %d and nsamples %d ",
                                                maci_is_log_ptr->num_tti_logged,
                                                maci_is_log_ptr->log_pkt_num_samples);
  }
  
  if(maci_is_log_ptr->num_tti_logged > 0)
  {
    for(idx = 0; idx < num_samples; idx++)
    {
      maci_per_tti_ptr = &(maci_is_log_ptr->maci_per_tti_info[idx]);
      if (maci_per_tti_ptr->ctrl_info.tx_type  == 0)
      {
        num_new_tx++;
        if (maci_per_tti_ptr->num_mac_i_hdrs > 0)
        {
          num_continue_tx += (maci_per_tti_ptr->num_mac_i_hdrs - 1);
        }
        
        if(maci_per_tti_ptr->ctrl_info.si_present)
        {
          num_si_present++;
        }
      }
      else if (maci_per_tti_ptr->ctrl_info.tx_type  == 1)
      {
        num_re_tx++;
      }
      else if (maci_per_tti_ptr->ctrl_info.tx_type == 2)
      {
        num_dtx++;
      }
      else if(maci_per_tti_ptr->ctrl_info.tx_type == 3)
      {
        num_continue_tx++;
      }
    }
    
    if((num_new_tx + num_re_tx + num_continue_tx + num_dtx) != maci_is_log_ptr->log_pkt_num_samples)
    {
      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: mismatch in logging numsample %d, new tx %d, continue tx %d",
                                                  maci_is_log_ptr->log_pkt_num_samples,
                                                  num_new_tx,
                                                  num_continue_tx);
    }
    
    //if (maci_logging_debug_f3_enable)
    {
      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: nsamples %d, new tx %d, continue tx %d",
                                                 maci_is_log_ptr->log_pkt_num_samples,
                                                 num_new_tx,
                                                 num_continue_tx);
      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: num_re_tx %d, num_dtx %d, num_tti_logged %d",
                                                 num_re_tx,
                                                 num_dtx,
                                                 maci_is_log_ptr->num_tti_logged);
    }
    
    pkt_len = MAC_HS_UL_MACI_IS_INFO_LOG_PKT_SIZE((num_new_tx + num_re_tx), num_continue_tx, num_dtx);
    mac_hs_ul_maci_is_log_pkt_type_ptr = (WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type *)log_alloc( WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET,pkt_len);
    log_ptr = (uint8 *)mac_hs_ul_maci_is_log_pkt_type_ptr;
    
    if(log_ptr != NULL)
    {
      log_ptr += FPOS(WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET_type, version);
      *log_ptr++ = 1; //Log packet Version
      
      /*  num_sample_rrc_state  */
      *log_ptr = (maci_is_log_ptr->rrc_state & 0x03);
      *log_ptr |= ((maci_is_log_ptr->log_pkt_num_samples << 2) & (0xFC));
      log_ptr++;
      
      // Mark if it is 2ms or 10ms tti
      if (maci_is_log_ptr->tti == E_TTI_2MS)
      {
        *log_ptr++ = maci_is_log_ptr->start_frame_num & 0xFF;
        *log_ptr = (maci_is_log_ptr->start_frame_num & 0xF00) >> 8;
      }
      else
      {
        *log_ptr++ = maci_is_log_ptr->start_frame_num;
        *log_ptr = 0;
      }
      
      // Store the start harq id
      MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, HARQ_ID, maci_is_log_ptr->harq_id);
      *log_ptr |= ((maci_is_log_ptr->tti << 7) & 0x80);
      log_ptr++;
     
      for (idx = 0 ; idx < num_samples; idx++)
      {
        maci_per_tti_ptr = &(maci_is_log_ptr->maci_per_tti_info[tti_idx++]);
        
        *log_ptr = 0;
        // Store the happy bit tx type
        MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, TX_TYPE, maci_per_tti_ptr->ctrl_info.tx_type);
        
        if(maci_per_tti_ptr->ctrl_info.tx_type == 2)
        {
          //DTX - ETFCI_REASON, S_BUF, NS_BUF
          *log_ptr = 0x02;
          *log_ptr |= ((maci_per_tti_ptr->etfci_restricted_reason << 3) & (0x78));
          log_ptr++;
          
          // Store the Sched buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_buf_status & 0xFF00) >> 8);
          
          // Store the non_sched_buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->non_sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->non_sched_buf_status & 0xFF00) >> 8);
          
          if (maci_logging_debug_f3_enable)
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: N_sample %d DTX , log_ch_hdr_idx %d",
                                                       idx, log_ch_hdr_idx);
          }
          continue;
          
        }
        else if(maci_per_tti_ptr->ctrl_info.tx_type == 1)
        {
          //Re_TX - HB,ETFCI_REASON, ETFCI, S_BUF, NS_BUF
          *log_ptr = 0x01;
          // Store the happy bit
          MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, HB, maci_per_tti_ptr->ctrl_info.happy_bit);
                                    
          *log_ptr |= ((maci_per_tti_ptr->etfci_restricted_reason << 3) & (0x78));
          log_ptr++;
          
          // Store the ETFCI value
          *log_ptr++ = maci_per_tti_ptr->ctrl_info.etfci;
          
          // Store the Sched buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_buf_status & 0xFF00) >> 8);
          
          // Store the non_sched_buf status
          *log_ptr++ = (uint8)(maci_per_tti_ptr->non_sched_buf_status & 0x00FF);
          *log_ptr++ = (uint8)((maci_per_tti_ptr->non_sched_buf_status & 0xFF00) >> 8);
          continue;
        }
        else if(maci_per_tti_ptr->ctrl_info.tx_type == 3)
        {
            //Write the TX-TYPE to CONTINUE for the Next MAC-I header.
            *log_ptr++ = 0x03;
          if (maci_logging_debug_f3_enable)
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG:N_sample %d CONTINUE_TX, type %d",
                                                       idx,
                                                       maci_per_tti_ptr->ctrl_info.tx_type);
          }
        }
        else if(maci_per_tti_ptr->ctrl_info.tx_type == 0)
          {
            // Store the happy bit
            MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, HB, maci_per_tti_ptr->ctrl_info.happy_bit);
                                      
            *log_ptr |= ((maci_per_tti_ptr->etfci_restricted_reason << 3) & (0xF8));
            log_ptr++;
            
            // Store the ETFCI value
            *log_ptr++ = maci_per_tti_ptr->ctrl_info.etfci;
            
            // Store the Sched buf status
            *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_buf_status & 0x00FF);
            *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_buf_status & 0xFF00) >> 8);
            
            // Store the non_sched_buf status
            *log_ptr++ = (uint8)(maci_per_tti_ptr->non_sched_buf_status & 0x00FF);
            *log_ptr++ = (uint8)((maci_per_tti_ptr->non_sched_buf_status & 0xFF00) >> 8);
            
            // Store the Sched payload txed after etfci selecltion
            *log_ptr++ = (uint8)(maci_per_tti_ptr->sched_payload & 0x00FF);
            *log_ptr++ = (uint8)((maci_per_tti_ptr->sched_payload & 0xFF00) >> 8);
            
            //Store the NonSched payload txed after etfci selection
            *log_ptr++ = (uint8)(maci_per_tti_ptr->ns_payload & 0x00FF);
            *log_ptr++ = (uint8)((maci_per_tti_ptr->ns_payload & 0xFF00) >> 8);
            
            if (maci_per_tti_ptr->ctrl_info.si_present)
            {
              if(maci_per_tti_ptr->ctrl_info.si_triggered)
              {
                *log_ptr = 2;//SI_TRIG
              }
              else
              {
                *log_ptr = 1;//SI_PAD
              }
              
              *log_ptr |= (( maci_per_tti_ptr->ctrl_info.uph & MAC_HS_UL_HDR_LOG_UPH_BMASK) << 2);
              log_ptr++;
              
              #if 0
              //log_hdr_status_ptr->si_content[0]
              // Set the HLID
              MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, HLID, maci_per_tti_ptr->ctrl_info.hlid);
              // Set HLBS value
              MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, HLBS,maci_per_tti_ptr->ctrl_info.hlbs);
              log_ptr++;
              //Set TEBS value
              MAC_HS_UL_LOG_PKT_SET_FIELD(*log_ptr, HDR_LOG, TEBS,maci_per_tti_ptr->ctrl_info.tebs);
              log_ptr++;
              #endif
              //HLBS|HLID - each of 4bits
              *log_ptr = (maci_per_tti_ptr->ctrl_info.hlid & 0xF);
              *log_ptr |= ((maci_per_tti_ptr->ctrl_info.hlbs & 0xF) << 4);
              log_ptr++;
              
              //TEBS- 5bits
              *log_ptr = (maci_per_tti_ptr->ctrl_info.tebs & 0x1F);
              log_ptr++;
              
            }
            else
            {
              *log_ptr++ = 0;// NO-SI
  //          log_ptr += 2;
            }
          }
          else
          {
            log_ptr++;
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Invalid TxTYPe %d, idx %d ",
                                                        idx,
                                                        maci_per_tti_ptr->ctrl_info.tx_type);
            continue;
          }
          
          for (continue_tx_type_indx = 0; 
               ((continue_tx_type_indx < maci_per_tti_ptr->num_mac_i_hdrs) && 
               (log_ch_hdr_idx < MAC_HS_UL_NUM_MACI_IS_STATUS_LOG_PKT)); continue_tx_type_indx++)
          {
            if(continue_tx_type_indx != 0)
            {
              //Write the TX-TYPE to CONTINUE for the Next MAC-I header.
              *log_ptr++ = 0x03;
              idx++;
              if (maci_logging_debug_f3_enable)
              {
                MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: Tx_Type continue, n maci_hdrs %d,idx %d ",
                                                           maci_per_tti_ptr->num_mac_i_hdrs,
                                                           idx);
              }
            }
            
            #if 0
            In MAC code - (0)-pc, (1)-Pe, (2)-Ps,(3)-Pm
            In Apex     - (0)-pc, (1)-Ps, (2)-Pm,(3)-Pe
            #endif
            maci_logch_hdr_ptr = &(maci_is_log_ptr->maci_logch_hdr_info[log_ch_hdr_idx++]);
            if(((continue_tx_type_indx == 0) && (maci_per_tti_ptr->num_mac_i_hdrs == 1)) ||
               ((continue_tx_type_indx != 0) && (continue_tx_type_indx == (maci_per_tti_ptr->num_mac_i_hdrs - 1))))
            {
              maci_logch_hdr_ptr->flag_bit = 1;
            }
            else
            {
              maci_logch_hdr_ptr->flag_bit = 0;
            }
            
            //SS-TSN
            *log_ptr = (maci_logch_hdr_ptr->tsn & 0x3F);
              *log_ptr++ |= ((maci_logch_hdr_ptr->ss & 0x03) << 6);
            
            //SDU-Size
            *log_ptr++ = (uint8)(maci_logch_hdr_ptr->maci_sdu_size & 0x00FF);
            *log_ptr = (uint8)((maci_logch_hdr_ptr->maci_sdu_size & 0x0700) >> 8);
            
            // LC-Id , ERNTI, N_SDU, F
            *log_ptr |= ((maci_logch_hdr_ptr->logch_id & 0x0F) << 3);
            *log_ptr |= ((maci_logch_hdr_ptr->ernti & 0x01) << 7);
            log_ptr++;
            *log_ptr = (maci_logch_hdr_ptr->n_sdu & 0x7F);
            *log_ptr |= ((maci_logch_hdr_ptr->flag_bit & 0x01) << 7);
            log_ptr++;
            
//            log_ch_hdr_idx++;
            if (maci_logging_debug_f3_enable)
            {
              MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                "|TX %d |ETFCI %d|S-Buf %d |SI %d|TSN %d|SS %d|SIZE %d|LCID %d|N_SDU %d|",
                 maci_per_tti_ptr->ctrl_info.tx_type, maci_per_tti_ptr->ctrl_info.etfci, 
                 maci_per_tti_ptr->sched_buf_status, maci_per_tti_ptr->ctrl_info.si_triggered,
                 maci_logch_hdr_ptr->tsn, maci_logch_hdr_ptr->ss,
                 maci_logch_hdr_ptr->maci_sdu_size, maci_logch_hdr_ptr->logch_id, maci_logch_hdr_ptr->n_sdu);
             }
          }
          //SI-only triggere case- or - NO data TX but Tx_Type is NEW(SRB delay)
          if((maci_per_tti_ptr->ctrl_info.tx_type == 0) && (maci_per_tti_ptr->num_mac_i_hdrs == 0))
          {
            if (maci_logging_debug_f3_enable)
            {          
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: SI only Triggered or No-data Tx- idx %d, maci_hdr_idx %d, ",
                                                         idx, log_ch_hdr_idx);
            }  
          
            //SS-TSN
            *log_ptr = (0x40 & 0x3F);// TSN_64-Assigned
            *log_ptr++ |= ((0) << 6);
            //*log_ptr++;
            
            //SDU-Size
            *log_ptr++ = (uint8)(0);
            *log_ptr = (uint8)((0) >> 8);
            
            // LC-Id , ERNTI,
            *log_ptr |= ((0) << 3);
            *log_ptr |= ((0) << 7);
            log_ptr++;
            
            //N_SDU, Flag_bit
            *log_ptr = (0);
            *log_ptr |= ((1 & 0x01) << 7);
            log_ptr++;
        }
#if 0
        MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
              "|TX %d |HB %d|ETFCI_Reason %d|ETFCI %d|S-Buf %d |NS-Buf %d|S_pyld %d|NS_pyld %d|SI %d|",
               maci_per_tti_ptr->ctrl_info.tx_type, maci_per_tti_ptr->ctrl_info.happy_bit, maci_per_tti_ptr->etfci_restricted_reason,
               maci_per_tti_ptr->ctrl_info.etfci, maci_per_tti_ptr->sched_buf_status, maci_per_tti_ptr->non_sched_buf_status,
               maci_per_tti_ptr->sched_payload, maci_per_tti_ptr->ns_payload, maci_per_tti_ptr->ctrl_info.si_triggered);
        
        MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
              "|HLBS %d|TEBS %d|TSN %d|SS %d|SIZE %d|LCID %d|N_SDU %d|ERNTI %d|F %d|",
              maci_per_tti_ptr->ctrl_info.hlbs, maci_per_tti_ptr->ctrl_info.tebs, maci_logch_hdr_ptr->tsn, maci_logch_hdr_ptr->ss,
              maci_logch_hdr_ptr->maci_sdu_size, maci_logch_hdr_ptr->logch_id, maci_logch_hdr_ptr->n_sdu, maci_logch_hdr_ptr->ernti,
              maci_logch_hdr_ptr->flag_bit);
 #endif
              
        if((maci_logging_debug_f3_enable) && (log_ch_hdr_idx != idx))
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Not on same index- log_ch_hdr_idx %d , idx %d",
                                                      log_ch_hdr_idx, idx);
        }
      }
      
      log_commit(mac_hs_ul_maci_is_log_pkt_type_ptr);
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_I_LOG: Unable to allocate memory for Mac-i-is log_alloc");
    }
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_I_LOG: num samples zero ");
  }
  
  // Clear the num_samples and dirty_bit 
  maci_is_log_ptr->log_pkt_num_samples = 0;
  maci_is_log_ptr->dirty_bit = FALSE;
  maci_is_log_ptr->num_tti_logged = 0;
}

#endif /*  FEATURE_WCDMA_DC_HSUPA  */

#endif /*  FEATURE_MAC_I  */

#ifdef FEATURE_WCDMA_HS_RACH

/*===========================================================================
FUNCTION mac_hs_ul_process_hsrach_timers_log_pkt

DESCRIPTION
  This function would be called from l2ultask.c when corresponding command is processed by MAC HS task.
  Copies mac hs rach timer info to be logged from global structure and commits to log buffers.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resest the global buffer from which info is logged - memset it to 0x0
  Thus next time when we start using this buffer, this would aviod overwriting new info on top of previously logged info.
===========================================================================*/

void mac_hs_ul_process_hsrach_timers_log_pkt(void)
{
  WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET_type    *mac_hsrach_timers_log_pkt_ptr = NULL;    
  mac_hs_ul_hsrach_mac_timer_log_pkt_type               *hs_rach_mac_timers_submit_log_pk_ptr = NULL;
  uint8                                                                   loop_idx = 0, *log_ptr = NULL;
  uint32                                              pkt_len = 0;
 
  if(hs_rach_mac_timers_log_pkt_status == TRUE)
  {
    if(hs_rach_mac_timers_cur_log_buf_flag == TRUE)
    {
      hs_rach_mac_timers_submit_log_pk_ptr = &hs_rach_mac_timers_prev_log_pkt_info; 
    }
    else
    {
      hs_rach_mac_timers_submit_log_pk_ptr = &hs_rach_mac_timers_log_pkt_info;
    }
  
    if(hs_rach_mac_timers_submit_log_pk_ptr != NULL)
    {
       pkt_len = WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET_SIZE(hs_rach_mac_timers_submit_log_pk_ptr->log_pkt_num_samples);
    }
    else
    {
       MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HSRACH_LOG: hs_rach_mac_timers_submit_log_pk_ptr is NULL ");
    }
    
    mac_hsrach_timers_log_pkt_ptr = (WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET_type *)log_alloc( WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET, pkt_len);
    log_ptr = (uint8 *)mac_hsrach_timers_log_pkt_ptr;

    if(log_ptr != NULL )
    {
      if(hs_rach_mac_timers_submit_log_pk_ptr != NULL)
      {
        log_ptr += FPOS(WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET_type, log_pkt_version);        
        *log_ptr++ = (uint8)1;

        *log_ptr++ = (uint8)hs_rach_mac_timers_submit_log_pk_ptr->log_pkt_num_samples;

        *log_ptr++ = (uint8)(hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[0].cfn_sfn & (0x00FF));
        *log_ptr++ = (uint8)((hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[0].cfn_sfn & (0xFF00)) >> 8 );
     
        for(loop_idx = 0; loop_idx < hs_rach_mac_timers_submit_log_pk_ptr->log_pkt_num_samples; loop_idx++)
        {  
           *log_ptr++ = (uint8)(hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].cfn_sfn & (0x00FF));
           *log_ptr++ = (uint8)((hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].cfn_sfn & (0xFF00)) >> 8 );

           *log_ptr++ = (uint8) (hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].ccch_tebs_val); 

           *log_ptr++ = (uint8) (hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].rsrc_avail_ccch_timer_val);

           *log_ptr++ = (uint8) (hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].agch_coll_resol_timer_val);

           *log_ptr++ = (uint8) (hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].tb_timer_val);       

           *log_ptr++ = (uint8) (hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].dxch_tebs_val);

           *log_ptr++ = (uint8) (hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].misc_vals);
           
           if(mac_hs_rach_f3_enable )
           {
             MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                 "MAC_HSRACH_LOG: loop_idx %d, cch_bo %d ,ccch_timer %d, tb_timer %d, dxch_bo %d, misc %d, agch_timer %d, cfn %d, cur_log-buf_flag %d",
                 loop_idx,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].ccch_tebs_val,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].rsrc_avail_ccch_timer_val,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].tb_timer_val,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].dxch_tebs_val,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].misc_vals,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].agch_coll_resol_timer_val,
                 hs_rach_mac_timers_submit_log_pk_ptr->hsrach_mac_timer_info[loop_idx].cfn_sfn,
                 hs_rach_mac_timers_cur_log_buf_flag);
           }
        }
        log_commit(mac_hsrach_timers_log_pkt_ptr);
        
        /* Clear already logged buffer */
        memset((void *)hs_rach_mac_timers_submit_log_pk_ptr,0x0, sizeof(mac_hs_ul_hsrach_mac_timer_log_pkt_type));
      }
      else
      {
         MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HSRACH_LOG: hs_rach_mac_timers_submit_log_pk_ptr is NULL ");
      }
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HSRACH_LOG: Unable to allocate memory for HSRACH timers log_alloc ");
    }
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_HSRACH_LOG: mac timer log packet disabled but signal set: %d",
                                                hs_rach_mac_timers_log_pkt_status);
  }
}
#endif //FEATURE_WCDMA_HS_RACH
#endif // FEATURE_MAC_HS_UL_QXDM_LOGGING
#endif // FEATURE_WCDMA_HSUPA
