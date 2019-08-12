#ifndef TM_UMTS_CP_SUBMODULE_IFACE_H
#define TM_UMTS_CP_SUBMODULE_IFACE_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          UMTS_CP / Submodules (SS WDMA GSM) Interface Header File

GENERAL DESCRIPTION

  Prototypes of functions / variables defined in tm_umts_cp.c and used by 
  SS/WCDMA/GSM.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/prot/tm_umts_cp_submodule_iface.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  05/03/07   LT      Initial version.

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
                            Data Section
****************************************************************************/



/*-----------------------------------------------------------------------

                           Extern Functions
    
-----------------------------------------------------------------------*/

extern boolean tm_umts_cp_request_molr
( prtl_start_sess_req_param_s_type *start_sess_req_param, 
  molr_type_e_type mo_lr_type,
  sm_InternalAssistDataStatusStructType *p_assist_data_status 
);

extern boolean tm_umts_cp_request_lcs_loc_notif_return_result
(const pdsm_lcs_notification_resp_s_type *p_user_resp);

extern void tm_umts_cp_submodule_end_session (void);

#ifdef __cplusplus
}
#endif


#endif /* TM_UMTS_CP_SUBMODULE_IFACE_H */
