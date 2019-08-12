/*!
  @file
  lte_rlcdl_embms.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  gxiao

*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_embms.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/06/05   mg      CR904992: Enhance DPD logging for 0xB08D log packet using 
                   reserved fields
10/02/15   mg      CR904992: For L+G case use B factor to protect video TOI 
                   against BLER by trading off G page block rate.
09/18/15   mg      CR904992: EMBMS DPD tradeoff G and 1x paging (complete fix)
09/15/15   mg      CR904992: EMBMS DPD tradeoff G and 1x paging
08/16/15   mg      CR870041: QSH reset for TRM and ML1 during service switch
                   0xB08C and 0xB08C ver 2 overhaul for two client changes
08/04/15   mg      CR881179: Fix for two client reservations where high 
                   reservation is made for greater than 10 seconds
07/12/15   mg      CR876947: eMBMS DPD L2 QSH and burst collision changes
06/14/15   mg      CR832574: EMBMS DPD two client implementation
04/30/15   mg      CR832561: Adaptive b factor scaling
04/10/15   mg      CR812886: Updated DPD approximate loss and estimated loss
                   calculation - Calculate number of subframes from bitmask
02/27/15   mg      CR802448: Disable DPD for not tuneway case
02/05/15   mg      CR788317: Invalid MSI and 0xB08C logging changes
02/03/15   mg      CR789070: Handle service layer message before L2 is active.
                   Update burst reservation to include the end of the burst.
01/27/15   mg      CR786757: EMBMS DPD not kicking in on service startup
01/22/15   mg      CR785048: Embms DSDS DPD feature - Phase 3
01/14/15   mg      CR779901: Embms DSDS DPD feature - Phase 2
01/03/15   sb      CR776981: Calculate num bytes in RLCDL and other QSH improvements
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1
12/12/13   mg      CR583687: Support eMBMS data path with new DS-DPM design
05/08/13   sb      CR466846: Remove assert and free the SDU if LTE_RLCDL_MCCH_PDU_IND
                   message sending fails to RRC
04/23/12   ax      CR333829: capability to deal with area_id > 8, etc
04/17/12   ru      CR 342859: Added eMBMS logging changes  
10/17/11   ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_EMBMS_H
#define LTE_RLCDL_EMBMS_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "lte.h"
#include "lte_rlc_rrc.h"
#include "lte_mac_rlc.h"
#include "dsm_queue.h"
#include "lte_rlcdl_hdr.h"
#include "lte_rlc_msg.h"
#include <lte_l2_timer.h>
#include <lte_cphy_msg.h>
#include <trm.h>
#include <wireless_data_service_v01.h>
#include <ds3gpp_ext_msg.h>
#include <mcs_timer.h>
#include "lte_misc.h"
#include "lte_l2_iface.h"
#include "timetick.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief maximum active MTCH + maximum active MCCH (one per area)
*/
#define LTE_RLCDL_EMBMS_MAX_MRB  (LTE_MAX_ACTIVE_MRB + LTE_EMBMS_MAX_MBSFN_AREA)

/*! @brief invalid control block index
*/
#define LTE_RLCDL_EMBMS_INVALID_CB_IDX                                 0xFF

/*! @brief invalid lc_id 
*/
#define LTE_RLCDL_EMBMS_INVALID_LC_ID                                  0XFF

/*! @brief RLC sequence length
*/
#define LTE_RLCDL_EMBMS_MRB_SN_LEN                                        5

/*! @brief total number of valid area id [0, 255]
*/
#define LTE_RLCDL_EMBMS_NUM_AREA_ID                                   256

/*! @brief IP version 4
*/
#define LTE_RLCDL_EMBMS_IPV4                                          4

/*! @brief IP version 4 header size + UDP header size
*/
#define LTE_RLCDL_EMBMS_IPV4_OFFSET                                   28

/*! @brief IP version 6
*/
#define LTE_RLCDL_EMBMS_IPV6                                          6

/*! @brief IP version 6 header size + UDP header size
*/
#define LTE_RLCDL_EMBMS_IPV6_OFFSET                                   48

/*! @brief TSI offset from the beginning of FLUTE header
*/
#define LTE_RLCDL_EMBMS_TSI_OFFSET                                    8

/*! @brief TOI offset from the beginning of FLUTE header
*/
#define LTE_RLCDL_EMBMS_TOI_OFFSET                                    10

/*! @brief SBN offset from the beginning of FLUTE header
*/
#define LTE_RLCDL_EMBMS_SBN_OFFSET                                    12


/*! @brief ESI offset from the beginning of FLUTE header
*/
#define LTE_RLCDL_EMBMS_ESI_OFFSET                                    14

/*! @brief Size of IPversion buffer size
*/
#define LTE_RLCDL_EMBMS_IPVER_BUF_SZ                                  1

/*! @brief TOI array size
*/
#define LTE_RLCDL_EMBMS_TOI_ARR_SZ                                    8

/*! @brief TOI array size
*/
#define LTE_RLCDL_EMBMS_FLUTE_HDR_SZ                                  16

/*! @brief Maximum number of radio frames in a segment
*/
#define LTE_RLCDL_EMBMS_MAX_SEG_RFS                                   128

/*! @brief Maximum value of embms service priority
*/
#define LTE_RLCDL_EMBMS_MAX_SERV_PRI                                   5


/*! @brief convert network byte order uint16 into host byte order uint16
*/
#define LTE_RLCDL_EMBMS_NTOHS(src) \
  ( (src & 0x00FF) <<8) | ( (src & 0xFF00) >>8 )


/*! @brief 404 array size
*/
#define LTE_RLCDL_EMBMS_404_ARR_SZ                                    8

/*! @brief Segment size below which a TOI is considered as inband TOI
*/
#define LTE_RLCDL_EMBMS_INBAND_TOI_THRES                              4000

/*! @brief OneX reservation duration value to be used while making two client 
  *  reservations
  */
#define LTE_RLCDL_EMBMS_UPDATED_1X_RES_DUR                            90



/*! @brief MRB statistics
*/
typedef struct
{
  uint32  num_data_pdu;       /*!< number of RLC data pdu received */
  uint32  data_pdu_bytes;     /*!< RLC data PDU in bytes received */
  uint32  num_invalid_pdu;    /*!< number of invalid PDUs */
  uint32  invalid_pdu_bytes;  /*!< invalid PDU in bytes */
  uint32  num_sdu;            /*!< number of RLC sdu reassembled */
  uint32  sdu_bytes;          /*!< RLC SDUs in bytes reassembled */
  uint32  num_holes;          /*!< number of times holes are encountered */
  uint32  num_holes_discard;  /*!< number of times holes that resulted in seg discard */
  uint32  sdu_bytes_discard;  /*!< SDU bytes discarded: reassembly failure */
  uint32  num_empty_wm_sdu;   /*!< number of sdus dropped because of empty watermark pointer*/
} lte_rlcdl_embms_stats_s;

/*! @brief eMBMS control block structure
*/
typedef struct
{
  lte_rlcdli_state_e          rb_state; /*!< state of the RB */
  lte_rlcdl_mrb_info_s        cfg;      /*!< configuration */
  uint16                      vr_r;              /*!< VR(R) or VR(UR) */
  dsm_item_type*              sdu_fragment_ptr;   /*!< residual sdu fragment */
  uint32                      sdu_fragment_sz;    /*!< size of the fragment */
  lte_rlcdl_embms_stats_s     stats;    /*!< statistics of the RB */
} lte_rlcdl_embms_cb_s;

typedef struct
{
  /*A unique combination of TOI and TSI identifies the current object*/
  /*Do not track the packet with TSI=0 and TOI=0 since it is a FDT*/
  uint16                toi; /*Transport Object ID*/
  uint16                tsi; /*Transport Session ID*/
  uint16                fec;/*in percentage*/
  uint16                symbol_size; /*size of the symbols*/
  uint32                size; /*Size of this TOI without the FEC*/
}lte_rlcdl_embms_toi_cfg_s;

typedef struct
{
  uint32                cur_seg; /*Current internal segment number which is 
                                 used. Updated if needed*/
  uint16                last_sbn; /*< Last SBN received on this TOI*/                                 
  uint16                last_esi; /*Last ESI received on this TOI.
                                  Used to calculate the number of 
                                  symbols/bytes lost so far on this TOI*/
  uint16                first_sbn; /*First SBN received on this TOI*/                                 
  uint16                first_esi; /*Used to calculate start of the segment*/
  boolean               rcvd_service_info; /*Whether we received the symbol
                                  size and FEC from the service layer for this TOI. 
                                  Else use the previous information*/
  boolean               video_toi; /*This TOI is treated as video TOI if the 
                                  size(or bytes received) is greater than a 
                                  threshold  or if this information is received
                                  from service layer*/
  boolean               decoded; /*Got an indication from service layer 
                                   whether this TOI is decoded*/
  uint16                decoded_symbol_id; /*The symbol ID which the service 
                                   layer used to decode the TOI*/
  lte_sfn_s             first_sfn; /*!< The system frame number of first 
                                   packet on this TOI */
  lte_sfn_s             last_sfn;  /*!< The system frame number of last 
                                   packet on this TOI*/
  uint16                last_beg_esi; /*First ESI of the last packet received.*/
  uint16                last_pay_size; /*Payload size of the last packet received*/
  boolean               logged_exp_bytes; /*Has the expected bytes already been logged*/
}lte_rlcdl_embms_toi_state_s;

typedef struct
{
  uint32                bytes_rcvd; /*Bytes received so far on this TOI*/
  uint32                bytes_lost; /*Bytes lost so far on this TOI*/
  uint32                approx_bytes_lost; /*Approximate bytes lost so far*/
  uint32                symbols_rcvd; /*Total Symbols received so far*/
  uint32                symbols_lost; /*Total Symbola lost so far*/
  uint32                num_addl_symbols; /*Number of additional symbols processed
                                    for this TOI after it is decoded*/
}lte_rlcdl_embms_toi_stats_s;

/*!@brief Per TOI information*/
typedef struct
{
  lte_rlcdl_embms_toi_cfg_s cfg;
  lte_rlcdl_embms_toi_state_s state;
  lte_rlcdl_embms_toi_stats_s stats;
}lte_rlcdl_embms_toi_s;

/*! @brief The type of Audio Video Transmit pattern */
typedef enum
{
  LTE_RLCDL_EMBMS_AV_MUXED, /*!<Audio video multiplexed*/
  LTE_RLCDL_EMBMS_AV_SEP_INTER, /*!< Audio video separate but interleaved*/
  LTE_RLCDL_EMBMS_AV_SEP_NONINTER_AFIRST,/*!<Audio and video seperate and
  non-interleaved with audio coming first*/
  LTE_RLCDL_EMBMS_AV_SEP_NONINTER_VFIRST,/*!<Audio and video seperate and
  non-interleaved with video coming first*/ 
  LTE_RLCDL_EMBMS_AV_INVALID/*!<Invalid transmit pattern*/
}lte_rlcdl_embms_av_trasmit_pattern_e;

/*!@brief service config information*/
typedef struct
{
  lte_mbsfn_area_id_t   area_id; /*!< Area id */
  lte_pmch_id_t         pmch_id; /*!< pmch id */
  lte_lc_id_t           lcid; /*!< lc id */
  boolean               valid; /*Whether the following information is valid or not*/
  boolean               disable_reservation_flag;
  boolean               use_background_low_priority;
  uint32                serv_type;
  boolean               seg_start_hipri; /*< Segment start should have a high priority*/
  uint32                seg_duration; /*Segment duration in milliseconds*/
  uint8                 fec;/*in percentage*/
  uint32                 priority; /*Service priority level*/
  boolean               muxed_data; /*0- audio and video not multiplexed, 1- multiplexed*/
  uint32                video_data_rate; /*video datarate if not multiplexed, combined data rate
                                   if multiplexed*/
  uint32                audio_data_rate;
  uint32                msp_dur; /*Duration of the MSP in milliseconds*/
  uint8                 thres_pri; /*Threshold loss at which to bump up priority. 
    Thres_pri(th_a) = 1 - A/(1+Pfec)*/
  uint8                 giveup_pri; /*Loss rate beyond which the UE gives up bumping priority*/
  uint32                vid_seg_thres_size; /*Minumum size of the video segment (Th_a)*/
  uint32                th_a; /*Threshold to decide audio versus video segment in 
  bytes per second. NV/EFS item,default = 20,000 bytes/sec*/
  uint32                sca_fact_a; /*Threshold scaling factor A. 
  NV/EFS item, default = 103*/
  uint32                sca_fact_b; /*Threshold scaling factor B, used for not raising 
  priority when loss > Th_pri + B*1/N,where N = FLOOR{SegDuration/MSP} */
  boolean               first_burst_hi_pri; /*Whether first burst of every segment 
  should be high priority*/
  uint8                 enable_per_av_object_info; /*todo: should be enum*/
  uint8                 enable_per_status_object_info; /*todo: should be enum*/
  uint32                num_bytes_subfn; /*Number of bytes per subframe*/
  uint16                min_msp_bcast; /*Minimum MSP value to use LOW priority instead of 
  background priority*/
  uint16                hi_pri_timer; /*< Duration of high priority after a new service started - 
     default=10s*/
  lte_rlcdl_embms_av_trasmit_pattern_e av_trans_pat; /*Audio video transmit pattern*/
  uint32                sca_fact_delta; /*Threshold scaling factor B, used for not raising 
  priority when loss > Th_pri + B*1/N,where N = FLOOR{SegDuration/MSP} */
  uint32                sca_fact_b_min; /*Threshold B factor considering BLER*/
}lte_rlcdl_embms_service_cfg_s;

/*!@brief service state information*/
typedef struct
{
  boolean               raise_priority; /*< whether to send a raise priority indication to ML1*/
  lte_l2_timer_s        burst_end_timer; /*<Timer set to expire at end of burst */
  lte_l2_timer_s        segment_timer; /*<Timer set to Segment duration*/
  lte_l2_timer_s        onex_page_decode_timer; /*< Timer set to MSP before the next oneX_page_decode*/
  /*To be used until the info is received from the service layer*/
  lte_l1_mbsfn_msi_cfg_s msi_cfg; /*< Latest MSI for the interested Area*/
  lte_sfn_s             last_sfn; /*< sfn of the last received packet*/
  lte_rlcdl_embms_toi_s toi[LTE_RLCDL_EMBMS_TOI_ARR_SZ];
  uint16                 cur_burst_bitmask[LTE_RLCDL_EMBMS_MAX_SEG_RFS];
  lte_sfn_s             cur_burst_start; /*< sfn of the current burst start*/
  lte_sfn_s             cur_burst_end; /*< sfn of the current burst end*/
  lte_sfn_s             updated_cur_burst_end; /*< Add 1subfn to current burst end*/
  uint32                cur_burst_num_sfn; /*< Number of sfn in current burst*/
  uint32                cur_seg; /*Internal current segment number*/
  uint16                cur_toi; /*< Currently monitored toi to bump up or bump down priority*/
  lte_rlcdl_embms_toi_cfg_s cur_toi_cfg; /*< Config of the latest video toi available.*/
  trm_reason_enum_t     cur_priority; /*Current priority*/
  uint32                cur_loss; /*< Last calculated loss in percentage*/
  uint16                last_toi_index; /*< TOI index of the latest TOI received so far*/
  boolean               pending_serv_msg; /*< Whether a service message is received
   before LTE stack is up waiting to be processed*/
  wds_embms_content_desc_update_req_msg_v01 serv_msg; /*< Pending service message*/
  uint16                last_video_toi; /*< last video TOI on which the last packet was received*/
  uint16                cur_404_index;
  uint32                unique_404_buffer[LTE_RLCDL_EMBMS_404_ARR_SZ];
  boolean               msi_received; /*< Whether MSI is received in the last
   MSP or not*/
  boolean               onex_ta_collision_possible; /*< 1x tuneaways collision are possible with PMCH */
  boolean               onex_page_recieved; /*1x unlock received for the
  current TOI*/
  boolean               onex_page_decode_timer_running;
  uint16                onex_page_cycle_in_ms;
  timetick_type         onex_last_page_timetick;
  uint32                onex_collision_bytes; /*1x collision bytes*/
  uint16                onex_ta_duration; /*!< Value of the 1x duration to be used*/
  uint16                rcvd_onex_unlock_by_duration; /*!< 1x unlock by duration actually
  received from ML1*/
  uint32                max_onex_collision_bytes; /*maximum 1x collision bytes*/
  boolean               onex_front_collision_flag; /*Whether 1x start collides with the
   current burst or not*/
  uint32                max_extra_bytes_lost; /*Maximum number of extra bytes lost
  at the time of giving up*/
  uint32                min_extra_bytes_expected; /*Minimum number of extra
  bytes expected at the time the toi has received enough bytes*/
}lte_rlcdl_embms_service_state_s;

typedef struct
{
  uint32                num_bytes_rcvd; /*Number of bytes received so far*/
  uint32                num_bytes_lost; /*Number of bytes lost so far*/
  uint32                approx_bytes_lost; /*Approximate bytes lost so far*/
  uint32                symbols_rcvd;
  uint32                symbols_lost;
}lte_rlcdl_embms_cur_seg_stats_s;


/*!@brief service statistics information*/
typedef struct
{
  uint32                num_toi_rcvd;       /*Internal representation of the total number of segments*/
  uint32                num_inband_toi_rcvd; /*Total number of inabnd updates received*/
  uint32                num_toi_fail_decodes; /*Number of TOIs which are not decoded at SL*/
  uint32                num_toi_pass_decodes;
  uint32                num_fdt_misses;
  uint32                num_404_errors;
  uint32                num_unique_404_errors;
  uint32                num_fdt_pkts; /*Number of FDTs received for this service*/
  uint32                num_msi_rcvd; /*Number of MSIs received*/
  uint32                num_msi_missed;
  uint32                num_pri_req; /*Total number of requests made to ML1 - @burst expiry*/
  uint32                num_high_pri_req;  /*Total number of high priority requests*/
  uint32                num_low_pri_dur_ms; 
  uint32                num_low_pri_adj_dur_ms; 
  uint32                num_high_pri_dur_ms; 
  uint32                num_high_pri_adj_dur_ms;
  uint32                num_invalid_msp_errors; /*Number of invalid MSP info given by ML1*/
  uint32                num_acc_prediction; /*Number of times L2 made an accurate prediction*/
  lte_rlcdl_embms_cur_seg_stats_s       seg_stats;
  uint32                num_partial_ip_dropped; /*Total number of discarded partial IP packets*/
  uint16                num_unlocks_rcvd; /*Total number of unlock nows received*/
  uint16                num_unlocks_granted; /*Total number of unlocks granted*/
  uint16                num_1x_unlock_rejected; /*Total number of 1x unlocks rejected*/
  uint16                num_G_unlock_rejected; /*Total number of Geran unlocks rejected*/
  uint16                num_collision_unlock_honored; /*Total number of unlocks with collisions
   that are honored. Only those with unlock now are counted*/
  uint16                num_collision_unlock_rejected; /*Total number of unlocks with collsions
   that are rejected. Only those with unlock now are counted*/
  uint16                num_highsfn_error; /*Number of times highsfn is calculated 
     incorrectly*/
  uint16                num_g_rej_due_to_1x; /*Number of G rejections due to 1x*/
  uint16                num_1x_collisions; /*Number of 1x collisions with the current service*/
  uint16                num_1x_page_unlocks_rcvd; /*Number of 1x page unlocks received from ML1*/
  uint16                num_1x_page_notifications; /*Number of 1x page notifications received directly
   from 1x via an API*/
  uint16                num_segments_with_no_1x_page; /*Number of TOIs which are predicted to not have
   1x collisions*/
  uint16                num_unlock_reason_page_demod; /*Number of unlock requests received with page demod reason*/
  uint16                num_page_demod_rej; /*Number of unlock requests received 
   with page demod reason which are rejected by L2*/
  uint16                num_reserve_w_1x; /*Number of reservations made after 
   taking minimum 1x collisions bytes into account*/
  uint16                num_reserve_wo_1x; /*Number of reservations made without
   taking 1x collision bytes into account*/
  uint16                num_reserve_w_1x_max; /*Number of reservations made after 
   taking maximum 1x collisions bytes into account for L2 high collision(from ML1) with 1x */
  uint16                num_highsfn_adj_to_all_1x_res; /*Number of times 
   high_sfn is adjusted to delay by 15ms so that the ML1 high reservation does not collide with 1x*/
  uint16                num_reserve_w_det_bler; /*Number of reservations made 
   after saving bytes for BLER deterministically*/
  uint16                num_reserve_w_opp_bler; /*Number of reservations made 
   after saving bytes for BLER opportunistically*/
  uint16                num_adaptive_b_denials; /*Number of page denials due 
   to adaptive B factor accounting for BLER*/
}lte_rlcdl_embms_service_stats_s;

/*!@brief service info*/
typedef struct
{
  lte_rlcdl_embms_service_cfg_s   cfg;
  lte_rlcdl_embms_service_state_s state;
  lte_rlcdl_embms_service_stats_s stats;
}lte_rlcdl_embms_service_info_s;

/*! @brief Define the kind of log subpacket */
typedef enum
{
  LTE_RLCDL_EMBMS_DPI_SDU, /*!<RLC embms SDU*/
  LTE_RLCDL_EMBMS_DPI_PER_OBJ_INFO,/*!<Service layer per object info*/
  LTE_RLCDL_EMBMS_DPI_PER_OBJ_DECODE,/*!<Service layer per object decode info*/
  LTE_RLCDL_EMBMS_DPI_PER_SERV_INFO,/*!<Per service information*/
  LTE_RLCDL_EMBMS_DPI_PRI_MSG, /*!<Service bump up priority msg sent to ML1*/
  LTE_RLCDL_EMBMS_DPI_UNLOCK_REQ,/*!<Unlock request received from ML1*/
  LTE_RLCDL_EMBMS_DPI_INVALID/*!<Invalid packet type*/
} lte_rlcdl_embms_dpi_pkt_type_e;

/*! @brief Per IP packet info*/
typedef struct
{
  lte_rlcdl_embms_dpi_pkt_type_e  packet_type; /*< DPI_SDU=0; Per_Obj_Info=1; Per_Obj_Decode=2
                Per_Serv_Info=3, Dpi_Pri_Msg=4*/
  lte_sfn_s sfn; /*< Subframe number at which this packet is received*/  
  uint16 tsi; /*< TSI */
  uint16 toi; /*< TOI */
  uint16 sbn; /*< SBN */
  uint16 esi; /*< ESI */
  uint16 databytes; /*< Number of data bytes in this SDU after removing the 
                      IP header and the flute headers*/
  uint16 last_sbn; /*< SBN of the last packet received on this TOI/TSI combo*/
  uint16 last_esi; /*< ESI of the last packet received on this TOI/TSI combo*/
  lte_sfn_s first_sfn; /*< Subframe number at which the first packet for this TOI
                      is received*/
  lte_sfn_s last_sfn; /*< Subframe number at which the last packet for this TOI
                      is received*/
  uint32 bytes_rcvd; /*< Bytes received so far on this TOI*/
  uint32 bytes_lost; /*< Bytes lost so far on this TOI*/
  uint32 approx_bytes_lost; /*< Approximate bytes lost so far on
                      this TOI*/
  uint32 symbols_rcvd;/*< Total symbols received on this TOI*/
  uint32 symbols_lost;/*< Total symbols lost so far on this TOI*/
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
  lte_mbsfn_area_id_t   area_id; /*!< Area id */
  lte_pmch_id_t         pmch_id; /*!< pmch id */
  lte_lc_id_t           lcid; /*!< lc id */
  uint8 decode_status; /*!< 0-Success*/
  uint16 last_decode_esi; /*< Last decoded ESI */
  uint8  fec; /*< FEC for this TOI object in percentage*/
  uint16 symbol_size; /*< Symbol size of this TOI object*/
  uint32 object_size; /*< Actual size of this TOI object without the FEC*/
  uint32 seg_duration; /*< Segment duration in milliseconds*/
  uint32 serv_priority; /*Service priority level*/
  uint32 serv_type; /*Type of service - File download or streaming*/
  uint16 video_data_rate; /*video datarate if not multiplexed, combined data rate
                                   if multiplexed*/
  uint16 audio_data_rate; /*Audio rate if not multiplexed. Else 0*/
  uint16 switched_toi; /*TOI during TOI switch. 0 if there is no TOI switch*/
  uint16 instant_loss; /*Loss from the previous sdu*/
  uint32 num_extra_bytes_exp; /*Number of extra bytes expected after TOI decoded*/
  uint32 num_extra_bytes_lost; /*Number of extra bytes lost over the allowed limit
  making DPD enter give up state*/
} lte_rlcdl_embms_dpi_sdu_s;

/*! @brief eMBMS static data structure
*/
typedef struct
{
  mutils_circ_q_s*      mac_rlc_buf_ptr;  /*!< MAC-RLC circular buffer pointer */
  dsm_watermark_type*   rlc_embms_wm_ptr;        /*!< RLC-A2 watermark */
  lte_rlcdl_embms_cb_s  mcch_cb[LTE_EMBMS_MAX_MBSFN_AREA]; /*!< MCCH control block */
  lte_rlcdl_mcch_pdu_ind_s rrc_ind;       /*!< Indicating MCCH msg for RRC */
  uint8                 num_active_mrb;   /*!< number of active mrb */
  uint8                 dyn_pri;         /*!< Whether dynamic priority logic is active or 
                                             not*/
  boolean               tuneaway_enabled; /*!< Whether tuneaways are enabled are not*/
  uint32                mcch_cb_idx;      /*!< avaiable mcch control block index */
  uint32                num_embms_sdu_drop;  /*!<num of times LTE_RLCDL_MCCH_PDU_IND msg 
                                             sending fails */
  uint32                embms_bytes_cnt;  /*!< Num of emBMS bytes in RLCDL buffer */
  lte_rlcdl_embms_service_info_s embms_dyn_pri; /*< static structure to the 
                                             dynamic priority logic*/
  lte_rlcdl_embms_dpi_sdu_s dpi_sdu;  /*< static structure to log the per subframe embms
                                          information*/
  pthread_mutex_t       dpd_lock; /*!< Mutex lock over DPD processing */
} lte_rlcdl_embms_static_s;

/*! @brief eMBMS dynamic data structure
*/
typedef struct
{
  lte_rlcdl_embms_cb_s mtch_cb[LTE_MAX_ACTIVE_MRB]; /*!< MTCH control block */
} lte_rlcdl_embms_dynamic_s;

/*! @brief collision information structure
*/
typedef struct
{
  lte_sfn_s collision_start;
  lte_sfn_s collision_end;
  boolean collision;
}lte_rlcdl_embms_collision_info_s;
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
extern void lte_rlcdl_embms_init( void );

extern lte_errno_e lte_rlcdl_embms_proc_cfg
(
  const lte_rlcdl_cfg_req_s*   msg_ptr  
);

extern void lte_rlcdl_embms_proc_release( void );

extern void lte_rlcdl_embms_proc_pdus( void );

extern dsm_watermark_type* lte_rlcdl_embms_get_a2_wm( void );

extern lte_rlcdl_embms_dynamic_s* lte_rlcdl_embms_get_embms_data
(
  void
);

extern lte_rlcdl_embms_static_s* lte_rlcdl_embms_get_static_data
( 
  void 
);

extern uint8 lte_rlcdl_embms_get_num_active_mrb( void );

extern void lte_rlcdl_embms_register_req
(
  lte_rlcdl_embms_wm_register_req_msg_s* msg_ptr
);

extern void lte_rlcdl_embms_deregister_req
(
  lte_rlcdl_embms_wm_deregister_req_msg_s* msg_ptr
);

extern void lte_rlcdl_embms_free_dynamic_mem(void);
extern lte_rlcdl_embms_service_info_s* lte_rlcdl_embms_get_dsds_serv
(
  void
);

extern uint16 lte_rlcdl_embms_find_toi_index(uint16 toi);

extern void lte_rlcdl_embms_process_msi( lte_cphy_msi_req_s* msi_req);

extern void lte_rlcdl_embms_process_msp_data
(
  lte_cphy_embms_get_msp_data_ind_s* msp_msg
);

extern void lte_rlcdl_embms_process_burst_timer_expiry( void );
extern void lte_rlcdl_embms_process_seg_timer_expiry( void );

extern void lte_rlcdl_embms_process_onex_page_decode_timer_expiry( void );

extern void lte_rlcdl_embms_process_desc_update_req
(
  ds_3gpp_embms_content_desc_update_msg_s* msg_ptr
);

extern void lte_rlcdl_embms_process_crat_msim_feature_req
(
  lte_common_msim_crat_feature_req_s* msg_ptr
);

extern void lte_rlcdl_embms_create_mutexes
(
  void
);

extern void lte_rlcdl_embms_destroy_mutexes
(
  void
);

extern void lte_rlcdl_embms_lock_dpd(void);

extern void lte_rlcdl_embms_unlock_dpd(void);
extern void lte_rlcdl_embms_qsh_proc(void);

extern void lte_rlcdl_embms_qsh_reset(void);
extern void lte_rlcdl_embms_qsh_invoke_reset(void);
extern void lte_rlcdl_embms_update_onex_ta_duration(uint16 ta_dur);



#endif /* LTE_RLCDL_EMBMS_H */

