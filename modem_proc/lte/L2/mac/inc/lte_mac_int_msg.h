/*!
  @file
  lte_mac_int_msg.h

  @brief
  This file contains all the define MAC internal UMIDs and 
  its data structures 


*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/inc/lte_mac_int_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   el      CR911691: FR30143 stop TA timer when re-try preamble for contention
                   based HO rach to allow apply TA in msg2
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
08/03/14   mg      CR702710: QSH analysis and reset
07/07/14   el      CR 688341: SPS fix for no SR sending after reconnection
01/05/12   st      Added a new eMBMS config internal message
10/25/11   st      Added a new message to be sent on high data arrival to
                   MAC CTRL from the MAC QOS MODULE
09/17/10   wshaw   add watchdog support for LTE tasks
06/09/10   bn      Changed the RC from uint16 to int16
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_INT_MSG_H
#define LTE_MAC_INT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>

/* Start of L2 internal including */
#include "lte_mac.h"
#include "lte_mac_rrc.h"
#include "lte_mac_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*-----------------------------------------------------------------------*/
/*     LTE MAC Internal Request messages                                 */
/*-----------------------------------------------------------------------*/
/*! @brief Internal requests that the LTE MAC module receives/sends  */
enum
{
  LTE_MAC_FIRST_REQI = MSGR_UMID_BASE(MSGR_LTE_MAC, MSGR_TYPE_REQI),
  /*! send from MAC Control to MAC DL */
  MSGR_DEFINE_UMID(LTE, MAC, REQI, DL_CONFIG, 0x0, (void*)),
  /*! send from MAC control to MAC UL */
  MSGR_DEFINE_UMID(LTE, MAC, REQI, RANDOM_ACCESS, 0x1, (void*)),
  /* send from MAC control to MAC UL */
  MSGR_DEFINE_UMID(LTE, MAC, REQI, ACCESS_ABORT, 0x2, (void*)),
  /*! send from MAC Control to MAC DL */
  MSGR_DEFINE_UMID(LTE, MAC, REQI, EMBMS_CONFIG, 0x3, (void*)),
  /*! send from MAC Control to MAC UL and MAC DL*/
  MSGR_DEFINE_UMID(LTE, MAC, REQI, QSH_ANALYSIS, 0x4, (void*)),

  LTE_MAC_MAX_REQI,
  LTE_MAC_LAST_REQI       = LTE_MAC_MAX_REQI - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE MAC Internal Confirmation messages                            */
/*-----------------------------------------------------------------------*/
/*! @brief internal Confirmations that the LTE MAC module sends/receives  */
enum
{
  LTE_MAC_FIRST_CNFI = MSGR_UMID_BASE(MSGR_LTE_MAC, MSGR_TYPE_CNFI),
  /*! send from MAC DL to MAC control */
  MSGR_DEFINE_UMID(LTE, MAC, CNFI, DL_CONFIG, 0x0, (void*)),
  /*! send from MAC UL to MAC control*/
  MSGR_DEFINE_UMID(LTE, MAC, CNFI, RANDOM_ACCESS, 0x1, (void*)),
  /*! send from MAC UL to MAC control */
  MSGR_DEFINE_UMID(LTE, MAC, CNFI, ACCESS_ABORT, 0x2, (void*)),
  /*! send from MAC DL to MAC control */
  MSGR_DEFINE_UMID(LTE, MAC, CNFI, EMBMS_CONFIG, 0x3, (void*)),

  LTE_MAC_MAX_CNFI,
  LTE_MAC_LAST_CNFI       = LTE_MAC_MAX_CNFI - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE MAC Internal Indication messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief Internal Indications that the LTE MAC module sends/receives */
enum
{
  LTE_MAC_FIRST_INDI = MSGR_UMID_BASE(MSGR_LTE_MAC, MSGR_TYPE_INDI),
  /*! send from MAC DL to UL */
  MSGR_DEFINE_UMID(LTE, MAC, INDI, MSG4_RESULT, 0x0, (void*)),
  /*! send from MAC UL to DL*/
  MSGR_DEFINE_UMID(LTE, MAC, INDI, MONITOR_RAR, 0x1, (void*)),
  /*! send from MAC DL to UL */
  MSGR_DEFINE_UMID(LTE, MAC, INDI, PRACH_RESP_RESULT, 0x2, (void*)),
  /*! send from MAC control to MAC UL */
  MSGR_DEFINE_UMID(LTE, MAC, INDI, INFORM_ACTION, 0x3, (void*)),
  /*! send from MAC control to MAC UL to flush the UL TB log */
  MSGR_DEFINE_UMID(LTE, MAC, INDI, FLUSH_UL_TB_LOG, 0x4, (void*)),
  /*! send from MAC UL to MAC control to trigger an SR */
  MSGR_DEFINE_UMID(LTE, MAC, INDI, EXT_HIGH_DATA_ARRIVAL, 0x5, (void*)),
  /*! send from MAC control to MAC UL */
  MSGR_DEFINE_UMID(LTE, MAC, INDI, UL_CONFIG, 0x6, (void*)),

  LTE_MAC_MAX_INDI,
  LTE_MAC_LAST_INDI       = LTE_MAC_MAX_INDI - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE MAC Internal Timer Event Message                              */
/*-----------------------------------------------------------------------*/
/*! @brief Internal Timer Tick Events of all the LTE MAC */
enum 
{
  LTE_MAC_FIRST_TMRI = MSGR_UMID_BASE(MSGR_LTE_MAC,MSGR_TYPE_TMRI),
  
  /*! the time expired event for MAC UL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,UL_TIMER_EXPIRED,0x00,(void*)),
  /*! the TA timer expired event for MAC DL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,DL_TA_TIMER_EXPIRED,0x01,(void*)),
  /*! the log timer expired event for MAC DL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,DL_LOG_TIMER_EXPIRED,0x02,(void*)),
  /*! the time expired event for MAC CTRL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,CTRL_TIMER_EXPIRED,0x03,(void*)),
  /*! the time expired event for LTE L2 UTIL */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,TEST_UTIL_TIMER_EXPIRED,0x04,(void*)),
  /*! the WDOG timer expired event for MAC DL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,DL_WDOG_TIMER_EXPIRED,0x05,(void*)),
  /*! the WDOG timer expired event for MAC UL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,UL_WDOG_TIMER_EXPIRED,0x06,(void*)),
  /*! the WDOG timer expired event for MAC CTRL Task */
  MSGR_DEFINE_UMID(LTE,MAC,TMRI,CTRL_WDOG_TIMER_EXPIRED,0x07,(void*)),

  LTE_MAC_MAX_TMRI,
  LTE_MAC_LAST_TMRI       = LTE_MAC_MAX_TMRI - 1
};

/*-----------------------------------------------------------------------*/
/*! @brief LTE MAC Internal Messages define structures                   */
/*-----------------------------------------------------------------------*/
/*! @brief MAC internal DL config request request */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  lte_l2_cfg_reason_e cfg_type;  /*!< type of configuration */
  boolean             ta_timer_included; /*!< indicate ta_timer is present or not */
  uint16              ta_timer; /*!< TA timer expiry value*/
} lte_mac_intmsg_dl_config_req_s;

/*! @brief MAC internal eMBMS config request request */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  lte_l2_cfg_reason_e      cfg_type;
  lte_mac_embms_cfg_s      *embms_cfg; /*!< Pointer to embms cfg structure */
} lte_mac_intmsg_embms_config_req_s;

/*! @brief MAC internal random access request */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  uint8                    ra_id; /*!< RA index. Invalid value = 0xFF*/
  /*! Explicitly signalled PRACH resource PRACH Mask Index */ 
  uint8                    ra_id_mask; 
  int16                    rc;    /*!< radio condition TBD */
  uint16                   crnti; /*!< UE C-RNTI*/
  lte_mac_access_reason_e  access_reason; /*!< access reason */
  uint8                    conn_req_sdu[LTE_MAC_MAX_CONN_REQ_SDU_SIZE];/*!< connection req SDU*/
  lte_mac_rach_cfg_s       *rach_cfg; /*!< current RACH configuration */
  uint8                    shift_factor; /*!<current RACH count shift factor*/
} lte_mac_intmsg_random_access_req_s;

/*! @brief Access Confirmation from UL Task to Control Task
*/
typedef struct
{
  msgr_hdr_struct_type     hdr;            /*!< message header */
  /* Message payload */
  lte_mac_access_reason_e  access_reason;  /*!< access reason */
  boolean                  crnti_included; /*!< if C-RNTI is present */
  uint16                   crnti;          /*!< C-RNTI value */
} lte_mac_intmsg_random_access_cnf_s;

/*! @brief MAC Message 4 result indication */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  boolean                  matched; /* !< UE ID matched success */
  lte_sfn_s                sfn;     /*!< The system frame number */
} lte_mac_intmsg_msg4_result_ind_s;

/*! @brief MAC monitor RAR indication */
typedef struct 
{
  msgr_hdr_struct_type     hdr;   /*!< message header */
  /*! Message payload */
  boolean                  cb_rach;      /*!< CB RACH, won't terminate after MSG2 */
  boolean                  check_crnti;  /*!< check to make sure the C-RNTI is used to decode PDCCH */
  boolean                  match_contention_mce; /*!< match the contention resolution MCE */
  uint8                    ra_id;        /*!< RA Index between 0 and 63 */
  uint8                    match_id[LTE_MAC_MAX_CONN_REQ_SDU_SIZE];  /*!< UE Identification to be matched */
  lte_mac_access_reason_e  access_reason;   /*!< access reason */
} lte_mac_intmsg_monitor_rar_ind_s;

/*! @brief MAC PRACH response result indication */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  
  boolean                    success; /*!<  RAR response match */
  boolean                    backoff_val_incl; /*!<  The backoff_val field is valid */
  uint8                      backoff_val; /*!<  overload indicator value received in RAR response */
  uint16                     temp_rnti;   /*!< TEMP RNTI  received in RAR response */
  boolean                    ta_incl;     /*!< Timing advance valid */
  uint16                     ta;          /*!< Timing advance to adjust UL timing Range: TDB */
  uint32                     msg3_grant_raw; /*!< MSG3 grant raw byte from the RAR*/
} lte_mac_intmsg_prach_resp_result_ind_s;

/*! @brief Request to abort ACCESS procedure from MAC Control to MAC UL */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
} lte_mac_intmsg_access_abort_req_s;

/*! @brief Abort ACCESS procedure confirmation from MAC UL to MAC Control */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
} lte_mac_intmsg_access_abort_cnf_s;

/*! @brief Internal Timer Expired Event Message for MAC Tasks */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  int                      timer_ctx;/*!< timer context */        
} lte_mac_intmsg_timer_expired_tmr_s;

/*! @brief Inform action indication from MAC Control to MAC UL */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /* informed action */
  lte_mac_inform_action_e  action;
  /*! Message payload */
} lte_mac_intmsg_inform_action_ind_s;

/*! @brief UL config indication from MAC Control to MAC UL */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /* UL configuration pointer */
  lte_mac_ul_cfg_s	*cfg_ptr;
  /* UL config bitmask */
  uint8 ul_cfg_bitmask;
  /*! Message payload */
} lte_mac_intmsg_ul_config_ind_s;

/*! @brief Send SR indication from PDCP UL to MAC Control */
typedef struct 
{
  msgr_hdr_struct_type     hdr; /*!< message header */
  /*! Message payload */
  lte_lc_id_t              lcid;
} lte_mac_intmsg_ext_hd_arrival_ind_s;

/*! @brief LTE_MAC_QSH_ANALYSIS_REQI request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  /*! whether to run analysis, reset stats ... */
  uint32 action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed; 
} lte_mac_intmsg_qsh_analysis_req_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_MAC_INT_MSG_H */
