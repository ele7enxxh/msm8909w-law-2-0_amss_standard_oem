#ifndef RFM_COMMON_SAR_H
#define RFM_COMMON_SAR_H

/*!
   @file
   rfm_common_sar.h

   @brief
      This file contains all declarations and definitions necessary to use the RF
   SAR APIs.

   @details

*/

/*===========================================================================
Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_common_sar.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/12/11    ap     Fix compiler warning 
08/11/11    ap     Implement Get SAR State API (without mode parameter) 
03/15/11   lcl     Deleted get sar state prototype
03/09/11   Saul    rfm_common_sar_get_state now takes mode
02/28/11   ka      Initial revision 
============================================================================*/ 


/*===========================================================================

                           TYPEDEFS

===========================================================================*/
typedef enum 
{
  /* First SAR state is used to indicate the default behavior */
  RF_SAR_STATE_DEFAULT,
  RF_SAR_STATE_1,
  RF_SAR_STATE_2,
  RF_SAR_STATE_3,
  RF_SAR_STATE_4,
  RF_SAR_STATE_5,
  RF_SAR_STATE_6,
  RF_SAR_STATE_7,
  RF_SAR_STATE_8,
  RF_SAR_STATE_MAX,
} rfm_common_sar_state_type;

typedef enum
{
  RF_SAR_DEFAULT,
  RF_SAR_EMERGENCY_CALL,
  RF_SAR_ACCESS,
} rfm_common_sar_override_type;

/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*--------------------------------------------------------------------------*/
extern void rfm_common_sar_set_state(rfm_common_sar_state_type sar_state);

/*--------------------------------------------------------------------------*/
extern void rfm_common_sar_set_overrride(rfm_common_sar_override_type override);

/*--------------------------------------------------------------------------*/
rfm_common_sar_state_type rfm_common_sar_get_state(void);

#endif /* RFM_COMMON_SAR_H */
