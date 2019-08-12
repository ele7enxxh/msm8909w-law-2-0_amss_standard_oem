#ifndef DB_H
#define DB_H

/*===========================================================================

                                           Database Interface
*/
/** 
@file db.h

@brief Database Header File. 

This header file contains the definitions necessary to use
the Database from other modules.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CDMA2k_1X_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the CDMA2k_1X_rapi_mainpage.dox file or 
      contact Tech Pubs.

      The above description for this file is part of the "db_interface" group 
      description in the CDMA2k_1X_rapi_mainpage.dox file. 
===========================================================================*/

/*===========================================================================
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/db.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/11   ssh     Added Doxygen comments.
02/10/11   ssh     Added DSDS status indicator in db for MMODE, SRCH to use. 
11/01/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
09/01/10   azh     Added Doxygen comments. 
08/31/09   adw     Removed unnecessary customer.h include.
04/02/09   jj      CMI Phase II changes.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Merge from main/latest, based on VU_MODEM_1X_CP.07.02.14
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
09/16/08   pk      Updating to keep in line with RF changes for 8650 bringup
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

/** @addtogroup db_interface 
@{ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/** Enumeration of items in the database. */
typedef enum {
  DB_IN_USE,             /**< Call is in progress. */
  DB_NO_SVC,             /**< Cannot find a cellular service. */
  DB_DIGITAL,            /**< Digital service. */
  DB_CONV,               /**< Conversation is taking place. */
  DB_ROAM,               /**< Roaming status. */
  DB_CHANNEL,            /**< Current channel to which the mobile is tuned. */
  DB_CODE_CHANNEL,       /**< Current code channel to which the mobile is tuned. */
  DB_PILOT_PN_OFF,       /**< Current pilot pseudonoise offset. */
  DB_SID,                /**< Current system identifier. */
  DB_LOCAID,             /**< Current location identifier. */
  DB_NID,                /**< Current network identifier. */
  DB_LP_SEC,             /**< Leap seconds since the start of the system time. */
  DB_LTM_OFF,            /**< Local time offset from the system time. */
  DB_DAYLT,              /**< Daylight saving time. */
  DB_DMSS_STATE,         /**< DMSS system state. */
  DB_POWER,              /**< Current mobile output power level. */
  DB_RSSI,               /**< Current frequency modulation received signal 
                                strength indicator level. */
  DB_CDMA_RSSI,          /**< Current CDMA received signal strength indicator 
                                level. */
  DB_BATTERY_LEVEL,      /**< Current battery level. */
  DB_TEMPERATURE,        /**< Current temperature. */
  DB_RF_MODE,            /**< Current radio frequency mode (analog, CDMA, PCN). */
  DB_HOLD_ORIG,          /**< Holds incoming origination commands. */
  DB_POWERUP_SCANNING,   /**< Holds origination on power up. */
  DB_ON_HOOK,            /**< Indicates whether the subscriber unit is on-hook. */
  DB_VOX,                /**< Current voice-operated exchange mode. */
  DB_DATA_SRVC_STATE,    /**< Current state of Data Services task. */
  DB_SLEEP_MODE,         /**< Indicates whether Sleep mode is enabled. */
  DB_SLEEP_ACTIVE,       /**< Indicates whether the current state is set to Sleep. */
  DB_ACQ_MODE,           /**< Type of CDMA Acquisition mode. */
  DB_13K_VOC_AVAILABLE,  /**< Vocoder 13K availability flag. */
  DB_BS_P_REV,           /**< Protocol revision level of the base station. */
  DB_VOICE_AS_DATA,      /**< Answer the next call as data. */
  DB_GPS_FACT_TEST,      /**< GPS Factory Test mode. */
  DB_DSDS_STATUS,        /**< Dual subscription status */

  DB_LAST_ITEM           /**< Maximum value of the enumeration. For internal use 
                              only. */
} db_items_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** @name Database Item Types
The following types are associated with items in the database.
@{ */

/** Roaming status enumeration for the DB_ROAM item. */
typedef enum {
  DB_ROAM_OFF,  /**< Roaming is off (type 0).*/
  DB_ROAM_ON,   /**< Roaming is on (type 1). */
  DB_ROAM_BLINK /**< Roaming is alternating between off and on (type 2). */
} db_roam_type;

/** Radio Frequency mode enumeration for the DB_RF_MODE item. */
typedef enum {
  DB_RF_ANALOG,              /**< RF is in the Analog (FM) mode of operation. */
  DB_RF_CDMA,                /**< RF is in the CDMA mode of operation. */
  DB_RF_PCN,                 /**< RF is in the Personal Communication Network 
                                  mode of operation. */
  DB_RF_900MHZ,              /**< RF is in the 900 MHz mode of operation. */
  DB_RF_SLEEP,               /**< RF is in the Sleep mode of operation. */
  DB_RF_GPS,                 /**< RF is in the GPS mode of operation. */
  DB_RF_HDR,                 /**< RF is in the High Data Rate mode of operation. */
  DB_RF_HDR_SC = DB_RF_HDR,  /**< RF is in the Single-Carrier HDR mode of operation. */
  DB_RF_HDR_MC               /**< RF is in the Multi-Carrier HDR mode of operation. */
} db_rf_mode_type;

/** DMSS system state enumeration for the DB_DMSS_STATE item. */
typedef enum {
   DB_DMSS_POWERUP_S,          /**< Power Up state (type 0). */
   DB_DMSS_ONLINE_ANALOG_S,    /**< Analog is in the Online state (type 1). */
   DB_DMSS_ONLINE_DIGITAL_S,   /**< Digital is in the Online state (type 2). */
   DB_DMSS_OFFLINE_ANALOG_S,   /**< Analog is in the Offline state (type 3). */
   DB_DMSS_OFFLINE_DIGITAL_S,  /**< Digital is in the Offline state (type 4). */
   DB_DMSS_RESET_S,            /**< Reset state (type 5). */
   DB_DMSS_POWERDOWN_S,        /**< Power Down state (type 6). */
   DB_DMSS_GPS_S               /**< GPS state (type 7). */
 } db_dmss_state_type;

/** Sleep mode enumeration for the DB_SLEEP_MODE item. */
typedef enum {
  DB_SLEEP_ENABLED,            /**< All sleep is enabled. */
  DB_SLEEP_ZZ_OFF,             /**< Idle sleep is disabled. */
  DB_SLEEP_DEEP_OFF,           /**< Deep sleep is disabled. */
  DB_SLEEP_DISABLED            /**< All sleep is disabled. */
} db_sleep_mode_type;


/** Sleep status enumeration for the DB_SLEEP_ACTIVE item. */
typedef enum {
  DB_SLEEP_ON,                 /**< Sleep is enabled. */ 
  DB_SLEEP_OFF                 /**< Sleep is disabled. */
} db_sleep_active_type;


/** Acquisition mode enumeration for the DB_ACQ_MODE item. */
typedef enum {
 DB_ACQ_FULL,                  /**< Full acquisition. */
 DB_ACQ_MINI,                  /**< Mini acquisition. */
 DB_ACQ_MICRO                  /**< Micro acquisition. */
} db_acq_mode_type;

/** Voice-as-data enumeration for the DB_VOICE_AS_DATA item. */
typedef enum {
  DB_VOICE_AS_DATA_NEVER,      /**< Never answer a voice call as a data call. */
  DB_VOICE_AS_FAX_ONCE,        /**< Answer a voice call as a fax once. */
  DB_VOICE_AS_FAX_ALWAYS,      /**< Always answer a voice call as a fax. */
  DB_VOICE_AS_MODEM_ONCE,      /**< Answer a voice call as a modem once. */
  DB_VOICE_AS_MODEM_ALWAYS     /**< Always answer a voice call as a modem. */
} db_voice_as_data_type;

/** @} */ /* end_name Database Item Types */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** @brief Union of all the types. */
typedef union {
  boolean               in_use;         /**< Indicates whether the cellular service 
                                             is available. */
  boolean               no_svc;         /**< Indicates whether a cellular service 
                                             can be found. */
  boolean               digital;        /**< Indicates whether the service is 
                                             digital. */
  boolean               conv;           /**< Indicates whether a conversation 
                                             is taking place. */
  db_roam_type          roam;           /**< Indicates the roaming status. */
  word                  channel;        /**< Channel to which the mobile is tuned. */
  byte                  code_channel;   /**< Code channel to which the mobile is tuned. */
  word                  pilot_pn_off;   /**< Pilot pseudonoise offset. */
  word                  sid;            /**< System identifier. */
  word                  locaid;         /**< Location identifier. */
  word                  nid;            /**< Network identifier. */
  byte                  lp_sec;         /**< Leap seconds since the start of the 
                                             system time. */
  byte                  ltm_off;        /**< Local time offset from the system 
                                             time. */
  boolean               daylt;          /**< Indicates whether daylight saving 
                                             time is enabled. */
  db_dmss_state_type    dmss_state;     /**< Dual-mode subscriber station system 
                                             state. */
  byte                  power;          /**< Mobile output power level. */
  word                  rssi;           /**< Received signal strength indicator. */
  word                  cdma_rssi;      /**< CDMA received signal strength 
                                             indicator level. */
  word                  battery_level;  /**< Battery level indicator. */
  word                  temperature;    /**< Current temperature level. */
  db_rf_mode_type       rf_mode;        /**< Radio Frequency mode (analog, CDMA, PCN). */
  boolean               hold_orig;      /**< Indicates whether incoming origination 
                                             commands are being held. */
  boolean               powerup_scanning; /**< Indicates whether origination on 
                                               power up is being held. */
  boolean               on_hook;          /**< Indicates whether the subscriber 
                                               unit is on-hook. */
  boolean               vox;              /**< Indicates whether voice-operated 
                                               exchange (voice-operated switch)
                                               is enabled. */
  boolean               data_srvc_enabled; /**< Indicates whether the Data Services 
                                                task is enabled. */
  db_sleep_mode_type    sleep_mode;        /**< Indicates whether Sleep mode is 
                                                enabled. */
  db_sleep_active_type  sleep_active;      /**< Indicates whether the current 
                                                state is set to Sleep. */
  db_acq_mode_type      acq_mode;          /**< Acquisition mode. */
  boolean               voc13k_available;  /**< Indicates whether Vocoder 13K is 
                                                available. */
  byte                  bs_p_rev;          /**< Protocol revision of the base station. */
  db_voice_as_data_type voice_as_data;     /**< Indicates whether answering the voice 
                                                call as a data call is allowed. */
  boolean               gps_fact_test_mode; /**< Indicates whether GPS Factory 
                                                 Test mode is enabled. */
  boolean               dsds_status;        /**< Dual subscription status */
} db_items_value_type;

/*~ CASE DB_IN_USE            db_items_value_type.in_use */
/*~ CASE DB_NO_SVC            db_items_value_type.no_svc */
/*~ CASE DB_DIGITAL           db_items_value_type.digital */
/*~ CASE DB_CONV              db_items_value_type.conv */
/*~ CASE DB_ROAM              db_items_value_type.roam */
/*~ CASE DB_CHANNEL           db_items_value_type.channel */
/*~ CASE DB_CODE_CHANNEL      db_items_value_type.code_channel */
/*~ CASE DB_PILOT_PN_OFF      db_items_value_type.pilot_pn_off */
/*~ CASE DB_SID               db_items_value_type.sid */
/*~ CASE DB_LOCAID            db_items_value_type.locaid */
/*~ CASE DB_NID               db_items_value_type.nid */
/*~ CASE DB_LP_SEC            db_items_value_type.lp_sec */
/*~ CASE DB_LTM_OFF           db_items_value_type.ltm_off */
/*~ CASE DB_DAYLT             db_items_value_type.daylt */
/*~ CASE DB_DMSS_STATE        db_items_value_type.dmss_state */
/*~ CASE DB_POWER             db_items_value_type.power */
/*~ CASE DB_RSSI              db_items_value_type.rssi */
/*~ CASE DB_CDMA_RSSI         db_items_value_type.cdma_rssi */
/*~ CASE DB_BATTERY_LEVEL     db_items_value_type.battery_level */
/*~ CASE DB_TEMPERATURE       db_items_value_type.temperature */
/*~ CASE DB_RF_MODE           db_items_value_type.rf_mode */
/*~ CASE DB_HOLD_ORIG         db_items_value_type.hold_orig */
/*~ CASE DB_POWERUP_SCANNING  db_items_value_type.powerup_scanning */
/*~ CASE DB_ON_HOOK           db_items_value_type.on_hook */
/*~ CASE DB_VOX               db_items_value_type.vox */
/*~ CASE DB_DATA_SRVC_STATE   db_items_value_type.data_srvc_enabled */
/*~ CASE DB_SLEEP_MODE        db_items_value_type.sleep_mode */
/*~ CASE DB_SLEEP_ACTIVE      db_items_value_type.sleep_active */
/*~ CASE DB_ACQ_MODE          db_items_value_type.acq_mode */
/*~ CASE DB_13K_VOC_AVAILABLE db_items_value_type.voc13k_available */
/*~ CASE DB_BS_P_REV          db_items_value_type.bs_p_rev */
/*~ CASE DB_VOICE_AS_DATA     db_items_value_type.voice_as_data */
/*~ CASE DB_GPS_FACT_TEST     db_items_value_type.gps_fact_test_mode */
/*~ CASE DB_DSDS_STATUS       db_items_value_type.dsds_status */

/** @} */ /* end_addtogroup db_interface */
/** @addtogroup db_interface 
@{ */ 

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */

/*===========================================================================*/
/* db_put */
/**
  Changes the value of a specified item. The tasks that need to know when
  a value has changed are notified.

  @param[in] item        Item to be changed.
  @param[in] value_ptr   Pointer to the buffer that holds the value.
    
  @return
  None.

  @dependencies
  None.
*/
extern void db_put
(
  db_items_type        item,
  db_items_value_type  *value_ptr
);

/*~ FUNCTION db_put */
/*~ PARAM value_ptr POINTER DISC item */

/* <EJECT> */
/*===========================================================================*/
/* db_get */
/**
  Retrieves the value of an item from the database.

  @param[in] item        Item from which to get the value.
  @param[in] value_ptr   Pointer to the buffer that holds the value.
    
  @return
  None.

  @dependencies
  None.
*/
extern void db_get
(
  db_items_type         item,
  db_items_value_type  *value_ptr
);

/*~ FUNCTION db_get */
/*~ PARAM OUT value_ptr POINTER DISC item */

/* <EJECT> */
/*===========================================================================*/
/* db_init */
/**
  Initializes the database to default values. No notifications are made when 
  setting the default values.
    
  @return
  None. All values of the database are initialized to a known default.
    
  @dependencies
  None.
*/
extern void db_init
(
  void
);

/** @} */ /* end_addtogroup db_interface */

#endif
