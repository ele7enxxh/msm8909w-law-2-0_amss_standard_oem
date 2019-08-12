/*!
  @file
  lte_LL1_log_multi_rat.h

  @brief
  ul log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Fri May 14 11:46:16 2010.
  Command line used: perl w2h.pl log_ul ul 2 2 2 2 1 1
   Note: The params, [2 2 2 2 1 1], correspond to version numbers for
         each log message in the listed order in the ICD

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_multi_rat.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_MULTI_RAT_H
#define LTE_LL1_LOG_MULTI_RAT_H


/*==========================================================================*/
/*! @brief
 * 1 LTE LL1 Multi Rat DL Report (Log code ID: TBD)
 *
 *  This log packet gives important parameters related to the multi-rat 
 *  report. The log record is generated once every 20 sub-frames 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_MULTI_RAT_CONFLICT_DL_VERSION 1
#define LTE_LL1_LOG_MULTI_RAT_CONFLICT_DL_NUMBER_OF_RECORDS_FIXED 20

/*! @brief lte_LL1_log_multi_rat_conflict_dl_records_s struct
 */
typedef struct
{

  uint32 reg_usmtr_start                  ;  ///<  resigster start ustmr time
  uint32 reg_usmtr_end                    ;  ///<  resigster end ustmr time
  uint32 freq_id                          ;  ///<  freq ID
  uint32 priority                    : 8  ;  ///<  priority
  uint32 conflict_chk_status         : 4  ;  ///<  conflict check status: 
                                             ///<  0=FW_CXM_NO_CONFLICT, 
                                             ///<  1=FW_CXM_PROCEED,
                                             ///<  2=FW_CXM_YIELD,
  uint32 activitiy_type              : 4  ;  ///<  activity type
                                             ///<  0=FW_CXM_ACTIVITY_NONE,
                                             ///<  1=FW_CXM_ACTIVITY_RX,
                                             ///<  2=FW_CXM_ACTIVITY_TX,
  uint32 frame_subfn                 : 16 ;  ///<  frame/subframe, as 10*SFN + SF
  uint32 action_type                      ;  ///<  0=NONE
                                             ///<  1=blanking
                                             ///<  2=backoff (for TX only)
}lte_LL1_log_multi_rat_conflict_dl_records_s;


/*! @brief lte_LL1_log_multi_rat_conflict_dl_ind_struct
 */
typedef struct
{
  uint32 version                        : 8  ;  ///< Log packet version. Range: 0...255.
  uint32 number_of_records              : 5  ;  ///< Range: 0..10
  uint32 reserved0                      : 19 ; 
  uint16 dispatch_sfn_sf                     ;  ///< System / Sub Frame Number, as 10*SFN + SF
  uint16 reserved1                           ; 
  lte_LL1_log_multi_rat_conflict_dl_records_s conflict_record[LTE_LL1_LOG_MULTI_RAT_CONFLICT_DL_NUMBER_OF_RECORDS_FIXED];
}lte_LL1_log_multi_rat_conflict_dl_ind_struct;


/*==========================================================================*/
/*! @brief
 * 1 LTE LL1 Multi Rat UL Report (Log code ID: TBD)
 *
 *  This log packet gives important parameters related to the multi-rat 
 *  report. The log record is generated once every 20 sub-frames 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_MULTI_RAT_CONFLICT_UL_VERSION 2
#define LTE_LL1_LOG_MULTI_RAT_CONFLICT_UL_NUMBER_OF_RECORDS_FIXED 20

/*! @brief lte_LL1_log_multi_rat_conflict_ul_records_s struct
 */
typedef struct
{

  uint32 reg_usmtr_start                  ;  ///<  resigster start ustmr time
  uint32 reg_usmtr_end                    ;  ///<  resigster end ustmr time
  uint32 freq_id                          ;  ///<  freq ID
  uint32 priority                    : 8  ;  ///<  priority
  uint32 conflict_chk_status         : 4  ;  ///<  conflict check status: 
                                             ///<  0=FW_CXM_NO_CONFLICT, 
                                             ///<  1=FW_CXM_PROCEED,
                                             ///<  2=FW_CXM_YIELD,
  uint32 activitiy_type              : 4  ;  ///<  activity type
                                             ///<  0=FW_CXM_ACTIVITY_NONE,
                                             ///<  1=FW_CXM_ACTIVITY_RX,
                                             ///<  2=FW_CXM_ACTIVITY_TX,
  uint32 frame_subfn                 : 16 ;  ///<  frame/subframe, as 10*SFN + SF
  uint32 action_type                      ;  ///<  0=NONE
                                             ///<  1=blanking
                                             ///<  2=backoff (for TX only)
  uint16 MTPL                             ;  ///<  MTPL value
  uint16 ul_chan_type                     ;  ///<  0=PUSCH 
                                             ///<  1=PUCCH 
                                             ///<  2=PRACH
                                             ///<  3=SRS
                                             ///<  4=UPPTS_SRS
}lte_LL1_log_multi_rat_conflict_ul_records_s;


/*! @brief lte_LL1_log_multi_rat_conflict_ul_ind_struct
 */
typedef struct
{
  uint32 version                        : 8  ;  ///< Log packet version. Range: 0...255.
  uint32 number_of_records              : 5  ;  ///< Range: 0..10
  uint32 reserved0                      : 19 ; 
  uint16 dispatch_sfn_sf                     ;  ///< System / Sub Frame Number, as 10*SFN + SF
  uint16 reserved1                           ; 
  lte_LL1_log_multi_rat_conflict_ul_records_s conflict_record[LTE_LL1_LOG_MULTI_RAT_CONFLICT_UL_NUMBER_OF_RECORDS_FIXED];
}lte_LL1_log_multi_rat_conflict_ul_ind_struct;


#endif //LTE_LL1_LOG_MULTI_RAT_H
