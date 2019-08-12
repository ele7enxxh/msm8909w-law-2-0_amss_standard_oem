#ifndef _DS_QMI_WDS_UMTS_H
#define _DS_QMI_WDS_UMTS_H
/*===========================================================================

                         D S _ Q M I _ W D S _ U M T S . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface header file.

EXTERNALIZED FUNCTIONS

  QMI_WDS_UMTS_GET_RUNTIME_SETTINGS()
    Get the runtime settings for a given UMTS profile.


Copyright (c) 2005-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_umts.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/30/05    ks     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined (FEATURE_LTE) || defined(FEATURE_TDSCDMA)
#include "dsm.h"

#include "ds_qmi_defs.h"
#include "ps_iface_ioctl.h"
#include "wireless_data_service_v01.h"
#include "qmi_si.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION QMI_WDS_UMTS_GET_RUNTIME_SETTINGS()

  DESCRIPTION
    Get the runtime settings for a given UMTS profile.

  PARAMETERS
    req_settings     : mask of requesting settings
    resp_msg         : output buffer to fill the response
    iface_ptr        : the ps_iface pointer
    bearer_tech_type : the bearer technology type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_wds_umts_get_runtime_settings
(
  uint32                           req_settings,
  wds_get_runtime_settings_resp_msg_v01* response,
  ps_iface_type *                  iface_ptr,
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_sys_system_status_info_type   bearer_tech_ex_type
#else
  ps_iface_bearer_technology_type  bearer_tech_type
#endif /* FEATURE_DATA_WLAN_MAPCON */
);


#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined (FEATURE_LTE) || defined(FEATURE_TDSCDMA)*/
#endif /* _DS_QMI_WDS_UMTS_H */
