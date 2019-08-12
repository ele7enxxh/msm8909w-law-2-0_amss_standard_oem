#ifndef _DS_QMI_WDS_LTE_H
#define _DS_QMI_WDS_LTE_H
/*===========================================================================

                         D S _ Q M I _ W D S _ L T E . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface header file
 that contains LTE specific functions.

EXTERNALIZED FUNCTIONS

  QMI_WDS_SET_LTE_ATTACH_PDN_LIST()
     Set the lte attach pdn profile list by calling into the PS SYS layer
  QMI_WDS_GET_LTE_ATTACH_PDN_LIST()
     Retrieves lte attach pdn list by calling into the PS SYS layer
  QMI_WDS_GET_LTE_MAX_ATTACH_PDN_LIST_NUM()
     Retrieves max lte attach pdn list size
  QMI_WDS_SET_LTE_DATA_RETRY()
     Modifies the LTE Data Retry setting.
  QMI_WDS_GET_LTE_DATA_RETRY()
     Retrieves the LTE Data Retry setting.
  QMI_WDS_SET_LTE_ATTACH_TYPE()
     Modifies the LTE Attach Type setting.
  QMI_WDS_GET_LTE_ATTACH_TYPE()
     Retrieves the LTE Attach Type setting.
  QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES()
     Updates the LTE attach PDN list profile params.


Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_lte.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/13/13    sj     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined (FEATURE_DATA_LTE)
#include "dsm.h"
#include "ps_sys.h"

#define WDS_LTE_ATTACH_PDN_PROFILE_LIST_MAX  PS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX
/* LTE Attach PDN List structure definition */
typedef PACKED struct PACKED_POST
{
  uint8  attach_pdn_profile_list_num; /* Num of attach PDN in the array */
  uint16 attach_pdn_profile_list[WDS_LTE_ATTACH_PDN_PROFILE_LIST_MAX];
} qmi_wds_lte_attach_pdn_list_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION QMI_WDS_SET_LTE_ATTACH_PDN_LIST()

  DESCRIPTION
    Set the lte attach pdn profile list by calling into the PS SYS layer

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_set_lte_attach_pdn_list
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
);

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_ATTACH_PDN_LIST()

  DESCRIPTION
    Retrieves lte attach pdn list by calling into the PS SYS layer

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_get_lte_attach_pdn_list
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
);

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_MAX_ATTACH_PDN_LIST_NUM()

  DESCRIPTION
    Retrieves lte attach pdn list

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_get_lte_max_attach_pdn_list_num
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
);

/*===========================================================================
  FUNCTION QMI_WDS_SET_LTE_DATA_RETRY()

  DESCRIPTION
    Modifies the LTE Data Retry setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

dsm_item_type*  qmi_wds_set_lte_data_retry(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**);

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_DATA_RETRY()

  DESCRIPTION
    Retrieves the LTE Data Retry setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_get_lte_data_retry(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**);

/*===========================================================================
  FUNCTION QMI_WDS_SET_LTE_ATTACH_TYPE()

  DESCRIPTION
    Modifies the LTE Attach Type setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_set_lte_attach_type(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**);

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_ATTACH_TYPE()

  DESCRIPTION
    Retrieves the LTE Attach Type setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_get_lte_attach_type(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**);

/*===========================================================================
  FUNCTION QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES()

  DESCRIPTION
    Updates the LTE attach PDN list profile params

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_update_lte_attach_pdn_list_profiles(
                                                        void*,
                                                        void*,
                                                        void*,
                                                        dsm_item_type**);
#endif /* defined(FEATURE_DATA_LTE)*/
#endif /* _DS_QMI_WDS_LTE_H */
