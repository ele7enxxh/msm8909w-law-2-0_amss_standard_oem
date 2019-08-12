/*!
  @file
  lte_rlcdl_log_ext.h

  @brief
  RLCDL new log packet related functionality.

  @author
  mohankri

*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_log_ext.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/16/15   mg      CR870041: QSH reset for TRM and ML1 during service switch
                   0xB08C and 0xB08C ver 2 overhaul for two client changes
02/20/14   mg      CR788305: DPD interface with G + MISC changes
02/05/15   mg      CR788317: Invalid MSI and 0xB08C logging changes
01/22/15   mg      CR785048: Embms DSDS DPD feature - Phase 3
01/14/15   mg      CR779901: Embms DSDS DPD feature - Phase 2
12/18/14   mg      CR768623: Embms DSDS DPD feature - Phase 1 
12/18/14   mg      initial version
==============================================================================*/

#ifndef LTE_RLCDL_LOG_EXT_H
#define LTE_RLCDL_LOG_EXT_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <genlog.h>
#include <dsm_pool.h>
#include "lte_as.h"
#include "lte_rlc_log.h"
#include "lte_rlcdli.h"
#include "lte_rlcdl_cfg.h"
#include "lte_rlc_cfg.h"
#include "lte_l2_profile.h"
#include "lte_rlcdl_embms.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/  

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*DPI SDU Log subpacket version*/
#define LTE_RLCDL_LOG_EMBMS_DPI_SDU_VERSION 16
/*DSDS statistics subpacket version*/
#define LTE_RLCDL_LOG_EMBMS_DSDS_SERV_VERSION 16


/*!@brief DSDS Service Configuration information*/
typedef PACK(struct)
{
  lte_mbsfn_area_id_t   area_id; /*!< Area id */
  lte_pmch_id_t         pmch_id; /*!< pmch id */
  lte_lc_id_t           lcid; /*!< lc id */
  boolean               valid; /*!< Deep Packet Inspection is active or not*/
  boolean               disable_reservation_flag; /*!< Disable sending a raise 
                                       priority indication to ML1*/
  boolean               use_background_low_priority;/*!< Use background traffic for
                        embms low priority*/
  uint32                serv_type;/*!<Service type*/
  boolean               seg_start_hipri; /*!< Segment start should have 
                                       high priority*/
  uint32                seg_duration; /*!< Segment duration in milliseconds*/
  uint8                 fec;/*!< Additional FEC symbols in percentage*/
  uint8                 priority; /*!< Service priority level*/
  boolean               muxed_data; /*!< 0- audio and video not multiplexed, 
                                       1- multiplexed*/
  uint32                video_data_rate; /*!< Video datarate if not multiplexed, 
                                        combined data rate if multiplexed*/
  uint32                audio_data_rate; /*!< Audio datarate if not multiplexed.
                                        Else 0*/                                  
  uint32                msp_dur; /*!< Duration of the MSP in milliseconds*/
  uint8                 thres_pri; /*!< Threshold loss to bump up priority.*/
  uint8                 giveup_pri; /*!< Loss rate beyond which the UE gives up 
                                        bumping priority*/
  uint32                vid_seg_thres_size; /*!<Minumum size of video TOI*/
  uint32                th_a; /*!< Threshold to decide audio versus video segment
                               in bytes per second. NV/EFS item,default = 20,000 bytes/sec*/
  uint32                sca_fact_a; /*!<Threshold scaling factor A*/
  uint32                sca_fact_b; /*!<Threshold scaling factor B */
  boolean               first_burst_hi_pri; /*!<First burst of every segment 
                                should be high priority*/
  uint8                 enable_per_av_object_info; /*!<Request per object info*/
  uint8                 enable_per_status_object_info; /*!< Request per object
                                decode info*/
  uint32                num_bytes_subfn; /*!<Number of bytes per subframe*/
  uint16                min_msp_bcast; /*!<Minimum MSP value to use LOW 
                                priority instead of background priority*/
  uint16                hi_pri_timer; /*!< Duration of high priority after a new 
                                service started*/
  lte_rlcdl_embms_av_trasmit_pattern_e av_trans_pat: 8; /*!< Audio video transmit pattern*/
  uint32                reserved1;/*!< Reserved field 1*/ 
  uint32                reserved2;/*!< Reserved field 2*/ 
  uint32                reserved3;/*!< Reserved field 3*/ 
  uint32                reserved4;/*!< Reserved field 4*/ 
}lte_rlcdl_log_ext_embms_service_cfg_s;

typedef PACK(struct)
{
  uint16                toi; /*!< Transport Object ID*/
  uint16                tsi; /*!< Transport Session ID*/
  uint16                fec;/*!< Forward error correction in percentage*/
  uint16                symbol_size; /*!< Size of the symbols*/
  uint32                size; /*!< Size of this TOI without the FEC*/
}lte_rlcdl_log_ext_embms_toi_cfg_s;

typedef PACK(struct)
{
  uint32                cur_seg; /*!< Current internal segment number which is 
                                 used. Updated if needed*/
  uint16                last_sbn; /*!< Last SBN received on this TOI*/                                 
  uint16                last_esi; /*!< Last ESI received on this TOI.
                                  Used to calculate the number of 
                                  symbols/bytes lost so far on this TOI*/
  uint16                first_sbn; /*!< First SBN received on this TOI*/                                 
  uint16                first_esi; /*!< First ESI received on this TOI*/
  boolean               rcvd_service_info;/*!< Whether we received the symbol
                                  size and FEC from the service layer for this TOI. 
                                  Else use the previous information*/
  boolean               video_toi;/*!< This TOI is treated as video TOI if the 
                                  size(or bytes received) is greater than a 
                                  threshold  or if this information is received
                                  from service layer*/
  boolean               decoded;/*!< Got an indication from service layer 
                                   whether this TOI is decoded*/
  uint16                decoded_symbol_id; /*!< The symbol ID which the service 
                                   layer used to decode the TOI*/
  lte_sfn_s             first_sfn;/*!< The system frame number of first 
                                   packet on this TOI */
  lte_sfn_s             last_sfn; /*!< The system frame number of last 
                                   packet on this TOI*/
}lte_rlcdl_log_ext_embms_toi_state_s;

typedef PACK(struct)
{
  uint32                bytes_rcvd;/*!< Bytes received so far on this TOI*/
  uint32                bytes_lost;/*!< Bytes lost so far on this TOI*/
  uint32                approx_bytes_lost;/*!< Approximate bytes lost so far*/
  uint32                symbols_rcvd;/*!< Total Symbols received so far*/
  uint32                symbols_lost;/*!< Total Symbola lost so far*/
  uint32                num_addl_symbols;/*!< Number of additional symbols processed
                                    for this TOI after it is decoded*/
}lte_rlcdl_log_ext_embms_toi_stats_s;

/*!@brief DSDS service state information*/
typedef PACK(struct)
{
  boolean               raise_priority;/*!< Send a raise priority indication to ML1*/
  lte_sfn_s             last_sfn; /*!< Sfn of the last received packet*/
  uint16                cur_toi; /*!< Currently monitored toi to bump up or bump down priority*/
  lte_rlcdl_log_ext_embms_toi_cfg_s cur_toi_cfg; /*!< Config of the latest video toi available.*/
  lte_rlcdl_log_ext_embms_toi_state_s cur_toi_state; /*!< Current state of the latest video toi */
  lte_rlcdl_log_ext_embms_toi_stats_s cur_toi_stats; /*!< Current statistics of the latest video toi*/
  uint32                reserved1;/*!< Reserved field 1*/ 
  uint32                reserved2;/*!< Reserved field 2*/ 
  uint32                reserved3;/*!< Reserved field 3*/ 
  uint32                reserved4;/*!< Reserved field 4*/ 
}lte_rlcdl_log_ext_embms_service_state_s;


/*!@brief DSDS service statistics information*/
typedef PACK(struct)
{
  uint32                num_toi_rcvd; /*!<Number of TOIs received*/
  uint32                num_inband_toi_rcvd;/*!< Total number of inband updates received*/
  uint32                num_toi_fail_decodes; /*!<Number of TOI failed decodes at SL*/
  uint32                num_toi_pass_decodes;/*!<Number of TOI passed decodes at SL*/
  uint32                num_fdt_misses;/*!< Number of TOI failed due to missed FDT*/
  uint32                num_404_errors;/*!< Number of 404 errors received for this service*/
  uint32                num_unique_404_errors;/*!< Number of unique 404 errors received for this service*/
  uint32                num_fdt_pkts; /*!<Number of FDTs received for this service*/
  uint32                num_msi_rcvd; /*!<Number of MSIs received*/
  uint32                num_msi_missed;/*!<Number of missed MSIs*/
  uint32                num_pri_req; /*!< Total number of requests made to ML1 at burst expiry*/
  uint32                num_high_pri_req; /*!<Total number of high priority requests at burst expiry*/
  uint32                num_low_pri_dur_ms;/*!<Total low priority duration in ms
                                      calculated at burst expiry*/
  uint32                num_low_pri_adj_dur_ms;/*!<Total low priority duration in ms
                                      calculated after MSI is received*/ 
  uint32                num_high_pri_dur_ms;/*!<Total high priority duration in ms
                                      calculated at burst expiry*/
  uint32                num_high_pri_adj_dur_ms;/*!<Total high priority duration in ms
                                      calculated after MSI is received*/ 
  uint32                num_invalid_msp_errors; /*!<Number of invalid MSP info given by ML1*/
  uint32                num_acc_prediction; /*!<Number of times L2 made an accurate prediction*/
  uint32                num_partial_ip_dropped; /*!< Number of partial IP packets dropped.*/
  uint16                num_unlocks_rcvd; /*!< Total number of unlock nows received*/
  uint16                num_unlocks_granted;/*!< Total number of unlocks granted*/
  uint16                num_1x_unlock_rejected;/*!< Total number of 1x unlocks rejected*/
  uint16                num_G_unlock_rejected;/*!< Total number of Geran unlocks rejected*/
  uint16                num_collision_unlock_honored;/*!< Total number of unlocks with collisions
   that are honored. Only those with unlock now are counted*/
  uint16                num_collision_unlock_rejected;/*!< Total number of unlocks with collsions
   that are rejected. Only those with unlock now are counted*/
  uint32                reserved1;/*!< Reserved field 1*/ 
  uint32                reserved2;/*!< Reserved field 2*/ 
  uint32                reserved3;/*!< Reserved field 3*/ 
  uint32                reserved4;/*!< Reserved field 4*/ 
}lte_rlcdl_log_ext_embms_service_stats_s;

/*!@brief DSDS Service Info*/
typedef PACK(struct)
{
  lte_rlcdl_log_ext_embms_service_cfg_s   cfg; /*!< DSDS service config info*/
  lte_rlcdl_log_ext_embms_service_state_s state; /*!< DSDS service state info*/
  lte_rlcdl_log_ext_embms_service_stats_s stats; /*!< DSDS service stats info*/
}lte_rlcdl_log_ext_embms_service_info_s;

/*! @brief This struct defines the format of the RLC DL EMBMS DSDS SERV subpkt
@log_id 0xB08C
*/
typedef PACK(struct)
{
  lte_sfn_s sfn; /*!< Subframe number at which the packet is logged*/
  lte_rlcdl_log_ext_embms_service_info_s service_info; /*!< DSDS service info*/
} lte_rlcdl_log_embms_dsds_serv_s;

/*! @brief 
      This struct defines the format of UNIFIED RLC DL EMBMS DPI SDU subpacket 
      Log frequency varies depending on when the log buffer is filled or is force flushed
      Logcode: 0xB08D
      @internal
*/
typedef PACK(struct)
{
  uint16  num_pdus; /*!< number of subpackets in this log buffer */
  uint8   log_buf[LTE_RLC_CFG_DL_LOG_PDU_BUF_SZ]; /*!< buffer for PDU's */
  uint16  size;      /*!< Size of the payload, this is not sent cross the wire */
} lte_rlcdl_log_unified_sub_pkt_embms_dpi_sdu_s;


/*! @brief Embms dpi sdu unified subpkt structure 
      @log_id 0xB08D
      @internal
*/
typedef PACK(struct)
{
  lte_rlcdl_embms_dpi_pkt_type_e packet_type:8;/*!< DPI_SDU=0; Per_Obj_Info=1; Per_Obj_Decode=2
                Per_Serv_Info=3, Dpi_Pri_Msg=4*/
  lte_sfn_s sfn;/*!< Subframe number at which this packet is received*/  
  uint16 tsi; /*!< TSI */
  uint16 toi; /*!< TOI */
  uint16 sbn; /*!< SBN */
  uint16 esi; /*!< ESI */
  uint16 databytes; /*!< Number of data bytes in this SDU after removing the 
                      IP header and the flute headers*/
  uint16 last_sbn; /*!< SBN of the last packet received on this TOI/TSI combo*/
  uint16 last_esi; /*!< ESI of the last packet received on this TOI/TSI combo*/
  lte_sfn_s first_sfn; /*!< Subframe number at which the first packet for this TOI
                      is received*/
  lte_sfn_s last_sfn; /*!< Subframe number at which the first packet for this TOI
                      is received*/
  uint32 bytes_rcvd; /*!< Bytes received so far on this TOI*/
  uint32 bytes_lost; /*!< Bytes lost so far on this TOI*/
  uint32 approx_bytes_lost; /*!< Approximate bytes lost so far on this TOI*/
  uint32 symbols_rcvd;/*!< Total symbols received on this TOI*/
  uint32 symbols_lost;/*!< Total symbols lost so far on this TOI*/
  uint8  ipver; /*!< IP version */
  uint8  calc_loss; /*!< Calculated loss*/
  trm_client_enum_t client1;/*!<Client ID used for the first reservation */
  uint16  cur_pri1; /*!<Current priority state - 0 - low; 1-high */
  lte_sfn_s  start_sfn1; /*!< Start sfn of the burst sent to ML1*/
  uint16 burst_dur1; /*!< Burst duration*/
  boolean l1_override_priority1;/*!<Client ID used for the first reservation */
  trm_client_enum_t client2;/*!<Client ID used for the second reservation */
  uint16  cur_pri2; /*!<Current priority state - 0 - low; 1-high */
  lte_sfn_s  start_sfn2; /*!< Start sfn of the burst sent to ML1*/
  uint16 burst_dur2; /*!< Burst duration*/
  boolean l1_override_priority2;/*!<Client ID used for the first reservation */
  lte_sfn_s cur_burst_start;/*!< Current burst start*/
  lte_sfn_s cur_burst_end;/*!< Current burst end*/
  lte_sfn_s unlock_by_start_time; /*!< Time when is unlock requested */
  uint16 unlock_by_duration; /*!< How long is unlock requested for. In units of msecs */
  uint8 winning_client_id; /*!< Unlock requested for which client. 0 - 1x 8 - GSM*/
  lte_rlcdl_embms_unlock_req_pri_e    unlock_priority; /*!< Priority of the winning client*/
  boolean unlock_now;  /*!< Whether LTE is going to unlock now or sometime in the future.
   Set to true just before tuneaway and by LTE. Set to false only
   for scheduling the unlock by TAM*/
  boolean collision;
  lte_sfn_s collision_start;
  lte_sfn_s collision_end;  
  uint16 cur_burst_sfn;/*!< Number of subframes in cur_burst*/
  uint16 afford_sfn_loss;/*!< Number of subframes that can be lost*/
  uint16 collision_sfn_loss;/*!<Number of subframes lost due to collision */
  uint16 last_tb_size;/*!< Last MAC transport block size*/
  boolean honor_unlock;/*!< Unlock is honored or not*/
  lte_sfn_s defer_sfn;/*!< If unlock is not honored then this indicates the latest sfn at which 
  unlock can be honored*/
  lte_mbsfn_area_id_t area_id; /*!< Area id */
  lte_pmch_id_t pmch_id; /*!< pmch id */
  lte_lc_id_t lcid; /*!< Lcid */
  uint8  decode_status; /*!< Whether the TOI is decoded by the service 
           layer or not: 0-success*/
  uint16 last_decode_esi; /*!< Symbol ID of the last symbol decoded by the 
           service layer for this TOI*/
  uint8  fec; /*!< FEC for this TOI object in percentage*/
  uint16 symbol_size; /*!< Symbol size of the TOI object*/
  uint32 object_size; /*!< Actual size of the TOI object without the FEC*/
  uint32 seg_duration; /*!< Segment duration in milliseconds*/
  uint32 serv_priority; /*!< Service priority level*/
  uint32 serv_type; /*!< Service type*/
  uint16 video_data_rate; /*!<Video datarate if not multiplexed, combined data 
           rate if multiplexed*/
  uint16 audio_data_rate; /*!<Audio datarate if not multiplexed*/
  uint32 object_size_w_fec; /*!<TOI Object size with FEC*/
  uint32 reserved1; /*!< Reserved field 1*/                                  
  uint32 reserved2; /*!< Reserved field 2*/
  uint32 reserved3; /*!< Reserved field 3*/
  uint32 reserved4; /*!< Reserved field 4*/
} lte_rlc_log_unified_embms_dpi_sdu_s;

extern void lte_rlcdl_log_embms_submit_dsds_serv_log_pkt( void );
extern void lte_rlcdl_log_embms_dsds_serv( void );

extern void lte_rlcdl_log_embms_dpi_sdu
(
  lte_rlcdl_embms_cb_s*         cb_ptr,        /*!< control block pointer */
  lte_rlcdl_embms_dpi_sdu_s*    dpi_sdu_ptr
);

extern void lte_rlcdl_log_embms_submit_dpi_sdu_log_pkt
(
  void
);



#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* LTE_RLCDL_LOG_EXT_H */
