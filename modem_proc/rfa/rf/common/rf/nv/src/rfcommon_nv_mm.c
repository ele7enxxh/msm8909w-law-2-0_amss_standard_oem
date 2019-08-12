/*!
   @file
   rfcommon_nv_mm.c

   @brief
    leveraged from rfnv.c

   @details

*/

/*===========================================================================
Copyright (c) 2009 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/nv/src/rfcommon_nv_mm.c#1 $
$DateTime: 2016/12/13 07:59:52 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
01/04/15   vs     Enable LPM feature on JO 1.2 builds
11/17/15   jr     NV support to enable/disable LPM feature
08/18/14   whz    Fix DRx NV for TDS notch filter
05/07/14   npi    Enable notch threshold NV support
01/16/14   ndb    Added RFNV_RFC_DISABLE_FAILURE_FOR_NONFATAL_DEVICES_I
11/05/13   adk    Added support for extensions to NV item RFNV_ATUNER_ALGO_TYPE
07/15/13   ndb    Added RFNV_ATUNER_ALGO_TYPE NV support 
06/24/13   sar    Replaced memcpy calls with safe memscpy. 
05/30/13   php    Add LTE featurization
05/30/13   php    Pass only bands with Critical NVs to CM for Freq Scan
05/21/13   jr     Added notch filter support for TDSCDMA
04/13/13   jyu    Added support to read RFNV_TDSCDMA_BC_CONFIG_DIV_I
03/14/13   sar    Fixed compiler errors in APQ target. 
03/11/13   sar    Updated file for APQ, GNSS only target.
03/04/13   zhw    Clean up unused SPI NV items
02/19/13   sbm    Bandedge max power backoff support in LTE(Nikel CR375880)
01/15/13   aca    DIME DAC cal NV update
12/03/12   aca    DIME Dac cal
08/08/12   sar    Fix Compiler warning by added the missing initialization in 
                  rfcommon_nv_tbl.
05/03/12   id     Enabled NV Support for Tx Internal Cal after NV checkin
03/22/12   id     Added NV support for Tx Internal Device Cal
03/02/12   sar    Removed unused/disabled code.
02/24/12   shb    Fixed compiler warning
02/24/12   shb    Fixed compiler error
02/24/12   shb    Added new NV support for HDET autocal
02/16/12   ac     added support for notch for wcdma
02/15/12   sar    Removed FEATURE_ENABLE_TEST_NV_ITEMS & FEATURE_RF_AFC.
02/10/12   gh     Update W min Tx power
02/13/12   sar    Feature reduction changes.
02/09/12   pl     Added Support for Rx spurs NV
11/29/11   adk    Added support for SV diversity band-class config.
11/02/11   vb     Support for performing DAC cal on dynamic number of Iref values
10/20/11   adk    Added support for SV diversity.
09/26/11   aro    Removed old RC tuner NV structure
09/25/11   vb     Support for DAC cal using different Irefs
09/20/11   sar    Reverting a KW fix that was causing cal failure.
09/18/11   aro    Added generic RC Tuner calibration data structure to
                  remove RTR8600 specific data structure
09/14/11   sar     Fixed KW errors. 
09/14/11   can    Merge MDM9K tips to NikeL.
09/14/11   aro    Added new RC tuner and HDET calibration data containers
09/13/11   aro    Added support to load RC tuner NV for second RTR
09/08/11   vb     Support for DAC Cal
08/12/11   jhe    Added support for TDS BC config
05/26/11   can    HDET FAST Correction tolerance NV.  
05/24/11   cd     Added support for SV path BC configuration
01/31/11   sr     Removed therm_calibrate() which was NOT needed. 
12/23/10   cri    Added NV support for RAFT circuit for all techs
12/02/10   cri    Added support for initial and dac modes for SPI core
11/11/10   cri    Added TXPL debug message support
06/24/10   can    Support for LTE BC Config.
04/13/10   adk    Merged branch scmm_mdm9k_merge_final
12/23/09   bn     PA MPR NV feature support
12/01/09   ntn    Code Cleanup
11/30/09   bmg    Abort rfcommon_nv_mm_retrive_items as soon as an invalid
                  NV configuration is found.
10/22/09   shb    Added support for new NV - NV_RC_PROCESS_ERRORS_I
08/11/09   shb    Added NV support for RC Tuner Calibration. Temporarily,
                  using NV_VCO_COARSE_TUNE_TABLE_I
06/23/09   can    MDM9k compilation error fixes.
03/12/09   sar    Changed return type of rfcommon_nv_mm_get_tbl_ptr()
02/27/09   sr     Initial version
===========================================================================*/
#include "comdef.h"
#include "rfa_variation.h"
#include "task.h"
#include "rfcommon_nv.h"
#include "msg.h"
#include "rfcom.h"
#include "stringl.h" /* for memscpy */

#include "rfcommon_nv_mm.h"
#include "rfcommon_core_types.h"
#ifdef FEATURE_LTE
#include "rflte_mc.h"
#endif /*FEATURE_LTE*/

/*function prototype*/
rfcommon_nv_mm_tbl_type *rfcommon_nv_mm_get_tbl_ptr( void);

const rfcommon_rfnv_items_list_type rfcommon_nv_items_list[ ] =
{
  /* RF Band mask for supported bands on primary path - Must read before RF_HW_CONFIG */
  { RFNV_RF_BC_CONFIG_I,     NV_ITEM_ATTRIB_CRTCL_BMSK },

  /* RF Band mask for supported bands on diversity path */
  { RFNV_RF_BC_CONFIG_DIV_I,  NV_ITEM_ATTRIB_CRTCL_BMSK},

  /* RF Band mask for supported bands on SV path */
  { RFNV_RF_BC_CONFIG_C2_I,  NV_ITEM_ATTRIB_CRTCL_BMSK},

  /* RF Band mask for supported bands on SV diversity path */
  { RFNV_RF_BC_CONFIG_C3_I,  NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* RF HW type in the current target */
  { RFNV_RF_HW_CONFIG_I,     NV_ITEM_ATTRIB_CRTCL_BMSK },

  /* New NV for R/RC Tuner Calibrated Data Storage */
  { RFNV_RC_PROCESS_ERRORS_I,  NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* New NV for RF config */
  { RFNV_RF_CONFIG_I,  NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* ADC HDET reading offset */
  { RFNV_WCDMA_HDET_OFF_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /*LTE BC CONFIG NV ITEM*/
  { RFNV_LTE_BC_CONFIG_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

   /* LTE Bandedge maxpower relexation enable */
  { RFNV_LTE_BANDEDGE_MAXPOWER_RELAXATION_ENABLE, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

#ifdef FEATURE_TDSCDMA
  /*TDS BC CONFIG NV ITEM*/
  { RFNV_TDSCDMA_BC_CONFIG_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /*TDS BC CONFIG DIV NV ITEM*/
  { RFNV_TDSCDMA_BC_CONFIG_DIV_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* TDSCDMA SPURIOUS TABLE */
  { RFNV_TDSCDMA_C0_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_TDSCDMA_C1_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
#endif

  /* LTE TXPL DEBUG MESSAGE NV */
  { RFNV_LTE_ENABLE_TXPL_DEBUG_MSG, NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* NV holds the results from MSM Internal device cal routines */
  { RFNV_MSM_SELF_CAL_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /*HDET FAST CORRECTION TOLERANCE */
  { RFNV_HDET_FAST_CORRECTION_TOLERANCE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* LTE SPURIOUS TABLE */
  { RFNV_LTE_C0_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_LTE_C1_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

/* WCDMA SPURIOUS TABLE */
  { RFNV_WCDMA_C0_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_WCDMA_C1_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* HDET autocal NV */
  { RFNV_RFDEVICE_HDET_AUTOCAL_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* GNSS Blanking during TD-LTE */
  {RFNV_LTE_GNSS_BLANKING_RISE_FALL_DELTA, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* NV is a bit mask to determine if internal device cal needs to be */
  /* run for the corresponding RF Mode */
  { RFNV_DO_INTERNAL_DEVICE_CAL, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* NV is a bit mask to determine if internal device was successfully */
  /* run for the corresponding RF Mode */
  { RFNV_INTERNAL_DEVICE_CAL_STATUS, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  
  /*NV to select between AOL or CL type*/
  { RFNV_ATUNER_ALGO_TYPE, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  
  /*NV to disable the failures for non-fatal RF devices */ 
  { RFNV_RFC_DISABLE_FAILURE_FOR_NONFATAL_DEVICES_I,NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* LTE SPURS TABLE for C2 and C3*/
  { RFNV_LTE_C2_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_LTE_C3_SPURS_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* LTE SPURS THRESHOLD TABLE */
  { RFNV_LTE_C0_SPURS_THRESH_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_LTE_C1_SPURS_THRESH_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_LTE_C2_SPURS_THRESH_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  { RFNV_LTE_C3_SPURS_THRESH_TABLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /*RF Spare NV for enabling/disabling LPM feature*/
  { RFNV_SPARE_1_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK} ,

};

rfcommon_nv_tbl_type rfcommon_nv_tbl = 
{
  /* RF hardware configuration */
  (uint8)RF_TARGET_NONE,
  (uint8)RF_DEF_HW,
  {
    (uint64)RF_DEF_BC, /* RF NV Path 0 */
    (uint64)RF_DEF_BC, /* RF NV Path 1 */
    (uint64)RF_DEF_BC, /* RF NV Path 2 */
    (uint64)RF_DEF_BC  /* RF NV Path 3 */
  },
  0,
  (uint64)0,
/* rf_lte_bandedge_maxpower_relexation*/
   FALSE,
#ifdef FEATURE_TDSCDMA
  {
    (uint64)0,
    (uint64)0
  },
  // rf_tdscdma_c0_spurious_table
  { 
    {
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
  // rf_tdscdma_c1_spurious_table
  {
    {
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
      {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
#endif
  0,
  { 
    /* rf_msm_cal_result */
    FALSE, /* validity flag */
    {/* MSB validity, MSB array, DC validity, DC array */ 
      {  0,{0},0,{0} ,  /* Non ET*/
         0,{0},0,{0} }, /* ET */
       /* Chain 0 */
      {  0,{0},0,{0} , /* Non ET */
         0,{0},0,{0} } /* ET */
       /* Chain 1 */
    }
  },
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0},
  // rf_lte_c0_spurious_table
  { 
    {
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
  // rf_lte_c1_spurious_table
  {
    {
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
  // rf_lte_c2_spurious_table
  {
    {
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
  // rf_lte_c3_spurious_table
  {
    {
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  }, 
 
/* WCDMA SPURIOUS TABLE */
 { 
    {
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
  // rf_wcdma_c1_spurious_table
  {
    {
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
    },
  },
  /* hdet_autocal */
  {
    0, /* dev0 */
    0  /* dev1 */
  },
  /*LTE GNSS Blanking delta*/
  {
    0, /*rise delay*/
    0  /*fall delay*/
  },
  /* cal rfmode list */
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  /* cal status list */
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  /* K sensor K0 value */
  0,
  /* Atuner Algo Type */
  0,
  /*Enable  the failures for non-fatal RF devices by default*/ 
  0,
  // rf_lte_c0_spurs_thresh_table
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
  // rf_lte_c1_spurs_thresh_table
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
  // rf_lte_c2_spurs_thresh_table
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
  // rf_lte_c3_spurs_thresh_table
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
  /* lpm feature */
  1,

};

/* Initialize pointer to the default Common NV table */
rfcommon_nv_tbl_type *rfcommon_nv_tbl_ptr = &rfcommon_nv_tbl;

void rfcommon_nv_copy_item(rfnv_item_id_enum_type item_code, rfnv_item_type *rfnv_item_ptr);
void *rfcommon_nv_item_table_get(void);
uint16 rfcommon_nv_list_size_get( void );

//Externalized functions:
extern void rfcommon_core_calibrate_therm( uint8 therm_min, uint8 therm_max );


rfcommon_rfnv_info_type rfcommon_rfnv_info_table = {
  /* Common Multimode NV Items */
  rfcommon_nv_items_list,
  rfcommon_nv_item_table_get,
  rfcommon_nv_list_size_get,
  TRUE,
  rfcommon_nv_copy_item
};

/* Common RF NV item identifier array */
/* This array defines common RF NV items that are supported and will be read */
const rfcommon_nv_items_list_type rfcommon_nv_mm_items_list[ ] =
{
/* VBATT min and max ADC value */
{NV_WCDMA_VBATT_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

/* THERM min and max ADC value */
{NV_THERM_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 

{NV_TRK_LO_ADJ_DEFAULT_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

{NV_TRK_LO_ADJ_SLOPE_DEFAULT_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

{NV_RFR_IQ_LINE_RESISTOR_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

{NV_RFR_BB_FILTER_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

{NV_RTR_BB_FILTER_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
{NV_WCDMA_MIN_TX_POWER_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  #endif

#ifdef RF_HAS_RX_DIVERSITY
{NV_GPS_DRX_MODE_SEL_I     , NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
#endif

/* RF PMIC configuration */
{NV_RF_PM_CONFIG_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

{NV_ENH_THERM_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

/* MPR based PA switchpoints backoffs */
{NV_WCDMA_REL7_PA_MPR_BACKOFF_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

}; /* const rfnv_items_list_type rfcommon_nv_items_list[ ] */


/* WCDMA RF NV table with default values */
/* If these items are not read from NV, the default values are used */
rfcommon_nv_mm_tbl_type rfcommon_nv_mm_tbl = 
{
/* min and max value for VBATT ADC reading */
{0,255},


/* min and max value for THERM ADC reading */
{0,255},

2047,

346,

0,

0,

  0,

  #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
-382, /* db10 units corresponding to -57 dBm */
  #endif

/* delay delta between primary/diversity paths */
  #ifdef RF_HAS_RX_DIVERSITY
/* GPS DRX mode select switch values */
{0,0,0},
  #endif

/* RF PMIC configuration */
0,

/*NV ENH THERM*/
2840,
2320,

/* MPR based PA switchpoints backoffs */
{10,15,20,25,30,35},

}; /* rfcommon_nv_items_type rfcommon_nv_tbl */

/* Initialize pointer to the default Common NV table */
rfcommon_nv_mm_tbl_type *rfcommon_nv_mm_tbl_ptr = &rfcommon_nv_mm_tbl;

void rfcommon_nv_mm_copy_item(nv_items_enum_type item_code,
                              nv_item_type *item_ptr);
void *rfcommon_nv_mm_item_table_get(void);
uint16 rfcommon_nv_mm_list_size_get( void );

rfcommon_nv_info_type rfcommon_nv_mm_info_table = {
  /* Common Multimode NV Items */
  rfcommon_nv_mm_items_list,
  rfcommon_nv_mm_item_table_get,
  rfcommon_nv_mm_list_size_get,
  TRUE,
  rfcommon_nv_mm_copy_item
};

/* True, when RF_CONFIG is read. */
boolean rfcommon_nv_rf_config_read = FALSE;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Calibration Status.

  @details

  @retval
  TRUE if nv is read successfully, FALSE otherwise.
*/

boolean rfcommon_nv_get_cal_stat(void)
{
  return rfcommon_nv_rf_config_read;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function copies one RF NV data item, as specified by 
  "item_code/identifier" into an element of the RF NV item table. 
  This function must be called during initialization for each and every RF NV item.

  @details

  @param item_code : Item to put
  @param item_ptr  : Pointer where to get the item
*/

void rfcommon_nv_copy_item(rfnv_item_id_enum_type item_code, rfnv_item_type *rfnv_item_ptr)
{

  nv_item_type* item_ptr = (nv_item_type*) rfnv_item_ptr;

  /* read RF NV item based on its identifier/item code */
  switch ( item_code )
  {
  case RFNV_RF_CONFIG_I:
    rfcommon_nv_tbl.rf_config = item_ptr->rf_config;
    
    /* For targets that do not use RF_CONFIG NV item and for
    ** some reason have bogus values in RF_CONFIG NV entry,
    ** read RF_CONFIG here.  It will be overwritten later
    ** with value from RF ID bits.  The reason why it is
    ** read at all is that some WLL targets read RF ID bits
    ** and some do not, and if RF_CONFIG NV item is to be
    ** read, the logical place would be where all other NV
    ** items are read.
    */      
    /* rfnv_rf_config_read moved to case NV_RF_HW_CONFIG_I 
    ** since NV_RF_HW_CONFIG_I supersedes NV_RF_CONFIG_I
    */  
    break;

  case RFNV_RF_BC_CONFIG_I:
    rfcommon_nv_tbl.rf_bc_config[RF_PATH_0] = item_ptr->rf_bc_config;    
    break;
    
  case RFNV_RF_BC_CONFIG_DIV_I:    
    rfcommon_nv_tbl.rf_bc_config[RF_PATH_1] = item_ptr->rf_bc_config_div;
    break;
    
  case RFNV_RF_BC_CONFIG_C2_I:
    memscpy(&rfcommon_nv_tbl.rf_bc_config[RF_PATH_2],sizeof(uint64), 
           rfnv_item_ptr, sizeof(uint64));
    break;

  case RFNV_RF_BC_CONFIG_C3_I:
    memscpy(&rfcommon_nv_tbl.rf_bc_config[RF_PATH_3],sizeof(uint64), 
           rfnv_item_ptr, sizeof(uint64));
    break;

  case RFNV_RF_HW_CONFIG_I:
    rfcommon_nv_tbl.rf_hw_config = item_ptr->rf_hw_config;
    /* Enabling this flag would mean NV_RF_CONFIG_I was read successfully*/
    rfcommon_nv_rf_config_read = TRUE;
    break;

  case RFNV_RC_PROCESS_ERRORS_I:
    rfcommon_nv_tbl.rc_process_error[0] = item_ptr->rc_process_errors[0];
    rfcommon_nv_tbl.rc_process_error[1] = item_ptr->rc_process_errors[1];
    rfcommon_nv_tbl.rc_process_error[2] = item_ptr->rc_process_errors[2];
    rfcommon_nv_tbl.rc_process_error[3] = item_ptr->rc_process_errors[3];
    rfcommon_nv_tbl.rc_process_error[4] = item_ptr->rc_process_errors[4];
    rfcommon_nv_tbl.rc_process_error[5] = item_ptr->rc_process_errors[5];
    rfcommon_nv_tbl.rc_process_error[6] = item_ptr->rc_process_errors[6];
    rfcommon_nv_tbl.rc_process_error[7] = item_ptr->rc_process_errors[7];
    rfcommon_nv_tbl.rc_process_error[8] = item_ptr->rc_process_errors[8];
    rfcommon_nv_tbl.rc_process_error[9] = item_ptr->rc_process_errors[9];
    rfcommon_nv_tbl.rc_process_error[10] = item_ptr->rc_process_errors[10];
    rfcommon_nv_tbl.rc_process_error[11] = item_ptr->rc_process_errors[11];
    rfcommon_nv_tbl.rc_process_error[12] = item_ptr->rc_process_errors[12];
    rfcommon_nv_tbl.rc_process_error[13] = item_ptr->rc_process_errors[13];
    rfcommon_nv_tbl.rc_process_error[14] = item_ptr->rc_process_errors[14];
    rfcommon_nv_tbl.rc_process_error[15] = item_ptr->rc_process_errors[15];

    break;

  case RFNV_WCDMA_HDET_OFF_I:
    rfcommon_nv_tbl.hdet_off = item_ptr->wcdma_hdet_off;
    break;

  case RFNV_LTE_BC_CONFIG_I:
    rfcommon_nv_tbl.rf_lte_bc_config = item_ptr->lte_bc_config.lte_bc_config;
    break;

  case RFNV_LTE_BANDEDGE_MAXPOWER_RELAXATION_ENABLE:
    memscpy(&rfcommon_nv_tbl.rf_lte_bandedge_maxpower_relaxation_enable,
            sizeof(boolean),
                  rfnv_item_ptr,
                  sizeof(boolean));   
       break;

  case RFNV_LTE_C0_SPURS_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c0_spurious_table,
            sizeof(rfcommon_spur_table_type),
           rfnv_item_ptr,
           sizeof(rfcommon_spur_table_type));
    break;

  case RFNV_LTE_C1_SPURS_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c1_spurious_table,
            sizeof(rfcommon_spur_table_type),
           rfnv_item_ptr,
           sizeof(rfcommon_spur_table_type));
    break;

  case RFNV_WCDMA_C0_SPURS_TABLE_I:
   memscpy(&rfcommon_nv_tbl.rf_wcdma_c0_spurious_table,
           sizeof(rfcommon_spur_table_type),
        rfnv_item_ptr,
         sizeof(rfcommon_spur_table_type));
  break;

 case RFNV_WCDMA_C1_SPURS_TABLE_I:
   memscpy(&rfcommon_nv_tbl.rf_wcdma_c1_spurious_table,
          sizeof(rfcommon_spur_table_type),
         rfnv_item_ptr,
         sizeof(rfcommon_spur_table_type));
  break;

#ifdef FEATURE_TDSCDMA
  case RFNV_TDSCDMA_BC_CONFIG_I:
    memscpy(&rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_0],
            sizeof(rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_0]),
                  rfnv_item_ptr,
                  sizeof(rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_0]));
    break;

  case RFNV_TDSCDMA_BC_CONFIG_DIV_I:
    memscpy(&rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_1],
            sizeof(rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_1]),
                  rfnv_item_ptr,
                  sizeof(rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_1]));
    break;
	
  case RFNV_TDSCDMA_C0_SPURS_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_tdscdma_c0_spurious_table,
       sizeof(rfcommon_spur_table_type),
		  rfnv_item_ptr,
		  sizeof(rfcommon_spur_table_type));
    break;
	
  case RFNV_TDSCDMA_C1_SPURS_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_tdscdma_c1_spurious_table,
      sizeof(rfcommon_spur_table_type),
		  rfnv_item_ptr,
		  sizeof(rfcommon_spur_table_type));
    break;
#endif
  
  case RFNV_LTE_ENABLE_TXPL_DEBUG_MSG:
    memscpy(&rfcommon_nv_tbl.rf_lte_enable_txpl_debug_msg,
       sizeof(rfcommon_nv_tbl.rf_lte_enable_txpl_debug_msg),
                  rfnv_item_ptr,
                  sizeof(rfcommon_nv_tbl.rf_lte_enable_txpl_debug_msg));
    break;

  case RFNV_MSM_SELF_CAL_I:
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
    memscpy( &(rfcommon_nv_tbl.rf_msm_cal_result),
       sizeof(rfcommon_msm_cal_result_type),
            rfnv_item_ptr,
            sizeof(rfcommon_msm_cal_result_type) );

    rfcommon_nv_tbl.rf_msm_cal_result.validity_flag = TRUE;
#endif
    break;

  case RFNV_HDET_FAST_CORRECTION_TOLERANCE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_hdet_fast_correction,
       sizeof(rfcommon_nv_tbl.rf_lte_hdet_fast_correction),
                  rfnv_item_ptr,
                  sizeof(rfcommon_nv_tbl.rf_lte_hdet_fast_correction));
    break;
  case RFNV_RFDEVICE_HDET_AUTOCAL_I:
    memscpy( &(rfcommon_nv_tbl.hdet_autocal),
       sizeof(rfcommon_nv_hdet_autocal_type),
            rfnv_item_ptr,
            sizeof(rfcommon_nv_hdet_autocal_type) );
    break;

  case RFNV_LTE_GNSS_BLANKING_RISE_FALL_DELTA:
    memscpy( &(rfcommon_nv_tbl.lte_gnss_blanking_rise_fall_delta),
       sizeof(rfcommon_nv_gnss_blanking_rise_fall_delta_type),
            rfnv_item_ptr,
            sizeof(rfcommon_nv_gnss_blanking_rise_fall_delta_type) );
    break;

  case RFNV_DO_INTERNAL_DEVICE_CAL:
    memscpy(&rfcommon_nv_tbl.rf_int_dev_cal_rfmode_list,
       sizeof(rfcommon_nv_mm_int_dev_cal_rfmode_list),
                  rfnv_item_ptr,
                  sizeof(rfcommon_nv_mm_int_dev_cal_rfmode_list));
    break;

  case RFNV_INTERNAL_DEVICE_CAL_STATUS:
    memscpy(&rfcommon_nv_tbl.rf_int_dev_cal_status_rfmode_list,
       sizeof(rfcommon_nv_mm_int_dev_cal_rfmode_list),
                  rfnv_item_ptr,
                  sizeof(rfcommon_nv_mm_int_dev_cal_rfmode_list));
    break;

  case RFNV_ATUNER_ALGO_TYPE:
    memscpy(&rfcommon_nv_tbl.rf_atuner_algo_type,
            sizeof(uint8),
            rfnv_item_ptr,
            sizeof(uint8));  
    break;

  case RFNV_RFC_DISABLE_FAILURE_FOR_NONFATAL_DEVICES_I:
    memscpy(&rfcommon_nv_tbl.rfc_disable_failure_nonfatal_devices,
            sizeof(uint8),
            rfnv_item_ptr,
            sizeof(uint8));  
    break;
    
  case RFNV_LTE_C2_SPURS_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c2_spurious_table,
            sizeof(rfcommon_spur_table_type),
           rfnv_item_ptr,
           sizeof(rfcommon_spur_table_type));
    break;

  case RFNV_LTE_C3_SPURS_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c3_spurious_table,
            sizeof(rfcommon_spur_table_type),
           rfnv_item_ptr,
           sizeof(rfcommon_spur_table_type));
    break;

  case RFNV_LTE_C0_SPURS_THRESH_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c0_spurs_thresh_table,
            sizeof(rfcommon_spur_thresh_table_type),
            rfnv_item_ptr,
            sizeof(rfcommon_spur_thresh_table_type));
    break;

  case RFNV_LTE_C1_SPURS_THRESH_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c1_spurs_thresh_table,
            sizeof(rfcommon_spur_thresh_table_type),
            rfnv_item_ptr,
            sizeof(rfcommon_spur_thresh_table_type));
    break;

  case RFNV_LTE_C2_SPURS_THRESH_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c2_spurs_thresh_table,
            sizeof(rfcommon_spur_thresh_table_type),
            rfnv_item_ptr,
            sizeof(rfcommon_spur_thresh_table_type));
    break;

  case RFNV_LTE_C3_SPURS_THRESH_TABLE_I:
    memscpy(&rfcommon_nv_tbl.rf_lte_c3_spurs_thresh_table,
            sizeof(rfcommon_spur_thresh_table_type),
            rfnv_item_ptr,
            sizeof(rfcommon_spur_thresh_table_type));
    break;
  case RFNV_SPARE_1_I:
	memscpy(&rfcommon_nv_tbl.rf_nv_control_lpm_feature,
				sizeof(uint16),
				rfnv_item_ptr,
				sizeof(uint16));
	break; 
  default:
    MSG_ERROR( "Unsupported RF NV item identifier: %d", item_code, 0,0 );

  } /* switch ( item_code )  */

} /* end of rfcommon_nv_mm_copy_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns pointer to Common MM RF NV table.

  @details

*/

rfcommon_nv_tbl_type *rfcommon_nv_get_tbl_ptr( void)
{
  return( &rfcommon_nv_tbl);
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the number of items in the RF Common MM items NV list.

  @details

  @retval
  Number of item in the NV list.
*/

uint16 rfcommon_nv_list_size_get( void )
{
  return( ARR_SIZE( rfcommon_nv_items_list ) );
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the number of items in the RF NV list.

  @details

*/

void *rfcommon_nv_item_table_get(void)
{
  return((void *) &rfcommon_nv_tbl);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the RF Configuration flag value.

  @details

  @retval
  NV_RF_CONFIG id.
*/
rf_hw_type rfcommon_nv_get_rf_config(void)
{
  return (rf_hw_type) rfcommon_nv_tbl.rf_hw_config;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the auto cal value for rf asic HDET.

  @details

  @retval
  NV_WCDMA_HDET_OFF_I id.... temporary use of this enum
*/
uint8 rfcommon_nv_get_rf_asic_hdet_autocal(void)
{
  return rfcommon_nv_tbl.hdet_off;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the auto cal value for rf asic HDET to 
  the common nv table ptr

  @details
  

  @retval
  none
*/
void rfcommon_nv_set_rf_asic_hdet_autocal(uint8 hdet_autocal)
{
  rfcommon_nv_tbl.hdet_off = hdet_autocal;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the RF BC Configuration flag value for either 
  primary or diversity chain.

  @details

  @retval
  
*/
uint64 rfcommon_nv_get_rf_bc_config(rf_path_enum_type path)
{
  uint64 config = 0;

  if ( (path == RF_PATH_0) || 
       (path == RF_PATH_1) ||
       (path == RF_PATH_2) ||
       (path == RF_PATH_3) )
  {
    config = rfcommon_nv_tbl.rf_bc_config[path];
  }
  else
  {
    MSG_ERROR("Unrecognized path in rfcommon_nv_get_rf_bc_config(): %d",
              path, 0, 0);
  }

  return config;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the RF BC Configuration flag value for either 
  primary or diversity chain.

  @details

  @retval
  
*/
uint64 rfcommon_nv_get_rf_lte_bc_config(void)
{
  uint64 config = 0;

  #ifdef FEATURE_LTE
  config = rflte_mc_get_supported_band_config();
  #endif

  return config;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the TDS RF BC Configuration flag value for either 
  primary or diversity chain.

  @details

  @retval
  
*/
uint64 rfcommon_nv_get_rf_tds_bc_config(rf_path_enum_type path)
{
  uint64 config = 0;

#ifdef FEATURE_TDSCDMA
  if ( (path == RF_PATH_0) || 
       (path == RF_PATH_1) )
  {
    config = rfcommon_nv_tbl.rf_tds_bc_config[path];
  }
  else
  {
    MSG_ERROR("Unrecognized path in rfcommon_nv_get_rf_tds_bc_config(): %d",
              path, 0, 0);
  }
#endif

  return config;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the Tx Internal Cal BC NV value for all devices
  supported.

  @details

  @retval
  *rfcommon_nv_mm_int_dev_cal_rfmode_list which is the NV BC list
  
*/
rfcommon_nv_mm_int_dev_cal_rfmode_list* rfcommon_nv_get_tx_int_dev_bc_config(void)
{ 
  return (&(rfcommon_nv_tbl.rf_int_dev_cal_rfmode_list));
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the value of NV item RFNV_ATUNER_ALGO_TYPE

  @details

  @retval
  (rfdevice_atuner_tuner_device_algo_type) RFNV_ATUNER_ALGO_TYPE

*/
uint32 rfcommon_nv_get_rf_atuner_algo( void)
{
  return ((uint32) rfcommon_nv_tbl.rf_atuner_algo_type);
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns NV for disabling/enabling the failures 
  for non-fatal RF devices 
  
  @details

  @retval
  uint32

*/
uint32 rfcommon_nv_get_rfc_disable_failure_nonfatal_devices_flag( void)
{
  return ((uint32) rfcommon_nv_tbl.rfc_disable_failure_nonfatal_devices);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function copies one RF NV data item, as specified by "item_code/identifier" 
  into an element of the RF NV item table. 
  This function must be called during initialization for each and every RF NV item.

  @details

  @param item_code : Item to put
  @param item_ptr : Pointer where to get the item
  @retval
  
*/

void rfcommon_nv_mm_copy_item(nv_items_enum_type item_code,nv_item_type *item_ptr)
{
  /* Loop Counter */
  uint16 loop_ctr;        

  /* read RF NV item based on its identifier/item code */

  switch ( item_code )
  {
    case NV_WCDMA_VBATT_I:
      for (loop_ctr=0; loop_ctr < 2; loop_ctr++)
      {
        rfcommon_nv_mm_tbl.vbatt[loop_ctr] =
        item_ptr->wcdma_vbatt[loop_ctr];
      }

      break;

    case NV_THERM_I:                   
      //for (loop_ctr=0; loop_ctr < 2; loop_ctr++)
      // {
      rfcommon_nv_mm_tbl.therm[0] = item_ptr->therm.min;
      rfcommon_nv_mm_tbl.therm[1] = item_ptr->therm.max;
      // }

      rfcommon_core_calibrate_therm(rfcommon_nv_mm_tbl.therm[0], 
                                    rfcommon_nv_mm_tbl.therm[1]);

      break;


  #ifdef RF_HAS_TX_INBAND_SPUR_FIX_CHECKING
    case NV_RF_CAL_VER_I:
      for (loop_ctr=0; loop_ctr < NV_SIZE_OF_VERSION; loop_ctr++)
      {
        rfcommon_nv_mm_tbl.rf_cal_ver[loop_ctr] = item_ptr->rf_cal_ver[loop_ctr]; }

      rfnv_check_rf_cal_ver();
      break;

  #endif

    case NV_TRK_LO_ADJ_DEFAULT_I:
      rfcommon_nv_mm_tbl.trk_lo_adj_default = item_ptr->trk_lo_adj_default;
      break;

    case NV_TRK_LO_ADJ_SLOPE_DEFAULT_I:
      rfcommon_nv_mm_tbl.trk_lo_adj_slope_default = item_ptr->trk_lo_adj_slope_default; break;

    case NV_RFR_BB_FILTER_I:
      rfcommon_nv_mm_tbl.rfr_bb_filter = item_ptr->rfr_bb_filter;
      break; 

    case NV_RTR_BB_FILTER_I:
      rfcommon_nv_mm_tbl.rtr_bb_filter = item_ptr->rtr_bb_filter;
      break;

    case NV_RFR_IQ_LINE_RESISTOR_I:
      rfcommon_nv_mm_tbl.rfr_iq_line_resistor = item_ptr->rfr_iq_line_resistor;
      break;

#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
    case NV_WCDMA_MIN_TX_POWER_I:
      rfcommon_nv_mm_tbl.min_tx_power = item_ptr->wcdma_min_tx_power;
      break;
#endif

#ifdef RF_HAS_RX_DIVERSITY
    case NV_GPS_DRX_MODE_SEL_I:
      memscpy(&rfcommon_nv_mm_tbl.rf_gps_drx_mode_sel[0], 
         sizeof(uint8)*3,
             (void *)&item_ptr->gps_drx_mode_sel[0], 
             sizeof(uint8)*3);
      break;
#endif

    case NV_RF_PM_CONFIG_I:
      rfcommon_nv_mm_tbl.rf_pm_config = item_ptr->rf_pm_config;
      break;    

    case NV_ENH_THERM_I:
      rfcommon_nv_mm_tbl.rf_enh_therm_max = item_ptr->enh_therm.max;
      rfcommon_nv_mm_tbl.rf_enh_therm_min = item_ptr->enh_therm.min;  
      break;

    case NV_WCDMA_REL7_PA_MPR_BACKOFF_I:
      for (loop_ctr=0; loop_ctr < RFNV_MPR_BACKOFF_TBL_SIZE; loop_ctr++)
      {
        rfcommon_nv_mm_tbl.wcdma_rel7_pa_mpr_backoff[loop_ctr] = item_ptr->wcdma_rel7_pa_mpr_backoff[loop_ctr];
      }
      break;

    default:
      MSG_ERROR( "Unsupported RF NV item identifier: %d", item_code, 0,0 );

  } /* switch ( item_code )  */

} /* end of rfcommon_nv_mm_copy_item */

/*===========================================================================

FUNCTION rfcommon_nv_get_tbl_ptr

DESCRIPTION
  Returns pointer to Common RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to common RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfcommon_nv_mm_tbl_type *rfcommon_nv_mm_get_tbl_ptr( void)
{
  return( &rfcommon_nv_mm_tbl);
} 

/*===========================================================================

FUNCTION rfcommon_nv_LIST_SIZE                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF Common items NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfcommon_nv_mm_list_size_get( void )
{
  return( ARR_SIZE( rfcommon_nv_mm_items_list ) );
} 

/*===========================================================================

FUNCTION rfcommon_nv_ITEM_TABLE                            EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
void *rfcommon_nv_mm_item_table_get(void)
{
  return((void *) &rfcommon_nv_mm_tbl);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  

  @details

  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV 

  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

*/

rfcommon_nv_status_type 
rfcommon_nv_mm_retrive_items(rex_tcb_type *task_ptr, 
                             rex_sigs_type task_wait_sig,
                             void (*task_wait_handler)( rex_sigs_type ))
{
  rfcommon_nv_status_type status = RFCOMMON_NV_READ_SUCCESS;
  int16 nv_tbl_size = sizeof(rfcommon_rfnv_info_table) / sizeof(rfcommon_rfnv_info_type);
  int16 nv_mm_tbl_size = sizeof(rfcommon_nv_mm_info_table) / sizeof(rfcommon_nv_info_type);

  status = rfcommon_rfnv_retrieve_items(&rfcommon_rfnv_info_table, nv_tbl_size, 
                                      task_ptr, task_wait_sig, 
                                      task_wait_handler);
  if ( status != RFCOMMON_NV_READ_SUCCESS )
  {
    /* A read failure occured, and NV is not valid.  Exit immediately
       before reading anything else. */
    return status;
  }

  status = rfcommon_nv_retrieve_items(&rfcommon_nv_mm_info_table, 
                                      nv_mm_tbl_size, task_ptr, 
                                      task_wait_sig, task_wait_handler);

  return status;
}

/*!
  @brief
    Returns the value based on RFNV_SPARE_1_I , which is used to indicate if LPM feature
     is set or not

  @retval
    TRUE - If RFNV_SPARE_1_I is Undefined (or) non zero values
    FALSE - If RFNV_SPARE_1_I is zero
*/
boolean rfcommon_nv_get_lpm_feature_enabled(void )
{
  boolean feature_enable_flag = TRUE;
  
  if(rfcommon_nv_tbl.rf_nv_control_lpm_feature)
  {
    feature_enable_flag =  TRUE;
  }	 
  else
  {
    feature_enable_flag =  FALSE; 	
  }	
  return (feature_enable_flag);
}
