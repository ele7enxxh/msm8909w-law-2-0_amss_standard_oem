#ifndef TTL_MAP_H
#define TTL_MAP_H

/**===========================================================================
@FILE_DOC

                                 MCS _ SUPS

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for enabling
  code profiling via TTL.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

===========================================================================*/


/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/ttl_map.h#1 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/28/13    gr	   Initial revision.

===========================================================================*/

#ifdef FEATURE_TTL
   #define TTLMARKER(x) MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Generic Marker: " #x )
   #define TTLBOOTUP(x) MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Boot Marker: " #x )
#else
   #define TTLMARKER(x)
   #define TTLBOOTUP(x)
#endif

#endif /* TTL_MAP_H */