#ifndef ASSERT_H
#define ASSERT_H

/**
  @file amssassert.h
  @brief Definitions for ASSERT macro.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_assert" group 
      description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/

/*==========================================================================

            A S S E R T   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  Definitions for ASSERT macro.
  
Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/assert.h_v   1.0   Aug 21 2000 11:51:02   lpetrucc  $
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/amssassert.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/09/10   EBR     Doxygenated file
12/15/98    ms     Removed spurious tabs.
02/19/98   jct     Created

===========================================================================*/


/*===========================================================================
 
                            INCLUDE FILES

===========================================================================*/


/*===========================================================================
 
                            CONSTANT DEFINITIONS

===========================================================================*/

/* -----------------------------------------------------------------------
** As a compiler definition, define ASSERT to be one of:
**    /DASSERT=ASSERT_FATAL
**    /DASSERT=ASSERT_WARN
** ASSERT_FATAL:
**   Invokes ERR_FATAL when the ASSERTion fails
** ASSERT_WARN:
**   Invokes ERR when the ASSERTion fails
** Others or no definition:
**   No processing
** ----------------------------------------------------------------------- */

/** Invokes ERR_FATAL when the assertion fails. */
#define ASSERT_FATAL 1
/** Invokes ERR when the assertion fails. */
#define ASSERT_WARN  2


/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

MACRO ASSERT

===========================================================================*/

/** @addtogroup utils_assert
@{ */

/**
  Given a boolean expression, ASSERT verifies that the condition is TRUE. If
  the condition is FALSE, ASSERT performs one of the following actions:
  
  - For ASSERT_FATAL -- Invokes ERR_FATAL processing.
  - For ASSERT_WARN -- Invokes ERR processing.
  - For all others -- No processing occurs.

  Examples are:
  
  - ASSERT will trigger because i is equal to 5, and the assertion states 
    that i must NOT be 5:
<pre>
  int i=5;
  ASSERT( i != 5 );
</pre>

  - ASSERT will NOT trigger because i is not equal to 5, and the assertion states 
    that i must NOT be 5:
<pre>
  int i=4;
  ASSERT( i != 5 );
</pre>
  
  @param[in] xx_exp Condition to be asserted.

  @return
  None.

  @dependencies
  For ASSERT_FATAL and ASSERT_WARN: err.h.
*/
#if   (ASSERT==ASSERT_FATAL)

  #undef ASSERT 

  #ifndef ERR_H
    #include "err.h"
  #endif
  
  #define ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR_FATAL( "Assertion " #xx_exp " failed", 0, 0, 0 ); \
     }
     
#elif (ASSERT==ASSERT_WARN)

  #undef ASSERT

  #ifndef ERR_H
    #include "err.h"
  #endif
  
  #define ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "Assertion " #xx_exp " failed",0,0,0 ); \
     }
          
#else

  #define ASSERT( xx_exp ) ((void)0)
  
#endif

/** @} */ /* end_addtogroup utils_assert */
#endif /* ASSERT.H */

