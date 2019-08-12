#ifndef MODEM_MCS_DEFS_H
#define MODEM_MCS_DEFS_H
/*===========================================================================
                      M O D E M   MCS   D E F S

DESCRIPTION
  This file contains modem MCS definitions.

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


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cust/inc/modem_mcs_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/12   ejv     Add Dime and unicore support.
02/13/12   rj      Remove CMAPI sub-features
02/10/12   sr      TCXOMGR Feature Cleanup
01/17/12   ag      TRM feature cleanup
01/16/12   ejv     Set CMAPI features based on top-level.
07/06/11   ag      Remove ADCDAL featurization 
03/31/11   ns      Enable RPC client or server for only Type II fusion
02/09/11   ejv     Allow TEST_FRAMEWORK target.
02/07/11   ejv     Add FEATURE_MSGRX2 for NIKEL.
02/02/11   sm      Move PAM Feature to custmcs
01/14/11   jg      Changed T_MSM8960 to FEATURE_MODEM_HW_NIKEL.
12/13/10   ag      Feature for 8960.
11/10/10   cjb     Enable AFLT for MDM9K 2.2 PL ONLY.
10/11/10   ag      Enable PAM on Genesis 2.2
08/19/10   ns      Mainline INO DAL and beautified the code 
05/25/10   sm      Added INO DAL support for WCDMA IQ sample collection
03/09/10   ag      Removing FEATURE_DSM_Q_DESTROY as its breaking apps build
03/08/10   ag      Define FEATURE_DSM_Q_DESTROY for GENESIS.
02/01/10   sm      Enabled ADC DAL for GENESIS 
11/25/09   ag      Added define FEATURE_TCXOMGR_ADCDAL for Poseidon.
11/13/09   ag      Define mutex features for GENESIS modem. 
11/09/09   ag      Undefined SRCH4 features for GENESIS modem.
09/14/09   sb      Add support for MDM9K 
05/20/09   ns      Add support for Karura 
03/27/09   ns      Initial revision.

===========================================================================*/

#include "mcs_variation.h"
#include "customer.h"


/*===========================================================================
                          MODEM FEATURE DEFINES
===========================================================================*/

/* ------------------------------------------------------------------------
** Target Verification
** ------------------------------------------------------------------------ */

/* Check target configuration to ensure only one modem is enabled */
#if ( defined( T_POSEIDON2 ) && defined( T_MSM8650B ) )
  #error Unsupported target configuration - multiple modem definitions
#endif

/* ------------------------------------------------------------------------
** Modem Configuration
** ------------------------------------------------------------------------ */

/* Enable the modem based on target configuration */
#ifdef FEATURE_MODEM_HW_DIME
  /* Dime modem */
  #ifdef UNICORE_MODEM
    /* Unicore does not need dual MSGR */
    #undef FEATURE_MSGRX2
  #else
    /* Enable dual MSGR */
    #define FEATURE_MSGRX2
  #endif /* UNICORE_MODEM */
  #undef FEATURE_MSGR_2ND_PORT
#elif defined( FEATURE_MODEM_HW_NIKEL )
  /* NikeL modem */
  #define FEATURE_MODEM_MCS_POSEIDON
  #define FEATURE_MCS_Q6
  #ifdef UNICORE_MODEM
    /* Unicore does not need dual MSGR */
    #undef FEATURE_MSGRX2
  #else
    /* Enable dual MSGR */
    #define FEATURE_MSGRX2
  #endif /* UNICORE_MODEM */
  #define FEATURE_MSGR_2ND_PORT
#elif defined( T_MDM9X00 )
  /* Genesis Modem */
  #define FEATURE_MODEM_MCS_GENESIS
  #define FEATURE_MODEM_MCS_POSEIDON
  #define FEATURE_MCS_Q6
  #define FEATURE_MSGR_2ND_PORT
  //#define FEATURE_MSGRX2 defined at target level for 9x00
#elif defined( T_POSEIDON2 )
  /* Poseidon Modem */
  #define FEATURE_MODEM_MCS_POSEIDON    
#elif defined( T_MSM8650B )
  /* Artemis Modem */
  #define FEATURE_MODEM_MCS_ARTEMIS
#elif defined( FEATURE_KARURA )
  /* Apollo code has been mainlined */ 
#elif defined( TEST_FRAMEWORK )
  #error code not present
#else
  /* Unknown Modem */
  #error Unknown modem specification
#endif

#endif /* MODEM_MCS_DEFS_H */

