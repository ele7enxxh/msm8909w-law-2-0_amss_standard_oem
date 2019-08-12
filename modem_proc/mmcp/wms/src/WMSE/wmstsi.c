/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmstsi.c -- Translation Services Definitions

  The WMS module which implements the User API for SMS. This source file
  implements the translation functions for the client applications to call
  and for internal use.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmstsi.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmstsi.h"
#include "wmsts.h"
#include "msg.h"
#include "stringl.h"
#include "wmsutils.h"


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/
#ifdef FEATURE_GWSMS
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
)
{
  /* 1 byte for number type/plan and 10 bytes for number of digits */
  uint8   buf[WMS_GW_ADDRESS_MAX + 1];

  if( addr_ptr == NULL || bcd_addr_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return;
  }

  bcd_addr_ptr->present = FALSE;

  /* buf is the result which has number of digits and the remaining bytes
     bcd_addr_ptr->length does not include the byte for number of digits */
  bcd_addr_ptr->length = wms_ts_encode_address( addr_ptr, buf );
  bcd_addr_ptr->length -= 1;
  bcd_addr_ptr->length = MIN(sizeof(bcd_addr_ptr->data), bcd_addr_ptr->length);
  (void)memscpy( bcd_addr_ptr->data,
                 MIN(WMS_GW_ADDRESS_MAX, bcd_addr_ptr->length),
                 buf + 1,
                 MIN(WMS_GW_ADDRESS_MAX, bcd_addr_ptr->length) );

  bcd_addr_ptr->present = TRUE;

  return;

} /* wms_ts_convert_to_bcd_address() */

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
  const cm_called_party_bcd_no_T    *bcd_addr_ptr,   // INPUT
  wms_address_s_type                *addr_ptr        // OUTPUT
)
{
  uint8   buf[WMS_GW_ADDRESS_MAX + 1];
  uint8 pos;
  byte  data;

  if( addr_ptr == NULL || bcd_addr_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return;
  }

  if( 0 == bcd_addr_ptr->length || FALSE == bcd_addr_ptr->present )
  {
    addr_ptr->number_of_digits = 0;
  }
  else if ( bcd_addr_ptr->length <= CM_CALLED_PARTY_BCD_NO_LENGTH )
  {
    /*Calculate number of digits */
    buf[0] =(uint8)( 2 * (bcd_addr_ptr->length-1));
    data = bcd_addr_ptr->data[bcd_addr_ptr->length -1];
    if( (data & 0xF0) == 0xF0)
    {
      buf[0] -= 1;
    }

    /*copy the bytes after the first byte that has the number of digits to buf*/
    (void)memscpy( buf + 1,
                   MIN(bcd_addr_ptr->length, WMS_GW_ADDRESS_MAX),
                   bcd_addr_ptr->data,
                   MIN(bcd_addr_ptr->length, WMS_GW_ADDRESS_MAX) );

    pos =  wms_ts_decode_address( buf, addr_ptr );

    if (pos != (bcd_addr_ptr->length+1)  )
    {
      MSG_ERROR_0("Invalid BCD address Length");
    }
  }

  /* done */
  return;

} /* wms_ts_convert_from_bcd_address() */

#ifdef FEATURE_GSTK

/*=========================================================================
FUNCTION
  wms_ts_convert_to_gstk_address

DESCRIPTION
  Converts WMS Address to GSTK Address

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_convert_to_gstk_address
(
  const wms_address_s_type   * address_ptr,
  gstk_address_type    * gstk_address_ptr
)
{

  if( address_ptr == NULL || gstk_address_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return FALSE;
  }

  gstk_address_ptr->TON = (gstk_ton_enum_type) address_ptr->number_type;
  gstk_address_ptr->NPI = (gstk_npi_enum_type) address_ptr->number_plan;

  gstk_address_ptr->length = address_ptr->number_of_digits;

  (void)memscpy( gstk_address_ptr->address,
                 address_ptr->number_of_digits,
                 address_ptr->digits,
                 address_ptr->number_of_digits );

  return TRUE;

} /* wms_ts_convert_to_gstk_address() */

/*=========================================================================
FUNCTION
  wms_ts_convert_from_gstk_address

DESCRIPTION
  Converts WMS Address from GSTK Address

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Success;
  FALSE: Failure.

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_convert_from_gstk_address
(
  const gstk_address_type    * gstk_address_ptr,
  wms_address_s_type   * address_ptr
)
{

  if( address_ptr == NULL || gstk_address_ptr == NULL )
  {
    wms_check_for_null_ptr();
    return FALSE;
  }

  address_ptr->digit_mode  = WMS_DIGIT_MODE_4_BIT;
  address_ptr->number_type = (wms_number_type_e_type) gstk_address_ptr->TON;
  address_ptr->number_plan = (wms_number_plan_e_type) gstk_address_ptr->NPI;

  if (gstk_address_ptr->length > WMS_ADDRESS_MAX)
  {
     MSG_ERROR_0("Partial Address Copy in wms_ts_convert_from_gstk_address");
  }

  address_ptr->number_of_digits = MIN(gstk_address_ptr->length, WMS_ADDRESS_MAX );
  (void)memscpy( address_ptr->digits,
                 address_ptr->number_of_digits,
                 gstk_address_ptr->address,
                 address_ptr->number_of_digits );

  return TRUE;

} /* wms_ts_convert_from_gstk_address() */

#endif /* FEATURE_GSTK */

#endif/*FEATURE_GWSMS*/

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
  const wms_msg_ruim_sms_s_type     * ruim_sms, /* IN */
  uint8                             * result /* OUT */
)
{
  /*---- checking ---- */
  if ( ruim_sms == NULL || result == NULL)
  {
    MSG_ERROR_0("wms_ts_cdma_pack_ruim_sms null ptr!");
    return FALSE;
  }

  result[0] = ruim_sms->status;
  result[1] = (uint8) ruim_sms->ota.data_len;

  if( ruim_sms->ota.data_len > sizeof(ruim_sms->ota.data) )
  {
    return FALSE;
  }
  else
  {
    (void)memscpy( result + 2,
                   ruim_sms->ota.data_len,
                   (uint8 *)ruim_sms->ota.data,
                   ruim_sms->ota.data_len );
    return TRUE;
  }

} /* wms_ts_cdma_pack_ruim_sms() */

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
  const uint8              * data,  /* IN */
  wms_msg_ruim_sms_s_type  * result /* OUT */
)
{

  /*---- checking ---- */
  if ( data == NULL || result == NULL)
  {
    MSG_ERROR_0("wms_ts_cdma_unpack_ruim_sms null ptr!");
    return FALSE;
  }

  result->status = data[0];
  result->ota.data_len = data[1];
  result->ota.format   = WMS_FORMAT_CDMA;

  if( result->ota.data_len > sizeof(result->ota.data) )
  {
    return FALSE;
  }
  else
  {
    (void)memscpy( (uint8 *)result->ota.data,
                   result->ota.data_len,
                   data + 2,
                   result->ota.data_len );
    return TRUE;
  }

} /* wms_ts_cdma_unpack_ruim_sms() */

#endif /* FEATURE_CDSMS */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
