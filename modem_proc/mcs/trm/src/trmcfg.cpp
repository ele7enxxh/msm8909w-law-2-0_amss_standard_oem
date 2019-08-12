/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

            T R A N S C E I V E R   R E S O U R C E   M A N A G E R

                Transceiver Resource Manager Configuration File

GENERAL DESCRIPTION

  This file supports configuration Transceiver Resource Manager


EXTERNALIZED FUNCTIONS

  None
  

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trmcfg.cpp#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
05/11/2016   ag      Updated MC_TRAFFIC for GSM to 95
04/04/2016   jp      Enable TRM_ACQUISITION_LOW reason for LTE.
03/14/2016   sk      Make BG TRAFFIC for 1x valid(897417)
06/26/2015   pap     Add changes to enable CM_INV reason for 1x.
05/28/2015   mn      Enable ACQ_INV reason for TDS (CR: 840676).
02/18/2015   sk      Update log v8 for eMBMS logging(796676) 
01/13/2015   mn      Increased the Priority LTE Total Control (TRM_LTE client). 
                      (CR: 778286).
01/28/2014   sk      Made ACQUISITION_HI DR incompatible(786126) 
01/13/2015   sk      WLAN priority made lower than CA priorities(728239) 
12/24/2014   mn      Enable TRM_LOW_LAT_TRAFFIC for use by LTE. (CR: 769352).
12/11/2014   mn      LTE EMBMS functional changes (CR: 769538).
11/14/2014   sr      Add a SLEEP Diversity reason for LTE to use for diversity
                     to avoid sending wrong unlocks due to PAM (CR: 751597) 
11/04/2014   sr      Make High Priority Signalling DR capable (CR:747380) 
11/04/2014   sr      Making LTE Diversity compatible with all other div clients
                     (CR: 747740).
11/04/2014   sr      LTE Div should be compatible with only DR reasons (CR:727672).
10/21/2014   sk      Div modes made compatible to DR modes only in DRDSDS(743095) 
10/24/2014   mn      Support for LTE HO RxD (CR: 744539)
10/20/2014   sk      Fix memory issue on offtarget(734841)
10/11/2014   mn      Increase LTE CA priority above LTE CA SEC (CR: 735178).
09/29/2014   mn      Added a new TRM_DIVERSITY_IRAT_MEASUREMENT reason (CR: 712921).
09/07/2014   sr      Make LTE Access a DR Capable reason(CR: 712328)
09/04/2014   ag      Made LTE TOTAL CONTROL > DO to avoid 1x2L reselect failure
08/12/2014   sr      IRAT should be made DR compatible for LTE(714342)
08/12/2014   ag      Made G ACQ_MAX > G RESELECT/_HI
08/05/2014   mn      Increase the SUBS_CAP priority to be above GSM RESELECT 
                       (CR: 700737).
07/23/2014   sr      GERAN using deprecated priority causing L2G resel/redir/CCO 
                     failure in DR-DSDS mode (700576)
07/23/2014   mn      Adding new GSM secondary clients (CR: 696634).
07/17/2014   mn      TRM should map 1x and GSM2 to chain 2 when LTE is holding 
                      both chain 0 and chain 1 (CR: 695108).
07/15/2014   sk      Added missing compatibility in SGLTE/SGTDS case (695500)
07/11/2014   mn      All diversity modes should be compatible with other modes 
                      in DR-DSDS mode (CR: 691656).
07/08/2014   mn      TRM: TDSCDMA secondary to be made compatible with GSM in 
                      DR mode. (CR: 690332).
06/15/2014   sk      WLAN antenna sharing changes (615092)
05/21/2014   mn      Changed 1x ACQ_INV mode to TRM_MODE_1X_ACQ. (CR: 668618).
05/19/2014   mn      Made D2L measurements incompatible with GSM SLTE modes
                      for SVLTE + G DSDA mode (CR: 667375).
05/16/2014   sk      G SLTe mode made compatible with W div for DSDA(664431)
03/25/2014   sk      New Reason for LTE high priority signalling(653561)
05/05/2014   sk      Remove incompatibility between W Div and G2 in DSDA 660131) 
04/20/2014   sk      LTE sleep priorityfor sec client cahnged (CR: 649238)
04/09/2014   sk      Changes to support 1x acq throttling (CR: 626286)
04/09/2014   sr      LTE Paging Priority should be 185 (CR: 644158) 
04/01/2014   mn      SLTE + CA support (CR: 616403).
04/03/2014   sk      New Reason for LTE emergency SIB Reads
03/20/2014   mn      Made the new SLTE GSM modes compatible with WCDMA diversity 
                      mode. (CR: 635616).
02/27/2014   sr      SLTE Changes.
03/12/2014   mn      Adding support for connected mode WTR hopping.
03/04/2014   mn      Made W div compatible with GSM client (CR: 616239).
02/24/2014   mn      Enabled CHANNEL_MAINTENANCE reason for HDR.
02/13/2014   sk      Added channel_maintenance_inv reason
01/23/2014   mn      Lowering the priority of SUBS_PRIO request to prevent 
                     unlock callbacks and denials of W/T in single SIM mode.
01/20/2014   sk      Added compatibility between TDS_DIV and GPS
01/03/2013   sk      Change the priorities for CA client to the lowest
01/05/2014   sr      In PBR, increments within 100 ms are redundant(CR:591479)
01/03/2013   sk      Reset the compatibility bitmask before setting new 
11/26/2013   sr      Change for Dime OMRD (CR:  589736)
11/26/2013   mn      Opp. SGLTE/SGTDS/SVLTE +G DSDA support.
11/25/2013   sr      PBR Algorithm Implementation
10/29/2013   sk      IRAT client priority changed to maximum
09/12/2013   sk      L+G DSDS support
08/29/2013   rj      Adding TRM support for G2W TRM_LTA_SUPPORTED
05/21/2013   mn      Adding TSTS support.
05/14/2013   rj      Added support for SGLTE in TRM
03/25/2013   rj      Updating TDSCDMA priority for Triton
02/19/2012   sr      HDR Diversity not working during acquisition.(CR:454030)
02/06/2012   sr      DO is not able acquire div when LTE is active.(CR: 449718)
02/04/2012   sr      Add missing GPS compatibilities(CR:447595)
02/04/2012   sr      Add a new priority TRM_ACQ_DIVERSITY for HDR(CR:439864)
12/04/2012   sr      Set 1X Div and HDR Div compatible for SVDO.
11/27/2012   mn      Triton DSDA changes.
11/26/2012   rj      Additional changes for adding TRM_CM client ID.
10/03/2012   mn      Nikel DSDS Merge.
08/13/2012   ag      Added new reason for non-urgent 1x rude wakeup
04/02/2012   ag      Make HDR IRAT < 1X DEMOD and 1X ACQ
02/16/2012   ag      Make DO IRAT compatible witk LTE and LTE div > 1x page
01/13/2012   ag      TRM Feature Cleanup  
01/10/2012   ag      Added two new diversity reasons 
12/20/2011   ag      Make GPS and HDR compatible in SVDO mode and LTE div>1xdiv
12/07/2011   ag      Remove (2,6) NV check for DO acq/idle with 1x idle compat.
11/09/2011   ns      Increase GPS priority > 1x acq and demod on secondary 
08/01/2011   sr      Made MODE_1X compatible with MODE_HDR_IDLE and MODE_HDR
08/01/2011   sr      Make 1x acquisition and DO compatible
08/15/2011   sg      Added support for TDSCDMA Layer1
06/14/2011   ag      Remove DORB featurization to make MCDO_TRAFFIC same as LPT.
06/09/2011   ag      Make DO mc_traffic < 1x acq
05/31/2011   ag      Make priority of interlock div same as diversity for 1x.
04/21/2011   ag      Added an API to check whether SHDR mode is supported.
04/11/2011   sg      Modified compatability setting for MCDO and 1X
02/10/2011   sg      Added support for ET and HDR-IRAT measurements
01/20/2011   sg      Added compatability for MCDO and 1X for DOrB 
11/22/2010   ag      Added new reason SMALL_SCI_CONTINUATION for HDR. 
10/18/2010   ag      Merged DSDS functionality.
08/23/2010   ag      Merged GSM/WCDMA reasons.
06/17/2010   ag      Added mode TRM_MODE_HDR_MCDO_CONNECTED to make the new 
                     MC_TRAFFIC reason incompatible with 1X in SHDR.
06/02/2010   hh      Added priority value of 50 to TRM_SYSTEM_MEASUREMENT 
                     reason in 1x secondary 
06/01/2010   hh      Added new reason TRM_SYSTEM_MEASUREMENT for 1x system 
                     measurement changes.
04/06/2010   sm      Added Support for PAM functionality
11/13/2009   ag      Added modem_mcs_defs.h for GENESIS modem.  
11/16/2009   ag      Changed GPS demod priority to 155 for non-FTS mode.
11/12/2009   ag      Fix KW error - array out of bounds.
10/19/2009   ag      Changed HDR acq. priority to 111 for non-FTS mode. 
08/28/2009   ag      Fixed some medium lint warnings.
07/31/2009   ag      Fixed lint warnings 
07/21/2009   ag      Merged INTLOCK and other Q6 changes from //depot
27/03/2009   ns      Clean up to support SCMM and 8650 builds only
03/13/2009   ag      Added a change for off target build (TRM_BYTES_PER_CLIENT)
02/17/2009   cpk     Added support for Off Target support
10/21/2008   aps     Lower HDR Acquisition priority
09/18/2008   rkc     Added TRM_DEMOD_PAGE_CONTINUATION to trmcfg tables.
09/09/2008   sg      Fix lint/klocwork warnings
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
06/06/2008   sg      Added high priority BCMCS support.
05/22/2008   sg      Integrated changes from old depot 
09/21/2007   grl     Added full time SHDR support.
08/01/2007   ebb     Updated TRM_BYTES_PER_CLIENT
05/24/2007   ebb     Changed TRM_BYTES_PER_CLIENT to account for 64 bit additions
                     in client struct.
10/17/2006   cab     Added TRM_DEMOD_SMALL_SCI_PAGE reason.
                     1x acq and hi pri HDR traffic always compatible
09/25/2006   cab     Allowed 1x acq simultaneous with HDR high pri traffic
09/15/2006   cab     Integrated UMTS
07/25/2006   cab     Lowered HDR access priority to support access hybrid
06/07/2006   cab     Allowed GPS to run concurrently with HDR HPT
06/06/2006   pa      Featurize 1x acq on secondary chain for 6800B/7500B
06/06/2006   pa      Use lower priority than GPS for 1x acq on secondary chain.
06/06/2006   pa      Enable 1x acquisition on secondary chain
05/02/2006   cab     Added ifdef's for offline compilation and testing
04/20/2006   grl     Increased the priority of GPS above 1x paging for 6800.
12/02/2005   awj     Added priority for 1x secondary demod commands
11/21/2005   cab     Moved NV item id's to compile time assign outside library
11/08/2005   grl     Lowered priority of broadcast so that 1x acq can run.
10/21/2005   grl     Added SGPS-DO compatibility and HDR div compatibility.
09/10/2005   grl     Added broadcast access/traffic reasons.
08/29/2005   grl     Added support for dynamic compatibility masks.
07/19/2005   grl     Added support for SGPS+1x acq and removed S-idle-idle.
07/11/2005   grl     Added TRM config mask bit for SHDR enabling.
06/21/2005   ajn     Linting changes.
06/14/2005   ajn     Mode table compile-time initialized.
06/13/2005   grl     Added support for 1x to be active on both RX1 and TXRX1.
05/26/2005   ajn     Code review comment changes
03/28/2005   ajn     Added GPS_TDM reason, and client/reason compatibility
03/02/2005   ajn     Initial AMSS version

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mcs_variation.h"
#include <customer.h>
#include "trmcfg.h"

extern "C"
{
    #include "modem_mcs_defs.h"
    #include <err.h>
}


/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/*-----------------------------------------------------------------------------
  Locally defined TRM CFG related features
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Number of clients in trm_client_enum_t
-----------------------------------------------------------------------------*/

const uint32                  TRM::num_clients = TRM_MAX_CLIENTS;



/*-----------------------------------------------------------------------------
  NV item identifiers
-----------------------------------------------------------------------------*/

const nv_items_enum_type      TRM::nv_trm_config = NV_TRM_CONFIG_I;
const nv_items_enum_type      TRM::nv_gps_rf_config = NV_GPS_RF_CONFIG_I;

/*-----------------------------------------------------------------------------
  Client mode identifiers
-----------------------------------------------------------------------------*/

typedef enum
{
  /*-----------
    1X modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_1X,

  TRM_MODE_1X_DR_CAPABLE,

  /* Acquisition may be incompatible with GPS */
  TRM_MODE_1X_ACQ,

  /* 1X in diversity */
  TRM_MODE_1X_DIV,

  /* 1X performing ET */
  TRM_MODE_1X_ET,

  /* 1X inter lock diversity */
  TRM_MODE_1X_INTERLOCK_DIV,

  /* 1X inter-rat measurement mode */
  TRM_MODE_1X_IRAT_MEASUREMENT,

  /*-----------
    HDR modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_HDR,

  /* HDR in a traffic call */
  TRM_MODE_HDR_CONNECTED,

  TRM_MODE_HDR_BG_CONNECTED,

  /* HDR in broadcast */
  TRM_MODE_HDR_BROADCAST,

  /* Low latency traffic or high priority broadcast */
  TRM_MODE_HDR_LOW_LAT,

  /* HDR in idle  */
  TRM_MODE_HDR_IDLE,

  /* HDR in diversity */
  TRM_MODE_HDR_DIV,

  /* HDR in small SCI idle operation */
  TRM_MODE_HDR_SMALL_SCI_IDLE,

  /* HDR in multi carrier traffic */
  TRM_MODE_HDR_MCDO_CONNECTED,

  /* HDR performing IRAT measurements on LTE */
  TRM_MODE_HDR_IRAT_MEASUREMENT,

  /* HDR performing ET */
  TRM_MODE_HDR_ET,

  /*-----------
    GPS modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_GPS,

  /*-----------
    UMTS modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_UMTS,

  /* WCDMA in diversity */
  TRM_MODE_UMTS_DIV,

  /* WCDMA performing ET */
  TRM_MODE_UMTS_ET,

  /* Dual receive */
  TRM_MODE_UMTS_DR_CAPABLE,

  /*-----------
    GSM modes
  -----------*/

  /* General mode for GSM1 and GSM2 clients */
  TRM_MODE_GSM,

  /* GSM Modes which are capable of SLTE */
  TRM_MODE_GSM_SLTE_CAPABLE,

  TRM_MODE_GSM_DR_CAPABLE,

  /*-----------
    TDSCDMA modes
  -----------*/

  /* TDSCDMA general mode*/
  TRM_MODE_TDSCDMA,

  TRM_MODE_TDSCDMA_DR_CAPABLE,
  
  /* TDSCDMA in diversity mode*/
  TRM_MODE_TDSCDMA_DIV,


  /*-----------
    LTE modes
  -----------*/

  /* LTE general mode*/
  TRM_MODE_LTE,
  
  /* LTE Modes which are capable of SLTE */
  TRM_MODE_LTE_SLTE_CAPABLE,

  TRM_MODE_LTE_DR_CAPABLE,
  
  /* LTE in diversity mode*/
  TRM_MODE_LTE_DIV,
  
  /* LTE performing ET */
  TRM_MODE_LTE_ET,
  
  /*-----------
    CM modes
  -----------*/ 
  TRM_MODE_CM,  

  /*-----------
    GSM diversity modes
  -----------*/ 
  TRM_MODE_GSM_DIV,

  TRM_MODE_RF, 

  /* IRAT Modes */
  TRM_MODE_IRAT,

  /* WLAN Modes*/
  TRM_MODE_WLAN,

 TRM_MODE_TECH_CHANGE,

  TRM_MODE_LTE_SINGLE_RX_CA,

  /* Number of client modes */
  TRM_NUM_MODES
} trm_mode_id_enum_t;

/*-----------------------------------------------------------------------------
  Client mode id to bit-masks
-----------------------------------------------------------------------------*/

#define TRM_MASK_FROM_MODE(id)          ( (uint64)1 << (id) )

   
/*-----------------------------------------------------------------------------
  Returns the selected Simultaneous 1x HDR Mode
-----------------------------------------------------------------------------*/

#define TRMCFG_MSK_MODE_S1XHDR(mask)      \
  ( ( trmcfg_mode_s1xhdr_enum_type )( mask & 0x3 ) )


/* The standard S1X/HDR mode is off, meaning legacy hybrid mode. */
#define TRMCFG_MODE_S1XHDR_DEFAULT TRMCFG_MODE_S1XHDR_DISABLED

/*-----------------------------------------------------------------------------
  TRM Library memory requirements
-----------------------------------------------------------------------------*/

/* Space required by all clients, in bytes */
#define TRM_BYTES             (TRM_BYTES_PER_CLIENT * TRM_MAX_CLIENTS)

/* Space required by all client, in qwords */
#define TRM_QWORDS            ((TRM_BYTES+7)/8)


/*-----------------------------------------------------------------------------
  TRM Config static, non-constant data
-----------------------------------------------------------------------------*/
struct trmcfg_t
{
  /*-------------------------------------------------------------------------
    Mode to compatible modes table
  -------------------------------------------------------------------------*/
  trm_compatible_mask_t       compatible[ TRM_NUM_MODES ];

  /*-------------------------------------------------------------------------
    Memory buffer - Storage for TRM library, uint64 aligned
  -------------------------------------------------------------------------*/
  uint64                      buffer[ TRM_QWORDS ];
};


/* TRMCFG's static, non-constant data storage */
static trmcfg_t               trmcfg;



/*-----------------------------------------------------------------------------
  TRM Memory Buffer
-----------------------------------------------------------------------------*/

/* Location and size of storage */
TRM::Memory                   TRM::memory =
{
  /* Location of storage */
  (uint8 *) trmcfg.buffer,

  /* Size of storage */
  sizeof(trmcfg.buffer)
};

/*-----------------------------------------------------------------------------
  Set Compatibility Information Storage for Debug purposes.
-----------------------------------------------------------------------------*/
TRM::Set_Compatibility_Info  set_compatibility_info;

/*-----------------------------------------------------------------------------
  Client log code

  These values are used by the TRM log parser to identify clients.
  The values must not change.
-----------------------------------------------------------------------------*/

enum trmlog_client_enum_t
{
  TRMLOG_1X                   = 0,
  TRMLOG_1X_SECONDARY         = 1,
  TRMLOG_HDR                  = 2,
  TRMLOG_HDR_SECONDARY        = 3,
  TRMLOG_GPS                  = 4,
  TRMLOG_UMTS                 = 5,
  TRMLOG_UMTS_SECONDARY       = 6,
  TRMLOG_GSM1                 = 7,
  TRMLOG_GSM2                 = 8,
  TRMLOG_WCDMA                = 9,
  TRMLOG_TDSCDMA              = 10,
  TRMLOG_TDSCDMA_SECONDARY    = 11,
  TRMLOG_UMTS_CA              = 12,
  TRMLOG_UMTS_CA_SECONDARY    = 13,
  TRMLOG_LTE                  = 14,
  TRMLOG_LTE_SECONDARY        = 15,
  TRMLOG_LTE_CA               = 16,
  TRMLOG_LTE_CA_SECONDARY     = 17,
  TRMLOG_CM                   = 18,
  TRMLOG_GSM3                 = 19,
  TRMLOG_GSM_SECONDARY        = 20,
  TRMLOG_RF                   = 21,
  TRMLOG_IRAT                 = 22,
  TRMLOG_WLAN                 = 23,
  TRMLOG_GPRS1                = 24,
  TRMLOG_GPRS2                = 25,
  TRMLOG_LTE_CA1              = 26,
  TRMLOG_LTE_CA1_SECONDARY    = 27,
  TRMLOG_GSM1_SECONDARY       = 28,
  TRMLOG_GSM2_SECONDARY       = 29,
  TRMLOG_GSM3_SECONDARY       = 30,
  TRMLOG_LTE_HO_SECONDARY1    = 31,
  TRMLOG_LTE_HO_SECONDARY2    = 32,
  TRMLOG_LTE_EMBMS1           = 33,
  TRMLOG_LTE_EMBMS2           = 34,

  TRMLOG_MAX                  = TRMLOG_LTE_EMBMS2
};



/*=============================================================================

                   CLIENT/REASON -> PRIORITY & MODE MAPPING

  Conditional compilation may be used to remove some of the following arrays,
  to conserve memory, if a specific target does not need them.

=============================================================================*/


/*-----------------------------------------------------------------------------
  Priority/Mode for a given client/reason pair
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Priority for given client/reason */
  trm_pri_t                   pri;

  /* Mode for the the current client/reason */
  trm_mode_id_enum_t               mode;
}
trmcfg_pri_mode_t;



/*-----------------------------------------------------------------------------
  TRM_1X 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_1x[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 230, TRM_MODE_1X },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_1X },
  /* TRM_BROADCAST_ACCESS */          { 230, TRM_MODE_1X },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_1X },
  /* TRM_TRAFFIC */                   { 240, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE */                { 185, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { 145, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_QPCH */                { 185, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION */               { 85, TRM_MODE_1X_ACQ },
  /* TRM_OFS */                       { BAD, TRM_MODE_1X },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_1X_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_1X },
  /* TRM_BROADCAST_TRAFFIC */         { 240, TRM_MODE_1X },
  /* TRM_LOW_LAT_TRAFFIC */           { 240, TRM_MODE_1X },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_1X },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_1X },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_1X_ACQ },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_1X },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_1X },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_1X },
  /* TRM_CHANNEL_MAINTENANCE */       { 130, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_INV */           { 122, TRM_MODE_1X_ACQ },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_1X_ACQ },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_1X },
  /* TRM_BG_TRAFFIC */                { 95, TRM_MODE_1X },
  /* TRM_SYSTEM_MEASUREMENT */        {  50, TRM_MODE_1X },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_1X },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_1X },
  /* TRM_IRAT_MEASUREMENT */          { 60, TRM_MODE_1X_IRAT_MEASUREMENT },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_1X_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_1X_INTERLOCK_DIV},
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_1X_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_1X_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_1X_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { 150, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_1X },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_1X }, 
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_1X },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_1X },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { 210, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_1X },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_1X },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_1X },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_1X },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_1X },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_1X },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_1X },
};


/*-----------------------------------------------------------------------------
  TRM_1X_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_1x_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 230, TRM_MODE_1X },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_1X },
  /* TRM_BROADCAST_ACCESS */          { 230, TRM_MODE_1X },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_1X },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE */                { 185, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { 145, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_QPCH */                { 185, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION */               { 152, TRM_MODE_1X_ACQ },
  /* TRM_OFS */                       {  45, TRM_MODE_1X },
  /* TRM_DIVERSITY */                 {  30, TRM_MODE_1X_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_1X },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_1X },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_1X },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_1X },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_1X },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_1X_ACQ },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_1X },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_1X },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_1X },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_1X },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_1X_ACQ },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_1X_ACQ },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_1X },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_1X },
  /* TRM_SYSTEM_MEASUREMENT */        {  50, TRM_MODE_1X },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_1X },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_1X },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_1X_IRAT_MEASUREMENT },
  /* TRM_ENVELOPE_TRACKING */         {  15, TRM_MODE_1X_ET },
  /* TRM_DIVERSITY_INTERLOCK */       {  15, TRM_MODE_1X_INTERLOCK_DIV},
  /* TRM_DIVERSITY_LOWCMTHRESH */     {  20, TRM_MODE_1X_DIV},
  /* TRM_ACCESS_DIVERSITY */          {  30, TRM_MODE_1X_DIV},
  /* TRM_IDLE_DIVERSITY */            {  10, TRM_MODE_1X_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { 150, TRM_MODE_1X_DR_CAPABLE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_1X },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_1X }, 
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_1X_ACQ },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_1X },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { 210, TRM_MODE_1X },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_1X },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_1X },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ 060, TRM_MODE_1X_IRAT_MEASUREMENT },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_1X },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_1X },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_1X },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_1X },
};


/*-----------------------------------------------------------------------------
  TRM_HDR 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_hdr[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 125, TRM_MODE_HDR_CONNECTED },
  /* TRM_ACCESS_URGENT */             { 225, TRM_MODE_HDR_CONNECTED },
  /* TRM_BROADCAST_ACCESS */          { 225, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_HDR },
  /* TRM_TRAFFIC */                   {  95, TRM_MODE_HDR_CONNECTED },
  /* TRM_DEMOD_PAGE */                { 120, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_BROADCAST */           {  65, TRM_MODE_HDR_BROADCAST },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION */               { 105, TRM_MODE_HDR },
  /* TRM_OFS */                       { BAD, TRM_MODE_HDR },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_HDR_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_HDR },
  /* TRM_BROADCAST_TRAFFIC */         {  65, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOW_LAT_TRAFFIC */           { 225, TRM_MODE_HDR_LOW_LAT },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_HDR },
  /* TRM_SMALL_SCI_PAGE */            {  65, TRM_MODE_HDR_SMALL_SCI_IDLE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_HDR },
  /* TRM_HP_BROADCAST */              { 225, TRM_MODE_HDR_LOW_LAT },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { 183, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_HDR_BROADCAST },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_HDR },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_HDR },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_HDR },
  /* TRM_CHANNEL_MAINTENANCE */       { 130, TRM_MODE_HDR_IDLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_HDR },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_HDR },
  /* TRM_BG_TRAFFIC */                { 95, TRM_MODE_HDR_BG_CONNECTED },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_HDR },
  /* TRM_MC_TRAFFIC */                {  95, TRM_MODE_HDR_MCDO_CONNECTED },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ 183, TRM_MODE_HDR_SMALL_SCI_IDLE },
  /* TRM_IRAT_MEASUREMENT */          {  95, TRM_MODE_HDR_IRAT_MEASUREMENT },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_HDR_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_HDR},
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_HDR_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_HDR_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_HDR_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_HDR },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_HDR }, 
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_HDR },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_HDR },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { 210, TRM_MODE_HDR_IDLE },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_HDR },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_HDR },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_HDR },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_HDR },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_HDR },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_HDR },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_HDR },
};


/*-----------------------------------------------------------------------------
  TRM_HDR_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_hdr_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 125, TRM_MODE_HDR_CONNECTED },
  /* TRM_ACCESS_URGENT */             { 225, TRM_MODE_HDR_CONNECTED },
  /* TRM_BROADCAST_ACCESS */          { 225, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_HDR },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_HDR_CONNECTED },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_HDR_BROADCAST },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_HDR },
  /* TRM_OFS */                       {  40, TRM_MODE_HDR },
  /* TRM_DIVERSITY */                 {  25, TRM_MODE_HDR_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_HDR },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_HDR_LOW_LAT },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_HDR },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_HDR },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_HDR_LOW_LAT },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_HDR_BROADCAST },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_HDR },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_HDR },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_HDR },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_HDR },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_HDR },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_HDR },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_HDR },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_HDR },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_HDR_MCDO_CONNECTED },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_HDR_IDLE },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_HDR_IRAT_MEASUREMENT },
  /* TRM_ENVELOPE_TRACKING */         {  10, TRM_MODE_HDR_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_HDR},
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_HDR_DIV},
  /* TRM_ACCESS_DIVERSITY */          {  15, TRM_MODE_HDR_DIV},
  /* TRM_IDLE_DIVERSITY */            {  15, TRM_MODE_HDR_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_HDR },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_HDR }, 
  /* TRM_ACQ_DIVERSITY */             { 15, TRM_MODE_HDR_DIV },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_HDR },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_HDR },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_HDR },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_HDR },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_HDR },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_HDR },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_HDR },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_HDR },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_HDR },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_HDR },
};


/*-----------------------------------------------------------------------------
  TRM_GPS 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_gps[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_GPS },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_GPS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_GPS },
  /* TRM_LOCATION_FIX */              { 190, TRM_MODE_GPS },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_GPS */                 { 190, TRM_MODE_GPS },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_GPS },
  /* TRM_OFS */                       { BAD, TRM_MODE_GPS },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_GPS },
  /* TRM_GPS_TDM */                   { 220, TRM_MODE_GPS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_GPS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_GPS },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_GPS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_GPS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_GPS },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_GPS },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_GPS },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_GPS },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_GPS },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_GPS },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_GPS },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_GPS },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_GPS },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_GPS },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_GPS },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_GPS },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_GPS },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_GPS },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_GPS },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_GPS },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_GPS },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_GPS },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_GPS },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_GPS },  
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_GPS },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_GPS },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_GPS },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_GPS },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_GPS },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_GPS },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_GPS },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_GPS },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_GPS },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_GPS },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_GPS },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_GPS },
};


/*-----------------------------------------------------------------------------
  TRM_UMTS
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_umts[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 230, TRM_MODE_UMTS },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_UMTS },
  /* TRM_BROADCAST_ACCESS */          { 230, TRM_MODE_UMTS },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_UMTS },
  /* TRM_TRAFFIC */                   { 240, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE */                { 200, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { 155, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION */               {  85, TRM_MODE_UMTS },
  /* TRM_OFS */                       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_UMTS_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_UMTS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_UMTS },
  /* TRM_TOTAL_CONTROL */             { 225, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_UMTS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT */                  { 210, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE */       { 135, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_BG_TRAFFIC */                {  95, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_UMTS },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_UMTS },
  /* TRM_IRAT_MEASUREMENT */          { 20, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_UMTS_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_UMTS },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_UMTS },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_LOW */           {  65, TRM_MODE_UMTS },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_UMTS },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_UMTS },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_UMTS },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_UMTS },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_UMTS },
};


/*-----------------------------------------------------------------------------
  TRM_UMTS_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_umts_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_UMTS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_UMTS },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_UMTS },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_UMTS },
  /* TRM_OFS */                       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY */                 { 50, TRM_MODE_UMTS_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_UMTS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_UMTS },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_UMTS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_UMTS },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_UMTS },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_UMTS },
  /* TRM_ENVELOPE_TRACKING */         {  10, TRM_MODE_UMTS_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_UMTS},
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_UMTS },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_UMTS },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_UMTS },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_UMTS },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_UMTS },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_UMTS },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_UMTS },
};


/*-----------------------------------------------------------------------------
  TRM_GSM1, TRM_GSM2, TRM_GSM3, TRM_GPRS1 and TRM_GPRS2
-----------------------------------------------------------------------------*/
static const trmcfg_pri_mode_t  trmcfg_pri_mode_gsm[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 230, TRM_MODE_GSM },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_GSM },
  /* TRM_BROADCAST_ACCESS */          { 230, TRM_MODE_GSM },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_GSM },
  /* TRM_TRAFFIC */                   { 240, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE */                { 165, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { 140, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION */               {  80, TRM_MODE_GSM },
  /* TRM_OFS */                       { BAD, TRM_MODE_GSM },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_GSM },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_GSM },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_GSM },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_GSM },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_GSM },
  /* TRM_SMALL_SCI_PAGE */            { 130, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_MAX_SENS */            { 180, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_HP_BROADCAST */              { 204, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { 165, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_PAGE_HI */             { 199, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_BROADCAST_HI */        { 160, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_ACQUISITION_HI */            {  90, TRM_MODE_GSM },
  /* TRM_DEMOD_MAX_SENS_HI */         { 201, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_RESELECT */                  { 205, TRM_MODE_GSM_DR_CAPABLE },
  /* TRM_RESELECT_HI */               { 215, TRM_MODE_GSM_DR_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE */       { 130, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { 150, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_CBCH */                {  55, TRM_MODE_GSM_SLTE_CAPABLE }, 
  /* TRM_DEMOD_CBCH_HI */             {  60, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_ACQUISITION_INV */           { 110, TRM_MODE_GSM },
  /* TRM_ACQUISITION_INV_HI */        { 115, TRM_MODE_GSM },
  /* TRM_SMALL_SCI_PAGE_HI */         { 150, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_PAGE_INV */            { 203, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_DEMOD_PAGE_INV_HI */         { 203, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_BG_TRAFFIC */                {  70, TRM_MODE_GSM_DR_CAPABLE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_GSM },
  /* TRM_MC_TRAFFIC */                {  95, TRM_MODE_GSM },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_GSM },
  /* TRM_IRAT_MEASUREMENT */          { 50, TRM_MODE_GSM_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_GSM },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_GSM},
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_GSM},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_GSM},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_GSM},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { 199, TRM_MODE_GSM_SLTE_CAPABLE },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_GSM },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_LOW */           { 80,  TRM_MODE_GSM },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_MAX */           { 218, TRM_MODE_GSM },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_GSM },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_GSM },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_GSM },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_GSM },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_GSM },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_GSM },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_GSM },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_GSM },
};

/*-----------------------------------------------------------------------------
  TRM_GSM_SECONDARY, TRM_GSM1_SECONDARY, TRM_GSM2_SECONDARY, TRM_GSM3_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_gsm_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_GSM },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_GSM },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_GSM },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_GSM },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_GSM },
  /* TRM_OFS */                       { BAD, TRM_MODE_GSM },
  /* TRM_DIVERSITY */                 { 20,  TRM_MODE_GSM_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_GSM },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_GSM },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_GSM },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_GSM },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_GSM },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_GSM },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_GSM },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_GSM },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_GSM },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_GSM },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_GSM },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_GSM },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_GSM },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_GSM },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_GSM },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_GSM },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_GSM },
  /* TRM_DIVERSITY_INTERLOCK */       { 250, TRM_MODE_GSM_DIV},
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_GSM },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_GSM },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_GSM },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_GSM },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_GSM },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_GSM },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_GSM },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_GSM },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_GSM },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_GSM },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_GSM },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_GSM },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_GSM },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_GSM },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_GSM },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_GSM },
};


/*-----------------------------------------------------------------------------
  TRM_TDSCDMA 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_tdscdma[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 230, TRM_MODE_TDSCDMA },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_TDSCDMA },
  /* TRM_BROADCAST_ACCESS */          { 230, TRM_MODE_TDSCDMA },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_TDSCDMA },
  /* TRM_TRAFFIC */                   { 240, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE */                { 195, TRM_MODE_TDSCDMA_DR_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { 155, TRM_MODE_TDSCDMA_DR_CAPABLE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION */               {  85, TRM_MODE_TDSCDMA },
  /* TRM_OFS */                       { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_TOTAL_CONTROL */             { 225, TRM_MODE_TDSCDMA },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_RESELECT */                  { 210, TRM_MODE_TDSCDMA_DR_CAPABLE },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_TDSCDMA },
  /* TRM_CHANNEL_MAINTENANCE */       { 135, TRM_MODE_TDSCDMA_DR_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_INV */           { 110, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_BG_TRAFFIC */                {  95, TRM_MODE_TDSCDMA_DR_CAPABLE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_TDSCDMA },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_TDSCDMA },
  /* TRM_IRAT_MEASUREMENT */          { 20, TRM_MODE_TDSCDMA_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_LOW */           {  65, TRM_MODE_TDSCDMA },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_TDSCDMA },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_TDSCDMA },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_TDSCDMA },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_TDSCDMA },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_TDSCDMA },
};


/*-----------------------------------------------------------------------------
  TRM_TDSCDMA_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_tdscdma_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_TDSCDMA },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_TDSCDMA },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_TDSCDMA },
  /* TRM_OFS */                       { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DIVERSITY */                 {  10, TRM_MODE_TDSCDMA_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_TDSCDMA },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_TDSCDMA },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_TDSCDMA },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_TDSCDMA },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_TDSCDMA },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_TDSCDMA },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_TDSCDMA_DIV },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_TDSCDMA},
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_TDSCDMA},
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_TDSCDMA },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_TDSCDMA },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_TDSCDMA },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_TDSCDMA },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_TDSCDMA },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_TDSCDMA },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_TDSCDMA },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_TDSCDMA },
};

/*-----------------------------------------------------------------------------
  TRM_UMTS_CA 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_umts_ca[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_UMTS },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_UMTS },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_UMTS },
  /* TRM_OFS */                       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_UMTS_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_UMTS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_UMTS },
  /* TRM_TOTAL_CONTROL */             { 11, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_UMTS  },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_UMTS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_UMTS },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_UMTS },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_UMTS },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_UMTS_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_UMTS},
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_UMTS},
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_UMTS},
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_UMTS },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_UMTS },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_UMTS },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_UMTS },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_UMTS },
};


/*-----------------------------------------------------------------------------
  TRM_UMTS_CA_SECONDARY 
-----------------------------------------------------------------------------*/
static const trmcfg_pri_mode_t  trmcfg_pri_mode_umts_ca_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_UMTS },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_UMTS },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_UMTS },
  /* TRM_OFS */                       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY */                 { 10, TRM_MODE_UMTS_DR_CAPABLE },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_UMTS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_UMTS },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_UMTS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_UMTS },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_UMTS },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_UMTS },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_UMTS },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_UMTS },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_UMTS_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_UMTS_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_UMTS},
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_UMTS},
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_UMTS},
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_UMTS },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_UMTS },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_UMTS },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_UMTS },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_UMTS },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_UMTS },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_UMTS },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_UMTS },
};

/*-----------------------------------------------------------------------------
  TRM_LTE
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_lte[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 125, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ACCESS_URGENT */             { 235, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_LTE },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_LTE },
  /* TRM_TRAFFIC */                   { 240, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_DEMOD_PAGE */                { 185, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION */               { 85, TRM_MODE_LTE },
  /* TRM_OFS */                       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_LTE_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_LTE },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_LTE },
  /* TRM_LOW_LAT_TRAFFIC */           { 245, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_TOTAL_CONTROL */             { 245, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_LTE  },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_LTE },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_RESELECT */                  { 210, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE */       { 130, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV */           { 112, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_BG_TRAFFIC */                { 95,  TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_LTE },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_LTE },
  /* TRM_IRAT_MEASUREMENT */          { 20,  TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_LTE_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_LTE_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_LTE_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_LTE_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_LTE},
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_LTE},
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_LTE},
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_LOW */           { 65, TRM_MODE_LTE },
  /* TRM_SLEEP */                     {   5, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { 210, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ 210, TRM_MODE_LTE },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { 170, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_LTE },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_LTE },
  /* TRM_EMBMS */                     { 125, TRM_MODE_LTE_DR_CAPABLE },  
  /* TRM_EMBMS_INV */                 { 225, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_EMBMS_LOW */                 {  95, TRM_MODE_LTE_DR_CAPABLE },
};


/*-----------------------------------------------------------------------------
  TRM_LTE_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_lte_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_LTE },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_LTE },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_LTE },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_LTE },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_LTE },
  /* TRM_OFS */                       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY */                 { 50, TRM_MODE_LTE_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_LTE },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_LTE },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_LTE },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_LTE },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_LTE },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_LTE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_LTE },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_LTE },
  /* TRM_IRAT_MEASUREMENT */          {  10, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         {  10, TRM_MODE_LTE_ET },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_LTE_DIV},
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_LTE_DIV},
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_LTE_DIV},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_LTE},
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_LTE},
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_LTE },
  /* TRM_SLEEP */                     {   4, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_LTE },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_LTE },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_LTE },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_LTE },
  /* TRM_SLEEP_DIVERSITY */           { 4, TRM_MODE_LTE_DIV },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_LTE },
};

/*-----------------------------------------------------------------------------
  TRM_LTE_CA
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_lte_ca[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_ACCESS_URGENT */             {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_LTE },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_LTE },
  /* TRM_TRAFFIC */                   {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_DEMOD_PAGE */                {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION */               {  11, TRM_MODE_LTE },
  /* TRM_OFS */                       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_LTE },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_LTE },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_LTE },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_LTE },
  /* TRM_TOTAL_CONTROL */             {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_LTE },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_RESELECT */                  {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE */       {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV */           {  11, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_BG_TRAFFIC */                {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_LTE },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_LTE },
  /* TRM_IRAT_MEASUREMENT */          {  11, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_LTE },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_LTE },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_LTE },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_LTE },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_LTE },
  /* TRM_SLEEP */                     {  11, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_LTE },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_LTE },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_LTE },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_LTE },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_LTE },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { 11, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_SINGLE_RX_CA_DIV */          { 11, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_LTE },
};

/*-----------------------------------------------------------------------------
  TRM_LTE_CA_SECONDARY
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_lte_ca_secondary[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_LTE },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_LTE },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_LTE },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_LTE },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_LTE },
  /* TRM_OFS */                       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY */                 {  10, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_LTE },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_LTE },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_LTE },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_LTE },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_LTE },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_LTE },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_LTE },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_LTE },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_LTE },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_LTE },
  /* TRM_IRAT_MEASUREMENT */          { 10, TRM_MODE_LTE_DR_CAPABLE },
  /* TRM_ENVELOPE_TRACKING */         { 10, TRM_MODE_LTE_SLTE_CAPABLE },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_LTE },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_LTE },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_LTE },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_LTE },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_LTE },
  /* TRM_SUBS_CAP_CHANGE*/            { BAD, TRM_MODE_LTE },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_LTE },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_LTE },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_LTE },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_LTE },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_LTE },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_LTE },
  /* TRM_TECH_CHANGE                */{ 220, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_LTE },
  /* TRM_SLEEP_DIVERSITY */           { 4, TRM_MODE_LTE_DIV },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { 11, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_SINGLE_RX_CA_DIV */          { 11, TRM_MODE_LTE_SINGLE_RX_CA },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_LTE },
};


/*-----------------------------------------------------------------------------
  TRM_CM
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_cm[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_CM },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_CM },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_CM },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_CM },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_CM },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_CM },
  /* TRM_OFS */                       { BAD, TRM_MODE_CM },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_CM },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_CM },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_CM },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_CM },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_CM },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_CM },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE_HI */             { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD, TRM_MODE_CM },
  /* TRM_ACQUISITION_HI */            { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD, TRM_MODE_CM },
  /* TRM_RESELECT */                  { BAD, TRM_MODE_CM },
  /* TRM_RESELECT_HI */               { BAD, TRM_MODE_CM },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD, TRM_MODE_CM },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_CBCH */                { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_CBCH_HI */             { BAD, TRM_MODE_CM },
  /* TRM_ACQUISITION_INV */           { BAD, TRM_MODE_CM },
  /* TRM_ACQUISITION_INV_HI */        { BAD, TRM_MODE_CM },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE_INV */            { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD, TRM_MODE_CM },
  /* TRM_BG_TRAFFIC */                { BAD, TRM_MODE_CM },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD, TRM_MODE_CM },
  /* TRM_MC_TRAFFIC */                { BAD, TRM_MODE_CM },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD, TRM_MODE_CM },
  /* TRM_IRAT_MEASUREMENT */          { BAD, TRM_MODE_CM },
  /* TRM_ENVELOPE_TRACKING */         { BAD, TRM_MODE_CM },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD, TRM_MODE_CM },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD, TRM_MODE_CM },
  /* TRM_ACCESS_DIVERSITY */          { BAD, TRM_MODE_CM },
  /* TRM_IDLE_DIVERSITY */            { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD, TRM_MODE_CM },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD, TRM_MODE_CM },
  /* TRM_SUBS_CAP_CHANGE*/            { 220, TRM_MODE_CM },
  /* TRM_ACQ_DIVERSITY */             { BAD, TRM_MODE_CM },
  /* TRM_ACQUISITION_LOW */           { BAD, TRM_MODE_CM },
  /* TRM_SLEEP */                     { BAD, TRM_MODE_CM },
  /* TRM_ACQUISITION_MAX */           { BAD, TRM_MODE_CM },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD, TRM_MODE_CM },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD, TRM_MODE_CM },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD, TRM_MODE_CM },
  /* TRM_TECH_CHANGE                */{ BAD, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD, TRM_MODE_CM },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_CM },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_CM },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_CM },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_CM },
};

/*-----------------------------------------------------------------------------
  TRM_RF
-----------------------------------------------------------------------------*/
static const trmcfg_pri_mode_t  trmcfg_pri_mode_rf[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { BAD,TRM_MODE_RF },
  /* TRM_ACCESS_URGENT */             { BAD,TRM_MODE_RF },
  /* TRM_BROADCAST_ACCESS */          { BAD,TRM_MODE_RF },
  /* TRM_LOCATION_FIX */              { BAD,TRM_MODE_RF },
  /* TRM_TRAFFIC */                   { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_PAGE */                { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_BROADCAST */           { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_QPCH */                { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_GPS */                 { BAD,TRM_MODE_RF },
  /* TRM_ACQUISITION */               { BAD,TRM_MODE_RF },
  /* TRM_OFS */                       { BAD,TRM_MODE_RF },
  /* TRM_DIVERSITY */                 { BAD,TRM_MODE_RF },
  /* TRM_GPS_TDM */                   { BAD,TRM_MODE_RF },
  /* TRM_BROADCAST_TRAFFIC */         { BAD,TRM_MODE_RF },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD,TRM_MODE_RF },
  /* TRM_TOTAL_CONTROL */             { BAD,TRM_MODE_RF },
  /* TRM_SMALL_SCI_PAGE */            { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_MAX_SENS */            { BAD,TRM_MODE_RF },
  /* TRM_HP_BROADCAST */              { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_PAGE_HI */             { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_BROADCAST_HI */        { BAD,TRM_MODE_RF },
  /* TRM_ACQUISITION_HI */            { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_MAX_SENS_HI */         { BAD,TRM_MODE_RF },
  /* TRM_RESELECT */                  { BAD,TRM_MODE_RF },
  /* TRM_RESELECT_HI */               { BAD,TRM_MODE_RF },
  /* TRM_CHANNEL_MAINTENANCE */       { BAD,TRM_MODE_RF },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_CBCH */                { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_CBCH_HI */             { BAD,TRM_MODE_RF },
  /* TRM_ACQUISITION_INV */           { BAD,TRM_MODE_RF },
  /* TRM_ACQUISITION_INV_HI */        { BAD,TRM_MODE_RF },
  /* TRM_SMALL_SCI_PAGE_HI */         { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_PAGE_INV */            { BAD,TRM_MODE_RF },
  /* TRM_DEMOD_PAGE_INV_HI */         { BAD,TRM_MODE_RF },
  /* TRM_BG_TRAFFIC */                { BAD,TRM_MODE_RF },
  /* TRM_SYSTEM_MEASUREMENT */        { BAD,TRM_MODE_RF },
  /* TRM_MC_TRAFFIC */                { BAD,TRM_MODE_RF },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ BAD,TRM_MODE_RF },
  /* TRM_IRAT_MEASUREMENT */          { BAD,TRM_MODE_RF  },
  /* TRM_ENVELOPE_TRACKING */         { BAD,TRM_MODE_RF },
  /* TRM_DIVERSITY_INTERLOCK */       { BAD,TRM_MODE_RF },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { BAD,TRM_MODE_RF},
  /* TRM_ACCESS_DIVERSITY */          { BAD,TRM_MODE_RF},
  /* TRM_IDLE_DIVERSITY */            { BAD,TRM_MODE_RF},
  /* TRM_DEMOD_PAGE_NONURGENT */      { BAD,TRM_MODE_RF},
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { BAD,TRM_MODE_RF},
  /* TRM_SUBS_CAP_CHANGE */           { BAD,TRM_MODE_RF},
  /* TRM_ACQ_DIVERSITY */             { BAD,TRM_MODE_RF },
  /* TRM_ACQUISITION_LOW */           { BAD,TRM_MODE_RF },
  /* TRM_SLEEP */                     { BAD,TRM_MODE_RF },
  /* TRM_ACQUISITION_MAX */           { BAD,TRM_MODE_RF },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { BAD,TRM_MODE_RF },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ BAD,TRM_MODE_RF },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { BAD,TRM_MODE_RF },
  /* TRM_TECH_CHANGE                */{ BAD, TRM_MODE_TECH_CHANGE },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ BAD,TRM_MODE_RF },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_RF },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_RF },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_RF },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_RF },
};


/*-----------------------------------------------------------------------------
  TRM_IRAT, inorder to make IRAT client non-premptible, priority is set to 255
-----------------------------------------------------------------------------*/
static const trmcfg_pri_mode_t  trmcfg_pri_mode_irat[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 255,TRM_MODE_IRAT },
  /* TRM_ACCESS_URGENT */             { 255,TRM_MODE_IRAT },
  /* TRM_BROADCAST_ACCESS */          { 255,TRM_MODE_IRAT },
  /* TRM_LOCATION_FIX */              { 255,TRM_MODE_IRAT },
  /* TRM_TRAFFIC */                   { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE */                { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_BROADCAST */           { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_QPCH */                { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_GPS */                 { 255,TRM_MODE_IRAT },
  /* TRM_ACQUISITION */               { 255,TRM_MODE_IRAT },
  /* TRM_OFS */                       { 255,TRM_MODE_IRAT },
  /* TRM_DIVERSITY */                 { 255,TRM_MODE_IRAT },
  /* TRM_GPS_TDM */                   { 255,TRM_MODE_IRAT },
  /* TRM_BROADCAST_TRAFFIC */         { 255,TRM_MODE_IRAT },
  /* TRM_LOW_LAT_TRAFFIC */           { 255,TRM_MODE_IRAT },
  /* TRM_TOTAL_CONTROL */             { 255,TRM_MODE_IRAT },
  /* TRM_SMALL_SCI_PAGE */            { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_MAX_SENS */            { 255,TRM_MODE_IRAT },
  /* TRM_HP_BROADCAST */              { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE_HI */             { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_BROADCAST_HI */        { 255,TRM_MODE_IRAT },
  /* TRM_ACQUISITION_HI */            { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_MAX_SENS_HI */         { 255,TRM_MODE_IRAT },
  /* TRM_RESELECT */                  { 255,TRM_MODE_IRAT },
  /* TRM_RESELECT_HI */               { 255,TRM_MODE_IRAT },
  /* TRM_CHANNEL_MAINTENANCE */       { 255,TRM_MODE_IRAT },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_CBCH */                { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_CBCH_HI */             { 255,TRM_MODE_IRAT },
  /* TRM_ACQUISITION_INV */           { 255,TRM_MODE_IRAT },
  /* TRM_ACQUISITION_INV_HI */        { 255,TRM_MODE_IRAT },
  /* TRM_SMALL_SCI_PAGE_HI */         { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE_INV */            { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE_INV_HI */         { 255,TRM_MODE_IRAT },
  /* TRM_BG_TRAFFIC */                { 255,TRM_MODE_IRAT },
  /* TRM_SYSTEM_MEASUREMENT */        { 255,TRM_MODE_IRAT },
  /* TRM_MC_TRAFFIC */                { 255,TRM_MODE_IRAT },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ 255,TRM_MODE_IRAT },
  /* TRM_IRAT_MEASUREMENT */          { 255,TRM_MODE_IRAT },
  /* TRM_ENVELOPE_TRACKING */         { 255,TRM_MODE_IRAT },
  /* TRM_DIVERSITY_INTERLOCK */       { 255,TRM_MODE_IRAT },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { 255,TRM_MODE_IRAT },
  /* TRM_ACCESS_DIVERSITY */          { 255,TRM_MODE_IRAT },
  /* TRM_IDLE_DIVERSITY */            { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE_NONURGENT */      { 255,TRM_MODE_IRAT },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { 255,TRM_MODE_IRAT },
  /* TRM_SUBS_CAP_CHANGE */           { 255,TRM_MODE_IRAT },
  /* TRM_ACQ_DIVERSITY */             { 255,TRM_MODE_IRAT },
  /* TRM_ACQUISITION_LOW */           { 255,TRM_MODE_IRAT },
  /* TRM_SLEEP */                     { 255,TRM_MODE_IRAT },
  /* TRM_ACQUISITION_MAX */           { 255,TRM_MODE_IRAT },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { 255,TRM_MODE_IRAT },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ 255,TRM_MODE_IRAT },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { 255,TRM_MODE_IRAT },
  /* TRM_TECH_CHANGE                */{ 255,TRM_MODE_IRAT },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ 255,TRM_MODE_IRAT },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_IRAT },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_IRAT },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_IRAT },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_IRAT },
};

/*-----------------------------------------------------------------------------
  TRM_IRAT, inorder to make IRAT client non-premptible, priority is set to 255
-----------------------------------------------------------------------------*/
static const trmcfg_pri_mode_t  trmcfg_pri_mode_wlan[TRM_REASON_MAX] = 
{
  /* TRM_ACCESS        */             { 8,TRM_MODE_WLAN },
  /* TRM_ACCESS_URGENT */             { 8,TRM_MODE_WLAN },
  /* TRM_BROADCAST_ACCESS */          { 8,TRM_MODE_WLAN },
  /* TRM_LOCATION_FIX */              { 8,TRM_MODE_WLAN },
  /* TRM_TRAFFIC */                   { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE */                { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_BROADCAST */           { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_QPCH */                { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_GPS */                 { 8,TRM_MODE_WLAN },
  /* TRM_ACQUISITION */               { 8,TRM_MODE_WLAN },
  /* TRM_OFS */                       { 8,TRM_MODE_WLAN },
  /* TRM_DIVERSITY */                 { 8,TRM_MODE_WLAN },
  /* TRM_GPS_TDM */                   { 8,TRM_MODE_WLAN },
  /* TRM_BROADCAST_TRAFFIC */         { 8,TRM_MODE_WLAN },
  /* TRM_LOW_LAT_TRAFFIC */           { 8,TRM_MODE_WLAN },
  /* TRM_TOTAL_CONTROL */             { 8,TRM_MODE_WLAN },
  /* TRM_SMALL_SCI_PAGE */            { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_MAX_SENS */            { 8,TRM_MODE_WLAN },
  /* TRM_HP_BROADCAST */              { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE_HI */             { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_BROADCAST_HI */        { 8,TRM_MODE_WLAN },
  /* TRM_ACQUISITION_HI */            { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_MAX_SENS_HI */         { 8,TRM_MODE_WLAN },
  /* TRM_RESELECT */                  { 8,TRM_MODE_WLAN },
  /* TRM_RESELECT_HI */               { 8,TRM_MODE_WLAN },
  /* TRM_CHANNEL_MAINTENANCE */       { 8,TRM_MODE_WLAN },
  /* TRM_CHANNEL_MAINTENANCE_HI */    { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_CBCH */                { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_CBCH_HI */             { 8,TRM_MODE_WLAN },
  /* TRM_ACQUISITION_INV */           { 8,TRM_MODE_WLAN },
  /* TRM_ACQUISITION_INV_HI */        { 8,TRM_MODE_WLAN },
  /* TRM_SMALL_SCI_PAGE_HI */         { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE_INV */            { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE_INV_HI */         { 8,TRM_MODE_WLAN },
  /* TRM_BG_TRAFFIC */                { 8,TRM_MODE_WLAN },
  /* TRM_SYSTEM_MEASUREMENT */        { 8,TRM_MODE_WLAN },
  /* TRM_MC_TRAFFIC */                { 8,TRM_MODE_WLAN },
  /* TRM_SMALL_SCI_PAGE_CONTINUATION*/{ 8,TRM_MODE_WLAN },
  /* TRM_IRAT_MEASUREMENT */          { 8,TRM_MODE_WLAN },
  /* TRM_ENVELOPE_TRACKING */         { 8,TRM_MODE_WLAN },
  /* TRM_DIVERSITY_INTERLOCK */       { 8,TRM_MODE_WLAN },
  /* TRM_DIVERSITY_LOWCMTHRESH */     { 8,TRM_MODE_WLAN },
  /* TRM_ACCESS_DIVERSITY */          { 8,TRM_MODE_WLAN },
  /* TRM_IDLE_DIVERSITY */            { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE_NONURGENT */      { 8,TRM_MODE_WLAN },
  /* TRM_DEMOD_PAGE_CONTINUATION_HI*/ { 8,TRM_MODE_WLAN },
  /* TRM_SUBS_CAP_CHANGE */           { 8,TRM_MODE_WLAN },
  /* TRM_ACQ_DIVERSITY */             { 8,TRM_MODE_WLAN },
  /* TRM_ACQUISITION_LOW */           { 8,TRM_MODE_WLAN },
  /* TRM_SLEEP */                     { 8,TRM_MODE_WLAN },
  /* TRM_ACQUISITION_MAX */           { 8,TRM_MODE_WLAN },
  /* TRM_CHANNEL_MAINTENANCE_INV */   { 8,TRM_MODE_WLAN },
  /* TRM_SIB_EMERGENCY_NOTIFICATION */{ 8,TRM_MODE_WLAN },
  /* TRM_HIGH_PRIORITY_SIGNALLING */  { 8,TRM_MODE_WLAN },
  /* TRM_TECH_CHANGE                */{ 8,TRM_MODE_WLAN },
  /* TRM_DIVERSITY_IRAT_MEASUREMENT */{ 8,TRM_MODE_WLAN },
  /* TRM_SLEEP_DIVERSITY */           { BAD, TRM_MODE_WLAN },
  /* TRM_EMBMS */                     { BAD, TRM_NUM_MODES },  
  /* TRM_EMBMS_INV */                 { BAD, TRM_NUM_MODES },
  /* TRM_SINGLE_RX_CA */              { BAD, TRM_MODE_WLAN },
  /* TRM_SINGLE_RX_CA_DIV */          { BAD, TRM_MODE_WLAN },
  /* TRM_EMBMS_LOW */                 { BAD, TRM_MODE_WLAN },
};


/*=============================================================================

                             CLIENT CONSTANT DATA

=============================================================================*/

/*-----------------------------------------------------------------------------
  Client const data structure type
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Log code for the client */
  trmlog_client_enum_t           log_code;

  /* Number of reasons */
  trm_reason_enum_t              reasons;

  /* Mode for the the current client/reason */
  const trmcfg_pri_mode_t *      pri_mode;
}
trmcfg_client_t;

/*-----------------------------------------------------------------------------
  Array of all diversity modes
-----------------------------------------------------------------------------*/
static const trm_mode_id_enum_t trmcfg_div_modes[] =
{
  TRM_MODE_1X_DIV,
  TRM_MODE_HDR_DIV,
  TRM_MODE_UMTS_DIV,
  TRM_MODE_TDSCDMA_DIV,
  TRM_MODE_GSM_DIV,
  TRM_MODE_LTE_DIV,
};

/*-----------------------------------------------------------------------------
  Array of all dr modes
-----------------------------------------------------------------------------*/
static const trm_mode_id_enum_t trmcfg_dr_modes[] =
{
  TRM_MODE_1X_DR_CAPABLE,
  TRM_MODE_1X_IRAT_MEASUREMENT,
  TRM_MODE_UMTS_DR_CAPABLE,
  TRM_MODE_GSM_SLTE_CAPABLE,
  TRM_MODE_GSM_DR_CAPABLE,
  TRM_MODE_TDSCDMA_DR_CAPABLE,
  TRM_MODE_LTE_DR_CAPABLE,
  TRM_MODE_HDR_LOW_LAT,
  TRM_MODE_HDR_BROADCAST,
  TRM_MODE_HDR_BG_CONNECTED,
  TRM_MODE_HDR_IDLE,
  TRM_MODE_HDR_IRAT_MEASUREMENT,
  TRM_MODE_HDR_MCDO_CONNECTED,
  TRM_MODE_HDR_SMALL_SCI_IDLE
};

/*-----------------------------------------------------------------------------
  Client const data structures

  Conditional compilation may be used to remove some of the following array
  structures, to conserve memory, if a specific target does not need them.
-----------------------------------------------------------------------------*/

static const trmcfg_client_t trmcfg_client[] =
{
  /* TRM_1X */
  {
    TRMLOG_1X,
    TRM_REASON_MAX,
    trmcfg_pri_mode_1x
  },

  /* TRM_1X_SECONDARY */
  {
    TRMLOG_1X_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_1x_secondary
  },

  /* TRM_HDR */
  {
    TRMLOG_HDR,
    TRM_REASON_MAX,
    trmcfg_pri_mode_hdr
  },

  /* TRM_HDR_SECONDARY */
  {
    TRMLOG_HDR_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_hdr_secondary
  },

  /* TRM_GPS */
  {
    TRMLOG_GPS,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gps
  },

  /* TRM_UMTS */
  {
    TRMLOG_UMTS,
    TRM_REASON_MAX,
    trmcfg_pri_mode_umts
  },

  /* TRM_UMTS_SECONDARY */
  {
    TRMLOG_UMTS_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_umts_secondary
  },

  /* TRM_GSM1*/
  {
    TRMLOG_GSM1,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm
  },

  /* TRM_GSM2*/
  {
    TRMLOG_GSM2,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm
  },

  /* TRM_WCDMA*/
  {
    TRMLOG_WCDMA,
    TRM_REASON_MAX,
    trmcfg_pri_mode_umts
  },

  /* TRM_TDSCDMA */
  {
    TRMLOG_TDSCDMA,
    TRM_REASON_MAX,
    trmcfg_pri_mode_tdscdma
  },

  /* TRM_TDSCDMA_SECONDARY */
  {
    TRMLOG_TDSCDMA_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_tdscdma_secondary
  },

  /* TRM_UMTS_CA */
  {
    TRMLOG_UMTS_CA,
    TRM_REASON_MAX,
    trmcfg_pri_mode_umts_ca
  },

  /* TRM_UMTS_SECONDARY */
  {
    TRMLOG_UMTS_CA_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_umts_ca_secondary
  },


  /* TRM_LTE */
  {
    TRMLOG_LTE,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte
  },


  /* TRM_LTE_SECONDARY */
  {
    TRMLOG_LTE_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_secondary
  },

  /* TRM_LTE_CA */
  {
    TRMLOG_LTE_CA,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_ca
  },

  /* TRM_LTE_CA_SECONDARY */
  {
    TRMLOG_LTE_CA_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_ca_secondary
  },
 
  /* TRM_CM */
  {
    TRMLOG_CM,
    TRM_REASON_MAX,
    trmcfg_pri_mode_cm
  },

  /* TRM_GSM3*/
  {
    TRMLOG_GSM3,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm
  },

  /* TRM_GSM_SECONDARY*/
  {
    TRMLOG_GSM_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm_secondary
  },

  /* TRM_RF */
  {
    TRMLOG_RF,
    TRM_REASON_MAX,
    trmcfg_pri_mode_rf
  },

    /* TRM_IRAT */
  {
    TRMLOG_IRAT,
    TRM_REASON_MAX,
    trmcfg_pri_mode_irat
  },

    /* TRM_WLAN */
  {
    TRMLOG_WLAN,
    TRM_REASON_MAX,
    trmcfg_pri_mode_wlan
  },

  /* TRM_GPRS1*/
  {
    TRMLOG_GPRS1,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm
  },

  /* TRM_GPRS2*/
  {
    TRMLOG_GPRS2,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm
  },

  /* TRM_LTE_CA1 */
  {
    TRMLOG_LTE_CA1,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_ca
  },

  /* TRM_LTE_CA1_SECONDARY */
  {
    TRMLOG_LTE_CA1_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_ca_secondary
  },

  /* TRM_GSM1_SECONDARY*/
  {
    TRMLOG_GSM1_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm_secondary
  },

  /* TRM_GSM2_SECONDARY*/
  {
    TRMLOG_GSM2_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm_secondary
  },

  /* TRM_GSM3_SECONDARY*/
  {
    TRMLOG_GSM3_SECONDARY,
    TRM_REASON_MAX,
    trmcfg_pri_mode_gsm_secondary
  },

  /* TRM_LTE_HO_SECONDARY1 */
  {
    TRMLOG_LTE_HO_SECONDARY1,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_secondary
  },  

  /* TRM_LTE_HO_SECONDARY1 */
  {
    TRMLOG_LTE_HO_SECONDARY1,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte_secondary
  },

  /* TRM_LTE_EMBMS1 */
  {
    TRMLOG_LTE_EMBMS1,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte
  },

  /* TRM_LTE_EMBMS2 */
  {
    TRMLOG_LTE_EMBMS2,
    TRM_REASON_MAX,
    trmcfg_pri_mode_lte
  },
  
};




/*=============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/




/*=============================================================================

FUNCTION TRM::pri

DESCRIPTION
  Return the priority for a given client/reason pair.
  
DEPENDENCIES
  None

RETURN VALUE
  Priority

SIDE EFFECTS
  None

=============================================================================*/

trm_pri_t TRM::pri
(
  /* Client requesting the priority */
  trm_client_enum_t           client,

  /* Reason for the client's request for an RF chain */
  trm_reason_enum_t           reason
)
{

  trm_pri_t  priority = BAD;
/*---------------------------------------------------------------------------*/

  /* Out of Range check for client id */
  if ( ((uint32)client) < TRM_MAX_CLIENTS )
  {
    if ( reason < trmcfg_client[ client ].reasons )
    {
      priority = trmcfg_client[ client ].pri_mode[ reason ].pri;
    }
    else
    {
      ERR_FATAL("Client %d: Invalid reason %d", (int)client, (int)reason, 0);
    }
  }
  else
  {
    ERR_FATAL("Invalid client %d", (int)client, 0, 0);
  }

  return priority;

} /* TRM::pri( trm_client_enum_t, trm_reason_enum_t) */



/*=============================================================================

FUNCTION TRM::get_compatibility

DESCRIPTION
  Retrieve the compatibility mask for a given client/reason pair.
  
DEPENDENCIES
  None

RETURN VALUE
  A bitmask representing all the compatibilities of a given client/reason pair.

SIDE EFFECTS
  None

=============================================================================*/

trm_compatible_mask_t TRM::get_compatibility
(
  /* Client requesting the compatibility masks */
  trm_client_enum_t           client,

  /* Reason for the client's request */
  trm_reason_enum_t           reason
)
{
  /* Operating "Mode" for the client/reason */
  trm_mode_id_enum_t               mode_id;

  /* A bitmask representing the compatibility of the client/reason pair */
  trm_compatible_mask_t       compatibility = 0;

/*---------------------------------------------------------------------------*/

  /* Out of Range check for client id */
  if ( ((uint32) client) < TRM_MAX_CLIENTS )
  {
    /* Sanity check the reason */
    if ( reason < trmcfg_client[ client ].reasons )
    {
      /* Look up mode from client/reason pair */
      mode_id = trmcfg_client[ client ].pri_mode[ (int)reason ].mode;

      /* Sanity check the mode_id */
      if ( mode_id >= TRM_NUM_MODES )
      {
        ERR_FATAL("Invalid mode_id %d for client %d", (int)mode_id, 
                (int)client, 0 );
      }

      /* Look up compatible modes for the mode id */
      compatibility = trmcfg.compatible[ mode_id ];
    }
    else
    {
      ERR_FATAL("Client %d: Invalid reason %d", (int)client, (int)reason, 0); 
    }
  }
  else 
  {
    ERR_FATAL("Invalid client - %d", (int)client, 0, 0);
  }
  return compatibility;

} /* TRM::get_compatibility( client, reason ) */



/*=============================================================================

FUNCTION TRM::get_compatibility_mode

DESCRIPTION
  Retrieve the compatibility mode for a given client/reason pair.
  
DEPENDENCIES
  None

RETURN VALUE
  The compatibility mode of a given client/reason pair.

SIDE EFFECTS
  None

=============================================================================*/

trm_compatible_mask_t TRM::get_compatibility_mode
(
  /* Client requesting the compatibility masks */
  trm_client_enum_t           client,

  /* Reason for the client's request */
  trm_reason_enum_t           reason
)
{
  /* Operation "Mode" for the client/reason */
  trm_mode_id_enum_t               mode_id;

  /* A bitmask representing the client mode */
  trm_compatible_mask_t       mode = 0;

/*---------------------------------------------------------------------------*/

  /* Out of Range check for client id */
  if ( ((uint32) client) < TRM_MAX_CLIENTS )
  {
    if ( reason < trmcfg_client[ client ].reasons )
    {
      /* Look up mode from client/reason pair */
      mode_id = trmcfg_client[ client ].pri_mode[ reason ].mode;

      /* Convert mode id into a mode bitmask */
      mode = (trm_compatible_mask_t)TRM_MASK_FROM_MODE( mode_id );
    }
    else
    {
      ERR_FATAL("Client %d: Invalid reason %d", (int)client, (int)reason, 0); 
    }
  }
  else
  {
    ERR_FATAL("Invalid client - %d", (int)client, 0, 0);
  }

  return mode;

} /* TRM::get_compatibility_mode( client, reason ) */

/*=============================================================================

FUNCTION TRM::client_log_id

DESCRIPTION
  Return the log code for a client.
  
DEPENDENCIES
  None

RETURN VALUE
  Priority

SIDE EFFECTS
  None

=============================================================================*/

uint8 TRM::client_log_id
(
  /* Client log code is needed for */
  trm_client_enum_t client
)
{

/*---------------------------------------------------------------------------*/

  /* Added for KW issue Buffer Overflow warning */
  if (((uint32)client) < TRM_MAX_CLIENTS) 
  {
    return (uint8)(trmcfg_client[ client ].log_code);
  }

  /* If we get an invalid client, log it anyway
       so that we dont break parsing            */
  return (uint8)(TRMLOG_MAX+1) ;

} /* TRM::client_log_id( trm_client_enum_t ) */



/*=============================================================================

FUNCTION TRMCFG_SET_COMPATIBLE

DESCRIPTION
  Indicate modes are compatible (can run simultaneously) 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void trmcfg_set_compatible
(
  /* Primary client identifier */
  trm_mode_id_enum_t               mode_1_id,

  /* Secondary client identifier */
  trm_mode_id_enum_t               mode_2_id
)
{

/*---------------------------------------------------------------------------*/

  /* Set mode_1 as compatible with mode_2, and vis-versa */
  trmcfg.compatible[ mode_1_id ] |= (trm_compatible_mask_t)
                                       TRM_MASK_FROM_MODE( mode_2_id );
  trmcfg.compatible[ mode_2_id ] |= (trm_compatible_mask_t)
                                       TRM_MASK_FROM_MODE( mode_1_id );

} /* trmcfg_set_compatible( mode_1_id, mode_2_id ) */

/*=============================================================================

FUNCTION TRMCFG_SET_ALL_COMPATIBLE

DESCRIPTION
  Indicate modes are compatible (can run simultaneously) 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void trmcfg_set_all_compatible
(
  /* Primary client identifier */
  trm_mode_id_enum_t               mode_id
)
{
  int i = 0;

/*---------------------------------------------------------------------------*/

  /* Set all modes compatible with the input mode and vice versa */
  trmcfg.compatible[ mode_id ] = ((((uint64)1)<<TRM_NUM_MODES) - 1);
  
  for ( i = 0; i < TRM_NUM_MODES; i++ )
  {
    trmcfg.compatible[ i ] |= (trm_compatible_mask_t)
                                       TRM_MASK_FROM_MODE( mode_id );  
  }  
  
} /* trmcfg_set_compatible( mode_1_id, mode_2_id ) */

/*=============================================================================

FUNCTION TRMCFG_SET_INCOMPATIBLE

DESCRIPTION
  Indicate modes are uncompatible (can't run simultaneously) 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void trmcfg_set_incompatible
(
  /* Primary client identifier */
  trm_mode_id_enum_t               mode_1_id,

  /* Secondary client identifier */
  trm_mode_id_enum_t               mode_2_id
)
{

/*---------------------------------------------------------------------------*/

  /* Set mode_1 as compatible with mode_2, and vis-versa */
  trmcfg.compatible[ mode_1_id ] &= ~((trm_compatible_mask_t)
                                        TRM_MASK_FROM_MODE( mode_2_id ));
  trmcfg.compatible[ mode_2_id ] &= ~((trm_compatible_mask_t)
                                        TRM_MASK_FROM_MODE( mode_1_id ));

} /* trmcfg_set_incompatible( mode_1_id, mode_2_id ) */


/*============================================================================

FUNCTION TRM::IS_SHDR_ENABLED

DESCRIPTION
  Checks if SHDR is enabled or not.
    
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if SHDR is enabled. FALSE otherwise.

SIDE EFFECTS
  None

============================================================================*/

boolean TRM::is_shdr_enabled
( 
  uint32 mask
)
{
  /* Mode of s1x/hdr operation */
  trmcfg_mode_s1xhdr_enum_type  s1xhdr_mode;
  boolean val = FALSE;
  
  /* Grab the mode S1XHDR field from the mask */
  s1xhdr_mode = TRMCFG_MSK_MODE_S1XHDR( mask );

  if ( (s1xhdr_mode == TRMCFG_MODE_S1XHDR_BCMCS_ONLY) || 
       (s1xhdr_mode == TRMCFG_MODE_S1XHDR_ENABLED ) )
  {
    /* Simultaneous 1x and BCMCS mode (Traffic uses hybrid mode) OR 
	   Simultaneous 1x and HDR mode is ON */
    val = TRUE;
  }
  
  return val;

}

/*============================================================================

FUNCTION TRM::ENABLE_MODE

DESCRIPTION
  Enable the given TRM mode if possible.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void TRM::enable_mode
( 
  trm_mode_enum_t mode
)
{

/*--------------------------------------------------------------------------*/

  switch ( mode )
  {
    case TRM_MODE_SIMUL_1XIDLE_HDRTC:

      /* 1X (non-diversity) & HDR broadcast */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BROADCAST );

      /* 1X (non-diversity) & HDR connected */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BG_CONNECTED );
      
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_CONNECTED );
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BG_CONNECTED );

      break;
  }

} /* TRM::enable_mode( mode ) */



/*============================================================================

FUNCTION TRM::DISABLE_MODE

DESCRIPTION
  Disable the given TRM mode.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void TRM::disable_mode
( 
  trm_mode_enum_t mode
)
{

/*--------------------------------------------------------------------------*/

  switch ( mode )
  {
    case TRM_MODE_SIMUL_1XIDLE_HDRTC:

      /* 1X (non-diversity) & HDR broadcast */
      trmcfg_set_incompatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );
      trmcfg_set_incompatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BROADCAST );

      /* 1X (non-diversity) & HDR connected */
      trmcfg_set_incompatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );
      trmcfg_set_incompatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_CONNECTED );
            trmcfg_set_incompatible( TRM_MODE_1X,      TRM_MODE_HDR_BG_CONNECTED );
      trmcfg_set_incompatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BG_CONNECTED );

      break;
  }

} /* TRM::disable_mode( mode ) */



/*=============================================================================

FUNCTION TRM::set_compatible_clients

DESCRIPTION
  Identify clients which can run in parallel using seperate RF chains 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void TRM::set_compatible_clients
(
  /* Set Compatibility Information */
  const Set_Compatibility_Info info

)
{

  /* Mode of s1x/hdr operation */
  trmcfg_mode_s1xhdr_enum_type  s1xhdr_mode;

/*---------------------------------------------------------------------------*/

  TRM_COMPILE_ASSERT(TRM_ARR_SIZE(trmcfg_client) == TRM_MAX_CLIENTS);

  /* Store the information into global storage for debugging */
  set_compatibility_info.independent_chains  =  info.independent_chains;
  set_compatibility_info.mask                =  info.mask;
  set_compatibility_info.pam_status          =  info.pam_status;
  set_compatibility_info.dual_receive_bitmask  =  info.dual_receive_bitmask;

  
  

  /* reset the compatibility array first */
  memset( trmcfg.compatible, 0, sizeof(trmcfg.compatible) );

  /*------------------------------------------------------------------------
    Set up 1X modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X_DIV );
  trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_1X_DIV );
  trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_1X_DIV );
  trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,  TRM_MODE_1X_DIV );

  /* Set up 1X for ET*/
  trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X_ET );
  trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_1X_ET );
  trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_1X_ET );

  /*------------------------------------------------------------------------
    Set up HDR modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_HDR,                 TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_CONNECTED,       TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,       TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,  TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,       TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,         TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_IDLE,            TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,  TRM_MODE_HDR_DIV );

  /* Set up ET for HDR*/
  trmcfg_set_compatible( TRM_MODE_HDR,                 TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_CONNECTED,       TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,       TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,  TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,       TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,         TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_IDLE,            TRM_MODE_HDR_ET );
  trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,  TRM_MODE_HDR_ET );


  
  /*------------------------------------------------------------------------
    Set up LTE modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_LTE,      TRM_MODE_LTE_DIV );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_DIV );
  trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,      TRM_MODE_LTE_DIV );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_DIV );
  trmcfg_set_compatible( TRM_MODE_LTE_DIV,      TRM_MODE_LTE_DIV );

  /* Set up ET for UMTS */
  trmcfg_set_compatible( TRM_MODE_LTE,      TRM_MODE_LTE_ET );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_ET );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_DR_CAPABLE);
  
  trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,      TRM_MODE_LTE_ET );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_ET );

/* for CA */
  trmcfg_set_compatible( TRM_MODE_LTE,      TRM_MODE_LTE );
  trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,      TRM_MODE_LTE_SLTE_CAPABLE );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_SLTE_CAPABLE );
  trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,      TRM_MODE_LTE_DR_CAPABLE );
  trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,      TRM_MODE_LTE );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE );

  /* for CA */
  trmcfg_set_compatible( TRM_MODE_LTE_DIV,  TRM_MODE_LTE_DIV );

  /* for single Rx CA */
  trmcfg_set_compatible( TRM_MODE_LTE,      TRM_MODE_LTE_SINGLE_RX_CA );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_SINGLE_RX_CA );
  trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,      TRM_MODE_LTE_SINGLE_RX_CA );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,      TRM_MODE_LTE_SINGLE_RX_CA );
  trmcfg_set_compatible( TRM_MODE_LTE_DIV,      TRM_MODE_LTE_SINGLE_RX_CA );
  trmcfg_set_compatible( TRM_MODE_LTE_ET,      TRM_MODE_LTE_SINGLE_RX_CA );
  trmcfg_set_compatible( TRM_MODE_LTE_SINGLE_RX_CA,      TRM_MODE_LTE_SINGLE_RX_CA );
  

  /*------------------------------------------------------------------------
    Set up UMTS modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_UMTS,      TRM_MODE_UMTS_DIV );
  /* Set up ET for UMTS */
  trmcfg_set_compatible( TRM_MODE_UMTS,      TRM_MODE_UMTS_ET );
  /* WCDMA & UMTS Secondary */
  trmcfg_set_compatible( TRM_MODE_UMTS,      TRM_MODE_UMTS_DR_CAPABLE);
  trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,     TRM_MODE_UMTS_DIV);
  trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE, TRM_MODE_UMTS_DR_CAPABLE);
  trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,      TRM_MODE_UMTS );

  /* GSM & UMTS Secondary */
  trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_UMTS_DIV);
  trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,       TRM_MODE_UMTS_DIV);
  trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,       TRM_MODE_UMTS_DIV);

  /* for CA */
  trmcfg_set_compatible( TRM_MODE_UMTS,      TRM_MODE_UMTS );

  /* for CA */
  trmcfg_set_compatible( TRM_MODE_UMTS_DIV,  TRM_MODE_UMTS_DIV );

  /* GSM & GSM Secondary */
  trmcfg_set_compatible( TRM_MODE_GSM,       TRM_MODE_GSM_DIV);
  trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,       TRM_MODE_GSM_DIV);
  trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,       TRM_MODE_GSM_DIV);

  /*------------------------------------------------------------------------
    Set up TDSCDMA modes
  ------------------------------------------------------------------------*/
	trmcfg_set_compatible( TRM_MODE_TDSCDMA,	   TRM_MODE_TDSCDMA_DIV );
  trmcfg_set_compatible( TRM_MODE_TDSCDMA_DR_CAPABLE,	   TRM_MODE_TDSCDMA_DIV );


  /*------------------------------------------------------------------------
    Supported simultaneous modes
  ------------------------------------------------------------------------*/

  if ( info.independent_chains )
  {
    /* All 1X modes are compatible with any other 1x mode. */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X );
    trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_1X );
    trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_1X_DR_CAPABLE );
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X_IRAT_MEASUREMENT);
    trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_1X_IRAT_MEASUREMENT);
    trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_1X_IRAT_MEASUREMENT);
    trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT, TRM_MODE_1X_IRAT_MEASUREMENT);
    trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT, TRM_MODE_1X_DIV );
    
    /* make WLAN compatible to all clients */
    trmcfg_set_all_compatible( TRM_MODE_WLAN );
    
    /*---------------------------------------------------------------------
      Dual Subscription Dual Active Mode
     --------------------------------------------------------------------*/
    if ( TRM_MSK_BIT_ON( info.mask, TRMCFG_MSK_DSDA_ENABLED_BIT ) )
    {
      /* DSDA is on */
      trmcfg_set_all_compatible( TRM_MODE_GSM );
      trmcfg_set_all_compatible( TRM_MODE_GSM_SLTE_CAPABLE );
      trmcfg_set_all_compatible( TRM_MODE_GSM_DR_CAPABLE );
      
      trmcfg_set_incompatible( TRM_MODE_GSM, TRM_MODE_CM );
      trmcfg_set_incompatible( TRM_MODE_GSM_SLTE_CAPABLE, TRM_MODE_CM );
      trmcfg_set_incompatible( TRM_MODE_GSM_DR_CAPABLE, TRM_MODE_CM );

    }

    /*---------------------------------------------------------------------
      Simultaneous 1x and LTE modes
     --------------------------------------------------------------------*/
    if ( TRM_MSK_BIT_ON( info.mask, TRMCFG_MSK_SVLTE_ENABLED_BIT ) )
    {
      /* SVLTE is on */

      /* Set LTE mode and irat measurement compatible */
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT, TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT, TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT, TRM_MODE_LTE_DR_CAPABLE);

      /* LTE */
      trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_DR_CAPABLE);
      
      trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ, TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ, TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ, TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DIV, TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_1X_DIV, TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DIV, TRM_MODE_LTE_DR_CAPABLE);

      trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ, TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_1X_DIV, TRM_MODE_LTE_DIV);

      /* LTE and DO IRAT are compatible for D2L and L2D measurements */
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT, TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT, TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT, TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT, TRM_MODE_LTE_DR_CAPABLE);

      trmcfg_set_compatible( TRM_MODE_1X_DIV, TRM_MODE_LTE_DIV);
      trmcfg_set_compatible(TRM_MODE_HDR_IDLE, TRM_MODE_LTE);
      trmcfg_set_compatible(TRM_MODE_HDR_IDLE, TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible(TRM_MODE_HDR_IDLE, TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible(TRM_MODE_HDR_IDLE, TRM_MODE_LTE_DIV);
      trmcfg_set_compatible(TRM_MODE_HDR_DIV, TRM_MODE_LTE_DIV);
      trmcfg_set_compatible(TRM_MODE_HDR_DIV, TRM_MODE_LTE);
      trmcfg_set_compatible(TRM_MODE_HDR_DIV, TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible(TRM_MODE_HDR_DIV, TRM_MODE_LTE_SLTE_CAPABLE);

      /* DO IRAT should be incompatible with GSM modes for SVLTE+G,
         this will be made compatible for DSDA and if SVLTE is also
         enabled following will reset the compatiblity */
      trmcfg_set_incompatible( TRM_MODE_HDR_IRAT_MEASUREMENT, TRM_MODE_GSM );
      trmcfg_set_incompatible( TRM_MODE_HDR_IRAT_MEASUREMENT, TRM_MODE_GSM_SLTE_CAPABLE );
    }
    else if(info.dual_receive_bitmask & TRM_DR_SLTE_1X)
    {
       trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_SLTE_CAPABLE);
       trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_DR_CAPABLE);
       trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_LTE_DIV);
       trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE_SLTE_CAPABLE);
       trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
       trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_LTE_DIV);

    }
	
    /*---------------------------------------------------------------------
      Simultaneous GSM and LTE/TDS modes
     --------------------------------------------------------------------*/
    if ( TRM_MSK_BIT_ON( info.mask, TRMCFG_MSK_SGLTE_SGTDS_ENABLED_BIT ) )
    {
      /* SGLTE is on */

      /* LTE */
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_LTE_DR_CAPABLE);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_LTE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
	    trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_LTE_SLTE_CAPABLE);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_LTE_DR_CAPABLE);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
        trmcfg_set_compatible( TRM_MODE_GSM, TRM_MODE_LTE_DIV);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE, TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE, TRM_MODE_LTE_DIV);

      /* TDSCDMA */
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_TDSCDMA);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_TDSCDMA);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_TDSCDMA);

      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_TDSCDMA_DR_CAPABLE);

      trmcfg_set_compatible( TRM_MODE_GSM, TRM_MODE_TDSCDMA_DIV);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE, TRM_MODE_TDSCDMA_DIV);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE, TRM_MODE_TDSCDMA_DIV);
	  
	  /* GSM and WCDMA for roaming conditions */
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_UMTS);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_UMTS);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_UMTS);
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_UMTS_DIV);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_UMTS_DIV);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_UMTS_DIV);
      trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_UMTS_ET);
	    trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_UMTS_ET);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_UMTS_ET);
	  
      /* Only for SGLTE mode we should Single Rx CA + GSM */
      trmcfg_set_compatible(TRM_MODE_GSM, TRM_MODE_LTE_SINGLE_RX_CA);
      trmcfg_set_compatible(TRM_MODE_GSM_SLTE_CAPABLE, TRM_MODE_LTE_SINGLE_RX_CA);
      trmcfg_set_compatible(TRM_MODE_GSM_DR_CAPABLE, TRM_MODE_LTE_SINGLE_RX_CA);
    }
    else  if(info.dual_receive_bitmask & TRM_DR_SLTE_GSM)
    {
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_LTE_DIV);
    }
	
    /*---------------------------------------------------------------------
      Simultaneous 1x and HDR modes
     --------------------------------------------------------------------*/
    if ( TRM_MSK_BIT_ON( info.mask, TRMCFG_MSK_SVDO_ENABLED_BIT ) )
    {
      /* SVDO is on */

      /* 1X (non diversity, non acquisition, non irat) */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_IDLE );
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BG_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_MCDO_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_DIV);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_LOW_LAT);
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_SMALL_SCI_IDLE );
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_IDLE );
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BG_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BROADCAST );
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_MCDO_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_DIV);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_LOW_LAT);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_SMALL_SCI_IDLE );

      /* 1X acq */
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_IDLE );
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_BG_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_BROADCAST);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_MCDO_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_SMALL_SCI_IDLE);
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_DIV);
  	  trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_LOW_LAT);

      /* 1X div */
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_IDLE );
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_BG_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_BROADCAST);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_MCDO_CONNECTED);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_LOW_LAT);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_SMALL_SCI_IDLE);
      trmcfg_set_compatible( TRM_MODE_1X_DIV,  TRM_MODE_HDR_IRAT_MEASUREMENT);

      /* 1X Div, HDR Div */
      trmcfg_set_compatible(TRM_MODE_1X_DIV, TRM_MODE_HDR_DIV);

      /* GPS */     

      /* 1X DIV & GPS */
      trmcfg_set_compatible( TRM_MODE_1X_DIV,        TRM_MODE_GPS );          
      /* HDR  DIV & GPS */
      trmcfg_set_compatible( TRM_MODE_HDR_DIV,      TRM_MODE_GPS );
    }
    else if( info.dual_receive_bitmask & TRM_DR_DSDS_ENABLED )
    {
      /* We are in DR-DSDS mode. We need to enable following compatible:
         Idle, IRAT, Data Call, Sleep, HP Broadcast, Broadcast Access,
         Reselection, SCI and Channel Maintenance.*/

      /* Set up DO modes */
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                   TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                   TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                   TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                   TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                   TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_1X_IRAT_MEASUREMENT);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_1X_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      
      /* 1X ACQ and DO Page should be made compatible */
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,                 TRM_MODE_1X_ACQ);
      trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,         TRM_MODE_1X_ACQ);
      


      /* Set up 1X Modes */
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,                 TRM_MODE_LTE_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,                 TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,                 TRM_MODE_LTE_DR_CAPABLE);
	  trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,                 TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,                 TRM_MODE_UMTS_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,                 TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,                 TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,                 TRM_MODE_TDSCDMA_DR_CAPABLE);

      /* Set up LTE Modes */
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,   TRM_MODE_LTE_DR_CAPABLE); 
	  trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,                 TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_LTE_DR_CAPABLE); 
	  trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,                 TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_TDSCDMA_DR_CAPABLE, TRM_MODE_LTE_DR_CAPABLE); 
	  trmcfg_set_compatible( TRM_MODE_TDSCDMA_DR_CAPABLE,                 TRM_MODE_LTE_DIV);
      trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,   TRM_MODE_LTE_DR_CAPABLE);      
	  trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,                 TRM_MODE_LTE_DIV);

      /* Set up WCDMA Modes */
      trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,     TRM_MODE_TDSCDMA_DR_CAPABLE); 
      trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,     TRM_MODE_GSM_DR_CAPABLE); 
      trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,     TRM_MODE_GSM_SLTE_CAPABLE); 
      trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,     TRM_MODE_1X); 

      /* Set up GSM Modes */
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,   TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_TDSCDMA_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,   TRM_MODE_GSM_SLTE_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,   TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_GSM_DR_CAPABLE);
      trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_GSM_DR_CAPABLE);

      /* Set up TDS Modes */

      /* Set diversity modes compatibility */
      for ( uint8 loopcntr = 0; loopcntr < TRM_ARR_SIZE(trmcfg_div_modes); loopcntr++ )
      {
        /* set all div mode compatible with DR modes */
	for(uint8 innercnt1 =0; innercnt1 < TRM_ARR_SIZE(trmcfg_dr_modes); innercnt1++)
  	{
	  trmcfg_set_compatible(trmcfg_div_modes[loopcntr], trmcfg_dr_modes[innercnt1]);
        }
	
	/* set all div mode compatible with other div modes */
	for(uint8 innercnt2 =0; innercnt2 < TRM_ARR_SIZE(trmcfg_div_modes); innercnt2++)
	{
	  trmcfg_set_compatible(trmcfg_div_modes[loopcntr], trmcfg_div_modes[innercnt2]);
        }
      }
    trmcfg_set_compatible(TRM_MODE_HDR_DIV, TRM_MODE_1X_ACQ);		
	  
    }
    else
    {
      /* SVDO is off */

      /* Grab the mode S1XHDR field from the TRM_CONFIG mask */
      s1xhdr_mode = TRMCFG_MSK_MODE_S1XHDR( info.mask );

      if ( s1xhdr_mode == TRMCFG_MODE_S1XHDR_UNSET )
      {
        /* If the mode has not been explicitly set, set the mode to the 
           compile time default mode. */
        s1xhdr_mode = TRMCFG_MODE_S1XHDR_DEFAULT;
      }

      /* Now set the compatibilities appropriately based on the SHDR mode
         selected. */

      /* Simultaneous 1x and BCMCS mode (Traffic uses hybrid mode) */
      if ( s1xhdr_mode == TRMCFG_MODE_S1XHDR_BCMCS_ONLY )
      {
        /* 1X (non-diversity) & HDR broadcast */
        trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BROADCAST );
      }
      /* Standard simultaneous 1x and HDR mode */
      else if ( s1xhdr_mode == TRMCFG_MODE_S1XHDR_ENABLED  )
      {
        /* 1X (non-diversity) & HDR broadcast */
        trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BROADCAST );

        /* 1X (non-diversity) & HDR connected */
        trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_CONNECTED );
        trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BG_CONNECTED );
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_BG_CONNECTED );

        /* MCDO and 1X can be compatible only for DOrB SSMA PLs and 
           only when when SHDR is enabled.
           In all other targets/modes MCDO is not compatible with 1X */
        trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_MCDO_CONNECTED);
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_MCDO_CONNECTED);

        /* 1X acq & HDR idle */
        trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_IDLE );

        /* 1X acq & HDR connected */
        trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_CONNECTED);
        trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_BG_CONNECTED);

        /* 1X acq & HDR broadcast */
        trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_BROADCAST);

        /* 1X (non diversity, non acquisition) & HDR idle */
        trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_IDLE );
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_HDR_IDLE );

        /* 1X (non diversity, non acquisition) & HDR Acquisition */
        trmcfg_set_compatible( TRM_MODE_1X,  TRM_MODE_HDR);
        trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,  TRM_MODE_HDR);

      }

      /* 1X acq & HDR high priority traffic or broadcast are always compatible. */
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_LOW_LAT);

      /* 1X & Low Latency traffic or broadcast are always compatible */
      trmcfg_set_compatible( TRM_MODE_1X,     TRM_MODE_HDR_LOW_LAT);
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,     TRM_MODE_HDR_LOW_LAT);

	    /* 1X (non-diversity) & HDR idle */
      trmcfg_set_compatible( TRM_MODE_1X,  TRM_MODE_HDR_SMALL_SCI_IDLE );
      trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,  TRM_MODE_HDR_SMALL_SCI_IDLE );
	
	    /* 1X Interlock compatible with HDR */
	    trmcfg_set_compatible( TRM_MODE_HDR, TRM_MODE_1X_INTERLOCK_DIV);
      trmcfg_set_compatible( TRM_MODE_HDR_CONNECTED,       TRM_MODE_1X_INTERLOCK_DIV );
      trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,  TRM_MODE_1X_INTERLOCK_DIV );
      trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,       TRM_MODE_1X_INTERLOCK_DIV );
      trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,         TRM_MODE_1X_INTERLOCK_DIV );
      trmcfg_set_compatible( TRM_MODE_HDR_IDLE,            TRM_MODE_1X_INTERLOCK_DIV );
      trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,  TRM_MODE_1X_INTERLOCK_DIV );
	  
	  } /* if SVDO bit is on */

    /*------------------------------------------------------------------------
      Set up simultaneous GPS modes
    ------------------------------------------------------------------------*/
  
    /* 1X (except Acq) & GPS */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_1X_DR_CAPABLE,      TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_1X_IRAT_MEASUREMENT,  TRM_MODE_GPS );

    /* UMTS & GPS */
    trmcfg_set_compatible( TRM_MODE_UMTS,    TRM_MODE_GPS );

    /* LTE & GPS */
  trmcfg_set_compatible( TRM_MODE_LTE,    TRM_MODE_GPS );
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE,    TRM_MODE_GPS );
	trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE,    TRM_MODE_GPS );

    /* 1X DIV & GPS */
    trmcfg_set_compatible( TRM_MODE_1X_DIV,        TRM_MODE_GPS );          
    /* HDR  DIV & GPS */
    trmcfg_set_compatible( TRM_MODE_HDR_DIV,      TRM_MODE_GPS );

    /* GPS and LTE */
  trmcfg_set_compatible( TRM_MODE_LTE, TRM_MODE_GPS);
  trmcfg_set_compatible( TRM_MODE_LTE_DR_CAPABLE, TRM_MODE_GPS);
	trmcfg_set_compatible( TRM_MODE_LTE_SLTE_CAPABLE, TRM_MODE_GPS);

    /* GPS and LTE Div */
    trmcfg_set_compatible( TRM_MODE_LTE_DIV, TRM_MODE_GPS);

    /* GPS and UMTS */
    trmcfg_set_compatible( TRM_MODE_UMTS, TRM_MODE_GPS);
    trmcfg_set_compatible( TRM_MODE_UMTS_DR_CAPABLE,    TRM_MODE_GPS );

     /* GPS and UMTS Div */
    trmcfg_set_compatible( TRM_MODE_UMTS_DIV, TRM_MODE_GPS);

	  /* TDSCDMA & GPS */
    trmcfg_set_compatible( TRM_MODE_TDSCDMA,    TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_TDSCDMA_DR_CAPABLE,    TRM_MODE_GPS );

    /* GSM & GPS */
    trmcfg_set_compatible( TRM_MODE_GSM,     TRM_MODE_GPS );

	trmcfg_set_compatible( TRM_MODE_GSM_SLTE_CAPABLE,     TRM_MODE_GPS );
  trmcfg_set_compatible( TRM_MODE_GSM_DR_CAPABLE,     TRM_MODE_GPS );

    /* CM & GPS */
    trmcfg_set_compatible( TRM_MODE_CM,      TRM_MODE_GPS );

    /* GSM DIV & GPS */
    trmcfg_set_compatible( TRM_MODE_GSM_DIV,        TRM_MODE_GPS );

    /* TDSCDMA DIV and GPS */
    trmcfg_set_compatible( TRM_MODE_TDSCDMA_DIV,    TRM_MODE_GPS );

    if(info.pam_status)
    {
      /* UMTS DIV & GPS */
      trmcfg_set_compatible( TRM_MODE_UMTS_DIV,    TRM_MODE_GPS );
          
      /* LTE DIV & GPS */
      trmcfg_set_compatible( TRM_MODE_LTE_DIV,     TRM_MODE_GPS );
          
      /* 1X DIV & GPS */
      trmcfg_set_compatible( TRM_MODE_1X_DIV,        TRM_MODE_GPS );
          
      /* HDR  DIV & GPS */
      trmcfg_set_compatible( TRM_MODE_HDR_DIV,      TRM_MODE_GPS );
		  
      /* Make ET compatible with GPS when PAM is enabled */
      trmcfg_set_compatible( TRM_MODE_HDR_ET,	     TRM_MODE_GPS );
      trmcfg_set_compatible( TRM_MODE_1X_ET,        TRM_MODE_GPS );
      trmcfg_set_compatible( TRM_MODE_UMTS_ET,      TRM_MODE_GPS );
      trmcfg_set_compatible( TRM_MODE_LTE_ET,       TRM_MODE_GPS );

      /* TDSCDMA DIV and GPS */
      trmcfg_set_compatible( TRM_MODE_TDSCDMA_DIV,    TRM_MODE_GPS );
    }

    /* GPS is not using SRCH4 engine, so it can operate in parallel
       with 1x Acq. */
    trmcfg_set_compatible( TRM_MODE_1X_ACQ,        TRM_MODE_GPS );

    /* SGPS + HDR is enabled. */
    trmcfg_set_compatible( TRM_MODE_HDR,                 TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_CONNECTED,       TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_BG_CONNECTED,       TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_MCDO_CONNECTED,  TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_IDLE,            TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,  TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,       TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_IRAT_MEASUREMENT,TRM_MODE_GPS );

	  /* GPS higest chain feature */
	  trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,        TRM_MODE_GPS );
	
	  /* Set 1x Inter Lock diveristy compatible with GPS. */
    trmcfg_set_compatible( TRM_MODE_1X_INTERLOCK_DIV,         TRM_MODE_GPS );
	
  } /* if independent chains ... */
  
} /* TRM::set_compatible_clients( independent_chains ) */

