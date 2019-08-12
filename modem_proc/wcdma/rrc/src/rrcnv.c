/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   N V   M O D U L E

GENERAL DESCRIPTION

  This source file provides the NV interface functions for RRC.

EXTERNALIZED FUNCTIONS
  rrc_get_nv
    Get an item from NV, handle non-active items

  rrc_put_nv
    Write an item to NV

  rrc_replace_nv
    Replaces an item in NV

  rrc_free_nv
    Free an item from NV

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2009 Qualcomm Technologies, Inc.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcnv.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/16    sr      FR 38167: Changes to restrict the code to specific PLMN
06/06/16   sp      Made changes for Mav debug snapshot
01/12/16   sn      Changes for FR30881
05/11/16   sn      Changes for f3 logging reduction
10/30/15   vi     Added individual NV control on M2M features
10/16/15   as      Added code to support LTA durting activation time
10/13/15   vi     Introduced rrc_m2m_feature_enabled NV
07/20/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
09/09/15   vi      Made changes to use MCFG apis for EFS read/write operations.
12/29/14   bc      Made changes to control the range of frequencies to be skipped around a WCDMA cell by an nv.
12/2/14    vi      Added NV to half the DRX length.
10/05/14   sg      Made changes to disable EDRX and HSRACH DS concurrency capabilities overriding the NV values set.
11/03/14   ad      Made changes to control different DB DC band combinations through NV
10/08/14   sn      FR 22503 PCH to FACH optimization
07/25/14   sr      API return type change for NV refresh FR
07/21/14   ac      NV reorganization fixes in rrc_create_external_nv_item
07/18/14   sp      Made changes to set the default value of DSDS concurrency NV(72581) to enable DCHSDPA
07/17/14   sr      Changes for NV refresh FR
07/15/14   ac      NV reorganization
07/08/14   vi      Made changes to add NV control over meas id extn feature.
06/23/14   vi      Made changes to skip SIB7 reading if SIB7 expiration timer is stil running
06/24/14   sp      Made changes to enable integrigty and ciphering by default if the user has not written to NV
06/20/14   sa      Made changes to update the UL 16QAM
06/17/14   sp      Made changes to mainline DCH-FACH transition optimization changes
04/29/14   sa      Made changes for NV support for default config in CELL FACH
05/16/14   vi      Made changes to support MOB framework
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/14/14   vi      Created new NV to prioritize 850 band
05/12/14   sn      Changes to control SIB sleep before receiving SBs using NV
04/10/14   db      Made changes to enable/disable feature support in DSDS/DSDA/TSTS modes through NV
03/28/14   vi      Added new NV rrc_constant_320ms_drx_nv
03/26/14   sr      Added NV support for uplink compression capability
03/17/14   sn      Changes to control ASF timer with a NV 
03/11/14   vi      Made changes to disable IRAT and IFREQ measurements in different states based on the NV rrc_disable_meas_nv
01/27/14   sr      Added NV control for MFBI support
01/03/14   sa      Made changes to print unconditional wcdma_nv_hsupa_category per L1
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
12/13/13   sn      Manual PLMN search optimizations
12/03/13   mn      Moved all declarations to top of the code.
12/03/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
11/15/13   as      Made changes to set default EUL NV category correctly
10/15/13   sr      Made changes to fix compilation errors
09/24/13   vi      Introduced new NV for FE-FACH
08/28/13   vi      Introduced new NV for GANSS feature
08/21/13   gv      Enabling PPAC NV by default
08/13/13   vi      Fixed buffer overflow case while doing strlcpy
07/18/13   vi      Made changes to pass correct data type for rrc_fast_return_to_lte_after_csfb_timer_nv
06/19/13   vi      Made changes to pass correct data type for rrc_nv_freq_lock_item 
05/30/13   gv      Made changes to perform acq db only scan when requested so by higher layers
05/15/13   db      Made changes to enhance NV 67333 to control FDPCH and CPC DTX/DRX features independently
05/10/13   gv      Made changes to modify the default band priority
05/09/13   sn      Reduce fine/raw scan skip range for manual CSG search
05/07/03   vi      Removing feature FEATURE_WCDMA_RRC_SINGLE_CONF_FILE
05/01/13   vi      Fixed compiler warnings
04/30/13   vi      Made changes to use single wcdma_rrc_external.conf file for nv items
04/23/13   vi      Made default value of wcdma_rrc_wtol_tdd_ps_ho_support_nv to TRUE
04/19/13   vi      Made changes to remove IRAT bringup changes
02/21/13   sa      Made the changes to mainline the FEATURE_THIN_UI feature
12/07/12   sn      CSG feature changes
11/29/12   mp      Made chagnes to ignore meas reports for PSC lock under NV
11/02/12   gv      Corrected the changes under FEATURE_WCDMA_DIME_SW
11/02/12   sg      Made changes to remove the nv "rrc_nv_a2_power_opt" and remove
                   remove the code related to HW integrity protection.  
10/16/12   sr      Made changes to disable W2L measurements both in idle and connected mode through NV item
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr      Adding feature definitions for Dime
07/03/12   sks     Made changes to support NV item for wtol_TDD ps_ho
09/05/12   md      Mainlined feature flags FEATURE_RRC_INTEGRITY_SW_ENGINE,
                   FEATURE_WCDMA_A2_POWER_COLLAPSE and FEATURE_WCDMA_NIKEL_SW
05/02/12   rd      Made changes to support NV item for wtol_ps_ho
04/02/12   db      Made changes to disable CPC and FDPCH through NV
04/10/12   pm      Added code to support the band priority nv
02/24/12   sn      Changes to periodically save ACQ DB in NV
03/09/12   gv      Provided NV support to enable/disable PPAC functionality
                   and also to use existing conf file to create new NV items
01/02/12   pm      Added rrc_wtol_cm_support_nv to put FEATURE_WCDMA_CM_LTE_SEARCH feature under nv check 
11/17/11   ad      Added featurization for A2 power optimization
11/15/11   sks     Made the changes for A2 power optimization.
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
09/16/11   rl      Made changes to remove the AEEstd library functions 
                   to CoreBsp secured function cal
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/29/11   su      Added code for NV support to enable/disable
                   the FEATURE_RRC_DO_NOT_FORWARD_PAGE.
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/05/11   vg      added a new NV support for channal locking feature 
05/03/11   su      Added code changes to provide NV support to enable/disable
	           FEATURE_3GPP_CSFB_SKIP_OPT_WCDMA_SIBS.
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   su      Made changes to set SRB2 suspend_offset based on 
                   rrc_set_srb2_act_time_nv item.
03/16/11   su      Added code changes to NV item support to enable/disable support
                   for snow3g_security_algo.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/11/11   rl      Fixed the lint warning
03/09/11   su      Made changes to mainline code under FEATURE_WCDMA HSUPA 2MS TTI.
03/08/11   su      Added code to support enabling and disabling of 
                   FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS using NV item.
03/07/11   su      Added code to support device type NV.
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/30/09   ps      Removed changes to enable/disable HE field usingthe NV flag
                   NV_WCDMA_TEST_NV_1_I
03/26/09   ps      Made changes to include hw.h instead of hw_api.h, as hw_api.h
                   will be removed for some of the targets  
02/09/09   ps      Made changes to disbale enable HE field on UL based on
                   NV flag NV_WCDMA_TEST_NV_1_I
10/23/08   ps      Made changes to support FEATURE_WCDMA_TRIM_CAPABILITY
02/17/08   sm      Added function rrc_get_hsupa_category() which is called by L1
                   to get the hsupa category
02/01/08   sm      Modified default value of nv_hsupa_category when feature 
                   FEATURE_WCDMA HSUPA 2MS TTI is defined
10/08/07   da      Added support for reading HSUPA+CM NV item
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE WCDMA HSUPA, FEATURE_NV WCDMA_OPTIONAL_FEATURE_LIST_SUPPORT,
                   FEATURE_NV_WCDMA HSUPA CATEGORY
04/17/06   vk      Added support for reading NV Item HSDPA_COMPRESSED_MODE_ENABLED
                   under FEATURE HSDPA
03/28/06   vk      Added support for reading hsdpa_cat NV Item
08/31/05   vm      Added support for reading two new NV items: wcdma_dl_freq and
                   wcdma_dl_freq_enabled.
12/23/04   vn      Fixed lint error related to initialization of read_index
08/27/02   kc      Initialized Security Related NV Items to FALSE if user has
                   not written to NV RAM.
08/02/02   kc      Added support to read Integrity, Ciphering, Fake Security
                   Enabled/Disabled flags from NV
05/10/02   bu      Removed rrc_wait_nv(). Now we call the rrc_event_wait().
02/22/02   bu      Added case to assign default value to NV_ACQ_LIST_4_I in
                   rrc_get_nv().
02/20/02   bu      Added cases to give default values to 3 NV_ACQ_LIST items
                   in rrc_get_nv().
01/23/02   bu      Created a new file with NV routines.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include <stringl/stringl.h>

#include "err.h"
#include "nv.h"
#include "fs_async_put.h"
#include "rrcsigs.h"
#include "dog.h"
#include "rrccspdb.h"
#include "nv_items.h"
#include "rrcdispatcher.h"
#include "rrcnv.h"
#include "fs_fcntl.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "string.h"
#include "fs_lib.h"
#include "rrcsibproc.h"
#include "rrcueci.h"
#include "rrcllc.h"
#include "rrcllcoc.h"
#include "rrcenhstatecfg.h"
#include "mcfg_fs.h"
#include "rrcsmc.h"
#include "rrcrce.h"
#include "rrcdt.h"



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.


===========================================================================*/
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

static  nv_cmd_type nvi;           /* nv command buffer */


/* variable to indicate support of feature list supported.
 * This variable will be set using the value of NV item 'NV_WCDMA_OPTIONAL_FEATURE_LIST_I'
 * WCDMA_NV_OPTIONAL_FEATURE_HSDPA 0x1<<1 
 * WCDMA_NV_OPTIONAL_FEATURE_HSUPA 0x1<<2
 * WCDMA_NV_OPTIONAL_FEATURE_MBMS 0x1<<3
 * WCDMA_NV_OPTIONAL_FEATURE_RX_DIVERSITY 0x1<<4
 * WCDMA_NV_OPTIONAL_FEATURE_PS_DATA_HO 0x1<<5
 * all other bits are spare bits - for future use
 */
uint32 wcdma_nv_optional_feature_list;


/* variable to store the hsupa category.
 * This variable will be set using the value of NV item 'NV_WCDMA_HSUPA_CATEGORY_I'
 */
uint16 wcdma_nv_hsupa_category = RRC_WCMDA_HSUPA_DEFAULT_CATEGORY;

/* Boolean to control HSUPA + CM feature enabling\disabling via NV item*/
boolean rrc_wcdma_hsupa_cm_enabled = FALSE;
/* Boolean to enable/disable DCH_FACH optimization. */
boolean feature_dch_fach_opt_enabled = TRUE;


#define RRC_NV_CONF_FILE_PERMISSIONS_COMMON 0777

uint8 rrc_device_type_nv = 0;


uint8 rrc_ignore_cell_bar_nv = 0;

#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
extern boolean snow3g_security_algo_supported;
#endif

#ifdef FEATURE_SMC_SRB2_SUSPENSION
uint8 rrc_set_srb2_act_time_nv = 0;
#endif


uint8 rrc_csfb_skip_sib11_opt_nv = 0;

uint16 rrc_nv_channel_lock_item_value = 0; 

uint8 rrc_do_not_forward_page_nv = 0;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB

boolean rrc_fast_return_to_lte_after_csfb_nv = FALSE;

uint16 rrc_fast_return_to_lte_after_csfb_timer_nv = 2000;

#endif

boolean rrc_wtol_cm_support_nv = 1;

boolean rrc_ppac_support_nv = TRUE;

boolean rrc_save_acqdb_nv = FALSE;

uint8 rrc_disable_cpc_fdpch_nv = 0;

uint8 rrc_disable_crash_debug_info_nv = 0;

#ifdef FEATURE_WCDMA_DEBUG_ATTEMPT_FULL_SRCH
/* NV ID for PSC lock: 70241  */
boolean rrc_enable_psc_lock_nv = FALSE;
#endif

/* If NV item not set yet, we default to Bands */
rrc_nv_band_priority_config_type  rrc_nv_band_priority_config =
{
   {0x0001, 0x0400, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0004}
};

/* EFS files to manage band priority configuration NV items. */
boolean wcdma_rrc_wtol_ps_ho_support_nv = TRUE;

boolean wcdma_rrc_wtol_tdd_ps_ho_support_nv = TRUE;

#ifdef FEATURE_FEMTO_CSG
  uint32 wcdma_rrc_csg_max_prune_count_nv = 8640; /* Number of ASF searches in one month */

  /*If enabled manual CSG search considers minimum spacing between two carriers as 2.5MHz otherwise 4.4MHz*/
  /* NV# 70363 */
  boolean wcdma_csg_srch_carrier_space_nv = FALSE;


  /* 1 --> 1min,  2--> 2min ...  4 --> 4min 0 --> 5min*/
  /*NV#72574*/
  uint8 wcdma_asf_timer_nv = 0;
#endif

  uint32 wcdma_rrc_freq_scan_timer_nv = 1800000; /* This defines the max time (default - 30 mins) for which Frequency scan should not be initiated */

/* This defines the max time for which service requests should not be entertained on finding no service
 * for a previous service request in deep sleep and empty available plmns.
 */
  uint32 wcdma_rrc_deep_sleep_no_svc_timer_nv = 10000;

/* This defines the max time (default - 15 mins) for which BPLMN Frequency scan should not be initiated */
  uint32 wcdma_rrc_bplmn_freq_scan_timer_nv = 900000;

#ifdef FEATURE_GANSS_SUPPORT
  boolean rrc_ganss_support_nv = FALSE;
#endif


/*NV value  :  behavior 
              1  :  support both legacy and rel8 fast dormancy 
              2  :  support only rel8 fast dormancy
              3  :  Disable complete fast dormancy
*/
uint8 rrc_dormancy_support_nv = RRC_NV_DORMANCY_ENABLED;
  /*If enabled WRRC starts SIB sleep before receiving Scheduling blocks*/
  boolean wcdma_rrc_sib_sleep_sb_nv = TRUE;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  uint8 rrc_fe_fach_support_nv = 0;
#endif

#ifdef FEATURE_DUAL_SIM
/*NV72581*/
#ifdef FEATURE_WCDMA_DC_HSDPA
uint16 rrc_dsds_concurrency = NV_DS_DCHSDPA_CONCURRENCY;
#else
uint16 rrc_dsds_concurrency = 0;
#endif

/*NV72582*/
uint16 rrc_dsda_concurrency = 0;

/*NV72583*/
#ifdef FEATURE_TRIPLE_SIM
uint16 rrc_tsts_concurrency = 0;
#endif /*FEATURE_TRIPLE_SIM*/
#endif /*FEATURE_DUAL_SIM*/

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
  /* NV# 72548*/
  boolean rrc_mfbi_support_nv = 0;
#endif

  boolean rrc_constant_320ms_drx_nv = FALSE;

  uint16 rrc_disable_meas_nv = 0;

#ifdef FEATURE_WCDMA_UL_COMPR
  /* NV# 72576 */
  boolean rrc_ul_compr_cap_nv = 1;
#endif
  boolean rrc_prioritize_850_band_nv = FALSE;


#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
/*NV #72526*/
/*Support HS-FACH and CPC by default*/
uint8 rrc_default_cfg_in_cell_fach_nv = 0 ;
#endif
  uint32 rrc_sib7_time_nv = 0;

  extern boolean rrc_meas_id_extn_support;

  /*Bit mask to for RRC idle features*/
  uint32 wcdma_rrc_idle_features = 0;

  boolean rrc_half_drx_len_nv;

  uint16 wcdma_rrc_db_dc_band_comb_nv_control = (1 << (RRC_DB_DC_BAND1_BAND8-1));

  uint8 wcdma_freq_skip_range_nv;

  boolean rrc_m2m_feature_enabled;

  wcdma_rrc_support_lta_type wcdma_rrc_support_lta;

  uint8 rrc_m2m_bitmask_nv;

  rrc_nv_pseudo_sib19_type rrc_nv_psuedo_sib19_nv = {
                                                   WRRCNV_PSEUDO_SIB19_UTRA_PRI,
                                                   WRRCNV_PSEUDO_SIB19_S_PRI_SRCH1,
                                                   WRRCNV_PSEUDO_SIB19_THR_SERV_LOW,
                                                   WRRCNV_PSEUDO_SIB19_EUTRA_PRI,
                                                   WRRCNV_PSEUDO_SIB19_EUTRA_MEAS_BW,
                                                   WRRCNV_PSEUDO_SIB19_THR_X_HIGH,
                                                   WRRCNV_PSEUDO_SIB19_THR_X_LOW,
                                                   WRRCNV_PSEUDO_SIB19_EUTRA_QRXLEVMIN,
                                                  };

  rrc_plmn_identity_type efs_plmn_id;
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */



/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */


/*=========================================================================

FUNCTION     : rrc_get_hsupa_category

DESCRIPTION  : RRC returns the HSUPA Category to L1. RRC reads NV to determine 
               UE CAT (valid NV value, invalid NV value, NV not present, 
               NV reading failure)  

DEPENDENCIES : None

RETURN VALUE : HSUPA Category value 1..6

SIDE EFFECTS : None

=========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint16 rrc_get_hsupa_category (void)
{
  return(wcdma_nv_hsupa_category);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================
FUNCTION RRC_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the RRC task.
===========================================================================*/
nv_stat_enum_type rrc_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
  uint8 i;

  nvi.tcb_ptr = rex_self();          /* notify this task when done */
  nvi.sigs = RRC_NV_SIG;
  nvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  nvi.item = item;                 /* item to read */
  nvi.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  nvi.data_ptr = data_ptr;


  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( rex_self(), RRC_NV_SIG );
  nv_cmd( &nvi );
  rrc_event_wait(RRC_NV_SIG);

/* Handle a NV_NOTACTIVE_S or NV_FAIL_S status internally by replacing
** the random data returned with a default value of our own.  Items that
** share the same structure are lumped together in the switch.
*/

  if( nvi.status == NV_NOTACTIVE_S || nvi.status == NV_FAIL_S)
  {  
    WRRC_MSG2_HIGH("NV Item status was %d for %d ", nvi.status , nvi.item); 
    nvi.status = NV_DONE_S;

    switch( nvi.item )
    {           /* provide our own values */
      case NV_ACQ_DB_I:       /* stored ACQ database */

        /* Initialize Current Read Index to -1 */
        data_ptr->acq_db.curr_rd_index = ~0;

        /* Initialize Current Number of Pointers to 0 */
        data_ptr->acq_db.curr_wr_index = 0;

        data_ptr->acq_db.last_entry_invalid = TRUE;

        /* Initialize all values in ordered acquisition list in acq database to
        */
        for (i=0; i < MAX_ACQ_DB_ENTRIES; i++)
        {
          data_ptr->acq_db.acq_list_indices[i] = i;
        }
        break;

      default:
        nvi.status = NV_NOTACTIVE_S;
        break;
    }
  }
  else
  {
    if( nvi.status != NV_DONE_S )
    {
      WRRC_MSG2_ERROR( "NV Read Failed Item %d Code %d", nvi.item, nvi.status);
    }
  }

  return( nvi.status );
}


/*===========================================================================
FUNCTION RRC_PUT_NV

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the RRC task.
===========================================================================*/
nv_stat_enum_type rrc_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{

  nvi.tcb_ptr = rex_self();        /* Notify this task when done */
  nvi.sigs = RRC_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to write */
  nvi.cmd = NV_WRITE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */


  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( rex_self(), RRC_NV_SIG );
  nv_cmd( &nvi );
  rrc_event_wait(RRC_NV_SIG);
  
  if( nvi.status != NV_DONE_S )
  {
    WRRC_MSG2_ERROR( "NV Write Failed Item %d Code %d", nvi.item, nvi.status);
  }

  return( nvi.status );

}

/*===========================================================================
FUNCTION RRC_QUEUE_NV_WRITE

DESCRIPTION
  Posts a NV write request to async_efs_put

RETURN VALUE
  Zero if request is successful otherwise -1

DEPENDENCIES
===========================================================================*/
uint32 rrc_queue_nv_write(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr,          /* pointer to data for item */
  uint32 size
)
{

  char f_name[120];
  uint32 status = -1;

  (void)snprintf(f_name,sizeof(f_name),"/nvm/num/%d",item);
  
  status = efs_async_put(f_name, (void*)data_ptr, size,O_CREAT|O_AUTODIR, 0777);

  return status;
}

/*===========================================================================
FUNCTION RRC_REPLACE_NV

DESCRIPTION
  Replace an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the RRC task.
===========================================================================*/
nv_stat_enum_type rrc_replace_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{

  nvi.tcb_ptr = rex_self();        /* Notify this task when done */
  nvi.sigs = RRC_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to write */
  nvi.cmd = NV_REPLACE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */


  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( rex_self(), RRC_NV_SIG );
  nv_cmd( &nvi );
  rrc_event_wait(RRC_NV_SIG);
  
  if( nvi.status != NV_DONE_S )
  {
    WRRC_MSG2_ERROR( "NV Replace Failed Item %d Code %d", nvi.item, nvi.status);
  }

  return( nvi.status );
}

/*===========================================================================
FUNCTION RRC_FREE_NV

DESCRIPTION
  Free an item in NV memory.  Wait till free is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the RRC task.
===========================================================================*/
nv_stat_enum_type rrc_free_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
)
{

  nvi.tcb_ptr = rex_self();        /* Notify this task when done */
  nvi.sigs = RRC_NV_SIG;
  nvi.done_q_ptr = NULL;           /* command goes to no queue when done */

  nvi.item = item;                 /* item to free */
  nvi.cmd = NV_FREE_F;

  nvi.data_ptr =  data_ptr;        /* the data to write */

  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( rex_self(), RRC_NV_SIG );
  nv_cmd( &nvi );
  rrc_event_wait(RRC_NV_SIG);
  
  if( nvi.status != NV_DONE_S )
  {
    WRRC_MSG2_ERROR( "NV Free Failed Item %d Code %d", nvi.item, nvi.status);
  }

  return( nvi.status );

}


/*===========================================================================

FUNCTION rrc_read_nv_wcdma_optional_feature_list

DESCRIPTION
  This  function gets the NV item NV_WCDMA_OPTIONAL_FEATURE_LIST_I and stores it in 
  rrc global variable 'wcdma_nv_optional_feature_list'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_read_nv_wcdma_optional_feature_list
(
  sys_modem_as_id_e_type wrrc_as_id
)
{

#ifdef FEATURE_NV_WCDMA_OPTIONAL_FEATURE_LIST_SUPPORT
  #error code not present
#else
  wcdma_nv_optional_feature_list = WNV_GET_DEFAULT(WNV_ID_WCDMA_OPTIONAL_FEATURE_LIST_I);
  WRRC_MSG0_HIGH("FEATURE_NV_WCDMA_OPTIONAL_FEATURE_LIST_SUPPORT not defined. ");
#endif /* FEATURE_NV_WCDMA_OPTIONAL_FEATURE_LIST_SUPPORT */
}  

/*===========================================================================

FUNCTION rrc_read_nv_wcdma_hsupa_category

DESCRIPTION
  This  function gets the NV item NV_WCDMA_HSUPA_CATEGORY_I and stores it in 
  rrc global variable 'wcdma_nv_hsupa_category'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_read_nv_wcdma_hsupa_category
(
  sys_modem_as_id_e_type wrrc_as_id
)
{
  uint8 local_hsupa_category = 0;

  /*read wcdma rrc release indicator from NV*/
  wnv_get_nv_msim( WNV_ID_WCDMA_HSUPA_DEFAULT_CATEGORY_I, 
                  &local_hsupa_category,
                  sizeof(local_hsupa_category),
                  wrrc_as_id);


  /* check for valid hsupa category in NV item - 
     this validity may differ from target to target. Need to take care of this check
     later by adding appropriate check for new target or 2ms support */
  if ((local_hsupa_category < 1) || (local_hsupa_category > 6))
  {
#ifdef FEATURE_WCDMA_DC_HSUPA
    if (local_hsupa_category > RRC_WCMDA_HSUPA_DEFAULT_CATEGORY)
    {
      local_hsupa_category = MAX_DC_HSUPA_CATEGORY;
    }
    else
#endif
    {     
#ifdef FEATURE_WCDMA_16_QAM
      if (local_hsupa_category > RRC_WCMDA_HSUPA_DEFAULT_CATEGORY)
      {
        local_hsupa_category = MAX_UL_16QAM_HSUPA_CATEGORY;       
      }
      else
#endif
      {  
        WRRC_MSG1_ERROR("RRCEUL: HSUPA category NV (nv item: 4210) contains invalid category(%d). set to default",local_hsupa_category);
        local_hsupa_category = RRC_WCMDA_HSUPA_DEFAULT_CATEGORY;
      }
    }
  }
  WRRC_MSG1_HIGH("RRCEUL: NV_WCDMA_HSUPA_CATEGORY_I(nv item: 4210) is set to %d", local_hsupa_category);

  /* use stored nv variable value to set rrc global variable */
  wcdma_nv_hsupa_category = local_hsupa_category;
}  


/*===========================================================================

FUNCTION rrc_get_wcdma_hsupa_cm_ctrl_nv

DESCRIPTION
  This  function gets the NV item wcdma_hsupa_cm_ctrl and stores it in 
  rrc global variable 'rrc_wcdma_hsupa_cm_enabled'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrc_get_wcdma_hsupa_cm_ctrl_nv
(
  sys_modem_as_id_e_type wrrc_as_id
)
{
  /*read wcdma rrc release indicator from NV*/
  wnv_get_nv_msim(WNV_ID_WCDMA_HSUPA_CM_CTRL_I, &rrc_wcdma_hsupa_cm_enabled, sizeof(rrc_wcdma_hsupa_cm_enabled), wrrc_as_id);
  WRRC_MSG1_HIGH("wcdma_hsupa_cm_ctrl (nv item number 5090) is %d", rrc_wcdma_hsupa_cm_enabled);
} /* rrc_get_wcdma_hsupa_cm_ctrl_nv() */




/*===========================================================================

FUNCTION rrc_read_nv_items

DESCRIPTION
  This  function should call all the functions which read NV items to be used in RRC

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_read_nv_items(sys_modem_as_id_e_type wrrc_as_id)
{
  rrc_read_nv_wcdma_optional_feature_list(wrrc_as_id);

  rrc_read_nv_wcdma_hsupa_category(wrrc_as_id);

  rrc_get_wcdma_hsupa_cm_ctrl_nv(wrrc_as_id);

}



/*===========================================================================

FUNCTION rrc_read_from_efs

DESCRIPTION
This function takes NV conf item, the nv item variable address and 
the size of nv item variable to be read. This uses efs_get() to read
the NV item value from the efs NV item.


DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrc_read_from_efs(char *nv_conf_item, void *value,fs_size_t size)
{
  (void)efs_get(nv_conf_item,value,size);
  WRRC_MSG1_HIGH(" NV item read is %d",value);

}


/*===========================================================================

FUNCTION rrc_write_to_efs

DESCRIPTION
This function takes conf file descriptorNV conf item, address of 
default value to be written and size of the NV item as arguments 
and creates the NV item. This function assigns the default value to 
the NV item

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrc_write_to_efs(
  int32 rrc_feature_conf_fd,
  char *nv_conf_item,
  void *default_value,
  fs_size_t default_size
)
{
  char buf[100];

  memset((void *)buf,'\0', sizeof(buf));
  strlcpy(buf, nv_conf_item, sizeof(buf));
  strlcat(buf, "\n",sizeof(buf));
  /*lint +e421 */
  (void)efs_write(rrc_feature_conf_fd, buf, strlen(buf));
  efs_put(nv_conf_item,default_value,default_size,O_CREAT | O_AUTODIR,0777);

}

/*===========================================================================

FUNCTION rrc_check_and_write_to_efs

DESCRIPTION
This function takes file descriptor NV conf item, the nv item variable 
address and the size of nv item variable to be read. This uses efs_get() 
to read the NV item value from the efs NV item. If the NV conf item is not 
present this function will write the default value to the conf file

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrc_check_and_write_to_efs(
  int32 rrc_feature_conf_fd,
  char *nv_conf_item,
  void *default_value,
  fs_size_t default_size
)
{
  if(efs_get(nv_conf_item,default_value,default_size) < 0)
  {
    rrc_write_to_efs(rrc_feature_conf_fd,nv_conf_item,default_value,default_size);
  }
}

/*===========================================================================

FUNCTION rrc_create_external_nv_item

DESCRIPTION
This function creates the conf file and opens the file for 
writing NV items. If the conf file is already created this 
function does not perform any operation. All new external NV 
items to be created should call rrc_write_to_efs with appropriate
arguments accordingly

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrc_create_external_nv_item(sys_modem_as_id_e_type wrrc_as_id)
{
  /* Write conf/nv file and set default NV if it's missing */
  wnv_init_efs_conf_and_nv_files_msim(WNV_CONF_ID_RRC,wrrc_as_id);

  /* NV - 67256 */
  wnv_get_nv_msim(WNV_ID_RRC_WTOL_CM_SUPPORT, &rrc_wtol_cm_support_nv, sizeof(rrc_wtol_cm_support_nv),wrrc_as_id);
  /* NV - 67305 */
  wnv_get_nv_msim(WNV_ID_RRC_PPAC_SUPPORT, &rrc_ppac_support_nv, sizeof(rrc_ppac_support_nv),wrrc_as_id);
  /* NV - 67309 */
  wnv_get_nv_msim(WNV_ID_RRC_SAVE_ACQDB, &rrc_save_acqdb_nv, sizeof(rrc_save_acqdb_nv),wrrc_as_id);
  /* NV - 67293 */
  wnv_get_nv_msim(WNV_ID_RRC_BAND_PRIORITY_CONFIG,&rrc_nv_band_priority_config,sizeof(rrc_nv_band_priority_config_type),wrrc_as_id);  
  /* NV - 67333 */
  wnv_get_nv_msim(WNV_ID_RRC_DISABLE_CPC_FDPCH, &rrc_disable_cpc_fdpch_nv, sizeof(rrc_disable_cpc_fdpch_nv),wrrc_as_id);
  /* NV - 72515 (may change)*/
  wnv_get_nv_msim(WNV_ID_RRC_DISABLE_CRASH_DEBUG_INFO, &rrc_disable_crash_debug_info_nv, sizeof(rrc_disable_crash_debug_info_nv),wrrc_as_id);
  /* NV - 67347 */
  wnv_get_nv_msim(WNV_ID_RRC_WTOL_PS_HO_SUPPORT, &wcdma_rrc_wtol_ps_ho_support_nv, sizeof(wcdma_rrc_wtol_ps_ho_support_nv),wrrc_as_id);
  /* NV - 69732 */
  wnv_get_nv_msim(WNV_ID_RRC_WTOL_TDD_PS_HO_SUPPORT, &wcdma_rrc_wtol_tdd_ps_ho_support_nv, sizeof(wcdma_rrc_wtol_tdd_ps_ho_support_nv),wrrc_as_id);

#ifdef FEATURE_WCDMA_DEBUG_ATTEMPT_FULL_SRCH
  /* NV - 70241 */
  wnv_get_nv_msim(WNV_ID_WCDMA_RRC_ENABLE_PSC_LOCK,&rrc_enable_psc_lock_nv ,sizeof(rrc_enable_psc_lock_nv),wrrc_as_id);
#endif

#ifdef FEATURE_FEMTO_CSG
  /* NV - 70256 */
  wnv_get_nv_msim(WNV_ID_WCDMA_RRC_CSG_PRUNE_COUNTER,&wcdma_rrc_csg_max_prune_count_nv,sizeof(wcdma_rrc_csg_max_prune_count_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_WCDMA_CSG_SRCH_CARRIER_SPACE,&wcdma_csg_srch_carrier_space_nv,sizeof(wcdma_csg_srch_carrier_space_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_WCDMA_ASF_TIMER,&wcdma_asf_timer_nv,sizeof(wcdma_asf_timer_nv),wrrc_as_id);
    
  /*Protection against EFS corruption*/
  if((wcdma_asf_timer_nv == 0)||(wcdma_asf_timer_nv > 4))
  {
    wcdma_asf_timer_nv = 5;
  }
#endif

  wnv_get_nv_msim(WNV_ID_RRC_IGNORE_CELL_BAR_RESERVE_STATUS, &rrc_ignore_cell_bar_nv, sizeof(rrc_ignore_cell_bar_nv),wrrc_as_id);

  /*For snow3g_security_algo support */
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
  wnv_get_nv_msim(WNV_ID_WCDMA_RRC_SNOW3G_ENABLED,&snow3g_security_algo_supported,sizeof(snow3g_security_algo_supported),wrrc_as_id);
#endif

#ifdef FEATURE_SMC_SRB2_SUSPENSION
  wnv_get_nv_msim(WNV_ID_WCDMA_RRC_SET_SRB2_ACT_TIME,&rrc_set_srb2_act_time_nv,sizeof(rrc_set_srb2_act_time_nv),wrrc_as_id);
#endif

  wnv_get_nv_msim(WNV_ID_RRC_CSFB_SKIP_SIBL1_OPT, &rrc_csfb_skip_sib11_opt_nv, sizeof(rrc_csfb_skip_sib11_opt_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_FREQ_LOCK_ITEM, &rrc_nv_channel_lock_item_value, sizeof(rrc_nv_channel_lock_item_value),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_DO_NOT_FORWARD_PAGE, &rrc_do_not_forward_page_nv, sizeof(rrc_do_not_forward_page_nv),wrrc_as_id);

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  wnv_get_nv_msim(WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB,&rrc_fast_return_to_lte_after_csfb_nv,sizeof(rrc_fast_return_to_lte_after_csfb_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB_TIMER,&rrc_fast_return_to_lte_after_csfb_timer_nv,sizeof(rrc_fast_return_to_lte_after_csfb_timer_nv),wrrc_as_id);
#endif

  /* NV - 70350 */
  wnv_get_nv_msim(WNV_ID_RRC_FREQ_SCAN_TIMER_IN_MS, &wcdma_rrc_freq_scan_timer_nv, sizeof(wcdma_rrc_freq_scan_timer_nv),wrrc_as_id);
  /* NV - 70351 */
  wnv_get_nv_msim(WNV_ID_RRC_DEEP_SLEEP_NO_SVC_TIMER_IN_MS, &wcdma_rrc_deep_sleep_no_svc_timer_nv, sizeof(wcdma_rrc_deep_sleep_no_svc_timer_nv),wrrc_as_id);
  /* NV - 70352 */
  wnv_get_nv_msim(WNV_ID_RRC_BPLMN_FREQ_SCAN_TIMER_IN_MS, &wcdma_rrc_bplmn_freq_scan_timer_nv, sizeof(wcdma_rrc_bplmn_freq_scan_timer_nv),wrrc_as_id);

#ifdef FEATURE_GANSS_SUPPORT
  wnv_get_nv_msim(WNV_ID_RRC_GANSS_SUPPORT_NV,&rrc_ganss_support_nv,sizeof(rrc_ganss_support_nv),wrrc_as_id);
#endif

  /* NV - 72599 */
  wnv_get_nv_msim(WNV_ID_RRC_DORMANCY_ENABLE,&rrc_dormancy_support_nv,sizeof(rrc_dormancy_support_nv),wrrc_as_id);
  if((rrc_dormancy_support_nv == 0)||(rrc_dormancy_support_nv > RRC_NV_DORMANCY_DISABLED))
  {
    rrc_dormancy_support_nv = RRC_NV_DORMANCY_ENABLED;
  }

  wnv_get_nv_msim(WNV_ID_RRC_SIB_SLEEP_SB,&wcdma_rrc_sib_sleep_sb_nv,sizeof(wcdma_rrc_sib_sleep_sb_nv),wrrc_as_id);
  sib_sleep_after_sb1_sb2 = !wcdma_rrc_sib_sleep_sb_nv;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  wnv_get_nv_msim(WNV_ID_RRC_FE_FACH_SUPPORT,&rrc_fe_fach_support_nv,sizeof(rrc_fe_fach_support_nv),wrrc_as_id);
#endif

#ifdef FEATURE_DUAL_SIM
  wnv_get_nv_msim(WNV_ID_RRC_DSDS_CONCURRENCY_EXTENDED,&rrc_dsds_concurrency,sizeof(rrc_dsds_concurrency),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_DSDA_CONCURRENCY_EXTENDED,&rrc_dsda_concurrency,sizeof(rrc_dsda_concurrency),wrrc_as_id);

  /* Disabling DSDS/DSDA/TSTS concurrency for E-DRX and HS-RACH overriding the values set in NV */
  rrc_dsds_concurrency &= ~(NV_DS_EDRX_CONCURRENCY|NV_DS_HSRACH_CONCURRENCY);
  rrc_dsda_concurrency &= ~(NV_DS_EDRX_CONCURRENCY|NV_DS_HSRACH_CONCURRENCY);

  #ifdef FEATURE_TRIPLE_SIM
    wnv_get_nv_msim(WNV_ID_RRC_TSTS_CONCURRENCY_EXTENDED,&rrc_tsts_concurrency,sizeof(rrc_tsts_concurrency),wrrc_as_id);
    rrc_tsts_concurrency &= ~(NV_DS_EDRX_CONCURRENCY|NV_DS_HSRACH_CONCURRENCY);
  #endif
#endif

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
  wnv_get_nv_msim(WNV_ID_RRC_NV_ENABLE_MFBI_SUPPORT,&rrc_mfbi_support_nv,sizeof(rrc_mfbi_support_nv),wrrc_as_id);
#endif

#ifdef FEATURE_WCDMA_UL_COMPR
  wnv_get_nv_msim(WNV_ID_RRC_UL_COMPR_CAP_SUPPORT,&rrc_ul_compr_cap_nv,sizeof(rrc_ul_compr_cap_nv),wrrc_as_id);
#endif

  wnv_get_nv_msim(WNV_ID_RRC_PRIORITIZE_850_BAND,&rrc_prioritize_850_band_nv,sizeof(rrc_prioritize_850_band_nv),wrrc_as_id);

  wnv_get_nv_msim(WNV_ID_RRC_DISABLE_MEAS,&rrc_disable_meas_nv,sizeof(rrc_disable_meas_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_CONSTANT_320MS_DRX,&rrc_constant_320ms_drx_nv,sizeof(rrc_constant_320ms_drx_nv),wrrc_as_id);

#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
  wnv_get_nv_msim(WNV_ID_RRC_DEFAULT_CONFIG_IN_CELL_FACH,&rrc_default_cfg_in_cell_fach_nv,sizeof(rrc_default_cfg_in_cell_fach_nv),wrrc_as_id);
#endif
  wnv_get_nv_msim(WNV_ID_RRC_SIB7_EXP_TIME,&rrc_sib7_time_nv,sizeof(rrc_sib7_time_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_ENABLE_MEAS_ID_EXTN,&rrc_meas_id_extn_support,sizeof(rrc_meas_id_extn_support),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_IDLE_FEATURES,&wcdma_rrc_idle_features,sizeof(wcdma_rrc_idle_features),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_DB_DC_BAND_COMB_NV_CONTROL,&wcdma_rrc_db_dc_band_comb_nv_control,sizeof(wcdma_rrc_db_dc_band_comb_nv_control),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_HALF_DRX_LEN,&rrc_half_drx_len_nv,sizeof(rrc_half_drx_len_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_FREQ_SKIP_RANGE_NV,&wcdma_freq_skip_range_nv,sizeof(wcdma_freq_skip_range_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_M2M_FEATURE_ENABLE_NV,&rrc_m2m_feature_enabled,sizeof(rrc_m2m_feature_enabled),wrrc_as_id);
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
  wnv_get_nv_msim(WNV_ID_RRC_SUPPORT_LTA,&wcdma_rrc_support_lta,sizeof(wcdma_rrc_support_lta_type),wrrc_as_id);
#endif
  wnv_get_nv_msim(WNV_ID_RRC_M2M_BITMASK_NV,&rrc_m2m_bitmask_nv,sizeof(rrc_m2m_bitmask_nv),wrrc_as_id);
  wnv_get_nv_msim(WNV_ID_RRC_PSUEDO_SIB19,&rrc_nv_psuedo_sib19_nv,sizeof(rrc_nv_psuedo_sib19_nv),wrrc_as_id);
  /*Validation checks*/
  if(rrc_nv_psuedo_sib19_nv.utra_priority > 7)
  {
    rrc_nv_psuedo_sib19_nv.utra_priority = WRRCNV_PSEUDO_SIB19_UTRA_PRI;
  }

  if(rrc_nv_psuedo_sib19_nv.s_priority_search1 > 31)
  {
    rrc_nv_psuedo_sib19_nv.s_priority_search1 = WRRCNV_PSEUDO_SIB19_S_PRI_SRCH1;
  }

  if(rrc_nv_psuedo_sib19_nv.thresh_serving_low > 31)
  {
    rrc_nv_psuedo_sib19_nv.thresh_serving_low = WRRCNV_PSEUDO_SIB19_THR_SERV_LOW;
  }

  if(rrc_nv_psuedo_sib19_nv.eutra_priority > 7)
  {
    rrc_nv_psuedo_sib19_nv.eutra_priority = WRRCNV_PSEUDO_SIB19_EUTRA_PRI;
  }

  if(rrc_nv_psuedo_sib19_nv.eutra_meas_bandwidth > 5)
  {
    rrc_nv_psuedo_sib19_nv.eutra_meas_bandwidth = WRRCNV_PSEUDO_SIB19_EUTRA_MEAS_BW;
  }

  if(rrc_nv_psuedo_sib19_nv.eutra_thresh_high > 31)
  {
    rrc_nv_psuedo_sib19_nv.eutra_thresh_high = WRRCNV_PSEUDO_SIB19_THR_X_HIGH;
  }

  if(rrc_nv_psuedo_sib19_nv.eutra_thresh_low > 31)
  {
    rrc_nv_psuedo_sib19_nv.eutra_thresh_low = WRRCNV_PSEUDO_SIB19_THR_X_LOW;
  }

  if ((rrc_nv_psuedo_sib19_nv.eutra_qrxlevmin < -70) || (rrc_nv_psuedo_sib19_nv.eutra_qrxlevmin > -22))
  {
    rrc_nv_psuedo_sib19_nv.eutra_qrxlevmin = WRRCNV_PSEUDO_SIB19_EUTRA_QRXLEVMIN;
  }

  wnv_get_nv_msim(WNV_ID_RRC_BAND66_PLMN_SUPPORT,&efs_plmn_id,sizeof(efs_plmn_id),wrrc_as_id);

  if(efs_plmn_id.num_mnc_digits == 2)
  {
    efs_plmn_id.mnc[2] = WNV_ID_RRC_PLMN_ID_DEFAULT;
  }
}

/*===========================================================================

FUNCTION rrc_read_nv_item

DESCRIPTION
This function internally calls rrc_create_external_nv_item() 
in order to read or create(is not present) the NV items.This uses the globally defined NV conf file,
NV conf item, address of default value to be written and size of the NV item to call 
the above functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_read_nv_item(sys_modem_as_id_e_type wrrc_as_id)
{
  wnv_get_nv_msim(WNV_ID_RRC_DEVICE_TYPE, &rrc_device_type_nv, sizeof(rrc_device_type_nv),wrrc_as_id);

  rrc_create_external_nv_item(wrrc_as_id);
}

/*===========================================================================
FUNCTION RRC_PUT_EFS

DESCRIPTION
  Write data to given EFS file.

RETURN VALUE
  Boolean

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the RRC task.
===========================================================================*/
boolean rrc_put_efs(char *efs_file, void *data,fs_size_t size)
{
  enum mcfg_fs_status_e ret_val;
  if((ret_val = mcfg_fs_write(efs_file,data,size,MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,MCFG_FS_ALLPERMS,MCFG_FS_TYPE_EFS,MCFG_FS_SUBID_0)) != MCFG_FS_STATUS_OK)
  {
      WRRC_MSG1_HIGH("EFS_PUT not successful.. Error_code: %d",ret_val);
      return FALSE;
  }
  else
  {
     WRRC_MSG0_HIGH("EFS_PUT successful");
     return TRUE;
  }

}

/*===========================================================================
FUNCTION rrc_get_from_efs

DESCRIPTION
  Reads the data from given EFS file

RETURN VALUE
  Boolean

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the RRC task.
===========================================================================*/

boolean rrc_get_from_efs(char *efs_file, void *value,fs_size_t size)
{
  enum mcfg_fs_status_e ret_val;
  if((ret_val = mcfg_fs_read(efs_file,value,size,MCFG_FS_TYPE_EFS,MCFG_FS_SUBID_0))!= MCFG_FS_STATUS_OK)
  {
     WRRC_MSG1_HIGH("EFS_GET not successful.. Error_code: %d",ret_val);
     return FALSE;
  }
  else
  {
     return TRUE;
  }
}

/*===========================================================================

FUNCTION rrc_read_all_nv_items

DESCRIPTION
This function internally calls other NV read functions

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_read_all_nv_items(sys_modem_as_id_e_type wrrc_as_id)
{
  /* Read NV items Fake Security, Integrity Enabled and
     Ciphering Enabled from NV */
  rrcsmc_get_nv_items(wrrc_as_id);
  rrcdt_get_nv_items(wrrc_as_id);
  rrcrce_get_nv_items(wrrc_as_id);
	
  /* read wcdma rrc version from NV */
  rrc_get_wcdma_rrc_version_nv(wrrc_as_id);
	
  rrcueci_get_nv_items(wrrc_as_id);
  rrcllc_get_cm_hsdpa_nv_item(wrrc_as_id);
	
  rrcueci_get_wtog_nacc_nv_item(wrrc_as_id);
	
#ifdef FEATURE_UMTS_PDCP
  rrc_get_wcdma_rrc_pdcp_disabled_nv(wrrc_as_id);
#endif /* FEATURE_UMTS_PDCP */
	
  rrc_read_nv_items(wrrc_as_id);
	
#ifdef FEATURE_WCDMA_HS_FACH
  rrc_create_opt_feature_nv_item(wrrc_as_id);
  rrc_read_opt_feature_nv_item(wrrc_as_id);
#endif

  rrc_read_nv_item(wrrc_as_id);
}


#ifdef FEATURE_MODEM_CONFIG_REFRESH

/*===========================================================================

FUNCTION rrc_read_nv_item_refresh

DESCRIPTION
This function reads the NV items when SIM hotswap happens

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_read_nv_item_refresh(void)
{
  /* NV - 67256 */
  wnv_get_nv(WNV_ID_RRC_WTOL_CM_SUPPORT, &rrc_wtol_cm_support_nv, sizeof(rrc_wtol_cm_support_nv));
  /* NV - 67305 */
  wnv_get_nv(WNV_ID_RRC_PPAC_SUPPORT, &rrc_ppac_support_nv, sizeof(rrc_ppac_support_nv));
  /* NV - 67309 */
  wnv_get_nv(WNV_ID_RRC_SAVE_ACQDB, &rrc_save_acqdb_nv, sizeof(rrc_save_acqdb_nv));
  /* NV - 67293 */
  wnv_get_nv(WNV_ID_RRC_BAND_PRIORITY_CONFIG,&rrc_nv_band_priority_config,sizeof(rrc_nv_band_priority_config_type));  
  /* NV - 67333 */
  wnv_get_nv(WNV_ID_RRC_DISABLE_CPC_FDPCH, &rrc_disable_cpc_fdpch_nv, sizeof(rrc_disable_cpc_fdpch_nv));
  /* NV - 67347 */
  wnv_get_nv(WNV_ID_RRC_WTOL_PS_HO_SUPPORT, &wcdma_rrc_wtol_ps_ho_support_nv, sizeof(wcdma_rrc_wtol_ps_ho_support_nv));
  /* NV - 69732 */
  wnv_get_nv(WNV_ID_RRC_WTOL_TDD_PS_HO_SUPPORT, &wcdma_rrc_wtol_tdd_ps_ho_support_nv, sizeof(wcdma_rrc_wtol_tdd_ps_ho_support_nv));

#ifdef FEATURE_FEMTO_CSG
  /* NV - 70256 */
  wnv_get_nv(WNV_ID_WCDMA_RRC_CSG_PRUNE_COUNTER,&wcdma_rrc_csg_max_prune_count_nv,sizeof(wcdma_rrc_csg_max_prune_count_nv));
  wnv_get_nv(WNV_ID_WCDMA_CSG_SRCH_CARRIER_SPACE,&wcdma_csg_srch_carrier_space_nv,sizeof(wcdma_csg_srch_carrier_space_nv));
#endif

#ifdef FEATURE_SMC_SRB2_SUSPENSION
  wnv_get_nv(WNV_ID_WCDMA_RRC_SET_SRB2_ACT_TIME,&rrc_set_srb2_act_time_nv,sizeof(rrc_set_srb2_act_time_nv));
#endif

  wnv_get_nv(WNV_ID_RRC_CSFB_SKIP_SIBL1_OPT, &rrc_csfb_skip_sib11_opt_nv, sizeof(rrc_csfb_skip_sib11_opt_nv));

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  wnv_get_nv(WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB,&rrc_fast_return_to_lte_after_csfb_nv,sizeof(rrc_fast_return_to_lte_after_csfb_nv));
  wnv_get_nv(WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB_TIMER,&rrc_fast_return_to_lte_after_csfb_timer_nv,sizeof(rrc_fast_return_to_lte_after_csfb_timer_nv));
  WRRC_MSG2_HIGH("CSFB Fast Feature enabled %d, CSFB Fast timer %d", rrc_fast_return_to_lte_after_csfb_nv, rrc_fast_return_to_lte_after_csfb_timer_nv);
#endif

  /* NV - 70350 */
  wnv_get_nv(WNV_ID_RRC_FREQ_SCAN_TIMER_IN_MS, &wcdma_rrc_freq_scan_timer_nv, sizeof(wcdma_rrc_freq_scan_timer_nv));
  /* NV - 70351 */
  wnv_get_nv(WNV_ID_RRC_DEEP_SLEEP_NO_SVC_TIMER_IN_MS, &wcdma_rrc_deep_sleep_no_svc_timer_nv, sizeof(wcdma_rrc_deep_sleep_no_svc_timer_nv));
  /* NV - 70352 */
  wnv_get_nv(WNV_ID_RRC_BPLMN_FREQ_SCAN_TIMER_IN_MS, &wcdma_rrc_bplmn_freq_scan_timer_nv, sizeof(wcdma_rrc_bplmn_freq_scan_timer_nv));

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  wnv_get_nv(WNV_ID_RRC_FE_FACH_SUPPORT,&rrc_fe_fach_support_nv,sizeof(rrc_fe_fach_support_nv));
#endif

#ifdef FEATURE_DUAL_SIM
  wnv_get_nv(WNV_ID_RRC_DSDS_CONCURRENCY_EXTENDED,&rrc_dsds_concurrency,sizeof(rrc_dsds_concurrency));
  wnv_get_nv(WNV_ID_RRC_DSDA_CONCURRENCY_EXTENDED,&rrc_dsda_concurrency,sizeof(rrc_dsda_concurrency));
  #ifdef FEATURE_TRIPLE_SIM
    wnv_get_nv(WNV_ID_RRC_TSTS_CONCURRENCY_EXTENDED,&rrc_tsts_concurrency,sizeof(rrc_tsts_concurrency));
  #endif
#endif

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
  wnv_get_nv(WNV_ID_RRC_NV_ENABLE_MFBI_SUPPORT,&rrc_mfbi_support_nv,sizeof(rrc_mfbi_support_nv));
  WRRC_MSG1_HIGH("MFBI NV enabled %d", rrc_mfbi_support_nv);
#endif

#ifdef FEATURE_WCDMA_UL_COMPR
  wnv_get_nv(WNV_ID_RRC_UL_COMPR_CAP_SUPPORT,&rrc_ul_compr_cap_nv,sizeof(rrc_ul_compr_cap_nv));
  WRRC_MSG1_HIGH("UL compr cap NV %d", rrc_ul_compr_cap_nv);
#endif
}

/*===========================================================================
FUNCTION rrc_read_nv_refresh

DESCRIPTION
  Reads the NV/EFS values on receiving NV refresh indication

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void rrc_read_nv_refresh(void)
{
  sys_modem_as_id_e_type wrrc_as_id = SYS_MODEM_AS_ID_1;
  /* function gets the NV item NV_WCDMA_RRC_VERSION_I  */
  rrc_get_wcdma_rrc_version_nv(wrrc_as_id);

  /* function gets NV item NV_HSDPA_CAT_I  */
  rrcueci_get_nv_items(wrrc_as_id);

  /* function gets NV item NV_HSDPA_COMPRESSED_MODE_ENABLED_I */
  rrcllc_get_cm_hsdpa_nv_item(wrrc_as_id);

  /* function gets the NV item wcdma_hsupa_cm_ctrl */
  rrc_get_wcdma_hsupa_cm_ctrl_nv(wrrc_as_id);

  /* function gets the NV item NV_WCDMA_HSUPA_CATEGORY_I */
  rrc_read_nv_wcdma_hsupa_category(wrrc_as_id);

  /* function creates the EFS item RRC_FEATURE_NV_CONF_FILE */
  rrc_create_opt_feature_nv_item(wrrc_as_id);

  /* function reads the EFS item  */
  rrc_read_opt_feature_nv_item(wrrc_as_id);

  /* Creates and writes EFS item */
  rrc_read_nv_item_refresh();
}

/*===========================================================================
FUNCTION rrc_post_nv_refresh_ind

DESCRIPTION
  Callback fucntion invoked by MCFG to post NV refresh indication

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
boolean rrc_post_nv_refresh_ind_cb(mcfg_refresh_info_s_type *p_info)
{
  WRRC_MSG0_HIGH("MCFG callback received for NV refresh");

  /* Get the command buffer */
  rrc_cmd_type *cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_NV_REFRESH_IND;

  /* Copy the structures to local command */
  WCDMA_MEMCPY(&cmd_ptr->cmd.rrc_nv_refresh, 
               sizeof(rrc_mcfg_refresh_info_s_type),
               p_info,
               sizeof(rrc_mcfg_refresh_info_s_type));

  rrc_put_int_cmd( cmd_ptr );

  return TRUE;
}

/*===========================================================================
FUNCTION rrc_nv_refresh_mcfg_regiser_cb

DESCRIPTION
  Function used to register for callback for receiving NV refresh indication

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void rrc_nv_refresh_mcfg_regiser(void)
{
  mcfg_refresh_registration_s_type mcfg_refresh;

  /* Default ORDER for refresh registration 
     * certain task needs to refresh the NV items before the other tasks, similar to the concept of RC init task group. 
     * All the tasks  except policyman now tentatively register with default order 50 for integration. 
     *  The real order # might get bumped up or down depending on the integration result.
     */        
  mcfg_refresh.order = MCFG_REFRESH_ORDER_50;

  /* Client interested in getting refresh callback for all slots, MCFG_REFRESH_INDEX_MASK_ALL(Binary 0x7) should be used */  
  mcfg_refresh.slot_mask = MCFG_REFRESH_INDEX_MASK_ALL;

  /* Client interested in getting refresh callback for all subs, MCFG_REFRESH_INDEX_MASK_ALL(Binary 0x7) should be used */    
  mcfg_refresh.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;  

  /* "GROUP" field is reserved for future enhancement, not for this FR. 
     * The NV/EFS items might get divided into couple different groups. 
     * The client will only receive refresh call back for certain groups that it cares about
     */        
  mcfg_refresh.group = 0;

  /* Callback fucntion to receive the NV refresh notification */  
  mcfg_refresh.cb = rrc_post_nv_refresh_ind_cb;

  /* Register with MCFG api */
  if(!mcfg_refresh_register(&mcfg_refresh))
  {
    WRRC_MSG0_ERROR("NV refresh registration failed");
  }
}

#endif

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wrrc_qsh_dump_nv_vals

DESCRIPTION
  This function dumps nv values

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
  __attribute__((section(".uncompressible.text")))

size_t wrrc_qsh_dump_nv_vals(uint8* dump_addr, size_t size )
{
  wrrc_qsh_dump_tag_nv_values nv_values;
  qsh_client_dump_tag_hdr_init((qsh_dump_tag_hdr_s *)dump_addr,WRRC_QSH_DUMP_TAG_NV_VALUES,
                                   sizeof(wrrc_qsh_dump_tag_nv_values));
  WCDMA_MEMCPY(&nv_values,sizeof(wrrc_qsh_dump_tag_nv_values),
  (wrrc_qsh_dump_tag_nv_values*)dump_addr, sizeof(wrrc_qsh_dump_tag_nv_values));
  nv_values.wcdma_nv_hsupa_category = rrc_get_hsupa_category();
  nv_values.nv_hsdpa_category_debug = nv_hsdpa_category;
  nv_values.rrc_feature_nv_item_value = rrc_feature_nv_item_value;
  nv_values.rrc_dsds_concurrency = rrc_dsds_concurrency;
  nv_values.rrc_dsda_concurrency = rrc_dsda_concurrency;
  nv_values.rrc_nv_rel_indicator_debug = rrc_nv_rel_indicator;
  //nv_values.rrc_default_cfg_in_cell_fach_nv_debug = rrc_default_cfg_in_cell_fach_nv;
  nv_values.wcdma_look_for_concurrency_nv_debug = wcdma_look_for_concurrency_nv & 0x01;
  nv_values.feature_dch_fach_opt_enabled_debug = feature_dch_fach_opt_enabled & 0x01;

  if(sizeof(nv_values) >= size)
  {
    WCDMA_MEMCPY((wrrc_qsh_dump_tag_nv_values*)dump_addr, size,&nv_values, size);
    return ((size_t)size);
  }
  else
  {
    WCDMA_MEMCPY((wrrc_qsh_dump_tag_nv_values*)dump_addr,sizeof(nv_values), &nv_values, sizeof(nv_values));
    return ((size_t)(sizeof(nv_values)));
  }
}
#endif
