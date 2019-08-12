/******************************************************************************
  @file    ds_profile_3gpp_qmi.h
  @brief   

  DESCRIPTION
  Tech specific 3GPP Profile Management through QMI, header file

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/api/ds_profile_3gpp_qmi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/11   gs      First version of the header file.
===========================================================================*/
#ifndef DS_PROFILE_3GPP_QMI_H
#define DS_PROFILE_3GPP_QMI_H

#include "comdef.h"

#ifdef FEATURE_DS_PROFILE_ACCESS_QMI

#include "ds_profile_tech_common.h"

/*===========================================================================
FUNCTION DS_PROFILE_3GPP_QMI_INIT

DESCRIPTION
  This function is called on the library init. It initializes the function
  pointers to valid functions for 3gpp
  Also initializes the QMI client.
PARAMETERS
  fntbl : pointer to table of function pointers

DEPENDENCIES

RETURN VALUE
  returns the mask for 3gpp. (Used later as valid mask which is ORed value
  returned from all techs)
SIDE EFFECTS

===========================================================================*/
uint8 ds_profile_3gpp_qmi_init(tech_fntbl_type *fntbl);

#endif /* FEATURE_DS_PROFILE_ACCESS_QMI */

#endif /* DS_PROFILE_3GPP_QMI_H */
