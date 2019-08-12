#ifndef WMSLL_H
#define WMSLL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsll.h -- Lower-layer definitions

  This header file contains all the definitions necessary for
  Lower Layers to interface with the WMS module.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/inc_deprecated/wmsll.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#ifdef FEATURE_GWSMS /* SMS API */
#ifdef FEATURE_GWSMS_BROADCAST
/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "cbwmsif.h"

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#include "wms_modem.h"
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_GWSMS */

#endif /* WMSLL_H */

