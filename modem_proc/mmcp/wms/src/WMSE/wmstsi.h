#ifndef WMSTSI_H
#define WMSTSI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmstsi.c -- Translation Services Internal Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the internal translation functions.

  -------------

    Copyright (c) 2001-2008,2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmstsi.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "mmcp_variation.h"
#include "wmsmsg.h"


#ifdef FEATURE_GWSMS

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#endif /* FEATURE_GSTK */

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*=========================================================================
FUNCTION
  wms_ts_convert_to_bcd_address

DESCRIPTION
  Convert address from API structure to a BCD structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_convert_to_bcd_address
(
  const wms_address_s_type          *addr_ptr,      // INPUT
  cm_called_party_bcd_no_T            *bcd_addr_ptr   // OUTPUT
);

/*=========================================================================
FUNCTION
  wms_ts_convert_from_bcd_address

DESCRIPTION
  Convert address to API structure from BCD structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_convert_from_bcd_address
(
  const cm_called_party_bcd_no_T      *bcd_addr_ptr,   // INPUT
  wms_address_s_type                *addr_ptr        // OUTPUT
);

#ifdef FEATURE_GSTK
/*=========================================================================
FUNCTION
  wms_ts_convert_to_gstk_address

DESCRIPTION
  Converts WMS Address to GSTK Address Type

DEPENDENCIES
  None

RETURN VALUE
  Conversion Status

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_convert_to_gstk_address
(
  const wms_address_s_type   * address_ptr,
  gstk_address_type          * gstk_address_ptr
);

/*=========================================================================
FUNCTION
  wms_ts_convert_from_gstk_address

DESCRIPTION
  Converts GSTK Address to WMS Address Type

DEPENDENCIES
  None

RETURN VALUE
  Conversion Status

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_convert_from_gstk_address
(
  const gstk_address_type    * gstk_address_ptr,
  wms_address_s_type         * address_ptr
);

#endif /* FEATURE_GSTK */

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS

/*=========================================================================
FUNCTION
  wms_ts_cdma_pack_ruim_sms

DESCRIPTION
  This function packs the RUIM SMS Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_pack_ruim_sms
(
  const wms_msg_ruim_sms_s_type  * ruim_sms,
  uint8                    * result
);

/*=========================================================================
FUNCTION
  wms_ts_cdma_unpack_ruim_sms

DESCRIPTION
  This function unpacks the RUIM SMS Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_unpack_ruim_sms
(
  const uint8              * data,
  wms_msg_ruim_sms_s_type  * result
);

#endif /* FEATURE_CDSMS */

#endif /* WMSTSI_H */


