#ifndef CUSTMODEM_H
#define CUSTMODEM_H
/*=========================================================================== 

                           C U S T U S U R F

DESCRIPTION
  Customer file for the MSM6280 UMTS (GSM + WCDMA) SURF build.

  Copyright (c) 1996-2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/cust/custmodem.h#1 $ $DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $

 * 
 *    Rev 1.1   16 Jul 2002 12:57:32   balas
 * Updated for the initial DM build UZ1000.
 * 
 *    Rev 1.0   07 May 2002 13:58:22   balas
 * Initial revision.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/21/10   dlb'    Renaming as custmodem.h for 8960, removing all non-modem definitions
06/10/10   akb     Define FEATURE_NETWORK_SHARING for SU_MODEM_WCDMA_MDM8220.01.20.00 and
                   remove FEATURE_ERR_NV_LOG as it's intended only for ARM9
12/22/08    amitp  Enable FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2 
02/27/07    pma    Enable FEATURE_NAS_KLOCWORK requested by NAS SU#51
02/27/07    gfr    Moved FEATURE_KEYPAD_OKTS to custdrivers.h
02/20/07    bliu   add nas features
08/21/06    kvk    undef FEATURE_DSM_MEM_CHK_EXPENSIVE to save MIPS
08/04/06    hs     undefined FEATURE_WSLEEP_AWAKE_TIME_OPT and FEATURE_SLP_CLK_PHASE_2_CAL
                   (part of WCDMA SU #32)
                   defined FEATURE_G2W_SRCH_LOGGING_ENABLED (part of GSM sync up to 4025E)
06/23/06    kvk    Disabled FEATURE_LSM_PCH_REPORTING
06/07/06    kvk    Define #define FEATURE_UMTS_NASCC, FEATURE_LSM_PCH_REPORTING,
                   FEATURE_PREFERRED_MODE_CHANGE
06/07/06    kvk    Define #define FEATURE_UMTS_PDCP
05/31/06    pp     Enable FEATURE_SLEEP_GSM
05/04/06    hn     Removed FEATURE_I2C_SKIP_CLK_RESET and FEATURE_I2C_SKIP_CLK_HALT
03/07/06    dp     Defined some WCDMA features left out in the bringup
01/11/06    pp     Enable FEATURE_DUAL_RFR
12/08/05    pp     Remove FEATURE_GSM_ROTATOR_SUPPORT. Already present in custgsmdrv.h
12/07/05    pp     Enable FEATURE_RF_HAS_L1_INTERFACE
12/01/05    pp     Remove some features for cleanup
12/01/05    pp     Enable FEATURE_GW_MANUAL_PLMN_IN_PS_CALL
11/30/05    pp     Correct FEATURE_REL_ID_NV_SUPP to FEATURE_REL_ID_NV_SUPPORT
11/29/05    pp     Enable FEATURE_WTOW_BACKGROUND_PLMN_SEARCH, FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
11/29/05    pp     Enable FEATURE_CIPHER_IND and FEATURE_GW_ELIST_TFT_PCO
11/29/05    pp     Enable FEATURE_INTERRAT_PCCO_GTOW and FEATURE_REL_ID_NV_SUPP
11/18/05   rams    Enable GSM sleep features
11/14/05    pp     Enable FEATURE_GTOW_DONT_PASS_PSC_INFO
10/17/05   rams    Enabled FEATURE_GSM_GSAC_CLK_OPT
08/26/05   ttl     Enable FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
10/08/05   rams    Added FEATURE_GSM_TX_CLK_OPT from Graham
09/26/05   rams    Took from 6275/build/cust
08/09/05   niru    Corrected duplication of FEATURE_RUNTIME_RFSETUP_BUF_SIZE
08/09/05   niru    Define FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER, FEATURE_RUNTIME_RFSETUP_BUF_SIZE 
08/03/05   niru    Define FEATURE_RUNTIME_RFSETUP_BUF_SIZE 
07/21/05    sr     Feature management related to slow rex ticks - 
                   FEATURE_SLOW_REX_TICKS_DURING_WSLEEP and 
                   FEATURE_CLK_TIME_TICK_EXTENSION are undefined.
07/08/05    bk     Define DEBUG_SLEEP_PROFILING. 
06/21/05   ttl     Define FEATURE_GTOW_BACKGROUND_PLMN_SEARCH.
06/20/05  niru     Added FEATURE_FTM_EGPRS_BER 
06/17/05  niru     Defined FEATURE_CCBS
06/16/05    bk     Undef'd FEATURE_ERR_LCD 
06/16/05    bk     Enabled features FEATURE_GSM_SLEEP_FEE_FROM_TE and 
                   FEATURE_GSM_SLEEP_SUB_FRAME_SCHEDULING
06/10/05    gw     Added FEATURE_FTM_GSM_BER.
06/07/05    gw     Clean up of GSM sleep features.
05.24/05  niru     Removed FEATURE_WCDMA_TX_BETA_GAIN_TABLE_VER2 - This is now taken care of in custwcdma.h
05/21/05  niru     Added FEATURE_WCDMA_TX_BETA_GAIN_TABLE_VER2 temporarily for Raven Early 3.0; 
                   Need to clean up for Raven 3.0 
05/20/05  niru     Removed #undef FEATURE_HDLC_HW_ACCEL 
05/20/05    rv     Added  FEATURE_FTM_WCDMA_BER .
05/05/05   ttl     Added FEATURE_INTER_RAT_HO_OPT.
04/26/05    sr     Defined FEATURE_GSM_DEEPSLEEP. 
04/22/05    bk     Added FEATURE_MDSP_ATTEMPT_RECOVERY. Attempts to recover the GERAN
                   stack when a fatal condition is encountered.
04/21/05  niru     Added FEATURE_CPHS_EONS
04/13/05    sr     Removed FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING. Management
                   of this feature is moved to builds file
04/12/05   src     Re-enabled FEATURE_TCXOMGR_TMC.
04/07/05   src     Temporarily deleted FEATURE_TCXOMGR_TMC. Also, enabled
                   FEATURE_INTERRAT_REDIRECTION_WTOG.
03/17/05    sr     Enabled FEATURE_USE_SLEEP_CTLR, FEATURE_DEEP_SLEEP and 
                   FEATURE_WSLEEP_AWAKE_TIME_OPT for sleep(TCXO shutdown)
03/02/05   src     Enabled FEATURE_ENHANCED_NW_SELECTION.
02/19/05   src     Removed FEATURE_SLEEP_DISABLE_RF_GPIOS.
02/18/05    pv     removed FEATURE_UIM_USE_TCXO_DIV_5
01/11/05   src     Defined FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB.
12/09/04   gfr     Removed FEATURE_SBI_MDSP_CONTROLLER
11/17/04   src     Removed FEATURE_HDLC_HW_ACCEL.
11/16/04   sr      Removed FEATURE_UART_TCXO_CLK_FREQ from this file. Only
                   defined in custsio.h
07/22/04   mks     Removed LIST_SEARCH_IN_PAGING_BLOCK as it belongs to 
                   custgsm.h
07/19/04   mks     Enabled FEATURE_NAS_REMAIN_ON_FORBIDDEN_LAI. 
06/09/04   mks     Disabled FEATURE_RF_AFC
05/27/04   npr     Enabled FEATURE_GSM_DEEPSLEEP
05/25/04   mks     Disabled FEATURE_DYNAMIC_BBRX_CHANNEL1_SHUTDOWN to avoid 
                   RSSI fluctuations during CM
05/19/04   mks     Disabled FEATURE_FREQ_VS_TEMP_NV_UPDATE and enabled 
                   FEATURE_RF_AFC
05/01/04   mks     Enabled FEATURE_FREQ_VS_TEMP_NV_UPDATE 
04/28/04   mks     Enabled FEATURE_CLKREGIM_AUTOCAL_STUCK_WORKAROUND
04/28/04   mks     Removed FEATURE_PMIC_MOMENTARY_SUDDEN_PWR_LOSS 
04/26/04   ttl     Moved all WCDMA related feature definitions to custwcdma.h
04/23/04   mks     Enabled FEATURE_GMM_NV_ITEMS 
04/20/04   mks     Enabled FEATURE_CELL_FACH_MEAS_OCCASION
04/14/04   mks     Enabled hardware accelerators
04/13/04   mks     Disabled STTD features
04/13/04   mks     Disabled FEATURE_BUTTERFLY_CORRELATION temporarily to 
                   resolve physical channel establishment failures
04/07/04   mks     Enabled STTD and temporarily disabled G2W PCCO.
04/02/04   mks     Disabled the "spike" features. Enabled log and performance
                   improvements. Enabled G2W PCCO, SI2ter
03/26/04   mks     Removed FEATURE_HACK_FOR_GL1
03/26/04   mks     FEATURE_GSM_850_BAND and FEATURE_GSM_1900_BAND are now 
                   moved to msm6250.builds
03/24/04   mks     Disabled FEATURE_POWER_SAVE_UI. Enabled sub-frame scheduling
                   in GSM sleep timeline
03/19/04   mks     Enabled support for CELL PCH/URA PCH
03/17/04   mks     Added support for current optimization
03/16/04   mks     Added support for GSM Cell broadcast
03/12/04   bk      Synched to 3010_03 build
03/03/04   gfr     Added GSM rotator support.
02/27/04   mks     Synched to 3006 build
02/26/04   bk      Updated to SMNA3006_04 and added features FEATURE_GSM_PA_TEMP_COMP
                   and FEATURE_GSM_PA_TEMP_COMP_PWRLVL
01/26/04   mks     Enabled FEATURE_UIM_PMIC_ON_UIM1 and FEATURE_UIM_FLEXIBLE_COMPLIANCE.
                   Removed redundant inclusion of custsurf.h.
01/22/04   mks     Enabled FEATURE_MULTIBYTE_SBI
01/21/04   mks     Enabled FEATURE_PMIC_RTC
01/16/04   mks     Enabled spike mitigation and writing PICH offset to MDSP
01/14/04   mks     Added support for GSM850 band.
01/07/04   mks     Added support for Quadband feature
10/14/03   jac     removed undef for USB
08/28/03   ry      Added initial WCDMA sleep support.
07/28/03   ry      Merged features from UZ5002. Added support for high-speed 
                   UART. Defined FEATURE_MVS_RRC.
07/18/03   ry      Checked in custusurf.h#10 plus FEATURE_SUSPEND_WITH_ACT_TIME 
                   as custusurf.h#12.
===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                            Common Features
---------------------------------------------------------------------------*/
#define FEATURE_NAS_KLOCWORK
#define FEATURE_CS_FOLLOW_ON_REQUEST
#define FEATURE_PRESERVE_MANUAL_PLMN_LIST
#define FEATURE_NAS_VOC_SYNC
#define FEATURE_SDCC_8BIT_BUS_SETTING

// --- WCDMA ---
/* definition for Cheetah build (msm5200c/msm6200), need to be cleared up 
   later for complying with "Guidlines and Standards for the Definition and Use
   of Features, Target, MSM & Debug Symbols" by Patrick Marsh */

/* Indicates that a RF card is attached to the SURF instead of a baseband card.
*/
//top level feature removed: #define FEATURE_RF
/* Needed for P1/P2 to handle PA compensation under varying temps */
#define FEATURE_GSM_PA_TEMP_COMP
#define FEATURE_GSM_PA_TEMP_COMP_PWRLVL

// ---  GSM & WCDMA ---
// The following define can go into custsurf.h once SM builds support MDSP services.
/* Defines the MDSP Services task, which allows multiple clients access to the
** firmware applications of the MDSP.
*/
//#define FEATURE_MDSP_TASK

/* Enable MDSP services support for WCDMA L1
*/
//#define FEATURE_MDSP_SVC

/* Enables the GSM stack to recover when a fatal condition is hit */
//#define FEATURE_MDSP_ATTEMPT_RECOVERY

/* Enable MVS
*/ 
/* #define FEATURE_MVS   - dlb: Removed since this is a Multimedia feature */

#define FEATURE_MDSP_UWIRE

//top level feature removed: #define FEATURE_MDSP_USES_MODULES

/*  #define FEATURE_ENFORCE_CONFIG_WINDOW   - dlb: Not used in build */

/* #define FEATURE_EVENT_PROC_DELAY_CHECK   - dlb: Not used in build */

/* Enable inter-RAT re-direction */
//top level feature removed: #define FEATURE_INTERRAT_REDIRECTION_WTOG

/* Enable HSDPA */
//top level feature removed: #define FEATURE_REL_ID_NV_SUPPORT

/* Enable WCDMA to WCDMA Background PLMN search */
//top level feature removed: #define FEATURE_WTOW_BACKGROUND_PLMN_SEARCH

/* Enable RF interface to WCDMA L1 */
#define FEATURE_RF_HAS_L1_INTERFACE

//#define FEATURE_DUAL_RFR

/*---------------------------------------------------------------------------
                            General Interest
---------------------------------------------------------------------------*/

/* Customer's Over the Air Model Number.  Note that these must be carefully
** assigned and that QUALCOMM only has a few assigned to it.
*/
//#define CUST_MOB_MODEL 129

// ---  GSM  ---
/* Enable GSM code
*/
//#define FEATURE_GSM
//top level feature removed: #define FEATURE_NAS

// --- WCDMA ---
/* Enable WCDMA code
*/
//top level feature removed: #define FEATURE_WCDMA

/* Activates R99 June 2001 spec modifications in the code.
*/
#define FEATURE_REL_99_JUNE_2001

/* Indicates that WCDMA is supported by the MSM5200 ASIC.
*/
#define FEATURE_MSM_WCDMA

/* Enable MN logging.
*/
#define FEATURE_MN_LOGGING

/* Enable MN setup of skip indicator
*/
// #define FEATURE_MN_SET_SKIP_INDICATOR

// ---  GSM  ---
/* #define FEATURE_BSP  - dlb: Not used in code, looks (from comments) like CoreBSP feature */

/* Enable reading of GSM items like IMEI, classmark, revision_level, etc. 
** from NV 
*/
#define FEATURE_GSM_NVMEM_ENABLED

// ---  GSM & WCDMA ---

/* Event Reporting
*/
//top level feature removed: #define FEATURE_EVENTS_SUPPORTED


/*--------------------------------------------------------------------------
                                GSM/GPRS
---------------------------------------------------------------------------*/

#ifdef FEATURE_GSM
//#include "custgsm.h"
//#include "custgsmdrv.h"
#endif /* FEATURE_GSM */

/* Enable new TCXOMGR interface */
//#define FEATURE_TCXOMGR_INIT_NEW_INTERFACE

/* Enable saving TCXO PDM values to NV */
//#define FEATURE_TCXOMGR_TMC

/*---------------------------------------------------------------------------
                            Call Manager
---------------------------------------------------------------------------*/

// ---  GSM  ---
/* Debug - turns on CM event log for CM state
 */
//#define DEBUG_CM_EVENT_REPORTING

//#define FEATURE_IEI

/*---------------------------------------------------------------------------
                             User Interface
---------------------------------------------------------------------------*/

// --- WCDMA ---
/* Enables the support for Multiclient in GSDI
 */
//#define FEATURE_GSDI_MULTICLIENT_SUPPORT

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
  /* Enables +CPIN for ATCOP 
   */
  //#define FEATURE_DATA_ETSI_PIN
#endif

/* Enable dynamic memory allocation for the UI command buffers
 */
//#define FEATURE_REX_DYNA_MEM_UI

/* Disable BREW 
 */
// #define FEATURE_BREW_LITE

/* Allow an incoming voice call to occur during a PS data call
 */
//#define FEATURE_UI_CM_NON_SVD

/* A new data services baud rate settings menu was added for UMTS targets
 */
//#define FEATURE_DS_UI_BAUD


/* Enable Concurrent Services
 */
//top level feature removed: #define FEATURE_UMTS_CONC_SRVC
#ifdef FEATURE_UMTS_CONC_SRVC
  /* FEATURE_UI_CM_NON_SVD must be undefined to use the FEATURE_UMTS_CONC_SRVC
   */
#endif /* FEATURE_UMTS_CONC_SRVC */

/*---------------------------------------------------------------------------
                                  REG
---------------------------------------------------------------------------*/

//#include "custreg.h"

/*---------------------------------------------------------------------------
                              TASK TEST FLAGS
---------------------------------------------------------------------------*/

// --- WCDMA ---
/* Defines the MDSP Services task, which allows multiple clients access to the
** firmware applications of the MDSP.
*/
#ifdef FEATURE_MDSP_TASK
  #define INCLUDE_MDSP_TASK
  #define MDSP_INTERRUPTS_USED
#endif

/*---------------------------------------------------------------------------
                              Sound
---------------------------------------------------------------------------*/

// ---  GSM  ---
/* Enable EFR vocoder 
*/
//#define FEATURE_GSM_EFR

/*-------------------------------------------------------------------------
                              Non-Volatile Memory
---------------------------------------------------------------------------*/

// --- WCDMA ---
/* Enable the Embedded file system table probe
*/
//#define FEATURE_EFS_TABLE_PROBE

/* Define if using Flash memory for Voice Recognition ROM on the MSM5200 SURF
*/
//#undef FEATURE_SURF_VR_FLASH_MEMORY

/*---------------------------------------------------------------------------
                             WMS
---------------------------------------------------------------------------*/

/* Support SMS Status Reports and Commands
*/
//#define FEATURE_GWSMS_STATUS_REPORTS

/* Support message info cache for UI
*/
//#define FEATURE_GWSMS_CACHE

// ---  GSM  ---
/* Enable the message passing scheme for WMS
 */
//#define FEATURE_WMS_GS_SEND

//#if 0
/*---------------------------------------------------------------------------
                            Device Driver
---------------------------------------------------------------------------*/


// ---  GSM  ---
/* Enables downloading of the MDSP.
*/
#define FEATURE_MDSP_DOWNLOAD

/* Enable MDSP services support for GSM.
*/
#define FEATURE_MDSP_GSM_IMAGE

/* When FEATURE_RF is defined, use Config 4. Otherwise use Config 2.
 */
#ifdef FEATURE_RF
  #undef  FEATURE_ACTUAL_SFN_ENCODING
  #undef  FEATURE_PM1000_CHARGING
  #define FEATURE_MSM_SURF_CONFIG4
//  #define FEATURE_PM1000
//top level feature removed:   #define FEATURE_PMIC
  #define FEATURE_SURF_CLK_SRC_PLL
#else
  #define FEATURE_MSM_SURF_CONFIG2
  #define FEATURE_SURF_CLK_SRC_TIMON
#endif


/* Enable only LDOs on PM1000
*/
#undef  FEATURE_PM1000_LDO_ONLY

/* Enable reduction of SBI transactions
*/
#define FEATURE_VBATT_DUMMY_BATTERY

/* Enable support for B1 rev of ZIF
*/
//#define FEATURE_GSM_B1_SUPPORT

/* Enable Tx Phase error fix 
*/
//top level feature removed: #define FEATURE_TX_DYN_SETTLING_TIME

/* Support for Channel 0 to use clk from Channel 1 for GSM. This workaround is
** needed for dual mode operation.
*/                                                
#undef FEATURE_CLKREGIM_ADIE_CH1_CLK_GSM

// --- WCDMA ---
/* This feature enables the algorithm to compensate TX power limit 
** based on frequency and temperature.
*/
#define FEATURE_RF_TX_PWR_LIM


/* Enable RF Cal tracking items in NV
*/
//top level feature removed: #define FEATURE_RFCAL_VERSION

/* Enable split-IF
*/
#define FEATURE_RF_DUAL_IF_PLL

/* This feature initializes MDSP parameters with the data from RF NV items.
*/
#define FEATURE_RF_LOAD_NV_TO_MDSP

/* This feature updates the AGC linearizer table with frequency and 
** temperature compensation.
*/
#define FEATURE_RF_COMP_LIN

/* This feature can be turned on for Rev1 or higher revision of MSM5200. 
** PLL provides differential clock to Third-generation Over-sampled 
** Modulator (TOM/Neptune/analog die) through die-to-die connection.
*/
#define FEATURE_RF_PLL_DIFF_CLK

/* Enable feature to use initial Transmit Power limit data from RF calibration
*/
#define FEATURE_RF_TEMP_COMP

/* Enable new BBRX settings for dual mode support */
#define FEATURE_NEW_BBRX_SETTINGS

/* Enables downloading of the MDSP.
*/
#define FEATURE_MDSP_DOWNLOAD

 

// ---  Dual-Mode  ---
/* Enable MM RF API
*/
//top level feature removed: #define RF_MULTIMODE

// --- WCDMA ---
/* These features cause TMC to create and start the Sleep task.
*/
#define FEATURE_LPM_START_ALL_TASKS

//top level feature removed: #define FEATURE_FTM_TASK

//top level feature removed: #define FEATURE_FTM_WCDMA_BER

#define FEATURE_MMOC_TMC_FLOW_CNTR

/*---------------------------------------------------------------------------
                            U I M
---------------------------------------------------------------------------*/

// --- GSM & WCDMA ---
/* Turn on delay for starting up SMS 
*/ 
// Turn off feature until WMS does wms_sim_init for DM
// #define FEATURE_GWSMS_INIT_DELAY

/* Enable MM GSDI
*/
//top level feature removed: #define FEATURE_MMGSDI 
//top level feature removed: #define FEATURE_MMGSDI_GSM 
//top level feature removed: #define FEATURE_MMGSDI_UMTS

/* Enable power-up reads from the SIM for EF-SMS and EF-SMSP files to be
** done asynchronously
*/
#define FEATURE_GWSMS_ASYNC_SIM_INIT

//top level feature removed: #define FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME

//top level feature removed: #define FEATURE_UIM_PMIC_ON_UIM1  
//top level feature removed: #define FEATURE_UIM_FLEXIBLE_COMPLIANCE  

/* USAT support
*/
//top level feature removed: #define FEATURE_GSTK
//top level feature removed: #define FEATURE_UIM_TOOLKIT
#define FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR

/* Common PCN Handset Specification (CPHS) support */        
//top level feature removed: #define FEATURE_CPHS
//top level feature removed: #define FEATURE_ALS
//top level feature removed: #define FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
#define FEATURE_CPHS_EONS                
/*---------------------------------------------------------------------------
                              DSM FLAGS
---------------------------------------------------------------------------*/

// --- WCDMA ---
/* Turns on features that perform the DSM memory self-checking mechanism for
** debug purposes.
** Some of the DSM api calls invoke dsmi_verify_packet which traverses 
** the packet chain to verify that it is correct.  
** This can take a long time if the chain is long.   
** All of these calls are removed if this feature is not defined. 
*/
#undef FEATURE_DSM_MEM_CHK_EXPENSIVE
//top level feature removed: #define FEATURE_DSM_MEM_CHK_NO_FILL_AA
/*---------------------------------------------------------------------------
                              DATA SERVICES FLAGS
---------------------------------------------------------------------------*/

/* Enable Data Services
*/
//top level feature removed: #define FEATURE_DATA

/* Enable Multi Mode Data Services
*/
//top level feature removed: #define FEATURE_DATA_MM

/* Enable WCDMA Circuit-Switched Data
*/
// defined in commmon.builds #define FEATURE_DATA_WCDMA_CS

// ---  GSM  ---

// --- WCDMA ---
#define FEATURE_DIAG_DS_SIO_COUPLING

/* Enable Data Services Protocol Services
*/
//top level feature removed: #define FEATURE_DATA_PS

/* Enable ETSI specific functionality in PS
*/
//top level feature removed: #define FEATURE_DATA_WCDMA_PS
#undef FEATURE_DATA_WCDMA_PS_PDP_IP

/* Enables Network Model
*/
//top level feature removed: #define FEATURE_DS_NET_MODEL

/* Enables Sockets
*/
//top level feature removed: #define FEATURE_DS_SOCKETS

#ifdef FEATURE_GWSMS
  /* Enables SMS for ATCOP 
  */
  #define FEATURE_ETSI_SMS
#endif

#ifdef FEATURE_UI_PBM
  /* Enables PhoneBook Manager for ATCOP 
  */
#endif

/* Enables SMS updates 
*/
#define FEATURE_ETSI_SMS_WMS_FIX

/* Enable use of the HDLC hardware block
*/



//#define FEATURE_PPP_HW_ACCEL

/* Enable supplementary services support in ATCOP
*/
#define FEATURE_DATA_ETSI_SUPSERV
#define FEATURE_DATA_ETSI_SUPSERV_USSD

/* Enable V.80 
*/
//top level feature removed: #define FEATURE_DATA_V80
#define FEATURE_DSAT_V80


/* Enable feature to read DS baud info from NV
*/
//top level feature removed: #define FEATURE_DS_DEFAULT_BITRATE_NV

/* FEATURE_DS_DEFAULT_BITRATE_NV needs FEATURE_DS_UI_BAUD to be defined
*/
//top level feature removed: #define FEATURE_DS_UI_BAUD

/* Enables the flow control feature. 
 * Used for flow control when sending big response to TE.
*/
#define FEATURE_DSAT_FLOW_CONTROL

/* Enable Cell broadcast feature
*/


/*---------------------------------------------------------------------------
                           Dual Mode Sleep Features
---------------------------------------------------------------------------*/

//top level feature removed: #define FEATURE_SLEEP

/* hnassif - define TMC_SLEEP so that tmc can start the sleep task and we
 * don't get stuck in the power management thread during debugging */
#define TMC_SLEEP

//top level feature removed: #define FEATURE_SLEEP_SLOW_CLOCK
#define FEATURE_ENABLE_SLEEP_INT

/* We should not enable this. It was due to a HW bug in Raven. CF 9/28 */
#undef FEATURE_SLOW_ARM_FOR_SLEEP_CTLR_ACCESS

#ifdef FEATURE_SLEEP

//top level feature removed:   #define FEATURE_UNIFIED_SLEEP_TASK
//top level feature removed:   #define FEATURE_UNIFIED_SLEEP_CTLR_P1
//top level feature removed:   #define FEATURE_UNIFIED_SLEEP_CTLR_P2

//top level feature removed:   #define FEATURE_SLEEP_TASK
  
  #define FEATURE_SLEEP_WCDMA

//top level feature removed:   #define FEATURE_SLEEP_GSM
  
  /* Should not need this for Saber */
  /* Enable disabling clk regimes during sleep. 
  */
  #undef FEATURE_DISABLE_CLK_REGIMES_DURING_SLEEP

  /* Enable power saving 
  */
  #define FEATURE_IN_CALL_ARM_POWER_DOWN

  /* Enable Sleep QXDM DipSwitch
  */
//top level feature removed:   #define FEATURE_SLEEP_CTRL_VIA_QXDM_DIPSW
  
  /* Enable Deep Sleep
  */
//top level feature removed:   #define FEATURE_DEEP_SLEEP 

  #define FEATURE_SLEEP_CLK_SRC_DIGITAL

#endif /* FEATURE_SLEEP */

#ifdef FEATURE_SLEEP_WCDMA

  /*----------------------------------------------------------------
        FLAGS FOR TCXO SHUTDOWN DURING WCDMA SLEEP
  -----------------------------------------------------------------*/

  /* Workaround for a Saber h/w bug */
  #define FEATURE_FAST_SLEEP_CLK_ENABLED_WORKAROUND                  
  
  /* using sleep controller to implement sleep time line */
  #define FEATURE_USE_SLEEP_CTLR

  /* longer duration between rex ticks during sleep */
  #ifndef FEATURE_USE_TIME_VU
  #define FEATURE_SLOW_REX_TICKS_DURING_WSLEEP
  #endif
 
  /* allows tcxo to be disabled if appropriate consensus is reached */
  #define FEATURE_SLEEP_DISABLE_TCXO
 
  /* allows UIM to vote for tcxo shutdown */
//top level feature removed:   #define FEATURE_UIM_TCXO_SLEEP
 
  /* enables further RF current optimizations */
  #define FEATURE_RF_OPTIMIZE_TX
  
#endif /* FEATURE_SLEEP_WCDMA */

#ifdef FEATURE_SLEEP_GSM
    
    /* GSM L1 features */
    
//top level feature removed:     #define FEATURE_GSM_SLEEP_CONTROLLER
//top level feature removed:     #define FEATURE_GSM_SLEEP_FEE_FROM_TE 
//top level feature removed:     #define FEATURE_GSM_CLK_SWITCHING
    #define FEATURE_GSM_SLEEP_SUB_FRAME_SCHEDULING    
    #define FEATURE_GSM_SLEEP_WCDMA_CLK_OPT
    #define FEATURE_GSM_TX_CLK_OPT
    #define FEATURE_GSM_GSAC_CLK_OPT

    /* RF features */    
//top level feature removed:     #define FEATURE_GSM_RF_SLEEP
    #define FEATURE_GSM_SLEEP_PA_OFF_IN_IDLE
    #define FEATURE_GSM_RF_SLEEP_LDO_SHUTDOWN   
    
    /* Sleep task features */    
    #ifndef FEATURE_USE_TIME_VU
    #define FEATURE_CLK_TIME_TICK_EXTENSION
    #endif
//top level feature removed:     #define FEATURE_GSM_SLEEP_TASK
    #define FEATURE_GSM_TCXO_SHUTDOWN
    #define FEATURE_GSM_SLEEP_ARM_HALT
//top level feature removed:     #define T_SLEEP                   /* legacy sleep feature */
        
    /* Deep sleep features */
//top level feature removed:     #define FEATURE_GSM_DEEPSLEEP
    
#endif /* FEATURE_SLEEP_GSM */


/*---------------------------------------------------------------------------
                           Profiling Support Features
---------------------------------------------------------------------------*/
// ---  GSM & WCDMA  ---
#define FEATURE_RESET_COUNTER

//#endif /* #if 0 */

/*---------------------------------------------------------------------------
                           Dual Mode Features
---------------------------------------------------------------------------*/
/* David - added to make RF compile */
//top level feature removed: #define FEATURE_TX_DYN_SETTLING_TIME
#define FEATURE_RF_NEW_ARCH_FOR_UI_DEBUG_SCREEN

/* David - added to fix ATCOP link error */
#define FEATURE_DSAT_INFER_BATT_STATUS

// --- WCDMA ---

//top level feature removed: #define FEATURE_MCC_AND_850_800_BAND_COMBINATION

//top level feature removed: #define FEATURE_DSAC

//top level feature removed: #define FEATURE_CONNECTED_MODE_SERV_IND
/* Disable Scan Forever for RRC
*/
#undef FEATURE_FOREVER_SCAN

/* Enable Compressed Mode support
*/
//top level feature removed: #define FEATURE_CM_SUPPORTED

//top level feature removed: #define FEATURE_CM_TGPS_RUNTIME_ERROR

/* Enable activating and deactivating compressed mode 
** gap patterns through the measurement control message
*/
/* WCDMA - RRC feature */
//top level feature removed: #define FEATURE_INTER_RAT_MEAS

//top level feature removed: #define FEATURE_3GPPCR1714_CLASSMARK

/* Enable Compressed Mode Measurements
*/
//top level feature removed: #define FEATURE_CM_MEASUREMENT

//top level feature removed: #define MSMHW_GSM_INIT_IN_WL1_INIT

// ---  GSM & WCDMA  ---
/* Enable Multimode operation on RRC and WCDMA L1
*/
//top level feature removed: #define FEATURE_DUALMODE_BASELINE

/* Feature needed by MM CM
*/
#define FEATURE_CM_USE_UMTS_REG_INTERFACE

/* Enable Multimode operation on MM for RR
*/
//top level feature removed: #define FEATURE_MULTIMODE_GSM

/* Enable reading of IMEI from NV
*/
#define FEATURE_NAS_NVMEM_IMEI

/* Enable enhanced network selection (ENS) - HPLMN based on REG
 */
//top level feature removed: #define FEATURE_ENHANCED_NW_SELECTION

/* Enable Factory Test Mode support
*/
//top level feature removed: #define FEATURE_FACTORY_TESTMODE


//top level feature removed: #define FEATURE_SEND_SEQUENCE_NUMBER

#define FEATURE_GW_MANUAL_PLMN_IN_PS_CALL


// ---  Dual-Mode  ---
/* Enable WCDMA to GSM RAT Handover
*/
//top level feature removed: #define FEATURE_INTERRAT_HANDOVER_WTOG

/* Enable WCDMA to GSM Cell Reselection
*/
//top level feature removed: #define FEATURE_INTERRAT_CELL_RESELECTION_WTOG

/* Enable WCDMA to GSM Background PLMN search
 */
//top level feature removed: #define FEATURE_WTOG_BACKGROUND_PLMN_SEARCH

/* Inter RAT HO optimization */
//top level feature removed: #define FEATURE_INTER_RAT_HO_OPT

#ifdef RF_MULTIMODE
  /* Enable GSM to WCDMA Cell Reselection
  */
//top level feature removed:   #define FEATURE_INTERRAT_CELL_RESELECTION_GTOW

  /* Enable RF driver support for GSM to WCDMA inter-RAT measurements
  */
//top level feature removed:   #define FEATURE_RF_ENABLE_GTOW
  
  /* Spoof flag for G2W Reselection */
  /* Old feature name was DEBUG_INTEGRATION_TEST_GTOW_UARFCN_SC */
  #define FEATURE_GTOW_RESELECTION_SPOOF_ENABLE_UARFCN_SC
  
//top level feature removed:   #define DEBUG_SYSTEMS_RESELECTION_GTOW
  
  #undef DEBUG_FORCE_BLIND_HANDOVER
  
#endif


//top level feature removed: #define FEATURE_COMPRESSED_MODE

//top level feature removed: #define FEATURE_CM_SEARCH

//top level feature removed: #define FEATURE_CM_GSM_SEARCH

//top level feature removed: #define FEATURE_RF_ENABLE_CM

/* Enable event triggered handover
*/
#define FEATURE_2D_2F_MEAS
#define FEATURE_INTERF_SRCH_MEAS

/* Enable GSM to WCDMA Blind Handover
*/
//top level feature removed: #define FEATURE_INTERRAT_HANDOVER_GTOW

//top level feature removed: #define FEATURE_INTERRAT_CC_ORDER_WTOG

/* Enable PCCO GTOW */
//top level feature removed: #define FEATURE_INTERRAT_PCCO_GTOW

#define FEATURE_MANUAL_HPLMN_SELECTION

//top level feature removed: #define FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

//top level feature removed: #define FEATURE_IS683C_PRL
//top level feature removed: #define FEATURE_IS683A_PRL
//top level feature removed: #define FEATURE_EXTENDED_PRL

#define FEATURE_SET_BAND_IN_GL1
//top level feature removed: #define FEATURE_GSM_ARFCN_STRUCT


/* These features replace FEATURE_6250_COMPILE */   
#define FEATURE_NOT_USE_MDSP_DMA
//top level feature removed: #define FEATURE_CLKREGIM_HEIRARCHICAL
#define FEATURE_FAST_ENC_START_TIME
#define FEATURE_6250_WCDMA_RAM_VERSION
//top level feature removed: #define FEATURE_MAP_6250_DEFINES
//top level feature removed: #define FEATURE_SRCH_BITWIDTH_MULTI_FACTOR
//#define FEATURE_TB_IND_DETAIL_INFO

#define FEATURE_MULTIBYTE_SBI

/* Support for Multiple PDP contexts */
//top level feature removed: #define FEATURE_MULTIPLE_PRIMARY_PDP

//top level feature removed: #define FEATURE_INTERRAT_HANDOVER_GTOW 
#ifdef  FEATURE_INTERRAT_HANDOVER_GTOW 
  /* Spoof flag for G2W HO */  
  /* Old feature name was DEBUG_INTEGRATION_TEST_GTOW_HO_TO_UTRAN */ 
  #define FEATURE_GTOW_HO_TO_UTRAN_SPOOF_ENABLE 
  #define FEATURE_GTOW_NB_HO_TO_UTRAN_SPOOF_ENABLE 
  
  /* To set new AGC values */
  /* #define  DEBUG_NEW_AGC_INIT */ 
  /* To have L1 pass trk_lo_adj value to RR: */ 
  #define  FEATURE_INTERRAT_SEND_TRK_LO_ADJ_GTOW 
 
    /*  To have L1 log state change events: */ 
//top level feature removed:   #define  DEBUG_DISABLE_L1STATE_EVENT 
//top level feature removed:   #define FEATURE_INTERRAT_NON_BLIND_HANDOVER_GTOW 
  
//top level feature removed:   #define FEATURE_G2W_EVENTS_SUPPORTED 
  #define FEATURE_NONBLIND_HANDOVER_DEBUG 
  
#endif 


// ---  GSM  ---

/* Enable dual mode AMR features
*/
//David - commented out as in 6280
//#define FEATURE_MVS_RRC

#define FEATURE_GSM_TCXO_SRM_SUPPORT
//top level feature removed: #define FEATURE_WCDMA_TCXO_RESOURCE_SUPPORT

/* Enable Follow-on request feature */
#define FEATURE_PS_FOLLOW_ON_REQUEST

//top level feature removed: #define FEATURE_MDSP_SVC_DOES_CLK_SWITCHING

#define FEATURE_FFE_CGC_WORKAROUND

//#define FEATURE_MDSP_SVC_DOES_CLK_SWITCHING


/* Talk time current improvements */
#undef FEATURE_DYNAMIC_BBRX_CHANNEL1_SHUTDOWN

/* Resolve GCF test case issues with PTMSI signature inclusion */
#define FEATURE_GMM_NV_ITEMS 
#define FEATURE_GSM_GPRS_NV_IF

/* Disable passing of PDM and rotator values between RATs */
#undef FEATURE_FREQ_VS_TEMP_NV_UPDATE 

#undef FEATURE_RF_AFC

#define FEATURE_NAS_REMAIN_ON_FORBIDDEN_LAI

//top level feature removed: #define FEATURE_UUS

// #define FEATURE_FTM_GSM_BER removed for 8200

//#define FEATURE_FTM_EGPRS_BER removed for 8200

//top level feature removed: #define FEATURE_CCBS

#define FEATURE_RUNTIME_RFSETUP_BUF_SIZE 

//top level feature removed: #define FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER 

//top level feature removed: #define FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION

#define FEATURE_EMERGENCY_CATEGORY

//top level feature removed: #define FEATURE_MULTI_RF_CHAIN_API

//top level feature removed: #define FEATURE_DUAL_SERVICE_IN_DATA
//top level feature removed: #define FEATURE_DUAL_SERVICE_IN_VOICE

//top level feature removed: #define FEATURE_WTOG_NACC

//top level feature removed: #define FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE 

/*GSM SCH decode with large freq. error */
//top level feature removed: #define FEATURE_W2G_FREQ_ERR

#define FEATURE_GW_MINQOS_PDP_MODIFY

#define FEATURE_GTOW_DONT_PASS_PSC_INFO

/*#define FEATURE_LSM_PCH_REPORTING*/

//top level feature removed: #define FEATURE_PREFERRED_MODE_CHANGE

#define FEATURE_G2W_SRCH_LOGGING_ENABLED


/** SLEEP / BlueTooth **/
/** New sleep voting interface allows all subsystems to dynamically register to
*** vote for sleep at runtime, as opposed to the old interface which required
*** compile time registration.  The old sleep voting interface has continued
*** supported in the sleep module for those clients that have still not 
*** converted to the new interface.
**/

//top level feature removed: #define FEATURE_NEW_SLEEP_API


/** TCXOMGR version 3.0 **/
/** TRM Manager Manages Antenna resources to the primary RAT i.e. WCDMA, CDMA 1X or GERAN **/
/** TCXO manager 3.0 Combines 1x, GSM, WCDMA, SRM and TCXO manager into a single controlling entity **/

//#define FEATURE_WCDMA_TCXOMGR30
//#define FEATURE_WCDMA_TRM
/** Support for TRM to co-ordinate using RF resources between GPS/1x and UMTS **/
//top level feature removed: #define FEATURE_TRM_API_V2


/** TURN on UOOS Feature **/
//top level feature removed: #define FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT
//top level feature removed: #define FEATURE_DCH_TO_FACH_PCH_OUT_OF_SERVICE_ENHANCEMENT

/** TURN on 800 and 850 Band combination and BC9 and BC4 **/
//top level feature removed: #define FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION

/** TURN on W2G APP support **/
//top level feature removed: #define FEATURE_APP_W2G

/* Needed for SU_MODEM_GERAN.13.00.09 */
//top level feature removed: #define FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH

#ifdef FEATURE_LTE
//top level feature removed: #define FEATURE_RAT_PRIORITY_LIST
#endif

/* SU_MODEM_WCDMA_MDM8220.01.20.00 changes */
//top level feature removed: #define FEATURE_NETWORK_SHARING

#ifdef FEATURE_VOC_AMR_WB
#define WCDMA_RRC_INDICATE_CODEC_TO_CM
#endif

#endif /* CUSTMODEM_H */


/*****************************************************************************/
