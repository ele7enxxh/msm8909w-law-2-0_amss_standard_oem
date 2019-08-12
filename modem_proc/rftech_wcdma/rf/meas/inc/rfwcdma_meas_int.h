#ifndef RFWCDMA_MEAS_INT_H
#define RFWCDMA_MEAS_INT_H
/*!
  @file rfwcdma_meas.h

  @brief
  This file contains the WCDMA meas functions.

  @details

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/meas/inc/rfwcdma_meas_int.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/14   rmb     Pass device information to config/deconfig CCS. 
11/20/13   rmb     Bolt IRAT cleanup.
06/02/10   kguo     Initial Revision
===========================================================================*/

void rfmeas_ltow_setup( rfcom_device_enum_type device, uint16 channel, uint32 handle_id );


#endif /*RFWCDMA_MEAS_INT_H*/
