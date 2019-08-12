#ifndef CMCNAS_MN_H
#define CMCNAS_MN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           CMCNas_MN.h --

  Interfaces between WMS / WMSC and NAS MN Layer.


  -------------

    Copyright (c) 2008,2010-2012 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/CMCNas/inc/CMCNas_MN.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#ifdef FEATURE_GWSMS

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "CMCNas.h"

/*==============================================================================
      Function to Create Instance of CMCNas for MN and Return Object
      NOTE: Use NULL for CMCNas_Cb is not interested
==============================================================================*/
boolean CMCNas_MN_New(
  CMCNas_Cb                          pfnSM,
  CMCNas_Cb                          pfnLink,
  CMCNas_Cb                          pfnCB,
  CMCNas                           **ppOut
);

#endif /* FEATURE_GWSMS */

#endif /* CMCNAS_MN_H */
