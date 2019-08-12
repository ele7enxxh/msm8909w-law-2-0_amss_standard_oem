#ifndef SECSSL_VERSION_H
#define SECSSL_VERSION_H
/*===========================================================================

FILE:  secssl_version.h

SERVICES:  version for stamping packages with version string


GENERAL DESCRIPTION:
   Shared Library Framework 
   Definitions for versioning

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslversion.h#1 $ 
  $DateTime: 2016/12/13 07:58:24 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/10   nk      Initial Revision

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "msg.h"

/*===========================================================================
* MACRO: PRINT_LIBRARY_VERSION_INFO
*
* Description: 
*    This macro prints the header
* Usage: This Macro is to be used on the initiliasation of the module
*        Eg: PRINT_LIBRARY_VERSION_INFO("SECURED SOCKET LAYER (SSL)" );
*===========================================================================*/ 
/*#define PRINT_LIBRARY_VERSION_INFO(header) \
{ \
  MSG_HIGH("=============================================================",0,0,0); \
  MSG_HIGH("#header LIBRARY VERSION INFORMATION ",0,0,0); \
  MSG_HIGH("=============================================================",0,0,0); \
} */

/*===========================================================================
* MACRO: PRINT_VERSION
*
* Description: 
*    This macro prints the following information to the Debug logs
*    
* Usage: This Macro is to be used on the initiliasation of the module
*        Eg: PRINT_VERSION(SSL_SHARED_LIB_VER);
*            PRINT_VERSION(SSL_CHIPSET_LIB_VER);
*            PRINT_VERSION(SSL_SHARED_VU_VER);
*            PRINT_VERSION(SSL_CHIPSET_VU_VER);
*            PRINT_VERSION(CHIPSET_ID);
*            PRINT_VERSION(OS_NAME);
*===========================================================================*/ 
/*#define PRINT_VERSION(name) \
{ \
  extern const char* const name; \
  MSG_HIGH( "#name %s", name, 0, 0 ); \
  }*/  

/*=====================================================================
VERSION_TYPE

Description:
   Defines the type of version (libraries or vu or su).
=======================================================================

VERSION_MAJOR

Description:
   Defines the major release number.
=======================================================================

VERSION_MINOR

Description:
   Defines the minor release number.
=======================================================================
VERSION_MAINT

Description:
   Defines the maintenance release number.
=======================================================================
*/

/* 
======================================================================= 
VERSION

Description:
   Defines the version of library/vu/su.
=======================================================================
*/

typedef struct version{
        char *version_string;
        char *version_major;
        char *version_minor;
        char *version_maint;
}Sversion;

/* 
======================================================================= 
STAMP VERSION

Description:
   The following snippet shows the usage of the verioning structures.
   1. Include version.h in the file used to populate the versions
   
   example:
 	  STAMP_VERSION(  LIB,
                          "CORE_SME_SSL_SHARED",
                          "00",
                          "00",
                          "01" );
======================================================================= */
/*#define STAMP_VERSION( vertype, vername, vermajor, verminor, vermaint ) \
        { \
            static const Sversion vertype = \
            { (#vertype)##_##(#vername), \
              #vermajor, \
              #verminor, \
              #vermaint }; \
        }*/
 
/*======================================================================= 
*  FUNCTION: secsslversion_print
*
*  Description:
*     The function prints the Library version information 
*     when switched on
*   
*   example: secsslversion_print(1);
*
*======================================================================= */
INLINE void secsslversion_print(uint16 ver_info_switch)
{
  if(ver_info_switch == 1)
  {
  }
}

 
/* ==================================================================== */ 
#endif /* SECSSL_VERSION_H */
