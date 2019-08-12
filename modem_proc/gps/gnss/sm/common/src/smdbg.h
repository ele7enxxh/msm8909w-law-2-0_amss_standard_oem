#ifndef PDSMDBG_H
#define PDSMDBG_H

/*===========================================================================

     Position Determination Session Manager Header File.

DESCRIPTION
  This header file contains debug macro definitions 

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/smdbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/05   atien   Add the missing define "PDSM_STDDBG_MSG_ERROR" so 
                   undefining FEATURE_GPSONE_STANDALONE_PDSMDBG
                   will not cause compilation error in PDSM code.
10/09/03   ld      Removed #ifdef FEATURE_GPSONE
04/11/02   cah     Fixed Header for file revision.
03/14/98   SK      Initial release.

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */


/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/* Debug/PC independent macros.
*/
#ifdef FEATURE_GPSONE_STANDALONE

// uncomment for debugging messages for STANDALONE
#define FEATURE_GPSONE_STANDALONE_PDSMDBG

#ifdef FEATURE_GPSONE_STANDALONE_PDSMDBG
#define PDSM_STDDBG_MSG_LOW( str, a, b, c )       MSG_LOW( "=PDSM_STDBG= " str, a, b, c )
#define PDSM_STDDBG_MSG_MED( str, a, b, c )       MSG_MED( "=PDSM_STDBG= " str, a, b, c )
#define PDSM_STDDBG_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=PDSM_STDBG= " str, a, b, c )
#ifdef PDSM_DEBUG
#define PDSM_STDDBG_MSG_ERROR(str, a, b, c)       MSG_FATAL("=PDSM_STDBG= " str, a, b, c)
#else
#define PDSM_STDDBG_MSG_ERROR(str, a, b, c)       MSG_ERROR("=PDSM_STDBG= " str, a, b, c)
#endif /* PDSM_DEBUG */
#define PDSM_STDDBG_ERR_FATAL( str, a, b, c )     ERR_FATAL( "=PDSM_STDBG= " str, a, b, c )
#else
#define PDSM_STDDBG_MSG_LOW( str, a, b, c )
#define PDSM_STDDBG_MSG_MED( str, a, b, c )
#define PDSM_STDDBG_MSG_HIGH( str, a, b, c )
#define PDSM_STDDBG_MSG_ERROR( str, a, b, c) 
#define PDSM_STDDBG_ERR_FATAL( str, a, b, c )
#endif

#endif /* FEATURE_GPSONE_STANDALONE */

#define PDSM_MSG_LOW( str, a, b, c )       MSG_LOW( "=PDSM= " str, a, b, c )
#define PDSM_MSG_MED( str, a, b, c )       MSG_MED( "=PDSM= " str, a, b, c )
#define PDSM_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=PDSM= " str, a, b, c )
#define PDSM_ERR_FATAL( str, a, b, c )     ERR_FATAL( "=PDSM= " str, a, b, c )

#define PDSM_FUNC_NOT_IMP( func ) \
    PDSM_MSG_HIGH( #func " NOT Implemented!",0,0,0 )

#define PDSM_CASE_NOT_IMP( _case ) \
    PDSM_ERR( #_case " case NOT Implemented",0,0,0 )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Debug dependent macros.
*/

#ifdef PDSM_DEBUG
  
  #define PDSM_ERR( str,a,b,c )            ERR( "=PDSM= " str,a,b,c )
  #define PDSM_MSG_ERROR( str,a,b,c )      ERR_FATAL( "=PDSM= " str,a,b,c )
  #define PDSM_DBG_ERR( str,a,b,c )        ERR( "=PDSM= " str,a,b,c )
  #define PDSM_CASE_ERR( str,a,b,c )       ERR_FATAL( "=PDSM= " str,a,b,c )
  #define PDSM_SWITCH_ERR( str,a,b,c )     ERR_FATAL( "=PDSM= " str,a,b,c )
  #define PDSM_ELSE_ERR( str ) \
               else { ERR_FATAL( "=PDSM= else " str " make no sense",0,0,0 ); }

#else
  
  #define PDSM_ERR( str,a,b,c )            ERR( "=PDSM= " str,a,b,c )
  #define PDSM_MSG_ERROR( str,a,b,c )      MSG_ERROR( "=PDSM= " str,a,b,c )
  #define PDSM_DBG_ERR( str,a,b,c )
  #define PDSM_CASE_ERR( str,a,b,c )       ERR_FATAL( "=PDSM= " str,a,b,c )
  #define PDSM_SWITCH_ERR( str,a,b,c )     ERR_FATAL( "=PDSM= " str,a,b,c )
  #define PDSM_ELSE_ERR( str ) \
               else { MSG_MED( "=PDSM= " str " else make no sense",0,0,0 ); }

#endif



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* PC dependent macros.
*/

#if (TG==T_PC)
  
  #define PDSM_FUNC_START( str,a,b,c ) PDSM_MSG_LOW( "START " str,a,b,c )
  #define PDSM_FUNC_END( str,a,b,c )   PDSM_MSG_LOW( "END " str,a,b,c )

#else  
  
  #define PDSM_FUNC_START( str,a,b,c )
  #define PDSM_FUNC_END( str,a,b,c )

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* Debug and PC dependent macros.
*/

#ifdef PDSM_DEBUG

  #if (TG==T_PC)
    
    #define PDSM_DBG_ERR_FATAL( str,a,b,c )  ERR( "=PDSM= " str,a,b,c )

  #else  

    #define PDSM_DBG_ERR_FATAL( str,a,b,c )  ERR_FATAL( "=PDSM= " str,a,b,c )
  
  #endif

#else  

  #define PDSM_DBG_ERR_FATAL( str,a,b,c )
  
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Asserts and verifications macros.
*/

#ifdef PDSM_DEBUG
  #define PDSM_ASSERT( exp )       \
    if( !(exp) )                 \
    {                            \
      ERR_FATAL( "ASSERTION FAILED! " #exp,0,0,0 );  \
    }
#else
  #define PDSM_ASSERT( exp )
#endif


#ifdef PDSM_DEBUG
  #define PDSM_ASSERT_ERR( exp )  PDSM_ASSERT( exp )
#else
  #define PDSM_ASSERT_ERR( exp )                         \
    if( !(exp) )                                       \
    {                                                  \
      ERR( "=PDSM= ASSERTION FAILED! " #exp ,0,0,0);      \
    }
#endif


#define PDSM_NONE_DBG_ASSERT( exp )                      \
    if( !(exp) )                                       \
    {                                                  \
      ERR_FATAL( "ASSERTION FAILED! " #exp ,0,0,0 );    \
    }

#ifdef PDSM_DEBUG
  #define PDSM_VERIFY( exp )                             \
    if( !(exp) )                                       \
    {                                                  \
      ERR_FATAL( "VERIFICATION FAILED! " #exp,0,0,0 ); \
    }
#else
  #define PDSM_VERIFY( exp )                             \
    if( !(exp) )                                       \
    {                                                  \
      return FALSE;                                    \
    }
#endif

#endif /* PDSMDBG_H */





