#ifndef CUSTCGPS_H
#define CUSTCGPS_H
/*===========================================================================

            " C u s t - C G P S "   H E A D E R   F I L E

DESCRIPTION
  Configuration for GPS Feature.

Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/cust/inc/custcgps.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/10/15   mj      Enable W+W  FEATURE_CGPS_DUAL_WCDMA
09/30/14   md      Disable dynamic memory allocation from HLOS heap for RUMI
08/28/13   rh      Add FEATURE_GNSS_XTRA3 featurization
02/05/13   kj      Add FEATURE_CGPS_DOG_HEARTBEAT featurization for DIME 1.0
05/17/11   rc      Enabling Secure Position Reports through QMI
06/12/08   bp      Enabling serial PDAPI in ThinUI builds
03/31/08   kj      Created from the old custcgpsi.h file
===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                            gpsOne Features
---------------------------------------------------------------------------*/


#ifdef FEATURE_CGPS
  #ifdef T_APQ8064
    #define FEATURE_GNSS_GSS
  #endif

  #ifndef FEATURE_GNSS_SA
    /* Cell DB feature */
    #define FEATURE_CGPS_CELLDB
  #endif

  #ifdef FEATURE_CGPS_USES_CDMA
    // Removed because securemsm code is not available yet
    //#define FEATURE_GPSONE_SECURITY_OPEN_STANDALONE
    //#define FEATURE_TM_SECURITY

    #define FEATURE_CGPS_CDMA_IF

    #if defined( FEATURE_SRCH_CGPS_IF )
      #define FEATURE_SRCH_CGPS_IF_V2
    #endif /* FEATURE_SRCH_CGPS_IF */

    #ifdef FEATURE_JCDMA_GPSONE
      #define FEATURE_CGPS_EVDO_IF
    #endif /* FEATURE_JCDMA_GPSONE */

    /* Define CDMA specific Cell DB feature */
    #ifdef FEATURE_CGPS_CELLDB
      #define FEATURE_CGPS_CDMA_CELLDB
    #endif /* FEATURE_CGPS_CELLDB */

    #define FEATURE_GPSONE_VX_LCS_AGENT
    #define FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF


  #endif /* FEATURE_CGPS_USES_CDMA*/


  #ifdef FEATURE_CGPS_USES_UMTS
    /* Following definitions are needed in .builds file.
      FEATURE_CGPS_WCDMA                      - for targets that support WCDMA.
      FEATURE_CGPS_GSM                        - for targets that support GSM.
      FEATURE_CGPS_WCDMA and FEATURE_CGPS_GSM - for targets that support WCDMA
                                                and GSM. */

    #ifdef FEATURE_DUAL_WCDMA
	  #define FEATURE_CGPS_DUAL_WCDMA
	#endif
	
    #ifdef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS

      #ifdef FEATURE_CGPS_UMTS_UP
        #define FEATURE_CGPS_UMTS_UP_SUPL
      #endif /* FEATURE_CGPS_UMTS_UP*/

      #ifdef FEATURE_CGPS_UMTS_CP
        #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
          /* special behaviors */
          #define FEATURE_SS_RELEASE_COMPLETE_BEFORE_FACILITY_MESSAGE_ALLOWED /* releaseComplete before FACILITY allowed */
          //#define FEATURE_EXTENDED_RESP_TIME     /* extend RRC reportingIntervalLong */
          //#define FEATURE_REDUCE_PDAPI_POSREP_RESOLUTION_TO_GAD /* reduce pdapi_pos_report resolution to that of GAD */
        #endif /* FEATURE_CGPS_UMTS_CP_WCDMA */

        #define FEATURE_GPS_MO_LR
      #endif /* FEATURE_CGPS_UMTS_CP */

      //#define FEATURE_STALE_POS_ALLOWED
      #ifdef FEATURE_STALE_POS_ALLOWED
        #define FEATURE_RET_REFPOS_UPON_NOFIX  /* send reference position at timeout */
        #define FEATURE_POPULATE_NMEA_WITH_CELL_ID_POS /* cell id being either ref.
                                                        or cell_id fix. Non-CGPS'
                                                        don't have _POS in feature name */
      #endif /* FEATURE_STALE_POS_ALLOWED */

      /* if build supports TDS, GPS may enable a proprietary SUPL1 support*/
      #ifdef FEATURE_CGPS_USES_TDS /* defined in .build */
        #define FEATURE_SUPL1_TDS
      #endif

    #endif  /* FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS */

    /* Enable Rude-Timetag Feature */
    //#define FEATURE_RUDE_TIMETAG

    /* Define UMTS specific Cell DB feature */
    #ifdef FEATURE_CGPS_CELLDB
      #define FEATURE_CGPS_UMTS_CELLDB
    #endif /* FEATURE_CGPS_CELLDB */
  #endif  /* FEATURE_CGPS_USES_UMTS */

  #define FEATURE_GPSONE_HW_AGC

  /* Feature to keep track of Aries change out side of MGP module */
  //????????????? REMOVE?   #define FEATURE_GPSONE_ARIES_CHANGE

  /* Aries MC Unit test feature */
  //????????????? REMOVE? #define FEATURE_GPSONE_ARIES_MC_UNIT_TEST

  #define FEATURE_GPSONE_OVERRIDE_RDA

  #define FEATURE_GPS_HIGHEST_TRM_CHAIN1_PRIORITY
  
  #ifndef FEATURE_GNSS_SA
    #define FEATURE_CGPS_PDCOMM
    #define FEATURE_PDSM_ATL
    #define FEATURE_CGPS_PDCOMM_CLIENT
  #endif

  #ifdef FEATURE_CGPS_PDCOMM
    /*GPS PDCOMM TCP task on modem processor*/
    #define FEATURE_CGPS_PD_COMMS_TCP_ON_MODEM
  #endif /* FEATURE_CGPS_PDCOMM */

  /* Enable GPSONE Stitching feature */
  #define FEATURE_GPSONE_STITCH

  /* Enable GPSONE MGP PE-specific features */
  #define FEATURE_GPSONE_ARIES_MGP_PE

  #define FEATURE_GPSONE_IGNORE_DORMANT_TIMER
  //#define FEATURE_GPSONE_VX_LCS_AGENT_WITH_HANDOFF
  //#define FEATURE_GPSONE_VX_LCS_AGENT

  /* Enable internal IPC memory buffer bookkeeping for ipcmem debugging */

  #define FEATURE_CGPS_REL30

  #ifndef FEATURE_GNSS_LEAN_BUILD
    #define FEATURE_CGPS_FILE_SYSTEM /* Define the CGPS file system feature for FFA7200A/UNDP1 platforms */
  #endif /* !FEATURE_GNSS_LEAN_BUILD */

  
  #define FEATURE_CGPS_XTRA /* Disable the XTRA feature */
  
  /* Enable Sensor-KF feature */
  /* #define FEATURE_CGPS_SENSOR_KF */


  #define FEATURE_NF_KF_STATIC_MODE_ENH  /* time lag patch */
  #define FEATURE_PRR_DEEPER_REJECT

  /* Define this to have PGI as a separate task (used in Task/TMC)  */
  #define FEATURE_CGPS_PGI_TASK

  /* Enable Frequency bias learning feature */
  #define FEATURE_FREQ_BIAS_LEARNING

  /*
     Set this feature for QDSP6 like processors with multiple HW threads,
     which don't support INTLOCKs
   */
  #ifdef FEATURE_QDSP6
    #define FEATURE_INTLOCK_UNSUPPORTED

    /* MIMO operation on 8200 resulted in crashes because of time tag processing in GPS even when GPS
     * was OFF. Enabling this feature means that the important Time tag functions
     * return without doing anything.
     * When time tag is re-enabled on 8200 with work arounds for INTLOCKs, those changes must be
     * merged to Callisto before this feature is disabled. To make the code look clean, we should
     * even revert back the code wrapped in this feature in mc_timetag.c
     */
    //#define FEATURE_CGPS_DISABLE_ALL_TIMETAG_PROCESSING
  #endif /* FEATURE_QDSP6 */

  #ifdef FEATURE_LTE
    #define FEATURE_CGPS_LTE_CELLDB
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_QDSP6
    #define FEATURE_MODEM_MISC_CFG0_ABSENT
  #endif /* FEATURE_QDSP6 */

  /* Enable gpsOne App functionality*/
  #define FEATURE_IPOSDETEXT

  /* Enable the XTRA 2 feature */
  #define FEATURE_GNSS_XTRA2

  /* Enable the XTRA 3 feature */
  #define FEATURE_GNSS_XTRA3

/* Gen8A/B */
/* VER4 is Gen8A and most GPS functionality is there */
/* VER5 is Gen8B and an extension of Gen8A and needs VER4 to be defined */
#define FEATURE_GNSS_NAV_HW_VER4
#define FEATURE_GNSS_NAV_HW_VER5

#ifdef FEATURE_WDOG_DYNAMIC
  #define FEATURE_GNSS_WDOG_DYNAMIC
#endif /*FEATURE_WDOG_DYNAMIC*/


/*Disabling FEATURE_GNSS_IMEM & FEATURE_GNSS_IMEM_DEDICATED, Current version of DIME doesn't support GPS IMEM*/
//#define FEATURE_GNSS_IMEM
//#define FEATURE_GNSS_IMEM_DEDICATED

#ifndef FEATURE_GNSS_LEAN_BUILD
  #define FEATURE_GNSS_RELOCATABLE_MODEM
#endif /* !FEATURE_GNSS_LEAN_BUILD */


#ifdef FEATURE_LTE
  #define FEATURE_LOC_LTE_OTDOA
  #define FEATURE_LTE_ML1_POS_TIMEXFER
#endif /* FEATURE_LTE */

#ifdef FEATURE_TDSCDMA
  #ifdef FEATURE_CGPS_CELLDB
    #define FEATURE_CGPS_TDSCDMA_CELLDB
  #endif /* FEATURE_CGPS_CELLDB */
#endif /* FEATURE_TDSCDMA */

/* GNSS API to provide GNSS time to TCXO MGR for XO Aging*/
#define FEATURE_XO_AGING_GNSS_TIME_API
/* LTE TX power indication */
#define FEATURE_GNSS_LTE_TX_POWER_INDICATION

/* During RF Repartition transition period, this feature definition is needed
   for PGI.  Supported targets should be added here.  After RF Repartition is
   complete, this feature definition should be removed. */
#define FEATURE_NAVRF

/* EFS_COMPRESS is disabled on all targets until stack overflow in ZLIB is debugged */
#undef FEATURE_CGPS_EFS_COMPRESS

#ifdef FEATURE_MODEM_WDOG_HB
#define FEATURE_CGPS_DOG_HEARTBEAT
#endif

/* FEATURE_PMIC_RTC was mainlined, so mainlining our code using this #define */
#define FEATURE_GNSS_PMIC_RTC

/* Feature to enable logging timetag events for debugging */
#define FEATURE_TIMETAG_DEBUG_BUFFER

/* Feature to indicate that GPIO 50 will be used for WLAN Tx IMD indication */
#define FEATURE_GNSS_IMD_GPIO_50

/* Temp Compensation will be handled by GPS module. 
** Until that time, TCXO will handle it. */
#ifdef FEATURE_MCS_TCXO_REFACTORED
  /* This should be mainlined when all PLs have support for this */
  #define FEATURE_GNSS_TCXOMGR
#else
  #undef FEATURE_GNSS_TCXOMGR
#endif /**/

/* Enable PDR */
#define FEATURE_GNSS_PDR

#endif /* FEATURE_CGPS */
#endif /* CUSTCGPS_H */
