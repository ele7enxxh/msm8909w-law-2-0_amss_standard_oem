#ifndef WMSUTILS_H
#define WMSUTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsutils.h --

  Implementations of common internal WMS functions.


  -------------

    Copyright (c) 2010-2011 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsutils.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WMS_MEM_FREEIF(p)      if (p) { wms_mem_free((void*)p); (p) = 0; }

#define MSG_FATAL_0(str)                   MSG(   MSG_SSID_DFLT, MSG_LEGACY_FATAL, str )
#define MSG_ERROR_0(str)                   MSG(   MSG_SSID_DFLT, MSG_LEGACY_ERROR, str )
#define MSG_HIGH_0(str)                    MSG(   MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str )
#define MSG_MED_0(str)                     MSG(   MSG_SSID_DFLT, MSG_LEGACY_MED,   str )
#define MSG_LOW_0(str)                     MSG(   MSG_SSID_DFLT, MSG_LEGACY_LOW,   str )

#define MSG_FATAL_1(str, a)                MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a )
#define MSG_ERROR_1(str, a)                MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a )
#define MSG_HIGH_1(str, a)                 MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a )
#define MSG_MED_1(str, a)                  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a )
#define MSG_LOW_1(str, a)                  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a )

#define MSG_FATAL_2(str, a, b)             MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b )
#define MSG_ERROR_2(str, a, b)             MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b )
#define MSG_HIGH_2(str, a, b)              MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b )
#define MSG_MED_2(str, a, b)               MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b )
#define MSG_LOW_2(str, a, b)               MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b )

#define MSG_FATAL_3(str, a, b, c)          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b, c )
#define MSG_ERROR_3(str, a, b, c)          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b, c )
#define MSG_HIGH_3(str, a, b, c)           MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b, c )
#define MSG_MED_3(str, a, b, c)            MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b, c )
#define MSG_LOW_3(str, a, b, c)            MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b, c )

#define MSG_FATAL_4(str, a, b, c, d)       MSG_4( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b, c, d )
#define MSG_ERROR_4(str, a, b, c, d)       MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b, c, d )
#define MSG_HIGH_4(str, a, b, c, d)        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b, c, d )
#define MSG_MED_4(str, a, b, c, d)         MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b, c, d )
#define MSG_LOW_4(str, a, b, c, d)         MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b, c, d )

#define MSG_FATAL_5(str, a, b, c, d, e)    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b, c, d, e )
#define MSG_ERROR_5(str, a, b, c, d, e)    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b, c, d, e )
#define MSG_HIGH_5(str, a, b, c, d, e)     MSG_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b, c, d, e )
#define MSG_MED_5(str, a, b, c, d, e)      MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b, c, d, e )
#define MSG_LOW_5(str, a, b, c, d, e)      MSG_5( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b, c, d, e )

#define MSG_FATAL_6(str, a, b, c, d, e, f) MSG_6( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b, c, d, e, f )
#define MSG_ERROR_6(str, a, b, c, d, e, f) MSG_6( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b, c, d, e, f )
#define MSG_HIGH_6(str, a, b, c, d, e, f)  MSG_6( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b, c, d, e, f )
#define MSG_MED_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b, c, d, e, f )
#define MSG_LOW_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b, c, d, e, f )

#define MSG_FATAL_7(str, a, b, c, d, e, f, g) MSG_7( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b, c, d, e, f, g )
#define MSG_ERROR_7(str, a, b, c, d, e, f, g) MSG_7( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b, c, d, e, f, g )
#define MSG_HIGH_7(str, a, b, c, d, e, f, g)  MSG_7( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b, c, d, e, f, g )
#define MSG_MED_7(str, a, b, c, d, e, f, g)   MSG_7( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b, c, d, e, f, g )
#define MSG_LOW_7(str, aa, b, c, d, e, f, g)  MSG_7( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b, c, d, e, f, g )

#define MSG_FATAL_8(str, a, b, c, d, e, f, g, h) MSG_8( MSG_SSID_DFLT, MSG_LEGACY_FATAL, str, a, b, c, d, e, f, g, h )
#define MSG_ERROR_8(str, a, b, c, d, e, f, g, h) MSG_8( MSG_SSID_DFLT, MSG_LEGACY_ERROR, str, a, b, c, d, e, f, g, h )
#define MSG_HIGH_8(str, a, b, c, d, e, f, g, h)  MSG_8( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  str, a, b, c, d, e, f, g, h )
#define MSG_MED_8(str, a, b, c, d, e, f, g, h)   MSG_8( MSG_SSID_DFLT, MSG_LEGACY_MED,   str, a, b, c, d, e, f, g, h )
#define MSG_LOW_8(str, a, b, c, d, e, f, g, h)   MSG_8( MSG_SSID_DFLT, MSG_LEGACY_LOW,   str, a, b, c, d, e, f, g, h )



/*===========================================================================
                         DATA TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION wms_mem_malloc

DESCRIPTION
  Function allocates memory from heap.

DEPENDENCIES
  None

RETURN VALUE
  void pointer

SIDE EFFECTS
  None
===========================================================================*/
void * wms_mem_malloc(
  size_t                             size
);

/*===========================================================================
FUNCTION wms_mem_free

DESCRIPTION
  Function free memory back to heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_mem_free(
  void                              *mem_ptr
);

void wms_check_for_null_ptr();


#ifdef __cplusplus
}
#endif

#endif /* WMSUTILS_H */
