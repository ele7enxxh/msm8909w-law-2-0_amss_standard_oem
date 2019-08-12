/******************************************************************************
  @file    ds_atp_xml.h
  @brief   XML parser header file

  DESCRIPTION
  XML parser header file

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/atp/inc/ds_atp_xml.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/13   jz      Created
===========================================================================*/
#ifndef DS_ATP_XML_H
#define DS_ATP_XML_H

#include "datamodem_variation.h"

#ifdef FEATURE_DATA_ATP

#include "ds_atp_internal.h"


#ifdef __cplusplus
extern "C"
{
#endif

boolean ds_atp_xml_parsing(const char* xml_string_ptr, ds_atp_policy_info_internal_type* policy_ptr);

#ifdef __cplusplus
}
#endif

#endif  /*FEATURE_DATA_ATP*/

#endif
