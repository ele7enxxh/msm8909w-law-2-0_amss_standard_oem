
#ifndef SYS_H
#define SYS_H

/*============================================================================

              S Y S T E M   H E A D E R   F I L E
 
============================================================================*/ 

/**
  @file sys.h

  Definitions that are shared between the call manager, its clients,
  and the protocol stacks.
*/
/* 
  Organization of the file:

  The file is divided into multiple sections. Each section contains
  type definitions for a particular entity. The following entities
  are defined in this file:

  - Service Indicators
  - System Identifiers
  - Network Lists
  - Mobility Management Information
  - Menu Selection
  - BCMCS

  Each section is further subdivided to represent different types of
  information. The types of information include:

  - #defines
  - enumerated types
  - data types
  - functions

  @note
  Please do not change the order of the enumerations.  If a new
  enumeration value is needed, add to the end of the enumerated
  type.  Other code may be dependent on the existing order.
*/
/*===========================================================================
Copyright (c) 2002-2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/sys.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */
#include "time_svc.h"
#include "cm_gw.h"


/*------------------------------------------------------------------------------
   General macros
------------------------------------------------------------------------------*/

/** @addtogroup GM General Macros
    @{ */

/** Maximum size of the WLAN profile. This should be updated
    if the data structure of net_profile_s_type is modified.
*/
#define SYS_WLAN_PROFILE_MAX_SIZE  155

/** Macro to convert a bit number to a bitmask.
*/
#define SYS_BM( val )         (1<<(int)(val))

/** Macro to convert a 64-bit bit number to a bitmask.
*/
#define SYS_BM_64BIT( val )   ((uint64)1<<(int)(val))

/** Macro to convert a 32-bit bit number to a bitmask.
*/
#define SYS_BM_32BIT( val )   (sys_tech_mask_type)(1<<(int)(val))

/** @} */ /* end_addtogroup GM General Macros */

/** @addtogroup DDTM DDTM-Related Definitions
    @{ */

/** Maximum SO list size.
*/
#define SYS_DDTM_MAX_SO_LIST_SIZE 10

/** Maximum allowed SO list size.
*/
#define SYS_DDTM_MAX_ALLOW_SO_LIST_SIZE 37

/** Service capability type provided by the protocol stack.
*/
typedef uint16 sys_srv_opt_type;

/** @} */ /* end_addtogroup DDTM */

/* If there is a new band class, append to the end of list.
*/

/** @addtogroup DDTM
    @{ */

/** System band classes and band classes' sub-bands.
*/
typedef enum {
  SYS_SBAND_BC0_A = 0,
    /**< Band Class 0, A-System. */

  SYS_SBAND_BC0_B = 1,
    /**< Band Class 0, B-System. */

    /* Band Class 0 AB , GSM 850 Band? */

  SYS_SBAND_BC1 = 2,
    /**< Band Class 1, all blocks. */

  SYS_SBAND_BC2 = 3,
    /**< Band Class 2 place holder. */

  SYS_SBAND_BC3 = 4,
    /**< Band Class 3, A-System. */

  SYS_SBAND_BC4 = 5,
    /**< Band Class 4, all blocks. */

  SYS_SBAND_BC5 = 6,
    /**< Band Class 5, all blocks. */

  SYS_SBAND_GSM_DCS_1800 = 7,
    /**< GSM Digital Cellular Standard (DCS) band. */

  SYS_SBAND_GSM_EGSM_900 = 8,
    /**< GSM Extended GSM (E-GSM) band. */

  SYS_SBAND_GSM_PGSM_900 = 9,
    /**< GSM Primary GSM (P-GSM) band. */

  SYS_SBAND_BC6 = 10,
    /**< Band Class 6. */

  SYS_SBAND_BC7 = 11,
    /**< Band Class 7. */

  SYS_SBAND_BC8 = 12,
    /**< Band Class 8. */

  SYS_SBAND_BC9 = 13,
    /**< Band Class 9. */

  SYS_SBAND_BC10 = 14,
    /**< Band Class 10. */

  SYS_SBAND_BC11 = 15,
    /**< Band Class 11. */

  SYS_SBAND_GSM_450 = 16,
    /**< GSM 450 band. */

  SYS_SBAND_GSM_480 = 17,
    /**< GSM 480 band. */

  SYS_SBAND_GSM_750 = 18,
    /**< GSM 750 band. */

  SYS_SBAND_GSM_850 = 19,
    /**< GSM 850 band. */

  SYS_SBAND_GSM_RGSM_900 = 20,
    /**< GSM Railways GSM Band. */

  SYS_SBAND_GSM_PCS_1900 = 21,
    /**< GSM PCS band. */

  SYS_SBAND_WCDMA_I_IMT_2000 = 22,
    /**< WCDMA Europe, Japan, and China IMT 2100 band. */

  SYS_SBAND_WCDMA_II_PCS_1900 = 23,
    /**< WCDMA US PCS 1900 band. */

  SYS_SBAND_WCDMA_III_1700 = 24,
    /**< WCDMA Europe and China DCS 1800 band. */

  SYS_SBAND_WCDMA_IV_1700 = 25,
    /**< WCDMA US 1700 band. */

  SYS_SBAND_WCDMA_V_850 = 26,
    /**< WCDMA US850 band. */

  SYS_SBAND_WCDMA_VI_800 = 27,
    /**< WCDMA Japan 800 band. */

  SYS_SBAND_BC12 = 28,
    /**< Band Class 12. */

  SYS_SBAND_BC14 = 29,
   /**< Band Class 14. */

  SYS_SBAND_RESERVED_2 = 30,
   /**< Reserved 2. */

  SYS_SBAND_BC15 = 31,
   /**< Band Class 15. */

  SYS_SBAND_WLAN_US_2400 = 32,
    /**< WLAN US 2400 band. */

  SYS_SBAND_WLAN_EUROPE_2400 = 33,
    /**< WLAN ETSI 2400 band. */

  SYS_SBAND_WLAN_FRANCE_2400 = 34,
    /**< WLAN France 2400 band. */

  SYS_SBAND_WLAN_SPAIN_2400 = 35,
    /**< WLAN Spain 2400 band. */

  SYS_SBAND_WLAN_JAPAN_2400 = 36,
    /**< WLAN Japan 2400 band. */

  SYS_SBAND_WLAN_US_5000 = 37,
    /**< WLAN US 2400 band. */

  SYS_SBAND_WLAN_EUROPE_5000 = 38,
    /**< WLAN Europe 5000 band. */

  SYS_SBAND_WLAN_FRANCE_5000 = 39,
    /**< WLAN France 5000 band. */

  SYS_SBAND_WLAN_SPAIN_5000 = 40,
    /**< WLAN Spain 5000 band. */

  SYS_SBAND_WLAN_JAPAN_5000 = 41,
    /**< WLAN Japan 5000 band. */

  SYS_SBAND_WCDMA_VII_2600 = 48,
    /**< WCDMA Europe 2600 band. */

  SYS_SBAND_WCDMA_VIII_900 = 49,
    /**< WCDMA Europe and Japan 900 band. */

  SYS_SBAND_WCDMA_IX_1700 = 50,
    /**< WCDMA Japan 1700 band. */

  /* 51-55 reserved for WLAN */
  SYS_SBAND_BC16 = 56,
   /**< Band Class 16. */

  SYS_SBAND_BC17 = 57,
   /**< Band Class 17. */

  SYS_SBAND_BC18 = 58,
   /**< Band Class 18. */

  SYS_SBAND_BC19 = 59,
   /**< Band Class 19. */

  SYS_SBAND_WCDMA_XIX_850 = 60,
    /**< WCDMA Japan 850 band. */

  SYS_SBAND_WCDMA_XI_1500 = 61,
    /**< WCDMA 1500 band.  */

  SYS_SBAND_PERSISTENT = 62,
    /**< Persistent value from NV. */
  
  /** @cond
  */
  SYS_SBAND_MAX = 63  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_sband_e_type;
/*~ SENTINEL sys_sband_e_type.SYS_SBAND_MAX */


/** DDTM actions to enhance HDR data throughput.
*/
typedef enum
{
  SYS_DDTM_ACT_SUPPRESS_L2ACK   = 0,
    /**< Do not send L2 ACK on 1X. */

  SYS_DDTM_ACT_SUPPRESS_REG     = 1,
    /**< Suppress 1X registrations. */

  SYS_DDTM_ACT_IGN_SO_PAGES     = 2,
    /**< Ignore 1X pages with specified service options. */

  SYS_DDTM_ACT_SUPPRESS_MO_DBM  = 3,
    /**< Block MO SMS and DBM */  

  SYS_DDTM_ACT_ALLOW_SO_PAGES = 4,
    /**< Allow only pages with specified service options. */

  /** @cond
  */
  SYS_DDTM_ACT_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_ddtm_act_e_type;
/*~ SENTINEL sys_ddtm_act_e_type.SYS_DDTM_ACT_MAX */

/** DDTM action mask; converts the bit position of
    sys_ddtm_act_e_type to a bitmask.
*/
typedef enum {

  SYS_DDTM_ACT_MASK_EMPTY           = 0,
    /**< Empty action mask. */

  SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK  = SYS_BM(SYS_DDTM_ACT_SUPPRESS_L2ACK),
    /**< Mask for suppressing L2 acknowledgments in 1X. */

  SYS_DDTM_ACT_MASK_SUPPRESS_REG    = SYS_BM(SYS_DDTM_ACT_SUPPRESS_REG),
    /**< Mask for suppressing 1X registrations. */

  SYS_DDTM_ACT_MASK_IGN_SO_PAGES    = SYS_BM(SYS_DDTM_ACT_IGN_SO_PAGES),
    /**< Mask for ignoring pages with specified service options. */

  SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM = SYS_BM(SYS_DDTM_ACT_SUPPRESS_MO_DBM),
    /**< Mask for blocking MO SMS and DBM. */

  SYS_DDTM_ACT_MASK_ALLOW_SO_PAGES = SYS_BM(SYS_DDTM_ACT_ALLOW_SO_PAGES),
    /**< Mask for allowing pages with specified service options. */

  SYS_DDTM_ACT_MASK_ALL =  0x3fff,
    /**< Mask for enabling all DDTM actions. */

  SYS_DDTM_ACT_MASK_NO_CHANGE,
    /**< Reserved value for not changing current DDTM action settings. */

  /** @cond
  */
  SYS_DDTM_ACT_MASK_MAX   /* For internal use. */
  /** @endcond
  */

} sys_ddtm_act_mask_e_type;
/*~ SENTINEL sys_ddtm_act_mask_e_type.SYS_DDTM_ACT_MASK_MAX */

/** @} */ /* end_addtogroup DDTM */

/** @addtogroup DDTM
    @{ */

/*------------------------------------------------------------------------------
   Multiple EPS bearer Contexts Defines
------------------------------------------------------------------------------*/

/** Total number of slots for Evolved Packet System (EPS) bearer contexts.
*/
#define SYS_MAX_EPS_BEARERS_CONTEXTS          8
/* (1 for default and 7 for dedicated) */

/** @} */ /* end_addtogroup DDTM */

/*------------------------------------------------------------------------------
   Multiple PDP Contexts Defines
------------------------------------------------------------------------------*/

/** @addtogroup MPCD Multiple PDP Context Definitions
    @{ */

/** Maximum number of primary PDP contexts.
*/
#define SYS_MAX_PRIMARY_PDP_CONTEXTS          4

/** Minimum number of primary PDP contexts.
*/
#define SYS_MIN_PRIMARY_PDP_CONTEXTS          1

/** Maximum number of secondary PDP contexts per primary PDP context.
*/
#define SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY   2

/** Minimum number of secondary PDP contexts per primary PDP context.
*/
#define SYS_MIN_SEC_PDP_CONTEXT_PER_PRIMARY   0

/** Maximum number of PDP contexts.
*/
#define SYS_MAX_PDP_CONTEXTS                                                 \
  ( SYS_MAX_PRIMARY_PDP_CONTEXTS +                                           \
    ( SYS_MAX_PRIMARY_PDP_CONTEXTS * SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY ))

/** Maximum size of the SSID.
*/
#define SYS_WLAN_SSID_MAX_SIZE         (int)(32)

/** Minimum size of the SSID.
*/
#define SYS_WLAN_SSID_MIN_SIZE         (int)(1)

/** Limit for the number of spare bits in the end octet of the network short
    name.
*/
#define SYS_MAX_NW_NAME_SPARE_BITS    7

/** @} */ /* end_addtogroup MPCD Multiple PDP Context Definitions */

/*===============================================================================

                     S E R V I C E   I N D I C A T O R S

==============================================================================*/

/*------------------------------------------------------------------------------
   Service Indicators Defines
------------------------------------------------------------------------------*/

/** @addtogroup SI Service Indicators
    @{ */

/** Set window search size.
*/
typedef byte                      sys_srch_win_type;

/** Base station latitude.
*/
typedef int32                     sys_base_lat_type;

/** Base station longitude.
*/
typedef int32                     sys_base_long_type;

/** Base station ID.
*/
typedef word                     sys_base_id_type;

/** WLAN BSS ID.
*/
typedef uint64                   sys_wlan_bssid_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Service Indicators Enums
------------------------------------------------------------------------------*/
/**
Phone measurement modes; currently used only for HDR CP.
*/
typedef enum
{
  /** @cond
  */
  SYS_MEAS_MODE_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_MEAS_MODE_HDR_OFF = 0,
    /**< HDR enables network access. */

  SYS_MEAS_MODE_HDR_ON = 1,
    /**< HDR disables network access. */

  /** @cond
  */
  SYS_MEAS_MODE_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_meas_mode_e_type;
/*~ SENTINEL sys_meas_mode_e_type.SYS_MEAS_MODE_MAX */


/* For compatibility with the QXDM database, please do not change the following
   fixed assigned values. If new values are added, please also update the QXDM
   database.
*/
/** Phone operating modes.
*/
typedef enum
{
  /** @cond
  */
  SYS_OPRT_MODE_NONE = -1,   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_OPRT_MODE_PWROFF = 0,
    /**< Phone is powering off. */

  SYS_OPRT_MODE_FTM = 1,
    /**< Phone is in factory test mode. */

  SYS_OPRT_MODE_OFFLINE = 2,
    /**< Phone is offline. */

  SYS_OPRT_MODE_OFFLINE_AMPS = 3,
    /**< Phone is offline analog. */

  SYS_OPRT_MODE_OFFLINE_CDMA = 4,
    /**< Phone is offline CDMA. */

  SYS_OPRT_MODE_ONLINE = 5,
    /**< Phone is online. */

  SYS_OPRT_MODE_LPM = 6,
    /**< Phone is in Low Power Mode (LPM) . */

  SYS_OPRT_MODE_RESET = 7,
    /**< Phone is resetting (i.e., power-cycling). */

  SYS_OPRT_MODE_NET_TEST_GW = 8,
    /**< Phone is conducting network test for GSM/WCDMA. This mode can
         <i>not</i> be set by clients. It can only be set by the lower
         layers of the stack. */

  SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM = 9,
    /**< This mode is only set when there is an offline request during
         power-up. This mode cannot be set by clients. It can
         only be set by the task mode controller. */

  SYS_OPRT_MODE_PSEUDO_ONLINE = 10,
    /**< Phone is pseudo online, transmit disabled. */

  SYS_OPRT_MODE_RESET_MODEM = 11,
    /**< Phone is resetting the modem processor. */

  /** @cond
  */
  SYS_OPRT_MODE_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_oprt_mode_e_type;
/*~ SENTINEL sys_oprt_mode_e_type.SYS_OPRT_MODE_MAX */

/* For compatibility with the QXDM database, please do not change the following
   fixed assigned values. If new values are added, please also update the QXDM
   database.
*/
/** System modes.
*/
typedef enum
{
  /** @cond
  */
  SYS_SYS_MODE_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_SYS_MODE_NO_SRV=0,
    /**< No service; NV_MODE_INACTIVE. */

  SYS_SYS_MODE_AMPS=1,
    /**< Analog Mobile Phone System (AMPS) mode. */

  SYS_SYS_MODE_CDMA=2,
    /**< CDMA mode. */

  SYS_SYS_MODE_GSM=3,
    /**< GSM mode. */

  SYS_SYS_MODE_HDR=4,
    /**< HDR mode. */

  SYS_SYS_MODE_WCDMA=5,
    /**< WCDMA mode. */

  SYS_SYS_MODE_GPS=6,
    /**< GPS mode. */

  SYS_SYS_MODE_GW=7,
    /**< GSM and WCDMA mode. */

  SYS_SYS_MODE_WLAN=8,
    /**< WLAN mode. */

  SYS_SYS_MODE_LTE=9,
    /**< LTE mode. */

  SYS_SYS_MODE_GWL=10,
    /**< GSM, WCDMA, and LTE mode. */

  SYS_SYS_MODE_TDS=11,
  /**< TDS mode. */

  /** @cond
  */
  SYS_SYS_MODE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_sys_mode_e_type;
/*~ SENTINEL sys_sys_mode_e_type.SYS_SYS_MODE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Macro for creating a bitmask by shifting '1' left by a specified
    number indicated by 'val'. Useful for defining bitmask enum values.
*/
#define BM( val ) ( 1<< ((int)(val)) )

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** 
  Bitmasks of system modes. A combination of system modes can be created
  by using the ADD_SYSMODE macro.
*/
typedef enum
{

  SYS_SYS_MODE_MASK_NONE        = 0,
    /**< Acquire no system. */

  SYS_SYS_MODE_MASK_NO_SRV      = BM( SYS_SYS_MODE_NO_SRV ),
    /**< No service. */

  SYS_SYS_MODE_MASK_CDMA        = BM( SYS_SYS_MODE_CDMA ),
    /**< CDMA mode systems only. */

  SYS_SYS_MODE_MASK_HDR         = BM( SYS_SYS_MODE_HDR ),
    /**< Acquire HDR mode systems only. */

  SYS_SYS_MODE_MASK_GSM         = BM( SYS_SYS_MODE_GSM ),
    /**< Acquire GSM systems only. */

  SYS_SYS_MODE_MASK_WCDMA       = BM( SYS_SYS_MODE_WCDMA ),
    /**< Acquire WCDMA systems only. */

  SYS_SYS_MODE_MASK_WLAN        = BM( SYS_SYS_MODE_WLAN ),
    /**< Acquire WLAN systems only. */

  SYS_SYS_MODE_MASK_LTE         = BM( SYS_SYS_MODE_LTE ),
    /**< Acquire LTE systems only. */

  SYS_SYS_MODE_MASK_TDS         = BM( SYS_SYS_MODE_TDS ),
    /**< Acquire TDS systems only. */

  /** @cond
  */
  SYS_SYS_MODE_MASK_MAX
  /** @endcond
  */
} sys_sys_mode_mask_e_type;
/*~ SENTINEL sys_sys_mode_mask_e_type.SYS_SYS_MODE_MASK_MAX */
#ifndef FEATURE_GNSS_SA
/**     
  System band classes.
*/
typedef enum
{
  /** @cond
  */
  SYS_BAND_CLASS_NONE        = -1,
    /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_BAND_BC0               = 0,
    /**< Band Class 0: U.S. Cellular band (800 MHz). */

  SYS_BAND_BC1               = 1,
    /**< Band Class 1: U.S. */

  SYS_BAND_BC3               = 3,
    /**< Band Class 3: Japanese Cellular band (800 MHz). */

  SYS_BAND_BC4               = 4,
    /**< Band Class 4: Korean PCS band (1800 MHz). */

  SYS_BAND_BC5               = 5,
    /**< Band Class 5 (450 MHz). */

  SYS_BAND_BC6               = 6,
    /**< Band Class 6 (2 GHz). */

  SYS_BAND_BC7               = 7,
    /**< Band Class 7 (Upper 700 MHz). */

  SYS_BAND_BC8               = 8,
    /**< Band Class 8 (1800 MHz). */

  SYS_BAND_BC9               = 9,
    /**< Band Class 9 (900 MHz). */

  SYS_BAND_BC10              = 10,
    /**< Band Class 10 (Second 800 MHz). */

  SYS_BAND_BC11              = 11,
    /**< Band Class 11: European PAMR Band (400 MHz). */

  SYS_BAND_BC12              = 12,
  /**< Band Class 12: PAMR Band (800 MHz). */

  SYS_BAND_BC13              = 13,
  /**< Band Class 13: currently undefined. */

  SYS_BAND_BC14              = 14,
  /**< Band Class 14 (U.S. PCS 1.9 GHz Band). */

  SYS_BAND_BC15              = 15,
  /**< Band Class 15 (1700-2100 MHz -AWS). */

  SYS_BAND_BC16              = 16,
  /**< Band Class 16 (U.S. 2.5 GHz). */

  SYS_BAND_BC17              = 17,
  /**< Band Class 17 (U.S. 2.5 GHz Forward Link only band). */

  SYS_BAND_BC18              = 18,
  /**< Band Class 18 (700 MHz Public Safety Broadband). */

  SYS_BAND_BC19              = 19,
  /**< Band Class 19 (Lower 700 MHz band).*/

  SYS_BAND_BC_MAX            = 20,
  /**< Upper boundary for CDMA band classes.  */

  /* Reserved 20-39 for CDMA band classes. */

  SYS_BAND_GSM_450           = 40,
    /**< GSM 450 band (450 MHz). */

  SYS_BAND_GSM_480           = 41,
    /**< GSM 480 band (480 MHz). */

  SYS_BAND_GSM_750           = 42,
    /**< GSM 750 band (750 MHz). */

  SYS_BAND_GSM_850           = 43,
    /**< GSM 850 band (850 MHz). */

  SYS_BAND_GSM_EGSM_900      = 44,
    /**< GSM Extended GSM (E-GSM) 900 band (900 MHz). */

  SYS_BAND_GSM_PGSM_900      = 45,
    /**< GSM Primary GSM (P-GSM) 900 band (900 MHz). */

  SYS_BAND_GSM_RGSM_900      = 46,
    /**< GSM Railways GSM (R-GSM) 900 band (900 MHz). */

  SYS_BAND_GSM_DCS_1800      = 47,
    /**< GSM DCS band (1800 MHz). */

  SYS_BAND_GSM_PCS_1900      = 48,
    /**< GSM PCS band (1900 MHz). */

  /* Reserved 49-79 for GSM band classes. */

  SYS_BAND_WCDMA_I_IMT_2000  = 80,
    /**< WCDMA Europe, Japan, and China IMT 2100 band. */

  SYS_BAND_WCDMA_II_PCS_1900 = 81,
    /**< WCDMA U.S. PCS 1900 band. */

  SYS_BAND_WCDMA_III_1700    = 82,
    /**< WCDMA Europe and China DCS 1800 band. */

  SYS_BAND_WCDMA_IV_1700     = 83,
    /**< WCDMA U.S. 1700 band. */

  SYS_BAND_WCDMA_V_850       = 84,
    /**< WCDMA U.S. 850 band. */

  SYS_BAND_WCDMA_VI_800      = 85,
    /**< WCDMA Japan 800 band. */

  SYS_BAND_WCDMA_VII_2600    = 86,
    /**< WCDMA Europe 2600 band. */

  SYS_BAND_WCDMA_VIII_900    = 87,
    /**< WCDMA Europe and China 900 band. */

  SYS_BAND_WCDMA_IX_1700     = 88,
    /**< WCDMA Japan 1700 band. */

  /* Reserved 89 for WCDMA BC10-1700 band classes. */

  SYS_BAND_WCDMA_XI_1500     = 90,
    /**< WCDMA 1500 band. */

  SYS_BAND_WCDMA_XIX_850     = 91,
    /**< WCDMA Japan 850 band. */

  /* Reserved 85-109 for WCDMA band classes. */

  SYS_BAND_WLAN_US_2400    = 110,
    /**< WLAN U.S. 2400 MHz band. */

  SYS_BAND_WLAN_JAPAN_2400 = 111,
    /**< WLAN Japan 2400 MHz band. */

  SYS_BAND_WLAN_EUROPE_2400 = 112,
    /**< WLAN Europe 2400 MHz band. */

  SYS_BAND_WLAN_FRANCE_2400 = 113,
    /**< WLAN France 2400 MHz band. */

  SYS_BAND_WLAN_SPAIN_2400 = 114,
    /**< WLAN Spain 2400 MHz band. */

  SYS_BAND_WLAN_US_5000    = 115,
    /**< WLAN U.S. 5000 MHz band. */

  SYS_BAND_WLAN_JAPAN_5000 = 116,
    /**< WLAN Japan 5000 MHz band. */

  SYS_BAND_WLAN_EUROPE_5000 = 117,
    /**< WLAN Europe 5000 MHz band. */

  SYS_BAND_WLAN_FRANCE_5000 = 118,
    /**< WLAN France 5000 MHz band. */

  SYS_BAND_WLAN_SPAIN_5000 = 119,
    /**< WLAN Spain 5000 MHz band. */

  /* LTE bands*/  
    
  SYS_BAND_LTE_EUTRAN_BAND1 = 120,
  /**< UL:1920-1980; DL:2110-2170. */

  SYS_BAND_LTE_EUTRAN_BAND2 = 121,
  /**< UL:1850-1910; DL:1930-1990. */

  SYS_BAND_LTE_EUTRAN_BAND3 = 122,
  /**< UL:1710-1785; DL:1805-1880. */

  SYS_BAND_LTE_EUTRAN_BAND4 = 123,
  /**< UL:1710-1755; DL:2110-2155. */

  SYS_BAND_LTE_EUTRAN_BAND5 = 124,
  /**< UL: 824-849; DL: 869- 894. */

  SYS_BAND_LTE_EUTRAN_BAND6 = 125,
  /**< UL: 830-840; DL: 875-885. */

  SYS_BAND_LTE_EUTRAN_BAND7 = 126,
  /**< UL:2500-2570; DL:2620-2690. */

  SYS_BAND_LTE_EUTRAN_BAND8 = 127,
  /**< UL: 880-915; DL: 925-960. */

  SYS_BAND_LTE_EUTRAN_BAND9 = 128,
  /**< UL:1749.9-1784.9; DL:1844.9-1879.9. */

  SYS_BAND_LTE_EUTRAN_BAND10 = 129,
  /**< UL:1710-1770; DL:2110-2170. */

  SYS_BAND_LTE_EUTRAN_BAND11 = 130,
  /**< UL:1427.9-1452.9; DL:1475.9-1500.9. */

  SYS_BAND_LTE_EUTRAN_BAND12 = 131,
  /**< UL:698-716; DL:728-746. */

  SYS_BAND_LTE_EUTRAN_BAND13 = 132,
  /**< UL: 777-787; DL: 746-756. */

  SYS_BAND_LTE_EUTRAN_BAND14 = 133,
  /**< UL: 788-798; DL: 758-768. */

  /* Reserved for BAND 15, 16 */

  SYS_BAND_LTE_EUTRAN_BAND17 = 136,
  /**< UL: 704-716; DL: 734-746. */

  SYS_BAND_LTE_EUTRAN_BAND18 = 137,
  /**< UL: 815-830; DL: 860-875. */

  SYS_BAND_LTE_EUTRAN_BAND19 = 138,
  /**< UL: 830-845; DL: 875-890. */

  SYS_BAND_LTE_EUTRAN_BAND20 = 139,
  /**< UL: 832-862; DL: 791-821. */

  SYS_BAND_LTE_EUTRAN_BAND21 = 140,
  /**< UL: 1447.9-1462.9; DL: 1495.9-1510.9. */

  SYS_BAND_LTE_EUTRAN_BAND24 = 143,
  /**< UL: 1626.5-1660.5; DL: 1525 -1559. */

  SYS_BAND_LTE_EUTRAN_BAND25 = 144,
  /**< UL: 1850-1915; DL: 1930 -1995 . */
 
  SYS_BAND_LTE_EUTRAN_BAND33 = 152,
  /**< UL: 1900-1920; DL: 1900-1920. */

  SYS_BAND_LTE_EUTRAN_BAND34 = 153,
  /**< UL: 2010-2025; DL: 2010-2025. */

  SYS_BAND_LTE_EUTRAN_BAND35 = 154,
  /**< UL: 1850-1910; DL: 1850-1910. */

  SYS_BAND_LTE_EUTRAN_BAND36 = 155,
  /**< UL: 1930-1990; DL: 1930-1990. */

  SYS_BAND_LTE_EUTRAN_BAND37 = 156,
  /**< UL: 1910-1930; DL: 1910-1930. */

  SYS_BAND_LTE_EUTRAN_BAND38 = 157,
  /**< UL: 2570-2620; DL: 2570-2620. */

  SYS_BAND_LTE_EUTRAN_BAND39 = 158,
  /**< UL: 1880-1920; DL: 1880-1920. */

  SYS_BAND_LTE_EUTRAN_BAND40 = 159,
  /**< UL: 2300-2400; DL: 2300-2400. */

  SYS_BAND_LTE_EUTRAN_BAND41 = 160,
  /**< UL: 2496-2690; DL: 2496-2690. */

  SYS_BAND_LTE_EUTRAN_BAND42 = 161,
  /**< UL: 3400-3600; DL: 3400-3600. */

  SYS_BAND_LTE_EUTRAN_BAND43 = 162,
  /**< UL: 3600-3800; DL: 3600-3800. */

      /* TD-SCDMA bands */    
  SYS_BAND_TDS_BANDA = 163,
  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

  SYS_BAND_TDS_BANDB = 164,
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

  SYS_BAND_TDS_BANDC = 165,
  /**< TDS Band C 1910-1930 MHz */

  SYS_BAND_TDS_BANDD = 166,
  /**< TDS Band D 2570-2620 MHz */

  SYS_BAND_TDS_BANDE = 167,
  /**< TDS Band E 2300-2400 MHz */

  SYS_BAND_TDS_BANDF = 168,
  /**< TDS Band F 1880-1920 MHz */

  /** @cond
  */
  SYS_BAND_CLASS_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_band_class_e_type;
/*~ SENTINEL sys_band_class_e_type.SYS_BAND_CLASS_MAX */
#else
	/** 	
	  System band classes.
	*/
	typedef enum
	{
	  /** @cond
	  */
	  SYS_BAND_CLASS_NONE		 = -1,
		/* FOR INTERNAL USE OF CM ONLY! */
	  /** @endcond
	  */
	
	  SYS_BAND_BC0				 = 0,
		/**< Band Class 0: U.S. Cellular band (800 MHz). */
	
	  SYS_BAND_BC1				 = 1,
		/**< Band Class 1: U.S. */
	
	  SYS_BAND_BC3				 = 3,
		/**< Band Class 3: Japanese Cellular band (800 MHz). */
	
	  SYS_BAND_BC4				 = 4,
		/**< Band Class 4: Korean PCS band (1800 MHz). */
	
	  SYS_BAND_BC5				 = 5,
		/**< Band Class 5 (450 MHz). */
	
	  SYS_BAND_BC6				 = 6,
		/**< Band Class 6 (2 GHz). */
	
	  SYS_BAND_BC7				 = 7,
		/**< Band Class 7 (Upper 700 MHz). */
	
	  SYS_BAND_BC8				 = 8,
		/**< Band Class 8 (1800 MHz). */
	
	  SYS_BAND_BC9				 = 9,
		/**< Band Class 9 (900 MHz). */
	
	  SYS_BAND_BC10 			 = 10,
		/**< Band Class 10 (Second 800 MHz). */
	
	  SYS_BAND_BC11 			 = 11,
		/**< Band Class 11: European PAMR Band (400 MHz). */
	
	  SYS_BAND_BC12 			 = 12,
	  /**< Band Class 12: PAMR Band (800 MHz). */
	
	  SYS_BAND_BC13 			 = 13,
	  /**< Band Class 13: currently undefined. */
	
	  SYS_BAND_BC14 			 = 14,
	  /**< Band Class 14 (U.S. PCS 1.9 GHz Band). */
	
	  SYS_BAND_BC15 			 = 15,
	  /**< Band Class 15 (1700-2100 MHz -AWS). */
	
	  SYS_BAND_BC16 			 = 16,
	  /**< Band Class 16 (U.S. 2.5 GHz). */
	
	  SYS_BAND_BC17 			 = 17,
	  /**< Band Class 17 (U.S. 2.5 GHz Forward Link only band). */
	
	  SYS_BAND_BC18 			 = 18,
	  /**< Band Class 18 (700 MHz Public Safety Broadband). */
	
	  SYS_BAND_BC19 			 = 19,
	  /**< Band Class 19 (Lower 700 MHz band).*/
	
	  SYS_BAND_BC_MAX			 = 20,
	  /**< Upper boundary for CDMA band classes.  */
	
	  /* Reserved 20-39 for CDMA band classes. */
	
	  SYS_BAND_GSM_450			 = 40,
		/**< GSM 450 band (450 MHz). */
	
	  SYS_BAND_GSM_480			 = 41,
		/**< GSM 480 band (480 MHz). */
	
	  SYS_BAND_GSM_750			 = 42,
		/**< GSM 750 band (750 MHz). */
	
	  SYS_BAND_GSM_850			 = 43,
		/**< GSM 850 band (850 MHz). */
	
	  SYS_BAND_GSM_EGSM_900 	 = 44,
		/**< GSM Extended GSM (E-GSM) 900 band (900 MHz). */
	
	  SYS_BAND_GSM_PGSM_900 	 = 45,
		/**< GSM Primary GSM (P-GSM) 900 band (900 MHz). */
	
	  SYS_BAND_GSM_RGSM_900 	 = 46,
		/**< GSM Railways GSM (R-GSM) 900 band (900 MHz). */
	
	  SYS_BAND_GSM_DCS_1800 	 = 47,
		/**< GSM DCS band (1800 MHz). */
	
	  SYS_BAND_GSM_PCS_1900 	 = 48,
		/**< GSM PCS band (1900 MHz). */
	
	  /* Reserved 49-79 for GSM band classes. */
	
	  SYS_BAND_WCDMA_I_IMT_2000  = 80,
		/**< WCDMA Europe, Japan, and China IMT 2100 band. */
	
	  SYS_BAND_WCDMA_II_PCS_1900 = 81,
		/**< WCDMA U.S. PCS 1900 band. */
	
	  SYS_BAND_WCDMA_III_1700	 = 82,
		/**< WCDMA Europe and China DCS 1800 band. */
	
	  SYS_BAND_WCDMA_IV_1700	 = 83,
		/**< WCDMA U.S. 1700 band. */
	
	  SYS_BAND_WCDMA_V_850		 = 84,
		/**< WCDMA U.S. 850 band. */
	
	  SYS_BAND_WCDMA_VI_800 	 = 85,
		/**< WCDMA Japan 800 band. */
	
	  SYS_BAND_WCDMA_VII_2600	 = 86,
		/**< WCDMA Europe 2600 band. */
	
	  SYS_BAND_WCDMA_VIII_900	 = 87,
		/**< WCDMA Europe and China 900 band. */
	
	  SYS_BAND_WCDMA_IX_1700	 = 88,
		/**< WCDMA Japan 1700 band. */
	
	  /* Reserved 89 for WCDMA BC10-1700 band classes. */
	
	  SYS_BAND_WCDMA_XI_1500	 = 90,
		/**< WCDMA 1500 band. */
	
	  SYS_BAND_WCDMA_XIX_850	 = 91,
		/**< WCDMA Japan 850 band. */
	
	  /* Reserved 85-109 for WCDMA band classes. */
	
	  SYS_BAND_WLAN_US_2400    = 110,
		/**< WLAN U.S. 2400 MHz band. */
	
	  SYS_BAND_WLAN_JAPAN_2400 = 111,
		/**< WLAN Japan 2400 MHz band. */
	
	  SYS_BAND_WLAN_EUROPE_2400 = 112,
		/**< WLAN Europe 2400 MHz band. */
	
	  SYS_BAND_WLAN_FRANCE_2400 = 113,
		/**< WLAN France 2400 MHz band. */
	
	  SYS_BAND_WLAN_SPAIN_2400 = 114,
		/**< WLAN Spain 2400 MHz band. */
	
	  SYS_BAND_WLAN_US_5000    = 115,
		/**< WLAN U.S. 5000 MHz band. */
	
	  SYS_BAND_WLAN_JAPAN_5000 = 116,
		/**< WLAN Japan 5000 MHz band. */
	
	  SYS_BAND_WLAN_EUROPE_5000 = 117,
		/**< WLAN Europe 5000 MHz band. */
	
	  SYS_BAND_WLAN_FRANCE_5000 = 118,
		/**< WLAN France 5000 MHz band. */
	
	  SYS_BAND_WLAN_SPAIN_5000 = 119,
		/**< WLAN Spain 5000 MHz band. */
	
	  /* LTE bands*/  
		
	  SYS_BAND_LTE_EUTRAN_BAND1 = 120,
	  /**< UL:1920-1980; DL:2110-2170. */
	
	  SYS_BAND_LTE_EUTRAN_BAND2 = 121,
	  /**< UL:1850-1910; DL:1930-1990. */
	
	  SYS_BAND_LTE_EUTRAN_BAND3 = 122,
	  /**< UL:1710-1785; DL:1805-1880. */
	
	  SYS_BAND_LTE_EUTRAN_BAND4 = 123,
	  /**< UL:1710-1755; DL:2110-2155. */
	
	  SYS_BAND_LTE_EUTRAN_BAND5 = 124,
	  /**< UL: 824-849; DL: 869- 894. */
	
	  SYS_BAND_LTE_EUTRAN_BAND6 = 125,
	  /**< UL: 830-840; DL: 875-885. */
	
	  SYS_BAND_LTE_EUTRAN_BAND7 = 126,
	  /**< UL:2500-2570; DL:2620-2690. */
	
	  SYS_BAND_LTE_EUTRAN_BAND8 = 127,
	  /**< UL: 880-915; DL: 925-960. */
	
	  SYS_BAND_LTE_EUTRAN_BAND9 = 128,
	  /**< UL:1749.9-1784.9; DL:1844.9-1879.9. */
	
	  SYS_BAND_LTE_EUTRAN_BAND10 = 129,
	  /**< UL:1710-1770; DL:2110-2170. */
	
	  SYS_BAND_LTE_EUTRAN_BAND11 = 130,
	  /**< UL:1427.9-1452.9; DL:1475.9-1500.9. */
	
	  SYS_BAND_LTE_EUTRAN_BAND12 = 131,
	  /**< UL:698-716; DL:728-746. */
	
	  SYS_BAND_LTE_EUTRAN_BAND13 = 132,
	  /**< UL: 777-787; DL: 746-756. */
	
	  SYS_BAND_LTE_EUTRAN_BAND14 = 133,
	  /**< UL: 788-798; DL: 758-768. */
	
	  /* Reserved for BAND 15, 16 */
	
	  SYS_BAND_LTE_EUTRAN_BAND17 = 136,
	  /**< UL: 704-716; DL: 734-746. */
	
	  SYS_BAND_LTE_EUTRAN_BAND18 = 137,
	  /**< UL: 815-830; DL: 860-875. */
	
	  SYS_BAND_LTE_EUTRAN_BAND19 = 138,
	  /**< UL: 830-845; DL: 875-890. */
	
	  SYS_BAND_LTE_EUTRAN_BAND20 = 139,
	  /**< UL: 832-862; DL: 791-821. */
	
	  SYS_BAND_LTE_EUTRAN_BAND21 = 140,
	  /**< UL: 1447.9-1462.9; DL: 1495.9-1510.9. */
	
	  SYS_BAND_LTE_EUTRAN_BAND23 = 142,
	  /**< UL: 2000-2020; DL: 2180-2200 */
	
	  SYS_BAND_LTE_EUTRAN_BAND24 = 143,
	  /**< UL: 1626.5-1660.5; DL: 1525 -1559. */
	
	  SYS_BAND_LTE_EUTRAN_BAND25 = 144,
	  /**< UL: 1850-1915; DL: 1930 -1995 . */
	
	  SYS_BAND_LTE_EUTRAN_BAND26 = 145,
	  /**< UL: 814-849; DL: 859 -894 . */
	
	  SYS_BAND_LTE_EUTRAN_BAND33 = 152,
	  /**< UL: 1900-1920; DL: 1900-1920. */
	
	  SYS_BAND_LTE_EUTRAN_BAND34 = 153,
	  /**< UL: 2010-2025; DL: 2010-2025. */
	
	  SYS_BAND_LTE_EUTRAN_BAND35 = 154,
	  /**< UL: 1850-1910; DL: 1850-1910. */
	
	  SYS_BAND_LTE_EUTRAN_BAND36 = 155,
	  /**< UL: 1930-1990; DL: 1930-1990. */
	
	  SYS_BAND_LTE_EUTRAN_BAND37 = 156,
	  /**< UL: 1910-1930; DL: 1910-1930. */
	
	  SYS_BAND_LTE_EUTRAN_BAND38 = 157,
	  /**< UL: 2570-2620; DL: 2570-2620. */
	
	  SYS_BAND_LTE_EUTRAN_BAND39 = 158,
	  /**< UL: 1880-1920; DL: 1880-1920. */
	
	  SYS_BAND_LTE_EUTRAN_BAND40 = 159,
	  /**< UL: 2300-2400; DL: 2300-2400. */
	
	  SYS_BAND_LTE_EUTRAN_BAND41 = 160,
	  /**< UL: 2496-2690; DL: 2496-2690. */
	
	  SYS_BAND_LTE_EUTRAN_BAND42 = 161,
	  /**< UL: 3400-3600; DL: 3400-3600. */
	
	  SYS_BAND_LTE_EUTRAN_BAND43 = 162,
	  /**< UL: 3600-3800; DL: 3600-3800. */
	
		  /* TD-SCDMA bands */	  
	  SYS_BAND_TDS_BANDA = 163,
	  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */
	
	  SYS_BAND_TDS_BANDB = 164,
	  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */
	
	  SYS_BAND_TDS_BANDC = 165,
	  /**< TDS Band C 1910-1930 MHz */
	
	  SYS_BAND_TDS_BANDD = 166,
	  /**< TDS Band D 2570-2620 MHz */
	
	  SYS_BAND_TDS_BANDE = 167,
	  /**< TDS Band E 2300-2400 MHz */
	
	  SYS_BAND_TDS_BANDF = 168,
	  /**< TDS Band F 1880-1920 MHz */
	
	  /** @cond
	  */
	  SYS_BAND_CLASS_MAX   /* FOR INTERNAL USE ONLY! */
	  /** @endcond
	  */
	} sys_band_class_e_type;
	/*~ SENTINEL sys_band_class_e_type.SYS_BAND_CLASS_MAX */
#endif
/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** CDMA channel number.
*/
typedef uint16 sys_channel_num_type;

/** Pilot PN; valid values: 0-511.
*/
typedef uint16 sys_pilot_pn_type;

/** @brief CDMA/HDR channel.
*/
typedef struct
{
#ifndef FEATURE_GNSS_SA
  sys_band_class_e_type    band;
    /**< Band class: 0, 1, etc. */
#endif
  sys_channel_num_type     chan_num;
    /**< CDMA channel number. */

} sys_channel_type;


/** @brief Pilot strength measurement for a channel.
*/
typedef struct sys_pilot_info_s
{
  sys_pilot_pn_type         pilot_pn;
    /**< Pilot PN. */

  uint16                    pilot_pn_phase;
    /**< Pilot PN phase. */

  boolean                   is_chan_incl;
    /**< Channel included. */

  sys_channel_type          channel;
    /**< Channel: only valid if is_chan_incl = TRUE. */

  uint8                     pilot_strength;
    /**< Pilot strength. */

} sys_pilot_info_s_type;

/** System band class mask.
*/
typedef uint64                sys_band_mask_type;

/** Band class bitmask data type.
*/
typedef sys_band_mask_type    sys_band_mask_e_type;
    /* Acquire WCDMA Japan 1700 systems only */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

#define SYS_BAND_MASK_EMPTY             0
     /**< No band selected. */

#define SYS_BAND_MASK_BC0_A             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC0_A))
    /**< Acquire band class 0, A-side systems only. */

#define SYS_BAND_MASK_BC0_B             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC0_B))
    /**< Acquire band class 0, B-side systems only. */

#define SYS_BAND_MASK_BC0               ((sys_band_mask_type) (SYS_BM_64BIT(SYS_SBAND_BC0_A) | \
                                        SYS_BM_64BIT(SYS_SBAND_BC0_B)))
    /**< Acquire band class 0 systems only. */

#define SYS_BAND_MASK_BC1               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC1))
    /**< Acquire band class 1 systems only. */

#define SYS_BAND_MASK_BC2               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC2))
    /**< Acquire band class 2 place holder. */

#define SYS_BAND_MASK_BC3               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC3))
    /**< Acquire band class 3 systems only. */

#define SYS_BAND_MASK_BC4               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC4))
    /**< Acquire band class 4 systems only. */

#define SYS_BAND_MASK_BC5               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC5))
    /**< Acquire band class 5 systems only. */

#define SYS_BAND_MASK_GSM_DCS_1800      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_DCS_1800))
    /**< Acquire band class 8 systems only. */

#define SYS_BAND_MASK_GSM_EGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_EGSM_900))
    /**< Acquire GSM Extended GSM systems only. */

#define SYS_BAND_MASK_GSM_PGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_PGSM_900))
    /**< Acquire GSM Primary GSM systems only. */

#define SYS_BAND_MASK_BC6               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC6))
    /**< Acquire band class 6 systems only. */

#define SYS_BAND_MASK_BC7               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC7))
    /**< Acquire band class 7 systems only. */

#define SYS_BAND_MASK_BC8               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC8))
    /**< Acquire band class 8 systems only. */

#define SYS_BAND_MASK_BC9               ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC9))
    /**< Acquire band class 9 systems only. */

#define SYS_BAND_MASK_BC10              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC10))
    /**< Acquire band class 10 systems only. */

#define SYS_BAND_MASK_BC11              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC11))
    /**< Acquire band class 11 systems only. */

#define SYS_BAND_MASK_BC12              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC12))
    /**< Acquire band class 12 systems only. */

#define SYS_BAND_MASK_BC14              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC14))
    /**< Acquire band class 14 systems only. */

#define SYS_BAND_MASK_BC15              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC15))
    /**< Acquire band class 15 systems only. */

#define SYS_BAND_MASK_BC16              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC16))
    /**< Acquire band class 16 systems only. */

#define SYS_BAND_MASK_BC17              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC17))
    /**< Acquire band class 17 systems only. */

#define SYS_BAND_MASK_BC18              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC18))
    /**< Acquire band class 18 systems only. */

#define SYS_BAND_MASK_BC19              ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_BC19))
    /**< Acquire band class 19 systems only. */

#define SYS_BAND_MASK_GSM_450           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_450))
    /**< Acquire GSM 450 systems only. */

#define SYS_BAND_MASK_GSM_480           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_480))
    /**< Acquire GSM 480 systems only. */

#define SYS_BAND_MASK_GSM_750           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_750 ))
    /**< Acquire GSM 750 systems only. */

#define SYS_BAND_MASK_GSM_850           ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_850 ))
    /**< Acquire GSM 850 systems only. */

#define SYS_BAND_MASK_GSM_RGSM_900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_RGSM_900))
    /**< Acquire GSM Railway GSM systems only. */

#define SYS_BAND_MASK_GSM_PCS_1900      ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_GSM_PCS_1900))
    /**< Acquire GSM PCS systems only. */

#define SYS_BAND_MASK_WCDMA_I_IMT_2000  ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_I_IMT_2000))
    /**< Acquire WCDMA Europe, Japan, and China IMT 2100 systems only. */

#define SYS_BAND_MASK_WCDMA_II_PCS_1900 ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_II_PCS_1900))
    /**< Acquire WCDMA U.S. PCS 1900 system only. */

#define SYS_BAND_MASK_WCDMA_III_1700    ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_WCDMA_III_1700))
    /**< Acquire WCDMA Europe and China 1800 systems only. */

#define SYS_BAND_MASK_WCDMA_IV_1700     ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_IV_1700 ))
    /**< Acquire WCDMA U.S. 1700 systems only. */

#define SYS_BAND_MASK_WCDMA_V_850       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_V_850 ))
    /**< Acquire WCDMA U.S. 850 systems only. */

#define SYS_BAND_MASK_WCDMA_VI_800      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VI_800 ))
    /**< Acquire WCDMA Japan 800 systems only. */

#define SYS_BAND_MASK_WCDMA_VII_2600       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VII_2600 ))
    /**< Acquire WCDMA Europe 2600 systems only. */

#define SYS_BAND_MASK_WCDMA_VIII_900       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_VIII_900 ))
    /**< Acquire WCDMA Europe and China 900 systems only. */

#define SYS_BAND_MASK_WCDMA_IX_1700       ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_IX_1700 ))
    /**< Acquire WCDMA Japan 1700 systems only. */

#define SYS_BAND_MASK_WCDMA_XI_1500      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_XI_1500 ))
    /**< Acquire WCDMA 1500 systems only. */

#define SYS_BAND_MASK_WCDMA_XIX_850      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WCDMA_XIX_850 ))
    /**< Acquire WCDMA Japan 850 systems only. */

#define SYS_BAND_MASK_WLAN_US_2400      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_US_2400 ))
    /**< Acquire WLAN U.S. 2400 systems only. */

#define SYS_BAND_MASK_WLAN_EUROPE_2400  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_2400 ))
    /**< Acquire WLAN Europe 2400 systems only. */

#define SYS_BAND_MASK_WLAN_FRANCE_2400  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_2400 ))
    /**< Acquire WLAN France 2400 systems only. */

#define SYS_BAND_MASK_WLAN_SPAIN_2400   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_2400 ))
    /**< Acquire WLAN Spain 2400 systems only. */

#define SYS_BAND_MASK_WLAN_JAPAN_2400   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_2400 ))
    /**< Acquire WLAN Japan 2400 systems only. */

#define SYS_BAND_MASK_WLAN_US_5000      ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_US_5000 ))
    /**< Acquire WLAN U.S. 5000 systems only. */

#define SYS_BAND_MASK_WLAN_EUROPE_5000  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_EUROPE_5000 ))
    /**< Acquire WLAN Europe 5000 systems only. */

#define SYS_BAND_MASK_WLAN_FRANCE_5000  ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_FRANCE_5000 ))
    /**< Acquire WLAN France 5000 systems only. */

#define SYS_BAND_MASK_WLAN_SPAIN_5000   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_SPAIN_5000 ))
    /**< Acquire WLAN Spain 5000 systems only. */

#define SYS_BAND_MASK_WLAN_JAPAN_5000   ((sys_band_mask_type) SYS_BM_64BIT( SYS_SBAND_WLAN_JAPAN_5000 ))
    /**< Acquire WLAN Japan 5000 systems only. */

/** Acquire U.S./JAPAN/EUROPE/FRANCE/SPAIN 2400 WLAN systems in respective countries. */
#define SYS_BAND_MASK_WLAN_2400         (SYS_BAND_MASK_WLAN_US_2400      | \
                                         SYS_BAND_MASK_WLAN_JAPAN_2400   | \
                                         SYS_BAND_MASK_WLAN_EUROPE_2400  | \
                                         SYS_BAND_MASK_WLAN_FRANCE_2400  | \
                                         SYS_BAND_MASK_WLAN_SPAIN_2400)

/** Acquire U.S./JAPAN/EUROPE/FRANCE/SPAIN 5000 WLAN systems in respective countries. */
#define SYS_BAND_MASK_WLAN_5000         (SYS_BAND_MASK_WLAN_US_5000      | \
                                         SYS_BAND_MASK_WLAN_JAPAN_5000   | \
                                         SYS_BAND_MASK_WLAN_FRANCE_5000  | \
                                         SYS_BAND_MASK_WLAN_EUROPE_5000  | \
                                         SYS_BAND_MASK_WLAN_SPAIN_5000)

/** Acquire any of WLAN 2400/5000 systems in U.S./JAPAN/EUROPE/FRANCE/SPAIN. */
#define SYS_BAND_MASK_WLAN_ANY          (SYS_BAND_MASK_WLAN_2400 | \
                                         SYS_BAND_MASK_WLAN_5000)

/** Acquire any systems within the specified mode preferences. */
#define SYS_BAND_MASK_ANY               ((sys_band_mask_e_type)0x7FFFFFFFFFFFFFFFULL)
                                         /* No, this doesn't say "FULL". 
                                         ** "ULL" designates 
                                         **"unsigned long long" */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* If there is a new band class, append to the end of list.
   per 3GPP 36101-830.
*/
/** LTE system band classes.
*/
typedef enum {
  SYS_SBAND_LTE_EUTRAN_BAND1 = 0,
  /**< UL:1920-1980; DL:2110-2170. */

  SYS_SBAND_LTE_EUTRAN_BAND2 = 1,
  /**< UL:1850-1910; DL:1930-1990. */

  SYS_SBAND_LTE_EUTRAN_BAND3 = 2,
  /**< UL:1710-1785; DL:1805-1880. */

  SYS_SBAND_LTE_EUTRAN_BAND4 = 3,
  /**< UL:1710-1755; DL:2110-2155. */

  SYS_SBAND_LTE_EUTRAN_BAND5 = 4,
  /**< UL: 824-849; DL: 869-894. */

  SYS_SBAND_LTE_EUTRAN_BAND6 = 5,
  /**< UL: 830-840; DL: 875-885. */

  SYS_SBAND_LTE_EUTRAN_BAND7 = 6,
  /**< UL:2500-2570; DL:2620-2690. */

  SYS_SBAND_LTE_EUTRAN_BAND8 = 7,
  /**< UL: 880-915; DL: 925-960. */

  SYS_SBAND_LTE_EUTRAN_BAND9 = 8,
  /**< UL:1749.9-1784.9; DL:1844.9-1879.9. */

  SYS_SBAND_LTE_EUTRAN_BAND10 = 9,
  /**< UL:1710-1770; DL:2110-2170. */

  SYS_SBAND_LTE_EUTRAN_BAND11 = 10,
  /**< UL:1427.9-1452.9; DL:1475.9-1500.9. */

  SYS_SBAND_LTE_EUTRAN_BAND12 = 11,
  /**< UL:698-716; DL:728-746. */

  SYS_SBAND_LTE_EUTRAN_BAND13 = 12,
  /**< UL: 777-787; DL: 746-756. */

  SYS_SBAND_LTE_EUTRAN_BAND14 = 13,
  /**< UL: 788-798; DL: 758-768. */

  SYS_SBAND_LTE_EUTRAN_BAND17 = 16,
  /**< UL: 704-716; DL: 734-746. */

  SYS_SBAND_LTE_EUTRAN_BAND18 = 17,
  /**< UL: 815-830; DL: 860-875. */

  SYS_SBAND_LTE_EUTRAN_BAND19 = 18,
  /**< UL: 830-845; DL: 875-890. */

  SYS_SBAND_LTE_EUTRAN_BAND20 = 19,
  /**< UL: 832-862; DL: 791-821. */

  SYS_SBAND_LTE_EUTRAN_BAND21 = 20,
  /**< UL: 1447.9-1462.9; DL: 1495.9-1510.9. */

  SYS_SBAND_LTE_EUTRAN_BAND24 = 23,
  /**< UL: 1626.5-1660.5; DL: 1525 -1559. */

  SYS_SBAND_LTE_EUTRAN_BAND25 = 24,
  /**< UL: 1850-1915; DL: 1930 -1995 . */

  SYS_SBAND_LTE_EUTRAN_BAND33 = 32,
  /**< UL: 1900-1920; DL: 1900-1920. */

  SYS_SBAND_LTE_EUTRAN_BAND34 = 33,
  /**< UL: 2010-2025; DL: 2010-2025. */

  SYS_SBAND_LTE_EUTRAN_BAND35 = 34,
  /**< UL: 1850-1910; DL: 1850-1910. */

  SYS_SBAND_LTE_EUTRAN_BAND36 = 35,
  /**< UL: 1930-1990; DL: 1930-1990. */

  SYS_SBAND_LTE_EUTRAN_BAND37 = 36,
  /**< UL: 1910-1930; DL: 1910-1930. */

  SYS_SBAND_LTE_EUTRAN_BAND38 = 37,
  /**< UL: 2570-2620; DL: 2570-2620. */

  SYS_SBAND_LTE_EUTRAN_BAND39 = 38,
  /**< UL: 1880-1920; DL: 1880-1920. */

  SYS_SBAND_LTE_EUTRAN_BAND40 = 39,
  /**< UL: 2300-2400; DL: 2300-2400. */

  SYS_SBAND_LTE_EUTRAN_BAND41 = 40,
  /**< UL: 2496-2690; DL: 2496-2690 */

  SYS_SBAND_LTE_EUTRAN_BAND42 = 41,
  /**< UL: 3400-3600; DL: 3400-3600 */

  SYS_SBAND_LTE_EUTRAN_BAND43 = 42,
  /**< UL: 3600-3800; DL: 3600-3800 */
  
  SYS_SBAND_LTE_EUTRAN_BAND_MAX
  /**< Maximum number of EUTRAN bands. */

} sys_sband_lte_e_type;
 
/*
*  Enumeration of LTE system band class mask.
*  It converts bit position of sys_lte_sband_e_type to a bit
*  mask. To keep numbering scheme consistent,
*  use sys_lte_sband_e_type instead of numbers directly.
*/

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* LTE band class bitmask data type. It is a combination of
*  enumeration of LTE system band class mask. */

#define SYS_BAND_MASK_LTE_EMPTY             0
     /**< No LTE band selected. */

#define SYS_BAND_MASK_LTE_BAND1             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND1))
    /**< Acquire UL: 1920-1980; DL: 2110-2170. */

#define SYS_BAND_MASK_LTE_BAND2             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND2))
    /**< Acquire UL: 1850-1910; DL: 1930-1990. */

#define SYS_BAND_MASK_LTE_BAND3             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND3))
    /**< Acquire UL: 1710-1785; DL: 1805-1880. */

#define SYS_BAND_MASK_LTE_BAND4             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND4))
    /**< Acquire UL: 1710-1755; DL: 2110-2115. */

#define SYS_BAND_MASK_LTE_BAND5             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND5))
    /**< Acquire UL: 824-849; DL: 869-894. */

#define SYS_BAND_MASK_LTE_BAND6             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND6))
    /**< Acquire UL: 830-840; DL: 875-885. */

#define SYS_BAND_MASK_LTE_BAND7             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND7))
    /**< Acquire UL: 2500-2570; DL: 2620-2690. */

#define SYS_BAND_MASK_LTE_BAND8             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND8))
    /**< Acquire UL: 880-915; DL: 925-960. */

#define SYS_BAND_MASK_LTE_BAND9             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND9))
    /**< Acquire UL: 1749.9-1784.9; DL: 1844.9-1879.9. */

#define SYS_BAND_MASK_LTE_BAND10             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND10))
    /**< Acquire UL: 1710-1770; DL: 2110-2170. */

#define SYS_BAND_MASK_LTE_BAND11             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND11))
    /**< Acquire UL:1427.9-1452.9; DL: 1475.9-1500.9. */

#define SYS_BAND_MASK_LTE_BAND12             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND12))
    /**< Acquire UL:698-716; DL: 728-746. */

#define SYS_BAND_MASK_LTE_BAND13             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND13))
    /**< Acquire UL: 777-787; DL: 746-756. */

#define SYS_BAND_MASK_LTE_BAND14             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND14))
    /**< Acquire UL: 788-798; DL: 758-768. */

#define SYS_BAND_MASK_LTE_BAND17             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND17))
    /**< Acquire UL: 704-716; DL: 734-746. */
#define SYS_BAND_MASK_LTE_BAND18             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND18))
    /**< Acquire UL: 815–830; DL: 860–875. */

#define SYS_BAND_MASK_LTE_BAND19             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND19))
    /**< Acquire UL: 830–845; DL: 875–890. */

#define SYS_BAND_MASK_LTE_BAND20             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND20))
    /**< Acquire UL: 832–862; DL: 791–821. */

#define SYS_BAND_MASK_LTE_BAND21             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND21))
    /**< Acquire UL: 1447.9–1462.9; DL: 1495.9–1510.9. */

#define SYS_BAND_MASK_LTE_BAND24             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND24))
    /**< Acquire UL: 1626.5-1660.5; DL: 1525 -1559. */

#define SYS_BAND_MASK_LTE_BAND25             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND25))
    /**< Acquire UL: 1850-1915; DL: 1930 -1995  */

#define SYS_BAND_MASK_LTE_BAND33             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND33))
    /**< Acquire UL: 1900-1920; DL: 1900-1920. */

#define SYS_BAND_MASK_LTE_BAND34             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND34))
    /**< Acquire UL: 2010-2025; DL: 2010-2025. */

#define SYS_BAND_MASK_LTE_BAND35             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND35))
    /**< Acquire UL: 1850-1910; DL: 1850-1910. */

#define SYS_BAND_MASK_LTE_BAND36             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND36))
    /**< Acquire UL: 1930-1990; DL: 1930-1990. */

#define SYS_BAND_MASK_LTE_BAND37             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND37))
    /**< Acquire UL: 1910-1930; DL: 1910-1930. */

#define SYS_BAND_MASK_LTE_BAND38             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND38))
    /**< Acquire UL: 2570-2620; DL: 2570-2620. */

#define SYS_BAND_MASK_LTE_BAND39             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND39))
    /**< Acquire UL: 1880-1920; DL: 1880-1920. */

#define SYS_BAND_MASK_LTE_BAND40             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND40))
    /**< Acquire UL: 2300-2400; DL: 2300-2400. */

#define SYS_BAND_MASK_LTE_BAND41             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND41))
    /**< Acquire UL: 2496-2690; DL: 2496-2690. */

#define SYS_BAND_MASK_LTE_BAND42             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND42))
    /**< Acquire UL: 3400-3600; DL: 3400-3600. */

#define SYS_BAND_MASK_LTE_BAND43             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_LTE_EUTRAN_BAND43))
    /**< Acquire UL: 3600-3800; DL: 3600-3800. */
    
#define SYS_MAX_ACQ_PREF_RAT_LIST_NUM 15
    /**< Maximum number of acquisition lists. */


/**
*  Enumeration of TD-SCDMA system band classes. If there is a new
*  band class, append to the end of list. per 3GPP TS 125 102
*/
typedef enum {
  SYS_SBAND_TDS_BANDA = 0,
  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

  SYS_SBAND_TDS_BANDB = 1,
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

  SYS_SBAND_TDS_BANDC = 2,
  /**< TDS Band C 1910-1930 MHz */

  SYS_SBAND_TDS_BANDD = 3,
  /**< TDS Band D 2570-2620 MHz */

  SYS_SBAND_TDS_BANDE = 4,
  /**< TDS Band E 2300-2400 MHz */

  SYS_SBAND_TDS_BANDF = 5,
  /**< TDS Band F 1880-1920 MHz */

  SYS_SBAND_TDS_BAND_MAX
  /**< Maximum number of TD-SCDMA bands */

} sys_sband_tds_e_type;

/**
*  Enumeration of TD-SCDMA system band class mask.
*  It converts bit position of sys_tds_sband_e_type to a bit
*  mask. To keep numbering scheme consistent,
*  use sys_tds_sband_e_type instead of numbers directly.
**
*/

/**
*  TD-SCDMA band class bit mask data type. It is a combination of
*  enumeration of TD-SCDMA system band class mask.
*/

#define SYS_BAND_MASK_TDS_EMPTY             0
     /**< No TDS band selected */

#define SYS_BAND_MASK_TDS_BANDA             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDA))
     /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

#define SYS_BAND_MASK_TDS_BANDB             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDB))
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

#define SYS_BAND_MASK_TDS_BANDC             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDC))
  /**< TDS Band C 1910-1930 MHz */

#define SYS_BAND_MASK_TDS_BANDD             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDD))
  /**< TDS Band D 2570-2620 MHz */

#define SYS_BAND_MASK_TDS_BANDE             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDE))
  /**< TDS Band E 2300-2400 MHz */

#define SYS_BAND_MASK_TDS_BANDF             ((sys_band_mask_type) SYS_BM_64BIT(SYS_SBAND_TDS_BANDF))
  /**< TDS Band F 1880-1920 MHz */

    

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Channel data type.
*/
typedef word  sys_chan_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Personal Communication Services (PCS) frequency blocks/cellular systems.
*/
typedef enum
{
  /** @cond
  */
  SYS_BLKSYS_NONE = -1,   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_BLKSYS_PCS_A,
    /**< PCS block A. */

  SYS_BLKSYS_PCS_B,
    /**< PCS block B. */

  SYS_BLKSYS_PCS_C,
    /**< PCS block C. */

  SYS_BLKSYS_PCS_D,
    /**< PCS block D. */

  /* 4 */
  SYS_BLKSYS_PCS_E,
    /**< PCS block E. */

  SYS_BLKSYS_PCS_F,
    /**< PCS block F. */

  SYS_BLKSYS_PCS_CELL_A,
    /**< Cellular system A. */

  SYS_BLKSYS_PCS_CELL_B,
    /**< Cellular system B. */

  /* 8 */
  SYS_BLKSYS_PCS_G,
    /**< BC5/11 G-Block. */

  SYS_BLKSYS_PCS_H,
    /**< BC5/11 H-Block. */

  SYS_BLKSYS_PCS_I,
    /**< BC5/11 I-Block. */

  SYS_BLKSYS_PCS_J,
    /**< BC5/11 J-Block. */

  /* 12 */
  SYS_BLKSYS_PCS_K,
    /**< BC5/11 K-Block. */

  SYS_BLKSYS_PCS_L,
    /**< BC5 L-BLOCK. */

  /** @cond
  */
  SYS_BLKSYS_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_blksys_e_type;
/*~ SENTINEL sys_blksys_e_type.SYS_BLKSYS_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Preferred Roaming List (PRL) system types.
*/
typedef enum {

  SYS_PRL_AVAIL_BC0_A = 0,         
    /**< Available systems on Band Class 0, A-System only. */

  SYS_PRL_AVAIL_BC0_B = 1,         
    /**< Available systems on Band Class 0, B-System only. */

  /** @cond
  */
  SYS_PRL_MAX
  /** @endcond
  */
} sys_prl_e_type;
/*~ SENTINEL sys_prl_e_type.SYS_PRL_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Service status; indicates service availability.
*/
typedef enum
{
  /** @cond
  */
  SYS_SRV_STATUS_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_SRV_STATUS_NO_SRV,
    /**< No service. */

  SYS_SRV_STATUS_LIMITED,
    /**< Limited service. */

  SYS_SRV_STATUS_SRV,
    /**< Service available. */

  SYS_SRV_STATUS_LIMITED_REGIONAL,
    /**< Limited regional service. */

  /* 4 */
  SYS_SRV_STATUS_PWR_SAVE,
    /**< MS is in power save or deep sleep. */

  /** @cond
  */
  SYS_SRV_STATUS_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_srv_status_e_type;
/*~ SENTINEL sys_srv_status_e_type.SYS_SRV_STATUS_MAX */

/** Service type.
*/
typedef enum
{
  /** @cond
  */
  SYS_SRV_DOMAIN_NONE = -1,   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
  /* 0 */
  SYS_SRV_DOMAIN_NO_SRV,
    /**< No service. */

  SYS_SRV_DOMAIN_CS_ONLY,
    /**< Circuit switched only capable. */

  SYS_SRV_DOMAIN_PS_ONLY,
    /**< Packet switched only capable. */

  SYS_SRV_DOMAIN_CS_PS,
    /**< Circuit and packet switched capable. */

  /* 4 */
  SYS_SRV_DOMAIN_CAMPED,
    /**< MS found the right system but not yet registered or attached. */

  /** @cond
  */
  SYS_SRV_DOMAIN_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_srv_domain_e_type;
/*~ SENTINEL sys_srv_domain_e_type.SYS_SRV_DOMAIN_MAX */

/**
** The following enumerated type defines the CS service capabilities
** of LTE network.
*/
typedef enum
{
  /** @cond
  */
  SYS_LTE_CS_CAPABILITY_NONE = -1,     /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_LTE_CS_CAPABILITY_NO_VALUE           = 0, 
   /**< FULL service on CS domain is available. */

  SYS_LTE_CS_CAPABILITY_CSFB_NOT_PREFFERED = 1, 
   /**< CSFB is not preffered. */

  SYS_LTE_CS_CAPABILITY_SMS_ONLY           = 2, 
   /**< CS registation is for SMS only. */   

  SYS_LTE_CS_CAPABILITY_LIMITED            = 3, 
   /**< Used to covey that CS registation failed for 
   ** max attach or TAU attempts. */   

  /** @cond
  */
  SYS_LTE_CS_CAPABILITY_MAX         /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

}sys_lte_cs_capability_e_type;
/*~ SENTINEL sys_lte_cs_capability_e_type.SYS_LTE_CS_CAPABILITY_MAX */

/**
** The following enumerated type defines the UE 
** of mode of operation when operating in  LTE network.
*/

typedef enum
{
  /** @cond
  */
  SYS_LTE_UE_MODE_NONE               = -1, /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_LTE_UE_MODE_PS_MODE1           = 0, 
   /**< UE registers only to EPS services, 
       and UE's usage setting is voice centric. */

  SYS_LTE_UE_MODE_PS_MODE2           = 1,
   /**< UE registers only to EPS services, 
        and UE's usage setting is data centric. */

  SYS_LTE_UE_MODE_CS_PS_MODE1        = 2,
   /**< UE registers to both EPS and non-EPS services, 
        and UE's usage setting  is voice centric. */
  
  SYS_LTE_UE_MODE_CS_PS_MODE2        = 3, 
   /**<  UE registers to both EPS and non-EPS services,
         and UE's usage setting is data centric. */

  /** @cond
  */
  SYS_LTE_UE_MODE_MAX          /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

}sys_lte_ue_mode_of_operation_e_type;
/*~ SENTINEL sys_lte_ue_mode_of_operation_e_type.SYS_LTE_UE_MODE_MAX */


/* Defines UE usage setting
** Voice centric, Data centric and by default value.
** Applicable to LTE capable UE only.
*/
typedef enum
{
  /** @cond
  */
  SYS_UE_USAGE_SETTING_NONE   = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond */

  SYS_UE_USAGE_SETTING_VOICE_CENTRIC = 0,
    /**< UE usage is set to voice centric. */    

  SYS_UE_USAGE_SETTING_DATA_CENTRIC  = 1,
    /**< UE usage is set to data centric. */

  /**@cond
  */
  SYS_UE_USAGE_SETTING_MAX          /* FOR INTERNAL USE ONLY */
  /**@endcond */

} sys_ue_usage_setting_e_type;
/*~ SENTINEL sys_ue_usage_setting_e_type.SYS_UE_USAGE_SETTING_MAX */

/* Defines UE voice domain preference.
** Applicable to LTE capable UE only.
*/
typedef enum
{
  /** @cond
  */
  SYS_VOICE_DOMAIN_PREF_NONE   = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond */

  SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY   = 0,
    /**< CS Voice only. */    

  SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY   = 1,
    /**< IMS PS Voice only. */    

  SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED   = 2,
    /**< CS voice preferred, IMS PS Voice as secondary. */

  SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED   = 3,
    /**< IMS PS voice preferred, CS Voice as secondary. */

  /**@cond
  */
  SYS_VOICE_DOMAIN_PREF_MAX   /* FOR INTERNAL USE ONLY */
  /**@endcond */

} sys_voice_domain_pref_e_type;

/** Service capabilities of a network.
*/
typedef enum
{
  /** @cond
  */
  SYS_SRV_CAPABILITY_NONE = -1,   /*FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
    /**< Network cannot provide the service. */

  SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
    /**< Network can provide a circuit switched service. */

  SYS_SRV_CAPABILITY_PS_SERVICE_ONLY,
    /**< Network can provide a packet switched service. */

  SYS_SRV_CAPABILITY_CS_PS_SERVICE,
    /**< Network can provide a circuit and packet switched service. */

  /** @cond
  */
  SYS_SRV_CAPABILITY_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_srv_capability_e_type;
/*~ SENTINEL sys_srv_capability_e_type.SYS_SRV_CAPABILITY_MAX */

/** Roaming status.

The values for these enumerates were derived from the roaming display
indicators described in 3GPP2 C.R1001-A.

@note1hang The actual value of the enumerate is not consistent with the value
listed in the standard (e.g., the values of SYS_ROAM_STATUS_OFF and
SYS_ROAM_STATUS_ON are reversed from the standard).
*/
typedef enum
{
  /** @cond
  */
  SYS_ROAM_STATUS_NONE = -1,   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_ROAM_STATUS_OFF,
    /**< Roaming indicator off. */

  SYS_ROAM_STATUS_ON,
    /**< Roaming indicator on. */

  SYS_ROAM_STATUS_BLINK,
    /**< Roaming indicator flashing. */

  SYS_ROAM_STATUS_OUT_OF_NEIGHBORHOOD,
    /**< Out of neighborhood. */
  
  /* 4 */
  SYS_ROAM_STATUS_OUT_OF_BLDG,
    /**< Out of building. */

  SYS_ROAM_STATUS_PREF_SYS,
    /**< Roaming: preferred system. */

  SYS_ROAM_STATUS_AVAIL_SYS,
    /**< Roaming: available system. */

  SYS_ROAM_STATUS_ALLIANCE_PARTNER,
    /**< Roaming: alliance partner. */

  /* 8 */
  SYS_ROAM_STATUS_PREMIUM_PARTNER,
    /**< Roaming: premium partner. */

  SYS_ROAM_STATUS_FULL_SVC,
    /**< Roaming: full service functionality. */

  SYS_ROAM_STATUS_PARTIAL_SVC,
    /**< Roaming: partial service functionality. */

  SYS_ROAM_STATUS_BANNER_ON,
    /**< Roaming banner on. */

  /* 12 */
  SYS_ROAM_STATUS_BANNER_OFF,
    /**< Roaming banner off. */

  /** @cond
  */
  SYS_ROAM_STATUS_MAX  = 0xFF+1  /*   FOR INTERNAL USE ONLY!
      Values 0x00 - 0xFF are used in the standard */
  /** @endcond
  */
} sys_roam_status_e_type;

/** Radio Access Technologies (RATs).
*/
typedef enum
{
  /** @cond
  */
  SYS_RAT_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_RAT_GSM_RADIO_ACCESS,
    /**< GSM radio access technology. */

  SYS_RAT_UMTS_RADIO_ACCESS,
    /**< UMTS radio access technology. */

  SYS_RAT_LTE_RADIO_ACCESS,
    /**< LTE radio access technology. */

  SYS_RAT_TDS_RADIO_ACCESS,
    /**< TD-SCDMA radio access technology */

  /** @cond
  */
  SYS_RAT_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_radio_access_tech_e_type;
/*~ SENTINEL sys_radio_access_tech_e_type.SYS_RAT_MAX */

/** State of the SIM card.
*/
typedef enum
{
  /** @cond
  */
  SYS_SIM_STATE_NONE = -1,    /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  /* 0 */
  SYS_SIM_STATE_NOT_AVAILABLE,
    /**< The SIM is not available. */

  SYS_SIM_STATE_AVAILABLE,
    /**< The SIM is available. */

  SYS_SIM_STATE_CS_INVALID,
    /**< The SIM has been marked by the network
         as invalid for CS services. */

  SYS_SIM_STATE_PS_INVALID,
    /**< The SIM has been marked by the network
         as invalid for PS services. */

  /* 4 */
  SYS_SIM_STATE_CS_PS_INVALID,
    /**< The SIM has been marked by the network
         as invalid for CS and PS services. */

  /** @cond
  */
  SYS_SIM_STATE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_sim_state_e_type;
/*~ SENTINEL sys_sim_state_e_type.SYS_SIM_STATE_MAX */


/** MSM identification.
*/
typedef enum
{
  /** @cond
  */
  SYS_MSM_ID_NONE = -1,   /* used only for bounds checking. */
  /** @endcond
  */
  /* 0 */
  SYS_MSM_ID_6500,
    /**< MSM ID is 6500. */

  SYS_MSM_ID_6250,
    /**< MSM ID is 6250. */

  SYS_MSM_ID_MAIN,
    /**< MSM that has control of DPRAM. */

  SYS_MSM_ID_SECONDARY,
    /**< MSM that does not have control of DPRAM. */

  /* 4 */
  /** @cond
  */
  SYS_MSM_ID_MAX   /* Used for arrays & bounds checking. */
  /** @endcond
  */
} sys_msm_id_e_type;
/*~ SENTINEL sys_msm_id_e_type.SYS_MSM_ID_MAX */

/*--------------------------------------------------------------------------
              Domain Specific Access Control (DSAC) information
---------------------------------------------------------------------------*/

/** Enumeration of Domain access state information
*/
typedef enum 
{
  SYS_CELL_ACCESS_NONE = -1, 
    /**< FOR INTERNAL USE OF CM ONLY!
    */ 

  SYS_CELL_ACCESS_NORMAL_ONLY,
    /**< Access is barred for normal calls only
    */ 

  SYS_CELL_ACCESS_EMERGENCY_ONLY,
    /**< Access is barred for emergency calls only
    */ 

  SYS_CELL_ACCESS_NO_CALLS,
    /**< Access is not barred for any calls
    */ 

  SYS_CELL_ACCESS_ALL_CALLS,
    /**< Access is barred for all calls
    */ 

  SYS_CELL_ACCESS_MAX 
    /**< FOR INTERNAL USE OF CM ONLY!
    */ 
}sys_cell_access_status_e_type;
/*~ SENTINEL sys_cell_access_status_e_type.SYS_CELL_ACCESS_MAX */

/** Structure to define a DSAC information
*/
typedef struct
{
  sys_cell_access_status_e_type    cs_bar_status; 
    /**< Call barring status for CS calls */

  sys_cell_access_status_e_type    ps_bar_status; 
   /**< Call barring status for PS calls */

}sys_domain_access_bar_s_type;

/** Simultaneous modes, needed for Mobile Station Modem (MSM) 7600 to distinguish
    between WCDMA/1X (WX) and non-WX modes of operation.
*/
typedef enum
{
  /** @cond
  */
  SYS_SIMULTANEOUS_MODE_NONE = -1, /*   For Internal SD use */
  /** @endcond
  */
  SYS_SIMULTANEOUS_MODE_NON_WX,    /**< Not WX mode. */

  SYS_SIMULTANEOUS_MODE_WX ,       /**< Simultaneous WCDMA and 1X. */

  /** @cond
  */
  SYS_SIMULTANEOUS_MODE_MAX        /*   For Internal SD use */
  /** @endcond
  */

} sys_simultaneous_mode_e_type;
/*~ SENTINEL sys_simultaneous_mode_e_type.SYS_SIMULTANEOUS_MODE_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** High-Speed Downlink Packet Access (HSDPA) and High-Speed Uplink Packet
    Access (HSUPA) indication types.

    HSDPA and HSUPA can only be available within a WCDMA service.
*/
typedef enum
{
  /** @cond
  */
  SYS_HS_IND_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL,
    /**< Cell does not support either HSDPA or HSUPA. */

  SYS_HS_IND_HSDPA_SUPP_CELL,
    /**< Cell supports HSDPA. */

  SYS_HS_IND_HSUPA_SUPP_CELL,
    /**< Cell supports HSUPA. */

  SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL,
    /**< Cell supports HSDPA and HSUPA. */  

  /* HSDPA_PLUS status indicator values */

  SYS_HS_IND_HSDPAPLUS_SUPP_CELL,
    /**< Cell supports HSDPA+. */

  SYS_HS_IND_HSDPAPLUS_HSUPA_SUPP_CELL,
    /**< Cell supports HSDPA+ and HSUPA. */  

  SYS_HS_IND_DC_HSDPAPLUS_SUPP_CELL,
    /**< Cell supports DC HSDPA+. */

  SYS_HS_IND_DC_HSDPAPLUS_HSUPA_SUPP_CELL,
    /**< Cell supports DC HSDPA+ and HSUPA. */

  SYS_HS_IND_HSDPAPLUS_64QAM_HSUPA_SUPP_CELL,
    /**< Cell supports 64QAM HSDPA+ and HSUPA */

  SYS_HS_IND_HSDPAPLUS_64QAM_SUPP_CELL,
    /**< Cell supports 64QAM HSDPA+  */

  /** @cond
  */
  SYS_HS_IND_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
} sys_hs_ind_e_type;
/*~ SENTINEL sys_hs_ind_e_type.SYS_HS_IND_MAX */

/** Dual Transfer Mode (DTM) allows simultaneous CS and PS calls only during GSM
    service.
*/
typedef enum
{
  /** @cond
  */
  SYS_DTM_SUPPORT_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */  
  /** @endcond
  */

  SYS_DTM_SUPPORT_NOT_AVAIL,
    /**< Dual Transfer mode support is not available. */

  SYS_DTM_SUPPORT_AVAIL,
    /**< Dual Transfer mode support is available. */
  
  /** @cond
  */
  SYS_DTM_SUPPORT_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_dtm_support_e_type;
/*~ SENTINEL sys_dtm_support_e_type.SYS_DTM_SUPPORT_MAX */

/** Indicates whether Evolved GPRS (EGPRS) is available.
*/
typedef enum
{
  /** @cond
  */
  SYS_EGPRS_SUPPORT_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_EGPRS_SUPPORT_NOT_AVAIL,
    /**< EGPRS support is not available. */

  SYS_EGPRS_SUPPORT_AVAIL,
    /**< EGPRS support is available. */

  /** @cond
  */
  SYS_EGPRS_SUPPORT_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_egprs_support_e_type;
/*~ SENTINEL sys_egprs_support_e_type.SYS_EGPRS_SUPPORT_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Maximum length for a user zone name.
*/
#define  SYS_PUZL_MAX_UZ_NAME_LEN          32

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Name of the current user zone selected in a CDMA system.
*/
typedef struct
{
  byte     uz_name_encoding;
    /**< Type of encoding used for the name. */

  byte     uz_name_n_octets;
    /**< Number of bytes used for the name. */

  byte     uz_name[SYS_PUZL_MAX_UZ_NAME_LEN];
    /**< Buffer to hold encoded octets. */

} sys_puzl_uz_name_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   HDR related Defines^M
------------------------------------------------------------------------------*/

/** HDR acquisition modes.
*/
typedef enum {

  SYS_HDR_ACQ_MEASURE_RF,       /**< Only acquire if the raw power is high
                                     enough. */

  SYS_HDR_ACQ_MODE_MICRO,       /**< Micro acquisitions. */

  SYS_HDR_ACQ_MODE_SHALLOW,     /**< Shallow acquisitions only. */

  SYS_HDR_ACQ_MODE_DEEP,        /**< Deep acquisitions only. */

  SYS_HDR_ACQ_MODE_FULL,        /**< Shallow and deep acquisitions. */

  /** @cond
  */
  SYS_HDR_ACQ_MODE_MAX          /* For internal use only */
  /** @endcond
  */

} sys_hdr_acq_mode_e_type;
/*~ SENTINEL sys_hdr_acq_mode_e_type.SYS_HDR_ACQ_MODE_MAX */

/** Active protocol revision for various technologies.
*/
typedef enum {

  
  SYS_ACTIVE_PROT_NONE,           /**< The protocol revision is unknown.
                                       When HDR system ID is first acquired
                                       and no protocol revision is 
                                       available at this point. */

  SYS_ACTIVE_PROT_HDR_START,      /**< Lower boundary (excluding) for a valid
                                       HDR protocol revision */

  SYS_ACTIVE_PROT_HDR_REL0,       /**< HDR release 0. */

  SYS_ACTIVE_PROT_HDR_RELA,       /**< HDR release A (physical layer 
                                       sub-type 2 is active in the current
                                       personality). */
                                       
  SYS_ACTIVE_PROT_HDR_RELB,       /**< HDR release B. */

  SYS_ACTIVE_PROT_HDR_END,        /**< Internal use for HDR range check. */

                                  /* If adding other technologies, pls
                                     reserve some entries for HDR. */

  /** @cond
  */
  SYS_ACTIVE_PROT_MAX             /*   Internal use for enum range check. */
  /** @endcond
  */

} sys_active_prot_e_type;
/*~ SENTINEL sys_active_prot_e_type.SYS_ACTIVE_PROT_MAX */

/** Personality types for various technologies. The currently defined
    enumerations are applicable only for HDR.
*/
typedef enum {
  
  SYS_PERSONALITY_NONE,           /**< The personality is unknown (e.g., when
								       the system is first acquired and no
									   personality is available at that point). */

  SYS_PERSONALITY_HDR_START,      /**< For range checking. */

  SYS_PERSONALITY_HRPD,           /**< Personality does not have EHRPD (Evolved High Rate Packet Data)
                                       capability. */

  SYS_PERSONALITY_EHRPD,          /**< Personality has EHRPD capability. */

  SYS_PERSONALITY_HDR_END,        /**< Upper limit for HDR personality range. */

  /* If adding other technologies, please reserve some entries for HDR. */

  /** @cond
  */
  SYS_PERSONALITY_MAX             /*   Internal use for enum range check */
  /** @endcond
  */

} sys_personality_e_type;


/** PDP header compression types.
*/
typedef enum
{
  /** @cond
  */
  SYS_PDP_HEADER_COMP_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_PDP_HEADER_COMP_OFF = 0,
    /**< PDP header compression is OFF. Preserves earlier values of
         h_comp=FALSE for no header compression. */

  SYS_PDP_HEADER_COMP_ON  = 1,
    /**< PDP header compression is ON. Preserves earlier value of h_comp=TRUE
         for default header compression. */

  SYS_PDP_HEADER_COMP_RFC1144,
    /**< PDP header compression based on RFC 1144.
         3GPP TS 44.065 (section 6.5.1.1.4). */

  SYS_PDP_HEADER_COMP_RFC2507,
    /**< PDP header compression based on RFC 2507.
         3GPP TS 44.065 (section 6.5.1.1.4). */

  SYS_PDP_HEADER_COMP_RFC3095,
    /**< PDP header compression based on RFC 3095.
         3GPP TS 44.065 release-6 version-6.5.0 
        (sections 6.5.4 and 6.5.5). */

  /** @cond
  */
  SYS_PDP_HEADER_COMP_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_pdp_header_comp_e_type;
/*~ SENTINEL sys_pdp_header_comp_e_type.SYS_PDP_HEADER_COMP_MAX */

/** PDP data compression types.
*/
typedef enum
{
  /** @cond
  */
  SYS_PDP_DATA_COMP_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
  SYS_PDP_DATA_COMP_OFF = 0,
    /**< PDP data compression is OFF. Preserves the earlier value of
	     d_comp=FALSE for no data compression. */

  SYS_PDP_DATA_COMP_ON  = 1,
    /**< Default PDP data compression is enabled. Preserves the earlier
	     value of d_comp=TRUE for default data compression. */

  SYS_PDP_DATA_COMP_V42_BIS,
    /**< Data compression standard specified in
         3GPP TS 44.065 (section 6.6.1.1.4). */

  SYS_PDP_DATA_COMP_V44,
    /**< Data compression standard specified in
         3GPP TS 44.065 (section 6.6.1.1.4). */

  /** @cond
  */
  SYS_PDP_DATA_COMP_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_pdp_data_comp_e_type;
/*~ SENTINEL sys_pdp_data_comp_e_type.SYS_PDP_DATA_COMP_MAX */


/**
  NAS procedures  
*/
typedef enum
{

  /** @cond
  */
  SYS_NAS_PROCEDURE_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_NAS_PROCEDURE_MM,
    /**< MM Procedure */
  
  SYS_NAS_PROCEDURE_CM, 
    /**< CM Procedure */

  SYS_NAS_PROCEDURE_GMM_NON_COMBINED,
    /**< GMM Non-combined Procedure */

  SYS_NAS_PROCEDURE_SM, 
    /**< SM Procedure */

  SYS_NAS_PROCEDURE_GMM_COMBINED,
    /**< GMM Combined Procedure */

  SYS_NAS_PROCEDURE_MM_GMM_COMBINED,
    /**< MM GMM Combined Procedure */

  SYS_NAS_PROCEDURE_EMM_NON_COMBINED_EPS,
    /**< EMM Non-combined EPS Procedure */

  SYS_NAS_PROCEDURE_EMM_COMBINED_NON_EPS,
    /**< EMM Combined Non-EPS Procedure */

  SYS_NAS_PROCEDURE_EMM_COMBINED,
    /**< EMM Combined Procedure */

  SYS_NAS_PROCEDURE_ESM,
    /**< ESM Procedure */
  
  /** @cond
  */
  SYS_NAS_PROCEDURE_MAX  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */
}sys_nas_procedure_e_type;
/*~ SENTINEL sys_nas_procedure_e_type.SYS_NAS_PROCEDURE_MAX */


/*------------------------------------------------------------------------------
   WLAN related defines
------------------------------------------------------------------------------*/

/** WLAN scan types.
*/
typedef enum
{
  /** @cond
  */
  SYS_WLAN_SCAN_TYPE_NONE = -1,  /*   FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_WLAN_SCAN_TYPE_ACTIVE,
    /**< Active scanning.  */

  SYS_WLAN_SCAN_TYPE_PASSIVE,
    /**< Passive scanning. */

  /** @cond
  */
  /* FOR INTERNAL USE ONLY! */
  SYS_WLAN_SCAN_TYPE_MAX
  /** @endcond
  */

} sys_wlan_scan_type_e_type;
/*~ SENTINEL sys_wlan_scan_type_e_type.SYS_WLAN_SCAN_TYPE_MAX */


/** WLAN BSS types.
*/
typedef enum
{
  /** @cond
  */
  SYS_WLAN_BSS_TYPE_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_WLAN_BSS_TYPE_ADHOC,
    /**< Adhoc type BSS. */

  SYS_WLAN_BSS_TYPE_INFRA,
    /**< Infrastructure mode BSS. */

  SYS_WLAN_BSS_TYPE_ANY,
    /**< Any BSS mode type. */

  SYS_WLAN_BSS_TYPE_ADHOC_START,
    /**< Adhoc type BSS created by the MS. */

  /** @cond
  */
  SYS_WLAN_BSS_TYPE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_wlan_bss_e_type;
/*~ SENTINEL sys_wlan_bss_e_type.SYS_WLAN_BSS_TYPE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief SSID of WLAN system.
*/
typedef struct sys_wlan_ssid_s
{

  uint8                                len;
    /**< Length of the SSID; if == 0, then SSID = broadcast SSID.  */

  char                                 ssid[SYS_WLAN_SSID_MAX_SIZE];
    /**< SSID of the WLAN system. */

} sys_wlan_ssid_s_type;
 
/*~ FIELD sys_wlan_ssid_s.ssid VARRAY SYS_WLAN_SSID_MAX_SIZE
    LENGTH sys_wlan_ssid_s.len */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Technology type and version.
*/
typedef enum
{
  /** @cond
  */
  SYS_TECH_NONE                        = -1,  /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_TECH_WLAN_80211A                 = 0,
    /**< WLAN 802.11a technology. */

  SYS_TECH_WLAN_80211B                 = 1,
    /**< WLAN 802.11b technology. */

  SYS_TECH_WLAN_80211G                 = 2,
    /**< WLAN 802.11g technology. */

  /** @cond
  */
  SYS_TECH_RESERVED                    = 30,  /* Reserved values for CM use */
  /** @endcond
  */

  /** @cond
  */
    SYS_TECH_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_tech_e_type;
/*~ SENTINEL sys_tech_e_type.SYS_TECH_MAX */

/** Turns hybrid mode on and off.
*/
typedef enum
{
  /** @cond
  */
    SYS_HYBR_PREF_NONE                      = -1, /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

  SYS_HYBR_PREF_OFF                         = 0,
    /**< Hybrid mode is OFF. */

  SYS_HYBR_PREF_ON                          = 1,
    /**< Hybrid mode is ON. */

  /** @cond
  */
  SYS_BYBR_PREF_MAX     /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_hybr_pref_e_type;
/*~ SENTINEL sys_hybr_pref_e_type.SYS_BYBR_PREF_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/** @brief Information related to each RAT in the RAT priority list.
*/  
typedef struct   sys_priority_list_info_s 
{

  sys_band_mask_e_type  band_cap;
    /**< UE capable bands for the associated RAT used for
         reselection, redirection, and a PLMN search. */

  sys_sys_mode_e_type   acq_sys_mode;
    /**< Defines the RAT. */

  uint32 acq_sys_time_interval; 
    /**< Time interval given for a RAT to acquire a system. */
  
  boolean               bst_rat_acq_required;
    /**< This flag decides if an acquisition on the associated RAT should occur
	     as part of the current service request. */

  sys_band_mask_e_type  bst_band_cap;
    /**< The bands that have to be searched as part of this service request
         if bst_rat_acq_required = TRUE. If bst_rat_acq_required = FALSE,
         bst_band_cap should be ignored and bst_band_cap is used for PLMN/cell
         selection. */ 
  
} sys_priority_list_info_s_type; 

/** @brief Type used by the Enhanced Out-of-Service algorithms. This type
    specifies if a timer-based search should be used during system acquisition
    and if the next search is a continuation of the previous one.
*/
typedef struct eoos_gwl_scan_s
{  
  boolean      new_scan;
    /**< Continue with the previous search or start a new search */

  boolean      use_timer;
    /**<  Use a timer-based search if TRUE. */
} eoos_gwl_scan_s_type;

/** @brief RAT priority list.
*/
typedef struct sys_rat_pri_list_info_s
{

   uint32 num_items;
      /**< Number of valid items in the list. */

   uint32 next_acq_sys_index;
      /**< Pointer to which 3GPP system will be the next to be acquired. For
	       example, with priority_list_info [GSM(index=0), 1X, WCDMA, LTE],
		   if the next_acq_sys_index is 2, the WCDMA system is the first that
		   will be attempted to be acquired, then the LTE system. */

   sys_priority_list_info_s_type  priority_list_info[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
      /**< In the priority_list_info, system acquisition order starts from 0,
	       so index 0 is first, and SYS_MAX_ACQ_PREF_RAT_LIST_NUM-1 is the last
		   to be attempted. For example, with priority_list_info
		   [GSM(index=0)1X WCDMA LTE], GSM is first preferred to be acquired,
		   second is 1X, third is WCDMA, and LTE is last. */
   
   eoos_gwl_scan_s_type scan_type;
      /**< The type of scan to be used during 3GPP system acquisition. */
   
}sys_rat_pri_list_info_s_type;

/** @brief UE capability.
*/
typedef struct sys_mmss_ue_capability_s
{    
   sys_rat_pri_list_info_s_type    rat_pri_list_info;
     /**< RAT priority list information. */

} sys_mmss_ue_capability_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/*------------------------------------------------------------------------------
   Service Indicator Data Types
------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/** System ID type.
*/
typedef enum
{

  SYS_SYS_ID_TYPE_UNDEFINED,
    /**< System ID is undefined. */

  SYS_SYS_ID_TYPE_UMTS,
    /**< UMTS (WCDMA/GSM/LTE) system ID. */

  SYS_SYS_ID_TYPE_IS95,
    /**< IS-95 system ID. */

  SYS_SYS_ID_TYPE_IS856,
    /**< IS-856 (HDR) system ID. */

  SYS_SYS_ID_TYPE_WLAN
    /**< WLAN system ID. */

} sys_sys_id_type_e_type;

typedef enum 
{

  SYS_CELL_BROADCAST_CAPABILITY_NONE = -1,
    /**< Internal use only */   

  SYS_CELL_BROADCAST_CAPABILITY_UNKNOWN,
    /**< Cell broadcast capability not known */   

  SYS_CELL_BROADCAST_CAPABILITY_OFF,
    /**< Cell broadcast capability OFF */   

  SYS_CELL_BROADCAST_CAPABILITY_ON,
    /**< Cell broadcast capability ON */   

  SYS_CELL_BROADCAST_CAPABILITY_MAX
    /**< Internal use only */   
   
}sys_cell_broadcast_cap_e_type;
/*~ SENTINEL sys_cell_broadcast_cap_e_type.SYS_CELL_BROADCAST_CAPABILITY_MAX */


/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/*------------------------------------------------------------------------------
   System Identifier Data Types
------------------------------------------------------------------------------*/
/** @brief IS-95 system ID.
*/
typedef struct sys_is95_sys_id_s
{
  uint16         sid;
    /**< System ID. */

  uint16         nid;
    /**< Network ID. */

  word           mcc;       
    /**< Mobile County Code. */

  byte           imsi_11_12;
    /**< IMSI_11_12. */

} sys_is95_sys_id_s_type;

/** Public Land Mobile Network (PLMN) ID.

  A PLMN ID is defined within a GSM network to be the combination of a Mobile
  Country Code (MCC) and Mobile Network Code (MNC). A PLMN ID is stored in
  three octets, as specified in 3G TS 24.008, Section 10.5.1.3 and shown below:

  <pre>
                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|
  </pre>

  @brief The criteria for comparing one PLMN against another is specified in
  3GPP TS 23.122 Annex A.
*/
typedef struct sys_plmn_id_s
{

  byte                                identity[3];
    /**< Public Land Mobile Network ID. */

} sys_plmn_id_s_type;

/** Location Area Code (LAC).
*/
typedef word                          sys_lac_type;

/** @brief PLMN plus LAC.
*/
typedef struct sys_plmn_lac_id_s
{
  sys_plmn_id_s_type   plmn;  /**< Public Land Mobile Network. */
  sys_lac_type         lac;   /**< Location Area Code. */
}sys_plmn_lac_id_s_type;

/** @brief Type for system ID.
*/
typedef union sys_sys_id_u
{

  sys_is95_sys_id_s_type               is95;
    /**< IS-95 system ID. */

  byte                                 is856[16];
    /**< IS-856 system ID. */

  sys_plmn_id_s_type                   plmn;
    /**< UMTS (WCDMA/GSM) system ID. */

  sys_wlan_ssid_s_type                 ssid;
    /**< WLAN system ID. */

  sys_plmn_lac_id_s_type  plmn_lac;
    /**< UMTS (WCDMA/GSM) PLMN and LAC ID. */

} sys_sys_id_u_type;

/*~ PARTIAL sys_sys_id_u */
/*~ CASE SYS_SYS_ID_TYPE_UNDEFINED sys_sys_id_u.void */
/*~ CASE SYS_SYS_ID_TYPE_IS95      sys_sys_id_u.is95 */
/*~ CASE SYS_SYS_ID_TYPE_IS856     sys_sys_id_u.is856 */
/*~ CASE SYS_SYS_ID_TYPE_UMTS      sys_sys_id_u.plmn_lac */
/*~ CASE SYS_SYS_ID_TYPE_WLAN      sys_sys_id_u.ssid */

/** @brief Groups information for wireless network systems under acquisition.
*/
typedef struct sys_sys_id_s
{

  sys_sys_id_type_e_type              id_type;
    /**< Type of the system ID. */

  sys_sys_id_u_type                   id;
    /**< The system identifier. */

} sys_sys_id_s_type;
/*~ FIELD sys_sys_id_s.id DISC sys_sys_id_s.id_type */

/** Cell ID.
*/
typedef uint32                        sys_cell_id_type;

/** @brief Camped cell information.
*/
typedef struct sys_cell_info_s
{
  sys_cell_id_type         cell_id;
    /**< Cell ID. */

  sys_plmn_id_s_type       plmn_id;
    /**<  PLMN ID of the serving system. */

  sys_lac_type             lac_id;
    /**<  LAC of the serving system. */

  uint16                   arfcn;
    /**< Absolute Radio Frequency Channel Number (ARFCN) of the serving system
	     (applicable only in GSM mode). Range is 0 to 1023. */

  uint8                    bsic;
    /**< Base Station Identity Code of the serving system (applicable only
	     in GSM mode). Range is 0 to 63 (see 3GPP TS 24.008, section
		 10.5.1.3). */

  uint16                   psc;
    /**< The serving Primary Scrambling Code. */

  uint16                   uarfcn_dl;
    /**< Downlink the UMTS Terrestrial Radio Access (UTRA) Absolute Radio
         Freuency Channel Number (UARFCN) of the serving system. */

  uint16                   uarfcn_ul;         
    /**< Uplink the UARFCN of the serving system.  */

  uint16                   refpn;
    /**< Pseudo-random Noise (PN) number of the base station (applicable only
         in CDMA mode). */

  uint16                   earfcn_dl;
    /**< Downlink the Evolved-UTRA ARFCN (EARFCN) of the serving system. */

  uint16                   earfcn_ul;
    /**<  Uplink the EARFCN of serving system. */
               
}sys_cell_info_s_type;

/** Mobile country code type.
*/
typedef uint32                        sys_mcc_type;

/** Mobile network code type.
*/
typedef uint32                        sys_mnc_type;

/** Profile ID.
*/
typedef byte                          sys_profile_id_type;

#ifdef __cplusplus
extern "C" {
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME  sys_plmn_get_mcc_mnc
   
==============================================================================*/
/**
  Returns the MCC and MNC contained in a PLMN ID as integer values.

@note1hang This is a re-entrant function.

@param[in] plmn                       PLMN provided as the input from which
                                      to read the MCC and MNC.
@param[in] plmn_is_undefined_ptr      Flag indicating that the input PLMN
                                      contains a defined MCC and MNC
                                      (FALSE) or was set to 0xFFFFFF
                                      (TRUE).
@param[in] mnc_includes_pcs_digit_ptr Flag indicating whether the
                                      MNC included the third PCS digit
                                      (TRUE) or not (FALSE).
@param[in] mcc_ptr                    MCC converted from binary coded
                                      decimal digits in the PLMN to an
                                      integer.
@param[in] mnc_ptr                    MNC converted from binary coded
                                      decimal digits in the PLMN to an
                                      integer.

@return
  None.

@dependencies
  None.
*/
extern void sys_plmn_get_mcc_mnc
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
);
#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */


/** Wildcard MCC value.
*/
#define SYS_WILDCARD_MCC             0xFFF

/** Wildcard MNC value.
*/
#define SYS_WILDCARD_MNC             0xFFF

/** Maximum number of 3GPP RATs supported: GSM, WCDMA, and LTE.
*/
#define SYS_MAX_NUM_3GPP_RATS        3

/** Size of the PLMN with Active entry in the PLMN DB.
*/
#define SYS_PLMN_W_ACT_ENTRY_SIZE    5

/** Maximum network information list.
*/
#define SYS_NETWORK_LIST_INFO_MAX          (int)(25)

/** Minimum network information list.
*/
#define SYS_NETWORK_LIST_INFO_MIN          (int)(1)

/** Maximum number of user-preferred or controlled PLMNs.
*/
#define SYS_USER_PPLMN_LIST_MAX_LENGTH      85

/** Minimum number of user-preferred or controlled PLMNs.
*/
#define SYS_USER_PPLMN_LIST_MIN_LENGTH      1

/** Maximum length of the PLMN list.
*/
#define SYS_PLMN_LIST_MAX_LENGTH            40

/** Minimum length of the PLMN list.
*/
#define SYS_PLMN_LIST_MIN_LENGTH             1

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Network Lists Enums
------------------------------------------------------------------------------*/

/** Type of PLMN in the PLMN list.
*/
typedef enum
{
  /** @cond
  */
  SYS_DETAILED_PLMN_LIST_CATEGORY_NONE = -1,     /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN,
    /**< Home PLMN. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_PREFERRED,
    /**< Preferred PLMN category. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED,
    /**< User controlled PLMN (preferred) stored on the SIM. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED,
    /**< Operator controlled PLMN stored on the SIM. */

  SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER,
    /**< Other. */

  /** @cond
  */
  SYS_DETAILED_PLMN_LIST_CATEGORY_MAX            /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_detailed_plmn_list_category_e_type;
/*~ SENTINEL sys_detailed_plmn_list_category_e_type.SYS_DETAILED_PLMN_LIST_CATEGORY_MAX */

/** PLMN's signal quality.
*/
typedef enum
{
  /** @cond
  */
  SYS_SIGNAL_QUALITY_NONE = -1,                  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
  SYS_SIGNAL_QUALITY_HIGH,    /**< &nbsp; */
  SYS_SIGNAL_QUALITY_LOW,     /**< &nbsp; */

  /** @cond
  */
  SYS_SIGNAL_QUALITY_MAX                         /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_signal_quality_e_type;
/*~ SENTINEL sys_signal_quality_e_type.SYS_SIGNAL_QUALITY_MAX */

/** Access technologies that are defined for the elementary file,
    EFPLMNwAcT (user controlled PLMN selector with access technology). The
    #defines are to be used in a bitmask. Refer to 3GPP TS 11.11.
*/
typedef uint16 sys_access_tech_e_type;

/** Status of a manual PLMN search.
*/
typedef enum
{
  /** @cond
  */
  SYS_PLMN_LIST_NONE = -1,       /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_PLMN_LIST_SUCCESS,    /**< &nbsp; */
  SYS_PLMN_LIST_AS_ABORT,   /**< &nbsp; */
  SYS_PLMN_LIST_REJ_IN_RLF,

  /** @cond
  */
  SYS_PLMN_LIST_MAX              /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

} sys_plmn_list_status_e_type;
/*~ SENTINEL sys_plmn_list_status_e_type.SYS_PLMN_LIST_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Network Lists Data Types
------------------------------------------------------------------------------*/

/** PLMN's signal strength, in units that allow direct comparison of
    the signal strength between GSM and WCDMA cells.
*/
typedef int32 sys_signal_strength_type;

/** @brief PLMN information.
*/
typedef struct sys_detailed_plmn_list_info_s
{

  sys_plmn_id_s_type                        plmn;
    /**< PLMN ID. */

  sys_radio_access_tech_e_type              rat;
    /**< Radio Access Technology of the PLMN. */

  sys_srv_capability_e_type                 plmn_service_capability;
    /**< Type of service domain the PLMN can provide. */

  boolean                                   plmn_forbidden;
    /**< Whether the PLMN is forbidden. */

  sys_detailed_plmn_list_category_e_type    list_category;
    /**< Type of PLMN. */

  sys_signal_quality_e_type                 signal_quality;
    /**< Signal quality of the PLMN. */

  sys_signal_strength_type                  signal_strength;
    /**< Signal strength of the PLMN. */

} sys_detailed_plmn_list_info_s_type;

/** @brief Detailed PLMN list.
*/
typedef struct sys_detailed_plmn_list_s
{

  uint32                                    length;
    /**< The number of PLMNs in the list. */

  sys_detailed_plmn_list_info_s_type        info[SYS_PLMN_LIST_MAX_LENGTH];
    /**< PLMN information. */

} sys_detailed_plmn_list_s_type;
 
/*~ FIELD sys_detailed_plmn_list_s.info VARRAY SYS_PLMN_LIST_MAX_LENGTH
    LENGTH sys_detailed_plmn_list_s.length */

/** @brief Information regarding the user-preferred/controlled network. The
    access_tech_mask is a bitmask indicated by the sys_access_tech_e_type
    enumerates.
*/
typedef struct sys_user_pref_plmn_list_info_s
{
  sys_plmn_id_s_type                        plmn;
    /**< PLMN ID. */
  sys_access_tech_e_type                    access_tech;
    /**< Radio access technology of the PLMN. */

} sys_user_pref_plmn_list_info_s_type;

/** @brief List of user-preferred/controlled networks.
*/
typedef struct sys_user_pref_plmn_list_s
{

  byte                                      length;
    /**< Number of PLMNs in the list. */

  sys_user_pref_plmn_list_info_s_type       info[SYS_USER_PPLMN_LIST_MAX_LENGTH];
    /**< User preferred/controlled network. */

} sys_user_pref_plmn_list_s_type;
/*~ FIELD sys_user_pref_plmn_list_s.info VARRAY SYS_USER_PPLMN_LIST_MAX_LENGTH
    LENGTH sys_user_pref_plmn_list_s.length */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

#define sys_plmn_rat_mask_s_type sys_user_pref_plmn_list_s_type
#define WPA_RSN_MAX_PAIRWISE_CIPHER_SUITE  5
#define WPA_RSN_MAX_AKM_SUITE              2

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** WLAN security type.
*/
typedef enum
{
  SYS_TECH_WLAN_SEC_NONE = 0,    /**< &nbsp; */
  SYS_TECH_WLAN_SEC_WEP  = 1,    /**< &nbsp; */
  SYS_TECH_WLAN_SEC_WPA  = 2,    /**< &nbsp; */
  SYS_TECH_WLAN_SEC_WPA2 = 3     /**< &nbsp; */
} sys_wlan_security_e_type;

/** Cipher suite type.
*/
typedef enum
{
  SYS_WLAN_SEC_CIPHER_SUITE_WEP_40  = 1,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_TKIP    = 2,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_RSVD    = 3,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_CCMP    = 4,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_WEP_104 = 5,            /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_VENDOR_SPECIFIC = 6,    /**< &nbsp; */
  SYS_WLAN_SEC_CIPHER_SUITE_INVALID = 7             /**< &nbsp; */
} sys_wlan_cipher_suite_e_type;

/** Association and Key Management (AKM) suite type.
*/
typedef enum
{
  SYS_WLAN_SEC_AKM_SUITE_RSVD    = 0,            /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_8021X   = 1,            /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_PSK     = 2,            /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_VENDOR_SPECIFIC = 3,    /**< &nbsp; */
  SYS_WLAN_SEC_AKM_SUITE_INVALID = 4             /**< &nbsp; */
} sys_wlan_akm_suite_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief WLAN WPA security information.
*/
typedef struct sys_wlan_security_wpa_rsn_info_type
{

  sys_wlan_cipher_suite_e_type  grp_cipher_suite;
    /**< Cipher used for WLAN security. */
 
  uint16                        pairwise_cipher_count;
    /**< Total cipher pairs used. */

  sys_wlan_cipher_suite_e_type  pairwise_cipher_suites[WPA_RSN_MAX_PAIRWISE_CIPHER_SUITE];
    /**< List of cipher suites used in security. */
 
  uint16                        akm_count;
    /**< Number of AKM suites. */

  sys_wlan_akm_suite_e_type     akm_suites[WPA_RSN_MAX_AKM_SUITE];
    /**< Array of AKM suite.s */

} sys_wlan_security_wpa_rsn_info_s_type;
 
/** @brief WLAN security information.
*/
typedef struct sys_wlan_security_info_s
{
 
  sys_wlan_security_e_type                 sec_type;
    /**< Type of WLAN security. */

  sys_wlan_security_wpa_rsn_info_s_type    wpa_rsn_info;
    /**< WLAN WPA security information. */
  
} sys_wlan_security_info_s_type;

/** @brief BSS information in the WLAN.
*/
typedef struct sys_wlan_bss_info_s
{

  sys_chan_type                             chan;
    /**< Channel on which this BSS provides service. */
#ifndef FEATURE_GNSS_SA
  sys_band_class_e_type                     band;
    /**< Band in which this BSS provides service. */
#endif
  sys_wlan_bss_e_type                       bss_type;
    /**< Type of BSS: infrastructure or independent. */

  sys_wlan_bssid_type                       bssid;
    /**< BSS ID. */

  sys_wlan_ssid_s_type                      ssid;
    /**< SSID. */

  uint16                                    rssi;
    /**< Measured RSSI of the BSS. */

  sys_tech_e_type                           tech;
    /**< WLAN technology in use. */

  sys_wlan_security_info_s_type             security_info;
    /**< WLAN security in use. */

} sys_wlan_bss_info_s_type;

/** @brief BSS network list.
*/
typedef struct sys_wlan_bss_info_list_s
{

    int                            num_bss;
	  /**< Number of BSS in this list.  */

    sys_wlan_bss_info_s_type       bss_info[SYS_NETWORK_LIST_INFO_MAX];
	  /**< BSS list. */

}sys_wlan_bss_info_list_s_type;
/*~ FIELD sys_wlan_bss_info_list_s.bss_info VARRAY SYS_NETWORK_LIST_INFO_MAX
    LENGTH sys_wlan_bss_info_list_s.num_bss */


#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Mobility Management Info Defines
------------------------------------------------------------------------------*/
/*
Define constants used for mobility management information.
According to the standard, 3GPP TS 24.0008, the short name and
full name can be from 3 to N characters in length.
*/

/** @name Mobility Management Information Defines
    @{ */

#define SYS_MAX_SHORT_NAME_CHARS      255
#define SYS_MIN_SHORT_NAME_CHARS      1
#define SYS_MAX_FULL_NAME_CHARS       255
#define SYS_MIN_FULL_NAME_CHARS       1

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Network name coding scheme; the format is
    specified by 3GPP TS 24.008 and 03.38.
*/
typedef enum
{
  /** @cond
  */
  SYS_NW_NAME_CODING_SCHEME_NONE                             = -1,
    /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET  =  0,
    /**< Cell broadcast data coding, default alphabet, language unspecified. */

  SYS_NW_NAME_CODING_SCHEME_UCS2                             =  1,
    /**< UCS2 coding scheme. */

  SYS_NW_NAME_CODING_SCHEME_RESERVED                         =  2,
    /**< Reserved. */
  
  /** @cond
  */
  SYS_NW_NAME_CODING_SCHEME_MAX
    /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

} sys_network_name_coding_scheme_type_e_type;
/*~ SENTINEL sys_network_name_coding_scheme_type_e_type.SYS_NW_NAME_CODING_SCHEME_MAX */

/**  Local Service Area (LSA) identity; the format is specified by
     3GPP TS 24.008 and 23.003.
*/
typedef enum
{
  /** @cond
  */
  SYS_LSA_ID_TYPE_NONE         = -1,     /*   FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_LSA_ID_TYPE_PLMN_SIG_NUM =  0,
    /**< The LSA is a PLMN significant number. */

  SYS_LSA_ID_TYPE_UNIVERSAL    =  1,
    /**< The LSA is a universal LSA. */

  /** @cond
  */
  SYS_LSA_ID_TYPE_MAX                    /*   FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_lsa_identity_type_e_type;
/*~ SENTINEL sys_lsa_identity_type_e_type.SYS_LSA_ID_TYPE_MAX */

/** Daylight savings adjustment; the values for the enumerated constants
    are dictated by 3GPP TS 24.008.
**/
typedef enum
{
  /** @cond
  */
  SYS_DAY_SAV_ADJ_NONE          = -1,            /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_DAY_SAV_ADJ_NO_ADJUSTMENT =  0,  /**< &nbsp; */
  SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR =  1,  /**< &nbsp; */
  SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR =  2,  /**< &nbsp; */

  /** @cond
  */
  SYS_DAY_SAV_ADJ_MAX                            /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */
} sys_daylight_saving_adj_e_type;
/*~ SENTINEL sys_daylight_saving_adj_e_type.SYS_DAY_SAV_ADJ_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*------------------------------------------------------------------------------
   Mobility Management Info Data Types
------------------------------------------------------------------------------*/

/** @brief Network's short name and full name.
*/
typedef struct sys_network_short_name_s
{
  uint8                                       length;
    /**< Number of characters in the short name. */

  sys_network_name_coding_scheme_type_e_type  coding_scheme;
    /**< Coding scheme used to encode the short name of the network. */

  boolean                                     add_country_initials;
    /**< Flag that indicates whether the MS should add the country initials to
         the short name of the network. */

  uint8                                       name[SYS_MAX_SHORT_NAME_CHARS];
    /**< Short name of the network. */

  byte                                         spare_bits;
    /**< Number of spare bits in the end octet of the network short name. */

} sys_network_short_name_s_type;
 
/*~ FIELD sys_network_short_name_s.name VARRAY SYS_MAX_SHORT_NAME_CHARS
    LENGTH sys_network_short_name_s.length */

/** @brief Characteristics of the name of the given network.
*/
typedef struct sys_network_full_name_s
{
  uint8                                        length;
    /**< Number of characters in the full name. */

  sys_network_name_coding_scheme_type_e_type   coding_scheme;
    /**< The coding scheme used to encode the full name of the network. */

  boolean                                      add_country_initials;
    /**< Flag that indicates whether MS should add the country initials to
         the full name of the network. */

  uint8                                        name[SYS_MAX_FULL_NAME_CHARS];
    /**< The full name of the network.          */
    
  byte                                         spare_bits;
    /**< Number of spare bits in the end octet of the network full name. */
    
} sys_network_full_name_s_type;
/*~ FIELD sys_network_full_name_s.name VARRAY SYS_MAX_FULL_NAME_CHARS
    LENGTH sys_network_full_name_s.length */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* No information on LTM is present
** in 3.7.2.3.2.26 Sync Channel Message
** or HDR sector parameter message
** Could have also be named as NO_INFO. */

#define SYS_LTM_OFFSET_INVALID     0xFF /**< Land-to-Mobile information missing. */

/* No information on Leap seconds is available
** at present from Sync Channel Message
** or sector parameter message */

#define SYS_LEAP_SECONDS_NO_INFO   0xFF  /**< No information on leap seconds is
                                              available. */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Daylight savings information. 3.7.2.3.2.26 synch channel message only. This
    is not present in the HDR sector parameter message.
*/
typedef enum
{
  /** @cond
  */
  SYS_DAYLT_SAVINGS_NONE    = -1,
     /* INTERNAL USE */
  /** @endcond
  */

  SYS_DAYLT_SAVINGS_OFF = 0,     
    /**< Daylight savings is not in effect. */

  SYS_DAYLT_SAVINGS_ON = 1,
    /**< Daylight savings is in effect. */

  /** @cond
  */
  SYS_DAYLT_SAVINGS_MAX
     /* INTERNAL USE */
  /** @endcond
  */

} sys_daylt_savings_e_type;
/*~ SENTINEL sys_daylt_savings_e_type.SYS_DAYLT_SAVINGS_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

#define   SYS_DAYLT_SAVINGS_NO_INFO SYS_DAYLT_SAVINGS_OFF
#define   SYS_DAYLT_SAVINGS_INVALID SYS_DAYLT_SAVINGS_OFF

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief CDMA time-related information obtained from the synch
    channel message.
*/
typedef struct sys_cdma_time_s { 

  byte                     lp_sec;             
    /**< Number of leap seconds that have occurred since
         the start of system time. */
  
  byte                     ltm_offset; 
    /**< Offset of local time from system time (this is a twos-complement
         value). */

  sys_daylt_savings_e_type daylt_savings;   
    /**< Daylight savings indicator. */
 
} sys_cdma_time_s_type;   

/** @brief HDR system time obtained from the sector parameter
    message.
*/
typedef struct sys_hdr_time_s  
{
  byte lp_sec;   
    /**< Number of leap seconds that have occurred since
         the start of system time. */
          
  int16 ltm_offset;                      
    /**< Offset of local time from system time. */

}sys_hdr_time_s_type;

/** @brief CDMA and HDR time-related information.
*/
typedef union sys_time_info_u {

  sys_cdma_time_s_type    cdma_time;
    /**< Time obtained from the CDMA synch channel message. */

  sys_hdr_time_s_type     hdr_time;
    /**< Time obtained from the HDR sector parameter message. */

} sys_time_info_u_type;
/*~ IF (_DISC_ == SYS_SYS_MODE_CDMA ) sys_time_info_u.cdma_time */
/*~ IF (_DISC_ == SYS_SYS_MODE_HDR ) sys_time_info_u.hdr_time */
/*~ DEFAULT sys_time_info_u.void */

/** @brief CDMA/HDR time-related information. The sys_mode parameter acts as a
    discriminator to determine which time information is valid. If sys_mode is
    SYS_SYS_MODE_NO_SRV, the time information is invalid.
*/
typedef struct sys_time_info_s { 

  sys_sys_mode_e_type    sys_mode;
    /**< Mode (CDMA or HDR ) which last updated the time. */

  sys_time_info_u_type    time;
    /**< Union of CDMA and HDR time information structures. These are not the
         same, since HDR does not provide daylight savings information. */ 
  
} sys_time_info_s_type;
/*~ FIELD sys_time_info_s.time DISC sys_time_info_s.sys_mode */

/** Time zone, as an offset from universal time (i.e., the difference
    between local time and universal time), in increments of 15 minutes.
    The format for this type is specified in 3G TS 24.008 and 23.040.
*/
typedef int sys_time_zone_type;

/** @brief Time zone and universal time mobility management information.
    Each field is received as binary coded digits as specified in
    3GPP TS 24.008 and 23.040.
*/
typedef struct sys_time_and_time_zone_s
{
  uint8                                     year;       /**< Year field. */
  uint8                                     month;      /**< Month field. */
  uint8                                     day;        /**< Day field. */
  uint8                                     hour;       /**< Hour field. */
  uint8                                     minute;     /**< Minute field. */
  uint8                                     second;     /**< Second field. */
  sys_time_zone_type                        time_zone;  /**< Timezone field. */
} sys_time_and_time_zone_s_type;

/** @brief Local Service Area identity. The format
    is specified by 3GPP TS 24.008 and 23.003.
*/
typedef struct sys_lsa_identity_s
{
  sys_lsa_identity_type_e_type              type;
    /**< Localized Service Area identity. */
  uint8                                     identity[3];
    /**< Value of the LSA ID. */
} sys_lsa_identity_s_type;

/** @brief Currently registered network's mobility management information (i.e.,
    the network's name, date, time, and time zone).

    All fields in this structure are optional. If the "available" flag is
    TRUE, the corresponding data field contains valid information.
    If the "available" flag is FALSE, the corresponding data field
    should be ignored.
*/
typedef struct sys_mm_information_s
{
  /*
  Flags that indicate whether the particular mm information is
  available.
  */
  boolean                                   plmn_avail;
    /**< Whether the PLMN is available. */
  boolean                                   full_name_avail;
    /**< Whether the network full name is available. */
  boolean                                   short_name_avail;
    /**< Whether the network short name is available. */
  boolean                                   univ_time_and_time_zone_avail;
    /**< Whether the universal time is available. */
  boolean                                   time_zone_avail;
    /**< Whether the timezone is available. */
  boolean                                   lsa_identity_avail;
    /**< Whether the LSA ID is available. */
  boolean                                   daylight_saving_adj_avail;
    /**< Whether daylight saving information is available. */
  boolean                                   lac_avail;
   /**< Whether location area code (LAC) information is available. */


  sys_plmn_id_s_type                        plmn;
    /**< PLMN information. */
  sys_network_full_name_s_type              full_name;
    /**< Network full name. */
  sys_network_short_name_s_type             short_name;
    /**< Network short name. */
  sys_time_and_time_zone_s_type             univ_time_and_time_zone;
    /**< Universal Time Coordinated (UTC) time zone information. */
  sys_time_zone_type                        time_zone;
    /**< Current time zone information. */
  sys_lsa_identity_s_type                   lsa_identity;
    /**< LSA ID. */
  sys_daylight_saving_adj_e_type            daylight_saving_adj;
    /**< Daylight saving adjustment. */
  sys_lac_type                              lac_id;
    /**< LAC id. */
} sys_mm_information_s_type;
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** M511 table values.
*/
typedef enum
{
  /** @cond
  */
  SYS_JCDMA_M511_MIN = -1,   /*   used only for bounds checking           */
  /** @endcond
  */

  SYS_JCDMA_M511_PKT,
    /**< User-selected packet from the M511 menu. */

  SYS_JCDMA_M511_ASYNC_FAX,
    /**< User-selected async/fax from the M511 menu. */

  /** @cond
  */
  SYS_JCDMA_M511_MAX         /*   Used for arrays & bounds checking       */
  /** @endcond
  */

} sys_jcdma_m511_e_type;
/*~ SENTINEL sys_jcdma_m511_e_type.SYS_JCDMA_M511_MAX */

/** M512 table values.
*/
typedef enum
{
  /** @cond
  */
  SYS_JCDMA_M512_MIN = -1,   /*   Used only for bounds checking          */
  /** @endcond
  */

  SYS_JCDMA_M512_STD,
    /**< User-selected standard data rate. */

  SYS_JCDMA_M512_HIGH,
    /**< User-selected high data rate. */

  /** @cond
  */
  SYS_JCDMA_M512_MAX         /*   Used for arrays & bounds checking      */
  /** @endcond
  */

} sys_jcdma_m512_e_type;
/*~ SENTINEL sys_jcdma_m512_e_type.SYS_JCDMA_M512_MAX */

/** M513 table values.
*/
typedef enum
{
  /** @cond
  */
  SYS_JCDMA_M513_MIN = -1,   /*   Used only for bounds checking        */
  /** @endcond
  */

  SYS_JCDMA_M513_AUTO,       /**< AUTO setting. */
  SYS_JCDMA_M513_19200,      /**< 19200 bps. */
  SYS_JCDMA_M513_115200,     /**< 115200 bps. */
  SYS_JCDMA_M513_230400,     /**< 230400 bps. */

  /** @cond
  */
  SYS_JCDMA_M513_MAX         /*   Used for arrays & bounds checking    */
  /** @endcond
  */

} sys_jcdma_m513_e_type;
/*~ SENTINEL sys_jcdma_m513_e_type.SYS_JCDMA_M513_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Maximum size (in bytes) of the IS2000 message.
*/
#define    SYS_MAX_IS2000_MSG_SIZE     (256)

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/**
  UMTS-to-CDMA handover status causes.
*/
typedef enum
{
  SYS_UMTS_CDMA_HANDOVER_NONE,              /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_SUCCESS,           /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_NO_SRV,        /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_ACC_IN_PROG,   /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_IN_TC,         /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_UHDM_PARSE,    /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_CDMA_LOCK,     /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_OTHERS,        /**< &nbsp; */
  SYS_UMTS_CDMA_HANDOVER_ERR_BAD_MSG_TYPE,  /**< &nbsp; */

  /** @cond
  */
  SYS_UMTS_CDMA_HANDOVER_MAX
  /** @endcond
  */

}sys_umts_cdma_handover_e_type;
/*~ SENTINEL sys_umts_cdma_handover_e_type.SYS_UMTS_CDMA_HANDOVER_MAX */

/** UMTS-to-CDMA handover error statuses.
*/
typedef enum
{
  SYS_HO_STATUS_NONE,                       /**< &nbsp; */
  SYS_HDR_CDMA_HO_SUCCESS,                  /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_NO_SRV,               /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_ACC_IN_PROG,          /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_IN_TC,                /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_UHDM_PARSE,           /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_CDMA_LOCK,            /**< &nbsp; */
  SYS_HDR_CDMA_HO_ERR_OTHERS,               /**< &nbsp; */

  /** @cond
  */
  SYS_HO_STATUS_MAX
  /** @endcond
  */

} sys_ho_status_e_type;
/*~ SENTINEL sys_ho_status_e_type.SYS_HO_STATUS_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief IS2000 message.
*/
typedef struct {

  uint8                                type;
    /**< Message type. */

  uint32                               payload_len;
    /**< Message payload length in bits; maximum size
         is SYS_MAX_IS2000_MSG_SIZE * 8. */

  byte                                 payload[SYS_MAX_IS2000_MSG_SIZE];
    /**< Message payload. */

} sys_is2000_msg_s_type;
 
/*~ FIELD sys_is2000_msg_s_type.payload VARRAY SYS_MAX_IS2000_MSG_SIZE
    LENGTH ( sys_is2000_msg_s_type.payload_len / 8 ) */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*===============================================================================

  BCMCS - FEATURE_BCMCS

  BCMCS stands for "High rate BroadCast-MultiCast packet data air interface
  Specification"


==============================================================================*/

/*------------------------------------------------------------------------------
   BCMCS Defines
------------------------------------------------------------------------------*/
/** Maximum number of data flows supported. Remote functions should always use
    this value.
 */
#define SYS_BCMCS_MAX_FLOWS     40
       
/** Maximum number of monitored data flows.
*/
#define SYS_BCMCS_MAX_MON_FLOWS 10

/** Maximum number of data flows supported for BCMCS_2.0.
*/
#define SYS_BCMCS_20_MAX_FLOWS     40

/** Maximum number of monitored data flows for BCMCS_2.0.
*/
#define SYS_BCMCS_20_MAX_MON_FLOWS 10

/** Maximum number of data flows supported for BCMCS_1.0.
*/
#define SYS_BCMCS_10_MAX_FLOWS     10

/** Maximum number of data flows supported for non-BCMCS.
*/
#define SYS_BCMCS_MIN_FLOWS     1
/** Maximum number of monitored data flows for non-BCMCS.
*/
#define SYS_BCMCS_MIN_MON_FLOWS 1

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*-------------------------------------------------------------------------------
   BCMCS Enums
------------------------------------------------------------------------------*/

/** Broadcast/Multicast Specification (BCMCS) data service availability.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_SRV_STATUS_NONE = -1,
    /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_BCMCS_SRV_STATUS_NO_SRV,   /**< No BCMCS service is available.  */
  SYS_BCMCS_SRV_STATUS_SRV,      /**< BCMCS service is available.  */
 
  /** @cond
  */
  SYS_BCMCS_SRV_STATUS_MAX
    /*   FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_bcmcs_srv_status_e_type;
/*~ SENTINEL sys_bcmcs_srv_status_e_type.SYS_BCMCS_SRV_STATUS_MAX */

/** BCMCS flow status values.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_FLOW_STATUS_MIN = -1,
    /* For bounds checking only */
  /** @endcond
  */

  SYS_BCMCS_FLOW_STATUS_NONE = 0,
    /**< Used for initialization or to indicate that there is no flow status
	     information sent with the event or report. */

  /*---------------------------------------------------------------
    No-op
  ---------------------------------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_CANCELLED = 100,
    /**< A new flow request was received before the previous one is done
         processing. Cancel the remaining lookup and start processing
         the new request. */

  /*---------------------------------------------------------------
    Success - Flow is monitored  (DESIRED_MONITORED)
  ---------------------------------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_MONITORED = 200,
    /**< Flow is active and included in the broadcast overhead message. */

  /*---------------------------------------------------------------
    Successful request and db lookup, but the flow cannot be 
    monitored yet.

    Considered a temporary Failure - start a deregistration timer.
  ---------------------------------------------------------------*/
  /*-------------------------------------
    DESIRED_UNMONITORED
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR = 300,
    /**< Flow is available in overhead but cannot be monitored
         because its channel assignment conflicts with other flows. */

  /*-------------------------------------
    DESIRED_UNAVAILABLE
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_REQUESTED,
    /**< Registered but not included in broadcast overhead message. */

  SYS_BCMCS_FLOW_STATUS_TIMEOUT,
    /**< Broadcast supervision timeout. The Access Terminal (AT) is on a
	     system where broadcast is available. The AT waited for three times
		 BroadcastOverheadPeriod, and the flow did not appear in the BC OVHD
		 message. */

  SYS_BCMCS_FLOW_STATUS_LOST,
    /**< The AT lost acquisition and temporarily disabled its monitored
	     flows. It is as if the flows were not in the BC OVHD message, as
		 with SYS_BCMCS_FLOW_STATUS_REQUESTED. Another flow status will be
		 sent when the AT re-acquires. */

  SYS_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE,
    /**< The AT cannot process the flow request because BCMCS is in the
         CLOSED state (HDR not acquired). */

  SYS_BCMCS_FLOW_STATUS_REACHED_MAX_MON_FLOWS,
    /**< The flow is available in OVHD, but the AT cannot monitor it
         because it has reached the maximum number of flows it can 
         concurrently monitor. */

  /*---------------------------------------------------------------
    Failure (permanent) - flow should be deregistered.
  ---------------------------------------------------------------*/
  /*-------------------------------------
    AN is explicitely rejecting the flow
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_AVAILABLE = 400,
    /**< Flow was rejected explicitly by the network; the BCMCS flow ID
         and/or BCMCS program ID are not available. */

  SYS_BCMCS_FLOW_STATUS_REJECTED_NOT_TRANSMITTED,
    /**< Flow was rejected explicitly by the network; the BCMCS flow ID
         and/or BCMCS program ID were not transmitted. */

  SYS_BCMCS_FLOW_STATUS_REJECTED_INVALID_AUTH_SIG,
    /**< Flow was rejected explicitly by the network; invalid
         authorization signature. */

  /*-------------------------------------
    DB Lookup Failure
  -------------------------------------*/
  SYS_BCMCS_FLOW_STATUS_DB_UNAVAILABLE = 500,
    /**< A lookup in the XML file could not be completed because the database
         is in an invalid state. */  

  SYS_BCMCS_FLOW_STATUS_NO_MAPPING,
    /**< The XML file does not contain mapping for the current subnet. */

  SYS_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,
    /**< A lookup in the XML file cannot find a flow ID for
         the given multicast IP in the current subnet. */

  /** @cond
  */
  SYS_BCMCS_FLOW_STATUS_MAX
    /* For bounds checking only */
  /** @endcond
  */

} sys_bcmcs_flow_status_e_type;
/*~ SENTINEL sys_bcmcs_flow_status_e_type.SYS_BCMCS_FLOW_STATUS_MAX */

/** BCMCS flow status values.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_REG_STATUS_MIN = -1,  // For bounds checking only
  /** @endcond
  */

  SYS_BCMCS_REG_STATUS_NONE = 0,
    /**< Used for initialization or to indicate that there is no 
         registration status information sent with the event or report. */
  
  SYS_BCMCS_REG_STATUS_SUCCESS,
    /**< FlowRegistration message was sent successfully for this flow. */

  SYS_BCMCS_REG_STATUS_NOT_ALLOWED,
    /**< Could not send a FlowRegistration message; not allowed. */

  SYS_BCMCS_REG_STATUS_GEN_FAILURE,
    /**< Could not send a FlowRegistration message; generic failure. */

  /** @cond
  */
  SYS_BCMCS_REG_STATUS_MAX  // For bounds checking only
  /** @endcond
  */

} sys_bcmcs_reg_status_e_type;
/*~ SENTINEL sys_bcmcs_reg_status_e_type.SYS_BCMCS_REG_STATUS_MAX */

/** Flow service types.
*/
typedef enum 
{
  /** @cond
  */
  SYS_FLOW_SERVICE_NONE = -1,    /* For bounds checking only */
  /** @endcond
  */

  SYS_FLOW_SERVICE_BCMCS = 0,
    /**< Flow service type is BCMCS. */

  SYS_FLOW_SERVICE_MBMS = 1,
    /**< Flow service type is MBMS. */
  
  /** @cond
  */
  SYS_FLOW_SERVICE_MAX    /* For bounds checking only */
  /** @endcond
  */

} sys_flow_service_e_type;
/*~ SENTINEL sys_flow_service_e_type.SYS_FLOW_SERVICE_MAX */


/*------------------------------------------------------------------------------
   BCMCS Data Types
------------------------------------------------------------------------------*/

/** The supported IP address types; used to discriminate the union below
    (same as ip_addr_type from ps_ip_addr.h).
*/
typedef enum
{
  SYS_IP_ANY_ADDR     = 0,   /**< &nbsp; */
  SYS_IPV4_ADDR       = 4,   /**< &nbsp; */
  SYS_IPV6_ADDR       = 6,   /**< &nbsp; */
  SYS_IP_ADDR_INVALID = 255  /**< &nbsp; */

} sys_ip_addr_enum_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief IP address.
*/
typedef struct sys_ip_address_s
{
  sys_ip_addr_enum_type type;   /**< IPv4 or IPv6. */

  union {
    uint32 v4;    /**< Stores the address for IPv4. */
    uint64 v6[2]; /**< Stores the address for IPv6. */
  } addr;

} sys_ip_address_s_type;

 
/*~ IF ( (_DISC_ == SYS_IP_ANY_ADDR) ||
         (_DISC_ == SYS_IPV6_ADDR) ||
         (_DISC_ == SYS_IP_ADDR_INVALID) ) 
         sys_ip_address_s.addr.v6 */
/*~ IF ( _DISC_ == SYS_IPV4_ADDR )
         sys_ip_address_s.addr.v4 */
/*~ FIELD sys_ip_address_s.addr DISC sys_ip_address_s.type */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*---------------------------------------------------------------------------
                                MBMS
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) MACRO definitions.   
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

/** Maximum services allowed per request.
*/
#define SYS_MBMS_NUM_MAX_SRVS_PER_REQ 2

/* TMGI is a parameter used in 
** MBMS service act req. Gives
** the max length of TMGI.
*/

/** Maximum length of the Temporary Mobile Group ID.
*/
#define SYS_MAX_MBMS_TMGI_LENGTH 15

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*---------------------------------------------------------------------------
** MBMS enum definitions
---------------------------------------------------------------------------*/
/** Multimedia Broadcast/Multicast (MBMS) service status.
*/
typedef enum
{
  /** @cond
  */
  SYS_MBMS_SRV_STATUS_NONE = -1,
    /* For internal use only */
  /** @endcond
  */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_NETWORK_FAILURE,
    /**< MBMS service in unavailable due to a network failure. */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_SETUP_FAILURE,
    /**< MBMS service in unavailable due to a setup failure on the UE side. */

  SYS_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER,
    /**< MBMS service in unavailable due to another failure. */

  SYS_MBMS_SRV_STATUS_AVAILABLE,
    /**< MBMS service is available. */

  SYS_MBMS_SRV_STATUS_MONITORED,
    /**< MBMS service is monitored. */

  SYS_MBMS_SRV_STATUS_OTHER,
    /**< Reserved for future use. */
    
  SYS_MBMS_SRV_STATUS_RESERVED1,
    /**< Reserved for future use. */
    
  /** @cond
  */
  SYS_MBMS_SRV_STATUS_MAX  /* For internal use only */
  /** @endcond
  */

} sys_mbms_srv_status_e_type;
/*~ SENTINEL sys_mbms_srv_status_e_type.SYS_MBMS_SRV_STATUS_MAX */

/* To be discarded for phase 2a */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Kept for backwards compatibility. This structure has been superseded
    by sys_mbms_tmgi_uint_s_type.
*/
typedef struct
{ 

  uint32  service_id;      /**< Service ID. */
  boolean plmn_id_present; /**< Indicates whether PLMN ID is present. */
  uint32  plmn_id;         /**< PLMN ID if plmn_id_present is TRUE. */

} sys_mbms_tmgi_uint_old_s_type;

/** @brief MBMS logical channel ID.
*/
typedef struct
{

  boolean present;
  /**< TRUE  --  Logical channel ID is present.\n
       FALSE --  Logical channel ID is not present. */

  uint32  log_chan_id;
  /**< Logical channel ID; used between Data Services (DS) and Radio Link
       Control (RLC) on a traffic plane.  */

}sys_mbms_log_chan_id_s_type;

/** @brief TMGI (Temporary Mobile Group Identity) structure in array format.
*/
typedef struct
{

  boolean present;
    /**< TRUE if TMGI is present; FALSE otherwise. */

  uint8   length;
    /**< Length of TMGI. */

  uint8   value[SYS_MAX_MBMS_TMGI_LENGTH];
    /**< Actual TMGI value. */

}sys_mbms_tmgi_s_type;

/** @brief MBMS service priority.
*/
typedef struct
{

  boolean present;
    /**< TRUE if priority is present; FALSE otherwise. */

  uint32   priority;
    /**< Priority of a MBMS service request. */

}sys_mbms_srv_priority_s_type;

/** @brief MBMS information.
*/
typedef struct
{

  sys_mbms_tmgi_s_type              tmgi;
    /**< TMGI required for MBMS service. */

  sys_mbms_tmgi_uint_old_s_type     tmgi_conv;
    /**< Converted TMGI used by the MBMS manager. */

  sys_mbms_srv_priority_s_type      srv_priority;
    /**< Priority requested for MBMS. */

  sys_mbms_srv_status_e_type        srv_status;
    /**< Gives confirmation of MBMS activate or deactivate request. */

  sys_mbms_log_chan_id_s_type       srv_log_chan_id;
    /**< Logical channel ID; reported from the MBMS manager to the CM. */

}sys_mbms_srv_info_s_type;

/** @brief Flow ID structure. This type overloads the BCMCS flow ID structure
    to hold MBMS-specific information. This is done to resume
    cm_bcmcs_cmd_flow_request().
*/
typedef struct
{

  sys_ip_address_s_type ip_address;
    /**< IP address for the flow (MBMS and BCMCS). */

  uint16                port_number;
    /**< Port address for the flow (MBMS and BCMCS). */

  sys_flow_service_e_type  flow_service;
    /**< Identifies the type of flow (e.g., MBMS, BCMCS). */ 

  sys_mbms_srv_info_s_type mbms_srv_info;
    /**< Information specific to an MBMS service request. */

} sys_bcmcs_flow_addr_s_type;

/** @brief Flow status element type.
*/
typedef struct
{

  sys_bcmcs_flow_addr_s_type   flow_address;
    /**< Address of flow depending on BCMCS or MBMS. */

  sys_bcmcs_flow_status_e_type flow_status;
    /**< Status of flow depending on BCMCS or MBMS. */

} sys_bcmcs_flow_status_element_s_type;

/** @brief Status of a flow and its registration status.
*/
typedef struct {

  sys_bcmcs_flow_addr_s_type   flow_address;
    /**< Address of the flow, depending on whether BCMCS or MBMS. */

  sys_bcmcs_reg_status_e_type  reg_status;
    /**< Status of the registration. */

} sys_bcmcs_reg_status_element_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Registration setup.
*/
typedef enum
{
  /** @cond
  */
  SYS_BCMCS_REG_SETUP_MIN = -1,  /* For bounds checking only */
  /** @endcond
  */

  SYS_BCMCS_REG_SETUP_ALLOWED,
  /**< Registration setup allowed. */

  SYS_BCMCS_REG_SETUP_NOT_ALLOWED,
  /**< Registration setup not allowed. */

  /** @cond
  */
  SYS_BCMCS_REG_SETUP_MAX   /* For bounds checking only */
  /** @endcond
  */

}sys_bcmcs_reg_setup_e_type;
/*~ SENTINEL sys_bcmcs_reg_setup_e_type.SYS_BCMCS_REG_SETUP_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*===============================================================================

                     Manual selection types

==============================================================================*/

#define  SYS_MAX_MANUAL_CHANNELS        5

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Type of GW manual selection.
*/
typedef enum
{
  /** @cond
  */
  /* FOR INTERNAL USE ONLY! */
  SYS_MANUAL_SEL_TYPE_NONE = -1,
  /** @endcond
  */

  SYS_MANUAL_SEL_TYPE_USER_SPECIFIED,
    /**< System ID should be defined in the manual selection. */

  SYS_MANUAL_SEL_TYPE_HPLMN,
    /**< Select only the HPLMN. */

  SYS_MANUAL_SEL_TYPE_RPLMN,
    /**< Search for Registered PLMN (RPLMN) only. */

  /** @cond
  */
  SYS_MANUAL_SEL_TYPE_MAX,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_manual_sel_type_e_type;
/*~ SENTINEL sys_manual_sel_type_e_type.SYS_MANUAL_SEL_TYPE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief A manual system profile is stored in packed format in profile_buf.
    This must be unpacked with net_unpack_profile() to net_profile_s_type before
    use.
*/
typedef struct sys_manual_sys_profile_s{


  word                                   profile_size;
    /**< Size of the profile buffer. */

  byte                                   profile_buf[SYS_WLAN_PROFILE_MAX_SIZE];
    /**< Buffer to store the profile information. The valid length is
	     indicated by profile_size. */

} sys_manual_sys_profile_s_type;
 
/*~ FIELD sys_manual_sys_profile_s.profile_buf
    VARRAY SYS_WLAN_PROFILE_MAX_SIZE LENGTH sys_manual_sys_profile_s.profile_size */

/** @brief Parameters that define the WLAN manual system.
*/
typedef struct sys_wlan_manual_sys_info_s
{

  sys_wlan_bssid_type                    bssid;
    /**< BSS ID for WLAN manual selection; can be 0. */

  sys_chan_type                          channel;
    /**< Channel to search for BSS ID/System ID. If channel = 0, use any
         channel, otherwise it specifies the absolute channel for WLAN. */

  sys_band_mask_type                     band;
    /**< Band to use for manual system selection. */

  sys_profile_id_type                    profile_id;
    /**< Profile ID to use. */

  sys_wlan_bss_e_type                    bss_type;
    /**< BSS type. */

  sys_manual_sys_profile_s_type          profile;
    /**< Profile information for the manual system. */
  
} sys_wlan_manual_sys_info_s_type;

/** @brief Parameters that define the GSM/WCDMA manual system.
*/
typedef struct sys_gw_manual_sys_info_s
{

  sys_manual_sel_type_e_type             sel_type;
    /**< Manual selection type. */

} sys_gw_manual_sys_info_s_type;

/** @brief Parameters that define the CDMA manual system.
*/
typedef struct sys_cdma_manual_sys_info_s
{

  int                                    num_chans;
    /**< Number of channels; if num_chans = 0, use any applicable channel. */

  sys_chan_type                          chans[SYS_MAX_MANUAL_CHANNELS];
    /**< Channels to search for BSS ID/System ID. */

} sys_cdma_manual_sys_info_s_type;

/** @brief Parameters that define the manual system.
*/
typedef struct sys_manual_sys_info_s
{

  sys_sys_id_s_type                    sys_id;
    /**< System ID for manual selection; can also be wildcards. */

  union
  {
    sys_wlan_manual_sys_info_s_type    wlan;
	  /**< Additional parameters for WLAN manual system selection (applicable
           only if sys_id is WLAN). */

    sys_gw_manual_sys_info_s_type      gw;
	  /**< Additional parameters for GSM/WCDMA manual system
           selection (applicable only if sys_id is PLMN). */

    sys_cdma_manual_sys_info_s_type    cdma;
	  /**< Additional parameters for CDMA/HDR/AMPS manual system
           selection (applicable only if sys_id is IS95/IS856). */

  } prm;

} sys_manual_sys_info_s_type;
 
/*~ FIELD sys_manual_sys_info_s.prm DISC _OBJ_.sys_id.id_type */
/*~ CASE SYS_SYS_ID_TYPE_WLAN sys_manual_sys_info_s.prm.wlan */
/*~ CASE SYS_SYS_ID_TYPE_UMTS sys_manual_sys_info_s.prm.gw */
/*~ CASE SYS_SYS_ID_TYPE_IS95 sys_manual_sys_info_s.prm.cdma */
/*~ DEFAULT sys_manual_sys_info_s.prm.void */

/** @brief Dummy structure.
*/
typedef struct {

  int a;   /**< Dummy member. */

} sys_wlan_acq_fail_list_info_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Enumeration for Location Notification status.
*/
typedef enum
{
  /** @cond
  */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_NONE = -1, /* Internal CM use */
  /** @endcond
  */

  SYS_HDR_LOCATION_NOTIFICATION_STATUS_SUCCESSFUL,/**< LN was successful. */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_FAILED,    /**< LN failed. */

  /** @cond
  */
  SYS_HDR_LOCATION_NOTIFICATION_STATUS_MAX        /* Internal CM use */
  /** @endcond
  */

} sys_hdr_location_notification_status_e_type;
/*~ SENTINEL sys_hdr_location_notification_status_e_type.SYS_HDR_LOCATION_NOTIFICATION_STATUS_MAX */


/** Enumeration of no service cause. Indicates the cause when phone
    enters no service status. Carried in No Service indication.
*/
typedef enum
{
  /** @cond
  */
  SYS_NO_SVC_CAUSE_NONE = -1,  /* FOR INTERNAL USE ONLY */
  /** @endcond
  */

  SYS_NO_SVC_CAUSE_RLF,   
    /**< Radio link failure cause. */
    
  SYS_NO_SVC_CAUSE_NORMAL,   
    /**< Normal cause. */

  SYS_NO_SVC_CAUSE_IRAT_TO_LTE,   
    /**< pseudo no service returned by NAS due to reselection to LTE from
         3GPP2 RAT. */

  /** @cond
  */
  SYS_NO_SVC_CAUSE_MAX
    /* FOR INTERNAL USE ONLY */
  /** @endcond
  */
}sys_no_svc_cause_e_type; 
/*~ SENTINEL sys_no_svc_cause_e_type.SYS_NO_SVC_CAUSE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*===============================================================================

                     Statistics Types

==============================================================================*/

/*=*************************************************************
********************  STATS structures *************************
***************************************************************/

/*=**** STATS structures for CDMA 1X *****/

/** @name Statistics CDMA 1X Bitmask Values
    @{ */

#define SYS_CDMA_INFO_MASK_NONE	  	           0
#define SYS_CDMA_STATE_INFO_BIT_MASK	  	   SYS_BM_32BIT(0)
#define SYS_CDMA_CHANNEL_MEAS_INFO_BIT_MASK	   SYS_BM_32BIT(1)
#define SYS_CDMA_ACTIVE_SET_INFO_BIT_MASK      SYS_BM_32BIT(2)
#define SYS_CDMA_SERV_SYS_INFO_BIT_MASK        SYS_BM_32BIT(3)
#define SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK  SYS_BM_32BIT(4)
#define SYS_CDMA_CP_CALL_STAT_BIT_MASK	       SYS_BM_32BIT(5)
#define SYS_CDMA_REG_STAT_BIT_MASK	  	       SYS_BM_32BIT(6)
#define SYS_CDMA_INFO_MASK_ALL                 (SYS_BM_32BIT(7) - 1)
#define SYS_CDMA_MAX_ASET_PILOTS                   6

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** State of CDMA protocol.
*/
typedef enum sys_cdma_prot_state_e
{
  SYS_CDMA_PROT_INIT    = 1,      /**< Initial state. */
  SYS_CDMA_PROT_IDLE    = 2,      /**< Idle state. */
  SYS_CDMA_PROT_ACCESS  = 3,      /**< Access state. */
  SYS_CDMA_PROT_TRAFFIC = 4,      /**< Traffic state. */
  SYS_CDMA_PROT_EXIT    = 5       /**< Exit state. */
} sys_cdma_prot_state_e_type;

/** Status of the CDMA slotted mode.
*/
typedef enum sys_cdma_slotted_mode_e
{
  SYS_CDMA_UNSLOTTED_MODE   = 0,    /**< Unslotted mode. */
  SYS_CDMA_SLOTTED_MODE     = 1     /**< Slotted mode. */
} sys_cdma_slotted_mode_e_type;

/** Paging Channel (PCH) rate for CDMA.
*/
typedef enum sys_cdma_pch_rate_e
{
  SYS_CDMA_PCH_FULL_RATE   = 0,     /**< Full rate. */
  SYS_CDMA_PCH_HALF_RATE   = 1      /**< Half rate. */
} sys_cdma_pch_rate_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief CDMA Mobile Identification Number (MIN).
*/
typedef struct sys_cdma_min_s
{
  dword                   min1;  /**< 24 bits. */
  dword                   min2;  /**< 10 bits. */
} sys_cdma_min_s_type;

/** @brief State information for CDMA network and protocol.
*/
typedef struct sys_cdma_state_info_s
{
 sys_cdma_prot_state_e_type    cdma_prot_state;
   /**< Protocol state. */
 uint8                         band_class;
   /**< Band class supported. */
 sys_cdma_slotted_mode_e_type  slotted_mode;
   /**< Slotted mode or not. */
 sys_cdma_pch_rate_e_type      pch_rate;
   /**< Paging Channel rate. */
 uint16                        active_so;
   /**< Active Service Option. */
 sys_cdma_min_s_type           min;
   /**< Mobile Identification Number of current state. */
 int8                          ms_pref_sci;
   /**< Mobile Station preferred SCI (0-7). */
 uint8                         qpch_status;
   /**< Quick Paging Channel (QPCH) ON (1) or OFF (0). */   /* 1X L1 */
 uint8                   num_active_pilots;
   /**< Number of active pilots in traffic. */
}sys_cdma_state_info_s_type;

/** @brief Channel measurement information.
*/
typedef struct sys_cdma_channel_meas_s
{ 
  int16           rx_pwr;           /**< Rx instantaneous power in dBm. */
  int16           tx_pwr;           /**< Tx instantaneous power in dBm.*/
  uint16          pch_loss_count;   /**< Paging Channel loss count. */
  uint16          tch_loss_count;   /**< Traffic Channel loss count. */
  uint16          idle_ho_count;    /**< Idle hand off count. */
  uint16          acc_req_count;    /**< Number of access channel requests. */
  uint16          acc_rsp_count;    /**< Number of access channel responses. */
  uint16          failed_acc_att_count;  /**< Unsuccessful access attempts. */
  uint16          t41_expiry_count; /**< T41 timer expiration count. */
}sys_cdma_channel_meas_s_type;

/** @brief CDMA active set type.
*/
typedef struct sys_cdma_active_set
{
  uint16 pn;      /**< Pseudo-Noise. */
  uint16 ecio;    /**< Ec/Io. */
}sys_cdma_active_set_type;

/** @brief CDMA active set information.
*/
typedef struct sys_cdma_active_set_info_s
{
  sys_cdma_active_set_type   pilot_info[SYS_CDMA_MAX_ASET_PILOTS];
    /**< CDMA active set type list. */
  
}sys_cdma_active_set_info_s_type;

/** @brief Serving system information.
*/
typedef struct sys_cdma_serv_sys_info_s
{
  uint16                 pilot_pn_offset;
    /**< Pilot Pseudo-Noise offset. */
  uint16                 channel;
    /**< Channel. */
  uint8                  pagech;
    /**< Paging Channel. */
  uint16                 base_id;
    /**< Base station ID */
  uint16                 sid;
    /**< System ID. */
  uint16                 nid;
    /**< Network ID. */
  uint8                  packet_zone_id;
    /**< Packet zone ID. */
  uint8                  p_rev;
    /**< Protocol revision. */
  uint8                  min_p_rev;
    /**< Mobile Identification Number revision. */
  int8                   nw_pref_sci;
    /**< Network preferred SCI (0-7). */
}sys_cdma_serv_sys_info_s_type;

/** @brief Channel configuration information.
*/
typedef struct sys_cdma_channel_config_s
{
  uint16                 fsch0_mux;
    /**< Forward Supplemental Channel MUX. */

  uint8                  fsch0_rc;
    /**< Forward Supplemental Channel Radio Configuration. */

  boolean                fsch0_turbo_supported;
    /**< Whether turbo is supported. */

  uint16                 rsch0_mux;
    /**< Reverse Supplemental Channel MUX. */

  uint8                  rsch0_rc;
    /**< Reverse Supplemental Channel Radio Configuration. */

  boolean                rsch0_turbo_supported;
    /**< Reverse Supplemental Channel turbo. */

  boolean                fsch0_active;
    /**< Forward Supplemental Channel active. */

  uint8                  fsch0_duration;
    /**< Forward Supplemental Channel duration. */

  uint8                  fsch0_sccl_index;
    /**< Sample correlate, choose largest index. */

  uint8                  fsch0_num_pilots;
    /**< Number of pilots. */

  boolean                rsch0_active;
    /**< Reverse Supplemental Channel active. */

  uint8                  rsch0_duration;
    /**< Reverse Supplemental Channel duration. */

  uint8                  scrm_seq_num;
    /**< Sequence number. */

}sys_cdma_channel_config_s_type;

/** @brief Control Protocol call statistics.
*/
typedef struct sys_cdma_cp_call_stat_s
{
  uint16                 orig_acc_att_count;
  /**<  Origination access attempts count. */

  uint16                 page_rsp_acc_att_count;
  /**< Page response access attempts count. */

}sys_cdma_cp_call_stat_s_type;

/** @brief Registration status.
*/
typedef struct sys_cdma_reg_stat_s
{

  uint16                 powerdown_reg_count;
    /**< Power down registration count. */

  uint16                 timer_reg_count;
    /**< Timer-based registration count. */

  uint16                 parameter_reg_count;
    /**< Parameter-based registration count. */

  uint16                 powerup_reg_count;
    /**< Power up registration count. */

  uint16                 ordered_reg_count;
    /**< Ordered registration count. */

  uint16                 zone_reg_count;
    /**< Zone-based registration count. */

  uint16                 distance_reg_count;
    /**< Distance-based registration count. */

}sys_cdma_reg_stat_s_type;

/** @brief CDMA 1X payload structure.
*/
typedef struct sys_cdma_information_s
{
  uint32                                 changed_bitmask;
    /**< Bitmask of the changed field. */

  sys_cdma_state_info_s_type             state_info;
    /**< CDMA state information. */

  sys_cdma_channel_meas_s_type           meas_info;
    /**< Channel measurement information. */

  sys_cdma_active_set_info_s_type        active_set_info;
    /**< Active set information. */

  sys_cdma_serv_sys_info_s_type          serv_sys_info;
    /**< Serving system information. */

  sys_cdma_channel_config_s_type         channel_config_info;
    /**< Channel configuration information. */

  sys_cdma_cp_call_stat_s_type           call_stat;
    /**< Originations/terminations statistics. */

  sys_cdma_reg_stat_s_type               reg_stat;
    /**< Registration information. */

}sys_cdma_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for HDR -----*/

/* Bitmask values */

/** @name Statistics for HDR Bitmask Values
    @{ */

#define SYS_HDR_INFO_MASK_NONE	  	     0
#define SYS_HDR_INFO_MASK_ALL            SYS_BM_32BIT(0)

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** State of HDR protocol stack.
*/
typedef enum sys_hdr_prot_state_e
{
  SYS_HDR_PROT_INACTIVE  = 0, /**< Inactive state. */
  SYS_HDR_PROT_INIT      = 1, /**< Initialization state. */
  SYS_HDR_PROT_IDLE      = 2, /**< Idle state. */
  SYS_HDR_PROT_CONNECTED = 3  /**< Connected state. */

} sys_hdr_prot_state_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief HDR embodied information.
*/
typedef struct sys_hdr_information_s
{
  uint32                    changed_bitmask;
    /**< Indicates the fields that have changed. */

  sys_channel_type          channel;
    /**< Channel: band and frequency information. */

  sys_hdr_prot_state_e_type hdr_prot_state;
    /**< HDR protocol state: inactive, initialized, idle, or connected. */

  int16   c2i;
    /**< SINR of the serving sector in dB. */

  int32   rx_dbm;
    /**< Rx power in dBm. */

  int32   tx_dbm;
    /**< Tx power in dBm. */

  int16   ecio;
    /**< Ec/Io of reference sector in dB. */

  uint32  uati_24;
    /**< Three least significant bits of the Unique Access Terminal
	     Identifier (UATI). */

  uint8   color_code;
    /**< Color code for the UATI. */

  uint8   subnet_mask;
    /**< Subnet mask for the UATI. */

  uint16  mac_index;
    /**< Current MAC index. */

}sys_hdr_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for CM/SD -----*/

/* Bit mask values */

/** @name Statistics for CM/SD Bitmask Values
    @{ */

#define SYS_CM_INFO_MASK_NONE	  	    0
#define SYS_CM_PH_PREF_BIT_MASK             SYS_BM_32BIT(0)
#define SYS_CM_SYS_SEL_INFO_BIT_MASK        SYS_BM_32BIT(1)
#define SYS_CM_PRL_INFO_BIT_MASK            SYS_BM_32BIT(2)
#define SYS_CM_CALL_STAT_BIT_MASK           SYS_BM_32BIT(3)
#define SYS_CM_INFO_MASK_ALL                (SYS_BM_32BIT(4) - 1)

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

   /* For compatibility with QXDM database, please do not change the following
    fix assigned values. If new values are added please also update the QXDM
    database. */

/** PRL preferences.
*/
typedef enum sys_stats_prl_pref_e {

  /** @cond
  */
  SYS_STATS_PRL_PREF_NONE=0,   /* internal */
  /** @endcond
  */

  SYS_STATS_PRL_PREF_AVAIL_BC0_A = SYS_BM(SYS_PRL_AVAIL_BC0_A),
    /**< Acquire available system only on the B side. */

  SYS_STATS_PRL_PREF_AVAIL_BC0_B = SYS_BM(SYS_PRL_AVAIL_BC0_B),
    /**< Acquire available system only on the B side.  */

  SYS_STATS_PRL_PREF_ANY = 0x3FFF,
    /**< Acquire any available systems.  */

  SYS_STATS_PRL_PREF_NO_CHANGE,
    /**< Do not change the PRL preference.  */

  SYS_STATS_PRL_PREF_PERSISTENT,
    /**< Return to persistent preference setting.  */
  
  /** @cond
  */
  SYS_STATS_PRL_PREF_MAX   /* internal */
  /** @endcond
  */

} sys_stats_prl_pref_e_type;
/*~ SENTINEL sys_stats_prl_pref_e_type.SYS_STATS_PRL_PREF_MAX */

 /* For compatibility with QXDM database, please do not change the following
    fix assigned values. If new values are added please also update the QXDM
    database. */

/** Mode preferences.
*/
typedef enum sys_stats_mode_pref_e {

  /** @cond
  */
  SYS_STAT_MODE_PREF_NONE=-1,   /* internal */
  /** @endcond
  */

  SYS_STAT_MODE_PREF_AMPS_ONLY=0,
    /**< Service is limited to analog only (NV_MODE_ANALOG_ONLY). */

  SYS_STAT_MODE_PREF_DIGITAL_ONLY=1,
    /**< Service is limited to digital (CDMA, HDR, GW) only
	     (NV_MODE_DIGITAL_ONLY). */

  SYS_STAT_MODE_PREF_AUTOMATIC=2,
    /**< Determine the mode automatically (NV_MODE_AUTOMATIC). */

  SYS_STAT_MODE_PREF_EMERGENCY=3,
    /**< Emergency mode (NV_MODE_EMERGENCY). Intended to be used internally
	     by the CM.\ A client is not allowed to use it as parameter to change
		 the system selection preference command.  */

  /* For compatibility with QPST, do not change values or
  ** order. We start with NV_MODE_CELL_CDMA_ONLY+1 (i.e. 9).
  */
  SYS_STAT_MODE_PREF_CDMA_ONLY = 9,
    /**< Service is limited to CDMA only.  */

  SYS_STAT_MODE_PREF_HDR_ONLY=10,
    /**< Service is limited to HDR only. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_ONLY=11,
    /**< Service is limited to CDMA and AMPS only.  */

  SYS_STAT_MODE_PREF_GPS_ONLY=12,
    /**< Service is limited to GPS only. */

  /*
  ** The following enums are the radio access technologies for
  ** WCDMA and GSM.
  */
  SYS_STAT_MODE_PREF_GSM_ONLY=13,
    /**< Service is limited to GSM only. */

  SYS_STAT_MODE_PREF_WCDMA_ONLY=14,
    /**< Service is limited to WCMDA only. */

  /*
  ** Others
  */
  SYS_STAT_MODE_PREF_PERSISTENT=15,
    /**< Return to persistent mode preference. ph_ptr->mode_pref stores the
	     equivalent mapped CM type. */

  SYS_STAT_MODE_PREF_NO_CHANGE=16,
    /**< Do not change the mode preference. ph_ptr->mode_pref stores the
	     equivalent mapped CM type. */

  /*
  ** Even though logically the below belong with
  ** enums listed above, they were added at the end
  ** to not break compatibility with test tools
  */

  SYS_STAT_MODE_PREF_ANY_BUT_HDR=17,
    /**< Use any service but HDR.  */

  SYS_STAT_MODE_PREF_CURRENT_LESS_HDR=18,
    /**< Remove HDR component from current mode preference. ph_ptr->mode_pref
	     stores the equivalent mapped CM type. */

  SYS_STAT_MODE_PREF_GSM_WCDMA_ONLY=19,
    /**< Service is limited to GSM and WCDMA only. */

  SYS_STAT_MODE_PREF_DIGITAL_LESS_HDR_ONLY=20,
    /**< Acquire digital, non-HDR mode systems only (CDMA or GSM or WCDMA ). */

  SYS_STAT_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS=21,
    /**< Remove the HDR and AMPS component from the current mode preference.
	     ph_ptr->mode_pref stores the equivalent mapped CM type. */

  SYS_STAT_MODE_PREF_CDMA_HDR_ONLY=22,
    /**< Acquire only CDMA or HDR systems. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_HDR_ONLY=23,
    /**< Acquire only CDMA or AMPS or HDR systems. */

  SYS_STAT_MODE_PREF_CURRENT_LESS_AMPS=24,
    /**< Remove the AMPS component from the current mode preference.
	     ph_ptr->mode_pref stores the equivalent mapped CM type. */

  /* Clients check CM_API_WLAN for WLAN support for following
  ** preferences.
  */

  SYS_STAT_MODE_PREF_WLAN_ONLY=25,
    /**< Acquire only WLAN systems. */

  SYS_STAT_MODE_PREF_CDMA_WLAN=26,
    /**< Acquire only CDMA and WLAN systems. */

  SYS_STAT_MODE_PREF_HDR_WLAN=27,
    /**< Acquire only HDR and WLAN systems. */

  SYS_STAT_MODE_PREF_CDMA_HDR_WLAN=28,
    /**< Acquire only CDMA, HDR, and WLAN systems. */

  SYS_STAT_MODE_PREF_GSM_WLAN=29,
    /**< Acquire only GSM and WLAN systems. */

  SYS_STAT_MODE_PREF_WCDMA_WLAN=30,
    /**< Acquire only WCDMA and WLAN systems. */

  SYS_STAT_MODE_PREF_GW_WLAN=31,
    /**< Acquire only GSM/WCDMA and WLAN systems. */

  SYS_STAT_MODE_PREF_CURRENT_PLUS_WLAN=32,
    /**< Acquire WLAN systems in addition to the current system. */

  SYS_STAT_MODE_PREF_CURRENT_LESS_WLAN=33,
    /**< Remove WLAN systems from the current system. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY = 34,
    /**< Acquire CDMA, AMPS, HDR, and WLAN systems. */

  SYS_STAT_MODE_PREF_CDMA_AMPS_WLAN_ONLY = 35,
    /**< Acquire CDMA, AMPS, and WLAN systems. */

  SYS_STAT_MODE_PREF_INTERSECT_OR_FORCE = 36,
    /**< Acquire the mode that is common to (the intersection of) the
         current mode and the requested mode. If the intersection is NULL,
         force the requested preferences. */

  SYS_STAT_MODE_PREF_ANY_BUT_HDR_WLAN = 37,
    /**< Use any service except HDR and WLAN. */

  SYS_STAT_MODE_PREF_LTE_ONLY = 38,
    /**< Service is limited to LTE only. */

  SYS_STAT_MODE_PREF_TDS_ONLY = 39,
    /**< Service is limited to TD-SCMA only */

  SYS_STAT_MODE_PREF_TDS_GSM = 40,
    /**< Service is limited to TD-SCMA or GSM only */

  SYS_STAT_MODE_PREF_TDS_GSM_LTE = 41,
    /**< Service is limited to TD-SCDMA or GSM or LTE */
  
  /** @cond
  */
  SYS_STAT_MODE_PREF_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} sys_stats_mode_pref_e_type;
/*~ SENTINEL sys_stats_mode_pref_e_type.SYS_STAT_MODE_PREF_MAX */

/** System roam status.

  @note1hang The roaming indicator values (SYS_ROAM_STATUS_OFF and
  SYS_ROAM_STATUS_ON) that are defined in db.h are swapped compared to
  the PRL roaming indicator values that are defined in TSB-58.
*/
typedef enum {

  SYS_STAT_ROAM_OFF        = SYS_ROAM_STATUS_OFF,       
  /**< Roaming indicator is OFF. */

  SYS_STAT_ROAM_ON         = SYS_ROAM_STATUS_ON,        
  /**< Roaming indicator is ON. */

  SYS_STAT_ROAM_FLASH      = SYS_ROAM_STATUS_BLINK,     
  /**< Roaming indicator is flashing. */

  /** @cond
  */
  SYS_STAT_ROAM_MAX        = 0xFF
  /** @endcond
  */

} sys_stat_roam_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Phone information.
*/
typedef struct sys_cm_ph_info_s
{

 sys_stats_prl_pref_e_type      pref_sys;
  /**< Preferred system. */

 sys_stats_mode_pref_e_type     mode_pref;
  /**< Mode. */

 sys_band_mask_e_type           band_cap;
  /**< PCS/Cellular/AWS supported band capability. */

}sys_cm_ph_info_s_type;

/** @brief System selection information structure.
*/
typedef struct sys_cm_sys_sel_info_s
{

 sys_sys_id_s_type          sel_sys;
  /**< Selected system. */

 sys_stat_roam_e_type       roam_ind; 
  /**< Roaming indication. */

}sys_cm_sys_sel_info_s_type;

/** @brief CM payload.
*/
typedef struct sys_cm_information_s
{

  uint32                        changed_bitmask;
    /**< Bitmask. */

  sys_cm_ph_info_s_type         ph_info;
    /**< Phone information. */

  sys_cm_sys_sel_info_s_type    sys_sel_info;
    /**< System selection information. */

  word                          prl_ver; 
    /**< PRL version. */

  uint16                        send_key_orig_count;
    /**< Number of times the SEND key was pressed. */
  
}sys_cm_information_s_type;
 
/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for DS -----*/

/* Bit mask values */
/** @name Status for DS Bitmask Values
    @{ */

#define SYS_DS_INFO_MASK_NONE              0
#define SYS_DS_INFO_BIT_MASK               SYS_BM_32BIT(0)
#define SYS_DS_INFO_MASK_ALL               SYS_DS_INFO_BIT_MASK

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Forward and reverse link data.
*/
typedef struct sys_ds_hdr_link_info_s
{
  
  uint32 hdr_fwd_data_rate;
    /**< HDR forward link data rate in bytes/sec. */

  uint32 hdr_rev_data_rate;
    /**< HDR reverse link data rate in bytes/sec. */

}sys_ds_hdr_link_info_s_type;

/** @brief Data Services payload.
*/
typedef struct sys_ds_information_s
{

  uint32                   changed_bitmask;
  /**< Bitmask indicating which fields changed. */

  sys_ds_hdr_link_info_s_type  ds_hdr_link_info;
  /**< Forward and reverse link data. */

}sys_ds_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for NAS -----*/

/* NAS bitmask values */

/** @name Statistics for NAS Bitmask Values
    @{ */

#define SYS_NAS_INFO_MASK_NONE	  	    0
#define SYS_NAS_REGISTRATION_INFO_MASK  SYS_BM_64BIT(0)  
#define SYS_NAS_ATTACH_TYPE_MASK        SYS_BM_64BIT(1)
#define SYS_NAS_PERIODIC_TIMER_MASK  	SYS_BM_64BIT(2)
#define SYS_NAS_CS_REJECT_CAUSE_MASK  	SYS_BM_64BIT(3)
#define SYS_NAS_PS_REJECT_CAUSE_MASK  	SYS_BM_64BIT(4) 
#define SYS_NAS_PS_CALL_INFO_MASK       SYS_BM_64BIT(5)

/** All NAS statistics bitmask values.
*/
#define SYS_NAS_INFO_MASK_ALL           ( SYS_NAS_REGISTRATION_INFO_MASK | \
                                          SYS_NAS_ATTACH_TYPE_MASK       | \
                                          SYS_NAS_PERIODIC_TIMER_MASK    | \
                                          SYS_NAS_CS_REJECT_CAUSE_MASK   | \
                                          SYS_NAS_PS_REJECT_CAUSE_MASK   | \
                                          SYS_NAS_PS_CALL_INFO_MASK )

/** @} */

#define SYS_TMSI_SIZE 	4

#define SYS_PTMSI_SIZE 	SYS_TMSI_SIZE

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Registered network information.
*/
typedef struct sys_registered_nw_info_s
{
   byte        tmsi [SYS_TMSI_SIZE];        /**< TMSI value. */
   byte        ptmsi[SYS_PTMSI_SIZE];       /**< PTMSI value. */
   boolean     gprs_attach_status;          /**< Attached or detached. */
   sys_plmn_id_s_type  rplmn;               /**< PLMN info */
} sys_registered_nw_info_s_type;

/** @brief Periodic timer.
*/
typedef struct sys_nas_periodic_timers_s
{
  dword     T3212_value;
    /**< Periodic location update timer value (T3212). */

  dword     T3312_value;
    /**< Periodic Routing Area Update (RAU) timer value (T3312). */

} sys_nas_periodic_timers_s_type;

/** @brief Packet-Switched call information.
*/
typedef struct sys_ps_call_info_s
{
   pdp_address_T    pdp_address;  /**< Active PDP context: IP address. */
   qos_T            qos;          /**< Active PDP context: QoS. */
   apn_T            apn;          /**< Active PDP context: APN. */
 
}sys_ps_call_info_s_type;

/** @brief Non-Access Stratum information.
*/
typedef struct sys_nas_information_s
{
   uint16                         changed_bitmask;
     /**< Indicates changed information. */
   byte                           attach_type;
     /**< Attached type. */
   byte                           cs_reject_cause;
     /**< Mobility Management (MM) reject cause code. */
   byte                           ps_reject_cause;
     /**< GPRS MM (GMM) reject cause code. */
   sys_registered_nw_info_s_type  reg_nw_info;
     /**< Registered Network information. */
   sys_nas_periodic_timers_s_type periodic_timers;
     /**< Periodic timer */
   sys_ps_call_info_s_type        ps_call_info;
     /**< Active PS call information. */

} sys_nas_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*----- STATS structures for RRC -----*/

/** @name Statistics Bitmask Values
    @{ */

#define SYS_WCDMA_INFO_MASK_NONE                           0
#define SYS_WCDMA_SERVING_CELL_INFO_BIT_MASK               SYS_BM_64BIT(0)  
#define SYS_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK         SYS_BM_64BIT(1)  
#define SYS_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK  SYS_BM_64BIT(2)  
#define SYS_WCDMA_ACTIVE_SET_INFO_BIT_MASK                 SYS_BM_64BIT(3)  
#define SYS_WCDMA_MONITORED_SET_BIT_MASK                   SYS_BM_64BIT(4)  
#define SYS_WCDMA_RRC_STATE                                SYS_BM_64BIT(5) 

/** All statistics bitmask values.
*/
#define SYS_WCDMA_INFO_MASK_ALL (SYS_WCDMA_SERVING_CELL_INFO_BIT_MASK | \
                                 SYS_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK | \
                                 SYS_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK | \
                                 SYS_WCDMA_ACTIVE_SET_INFO_BIT_MASK | \
                                 SYS_WCDMA_MONITORED_SET_BIT_MASK | \
                                 SYS_WCDMA_RRC_STATE )

/** @} */

/** @name Modem Statistics Macro Definitions
    @{ */

#define SYS_WCDMA_INVALID_CELL_ID     0xFFFFFFFF        
#define SYS_WCDMA_INVALID_URA         0xFFFFFFFF
#define SYS_WCDMA_MAX_ASET_CELLS               6
#define SYS_WCDMA_MAX_MONITORED_FREQ           3
#define SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ 8
#define SYS_WCDMA_MAX_MONITORED_GSM_CELLS      8

/** @} */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/* Enums */

/** Network operation mode information.
*/
typedef enum
{
   SYS_NETWORK_OP_MODE_1,              /** &nbsp; *///!< SYS_NETWORK_OP_MODE_1
   SYS_NETWORK_OP_MODE_2,              /** &nbsp; *///!< SYS_NETWORK_OP_MODE_2
   SYS_NETWORK_OP_MODE_3,              /** &nbsp; *///!< SYS_NETWORK_OP_MODE_3
   SYS_PS_DOMAIN_SYS_INFO_UNAVAILABLE  /** &nbsp; *///!< SYS_PS_DOMAIN_SYS_INFO_UNAVAILABLE
} sys_network_op_mode_e_type;

/** WCDMA Radio Resource Control state type.
*/
typedef enum
{
  SYS_WCDMA_RRC_STATE_DISCONNECTED, /** &nbsp; */
  SYS_WCDMA_RRC_STATE_CELL_FACH,    /** &nbsp; */
  SYS_WCDMA_RRC_STATE_CELL_DCH,     /** &nbsp; */
  SYS_WCDMA_RRC_STATE_CELL_PCH,     /** &nbsp; */
  SYS_WCDMA_RRC_STATE_URA_PCH,      /** &nbsp; */
} sys_wcdma_rrc_state_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief WCDMA cell information.
*/
typedef struct
{
  uint16 psc;                 /**< Primary Scrambling Code. */
  int16 rscp;                 /**< Received Signal Code Power. */
  int16 ecio;                 /**< Ec/Io. */
} sys_wcdma_cell_info_s_type;

/** @brief WCDMA serving cell information.
*/
typedef struct
{

  uint16 uarfcn;
    /**< Serving cell DL UARFCN. */

  uint16 psc;
    /**< Serving cell primary scrambling code. */

  sys_plmn_id_s_type plmn_id;
    /**< PLMN ID. */

  sys_lac_type lac_id; 
    /**< Indicates the LAC. */
  
  uint16  rac_id;
    /**< Indicates the RAC. */

  uint32 ura;
    /**< UTRAN Registration Area (URA). */

  uint32 cell_id;
    /**< Indicates the service cell ID. */

  sys_network_op_mode_e_type nw_operation_mode;
    /**< Network operation mode. */

} sys_wcdma_serving_cell_info_s_type;

/** @brief WCDMA serving cell power information.
*/
typedef struct
{

  uint16 psc;
    /**< Serving cell Primary Scrambling Code. */

  int16 rscp;
    /**< Received Signal Code Power. */

  int16 ecio;
    /**< Ec/Io. */

}sys_wcdma_serving_cell_power_info_s_type;

/** @brief WCDMA active set reference Radio Link information.
*/
typedef struct
{

  uint16 uarfcn;
    /**< DL UARFCN. */

  uint16 psc;
    /**< Primary Scrambling Code. */

  sys_plmn_id_s_type plmn_id;
    /**< PLMN ID. */

  sys_lac_type lac_id;
    /**< Indicates the LAC. */
  
  uint16  rac_id;
    /**< Indicates the RAC. */

  uint32 cell_id;
    /**< Indicates the network cell. */
  
} sys_wcdma_active_set_reference_rl_info_s_type;

/** @brief WCDMA active set information.
*/
typedef struct sys_wcdma_active_set_info_s
{

  uint8 num_rls;
    /**< Number of Radio Links in the active set. */

  uint16 psc[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Primary Scrambling Codes. */

  uint32 cell_id[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Cell ID. */

  int16  rscp[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Received Signal Code Power. */

  int16 ecio[SYS_WCDMA_MAX_ASET_CELLS];
    /**< Ec/Io. */

} sys_wcdma_active_set_info_s_type;
/*~ FIELD sys_wcdma_active_set_info_s.psc
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */
/*~ FIELD sys_wcdma_active_set_info_s.cell_id
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */
/*~ FIELD sys_wcdma_active_set_info_s.rscp
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */
/*~ FIELD sys_wcdma_active_set_info_s.ecio
    VARRAY SYS_WCDMA_MAX_ASET_CELLS LENGTH sys_wcdma_active_set_info_s.num_rls */

/** @brief Frequency information of WCDMA monitored systems.
*/
typedef struct sys_wcdma_monitored_freq_info_s
{

  uint16 uarfcn;
    /**< UARFCN. */

  uint8 num_of_cells;
    /**< Number of cells in the frequency. */

  sys_wcdma_cell_info_s_type cell[SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ];
    /**< Cells information. */

} sys_wcdma_monitored_freq_info_s_type;
/*~ FIELD sys_wcdma_monitored_freq_info_s.cell
    VARRAY SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ LENGTH sys_wcdma_monitored_freq_info_s.num_of_cells */

/** @brief GSM cell information.
*/
typedef struct
{
  /*ARFCN*/
  uint16 arfcn;      /**< ARFCB; range 0-1023. */
  /*BSIC-NCC*/
  uint8 bsic_ncc;    /**< BSIC-NCC; range 0-7, 0XFF is invalid. */
  /*BSIC-BCC*/
  uint8 bsic_bcc;    /**< BSIC-BCC; range 0-7, 0XFF is invalid. */
  /*RSSI*/
  int16 rssi;        /**< RSSI. */
} sys_wcdma_gsm_cell_info_s_type;

/** @brief Monitored set information.
*/
typedef struct sys_wcdma_monitored_set_info_s
{

  uint8 num_of_freqs;
    /**< Number of frequencies. */

  sys_wcdma_monitored_freq_info_s_type freq[SYS_WCDMA_MAX_MONITORED_FREQ];
    /**< Frequency information. */

  uint8 num_gsm_cells;
    /**< Number of GSM cells. */  /* Range 0-WCDMA_MAX_MONITORED_GSM_CELLS */

  sys_wcdma_gsm_cell_info_s_type gsm_cell[SYS_WCDMA_MAX_MONITORED_GSM_CELLS];
    /**< GSM cell information. */

} sys_wcdma_monitored_set_info_s_type;
/*~ FIELD sys_wcdma_monitored_set_info_s.freq
    VARRAY SYS_WCDMA_MAX_MONITORED_FREQ LENGTH sys_wcdma_monitored_set_info_s.num_of_freqs */
/*~ FIELD sys_wcdma_monitored_set_info_s.gsm_cell
    VARRAY SYS_WCDMA_MAX_MONITORED_GSM_CELLS LENGTH sys_wcdma_monitored_set_info_s.num_gsm_cells */

/** @brief Generic event from the WCDMA stack.
*/
typedef struct
{

  sys_wcdma_serving_cell_info_s_type        wcdma_serving_cell_info;
    /**< Serving system cell information. */

  sys_wcdma_serving_cell_power_info_s_type  wcdma_serving_cell_power_info;
    /**< Power range of the serving cell. */

  sys_wcdma_active_set_reference_rl_info_s_type 
                                     wcdma_active_set_reference_rl_info;  
    /**< Active set reference Radio Link information. */

  sys_wcdma_active_set_info_s_type          wcdma_active_set_info;
    /**< Active set information. */

  sys_wcdma_monitored_set_info_s_type       wcdma_monitored_set_info;
    /**< Monitored set information. */

  sys_wcdma_rrc_state_e_type                wcdma_rrc_state;
    /**< Radio Resource Control state type. */

} sys_wcdma_event_info_type;

/** @brief Radio Resource Control message from the WCDMA stack.
*/
typedef struct
{

  uint64 changed_bitmask; 
    /**< Bitmask. */

  sys_wcdma_event_info_type wcdma_event_info;
    /**< Generic event from the WCDMA stack. */

} sys_wcdma_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/*---- RR structures ----*/


/* Bitmask values */
/** @name Radio Resource Bitmask Values
    @{ */

#define SYS_GERAN_ENG_MODE_NONE                0
#define SYS_GERAN_ENG_MODE_TIMING_ADVANCE      SYS_BM_64BIT(0) 
#define SYS_GERAN_ENG_MODE_NMR_INFO            SYS_BM_64BIT(1) 
#define SYS_GERAN_ENG_MODE_RX_QUAL             SYS_BM_64BIT(2) 
#define SYS_GERAN_ENG_MODE_TX_PWR              SYS_BM_64BIT(3) 
#define SYS_GERAN_ENG_MODE_SCELL_INFO          SYS_BM_64BIT(4) 
#define SYS_GERAN_ENG_MODE_SCELL_MEASUREMENTS  SYS_BM_64BIT(5) 
#define SYS_GERAN_ENG_MODE_AMR_INFO            SYS_BM_64BIT(6) 
#define SYS_GERAN_ENG_MODE_DTX_UL_STATUS       SYS_BM_64BIT(7) 
#define SYS_GERAN_ENG_MODE_TBF_INFO            SYS_BM_64BIT(8) 
#define SYS_GERAN_ENG_MODE_NW_PARAMS           SYS_BM_64BIT(9) 
#define SYS_GERAN_ENG_MODE_GPRS_UL_CS          SYS_BM_64BIT(10) 
#define SYS_GERAN_ENG_MODE_GPRS_DL_CS          SYS_BM_64BIT(11) 
#define SYS_GERAN_ENG_MODE_EGPRS_UL_MCS        SYS_BM_64BIT(12) 
#define SYS_GERAN_ENG_MODE_EGPRS_DL_MCS        SYS_BM_64BIT(13) 
#define SYS_GERAN_ENG_MODE_EDGE_MEAS           SYS_BM_64BIT(14) 
#define SYS_GERAN_ENG_SCELL_CONFIG             SYS_BM_64BIT(15) 
#define SYS_GERAN_ENG_HOPPING_STATUS           SYS_BM_64BIT(16) 

/** All radio resource bitmask values.
*/
#define SYS_GERAN_ENG_MODE_ALL ( SYS_GERAN_ENG_MODE_TIMING_ADVANCE | \
                       SYS_GERAN_ENG_MODE_NMR_INFO       | \
                       SYS_GERAN_ENG_MODE_RX_QUAL        | \
                       SYS_GERAN_ENG_MODE_TX_PWR         | \
                       SYS_GERAN_ENG_MODE_SCELL_INFO     | \
                       SYS_GERAN_ENG_MODE_SCELL_MEASUREMENTS | \
                       SYS_GERAN_ENG_MODE_AMR_INFO       | \
                       SYS_GERAN_ENG_MODE_DTX_UL_STATUS  | \
                       SYS_GERAN_ENG_MODE_TBF_INFO       | \
                       SYS_GERAN_ENG_MODE_NW_PARAMS      | \
                       SYS_GERAN_ENG_MODE_GPRS_UL_CS     | \
                       SYS_GERAN_ENG_MODE_GPRS_DL_CS     | \
                       SYS_GERAN_ENG_MODE_EGPRS_UL_MCS   | \
                       SYS_GERAN_ENG_MODE_EGPRS_DL_MCS   | \
                       SYS_GERAN_ENG_MODE_EDGE_MEAS      | \
                       SYS_GERAN_ENG_SCELL_CONFIG        | \
                       SYS_GERAN_ENG_HOPPING_STATUS )

/** @} */

#define SYS_ENG_MODE_MAX_NMR_ITEMS        6
#define SYS_ENG_MODE_TX_PWR_NUM_TIMESLOTS 8

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Engine mode cell callback cause type.
*/
typedef enum
{
  SYS_ENG_MODE_NO_CAUSE = 1          /**< &nbsp; *///!< SYS_ENG_MODE_NO_CAUSE

} sys_eng_mode_cell_cb_cause_e_type;

/** Reception type of a channel.
*/
typedef enum
{
  RX_LEVEL_TCH_PDTCH = 0,     /**< &nbsp; */
  RX_LEVEL_BCCH               /**< &nbsp; */

} sys_rx_level_channel_type_e_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Neighbor cell information.
*/
typedef struct
{
  uint32              cell_id; 
    /**< Cell ID. */

  /* range 0..65535, cell id not available: 0xffffffff 
     Data in sys_mode_ncell_id_info_s_type is invalid when cell_id invalid*/

  uint16              lac;
    /**< Location Area Code. */       /* range 0..65535 */

  sys_plmn_id_s_type  plmn;
    /**< PLMN ID. */

} sys_mode_ncell_id_info_s_type;

/** @brief Network Measurement Report.
*/
typedef struct
{
  uint16                   arfcn;
    /**< Absolute Radio Frequency Channel Number. */

  uint8                    bsic;
    /**< Base Station Identity Code. */

  uint8                    rxlev;
    /**< Receive level. */

  sys_mode_ncell_id_info_s_type  ncell_id;
    /**< Neighboring cell ID. */

} sys_nmr_element_s_type;

/** @brief Network Measurement Report engine mode.
*/
typedef struct sys_eng_mode_nmr_info_s
{
  uint8                     num_of_items; 
    /**< Number of NMRs. Set to zero if NMR information is invalid or there are
	    no neighbors to report. */

  sys_nmr_element_s_type     nmr[SYS_ENG_MODE_MAX_NMR_ITEMS];
    /**< NMR array. */

  time_type                time_stamp;
    /**< Indicates when information was collected. */

} sys_eng_mode_nmr_info_s_type;
/*~ FIELD sys_eng_mode_nmr_info_s.nmr
    VARRAY SYS_ENG_MODE_MAX_NMR_ITEMS LENGTH sys_eng_mode_nmr_info_s.num_of_items */

/** @brief Serving cell information (currently camped cell). Describes the
    identity of the serving cell.
*/
typedef struct
{
  uint32                   cell_id; 
    /**< Cell ID. Range is 0 to 65535; cell ID not available = 0xffffffff 
         (all data in eng_mode_scell_info_t is invalid when cell_id is invalid). */

  sys_lac_type             lac;
    /**< Location Area Code. */                      /* range 0..65535 */

  sys_plmn_id_s_type       plmn;
    /**< Public Land Mobile Network. */

  uint16                   arfcn;
    /**< Absolute Radio Frequency Channel Number. */     /*range 0..1023 */

  uint8                    bsic;
    /**< Base Station Identity Code. */             /*range 0…63, see 24.008 10.5.1.3*/

  sys_eng_mode_cell_cb_cause_e_type cell_info_cause;
    /**< Cause code of the serving cell. */

  uint16                   rac;     
    /**< RAC (not used). */        /*range 0..255, invalid=0xffff (i.e. GPRS not supported)*/
  
} sys_eng_mode_scell_info_s_type;

/** @brief Serving cell measurements, including signal strength and quality.
*/
typedef struct
{
  int32                   C1;
    /**< C1 as defined in 45.008 6.4, default: 0. */

  int32                   C2;
    /**< C2 as defined in 45.008 6.4, default: 0. */

  uint16                  rx_lev; 
    /**< RX_LEV range is 0 to 63. See 3GPP TS 45.008 Section 8.1.4; default = 0. */

  uint16                  rx_qual_sub;
    /**< Reception quality in dedicated mode. The range is 0 to 7; 0xffff is
         invalid. */

  sys_rx_level_channel_type_e_type channel_type; 
    /**< Indicates channel rx_lev is measured on. */

} sys_eng_mode_scell_measurements_s_type;

/** @brief Dynamic information about the current Adaptive Multirate (AMR)
    voice call.
*/
typedef struct
{
  uint8   ul_chan_mode; /**< Uplink channel mode.  */

  uint8   dl_chan_mode; /**< Downlink channel mode: \n
                              0 -- Full rate.\n
                              1 -- Half rate.\n
                              2 -- Enhanced full rate.\n
                              3 -- AMR full rate.\n
                              4 -- AMR half rate.\n
                              0xFF -- Invalid mode. */

  uint8   ul_acs[4];    /**< Uplink Active Code Set; AMR coding group. */

  uint8   dl_acs[4];    /**< 0 -- 4.75 kbit/s.\n
                             1 -- 5.15 kbit/s.\n
                             2 -- 5.90 kbit/s.\n
                             3 -- 6.70 kbit/s.\n
                             4 -- 7.40 kbit/s.\n
                             5 -- 7.95 kbit/s.\n
                             6 -- 10.2 kbit/s.\n
                             7 -- 12.2 kbit/s.\n
                             0xFF -- Undefined.  */

  boolean dl_dtx;       /**< 1 = On, 0 = Off; default = 0. */

  uint16  dl_c_over_i;  /**< Range 0..63, 0xFFFF -- Invalid. */

} sys_eng_mode_amr_info_s_type;

/** @brief Dynamic information about the currently active EGPRS Token Bucket
    Filter (TBF).
*/
typedef struct
{
  uint8 mac_mode;       /**< As specified in 3GPP TS 4.060 chapter 11.2.7.2.\n
                             0 -- Dynamic Allocation.\n
                             1 -- Extended Dynamic Allocation.\n
                             0xff -- Invalid. */

  uint8  tbf_mode;      /**< 1 -- GPRS.\n
                             2 -- EGPRS.\n
                             0xff -- Invalid. */

} sys_eng_mode_tbf_info_s_type;

/** @brief Network parameters, such as Network Mode of Operation (NMO), which
    typically apply to an operator.
*/
typedef struct
{
  uint8 nc_mode;
    /**< Network Control mode; relates to how the mobile sends measurements and
	     performs reselection:
          - 0 -- NC1 mode.
          - 1 -- NC2 mode.
          - 0xff -- Invalid. */
 
  uint8 nmo;
    /**< Network Mode of Operation:
         - 0 -- NMO I.
         - 1 -- NMO II.
         - 2 -- NMO III.
         - 0xff -- Invalid. */

} sys_eng_mode_nw_params_s_type;

/** @brief Quality measurements taken while an EGPRS TBF is active.
*/
typedef struct
{

  uint8 lq_meas_mode;   
    /**< Link Quality measurement mode. */
    /* spec 4.60 chap 11.2.7.2 ref for detail values 0 to 3. 0xff = invalid */
  
  uint8 bep_period;      
    /**< Bit Error Probability measured during EGPRS TBF. */
    /* spec ref 4.60 cha 12.24.1 and 5.08  chap 10.2.3.2.1 0xff = invalid */

} sys_eng_mode_edge_meas_s_type;

/** @brief Serving cell information that describes the configuration data of
    the cell.
*/
typedef struct
{

  uint8     pbcch_present;
    /**< Presence of PBCCH in cell:
	     - 0 -- No.
	     - 1 -- Yes.
	     - 0xff -- Invalid. */

} sys_eng_mode_scell_config_s_type;

/** @brief Determines whether hopping is enabled or disabled.
*/
typedef struct
{
  uint8    hopping_status;
    /**< Hopping status:
	     - 0 -- No.
	     - 1 -- Yes.
	     - 0xff -- Invalid. */
} sys_eng_mode_hopping_status_s_type;

/** @brief Final Radio Resource (RR) structure.
*/
typedef struct
{

  uint64                     changed_bitmask;
    /**< Indicates the updated values in GSM. */

  uint32                     timing_advance_value; 
    /**< Time offset; changes with the mobile distance from the base station. */
              
  sys_eng_mode_nmr_info_s_type nmr_info; 
    /**< Network Measurement Report engine mode. */

  uint32                     rx_qual; 
    /**< Reception quality. */  /*0xffffffff - Invalid*/

  int16                   tx_pwr_in_use[SYS_ENG_MODE_TX_PWR_NUM_TIMESLOTS]; 
    /**< Transmission power in use. */                      /*0 - Invalid*/

  sys_eng_mode_scell_info_s_type      scell_info;
    /**< Serving cell information (currently camped cell). */

  sys_eng_mode_scell_measurements_s_type  scell_measurements;
    /**< Serving cell measurements, including signal strength and quality. */

  sys_eng_mode_amr_info_s_type   amr_info;
    /**< Dynamic information about the current AMR voice call. */

  uint8                      dtx_ul_status;
    /**< Discontinuous Transmission status. */    /*0 - Off, 1 - On, 0xff - Invalid*/

  sys_eng_mode_tbf_info_s_type   tbf_info;
    /**< Dynamic information about the currently active EGPRS TBF. */

  sys_eng_mode_nw_params_s_type  nw_params;
    /**< Network parameters, such as NMO, which apply to an operator. */

  uint8                      gprs_ul_cs;
    /**< GPRS coding scheme used in Upload GPRS TBFs. */      /* Range 0-3, 0xff - invalid */

  uint8                      gprs_dl_cs;
    /**< GPRS coding scheme used in Download GPRS TBFs. */    /* Range 0-3, 0xff - invalid */

  uint8                      egprs_ul_mcs;
    /**< EGPRS coding scheme used in Upload EGPRS TBFs. */    /* Range 0-8, 0xff - invalid */

  uint8                      egprs_dl_mcs;
    /**< EGPRS coding scheme used in DL EGPRS TBFs. */        /* Range 0-8 , 0xff - invalid */

  sys_eng_mode_edge_meas_s_type       edge_meas;
    /**< Quality measurements taken while an EGPRS TBF is active. */

  sys_eng_mode_scell_config_s_type    scell_config;
    /**< Serving cell information that describes the configuration data of the
	     cell. */

  /*ENG_MODE_HOPPING_STATUS*/
  sys_eng_mode_hopping_status_s_type  hopping_status;
} sys_gsm_information_s_type;

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** Modem functional modules.
*/
typedef enum
{
  /** @cond
  */
  SYS_MODEM_MODULE_NONE = -1,   /* For CM internal use only */
  /** @endcond
  */

  SYS_MODEM_MODULE_NAS = 0,
    /**< NAS. */

  SYS_MODEM_MODULE_GSM = 1,
    /**< RR. */

  SYS_MODEM_MODULE_WCDMA = 2,
    /**< RRC. */

  SYS_MODEM_MODULE_CDMA  = 3,
    /**< CDMA - 1X. */

  SYS_MODEM_MODULE_HDR   = 4,
    /**< HDR. */

  SYS_MODEM_MODULE_CM    = 5,
    /**< CM/SD. */

  SYS_MODEM_MODULE_DS    = 6,
    /**< DS. */

  /** @cond
  */
  SYS_MODEM_MODULE_MAX   /* For CM internal use only */
  /** @endcond
  */

} sys_modem_module_e_type;
/*~ SENTINEL sys_modem_module_e_type.SYS_MODEM_MODULE_MAX */

/** @} */ /* end_addtogroup SI */

/** @addtogroup SI Service Indicators
    @{ */

/** @brief Payload to be sent to clients.
*/
typedef struct sys_modem_stats_info_s
{
  sys_modem_module_e_type type;   /**< Module responsible for generating a
								       report. */

  union
  {
    sys_nas_information_s_type    nas_info;   /**< NAS structure. */
    sys_gsm_information_s_type    gsm_info;   /**< GSM structure. */ 
    sys_wcdma_information_s_type  wcdma_info; /**< RRC structure. */
    sys_cdma_information_s_type   cdma_info;  /**< CDMA 1X structure. */
    sys_hdr_information_s_type    hdr_info;   /**< HDR structure. */
    sys_cm_information_s_type     cm_info;    /**< CM/SD structure. */
    sys_ds_information_s_type     ds_info;    /**< DS structure. */
  }prm;
   
} sys_modem_stats_info_s_type;
/*~ FIELD sys_modem_stats_info_s.prm DISC sys_modem_stats_info_s.type */
/*~ CASE SYS_MODEM_MODULE_NAS sys_modem_stats_info_s.prm.nas_info */
/*~ CASE SYS_MODEM_MODULE_GSM sys_modem_stats_info_s.prm.gsm_info */
/*~ CASE SYS_MODEM_MODULE_WCDMA sys_modem_stats_info_s.prm.wcdma_info */
/*~ CASE SYS_MODEM_MODULE_CDMA  sys_modem_stats_info_s.prm.cdma_info */
/*~ CASE SYS_MODEM_MODULE_HDR   sys_modem_stats_info_s.prm.hdr_info */
/*~ CASE SYS_MODEM_MODULE_CM    sys_modem_stats_info_s.prm.cm_info */
/*~ CASE SYS_MODEM_MODULE_DS    sys_modem_stats_info_s.prm.ds_info */
/*~ DEFAULT sys_modem_stats_info_s.prm.void */

/** @} */ /* end_addtogroup SI Service Indicators */

/** WLAN security information **/

typedef enum sys_wlan_sec_param_e
{
  SYS_WLAN_SEC_PARAM_NONE     = -1,
  /* For CM internal use only */
  SYS_WLAN_SEC_PARAM_ACQ_FAIL = 0,
  /* ACQ_FAIL */
  SYS_WLAN_SEC_PARAM_SECURITY_INFO = 1,
  /* SECURITY INFO */
  SYS_WLAN_SEC_PARAM_MAX 
  /* For CM internal use only */
}sys_wlan_sec_param_e_type;

typedef enum sys_wlan_acquire_fail_reason_e
{
  SYS_WLAN_ACQ_FAIL_REASON_NONE = 0,
  /* NONE */
  SYS_WLAN_ACQ_FAIL_REASON_GENERIC_FAILURE = 1,
  /* GENERIC_FAILURE */
  SYS_WLAN_ACQ_FAIL_REASON_AUTH_REJECTED = 2,
  /* AUTH_REJECTED */
  SYS_WLAN_ACQ_FAIL_REASON_UNREACHABLE = 3,
  /* UNREACHABLE */
  SYS_WLAN_ACQ_FAIL_REASON_SEC_FAILED = 4,
  /* SEC_FAILED */
  SYS_WLAN_ACQ_FAIL_REASON_MAX
  /* For CM internal use only */  
}sys_wlan_acquire_fail_reason_e_type;

/** Payload to be sent to clients **/
typedef struct sys_wlan_sec_params_s
{
  sys_wlan_sec_param_e_type    type;

  union
  {
    sys_wlan_acquire_fail_reason_e_type   err_cause;
    /* Connection fail error cause */
    sys_wlan_security_info_s_type         sec_info;
    /* Security info */
  }cmd;

}sys_wlan_sec_params_s_type;
/*~ FIELD sys_wlan_sec_params_s.cmd DISC sys_wlan_sec_params_s.type */
/*~ CASE SYS_WLAN_SEC_PARAM_ACQ_FAIL     sys_wlan_sec_params_s.cmd.err_cause */
/*~ CASE SYS_WLAN_SEC_PARAM_SECURITY_INFO sys_wlan_sec_params_s.cmd.sec_info */
/*~ DEFAULT sys_wlan_sec_params_s.cmd.void */


#define MAX_AS_IDS 2

/**
** Enumeration of Active Subscription ID's 
** Note: WMS APIs are a client of this enum
*/
typedef enum {

  SYS_MODEM_AS_ID_NONE = -1,
    /**< Internal Only */

  SYS_MODEM_AS_ID_1,
    /**< Subscription ID 1 */

  SYS_MODEM_AS_ID_2,
    /**< Subscription ID 2 */

  SYS_MODEM_AS_ID_NO_CHANGE,
    /**< No Change in Subscription ID */

  SYS_MODEM_AS_ID_MAX
    /**< Internal Only */

} sys_modem_as_id_e_type;
/*~ SENTINEL sys_modem_as_id_e_type.SYS_MODEM_AS_ID_MAX */


/** Enumeration of Dual Standby Preferences
*/
typedef enum {

  SYS_MODEM_DS_PREF_NONE,
    /**< Internal use, range checking */

  SYS_MODEM_DS_PREF_SINGLE_STANDBY,
    /**<
    ** Phone is in Single Standby mode
    */

  SYS_MODEM_DS_PREF_DUAL_STANDBY,
    /**<
    ** Phone is in Dual Standby mode
    */

  SYS_MODEM_DS_PREF_NO_CHANGE,
    /**<
    ** Do not change the Dual Standby preference.
    ** should be mapped to the existing value.
    ** Only temporary value and should never be written to NV.
    */

  SYS_MODEM_DS_PREF_DUAL_STANDBY_NO_TUNEAWAY,
    /**<
    ** Phone is in Dual Standby mode with no tune away.
    */

  SYS_MODEM_DS_PREF_AUTO,
    /**<
    ** Standby mode is determined by the number of subscriptions
    */

  SYS_MODEM_DS_PREF_AUTO_NO_TUNEAWAY,
  /**<
  ** Standby mode is determined by the number of subscriptions.
  ** With no tuneaway. 
  */

  SYS_MODEM_DS_PREF_MAX
    /**< Internal use for range checking 
    ** should never be written to NV.
    */

} sys_modem_dual_standby_pref_e_type;
/*~ SENTINEL sys_modem_dual_standby_pref_e_type.SYS_MODEM_DS_PREF_MAX */

/** @brief Cell Broadcast information
*/
typedef struct sys_cb_info_s
{
  sys_modem_as_id_e_type   subscription_id;
    /**< subscription id. */

  sys_cell_broadcast_cap_e_type   cell_bc_capability;
    /**< Cell broadcast capability. */
  
}sys_cell_broadcast_info_s_type;

boolean sys_srv_status_is_srv
(
  sys_srv_status_e_type  value
);
/** @brief Speech codec types
*/
#ifdef FEATURE_GNSS_SA
typedef enum {
   SYS_SPEECH_CODEC_NONE = 0x0000,
   SYS_SPEECH_CODEC_QCELP13K,
   SYS_SPEECH_CODEC_EVRC,
   SYS_SPEECH_CODEC_EVRC_B,
   SYS_SPEECH_CODEC_EVRC_WB,
   SYS_SPEECH_CODEC_EVRC_NW,
   SYS_SPEECH_CODEC_AMR_NB,
   SYS_SPEECH_CODEC_AMR_WB,
   SYS_SPEECH_CODEC_GSM_EFR,
   SYS_SPEECH_CODEC_GSM_FR,
   SYS_SPEECH_CODEC_GSM_HR,
   SYS_SPEECH_CODEC_MAX = 0x7FFF
} sys_speech_codec_e_type;
typedef enum
{
  /** @cond
  */
  SYS_CS_LCS_NONE = -1,   /*FOR INTERNAL USE ONLY! */
  /** @endcond
  */
  
  SYS_CS_LCS_INFO_NOT_AVAILBLE,
    /**< no information about support of location services 
         via CS domain is available */
    
  SYS_CS_LCS_NOT_SUPPORTED    ,
    /**< location services via CS domain not supported */

  SYS_CS_LCS_SUPPORTED,
    /*< location services via CS domain supported */

  /** @cond
  */
  SYS_CS_LCS_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

}sys_cs_lcs_val_e_type;
#endif
#endif /* #ifndef SYS_H */
