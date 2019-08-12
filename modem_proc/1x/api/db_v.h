#ifndef DB_V_H
#define DB_V_H
/*===========================================================================

                                           Database Interface

DESCRIPTION
   This header file contains the definitions necessary to use
   the Database from other modules.

  Copyright (c) 1992 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/db.h_v   1.3   29 Mar 2002 10:19:08   hexinw  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/db_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   jtm     Initial SU API split for CP.
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
03/09/07   an      Put FEATURE_GPSONE featurization
02/13/07   pg      Make DB_LP_SEC, DB_LTM_OFF, and DB_DAYLT persistent over
                   power cycle.
11/11/05   fc      Removed privacy_mode from db_items_value_type
11/11/05   sb      Removed voice_privacy (associated with DB_VOICE_PRIVACY).
09/29/05   sab     Removed DB_VOICE_PRIVACY and DB_PRIVACY_MODE.
03/23/05   fc      Renamed FEATURE_RPC to FEATURE_HTORPC_METACOMMENTS.
03/11/05   fc      Changes for RPC.
10/15/03   lh      Included customer.h because of FEATURE_GPSONE.
12/19/02   ph      mainlined FEATURE_QUICK_SCAN.
03/08/02   hxw     Added GPS mode to support MSBased gpsOne.
12/12/01   abh     Added support for gpsOne factory test.
06/20/01   kss     Added DB_RF_HDR to db_rf_mode_type.
08/31/00   cah     (dsb) Added DB_RF_GPS to db_rf_mode_type.
12/10/98   dna     Fix CR7143.  Add new db item for powerup scanning
02/26/99   ejv     Merged changes for FEATURE_QUICK_SCAN.  Two db_acq_mode
                   types are no longer necessary for QUICK_SCAN.
06/25/98   na      Made the following changes to db_voice_as_data_type: Added
                   DB_VOICE_AS_FAX_ONCE, DB_VOICE_AS_FAX_ALWAYS,
                   DB_VOICE_AS_MODEM_ONCE, DB_VOICE_AS_MODEM_ALWAYS. Deleted
                   DB_VOICE_AS_DATA_ONCE, DB_VOICE_AS_DATA_ALWAYS.
04/24/98   dbd     Added DB_VOICE_AS_DATA for service negotiation
02/02/98   dgy/smo Added new interfaces for CDMA pilot acquisition.
12/20/96   rdh     Changed functions to Pascal calling convention to save ROM.
12/21/95   jca     Added DB_BS_P_REV for 13K/8K service negotiation.
10/26/95   jca     Deleted DB_ORIG_MIN_INDEX.
10/04/95   rdh     Separated SLEEP_ACTIVE from SLEEP_MODE.
09/20/95   rdh     Added SLEEP_ACTIVE to tell whether or not we are sleeping.
08/10/95   cah     Added Voice Privacy active flag.
07/06/95   rah     Added 13K_VOC_AVAILABLE for 13kbps vocoder support.
06/29/95   rdh     Enums fixed for acq and sleep mode.
06/28/95   rdh     Added ACQ_MODE for setting acquisition mode.
06/21/95   rdh     Added SLEEP_MODE for disabling sleep.
12/09/94   ras     Added OFF_HOOK for hook switch state.
04/13/94   jjw     Added DB_DATA_SRVC_STATE boolean item
01/24/94   jca     Added DB_RF_900_MHZ to db_rf_mode_type
04/27/93   jai     Deleted DB_SID_NID_LIST item.
04/13/93   jjw     Added DB_SIO_INIT item
03/26/93   jca     Added DB_RF_SLEEP to db_rf_mode_type
09/16/92   ARH     Added DB_VOX
09/09/92   ARH     Added DB_HOLD_ORIG
09/04/92   ARH     Added DB_ORIG_MIN_INDEX
09/02/92   ARH     Added DB_CODE_CHANNEL and DB_PILOT_PN_OFF
07/26/92   ARH     Added DB_RF_MODE
06/20/92   ARH     Added SID_NID_LIST
03/10/92   ARH     Creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "db.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/








/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/




#endif /*DB_V_H*/
