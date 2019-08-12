/*!
  @file
  lte_rrc_mdti.h

  @brief
  Header file internal to RRC MDT Module

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the CRE module and its UTF test cases.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_mdti.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#ifndef LTE_RRC_MDTI_H
#define LTE_RRC_MDTI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte.h>
#include <lte_cphy_msg.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_dispatcher.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_csp.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_rrc_controller.h>
#include "lte_rrc_crp.h"
#include "lte_rrc_config.h"
#include "lte_rrc_llc.h"





/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/ 
/* MAX memory that can be used to MDT from heap */
#define LTE_RRC_MDT_MAX_RESERVED_HEAP_MEM 256

/*Max reports size after which UL msg prebuild is required */
#define LTE_RRC_MDT_MAX_NON_PREBUILT_REPORTS_MEM 7500

/*Max number of individual reports in UL as per ASN1 */
#define LTE_RRC_MDT_ASN1_MAX_REPORTS 520

#define LTE_RRC_MDT_ABSOLUTE_TIME_STAMP_NUM_BITS 48
#define LTE_RRC_MDT_TCE_ID_NUM_BITS 1

/*! @brief Data structure for prebuilt UE Info response msgs
*/
typedef struct lte_rrc_mdt_prebuit_info
{
  /*!< Size of log meas report */
  uint64 rpt_size;

  /*!< Pointer to UE Info UL MSG */
  uint8 *ueinfo_ul_msg_ptr;

  /*!< Size of first report in UL_MSG */
  uint64 first_rpt_size;

  /*!< Pointer to first report in UL_MSG */
  uint8 *log_meas_rpt_ptr;

  /*!< Pointer to next UL msg */
  struct lte_rrc_mdt_prebuit_info *next_ul_msg_ptr; 

} lte_rrc_mdt_prebuit_info_s;


/*! @brief Data structure for non-prebuilt UE Info response msgs
*/
typedef struct lte_rrc_mdt_non_prebuit_info
{
  /*!< Size of log meas report */
  uint64 rpt_size;

  /*!< Pointer to log meas report */
  uint8 *log_meas_rpt_ptr;

  /*!< Relative time stamp */
  uint16 rel_time_stamp;

  /*!< Pointer to next report */
  struct lte_rrc_mdt_non_prebuit_info *next_rpt_ptr; 

} lte_rrc_mdt_non_prebuit_info_s;


/*! @brief Data structure for saved Logged MDT reports
*/
typedef struct
{
  /*!< Availability for Log meas reports */
  boolean log_meas_available;

  /*!< Availability Heap memory to store reports */
  boolean heap_mem_avail;

  /*!< Number of prebuild UL UEInfo response */
  uint64 num_prebuilt_ul_msgs; 

  /*!< Memory used so far for saving prebuilt MDT logs */
  uint64 prebiult_memory_used; 

  /*!< Root of Linked list of prebuild UL UEInfo response */
  lte_rrc_mdt_prebuit_info_s *prebuilt_ul_msg_root_ptr;

  /*!< Linked list of prebuild UL UEInfo response */
  lte_rrc_mdt_prebuit_info_s *prebuilt_ul_msg_ptr;

   /*!< Number of non prebuilt reports */
  uint64 num_non_prebuilt_reports; 

  /*!< Memory used so far for saving non-prebuilt MDT logs */
  uint64 non_prebuilt_reports_memory_used; 

  /*!<Root of Linked list of non-prebuild UL UEInfo response */
  lte_rrc_mdt_non_prebuit_info_s *non_prebuilt_ul_msgs_root_ptr;

  /*!< Linked list of non-prebuild UL UEInfo response */
  lte_rrc_mdt_non_prebuit_info_s *non_prebuilt_ul_msg_ptr;

} lte_rrc_mdt_report_s;

/*! @brief Data structure for Dynamic data
*/
typedef struct
{
  /*!< Status of MDT session*/
  boolean is_mdt_active;

  /*!< Reports received so far for this logged meas cfg  */
  uint64 reports_received;  

  /*!< 24*2 hrs  report validity timer  */
  lte_rrc_tmr_s mdt_valid_tmr;

  /*!< Validity period derived from EFS or 48hrs  */
  uint64 mdt_validity_period;  

  /*!< Number of ticks */
  uint8 mdt_valid_tmr_tick;

  /*!< MDT logging duration - t330 timer */
  lte_rrc_tmr_s t330_tmr;

  /*!< Logged MDT Report*/
  lte_rrc_mdt_report_s logged_mdt_rpt;

  /*!< Logged MDT Cfg*/
  lte_rrc_mdt_cfg_r10_s logged_mdt_cfg;

  /*!< Last sent cfg in UL*/
  lte_rrc_mdt_rpt_type_e last_build_rpt_type;

  /*!< RPLMN at the time of LoggedMeasConfig*/
  lte_rrc_plmn_s rplmn;

} lte_rrc_mdt_dd_s;


/*! @brief Data structure for Sttaic data
*/
typedef struct
{
  /*Efs read for logging duration */
  uint8 t330_efs;

  /*Efs read for valid timer duration */
  uint64 valid_tmr_efs;

  /* Efs read for LTE_RRC_MDT_LOG_MEAS_AVAILABILITY_DURING_T330 */
  uint8 report_log_meas_avail;

  /*Efs read for MAX heap memory allocated for MDT */
  uint64 max_heap_mem;

} lte_rrc_mdt_sd_s;

typedef struct
{
  /*!< Dynamically allocated part of private data */
  lte_rrc_mdt_dd_s *dd_ptr;

  /*!< Statically allocated part of private data */
  lte_rrc_mdt_sd_s *sd_ptr;

} lte_rrc_mdt_s;

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_process_dlm

===========================================================================*/
/*!
  @brief
  Function to validate the received request DLM

  @detail
  Response to UEInformationRequest by the network.

  @return
  None

*/
/*=========================================================================*/
boolean lte_rrc_mdt_process_dlm
(
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr /*! DL message ptr*/
);

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_get_status

===========================================================================*/
/*!
  @brief
  Function to get MDT status

  @detail
  Function to get MDT status

  @return
  TRUE if either t330 or validity timer are running

*/
/*=========================================================================*/
boolean lte_rrc_mdt_get_status( void );

/*===========================================================================

  FUNCTION:  lte_rrc_get_mdt_log_meas_available

===========================================================================*/
/*!
  @brief
  Function to check if we have any MDT reports

  @detail
  Function to check if we have any MDT reports

  @return
  TRUE if either t330 or validity timer are running

*/
/*=========================================================================*/

boolean lte_rrc_mdt_log_meas_available( void );


/*===========================================================================

  FUNCTION:  lte_rrc_mdt_get_log_meas_rpt

===========================================================================*/
/*!
  @brief
  Prebuild Mog meas reports with MDT reports

  @detail
  Prebuild Mog meas reports with MDT reports


  @return
  None

*/
/*=========================================================================*/
lte_errno_e lte_rrc_mdt_get_log_meas_rpt
(
  lte_rrc_osys_LogMeasReport_r10 *logMeasReport_r10_ptr,
  lte_rrc_mdt_rpt_type_e rpt_type
);

/*===========================================================================

  FUNCTION:  lte_rrc_get_mdt_is_prebuilt_UL_available

===========================================================================*/
/*!
  @brief
  Function to get avaialabiliyt of prebuilt UL msgs

  @detail
  Function to get avaialabiliyt of prebuilt UL msgs

  @return
  boolean

*/
/*=========================================================================*/
boolean lte_rrc_get_mdt_is_prebuilt_UL_available( void );

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_response_cnfi_handler

===========================================================================*/
/*!
  @brief
  Handle UE-Info response cnf

  @detail
  Handle UE-Info response cnf
  
  @return
  None

*/
/*=========================================================================*/
void lte_rrc_mdt_response_cnfi_handler( void );

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_get_prebuilt_UL

===========================================================================*/
/*!
  @brief
  Function to get prebuilt UL msg

  @detail
  Function to get prebuilt UL msg

  @return
  void

*/
/*=========================================================================*/

void *lte_rrc_mdt_get_prebuilt_UL( void );

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_read_from_efs

===========================================================================*/
/*!
  @brief
  Function to get MDT status

  @detail
  Function to get MDT status

  @return
  TRUE if either t330 or validity timer are running

*/
/*=========================================================================*/
void lte_rrc_mdt_read_from_efs( void );

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_handle_rpt

===========================================================================*/
/*!
  @brief
  Handle MDT rpt from ML1 and store it on heap

  @detail
  Handle MDT rpt from ML1 and store it on heap

  @return
  None

*/
/*=========================================================================*/
lte_errno_e lte_rrc_mdt_handle_rpt
(
  lte_rrc_mdt_s *i_ptr,
  lte_cphy_ueinfo_mdt_report_ind_s *rpt_ptr
);

/*===========================================================================

  FUNCTION:  lte_rrc_mdt_get_prebuilt_ul_msg_len

===========================================================================*/
/*!
  @brief
  Function to get prebuilt UL msg length to sent to PDCP

  @detail
  Function to get prebuilt UL msg length to sent to PDCP

  @return
  void

*/
/*=========================================================================*/

long lte_rrc_mdt_get_prebuilt_ul_msg_len
( 
  void 
);


#endif
