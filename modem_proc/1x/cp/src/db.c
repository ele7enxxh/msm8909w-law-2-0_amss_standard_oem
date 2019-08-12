/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D M S S    D a t a b a s e

GENERAL DESCRIPTION
  This module implements the necessary functions to provide to higher
  layer modules a simple interface to the DMSS database.

EXTERNALIZED FUNCTIONS
  db_put() and db_get()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The initialization of each one of the items in the database is
  responsibility of the owner of the item.

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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/db.c_v   1.3   23 Jan 2002 11:28:10   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/db.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/13   jh      Changed initial rf_mode from CDMA to SLEEP
02/10/11   ssh     Added DSDS status indicator in db for MMODE, SRCH to use.
11/24/09   jj      Mainlined FEATURE_GPSONE for an item db_gps_fact_test_mode.
11/24/09   jj      Mainlined FEATURE_GPSONE for an item DB_GPS_FACT_TEST
08/02/09   jtm     Lint fixes.
05/14/09   ag      CMI phase 2 changes
05/13/09   ag      Set db_rf_mode to DB_RF_CDMA
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
02/13/07   pg      Make DB_LP_SEC, DB_LTM_OFF, and DB_DAYLT persistent over
                   power cycle.
04/26/06   an      Fixed Lint Errors
09/29/05   sab     Removed DB_VOICE_PRIVACY and DB_PRIVACY_MODE
12/12/01   abh     Added support for gpsOne factory test.
06/27/01   ych     Removed JCDMA code that would initialize db_nosvc to FALSE
                   and db_digital to TRUE
01/24/01   ych     Merged JCDMA features.
10/16/00   lad     Removed event reports (these were for test).
12/14/99   lad     Added event reports.
05/17/99   SIU     Added hold origination counter db item.
12/10/98   dna     Fix CR7143.  Add new db item for powerup scanning
04/24/98   dbd     Added DB_VOICE_AS_DATA for service negotiation
12/20/96   rdh     Changed functions to Pascal calling convention to save ROM.
12/21/95   jca     Added DB_BS_P_REV for 13K/8K service negotiation.
10/26/95   jca     Deleted DB_ORIG_MIN_INDEX.
10/04/95   rdh     Separated SLEEP_ACTIVE from SLEEP_MODE.
09/20/95   rdh     Added SLEEP_ACTIVE to tell whether or not we are sleeping.
08/10/95   cah     Added Voice Privacy active flag.
07/06/95   rah     Added 13K_VOC_AVAILABLE for 13kbps vocoder support.
06/28/95   rdh     Added ACQ_MODE for setting acquisition mode.
06/21/95   rdh     Added SLEEP_MODE for disabling sleep.
12/09/94   ras     Added OFF_HOOK for hook switch state.
02/02/94   jjw     Removed SIO_INIT, Added DB_DATA_SRVC_ENABLED (noticed the
                   addition of DB_RF_900MHZ???)
06/29/93   jca     Fixed some Lint errors.
04/27/93   jai     Deleted DB_SID_NID_LIST item.
04/13/93   jjw     Added DB_SIO_INIT item
09/16/92   ARH     Added DB_VOX
09/09/92   ARH     Added DB_HOLD_ORIG
09/04/92   ARH     Added DB_ORIG_MIN_INDEX
09/02/92   ARH     Added DB_CODE_CHAN and DB_PILOT_PN_OFF
09/01/92   JJW     Changed 'udpated' to 'updated' to conform to English
07/24/92   ARH     Added DB_RF_MODE item
06/10/92   ARH     Added SID_NID_LIST and SYSTEM_STATE item
04/22/92   ARH     Updated per SDD review
03/16/92   ARH     Creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "db_v.h"
#include "rex.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*
** Database Items
*/

LOCAL boolean      db_in_use;   /* Call in progress indication             */
LOCAL boolean      db_no_svc;   /* Cannot find cellular service indication */
LOCAL boolean      db_digital;  /* Indicates CDMA/Analog mode of operation */
LOCAL boolean      db_conv;     /* Indicates Conversation is taking place  */
LOCAL db_roam_type db_roam;     /* Roaming status indication               */
LOCAL word         db_channel;  /* Current channel mobile is tuned to      */
LOCAL byte         db_code_channel; /* Current code channel                */
LOCAL word         db_pilot_pn_off; /* Current pilot PN offset             */
LOCAL word         db_sid;      /* Current System ID                       */
LOCAL word         db_locaid;   /* Current Location ID                     */
LOCAL word         db_nid;      /* Current Network ID                      */
LOCAL byte         db_lp_sec;   /* Leap Seconds from System Time           */
LOCAL byte         db_ltm_off;  /* Local time offset from System Time      */
LOCAL boolean      db_daylt;    /* Daylight savings time indicator         */
LOCAL db_dmss_state_type  db_dmss_state; /* Current system state           */
LOCAL byte         db_power;    /* Current mobile output power level       */
LOCAL word         db_rssi;     /* Current RSSI level                      */
LOCAL word         db_cdma_rssi; /* Current CDMA RSSI level                */
LOCAL word         db_battery_level; /* Current battery level              */
LOCAL word         db_temperature;   /* Current temperature                */
LOCAL db_rf_mode_type db_rf_mode;    /* Current RF mode                    */
LOCAL boolean      db_hold_orig;     /* Hold origination commands flag     */
LOCAL word         db_hold_orig_cnt; /* Hold origination change counter    */
LOCAL boolean      db_powerup_scanning; /* Hold origination on powerup     */
LOCAL boolean      db_on_hook;       /* Is the subscriber unit on hook?    */
LOCAL boolean      db_vox;           /* Current VOX mode                   */
LOCAL boolean      db_data_srvc_enabled;  /* current state of Data task    */
LOCAL word         db_sleep_mode;    /* Is sleep enabled?                  */
LOCAL word         db_sleep_active;  /* Is sleep currently occurring?      */
LOCAL word         db_acq_mode;      /* Type of CDMA acquisition desired   */
LOCAL boolean      db_13k_voc_available;/* Flag to indicate 13k capability */
LOCAL byte         db_bs_p_rev;      /* Base station's protocol rev level  */
LOCAL db_voice_as_data_type db_voice_as_data; /* Answer next call as data  */
LOCAL boolean      db_gps_fact_test_mode; /* Whether GPS Factory test is   */
                                      /* enabled or not.               */
LOCAL boolean      db_dsds_status;   /* Flag to indicate if DSDS is active */

/*===========================================================================

                         PUBLIC PROCEDURES

===========================================================================*/

/*===========================================================================

FUNCTION db_put

DESCRIPTION
  Changes the value of the item specified. The tasks that need to know that
  the value of the item has changed are notified.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void db_put
(
  db_items_type        item,        /* Item to be changed                  */
  db_items_value_type  *value_ptr   /* Value to change the item to         */

)
{
  switch(item) {

    case DB_IN_USE:
      db_in_use  = value_ptr->in_use;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_IN_USE updated to %d",
        db_in_use);

      /* Add notifications here */
      break;

    case DB_NO_SVC:
      db_no_svc  = value_ptr->no_svc;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_NO_SVC updated to %d",
        db_no_svc);

      /* Add notifications here */
      break;

    case DB_DIGITAL:
      db_digital = value_ptr->digital;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_DIGITAL updated to %d",
        db_digital);

      /* Add notifications here */
      break;

    case DB_CONV:
      db_conv    = value_ptr->conv;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_CONV updated to %d",
        db_conv);

      /* Add notifications here */
      break;

    case DB_ROAM:
      db_roam    = value_ptr->roam;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_ROAM updated to %d",
        db_roam);

      /* Add notifications here */
      break;

    case DB_CHANNEL:
      db_channel = value_ptr->channel;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_CHANNEL updated to %d",
        db_channel);

      /* Add notifications here */
      break;

    case DB_CODE_CHANNEL:
      db_code_channel = value_ptr->code_channel;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_CODE_CHANNEL updated to %d",
        db_code_channel);

      /* Add notifications here */
      break;

    case DB_PILOT_PN_OFF:
      db_pilot_pn_off = value_ptr->pilot_pn_off;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_PILOT_PN_OFF updated to %d",
        db_pilot_pn_off);

      /* Add notifications here */
      break;

    case DB_SID:
      db_sid     = value_ptr->sid;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_SID updated to %d",
        db_sid);

      /* Add notifications here */
      break;

    case DB_LOCAID:
      db_locaid  = value_ptr->locaid;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_LOCAID updated to %d",
        db_locaid);

      /* Add notifications here */
      break;

    case DB_NID:
      db_nid     = value_ptr->nid;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_NID updated to %d",
        db_nid);

      /* Add notifications here */
      break;

    case DB_LP_SEC:
      db_lp_sec  = value_ptr->lp_sec;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_LP_SEC updated to %d",
        db_lp_sec);

      /* Add notifications here */
      break;

    case DB_LTM_OFF:
      db_ltm_off = value_ptr->ltm_off;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_LTM_OFF updated to %d",
        db_ltm_off);

      /* Add notifications here */
      break;

    case DB_DAYLT:
      db_daylt   = value_ptr->daylt;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_DAYLT updated to %d",
        db_daylt);

      /* Add notifications here */
      break;

    case DB_DMSS_STATE:
      db_dmss_state = value_ptr->dmss_state;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_DMSS_STATE updated to %d",
        value_ptr->dmss_state);

      /* Add notifications here */
      break;

    case DB_POWER:
      db_power   = value_ptr->power;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_POWER updated to %d",
        db_power);

      /* Add notifications here */
      break;

    case DB_RSSI:
      db_rssi = value_ptr->rssi;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_RSSI updated to %d",
        db_rssi);

      /* Add notifications here */
      break;

    case DB_CDMA_RSSI:
      db_cdma_rssi = value_ptr->cdma_rssi;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_CDMA_RSSI updated to %d",
        db_cdma_rssi);

      /* Add notifications here */
      break;

    case DB_BATTERY_LEVEL:
      db_battery_level = value_ptr->battery_level;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_BATTERY_LEVEL updated to %d",
        db_battery_level);

      /* Add notifications here */
      break;

    case DB_TEMPERATURE:
      db_temperature = value_ptr->temperature;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_TEMPERATURE updated to %d",
        db_temperature);

      /* Add notifications here */
      break;

    case DB_RF_MODE:
      db_rf_mode = value_ptr->rf_mode;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_RF_MODE updated to %d",
        db_rf_mode);

      /* Add notifications here */
      break;

    case DB_HOLD_ORIG:
      db_hold_orig = value_ptr->hold_orig;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_HOLD_ORIG updated to %d",
        db_hold_orig);

      /* Add notifications here */
      break;

    case DB_POWERUP_SCANNING:
      db_powerup_scanning = value_ptr->powerup_scanning;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_POWERUP_SCANNING updated to %d",
        db_powerup_scanning);

      /* Add notifications here */
      break;

    case DB_ON_HOOK:
      db_on_hook = value_ptr->on_hook;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_ON_HOOK updated to %d",
        db_on_hook);

      /* Add notifications here */
      break;

    case DB_VOX:
      db_vox = value_ptr->vox;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_VOX updated to %d",
        db_vox);

      /* Add notifications here */
      break;

    case DB_DATA_SRVC_STATE:
      db_data_srvc_enabled = value_ptr->data_srvc_enabled;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_DATA_SRVC_ENABLED updated to %d",
        db_data_srvc_enabled);

      /* Add notifications here */
      break;

    case DB_SLEEP_MODE:
      db_sleep_mode = value_ptr->sleep_mode; /*lint !e641 */
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_SLEEP_MODE updated to %d",
        db_sleep_mode);

      /* Add notifications here */
      break;

    case DB_SLEEP_ACTIVE:
      db_sleep_active = value_ptr->sleep_active; /*lint !e641 */
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_SLEEP_ACTIVE updated to %d",
        db_sleep_active);

      /* Add notifications here */
      break;

    case DB_ACQ_MODE:
      db_acq_mode = value_ptr->acq_mode; /*lint !e641 */
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_ACQ_MODE updated to %d",
        db_acq_mode);

      /* Add notifications here */
      break;

    case DB_13K_VOC_AVAILABLE:
      db_13k_voc_available = value_ptr->voc13k_available;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_13K_VOC_AVAILABLE updated to %d",
        db_13k_voc_available);

      /* Add notifications here */
      break;

    case DB_BS_P_REV:
      db_bs_p_rev = value_ptr->bs_p_rev;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_BS_P_REV updated to %d",
        db_bs_p_rev);

      /* Add notifications here */
      break;

    case DB_VOICE_AS_DATA:
      db_voice_as_data = value_ptr->voice_as_data;
      M1X_MSG( DCP, LEGACY_LOW,
        "DB_VOICE_AS_DATA updated to %d",
        db_voice_as_data);

      /* Add notifications here */
      break;

    case DB_GPS_FACT_TEST:
      db_gps_fact_test_mode = value_ptr->gps_fact_test_mode;
      break;

    case DB_DSDS_STATUS:
      db_dsds_status  = value_ptr->dsds_status;
      break;

    default:
      ERR_FATAL("Bad item parameter %d ", (int) item, 0,0);

  } /* end switch on item */

} /*lint !e818 */

/*===========================================================================

FUNCTION db_get

DESCRIPTION
  Retrieve the value of an item from the database

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void db_get
(
  db_items_type         item,       /* Item to get value from               */
  db_items_value_type  *value_ptr   /* Buffer to retrieve item into         */
)
{
  switch(item) {

    case DB_IN_USE:
      value_ptr->in_use              = db_in_use;
      break;

    case DB_NO_SVC:
      value_ptr->no_svc              = db_no_svc;
      break;

    case DB_DIGITAL:
      value_ptr->digital             = db_digital;
      break;

    case DB_CONV:
      value_ptr->conv                = db_conv;
      break;

    case DB_ROAM:
      value_ptr->roam                = db_roam;
      break;

    case DB_CHANNEL:
      value_ptr->channel             = db_channel;
      break;

    case DB_CODE_CHANNEL:
      value_ptr->code_channel        = db_code_channel;
      break;

    case DB_PILOT_PN_OFF:
      value_ptr->pilot_pn_off        = db_pilot_pn_off;
      break;

    case DB_SID:
      value_ptr->sid                 = db_sid;
      break;

    case DB_LOCAID:
      value_ptr->locaid              = db_locaid;
      break;

    case DB_NID:
      value_ptr->nid                 = db_nid;
      break;

    case DB_LP_SEC:
      value_ptr->lp_sec              = db_lp_sec;
      break;

    case DB_LTM_OFF:
      value_ptr->ltm_off             = db_ltm_off;
      break;

    case DB_DAYLT:
      value_ptr->daylt               = db_daylt;
      break;

    case DB_DMSS_STATE:
      value_ptr->dmss_state          = db_dmss_state;
      break;

    case DB_POWER:
      value_ptr->power               = db_power;
      break;

    case DB_RSSI:
      value_ptr->rssi                = db_rssi;
      break;

    case DB_CDMA_RSSI:
      value_ptr->cdma_rssi           = db_cdma_rssi;
      break;

    case DB_BATTERY_LEVEL:
      value_ptr->battery_level       = db_battery_level;
      break;

    case DB_TEMPERATURE:
      value_ptr->temperature         = db_temperature;
      break;

    case DB_RF_MODE:
      value_ptr->rf_mode             = db_rf_mode;
      break;

    case DB_HOLD_ORIG:
      value_ptr->hold_orig           = db_hold_orig;
      break;

    case DB_POWERUP_SCANNING:
      value_ptr->powerup_scanning    = db_powerup_scanning;
      break;

    case DB_ON_HOOK:
      value_ptr->on_hook             = db_on_hook;
      break;

    case DB_VOX:
      value_ptr->vox                 = db_vox;
      break;

    case DB_DATA_SRVC_STATE:
        value_ptr->data_srvc_enabled = db_data_srvc_enabled;
        break;

    case DB_SLEEP_MODE:
        value_ptr->sleep_mode        = (db_sleep_mode_type) db_sleep_mode;
        break;

    case DB_SLEEP_ACTIVE:
        value_ptr->sleep_active      = (db_sleep_active_type) db_sleep_active;
        break;

    case DB_ACQ_MODE:
        value_ptr->acq_mode          = (db_acq_mode_type) db_acq_mode;
        break;

    case DB_13K_VOC_AVAILABLE:
        value_ptr->voc13k_available = db_13k_voc_available;
        break;

    case DB_BS_P_REV:
        value_ptr->bs_p_rev         = db_bs_p_rev;
        break;

    case DB_VOICE_AS_DATA:
        value_ptr->voice_as_data    = db_voice_as_data;
        break;

    case DB_GPS_FACT_TEST:
        value_ptr->gps_fact_test_mode = db_gps_fact_test_mode;
        break;

    case DB_DSDS_STATUS:
      value_ptr->dsds_status = db_dsds_status;
      break;

    default:
      ERR_FATAL("Bad item parameter %d ",(int) item, 0,0);

  } /* end switch on item */

} /* end db_get */

/*===========================================================================

FUNCTION DB_INIT

DESCRIPTION
  Initializes the database to default values. No notifications are made
  when setting the values to default.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void db_init( void )
{
  word index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------
  ** Initialize each item of the database to some default value
  ** ---------------------------------------------------------- */
  for (index=0; index < ((word) DB_LAST_ITEM); index++) {
    switch(index) {
      case DB_IN_USE:
        db_in_use = FALSE;
        break;

      case DB_NO_SVC:
        db_no_svc = TRUE;
        break;

      case DB_DIGITAL:
        db_digital = FALSE;
        break;

      case DB_CONV:
        db_conv = FALSE;
        break;

      case DB_ROAM:
        db_roam = DB_ROAM_OFF;
        break;

      case DB_CHANNEL:
        db_channel = 0;
        break;

      case DB_CODE_CHANNEL:
        db_code_channel = 0;
        break;

      case DB_PILOT_PN_OFF:
        db_pilot_pn_off = 0;
        break;

      case DB_SID:
        db_sid = 0;
        break;

      case DB_LOCAID:
        db_locaid = 0;
        break;

      case DB_NID:
        db_nid = 0;
        break;

      case DB_LP_SEC:
        db_lp_sec = 0;
        break;

      case DB_LTM_OFF:
        db_ltm_off = 0;
        break;

      case DB_DAYLT:
        db_daylt = 0;
        break;

      case DB_DMSS_STATE:
        db_dmss_state = DB_DMSS_POWERUP_S;
        break;

      case DB_POWER:
        db_power = 0;
        break;

      case DB_RSSI:
        db_rssi = 0;
        break;

      case DB_CDMA_RSSI:
        db_cdma_rssi = 0;
        break;

      case DB_BATTERY_LEVEL:
        db_battery_level = 0;
        break;

      case DB_TEMPERATURE:
        db_temperature = 0;
        break;

      case DB_RF_MODE:
        db_rf_mode = DB_RF_SLEEP;  /* Start in sleep mode */
        break;

      case DB_HOLD_ORIG:
        db_hold_orig = TRUE;
        break;

      case DB_POWERUP_SCANNING:
        db_powerup_scanning = FALSE;
        break;

      case DB_ON_HOOK:
        db_on_hook = TRUE;
        break;

      case DB_VOX:
        db_vox = FALSE;
        break;

      case DB_DATA_SRVC_STATE:
        db_data_srvc_enabled = FALSE;
        break;

      case DB_SLEEP_MODE:
        db_sleep_mode = DB_SLEEP_ENABLED; /*lint !e641 */
        break;

      case DB_SLEEP_ACTIVE:
        db_sleep_active = DB_SLEEP_OFF; /*lint !e641 */
        break;

      case DB_ACQ_MODE:
        db_acq_mode = DB_ACQ_FULL; /*lint !e641 */
        break;

      case DB_13K_VOC_AVAILABLE:
        db_13k_voc_available = FALSE;
        break;

      case DB_BS_P_REV:
        db_bs_p_rev = 1;
        break;

      case DB_VOICE_AS_DATA:
        db_voice_as_data = DB_VOICE_AS_DATA_NEVER;
        break;

      case DB_GPS_FACT_TEST:
        db_gps_fact_test_mode = FALSE;
        break;

      case DB_DSDS_STATUS:
  db_dsds_status = FALSE;
        break;

      default:
        ERR_FATAL("Bad item parameter %d ",index,0,0);

    } /* end switch on item */

  } /* End loop for each item */

} /* end db_init() */

