#ifndef ATS_V_H
#define ATS_V_H
/*=============================================================================

       A T S Private Header File

GENERAL DESCRIPTION
  Common internal defines, xx_fmtuctures and functions

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/22/13    ab     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

/* In case Diag F3s are not present, define this feature there */
#ifndef ATS_DIAG_F3_NOT_PRESENT
#include "msg.h"

#define ATS_MSG(xx_fmt)                                      \
  MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt)

#define ATS_MSG_1(xx_fmt, a)                                   \
  MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a)

#define ATS_MSG_2(xx_fmt, a, b)                                \
  MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a, b)

#define ATS_MSG_3(xx_fmt, a, b, c)                             \
  MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a, b, c)

#define ATS_MSG_4(xx_fmt, a, b, c, d)                          \
  MSG_4(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d)

#define ATS_MSG_5(xx_fmt, a, b, c, d, e)                       \
  MSG_5(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e)
  
#define ATS_MSG_6(xx_fmt, a, b, c, d, e, f)                       \
  MSG_6(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e, f)
  
#define ATS_MSG_7(xx_fmt, a, b, c, d, e, f, g)                       \
  MSG_7(MSG_SSID_ATS, MSG_LEGACY_HIGH, xx_fmt, a, b, c, d, e, f, g)

#define ATS_ERR(xx_fmt)                                      \
  MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt)

#define ATS_ERR_1(xx_fmt, a)                                   \
  MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a)

#define ATS_ERR_2(xx_fmt, a, b)                                \
  MSG_2(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a, b)

#define ATS_ERR_3(xx_fmt, a, b, c)                             \
  MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a, b, c)

#define ATS_ERR_4(xx_fmt, a, b, c, d)                          \
  MSG_4(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a, b, c, d)

#define ATS_ERR_5(xx_fmt, a, b, c, d, e)                       \
  MSG_5(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a, b, c, d, e)

#define ATS_ERR_6(xx_fmt, a, b, c, d, e, f)                       \
  MSG_6(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a, b, c, d, e, f)  

#define ATS_ERR_7(xx_fmt, a, b, c, d, e, f, g)                       \
  MSG_7(MSG_SSID_ATS, MSG_LEGACY_ERROR, xx_fmt, a, b, c, d, e, f, g)  
  
#define ATS_MSG_LOW( xx_fmt, a, b, c )    \
     MSG_3  ( MSG_SSID_ATS, MSG_LEGACY_LOW , xx_fmt, a, b, c )
#define ATS_MSG_MED( xx_fmt, a, b, c )    \
     MSG_3  ( MSG_SSID_ATS, MSG_LEGACY_MED , xx_fmt, a, b, c )
#define ATS_MSG_HIGH( xx_fmt, a, b, c )   \
     MSG_3  ( MSG_SSID_ATS, MSG_LEGACY_HIGH , xx_fmt, a, b, c )
#define ATS_MSG_ERROR( xx_fmt, a, b, c )  \
     MSG_3  ( MSG_SSID_ATS, MSG_LEGACY_ERROR , xx_fmt, a, b, c )
#define ATS_MSG_SPRINTF_1( xx_fmt, a )  \
        MSG_SPRINTF_1  ( MSG_SSID_ATS, MSG_LEGACY_ERROR , xx_fmt, a)  
#define ATS_MSG_SPRINTF_2( xx_fmt, a, b )  \
     MSG_SPRINTF_2  ( MSG_SSID_ATS, MSG_LEGACY_ERROR , xx_fmt, a, b)	 
#define ATS_MSG_SPRINTF( xx_fmt, a, b, c )  \
     MSG_SPRINTF_3  ( MSG_SSID_ATS, MSG_LEGACY_ERROR , xx_fmt, a, b, c )
#define ATS_MSG_SPRINTF_HIGH( xx_fmt, a, b, c )  \
		 MSG_SPRINTF_3	( MSG_SSID_ATS, MSG_LEGACY_HIGH , xx_fmt, a, b, c )


#define ATS_ERR_FATAL( xx_fmt, a, b, c )     ERR_FATAL( xx_fmt, a, b, c )

#else

#define ATS_MSG(xx_fmt)
#define ATS_MSG_1(xx_fmt, a)
#define ATS_MSG_2(xx_fmt, a, b)
#define ATS_MSG_3(xx_fmt, a, b, c)
#define ATS_MSG_4(xx_fmt, a, b, c, d)
#define ATS_MSG_5(xx_fmt, a, b, c, d, e)

#define ATS_ERR(xx_fmt)
#define ATS_ERR_1(xx_fmt, a)
#define ATS_ERR_2(xx_fmt, a, b)
#define ATS_ERR_3(xx_fmt, a, b, c)
#define ATS_ERR_4(xx_fmt, a, b, c, d)
#define ATS_ERR_5(xx_fmt, a, b, c, d, e)

#define ATS_MSG_LOW( xx_fmt, a, b, c )
#define ATS_MSG_MED( xx_fmt, a, b, c )
#define ATS_MSG_HIGH( xx_fmt, a, b, c )
#define ATS_MSG_ERROR( xx_fmt, a, b, c )

#define ATS_ERR_FATAL( xx_fmt, a, b, c )

#endif /* ATS_DIAG_F3_NOT_PRESENT */

#endif /* ATS_V_H */

