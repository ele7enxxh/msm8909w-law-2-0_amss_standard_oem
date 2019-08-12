

/*!
  @file
  ds_wcdma_bearer_context.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/inc/ds_wcdma_bearer_context.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/12   ss      Fixed issue of RAB_REESTAB_IND received before resume ind
                   resulting in error in data path setup.
01/02/12   msh     Coan: Feature cleanup
12/21/11   ss      Added support for watermark tuning.
10/20/11   ss      Dynamic back pressure for TCP throughput optimization. 
02/23/11   ttv     Added support for watermark tracing.
01/31/11   ss      Added support for reporting bearer tech type in WCDMA 
                   and GPRS.
12/01/10   ss      Support for SIM REFRESH VOTING for WCDMA and GPRS.

===========================================================================*/

#ifndef DSWCDMABEARERCONTEXT_H
#define DSWCDMABEARERCONTEXT_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_WCDMA_PS
#include "sys.h"
#include "ds_umts_bearer_context.h"
#include "wl2api.h"
#include "ps_comp_logging_helper.h"
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================
FUNCTION DS_BEARER_CONTEXT_WCDMA_INIT

DESCRIPTION
  This function is used to init the WCDMA part of the bearer context.
  It initializes the UMTS_function table specific to the WCDMA mode.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsWcdmaBearerCntxt_init( void );

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CONTEXT_REFRESH_VOTING_HDLR

DESCRIPTION   
  This function checks the state of all WCDMA PS calls and decides whether an
  SIM refresh will be allowed to proceed.

PARAMETERS   
  subs_id         - Subscription Id
  
DEPENDENCIES  
  None.

RETURN VALUE  
  TRUE  - if SIM Refresh can proceed from mode handler perspective.
  FALSE - if SIM Refresh cannot proceed from mode handler perspective.

SIDE EFFECTS  
  None.
===========================================================================*/
boolean dsWcdmaBearerCntxt_refresh_voting_hdlr
(
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_GET_HS_STATUS_FROM_CELL_SRV_IND

DESCRIPTION
  This function returns the hs_status from the cell service ind

PARAMETERS
  cm_cell_srv_ind_s_type - cell_srv_ind

DEPENDENCIES
  None.

RETURN VALUE
  hs_status

SIDE EFFECTS
  None.
===========================================================================*/
sys_hs_ind_e_type ds_wcdma_bearer_cntxt_get_hs_status_from_cell_srv_ind
(
  cm_cell_srv_ind_s_type  *cell_srv_ind
);

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_GET_PS_RAT_MASK

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed sys_mode,
  hs_ind, and hs_call_status.

PARAMETERS 
  sys_mode - CM sys mode 
  hs_status - the HS status

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_wcdma_bearer_cntxt_get_ps_rat_mask  
(
   sys_sys_mode_e_type sys_mode,
   sys_hs_ind_e_type   hs_status
);

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_GET_PS_RAT_MASK_FOR_HS_STATUS

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed HS status.

PARAMETERS
  hs_status       - the HS status
  subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_wcdma_bearer_cntxt_get_ps_rat_mask_for_hs_status
(
  sys_hs_ind_e_type       hs_status,
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
FUNCTION DS_TDS_W_CDMA_BEARER_CNTXT_EMPTY_WATERMARK()

DESCRIPTION
  This is the function that purges the data in the UL watermark when call end is initiated.
      
PARAMETERS
  bearer_context_p
  
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  
  
===========================================================================*/
void ds_tds_w_cdma_bearer_cntxt_empty_watermark
(
  ds_bearer_context_s  *bearer_context_p
);

/*===========================================================================
FUNCTION DS_TDS_W_CDMA_BEARER_CNTXT_PURGE_CNF_HDLR

DESCRIPTION
  This function is used to Handle the purge status 

PARAMETERS
  ds3gpp_purge_cnf_ev which consist of
  cnf_status    - true if if purge status is RLC_PURGE_REQ_SUCCESS(success )             
                   false otherwise
  bearer_context_p - bearer context pointer
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_tds_w_cdma_bearer_cntxt_purge_cnf_hdlr
(
  ds_bearer_context_purge_cnf_ev_type *ds3gpp_purge_cnf_ev
);


/*===========================================================================
FUNCTION DS_TDS_W_CDMA_BEARER_CNTXT_PURGE_STATUS_CB

DESCRIPTION
  This function is used to know the status of the purge command posted to RLC 

PARAMETERS
   lc_id            - logical channel ID
   purge_status     - status of purge request 
   bearer_context_p - bearer context pointer
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
 void ds_tds_w_cdma_bearer_cntxt_purge_status_cb
(
  rlc_lc_id_type               lc_id,
  rlc_ul_wm_purge_rsp_status    purge_status,
  void                          *context
); 

/*===========================================================================
FUNCTION DS_WCDMA_GET_COST_OF_MODEM_POWER

DESCRIPTION
  This returns the cost of modem power on the current subscription ID
  on WCDMA RAT
 
PARAMETERS
  *argval_ptr       - Pointer to fill in the cost of modem Power
  *ps_errno         - PS Error code in case of error
 
DEPENDENCIES
  None.

RETURN VALUE
  0   - If the modem power has been determined successfully from Lower
        layers
  -1  - otherwise 

SIDE EFFECTS
  None.

===========================================================================*/
int16 ds_wcdma_get_cost_of_modem_power
(
  void          *argval_ptr,
  int16         *ps_errno
);

/*===========================================================================
FUNCTION DS_TDS_GET_COST_OF_MODEM_POWER

DESCRIPTION
  This returns the cost of modem power on the current subscription ID
  on TDS RAT
 
PARAMETERS
  *argval_ptr       - Pointer to fill in the cost of modem Power
  *ps_errno         - PS Error code in case of error
 
DEPENDENCIES
  None.

RETURN VALUE
  0   - If the modem power has been determined successfully from Lower
        layers
  -1  - otherwise 

SIDE EFFECTS
  None.

===========================================================================*/
int16 ds_tds_get_cost_of_modem_power
(
  void                  *argval_ptr,
  int16                 *ps_errno
);
#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CONTEXT_REPORT_WM_STATS

DESCRIPTION
  This function stores the current watermark status.

PARAMETERS
  bearer_instance - bearer context instance 
  call_id         - call id
  wm_stat_ptr     - where the wm values are stored

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsWcdmaBearerCntxt_reportWmStats
(
  uint32          bearer_instance,
  uint32          call_id,
  fc_wm_stat_type *wm_stat_ptr
);
#endif /* FEATURE_QXDM_WM_TRACING */

#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION DS_TDS_BEARER_CNTXT_INIT

DESCRIPTION
  This function is used to init the TDSCDMA bearer context. This function
  registers the TDSCDMA specific functions with UMTS bearer context function
  table. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_tds_bearer_cntxt_init( void );
#endif /* FEATURE_TDSCDMA */

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)

/*===========================================================================
FUNCTION DS_TDS_W_CDMA_BEARER_CNTXT_COMP_LOG_FETCH_IFACE

DESCRIPTION
  This Functions returns the iface corresponding to link control id

PARAMETERS
  logging_info_ptr - the Log information 
  ip_version       - ip version v4 or v6

DEPENDENCIES
  None.

RETURN VALUE
  Iface.
  
SIDE EFFECTS
  None.

===========================================================================*/

ps_iface_type* ds_tds_w_cdma_bearer_cntxt_comp_log_fetch_iface
(
  ps_comp_logging_instance_config_s *logging_info_ptr,
  ip_version_enum_type               ip_version
);
#endif /*  (FEATURE_DATA_WCDMA_CS) || (FEATURE_TDSCDMA) */

#endif /* DSWCDMABEARERCONTEXT_H */
