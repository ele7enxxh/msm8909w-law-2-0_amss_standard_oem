#ifndef QMI_WMS_H
#define QMI_WMS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           qmi_wms.h --

  The  QMI Wireless Message Service header file.


  -------------

    Copyright (c) 2007,2009-2012 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/inc/qmi_wms.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
EXTERNALIZED FUNCTIONS

   qmi_wms_init
     Register the WMS service with QMUX

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds_qmi_defs.h"
#include "ds_qmi_if_ext.h"

/*===========================================================================
  FUNCTION qmi_wms_init()

  DESCRIPTION
    Register the WMS service with QMUX

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_wms_init(
  void
);

/*===========================================================================
FUNCTION qmi_wms_task_set_stop_sig_rcvd

DESCRIPTION
  SET the global which tell if TASK_STOP has come or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_task_set_stop_sig_rcvd(
  boolean val
);

extern boolean qmi_wms_task_stop_sig_rcvd; 
#define QMI_WMS_TASK_STOP_SIG_IS_RECEIVED (TRUE == qmi_wms_task_stop_sig_rcvd)

#endif /* QMI_WMS_H */
