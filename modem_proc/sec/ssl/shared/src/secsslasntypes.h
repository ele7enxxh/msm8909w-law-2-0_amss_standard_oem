#ifndef SECSSLASNTYPES_H
#define SECSSLASNTYPES_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

            A B S T R A C T   S Y N T A X   N O T A T I O N   

       S S L   S P E C I F I C   T Y P E   D E F I N I T I O N S

GENERAL DESCRIPTION
  The Abstract Syntax Notation allows country and machine independent
  exchange of information.
  This file includes SSL specific type definitions.

  The following types are used by SSL to describe certificate
  contents:

  - ASN.1 Intergers
  - ASN.1 Strings
  - ASN.1 UTC Time

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslasntypes.h#1 $ 
  $DateTime: 2016/12/13 07:58:24 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/06   avm     Removed multiple definition of SECSSLASN_MAX_REC_LEN
                   This macro is defined in secx509asntypes.h
11/08/06   rv      increased SECSSLASN_MAX_REC_LEN to 64
05/30/01   om      Fixed pointer names
03/02/00   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "secx509asntypes.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM ASN.1

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/* ASN.1 Integer Type */
typedef struct 
{
  uint32   length;                 /* Length of the integer data           */
  uint8*   data_ptr;               /* Integer data                         */
} SECSSL_ASN1_INTEGER;

/* ASN.1 String Type */
typedef struct 
{
  uint32   length;                 /* Length of the string data            */
  uint8*   data_ptr;               /* String data                          */
} SECSSL_ASN1_STRING;

/* ASN.1 Universal Time Coordinated (UTC) Type */
typedef struct 
{
  uint32   length;                 /* Length of the time data              */
  uint8*   data_ptr;               /* Time data                            */
} SECSSL_ASN1_UTCTIME;


#endif /* FEATURE_SEC_SSL */

#endif
