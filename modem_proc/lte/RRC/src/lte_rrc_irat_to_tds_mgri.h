/*!
  @file
  lte_rrc_irat_to_tds_mgri.h

  @brief

  @detail

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_to_tds_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/2011 np      Initial revision
===========================================================================*/

#ifndef LTE_RRC_IRAT_TO_TDS_MGRI_H
#define LTE_RRC_IRAT_TO_TDS_MGRI_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_utils.h"
#include "tds_rrc_msg.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
#define MAX_TDS_ACQ_DB_ENTRIES 100
#define MAX_TDS_ACQ_DB_FREQ_PER_LTE_CELL_ID 6
#define MAX_EFS_TDS_ACQ_DB_FREQ_PER_LTE_CELL_ID 3
#define LTE_RRC_TDS_ACQ_DB_DEFAULT_SCALING_THRESHOLD 255
#define LTE_RRC_TDS_ACQ_DB_EFS_FILENAME "/nv/reg_files/modem/lte/rrc/irat_tds_acq_db"
#define LTE_RRC_TDS_ACQ_DB_EFS_SCALING_TRESHOLD "/nv/reg_files/modem/lte/rrc/tds_acq_db_scaling_treshold"

/*! EFS file permissions */
#define LTE_RRC_TDS_ACQ_DB_EFS_FILE_PERMISSIONS 0777

typedef enum
{
  /*! Default value */
  LTE_RRC_IRAT_TO_TDS_MODE_INITIAL            = 0,

  /*! IRAT Resel to tds started */
  LTE_RRC_IRAT_TO_TDS_MODE_RESEL_STARTED      = 1,

  /*! IRAT Resel to tds failed */
  LTE_RRC_IRAT_TO_TDS_MODE_RESEL_FAILED       = 2,

  /*! IRAT Resel to tds aborted */
  LTE_RRC_IRAT_TO_TDS_MODE_RESEL_ABORTED      = 3,

  /*! IRAT Redir to tds Started */
  LTE_RRC_IRAT_TO_TDS_MODE_REDIR_STARTED      = 4,

  /*! IRAT Redir to tds Failed */
  LTE_RRC_IRAT_TO_TDS_MODE_REDIR_FAILED       = 5,

  /*! IRAT Redir to tds Aborted */
  LTE_RRC_IRAT_TO_TDS_MODE_REDIR_ABORTED      = 6,

  /*! IRAT PLMN Srch W started */
  LTE_RRC_IRAT_TO_TDS_MODE_PLMN_SRCH_STARTED  = 7,

  /*! IRAT PLMN Srch W Aborted */
  LTE_RRC_IRAT_TO_TDS_MODE_PLMN_SRCH_ABORTED  = 8,

  /*! IRAT PLMN Srch W started */
  LTE_RRC_IRAT_TO_TDS_MODE_PLMN_SRCH_INACTIVE_STARTED  = 9,

  /*! IRAT PLMN Srch W Aborted */
  LTE_RRC_IRAT_TO_TDS_MODE_PLMN_SRCH_INACTIVE_ABORTED  = 10,

  /*! IRAT HO to tds started */
  LTE_RRC_IRAT_TO_TDS_MODE_HO_STARTED                  = 11,

  /*! IRAT HO to tds failed */
  LTE_RRC_IRAT_TO_TDS_MODE_HO_FAILED                   = 12,

  /*! IRAT HO to tds aborted */
  LTE_RRC_IRAT_TO_TDS_MODE_HO_ABORTED                  = 13,

  /*! TDS CGI reading started */
  LTE_RRC_IRAT_TO_TDS_MODE_CGI_STARTED  = 14,

  /*! TDS CGI reading aborted */
  LTE_RRC_IRAT_TO_TDS_MODE_CGI_ABORTED  = 15,

} lte_rrc_irat_to_tds_mode_e;

typedef union
{
  /* !< TDSCDMA Cell Info for reselections */
  lte_cphy_tdscdma_cell_info_s        tds_cell_info;

  /* !< TDSCDMA IRAT Redirection related info */
  lte_rrc_redir_info_s                redir_info; 

  /*!< TDS Get CGI Req*/
  lte_rrc_irat_from_lte_to_tds_get_cgi_reqi_s get_cgi_req;

}lte_rrc_to_tds_req_proc_u;

typedef union
{
  /* !< Resel Failed Data from TDSCDMA */
  tds_rrc_lte_resel_failed_rsp_type resel_failed_info;

  /*!< TDS Get CGI Rsp*/
  lte_rrc_irat_from_lte_to_tds_get_cgi_cnfi_s get_cgi_rsp;

}lte_rrc_to_tds_rsp_proc_u;

typedef struct
{
  /*! Count for each successful L2T resel*/
  uint8                               acq_count;
  /*! TDS freq that LTE reselected to*/
  lte_uarfcn_t                        uarfcn;

}lte_rrc_irat_to_tds_prev_acq_freq_s;

typedef struct
{
  /*! TDS DB tracks N TDS freq for successful L2T resel*/
  lte_rrc_irat_to_tds_prev_acq_freq_s           freq[MAX_TDS_ACQ_DB_FREQ_PER_LTE_CELL_ID];
  /*! Unique ID for LTE cell for every L2T resel */
  lte_rrc_global_cell_id_t                      cell_id;

}lte_rrc_irat_to_tds_prev_acq_s;

typedef struct
{
  /*! TDS DB tracks N TDS freq for successful L2T resel*/
  lte_rrc_irat_to_tds_prev_acq_freq_s           freq[MAX_EFS_TDS_ACQ_DB_FREQ_PER_LTE_CELL_ID];
  /*! Unique ID for LTE cell for every L2T resel */
  lte_rrc_global_cell_id_t                      cell_id;

}lte_rrc_irat_efs_tds_acq_s;

/*! @brief Structure for storing dynamic IRAT data
*/
typedef struct
{
  /*!< Current Inter-RAT procedure details */
  lte_rrc_irat_proc_e                 proc;

  /* !< Abort cause if an Abort request is received */
  lte_rrc_irat_abort_cause_type_e     abort_cause;

  /*!< Abort Proc type */
  lte_rrc_proc_e abort_proc;

  lte_rrc_to_tds_req_proc_u           proc_req;

  lte_rrc_to_tds_rsp_proc_u           proc_rsp;
  
  /*!< TDS PLMN SRCH Req*/
  lte_irat_plmn_srch_req_s            plmn_srch_req; 
  
  /*!< TDS PLMN Srch Rsp*/
  lte_irat_plmn_srch_rsp_s            plmn_srch_rsp;

  uint8                               dl_nas_count;
  dsm_item_type                       *dsm_ptr;

  /* !< Keeps track of IRAT mode */
  lte_rrc_irat_to_tds_mode_e            mode;

  /*!< Index of the last replaced LTE Cell ID in TDS Acq DB */
  uint8                               last_irat_db_replaced_index;

   /*!< Boolean to keep track if SIB-6 was sent */
  boolean                             sib6_sent_earlier;

  /*!< LTE cell for which sib6 info was sent last */
  lte_rrc_cell_info_s                 last_sib6_cell_info;

} lte_rrc_irat_to_tds_mgr_dynamic_data_s;

/*! @brief Structure for storing static IRAT data
*/
typedef struct
{
  /*! TDS Acq DB for successful L2T reselection counts */
  lte_rrc_irat_to_tds_prev_acq_s *tds_acq_db;

  /*! Scaling treshold for Tds Acq DB*/
  uint8                               tds_acq_db_scaling_threshold;

  /*! Has Acq DB been modified since last EFS Read*/
  boolean                             tds_acq_db_modified;
  
  /*!< Global cell id of camped LTE Cell when received To TDS Resel Req*/
  lte_rrc_global_cell_id_t            last_camped_global_cell_id_before_irat;

}lte_rrc_irat_to_tds_mgr_static_data_s;

/*! @brief Structure for storing IRAT data
 */
typedef struct
{ 
  /*! Dynamic data pointer */
  lte_rrc_irat_to_tds_mgr_dynamic_data_s    *dd_ptr;

  /*! Static part of private data */
  lte_rrc_irat_to_tds_mgr_static_data_s  *sd_ptr;
  
} lte_rrc_irat_to_tds_mgr_s;


#endif /* LTE_RRC_IRAT_TO_TDS_MGRI_H */


