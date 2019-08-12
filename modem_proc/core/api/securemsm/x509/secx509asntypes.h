#ifndef SECX509TYPES_H
#define SECX509TYPES_H

/** 
   @file secx509asntypes.h
   @brief ASN types for the SECX509 standard.
   This file contains the definitions of the constants, data structures
   and interfaces that provide ASN types for the SECX509 standard.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The secx509_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      secx509_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the secx509_apis group 
      description in the secx509_mainpage.dox file. 
===========================================================================*/
/*===========================================================================
   Copyright (c) 2000-2010 Qualcomm Technologies Incorporated.  
   All rights reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/x509/secx509asntypes.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/10   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
06/06/10   msh     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
10/31/06   df      CR100945 Increase Max rec len to 64
05/30/01   om      Fixed pointer names
03/02/00   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM ASN.1

===========================================================================*/
/** @addtogroup secx509_apis
  @{ */

/**
  Security SSL ASN.1 constants and definitions.
*/
#define SECSSLASN_MAX_REC_LEN       64
/** @} */  /* end_addtogroup secx509_apis */


/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
/** @addtogroup secx509_apis
  @{ */

/**
  @brief ASN.1 integer type.
*/
typedef struct
{
  uint32   length;                 /**< Length of the integer data.  */
  uint8*   data_ptr;               /**< Integer data.                */
} SECASN1_INTEGER;

/**
  @brief ASN.1 string type.
*/
typedef struct
{
  uint32   length;                 /**< Length of the string data.   */
  uint8*   data_ptr;               /**< String data.                 */
} SECASN1_STRING;

/**
  @brief ASN.1 Universal Time Coordinated (UTC) type.
*/
typedef struct
{
  uint32   length;                 /**< Length of the time data.     */
  uint8*   data_ptr;               /**< Time data.                   */
} SECASN1_UTCTIME;

/** @} */  /* end_addtogroup secx509_apis */
#endif
