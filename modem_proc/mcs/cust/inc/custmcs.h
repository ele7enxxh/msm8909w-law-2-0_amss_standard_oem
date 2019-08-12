#ifndef CUSTMCS_H
#define CUSTMCS_H
/*===========================================================================

DESCRIPTION
  Configuration for MCS.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cust/inc/custmcs.h#1 $

===========================================================================*/

/* ------------------------------------------------------------------------
** Search 4 Features
** ------------------------------------------------------------------------ */
 
/* Define Search 4 debug message mask */
#define MSG_DBG_SRCH4        MSG_MASK_20

/* ------------------------------------------------------------------------
** TCXO Manager Features
** ------------------------------------------------------------------------ */

#ifndef FEATURE_XO
  /* For VCTCXO targets, enable this feature always. Makes sure RGS is
     consistent across various variants */
  #define FEATURE_UMTS_NEGATE_ALL_TCXOMGR_VALUES
#endif

#ifdef T_RUMI_EMULATION
  /* Define features for RUMI bringup */
  
  /* For disabling temp Reads via ADC DAL */
  #define FEATURE_TCXOMGR_BRINGUP
  
  /* For disabling XO Trim PMIC read/writes */
  #define FEATURE_TCXOMGR_PMIC_BRINGUP
#endif

/* ------------------------------------------------------------------------
** Transceiver Resource Manager Features
** ------------------------------------------------------------------------ */

#ifndef FEATURE_MODEM_RCINIT 
  /* Turn on initialization of the TRM service within TMC */
  #define FEATURE_TMC_TRM
#endif

/* Enable Transceiver Resource Manager */
#define FEATURE_MCS_TRM

/* ------------------------------------------------------------------------
** Coexistence Manager Features
** ------------------------------------------------------------------------ */

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  #define TRM_ASDIV_FEATURE_ENABLED  
#endif

#endif /* CUSTMCS_H */
