/******************************************************************************
  @file    ds_atp_http.h
  @brief   HTTP header file for ATP(Application Traffic Paring)

  DESCRIPTION
  HTTP header file for ATP(Application Traffic Paring)

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/atp/inc/ds_atp_http.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/13   jz      Created
09/09/14   ml      Added ds_http api in place of ims http api
===========================================================================*/
#ifndef DS_ATP_HTTP_H
#define DS_ATP_HTTP_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ATP

#if defined(FEATURE_DATA_ATP_DS_HTTP) || defined(FEATURE_DATA_ATP_IMS_HTTP)

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"

void    ds_atp_init_http(void);
boolean ds_atp_http_get_policy(void);
boolean ds_atp_http_get_host_from_url(const char *http_url_ptr, char *host_ptr, const uint32 host_max_len);

#ifdef FEATURE_DATA_ATP_IMS_HTTP
void ds_atp_http_process_cmd_q(void);
#endif /* FEATURE_DATA_ATP_IMS_HTTP */

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ATP_DS_HTTP || FEATURE_DATA_ATP_IMS_HTTP */
#endif /* FEATURE_DATA_ATP */

#endif /* DS_ATP_HTTP_H */
