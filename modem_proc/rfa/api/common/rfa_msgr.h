/*!
  @file rfa_msgr.h

  @brief
  Describes all the mdoules using message router under Tech RFA. 

  @details
  This file contains all Tech_Module definitions under Tech RFA
  that needs to use the message router. RF driver of all modes
  (LTE, Wimax, 1x, HDR, WCDMA, GSM, GPS) that needs to use the 
  mesasge router needs to define the Tech_Module in this file in order
  to use the message router. This is a common header for rf
  driver of all modes.
  
*/

/*===========================================================================

  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfa_msgr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/31/13   sc      Added support for iRAT meas interface
07/23/12   Saul    COMMON. Renamed MSGR_MODULE_RF_CDMA to MSGR_MODULE_RF_COMMON
09/10/12   gh      Added support for RF tuner
04/04/12   pl      Added support for LTE_FW_UL
03/12/12   ggs     Added GSM FTM module support
03/26/12   sc      Added GSM module support
03/22/12   ka/ac   adding wcdma msgr   
03/22/12   pl      Adding LTE FW UL module
03/09/12   aro     Added MAX number of modules for RFA Tech
07/11/11   jhe     Added TD-SCDMA module
05/18/11   sty     Added MSG_ID for CDMA (1x and HDR combined)
05/18/11   sty     Deleted unused #defines
05/17/11   sty     Assigned values for HDR and 1x to be within valid bounds
05/18/10   aro     Defined 1x and HDR MSGR Modules
01/11/10   mkv     Update MODULE values for LTE L1 and FTM. 
08/26/09   mkv     Integrate REX MSGR interworking into RFA2
07/21/09   mkv	   Changed file location from task\posix to task\rex folder.
04/07/09   qma     Initial checkin and moved to common folder
===========================================================================*/

#ifndef RFA_MSGR_H
#define RFA_MSGR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */
#include "rfcom.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Note: The modules must be of the form MSGR_* for the umid_gen script to pick it up properly.
For each “module” in RFA, the following 2 lines are needed in rfa_msgr.h

#define MSGR_MODULE_<rfa_module>  0x??  (must be 1-byte)
#define MSGR_RFA_<rfa_module>     MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_<rfa_module> )*/

/*! Module for CDMA RF subsystem mesages */
#define MSGR_MODULE_RF_COMMON        0x0
/*! Module for common RF control mesages */
#define MSGR_MODULE_RF_CONTROL       0x01
/*! Module for WCDMA RF subsystem mesages */
#define MSGR_MODULE_RF_WCDMA         0x02
/*! Module for GSM RF subsystem messages */
#define MSGR_MODULE_RF_GSM           0x03
/*! Module for 1X RF subsystem mesages */
#define MSGR_MODULE_RF_1X            0x04
/*! Module for HDR RF subsystem mesages */
#define MSGR_MODULE_RF_HDR           0x05
/*! Module for GSM RF FTM subsystem messages */
#define MSGR_MODULE_RF_GSM_FTM       0x06
/*! Module for LTE RF subsystem mesages */
#define MSGR_MODULE_RF_LTE           0x07  /*Ensure that the value matches FCOM_LTE_MODE in rfcom_mode_enum_type*/
/*! Module for LTE RF Calibation subsystem mesages */
#define MSGR_MODULE_RF_LTE_FTM       0x08  /*Ensure that the value matches RFCOM_LTE_FTM_MODE in rfcom_mode_enum_type*/
/*! Module for LTE RF Calibation subsystem mesages */
#define MSGR_MODULE_RF_LTE_FW_UL     0x09  /*Ensure that the value matches RFCOM_LTE_FTM_MODE in rfcom_mode_enum_type*/
/*! Module for TDSCDMA RF subsystem mesages */
#define MSGR_MODULE_RF_TDSCDMA       0x0A
/*! Module for RF Tuner subsystem mesages */
#define MSGR_MODULE_RF_TUNER         0x0B
/*! Module for RF iRAT MEAS subsystem mesages */
#define MSGR_MODULE_RF_MEAS          0x0C
/*!< Module for TDSCDMA RF subsystem mesages */
#define MSGR_MODULE_RF_MAX           0x0F


/* Create additional modules e.g. 1x, HDR, WCDMA, GSM, GPS */

#define MSGR_RFA_RF_COMMON        MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_COMMON )
#define MSGR_RFA_RF_CONTROL       MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_CONTROL )
#define MSGR_RFA_RF_WCDMA         MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_WCDMA )
#define MSGR_RFA_RF_GSM           MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_GSM )
#define MSGR_RFA_RF_GSM_FTM       MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_GSM_FTM )
#define MSGR_RFA_RF_LTE           MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_LTE )
#define MSGR_RFA_RF_LTE_FTM       MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_LTE_FTM )
#define MSGR_RFA_RF_1X            MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_1X )
#define MSGR_RFA_RF_HDR           MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_HDR )
#define MSGR_RFA_RF_TDSCDMA       MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_TDSCDMA )
#define MSGR_RFA_RF_LTE_FW_UL     MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_LTE_FW_UL )
#define MSGR_RFA_RF_TUNER         MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_TUNER )
#define MSGR_RFA_RF_MEAS          MSGR_TECH_MODULE( MSGR_TECH_RFA, MSGR_MODULE_RF_MEAS )

/* Additional Tech_Module defintions e.g. 1x, HDR, WCDMA, GSM, GPS */

#endif /* RFA_MSGR_H */

