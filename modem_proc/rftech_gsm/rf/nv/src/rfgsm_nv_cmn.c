/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     G S M  R F   N V   M a n a g e r

DESCRIPTION
  This module retrieves and stores all of the GSM RF NV items.

EXTERNALIZED FUNCTIONS


INTIALIZATION AND SEQUENCING REQUIREMENTS

Typical operating sequence:
rfgsm_nv_load_items()    <-- reads all GSM NVs for a given rfm device

Copyright (c) 1997 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/nv/src/rfgsm_nv_cmn.c#1 $
$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/16   kpa     Add debug NV for excessive F3 logging reduction
02/18/15   ec      Fix for newly exposed KW error
01/28/15   ec      Changes to implement new Reserved NV item
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/16/14   ec      Add index and bitmask definitions for debug NV as well as new API definition
				   for accessing debug NV
11/19/14  huibingl take the channel number discontinuity into account when do GSM RX cal data interpolation
10/17/14   rr      Separate rx/tx alternate path core handles
10/08/14   cpm     implement C2 GSM CL antenna tuner NV items on rf path 2
09/19/14   ck      Fix Compiler warnings, implicit comparison
09/16/14   ck      Fix the case which does not apply Predistortion when pa_swpt == pd_swpt
09/15/14   rr      Fix for nv container loading
09/02/14   ec      Add check to avoid using HL cal table if no calibrated channels are present
09/01/14   tws     Fix compiler warning, invalid comparison (enum vs int)
08/22/14   rr      Changes to support nv c3
08/01/14   ec      Add code to handle HL NV items for SAWless targets
08/01/14   tws     Zero initialise the enhanced temp comp data if load fails
08/01/14   sml     Remove benign NULL ptr messages while accessing spare NV
08/01/14   sg      Changes to enable gsm core handle creation even when BC config is not supported
07/30/14   hoh     Add enhanced rx feature support 
06/25/14   zbz     Fix bug in previous check in
06/20/14   zbz     Add feature to adjust temp comp table based on therm read during cal
06/23/14   sb      Change max number of GSM NV data structures + debug hooks
04/30/14   hoh     Fix SMPS PDM default table and NV item ID name
04/23/14   ggs     Port CR638143: Fix incorrect bounds check in 
                   rfgsm_nv_load_band_specific_nv
04/02/14   zbz     LLVM compiler changes regarding structure alignment
03/19/14   hoh     Added support for 8 PA gain states
03/10/14   zbz     Add feature to control pa icq bias through NV item
03/05/14   zbz     Fix KW errors
02/19/14   sc      Remove GFW dep on table sizes
01/14/14   av      Delay Calibration and npler based delay support
11/11/13   tsr     LLVM compiler changes regarding structure allignment
10/17/13   ggs     Add support to override amam and ampm tables with default
                   values and disable pre-distortion and disable Vbatt/temp
                   comp for BOLT using debug flags
10/14/13   sml     Memsetting the memory to zero before use
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
09/25/13   sb      Added NULL ptr checks to GSM NV code
09/25/13   sb      Add NV hooks to GRFC PA range map
09/25/13   sb      Added changes to support GSM TX calibration/online mode operation in coexistence
09/23/13   ggs     Added default gsm tx cal data table
09/12/13   ry      NULL the freed memory to prevent crash due to double mem_free
08/07/13   ec      Fix KW error due to potentially out of range nv_path
08/06/13   sml     Fixing complier errors in off-target
08/05/13   sml     Fixing complier errors in off-target
08/02/13   sml     GSM RxD NV support
07/31/13   sb      Make changes for BC config to be NV container based
07/24/13   tws     Fix issue with wrong band mask being used for RFM_DEVICE_2
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.
07/04/13   tws     Correct issues with device to path mapping. Add function
                   to return lowest support band.
06/10/13   cpm     preferred use of memscpy()
06/05/13   svi     Adding Support of Enhanced Temperature Compensation in GSM.
04/30/13   aka     Moved rfgsm_nv_data_init to rfgsm_core files
04/20/13   par     Fix compilation error
04/20/13   par     Fix compilation error
04/11/13   par     Fixed RFM_DEVICE hardcoding in some APIs
03/27/13   sb      Added support for Enhanced APT
03/21/13   ec      Replace deprecated RFM_MAX_DEVICES with RFM_MAX_WAN_DEVICES
03/20/13   svi     corrected typo on rfgsm_nv_load_multislot_tx_pwr function.
03/20/13   svi     Replaced memcpy() by safer memscpy().
03/18/13   aka     GSM NV deinit fix
03/14/13   aka     support for new GSM err vs PCL field
03/05/13   aka     add scaling for default value of linear tx gain NV
03/05/12   sb      Support for Spare NV 
03/05/13   aka     Added default values for critical GSM static NVs
02/12/13   lcl     Added code to read PA range map NV
02/12/13   sr      changes to free the memory alloocated during rfm_init() 
02/01/13   aka     Added path 1 NV in rfgsm_nv_cmn_items_map
12/17/12   aka     Removing legacy GSM NV support
01/03/12   aka     added seperate counts for Rx and Tx NV buffers
12/10/12   tws     Use core_handle path_delay during calibration.
12/06/12   tws     Fix Klocwork errors.
12/05/12   aka     Added rfm device in rfgsm_nv_calc_path_delay API
12/03/12   aka     Changes to make Rx and Tx switch point Path dependent
11/29/12   tws     Add device id to FTM.
11/28/12   aka     Changes to extend path based computations
11/16/12   ggs     Edge Envelope gain in new NV format needs to be doubled for Dime
11/15/12   aka     Seperated data structures for AMAM and AMPM NV data
11/08/12   sg      Fixed compiler warnings
11/07/12   sg      Added new API's to remove NV dependency on the device driver code
09/18/12   sb      Fixed incorrect mapping of NV band to NV table
09/05/12   sr      fixed compiler warnings.
07/26/12   sr      added code to initialize the NV related items in the gsm handle.
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
02/01/12    kg     Remove conditional check on status for retriving band specific NV's 
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
09/15/11   sr      made trl lo adj NV as non-critical.
08/07/11   av      Port Enhanced temp comp changes
09/02/11   sr      ported SAR feature from MDM9k.
08/25/11   sb      Added GST related NV  
08/02/11   sr      Made changes to remove unused nv items,
                   increased the amam and ampm tables size,
                   added band specific tx_gain nv.
05/17/11   rsr/sr  Fix for env gain length
05/13/11   sar     Fixed compiler warnings.
05/06/11   bmg     Fixed pa_transition_tbl initializer with featurization
05/03/11    sr     featurized the pa_transition_tbl nv item for NikeL.
03/24/11   vrb     Compiler Warning Fixes
03/21/11    sr     removed un-used ocde.
03/11/11   sr      Removed un-used featurization
03/10/11   sr     fixed warnings.
03/03/11   sr     off-target error fix.
03/03/11    sr     merge from mdm9k branch
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
07/27/10   ggs     Fixed compiler warnings
03/19/10   lcl     Pull in changes from QSC6295.  More changes from QSC6295
01/20/09   tws     Move sigma_delta_max_db value to RFC card file.
01/12/10   vrb     Compiler Warnings Fixes
12/22/09   sr      changed the debug msg macro from legacy to new one.
06/26/09   sr      Updated function argument
07/24/08   can     Changes to gsm_linear_tx_gain_val.
04/09/08   can     Added support for tx gain values (envelope) for GSM & EDGE for linear PA.
03/07/08   ycl     Added support for GSM Tx Linear Edge PA switch points.
01/28/08   can     Added rfnv_gsm_precompute_tx_gain_ranges_info.
01/28/08   can     Added rfnv_gsm_get_tx_gain_info_ptr.
12/06/07   lcl     Added API to return spare1 nv (107)
09/13/07    rv     Removed code wrapped in #ifdef __GNUC__
02/05/07   ycl     Merge to 7200 tip.
01/25/07   lcl     Merge RSSI temp comp NV support.
10/17/06   ycl     Modifications for initial MSM7600 build.
07/11/06    bn     Fix lint errors
06/15/06   adm     Added the correct defaults for path delay items.
06/07/06   adm     Added internal therm nv item changes.
06/01/06   adm     Added polar path delay compensation changes.
03/01/06   adm     Compiler Warning Fixes
12/21/05   adm     Lint Fixes
08/10/05   adm     Removed common polar ramp profile item.
06/14/05   adm     Lint Cleanup
06/10/05   rsr     Added support for temp comp and vbatt backoff
05/27/05   rsr     Added NV for pa_transition table
05/25/05   adm     Added support for W1800 for Triple Taco
05/17/05   rsr     Added support for different precharge NV for each band.
04/21/05   rsr     Added NV item support for polar ramp, antenna timings, 
                   3 frequencies autocalibration.
03/04/05   rsr     Added RF_ prefix to some macros
03/04/05   lcl     Rename FEATURE_POLAR_RF to RF_HAS_POLAR.
01/17/05   jtn     Added FEATURE_RF_GSM_SCALE_COMP (TX ramp distortion adjustment).
08/19/04   et      added #ifdef T_RF_GWZRF6250 condition around
                   default values for antenna select truth tables.
07/26/04   jtn     Changes related to moving antenna selects to NV.
07/14/04   jtn     Sony #1 antenna switch support.
05/19/04   jtn     Support for new NV item GSM_TX_BURST_OFFSET_ADJ.
05/10/04   et      changed gsm_precharge val to 80 from 0
04/11/04   jtn     Lint.
04/07/04   rv      Lint cleanup.
08/21/03   ap      Final modifications made to incorporate into new NV mould using Quadband support
05/27/03   jtn     Use NV_TEST_CODE_VER_I instead of NV_RF_CAL_DATE_I to check for old rx gain settings
04/28/03   jtn     Changes for gain switchpoints in NV
04/17/03   jtn     Changed function rfnv_cal_older_than() to use rfnv_gsm_cmn_nv_tbl RAM structure instead
                   of reading from NVRAM every time it is called.  Added rf_cal_date to the structure.
04/02/03   jtn     Added function rfnv_cal_older_than() to compare versus a specific rf_cal_date.
03/21/03   jtn     set register 0x32 gain range 3 to value 0x47 only in DCS RX, update switchpoint values
02/10/03   pgh     updated default CAL data to CHEETAH_T46_FYW_B12_18_MAX_TX_POWER
02/04/03   drl     Merge leopard changes onto tip
12/11/02   aks     Changed rf_config default to 0x02, for External VCO.
12/05/02   JC      Added funtions that provide the frequency compensated
                   Rx switch points.
11/15/02   thh     Allow online mode only if RF_CONFIG is B1-external-VCO or
                   above.
11/06/02   thh     Set RF CONFIG default value to the latest configuration
                   always.
10/10/02   ih      Eliminated compiler warnings in the dual-band default ramp
                   table.
09/20/02   ih      Eliminated compiler warnings.
09/11/02   JC      Also, added default DCS PA start/stop times to pass
                   power vs. time masks.
09/11/02   JC      Fixed DCS TX dBm comments in default table 0..30 dBm;
                   Also, fixed default ramp tables for DCS.
08/30/02   JC      Rx freq. compensation bug fix for EGSM channels. Also,
                   obsoleted super-het radio support.
08/28/02   thh     Added DCS support.
08/12/02   thh     Added support for RF_CONFIG.
                   Vslick-beautify codes.
06/20/02   JC      Merged in changes for new RFCAL procedure, effective
                   from ZIF architecture moving forwards.
06/20/02   thh     Added GZIFTRIC support.  Brian support is removed.
                   Checking this in for archival purposes only.  Do not use
                   this revision in formal releases.
04/10/02   ih      Changed default values of gsm_precharge and
                   gsm_precharge_dur to 0.
04/03/02   thh     Added support for new Txprofile API.
03/25/02   thh     Zero-out the Tx freq comp table.  This will effectively
                   disable Tx frequency compensation when valid calibration
                   data is not in NV.
03/20/02   ih      Added PA start/stop time for GSM and DCS to NV table.
03/19/02   thh     Previously, only half of the PA profile was copied from NV.
                   Now, the whole PA profile from NV is used.
                   Updated copyright date.
03/13/02   thh     Added Tx frequency compensation support.
02/12/02   JC      Added DCS Rx and AFC items. Moved rfnv_gsm_items_type
                   from rfgsm_nv.h to this module to reduce coupling
                   to nv_items.h for GPRS development.
01/21/02   JC      Initial version for GSM. Introduced
                   rfnv_register_wait_handler() for abstraction purposes.
                   Also built the Rx compensation calibration functions.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include <memory.h>   /* Memory manipulation         */
#include "msg.h"      /* MSG_XXX() macros            */
#include "sys.h"      /* sys_band_mask_type */
#include "rfc_card_gsm.h"
#include "rfcommon_nv.h"
#include "rfcommon_nv_mm.h"
#include "rfgsm_nv_cmn.h" /* rfnv_task_wait_info_struct  */
#include "rfgsm_core.h"
#include "rfgsm_core_util.h"
#include "rfgsm_core_handle.h"
#include "rfcommon_core.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"  /*to use modem_mem_alloc api*/
#include "stringl.h"
#include "rfm_internal.h" // For Bring up flags

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/
/*!
   @brief
   The static maximum number of simultaneous bands that may be used by the
   GSM configuration.
	 
   @details 
   No current target requires more than 4 bands at once.
*/

	
#define RF_GSM_NV_BAND_NUM RFGSM_NV_MAX_SUPPORTED

/*!
  @brief
  The static maximum number of data structures that can be allocated.
 
  @details
  Currently support 2 chains (RF_GSM_NV_PATH_NUM) and 4 bands
  (RF_GSM_NV_BAND_NUM)
*/

#define RF_GSM_NV_MAX_STRUCTURES (RF_GSM_NV_PATH_NUM * RF_GSM_NV_BAND_NUM)

#define RFGSM_NV_NUM_PA_RANGES 8
 
 /* Tables to store fequency compensated Rx swt pts*/
 rfgsm_nv_rx_switch_type gsm_850_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_rx_switch_type gsm_900_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_rx_switch_type gsm_1800_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_rx_switch_type gsm_1900_rx_switch_dBm[RF_GSM_NV_PATH_NUM];

 /* Tables to store Rx enh swt pts*/
 rfgsm_nv_enh_rx_switch_type gsm_850_enh_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_enh_rx_switch_type gsm_900_enh_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_enh_rx_switch_type gsm_1800_enh_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_enh_rx_switch_type gsm_1900_enh_rx_switch_dBm[RF_GSM_NV_PATH_NUM];

 /* Tables to store frequency compensated Rx swt pts for High Linearity mode on SAWless targets */
 rfgsm_nv_rx_switch_type gsm_850_hl_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_rx_switch_type gsm_900_hl_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_rx_switch_type gsm_1800_hl_rx_switch_dBm[RF_GSM_NV_PATH_NUM];
 rfgsm_nv_rx_switch_type gsm_1900_hl_rx_switch_dBm[RF_GSM_NV_PATH_NUM];

 /* Tables to store Tx gain range information including swt pts*/
 rfnv_tx_gain_range_info_type gsm_850_gsm_tx_gain_range_info[RF_GSM_NV_PATH_NUM];
 rfnv_tx_gain_range_info_type gsm_850_edge_tx_gain_range_info[RF_GSM_NV_PATH_NUM];

 rfnv_tx_gain_range_info_type gsm_900_gsm_tx_gain_range_info[RF_GSM_NV_PATH_NUM];
 rfnv_tx_gain_range_info_type gsm_900_edge_tx_gain_range_info[RF_GSM_NV_PATH_NUM];

 rfnv_tx_gain_range_info_type gsm_1800_gsm_tx_gain_range_info[RF_GSM_NV_PATH_NUM];
 rfnv_tx_gain_range_info_type gsm_1800_edge_tx_gain_range_info[RF_GSM_NV_PATH_NUM];

 rfnv_tx_gain_range_info_type gsm_1900_gsm_tx_gain_range_info[RF_GSM_NV_PATH_NUM];
 rfnv_tx_gain_range_info_type gsm_1900_edge_tx_gain_range_info[RF_GSM_NV_PATH_NUM];

/*-------------------------- NV bootup Debug hooks----------------------------------------*/

typedef struct
{
 /* indicates whether memory for Rx/TX related Static NVs data allocated*/
 boolean static_nv_alloc_valid[RFC_CONFIG_RXTX_MAX];

 /*indicates whether memory for all the NV data :Rx + Tx static NV data is allocated or not*/
 boolean nv_data_alloc_valid;


 /*indicates whether TX is supported or not*/
 uint8 tx_supported;
}
nv_alloc_type;

/*This structure contains all NV memory allocation related information for logical device, NV container and NV band*/
static nv_alloc_type nv_alloc_info[RFM_MAX_DEVICES][RF_GSM_NV_PATH_MAX][RFGSM_NV_MAX_SUPPORTED];

/* This structure contains all the NV container related information for given logical device, band and RX/TX*/
static uint8 nv_container_array[RFM_MAX_DEVICES][RFCOM_NUM_GSM_BANDS][RFC_CONFIG_RXTX_MAX];

/*! 
  @brief The following structures define the access to debug NV used in the
  function rfgsm_nv_check_debug_nv()
 
  @details
  This comment block MUST be updated to reflect additions and amendments!
 
 
  GSM Debug NV consists of multiple 32 bit words. The 0th word is used to
  enable logging, the 1st word is used to override code execution.
 
 
  @struct rfgsm_debug_nv_mask_info_type
  This contains an NV index and a bitmask
 
  @var rfgsm_debug_nv_masks
  This is an array of rfgsm_debug_nv_mask_info_type, used to hold the
  information needed to access the GSM debug NV based on each entry
  in rfgsm_debug_nv_supported_type.
 
*/

typedef struct
{
  uint8 debug_nv_index;
  uint8 debug_nv_bitmask;
} rfgsm_debug_nv_mask_info_type;

static const rfgsm_debug_nv_mask_info_type rfgsm_debug_nv_masks[RFGSM_DEBUG_NV_MAX_SUPPORTED] = 
{
  /* RFNV_GSM_DEBUG_I NV Index 0 - LOGGING */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT0_MASK },  /* RFGSM_DEBUG_NV_FREQ_ERR_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT1_MASK },  /* RFGSM_DEBUG_NV_TEMP_COMP_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT2_MASK },  /* RFGSM_DEBUG_NV_VBATT_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT3_MASK },  /* RFGSM_DEBUG_NV_SAWLESS_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT4_MASK },  /* RFGSM_DEBUG_NV_TX_PCL_CALC_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT5_MASK },  /* RFGSM_DEBUG_NV_KV_CAL_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT6_MASK },  /* RFGSM_DEBUG_NV_RX_DEBUG_LOGGING_ENABLE */
  { RFGSM_SPARE_NV_LOGGING_ENABLE_IDX, NV_BIT7_MASK },  /* RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE */
  /* Debug NV Index 1 - Overrides */
  /* RFNV_GSM_DEBUG_I NV Index 1 - OVERRIDES */
  { RFGSM_SPARE_NV_OVERRIDE_IDX, NV_BIT0_MASK },        /* RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE */
  { RFGSM_SPARE_NV_OVERRIDE_IDX, NV_BIT1_MASK },        /* RFGSM_DEBUG_NV_SAWLESS_MODE_FORCE_HIGH */
  { RFGSM_SPARE_NV_OVERRIDE_IDX, NV_BIT2_MASK },        /* RFGSM_DEBUG_NV_SAWLESS_DISABLE_JDET */
  { RFGSM_SPARE_NV_OVERRIDE_IDX, NV_BIT3_MASK },        /* RFGSM_DEBUG_NV_SAWLESS_ALGORITH_IGNORE_JDET */
  { RFGSM_SPARE_NV_OVERRIDE_IDX, NV_BIT4_MASK },        /* RFGSM_DEBUG_NV_SAWLESS_DISABLE_HL_POWER_SCAN */
  { RFGSM_SPARE_NV_OVERRIDE_IDX, NV_BIT5_MASK },        /* RFGSM_DEBUG_NV_DISABLE_THERM_READ  */

  { 0x0, 0x0 },                                         /* RFGSM_DEBUG_NV_ITEM_BOUNDARY - NOT VALID FOR USE*/

  /* RFNV_GSM_RESERVED_I NV index 0 - DEBUG */
  { RFGSM_RESERVED_DEBUG_NV_IDX, NV_BIT0_MASK },        /* RFGSM_DEBUG_NV_ERR_FATAL_FIRST_SW_OVERRUN */
  { RFGSM_RESERVED_DEBUG_NV_IDX, NV_BIT1_MASK },        /* RFGSM_DEBUG_NV_ERR_FATAL_EXCEED_SW_OVERRUN_THRESHOLD */
  { RFGSM_RESERVED_DEBUG_NV_IDX, NV_BIT2_MASK },        /* RFGSM_DEBUG_NV_ERR_FATAL_GFW_CALL_RFLM_LATE */
  { RFGSM_RESERVED_DEBUG_NV_IDX, NV_BIT3_MASK },        /* RFGSM_DEBUG_NV_ERR_FATAL_CCS_CB_LATE */
  { RFGSM_RESERVED_DEBUG_NV_IDX, NV_BIT4_MASK },        /* RFGSM_DEBUG_NV_ERR_FATAL_RXBLANK_DUE_TO_LATE_ENTER_WAKEUP */
  /* RFNV_GSM_RESERVED_I NV index 1 - TIME PROFILING */
  { RFGSM_RESERVED_TIME_PROF_NV_IDX, NV_BIT0_MASK },    /* RFGSM_DEBUG_NV_TIME_PROFILE_CORE */
};


/*------------------------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold main pointer for Band dspecific NVs for a band and path

  @details
  This structures gives details of whether the NV read is valid or not through data_valid and last NV read. 
  rfgsm_nv_band_data_buffer_ptr is one pointer to the data where we hold the data of path, band, 
  and static/calibrated NVs for that band and path.
*/
	
typedef struct 
{
  /*!
   @brief
   Indiciates if this band NV was loaded successfully, and the data is valid.
   */
  boolean data_valid;
   		
  /*!
   @brief
   The last item that the NV load process was reading.
   		
   @details
   If data_valid is not TRUE, then this variable will indicate what NV item
   caused the NV loading to fail.
   */
  rfnv_item_id_enum_type last_nv_item_read;
		
 /*!
  @brief
  Pointer to the GSM configuration structure for this index entry.
  */
  rfgsm_nv_band_data_type *rfgsm_nv_band_data_buffer_ptr; 
}rfgsm_nv_band_node_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold main pointer for common NVs (common acorss the bands)

  @details
  This structures gives details of whether the NV read is valid or not through data_valid and last NV read. 
  rfgsm_nv_cmn_data_buffer_ptr is one pointer to the data where we hold the data of path, band, 
  and static/calibrated NVs for that band and path.
*/

typedef struct 
{
  /*!
  @brief
  Indiciates if this band NV was loaded successfully, and the data is valid.
  */
  boolean data_valid;
		
  /*!
  @brief
  The last item that the NV load process was reading.
		
  @details
  If data_valid is not TRUE, then this variable will indicate what NV item
  caused the NV loading to fail.
  */
  rfnv_item_id_enum_type last_nv_item_read;
		
  /*!
  @brief
  Pointer to the GSM configuration structure for this index entry.
  */
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_data_buffer_ptr; 
		  
}rfgsm_nv_cmn_node_type;
	

/*!
  @brief
  Static storage allocation for the GSM configuration data 
*/

	
static struct
{
  /*!
    @brief
      The current number of structures that have been allocated.
    @details
      This item starts at 0 during initialization and is incremented
      every time a buffer is requested.  If allocated becomes equal
      to RF_GSM_NV_MAX_STRUCTURES, NV memory is exhausted.
   */
  int rx_buffer_count;

  int tx_buffer_count;
	
  /*!
    @brief
      An array of configuration structures that may be returned
      by the allocator.

    @details
      The buffer indexed by the allocated structure member will
      be the next buffer returned by the allocator, as long as
      allocated is less than RF_GSM_NV_MAX_STRUCTURES.
  */
  gsm_rx_static_nv_type* rx_static_nv_data_buffers[RF_GSM_NV_MAX_STRUCTURES];
  gsm_tx_static_nv_type* tx_static_nv_data_buffers[RF_GSM_NV_MAX_STRUCTURES];

} rfgsm_nv_common_config_storage;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  GSM config index entry type per device.

  @details
  This structure is the element type in the first level index mapping
  (path, band) to GSM data structures.  Each device has an array
  of band index types for all possible bands in the GSM driver.
*/


typedef struct 
{
  /* common NVs for a given path */
  rfgsm_nv_cmn_node_type  rfgsm_nv_cmn_node;
  /* common NVs for a given band and path */
  rfgsm_nv_band_node_type rfgsm_nv_band_node[RFGSM_NV_MAX_SUPPORTED];
}rfgsm_nv_data_type;


/*----------------------------------------------------------------------------*/

/*
  @brief
  This table contains the maping of the split bands to the respective main bands.
  In case a band is split, it lists the split band enums and also the relative band enum 
  index for the split bands.
  When generated, the table will look like this for example in SGLTE 1800 coexistence

  {
    {RFGSM_NV_850, {RFGSM_NV_850, RFGSM_NV_MAX_SUPPORTED}, 0},
    {RFGSM_NV_900, {RFGSM_NV_900, RFGSM_NV_MAX_SUPPORTED}, 0},
    {RFGSM_NV_1800, {RFGSM_NV_1800, RFGSM_NV_1800_B}, 0},
    {RFGSM_NV_1900, {RFGSM_NV_1900, RFGSM_NV_MAX_SUPPORTED}, 0},
    {RFGSM_NV_1800, {RFGSM_NV_MAX_SUPPORTED, RFGSM_NV_MAX_SUPPORTED}, 1},

  };



   
*/



static rfgsm_coex_band_map_type *rfgsm_coex_nv_band_map_tbl[RFGSM_NV_MAX_SUPPORTED] = {NULL};




/*----------------------------------------------------------------------------*/
/*!
  @brief
  The root level of the index mapping (path, band) to the GSM
  data structures.

  @details
  This variable contains the band mappings for all supported NV
  paths in the system.  The NV path in this structure corresponds
  to the path or chain definitions in the NV enumerations, which
  are numbered from 0..N-1, where N is the maximum number of GSM
  paths that can be present in the system.

  The data structure is initialized to 0 on bootup, representing
  an empty index with NULL pointers for all devices and bands.
  As paths and bands are initialized, the structure is populated
  with non-NULL pointers in the cells that have been initailized.

  This NV path does not necessary match the RFM logical device that
  RF clients access the RF driver with.  For instance, a logical
  to NV mapping might consist of:
    RFM_DEVICE_0 -> RF_GSM_NV_PATH_0
    RFM_DEVICE_1 -> RF_GSM_NV_PATH_1

  Later when we need to access any NV. Then we can access to following ptr:
  rfgsm_nv_data[chain].band_index[band].rf_gsm_nv_data_buffer_ptr->rx_static_nv_data_ptr
  rfgsm_nv_data[chain].band_index[band].rf_gsm_nv_data_buffer_ptr->tx_static_nv_data_ptr
   
*/

static rfgsm_nv_data_type rfgsm_nv_data[RF_GSM_NV_PATH_NUM];

/*!
  @brief
  Static storage allocation for the GSM common NV containers 
*/

static rfgsm_nv_path_type rfgsm_cmn_nv_container[RFM_MAX_WAN_DEVICES];

/*!
  @brief
  Static storage allocation for the GSM configuration data that can be for static or calibrated NVs 
*/
static struct
{
  /*!
    @brief
    The current number of structures that have been allocated.

    @details
    This item starts at 0 during initialization and is incremented
    every time a buffer is requested.  If allocated becomes equal
    to RF_GSM_NV_MAX_STRUCTURES, NV memory is exhausted.
  */
  int allocated;

  /*!
    @brief
    An array of configuration structures that may be returned
    by the allocator.

    @details
    The buffer indexed by the allocated structure member will
    be the next buffer returned by the allocator, as long as
    allocated is less than RF_GSM_NV_MAX_STRUCTURES.
  */
  rfgsm_nv_band_data_type *buffers[RF_GSM_NV_MAX_STRUCTURES];
} rf_gsm_nv_config_storage;

/* table of default values for different static NVs*/

gsm_lna_switchpoint_type lna_switch_point_default = { -68,-80,-48,-48,-33,-57,127,127};

gsm_lna_switchpoint_type lna_enh_switch_point_default = { -102,-102,127,127,127,127,127,127};

gsm_ant_tuner_type gsm_ant_tuner_default = 
{
  {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  }
};

gsm_tx_timing_data_type gsm_tx_timing_data_default =
{
  -66,-1,0,2,-2,-30,1
};

gsm_extended_pa_switchpoint_type gsm_pa_switchpoint_default =
{
  2850,65535,65535,65535,65535,65535,65535,2150,65535,65535,65535,65535,65535,65535,2250,65535
};

gsm_env_gain_type env_gain_default = 
{
  {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024}, /*env_gain_freq_rgi_f1*/
  {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024}, /*env_gain_freq_rgi_f2*/
  {1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024}  /*env_gain_freq_rgi_f3*/
};

gsm_vbatt_type vbatt_default =
{
  {3200,3700,4200},    /* vbatt_levels*/
  {0, {0,0,0,0}},      /*vbatt_comp_value_lo*/
  {0, {0,0,0,0}}       /*vbatt_comp_value_hi*/
};

gsm_linear_tx_gain_param_type linear_tx_gain_param_default =
{
  83,                 /*gsm_linear_tx_gain_val*/
  47,                 /*edge_linear_tx_gain_val*/
  {5,15,40,60,1,2,3,4,7,9,20,25,30,50,55,0} /*charpredist_env_gain*/
};

gsm_power_levels power_levels_default = 
{
  450,650,850,1050,1250,1450,1650,1850,2050,2250,2450,2650,2850,3050,3250,3250
};

gsm_smps_pdm_tbl_type smps_pdm_tbl_default =
{
  {3700,3700,3700,3700,3700,3700,3700,3700},         /*cal_smps_pdm_tbl*/
  {3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700},  /*gsm_smps_pdm_tbl*/
  {3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700,3700}   /*edge_smps_pdm_tbl*/
};

gsm_pa_icq_bias_type pa_icq_bias_default = 
{
  {{{0}}}
};

gsm_pa_ramp_lut_type polar_paramp_lut_default =
{
  {0,0,0,150,300,496,758,1124,1581,2123,2737,3411,4128,4872,5626,6367,7077,7737,8331,8842,9252,9556,9740,9800,9800,9800,9800,9800,9800,9800},
  {9800,9740,9556,9252,8842,8331,7737,7077,6367,5626,4872,4128,3411,2737,2123,1581,1124,758,496,300,150,0,0,0,0,0,0,0,0,0}
};

amam_temp_comp gsm_amam_temp_comp_default =
{
 0,0,0,0,0,0,0,0,0,0,0,0,-2,-4,-6,-8
};

/*----------------------------------------------------------------------------*/
/*! This RF NV Item Mapping for base Band GSM850, these NVs IDs are used to get 
  NV ID for other  bands by adding a band specific offset .
  See rfgsm_nv_band_items_map_type for mapping of these NV IDs
  */

static rfgsm_nv_band_items_map_type
rfgsm_nv_band_items_map[RF_GSM_NV_PATH_NUM] =
{
/*RF_GSM_NV_PATH_0 */
{
  /*Rx cal*/
   RFNV_GSM_C0_GSM850_RX_CAL_DATA_I,
	  
  /*Tx cal*/
   {RFNV_GSM_C0_GSM850_TX_CAL_DATA_I, RFNV_GSM_C0_GSM850_TX_CAL_DATA_I},
 
  /*LNA switch points*/
   RFNV_GSM_C0_GSM850_LNA_SWPT_I,

  /*TX_RX_ANT_TUNER*/
   RFNV_GSM_C0_GSM850_TX_RX_ANT_TUNER_I,

  /* tx_timing_related NVs */
   RFNV_GSM_C0_GSM850_TX_TIMING_I,

  /*PA extended switch points*/
   RFNV_GSM_C0_GSM850_EXTENDED_PA_SWPT_I,

  /*KV CAL*/
   RFNV_GSM_C0_GSM850_KV_CAL_I,

  /*TX_GTR_THRESH*/
   RFNV_GSM_C0_GSM850_TX_GTR_THRESH_I,

  /*Polar path delay*/
   RFNV_GSM_C0_GSM850_POLAR_PATH_DELAY_I,

  /*VBATT*/
   RFNV_GSM_C0_GSM850_VBATT_I, 

  /*ENV GAIN*/
   RFNV_GSM_C0_GSM850_ENV_GAIN_I,

  /*AMAM TEMP COMP*/ 
   RFNV_GSM_C0_GSM850_AMAM_TEMP_COMP_I,

  /* TEMP COMP DATA*/
   RFNV_GSM_C0_GSM850_TEMP_COMP_I,

  /* ENH TEMP COMP DATA*/
   RFNV_GSM_C0_GSM850_ENH_TEMP_COMP_I,

  /* LINEAR_TX_GAIN_PARAM*/
   RFNV_GSM_C0_GSM850_LINEAR_TX_GAIN_PARAM_I,

  /*MULTISLOT_MAX_TX_PWR*/
   RFNV_GSM_C0_GSM850_MULTISLOT_MAX_TX_PWR_I,

  /*POWER LEVELS*/
   RFNV_GSM_C0_GSM850_POWER_LEVELS_I,

  /*SMPS_PDM_TBL*/
   RFNV_GSM_C0_GSM850_EXTENDED_SMPS_PDM_TBL_I,

  /*SAR BACKOFF*/
   RFNV_GSM_C0_GSM850_SAR_BACKOFF_I, 

  /* PA ramp profile*/
   RFNV_GSM_C0_GSM850_POLAR_RAMP_PROFILE_I,

  /* PA range map*/
   RFNV_GSM_C0_GSM850_PA_RANGE_MAP_I,

  /* Enhanced APT data*/
   RFNV_GSM_C0_GSM850_ENH_APT_I,

  /*PA BIAS ICQ data*/
   RFNV_GSM_C0_GSM850_PA_ICQ_BIAS,

  /* Therm Read During Calibration */
   RFNV_GSM_C0_GSM850_CAL_TEMP_I,

  #ifdef FEATURE_RF_HAS_QTUNER
  /*Antenna Tuner CL data*/
   RFNV_GSM_C0_850_ANT_TUNER_CL_I,
  #endif 

   RFNV_GSM_C2_GSM850_COEX_TX_OFFSET_INFO_TBL,

   RFNV_GSM_C2_GSM850_COEX_RX_OFFSET_INFO_TBL,

  /*LNA enh switch points*/
   RFNV_GSM_C0_GSM850_LNA_ENH_SWPT_I,

   /* Rx switchpoints and cal for HL mode on SAWless targets */
   RFNV_GSM_C0_GSM850_HL_RX_LNA_SWPT_I,

   RFNV_GSM_C0_GSM850_HL_RX_CAL_I
},
/*RF_GSM_NV_PATH_1
 there is no GSM diversity NVs available - copy Chain 0 NV Ids*/
{
  /*Rx cal*/
   RFNV_GSM_C1_GSM850_RX_CAL_DATA_I,
	  
  /*Tx cal*/
   {RFNV_GSM_C0_GSM850_TX_CAL_DATA_I, RFNV_GSM_C0_GSM850_TX_CAL_DATA_I},
 
  /*LNA switch points*/
   RFNV_GSM_C1_GSM850_LNA_SWPT_I,

  /*TX_RX_ANT_TUNER*/
   RFNV_GSM_C1_GSM850_TX_RX_ANT_TUNER_I,

  /* tx_timing_related NVs */
   RFNV_GSM_C0_GSM850_TX_TIMING_I,

  /*PA extended switch points*/
   RFNV_GSM_C0_GSM850_EXTENDED_PA_SWPT_I,

  /*KV CAL*/
   RFNV_GSM_C0_GSM850_KV_CAL_I,

  /*TX_GTR_THRESH*/
   RFNV_GSM_C0_GSM850_TX_GTR_THRESH_I,

  /*Polar path delay*/
   RFNV_GSM_C0_GSM850_POLAR_PATH_DELAY_I,

  /*VBATT*/
   RFNV_GSM_C0_GSM850_VBATT_I, 

  /*ENV GAIN*/
   RFNV_GSM_C0_GSM850_ENV_GAIN_I,

  /*AMAM TEMP COMP*/ 
   RFNV_GSM_C0_GSM850_AMAM_TEMP_COMP_I,

  /* TEMP COMP DATA*/
   RFNV_GSM_C0_GSM850_TEMP_COMP_I,

  /* ENH TEMP COMP DATA*/
   RFNV_GSM_C0_GSM850_ENH_TEMP_COMP_I,

  /* LINEAR_TX_GAIN_PARAM*/
   RFNV_GSM_C0_GSM850_LINEAR_TX_GAIN_PARAM_I,

  /*MULTISLOT_MAX_TX_PWR*/
   RFNV_GSM_C0_GSM850_MULTISLOT_MAX_TX_PWR_I,

  /*POWER LEVELS*/
   RFNV_GSM_C0_GSM850_POWER_LEVELS_I,

  /*SMPS_PDM_TBL*/
   RFNV_GSM_C0_GSM850_EXTENDED_SMPS_PDM_TBL_I,

  /*SAR BACKOFF*/
   RFNV_GSM_C0_GSM850_SAR_BACKOFF_I, 

  /* PA ramp profile*/
   RFNV_GSM_C0_GSM850_POLAR_RAMP_PROFILE_I,

  /* PA range map*/
   RFNV_GSM_C0_GSM850_PA_RANGE_MAP_I,

  /*Enhanced APT data */
   RFNV_GSM_C0_GSM850_ENH_APT_I,

  /*PA BIAS ICQ data*/
   RFNV_GSM_C0_GSM850_PA_ICQ_BIAS,

  /* Therm Read During Calibration */
   RFNV_GSM_C0_GSM850_CAL_TEMP_I,

   #ifdef FEATURE_RF_HAS_QTUNER
   /*Antenna Tuner CL data*/
   RFNV_GSM_C0_850_ANT_TUNER_CL_I,
   #endif 

   RFNV_GSM_C2_GSM850_COEX_TX_OFFSET_INFO_TBL,

   RFNV_GSM_C2_GSM850_COEX_RX_OFFSET_INFO_TBL,  

   /*LNA enh switch points*/
   RFNV_GSM_C0_GSM850_LNA_ENH_SWPT_I,

   /* Rx switchpoints and cal for HL mode on SAWless targets */
   RFNV_GSM_C1_GSM850_HL_RX_LNA_SWPT_I,

   RFNV_GSM_C1_GSM850_HL_RX_CAL_I
},
/*RF_GSM_NV_PATH_2 */
{
  /*Rx cal*/
   RFNV_GSM_C2_GSM850_RX_CAL_DATA_I,
	  
  /*Tx cal*/
   {RFNV_GSM_C2_GSM850_TX_CAL_DATA_I,RFNV_GSM_C2_GSM850_B_TX_CAL_DATA_I},
 
  /*LNA switch points*/
   RFNV_GSM_C2_GSM850_LNA_SWPT_I,

  /*TX_RX_ANT_TUNER*/
   RFNV_GSM_C2_GSM850_TX_RX_ANT_TUNER_I,

  /* tx_timing_related NVs */
   RFNV_GSM_C2_GSM850_TX_TIMING_I,

  /*PA switch points*/
   RFNV_GSM_C2_GSM850_EXTENDED_PA_SWPT_I,

  /*KV CAL*/
   RFNV_GSM_C2_GSM850_KV_CAL_I,

  /*TX_GTR_THRESH*/
   RFNV_GSM_C2_GSM850_TX_GTR_THRESH_I,

  /*Polar path delay*/
   RFNV_GSM_C2_GSM850_POLAR_PATH_DELAY_I,

  /*VBATT*/
   RFNV_GSM_C2_GSM850_VBATT_I, 

  /*ENV GAIN*/
   RFNV_GSM_C2_GSM850_ENV_GAIN_I,

  /*AMAM TEMP COMP*/ 
   RFNV_GSM_C2_GSM850_AMAM_TEMP_COMP_I,

  /* TEMP COMP DATA*/
   RFNV_GSM_C2_GSM850_TEMP_COMP_I,

  /* ENH TEMP COMP DATA*/
   RFNV_GSM_C2_GSM850_ENH_TEMP_COMP_I,

  /* LINEAR_TX_GAIN_PARAM*/
   RFNV_GSM_C2_GSM850_LINEAR_TX_GAIN_PARAM_I,

  /*MULTISLOT_MAX_TX_PWR*/
   RFNV_GSM_C2_GSM850_MULTISLOT_MAX_TX_PWR_I,

  /*POWER LEVELS*/
   RFNV_GSM_C2_GSM850_POWER_LEVELS_I,

  /*SMPS_PDM_TBL*/
   RFNV_GSM_C2_GSM850_SMPS_PDM_TBL_I,

  /*SAR BACKOFF*/
   RFNV_GSM_C2_GSM850_SAR_BACKOFF_I, 

  /* PA ramp profile*/
   RFNV_GSM_C2_GSM850_POLAR_RAMP_PROFILE_I,

  /* PA range map*/
   RFNV_GSM_C2_GSM850_PA_RANGE_MAP_I,

  /*Enhanced APT data */
   RFNV_GSM_C2_GSM850_ENH_APT_I,
  
  /*PA BIAS ICQ data*/
   RFNV_GSM_C2_GSM850_PA_ICQ_BIAS,
  
  /* Therm Read During Calibration */
   RFNV_GSM_C2_GSM850_CAL_TEMP_I,
  
   #ifdef FEATURE_RF_HAS_QTUNER
  /*Antenna Tuner CL data*/
   RFNV_GSM_C2_850_ANT_TUNER_CL_I,
   #endif

   RFNV_GSM_C2_GSM850_COEX_TX_OFFSET_INFO_TBL,

   RFNV_GSM_C2_GSM850_COEX_RX_OFFSET_INFO_TBL,

   /*LNA enh switch points*/
   RFNV_GSM_C2_GSM850_LNA_ENH_SWPT_I,

   /* Rx switchpoints and cal for HL mode on SAWless targets */
   RFNV_GSM_C2_GSM850_HL_RX_LNA_SWPT_I,

   RFNV_GSM_C2_GSM850_HL_RX_CAL_I
},
/*RF_GSM_NV_PATH_3
 there is no GSM diversity NVs available - copy Chain 0 NV Ids*/
{
  /*Rx cal*/
   RFNV_GSM_C3_GSM850_RX_CAL_DATA_I,
	  
  /*Tx cal*/
   {RFNV_GSM_C2_GSM850_TX_CAL_DATA_I, RFNV_GSM_C2_GSM850_TX_CAL_DATA_I},
 
  /*LNA switch points*/
   RFNV_GSM_C3_GSM850_LNA_SWPT_I,

  /*TX_RX_ANT_TUNER*/
   RFNV_GSM_C3_GSM850_TX_RX_ANT_TUNER_I,

  /* tx_timing_related NVs */
   RFNV_GSM_C2_GSM850_TX_TIMING_I,

  /*PA extended switch points*/
   RFNV_GSM_C2_GSM850_EXTENDED_PA_SWPT_I,

  /*KV CAL*/
   RFNV_GSM_C2_GSM850_KV_CAL_I,

  /*TX_GTR_THRESH*/
   RFNV_GSM_C2_GSM850_TX_GTR_THRESH_I,

  /*Polar path delay*/
   RFNV_GSM_C2_GSM850_POLAR_PATH_DELAY_I,

  /*VBATT*/
   RFNV_GSM_C2_GSM850_VBATT_I, 

  /*ENV GAIN*/
   RFNV_GSM_C2_GSM850_ENV_GAIN_I,

  /*AMAM TEMP COMP*/ 
   RFNV_GSM_C2_GSM850_AMAM_TEMP_COMP_I,

  /* TEMP COMP DATA*/
   RFNV_GSM_C2_GSM850_TEMP_COMP_I,

  /* ENH TEMP COMP DATA*/
   RFNV_GSM_C2_GSM850_ENH_TEMP_COMP_I,

  /* LINEAR_TX_GAIN_PARAM*/
   RFNV_GSM_C2_GSM850_LINEAR_TX_GAIN_PARAM_I,

  /*MULTISLOT_MAX_TX_PWR*/
   RFNV_GSM_C2_GSM850_MULTISLOT_MAX_TX_PWR_I,

  /*POWER LEVELS*/
   RFNV_GSM_C2_GSM850_POWER_LEVELS_I,

  /*SMPS_PDM_TBL*/
   RFNV_GSM_C2_GSM850_EXTENDED_SMPS_PDM_TBL_I,

  /*SAR BACKOFF*/
   RFNV_GSM_C2_GSM850_SAR_BACKOFF_I, 

  /* PA ramp profile*/
   RFNV_GSM_C2_GSM850_POLAR_RAMP_PROFILE_I,

  /* PA range map*/
   RFNV_GSM_C2_GSM850_PA_RANGE_MAP_I,

  /*Enhanced APT data */
   RFNV_GSM_C2_GSM850_ENH_APT_I,

  /*PA BIAS ICQ data*/
   RFNV_GSM_C2_GSM850_PA_ICQ_BIAS,

  /* Therm Read During Calibration */
   RFNV_GSM_C2_GSM850_CAL_TEMP_I,

   #ifdef FEATURE_RF_HAS_QTUNER
   /*Antenna Tuner CL data*/
   RFNV_GSM_C2_850_ANT_TUNER_CL_I,
   #endif 

   RFNV_GSM_C2_GSM850_COEX_TX_OFFSET_INFO_TBL,

   RFNV_GSM_C2_GSM850_COEX_RX_OFFSET_INFO_TBL,  

   /*LNA enh switch points*/
   RFNV_GSM_C2_GSM850_LNA_ENH_SWPT_I,

   /* Rx switchpoints and cal for HL mode on SAWless targets */
   RFNV_GSM_C3_GSM850_HL_RX_LNA_SWPT_I,

   RFNV_GSM_C3_GSM850_HL_RX_CAL_I
},
};

/*----------------------------------------------------------------------------*/
/*! RF NV Item Mapping for NVs which are commong across the bands.
      See rfgsm_nv_cmn_items_map_type for mapping of these NV IDs
  */

static rfgsm_nv_cmn_items_map_type
rfgsm_nv_cmn_items_map[RF_GSM_NV_PATH_NUM] =
{
  /*RF_GSM_NV_PATH_0 */
  {
    RFNV_GSM_DEBUG_I,
	
    RFNV_GSM_ENH_TEMP_COMP_ENABLE_I,

    RFNV_GSM_C0_PA_ICQ_BIAS_ENABLE,

    RFNV_GSM_CAL_TEMP_ENABLE_I,

    RFNV_GSM_RESERVED_I,
  },
  /*RF_GSM_NV_PATH_1 */
  {
    RFNV_GSM_DEBUG_I,
	
    RFNV_GSM_ENH_TEMP_COMP_ENABLE_I,

    RFNV_GSM_C0_PA_ICQ_BIAS_ENABLE,

    RFNV_GSM_CAL_TEMP_ENABLE_I,

    RFNV_GSM_RESERVED_I,
  },
  /*RF_GSM_NV_PATH_2 */
  {
    RFNV_GSM_DEBUG_I,
	
    RFNV_GSM_ENH_TEMP_COMP_ENABLE_I,

    RFNV_GSM_C2_PA_ICQ_BIAS_ENABLE,

    RFNV_GSM_CAL_TEMP_ENABLE_I,

    RFNV_GSM_RESERVED_I,
  },
};		

/*===========================================================================

FUNCTION rfcommon_nv_get_rf_bc_config

DESCRIPTION
  This externed function returns the Band Configuration value.

DEPENDENCIES
  None

RETURN VALUE
  NV_RF_CONFIG id.

SIDE EFFECTS
  None
  
===========================================================================*/
extern uint64 rfcommon_nv_get_rf_bc_config(rf_path_enum_type path);

/*===========================================================================

FUNCTION rfgsm_nv_init_override_amam_ampm_tables

DESCRIPTION
  This function initialises the AMAM and AMPM tables with defaults
  values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void rfgsm_nv_init_override_amam_ampm_tables(gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data)
{
  uint8 i = 0;
  uint16 j = 0;

  gsm_amam_data_type_unalign* amam_table_ptr = NULL;
  gsm_ampm_data_type_unalign* ampm_table_ptr = NULL; 

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"AMAM AMPM Tables overidden with default data", 0);

  /* Initialise the AMAM Tables to a default linear value */
  for ( i = 0; i < RFCOM_GSM_NUM_TX_CAL_CHAN; i++ )
  {
    amam_table_ptr = &(gsm_tx_cal_nv_data->amam_master_table.amam_tbl[i]);
    amam_table_ptr->data_valid = 1;

    for ( j = 0; j < (GSM_AMAM_TBL_SIZE * RFGSM_LINEAR_EXPANDED_TBL); j++ )
    {
       amam_table_ptr->data[j] = j*(1 << 8);
    }
  }

  /* Initialise the AMPM Tables to a default linear value */
  for ( i = 0; i < RFCOM_GSM_NUM_TX_CAL_CHAN; i++ )
  {
    ampm_table_ptr = &(gsm_tx_cal_nv_data->ampm_master_table.ampm_tbl[i]);
    ampm_table_ptr->data_valid = 1;

    memset(&(ampm_table_ptr->data[0]), 0, sizeof(uint16)*GSM_AMPM_TBL_SIZE*RFGSM_LINEAR_EXPANDED_TBL);

  }

  /* Initialise amam_max_dbm */
  gsm_tx_cal_nv_data->amam_max_dbm = 3034;

  /* Initialise rgi_used_for_pred */
  gsm_tx_cal_nv_data->rgi_for_pred = 17;

}


/*===========================================================================

FUNCTION rfgsm_nv_init_default_tx_cal_values

DESCRIPTION
  This function only initialises the AMAM and AMPM tables for now.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void rfgsm_nv_init_default_tx_cal_values(gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data)
{
  rfgsm_nv_init_override_amam_ampm_tables(gsm_tx_cal_nv_data);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the RF BC Configuration flag value for a given
  logical RF device

  @details

  @retval
  
*/
uint64 rfgsm_nv_get_rf_bc_config( rfm_device_enum_type rfm_dev )
{
  uint64 config = 0;
  rfcommon_nv_tbl_type * rfcommon_nv_tbl_ptr = rfcommon_nv_get_tbl_ptr();

  if ( rfm_dev == RFM_DEVICE_0 ||
       rfm_dev == RFM_DEVICE_1 ||
       rfm_dev == RFM_DEVICE_2 ||
       rfm_dev == RFM_DEVICE_3 )
  {
    config = rfcommon_nv_tbl_ptr->rf_bc_config[rfm_dev];
  }
  else
  {
    MSG_ERROR("Unrecognized device in rfgsm_nv_get_rf_bc_config(): %d",
              rfm_dev, 0, 0);
  }

  return config;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the lowest supported band for a given device

  @details
  
  @param rfm_dev
  The RFM device for which the lowest GSM band supported is required
 
  @return
  GSM band for passed RFM device
*/
rfcom_gsm_band_type rfgsm_nv_get_lowest_supported_band( rfm_device_enum_type rfm_dev )
{
  uint64 band_mask;
  rfcom_gsm_band_type first_nv_band_supported;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return RFCOM_BAND_GSM850;
  }

  band_mask = rfgsm_nv_get_rf_bc_config( rfm_dev );

  if( band_mask & SYS_BAND_MASK_GSM_850 )
  {
    first_nv_band_supported = RFCOM_BAND_GSM850;
  }
  else if( band_mask & SYS_BAND_MASK_GSM_EGSM_900 )
  {
    first_nv_band_supported = RFCOM_BAND_GSM900;
  }
  else if( band_mask & SYS_BAND_MASK_GSM_DCS_1800 )
  {
    first_nv_band_supported = RFCOM_BAND_GSM1800;
  }
  else if ( band_mask & SYS_BAND_MASK_GSM_PCS_1900 )
  {
    first_nv_band_supported = RFCOM_BAND_GSM1900;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF BC config =%d, does not support any GSM bands for rfm device =%d", band_mask, rfm_dev);
    return RFCOM_BAND_GSM850;
  }

  return( first_nv_band_supported );
}


/*===========================================================================

FUNCTION RFGSM_NV_GET_TBL_PTR

DESCRIPTION
  Returns pointer to GSM RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_band_data_type *rfgsm_nv_get_tbl_ptr
( 
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band,
  rfgsm_nv_rxtx_enum_type rx_tx
) 
{
  rfgsm_nv_band_data_type *nv_data_ptr;
  rfgsm_nv_path_type nv_path;
  rfgsm_nv_supported_type nv_band = RFGSM_NV_MAX_SUPPORTED;

  nv_path = rfgsm_nv_get_nv_container(rfm_dev,band,rx_tx);  

  if (nv_path >= RF_GSM_NV_PATH_NUM )
  {

    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_tbl_ptr(), Invalid NV container, logical dev %d, band %d, rx_tx = %d", nv_path, nv_band, rx_tx);
    return NULL;
  }

  nv_band = rfgsm_core_rfcom_band_to_nvband(band);

  if ((rfm_dev < RFM_MAX_WAN_DEVICES ) && (nv_band < RFGSM_NV_MAX_SUPPORTED) && (rx_tx < RFGSM_NV_RXTX_INVALID))
  {
    nv_data_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_band_node[nv_band].rfgsm_nv_band_data_buffer_ptr;
  	
    if (nv_data_ptr == NULL)
    {
      /* This is bad, need to check what we can do as error notification. An err_fatal may be required*/
      MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "No NV structure, path %d, band %d, rx_tx = %d", nv_path, nv_band, rx_tx);
    }
  }
  else
  {
     nv_data_ptr = NULL;  
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_tbl_ptr:INVALID NV path = %d", nv_path);
  } 
 
  return nv_data_ptr;
} /* end of rfgsm_nv_get_tbl_ptr */ 

/*===========================================================================

FUNCTION RFGSM_NV_GET_CMN_TBL_PTR

DESCRIPTION
  Returns pointer to GSM COMMON RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM COMMON RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_cmn_data_type *rfgsm_nv_get_cmn_tbl_ptr (rfm_device_enum_type rfm_dev) 
{
  rfgsm_nv_cmn_data_type *nv_data_ptr = NULL;
  rfgsm_nv_path_type nv_path = RF_GSM_NV_PATH_MAX;


 
  /* Assuming NO TX Band split in GSM i.e. all TX bands on same WTR, for common NV table we choose the container for the first TX/RX band supported*/



  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {
     ERR_FATAL("rfgsm_nv_get_cmn_tbl_ptr: invalid rfm_dev: %d", rfm_dev,0,0);
  } 

  /* For the common nv table,Get the nv container for the first supported band on a given logical device*/

  nv_path = rfgsm_nv_get_cmn_nv_container( rfm_dev);
 
  if (nv_path < RF_GSM_NV_PATH_NUM)
  {
    nv_data_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr;
  	
    if (nv_data_ptr == NULL)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_cmn_tbl_ptr: nv_data_ptr is NULL, path = %d", nv_path);
    }
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_cmn_tbl_ptr: couldn't find a supported band for logical device:%d . No NV container found", rfm_dev);

  } 
 
  return nv_data_ptr;
} /* end of rfgsm_nv_get_cmn_tbl_ptr */ 

/*===========================================================================

FUNCTION rfgsm_nv_get_tx_gain_info_ptr                                 

DESCRIPTION
  Returns pointer to GSM TX frequency compensation switchpoints.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_tx_gain_range_info_type *rfgsm_nv_get_tx_gain_info_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band,
  rfgsm_modulation_type mod_type
)
{
  rfnv_tx_gain_range_info_type *tx_gain_info_ptr = NULL;
  rfgsm_nv_path_type nv_path;

  nv_path = rfgsm_nv_get_nv_container(rfm_dev,band,RFGSM_NV_TX);

  if (nv_path < RF_GSM_NV_PATH_NUM)
  {
    switch (band)
    {
    case RFCOM_BAND_GSM850:
      tx_gain_info_ptr = 
	  	((mod_type == RF_MOD_GMSK) ? (&gsm_850_gsm_tx_gain_range_info[nv_path]) : (&gsm_850_edge_tx_gain_range_info[nv_path]));
      break;
    
    case RFCOM_BAND_GSM900:
      tx_gain_info_ptr = 
	  	((mod_type == RF_MOD_GMSK) ? (&gsm_900_gsm_tx_gain_range_info[nv_path]) : (&gsm_900_edge_tx_gain_range_info[nv_path]));
      break;
    
    case RFCOM_BAND_GSM1800:
      tx_gain_info_ptr = 
	  	((mod_type == RF_MOD_GMSK) ? (&gsm_1800_gsm_tx_gain_range_info[nv_path]) : (&gsm_1800_edge_tx_gain_range_info[nv_path]));
      break;

    case RFCOM_BAND_GSM1900:
	  tx_gain_info_ptr = 
		((mod_type == RF_MOD_GMSK) ? (&gsm_1900_gsm_tx_gain_range_info[nv_path]) : (&gsm_1900_edge_tx_gain_range_info[nv_path]));
	break;
    
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_tx_gain_info_ptr():INVALID BAND", 0);
      break;
    } 
  }

  return tx_gain_info_ptr;
} /* end of rfgsm_nv_get_tx_gain_info_ptr */

/*===========================================================================

FUNCTION RFGSM_NV_GET_RX_FREQ_COMP_SWITCH_PTR                                 

DESCRIPTION
  Returns pointer to GSM RX frequency compensation switch.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_rx_switch_type *rfgsm_nv_get_rx_freq_comp_switch_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band
)
{

  rfgsm_nv_path_type nv_path = RF_GSM_NV_PATH_0;
  rfgsm_nv_supported_type nv_band = RFGSM_NV_850;
  rfgsm_nv_rx_switch_type *rx_switch_ptr = NULL;
  
  nv_path = rfgsm_nv_get_nv_container(rfm_dev,band,RFGSM_NV_RX);
  nv_band = rfgsm_core_rfcom_band_to_nvband(band);

  rx_switch_ptr = &gsm_900_rx_switch_dBm[nv_path];

  switch (nv_band)
  {
  case RFGSM_NV_850:
    rx_switch_ptr = &gsm_850_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_900:
    rx_switch_ptr = &gsm_900_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_1800:
    rx_switch_ptr = &gsm_1800_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_1900:
    rx_switch_ptr = &gsm_1900_rx_switch_dBm[nv_path];
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_rx_freq_comp_switch_ptr():INVALID BAND", 0);
    break;
  } 
 
  return rx_switch_ptr;
} /* end of rfgsm_nv_get_rx_freq_comp_switch_ptr */

/*===========================================================================

FUNCTION RFGSM_NV_GET_HL_RX_FREQ_COMP_SWITCH_PTR                                 

DESCRIPTION
  Returns pointer to GSM RX frequency compensation switch to be used in HL mode
  on SAWless targets.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch to be used in HL mode
  on SAWless targets.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_rx_switch_type *rfgsm_nv_get_hl_rx_freq_comp_switch_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band
)
{

  rfgsm_nv_path_type nv_path = RF_GSM_NV_PATH_0;
  rfgsm_nv_supported_type nv_band = RFGSM_NV_850;
  rfgsm_nv_rx_switch_type *rx_switch_ptr = NULL;
  
  nv_path = rfgsm_nv_get_nv_container(rfm_dev,band,RFGSM_NV_RX);
  nv_band = rfgsm_core_rfcom_band_to_nvband(band);

  rx_switch_ptr = &gsm_900_hl_rx_switch_dBm[nv_path];

  switch (nv_band)
  {
  case RFGSM_NV_850:
    rx_switch_ptr = &gsm_850_hl_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_900:
    rx_switch_ptr = &gsm_900_hl_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_1800:
    rx_switch_ptr = &gsm_1800_hl_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_1900:
    rx_switch_ptr = &gsm_1900_hl_rx_switch_dBm[nv_path];
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_hl_rx_freq_comp_switch_ptr():INVALID BAND", 0);
    break;
  } 
 
  return rx_switch_ptr;
} /* end of rfgsm_nv_get_rx_freq_comp_switch_ptr */



/*===========================================================================

FUNCTION RFGSM_NV_GET_ENH_RX_SWITCH_PTR                                 

DESCRIPTION
  Returns pointer to GSM RX frequency compensation switch.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_enh_rx_switch_type *rfgsm_nv_get_enh_rx_switch_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band
)
{

  rfgsm_nv_path_type nv_path = RF_GSM_NV_PATH_0;
  rfgsm_nv_supported_type nv_band = RFGSM_NV_850;
  rfgsm_nv_enh_rx_switch_type *enh_rx_switch_ptr = NULL;
  
  nv_path = rfgsm_nv_get_nv_container(rfm_dev,band,RFGSM_NV_RX);
  nv_band = rfgsm_core_rfcom_band_to_nvband(band);

  enh_rx_switch_ptr = &gsm_900_enh_rx_switch_dBm[nv_path];

  switch (nv_band)
  {
  case RFGSM_NV_850:
    enh_rx_switch_ptr = &gsm_850_enh_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_900:
    enh_rx_switch_ptr = &gsm_900_enh_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_1800:
    enh_rx_switch_ptr = &gsm_1800_enh_rx_switch_dBm[nv_path];
    break;
  
  case RFGSM_NV_1900:
    enh_rx_switch_ptr = &gsm_1900_enh_rx_switch_dBm[nv_path];
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_enh_rx_switch_ptr():INVALID BAND", 0);
    break;
  } 
 
  return enh_rx_switch_ptr;
} /* end of rfgsm_nv_get_enh_rx_switch_ptr */


/*===========================================================================

FUNCTION RFGSM_NV_GET_CLOSEST_ARFCN_INDEX

DESCRIPTION
  This function returns the index of the the ARFCN that is closest to the 
  desired ARFCN.

DEPENDENCIES
  Assumes that the ARFCNs are stored in ascending order by the RFCAL program.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint8 rfgsm_nv_get_closest_arfcn_index( rfm_device_enum_type rfm_dev, uint16 arfcn)
{
  /* --------------------------------------------------------------------- */
  /*                  Search for Right Bin                                 */
  /* --------------------------------------------------------------------- */
  uint8 arfcn_index = 0;
    uint16 prev_delta_ch_space=RF_LARGEST_DELTA_CH_SPACE, curr_delta_ch_space;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_band_data_type *rfgsm_rx_nv_tbl = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return 0;
  }
  
  rfgsm_rx_nv_tbl = rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl;
  if (rfgsm_rx_nv_tbl == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_rx_nv_tbl is NULL!", 0);
    return 0;
  }  

  /* Try to converge on the ARFCN entry in the table that is closest to
  ** the desired ARFCN.  Achieve this by evaluating the difference between
  ** the desired ARFCN and the table entry.
  */
 
  for (arfcn_index=0; arfcn_index< rfgsm_rx_nv_tbl->rx_cal_nv_data.rx_cal_chan_size; arfcn_index++)
  {
    curr_delta_ch_space = (arfcn < rfgsm_rx_nv_tbl->rx_cal_nv_data.rx_cal_chan_list[arfcn_index] ?
                                   rfgsm_rx_nv_tbl->rx_cal_nv_data.rx_cal_chan_list[arfcn_index] - arfcn :
                                   arfcn - rfgsm_rx_nv_tbl->rx_cal_nv_data.rx_cal_chan_list[arfcn_index]);

    if (curr_delta_ch_space < prev_delta_ch_space)
    {
      prev_delta_ch_space = curr_delta_ch_space;
    }
    else
    {
      break;
    }
  }

  arfcn_index = arfcn_index - 1;

  if (arfcn_index < NV_DCS_CAL_ARFCN_SIZ)
  {
    return ( arfcn_index );
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, " arfcn index is out of range: %d",arfcn_index);
    return 0;
  }
} /* end of rfgsm_nv_get_closest_arfcn_index */


/*===========================================================================

FUNCTION RFGSM_NV_RX_FREQ_COMP

DESCRIPTION
  This function returns the shadowed copy of the specific NV data item
  for the specified ARFCN.  It determines the band class and applies
  the appropriate frequency compensation values.

DEPENDENCIES
  rfnv_retrieve_nv_items() must have been called apriori to populate the
  rfgsm_nv_xxxx_tbl.  Also, assumes that the ARFCNs are stored in
  ascending order by the RFCAL program.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int16 rfgsm_nv_rx_freq_comp
(
  rfm_device_enum_type rfm_dev,
  rfnv_gsm_lna_state_type rfnv_gsm_lna_state,
  uint8 arfcn_index
)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  rfgsm_nv_band_data_type *rfgsm_rx_nv_tbl = NULL;
  gsm_rx_cal_data_nv_type *rfgsm_rx_cal_tbl = NULL;
  boolean is_high_lin = FALSE;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return 0;
  }
  
  rfgsm_rx_nv_tbl = rfgsm_core_handle_ptr->rfgsm_rx_nv_tbl;
  if (rfgsm_rx_nv_tbl == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_rx_nv_tbl is NULL!", 0);
    return 0;
  }  
  
  /* Get the standard Rx Cal table in the firs instance */
  rfgsm_rx_cal_tbl = &(rfgsm_rx_nv_tbl->rx_cal_nv_data);

  /* Check if we need to use the HL cal table instead */
  if ( rfgsm_core_get_band_sawless_status(rfm_dev, rfgsm_core_handle_ptr->band) == TRUE)
  {
    /* Check lin mode if the current band is SAWless */
    if ( rfgsm_core_is_arfcn_in_high_lin(rfm_dev, 
                                         rfgsm_core_handle_ptr->rf_rx_arfcn,
                                         rfgsm_core_handle_ptr->band,
                                         &is_high_lin) == FALSE )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not get lin status!", 0);
    }
    else
    {
      /* Check override status and override the flag accordingly */
      if ( rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_HL_OVERRIDE )
      {
        is_high_lin = TRUE;
      }
      else if ( rfgsm_core_handle_ptr->sawless_linearity_control.linearity_override == RFGSM_SAWLESS_LL_OVERRIDE )
      {
        is_high_lin = FALSE;
      }
      /* Check if we need to use the HL cal table and if there are calibrated channels in the NV */
      if ( is_high_lin && (rfgsm_rx_nv_tbl->hl_rx_cal_nv_data.rx_cal_chan_size > 0) )
      {
        /* Switch to use the HL table if the current channel is in HL mode or the HL override is in effect */
        rfgsm_rx_cal_tbl = &(rfgsm_rx_nv_tbl->hl_rx_cal_nv_data);
      }
      
    }
  }

  if (arfcn_index < rfgsm_rx_cal_tbl->rx_cal_chan_size)
  {
    return rfgsm_rx_cal_tbl->rx_freq_comp_data[rfnv_gsm_lna_state].freq_comp_data[arfcn_index];
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "cal_chan_index is out of range: %d",arfcn_index);
    return 0;
  }
}  /* end of rfgsm_nv_rx_freq_comp */

/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_RX_SWITCH_POINTS_BAND

DESCRIPTION
  This function computes the Rx switch points for the set of calibrated
  channels (ARFCNs).  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rfgsm_nv_precompute_rx_switch_points_band(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  uint8 i;
  int16 gr1_max_dBm, gr2_max_dBm, gr3_max_dBm, gr4_max_dBm;
  int16 gr2_min_dBm, gr3_min_dBm, gr4_min_dBm, gr5_min_dBm;
  rfgsm_nv_band_data_type *rf_gsm_nv_data_buffer_ptr = NULL;
  rfgsm_nv_rx_switch_type *rx_switch_ptr = NULL;
  gsm_lna_switchpoint_type switchpoints;
  gsm_rx_cal_data_nv_type rx_cal_data;
  uint16 rf_sigma_delta_max_db;

  /* get NV data Buffer ptr for given NV path and band*/
  rf_gsm_nv_data_buffer_ptr = rfgsm_nv_get_tbl_ptr(rfm_dev, band, RFGSM_NV_RX);

  if (rf_gsm_nv_data_buffer_ptr == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_gsm_nv_data_buffer_ptr is NULL! for device %d, band %d", rfm_dev, band);
    return;
  }
  
  switchpoints = rf_gsm_nv_data_buffer_ptr->rx_static_nv_data_ptr->lna_switch_point;

  rx_cal_data = rf_gsm_nv_data_buffer_ptr->rx_cal_nv_data;

  /* get ptr for Rx freq comp switch data  for given device and band*/
  rx_switch_ptr =  rfgsm_nv_get_rx_freq_comp_switch_ptr(rfm_dev, band);
  
  /* get sigma_delta_max_db for given device*/
  rf_sigma_delta_max_db = rfgsm_core_get_rf_sigma_delta_max_db(rfm_dev);

  /* Populate entries */
  for (i=0; i< RFCOM_GSM_NUM_RX_CAL_CHAN; i++)
  {

    gr1_max_dBm = (int16)((switchpoints.max_gainrange_1 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_1)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_1].freq_comp_data[i]);

    gr2_min_dBm = (int16)((switchpoints.min_gainrange_2 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_2)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_2].freq_comp_data[i]);


    rx_switch_ptr->rx_switch_1_2_dBm[i] = (gr1_max_dBm + gr2_min_dBm)/2;

    gr2_max_dBm = (int16)((switchpoints.max_gainrange_2 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_2)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_2].freq_comp_data[i]);


    gr3_min_dBm = (int16)((switchpoints.min_gainrange_3 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_3)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_3].freq_comp_data[i]);

    rx_switch_ptr->rx_switch_2_3_dBm[i] = (gr2_max_dBm + gr3_min_dBm)/2;

    gr3_max_dBm = (int16)((switchpoints.max_gainrange_3 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_3)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_3].freq_comp_data[i]);

    gr4_min_dBm = (int16)((switchpoints.min_gainrange_4 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_4)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_4].freq_comp_data[i]);

    rx_switch_ptr->rx_switch_3_4_dBm[i] = (gr3_max_dBm + gr4_min_dBm)/2;

    gr4_max_dBm = (int16)((switchpoints.max_gainrange_4 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_4)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_4].freq_comp_data[i]);

    gr5_min_dBm = (int16)((switchpoints.min_gainrange_5 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_5)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_5].freq_comp_data[i]);
  
    rx_switch_ptr->rx_switch_4_5_dBm[i] = (gr4_max_dBm + gr5_min_dBm)/2;
    }

  }  /*end of rfgsm_nv_precompute_rx_switch_points_band*/

/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_HL_RX_SWITCH_POINTS_BAND

DESCRIPTION
  This function computes the Rx switch points for the set of calibrated
  channels (ARFCNs).  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rfgsm_nv_precompute_hl_rx_switch_points_band(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  uint8 i;
  int16 gr1_max_dBm, gr2_max_dBm, gr3_max_dBm, gr4_max_dBm;
  int16 gr2_min_dBm, gr3_min_dBm, gr4_min_dBm, gr5_min_dBm;
  rfgsm_nv_band_data_type *rf_gsm_nv_data_buffer_ptr = NULL;
  rfgsm_nv_rx_switch_type *rx_switch_ptr = NULL;
  gsm_lna_switchpoint_type switchpoints;
  gsm_rx_cal_data_nv_type rx_cal_data;
  gsm_rx_cal_data_nv_type hl_rx_cal_offset_data;
  uint16 rf_sigma_delta_max_db;

  /* get NV data Buffer ptr for given NV path and band*/
  rf_gsm_nv_data_buffer_ptr = rfgsm_nv_get_tbl_ptr(rfm_dev, band, RFGSM_NV_RX);

  if (rf_gsm_nv_data_buffer_ptr == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_gsm_nv_data_buffer_ptr is NULL! for device %d, band %d", rfm_dev, band);
    return;
  }
  
  switchpoints = rf_gsm_nv_data_buffer_ptr->rx_static_nv_data_ptr->hl_lna_switch_point;

  /* Get the HL cal data */
  rx_cal_data = rf_gsm_nv_data_buffer_ptr->hl_rx_cal_nv_data;

  /* get ptr for Rx freq comp switch data  for given device and band*/
  rx_switch_ptr =  rfgsm_nv_get_hl_rx_freq_comp_switch_ptr(rfm_dev, band);
  
  /* get sigma_delta_max_db for given device*/
  rf_sigma_delta_max_db = rfgsm_core_get_rf_sigma_delta_max_db(rfm_dev);

  /* Populate entries */
  for (i=0; i< RFCOM_GSM_NUM_RX_CAL_CHAN; i++)
  {
    gr1_max_dBm = (int16)((switchpoints.max_gainrange_1 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_1)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_1].freq_comp_data[i]);


    gr2_min_dBm = (int16)((switchpoints.min_gainrange_2 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_2)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_2].freq_comp_data[i]);


    rx_switch_ptr->rx_switch_1_2_dBm[i] = (gr1_max_dBm + gr2_min_dBm)/2;

    gr2_max_dBm = (int16)((switchpoints.max_gainrange_2 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_2)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_2].freq_comp_data[i]);


    gr3_min_dBm = (int16)((switchpoints.min_gainrange_3 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_3)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_3].freq_comp_data[i]);

    rx_switch_ptr->rx_switch_2_3_dBm[i] = (gr2_max_dBm + gr3_min_dBm)/2;

    gr3_max_dBm = (int16)((switchpoints.max_gainrange_3 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_3)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_3].freq_comp_data[i]);

    gr4_min_dBm = (int16)((switchpoints.min_gainrange_4 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_4)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_4].freq_comp_data[i]);

    rx_switch_ptr->rx_switch_3_4_dBm[i] = (gr3_max_dBm + gr4_min_dBm)/2;

    gr4_max_dBm = (int16)((switchpoints.max_gainrange_4 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_4)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_4].freq_comp_data[i]);

    gr5_min_dBm = (int16)((switchpoints.min_gainrange_5 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_5)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_5].freq_comp_data[i]);
  
    rx_switch_ptr->rx_switch_4_5_dBm[i] = (gr4_max_dBm + gr5_min_dBm)/2;
    }

  }  /*end of rfgsm_nv_precompute_hl_rx_switch_points_band*/

/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_RX_SWITCH_POINTS

DESCRIPTION
  This function computes the Rx switch points for the set of calibrated
  channels (ARFCNs).  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

  This function precomputes the standard switchpoints and additional HL switchpoints
  for use on SAWless targets

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_precompute_rx_switch_points(rfm_device_enum_type rfm_dev)
{
  
  /* Compute rx switch points for GSM850.
  */
  rfgsm_nv_precompute_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM850);
  rfgsm_nv_precompute_hl_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM850);

  /* Compute rx switch points for GSM900.
  */
  rfgsm_nv_precompute_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM900);
  rfgsm_nv_precompute_hl_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM900);

  /* Compute rx switch points for GSM1800.
  */
  rfgsm_nv_precompute_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM1800);
  rfgsm_nv_precompute_hl_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM1800);

  /* Compute rx switch points for GSM1900.
  */
  rfgsm_nv_precompute_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM1900);
  rfgsm_nv_precompute_hl_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM1900);

}/* end of rfgsm_nv_precompute_rx_switch_points */

/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_ENH_RX_SWITCH_POINTS_BAND

DESCRIPTION
  This function computes the enhanced rx switch points for the set of calibrated
  channels (ARFCNs).  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rfgsm_nv_precompute_enh_rx_switch_points_band(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  uint8 i;
  int16 gr1_max_dBm, gr2_min_dBm;
  rfgsm_nv_band_data_type *rf_gsm_nv_data_buffer_ptr = NULL;
  rfgsm_nv_enh_rx_switch_type *enh_rx_switch_ptr = NULL;
  gsm_lna_switchpoint_type enh_switchpoints;
  gsm_rx_cal_data_nv_type rx_cal_data;
  uint16 rf_sigma_delta_max_db;

  /* get NV data Buffer ptr for given NV path and band*/
  rf_gsm_nv_data_buffer_ptr = rfgsm_nv_get_tbl_ptr(rfm_dev, band, RFGSM_NV_RX);

  if (rf_gsm_nv_data_buffer_ptr == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_gsm_nv_data_buffer_ptr is NULL! for device %d, band %d", rfm_dev, band);
    return;
  }
  
  enh_switchpoints = rf_gsm_nv_data_buffer_ptr->rx_static_nv_data_ptr->lna_enh_switch_point;

  rx_cal_data = rf_gsm_nv_data_buffer_ptr->rx_cal_nv_data;

  /* get ptr for Rx freq comp switch data  for given device and band*/
  enh_rx_switch_ptr =  rfgsm_nv_get_enh_rx_switch_ptr(rfm_dev, band);
  
  /* get sigma_delta_max_db for given device*/
  rf_sigma_delta_max_db = rfgsm_core_get_rf_sigma_delta_max_db(rfm_dev);

  /* only highest gain range switchpoint is used for enhanced Rx mode */
  for (i=0; i< RFCOM_GSM_NUM_RX_CAL_CHAN; i++)
  {

    gr1_max_dBm = (int16)((enh_switchpoints.max_gainrange_1 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_1)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_1].freq_comp_data[i]);

    gr2_min_dBm = (int16)((enh_switchpoints.min_gainrange_2 + 
                           rf_sigma_delta_max_db + 
                           rfgsm_mdsp_get_digital_gain(RFGSMLIB_GAIN_RANGE_2)) * 16 - 
                           rx_cal_data.rx_freq_comp_data[RFNV_GSM_LNA_STATE_2].freq_comp_data[i]);

    enh_rx_switch_ptr->enh_rx_switch_1_2_dBm[i] = (gr1_max_dBm + gr2_min_dBm)/2;
  }
  MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"enh_rx: max_gainrange_1 %d, min_gainrange_2 %d, enh_rx_switch_1_2_dBm[0] %d, enh_rx_switch_1_2_dBm[2] %d", enh_switchpoints.max_gainrange_1, enh_switchpoints.min_gainrange_2, enh_rx_switch_ptr->enh_rx_switch_1_2_dBm[0], enh_rx_switch_ptr->enh_rx_switch_1_2_dBm[2]);
}  /*end of rfgsm_nv_precompute_enh_rx_switch_points_band*/


/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_ENH_RX_SWITCH_POINTS

DESCRIPTION
  This function computes the enhanced rx switch points for the set of calibrated
  channels (ARFCNs).  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_precompute_enh_rx_switch_points(rfm_device_enum_type rfm_dev)
{
  
  /* Compute enhanced rx switch points for GSM850.
  */
  rfgsm_nv_precompute_enh_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM850);

  /* Compute enhanced rx switch points for GSM900.
  */
  rfgsm_nv_precompute_enh_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM900);

  /* Compute enhanced rx switch points for GSM1800.
  */
  rfgsm_nv_precompute_enh_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM1800);

  /* Compute enhanced rx switch points for GSM1900.
  */
  rfgsm_nv_precompute_enh_rx_switch_points_band(rfm_dev, RFCOM_BAND_GSM1900);

}/* end of rfgsm_nv_precompute_rx_switch_points */


/*===========================================================================


FUNCTION rfgsm_nv_precompute_tx_gain_ranges_info

DESCRIPTION
  This function computes the number of total pa range switchpoints based on
   band and also fills in the pointers passed for pa range switchpoints, 
  predistortion switchpoints, and r0 r1 values from NV. It also stores some 
  of the TX gain buffer items.

DEPENDENCIES
  This function can be used for any GSM bands as all bands can now have up to 8 PA switch points.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rfgsm_nv_precompute_tx_gain_ranges (rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfcom_band)
{

  uint16 actual_pa_range_count = 0;
  uint16 count_pa_swp = 0;

  /* For loop counters. */
  uint16 predistort_count = 0;
  uint16 pa_range_count = 0;
  uint8 pa_gain_count = 0;
  
  /* List of PA switchpoints */
  uint16 pa_range_swpts[RFGSM_NV_NUM_PA_RANGES]= {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
  uint8 num_valid_pa_ranges = 0;

  rfnv_tx_gain_range_info_type *rfnv_tx_gain_range_info_ptr = NULL;
  rfgsm_nv_band_data_type *rfgsm_nv_tbl = NULL;
  gsm_extended_pa_switchpoint_type  pa_switch_point_data;

  /* List of Predistortion switchpoints
  * To disable a predistortion level, it can be set to 
  * 0xFFFF which will be smaller than any pa range.*/
  uint16 pred_swpts[RFGSM_NV_MAX_LINEAR_PA_PRED_SWPTS] = {0xFFFF,0xFFFF};

  uint16 pa_state[RFGSM_NV_NUM_PA_RANGES] = {0,0,0,0,0,0,0,0};

  rfgsm_nv_tbl = rfgsm_nv_get_tbl_ptr(rfm_dev, rfcom_band, RFGSM_NV_TX);

  if (rfgsm_nv_tbl == NULL)
  {
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_tbl ptr is NULL! for device %d", rfm_dev);
	return;
  }

  pa_switch_point_data = rfgsm_nv_tbl->tx_static_nv_data_ptr->pa_switch_point;

	//----------------------Edge Portion ------------------------//
  /* Get Tx gain range info ptr*/
  rfnv_tx_gain_range_info_ptr = rfgsm_nv_get_tx_gain_info_ptr(rfm_dev, rfcom_band, RF_MOD_8PSK);

  if (rfnv_tx_gain_range_info_ptr == NULL)
  {
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_tx_gain_range_info_ptr ptr is NULL!", 0);
	return;
  }

  if (pa_switch_point_data.edge_pa_swpt_r6_to_r7 != 0xFFFF)
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r6_to_r7;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_7;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.edge_pa_swpt_r5_to_r6 != 0xFFFF) && (pa_switch_point_data.edge_pa_swpt_r5_to_r6 != pa_switch_point_data.edge_pa_swpt_r6_to_r7))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r5_to_r6;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_6;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.edge_pa_swpt_r4_to_r5 != 0xFFFF) && (pa_switch_point_data.edge_pa_swpt_r4_to_r5 != pa_switch_point_data.edge_pa_swpt_r5_to_r6))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r4_to_r5;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_5;
	num_valid_pa_ranges++;
  } 
  if ((pa_switch_point_data.edge_pa_swpt_r3_to_r4 != 0xFFFF) && (pa_switch_point_data.edge_pa_swpt_r3_to_r4 != pa_switch_point_data.edge_pa_swpt_r4_to_r5))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r3_to_r4;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_4;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.edge_pa_swpt_r2_to_r3 != 0xFFFF) && (pa_switch_point_data.edge_pa_swpt_r2_to_r3 != pa_switch_point_data.edge_pa_swpt_r3_to_r4))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r2_to_r3;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_3;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.edge_pa_swpt_r1_to_r2 != 0xFFFF) && (pa_switch_point_data.edge_pa_swpt_r1_to_r2 != pa_switch_point_data.edge_pa_swpt_r2_to_r3))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r1_to_r2;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_2;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.edge_pa_swpt_r0_to_r1 != 0xFFFF) && (pa_switch_point_data.edge_pa_swpt_r0_to_r1 != pa_switch_point_data.edge_pa_swpt_r1_to_r2))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.edge_pa_swpt_r0_to_r1;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_1;
	num_valid_pa_ranges++;
  }
  pa_range_swpts[num_valid_pa_ranges] = 0xFFFF;
  pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_0;
  num_valid_pa_ranges++;

  /* The first pred swpt is the minimum of the two */
  pred_swpts[0] = pa_switch_point_data.pa_predist_swpt1 <=	pa_switch_point_data.pa_predist_swpt2 ? pa_switch_point_data.pa_predist_swpt1 : pa_switch_point_data.pa_predist_swpt2;
  pred_swpts[1] = pa_switch_point_data.pa_predist_swpt1 <  pa_switch_point_data.pa_predist_swpt2 ? pa_switch_point_data.pa_predist_swpt2 : pa_switch_point_data.pa_predist_swpt1;


  for (predistort_count=0; predistort_count<RFGSM_NV_MAX_LINEAR_PA_PRED_SWPTS; predistort_count++)
  {
	/* actual_pa_range_count. This is used to keep track of the last pa range entered into the sorted table.*/
	for (pa_range_count=actual_pa_range_count; pa_range_count<num_valid_pa_ranges; pa_range_count++)
	{
	  if(count_pa_swp >= (RFGSM_NV_MAX_TX_SWPT_RANGES - 1))
	  {
		MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"switch_point index out of range %d",count_pa_swp); 
		break;
	  }

	  //if the predistortion switchpoint falls within one PA range, then 
	  // add this to the switchpoint to the list. Then insert the current 
	  // PA range should be added.
	  if ( (pred_swpts[predistort_count] < pa_range_swpts[pa_range_count]) &&
		   (pred_swpts[predistort_count] != 0xFFFF) )
	  {
		//insert the predistortion level followed by the pa gain range swpt to which
		//we just compared. 
		rfnv_tx_gain_range_info_ptr->switch_point[count_pa_swp]= pred_swpts[predistort_count];
		rfnv_tx_gain_range_info_ptr->predistortion[count_pa_swp]  = 0;
		rfnv_tx_gain_range_info_ptr->pa_state[count_pa_swp] = pa_state[pa_range_count];
		rfnv_tx_gain_range_info_ptr->amam_tbl_bank[count_pa_swp]= 0; 
		count_pa_swp++;
		//now add the pa range level:
		rfnv_tx_gain_range_info_ptr->switch_point[count_pa_swp] = pa_range_swpts[pa_range_count];
		rfnv_tx_gain_range_info_ptr->predistortion[count_pa_swp] = 1;
		rfnv_tx_gain_range_info_ptr->pa_state[count_pa_swp] = pa_state[pa_range_count];
		rfnv_tx_gain_range_info_ptr->amam_tbl_bank[count_pa_swp] = predistort_count;
		count_pa_swp++;
		//increase actual PA range count by one:
		actual_pa_range_count++;
		//make sure you do not insert this power range more than once:
		break;
	  } //if
	  //if it does not fall within one gain range, just add the switchpoint of the current
	  //pa range to the list.
	  else
	  {
		rfnv_tx_gain_range_info_ptr->switch_point[count_pa_swp] = pa_range_swpts[pa_range_count];
		rfnv_tx_gain_range_info_ptr->predistortion[count_pa_swp] = 0;
		rfnv_tx_gain_range_info_ptr->pa_state[count_pa_swp] = pa_state[pa_range_count];
		rfnv_tx_gain_range_info_ptr->amam_tbl_bank[count_pa_swp]= 0; 
		count_pa_swp++;
		//increase actual PA range count by one:
		actual_pa_range_count++;
	  }//else
	} //pa_range
  } //predistortion

  rfnv_tx_gain_range_info_ptr->num_gain_ranges = count_pa_swp;

  while (count_pa_swp > 0 && count_pa_swp < RFGSM_NV_MAX_TX_SWPT_RANGES)
  {
	rfnv_tx_gain_range_info_ptr->switch_point[count_pa_swp] = 0xFFFF;
	rfnv_tx_gain_range_info_ptr->predistortion[count_pa_swp] = 0; //no need for a predistortion.
	if(rfnv_tx_gain_range_info_ptr->num_gain_ranges <= RFGSM_NV_NUM_PA_RANGES)
	{
	  rfnv_tx_gain_range_info_ptr->pa_state[count_pa_swp] = pa_state[rfnv_tx_gain_range_info_ptr->num_gain_ranges-1];
	}
	rfnv_tx_gain_range_info_ptr->amam_tbl_bank[count_pa_swp] = 0; 
	count_pa_swp++;
  }

  //---------------------------- GSM Portion -------------------------------------------//
  /* Get Tx gain range info ptr*/
  rfnv_tx_gain_range_info_ptr = rfgsm_nv_get_tx_gain_info_ptr(rfm_dev, rfcom_band, RF_MOD_GMSK);

  if (rfnv_tx_gain_range_info_ptr == NULL)
  {
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_tx_gain_range_info_ptr ptr is NULL!", 0);
	return;
  }

  /* Re-initialize all swpts to 0xFFFF before populating with NV data */
  for (pa_gain_count = 0; pa_gain_count < RFGSM_NV_NUM_PA_RANGES; pa_gain_count++)
  {
    pa_range_swpts[pa_gain_count] = 0xFFFF;
  }

  /* Initialize swpts and add a very high switchpoint for the last gain range. 
  i.e above the last pa_range switchpoint from NV. 
  This switchpoint field for this is invalid.*/
  num_valid_pa_ranges = 0;

  if (pa_switch_point_data.gmsk_pa_swpt_r6_to_r7 != 0xFFFF)
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r6_to_r7;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_7;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.gmsk_pa_swpt_r5_to_r6 != 0xFFFF) && (pa_switch_point_data.gmsk_pa_swpt_r5_to_r6 != pa_switch_point_data.gmsk_pa_swpt_r6_to_r7))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r5_to_r6;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_6;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.gmsk_pa_swpt_r4_to_r5 != 0xFFFF) && (pa_switch_point_data.gmsk_pa_swpt_r4_to_r5 != pa_switch_point_data.gmsk_pa_swpt_r5_to_r6))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r4_to_r5;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_5;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.gmsk_pa_swpt_r3_to_r4 != 0xFFFF) && (pa_switch_point_data.gmsk_pa_swpt_r3_to_r4 != pa_switch_point_data.gmsk_pa_swpt_r4_to_r5))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r3_to_r4;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_4;
  num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.gmsk_pa_swpt_r2_to_r3 != 0xFFFF) && (pa_switch_point_data.gmsk_pa_swpt_r2_to_r3 != pa_switch_point_data.gmsk_pa_swpt_r3_to_r4))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r2_to_r3;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_3;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.gmsk_pa_swpt_r1_to_r2 != 0xFFFF) && (pa_switch_point_data.gmsk_pa_swpt_r1_to_r2 != pa_switch_point_data.gmsk_pa_swpt_r2_to_r3))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r1_to_r2;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_2;
	num_valid_pa_ranges++;
  }
  if ((pa_switch_point_data.gmsk_pa_swpt_r0_to_r1 != 0xFFFF) && (pa_switch_point_data.gmsk_pa_swpt_r0_to_r1 != pa_switch_point_data.gmsk_pa_swpt_r1_to_r2))
  {
	pa_range_swpts[num_valid_pa_ranges] = pa_switch_point_data.gmsk_pa_swpt_r0_to_r1;
	pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_1;
	num_valid_pa_ranges++;
  }
  pa_range_swpts[num_valid_pa_ranges] = 0xFFFF;
  pa_state[num_valid_pa_ranges] = RFGSM_PA_STATE_0;
  num_valid_pa_ranges++;

  count_pa_swp = 0;

  for (pa_range_count = 0; pa_range_count < num_valid_pa_ranges; pa_range_count++)
  {
	/* If this switchpoint is invalid skip to the next. */
	rfnv_tx_gain_range_info_ptr->switch_point[count_pa_swp] = pa_range_swpts[pa_range_count];
	rfnv_tx_gain_range_info_ptr->predistortion[count_pa_swp] = 0;
	rfnv_tx_gain_range_info_ptr->pa_state[count_pa_swp] = pa_state[pa_range_count];
	rfnv_tx_gain_range_info_ptr->amam_tbl_bank[count_pa_swp] = 0; 
	count_pa_swp++;

  }

  rfnv_tx_gain_range_info_ptr->num_gain_ranges = count_pa_swp;

  while (count_pa_swp > 0 && count_pa_swp < RFGSM_NV_MAX_TX_SWPT_RANGES )
  {
	rfnv_tx_gain_range_info_ptr->switch_point[count_pa_swp] = 0xFFFF;
	rfnv_tx_gain_range_info_ptr->predistortion[count_pa_swp] = 0; //no need for a predistortion.
	rfnv_tx_gain_range_info_ptr->pa_state[count_pa_swp] = pa_state[rfnv_tx_gain_range_info_ptr->num_gain_ranges-1];
	rfnv_tx_gain_range_info_ptr->amam_tbl_bank[count_pa_swp] = 0; 
	count_pa_swp++;
  }
}


/*===========================================================================

FUNCTION rfgsm_nv_precompute_tx_gain_ranges_info

DESCRIPTION
  This function pre-computes the TX switchpoint for linear PA, combining PA 
  switchpoints and pre-distortion switchpoints from NV.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_precompute_tx_gain_ranges_info(rfm_device_enum_type rfm_dev)
{
  
  /* Compute TX switch points for GSM850.
  */
  rfgsm_nv_precompute_tx_gain_ranges(rfm_dev, RFCOM_BAND_GSM850);


  /* Compute TX switch points for GSM900.
  */
  rfgsm_nv_precompute_tx_gain_ranges(rfm_dev, RFCOM_BAND_GSM900);


  /* Compute TX switch points for GSM1800.
  */
  rfgsm_nv_precompute_tx_gain_ranges(rfm_dev, RFCOM_BAND_GSM1800);


  /* Compute TX switch points for GSM1900.
  */
  rfgsm_nv_precompute_tx_gain_ranges(rfm_dev, RFCOM_BAND_GSM1900);

} /* end of rfgsm_nv_precompute_tx_switch_points */

/*===========================================================================

FUNCTION rfgsm_nv_init_amam_ampm_table

DESCRIPTION
  This function initializes the local AMPM table with corresponding values from
  RFNV. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_init_amam_ampm_table(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{

  int32 i =0;
  rfgsm_nv_band_data_type *rfgsm_nv_tbl = NULL;

  /*
    LLVM compiler changes:
    Unaligned means aligning to next byte. Equivalent to PACK
    Note: a member of an unaligned (packed) structure, must also be considered unaligned even though it was not defined so.  
  */

  gsm_amam_table_type_unalign *amam_master_table_ptr;
  gsm_ampm_table_type_unalign *ampm_master_table_ptr;

  rfgsm_nv_tbl = rfgsm_nv_get_tbl_ptr(rfm_dev, band, RFGSM_NV_TX);

  if (rfgsm_nv_tbl == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_tbl ptr is NULL!", 0);
    return;
  }

  /* Ptr to AMAM master table NV*/

  /*
  Prior to the LLVM changes, the LHS is a pointer to a non PACK or alligned structure but the right hand side 
  is a pointer to a PACK or unalligned structure (since, a member of an unaligned (packed) structure, 
  must also be considered unaligned even though it was not defined so. )

  This will cause incorrect loading of data from the address during runtime execution and hence needed to be corrected.
  */

  amam_master_table_ptr = &(rfgsm_nv_tbl->tx_cal_nv_data.amam_master_table); 
	
  /* Ptr to AMPM master table NV*/
  ampm_master_table_ptr = &(rfgsm_nv_tbl->tx_cal_nv_data.ampm_master_table);
  
  for (i = 2; i < RFGSM_AM_AM_TABLE_ENTRIES-1; i=i+2)
  {
 
    /* interpolate the amam tables from 128 NV entries  to 256 items */


  amam_master_table_ptr->amam_tbl[0].data[i] = (uint16)((amam_master_table_ptr->amam_tbl[0].data[i-1] + 
                                                                     amam_master_table_ptr->amam_tbl[0].data[i+1])/2);
  amam_master_table_ptr->amam_tbl[1].data[i] = (uint16)((amam_master_table_ptr->amam_tbl[1].data[i-1] + 
                                                                     amam_master_table_ptr->amam_tbl[1].data[i + 1])/2);
  amam_master_table_ptr->amam_tbl[2].data[i] = (uint16)((amam_master_table_ptr->amam_tbl[2].data[i-1] +
                                                                       amam_master_table_ptr->amam_tbl[2].data[i + 1])/2);

    /* interpolate the ampm tables from 128 NV entries  to 256 items */
  ampm_master_table_ptr->ampm_tbl[0].data[i] = ((int16)ampm_master_table_ptr->ampm_tbl[0].data[i-1] + 
                                                                 (int16)ampm_master_table_ptr->ampm_tbl[0].data[i+1])/2;
  ampm_master_table_ptr->ampm_tbl[1].data[i] = ((int16)ampm_master_table_ptr->ampm_tbl[1].data[i-1] + 
                                                                 (int16)ampm_master_table_ptr->ampm_tbl[1].data[i + 1])/2;
  ampm_master_table_ptr->ampm_tbl[2].data[i] = ((int16)ampm_master_table_ptr->ampm_tbl[2].data[i-1] +
                                                                 (int16)ampm_master_table_ptr->ampm_tbl[2].data[i + 1])/2;
  }

  /* Fill up amam_tbl[0] and ampm_tbl[0] by using the slope from the next 2 values and extrapolating down. */
  amam_master_table_ptr->amam_tbl[0].data[0] = (uint16)(amam_master_table_ptr->amam_tbl[0].data[1]*2 - 
                                                                     amam_master_table_ptr->amam_tbl[0].data[2]);

  amam_master_table_ptr->amam_tbl[1].data[0] = (uint16)(amam_master_table_ptr->amam_tbl[1].data[1]*2 - 
                                                                     amam_master_table_ptr->amam_tbl[1].data[2]);

  amam_master_table_ptr->amam_tbl[2].data[0] = (uint16)(amam_master_table_ptr->amam_tbl[2].data[1]*2 -
                                                                     amam_master_table_ptr->amam_tbl[2].data[2]);

  /* Fill up amam_tbl[0] and ampm_tbl[0] by using the slope from the next 2 values and extrapolating down. */
  ampm_master_table_ptr->ampm_tbl[0].data[0] = ((int16)ampm_master_table_ptr->ampm_tbl[0].data[1]*2 -  
                                                                   (int16)ampm_master_table_ptr->ampm_tbl[0].data[2]);

  ampm_master_table_ptr->ampm_tbl[1].data[0] = ((int16)ampm_master_table_ptr->ampm_tbl[1].data[1]*2 - 
                                                                   (int16)ampm_master_table_ptr->ampm_tbl[1].data[2]);

  ampm_master_table_ptr->ampm_tbl[2].data[0] = ((int16)ampm_master_table_ptr->ampm_tbl[2].data[1]*2 -
                                                                   (int16)ampm_master_table_ptr->ampm_tbl[2].data[2]);
}


/*===========================================================================

FUNCTION RFGSM_NV_GET_SPARE_NV

DESCRIPTION
  This function returns the spare_1 NV value.  The default value for this
  item is 0.  Thus, if the NV is not active, a 0 will be returned.

DEPENDENCIES
  None

RETURN VALUE
  spare_1 NV value.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfgsm_nv_get_spare(void)
{
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_data_buffer_ptr;
  uint32 debug_nv = 0;
  rfgsm_nv_path_type nv_path;

  for (nv_path = RF_GSM_NV_PATH_0; nv_path < RF_GSM_NV_PATH_NUM; nv_path++)
  {
    rfgsm_nv_cmn_data_buffer_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr;

    if (rfgsm_nv_cmn_data_buffer_ptr != NULL)
    {
      break;
    }
  }

  if (rfgsm_nv_cmn_data_buffer_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_get_spare, NULL ptr for all paths");
    return 0;
  }

  debug_nv = rfgsm_nv_cmn_data_buffer_ptr->gsm_debug_nv[0];

  return (debug_nv & 0x0000FFFF);
}


/*===========================================================================

FUNCTION RFGSM_DEBUG_NV_GET_WORD

DESCRIPTION
  This function returns the a word chunk of debug NV based on the index.  

DEPENDENCIES
  None

RETURN VALUE
  NV contents of element specified by index.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfgsm_debug_nv_get_word(rfm_device_enum_type rfm_dev,uint8 index)
{
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_data_buffer_ptr;
  uint32 debug_nv = 0;
  rfgsm_nv_path_type nv_path;
  uint16 size_debug_nv=0;
  uint8 num_elements;
  nv_path = rfgsm_nv_get_cmn_nv_container( rfm_dev);

  rfgsm_nv_cmn_data_buffer_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr;


  if (rfgsm_nv_cmn_data_buffer_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_debug_nv_get_word, NULL ptr %d", 0);
    return 0;
  }

  /*Total Size of Debug NV can only be a multiple of 32 */
  size_debug_nv = rfgsm_nv_cmn_data_buffer_ptr->size_gsm_debug_nv;


  num_elements = (size_debug_nv-(size_debug_nv%32))/32; 

  if (index < num_elements)
  {
    debug_nv = rfgsm_nv_cmn_data_buffer_ptr->gsm_debug_nv[index];
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Index :%d out of bounds", index);
    debug_nv = 0;
  }
 

  return (debug_nv );
}


/*===========================================================================

FUNCTION RFGSM_DEBUG_NV_GET_BITS

DESCRIPTION
  This function returns a bit or a set of bits (max 16) from the entire debug NV data.  
  'i' represents the bit  index number from which the extraction will begin, 'num_bits' 
  represents maximum number of bits (up to 16) that can beextracted from the data in GSM debug NV.
  rfm_dev denotes the RF device with which debug NV is associated.

DEPENDENCIES
  None

RETURN VALUE
  A 16 bit number containing the bits extracted.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfgsm_debug_nv_get_bits(rfm_device_enum_type rfm_dev,uint8 i, uint8 num_bits)
{
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_data_buffer_ptr;
  rfgsm_nv_path_type nv_path;
  uint16 size_debug_nv=0;
  uint8 num_elements,indx;
  uint16 ls_bit,last_bit,num_bits_rem;
  uint32 tmp1,tmp2;
  uint32 final_op = 0;
  uint32 bit_mask = 0x0000FFFF;  
  uint16 max_bits_copied = 16;
  uint32 debug_nv = 0;

  nv_path = rfgsm_nv_get_cmn_nv_container( rfm_dev);
  rfgsm_nv_cmn_data_buffer_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr;


  if (rfgsm_nv_cmn_data_buffer_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_debug_nv_get_bits, NULL ptr %d", 0);
    return 0;
  }
  /*Total Size of Debug NV can only be a multiple of 32 */
  size_debug_nv = rfgsm_nv_cmn_data_buffer_ptr->size_gsm_debug_nv;


  num_elements = (size_debug_nv-(size_debug_nv%32))/32; 

  /*truncating to multiple of 32*/
  size_debug_nv = num_elements * 32;

  
  for (indx=0;indx < num_elements;indx++)
  {
    /* Find which Debug NV element index i lies in. The element number is represented by indx*/
  	if (i < (indx+1)*32)
  	{
    	debug_nv = rfgsm_nv_cmn_data_buffer_ptr->gsm_debug_nv[indx];
      break;
  	}
  }


  //least significant bit index in the array element of index = 'indx'
  ls_bit = ((32*(indx+1))-1);

  // last bit index number of capture, i being the first
  last_bit = (i+num_bits-1);

  /*Ensure that bits to be read lie withing debug NV bounds*/
  if ( last_bit >= size_debug_nv)
  {

    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Bits to be extracted from index:%d are beyond the last bit idnex in bound: %d", i, last_bit);
    return 0;
  }


  /*If only one bit is to be read*/
  if(num_bits == 1)
  {
  	final_op = (debug_nv >> i)& 0x1;
  }
  else  /*if more than one bit is to be read starting from ith position */
  {

  	if( (num_bits > 1) && (num_bits <= 16))
  	{

      if (last_bit <= ls_bit)
      {
        /*if all the bits to be extracted exist in current NV element*/
        final_op = (debug_nv >> ( ls_bit - last_bit))& ( bit_mask >> (max_bits_copied - num_bits));

      }
      else
      {
        if ((indx+1)<num_elements)
        {
          debug_nv = rfgsm_nv_cmn_data_buffer_ptr->gsm_debug_nv[indx];

          // number of bits to extract from current element
          num_bits_rem =(ls_bit+1-i);
          tmp1= debug_nv  & ( bit_mask >> (max_bits_copied - num_bits_rem));
          debug_nv = rfgsm_nv_cmn_data_buffer_ptr->gsm_debug_nv[indx+1];
          
          // remaining number of bits to extract
          num_bits_rem = num_bits - num_bits_rem;
          tmp2= (debug_nv >> (32 - num_bits_rem )) & ( bit_mask >> (max_bits_copied - num_bits_rem));
          final_op = (tmp1 << (num_bits_rem)) | tmp2;
        }       
      }
  	}
   
  }

  return ((uint16)(final_op));
} 
/*-----------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION rfgsm_get_intf_debug_nv

DESCRIPTION
  This function returns the RFGSM interface debug NV item  

DEPENDENCIES
  None

RETURN VALUE
  NV contents

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfgsm_get_intf_debug_nv(rfm_device_enum_type rfm_dev, uint8 index )
{
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_data_buffer_ptr;
  rfgsm_nv_path_type nv_path = rfgsm_nv_get_cmn_nv_container( rfm_dev );

  rfgsm_nv_cmn_data_buffer_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr;

  if( rfgsm_nv_cmn_data_buffer_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_get_intf_debug_nv, NULL ptr %d", 0);
    return 0;
  }

  /* Two 32-bit words so index should only be 0 or 1 */
  if ( index >= GSM_DEBUG_NV_MAX_SIZE ) 
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_get_intf_debug_nv, invalid index %d", index);
     return 0;
  }

  return rfgsm_nv_cmn_data_buffer_ptr->gsm_rf_reserved_nv[index];
}

/*!
  @brief
  Return GSM850 RF NV Table Pointer
 
  @param nv_path
  NV path for which items are requested
 
  @return
  GSM850 RF NV Table Pointer
*/


rfgsm_nv_band_items_map_type*
rfgsm_nv_get_band_nv_table
(
  rfgsm_nv_path_type nv_path
)
{
  return ( &rfgsm_nv_band_items_map[nv_path] );
} /* rfgsm_nv_get_band_specific_static_nv_table */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return common RF  NV Table Pointer
 
  @param nv_path
  NV path for which items are requested
 
  @return
  Common RF Static NV Table Pointer
*/


rfgsm_nv_cmn_items_map_type*
rfgsm_nv_get_cmn_nv_table
(
  rfgsm_nv_path_type nv_path
)
{
  return ( &rfgsm_nv_cmn_items_map[nv_path] );
} /* rfgsm_nv_get_cmn_nv_table */

/*----------------------------------------------------------------------------*/
/*!
@brief
  This function allocates the buffer for Rx static NVs

  @param 
     path - path for which NV buffer allocation is being done
     band -band for which NV buffer allocation is being done    
 
  @return
  Rx Static NV Table Pointer
*/
static
gsm_rx_static_nv_type*
rfgsm_rx_static_nv_alloc
(
  rfgsm_nv_path_type path,
  rfgsm_nv_supported_type  band
)
{
  if ( rfgsm_nv_common_config_storage.rx_buffer_count < RF_GSM_NV_MAX_STRUCTURES )
  {
    rfgsm_nv_common_config_storage.rx_static_nv_data_buffers[rfgsm_nv_common_config_storage.rx_buffer_count] = modem_mem_alloc( sizeof(gsm_rx_static_nv_type), MODEM_MEM_CLIENT_RFA );

    if ( rfgsm_nv_common_config_storage.rx_static_nv_data_buffers[rfgsm_nv_common_config_storage.rx_buffer_count]
         == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_alloc_common_config: Could not allocate NV memory on heap", 0);	  
      return NULL;
    }
    else
    {
      return rfgsm_nv_common_config_storage.rx_static_nv_data_buffers[rfgsm_nv_common_config_storage.rx_buffer_count++];
    }
  }
  else
  {
    return NULL;
  }
} /* rfgsm_rx_static_nv_alloc() */

/*----------------------------------------------------------------------------*/
/*!
@brief
  This function allocates the buffer for Tx static NVs

  @param 
     path - path for which NV buffer allocation is being done
     band -band for which NV buffer allocation is being done    
 
  @return
  Rx Static NV Table Pointer
*/

static
gsm_tx_static_nv_type*
rfgsm_tx_static_nv_alloc
(
  rfgsm_nv_path_type path,
  rfgsm_nv_supported_type  band
)
{
  if ( rfgsm_nv_common_config_storage.tx_buffer_count < RF_GSM_NV_MAX_STRUCTURES )
  {
 
    rfgsm_nv_common_config_storage.tx_static_nv_data_buffers[rfgsm_nv_common_config_storage.tx_buffer_count] = modem_mem_alloc( sizeof(gsm_tx_static_nv_type), MODEM_MEM_CLIENT_RFA );

    if ( rfgsm_nv_common_config_storage.tx_static_nv_data_buffers[rfgsm_nv_common_config_storage.tx_buffer_count]
         == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_alloc_common_config: Could not allocate NV memory on heap", 0);	  
      return NULL;
    }
    else
    {
      return rfgsm_nv_common_config_storage.tx_static_nv_data_buffers[rfgsm_nv_common_config_storage.tx_buffer_count++];
    }
  }
  else
  {
    return NULL;
  }
} /* rfgsm_tx_static_nv_alloc() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate a new NV data structure for storing the GSM configuration
  items for a band and path.

  @details
  The constant data member pointing to the GSM configuration
  data structure is initialized with the constructor parameter
  rf_gsm_config_ptr.
 
  Allocation is not thread-safe, so the entire configuration loading
  process must insure that it is executed atomically.
 
  @param
  rx_static_nv_ptr
     A a pre-allocated memory pointer to the data structure for GSM Rx static NVs.

  tx_static_nv_ptr
     A a pre-allocated memory pointer to the data structure for GSM Tx static NVs.

  
  @return
  A pointer to a structure that is now dedicated to this path and band
  for storing configuration data.  If no space is available, NULL is
  returned.
*/

static
rfgsm_nv_band_data_type*
rfgsm_nv_alloc_config
(
  gsm_rx_static_nv_type* rx_static_nv_ptr, gsm_tx_static_nv_type* tx_static_nv_ptr
)
{
  rfgsm_nv_band_data_type* rfgsm_nv_band_data_buffer;
  gsm_rx_static_nv_type** rx_config_ptr;
  gsm_tx_static_nv_type** tx_config_ptr;

  if ( rf_gsm_nv_config_storage.allocated < RF_GSM_NV_MAX_STRUCTURES )
  {
    /* Allocate the NV data buffer*/
    rfgsm_nv_band_data_buffer = (rfgsm_nv_band_data_type*)modem_mem_alloc( sizeof(rfgsm_nv_band_data_type), MODEM_MEM_CLIENT_RFA );

    if ( rfgsm_nv_band_data_buffer == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_alloc_config: Could not allocate NV memory on heap", 0);
      return NULL;
    }
    else
    {
      memset((void *)rfgsm_nv_band_data_buffer, 0, sizeof(rfgsm_nv_band_data_type));
	  
      /* if main buffer allocation succeeded, hook static NV ptrs to it*/
      rx_config_ptr = (gsm_rx_static_nv_type**)
        &(rfgsm_nv_band_data_buffer->rx_static_nv_data_ptr);
      *rx_config_ptr = rx_static_nv_ptr;

      tx_config_ptr = (gsm_tx_static_nv_type**)
        &(rfgsm_nv_band_data_buffer->tx_static_nv_data_ptr);
      *tx_config_ptr = tx_static_nv_ptr;
	  
      rf_gsm_nv_config_storage.buffers[rf_gsm_nv_config_storage.allocated] = rfgsm_nv_band_data_buffer;
      return rf_gsm_nv_config_storage.buffers[rf_gsm_nv_config_storage.allocated++];
    }
  }
  else
  {
    return NULL;
  }
} /* rfgsm_nv_alloc_config() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the NV storage for the GSM data for a
  given path and band.

  @details
  Constructs the GSM data structures necessary to store the data
  that will be loaded from NV.  Also populates the correct entry in
  the path+band index for rapid lookup of this new data structure
  when needed.
 
  @param path
  RF NV path that is being initialized
  
  @param band
  RF band that is being initialized
  
  @return
  A pointer to the index entry for the newly allocated configuation
  data structure, or NULL if an error occured.

  @retval NULL
  No storage was available for the new path and band.
*/


static
rfgsm_nv_band_node_type*
rfgsm_nv_alloc_band_nv_buf
(
  rfm_device_enum_type rfm_dev,
  rfgsm_nv_path_type path,
  rfgsm_nv_supported_type  band
)
{
  gsm_rx_static_nv_type*  rfgsm_rx_static_nv_ptr = NULL;
  gsm_tx_static_nv_type*  rfgsm_tx_static_nv_ptr = NULL;
  rfgsm_nv_band_data_type*   rf_gsm_config_ptr;
  rfc_cfg_info_type cfg_struct;

  /* Allocate memory for Rx related Static NVs data for a given GSM band and path */
  rfgsm_rx_static_nv_ptr =  rfgsm_rx_static_nv_alloc(path, band);

  /* Check if memory allocation was successful, otherwise return a failure*/
  if ( rfgsm_rx_static_nv_ptr == NULL )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to allocate Rx static NV structure for NV path %d and band %d", path, band);	
    nv_alloc_info[rfm_dev][path][band].static_nv_alloc_valid[RFC_CONFIG_RX] = FALSE;	
    return NULL;
  }

  nv_alloc_info[rfm_dev][path][band].static_nv_alloc_valid[RFC_CONFIG_RX] = TRUE;

    /* Allocate memory for Tx related Static NVs data for a given GSM band and path */
  if(rfc_gsm_get_cfg_info( rfm_dev, (rfcom_gsm_band_type)band, RFC_CONFIG_TX, &cfg_struct, 0))
  {
      rfgsm_tx_static_nv_ptr =  rfgsm_tx_static_nv_alloc(path, band);
  }


  /* Check if memory allocation was successful, otherwise return a failure*/
  if ( rfgsm_tx_static_nv_ptr == NULL )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to allocate Tx static NV structure for NV path %d and band %d", path, band);
    nv_alloc_info[rfm_dev][path][band].static_nv_alloc_valid[RFC_CONFIG_TX] = FALSE;	
//    return NULL;
  }


  nv_alloc_info[rfm_dev][path][band].static_nv_alloc_valid[RFC_CONFIG_TX] = TRUE;

  if(!rfc_gsm_get_cfg_info( rfm_dev, (rfcom_gsm_band_type)band, RFC_CONFIG_TX, &cfg_struct, 0))
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Tx chain is not present on device %d and band %d", rfm_dev, band);

      /*debug hook*/
      nv_alloc_info[rfm_dev][path][band].tx_supported = 2; // Means not supported

      /* Allocate memory to holds all the NV data for a given GSM band and path 
         memory for Rx and Tx static NV data are alreadt allocated above, just pass that ptr so that it can be linked to main NV memory structure*/
      rf_gsm_config_ptr =  rfgsm_nv_alloc_config( rfgsm_rx_static_nv_ptr, rfgsm_tx_static_nv_ptr );

      /* Check if memory allocation was successful, otherwise return a failure*/  
      if ( rf_gsm_config_ptr == NULL )
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to allocate GSM NV structure for NV path %d and band %d", path, band);

         nv_alloc_info[rfm_dev][path][band].nv_data_alloc_valid = FALSE;
	       return NULL;
       }
      else
       {
  
         rf_gsm_config_ptr->band = band;
         rf_gsm_config_ptr->path = path;

         if(rfgsm_nv_data[path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr == NULL)
         {
            rfgsm_nv_data[path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr= rf_gsm_config_ptr;
            rfgsm_nv_data[path].rfgsm_nv_band_node[band].data_valid = FALSE;		
         }
         
         nv_alloc_info[rfm_dev][path][band].nv_data_alloc_valid = TRUE;
         return &rfgsm_nv_data[path].rfgsm_nv_band_node[band];     
       }
    }


  nv_alloc_info[rfm_dev][path][band].tx_supported = 1; // 1 means supported

  
  /* Allocate memory to holds all the NV data for a given GSM band and path 
    memory for Rx and Tx static NV data are alreadt allocated above, just pass that ptr so that it can be linked to main NV memory structure*/
  rf_gsm_config_ptr =  rfgsm_nv_alloc_config( rfgsm_rx_static_nv_ptr, rfgsm_tx_static_nv_ptr );

  /* Check if memory allocation was successful, otherwise return a failure*/
  if ( rf_gsm_config_ptr == NULL )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed to allocate GSM NV structure for NV path %d and band %d", path, band);

    nv_alloc_info[rfm_dev][path][band].nv_data_alloc_valid = FALSE;
	  return NULL;
  }
  else
  {
  
  rf_gsm_config_ptr->band = band;
  rf_gsm_config_ptr->path = path;

  if(rfgsm_nv_data[path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr == NULL)
  {
    rfgsm_nv_data[path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr= rf_gsm_config_ptr;
    rfgsm_nv_data[path].rfgsm_nv_band_node[band].data_valid = FALSE;	
  } 	

  nv_alloc_info[rfm_dev][path][band].nv_data_alloc_valid = TRUE;
  return &rfgsm_nv_data[path].rfgsm_nv_band_node[band];
  }
  
} /* rfgsm_nv_alloc_band_nv_buf() */

static
rfgsm_nv_cmn_node_type*
rfgsm_nv_alloc_cmn_nv_buf
(
  rfgsm_nv_path_type path
)
{
  rfgsm_nv_cmn_data_type *rf_gsm_nv_cmn_data;

  rf_gsm_nv_cmn_data = (rfgsm_nv_cmn_data_type *)modem_mem_alloc( sizeof(rfgsm_nv_cmn_data_type), MODEM_MEM_CLIENT_RFA );

  if ( rf_gsm_nv_cmn_data == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_alloc_cmn_nv_buf: Could not allocate NV memory on heap", 0);
    return NULL;
  }
  /* Hook the assigned memory for cmn NV strcuture to main structure */
  rfgsm_nv_data[path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr = rf_gsm_nv_cmn_data;

  return &rfgsm_nv_data[path].rfgsm_nv_cmn_node;
  
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx timing related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the tx timing data for that band.

  @param gsm_tx_timing_data_type
  The gsm_tx_timing_data_type NV data structure that the tx timing data has to be loaded into

  @param tx_timing_data_item
  The NV item that contains the tx timing value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the tx timing value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/

boolean
rfgsm_nv_load_tx_timing
(
  gsm_tx_timing_data_type* static_nv_struct,
  rfnv_item_id_enum_type tx_timing_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     tx_timing_data_nv_status;
  rfnv_item_size_type   tx_timing_data_returned_size =
                               sizeof(rfnv_item_type);
//  rfnv_tx_timing_type   *tx_timing_nv_ptr;
  rfnv_item_type         tx_timing_data_buffer;

   /*Adding a offset to RF NV id. tx_timing_data_item = tx_timing_data_item + band*/
  tx_timing_data_item = tx_timing_data_item + band;
  
  tx_timing_data_nv_status =
    rfcommon_rfnv_get_item( tx_timing_data_item,
                            &tx_timing_data_buffer,
                            &tx_timing_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( tx_timing_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &tx_timing_data_buffer, sizeof(gsm_tx_timing_data_type));
  } /* if ( tx_timing_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    *static_nv_struct = gsm_tx_timing_data_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_timing: Initialzed the tx timing data to default", 0);	
    return TRUE;
  }

  return TRUE;
} /* rfgsm_nv_load_tx_timing() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the PA switch point related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the tx timing data for that band.

  @param gsm_extended_pa_switchpoint_type
  The gsm_extended_pa_switchpoint_type NV data structure where the PA switch point data has to be loaded into

  @param pa_swpt_data_item
  The NV item that contains the PA switch point value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the pa swpt value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_pa_swpt
(
  gsm_extended_pa_switchpoint_type* static_nv_struct,
  rfnv_item_id_enum_type pa_swpt_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pa_swpt_data_nv_status;
  rfnv_item_type        pa_swpt_data_buffer;
  rfnv_item_size_type   pa_swpt_data_returned_size =
                                sizeof(rfnv_item_type);
  /*Adding a offset to RF NV id. pa_swpt_data_item = pa_swpt_data_item + band*/
  pa_swpt_data_item = pa_swpt_data_item + band;
  
  pa_swpt_data_nv_status =
    rfcommon_rfnv_get_item( pa_swpt_data_item,
                            &pa_swpt_data_buffer,
                            &pa_swpt_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &pa_swpt_data_buffer, sizeof(gsm_extended_pa_switchpoint_type));
  } /* if (pa_swpt_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    *static_nv_struct = gsm_pa_switchpoint_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_pa_swpt: Initialzed the PA swpt data to default", 0);	
    return TRUE;
  } /* else: if (pa_swpt_data_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_pa_swpt() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the PA range map NV data for any band.

  @details
  The function will read the PA range map NV item from EFS.   

  @param  pa_range_map_item
  The NV item to be read.

  @param band
  The NV to be read for this band.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE regardless if the NV load is correct or not.
*/


boolean
rfgsm_nv_load_pa_range_map
(
  uint32* static_nv_struct,
  rfnv_item_id_enum_type pa_range_map_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pa_range_map_nv_status;
  rfnv_item_type        pa_range_map_buffer;
  rfnv_item_size_type   pa_range_map_returned_size =
                                sizeof(rfnv_item_type);
  /*Adding a offset to RF NV id. pa_range_map_item = pa_range_map_item + band*/
  pa_range_map_item = pa_range_map_item + band;
  
  pa_range_map_nv_status =
    rfcommon_rfnv_get_item( pa_range_map_item,
                            &pa_range_map_buffer,
                            &pa_range_map_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_range_map_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &pa_range_map_buffer, sizeof(uint32));
  } /* if (pa_range_map_nv_status == NV_DONE_S ) */
  else
  {
    *static_nv_struct = 0xFFFFFFFF;

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_pa_range_map: Initialzed the PA range map to default", 0);	
  } /* else: if (pa_range_map_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_pa_range_map() */



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the LNA switch point related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the tx timing data for that band.

  @param gsm_pa_switchpoint_type
  The gsm_lna_switchpoint_type NV data structure where the PA switch point data has to be loaded into

  @param lna_swpt_data_item
  The NV item that contains the LNA switch point value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the lna swpt value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_lna_swpt
(
  gsm_lna_switchpoint_type* static_nv_struct,
  rfnv_item_id_enum_type lna_swpt_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     lna_swpt_data_nv_status;
  rfnv_item_type        lna_swpt_data_buffer;
  rfnv_item_size_type   lna_swpt_data_returned_size =
                                sizeof(rfnv_item_type);
  //rfnv_lna_swpt_type   *lna_swpt_nv_ptr;

  /*Adding a offset to RF NV id. lna_swpt_data_item = lna_swpt_data_item + band*/
  lna_swpt_data_item = lna_swpt_data_item + band;
  
  lna_swpt_data_nv_status =
    rfcommon_rfnv_get_item( lna_swpt_data_item,
                            &lna_swpt_data_buffer,
                            &lna_swpt_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( lna_swpt_data_nv_status == NV_DONE_S )
  {
    memscpy( static_nv_struct, sizeof(*static_nv_struct), &lna_swpt_data_buffer, sizeof(gsm_lna_switchpoint_type));
  } 
  else
  {
    *static_nv_struct = lna_switch_point_default;
    //memcpy( static_nv_struct, &lna_switch_point_default, sizeof(gsm_lna_switchpoint_type));
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_lna_swpt: Initialzed the LNA swpt data to default", 0);	
    return TRUE;
  } /* else: if (lna_swpt_data_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_lna_swpt() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the LNA switch point related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the tx timing data for that band.

  @param gsm_pa_switchpoint_type
  The gsm_lna_switchpoint_type NV data structure where the PA switch point data has to be loaded into

  @param lna_swpt_data_item
  The NV item that contains the LNA switch point value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the lna swpt value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_lna_enh_swpt
(
  gsm_lna_switchpoint_type* static_nv_struct,
  rfnv_item_id_enum_type lna_enh_swpt_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     lna_enh_swpt_data_nv_status;
  rfnv_item_type        lna_enh_swpt_data_buffer;
  rfnv_item_size_type   lna_enh_swpt_data_returned_size =
                                sizeof(rfnv_item_type);
  /*Adding a offset to RF NV id. lna_enh_swpt_data_item = lna_enh_swpt_data_item + band*/
  lna_enh_swpt_data_item = lna_enh_swpt_data_item + band;
  
  lna_enh_swpt_data_nv_status =
    rfcommon_rfnv_get_item( lna_enh_swpt_data_item,
                            &lna_enh_swpt_data_buffer,
                            &lna_enh_swpt_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( lna_enh_swpt_data_nv_status == NV_DONE_S )
  {
    memscpy( static_nv_struct, sizeof(*static_nv_struct), &lna_enh_swpt_data_buffer, sizeof(gsm_lna_switchpoint_type));
  } 
  else
  {
    *static_nv_struct = lna_enh_switch_point_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_lna_enh_swpt: Initialzed the LNA enh swpt data to default", 0);	
    return TRUE;
  } /* else: if (lna_enh_swpt_data_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_lna_enh_swpt() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the KV CAL related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the KV CAL data for that band.

  @param kv_cal_type
  The kv_cal_type NV data structure where the KV CAL data has to be loaded into

  @param kv_cal_data_item
  The NV item that contains the KV CAL value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the kv cal value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_kv_cal
(
  kv_cal_type* static_nv_struct,
  rfnv_item_id_enum_type kv_cal_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     kv_cal_data_nv_status;
  rfnv_item_type        kv_cal_data_buffer;
  rfnv_item_size_type   kv_cal_data_returned_size =
                                sizeof(rfnv_item_type);
  //rfnv_kv_cal_type   *kv_cal_nv_ptr;

  /*Adding a offset to RF NV id. kv_cal_data_item = kv_cal_data_item + band*/
  kv_cal_data_item = kv_cal_data_item + band;
  
  kv_cal_data_nv_status =
    rfcommon_rfnv_get_item( kv_cal_data_item,
                            &kv_cal_data_buffer,
                            &kv_cal_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( kv_cal_data_nv_status == NV_DONE_S )
  {
    memscpy( static_nv_struct, sizeof(*static_nv_struct), &kv_cal_data_buffer, sizeof(kv_cal_type));
  } /* if (kv_cal_data_nv_status == NV_DONE_S ) */
  else
  {
    static_nv_struct->kv_cal_value = 0;
	static_nv_struct->kv_cal_low_chan = 0;
	static_nv_struct->kv_cal_high_chan = 0;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_kv_cal: Initialzed the KV CAL data to default", 0);	
    return TRUE;
  } /* else: if (kv_cal_data_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_kv_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the TX_GTR_THRESH related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the TX_GTR_THRESH data for that band.

  @param gsm_tx_gtr_thresh_type
  The gsm_tx_gtr_thresh_type NV data structure where the TX_GTR_THRESH data has to be loaded into

  @param gtr_thresh_data_item
  The NV item that contains the TX_GTR_THRESH value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the TX_GTR_THRESH value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_tx_gtr_thresh
(
  gsm_tx_gtr_thresh_type* static_nv_struct,
  rfnv_item_id_enum_type gtr_thresh_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     gtr_thresh_data_nv_status;
  rfnv_item_type        gtr_thresh_data_buffer;
  rfnv_item_size_type   gtr_thresh_data_returned_size =
                                sizeof(rfnv_item_type);
  //rfnv_gtr_thresh_type   *gtr_thresh_nv_ptr;

  /*Adding a offset to RF NV id. gtr_thresh_data_item = gtr_thresh_data_item + band */
  gtr_thresh_data_item = gtr_thresh_data_item + band;
  
  gtr_thresh_data_nv_status =
    rfcommon_rfnv_get_item( gtr_thresh_data_item,
                            &gtr_thresh_data_buffer,
                            &gtr_thresh_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( gtr_thresh_data_nv_status == NV_DONE_S )
  {
    memscpy( static_nv_struct, sizeof(*static_nv_struct), &gtr_thresh_data_buffer, sizeof(gsm_tx_gtr_thresh_type));
  } /* if (gtr_thresh_data_nv_status == NV_DONE_S ) */
  else
  {
    static_nv_struct->enable= 0;
	static_nv_struct->rise_thresh= 0;
	static_nv_struct->fall_thresh= 0;
	static_nv_struct->adv_time= 0;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_gtr_thresh: Initialzed the TX_GTR_THRESH data to default", 0);	
    return TRUE;
  } /* else: if (gtr_thresh_data_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_tx_gtr_thresh() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the POLAR_PATH_DELAY related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the POLAR_PATH_DELAY data for that band.

  @param int16
  The polar_path_delay NV data structure where the POLAR_PATH_DELAY data has to be loaded into

  @param polar_path_data_item
  The NV item that contains the POLAR_PATH_DELAY value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the POLAR_PATH_DELAY value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_polar_path_delay
(
  int16* static_nv_struct,
  rfnv_item_id_enum_type polar_path_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     polar_path_data_nv_status;
  rfnv_item_type        polar_path_data_buffer;
  rfnv_item_size_type   polar_path_data_returned_size =
                                sizeof(rfnv_item_type);
//  int16   *polar_path_nv_ptr;

  /*Adding a offset to RF NV id.polar_path_data_item = polar_path_data_item + band */
  polar_path_data_item = polar_path_data_item + band;
  
  polar_path_data_nv_status =
    rfcommon_rfnv_get_item( polar_path_data_item,
                            &polar_path_data_buffer,
                            &polar_path_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( polar_path_data_nv_status == NV_DONE_S )
  {
    memscpy( static_nv_struct, sizeof(*static_nv_struct), &polar_path_data_buffer, sizeof(int16));
  } /* if (polar_path_data_nv_status == NV_DONE_S ) */
  else
  {
    *static_nv_struct = 0;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_polar_path_delay: Initialzed the POLAR_PATH_DELAY data to default", 0);	
    return TRUE;
  } /* else: if (polar_path_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_polar_path_delay() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the VBATT related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the VBATT data for that band.

  @param gsm_vbatt_type
  The gsm_vbatt_type NV data structure where the VBATT data has to be loaded into

  @param vbatt_data_item
  The NV item that contains the VBATT value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the VBATT value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_vbatt_data
(
  gsm_vbatt_type* static_nv_struct,
  rfnv_item_id_enum_type vbatt_data_item,
  
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     vbatt_data_nv_status;
  rfnv_item_type        vbatt_data_buffer;
  rfnv_item_size_type   vbatt_data_returned_size =
                                sizeof(rfnv_item_type);

  /*Adding a offset to RF NV id. vbatt_data_item = vbatt_data_item + band*/
  vbatt_data_item = vbatt_data_item + band;
  
  vbatt_data_nv_status =
    rfcommon_rfnv_get_item( vbatt_data_item,
                            &vbatt_data_buffer,
                            &vbatt_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( vbatt_data_nv_status == NV_DONE_S )
  {
    
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &vbatt_data_buffer, sizeof(gsm_vbatt_type));
	
  } /* if (vbatt_data_nv_status == NV_DONE_S ) */
  else
  {
    /*initialize the structure with default values accordlingly*/
    *static_nv_struct = vbatt_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_vbatt_data: Initialzed the VBATT data to default", 0);	
    return TRUE;
  } /* else: if (vbatt_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_vbatt_data() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the ENV GAIN related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the ENV GAIN data for that band.

  @param gsm_env_gain_type
  The gsm_vbatt_type NV data structure where the ENV GAIN data has to be loaded into

  @param env_gain_data_item
  The NV item that contains the ENV GAIN  value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the ENV GAIN value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_env_gain
(
  gsm_env_gain_type* static_nv_struct,
  rfnv_item_id_enum_type env_gain_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     env_gain_data_nv_status;
  rfnv_item_type        env_gain_data_buffer;
  rfnv_item_size_type   env_gain_data_returned_size =
                                sizeof(rfnv_item_type);
 
  /*Adding a offset to RF NV id. env_gain_data_item = env_gain_data_item + band */
  env_gain_data_item = env_gain_data_item + band;
  
  env_gain_data_nv_status =
    rfcommon_rfnv_get_item( env_gain_data_item,
                            &env_gain_data_buffer,
                            &env_gain_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( env_gain_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &env_gain_data_buffer, sizeof(gsm_env_gain_type));
  } /* if (env_gain_data_nv_status == NV_DONE_S ) */
  else
  {
    /*initialize the structure with default values accordlingly*/
    *static_nv_struct = env_gain_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_env_gain: Initialzed the ENV GAIN data to default", 0);	
    return TRUE;
  } /* else: if (env_gain_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_env_gain() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the AMAM TEMP COMP related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the AMAM TEMP COMP data for that band.

  @param amam_temp_comp
  The amam_temp_comp NV data structure where the AMAM TEMP COMP data has to be loaded into

  @param amam_temp_data_item
  The NV item that contains the AMAM TEMP COMP  value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the AMAM TEMP COMPvalue is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_amam_temp_comp
(
  amam_temp_comp* static_nv_struct,
  rfnv_item_id_enum_type amam_temp_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     amam_temp_data_nv_status;
  rfnv_item_type        amam_temp_data_buffer;
  rfnv_item_size_type   amam_temp_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id.  amam_temp_data_item = amam_temp_data_item + band*/
  amam_temp_data_item = amam_temp_data_item + band;
  
  amam_temp_data_nv_status =
    rfcommon_rfnv_get_item( amam_temp_data_item,
                            &amam_temp_data_buffer,
                            &amam_temp_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( amam_temp_data_nv_status == NV_DONE_S )
  {
    
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &amam_temp_data_buffer, sizeof(amam_temp_comp));
	
  } /* if (amam_temp_data_nv_status == NV_DONE_S ) */
  else
  {
    /*initialize the structure with default values accordlingly*/
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &gsm_amam_temp_comp_default, sizeof(amam_temp_comp));
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_amam_temp: Initialzed the AMAM TEMP COMP data to default", 0);	
    return TRUE;
  } /* else: if (amam_temp_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_amam_temp() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the TEMP COMP PCL related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the TEMP COMP PCL data for that band.

  @param gsm_temp_comp_data_type
  The gsm_temp_comp_pcl_type NV data structure where the TEMP COMP PCL data has to be loaded into

  @param temp_comp_data_item
  The NV item that contains the TEMP COMP PCL value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the TEMP COMP PCL value is loaded.
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_temp_comp_data
(
  gsm_temp_comp_data_type* static_nv_struct,
  rfnv_item_id_enum_type temp_comp_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     temp_comp_data_nv_status;
  rfnv_item_type        temp_comp_data_buffer;
  rfnv_item_size_type   temp_comp_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id. temp_comp_data_item = temp_comp_data_item + band */
  temp_comp_data_item = temp_comp_data_item + band;
  
  temp_comp_data_nv_status =
    rfcommon_rfnv_get_item( temp_comp_data_item,
                            &temp_comp_data_buffer,
                            &temp_comp_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( temp_comp_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &temp_comp_data_buffer, sizeof(gsm_temp_comp_data_type));
  } /* if (temp_comp_data_nv_status == NV_DONE_S ) */
  else
  {
    /* Clear the temp comp data to ensure no garbage values are used */
    memset( static_nv_struct, 0, sizeof(gsm_temp_comp_data_type) );

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_temp_comp_data: Initialzed the TEMP COMP PCL data to default", 0);	
    return TRUE;
  } /* else: if (temp_comp_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_temp_comp_data() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the TEMP COMP PCL related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the TEMP COMP PCL data for that band.

  @param gsm_temp_comp_data_type
  The gsm_temp_comp_pcl_type NV data structure where the TEMP COMP PCL data has to be loaded into

  @param temp_comp_data_item
  The NV item that contains the TEMP COMP PCL value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the TEMP COMP PCL value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_enh_temp_comp_data
(
  gsm_enh_temp_comp_data_type* static_nv_struct,
  rfnv_item_id_enum_type enh_temp_comp_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     enh_temp_comp_data_nv_status;
  rfnv_item_type        enh_temp_comp_data_buffer;
  rfnv_item_size_type   enh_temp_comp_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id. temp_comp_data_item = temp_comp_data_item + band */
  enh_temp_comp_data_item = enh_temp_comp_data_item + band;
  
  enh_temp_comp_data_nv_status =
    rfcommon_rfnv_get_item( enh_temp_comp_data_item,
                            &enh_temp_comp_data_buffer,
                            &enh_temp_comp_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( enh_temp_comp_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &enh_temp_comp_data_buffer, sizeof(gsm_enh_temp_comp_data_type));
  } /* if (temp_comp_data_nv_status == NV_DONE_S ) */
  else
  {
    /* Clear the temp comp data to ensure no garbage values are used */
    memset( static_nv_struct, 0, sizeof(gsm_enh_temp_comp_data_type) );

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_temp_comp_data: Initialzed the TEMP COMP PCL data to default", 0);	
    return TRUE;
  } /* else: if (temp_comp_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_temp_comp_data() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the _LINEAR_TX_GAIN_PARAM related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the _LINEAR_TX_GAIN_PARAM data for that band.

  @param gsm_linear_tx_gain_param_type
  The gsm_linear_tx_gain_param_type NV data structure where the _LINEAR_TX_GAIN_PARAM data has to be loaded into

  @param tx_gain_data_item
  The NV item that contains the _LINEAR_TX_GAIN_PARAM value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the _LINEAR_TX_GAIN_PARAM value is loaded,
  FALSE if NV did not load, and some more action is required by caller.
*/


boolean
rfgsm_nv_load_tx_gain_param
(
  gsm_linear_tx_gain_param_type* static_nv_struct,
  rfnv_item_id_enum_type tx_gain_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     tx_gain_data_nv_status;
  rfnv_item_type        tx_gain_data_buffer;
  rfnv_item_size_type   tx_gain_data_returned_size =
                                sizeof(rfnv_item_type);
    
  /*Adding a offset to RF NV id.  tx_gain_data_item = tx_gain_data_item + band*/
  tx_gain_data_item = tx_gain_data_item + band;
  
  tx_gain_data_nv_status =
    rfcommon_rfnv_get_item( tx_gain_data_item,
                            &tx_gain_data_buffer,
                            &tx_gain_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( tx_gain_data_nv_status == NV_DONE_S )
  {
    
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &tx_gain_data_buffer, sizeof(gsm_linear_tx_gain_param_type));

    /*Scale the NV value for GSM and edge gain by 16*/
    static_nv_struct->gsm_linear_tx_gain_val = (static_nv_struct->gsm_linear_tx_gain_val)<<4;
    static_nv_struct->edge_linear_tx_gain_val = (static_nv_struct->edge_linear_tx_gain_val)<<4;	
  } /* if (tx_gain_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    *static_nv_struct = linear_tx_gain_param_default;

    /*Scale the NV value for GSM and edge gain by 16*/
    static_nv_struct->gsm_linear_tx_gain_val = (static_nv_struct->gsm_linear_tx_gain_val)<<4;
    static_nv_struct->edge_linear_tx_gain_val = (static_nv_struct->edge_linear_tx_gain_val)<<4;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_gain_param: Initialzed the LINEAR TX GAIN_PARAM data to default", 0);	
    return TRUE;
  } /* else: if (tx_gain_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_tx_gain_param() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the MULTISLOT_MAX_TX_PWR related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the MULTISLOT_MAX_TX_PWR data for that band.

  @param gsm_multislot_max_tx_pwr_type
  The gsm_multislot_tx_pwr_type NV data structure where the MULTISLOT_TX_PWR data has to be loaded into

  @param tx_pwr_data_item
  The NV item that contains the MULTISLOT_MAX_TX_PWR value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the MULTISLOT_MAX_TX_PWR value is loaded,
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_multislot_tx_pwr
(
  gsm_multislot_max_tx_pwr_type* static_nv_struct,
  rfnv_item_id_enum_type tx_pwr_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     tx_pwr_data_nv_status;
  rfnv_item_type        tx_pwr_data_buffer;
  rfnv_item_size_type   tx_pwr_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id. tx_pwr_data_item = tx_pwr_data_item + band*/
  tx_pwr_data_item = tx_pwr_data_item + band;
  
  tx_pwr_data_nv_status =
    rfcommon_rfnv_get_item( tx_pwr_data_item,
                            &tx_pwr_data_buffer,
                            &tx_pwr_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( tx_pwr_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &tx_pwr_data_buffer, sizeof(gsm_multislot_max_tx_pwr_type));
  } /* if (tx_pwr_data_nv_status == NV_DONE_S ) */
  else
  {
    /* Retrun false to ensure that default values are loaded properly*/
    return FALSE;
  } /* else: if (tx_pwr_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_multislot_tx_pwr() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function to init the MULTISLOT_MAX_TX_PWR related data to default values.

  @details
   initalizes the MULTISLOT_MAX_TX_PWR data for a band with default values

  @param tx_static_nv_data_ptr

  @param band

  @return
  TRUE
*/


boolean
rfgsm_nv_load_default_mulislot_tx_pwr
(
  rfgsm_nv_supported_type band,
  gsm_tx_static_nv_type* tx_static_nv_data_ptr
)
{
  uint8 i;

  /*Set GMSK values to highest PCL, 8PSK values to highest PCL-2*/

  /* low bands have RFNV_NUMBER_OF_PCLS-1 as highest PCL*/
  if (band == RFGSM_NV_850 || band == RFGSM_NV_900)
  {
    for ( i=0; i<RFGSM_MAX_MULTISLOTS; i++ )
    {
      tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_gmsk[i] =
          tx_static_nv_data_ptr->power_levels[RFNV_NUMBER_OF_PCLS-2];
      tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_8psk[i] = 	  
          tx_static_nv_data_ptr->power_levels[RFNV_NUMBER_OF_PCLS-5];    /* to check with Lam, should it not be be RFNV_NUMBER_OF_PCLS- 4*/
    }
  }
  else
  {
    for ( i=0; i<RFGSM_MAX_MULTISLOTS; i++ )
    {
      tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_gmsk[i] =
          tx_static_nv_data_ptr->power_levels[RFNV_NUMBER_OF_PCLS-1];
      tx_static_nv_data_ptr->multislot_max_tx_power.gsm_multislot_tx_pwr_8psk[i] =	  
          tx_static_nv_data_ptr->power_levels[RFNV_NUMBER_OF_PCLS-3];
    }
  }
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Initialzed the MULTISLOT TX PWR data to default for band %d", band);	
  
  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the POWER_LEVELS related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the POWER_LEVELS data for that band.

  @param gsm_power_levels
  The gsm_power_levels NV data structure where the POWER_LEVELS data has to be loaded into

  @param pwr_lvls_data_item
  The NV item that contains the POWER_LEVELS value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the POWER_LEVELS value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_power_levels
(
  gsm_power_levels* static_nv_struct,
  rfnv_item_id_enum_type pwr_lvls_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pwr_lvls_data_nv_status;
  rfnv_item_type        pwr_lvls_data_buffer;
  rfnv_item_size_type   pwr_lvls_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id.  pwr_lvls_data_item = pwr_lvls_data_item + band*/
  pwr_lvls_data_item = pwr_lvls_data_item + band;
  
  pwr_lvls_data_nv_status =
    rfcommon_rfnv_get_item( pwr_lvls_data_item,
                            &pwr_lvls_data_buffer,
                            &pwr_lvls_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pwr_lvls_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &pwr_lvls_data_buffer, sizeof(gsm_power_levels));
  } /* if (pwr_lvls_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &power_levels_default, sizeof(gsm_power_levels));
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_power_levels: Initialzed the POWER LEVELS data to default", 0);	
    return TRUE;
  } /* else: if (pwr_lvls_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_power_levels() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the SMPS_PDM_TBL related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the SMPS_PDM_TBL data for that band.

  @param gsm_smps_pdm_tbl_type
  The gsm_smps_pdm_tbl_type NV data structure where the SMPS_PDM_TBL data has to be loaded into

  @param pdm_tbl_data_item
  The NV item that contains the SMPS_PDM_TBL value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the SMPS_PDM_TBL value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_smps_pdm_tbl
(
  gsm_smps_pdm_tbl_type* static_nv_struct,
  rfnv_item_id_enum_type pdm_tbl_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pdm_tbl_data_nv_status;
  rfnv_item_type        pdm_tbl_data_buffer;
  rfnv_item_size_type   pdm_tbl_data_returned_size =
                                sizeof(rfnv_item_type);
  

  /*Adding a offset to RF NV id.  pdm_tbl_data_item = pdm_tbl_data_item + band*/
  pdm_tbl_data_item = pdm_tbl_data_item + band;
  
  pdm_tbl_data_nv_status =
    rfcommon_rfnv_get_item( pdm_tbl_data_item,
                            &pdm_tbl_data_buffer,
                            &pdm_tbl_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pdm_tbl_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &pdm_tbl_data_buffer, sizeof(gsm_smps_pdm_tbl_type));
  } /* if (pdm_tbl_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    *static_nv_struct = smps_pdm_tbl_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_smps_pdm_tbl: Initialzed the SMPS PDM TBL data to default", 0);	
    return TRUE;
  } /* else: if (pdm_tbl_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_smps_pdm_tbl() */





boolean
rfgsm_nv_load_enh_apt_data
(
  enh_apt_data_type* static_nv_struct,
  rfnv_item_id_enum_type enh_apt_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     enh_apt_data_nv_status;
  rfnv_item_type        enh_apt_data_buffer;
  rfnv_item_size_type   enh_apt_data_returned_size =
                                sizeof(rfnv_item_type);
  

  /*Adding a offset to RF NV id.  enh_apt_data = enh_apt_data + band*/
  enh_apt_data_item = enh_apt_data_item + band;
  
  enh_apt_data_nv_status =
    rfcommon_rfnv_get_item( enh_apt_data_item,
                            &enh_apt_data_buffer,
                            &enh_apt_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );



  if ( enh_apt_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(enh_apt_data_type), &enh_apt_data_buffer, sizeof(enh_apt_data_type));
  } /* if (enh_apt_data_nv_status == NV_DONE_S ) */
  else
  {

    /*ALL Bypass mode*/
    static_nv_struct->pcl_threshold = 16;
    static_nv_struct->vbatt_thres_low_pwr = 6000;
    static_nv_struct->vbatt_thresold = 6000;

    /* initialize the structure with default values accordlingly*/
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_enh_apt_data: Initialzed the Enhanced APT data to default, status:%d",enh_apt_data_nv_status );	


    return TRUE;
  } 



  return TRUE;
} /* rfgsm_nv_load_enh_apt_data() */



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the SAR_BACKOFF related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the SAR_BACKOFF data for that band.

  @param gsm_sar_lut_type
  The gsm_sar_lut_type NV data structure where the SAR_BACKOFF data has to be loaded into

  @param sar_lut_data_item
  The NV item that contains the SAR_BACKOFF value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the SAR_BACKOFF value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_sar_lut
(
  gsm_sar_lut_type* static_nv_struct,
  rfnv_item_id_enum_type sar_lut_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     sar_lut_data_nv_status;
  rfnv_item_type        sar_lut_data_buffer;
  rfnv_item_size_type   sar_lut_data_returned_size =
                                sizeof(rfnv_item_type);
  uint8 slot_count, sar_state;
  
  /*Adding a offset to RF NV id. sar_lut_data_item = sar_lut_data_item + band*/
  sar_lut_data_item = sar_lut_data_item + band;
  
  sar_lut_data_nv_status =
    rfcommon_rfnv_get_item( sar_lut_data_item,
                            &sar_lut_data_buffer,
                            &sar_lut_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( sar_lut_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &sar_lut_data_buffer, sizeof(gsm_sar_lut_type));
  } /* if (sar_lut_data_nv_status == NV_DONE_S ) */
  else
  {
    for (slot_count=0; slot_count < RFNV_GSM_SAR_NUM_UL_SLOT_MAX; slot_count++ )
    {
      for (sar_state = 0; sar_state < RF_SAR_STATE_MAX - 1; sar_state++)
      {
        static_nv_struct->sar_lut[slot_count][sar_state].sar_gmsk = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT;
        static_nv_struct->sar_lut[slot_count][sar_state].sar_8psk = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT;
      }
    }
	    
    /* initialize the structure with default values accordlingly*/
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_sar_lut: Initialzed the SAR BACKOFF data to default", 0);	
    return TRUE;
  } /* else: if (sar_lut_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_sar_lut() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
   Generic function to load pa icq bias related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the pa icq bias data for that band.
*/

boolean
rfgsm_nv_load_pa_icq_bias
(
  gsm_pa_icq_bias_type* static_nv_struct,
  rfnv_item_id_enum_type pa_icq_bias_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pa_icq_bias_data_nv_status;
  rfnv_item_type        pa_icq_bias_data_buffer;
  rfnv_item_size_type   pa_icq_bias_data_returned_size =
                                sizeof(rfnv_item_type);
  

  /*Adding a offset to RF NV id.  pdm_tbl_data_item = pdm_tbl_data_item + band*/
  pa_icq_bias_data_item = pa_icq_bias_data_item + band;
  
  pa_icq_bias_data_nv_status =
    rfcommon_rfnv_get_item( pa_icq_bias_data_item,
                            &pa_icq_bias_data_buffer,
                            &pa_icq_bias_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_icq_bias_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &pa_icq_bias_data_buffer, sizeof(gsm_pa_icq_bias_type));
  } /* if (pa_bias_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    *static_nv_struct = pa_icq_bias_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_pa_icq_bias: Initialzed the PA ICQ BIAS data to default", 0);	
    return TRUE;
  } /* else: if (pa_bias_data_nv_status == NV_DONE_S ) */


  return TRUE;
} /* rfgsm_nv_load_pa_bias */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Polar PA Ramp LUT related data for any band.

  @return
  TRUE if the Polar PA Ramp LUT  value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_pa_ramp_lut
(
  gsm_pa_ramp_lut_type* static_nv_struct,
  rfnv_item_id_enum_type polar_pa_ramp_lut_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pa_ramp_lut_data_nv_status;
  rfnv_item_type        pa_ramp_lut_data_buffer;
  rfnv_item_size_type   pa_ramp_lut_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id. polar_pa_ramp_lut_data_item = polar_pa_ramp_lut_data_item + band*/
  polar_pa_ramp_lut_data_item = polar_pa_ramp_lut_data_item + band;
  
  pa_ramp_lut_data_nv_status =
    rfcommon_rfnv_get_item( polar_pa_ramp_lut_data_item,
                            &pa_ramp_lut_data_buffer,
                            &pa_ramp_lut_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_ramp_lut_data_nv_status == NV_DONE_S )
  {
    
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &pa_ramp_lut_data_buffer, sizeof(gsm_pa_ramp_lut_type));
	
  } /* if (pa_ramp_lut_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    *static_nv_struct = polar_paramp_lut_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_pa_ramp_lut: Initialzed the PA Ramp LUT data to default", 0);	
    return TRUE;
  } /* else: if (pa_ramp_lut_data_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_pa_ramp_lut() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the TX_RX_ANT_TUNER related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the TX_RX_ANT_TUNER data for that band.

  @param gsm_ant_tuner_type
  The gsm_ant_tuner_type NV data structure where the TX_RX_ANT_TUNER data has to be loaded into

  @param ant_tuner_data_item
  The NV item that contains the TX_RX_ANT_TUNER value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the TX_RX_ANT_TUNER value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_ant_tuner
(
  gsm_ant_tuner_type* static_nv_struct,
  rfnv_item_id_enum_type ant_tuner_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     ant_tuner_data_nv_status;
  rfnv_item_type        ant_tuner_data_buffer;
  rfnv_item_size_type   ant_tuner_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id. ant_tuner_data_item = ant_tuner_data_item + band*/
  ant_tuner_data_item = ant_tuner_data_item + band;
  
  ant_tuner_data_nv_status =
    rfcommon_rfnv_get_item( ant_tuner_data_item,
                            &ant_tuner_data_buffer,
                            &ant_tuner_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( ant_tuner_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &ant_tuner_data_buffer, sizeof(gsm_ant_tuner_type));
  } /* if (ant_tuner_data_nv_status == NV_DONE_S ) */
  else
  {
    /*Initialize the structure with default values accordlingly*/
    *static_nv_struct = gsm_ant_tuner_default;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_ant_tuner: Initialzed the TX/RX ANT TUNER data to default", 0);	
    return TRUE;
  } /* else: if (ant_tuner_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_ant_tuner() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
   Generic function to load therm read value during cal for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the therm read value during calibration for that band.
*/

boolean
rfgsm_nv_load_cal_temp
(
  gsm_cal_temp_data_type* static_nv_struct,
  rfnv_item_id_enum_type cal_temp_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     cal_temp_data_nv_status;
  rfnv_item_type        cal_temp_data_buffer;
  rfnv_item_size_type   cal_temp_data_returned_size =
                                sizeof(rfnv_item_type);
  

  /*Adding a offset to RF NV id.  cal_temp_data_item = cal_temp_data_item + band*/
  cal_temp_data_item = cal_temp_data_item + band;
  
  cal_temp_data_nv_status =
    rfcommon_rfnv_get_item( cal_temp_data_item,
                            &cal_temp_data_buffer,
                            &cal_temp_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( cal_temp_data_nv_status == NV_DONE_S )
  {
    memscpy(static_nv_struct, sizeof(*static_nv_struct), &cal_temp_data_buffer, sizeof(gsm_cal_temp_data_type));
  } /* if (cal_temp_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    static_nv_struct->therm_read = 0;
    return TRUE;
  } /* else: if (cal_temp_data_nv_status == NV_DONE_S ) */


  return TRUE;
} /* rfgsm_nv_load_cal_temp */


#ifdef FEATURE_RF_HAS_QTUNER
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the ANT_TUNER_CL related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the TX_RX_ANT_TUNER data for that band.

  @param gsm_ant_tuner_type
  The gsm_ant_tuner_type NV data structure where the TX_RX_ANT_TUNER data has to be loaded into

  @param ant_tuner_data_item
  The NV item that contains the TX_RX_ANT_TUNER value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the TX_RX_ANT_TUNER value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_ant_tuner_cl
(
  rfcommon_nv_ant_tuner_cl_ctrl_type* static_nv_struct,
  rfnv_item_id_enum_type ant_tuner_cl_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     ant_tuner_cl_data_nv_status;
  rfnv_item_type        ant_tuner_cl_data_buffer;
  rfnv_item_size_type   ant_tuner_cl_data_returned_size =
                                sizeof(rfnv_item_type);
  
  /*Adding a offset to RF NV id. ant_tuner_data_item = ant_tuner_data_item + band*/
  ant_tuner_cl_item = ant_tuner_cl_item + band;
  
  /*Make sure pointer is initialized to NULL before loading NV*/
  static_nv_struct->payload = NULL;

  ant_tuner_cl_data_nv_status =
    rfcommon_rfnv_get_item( ant_tuner_cl_item,
                            &ant_tuner_cl_data_buffer,
                            &ant_tuner_cl_data_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( ant_tuner_cl_data_nv_status == NV_DONE_S )
  {
   rfcommon_load_ant_tuner_cl_ctrl_info_from_nv( &ant_tuner_cl_data_buffer, static_nv_struct);
  } /* if (ant_tuner_cl_data_nv_status == NV_DONE_S ) */
  else
  {
    /*Print error message*/
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_ant_tuner_cl: Error while loading NV item!", 0);	
    return TRUE;
  } /* else: if (ant_tuner_cl_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rfgsm_nv_load_ant_tuner_cl() */
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Rx cal related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the Rx cal data for that band.

  @param gsm_rx_cal_data_nv_type
  The gsm_rx_cal_data_nv_type NV data structure where the Rx cal data has to be loaded into

  @param rx_cal_data_item
  The NV item that contains the Rx cal value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the Rx cal value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/


boolean
rfgsm_nv_load_rx_cal_items
(
  gsm_rx_cal_data_nv_type* gsm_rx_cal_nv_struct,
  rfnv_item_id_enum_type rx_cal_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{

  /* Overall status for this API */
  boolean api_status = TRUE;

  /* NV read status */
  nv_stat_enum_type  rx_cal_nv_read_status = NV_DONE_S;

  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;

  /* actual size of memory read from the NV*/
  rfnv_item_size_type   gsm_rx_cal_nv_read_size = sizeof(rfnv_item_type);
  

  nv_data_ptr = (rfnv_item_type *)modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
  
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_rx_cal_items - failed to allocate memory for %d bytes.", sizeof(rfnv_item_type));
    api_status = FALSE;
    return api_status;
  }
  
  memset((void *)nv_data_ptr, 0, sizeof(rfnv_item_type));

  /*Adding a offset to RF NV id. rx_cal_data_item = rx_cal_data_item + band*/
  rx_cal_data_item = rx_cal_data_item + band;
  
  rx_cal_nv_read_status = rfcommon_rfnv_get_item( rx_cal_data_item,
                            nv_data_ptr,
                            &gsm_rx_cal_nv_read_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );
  
  if ( rx_cal_nv_read_status == NV_DONE_S )
  {
    memscpy(gsm_rx_cal_nv_struct, sizeof(*gsm_rx_cal_nv_struct), nv_data_ptr, gsm_rx_cal_nv_read_size);
    rfgsm_nv_sort_rx_cal_data(gsm_rx_cal_nv_struct, (rfcom_gsm_band_type)band, gsm_rx_cal_nv_read_size);
  }/* if (rx_cal_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_rx_cal_items: Initialzed the RX cal data to default", 0);	
    api_status = TRUE;
  } /* else: if (rx_cal_nv_status == NV_DONE_S ) */

  /* We are done, do the clean-up */
  modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  nv_data_ptr = NULL;

  return api_status;
} /* rfgsm_nv_load_rx_cal_items() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx cal related data for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the Tx cal data for that band.

  @param gsm_tx_cal_data_nv_type
  The gsm_tx_cal_data_nv_type NV data structure where the Rx cal data has to be loaded into

  @param tx_cal_data_item
  The NV item that contains the tx cal value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the Tx cal value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/

/* Size of Fixed part (10 bytes) and variant header (4 bytes), total 14 bytes*/ 
#define RF_GSM_TX_CAL_NV_FIXED_SIZE  (0x0A + sizeof(rfnv_data_variant_marker_type))

boolean
rfgsm_nv_load_tx_cal_items
(
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data,
  rfnv_item_id_enum_type tx_cal_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type ),
  rfnv_gsm_read_status *nv_read_status_ptr
)
{
  /* Overall status for this API */
  boolean api_status = TRUE;

  /* NV read status */
  nv_stat_enum_type  tx_cal_nv_read_status = NV_DONE_S;

  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type);

  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;

  /* Helper pomiter to hold moving ptr location during data copy*/
  uint8     *temp_data_ptr = NULL;

  /* actual size of memory read from the NV*/
  rfnv_item_size_type   gsm_tx_cal_nv_read_size = sizeof(rfnv_item_type);

  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;

  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed = 0;

  /* contains no of type_ids found in Tx cal NV*/
  uint16 num_type_id = 0;

  /* Used for looping through TYPE IDs */
  uint16 type_id_idx;

  /* The TYPE ID of the current element */
  uint16 type_id;
  
  /* Helper */
  uint32 temp_num_bytes_left;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_alloc( sizeof(rfnv_item_type),
                                                   MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
  
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items - failed to allocate memory for %d bytes.", nv_item_size);
    api_status = FALSE;
    return FALSE;
  }

  
  /*Adding a offset to RF NV id. tx_cal_data_item = tx_cal_data_item + band*/
  tx_cal_data_item = tx_cal_data_item + band;
  
  tx_cal_nv_read_status = rfcommon_rfnv_get_item( tx_cal_data_item,
                                                  nv_data_ptr,
                                                  &gsm_tx_cal_nv_read_size,
                                                  caller_tcb_ptr,
                                                  task_nv_wait_sig,
                                                  task_wait_func_ptr );

  if ( tx_cal_nv_read_status == NV_DONE_S )
  {
    
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items: TX cal NV read size = %d", gsm_tx_cal_nv_read_size);
	
  } /* if (tx_cal_data_nv_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items: Initialzed the TX cal data to default", 0);	
    rfgsm_nv_init_default_tx_cal_values( gsm_tx_cal_nv_data );
  	api_status = FALSE;

    if( nv_data_ptr != NULL )
    {
      modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
      nv_data_ptr = NULL;
    }

    if (nv_read_status_ptr != NULL)
    {
      nv_read_status_ptr->last_nv_item = tx_cal_data_item;
      nv_read_status_ptr->nv_read_status_flag =  FALSE;
    }

    return TRUE;
  } /* else: if (tx_cal_nv_status == NV_DONE_S ) */


  /* If NV data has been successfully retrieved proceed to populating the mission mode band_cal data structure */
  if (api_status == TRUE) 
  {

    temp_data_ptr = (uint8 *)nv_data_ptr;

    /* copy data for fixed part and variant header of the GSM Tx cal NV, need to adjust size*/
    nv_element_size = RF_GSM_TX_CAL_NV_FIXED_SIZE;

    memscpy((void *)gsm_tx_cal_nv_data, sizeof(*gsm_tx_cal_nv_data), (void*)temp_data_ptr, nv_element_size );

    /* Update running total of bytes already parsed */
    num_bytes_parsed += nv_element_size;

    /* now we know no of type_ids present in the NV, copy it*/
    num_type_id = gsm_tx_cal_nv_data->rfnv_data_variant_marker.num_elements;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items - found %d no of dynamic data",num_type_id);

    /* print some debug - to be removed*/
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items - no of cal channels %d",gsm_tx_cal_nv_data->tx_cal_chan_size);


    /* copy all data nodes one by one*/
    for( type_id_idx = 0; (type_id_idx < num_type_id) && (api_status == TRUE); type_id_idx++ )
    {
       /* Get type ID of the subsequent element*/
       nv_element_size = RFNV_DATA_TYPE_ID_SIZE;

       MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "nv_element_size = %d", nv_element_size );

       /* Ensure there is enough memory left before actually extracting the element */
       if((gsm_tx_cal_nv_read_size - num_bytes_parsed) < nv_element_size )
       {
         MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items - exiting num_bytes_parsed = %d", num_bytes_parsed );
         api_status = FALSE;
         break;
       }

       /* Check that we are not going to overflow the buffer */
       if ( (num_bytes_parsed + nv_element_size) >= RFNV_DATA_SIZE_MAX)
       {
         MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items - memory exhausted", 0 );
         api_status = FALSE;
         break;
       }

       /* EXTRACT ELEMENT: Type ID */
       temp_data_ptr = (uint8 *)nv_data_ptr + num_bytes_parsed;

       memscpy( (void*) &type_id, sizeof(type_id), (void*)temp_data_ptr, nv_element_size );

       /* Update running total of bytes parsed with num bytes from last element extracted */
       num_bytes_parsed += nv_element_size;
       
       temp_num_bytes_left = gsm_tx_cal_nv_read_size - num_bytes_parsed;

       if( temp_num_bytes_left <= 0 )
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "rfgsm_nv_load_tx_cal_items - No elements left to extract "
               "item of TYPE_ID for type index %d", type_id, type_id_idx);
         api_status = FALSE;
         break;
       }

       /* Make sure we haven't overflowed the buffer */
       num_bytes_parsed = MIN(num_bytes_parsed,RFNV_DATA_SIZE_MAX);

      /* EXTRACT ELEMENT corresponding to the this type ID */
       temp_data_ptr = (uint8 *)nv_data_ptr + num_bytes_parsed;

       switch( type_id )
       {
       case RFNV_DATA_TYPE_ID_RGI_LIST:
          api_status &= rfgsm_nv_extract_tx_rgi_list(temp_data_ptr,
                                                      temp_num_bytes_left,
                                                      &nv_element_size,
                                                      gsm_tx_cal_nv_data );  
          break;

       case RFNV_DATA_TYPE_ID_PMEAS_LIST:
          api_status &= rfgsm_nv_extract_tx_pmeas_data(temp_data_ptr,
                                                      temp_num_bytes_left,
                                                      &nv_element_size,
                                                      gsm_tx_cal_nv_data );  
          break;

      case RFNV_DATA_TYPE_ID_AMAM_LIST:
          api_status &= rfgsm_nv_extract_amam_table(temp_data_ptr,
                                                      temp_num_bytes_left,
                                                      &nv_element_size,
                                                      gsm_tx_cal_nv_data );  
          break;

      case RFNV_DATA_TYPE_ID_AMPM_LIST:
          api_status &= rfgsm_nv_extract_ampm_table(temp_data_ptr,
                                                      temp_num_bytes_left,
                                                      &nv_element_size,
                                                      gsm_tx_cal_nv_data );  
          break;

       case RFNV_DATA_TYPE_TX_PWR_ERR_LIST:
          api_status &= rfgsm_nv_extract_tx_pwr_err(temp_data_ptr,
                                                      temp_num_bytes_left,
                                                      &nv_element_size,
                                                      gsm_tx_cal_nv_data );
          break;

      default:
          {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_cal_items - element of type ID %d not supported "
                  "for type index %d", type_id, type_id_idx);
            nv_element_size = 0;
            api_status = FALSE;
          }
       }

     /* Update running total of bytes parsed with num bytes from last element extracted */
     num_bytes_parsed += nv_element_size;
    } /* end of for loop*/

    if ( rfgsm_debug_flags.override_amam_ampm_tables == TRUE )
    {
     rfgsm_nv_init_override_amam_ampm_tables( gsm_tx_cal_nv_data );
    }

  }

  /* We are done, do the clean-up  */
  if( nv_data_ptr != NULL )
  {
    modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
    nv_data_ptr = NULL;
  }

  return TRUE;

} /* rfgsm_nv_load_tx_cal_items() */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting Tx RGI list for a given PA range and mode
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfgsm_nv_extract_tx_rgi_list
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  rfnv_data_rgi_list_type rgi_node_data;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* For indexing */
  uint8 rgi_idx;
  /* PA state from variant data_node */
  uint8 pa_state;

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(rgi_node_data);

  RF_MSG( RF_HIGH, "rfgsm_nv_extract_tx_rgi_list" );

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_extract_tx_rgi_list - Not enough elements left to extract",0,0 );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &rgi_node_data, sizeof(rgi_node_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;
    
    /* extract and fill RGI list in mission mode NV strcutures based on mode and PA state*/
    if (rgi_node_data.mod == 0) /* GMSK case*/
    {
      pa_state = rgi_node_data.pa_state;
      gsm_tx_cal_nv_data->gsm_rgi_table[pa_state].rgi_list_size = rgi_node_data.valid_rgi_num;

      /* Copy the RGI list in corresponding NV structure*/
      for (rgi_idx = 0; rgi_idx < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; rgi_idx++)
      {
        gsm_tx_cal_nv_data->gsm_rgi_table[pa_state].rgi_list[rgi_idx] = rgi_node_data.rgi_list[rgi_idx];
      }
      /* Set the data_valid flag for rgi table of this PA state */
      gsm_tx_cal_nv_data->gsm_rgi_table[pa_state].data_valid = TRUE;
    }
	
    else if (rgi_node_data.mod == 1) /* EDGE case */
    {
      pa_state = rgi_node_data.pa_state;
      gsm_tx_cal_nv_data->edge_rgi_table[pa_state].rgi_list_size = rgi_node_data.valid_rgi_num;
	
      /* Copy the RGI list in corresponding NV structure*/
      for (rgi_idx = 0; rgi_idx < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; rgi_idx++)
      {
        gsm_tx_cal_nv_data->edge_rgi_table[pa_state].rgi_list[rgi_idx] = rgi_node_data.rgi_list[rgi_idx];
		
        if(rfgsm_nv_check_debug_nv(0,   RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE))
        {   
          RF_MSG_3( RF_HIGH, "debug rfgsm_nv_extract_tx_rgi_list, pa_state=%d, mod=%d, rgi=%d",
			  pa_state, rgi_node_data.mod, gsm_tx_cal_nv_data->edge_rgi_table[pa_state].rgi_list[rgi_idx] );
        }
      }
      /* Set the data_valid flag for rgi table of this PA state */
      gsm_tx_cal_nv_data->edge_rgi_table[pa_state].data_valid = TRUE;
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_extract_tx_rgi_list invalid mode=%d",rgi_node_data.mod);
	  api_status = FALSE;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_tx_cal_chans */


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting Tx Pmeas list for a given PA range and mode
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfgsm_nv_extract_tx_pmeas_data
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  rfnv_data_pmeas_list_type pmeas_data;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* For indexing */
  uint8 rgi_idx;

  /* PA state from variant data_node */
  uint8 pa_state;

  /* Cal channel index from variant data_node */
  uint8 cal_channel_idx;

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(pmeas_data);

  if(rfgsm_nv_check_debug_nv(0,	RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE))
  {
    RF_MSG( RF_HIGH, "rfgsm_nv_extract_tx_pmeas_data" );
  }
  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfgsm_nv_extract_tx_pmeas_data - Not enough elements left to extract" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &pmeas_data, sizeof(pmeas_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;
    
    /* extract and fill PMEAS list in mission mode NV strcutures based on mode and PA state*/
    if (pmeas_data.mod == 0) /* GMSK case*/
    {
      pa_state = pmeas_data.pa_state;
      cal_channel_idx = pmeas_data.channel_index;

      /* Copy the PMEAS list in corresponding NV structure*/
      for (rgi_idx = 0; rgi_idx < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; rgi_idx++)
      {
        gsm_tx_cal_nv_data->gsm_pmeas_table[pa_state].tx_pmeas_data_table[cal_channel_idx].pmeas[rgi_idx] = pmeas_data.pmeas_list[rgi_idx];
		
	  if(rfgsm_nv_check_debug_nv(0,	RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE))
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "debug rfgsm_nv_extract_tx_pmeas_data, pa_state=%d, mode=%d, pmeas=%d",
                  pa_state, pmeas_data.mod, gsm_tx_cal_nv_data->gsm_pmeas_table[pa_state].tx_pmeas_data_table[cal_channel_idx].pmeas[rgi_idx]);
          } 
      }
      /* Set the data_valid flag for PMEAS table of this PA state */
      gsm_tx_cal_nv_data->gsm_pmeas_table[pa_state].tx_pmeas_data_table[cal_channel_idx].data_valid = TRUE;
    }
	
    else if (pmeas_data.mod == 1) /* EDGE case */
    {
      pa_state = pmeas_data.pa_state;
      cal_channel_idx = pmeas_data.channel_index;
		
      /* Copy the RGI list in corresponding NV structure*/
      for (rgi_idx = 0; rgi_idx < RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE; rgi_idx++)
      {
        gsm_tx_cal_nv_data->edge_pmeas_table[pa_state].tx_pmeas_data_table[cal_channel_idx].pmeas[rgi_idx] = pmeas_data.pmeas_list[rgi_idx];
		  
	if(rfgsm_nv_check_debug_nv(0,	RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE))
        {
          MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "debug rfgsm_nv_extract_tx_pmeas_data, pa_state=%d, mode=%d, pmeas=%d",
		pa_state, pmeas_data.mod, gsm_tx_cal_nv_data->edge_pmeas_table[pa_state].tx_pmeas_data_table[cal_channel_idx].pmeas[rgi_idx]);
         }
      }
      /* Set the data_valid flag for PMEAS table of this PA state */
      gsm_tx_cal_nv_data->edge_pmeas_table[pa_state].tx_pmeas_data_table[cal_channel_idx].data_valid = TRUE;
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_extract_tx_pmeas_data invalid mode=%d",pmeas_data.mod);
	  api_status = FALSE;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_tx_cal_chans */


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx AMAM list for a given cal channel.
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfgsm_nv_extract_amam_table
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  rfnv_data_amam_list_type amam_data;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* cal channel index from variant data_node */
  uint8 cal_channel_idx;
  /* For indexing */
  uint16 nv_index = 0, table_idx = 0;


  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(amam_data);

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfgsm_nv_extract_amam_table - Not enough elements left to extract" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &amam_data, sizeof(amam_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;
    
    /* extract and fill AMAM list in mission mode NV strcutures based on cal channel*/
    if (amam_data.amam_ampm_identifier == 0) /* AMAM case*/
    {
      cal_channel_idx = amam_data.channel_index;

      /* Copy the AMAM data in corresponding NV structure*/
      for (nv_index = 0; nv_index < GSM_AMAM_TBL_SIZE; nv_index++)
      {
        /*Need to fill the NV data of 128 values in 256 size AMAM table by skipping every second index*/
        table_idx = (nv_index * RFGSM_LINEAR_EXPANDED_TBL) + 1;

        gsm_tx_cal_nv_data->amam_master_table.amam_tbl[cal_channel_idx].data[table_idx] = amam_data.data_list[nv_index];

	/*moved out this log to reduce excessive F3s by removing data parameter*/
	/*
        MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_nv_extract_amam_table, cal_channel_index=%d, data=%d",
                cal_channel_idx, gsm_tx_cal_nv_data->amam_master_table.amam_tbl[cal_channel_idx].data[table_idx]);
	*/

      }
	  
	  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_nv_extract_ampm_table, cal_channel_index=%d",cal_channel_idx);
	  
      /* Set the data_valid flag for AMAM table of this channel */
      gsm_tx_cal_nv_data->amam_master_table.amam_tbl[cal_channel_idx].data_valid = TRUE;
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_extract_amam_table invalid identifier=%d",amam_data.amam_ampm_identifier);
      api_status = FALSE;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_tx_cal_chans */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx AMPM list for a given cal channel.
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfgsm_nv_extract_ampm_table
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  rfnv_data_ampm_list_type ampm_data;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* cal channel index from variant data_node */
  uint8 cal_channel_idx;
  /* For indexing */
  uint16 nv_index = 0, table_idx = 0;


  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(ampm_data);

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfgsm_nv_extract_ampm_table - Not enough elements left to extract" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &ampm_data, sizeof(ampm_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;
    
    /* extract and fill AMAM list in mission mode NV strcutures based on cal channel*/
    if (ampm_data.amam_ampm_identifier == 1) /* AMPM case*/
    {
      cal_channel_idx = ampm_data.channel_index;

      /* Copy the AMPM data in corresponding NV structure*/
      for (nv_index = 0; nv_index < GSM_AMPM_TBL_SIZE; nv_index++)
      {
        /*Need to fill the NV data of 128 values in 256 size AMPM table by skipping every second index*/
        table_idx = (nv_index * RFGSM_LINEAR_EXPANDED_TBL) + 1;

        gsm_tx_cal_nv_data->ampm_master_table.ampm_tbl[cal_channel_idx].data[table_idx] = ampm_data.data_list[nv_index];

	/*moved out this log to reduce excessive F3s by removing data parameter*/
	/*
        MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_nv_extract_ampm_table, cal_channel_index=%d, data=%d",
                cal_channel_idx, gsm_tx_cal_nv_data->ampm_master_table.ampm_tbl[cal_channel_idx].data[table_idx]);
	*/

      }
	  
	  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_nv_extract_ampm_table, cal_channel_index=%d",cal_channel_idx);
	  
      /* Set the data_valid flag for AMPM table of this channel */
      gsm_tx_cal_nv_data->ampm_master_table.ampm_tbl[cal_channel_idx].data_valid = TRUE;
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_extract_ampm_table invalid identifier=%d",ampm_data.amam_ampm_identifier);
      api_status = FALSE;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_tx_cal_chans */


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx PCL vs pwr err list
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfgsm_nv_extract_tx_pwr_err
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  rfnv_data_pwr_err_list_type pwr_err_data;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* cal channel index from variant data_node */
  uint8 cal_channel_idx;
  /* For indexing */
  uint16 table_idx = 0;

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof(pwr_err_data);

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfgsm_nv_extract_tx_pwr_err - Not enough elements left to extract" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &pwr_err_data, sizeof(pwr_err_data), nv_data_ptr, num_bytes_needed );

    /* update this variable so that caller API can know how many bytes are copied in this call */
    *extracted_element_size = num_bytes_needed;
    
    /* extract and fill AMAM list in mission mode NV strcutures based on cal channel*/
	/* GSM case*/
    if (pwr_err_data.mod == 0)
    {
      cal_channel_idx = pwr_err_data.channel_index;

      /* Copy the pwr err data in corresponding NV structure for GSM mod*/
      for (table_idx = 0; table_idx < RFNV_NUMBER_OF_PCLS; table_idx++)
      {
        gsm_tx_cal_nv_data->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[cal_channel_idx].data[table_idx] = pwr_err_data.pwr_err_list[table_idx];

	  if(rfgsm_nv_check_debug_nv(0,   RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE))
          {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_nv_extract_tx_pwr_err, chan_index=%d, data=%d",
                  cal_channel_idx, gsm_tx_cal_nv_data->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[cal_channel_idx].data[table_idx]);
          } 
      }
      /* Set the data_valid flag for Err pwr per PCL table of this channel */
      gsm_tx_cal_nv_data->gsm_tx_pwr_err_per_pcl.tx_pwr_err_data[cal_channel_idx].data_valid = TRUE;
    }
	/* EDGE case*/
    else
    {
      cal_channel_idx = pwr_err_data.channel_index;

      /* Copy the pwr err data in corresponding NV structure for EDGE mod*/
      for (table_idx = 0; table_idx < RFNV_NUMBER_OF_PCLS; table_idx++)
      {
        gsm_tx_cal_nv_data->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[cal_channel_idx].data[table_idx] = pwr_err_data.pwr_err_list[table_idx];

	  if(rfgsm_nv_check_debug_nv(0,   RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE))
          { 
            MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_nv_extract_tx_pwr_err, chan_index=%d, data=%d",
                  cal_channel_idx, gsm_tx_cal_nv_data->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[cal_channel_idx].data[table_idx]);
          }
      }
	  
      /* Set the data_valid flag for Err pwr per PCL table of this channel */
        gsm_tx_cal_nv_data->edge_tx_pwr_err_per_pcl.tx_pwr_err_data[cal_channel_idx].data_valid = TRUE;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_tx_cal_chans */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns an array of NV containers, one for tx other for RX, for a given RFM device and band

  @details
  
  @param rf_chain
  The RFM device for which GSM NV path is required
  band
 
  @return
  GSM NV container array for passed RFM device
*/



rfgsm_nv_container_array_type rfgsm_nv_get_tx_rx_nv_container(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{

  rfgsm_nv_container_array_type nv_cont_array;

  nv_cont_array.nv_container[0] = rfgsm_nv_get_nv_container(rfm_dev,band,RFGSM_NV_RX);

  nv_cont_array.nv_container[1] = rfgsm_nv_get_nv_container(rfm_dev,band,RFGSM_NV_TX);

  return(nv_cont_array);
}


/*!
  @brief
  This function returns the NV container for given RFM device, band and RX/TX

  @details
  
  @param rf_chain
  The RFM device for which GSM NV path is required
  band
  RX/TX
 
  @return
  GSM NV container array for passed RFM device
*/
rfgsm_nv_path_type rfgsm_nv_get_nv_container(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfgsm_nv_rxtx_enum_type rx_tx)
{
  uint32 rx_alt_path;
  uint32 tx_alt_path;  
  rfc_cfg_info_type cfg_struct = {0, RF_GSM_NV_PATH_NUM};
  boolean rfc_dev_cfg_supported =  FALSE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if (rfgsm_core_handle_ptr == NULL)
  	{
      rx_alt_path = 0;
      tx_alt_path = 0;	  
  	}
  else
  	{
  	  rx_alt_path = rfgsm_core_handle_ptr->rx_alternate_path; 
  	  tx_alt_path = rfgsm_core_handle_ptr->tx_alternate_path; 	  
  	}
  
  //rfcom_gsm_band_type first_nv_band_supported = rfgsm_nv_get_lowest_supported_band(rfm_dev);

  if (rx_tx == RFGSM_NV_RX)
  {
    rfc_dev_cfg_supported = rfc_gsm_get_cfg_info( rfm_dev, band, RFC_CONFIG_RX, &cfg_struct, rx_alt_path);
  }
  else if (rx_tx == RFGSM_NV_TX)
  {
    rfc_dev_cfg_supported = rfc_gsm_get_cfg_info( rfm_dev, band, RFC_CONFIG_TX, &cfg_struct, tx_alt_path);
  }
  else
  {
    rfc_dev_cfg_supported = FALSE;
  }


  if (rfc_dev_cfg_supported == FALSE)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_get_nv_container, RFC device configuration not supported for log device:%d, band:%d, rx_tx:%d", rfm_dev,band,rx_tx);
    
  }


#if 0
  if (rfgsm_core_handle_ptr != NULL)  	
      MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_get_nv_container, rfm_dev:%d, rx_tx:%d, alt_path:%d, nv_container: %d", rfm_dev,rx_tx,alt_path, cfg_struct.nv_container);
#endif

  return((rfgsm_nv_path_type)cfg_struct.nv_container);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the NV data for Chain 0 or 2 for passed band.

  @details
  Performs validation on GSM NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The GSM band for which NV data must be loaded on this chain
 
  @param nv_path
  The GSM NV path for which NV must be loaded
 
  @param index_entry
  The GSM path+band index entry that contains the pointers to the
  configuration data structures that are going to be initialized.
 
  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the configuration was successfully loaded from NV
  FALSE if NV did not load, and some more action is required by caller
*/


static
boolean
rfgsm_nv_load_band_static_items
(
  rfm_device_enum_type rfm_dev,
  rfgsm_nv_supported_type band,
  rfgsm_nv_path_type nv_path,
  rfgsm_nv_band_node_type* index_entry,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;
  gsm_rx_static_nv_type* rx_static_nv_data_ptr;
  gsm_tx_static_nv_type* tx_static_nv_data_ptr;

  uint8 split_band_index = 0;

  rfgsm_nv_band_items_map_type *rfgsm_nv_items_map_ptr; /* Pointer to Band specific NV table*/
  rfc_cfg_info_type cfg_struct;
  
  rfgsm_nv_band_data_type* rf_gsm_nv_band_data_buffer_ptr = index_entry->rfgsm_nv_band_data_buffer_ptr;

  if ( rf_gsm_nv_band_data_buffer_ptr == NULL )
  {

	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_static_items: Invalid NULL pointer for the config data", 0);	
    return FALSE;
  }

  rx_static_nv_data_ptr = rf_gsm_nv_band_data_buffer_ptr->rx_static_nv_data_ptr;
  tx_static_nv_data_ptr = rf_gsm_nv_band_data_buffer_ptr->tx_static_nv_data_ptr;
 
  /* Get split band index if this is a split band*/

  if (rfgsm_coex_nv_band_map_tbl[band] != NULL)
  {
    split_band_index = rfgsm_coex_nv_band_map_tbl[band]->split_band_index;
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_static_items: rfgsm_coex_nv_band_map_tbl for nv band:%d is NULL. Switching to default values.", band);	
    split_band_index = 0;
  }

  if (split_band_index >= RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND)
  {
    split_band_index = 0;
  }
 

  /* Get NV ID Table for given NV path*/
  rfgsm_nv_items_map_ptr = rfgsm_nv_get_band_nv_table(nv_path);
  
  if ( rfgsm_nv_items_map_ptr == NULL )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_static_items: "
		                                  "Static NV data structure error for band %d, nv_path %d", band, nv_path);	
    return FALSE;
  }

  /* -------------------------------- Rx Static data ---------------------------------*/
  /* Load LNA swicth point related data */
   
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->lna_switch_point;
    retval = rfgsm_nv_load_lna_swpt( &rx_static_nv_data_ptr->lna_switch_point,
                                          rfgsm_nv_items_map_ptr->lna_switch_point,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );

  } 

  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->hl_lna_switch_point;
    retval = rfgsm_nv_load_lna_swpt( &rx_static_nv_data_ptr->hl_lna_switch_point,
                                      rfgsm_nv_items_map_ptr->hl_lna_switch_point,
                                      band,
                                      caller_tcb_ptr,
                                      task_nv_wait_sig,
                                      task_wait_func_ptr );

  } 
  
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->lna_enh_switch_point;
    retval = rfgsm_nv_load_lna_enh_swpt( &rx_static_nv_data_ptr->lna_enh_switch_point,
                                          rfgsm_nv_items_map_ptr->lna_enh_switch_point,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );

  }

  /* Load Antenna Tuner related data */
	  
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->ant_tuner;
    retval = rfgsm_nv_load_ant_tuner( &rx_static_nv_data_ptr->ant_tuner,
                                          rfgsm_nv_items_map_ptr->ant_tuner,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }



  
   if (retval == TRUE)
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->coex_rx_char_offset_info_tbl_id;

    retval = rfgsm_nv_load_coex_rx_char_offset_info_tbl(&rx_static_nv_data_ptr->coex_rx_char_offset_info_tbl,
                                                        rfgsm_nv_items_map_ptr->coex_rx_char_offset_info_tbl_id,
                                                        band,
                                                        caller_tcb_ptr,
                                                        task_nv_wait_sig,
                                                        task_wait_func_ptr );
  }



  /* -------------------------------- Tx Static data ---------------------------------*/

  if(!rfc_gsm_get_cfg_info( rfm_dev, (rfcom_gsm_band_type)band, RFC_CONFIG_TX, &cfg_struct, 0))
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Tx chain is not present on device %d and band %d", rfm_dev, band);	    
      return retval;
    }
  /* Load Tx timing related data */
  
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->tx_timing_data;
    retval = rfgsm_nv_load_tx_timing( &tx_static_nv_data_ptr->tx_timing_data,
                                          rfgsm_nv_items_map_ptr->tx_timing_data,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

   /* Load PA swicth point related data */
   
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->pa_switch_point;
    retval = rfgsm_nv_load_pa_swpt( &tx_static_nv_data_ptr->pa_switch_point,
                                          rfgsm_nv_items_map_ptr->pa_switch_point,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* load PA range map data */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->pa_range_map;
    retval = rfgsm_nv_load_pa_range_map( &tx_static_nv_data_ptr->pa_range_map,
                                          rfgsm_nv_items_map_ptr->pa_range_map,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }


  /* Load KV CAL related data */
   
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->kv_cal;
    retval = rfgsm_nv_load_kv_cal( &tx_static_nv_data_ptr->kv_cal,
                                          rfgsm_nv_items_map_ptr->kv_cal,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* Load TX_GTR_THRESH related data */
   
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->tx_gtr_thresh;
    retval = rfgsm_nv_load_tx_gtr_thresh( &tx_static_nv_data_ptr->tx_gtr_thresh,
                                          rfgsm_nv_items_map_ptr->tx_gtr_thresh,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* Load POLAR PATH DELAY related data */
   
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->polar_path_delay;
    retval = rfgsm_nv_load_polar_path_delay( &tx_static_nv_data_ptr->polar_path_delay,
                                          rfgsm_nv_items_map_ptr->polar_path_delay,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* Load VABTT related data */

  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->vbatt_data;
    retval = rfgsm_nv_load_vbatt_data( &tx_static_nv_data_ptr->vbatt_data,
                                          rfgsm_nv_items_map_ptr->vbatt_data,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  
  /* Load ENV GAIN related data */
  
 if ( retval == TRUE )
 {
   index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->env_gain;
   retval = rfgsm_nv_load_env_gain( &tx_static_nv_data_ptr->env_gain,
                                          rfgsm_nv_items_map_ptr->env_gain,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
 }

  /* Load AMAM TEMP COMP related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->amam_temp_comp;
	retval = rfgsm_nv_load_amam_temp_comp( &tx_static_nv_data_ptr->gsm_amam_temp_comp,
                                                rfgsm_nv_items_map_ptr->amam_temp_comp,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }
    
  /* Load TEMP COMP related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->temp_comp_data;
	retval = rfgsm_nv_load_temp_comp_data( &tx_static_nv_data_ptr->temp_comp_data,
                                                rfgsm_nv_items_map_ptr->temp_comp_data,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }

	  
  /* Load ENH TEMP COMP related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->temp_comp_data;
	retval = rfgsm_nv_load_enh_temp_comp_data( &tx_static_nv_data_ptr->enh_temp_comp_data,
                                                rfgsm_nv_items_map_ptr->enh_temp_comp_data,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }

 /* Load LINEAR_TX_GAIN_PARAM related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->tx_gain_param;
	retval = rfgsm_nv_load_tx_gain_param( &tx_static_nv_data_ptr->tx_gain_param,
                                                rfgsm_nv_items_map_ptr->tx_gain_param,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }	  

 /* Load POWER LEVELS related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->power_levels;
	retval = rfgsm_nv_load_power_levels( &tx_static_nv_data_ptr->power_levels,
                                                rfgsm_nv_items_map_ptr->power_levels,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }

 /* Load MULTISLOT_MAX_TX_PWR related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->multislot_max_tx_pwr;
	retval = rfgsm_nv_load_multislot_tx_pwr( &tx_static_nv_data_ptr->multislot_max_tx_power,
                                                rfgsm_nv_items_map_ptr->multislot_max_tx_pwr,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
	if (retval == FALSE)
	{
	  /* Set default values for the back off power levels to no back-off*/
	  retval = rfgsm_nv_load_default_mulislot_tx_pwr(band, tx_static_nv_data_ptr);
	}
  }

  /* Load SMPS_PDM_TBL related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->smps_pdm_tbl;
	retval = rfgsm_nv_load_smps_pdm_tbl( &tx_static_nv_data_ptr->smps_pdm_tbl,
                                                rfgsm_nv_items_map_ptr->smps_pdm_tbl,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
   }

 /* Load SAR BACKOFF related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->sar_lut;
	retval = rfgsm_nv_load_sar_lut( &tx_static_nv_data_ptr->sar_lut_data,
                                                rfgsm_nv_items_map_ptr->sar_lut,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }	  

 /* Load pa_icq_bias related data */

  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->pa_icq_bias;
	retval = rfgsm_nv_load_pa_icq_bias( &tx_static_nv_data_ptr->pa_icq_bias_data,
                                                rfgsm_nv_items_map_ptr->pa_icq_bias,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }	  
  
/* Load Enhanced APT data */
  if ( retval == TRUE )
  {

	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->enh_apt_data;
	retval = rfgsm_nv_load_enh_apt_data( &tx_static_nv_data_ptr->enh_apt_data,
                                                rfgsm_nv_items_map_ptr->enh_apt_data,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }
  

 /* Load PA Ramp LUT related data */
	
  if ( retval == TRUE )
  {
	index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->polar_paramp_lut;
	retval = rfgsm_nv_load_pa_ramp_lut( &tx_static_nv_data_ptr->polar_paramp_lut,
                                                rfgsm_nv_items_map_ptr->polar_paramp_lut,
                                                band,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }
    
 #ifdef FEATURE_RF_HAS_QTUNER
 /* Load Antenna Tuner CL related data */
	  
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->ant_tuner_cl;
    retval = rfgsm_nv_load_ant_tuner_cl( &tx_static_nv_data_ptr->ant_tuner_cl_data,
                                          rfgsm_nv_items_map_ptr->ant_tuner_cl,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }
  #endif



  if (retval == TRUE)
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->coex_tx_char_offset_info_tbl_id;



    retval = rfgsm_nv_load_coex_tx_char_offset_info_tbl(&tx_static_nv_data_ptr->coex_tx_char_offset_info_tbl,
                                                    rfgsm_nv_items_map_ptr->coex_tx_char_offset_info_tbl_id,
                                                    band,
                                                    caller_tcb_ptr,
                                                    task_nv_wait_sig,
                                                    task_wait_func_ptr);

  }



  return retval;
} /* rfgsm_nv_load_band_static_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the NV data for Chain 0 or 2  for all bands.

  @details
  Performs validation on GSM NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The GSM band for which NV data must be loaded on this chain
 
  @param nv_path
  The GSM NV path for which NV must be loaded
 
  @param index_entry
  The GSM path+band index entry that contains the pointers to the
  configuration data structures that are going to be initialized.
 
  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the configuration was successfully loaded from NV
  FALSE if NV did not load, and some more action is required by caller
*/


// static
boolean
rfgsm_nv_load_band_cal_items
(
  rfm_device_enum_type rfm_dev,
  rfgsm_nv_supported_type band,
  rfgsm_nv_path_type nv_path,
  rfgsm_nv_band_node_type* index_entry,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type ),
  rfnv_gsm_read_status *nv_read_status_ptr
)
{
  rfgsm_nv_band_data_type* rf_gsm_nv_data_buffer_ptr = index_entry->rfgsm_nv_band_data_buffer_ptr;

  
  boolean retval = TRUE;
  uint8 split_band_index = 0;
  rfgsm_nv_supported_type main_nv_band = RFGSM_NV_850;  // default value
  
  rfgsm_nv_band_items_map_type *rfgsm_nv_items_map_ptr = NULL; /* Pointer to  NV Table*/
  
  rfc_cfg_info_type cfg_struct;

   if ( rf_gsm_nv_data_buffer_ptr == NULL )
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_cal_items: Invalid NULL pointer for the config data", 0);	
    return FALSE;
  }

   /* Get split band index if this is a split band*/

  if (rfgsm_coex_nv_band_map_tbl[band] != NULL)
  {
   split_band_index = rfgsm_coex_nv_band_map_tbl[band]->split_band_index;
   main_nv_band = rfgsm_coex_nv_band_map_tbl[band]->main_rfnv_band;
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_cal_items: rfgsm_coex_nv_band_map_tbl for nv band:%d is NULL. Switching to default values.", band);	
  }


  if (split_band_index >= RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND)
  {
    split_band_index = 0;
  }
 

  /* Get Table for NV items */
  rfgsm_nv_items_map_ptr = rfgsm_nv_get_band_nv_table(nv_path);

  if ( rfgsm_nv_items_map_ptr == NULL )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_cal_items:"
		                               "Cal NV data structure error for Band %d, NV path %d", band, nv_path);	
    return FALSE;
  }

  /* Load Rx cal data */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->rx_cal_data + band;
    retval = rfgsm_nv_load_rx_cal_items( &rf_gsm_nv_data_buffer_ptr->rx_cal_nv_data,
                                          rfgsm_nv_items_map_ptr->rx_cal_data,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* Load HL Rx cal data */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->hl_rx_cal_data + band;
    retval = rfgsm_nv_load_rx_cal_items( &rf_gsm_nv_data_buffer_ptr->hl_rx_cal_nv_data,
                                          rfgsm_nv_items_map_ptr->hl_rx_cal_data,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  
  if(!rfc_gsm_get_cfg_info( rfm_dev, (rfcom_gsm_band_type)band, RFC_CONFIG_TX, &cfg_struct, 0))
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Tx chain is not present on device %d and band %d", rfm_dev, band);	    
      return retval;
    }

  /* Load Tx cal data */
  if ( retval == TRUE )
  {

      index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->tx_cal_data[split_band_index] + main_nv_band;
    
  
    

      retval = rfgsm_nv_load_tx_cal_items( &rf_gsm_nv_data_buffer_ptr->tx_cal_nv_data,
                                          rfgsm_nv_items_map_ptr->tx_cal_data[split_band_index],
                                          main_nv_band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr,
                                          nv_read_status_ptr );
    
  }
  
  /* Load Cal Therm Read Data */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->cal_temp + band;
    retval = rfgsm_nv_load_cal_temp( &rf_gsm_nv_data_buffer_ptr->tx_cal_nv_data.cal_temp,
                                          rfgsm_nv_items_map_ptr->cal_temp,
                                          band,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }
  
  return retval;
} /* rfgsm_nv_load_band_cal_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the band specific NV data for any chain and band.

  @details
  Performs validation on GSM NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The GSM band for which NV data must be loaded on this chain
 
  @param index_entry
  The GSM path+band index entry that contains the pointers to the
  configuration data structures that are going to be initialized.
 
  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the configuration was successfully loaded from NV
*/

static boolean
rfgsm_nv_load_band_specific_nv
(
  rfm_device_enum_type rfm_dev,
  rfgsm_nv_path_type nv_path,
  rfgsm_nv_supported_type band,
  rfgsm_nv_band_node_type* index_entry,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;
  rfnv_gsm_read_status nv_read_status = {TRUE, RFNV_ITEMS_ENUM_MAX};
  rfgsm_nv_supported_type main_nv_band = RFGSM_NV_850;

  if(band < RFGSM_NV_MAX_SUPPORTED)
  {
     if (rfgsm_coex_nv_band_map_tbl[band]!= NULL)
     {
       main_nv_band = rfgsm_coex_nv_band_map_tbl[band]->main_rfnv_band;
     }
     else
     {
       MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_specific_nv: rfgsm_coex_nv_band_map_tbl for nv band:%d is NULL. Switching to default values.", band);	
     }
     /* Load Static NV Items of main band only*/
     retval = rfgsm_nv_load_band_static_items( rfm_dev,
                                               main_nv_band,
                                               nv_path,
                                               index_entry,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );


     /* Load Calibrated NV Items */
     retval = rfgsm_nv_load_band_cal_items( rfm_dev,
                                            band,
                                            nv_path,
                                            index_entry,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr,
                                            &nv_read_status );

     /* if TX cal data for a split band not found then use TX cal data for main band*/
     if (( rfgsm_coex_nv_band_map_tbl[band]!= NULL) && (band != main_nv_band) && (main_nv_band < RFGSM_NV_MAX_SUPPORTED ) && (nv_read_status.nv_read_status_flag == FALSE) )
     {
       retval = rfgsm_nv_load_band_cal_items( rfm_dev,
                                              main_nv_band,
                                              nv_path,
                                              index_entry,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr,
                                              &nv_read_status );
     }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_specific_nv: Invalid band", 0);	
  }

  return retval;
} /* rf_gsm_nv_load_c0 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the cmn NV data for any chain.

  @details
  Performs validation on GSM NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param index_entry
  The GSM path+band index entry that contains the pointers to the
  configuration data structures that are going to be initialized.
 
  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the configuration was successfully loaded from NV
*/


static boolean
rfgsm_nv_load_cmn_nv
(
  rfgsm_nv_path_type nv_path,
  rfgsm_nv_cmn_node_type* index_entry,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type   nv_read_status;
  rfnv_item_size_type item_size;
  rfnv_item_type      item_buffer;
  rfgsm_nv_cmn_data_type* rf_gsm_nv_data_buffer_ptr;
  rfgsm_nv_cmn_items_map_type *rfgsm_nv_items_map_ptr; 

  item_size = sizeof(rfnv_item_type);
  
  rf_gsm_nv_data_buffer_ptr = index_entry->rfgsm_nv_cmn_data_buffer_ptr;

  if ( rf_gsm_nv_data_buffer_ptr == NULL )
  {
   
   MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_band_static_items: Invalid NULL pointer for the config data", 0);	 
   return FALSE;
  }
   
  /* Get NV ID Table for given NV path*/
  rfgsm_nv_items_map_ptr = rfgsm_nv_get_cmn_nv_table(nv_path);


  /*Copy GSM Debug NV */
  index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->gsm_debug_nv_id;
  nv_read_status =  rfcommon_rfnv_get_item( rfgsm_nv_items_map_ptr->gsm_debug_nv_id,
						  &item_buffer,
						  &item_size,
						  caller_tcb_ptr,
						  task_nv_wait_sig,
						  task_wait_func_ptr );

  if (nv_read_status == NV_DONE_S)
  {
    memscpy(rf_gsm_nv_data_buffer_ptr->gsm_debug_nv, sizeof(rf_gsm_nv_data_buffer_ptr->gsm_debug_nv), &item_buffer, item_size);
  }
  else
  {    
    /* by default set to 0 */
    memset(rf_gsm_nv_data_buffer_ptr->gsm_debug_nv, 0, sizeof(rf_gsm_nv_data_buffer_ptr->gsm_debug_nv));
  }
  
    //Copy size of gsm_debug_nv
    rf_gsm_nv_data_buffer_ptr->size_gsm_debug_nv = GSM_DEBUG_NV_MAX_SIZE*32;

  item_size = sizeof(rfnv_item_type);

  /*Copy Enh temp comp enable NV*/
  index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->enh_temp_comp_enable_nv_id;
  
  nv_read_status =  rfcommon_rfnv_get_item( rfgsm_nv_items_map_ptr->enh_temp_comp_enable_nv_id,
  						&item_buffer,
  						&item_size,
  						caller_tcb_ptr,
  						task_nv_wait_sig,
  						task_wait_func_ptr );

  if (nv_read_status == NV_DONE_S)
  {
     memscpy(&(rf_gsm_nv_data_buffer_ptr->enh_tempcomp_enable_nv), sizeof(rf_gsm_nv_data_buffer_ptr->enh_tempcomp_enable_nv),  &item_buffer, item_size);
  }
  else
  {
    /* by default set to 0 */
    rf_gsm_nv_data_buffer_ptr->enh_tempcomp_enable_nv = 0;
  }

  item_size = sizeof(rfnv_item_type);

  /*Copy Pa bias enable NV*/
  index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->pa_icq_bias_nv_enable;
  
  nv_read_status =  rfcommon_rfnv_get_item( rfgsm_nv_items_map_ptr->pa_icq_bias_nv_enable,
  						&item_buffer,
  						&item_size,
  						caller_tcb_ptr,
  						task_nv_wait_sig,
  						task_wait_func_ptr );

  if (nv_read_status == NV_DONE_S)
  {
    memscpy(&(rf_gsm_nv_data_buffer_ptr->pa_icq_bias_nv_enable), sizeof(rf_gsm_nv_data_buffer_ptr->pa_icq_bias_nv_enable),  &item_buffer, item_size);
  }
  else
  {   
    /* by default set to 0 */ 
    rf_gsm_nv_data_buffer_ptr->pa_icq_bias_nv_enable = 0;
  }

  item_size = sizeof(rfnv_item_type);

  /*Copy Cal Therm Read Enable NV*/
  index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->cal_temp_nv_enable;

  nv_read_status = rfcommon_rfnv_get_item( rfgsm_nv_items_map_ptr->cal_temp_nv_enable,
                                               &item_buffer,
                                               &item_size,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );

  if (nv_read_status == NV_DONE_S)
  {
    memscpy(&(rf_gsm_nv_data_buffer_ptr->cal_temp_nv_enable), sizeof(rf_gsm_nv_data_buffer_ptr->cal_temp_nv_enable), &item_buffer, item_size);
  }
  else
  {
    rf_gsm_nv_data_buffer_ptr->cal_temp_nv_enable = 0;
  }

  item_size = sizeof(rfnv_item_type);

  /* Copy GSM Interface Debug NV */
  index_entry->last_nv_item_read = rfgsm_nv_items_map_ptr->gsm_intf_debug_nv_id;  
  nv_read_status =  rfcommon_rfnv_get_item( rfgsm_nv_items_map_ptr->gsm_intf_debug_nv_id,
  						&item_buffer,
  						&item_size,
  						caller_tcb_ptr,
  						task_nv_wait_sig,
  						task_wait_func_ptr );

  if (nv_read_status == NV_DONE_S)
  {
    memscpy(&(rf_gsm_nv_data_buffer_ptr->gsm_rf_reserved_nv), sizeof(rf_gsm_nv_data_buffer_ptr->gsm_rf_reserved_nv),  &item_buffer, item_size);
  }
  else
  {   
    /* by default set to 0 */ 
    rf_gsm_nv_data_buffer_ptr->gsm_rf_reserved_nv[RFGSM_RESERVED_DEBUG_NV_IDX] = 0;
    rf_gsm_nv_data_buffer_ptr->gsm_rf_reserved_nv[RFGSM_RESERVED_TIME_PROF_NV_IDX] = 0;
  }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Load GSM NV
 
  @details
  This will Load New GSM NV to memory.
 
  @param task_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_handler
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.
 
  @return
  Return the Status of NV Load
*/


rfcommon_nv_status_type rfgsm_nv_load_items
(
  rfm_device_enum_type rfm_dev,
  rex_tcb_type *task_ptr,
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)(rex_sigs_type),
  uint8 * flag_ptr
)
{
  rfcommon_nv_status_type status = RFCOMMON_NV_READ_SUCCESS;
  rfgsm_nv_path_type rfgsm_nv_path;
  uint64 band_mask = 0;
  rfgsm_nv_cmn_node_type *index_entry_cmn = NULL;  // for common NV table
  rfgsm_nv_band_node_type *index_entry = NULL;  // for band specific NV tables
  rfcom_gsm_band_type band = RFCOM_NUM_GSM_BANDS;
  rfgsm_nv_container_array_type nv_cont_array;
  rfgsm_nv_path_type prev_nv_container;
  uint8 i = 0;

  rfgsm_nv_supported_type rfnv_band = RFGSM_NV_MAX_SUPPORTED;

  rfgsm_nv_supported_type rfnv_band_split = RFGSM_NV_MAX_SUPPORTED;

  boolean bc_cfg_check = FALSE;
  boolean any_valid_band = FALSE;

  uint64 predef_band_mask[RFCOM_NUM_GSM_BANDS] = {SYS_BAND_MASK_GSM_850,SYS_BAND_MASK_GSM_EGSM_900,SYS_BAND_MASK_GSM_DCS_1800,SYS_BAND_MASK_GSM_PCS_1900};
  if (rfm_dev >= RFM_MAX_WAN_DEVICES)
  {

    ERR_FATAL("rfgsm_nv_load_items: invalid rfm_dev: %d", rfm_dev,0,0);

  }


  //Initialize common NV container for a logical device
  rfgsm_cmn_nv_container[rfm_dev] = RF_GSM_NV_PATH_NUM;

  /* For the common nv table,Get the nv container for the first supported band on a given logical device*/
  /* if any of the GSM bands are enabled read the gsm common NV items */
  for (band=0;band<RFCOM_NUM_GSM_BANDS;band++)
  {

    nv_cont_array =  rfgsm_nv_get_tx_rx_nv_container(rfm_dev,band);
    //Choose valid nv container if any
    rfgsm_nv_path =  (nv_cont_array.nv_container[0] < RF_GSM_NV_PATH_NUM)? nv_cont_array.nv_container[0]:nv_cont_array.nv_container[1];

    //If none of the NV containers are valid, move to next band
    if (rfgsm_nv_path < RF_GSM_NV_PATH_NUM)
    {

      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsm_nv_load_items, rfm_device %d is supported", rfm_dev);
      *flag_ptr = (uint8)TRUE;
      any_valid_band = TRUE;

      band_mask = rfcommon_nv_get_rf_bc_config( (rf_path_enum_type)rfgsm_nv_path );

      //Confirm the band is in fact supported
     if (band_mask & (SYS_BAND_MASK_GSM_EGSM_900 |
                     SYS_BAND_MASK_GSM_DCS_1800 |
                     SYS_BAND_MASK_GSM_850	    |
                     SYS_BAND_MASK_GSM_PCS_1900) )
     {
       bc_cfg_check =  TRUE;
       
       //Supported band found
       break;
     }
    }
  }

  if (bc_cfg_check == TRUE)
  {
    //Store valid NV container of a supported band for common table based on rfm_device
    rfgsm_cmn_nv_container[rfm_dev] = rfgsm_nv_path;
    
    index_entry_cmn = rfgsm_nv_alloc_cmn_nv_buf(rfgsm_nv_path);

    if (index_entry_cmn == NULL)
    {
      return RFCOMMON_NV_READ_FAILED;
    }

    index_entry_cmn->data_valid = rfgsm_nv_load_cmn_nv(rfgsm_nv_path,
                                                   index_entry_cmn,
                                                   task_ptr,
                                                   task_wait_sig,
                                                   task_wait_handler);
  }
  else
  {
    if (any_valid_band == FALSE)
    {
      // When no supported band is found for this RFM device
      *flag_ptr = (uint8)FALSE;
    }

    return RFCOMMON_NV_READ_FAILED;
  }


  
  /* -------------band specific nvs---------------------------*/

  for (band = 0; band < RFCOM_NUM_GSM_BANDS; band++)
  {

    nv_cont_array =  rfgsm_nv_get_tx_rx_nv_container(rfm_dev,band);


    // Reset previous NV container
    prev_nv_container = RF_GSM_NV_PATH_NUM;

    for (i = 0; i < 2; i++)  //  0 for RX and 1 for Tx in each band
    {
      
      if(nv_cont_array.nv_container[i] >= RF_GSM_NV_PATH_NUM)
      {
        continue;
      }

      if (nv_cont_array.nv_container[i] != prev_nv_container)
      {
        prev_nv_container = nv_cont_array.nv_container[i];
      }
      else
      {
        /*log*/
        nv_container_array[rfm_dev][band][i]= prev_nv_container;
        continue;
      }
      /*log*/
      nv_container_array[rfm_dev][band][i]= nv_cont_array.nv_container[i];

      band_mask = rfcommon_nv_get_rf_bc_config( (rf_path_enum_type)nv_cont_array.nv_container[i] );

      rfnv_band = rfgsm_core_rfcom_band_to_nvband(band) ;
      

      if (band_mask & predef_band_mask[band])
      {
        index_entry = rfgsm_nv_alloc_band_nv_buf(rfm_dev, nv_cont_array.nv_container[i], rfnv_band);

        if (index_entry == NULL)
        {
          return RFCOMMON_NV_READ_FAILED;
        }

        index_entry->data_valid = rfgsm_nv_load_band_specific_nv( rfm_dev,
                                                                 nv_cont_array.nv_container[i],
                                                                 rfnv_band,
                                                                 index_entry,
                                                                 task_ptr,
                                                                 task_wait_sig,
                                                                 task_wait_handler);


        if (index_entry->data_valid == TRUE)
        {
          status += RFCOMMON_NV_READ_SUCCESS;
        }
        else
        {
          status += RFCOMMON_NV_READ_FAILED;
        }


        /* For Co-existence*/

        /*Determine Split NV band for the rf com band supported in bc config*/
        if (rfgsm_coex_nv_band_map_tbl[band] != NULL)
        {
          rfnv_band_split = rfgsm_coex_nv_band_map_tbl[band]->rfnv_band[1];
        }
        else
        {

          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_items: rfgsm_coex_nv_band_map_tbl for rfcom band:%d is NULL. Switching to default values.", band);	
        }

        if (rfnv_band_split < RFGSM_NV_MAX_SUPPORTED)
        {

          index_entry = rfgsm_nv_alloc_band_nv_buf(rfm_dev, nv_cont_array.nv_container[i], rfnv_band_split);

          if (index_entry == NULL)
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_load_items, allocate band NV failed NV band:%d ", rfnv_band_split);
            continue;
          }

          index_entry->data_valid = rfgsm_nv_load_band_specific_nv( rfm_dev,
                                                                   nv_cont_array.nv_container[i],
                                                                   rfnv_band_split,
                                                                   index_entry,
                                                                   task_ptr,
                                                                   task_wait_sig,
                                                                   task_wait_handler);


          MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfgsm_nv_load_items, load band specific Nv for coex split band status:%d ", index_entry->data_valid);
/*
          if (index_entry->data_valid == TRUE)
          {
            status += RFCOMMON_NV_READ_SUCCESS;

          }
*/
      }


      }
    }
  }

 

  return status;
}  

void rfgsm_nv_cmn_get_sar_value(rfm_device_enum_type rfm_dev, uint8 num_tx_slots, rfm_common_sar_state_type sar_state, rfnv_gsm_sar_unit_type *sar_limit)
{
   uint8 sar_indx=0;
   rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
   gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;
   
   rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
   
   if (rfgsm_core_handle_ptr == NULL)
   {
	 MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
	 return;
   }
   
   tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;

  /* If sar_index is RF_SAR_STATE_DEFAULT or
   * if sar_index is greater than or equal to RF_SAR_STATE_MAX or 
   * if num_tx_slots is 0 or 
   * if num_tx_slots is greater than the max slot allow, 
   * set the SAR limit to 127dBm. 
   *  
   * Else, return the SAR limit
   *  
   */
   if(sar_limit != NULL)
   {
    if( (sar_state == RF_SAR_STATE_DEFAULT) ||
        (sar_state >= RF_SAR_STATE_MAX) ||
        (0==num_tx_slots) ||
        (num_tx_slots > RFNV_GSM_SAR_NUM_UL_SLOT_MAX)
       )
    {
      sar_limit->sar_gmsk = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT;  /* 127 dBm */
      sar_limit->sar_8psk = RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT;  /* 127 dBm */
    }
    else
    {
      sar_indx = (uint8)sar_state;
      sar_limit->sar_gmsk = tx_static_nv_data_ptr->sar_lut_data.sar_lut[num_tx_slots-1][sar_indx-1].sar_gmsk;
      sar_limit->sar_8psk = tx_static_nv_data_ptr->sar_lut_data.sar_lut[num_tx_slots-1][sar_indx-1].sar_8psk;
    }
   }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This function gets the path delay value that is stored in the NV and offsets
  this with the WTR/RTR specific delay backoff value (from the base delay number) 
  and gives the final path delay that is given to FW. In FTM mode this API 
  allows to directly feed a override value that can be passed to the Modem 
  registers.

  @details
  This function gets the path delay value that is stored in the NV and offsets
  this with the WTR/RTR specific delay backoff value (from the base delay number) 
  and gives the final path delay that is given to FW. In FTM mode this API 
  allows to directly feed a override value that can be passed to the Modem 
  registers.

 
  @param rfm_dev
  RFM_DEVICE0 or RFM_DEVICE2 based on the current device that is being used

  @param delay_backoff
  Delay backoff value corresponding to the different NPLER plan that will
  offset the base delay value for the WTR/RTR paths.

  @param path_delay
  Pointer to the final path_delay variable that is finally updated in this API.

  @retval None
*/

void rfgsm_nv_calc_path_delay(rfm_device_enum_type rfm_dev, uint16 delay_backoff, int16 *path_delay)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }
  
  tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;
   
   /* Override the current path delay if we are doing delay cal */

   if( rfgsm_core_handle_ptr->ftm_path_delay_cal_override )
   {
     *path_delay = rfgsm_core_handle_ptr->ftm_path_delay_override_val;
   }
   else
   {
      *path_delay = (tx_static_nv_data_ptr->polar_path_delay) - delay_backoff;
   }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This function is to fetch the PA_EN start and stop offset adj values whatever
  is populated in the Tx timing NV items

  @details
  This function is to fetch the PA_EN start and stop offset adj values whatever
  is populated in the Tx timing NV items
 
  @param on_off
  Boolean value to indicate if we want to get the start or the stop adjust
  timing values

  @param timing
  Parameter that is used for getting the PA_EN timing value - start or the
  stop offset adjust values

  @retval None
*/

void rfgsm_nv_get_pa_en_timing
(
  boolean on_off,
  int16 *timing
)
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;
   
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(RFM_DEVICE_0);
   
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }
   
  tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;

  if(on_off)
  {
    *timing = tx_static_nv_data_ptr->tx_timing_data.pa_en_start_offset_adj;
  }
  else
  {			
    *timing = tx_static_nv_data_ptr->tx_timing_data.pa_en_stop_offset_adj;
  }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This function is to fetch the Kv parameters (cal channels and the Kv code value
  if it is populated in the NV

  @details
  This function is to fetch the Kv parameters (cal channels and the Kv code value
  if it is populated in the NV

  @param band
  RFGSM band for which the Kv cal paramters are being requested

  @param cal_chan1
  Pointer to the variable where the cal channel1 in NV is returned

  @param cal_chan2
  Pointer to the variable where the cal channel2 in NV is returned

  @param kv_cal_value
  Pointer to the variable where the KV code value in NV is returned
  
  @retval
*/

void rfgsm_nv_get_kv_params
(
  rfcom_gsm_band_type band,
  uint16 *cal_chan1,
  uint16 *cal_chan2, 
  uint32 *kv_cal_value
)
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  gsm_tx_static_nv_type *tx_static_nv_data_ptr = NULL;
   
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(RFM_DEVICE_0);
   
  if (rfgsm_core_handle_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return;
  }
   
  tx_static_nv_data_ptr = rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr;

  *cal_chan1 = tx_static_nv_data_ptr->kv_cal.kv_cal_low_chan;  /* low_chan */
  *cal_chan2 = tx_static_nv_data_ptr->kv_cal.kv_cal_high_chan; /* high_chan */

 /* ARFCN for GSM900 need to be mapped linearly */
  *cal_chan1 = rfgsm_map_arfcn_linearly( band, *cal_chan1 );
  *cal_chan2 = rfgsm_map_arfcn_linearly( band, *cal_chan2 );

  *kv_cal_value = tx_static_nv_data_ptr->kv_cal.kv_cal_value;
}

/*----------------------------------------------------------------------------*/


boolean rfgsm_nv_load_coex_tx_char_offset_info_tbl
(  
  gsm_coex_tx_char_offset_info_type *static_nv_struct,
  rfnv_item_id_enum_type tx_char_offset_info_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{

  nv_stat_enum_type     tx_char_offset_info_tbl_nv_status;
  rfnv_item_type tx_char_offset_data_buffer;
  rfnv_item_size_type tx_char_info_tbl_buffer_size = sizeof(rfnv_item_type);
  rfnv_item_size_type copy_size;

  if (static_nv_struct == NULL)
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_coex_tx_char_offset_info_tbl, NULL static_nv_struct ptr passed", 0);	
    return FALSE;
  }

  tx_char_offset_info_item = tx_char_offset_info_item + band;

  tx_char_offset_info_tbl_nv_status = rfcommon_rfnv_get_item( tx_char_offset_info_item,
                                                              &tx_char_offset_data_buffer,
                                                              &tx_char_info_tbl_buffer_size,
                                                              caller_tcb_ptr,
                                                              task_nv_wait_sig,
                                                              task_wait_func_ptr );

  if (tx_char_offset_info_tbl_nv_status == NV_DONE_S)
  {

    if (tx_char_info_tbl_buffer_size >= sizeof(gsm_coex_tx_char_offset_info_type))
    {
      copy_size = sizeof(gsm_coex_tx_char_offset_info_type);
    }
    else
    {

      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_char_offset_info_tbl, size of destination buffer passed exceeds max nv buffer size. ", 0);	
      copy_size = tx_char_info_tbl_buffer_size;
    }
    memscpy(static_nv_struct, sizeof(gsm_coex_tx_char_offset_info_type), &tx_char_offset_data_buffer, copy_size);
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_tx_char_offset_info_tbl, setting TX data to default 0", 0);	
    memset( static_nv_struct, 0, sizeof(gsm_coex_tx_char_offset_info_type) );
  }

  return TRUE;

}

/*----------------------------------------------------------------------------*/

boolean rfgsm_nv_load_coex_rx_char_offset_info_tbl
(  
  gsm_coex_rx_char_offset_info_type* static_nv_struct,
  rfnv_item_id_enum_type rx_char_offset_info_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     rx_char_offset_data_tbl_nv_status;
  rfnv_item_type rx_char_offset_data_buffer;
  rfnv_item_size_type rx_char_info_tbl_buffer_size = sizeof(rfnv_item_type);
  rfnv_item_size_type copy_size;

  if (static_nv_struct == NULL)
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_coex_rx_char_offset_info_tbl, NULL static_nv_struct ptr passed", 0);	
    return FALSE;
  }

  rx_char_offset_info_item = rx_char_offset_info_item + band;

  rx_char_offset_data_tbl_nv_status = rfcommon_rfnv_get_item( rx_char_offset_info_item,
                                                              &rx_char_offset_data_buffer,
                                                              &rx_char_info_tbl_buffer_size,
                                                              caller_tcb_ptr,
                                                              task_nv_wait_sig,
                                                              task_wait_func_ptr );

  if (rx_char_offset_data_tbl_nv_status == NV_DONE_S)
  {

    if (rx_char_info_tbl_buffer_size >= sizeof(gsm_coex_rx_char_offset_info_type))
    {
      copy_size = sizeof(gsm_coex_rx_char_offset_info_type);
    }
    else
    {

      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_rx_char_offset_info_tbl, size of destination buffer passed exceeds max nv buffer size. ", 0);	
      copy_size = rx_char_info_tbl_buffer_size;
    }
    memscpy(static_nv_struct, sizeof(gsm_coex_rx_char_offset_info_type), &rx_char_offset_data_buffer, copy_size);
  }
  else
  {                                                    

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_load_rx_char_offset_info_tbl, setting RX data to default 0", 0);	
    memset( static_nv_struct, 0, sizeof(gsm_coex_rx_char_offset_info_type) );
  }

  return TRUE;


}




/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free the memory allocated for NV structures.

  @details
  Free the memory allocated for NV structures. 
*/
void rfgsm_nv_deinit(rfm_device_enum_type rfm_dev)
{
  uint8 nv_path = 0;
  uint8 band = 0;
  rfgsm_nv_container_array_type nv_cont_array;
  uint8 i=0;

  //get nv container for first supported band on given logical device
  nv_path = rfgsm_nv_get_cmn_nv_container( rfm_dev);
  
  if (nv_path >= RF_GSM_NV_PATH_NUM)
  {
      RFGC_MSG_1(MSG_LEGACY_ERROR, "nv_path %d returned from rfgsm_nv_get_cmn_nv_container is invalid! Cannot de-init NV!", nv_path);
      return;
  }

  if (rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr != NULL)
  {
    modem_mem_free( rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr, MODEM_MEM_CLIENT_RFA );
    rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr = NULL;
  }

  for (band = RFGSM_NV_850; band < RFGSM_NV_MAX_SUPPORTED; band++)
  {

    nv_cont_array = rfgsm_nv_get_tx_rx_nv_container( rfm_dev, (rfcom_gsm_band_type)band);

    for (i = 0;i < 2;i++)
    {
      nv_path = nv_cont_array.nv_container[i];

      if (nv_path >= RF_GSM_NV_PATH_NUM)
      {
        continue;
      }

      if (rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr != NULL)
      {
        if (rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr->rx_static_nv_data_ptr != NULL)
        {
          modem_mem_free( rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr->rx_static_nv_data_ptr, MODEM_MEM_CLIENT_RFA );
          rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr->rx_static_nv_data_ptr = NULL;
          rfgsm_nv_common_config_storage.rx_buffer_count--;
        }

        if (rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr->tx_static_nv_data_ptr != NULL)
        {
          modem_mem_free( rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr->tx_static_nv_data_ptr, MODEM_MEM_CLIENT_RFA );
          rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr->tx_static_nv_data_ptr = NULL;
          rfgsm_nv_common_config_storage.tx_buffer_count--;
        }

        modem_mem_free( rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr, MODEM_MEM_CLIENT_RFA );
        rfgsm_nv_data[nv_path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr = NULL;
      
      rf_gsm_nv_config_storage.allocated--;
      }
    }
  }
}


/*!
  @brief
  returns the NV container for the common NVs.

  @details
  returns the NV container for the common NVs. The NV container is for Tx or RX of the first supported band
*/

rfgsm_nv_path_type rfgsm_nv_get_cmn_nv_container(rfm_device_enum_type rfm_dev)
{
  if (rfm_dev < RFM_MAX_WAN_DEVICES)
  {
    return(rfgsm_cmn_nv_container[rfm_dev]);
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_get_cmn_nv_container, Invalid rfm device:%d", rfm_dev);
    return RF_GSM_NV_PATH_NUM;
  }
}


/*
  @brief
 
  This function is called to generate the coexistence band enum mapping table rfgsm_coex_nv_band_map_tbl.
  This table will then be used in loading cal or static NV data for the split bands.
  
*/

void rfgsm_nv_populate_coex_band_map_tbl( void )
{

  rfc_band_split_info_type *band_split_cfg_ptr = NULL;
  uint8 num_split_bands = 0;
  rfm_device_enum_type rfm_dev;
  rfc_rxtx_enum_type rx_tx;
  rfcom_gsm_band_type rfgsm_com_band;
  int split_com_band;
  rfgsm_nv_supported_type rfnv_band;
  rfgsm_nv_supported_type split_rfnv_band;

  /* debug hooks for bootup NV loading*/
  memset( &nv_alloc_info, 0, sizeof(nv_alloc_type) );
  memset( &nv_container_array, 0xFF, sizeof(nv_container_array) );

  /*Initialize*/
  for (rfnv_band = 0; rfnv_band < RFGSM_NV_MAX_SUPPORTED; rfnv_band ++)
  {
    rfgsm_coex_nv_band_map_tbl[rfnv_band] = NULL;
  }

  /* Allocate memory*/
  for (rfnv_band = 0; rfnv_band < RFGSM_NV_MAX_SUPPORTED; rfnv_band ++)
  {
    rfgsm_coex_nv_band_map_tbl[rfnv_band] = 
      modem_mem_alloc( sizeof(rfgsm_coex_band_map_type), MODEM_MEM_CLIENT_RFA );

    if (rfgsm_coex_nv_band_map_tbl[rfnv_band] == NULL) // failed to allocate memory
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "Failed to create rfgsm_coex_nv_band_map_tbl entry for rfnv band: %d",
            rfnv_band );

      return;
    }
    else
    {
      /* Initialise Memory */
      memset( rfgsm_coex_nv_band_map_tbl[rfnv_band], 0, sizeof(rfgsm_coex_band_map_type) );
    }
  } //  for (rfnv_band = 0; rfnv_band < RFGSM_NV_MAX_SUPPORTED; rfnv_band ++)


  /*initialize table*/
  for (rfgsm_com_band = 0; rfgsm_com_band < RFCOM_NUM_GSM_BANDS; rfgsm_com_band ++)
  {

    rfnv_band = rfgsm_core_rfcom_band_to_nvband(rfgsm_com_band);
 
    for (num_split_bands = 0; num_split_bands< RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND; num_split_bands++)
    {
      rfgsm_coex_nv_band_map_tbl[rfnv_band]->rfnv_band[num_split_bands]= RFGSM_NV_MAX_SUPPORTED;
      rfgsm_coex_nv_band_map_tbl[rfnv_band]->split_band_index = 0;
    } // ^ for (num_split_bands = 0; num_split_bands< RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND; num_split_bands++)

    switch (rfnv_band)
    {
    case RFGSM_NV_850:
    case RFGSM_NV_900:
    case RFGSM_NV_1800:
    case RFGSM_NV_1900:
      rfgsm_coex_nv_band_map_tbl[rfnv_band]->main_rfnv_band = rfnv_band;
      /*For main bands with no split, update the split bands section*/
      rfgsm_coex_nv_band_map_tbl[rfnv_band]->rfnv_band[0] = rfnv_band;
      break;

    case RFGSM_NV_1800_B:
      rfgsm_coex_nv_band_map_tbl[rfnv_band]->main_rfnv_band = RFGSM_NV_1800;
      break;

    default:
      rfgsm_coex_nv_band_map_tbl[rfnv_band]->main_rfnv_band = RFGSM_NV_MAX_SUPPORTED;
      break;
    } // ^ switch (rfnv_band)
  } // for (rfgsm_com_band = 0; rfgsm_com_band < RFCOM_NUM_GSM_BANDS; rfgsm_com_band ++)

  /* Populate table*/
  for (rfgsm_com_band = 0; rfgsm_com_band < RFCOM_NUM_GSM_BANDS; rfgsm_com_band ++)
  {
    rfnv_band = rfgsm_core_rfcom_band_to_nvband(rfgsm_com_band);

    if (rfnv_band >= RFGSM_NV_MAX_SUPPORTED )
    {
      continue;
    }

    for (rfm_dev = 0;rfm_dev < RFM_MAX_WAN_DEVICES; rfm_dev++)
    {
      for (rx_tx = 0; rx_tx <RFC_CONFIG_RXTX_MAX; rx_tx++)
      {
        /*If split bands found, just update the table*/
        if ( rfc_band_split_cfg_data_get(rfm_dev,rx_tx,rfgsm_com_band, &band_split_cfg_ptr) != TRUE)
        {  
          continue;
        }

        /*loop over number of split bands*/
        for (num_split_bands = 0; num_split_bands< RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND; num_split_bands++)
        {
          split_com_band = band_split_cfg_ptr->band_info[num_split_bands].band;

          if( ( split_com_band != RFC_INVALID_PARAM) && (split_com_band < RFCOM_NUM_GSM_BANDS) )
          {
            split_rfnv_band = rfgsm_core_rfcom_band_to_nvband((rfcom_gsm_band_type) split_com_band);

            /* Update Split bands*/
            rfgsm_coex_nv_band_map_tbl[rfnv_band]->rfnv_band[num_split_bands]= split_rfnv_band;

            if (split_rfnv_band < RFGSM_NV_MAX_SUPPORTED)
            {
              /*Update split band index*/
              rfgsm_coex_nv_band_map_tbl[split_rfnv_band]->split_band_index = num_split_bands;

              /* Assumption is that the very first RFcom band in the band split sfg table is main rf com band*/             
              rfgsm_coex_nv_band_map_tbl[split_rfnv_band]->main_rfnv_band= rfgsm_core_rfcom_band_to_nvband((uint32)(band_split_cfg_ptr->band_info[0].band));

            } // ^ if (split_rfnv_band < RFGSM_NV_MAX_SUPPORTED)

          } // ^ if( ( split_com_band != RFC_INVALID_PARAM) && (split_com_band < RFCOM_NUM_GSM_BANDS) )

        } // ^ for (num_split_bands = 0; num_split_bands< RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND; num_split_bands++)

      } // ^ for (rx_tx = 0; rx_tx <RFC_CONFIG_RXTX_MAX; rx_tx++)

    } // ^ for (rfm_dev = 0;rfm_dev < RFM_MAX_WAN_DEVICES; rfm_dev++)

  } // ^ for (rfgsm_com_band = 0; rfgsm_com_band < RFCOM_NUM_GSM_BANDS; rfgsm_com_band ++)

}




boolean rfgsm_nv_get_coex_band_map_tbl(rfgsm_nv_supported_type nv_band,rfgsm_coex_band_map_type *band_info_ptr)
{
 boolean status = FALSE;

  if (nv_band < RFGSM_NV_MAX_SUPPORTED )
  {
    if((rfgsm_coex_nv_band_map_tbl[nv_band] != NULL)&& (band_info_ptr != NULL))
    {
      /* We are using memscpy only because rfgsm_coex_band_map_type doesn't have any ptr in the structure*/
      memscpy(band_info_ptr, sizeof(rfgsm_coex_band_map_type),rfgsm_coex_nv_band_map_tbl[nv_band],sizeof(rfgsm_coex_band_map_type) );
      status = TRUE;
    }
    else
    {

      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_get_coex_band_map_tbl, Source or destination ptr is NULL, nv band:%d", nv_band);
    }

  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_nv_get_coex_band_map_tbl, NV band passed is invalid:%d", nv_band);

  }


  return status;
}

/*
  @brief
 
  This function initalizes nv containers to NULL in order to check whether container 
  was created in rfgsm_nv_alloc_band_nv_buf.
  
*/


void rfgsm_nv_data_structure_init(void)
{
   
   rfgsm_nv_path_type path;
   rfgsm_nv_supported_type	band;  
   

   for(path=0; path <RF_GSM_NV_PATH_MAX; path++)
   {
      for(band=0; band < RFGSM_NV_MAX_SUPPORTED; band++)
      {
      	
		rfgsm_nv_data[path].rfgsm_nv_band_node[band].rfgsm_nv_band_data_buffer_ptr = NULL;
      	
      }
   }

}


/*===========================================================================
FUNCTION RFGSM_NV_SORT_RX_CAL_DATA

 @brief
  Sort gsm Rx calibration data and take the channel number discontinuity into account

  @details
  This function sort gsm Rx calibration data and take the channel number discontinuity into account,
  Rx cal data will be sorted by channel number ascending order
  
  @param rx_cal_nv_data
  Rx cal data retrived from NV

  @param band
  RFGSM NV band type for which rx_cal_nv_data is 

  @param gsm_rx_cal_nv_read_size
  rx_cal_nv_data memory size
  
  @return
   Null
===========================================================================*/
void rfgsm_nv_sort_rx_cal_data(gsm_rx_cal_data_nv_type* rx_cal_nv_data, rfcom_gsm_band_type band, rfnv_item_size_type gsm_rx_cal_nv_read_size)
{
  gsm_rx_cal_data_nv_type* temp_rx_cal_nv_data;
  uint8 arfcn_idx = 0;
  uint8 lna_state_idx = 0;
  uint16 temp_arfcn = 0;
  uint8 chan_order[RFCOM_GSM_NUM_RX_CAL_CHAN];
  uint8 table_idx1;
  uint8 table_idx2;
  uint8 temp_order;

  temp_rx_cal_nv_data = (gsm_rx_cal_data_nv_type *)modem_mem_alloc( sizeof(gsm_rx_cal_data_nv_type), MODEM_MEM_CLIENT_RFA );

  if (temp_rx_cal_nv_data == NULL)
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_sort_rx_cal_data - failed to allocate memory for %d bytes.", sizeof(gsm_rx_cal_data_nv_type));
     return;
  }  
  
  memset(temp_rx_cal_nv_data, 0, sizeof(gsm_rx_cal_data_nv_type));
  memscpy(temp_rx_cal_nv_data, sizeof(gsm_rx_cal_data_nv_type), rx_cal_nv_data, gsm_rx_cal_nv_read_size);
  
  if(temp_rx_cal_nv_data->rx_cal_chan_size > RFCOM_GSM_NUM_RX_CAL_CHAN)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RX cal channel is out of range: %d", temp_rx_cal_nv_data->rx_cal_chan_size);
  }
  
  for(arfcn_idx=0; arfcn_idx<temp_rx_cal_nv_data->rx_cal_chan_size; arfcn_idx++)
  {
    temp_arfcn = temp_rx_cal_nv_data->rx_cal_chan_list[arfcn_idx];
    temp_rx_cal_nv_data->rx_cal_chan_list[arfcn_idx] = rfgsm_map_arfcn_linearly(band,temp_arfcn);
    chan_order[arfcn_idx] = arfcn_idx;
  }
   
  for(table_idx1=0; table_idx1<temp_rx_cal_nv_data->rx_cal_chan_size-1; table_idx1++)
  {
    for(table_idx2=table_idx1+1; table_idx2<temp_rx_cal_nv_data->rx_cal_chan_size; table_idx2++)
    {
      if(temp_rx_cal_nv_data->rx_cal_chan_list[table_idx1] > temp_rx_cal_nv_data->rx_cal_chan_list[table_idx2])
      {
        temp_order = chan_order[table_idx1];
        chan_order[table_idx1] = chan_order[table_idx2];
        chan_order[table_idx2] = temp_order;
        temp_arfcn = temp_rx_cal_nv_data->rx_cal_chan_list[table_idx1];
        temp_rx_cal_nv_data->rx_cal_chan_list[table_idx1] = temp_rx_cal_nv_data->rx_cal_chan_list[table_idx2];
        temp_rx_cal_nv_data->rx_cal_chan_list[table_idx2] = temp_arfcn;
      }
    }
  }

  for(arfcn_idx=0; arfcn_idx<temp_rx_cal_nv_data->rx_cal_chan_size; arfcn_idx++)
  {
    rx_cal_nv_data->rx_cal_chan_list[arfcn_idx] = temp_rx_cal_nv_data->rx_cal_chan_list[arfcn_idx];
    for(lna_state_idx=0;lna_state_idx<RFNV_GSM_NUM_LNA_STATE; lna_state_idx++)
    {
      rx_cal_nv_data->rx_freq_comp_data[lna_state_idx].freq_comp_data[arfcn_idx] = temp_rx_cal_nv_data->rx_freq_comp_data[lna_state_idx].freq_comp_data[chan_order[arfcn_idx]];
    }
  }
  modem_mem_free( temp_rx_cal_nv_data, MODEM_MEM_CLIENT_RFA );
}

/*!===========================================================================
  FUNCTION RFGSM_NV_CHECK_DEBUG_NV

  @brief
  This API is intended to provide a robust and extendible method to check debug NV settings.
 
  @details
  This API requires a device (to get the nv path) and a debug_nv code of type rfgsm_debug_nv_supported_type.
  The debug_nv code is used to access rfgsm_debug_nv_masks, which defines an index into the GSM debug nv,
  and a bit to mask within that index. This is indended to discourage the use of 'magic numbers' in driver code,
  and to encourage the definition of well documented debug NV entries within rfgsm_nv_cmn files.
 
  @note
  Other APIs to access the debug/spare NV should be deprecated wherever possible.
  
  @param rfm_dev
  RFM logical device

  @param debug_nv
  Debug NV code which is used to determine the debug NV word and bit to mask
  
  @return boolean
  TRUE indicates that debug nv bit is set, FALSE indicates it is cleared
 
===========================================================================*/
boolean rfgsm_nv_check_debug_nv ( rfm_device_enum_type rfm_dev, rfgsm_debug_nv_supported_type debug_nv )
{  
  rfgsm_nv_cmn_data_type *rfgsm_nv_cmn_data_buffer_ptr;
  rfgsm_nv_path_type nv_path;
  uint32 nv_bitmask, nv_item;
  uint8 nv_idx;

  /* Screen out invalid codes */
  if ( debug_nv >= RFGSM_DEBUG_NV_MAX_SUPPORTED )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_check_debug_nv, invalid debug nv code %d", debug_nv );
    return FALSE;
  }

  nv_path = rfgsm_nv_get_cmn_nv_container( rfm_dev);

  rfgsm_nv_cmn_data_buffer_ptr = rfgsm_nv_data[nv_path].rfgsm_nv_cmn_node.rfgsm_nv_cmn_data_buffer_ptr;

  if (rfgsm_nv_cmn_data_buffer_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_check_debug_nv, NULL ptr retrieved for device %d", rfm_dev );
    return FALSE;
  }

  /* Extract index and bit to mask using debug_nv code */
  nv_idx = rfgsm_debug_nv_masks[debug_nv].debug_nv_index;
  nv_bitmask = rfgsm_debug_nv_masks[debug_nv].debug_nv_bitmask;

  /* Screen out incorrectly defined NV indexes */
  if ( nv_idx >= GSM_DEBUG_NV_MAX_SIZE )
  {
    MSG_3( MSG_SSID_RF, 
           MSG_LEGACY_ERROR, 
           "rfgsm_nv_check_debug_nv retrieved index %d for nv code %d exceeds size of debug nv %d",
           nv_idx,
           debug_nv,
           GSM_DEBUG_NV_MAX_SIZE);
    return FALSE;
  }

  if ( debug_nv > RFGSM_DEBUG_NV_ITEM_BOUNDARY )
  {
    /* Check against the RFNV_GSM_RESERVED_I NV item */
    nv_item = rfgsm_nv_cmn_data_buffer_ptr->gsm_rf_reserved_nv[nv_idx];
  }
  else if ( debug_nv < RFGSM_DEBUG_NV_ITEM_BOUNDARY )
  {
    /* Check against the RFNV_GSM_DEBUG_I NV item */
    nv_item = rfgsm_nv_cmn_data_buffer_ptr->gsm_debug_nv[nv_idx];
  }
  else 
  {
    /* We must have been passed the boundary code which is not valid */
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_nv_check_debug_nv, boundary nv code %d passed, not valid", debug_nv );
    return FALSE;
  }

  /* Bitwise mask against debug nv word */
  if ( nv_item & nv_bitmask )
  {
    /* Debug NV is set */
    return TRUE;
  }
  
  /* Debug NV is not set */
  return FALSE;
}
