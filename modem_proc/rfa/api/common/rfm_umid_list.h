#ifndef RFM_UMID_LIST_H
#define RFM_UMID_LIST_H
/*!
   @file
   rfm_umid_list.h

   @brief
   RF Driver's interface file with MSGR UMIDs.

   @details

*/

/*===========================================================================
Copyright (c) 2002 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_umid_list.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/05/14   px     Added new structures for rffe scan device dump feature
05/16/14   sb     port only interface change for DPM 2.0 compilation 
10/02/13   sb     Initial version
10/03/13   sb     Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                  path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_msgr.h"
#include "rfcom.h"
#include "msgr_types.h"
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define RFA_RF_COMMON_MSG_ID_BYPASS_FILTERED_PATH         0x01
#define RFA_RF_COMMON_MSG_ID_RFFE_SCAN_EFS_DUMP           0x02

/*---------------------------------------------------------------------------*/
/*
  @brief
  This enum denotes the different outcomes of a MSGR request or response
 
*/

typedef enum
{
  RFM_DISPATCH_SUCCESSFUL=1,
  RFM_DISPATCH_UNSUCCESSFUL=2,
  RFM_DISPATCH_NULL_PTR=3,
  RFM_DISPATCH_INVALID_PARAM =4,
  RFM_DISPATCH_MAX
}rfm_dispatch_outcome_type;


/*---------------------------------------------------------------------------*/
/*
  @brief
  This is the structure of the MSGR request that an external component sends RF.
 
  @details
  The structure consiste of a header, use_filtered_path flag and another flag to specify whether response to the MSGR request is needed or not.
 
  @params

 
  @retval

*/
typedef struct
{
  msgr_hdr_struct_type          hdr;
  boolean                       rsp_required;  /* Set this to TRUE if response is expected*/
  boolean                       use_filtered_path_flag; /* This flag is for GSM Rx filter path i.e GSM as Victim */

  boolean                       use_gsm_aggr_filter_path_flag; /* This flag is for GSM Tx filter path i.e GSM as aggressor */
  /*Only for internal use*/
  boolean                       internal_cmd_ind; /*True indicates cmd from internal to RF, FALSE indicates external to RF*/
  /*Usage Example: In SGLTE scenario, This flag needs to be set TRUE when GSM DCS coexists with B39 LTE Or B34/B39 TDSCDMA in connected mode. 
    In all other combinations this flag should be set to FALSE*/

//rfm_bands_bitmask              bitmask;
} rfm_rf_cmn_bypass_filtered_path_req_s;

/*---------------------------------------------------------------------------*/
/*
  @brief
  This is the structure of the dummy payload used by rfm_rf_cmn_rffe_scan_efs_dump_req_s
 
  @details
  The structure contains a dummy value but can be expanded.

  @params

 
  @retval
  
*/
typedef struct
{
  boolean dummy_payload;
}rffe_scan_efs_dump_payload_type;

/*---------------------------------------------------------------------------*/
/*
  @brief
  This is the structure of the RFFE Scan EFS Dump that RF dispatches to apps task
 
  @details
  The structure consists of a header and a dummy payload that could be used in the future
 
  @params
  
 
  @retval

*/
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rffe_scan_efs_dump_payload_type payload;            

} rfm_rf_cmn_rffe_scan_efs_dump_req_s;


/*---------------------------------------------------------------------------*/

/*
  @brief
  This is the structure of the MSGR response that RF can send to the external caller.
 
  @details
  The structure consiste of a header and the outcome of the request sent to RF from external entity.
 
  @params

 
  @retval

*/
typedef struct
{
  msgr_hdr_struct_type             hdr;
  rfm_dispatch_outcome_type        cmd_result;
} rfm_rf_cmn_bypass_filtered_path_rsp_s;


/*---------------------------------------------------------------------------*/
/*
  @brief
  This is the union of all MSGR request structure types that RF can receive from external component.
 
  @details
 
  @params

 
  @retval

*/

typedef union
{
  rfm_rf_cmn_bypass_filtered_path_req_s    rfa_rf_cmn_bypass_filtered_path;

} rfm_msg_u;


/*---------------------------------------------------------------------------*/
/*
  @brief
  This is the definition of the MSGR cmd ID (both request and response).
 
  @details
 
  @params

 
  @retval

*/

/*MSGR_RFA_RF_COMMON  defined in rfa_msgr.h*/
enum
{
  MSGR_DEFINE_UMID(RFA, RF_COMMON, REQ, BYPASS_FILTERED_PATH, RFA_RF_COMMON_MSG_ID_BYPASS_FILTERED_PATH, rfm_rf_cmn_bypass_filtered_path_req_s),   
  MSGR_DEFINE_UMID(RFA, RF_COMMON, REQ, RFFE_SCAN_EFS_DUMP, RFA_RF_COMMON_MSG_ID_RFFE_SCAN_EFS_DUMP, rfm_rf_cmn_rffe_scan_efs_dump_req_s),   
};



enum
{

 MSGR_DEFINE_UMID(RFA, RF_COMMON, RSP, BYPASS_FILTERED_PATH, RFA_RF_COMMON_MSG_ID_BYPASS_FILTERED_PATH, rfm_rf_cmn_bypass_filtered_path_rsp_s),   
};

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* RFM_UMID_LIST_H */
