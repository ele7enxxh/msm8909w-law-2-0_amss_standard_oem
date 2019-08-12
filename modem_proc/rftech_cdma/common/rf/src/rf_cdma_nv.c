/*!
  @file
  rf_cdma_nv.c

  @details
  This file contains the definitions and declartions used by RF CDMA NV Manager
 
  @addtogroup RF_CDMA_COMMON_NV
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_nv.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/16/15   vr     CDMA predictive clipping NV based power thresh support
12/22/14   vc      Printing Max Power values with in array bounds
10/07/14   weiz    Fix compile warning
10/06/14   weiz	   Support compressed PA static NV item
09/30/14   zhw     Compiler warning fix
09/17/14   spa     Determine NV path to be Rx only or RxTx based on RFC
09/10/14   vr      RUMI support for NV initialization
09/08/14   spa     Avoid double cleanup of tx multi lin in rf_cdma_nv_cleanup
07/18/14   dr      [RUMI] Jolokia Rumi bringup changes
07/08/14   cd      Load HDR Access Rise swpts correctly from PA static NV
06/12/14   cd      Use invalid PA range to ensure PA programming is accurate 
                   for valid PA states
05/27/14   cd      - Load Pin/Pout data from PA static NV item
                   - Deallocate PA static NV data struct from heap once loaded
05/21/14   spa     Remove deprecated pin/pout part of NV (now in common)
05/02/14   cd      8-PA state functionality updates
04/23/14   cd      Support for 8 PA state functionality
04/21/14   JJ      Check rfcommon_fbrx_nv_retrieve_1x_data() status
03/17/14   pk      Free linearizer mem when freeing larger NV structures
02/21/14   JJ      Fixed a compilation warning
02/20/14   JJ      Hooked up api to loaded FBRx NVs
01/24/14   zhw     Pin Cal vs Freq support
01/22/14   cc      CDMA linearizer reloading memory leak fix
01/08/14   zhw     CDMA NV loading memory leak fix 
10/25/13   spa     Set use_fake_nv_init flag to FALSE for D3925
10/24/13   spa     C0/C1 NV load done based on use_fake_nv_init debug flag
10/22/13   cd      Added functionality for fake multilin v3 initialization
10/15/13   APU     Temp DoRevB LNA switchpoints for Bolt BU.
10/01/13   JJ      Load rf_cdma_nv_load_c0_fake_init for Chain 1
09/12/13   sar     Fixed compiler warnings.
08/19/13   JJ      Fix a typo in fake nv 
08/12/13   Saul    XPT. DPD Scaling.
08/06/13   JJ      Load rf_cdma_nv_load_c0_fake_init for Bolt bringup 
07/30/13   sbo     Add support for BC14
07/25/13   zhw     Only allocate Tuner CL NV memory once
07/24/13   zhw     Memset Tuner CL NV memory to 0 for initialization
07/24/13   zhw     Allocate memory for Tuner CL NV
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/03/13   zhw     Antenna Tuner CL control data NV support
06/19/13   APU     Added new NV for ET delay. 
06/10/13   JJ      Remove code that loads legacy NVs  
06/04/13   cd      ET delay value NV must be loaded as static NV
05/30/13   JJ      Remove un-necessary rfm_band to sys_band conversion
05/29/13   sty     Deleted rf_cdma_nv_load_antenna_tuner() and refs to it
05/23/13   cd      Therm bins characterized NV must be loaded as static NV
05/21/13   sty     Return correct result in rf_cdma_nv_load_tx_chan_list
05/16/13   cd      Temp Comp - Fixed scenario where both legacy and new Temp 
                   comp NVs are present in the phone
04/28/13   cd      Temp Comp - Fixed compiler warnings
04/28/13   cd      Temp Comp - Load char chan list and therm bins NV data into 
                   common Tx linearizer data structures
04/22/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
                   - Load static temp comp NVs into new common data structs
04/05/13   cd      Pin polarity will be handled in NV by characterization. 
                   No need to invert in SW.
04/04/13   bmg     Zero-initialize NV data for general safety/debugging
03/29/13   cd      XPT Temp Comp, Pin NV polarity needs to be handled 
                   correctly
03/19/13   APU     Replaced memcpy with memscpy.
03/20/13   cd      XPT Temp comp support
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.
03/11/13   cd      Add support for loading new lin vs temp vs freq Pin and 
                   Pout NV items
02/22/13   Saul    Tx ET Path Delay NV support.
12/17/12   sty     Null pointer check ni rf_cdma_nv_load_dpd_cfg
12/03/12   APU     Added a one time check for therm cal monotonicity. 
11/28/12   aki     Added rf_cdma_nv_cleanup
09/25/12   zhw     Remove old hack from NikeL
09/24/12   zhw     Remove fake NV hack for Chain 1 
09/20/12   sty     Deleted using fake_NVs - use NV read from qcn
08/28/12   sty     Deleted unused variable 
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/30/12   CC/hdz  Added c2k LNA offset check
07/26/12   APU     Added Enhanced PA backoff support.
07/09/12   cri     Added remaining support for loading EPT config param from NV 
05/17/12   cd      Fixed merge error
05/15/12   cd      Added functionality to populate MCDO linearizer with first 
                   sweep data when loading calv2 Tx cal NV items 
05/14/12   cd      Refactor NV loading functions to generalize across all 
                   chains and separate static vs. calibrated NV loading
05/01/12   cri     Updated EPT linearizer structure and processing
04/10/12   id      Support for accessing Multi Lin global structure
04/04/12   cri     Support for processing EPT linearizers 
03/23/12   cri     Support for loading EPT linearizers from NV
03/22/12    id     Added support for getting Tx freq to chan conversion handler
03/22/12   sty     Deleted redundant rf_cdma_nv_set_pa_range_map_arr()
03/12/12   cri     EPT linearizer support in tx_band_cal_type
03/08/12   cd      Load Tx GTR Threshold NV data
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
02/29/12   cd      Moved PA backoffs related to inactive data swpts into NV 
                   loading functionality
02/10/12   sty     Load Spur table Nv for chain-2
02/08/12   sty     Added support to read NVs for spurs
02/02/12   cd      Fixed errors in loading LPM HDET NV
01/30/12   APU     Klockwork Fix.
01/30/12   APU     Support for variable number of HDR PWR LIMITS NV items.
01/27/12   APU     Added new NV items for HDR PWR LIMITS.
01/25/12   sty     revert previous change since rfnv_items.h has not been changed
01/25/12   sty     Added APIs to load spur info table
                   Added rf_cdma_nv_load_1x_spur_info and 
                   rf_cdma_nv_load_hdr_spur_info
01/18/12   aro     Added Tx APT NV load for HDR on C2 chain
12/19/11   aro     Support to load C2 and C3 HDR Rx/Tx Switchpoint NV
12/12/11   bmg     Move PA range mapper loading to the beginning of NV to
                   insure that it is set for RF calibration.  No other NV
                   items are needed to calibrate.
12/12/11   APU     Since diversity path has only 1 linearity state LM DVGA 
                   should adjustment should not use the anything other than 
                   the only available LIN STATE  
12/09/11   aro     Fixed the failure to load CDMA NV items seem after loading
                   CalChannelList (after Erasenand). CalChannel List is
                   not expected to be active after erasenand.
12/08/11   bmg     Adjust Rx cal that looks like "old data" for new DVGA offset
                   with RxLM 1.6.5
12/06/11   bmg     Added data_valid and last_nv_item_read to NV data for each
                   device + band.
12/02/11   cd      Update default functionality for Tx switchpoints 
12/02/11   cd      Load Tx MultiLin APT Adjust static NV data 
12/01/11   cd      Load new Tx switchpoint NV for 1x and HDR 
11/11/11   Saul    Thermal Backoff Support.
10/25/11   adk     Clarified the definition of RF_CDMA_NV_MAX_STRUCTURES.
10/20/11   adk     Added support for SV diversity.
10/18/11   cd      Fixed compiler warning
10/18/11   cd      Added functionality to load linearizer temp comp NV and 
                   therm bins NV for C0 and C2
10/18/11   cd      Removed linearizer loading into old data structures and
                   renamed the new functions to be default
10/12/11   bmg     Added lim vs temp and SAR lim vs temp NV loading
10/10/11   cd      Additional updates for BC4 and BC10
09/20/11   cd      Added BC4 and BC10 NV loading support 
09/20/11   cd      Fixed usage of rfnv_item_type by allocating memory on the 
                   heap in order to avoid stack overflow 
09/20/11   aro     Added NULL pointer check
09/19/11   aro     Fixed boot-up crash due to stack overflow
09/19/11   cd      Load legacy Tx switchpoints
09/16/11   sar     Fixed compiler errors. 
09/12/11   aro     Added support to load BC0 antenna tuner data for all chains
09/12/11   aro     Added support to load BC1 antenna tuner data for all chains
09/12/11   aro     Added support to load BC6 antenna tuner data for all chains
09/12/11   aro     Added support to load BC15 antenna tuner data for all chains
08/31/11   cd      Pass item size into rfcommon_rfnv_get_item function to 
                   support efs_put updates.
08/30/11   cd      Add loading of multi-linearizer NV using common functionality
08/29/11   cd      Load Enc BTF delay NV for chains 0 and 2
08/22/11   cd      Updated HDET NV nomenclature
08/17/11   cd      Fixed NV load changes made for KW errors with correct data
                   type
08/17/11   cd      Fixed KW warnings
08/15/11   cd      - Moved multi-linearizer field under tx_cal data
                   - Provide conversion for invalid LUT/Pwr delimiters in old 
                   NV to new internal data representation
                   - Use 65535 as invalid RGI threshold and 128 as max to 
                   present single freq comp across all bins for old NV
                   - Functionality to assign ref chan frequency in the case of 
                   old NV, where single linearizer exists at ref channel
                   - API to return immutable pointer to multi-linearizer 
                   internal data
08/11/11   cd      Restored pack struct types to header to file to support 
                   packing override for off-target test cases
08/11/11   cd      - Moved pack struct types to source file
                   - Added functionality to load HDET calibration NV items for 
                   Chain0 and Chain2. LPM HDET NV functionality is unused.
08/09/11   cd      Added BC15 NV loading support
08/04/11   cd      Old linearizer NV is loaded into multi-linearizer data 
                   structures in sorted increasing RGI order. Fixed other 
                   configuration issues based on fall-back NV design
08/04/11   cd      Fixed compiler warning for unused variable
08/04/11   cd      Fixed compilation error
08/03/11   cd      Added support to load old Linearizer NV data into common 
                   data types created for APT and multilinearizer support
08/03/11   cd      Removed common PA R Map NV loading. CDMA load functionality 
                   fails if band-specific PA R Map NV is not active.
08/02/11   cd      Moved band-specific NV loading functions to common file in 
                   order to remove redundancy. Only chain-specific  
07/25/11   cd      Removed error fatal for unsupported bands
07/19/11   cd      Added intelliceiver cal data load functionality
06/13/11   cd      Changed Rx Switchpoint loading design for inactive NV items.
06/10/11   cd      Changed static NV items map to array of structs of size 
                   RF_CDMA_NV_PATH_NUM, so get API takes path as parameter
06/09/11   cd      Updated PA R Map load functionality to use common NV item if
                   band-specific NV not available.
06/08/11   cd      Fixed compiler warning
06/08/11   cd      Added rf_cdma_nv_load_tx_pa_r_map() to load band specific 
                   Tx PA R Map
06/06/11   cd      - Deprecated rf_cdma_nv_device_to_path()
                   - rf_cdma_nv_add_cal_chan() takes nv path as parameter
06/06/11   cd      Changed cal items map to array of structs, so get API takes 
                   path as parameter
06/02/11   cd      - Added support for HDR NV pointers in C0 and C1 load 
                   functions
                   - Added functionality to load Rx switchpoint NV items. 
                   Currently used only for C2
                   - Added rf_cdma_nv_get_static_nv_table to return pointer 
                   to static NV items
06/02/11   aro     Changed the NV Path check to enabled SV
05/27/11   cd      Support to load Rx LRU channel list into new variable
05/24/11   cd      Added support to load SV NV items. Tx loading currently 
                   supports v2 functionality for new SV NV configuration
05/10/11   cd      Removed rf_cdma_utils_get_freq_bin()
05/10/11   cd      Added rf_cdma_nv_get_tx_cal() to return Tx cal data pointer 
                   outside of NV
05/02/11   cd      Added
                   - rf_cdma_nv_load_tx_cal()
                   - rf_cdma_nv_load_tx_rgi_lin_cal()
                   - rf_cdma_nv_load_tx_lin_freq_cal()
                   to load Tx linearizer NV data into RF data structures
04/26/11   cd      - Added load_tx_cal_chan_list functionality
                   - Code cleanup
04/22/11   cd      Replaced old Rx cal channel list with new data structure
04/21/11   cd      Removed old NV structure LNA offsets
04/20/11   cd      KW fixes to handle NULL return value and buffer overflow
04/14/11   cd      Changed parameter type for gain state from int to 
                   rfm_lna_gain_state_type
04/13/11   cd      Extended rx cal load to all LNA gain states. Renamed 
                   rf_cdma_nv_load_rx_g0_cal() to rf_cdma_nv_load_rx_cal()
03/24/11   bmg     Added interpolation to rf_cdma_nv_get_gain_cal()
03/23/11   bmg     Added common rf_cdma_nv_load_rx_chan_list() function.
03/22/11   bmg     Removed unused function rf_cdma_nv_load_dvga_offset_data().
                   Added BC1 and BC0 chain 0 and chain 1 NV load.
03/22/11   bmg     Added HDR config structure allocation and query functions.
                   Added internal rf_cdma_nv_get_mutable_common_data() function
                   for updating calibration information.
                   Added rf_cdma_nv_update_gain_cal() and rf_cdma_nv_get_gain()
                   for setting and querying gain state cal data.
                   Converted channel list maintnence functions to use new data
                   structures for G0 cal data.
03/21/11   bmg     Moved rf_cdma_nv_load_rx_g0_cal() to common file.
                   Added gain cal support for different sized NV items.
                   Added BC0 C1 config structure load support.
03/20/11   bmg     Added NV config allocation for chain 1 bands
03/19/11   bmg     Fixed porting bug in rf_cdma_nv_add_cal_chan(), where adding
                   a channel already in NV would not return the correct index
                   for the matching column.
03/16/11   aro     Renamed enum to RF_CDMA_TX_LINEARIZER_NUM
03/10/11   sty     Added rf_cdma_nv_set_pa_range_map_arr()
03/08/11   bmg     Added inital BC0 NV loading for the new config structure.
                   Changed rf_cdma_nv_alloc() to return pointers to the non-
                   const 1x and HDR config structures that can be used to load
                   from NV.
03/03/11   sty     Added implementation for rf_cdma_nv_get_common_table()
02/25/11   aro     Removed unused function
02/24/11   aro     [1] Added suuport for Common / BC0 NV load
                   [2] Renamed type name to rf_cdma_nv_band_items_type
02/24/11   bmg     Added Path0, BC0 1x configuration initialization.
                   Added parameter checking for rf_cdma_nv_get_1x_data().
                   Added Path0, BC1, 3, 4, 5, 6, 10, 14, and 15 1x config init.
                   Extracted common method rf_cdma_nv_alloc()
02/23/11   bmg     Added rf_cdma_nv_get_1x_data() config structure query
                   function.
                   Added data types for (path, band) NV lookup index
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added function to add channel to Calibration
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/30/11   aro     Removed Function Table to replace with CASE
01/03/11   aro     Changed the return value to TRUE
12/29/10   aro     Added function to return Frequency Bin for given band/chan
12/29/10   aro     Added Virtual Table for NV Dispatches
12/17/10   aro     Added NV Load function
12/13/10   aro     Doxygen Update
12/09/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "amssassert.h"
#include "modem_mem.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_nv_bc0.h"
#include "rf_cdma_nv_bc1.h"
#include "rf_cdma_nv_bc4.h"
#include "rf_cdma_nv_bc6.h"
#include "rf_cdma_nv_bc10.h"
#include "rf_cdma_nv_bc14.h"
#include "rf_cdma_nv_bc15.h"
#include "rfm_cdma_power_mode_types.h"
#include "rfcommon_msg.h"
#include "nv.h"
#include "task.h"
#include "rfcommon_nv.h"
#include "rfcommon_nv_mm.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_math.h"
#include "rf_cdma_mdsp_types.h"
#include "rfm_internal.h"
#include "rf_cdma_temp_comp.h"
#include "math.h"
#include "rfcommon_core_txlin_types.h"
#include "rfcommon_fbrx_api.h"

/* Functions to Un-Compress compressed NV Items */
#include "zlib.h"

/* memscpy */
#include "stringl.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA config index entry type per band.

  @details
  This structure is the element type in the second level index mapping
  (path, band) to 1x and HDR data structures.  Since NV is simultaneously
  loaded for 1x and HDR, each band entry will contain the pointers to
  both systems.
*/
typedef struct rf_cdma_config_band_index_s
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
    Pointer to the 1x configuration structure for this index entry.
  */
  rf_1x_config_type*       ptr_1x;

  /*!
    @brief
    Pointer to the HDR configuration structure for this index entry.
  */
  rf_hdr_config_type* ptr_hdr;
} rf_cdma_config_band_index_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA config index entry type per device.

  @details
  This structure is the element type in the first level index mapping
  (path, band) to 1x and HDR data structures.  Each device has an array
  of band index types for all possible bands in the CDMA driver.
*/
typedef struct rf_cdma_config_device_index_s
{
  /*!
    @brief
    Array of band index entries for each potential band in the driver.
  */
  rf_cdma_config_band_index_type band_index[RFM_CDMA_MAX_BAND];
} rf_cdma_config_device_index_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The root level of the index mapping (path, band) to the 1x and HDR
  data structures.

  @details
  This variable contains the band mappings for all supported NV
  paths in the system.  The NV path in this structure corresponds
  to the path or chain definitions in the NV enumerations, which
  are numbered from 0..N-1, where N is the maximum number of 1x
  and/or HDR paths that can be present in the system.

  The data structure is initialized to 0 on bootup, representing
  an empty index with NULL pointers for all devices and bands.
  As paths and bands are initialized, the structure is populated
  with non-NULL pointers in the cells that have been initailized.

  This NV path does not necessary match the RFM logical device that
  RF clients access the RF driver with.  For instance, a logical
  to NV mapping might consist of:
    RFM_DEVICE_0 -> RF_CDMA_NV_PATH_0
    RFM_DEVICE_1 -> RF_CDMA_NV_PATH_1
    RFM_DEVICE_2 -> RF_CDMA_NV_PATH_2
    RFM_DEVICE_4 -> RF_CDMA_NV_PATH_3
*/
static rf_cdma_config_device_index_type
  rf_cdma_config_device_index[RF_CDMA_NV_PATH_NUM];

/*!
  @brief
  The static maximum number of simultaneous bands that may be used by the
  CDMA configuration.
 
  @details
  No current target requires more than 6 bands at once.
*/
#define RF_CDMA_NV_BAND_NUM  6

/*!
  @brief
  The static maximum number of data structures that can be allocated.
 
  @details
  Currently support 4 chains (RF_CDMA_NV_PATH_NUM) and 6 bands
  (RF_CDMA_NV_BAND_NUM)
*/
#define RF_CDMA_NV_MAX_STRUCTURES (RF_CDMA_NV_PATH_NUM * RF_CDMA_NV_BAND_NUM)

/*!
  @brief
  Static storage allocation for the CDMA configuration data that is
  common between 1x and HDR.
*/
static struct
{
  /*!
    @brief
    The current number of structures that have been allocated.

    @details
    This item starts at 0 during initialization and is incremented
    every time a buffer is requested.  If allocated becomes equal
    to RF_CDMA_NV_MAX_STRUCTURES, NV memory is exhausted.
  */
  int allocated;

  /*!
    @brief
    An array of configuration structures that may be returned
    by the allocator.

    @details
    The buffer indexed by the allocated structure member will
    be the next buffer returned by the allocator, as long as
    allocated is less than RF_CDMA_NV_MAX_STRUCTURES.
  */
  rf_cdma_config_type *buffers[RF_CDMA_NV_MAX_STRUCTURES];
} rf_cdma_nv_common_config_storage;

/*!
  @brief
  Static storage allocation for the CDMA configuration data that is
  specific for 1x.
*/
static struct
{
  /*!
    @brief
    The current number of structures that have been allocated.

    @details
    This item starts at 0 during initialization and is incremented
    every time a buffer is requested.  If allocated becomes equal
    to RF_CDMA_NV_MAX_STRUCTURES, NV memory is exhausted.
  */
  int allocated;

  /*!
    @brief
    An array of configuration structures that may be returned
    by the allocator.

    @details
    The buffer indexed by the allocated structure member will
    be the next buffer returned by the allocator, as long as
    allocated is less than RF_CDMA_NV_MAX_STRUCTURES.
  */
  rf_1x_config_type *buffers[RF_CDMA_NV_MAX_STRUCTURES];
} rf_cdma_nv_1x_config_storage;

/*!
  @brief
  Static storage allocation for the CDMA configuration data that is
  specific for HDR.
*/
static struct
{
  /*!
    @brief
    The current number of structures that have been allocated.

    @details
    This item starts at 0 during initialization and is incremented
    every time a buffer is requested.  If allocated becomes equal
    to RF_CDMA_NV_MAX_STRUCTURES, NV memory is exhausted.
  */
  int allocated;

  /*!
    @brief
    An array of configuration structures that may be returned
    by the allocator.

    @details
    The buffer indexed by the allocated structure member will
    be the next buffer returned by the allocator, as long as
    allocated is less than RF_CDMA_NV_MAX_STRUCTURES.
  */
  rf_hdr_config_type *buffers[RF_CDMA_NV_MAX_STRUCTURES];
} rf_cdma_nv_hdr_config_storage;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure type containing RGI and Power data from Linearizer NV. 

  @details
  This type is used to populate an array of structs with the data obtained
  from linearizer NV.
*/
typedef struct
{
  /*!
    @brief
    RF Gain word Index.
  */
  uint16 rgi;

  /*!
    @brief
    Power in dBm10.
  */
  int16  pwr;
} rgi_pwr_lin_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Common NV calibration table.  Initially contains default values. If these
  items are not read from NV, the default values are used.
*/
static rf_cdma_nv_common_items_type rf_cdma_nv_common_tbl = {

  /* test_code_ver
  */
  RF_TEST_CODE_VER,

  /* sys_sw_ver
  */
  RF_SYS_SW_VER,

  /* rf_cal_ver
  */
  RF_RF_CAL_VER,

  /* rf_config_ver
  */
  RF_CONFIG_VER,

  /* rf_cal_date
  */
  RF_CAL_DATE,

  /* rf_nv_loaded_date
  */
  RF_NV_LOADED_DATE,

  /* rf_cal_dat_file
  */
  RF_CAL_DAT_FILE,

  /* Defaults for int16 rfnv_cdma_thermal_max_limit[NV_TEMP_TABLE_SIZ] */
  RF_THERMAL_MAX_LIMIT_DEFAULT_V
};

/*----------------------------------------------------------------------------*/
/*! List of all Common CDMA NV items */
static 
rfcommon_rfnv_items_list_type rf_cdma_nv_common_item_list[] =
{
#ifdef FEATURE_RFCAL_VERSION
    /*  DMSS S/W Version                                                        */
    { RFNV_SYS_SW_VER_I,                    NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  RF CAL Program version                                                  */
    { RFNV_RF_CAL_VER_I,                    NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  RF CAL configuration file version                                       */
    { RFNV_RF_CONFIG_VER_I,                 NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Date RF calibration was done                                            */
    { RFNV_RF_CAL_DATE_I,                   NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Date RF calibration data loaded                                         */
    { RFNV_RF_NV_LOADED_DATE_I,             NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Name of RFCAL .dat                                                      */
    { RFNV_RF_CAL_DAT_FILE_I,               NV_ITEM_ATTRIB_NON_CRTCL_BMSK       }
    /*  Data Services domain name                                               */
#endif
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper API to return the corresponding device associated with an NV path
 
  @param nv_path
  The NV path requested
 
  @return
  Device tied to NV path
  returns invalid device if NV path is not supported 
*/
static rfm_device_enum_type
rf_cdma_nv_map_nv_path_to_device
(
   const rf_cdma_nv_path_type nv_path
)
{
  rfm_device_enum_type rf_device;

  /* Return device based on the nv path passed in */
  switch ( nv_path )
  {
     case RF_CDMA_NV_PATH_0: 
       rf_device = RFM_DEVICE_0;
       break;

     case RF_CDMA_NV_PATH_1: 
       rf_device = RFM_DEVICE_1;
       break;

     case RF_CDMA_NV_PATH_2: 
       rf_device = RFM_DEVICE_2;
       break;

     case RF_CDMA_NV_PATH_3: 
       rf_device = RFM_DEVICE_3;
       break;
     default:

       RF_MSG_1( RF_ERROR, "rf_cdma_nv_map_nv_path_to_device: "
                         "Invalid NV path:%d", nv_path );
       rf_device = RFM_INVALID_DEVICE;
       break;
  }

  return rf_device;

}/* rf_cdma_nv_map_nv_path_to_device */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fake initialization function for testing driver functionality without a
  valid QCN. Initializes multilin V3 NV data
 
  @param tx_multi_lin_cal
  Pointer to tx Band cal data type structure for band/path
 
  @return
  TRUE if successful, else FALSE
*/
static boolean
rf_cdma_nv_fake_multilinv3_init
(
  tx_band_cal_type *tx_multi_lin_cal
)
{
  tx_linearizer_table_type cal_tx_lut, first_sweep_tx_lut;
  uint8 i, j = 0;
  tx_multi_linearizer_index_node_type *multi_lin_data;
  uint8 freq_index;

  uint16 rgi_delta_sweep[26] = {
  70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,
  90,91,92,93,94,95 };

  int16 pwr_delta_sweep[26] = {
  25,32,45,57,66,78,89,98,106,117,
  126,137,151,159,170,178,184,190,196,199,
  208,219,224,229,235,241 };

  int16 freq_comp[4][RFCOM_TXLIN_NUM_CHANNELS] = {
  {-12,-4,1,4,6,6,3,0,-4,-9,-12,-13,-12,-10,-6,-3},
  {-9,-3,-1,5,9,7,4,0,-3,-8,-11,-14,-11,-9,-7,-2},
  {-11,-4,0,4,5,6,3,0,-4,-9,-12,-13,-13,-9,-7,-1},
  {-11,-1,3,5,8,9,5,0,-2,-7,-11,-12,-11,-8,-4,-2} };

  tx_multi_lin_cal->nv_rev_type = TX_MULTI_LIN_REV_3;
  tx_multi_lin_cal->tx_cal_chan_size = 16;

  tx_multi_lin_cal->tx_cal_chan[0] = 1018;
  tx_multi_lin_cal->tx_cal_chan[1] = 46;
  tx_multi_lin_cal->tx_cal_chan[2] = 98;
  tx_multi_lin_cal->tx_cal_chan[3] = 150;
  tx_multi_lin_cal->tx_cal_chan[4] = 202;
  tx_multi_lin_cal->tx_cal_chan[5] = 254;
  tx_multi_lin_cal->tx_cal_chan[6] = 306;
  tx_multi_lin_cal->tx_cal_chan[7] = 358;
  tx_multi_lin_cal->tx_cal_chan[8] = 410;
  tx_multi_lin_cal->tx_cal_chan[9] = 462;
  tx_multi_lin_cal->tx_cal_chan[10] = 514;
  tx_multi_lin_cal->tx_cal_chan[11] = 566;
  tx_multi_lin_cal->tx_cal_chan[12] = 618;
  tx_multi_lin_cal->tx_cal_chan[13] = 670;
  tx_multi_lin_cal->tx_cal_chan[14] = 722;
  tx_multi_lin_cal->tx_cal_chan[15] = 774;

  tx_multi_lin_cal->tx_cal_freq[0] = 824850;
  tx_multi_lin_cal->tx_cal_freq[1] = 826380;
  tx_multi_lin_cal->tx_cal_freq[2] = 827940;
  tx_multi_lin_cal->tx_cal_freq[3] = 829500;
  tx_multi_lin_cal->tx_cal_freq[4] = 831060;
  tx_multi_lin_cal->tx_cal_freq[5] = 832620;
  tx_multi_lin_cal->tx_cal_freq[6] = 834180;
  tx_multi_lin_cal->tx_cal_freq[7] = 835740;
  tx_multi_lin_cal->tx_cal_freq[8] = 837300;
  tx_multi_lin_cal->tx_cal_freq[9] = 838860;
  tx_multi_lin_cal->tx_cal_freq[10] = 840420;
  tx_multi_lin_cal->tx_cal_freq[11] = 841980;
  tx_multi_lin_cal->tx_cal_freq[12] = 843540;
  tx_multi_lin_cal->tx_cal_freq[13] = 845100;
  tx_multi_lin_cal->tx_cal_freq[14] = 846660;
  tx_multi_lin_cal->tx_cal_freq[15] = 848220;

  /* Initialize calibrated linearizer at ref channel 835.74 MHz */
  cal_tx_lut.chan_freq = 835740;
  cal_tx_lut.dpd_type = TX_LINEARIZER_NO_AUX_DATA;
  cal_tx_lut.fcomp_type = TX_FREQCOMP_APT;
  cal_tx_lut.num_dpd = 0;
  cal_tx_lut.num_fcomp = 4;
  cal_tx_lut.table[0].rgi = 93;
  cal_tx_lut.table[0].power = 242;
  cal_tx_lut.table[0].apt = 3450;
  cal_tx_lut.table[0].iq_offset = 0;
  cal_tx_lut.table[0].dpd_index = 65535;
  cal_tx_lut.table[0].fcomp_idx = 0;
  cal_tx_lut.table[1].rgi = 91;
  cal_tx_lut.table[1].power = 223;
  cal_tx_lut.table[1].apt = 2890;
  cal_tx_lut.table[1].iq_offset = 0;
  cal_tx_lut.table[1].dpd_index = 65535;
  cal_tx_lut.table[1].fcomp_idx = 1;
  cal_tx_lut.table[2].rgi = 89;
  cal_tx_lut.table[2].power = 199;
  cal_tx_lut.table[2].apt = 2150;
  cal_tx_lut.table[2].iq_offset = 0;
  cal_tx_lut.table[2].dpd_index = 65535;
  cal_tx_lut.table[2].fcomp_idx = 2;
  cal_tx_lut.table[3].rgi = 88;
  cal_tx_lut.table[3].power = 182;
  cal_tx_lut.table[3].apt = 1850;
  cal_tx_lut.table[3].iq_offset = 0;
  cal_tx_lut.table[3].dpd_index = 65535;
  cal_tx_lut.table[3].fcomp_idx = 3;
  cal_tx_lut.table[4].rgi = 87;
  cal_tx_lut.table[4].power = 172;
  cal_tx_lut.table[4].apt = 1850;
  cal_tx_lut.table[4].iq_offset = 0;
  cal_tx_lut.table[4].dpd_index = 65535;
  cal_tx_lut.table[4].fcomp_idx = 3;
  cal_tx_lut.table[5].rgi = 86;
  cal_tx_lut.table[5].power = 160;
  cal_tx_lut.table[5].apt = 1850;
  cal_tx_lut.table[5].iq_offset = 0;
  cal_tx_lut.table[5].dpd_index = 65535;
  cal_tx_lut.table[5].fcomp_idx = 3;
  cal_tx_lut.table[6].rgi = 85;
  cal_tx_lut.table[6].power = 151;
  cal_tx_lut.table[6].apt = 1850;
  cal_tx_lut.table[6].iq_offset = 0;
  cal_tx_lut.table[6].dpd_index = 65535;
  cal_tx_lut.table[6].fcomp_idx = 3;
  cal_tx_lut.table[7].rgi = 84;
  cal_tx_lut.table[7].power = 141;
  cal_tx_lut.table[7].apt = 1850;
  cal_tx_lut.table[7].iq_offset = 0;
  cal_tx_lut.table[7].dpd_index = 65535;
  cal_tx_lut.table[7].fcomp_idx = 3;
  cal_tx_lut.table[8].rgi = 83;
  cal_tx_lut.table[8].power = 131;
  cal_tx_lut.table[8].apt = 1850;
  cal_tx_lut.table[8].iq_offset = 0;
  cal_tx_lut.table[8].dpd_index = 65535;
  cal_tx_lut.table[8].fcomp_idx = 3;
  cal_tx_lut.table[9].rgi = 82;
  cal_tx_lut.table[9].power = 120;
  cal_tx_lut.table[9].apt = 1850;
  cal_tx_lut.table[9].iq_offset = 0;
  cal_tx_lut.table[9].dpd_index = 65535;
  cal_tx_lut.table[9].fcomp_idx = 3;
  cal_tx_lut.table[10].rgi = 81;
  cal_tx_lut.table[10].power = 111;
  cal_tx_lut.table[10].apt = 1850;
  cal_tx_lut.table[10].iq_offset = 0;
  cal_tx_lut.table[10].dpd_index = 65535;
  cal_tx_lut.table[10].fcomp_idx = 3;
  cal_tx_lut.table[11].rgi = 80;
  cal_tx_lut.table[11].power = 102;
  cal_tx_lut.table[11].apt = 1850;
  cal_tx_lut.table[11].iq_offset = 0;
  cal_tx_lut.table[11].dpd_index = 65535;
  cal_tx_lut.table[11].fcomp_idx = 3;
  cal_tx_lut.table[12].rgi = 79;
  cal_tx_lut.table[12].power = 91;
  cal_tx_lut.table[12].apt = 1850;
  cal_tx_lut.table[12].iq_offset = 0;
  cal_tx_lut.table[12].dpd_index = 65535;
  cal_tx_lut.table[12].fcomp_idx = 3;
  cal_tx_lut.table[13].rgi = 78;
  cal_tx_lut.table[13].power = 80;
  cal_tx_lut.table[13].apt = 1850;
  cal_tx_lut.table[13].iq_offset = 0;
  cal_tx_lut.table[13].dpd_index = 65535;
  cal_tx_lut.table[13].fcomp_idx = 3;
  cal_tx_lut.table[14].rgi = 77;
  cal_tx_lut.table[14].power = 71;
  cal_tx_lut.table[14].apt = 1850;
  cal_tx_lut.table[14].iq_offset = 0;
  cal_tx_lut.table[14].dpd_index = 65535;
  cal_tx_lut.table[14].fcomp_idx = 3;
  cal_tx_lut.table[15].rgi = 76;
  cal_tx_lut.table[15].power = 59;
  cal_tx_lut.table[15].apt = 1850;
  cal_tx_lut.table[15].iq_offset = 0;
  cal_tx_lut.table[15].dpd_index = 65535;
  cal_tx_lut.table[15].fcomp_idx = 3;
  cal_tx_lut.table[16].rgi = 75;
  cal_tx_lut.table[16].power = 41;
  cal_tx_lut.table[16].apt = 1850;
  cal_tx_lut.table[16].iq_offset = 0;
  cal_tx_lut.table[16].dpd_index = 65535;
  cal_tx_lut.table[16].fcomp_idx = 3;
  cal_tx_lut.table[17].rgi = 74;
  cal_tx_lut.table[17].power = 30;
  cal_tx_lut.table[17].apt = 1850;
  cal_tx_lut.table[17].iq_offset = 0;
  cal_tx_lut.table[17].dpd_index = 65535;
  cal_tx_lut.table[17].fcomp_idx = 3;
  cal_tx_lut.table[18].rgi = 73;
  cal_tx_lut.table[18].power = 21;
  cal_tx_lut.table[18].apt = 1850;
  cal_tx_lut.table[18].iq_offset = 0;
  cal_tx_lut.table[18].dpd_index = 65535;
  cal_tx_lut.table[18].fcomp_idx = 3;
  cal_tx_lut.table[19].rgi = 72;
  cal_tx_lut.table[19].power = 0;
  cal_tx_lut.table[19].apt = 1850;
  cal_tx_lut.table[19].iq_offset = 0;
  cal_tx_lut.table[19].dpd_index = 65535;
  cal_tx_lut.table[19].fcomp_idx = 3;
  cal_tx_lut.table[20].rgi = 71;
  cal_tx_lut.table[20].power = -11;
  cal_tx_lut.table[20].apt = 1850;
  cal_tx_lut.table[20].iq_offset = 0;
  cal_tx_lut.table[20].dpd_index = 65535;
  cal_tx_lut.table[20].fcomp_idx = 3;
  cal_tx_lut.table[21].rgi = 70;
  cal_tx_lut.table[21].power = -23;
  cal_tx_lut.table[21].apt = 1850;
  cal_tx_lut.table[21].iq_offset = 0;
  cal_tx_lut.table[21].dpd_index = 65535;
  cal_tx_lut.table[21].fcomp_idx = 3;
  cal_tx_lut.table[22].rgi = 69;
  cal_tx_lut.table[22].power = -31;
  cal_tx_lut.table[22].apt = 1850;
  cal_tx_lut.table[22].iq_offset = 0;
  cal_tx_lut.table[22].dpd_index = 65535;
  cal_tx_lut.table[22].fcomp_idx = 3;
  /* Fill up remaining linearizer with invalids */
  for (i = 23; i < RFCOM_TXLIN_NUM_SEGMENTS; i++)
  {
    cal_tx_lut.table[i].rgi = RFCOM_TXLIN_INVALID_RGI;
    cal_tx_lut.table[i].power = 32767;
    cal_tx_lut.table[i].apt = 65535;
    cal_tx_lut.table[i].iq_offset = 0;
    cal_tx_lut.table[i].dpd_index = 65535;
    cal_tx_lut.table[i].fcomp_idx = 65535;
  }

  first_sweep_tx_lut.chan_freq = 
           tx_multi_lin_cal->tx_cal_freq[RFCOM_TXLIN_PWR_COMP_REF_FREQ_INDEX];
  first_sweep_tx_lut.dpd_type = TX_LINEARIZER_NO_AUX_DATA;
  first_sweep_tx_lut.fcomp_type = TX_FREQCOMP_FB;
  first_sweep_tx_lut.num_dpd = 0;
  first_sweep_tx_lut.num_fcomp = 1;
  for (i = 0; i < 26; i++)
  {
    first_sweep_tx_lut.table[i].rgi = rgi_delta_sweep[i];
    first_sweep_tx_lut.table[i].power = pwr_delta_sweep[i];
    first_sweep_tx_lut.table[i].apt = 3700;
    first_sweep_tx_lut.table[i].iq_offset = 0;
    first_sweep_tx_lut.table[i].dpd_index = 65535;
    first_sweep_tx_lut.table[i].fcomp_idx = 0;
  }
  /* Fill up remaining linearizer with invalids */
  for (; i < RFCOM_TXLIN_NUM_SEGMENTS; i++)
  {
    first_sweep_tx_lut.table[i].rgi = RFCOM_TXLIN_INVALID_RGI;
    first_sweep_tx_lut.table[i].power = 32767;
    first_sweep_tx_lut.table[i].apt = 65535;
    first_sweep_tx_lut.table[i].iq_offset = 0;
    first_sweep_tx_lut.table[i].dpd_index = 65535;
    first_sweep_tx_lut.table[i].fcomp_idx = 65535;
  }

  /* Initialize freq comp data */
  for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
  {
    for ( j = 0; j < RFCOM_TXLIN_NUM_CHANNELS; j++ )
    {
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_FB][0].rgi_threshold = 
                             RFCOM_TXLIN_MAX_RGI_INDEX; /* Don't care for V3 */
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_FB][0].freq_comp[j] = 0;
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_APT][0].rgi_threshold = 
                             RFCOM_TXLIN_MAX_RGI_INDEX; /* Don't care for V3 */
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_APT][0].freq_comp[j] = 
                                            freq_comp[0][j];
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_APT][1].freq_comp[j] = 
                                            freq_comp[1][j];
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_APT][2].freq_comp[j] = 
                                            freq_comp[2][j];
      tx_multi_lin_cal->tx_pa_state_cal_data[i].
          tx_freq_offsets[TX_FREQCOMP_APT][3].freq_comp[j] = 
                                            freq_comp[3][j];

    }

    /* Initialize first sweep waveform data */
    multi_lin_data = 
      &tx_multi_lin_cal->tx_pa_state_cal_data[i].
                                    linearizer_index[TX_LINEARIZER_FIRST_SWEEP][0];
    for( freq_index = 0; freq_index < RFCOM_TXLIN_NUM_CHANNELS; freq_index++ )
    {
      multi_lin_data[freq_index].upper_bound_freq = 0;
      multi_lin_data[freq_index].node_data.lin_data = NULL;
    }
    multi_lin_data[0].upper_bound_freq = tx_multi_lin_cal->tx_cal_freq[15];
    multi_lin_data[0].node_data.lin_data = 
               (tx_linearizer_table_type *)
                      modem_mem_alloc( sizeof(tx_linearizer_table_type),
                                       MODEM_MEM_CLIENT_RFA ); 
    memscpy( (void*)multi_lin_data[0].node_data.lin_data, 
             sizeof(tx_linearizer_table_type),
             (void*)&first_sweep_tx_lut,
             sizeof(tx_linearizer_table_type)
          );

    /* Initialize voice waveform data */
    multi_lin_data = 
      &tx_multi_lin_cal->tx_pa_state_cal_data[i].
                                    linearizer_index[TX_LINEARIZER_VOICE][0];
    for( freq_index = 0; freq_index < RFCOM_TXLIN_NUM_CHANNELS; freq_index++ )
    {
      multi_lin_data[freq_index].upper_bound_freq = 0;
      multi_lin_data[freq_index].node_data.lin_data = NULL;
    }
    multi_lin_data[0].upper_bound_freq = tx_multi_lin_cal->tx_cal_freq[15];
    multi_lin_data[0].node_data.lin_data = &cal_tx_lut;
    multi_lin_data[0].node_data.lin_data = 
               (tx_linearizer_table_type *)
                      modem_mem_alloc( sizeof(tx_linearizer_table_type),
                                       MODEM_MEM_CLIENT_RFA ); 
    memscpy( (void*)multi_lin_data[0].node_data.lin_data, 
             sizeof(tx_linearizer_table_type),
             (void*)&cal_tx_lut,
             sizeof(tx_linearizer_table_type)
          );
  }

  return TRUE;
}/* rf_cdma_nv_fake_multilinv3_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fake initialization function for testing driver functionality without a
  valid QCN
 
  @param index_entry
  Pointer to band/path specific NV data structure
 
  @return
  TRUE if successful, else FALSE
*/
boolean
rf_cdma_nv_load_c0_fake_init
(
  rf_cdma_config_band_index_type* const index_entry
)
{
  uint8 i;
  rf_1x_config_type* const rf_1x_config_data = index_entry->ptr_1x;
  rf_hdr_config_type* const rf_hdr_config_data = index_entry->ptr_hdr;
  rf_cdma_config_type* rf_common_config_ptr;

  if ( rf_1x_config_data == NULL ||
       rf_hdr_config_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_c0_fake_init: "
                      "Invalid NULL pointer for the config data" );
    return FALSE;
  }

  /* populate Common items for 1x */
  rf_common_config_ptr = rf_1x_config_data->cdma_common_ptr;

  //rf_common_config_ptr->path = RF_CDMA_NV_PATH_0;
  //rf_common_config_ptr->band = RFM_CDMA_BC0;
  rf_common_config_ptr->rx_cal.channel_list[0] =1018;
  rf_common_config_ptr->rx_cal.channel_list[1] = 46 ;
  rf_common_config_ptr->rx_cal.channel_list[2] = 98 ;
  rf_common_config_ptr->rx_cal.channel_list[3] = 150;   
  rf_common_config_ptr->rx_cal.channel_list[4] = 202;   
  rf_common_config_ptr->rx_cal.channel_list[5] = 254;   
  rf_common_config_ptr->rx_cal.channel_list[6] = 306;   
  rf_common_config_ptr->rx_cal.channel_list[7] = 358;   
  rf_common_config_ptr->rx_cal.channel_list[8] = 410;   
  rf_common_config_ptr->rx_cal.channel_list[9] = 462;   
  rf_common_config_ptr->rx_cal.channel_list[10]= 514;   
  rf_common_config_ptr->rx_cal.channel_list[11]= 566;   
  rf_common_config_ptr->rx_cal.channel_list[12]= 618;   
  rf_common_config_ptr->rx_cal.channel_list[13]= 670;   
  rf_common_config_ptr->rx_cal.channel_list[14]= 722;   
  rf_common_config_ptr->rx_cal.channel_list[15]= 774;   

  rf_common_config_ptr->rx_cal.lru_channel_list[0] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[1] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[2] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[3] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[4] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[5] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[6] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[7] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[8] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[9] = 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[10]= 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[11]= 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[12]= 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[13]= 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[14]= 0; 
  rf_common_config_ptr->rx_cal.lru_channel_list[15]= 0; 


  rf_common_config_ptr->rx_cal.freq_list[0] =   869850;
  rf_common_config_ptr->rx_cal.freq_list[1] =   871380;
  rf_common_config_ptr->rx_cal.freq_list[2] =   872940;
  rf_common_config_ptr->rx_cal.freq_list[3] =   874500;
  rf_common_config_ptr->rx_cal.freq_list[4] =   876060;
  rf_common_config_ptr->rx_cal.freq_list[5] =   877620;
  rf_common_config_ptr->rx_cal.freq_list[6] =   879180;
  rf_common_config_ptr->rx_cal.freq_list[7] =   880740;
  rf_common_config_ptr->rx_cal.freq_list[8] =   882300;
  rf_common_config_ptr->rx_cal.freq_list[9] =   883860;
  rf_common_config_ptr->rx_cal.freq_list[10] =  885420;
  rf_common_config_ptr->rx_cal.freq_list[11] =  886980;
  rf_common_config_ptr->rx_cal.freq_list[12] =  888540;
  rf_common_config_ptr->rx_cal.freq_list[13] =  890100;
  rf_common_config_ptr->rx_cal.freq_list[14] =  891660;
  rf_common_config_ptr->rx_cal.freq_list[15] =  893220;

  rf_common_config_ptr->rx_cal.freq_list_size = 16;

  /* Keeping rx_gain_calibration values to zero to match to CSIM 
  verification for Y1Y2 on Rumi */
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][0] = 0; 
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][1] = 0; 
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][2] = 0; 
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][3] = 0; 
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][4] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][5] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][6] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][7] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][8] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][9] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][0][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][1][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][2][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][3][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[0][4][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][0][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][1][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][2][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][3][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[1][4][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][0][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][1][15] = 0;


  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][2][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][3][15] = 0;

  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][0]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][1]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][2]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][3]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][4]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][5]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][6]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][7]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][8]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][9]  = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][10] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][11] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][12] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][13] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][14] = 0;
  rf_common_config_ptr->rx_cal.rx_gain_calibration[2][4][15] = 0;

  rf_common_config_ptr->ic_data.ref_channel = 358;
  rf_common_config_ptr->ic_data.calibration[0][0] = 348;
  rf_common_config_ptr->ic_data.calibration[0][1] = 348;

  rf_common_config_ptr->ic_data.calibration[1][0] = 298;
  rf_common_config_ptr->ic_data.calibration[1][1] = 298;

  rf_common_config_ptr->ic_data.calibration[2][0] = 0;
  rf_common_config_ptr->ic_data.calibration[2][1] = 0;

  rf_common_config_ptr->ic_data.det_adjust[0][0] = 0;
  rf_common_config_ptr->ic_data.det_adjust[0][1] = 0;
  rf_common_config_ptr->ic_data.det_adjust[0][2] = 0;
  rf_common_config_ptr->ic_data.det_adjust[0][3] = 0;
  rf_common_config_ptr->ic_data.det_adjust[0][4] = 0;

  rf_common_config_ptr->ic_data.det_adjust[1][0] = 0;
  rf_common_config_ptr->ic_data.det_adjust[1][1] = 0;
  rf_common_config_ptr->ic_data.det_adjust[1][2] = 0;
  rf_common_config_ptr->ic_data.det_adjust[1][3] = 0;
  rf_common_config_ptr->ic_data.det_adjust[1][4] = 0;

  /* not populating rf_common_config_ptr->ant_tuner */

  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[0]  = -3;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[1]  = -1;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[2]  = -1;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[3]  = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[4]  = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[5]  = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[6]  = -1;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[7]  = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[8]  = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[9]  = 1;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[10] = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[11] = -1;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[12] = -2;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[13] = -2;  
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[14] = -1;
  rf_common_config_ptr->tx_cal.tx_lim_cal.hdet_vs_freq[15] = 0;

  rf_common_config_ptr->tx_cal.tx_lim_cal.hpm_hdet.enabled = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.lpm_threshold = 0;
  rf_common_config_ptr->tx_cal.tx_lim_cal.lpm_hdet.enabled = 0;

  /* Populate multilinV3 data */
  rf_cdma_nv_fake_multilinv3_init( 
                            &rf_common_config_ptr->tx_cal.tx_multi_lin_cal );

  /* not populating rf_common_config_ptr->tx_cal.tx_therm_cal */

  rf_common_config_ptr->tx_static.tx_pa_r_map = 7;
  /* not populating rf_common_config_ptr->tx_static.lim_vs_temp (all 0's) */

  for ( i=0; i<RF_CDMA_NV_TX_LINEARIZER_NUM; i++ )
  {
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[0] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[1] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[2] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[3] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[4] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[5] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[6] = 1;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].slope[7] = 1;

    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[0] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[1] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[2] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[3] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[4] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[5] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[6] = 0;
    rf_common_config_ptr->tx_static.lin_vs_temp[i].offset[7] = 0;
  }

  rf_common_config_ptr->tx_static.tx_gtr_thresh_data.active = 0;
  rf_common_config_ptr->tx_static.tx_gtr_thresh_data.rise_value = 0;
  rf_common_config_ptr->tx_static.tx_gtr_thresh_data.fall_value = 0;

  /* 1x specific NV */
  rf_1x_config_data->rx_static.lna_info.lin_info[0].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_ACTIVE;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[0].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[0].sp_data.fixed.fall = -906;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[0].sp_data.fixed.rise = -882;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[0].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[0].post_rise_hold = 6800;

  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[1].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[1].sp_data.fixed.fall = -818;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[1].sp_data.fixed.rise = -806;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[1].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[1].post_rise_hold = 6800;
                                                 
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[2].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[2].sp_data.olta.backoff = 290;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[2].sp_data.olta.hysteresis = 40;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[2].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[2].post_rise_hold = 1000;
                                                 
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[3].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[3].sp_data.fixed.fall = -127;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[3].sp_data.fixed.rise = -127;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[3].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[0].sp_info[3].post_rise_hold = 1000;
  
  rf_1x_config_data->rx_static.lna_info.lin_info[1].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_ACTIVE;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[0].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[0].sp_data.fixed.fall = -894;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[0].sp_data.fixed.rise = -882;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[0].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[0].post_rise_hold = 6800;
                                                 
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[1].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[1].sp_data.fixed.fall = -818;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[1].sp_data.fixed.rise = -806;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[1].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[1].post_rise_hold = 6800;
                                                 
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[2].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[2].sp_data.olta.backoff = 290;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[2].sp_data.olta.hysteresis = 40;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[2].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[2].post_rise_hold = 1000;
                                                 
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[3].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[3].sp_data.fixed.fall = -127;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[3].sp_data.fixed.rise = -127;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[3].post_fall_hold = 0;
  rf_1x_config_data->rx_static.lna_info.lin_info[1].sp_info[3].post_rise_hold = 1000;

  rf_1x_config_data->rx_static.lna_info.lin_info[2].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_INACTIVE;

  /* not loading rf_1x_config_data->rx_static.onex_spur_table */

  /* legacy access Switch points (SP) */
  rf_1x_config_data->tx_static.access_swpts.fall[0] = -190;  
  rf_1x_config_data->tx_static.access_swpts.fall[1] = -190;
  rf_1x_config_data->tx_static.access_swpts.fall[2] = 400;

  rf_1x_config_data->tx_static.access_swpts.rise[0] = -150;  
  rf_1x_config_data->tx_static.access_swpts.rise[1] = -150;
  rf_1x_config_data->tx_static.access_swpts.rise[2] = 400;

  rf_1x_config_data->tx_static.access_swpts.fall_hold_time = 20000;

  /* voice SP */
  rf_1x_config_data->tx_static.voice_swpts.fall[0] = -20;  
  rf_1x_config_data->tx_static.voice_swpts.fall[1] = -80;
  rf_1x_config_data->tx_static.voice_swpts.fall[2] = 400;

  rf_1x_config_data->tx_static.voice_swpts.rise[0] = 20;  
  rf_1x_config_data->tx_static.voice_swpts.rise[1] = 120;
  rf_1x_config_data->tx_static.voice_swpts.rise[2] = 400;
                               
  rf_1x_config_data->tx_static.voice_swpts.fall_hold_time = 20000;

  /* data SP */
  rf_1x_config_data->tx_static.data_swpts_rsch[0].fall[0] = -190;  
  rf_1x_config_data->tx_static.data_swpts_rsch[0].fall[1] = -90;
  rf_1x_config_data->tx_static.data_swpts_rsch[0].fall[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[0].rise[0] = -150;  
  rf_1x_config_data->tx_static.data_swpts_rsch[0].rise[1] = -50;
  rf_1x_config_data->tx_static.data_swpts_rsch[0].rise[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[0].fall_hold_time = 20000;

  rf_1x_config_data->tx_static.data_swpts_rsch[1].fall[0] = -190;  
  rf_1x_config_data->tx_static.data_swpts_rsch[1].fall[1] = -90;
  rf_1x_config_data->tx_static.data_swpts_rsch[1].fall[2] = 400;                                               
  rf_1x_config_data->tx_static.data_swpts_rsch[1].rise[0] = -150;  
  rf_1x_config_data->tx_static.data_swpts_rsch[1].rise[1] = -50;
  rf_1x_config_data->tx_static.data_swpts_rsch[1].rise[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[1].fall_hold_time = 20000;

  rf_1x_config_data->tx_static.data_swpts_rsch[2].fall[0] = -190;  
  rf_1x_config_data->tx_static.data_swpts_rsch[2].fall[1] = -90;
  rf_1x_config_data->tx_static.data_swpts_rsch[2].fall[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[2].rise[0] = -150;  
  rf_1x_config_data->tx_static.data_swpts_rsch[2].rise[1] = -50;
  rf_1x_config_data->tx_static.data_swpts_rsch[2].rise[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[2].fall_hold_time = 20000;

  rf_1x_config_data->tx_static.data_swpts_rsch[3].fall[0] = -190;  
  rf_1x_config_data->tx_static.data_swpts_rsch[3].fall[1] = -90;
  rf_1x_config_data->tx_static.data_swpts_rsch[3].fall[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[3].rise[0] = -150;  
  rf_1x_config_data->tx_static.data_swpts_rsch[3].rise[1] = -50;
  rf_1x_config_data->tx_static.data_swpts_rsch[3].rise[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[3].fall_hold_time = 20000;

  rf_1x_config_data->tx_static.data_swpts_rsch[4].fall[0] = -190;  
  rf_1x_config_data->tx_static.data_swpts_rsch[4].fall[1] = -90;
  rf_1x_config_data->tx_static.data_swpts_rsch[4].fall[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[4].rise[0] = -150;  
  rf_1x_config_data->tx_static.data_swpts_rsch[4].rise[1] = -50;
  rf_1x_config_data->tx_static.data_swpts_rsch[4].rise[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[4].fall_hold_time = 20000;

  rf_1x_config_data->tx_static.data_swpts_rsch[5].fall[0] = -190;  
  rf_1x_config_data->tx_static.data_swpts_rsch[5].fall[1] = -90;
  rf_1x_config_data->tx_static.data_swpts_rsch[5].fall[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[5].rise[0] = -150;  
  rf_1x_config_data->tx_static.data_swpts_rsch[5].rise[1] = -50;
  rf_1x_config_data->tx_static.data_swpts_rsch[5].rise[2] = 400;
  rf_1x_config_data->tx_static.data_swpts_rsch[5].fall_hold_time = 20000;

  /* not populating apt_adj_value */
  rf_1x_config_data->enc_btf_delay = 3378;

  /*--------------------------------------------------------------------------*/
  /* HDR NV items */
  /*--------------------------------------------------------------------------*/
  
  /* HDR specific NV */
 
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_ACTIVE;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[0].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[0].sp_data.fixed.fall = -926;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[0].sp_data.fixed.rise = -882;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[0].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[0].post_rise_hold = 6800;

  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[1].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[1].sp_data.fixed.fall = -818;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[1].sp_data.fixed.rise = -806;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[1].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[1].post_rise_hold = 6800;

  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[2].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[2].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[2].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[2].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[2].post_rise_hold = 1000;

  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[3].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[3].sp_data.fixed.fall = -127;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[3].sp_data.fixed.rise = -127;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[3].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[0].sp_info[3].post_rise_hold = 1000;
     
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_ACTIVE;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[0].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[0].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[0].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[0].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[0].post_rise_hold = 1000;

  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[1].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[1].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[1].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[1].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[1].post_rise_hold = 1000;
                                
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[2].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[2].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[2].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[2].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[2].post_rise_hold = 1000;
                                
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[3].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[3].sp_data.fixed.fall = -127;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[3].sp_data.fixed.rise = -127;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[3].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[1].sp_info[3].post_rise_hold = 1000;
                                
  rf_hdr_config_data->rx_static.lna_info_dor0_a.lin_info[2].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_INACTIVE;


  /*  Rev B SPs */
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_ACTIVE;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[0].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[0].sp_data.fixed.fall = -926;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[0].sp_data.fixed.rise = -882;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[0].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[0].post_rise_hold = 6800;

  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[1].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[1].sp_data.fixed.fall = -818;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[1].sp_data.fixed.rise = -806;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[1].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[1].post_rise_hold = 6800;

  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[2].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[2].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[2].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[2].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[2].post_rise_hold = 1000;

  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[3].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[3].sp_data.fixed.fall = -127;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[3].sp_data.fixed.rise = -127;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[3].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[0].sp_info[3].post_rise_hold = 1000;
     
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_ACTIVE;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[0].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[0].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[0].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[0].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[0].post_rise_hold = 1000;

  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[1].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[1].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[1].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[1].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[1].post_rise_hold = 1000;
                                
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[2].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_OLTA;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[2].sp_data.olta.backoff = 290;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[2].sp_data.olta.hysteresis = 40;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[2].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[2].post_rise_hold = 1000;
                                
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[3].sp_mode = RF_CDMA_LNA_SWITCH_PT_MODE_FIXED;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[3].sp_data.fixed.fall = -127;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[3].sp_data.fixed.rise = -127;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[3].post_fall_hold = 0;
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[1].sp_info[3].post_rise_hold = 1000;
                                
  rf_hdr_config_data->rx_static.lna_info_dorb_mc.lin_info[2].lin_mode_active = RF_CDMA_LNA_SWITCH_PT_INACTIVE;



  /* HDR TX */
  rf_hdr_config_data->tx_static.access_swpts.fall[0] = -190;
  rf_hdr_config_data->tx_static.access_swpts.fall[1] = -190;
  rf_hdr_config_data->tx_static.access_swpts.fall[2] = 400;

  rf_hdr_config_data->tx_static.access_swpts.rise[0] = -150;
  rf_hdr_config_data->tx_static.access_swpts.rise[1] = -150;
  rf_hdr_config_data->tx_static.access_swpts.rise[2] = 400;

  rf_hdr_config_data->tx_static.access_swpts.fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[0].fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[1].fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[2].fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[3].fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[4].fall_hold_time = 20000;

  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].fall[0] = -20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].fall[1] = 80;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].fall[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].rise[0] = 20;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].rise[1] = 120;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].rise[2] = 400;
  rf_hdr_config_data->tx_static.dor0_swpts_rri[5].fall_hold_time = 20000;

  /* not loading 
    rf_hdr_config_data->tx_static.dorA_swpts
    rf_hdr_config_data->tx_static.dorB_mc_swpts
    rf_hdr_config_data->tx_static.apt_adj_value
    rf_hdr_config_data->tx_static.tx_pwr_limit_data */

  return TRUE;

} /* rf_cdma_nv_load_c0_fake_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate a new NV data structure for storing the common CDMA configuration
  items.

  @details
  The constant data members for storing the NV path and band in the common
  configuration structure are initialized with the constructor parameters
  path and band.

  Allocation is not thread-safe, so the entire configuration loading
  process must insure that it is executed atomically.
 
  @param path
  RF NV path that this configuration structure will be associated with.
  
  @param band
  RF band that this configuration structure will be associated with.
  
  @return
  A pointer to a structure that is now dedicated to this path and band
  for storing configuration data.  If no space is available, NULL is
  returned.
*/
static
rf_cdma_config_type*
rf_cdma_nv_alloc_common_config
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type  band
)
{
  if ( rf_cdma_nv_common_config_storage.allocated < RF_CDMA_NV_MAX_STRUCTURES )
  {
    /* Perform some magic to get around the const-ness of the path and band
       members of the common structure for object construction. */
    rf_cdma_nv_path_type* path_ptr;
    rfm_cdma_band_class_type*  band_ptr;

    rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated] =
      modem_mem_alloc( sizeof(rf_cdma_config_type), MODEM_MEM_CLIENT_RFA );

    if ( rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated]
         == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_nv_alloc_common_config: Could not "
                        "allocate NV memory on heap" );
      return NULL;
    }
    else
    {
      memset(rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated],
             0, sizeof(rf_cdma_config_type));

      path_ptr = (rf_cdma_nv_path_type*)
          &(rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated]->path);
      band_ptr = (rfm_cdma_band_class_type*)
          &(rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated]->band);

      *path_ptr = path;
      *band_ptr = band;

      return rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated++];
    }
  }
  else
  {
    return NULL;
  }
} /* rf_cdma_nv_alloc_common_config() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate a new NV data structure for storing the 1x CDMA configuration
  items.

  @details
  The constant data member pointing to the common 1x and HDR configuration
  data structure is initialized with the constructor parameter
  rf_cdma_config_ptr.
 
  Allocation is not thread-safe, so the entire configuration loading
  process must insure that it is executed atomically.
 
  @param rf_cdma_config_ptr
  A pointer to the data structure that is common between 1x and HDR and
  is necessary to describe the entire 1x configuration.
  
  @return
  A pointer to a structure that is now dedicated to this path and band
  for storing configuration data.  If no space is available, NULL is
  returned.
*/
static
rf_1x_config_type*
rf_cdma_nv_alloc_1x_config
(
  rf_cdma_config_type* rf_cdma_config_ptr
)
{
  if ( rf_cdma_nv_1x_config_storage.allocated < RF_CDMA_NV_MAX_STRUCTURES )
  {
    /* Perform some magic to get around the const-ness of the common config
       pointer member of the 1x structure for object construction. */
    rf_cdma_config_type** common_config_ptr;

    rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated] =
      modem_mem_alloc( sizeof(rf_1x_config_type), MODEM_MEM_CLIENT_RFA );

    if ( rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated]
         == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_nv_alloc_1x_config: Could not "
                        "allocate NV memory on heap" );
      return NULL;
    }
    else
    {
      memset(rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated],
             0, sizeof(sizeof(rf_1x_config_type)));
      common_config_ptr = (rf_cdma_config_type**)
        &(rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated]->cdma_common_ptr);

      *common_config_ptr = rf_cdma_config_ptr;

      return rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated++];
    }
  }
  else
  {
    return NULL;
  }
} /* rf_cdma_nv_alloc_1x_config() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate a new NV data structure for storing the HDR CDMA configuration
  items.

  @details
  The constant data member pointing to the common 1x and HDR configuration
  data structure is initialized with the constructor parameter
  rf_cdma_config_ptr.
 
  Allocation is not thread-safe, so the entire configuration loading
  process must insure that it is executed atomically.
 
  @param rf_cdma_config_ptr
  A pointer to the data structure that is common between 1x and HDR and
  is necessary to describe the entire 1x configuration.
  
  @return
  A pointer to a structure that is now dedicated to this path and band
  for storing configuration data.  If no space is available, NULL is
  returned.
*/
static
rf_hdr_config_type*
rf_cdma_nv_alloc_hdr_config
(
  rf_cdma_config_type* rf_cdma_config_ptr
)
{
  if ( rf_cdma_nv_hdr_config_storage.allocated < RF_CDMA_NV_MAX_STRUCTURES )
  {
    /* Perform some magic to get around the const-ness of the common config
       pointer member of the 1x structure for object construction. */
    rf_cdma_config_type** common_config_ptr;

    rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated] =
      modem_mem_alloc( sizeof(rf_hdr_config_type), MODEM_MEM_CLIENT_RFA );

    if ( rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated]
         == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_nv_alloc_hdr_config: Could not "
                        "allocate NV memory on heap" );
      return NULL;
    }
    else
    {
      memset(rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated],
             0, sizeof(rf_hdr_config_type));

      common_config_ptr = (rf_cdma_config_type**)
        &(rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated]->cdma_common_ptr);

      *common_config_ptr = rf_cdma_config_ptr;

      return rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated++];
    }
  }
  else
  {
    return NULL;
  }
} /* rf_cdma_nv_alloc_hdr_config() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the NV storage for the 1x and HDR configuration data for a
  given path and band.

  @details
  Constructs the 1x and HDR data structures necessary to store the data
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
rf_cdma_config_band_index_type*
rf_cdma_nv_alloc
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type  band
)
{
  rf_cdma_config_type* rf_cdma_config_ptr;
  rf_1x_config_type*   rf_1x_config_ptr;
  rf_hdr_config_type*  rf_hdr_config_ptr;

  rf_cdma_config_ptr =
    rf_cdma_nv_alloc_common_config( path, band );
  if ( rf_cdma_config_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "Failed to allocate common CDMA config structure for loading NV");
    return FALSE;
  }

  rf_1x_config_ptr =
    rf_cdma_nv_alloc_1x_config( rf_cdma_config_ptr );
  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "Failed to allocate 1x CDMA config structure for loading NV");
    return FALSE;
  }

  rf_hdr_config_ptr =
    rf_cdma_nv_alloc_hdr_config( rf_cdma_config_ptr );
  if ( rf_hdr_config_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "Failed to allocate HDR CDMA config structure for loading NV");
    return FALSE;
  }

  rf_cdma_config_device_index[path].band_index[band].ptr_1x =
    rf_1x_config_ptr;
  rf_cdma_config_device_index[path].band_index[band].ptr_hdr =
    rf_hdr_config_ptr;
  rf_cdma_config_device_index[path].band_index[band].data_valid = FALSE;

  return &rf_cdma_config_device_index[path].band_index[band];
} /* rf_cdma_nv_alloc() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx max power limit vs temperature data from NV,
  for SAR states 0 through 8.

  @details
  Checks that the Tx Lim vs Temp NV item for the requested device and band
  is loaded currently from NV, and initializes the data for use by the
  max power limit module.

  Limit vs temperature data is stored in NV in Tx AGC 0-1023 units, but
  the temperature compensation module expects values in dBm10 units.  This
  loading function converts from Tx AGC units to dBm10 based on the band
  that is being loaded.

  @param tx_static_struct
  The Tx NV data structure that max power limit information must be loaded
  into.

  @param band
  The CDMA band that is being loaded.  This is needed to convert from
  Tx AGC unit into dBm10 units while loading.

  @param lim_vs_temp_sar_0_item
  The NV item that contanis the temp comp data for SAR state 0.

  @param lim_vs_temp_sar_1_8_item
  The NV item that contains the temp comp data for SAR states 1-8.
  If this item is inactive, SAR states 1-8 are populated with the
  data from lim_vs_temp_sar_0_item.

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
  TRUE if the Lim vs Freq data is loaded, FALSE if there was a problem
  identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_lim_temp
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfm_cdma_band_class_type band,

  const rfnv_item_id_enum_type lim_vs_temp_sar_0_item,
  const rfnv_item_id_enum_type lim_vs_temp_sar_1_8_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *lim_vs_temp_buffer;
  rfnv_item_size_type   lim_vs_temp_returned_size =
                                  sizeof(rfnv_item_type);

  /* Data structure iterator */
  int i;

  lim_vs_temp_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                        MODEM_MEM_CLIENT_RFA );
  if ( lim_vs_temp_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lim_temp: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( lim_vs_temp_sar_0_item,
                            lim_vs_temp_buffer,
                            &lim_vs_temp_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S &&
       lim_vs_temp_returned_size >= sizeof(rfnv_tx_pwr_sar_0_temp_comp_type) )
  {
    rfnv_tx_pwr_sar_0_temp_comp_type* lim_temp_sar_0_data_ptr;

    lim_temp_sar_0_data_ptr =
      (rfnv_tx_pwr_sar_0_temp_comp_type*)lim_vs_temp_buffer;

    /* Load the max power vs temperature data into index 0 for
       the default SAR state. */
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      /* While loading, need to convert from unsigned top-quarter (0..255
         corresponding to 768..1023) Tx AGC units to dBm10 units.  The
         conversion equation is
           (TxAGC_top_quarter + 768 + 127) + 10*TA
         Tx AGC 0 is min power => -603 (dBm10) for low bands,
                               => -633 (dbm10) for high bands.
      */
      tx_static_struct->lim_vs_temp[0][i] = 
        (lim_temp_sar_0_data_ptr->temp_comp[i] + 768 + 127)
        + 10*rf_cdma_get_turnaround_constant(band);
    }

    retval = TRUE;

    /* Then load SAR states 1 through 8, if they are active. */
    lim_vs_temp_returned_size = sizeof(rfnv_item_type);
    nv_status =
      rfcommon_rfnv_get_item( lim_vs_temp_sar_1_8_item,
                              lim_vs_temp_buffer,
                              &lim_vs_temp_returned_size,
                              caller_tcb_ptr,
                              task_nv_wait_sig,
                              task_wait_func_ptr );

    if ( nv_status == NV_DONE_S &&
         lim_vs_temp_returned_size >= sizeof(rfnv_tx_pwr_sar_1_8_temp_comp_type) )
    {
      int sar_state;
      rfnv_tx_pwr_sar_1_8_temp_comp_type* lim_temp_sar_1_8_data_ptr;

      lim_temp_sar_1_8_data_ptr =
        (rfnv_tx_pwr_sar_1_8_temp_comp_type*)lim_vs_temp_buffer;

      /* Load the max power vs temperature data into indicies 1 through 8
         for the SAR states. */
      for ( sar_state = 1;
            sar_state < RF_CDMA_NV_DSI_SAR_STATE_SIZE;
            sar_state++ )
      {
        for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
        {
          /* While loading convert from unsigned Tx AGC units to dBm10 units.
             In this case this NV is already in 10-bit unsigned format
             (0..1023), so there is no need to add 768. */
          tx_static_struct->lim_vs_temp[sar_state][i] = 
            (lim_temp_sar_1_8_data_ptr->temp_comp[sar_state-1][i] + 127)
            + 10*rf_cdma_get_turnaround_constant(band);
        }
      }
    } /* if (nv_status == NV_DONE_S ...) */
    else if ( nv_status == NV_NOTACTIVE_S )
    {
      /* Inactive SAR NV item means state 0 should be copied to the rest
         of the states, for safety. */
      int sar_state;

      for ( sar_state = 1;
            sar_state < RF_CDMA_NV_DSI_SAR_STATE_SIZE;
            sar_state++ )
      {
        for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
        {
          /* SAR state 0 has already been converted to dBm10 */
          tx_static_struct->lim_vs_temp[sar_state][i] = 
            tx_static_struct->lim_vs_temp[0][i];
        }
      } /* for (sar_state = 1..8) */
    } /* else if ( nv_status == NV_NOTACTIVE_S ) for SAR 1-8 items */
    else
    {
      /* Something went wrong reading SAR temp comp item, fail this band. */
      RF_MSG_1( RF_ERROR,
                "rf_cdma_nv_load_tx_lim_temp: "
                "SAR NV state is inconsistent for band %d",
                band );
      retval = FALSE;
    }
  } /* if ( SAR state 0 NV read == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If Tx limit data isn't set in NV, this is a configuration error. */
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_lim_temp: "
              "No Tx limit NV has been specified for band %d",
              band );

    if ( rf_cdma_debug_flags.default_max_power_limit )
    {
      /* Until NV is defined in all QCN fragments,
         don't allow inactive NV to kill 1x initialization. */
      int sar_state;

      for ( sar_state = 0;
            sar_state < RF_CDMA_NV_DSI_SAR_STATE_SIZE;
            sar_state++ )
      {
        for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
        {
          tx_static_struct->lim_vs_temp[sar_state][i] = 
          (uint16) rf_cdma_debug_flags.default_max_power_limit;
        }
      } /* for (sar_state = 0..8) */

      retval = TRUE;
    } /* if ( rf_cdma_debug_flags.default_max_power_limit ) */
    else
    {
      retval = FALSE;
    }
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lim_temp: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  modem_mem_free( lim_vs_temp_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_tx_lim_temp() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx linearizer vs temperature data from NV, for
  all PA states.

  @details
  Checks that the Tx Lin vs Temp NV item for the requested device and band
  is loaded currently from NV, and initializes the data for use by the
  temperature compensation loops.

  Linearizer vs Temperature data for a given band and device consists of
  static characterization that is provided as slope adjustment and fixed
  offset compensation. Both of these values are used to compensate the
  linearizer based on the current therm reading. This function loads
  data for both of these entities for all PA states across 8 discrete
  therm bins.
 
  @param tx_static_struct
  The Tx NV data structure that static linearizer vs temperature data
  must be loaded into.
 
  @param lin_vs_temp_item
  The NV item that contains the linearizer temp comp data for all PA
  states.
 
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
  TRUE if the Lin vs Temp data is successfully initialized,
  FALSE if there was a problem identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_lin_temp
(
  rf_cdma_tx_static_type* const tx_static_struct,

  const rfnv_item_id_enum_type lin_vs_temp_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *lin_vs_temp_buffer;
  rfnv_item_size_type   lin_vs_temp_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_tx_lin_vs_temp   *lin_vs_temp_data;

  /* Data structure iterator */
  int i, j, k;

  lin_vs_temp_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                        MODEM_MEM_CLIENT_RFA );
  if ( lin_vs_temp_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lin_temp: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( lin_vs_temp_item,
                            lin_vs_temp_buffer,
                            &lin_vs_temp_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    lin_vs_temp_data = (rfnv_tx_lin_vs_temp*)lin_vs_temp_buffer;

    /* Populate linearizer vs temperature data for all 4 PA states
    and across 8 therm bins */
    for ( i = 0; i < RF_CDMA_NV_TX_LINEARIZER_NUM; i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_TEMP_LIMIT_SIZE; j++ )
      {
        tx_static_struct->lin_vs_temp[i].slope[j] =
          lin_vs_temp_data->data[i].slope[j];
        tx_static_struct->lin_vs_temp[i].offset[j] =
          lin_vs_temp_data->data[i].offset[j];
      }
    }

    /* Populate new temp comp data structures for common temp comp 
    implementation */
    tx_static_struct->lin_temp_comp_data.is_legacy_temp_comp_nv = TRUE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
    {
      for ( j = 0; j < RFCOM_TXLIN_TEMP_BIN_SIZE; j++ )
      {
        tx_static_struct->lin_temp_comp_data.legacy_lin_vs_temp[i].slope[j] = 
          lin_vs_temp_data->data[i].slope[j];
        tx_static_struct->lin_temp_comp_data.legacy_lin_vs_temp[i].offset[j] =
          (int16)lin_vs_temp_data->data[i].offset[j];

        /* Freq deltas for C2K are zero */
        for ( k = 0; k < RFCOM_TXLIN_NUM_CHANNELS; k++ )
        {
          tx_static_struct->
           lin_temp_comp_data.
                     legacy_lin_vs_temp[i].temp_vs_freq_delta[j][k] = 0;
        }
      }
    }

    retval = TRUE;
  }
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If the NV item is inactive, load defaults, where slope = 1
    and fixed offset = 0. This ensures that linearizer temp comp
    is disabled */
    for ( i = 0; i < RF_CDMA_NV_TX_LINEARIZER_NUM; i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_TEMP_LIMIT_SIZE; j++ )
      {
        tx_static_struct->lin_vs_temp[i].slope[j] = 1;
        tx_static_struct->lin_vs_temp[i].offset[j] = 0;
      }
    }

    /* Populate new temp comp data structures for common temp comp 
    implementation */
    tx_static_struct->lin_temp_comp_data.is_legacy_temp_comp_nv = FALSE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
    {
      for ( j = 0; j < RFCOM_TXLIN_TEMP_BIN_SIZE; j++ )
      {
        tx_static_struct->lin_temp_comp_data.legacy_lin_vs_temp[i].slope[j] = 
          1;
        tx_static_struct->lin_temp_comp_data.legacy_lin_vs_temp[i].offset[j] =
          0;

        /* Freq deltas for C2K are zero */
        for ( k = 0; k < RFCOM_TXLIN_NUM_CHANNELS; k++ )
        {
          tx_static_struct->
           lin_temp_comp_data.
                     legacy_lin_vs_temp[i].temp_vs_freq_delta[j][k] = 0;
        }
      }
    }

    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lin_temp: "
            "NV calibration state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( lin_vs_temp_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_tx_lin_temp() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx Therm cal data for a given device, across 8
  therm bins.

  @details
  Checks that the Therm Bins NV item for the requested device
  is loaded currently from NV, and initializes the data for use by the
  temperature compensation loops.

  This NV item contains eight elements and is used to calibrate the
  temperature ADC readings. Each element contains the ADC reading
  corresponding to the index temperature of interest. Index [0]
  corresponds to the minimum thermistor ADC value of interest, which
  typically corresponds to the maximum temperature of interest.
  Index[7] corresponds to the maximum thermistor ADC value of
  interest, which typically corresponds to the minimum temperature
  of interest. 
 
  @param tx_cal_struct
  The Tx NV data structure that therm bins cal data must be loaded into.

  @param tx_static_struct
  Tx static NV structure for common lin vs temp comp NV storage 
 
  @param therm_bins_item
  The NV item that contains the therm bins data for the requested device.
 
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
  TRUE if the Therm Bins data is successfully initialized,
  FALSE if there was a problem identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_therm_bins
(
  rf_cdma_tx_cal_type* const tx_cal_struct,
  rf_cdma_tx_static_type* const tx_static_struct,

  const rfnv_item_id_enum_type therm_bins_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *therm_bins_buffer;
  rfnv_item_size_type   therm_bins_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_tx_therm_cal     *therm_bins_data;

  /* Data structure iterator */
  int i;

  therm_bins_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                       MODEM_MEM_CLIENT_RFA );
  if ( therm_bins_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_therm_bins: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( therm_bins_item,
                            therm_bins_buffer,
                            &therm_bins_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    therm_bins_data = (rfnv_tx_therm_cal*)therm_bins_buffer;

    /* Populate thermistor calibrated data for the 8 bins */
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      tx_cal_struct->therm_cal_data.tx_therm_cal[i] = 
        therm_bins_data->therm_cal[i] ;
    }

    tx_cal_struct->therm_cal_data.is_therm_cal_nv_data_valid = 
      rf_cdma_temp_comp_check_monotonicity (  
      &( tx_cal_struct->therm_cal_data.tx_therm_cal[0] ) , 
      RF_CDMA_NV_TEMP_LIMIT_SIZE ) ;

    /* Also populate common Tx linearizer vs temp structure with 
    therm bins NV data */
    if ( tx_cal_struct->therm_cal_data.is_therm_cal_nv_data_valid )
    {
      for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
      {
        tx_static_struct->lin_temp_comp_data.tx_therm_bins[i] = 
          therm_bins_data->therm_cal[i] ;
      }
    }
    /* Irrespective of therm cal data monotonocity, continue with NV loading. We
       can disable temp comp if the items turn out to be non-monotonic */
    retval = TRUE;
  }
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If the NV item is inactive, load defaults, all thermistor
    readings are zero*/
    memset( &(tx_cal_struct->therm_cal_data), 0,
            (sizeof (rf_cdma_therm_properties_type)) );
    memset( &(tx_static_struct->lin_temp_comp_data.tx_therm_bins), 0,
            (sizeof(uint16)*RF_CDMA_NV_TEMP_LIMIT_SIZE) );

    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_therm_bins: "
            "NV calibration state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( therm_bins_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_tx_therm_bins() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load 1x Access Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the 1x radio is in
  access state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure
 
  @param tx_swpt_nv_item
  1x Access Tx Switchpoint NV item
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_1x_access_tx_swpts
(
  rf_1x_tx_static_type* tx_1x_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for 1x HDR Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_swpt_type   *tx_swpt_nv_ptr;

  /* Iterator */
  int pa_state;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_1x_access_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_swpt_type *)nv_buffer;
    /* Lowest PA state switchpoints are -70 dBm */
    tx_1x_ptr->access_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
    tx_1x_ptr->access_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
    for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
          pa_state++ )
    {
      tx_1x_ptr->access_swpts.fall[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].fall;
      tx_1x_ptr->access_swpts.rise[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].rise;
    }
    /* Set remaining PA states to max power 40 dBm */
    for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_1x_ptr->access_swpts.fall[pa_state] = 400;
      tx_1x_ptr->access_swpts.rise[pa_state] = 400;
    }

    /* Copy the hysteresis value */
    tx_1x_ptr->access_swpts.fall_hold_time = 
      tx_swpt_nv_ptr->fall_time_hysteresis;

    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If 1x access swpts are not-active, use voice switchpoint data as
    default */
    for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_1x_ptr->access_swpts.fall[pa_state] = 
        tx_1x_ptr->voice_swpts.fall[pa_state];
      tx_1x_ptr->access_swpts.rise[pa_state] = 
        tx_1x_ptr->voice_swpts.rise[pa_state];
    }
    /* Copy the hysteresis value */
    tx_1x_ptr->access_swpts.fall_hold_time = 
      tx_1x_ptr->voice_swpts.fall_hold_time;

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_1x_access_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_1x_access_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load 1x Voice Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the 1x radio is in
  voice traffic state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param band
  Band for which legacy PA switchpoints may be loaded

  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure
 
  @param tx_swpt_nv_item
  1x Voice Tx Switchpoint NV item
 
  @param pa_r1_rise_item
  The NV item that contains the PA R1 Rise value.
 
  @param pa_r1_fall_item
  The NV item that contains the PA R1 Fall value.
 
  @param pa_r2_rise_item
  The NV item that contains the PA R2 Rise value.
 
  @param pa_r2_fall_item
  The NV item that contains the PA R2 Fall value.

  @param pa_r3_rise_item
  The NV item that contains the PA R3 Rise value.
 
  @param pa_r3_fall_item
  The NV item that contains the PA R3 Fall value.
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_1x_voice_tx_swpts
(
  rfm_cdma_band_class_type band,
  rf_1x_tx_static_type* tx_1x_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,
  const rfnv_item_id_enum_type pa_r1_rise_item,
  const rfnv_item_id_enum_type pa_r1_fall_item,
  const rfnv_item_id_enum_type pa_r2_rise_item,
  const rfnv_item_id_enum_type pa_r2_fall_item,
  const rfnv_item_id_enum_type pa_r3_rise_item,
  const rfnv_item_id_enum_type pa_r3_fall_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for 1x Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_swpt_type   *tx_swpt_nv_ptr;

  /* Iterator */
  int pa_state;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_1x_voice_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_swpt_type *)nv_buffer;
    /* Lowest PA state switchpoints are -70 dBm */
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
    for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
          pa_state++ )
    {
      tx_1x_ptr->voice_swpts.fall[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].fall;
      tx_1x_ptr->voice_swpts.rise[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].rise;
    }
    /* Set remaining PA states to max power 40 dBm */
    for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_1x_ptr->voice_swpts.fall[pa_state] = 400;
      tx_1x_ptr->voice_swpts.rise[pa_state] = 400;
    }

    /* Copy the hysteresis value */
    tx_1x_ptr->voice_swpts.fall_hold_time = 
      tx_swpt_nv_ptr->fall_time_hysteresis;
    
    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If 1x voice swpts are not-active, default to old legacy
    switchpoint NV */
    rf_cdma_nv_load_old_pa_swpts( band,
                                  tx_1x_ptr,
                                  pa_r1_rise_item, pa_r1_fall_item, 
                                  pa_r2_rise_item, pa_r2_fall_item,
                                  pa_r3_rise_item, pa_r3_fall_item,
                                  caller_tcb_ptr, 
                                  task_nv_wait_sig,
                                  task_wait_func_ptr );

    /* Copy the hysteresis value */
    tx_1x_ptr->voice_swpts.fall_hold_time = 20000;

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_1x_voice_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_1x_voice_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load 1x Data Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the 1x radio is in
  data traffic state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure
 
  @param tx_swpt_nv_item
  1x Data Tx Switchpoint NV item
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_1x_data_tx_swpts
(
  rfm_cdma_band_class_type band,
  rf_1x_tx_static_type* tx_1x_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for 1x Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_1x_data_swpt_type *tx_swpt_nv_ptr;

  /* Iterators */
  int pa_state, rc_rate;

  /* In order to provide legacy support, we handle PA backoffs for
  data switchpoints in this NV loading function -> if the data swpt
  NV is inactive, but voice swpts are available, then apply backoffs
  on top of the voice swpt values to obtain data swpts. */
  int16 pa_backoff_val, fall_val, rise_val;
  /* Use a conservative invalid value, that applies across
  hi and lo bands: +39 dBm */
  int16 invalid_swpt_val = 390;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_1x_data_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_1x_data_swpt_type *)nv_buffer;
    /* Copy switchpoint data for all reverse link data rates */
    for ( rc_rate = 0;
          rc_rate < RF_CDMA_NV_1X_R_SCH_RATES_NUM;
          rc_rate ++ )
    {
      /* Lowest PA state switchpoints are -70 dBm */
      tx_1x_ptr->data_swpts_rsch[rc_rate].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[rc_rate].rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[rc_rate].fall[pa_state] = 
          tx_swpt_nv_ptr->rate[rc_rate].sp.sp_data[pa_state-1].fall;
        tx_1x_ptr->data_swpts_rsch[rc_rate].rise[pa_state] = 
          tx_swpt_nv_ptr->rate[rc_rate].sp.sp_data[pa_state-1].rise;
      }
      /* Set remaining PA states to max power 40 dBm */
      for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[rc_rate].fall[pa_state] = 400;
        tx_1x_ptr->data_swpts_rsch[rc_rate].rise[pa_state] = 400;
      }

      /* Copy the hysteresis value */
      tx_1x_ptr->data_swpts_rsch[rc_rate].fall_hold_time = 
        tx_swpt_nv_ptr->rate[rc_rate].sp.fall_time_hysteresis;
    }
    
    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If 1x data swpts are not-active, use voice swpt data for
    all reverse link rates. However, apply PA backoffs to all
    the voice switchpoints */
    if ( rf_cdma_is_high_band(band) )
    {
      pa_backoff_val = 200; /* 20 dB */
    }
    else
    {
      pa_backoff_val = 170; /* 17 dB */
    }

    for ( rc_rate = 0;
          rc_rate < RF_CDMA_NV_1X_R_SCH_RATES_NUM;
          rc_rate ++ )
    {
      /* Lowest PA state switchpoints are -70 dBm */
      tx_1x_ptr->data_swpts_rsch[rc_rate].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[rc_rate].rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
            pa_state++ )
      {
        /* Apply the PA backoff values if a valid switchpoint
        is obtained for the PA state */
        fall_val = tx_1x_ptr->voice_swpts.fall[pa_state];
        if ( fall_val < invalid_swpt_val )
        {
          fall_val -= pa_backoff_val;
        }
        tx_1x_ptr->data_swpts_rsch[rc_rate].fall[pa_state] = 
          fall_val;

        rise_val = tx_1x_ptr->voice_swpts.rise[pa_state];
        if ( rise_val < invalid_swpt_val )
        {
          rise_val -= pa_backoff_val;
        }
        tx_1x_ptr->data_swpts_rsch[rc_rate].rise[pa_state] = 
          rise_val;
      }
      /* Set remaining PA states to max power 40 dBm */
      for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[rc_rate].fall[pa_state] = 400;
        tx_1x_ptr->data_swpts_rsch[rc_rate].rise[pa_state] = 400;
      }

      /* Copy the hysteresis value */
      tx_1x_ptr->data_swpts_rsch[rc_rate].fall_hold_time = 
        tx_1x_ptr->voice_swpts.fall_hold_time;
    }

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_1x_data_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_1x_data_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR Access Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the HDR radio is in
  access state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure, used to load default
  data
 
  @param tx_hdr_ptr
  Pointer to the HDR Tx static NV data structure, used to load default
  data

  @param tx_swpt_nv_item
  HDR Access Tx Switchpoint NV item
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_hdr_access_tx_swpts
(
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_hdr_tx_static_type* tx_hdr_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for 1x HDR Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_swpt_type   *tx_swpt_nv_ptr;

  /* Iterator */
  int pa_state;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_hdr_access_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_swpt_type *)nv_buffer;
    /* Lowest PA state switchpoints are -70 dBm */
    tx_hdr_ptr->access_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
    tx_hdr_ptr->access_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
    for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
          pa_state++ )
    {
      tx_hdr_ptr->access_swpts.fall[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].fall;
      tx_hdr_ptr->access_swpts.rise[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].rise;
    }
    /* Set remaining PA states to max power 40 dBm */
    for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_hdr_ptr->access_swpts.fall[pa_state] = 400;
      tx_hdr_ptr->access_swpts.rise[pa_state] = 400;
    }

    /* Copy the hysteresis value */
    tx_hdr_ptr->access_swpts.fall_hold_time = 
      tx_swpt_nv_ptr->fall_time_hysteresis;

    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If HDR access swpts are not-active, use voice switchpoint data as
    default */
    for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_hdr_ptr->access_swpts.fall[pa_state] = 
        tx_1x_ptr->access_swpts.fall[pa_state];
      tx_hdr_ptr->access_swpts.rise[pa_state] = 
        tx_1x_ptr->access_swpts.rise[pa_state];
    }
    /* Copy the hysteresis value */
    tx_hdr_ptr->access_swpts.fall_hold_time = 
      tx_1x_ptr->access_swpts.fall_hold_time;

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_1x_access_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_hdr_access_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR Rev0 Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the HDR radio is in
  Rev0 state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure, used to load default
  data
 
  @param tx_hdr_ptr
  Pointer to the HDR Tx static NV data structure, used to load default
  data

  @param tx_swpt_nv_item
  HDR Rev0 Tx Switchpoint NV item
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_dor0_tx_swpts
(
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_hdr_tx_static_type* tx_hdr_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_dor0_swpt_type *tx_swpt_nv_ptr;

  /* Iterators */
  int pa_state, rc_rate;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dor0_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_dor0_swpt_type *)nv_buffer;
    /* Copy switchpoint data for all reverse link data rates */
    for ( rc_rate = 0;
          rc_rate < RF_CDMA_NV_HDR_RRI_RATES_NUM;
          rc_rate ++ )
    {
      /* Lowest PA state switchpoints are -70 dBm */
      tx_hdr_ptr->dor0_swpts_rri[rc_rate].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[rc_rate].rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[rc_rate].fall[pa_state] =
          tx_swpt_nv_ptr->rate[rc_rate].sp.sp_data[pa_state-1].fall;
        tx_hdr_ptr->dor0_swpts_rri[rc_rate].rise[pa_state] =
          tx_swpt_nv_ptr->rate[rc_rate].sp.sp_data[pa_state-1].rise;
      }
      /* Set remaining PA states to max power 40 dBm */
      for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[rc_rate].fall[pa_state] = 400;
        tx_hdr_ptr->dor0_swpts_rri[rc_rate].rise[pa_state] = 400;
      }

      /* Copy the hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[rc_rate].fall_hold_time = 
        tx_swpt_nv_ptr->rate[rc_rate].sp.fall_time_hysteresis;
    }

    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If HDR Rev0 swpts are not-active, use 1x voice swpt data for
    all reverse link rates */
    for ( rc_rate = 0;
          rc_rate < RF_CDMA_NV_HDR_RRI_RATES_NUM;
          rc_rate ++ )
    {
      for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[rc_rate].fall[pa_state] = 
          tx_1x_ptr->voice_swpts.fall[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[rc_rate].rise[pa_state] = 
          tx_1x_ptr->voice_swpts.rise[pa_state];
      }
      /* Copy the hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[rc_rate].fall_hold_time = 
        tx_1x_ptr->voice_swpts.fall_hold_time;
    }

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_dor0_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_dor0_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR RevA Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the HDR radio is in
  RevA state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure, used to load default
  data
 
  @param tx_hdr_ptr
  Pointer to the HDR Tx static NV data structure, used to load default
  data

  @param tx_swpt_nv_item
  HDR RevA Tx Switchpoint NV item
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_dora_tx_swpts
(
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_hdr_tx_static_type* tx_hdr_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR RevA Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_swpt_type   *tx_swpt_nv_ptr;

  /* Iterator */
  int pa_state;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dora_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_swpt_type *)nv_buffer;
    /* Lowest PA state switchpoints are -70 dBm */
    tx_hdr_ptr->dorA_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
    tx_hdr_ptr->dorA_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
    for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
          pa_state++ )
    {
      tx_hdr_ptr->dorA_swpts.fall[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].fall;
      tx_hdr_ptr->dorA_swpts.rise[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].rise;
    }
    /* Set remaining PA states to max power 40 dBm */
    for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_hdr_ptr->dorA_swpts.fall[pa_state] = 400;
      tx_hdr_ptr->dorA_swpts.rise[pa_state] = 400;
    }

    /* Copy the hysteresis value */
    tx_hdr_ptr->dorA_swpts.fall_hold_time = 
      tx_swpt_nv_ptr->fall_time_hysteresis;

    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If HDR RevA swpts are not-active, use 1x voice switchpoint data as
    default */
    for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_hdr_ptr->dorA_swpts.fall[pa_state] = 
        tx_1x_ptr->voice_swpts.fall[pa_state];
      tx_hdr_ptr->dorA_swpts.rise[pa_state] = 
        tx_1x_ptr->voice_swpts.rise[pa_state];
    }

    /* Copy the hysteresis value */
    tx_hdr_ptr->dorA_swpts.fall_hold_time = 
      tx_1x_ptr->voice_swpts.fall_hold_time;

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_dora_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;

} /* rf_cdma_nv_load_dora_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR RevB Tx switchpoint data for the requested NV item.

  @details
  Loads the Tx switchpoint values that are used when the HDR radio is in
  RevB state. The NV data contains fall and rise switchpoints in dBm10
  for each PA state transition, upto a maximum of 3, as well as the
  fall time hysteresis value common across all PA state transitions.
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure, used to load default
  data
 
  @param tx_hdr_ptr
  Pointer to the HDR Tx static NV data structure, used to load default
  data

  @param tx_swpt_nv_item
  HDR RevB Tx Switchpoint NV item
 
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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_dorb_tx_swpts
(
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_hdr_tx_static_type* tx_hdr_ptr,
  const rfnv_item_id_enum_type tx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR RevB Tx switchpoint items */
  nv_stat_enum_type   nv_status;
  rfnv_item_type      *nv_buffer;
  rfnv_item_size_type returned_size =
                            sizeof(rfnv_item_type);
  rfnv_tx_swpt_type   *tx_swpt_nv_ptr;

  /* Iterator */
  int pa_state;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );
  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dorb_tx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( tx_swpt_nv_item,
                            nv_buffer,
                            &returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* If Tx switchpoints were read successfully, configure the data
    structure */
    tx_swpt_nv_ptr = (rfnv_tx_swpt_type *)nv_buffer;
    /* Lowest PA state switchpoints are -70 dBm */
    tx_hdr_ptr->dorB_mc_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
    tx_hdr_ptr->dorB_mc_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
    for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_4;
          pa_state++ )
    {
      tx_hdr_ptr->dorB_mc_swpts.fall[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].fall;
      tx_hdr_ptr->dorB_mc_swpts.rise[pa_state] = 
        tx_swpt_nv_ptr->sp_data[pa_state-1].rise;
    }
    /* Set remaining PA states to max power 40 dBm */
    for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_hdr_ptr->dorB_mc_swpts.fall[pa_state] = 400;
      tx_hdr_ptr->dorB_mc_swpts.rise[pa_state] = 400;
    }

    /* Copy the hysteresis value */
    tx_hdr_ptr->dorB_mc_swpts.fall_hold_time = 
      tx_swpt_nv_ptr->fall_time_hysteresis;

    retval = TRUE;  
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If HDR RevB swpts are not-active, use 1x voice switchpoint data as
    default */
    for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state < RFM_PA_GAIN_STATE_NUM;
          pa_state++ )
    {
      tx_hdr_ptr->dorB_mc_swpts.fall[pa_state] = 
        tx_1x_ptr->voice_swpts.fall[pa_state];
      tx_hdr_ptr->dorB_mc_swpts.rise[pa_state] = 
        tx_1x_ptr->voice_swpts.rise[pa_state];
    }
    /* Copy the hysteresis value */
    tx_hdr_ptr->dorB_mc_swpts.fall_hold_time = 
      tx_1x_ptr->voice_swpts.fall_hold_time;

    retval = TRUE;
  } /* else if ( nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_dorb_tx_swpts: "
              "NV state is inconsistent: Swpt item %d status %d",
              tx_swpt_nv_item, nv_status );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;

} /* rf_cdma_nv_load_dorb_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load 1x Tx switchpoint data for the requested NV items,
  across all reverse link configurations.

  @details
  Loads the 1x Tx switchpoints for all reverse link configurations
 
  @param band
  Band for which legacy PA switchpoints may be loaded

  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure
 
  @param tx_access
  1x Access Tx Switchpoint NV item
 
  @param tx_voice
  1x Voice Tx Switchpoint NV item
 
  @param tx_data
  1x Data Tx Switchpoint NV item
 
  @param pa_r1_rise_item
  The NV item that contains the PA R1 Rise value.
 
  @param pa_r1_fall_item
  The NV item that contains the PA R1 Fall value.
 
  @param pa_r2_rise_item
  The NV item that contains the PA R2 Rise value.
 
  @param pa_r2_fall_item
  The NV item that contains the PA R2 Fall value.

  @param pa_r3_rise_item
  The NV item that contains the PA R3 Rise value.
 
  @param pa_r3_fall_item
  The NV item that contains the PA R3 Fall value.

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
  TRUE if the the NV item is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_1x_tx_swpts
(
  rfm_cdma_band_class_type band,
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_cdma_static_nv_items_map_type *static_nv_tbl,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  const rfnv_item_id_enum_type tx_access = static_nv_tbl->access_1x_swpts;
  const rfnv_item_id_enum_type tx_voice = static_nv_tbl->voice_1x_swpts;
  const rfnv_item_id_enum_type tx_data = static_nv_tbl->data_1x_swpts;

  const rfnv_item_id_enum_type pa_r1_rise_item = static_nv_tbl->pa_r1_rise;
  const rfnv_item_id_enum_type pa_r1_fall_item = static_nv_tbl->pa_r1_fall;
  const rfnv_item_id_enum_type pa_r2_rise_item = static_nv_tbl->pa_r2_rise;
  const rfnv_item_id_enum_type pa_r2_fall_item = static_nv_tbl->pa_r2_fall;
  const rfnv_item_id_enum_type pa_r3_rise_item = static_nv_tbl->pa_r3_rise;
  const rfnv_item_id_enum_type pa_r3_fall_item = static_nv_tbl->pa_r3_fall;

  /* Load the 1x Voice Tx switchpoints. These are considered as default
  for all other switchpoint items that are not active. If these NVs
  are inactive, we load legacy PA switchpoints or default data as of now */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_1x_voice_tx_swpts( band,
                                                tx_1x_ptr,
                                                tx_voice,
                                                pa_r1_rise_item, 
                                                pa_r1_fall_item,
                                                pa_r2_rise_item, 
                                                pa_r2_fall_item,
                                                pa_r3_rise_item, 
                                                pa_r3_fall_item,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }
  /* Load the 1x Access Tx switchpoints. These are considered as default
  for HDR access if the HDR access switchpoint item is not active */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_1x_access_tx_swpts( tx_1x_ptr,
                                                 tx_access,
                                                 caller_tcb_ptr,
                                                 task_nv_wait_sig,
                                                 task_wait_func_ptr );
  }
  /* Load the 1x Data Tx switchpoints */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_1x_data_tx_swpts( band,
                                               tx_1x_ptr,
                                               tx_data,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_1x_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR Tx switchpoint data for the requested NV items,
  across all reverse link configurations.

  @details
  Loads the HDR Tx switchpoints for all reverse link configurations
 
  @param tx_1x_ptr
  Pointer to the 1x Tx static NV data structure, used to load default
  data
 
  @param tx_hdr_ptr
  Pointer to the HDR Tx static NV data structure

  @param tx_access
  HDR Access Tx Switchpoint NV item
 
  @param tx_dor0
  HDR Rev0 Tx Switchpoint NV item
 
  @param tx_dora
  HDR RevA Tx Switchpoint NV item
 
  @param tx_dorb
  HDR RevB Tx Switchpoint NV item

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
  TRUE if the the NV item is valid, otherwise FALSE.
 
  @internal
  The HDR tx switchpoint NV loading process relies on the 1x items to
  be previously loaded. The 1x data is used as default in case the
  HDR items are inactive. If the 1x data is uninitialized, the HDR
  items will contain the same uninitialized data.
*/
static
boolean
rf_cdma_nv_load_hdr_tx_swpts
(
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_hdr_tx_static_type* tx_hdr_ptr,
  rf_cdma_static_nv_items_map_type *static_nv_tbl,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;
  const rfnv_item_id_enum_type tx_access = static_nv_tbl->access_hdr_swpts;
  const rfnv_item_id_enum_type tx_dor0 = static_nv_tbl->dor0_hdr_swpts;
  const rfnv_item_id_enum_type tx_dora = static_nv_tbl->dora_hdr_swpts;
  const rfnv_item_id_enum_type tx_dorb = static_nv_tbl->dorb_hdr_swpts;

  /* Load the HDR Access Tx switchpoints */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_hdr_access_tx_swpts( tx_1x_ptr,
                                                  tx_hdr_ptr,
                                                  tx_access,
                                                  caller_tcb_ptr,
                                                  task_nv_wait_sig,
                                                  task_wait_func_ptr );
  }
  /* Load the HDR Rev0 Tx switchpoints */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_dor0_tx_swpts( tx_1x_ptr,
                                            tx_hdr_ptr,
                                            tx_dor0,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );
  }
  /* Load the HDR RevA Tx switchpoints */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_dora_tx_swpts( tx_1x_ptr,
                                            tx_hdr_ptr,
                                            tx_dora,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );
  }
  /* Load the HDR RevB Tx switchpoints */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_dorb_tx_swpts( tx_1x_ptr,
                                            tx_hdr_ptr,
                                            tx_dorb,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_hdr_tx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Multi Linearizer Tx APT Adjust data for 1x and HDR.

  @details
  Loads the Tx linearizer/DAC set-point APT adjust values for both 1x and
  HDR. These values are based on different reverse link configurations,
  and are provided as static NV. The driver will differentiate between
  1x and HDR data values, but are loaded from the same NV item, which is
  only device and band specific.
 
  Power adjustments are provided for different 1x RCs for voice and data,
  and for different RRIs and payload sizes for DO Rev0 and RevA
  respectively.
 
  The default loading functionality must provide for a fixed reverse link
  configuration to be used as default for both 1x and HDR.

  @param static_tx_struct_1x
  Data structure for 1x Tx static NV
 
  @param static_tx_struct_hdr
  Data structure for HDR Tx static NV. Should be NULL for Chain 2
 
  @param multi_lin_apt_adj_item
  NV item containing power adjustment values for both 1x and HDR
 
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
  TRUE always. If the Switchpoint NV is inactive, use the default
  values hardcoded in the old NV table.
*/
static
boolean
rf_cdma_nv_load_tx_apt_adj
(
  rf_1x_tx_static_type* const static_tx_struct_1x,
  rf_hdr_tx_static_type* const static_tx_struct_hdr,

  const rfnv_item_id_enum_type multi_lin_apt_adj_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_tx_apt_adj_type  *apt_adj_data;

  /* Iterator */
  int rc_type;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_apt_adj: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( multi_lin_apt_adj_item,
                            nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    apt_adj_data = (rfnv_tx_apt_adj_type*)nv_buffer;

    /* Populate the power adjustment values for the respective 1x
    and HDR reverse link configuration based on the NV structure
    definition */

    /* 1x values */
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_PILOT].lut_offset = 
                                         apt_adj_data->data_1x.pilot;
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_PILOT].iq_gain_adj =
     rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_1x.pilot);
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC1].lut_offset = 
                                         apt_adj_data->data_1x.rc1rc2;
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC1].iq_gain_adj = 
     rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_1x.rc1rc2);
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC3_VOICE].lut_offset = 
                                         apt_adj_data->data_1x.rc3rc4_voice;
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC3_VOICE].iq_gain_adj = 
     rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_1x.rc3rc4_voice);
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC3_DATA].lut_offset = 
                                         apt_adj_data->data_1x.rc3rc4_data;
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC3_DATA].iq_gain_adj = 
      rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_1x.rc3rc4_data);
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC8_VOICE].lut_offset = 
                                         apt_adj_data->data_1x.rc8_voice;
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC8_VOICE].iq_gain_adj = 
      rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_1x.rc8_voice);
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC8_DATA].lut_offset = 
                                         apt_adj_data->data_1x.rc8_data;
    static_tx_struct_1x->
      apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC8_DATA].iq_gain_adj = 
      rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_1x.rc8_data);


    /* No need to load HDR data for Chain 2 */
    if ( static_tx_struct_hdr != NULL )
    {
      /* HDR Rev0 values */
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_PILOT].lut_offset = 
                                         apt_adj_data->data_dor0.dor0_pilot;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_PILOT].iq_gain_adj = 
       rf_cdma_utils_compute_tx_iq_gain_adj_val(
               apt_adj_data->data_dor0.dor0_pilot);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI0].lut_offset =
       apt_adj_data->data_dor0.dor0_1;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI0].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dor0.dor0_1);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI1].lut_offset =
       apt_adj_data->data_dor0.dor0_2;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI1].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dor0.dor0_2);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI2].lut_offset =
       apt_adj_data->data_dor0.dor0_3;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI2].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dor0.dor0_3);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI3].lut_offset =
       apt_adj_data->data_dor0.dor0_4;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI3].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dor0.dor0_4);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI4].lut_offset =
       apt_adj_data->data_dor0.dor0_5;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI4].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dor0.dor0_5);
  
      /* HDR RevA values */
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS0].lut_offset =
       apt_adj_data->data_dorA.dorA_ps0;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS0].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps0);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS1].lut_offset =
       apt_adj_data->data_dorA.dorA_ps1;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS1].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps1);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS2].lut_offset =
       apt_adj_data->data_dorA.dorA_ps2;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS2].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps2);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS3].lut_offset =
       apt_adj_data->data_dorA.dorA_ps3;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS3].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps3);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS4].lut_offset =
       apt_adj_data->data_dorA.dorA_ps4;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS4].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps4);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS5].lut_offset =
       apt_adj_data->data_dorA.dorA_ps5;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS5].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps5);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS6].lut_offset =
       apt_adj_data->data_dorA.dorA_ps6;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS6].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps6);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS7].lut_offset =
       apt_adj_data->data_dorA.dorA_ps7;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS7].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps7);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS8].lut_offset =
       apt_adj_data->data_dorA.dorA_ps8;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS8].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps8);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS9].lut_offset =
       apt_adj_data->data_dorA.dorA_ps9;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS9].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps9);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS10].lut_offset =
       apt_adj_data->data_dorA.dorA_ps10;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS10].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps10);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS11].lut_offset =
       apt_adj_data->data_dorA.dorA_ps11;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS11].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps11);
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS12].lut_offset =
       apt_adj_data->data_dorA.dorA_ps12;
      static_tx_struct_hdr->
        apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS12].iq_gain_adj =
       rf_cdma_utils_compute_tx_iq_gain_adj_val(apt_adj_data->data_dorA.dorA_ps12);
    } /* if ( static_tx_struct_hdr != NULL ) */

    retval = TRUE;
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If the NV item is inactive, load default values.
    Currently, program all power adjustments to LUT to zero and
    the IQ gain must be backed-off to match the legacy IQ gain values.
    Legacy IQ gain = 901 (0x385)
    New default IQ gain = 901 (0x385)
    Backoff value in Q16 = 65536 (0x10000) = (legacy IQ gain/new IQ gain) << 16
    */
    for ( rc_type = RF_CDMA_NV_1X_TX_IQGAIN_PILOT;
          rc_type < RF_CDMA_NV_1X_TX_IQGAIN_RC_NUM;
          rc_type ++ )
    {
      static_tx_struct_1x->apt_adj_value[rc_type].lut_offset = 0;
      static_tx_struct_1x->apt_adj_value[rc_type].iq_gain_adj =
       RF_CDMA_NV_DEFAULT_IQ_GAIN_BACKOFF;
    }

    /* No need to load HDR data for Chain 2, the pointer passed in
    is NULL */
    if ( static_tx_struct_hdr != NULL )
    {
      for ( rc_type = RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_PILOT;
            rc_type < RF_CDMA_NV_HDR_TX_IQGAIN_RC_NUM;
            rc_type ++ )
      {
        static_tx_struct_hdr->apt_adj_value[rc_type].lut_offset = 0;
        static_tx_struct_hdr->apt_adj_value[rc_type].iq_gain_adj = 
         RF_CDMA_NV_DEFAULT_IQ_GAIN_BACKOFF;
      }
    } /* if ( static_tx_struct_hdr != NULL ) */

    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_apt_adj: "
            "NV calibration state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_tx_apt_adj() */

int16
hi_resolution_log_div ( uint8 divisor , int max_power_dBm10 )
{
  int16 max_power_dBm = max_power_dBm10 / 10 ; 
  int max_power_per_carrier_dBm10 = 0 ;

  /* from 12 to 30 dBm */
  uint32 wattTranslation[] = {
    /*dBM:  7       8      9      10       11                             */
           5012 , 6310 , 7943 , 10000 , 12589 , 
    /* mW X 100 =>                                                        */

    /*dBM:  12       13      14      15      16      17      18      19   */
           15849 , 19953 , 25118 , 31623 , 39811 , 50119 , 63096 , 79433 , 
    /* mW X 100 =>                                                        */

    /* dBM: 20       21       22       23       24       25        26     */      
          100000 , 125893 , 158490 , 199526 , 251189 , 316228 ,  398107 ,        
    /* mW X 100 =>                                                        */
    /* dBM: 27       28       29       30        31       32              */      
          501187 , 630957 , 794328 , 1000000 , 1258925 , 1584893          
    /* mW X 100 =>                                                        */
   } ;
 
  /* 1 to 3 carriers with max power between 12dBm and 32 dBm10 */
  if ( ( divisor <= RFM_CDMA_CARRIER_NUM ) &&
       ( max_power_dBm >= 12 ) &&
       ( max_power_dBm <= 32 ) )
  {
    uint8 index = max_power_dBm - 12 + 5 ;
    uint8 reverse_lookup_count = 0 ;

    /* Equal power divided between 2 or 3 carriers */
    uint32 mW = wattTranslation[index]/divisor  ; 

    /* Start from 12 dBm because thats the lowest input power permissible */
    for ( reverse_lookup_count = 7 ; 
          reverse_lookup_count < ( ( (sizeof wattTranslation)/(sizeof wattTranslation[0])) - 1 ) ;
          reverse_lookup_count++  )
    {
      if ( ( mW >= wattTranslation[reverse_lookup_count] ) && 
           ( mW <= wattTranslation[reverse_lookup_count + 1] ) )
      {
        uint32 diff1 = abs ( wattTranslation[reverse_lookup_count] ) ;
        uint32 diff2 = abs ( wattTranslation[reverse_lookup_count + 1] ) ;    
        
        /* Bigger the difference, more closer one is to the next entry */
        uint8 per_carrier_dbm_power = (diff1 > diff2) ? 
           (reverse_lookup_count + 1) : reverse_lookup_count ;  

        /* Back to dBm10 */
        max_power_per_carrier_dBm10 = (per_carrier_dbm_power + 7) * 10 ;
        break ;
      }
    }
  }
  return max_power_per_carrier_dBm10 ;
}


uint8
bitcount( uint16 n  ) 
{
  uint8 nos_of_ones = 0 ;
  uint16 i = 0 ;
  for ( i = 1 ; i <= n ; i = i << 1 )
  {
    if  (n & i) ++nos_of_ones ;
  }

  return nos_of_ones ;
}

int16
low_resolution_log_div ( uint8 divisor , int16 max_power_dBm10 )
{
  int16 max_power_per_carrier_dBm10 = 0 ;
  switch ( divisor )
 {
   case 1: 
   max_power_per_carrier_dBm10 = max_power_dBm10 ;
   break;

  case 2:
    /* 1/2 linear is 3 dB lower in log */
    max_power_per_carrier_dBm10 = max_power_dBm10 - 30 ;
    break;

  case 3:
    /* 1/3 linear is 4.7 dB lower in log */
    max_power_per_carrier_dBm10 = max_power_dBm10 - 47 ;
    break;

  default:
    RF_MSG_1 ( RF_LOW , 
               "low_resolution_log_div() Don not know how to use %d" 
               , divisor ) ;

    break ;
  }
  return max_power_per_carrier_dBm10 ;
}

int16
calculate_eighth__y_element (
  const rf_cdma_tx_pwr_limit_data_type* p_hdr_data )
{
  int16 max_eq_power = p_hdr_data->tx_pwr_limit_dBm10[0] , count = 0 ;
  const int max_count = MAX_HDR_PWR_LIMIT_COORDINATES - 1 ;

  /* Choose the lowest of MAX_HDR_PWR_LIMIT_COORDINATES entries */
  for ( count = 0 ; count < max_count ; count++ )
  { 
    if ( max_eq_power > 
          p_hdr_data->tx_pwr_limit_dBm10[count + 1])
    {
      max_eq_power =
        p_hdr_data->tx_pwr_limit_dBm10[count + 1] ;
    }
  }
  return max_eq_power ;
}

int16
calculate_eighth__x_element ( 
  int16 max_power_dBm10  ,
  uint16 carrier_config_pattern )  
{
  /* For SCDO and widlcard implementaions  */
  int16 max_power_per_carrier_dBm10 = max_power_dBm10 ;

  if ( ( carrier_config_pattern != ((uint16)~0) ) && 
       ( max_power_dBm10 != 0 ) )
  {
    uint8 nos_of_ones = bitcount ( carrier_config_pattern ) ;
    max_power_per_carrier_dBm10 =
      low_resolution_log_div ( nos_of_ones , max_power_dBm10 ) ;
  }
  return max_power_per_carrier_dBm10 ;
}

void 
rf_hdr_print_max_power_values ( 
  const rf_cdma_tx_pwr_limit_data_type* p_hdr_data )
{
  if ( NULL != p_hdr_data )
  {
     RF_MSG_4 ( RF_LOW ,
                "BW:%d, CCP:%d, LFP:%d, HFP:%d ",
                p_hdr_data->bw ,
                p_hdr_data->carrier_config_pattern ,
                p_hdr_data->lowest_freq_chan ,
                p_hdr_data->highest_freq_chan ) ;

     RF_MSG_8 ( RF_LOW ,
                  "txLim:%d,%d,%d,%d,%d,%d,%d,%d",
                  p_hdr_data->tx_pwr_limit_dBm10[0] ,
                  p_hdr_data->tx_pwr_limit_dBm10[1] ,
                  p_hdr_data->tx_pwr_limit_dBm10[2] ,
                  p_hdr_data->tx_pwr_limit_dBm10[3] ,
                  p_hdr_data->tx_pwr_limit_dBm10[4] ,
                  p_hdr_data->tx_pwr_limit_dBm10[5] ,
                  p_hdr_data->tx_pwr_limit_dBm10[6] ,
                  p_hdr_data->tx_pwr_limit_dBm10[7] ) ; 


     RF_MSG_8 ( RF_LOW ,
                  "HI X:%d,%d,%d,%d,%d,%d,%d,%d",
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[0].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[1].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[2].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[3].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[4].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[5].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[6].x_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[7].x_axis_val ) ; 
     RF_MSG_8 ( RF_LOW ,
                  "HI Y:%d,%d,%d,%d,%d,%d,%d,%d",
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[0].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[1].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[2].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[3].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[4].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[5].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[6].y_axis_val ,
                  p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[7].y_axis_val ) ; 

     RF_MSG_8 ( RF_LOW ,
                  "LO X:%d,%d,%d,%d,%d,%d,%d,%d",
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[0].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[1].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[2].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[3].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[4].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[5].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[6].x_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[7].x_axis_val ) ; 
     RF_MSG_8 ( RF_LOW ,
                  "LO Y:%d,%d,%d,%d,%d,%d,%d,%d",
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[0].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[1].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[2].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[3].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[4].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[5].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[6].y_axis_val ,
                  p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[7].y_axis_val ) ; 

     RF_MSG_4 ( RF_LOW ,
                "LI:%d, FB:%d, RefTempMDSP:%d, ver:%d ",
                p_hdr_data->LUT_indicator ,
                p_hdr_data->funnel_bias ,
                p_hdr_data->mdsp_reading_at_reference_temperature ,
                p_hdr_data->table_version ) ;

  }
}

boolean
check_if_max_eq_pwr_greater_than_max_neq_value (
  const rf_cdma_tx_pwr_limit_data_type* p_hdr_data ) 
{
  boolean is_max_eq_greater_than_y8 = FALSE ;
  if ( NULL != p_hdr_data )
  {
    if ( p_hdr_data->table_monotonicity == LUT_MONOTONIC ) 
    {  
      int16 max_eq_power = p_hdr_data->tx_pwr_limit_dBm10[0] , count = 0 ;
      const int max_count = MAX_HDR_PWR_LIMIT_COORDINATES - 1 ;
      int16 max_neq_power_lo_y8 = 
        p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[max_count].y_axis_val ;
      int16 max_neq_power_hi_y8 = 
        p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[max_count].y_axis_val ; 
         
      /* Choose the highest of MAX_HDR_PWR_LIMIT_COORDINATES entries */
      for ( count = 0 ; count < max_count ; count++ )
      { 
        if ( max_eq_power < 
              p_hdr_data->tx_pwr_limit_dBm10[count + 1])
        {
          max_eq_power =
            p_hdr_data->tx_pwr_limit_dBm10[count + 1] ;
        }
      }
      if ( ( max_eq_power > max_neq_power_lo_y8 ) ||
           ( max_eq_power > max_neq_power_hi_y8 ) )
      {
        RF_MSG ( RF_HIGH , "max_eq_power is greater than max_neq_power " ) ;
      }
      else
      {
        RF_MSG ( RF_HIGH , "max_eq_power is less or eq to max_neq_power " ) ;
        is_max_eq_greater_than_y8 = TRUE ;
      }
      RF_MSG_3 ( RF_HIGH , "In dBm10 eq = %d | neq_lo = %d | neq_hi = %d " ,
                 max_eq_power , max_neq_power_lo_y8 , max_neq_power_hi_y8 ) ;
    }
    else
    {
      RF_MSG ( RF_HIGH , 
               "Skipping neq greater than eq pwr check "
               "because monotonicty check failed " ) ;
    }
 }
  return is_max_eq_greater_than_y8 ;
}

boolean  
check_if_descending ( int16 data[] , const int length )
{
  int count = 0 ;
  boolean descending = TRUE ;
  for ( ; count < (length - 1) ; count++ )
  {
    if ( data[count] < data[count + 1] )
    {
      descending = FALSE ;  
      break ;
    }
  }
  return descending ;
}

boolean  
check_if_ascending ( int16 data[] , const int length )
{
  int count = 0 ;
  boolean ascending = TRUE ;
  for ( ; count < (length - 1) ; count++ )
  {
    if ( data[count] > data[count + 1] )
    {
      ascending = FALSE ;  
      break ;
    }
  }
  return ascending ;
}


lut_monotonicity_t 
check_nv_monotonocity (const rf_cdma_tx_pwr_limit_data_type* p_hdr_data ) 
{
 
  lut_monotonicity_t non_monotonic = LUT_MONOTONIC ;
  boolean success = TRUE ;

  if ( NULL != p_hdr_data )
  {
    int16 data[2][MAX_HDR_PWR_LIMIT_COORDINATES] , count ;

    /* Order of the remaining things executed in this fucntion is important*/
    /* HI LUT */
    for ( count = 0 ; count < MAX_HDR_PWR_LIMIT_COORDINATES ; count++ )
    {
      data[0][count] = 
        p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[count].x_axis_val ;
      data[1][count] = 
        p_hdr_data->hi_tx_imbalanced_carrier_pwr_lut[count].y_axis_val ;
    }
    success = 
      check_if_ascending ( data[0] , MAX_HDR_PWR_LIMIT_COORDINATES ) ;
    non_monotonic |= 
      ( success == TRUE ) ? 0 : ( 1 << 0 ) ;

    success = 
      check_if_descending ( data[1] , MAX_HDR_PWR_LIMIT_COORDINATES ) ;
    non_monotonic |= 
      ( success == TRUE ) ? 0 : ( 1 << 1 ) ;


    /* LOW LUT */
    for ( count = 0 ; count < MAX_HDR_PWR_LIMIT_COORDINATES ; count++ )
    {
      data[0][count] = 
        p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[count].x_axis_val ;
      data[1][count] = 
        p_hdr_data->lo_tx_imbalanced_carrier_pwr_lut[count].y_axis_val ;
    }
    success = 
      check_if_ascending ( data[0] , MAX_HDR_PWR_LIMIT_COORDINATES ) ;
    non_monotonic |= 
      ( success == TRUE ) ? 0 : ( 1 << 2 ) ;


    success = 
      check_if_descending ( data[1] , MAX_HDR_PWR_LIMIT_COORDINATES ) ;
    non_monotonic |= 
      ( success == TRUE ) ? 0 : ( 1 << 3 ) ;

 }
 return non_monotonic ;
}

/*!
  @brief
  Function to load hdr power limts.

  @details
  Checks that the NV items for this device's and band's hdr power limit 
  configuration data are loaded in NV, and initializes the internal data 
  structures with this data if available.

  @param tx_static_struct
  A pointer to the rf_hdr_tx_static_type data structure that is to be loaded
  with the NV data.

  @param band
  The band for which hdr power limit data needs to be loaded. 

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
  FALSE an unexpected NV error occurred, otherwise TRUE.
*/
static boolean
rf_cdma_nv_load_hdr_pwr_limits
(
  rf_hdr_tx_static_type* const tx_static_struct ,
  const rfm_cdma_band_class_type band ,
  const rfnv_item_id_enum_type hdr_tx_pwr_lim_item ,
  rex_tcb_type* const caller_tcb_ptr ,
  const rex_sigs_type task_nv_wait_sig ,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval  = TRUE ;

  nv_stat_enum_type     nv_status ; 
  rfnv_item_type*       hdr_tx_pwr_lim_compressed_buffer ;
  rfnv_item_size_type   hdr_tx_pwr_returned_size =
                                  sizeof(rfnv_item_type) ;

  hdr_tx_pwr_lim_compressed_buffer = modem_mem_alloc( sizeof(rfnv_item_type) ,
                                        MODEM_MEM_CLIENT_RFA ) ;
  if ( hdr_tx_pwr_lim_compressed_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_hdr_pwr_limits: Could not "
                      "allocate buffer memory on heap" ) ;
    retval = FALSE ;
  }
  else
  {

    nv_status =
      rfcommon_rfnv_get_item( hdr_tx_pwr_lim_item,
                              hdr_tx_pwr_lim_compressed_buffer,
                              &hdr_tx_pwr_returned_size,
                              caller_tcb_ptr,
                              task_nv_wait_sig,
                              task_wait_func_ptr );
  
    if ( ( nv_status == NV_DONE_S ) || ( nv_status == NV_NOTACTIVE_S ) ) 
    {

      uint8 number_of_items_read = 0 ;
      uint32 uncompress_size = sizeof (rfnv_tx_pwr_limit_data_type) * MAX_HDR_PWR_PATTERNS_PER_BAND ;
      rfnv_tx_pwr_limit_data_type* hdr_nv_tx_pwr_lim_uncompressed =
        (rfnv_tx_pwr_limit_data_type*)modem_mem_alloc(uncompress_size, MODEM_MEM_CLIENT_RFA); 
       rfnv_tx_pwr_limit_data_type* hdr_nv_tx_pwr_lim_uncompressed_copy =
         hdr_nv_tx_pwr_lim_uncompressed ;
      int8 uncompress_status = Z_ERRNO;

      /* For covering else if ( nv_status == NV_NOTACTIVE_S ) case */
      rf_cdma_tx_pwr_limit_data_type* p_tx_pwr_limit_data = NULL ;
      uint16 count = 0 , channel_count = 0 ;      

      if ( nv_status == NV_DONE_S )
      {
        /* Decompress NV data */
        uncompress_status = uncompress((uint8 *) hdr_nv_tx_pwr_lim_uncompressed ,  
                                   &uncompress_size,
                                   (const uint8 *)hdr_tx_pwr_lim_compressed_buffer , 
                                   (uint32)hdr_tx_pwr_returned_size);
      }

      /* Zero out the items in case of any failure*/
      if ( ( nv_status == NV_NOTACTIVE_S ) || 
           ( uncompress_status != Z_OK ) ||
           ( hdr_nv_tx_pwr_lim_uncompressed == NULL ) )
      {
          tx_static_struct->tx_pwr_limit_data = NULL ;
      }
      else
      {
        number_of_items_read = (uint8)(uncompress_size/sizeof(rfnv_tx_pwr_limit_data_type)) ;
        p_tx_pwr_limit_data = (rf_cdma_tx_pwr_limit_data_type*)modem_mem_alloc(
                                    (number_of_items_read*(sizeof(rf_cdma_tx_pwr_limit_data_type))) ,
                                    MODEM_MEM_CLIENT_RFA ) ; 
        tx_static_struct->tx_pwr_limit_data = p_tx_pwr_limit_data ;
        if ( NULL != p_tx_pwr_limit_data )
        {
          /* Initialize all members inside NV struct to be zero */
          memset( p_tx_pwr_limit_data, 0, 
                  number_of_items_read*(sizeof(rf_cdma_tx_pwr_limit_data_type)) );

          for ( channel_count = 0 ; channel_count < number_of_items_read ; channel_count++ )
          { 
            /* Member Copy */
              p_tx_pwr_limit_data[channel_count].number_of_elements = number_of_items_read ;
              p_tx_pwr_limit_data[channel_count].bw = hdr_nv_tx_pwr_lim_uncompressed[channel_count].bw ;
              p_tx_pwr_limit_data[channel_count].carrier_config_pattern = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].carrier_config_pattern ;
              p_tx_pwr_limit_data[channel_count].lowest_freq_chan = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].lowest_freq_chan ;
              p_tx_pwr_limit_data[channel_count].highest_freq_chan = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].highest_freq_chan ;
              

            for ( count = 0 ; count < MAX_HDR_PWR_LIMIT_THERMAL_BINS ; count++ )
              {        
                  p_tx_pwr_limit_data[channel_count].tx_pwr_limit_dBm10[count] = 
                      hdr_nv_tx_pwr_lim_uncompressed[channel_count].tx_pwr_limit_dBm10[count] ;
            }

            for ( count = 0 ; count < MAX_NV_HDR_PWR_LIMIT_COORDINATES ; count++ )
            {
              p_tx_pwr_limit_data[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].x_axis_val = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].x_axis_val ;
              p_tx_pwr_limit_data[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].y_axis_val = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].y_axis_val ;
                  
              p_tx_pwr_limit_data[channel_count].lo_tx_imbalanced_carrier_pwr_lut[count].x_axis_val = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].lo_tx_imbalanced_carrier_pwr_lut[count].x_axis_val ;
              p_tx_pwr_limit_data[channel_count].lo_tx_imbalanced_carrier_pwr_lut[count].y_axis_val = 
                  hdr_nv_tx_pwr_lim_uncompressed[channel_count].lo_tx_imbalanced_carrier_pwr_lut[count].y_axis_val ;
              } /* for ( count = 0 */

             
            p_tx_pwr_limit_data[channel_count].LUT_indicator = 
                hdr_nv_tx_pwr_lim_uncompressed[channel_count].LUT_indicator ;
            p_tx_pwr_limit_data[channel_count].funnel_bias = 
                hdr_nv_tx_pwr_lim_uncompressed[channel_count].funnel_bias ;
            p_tx_pwr_limit_data[channel_count].mdsp_reading_at_reference_temperature = 
                hdr_nv_tx_pwr_lim_uncompressed[channel_count].mdsp_reading_at_reference_temperature ;
            p_tx_pwr_limit_data[channel_count].table_version = 
                hdr_nv_tx_pwr_lim_uncompressed[channel_count].table_version ;


            /* THE LAST ELEMENT
            The 8th entry in the NV is absent and needs to be filled once. The x dn y
               coordinates are:
               y = Legacy Max Power
               x = Legacy Max Power / Number of carriers */
            /* Substract 1 to make the bin_number 0 based */
            p_tx_pwr_limit_data[channel_count].lo_tx_imbalanced_carrier_pwr_lut[count].y_axis_val =
            p_tx_pwr_limit_data[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].y_axis_val = 
            calculate_eighth__y_element (&(p_tx_pwr_limit_data[channel_count]));

            p_tx_pwr_limit_data[channel_count].lo_tx_imbalanced_carrier_pwr_lut[count].x_axis_val =
            p_tx_pwr_limit_data[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].x_axis_val =
             calculate_eighth__x_element (  
               p_tx_pwr_limit_data[channel_count].hi_tx_imbalanced_carrier_pwr_lut[count].y_axis_val ,
               p_tx_pwr_limit_data[channel_count].carrier_config_pattern ) ;  


            /* Sequence is important. Calculate monotonicity first and then check for
               lo_lut_y[8] and hi_lut_y[8]*/
            p_tx_pwr_limit_data[channel_count].table_monotonicity =
               check_nv_monotonocity ( &(p_tx_pwr_limit_data[channel_count]) ) ;  

            p_tx_pwr_limit_data[channel_count].
              is_max_eq_pwr_greater_than_max_neq_value = TRUE ;
              /* check_if_max_eq_pwr_greater_than_max_neq_value ( 
                &(p_tx_pwr_limit_data[channel_count])) ; */

            rf_hdr_print_max_power_values ( 
             &(p_tx_pwr_limit_data[channel_count]) ) ;
          } /*  for ( channel_count = 0  */
        } /* if ( NULL != p_tx_pwr_limit_data ) */
      } /*  else ( nv_status != NV_NOTACTIVE_S ) */

      /* Free Data if alloc had passed*/
      if ( NULL != hdr_nv_tx_pwr_lim_uncompressed_copy)
      {
        modem_mem_free(hdr_nv_tx_pwr_lim_uncompressed_copy, MODEM_MEM_CLIENT_RFA) ;
      }

    } /* if ( ( nv_status == NV_DONE_S ) || ( nv_status == NV_NOTACTIVE_S ) )  */
    else
    {
      RF_MSG( RF_ERROR,
              "rf_cdma_nv_load_hdr_pwr_limits: "
              "NV calibration state is inconsistent" );
      retval = FALSE;
    }
  
    modem_mem_free( hdr_tx_pwr_lim_compressed_buffer, MODEM_MEM_CLIENT_RFA ) ;
  } /* if ( hdr_tx_pwr_lim_compressed_buffer == NULL ) */

  return retval;
} /* rf_cdma_nv_load_hdr_pwr_limits() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx GTR Threshold NV for CDMA bands

  @details
  Loads the Tx GTR Threshold indicator NV item for the requested band item.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param tx_gtr_thresh_item
  The NV item that contains the Tx GTR Thresh values.

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
  TRUE if the Tx Thresh NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_gtr_thresh
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type tx_gtr_thresh_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     nv_status;
  nv_item_type          nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                              sizeof(nv_item_type);
  rfnv_tx_gtr_thresh_type *tx_thresh_ptr;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  nv_status =
    rfcommon_rfnv_get_item( tx_gtr_thresh_item,
                            (rfnv_item_type*)&nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    tx_thresh_ptr = (rfnv_tx_gtr_thresh_type *)&nv_buffer;

    /* Load the Tx static data for threshold indicator NV if the load
    was successful */
    tx_static_struct->tx_gtr_thresh_data.active = tx_thresh_ptr->active;
    tx_static_struct->tx_gtr_thresh_data.fall_value = 
      tx_thresh_ptr->fall_value;
    tx_static_struct->tx_gtr_thresh_data.rise_value =
      tx_thresh_ptr->rise_value;

  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* The BC NV item for Tx thresh indicator was not loaded successfully.
    Load defaults */
    tx_static_struct->tx_gtr_thresh_data.active = FALSE;
    tx_static_struct->tx_gtr_thresh_data.fall_value = 0;
    tx_static_struct->tx_gtr_thresh_data.rise_value = 0;

    RF_MSG( RF_MED,
            "rf_cdma_nv_load_tx_gtr_thresh: Uninitialized Tx Thresh NV" );
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    tx_static_struct->tx_gtr_thresh_data.active = FALSE;
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_gtr_thresh: NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( enc_btf_nv_status == NV_DONE_S ) */

  return load_success;
} /* rf_cdma_nv_load_tx_gtr_thresh() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the DPD Config NV for CDMA bands

  @details
  Loads the DPD configuration NV item for the requested band item.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param dpd_cfg_item
  The NV item that contains the DPD configuration values.

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
  TRUE if the DPD config items were loaded successfully or is inactive, 
  FALSE otherwise.
*/

static
boolean
rf_cdma_nv_load_dpd_cfg
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type dpd_cfg_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     dpd_cfg_status;
  rfnv_item_type        *dpd_cfg_buffer;
  rfnv_item_size_type   dpd_cfg_nv_returned_size =
                              sizeof(nv_item_type);
  rfnv_dpd_cfg_type     *dpd_cfg_ptr;
  int i;
  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  dpd_cfg_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );

  if ( NULL != dpd_cfg_buffer )
  {
    dpd_cfg_status =
      rfcommon_rfnv_get_item( dpd_cfg_item,
                              (rfnv_item_type*)dpd_cfg_buffer,
                              &dpd_cfg_nv_returned_size,
                              caller_tcb_ptr,
                              task_nv_wait_sig,
                              task_wait_func_ptr );

    if ( dpd_cfg_status == NV_DONE_S )
    {
      dpd_cfg_ptr = (rfnv_dpd_cfg_type *)dpd_cfg_buffer;

      /* Load the Tx static data for DPD config params NV if the load
      was successful */
      for( i=0;i<MAX_EPT_NUM_CONFIG_PARAMS;i++ )
      {
        tx_static_struct->ept_cfg_params[i] = dpd_cfg_ptr->dpd_cfg[i];
      }

    } /* if ( nv_status == NV_DONE_S ) */
    else if ( dpd_cfg_status == NV_NOTACTIVE_S )
    {
      /* The BC NV item for DPD config items was not loaded successfully.
      Load defaults */
      for( i=0;i<MAX_EPT_NUM_CONFIG_PARAMS;i++ )
      {
        tx_static_struct->ept_cfg_params[i] = 0;
      }

      RF_MSG( RF_MED,
              "rf_cdma_nv_load_dpd_cfg: Uninitialized Tx Thresh NV" );
    }
    else
    {
      /* The NV state is not consistent. Return FALSE to indicate failure */
      for( i=0;i<MAX_EPT_NUM_CONFIG_PARAMS;i++ )
      {
        tx_static_struct->ept_cfg_params[i] = 0;
      }
      RF_MSG( RF_ERROR,
              "rf_cdma_nv_load_dpd_cfg: NV state inconsistent" );
      load_success = FALSE;
    } /* else: if ( enc_btf_nv_status == NV_DONE_S ) */

    modem_mem_free( dpd_cfg_buffer, MODEM_MEM_CLIENT_RFA );

  } /* if ( NULL != dpd_cfg_buffer ) */
  else
  {  
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dpd_cfg: Failed mem_alloc" );
    load_success = FALSE;
  }

  return load_success;
} /* rf_cdma_nv_load_dpd_cfg() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load HDET vs Temperature NV data

  @details
  This function loads the HDET vs Temperature characterized response, 
  available in static NV.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param hdet_vs_temp_item
  The NV item that contains HDET vs Temp values

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_hdet_vs_temp
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type hdet_vs_temp_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     nv_status;
  rfnv_item_type        *nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                              sizeof(nv_item_type);
  rfnv_hdet_vs_temp_type *hdet_vs_temp_ptr;

  /* Iterator */
  int i;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );

  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_hdet_vs_temp: Could not "
                      "allocate buffer memory on heap" );
    load_success = FALSE;
    return load_success;
  }

  nv_status =
    rfcommon_rfnv_get_item( hdet_vs_temp_item,
                            nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* Load the Tx static data for HDET vs Temp NV if the load
    was successful */
    hdet_vs_temp_ptr = (rfnv_hdet_vs_temp_type *)nv_buffer;
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      tx_static_struct->hdet_vs_temp[i] = hdet_vs_temp_ptr->hdet_vs_temp[i];
      RF_MSG_1( RF_HIGH,
            "rf_cdma_nv_load_tx_hdet_vs_temp: HDET vs Temp val in NV %d",
             tx_static_struct->hdet_vs_temp[i]  );
    }
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* Initialize the temp comp data to zero */
    memset( &(tx_static_struct->hdet_vs_temp), 0,
            sizeof(int8)*RF_CDMA_NV_TEMP_LIMIT_SIZE );

    RF_MSG( RF_MED,
            "rf_cdma_nv_load_tx_hdet_vs_temp: Uninitialized HDET vs Temp NV" );
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_hdet_vs_temp: NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( nv_status == NV_DONE_S ) */

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return load_success;
} /* rf_cdma_nv_load_tx_hdet_vs_temp() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Lim vs Temp vs Freq NV data

  @details
  Loads the Lim vs Temp vs Freq static NV item for the requested band.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param nv_item
  The NV item that contains Power limiting vs temp vs freq data

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_lim_vs_temp_vs_freq
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     nv_status;
  rfnv_item_type        *nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                              sizeof(nv_item_type);
  rfnv_lim_vs_temp_vs_freq_type *lim_vs_temp_vs_freq_ptr;

  /* Iterator */
  int i, j;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );

  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lim_vs_temp_vs_freq: Could not "
                      "allocate buffer memory on heap" );
    load_success = FALSE;
    return load_success;
  }

  nv_status =
    rfcommon_rfnv_get_item( nv_item,
                            nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* Load the Tx static data for Lim Vs Temp Vs Freq NV if the load
    was successful */
    lim_vs_temp_vs_freq_ptr = (rfnv_lim_vs_temp_vs_freq_type *)nv_buffer;
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_FREQ_LIST_SIZE; j++ )
      {
        tx_static_struct->lim_vs_temp_vs_freq[i][j] = 
          lim_vs_temp_vs_freq_ptr->lim_vs_temp_vs_freq[i][j];
      }
    }
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* Initialize the lim_vs_temp_vs_freq data from the legacy lim_vs_temp
    data */
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_FREQ_LIST_SIZE; j++ )
      {
        tx_static_struct->lim_vs_temp_vs_freq[i][j] = 
          tx_static_struct->lim_vs_temp[0][i];;
      }
    }

    RF_MSG( RF_MED,
            "rf_cdma_nv_load_tx_lim_vs_temp_vs_freq: "
            "Uninitialized NV. Loading Lim vs Temp as default" );
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lim_vs_temp_vs_freq: NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( nv_status == NV_DONE_S ) */

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return load_success;

} /* rf_cdma_nv_load_tx_lim_vs_temp_vs_freq() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load static NV for Tx Limiting

  @details
  Loads the Lim vs Temp, Lim_vs_Temp SAR levels, and the lim_vs_temp_vs_freq 
  items

  @param tx_static_struct
  The Tx static NV data structure

  @param band
  The band number for this NV item.

  @param lim_vs_temp_sar_0_item
  Lim vs Temp static NV item

  @param lim_vs_temp_sar_1_8_item
  Lim vs Temp SAR static NV item

  @param lim_vs_temp_vs_freq_nv_item
  Lim vs Temp vs Freq static NV item

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
  TRUE if the lim_vs_temp_vs_freq and the SAR data are initialized 
  successfully, else FALSE.
*/
static
boolean
rf_cdma_nv_load_tx_lim_static
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfm_cdma_band_class_type band,

  const rfnv_item_id_enum_type lim_vs_temp_sar_0_item,
  const rfnv_item_id_enum_type lim_vs_temp_sar_1_8_item,
  const rfnv_item_id_enum_type lim_vs_temp_vs_freq_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  if ( retval == TRUE )
  {
    /* Load Lim vs Temp items */
    retval = rf_cdma_nv_load_tx_lim_temp( tx_static_struct,
                                          band,
                                          lim_vs_temp_sar_0_item,
                                          lim_vs_temp_sar_1_8_item,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* Load Lim vs Temp vs Freq item */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_tx_lim_vs_temp_vs_freq( 
                                          tx_static_struct,
                                          lim_vs_temp_vs_freq_nv_item,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_tx_lim_static() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the static HDET Mod Range NV data

  @details
  Loads the static NV data for HDET mod range, which bounds the amount of 
  HDET correction applied to the linearizer.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param nv_item
  The NV item that contains the HDET Mod Range values

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_hdet_mod_range
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     nv_status;
  rfnv_item_type        *nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                              sizeof(nv_item_type);
  rfnv_hdet_mod_range_type *data_ptr;

  /* Iterator */
  int i, j;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );

  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_hdet_mod_range: Could not "
                      "allocate buffer memory on heap" );
    load_success = FALSE;
    return load_success;
  }

  nv_status =
    rfcommon_rfnv_get_item( nv_item,
                            nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* Load the Tx static data for HDET Mod Range NV if the load
    was successful */
    data_ptr = (rfnv_hdet_mod_range_type *)nv_buffer;
    for ( i = 0; i < RF_CDMA_NV_HDET_MOD_RANGE_NUM; i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_TEMP_LIMIT_SIZE; j++ )
      {
        tx_static_struct->hdet_mod_range[i][j] = 
          data_ptr->hdet_mod_range[i][j];
      }
    }
  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    for ( j = 0; j < RF_CDMA_NV_TEMP_LIMIT_SIZE; j++ )
    {
      tx_static_struct->hdet_mod_range[RF_CDMA_NV_HDET_MOD_RANGE_LO][j] = -50;
      tx_static_struct->hdet_mod_range[RF_CDMA_NV_HDET_MOD_RANGE_HI][j] = 50;
    }

    RF_MSG( RF_MED,
            "rf_cdma_nv_load_tx_hdet_mod_range: "
            "Uninitialized NV" );
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_hdet_mod_range: NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( nv_status == NV_DONE_S ) */

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return load_success;

} /* rf_cdma_nv_load_tx_hdet_mod_range() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx max power HDET vs freq data from NV

  @details
  Loads the HDET vs freq calibrated NV item. This item is meant to deprecate 
  Lim vs Freq NV.

  @param lim_nv_struct
  The Tx NV data structure that HDET calibration information must be loaded
  into

  @param hdet_vs_freq_item
  The NV item that contains Tx HDET vs Freq data.

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
  TRUE if the HDET vs Freq data is loaded, FALSE if there was a problem
  identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_hdet_vs_freq
(
  rf_cdma_tx_lim_cal_type* const lim_nv_struct,
  const rfnv_item_id_enum_type hdet_vs_freq_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     hdet_vs_freq_nv_status;
  rfnv_item_type        *hdet_vs_freq_buffer;
  rfnv_item_size_type   hdet_vs_freq_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_tx_pwr_freq_comp_type* hdet_vs_freq_data;

  /* Data structure iterator */
  int i;

  hdet_vs_freq_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                        MODEM_MEM_CLIENT_RFA );
  if ( hdet_vs_freq_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_hdet_vs_freq: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  hdet_vs_freq_nv_status =
    rfcommon_rfnv_get_item( hdet_vs_freq_item,
                            hdet_vs_freq_buffer,
                            &hdet_vs_freq_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( hdet_vs_freq_nv_status == NV_DONE_S )
  {
    /* Load the max power vs frequency compensation data across
    the list of calibration channels */
    hdet_vs_freq_data = (rfnv_tx_pwr_freq_comp_type*)hdet_vs_freq_buffer;

    for ( i = 0; i < RF_CDMA_NV_FREQ_LIST_SIZE; i++ )
    {
      lim_nv_struct->hdet_vs_freq[i] = 
        (int8)(hdet_vs_freq_data->freq_comp[i]);
    }

    retval = TRUE;
  }
  else if ( hdet_vs_freq_nv_status == NV_NOTACTIVE_S )
  {
    /* Initialize the freq comp data to zero and return FALSE, so that
    we can fall back to legacy lim freq items if possible */
    memset( &(lim_nv_struct->hdet_vs_freq), 0,
            sizeof(int8)*RF_CDMA_NV_FREQ_LIST_SIZE );

    retval = FALSE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_hdet_vs_freq: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  modem_mem_free( hdet_vs_freq_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_tx_hdet_vs_freq() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx ET path delay data.

  @param tx_cal_struct
  The data structure that NV data must be loaded into.
 
  @param et_path_delay_nv_id
  The NV item that contains the Tx ET path delay data.
 
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
  TRUE if the Therm Bins data is successfully initialized,
  FALSE if there was a problem identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_et_path_delay
(
  rf_cdma_tx_cal_type* const tx_cal_struct,

  const rfnv_item_id_enum_type et_path_delay_nv_id,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *et_path_delay_buffer;
  rfnv_item_size_type   et_path_delay_returned_size =
                                  sizeof(rfnv_item_type);

  et_path_delay_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                       MODEM_MEM_CLIENT_RFA );
  if ( et_path_delay_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_et_path_delay: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  nv_status =
    rfcommon_rfnv_get_item( et_path_delay_nv_id,
                            et_path_delay_buffer,
                            &et_path_delay_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    int32 et_delay = *((int32*)et_path_delay_buffer) ;

    if ( ( et_delay > MIN_VALID_ET_PATH_DELAY_IN_NS ) &&
         ( et_delay < MAX_VALID_ET_PATH_DELAY_IN_NS ) )
    {
      /* If there is a newer version of this NV item, say delay vs freq, 
        this entry will be overwritten by that item while NV loading.
        If a newer NV is absent, we can use this NV using the same algorithm 
        for calculating the NV delay.  */
      tx_cal_struct->et_path_delay_info.number_of_valid_entries = 1 ;
      tx_cal_struct->et_path_delay_info.et_path_delay_vs_freq[0] = et_delay ;
    }
    else
    {
      RF_MSG_3 ( RF_ERROR , "rf_cdma_nv_load_tx_et_path_delay() " 
                "The NV is not within the limits. Limits in nSecs " 
                "|NV: %d |Min Limit: %d |Max Limit: %d |" ,
                tx_cal_struct->et_path_delay_info.et_path_delay_vs_freq[0] ,
                MIN_VALID_ET_PATH_DELAY_IN_NS , MAX_VALID_ET_PATH_DELAY_IN_NS ) ;
      RF_MSG_1 ( RF_ERROR , "rf_cdma_nv_load_tx_et_path_delay() " 
                 "NV: %d discarded" , et_path_delay_nv_id ) ;
      tx_cal_struct->et_path_delay_info.number_of_valid_entries = 0 ;
      tx_cal_struct->et_path_delay_info.et_path_delay_vs_freq[0] = 0 ;
    }

    /* Even if we have garbage in NV, we return successfully because 
      The newer version of this NV may have the correct values */
    retval = TRUE;
  }
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* If this NV cannot be loaded and other newer versioned NVs also fail,
    we have no valid ET delay entries so we will have to revert to default value */
    memset( &(tx_cal_struct->et_path_delay_info), 0, sizeof(rf_cmda_et_nv_items_type) );
    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_et_path_delay: "
            "NV calibration state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( et_path_delay_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_tx_et_path_delay() */

static
boolean
rf_cdma_nv_load_tx_pin_cal_vs_freq
(
  rfcommon_core_txlin_pin_cal_vs_freq_type* const pin_cal_struct,
  const rfnv_item_id_enum_type pin_cal_vs_freq_id,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *pin_vs_freq_buffer;
  rfnv_item_size_type   pin_vs_freq_nv_size = sizeof(rfnv_item_type);

  if ( pin_cal_struct == NULL) 
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_pin_cal_vs_freq: "
                      "NULL CDMA NV struct " );
    retval = FALSE;
    return retval;
  }

  /* Memset struct to make sure all flags are initialized to FALSE */
  memset(pin_cal_struct, 0, sizeof(rfcommon_core_txlin_pin_cal_vs_freq_type)); 
  
  pin_vs_freq_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                        MODEM_MEM_CLIENT_RFA );
  if ( pin_vs_freq_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_pin_cal_vs_freq: Could not "
                      "allocate memory for pin_cal_vs_freq load buffer" );
    retval = FALSE;
    return retval;
  }

  nv_status = rfcommon_rfnv_get_item( pin_cal_vs_freq_id,
                                      pin_vs_freq_buffer,
                                      &pin_vs_freq_nv_size,
                                      caller_tcb_ptr,
                                      task_nv_wait_sig,
                                      task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    rfnv_cdma_pin_cal_vs_freq_type* pin_cal_nv = 
                          (rfnv_cdma_pin_cal_vs_freq_type*)pin_vs_freq_buffer;

    /* Loop through all PA states and channels to load pin cal value. */
    if ( pin_cal_nv != NULL )
    {
      uint8 pa_state_idx;

      for ( pa_state_idx = 0; pa_state_idx < RFCOM_TXLIN_NUM_PA_STATES;
            pa_state_idx ++  )
      {
        uint8 channel_idx;

        for ( channel_idx = 0; channel_idx < RFCOM_TXLIN_NUM_CHANNELS;
              channel_idx ++ )
        {
          int16 pin_cal_value;

          pin_cal_value = pin_cal_nv->pin_cal_vs_freq[pa_state_idx][channel_idx];

          /* If for a given PA state, if any channel has a invalide pin_cal 
          value, mark that PA state as invalid for pin_cal_vs_freq and skips
          the rest of channels. If a channel has good pin_cal value, mark the
          pa_state as valid and continue */
          if( pin_cal_value == RFNV_CDMA_INVALID_PIN_CAL_VS_FREQ )
          {
            pin_cal_struct->has_pin_freq_comp_cal_nv[pa_state_idx] = FALSE;            
            break;
          }
          else
          {
            pin_cal_struct->pin[pa_state_idx][channel_idx] = pin_cal_value;
            pin_cal_struct->has_pin_freq_comp_cal_nv[pa_state_idx] = TRUE;            
          }

        }/* for ( channel_idx = 0 */
      }/*  for ( pa_state_idx = 0 */

    }/* if ( pin_cal_nv != NULL ) */
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_pin_cal_vs_freq: Could not "
                        "NULL pointer returned from core NV loading API" );
      retval = FALSE;
    }

  }
  else if( nv_status == NV_NOTACTIVE_S )
  {
    RF_MSG( RF_HIGH,
            "rf_cdma_nv_load_tx_pin_cal_vs_freq: "
            "pin_cal_vs_freq NV inactive" );
    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_pin_cal_vs_freq: "
            "Error reading pin_cal_vs_freq NV from file system" );
    retval = FALSE;
  }

  if( retval == FALSE )
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_pin_cal_vs_freq: "
            "Error loading pin_cal_vs_freq NV" );
  }

  modem_mem_free( pin_vs_freq_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load CDMA Antenna Tuner CL Control NV data

  @param tx_static_struct
  The data structure that NV data must be loaded into.
 
  @param ant_tuner_cl_item
  The NV item that contains the ant tuner cl control NV
 
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
  TRUE if the Therm Bins data is successfully initialized,
  FALSE if there was a problem identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_ant_tuner_cl
(
  rf_cdma_tx_static_type* const tx_static_struct,

  const rfnv_item_id_enum_type ant_tuner_cl_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *ant_tuner_cl_buffer;
  rfnv_item_size_type   ant_tuner_cl_returned_size =
                                  sizeof(rfnv_item_type);

  ant_tuner_cl_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                        MODEM_MEM_CLIENT_RFA );
  if ( ant_tuner_cl_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_ant_tuner_cl: Could not "
                      "allocate buffer memory on heap for NV load buffer" );
    retval = FALSE;
    return retval;
  }

  nv_status = rfcommon_rfnv_get_item( ant_tuner_cl_item,
                                      ant_tuner_cl_buffer,
                                      &ant_tuner_cl_returned_size,
                                      caller_tcb_ptr,
                                      task_nv_wait_sig,
                                      task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    retval = rfcommon_load_ant_tuner_cl_ctrl_info_from_nv( ant_tuner_cl_buffer, 
                                               &(tx_static_struct->ant_tuner_cl) );
  }
  else if( nv_status == NV_NOTACTIVE_S )
  {
    RF_MSG( RF_HIGH,
            "rf_cdma_nv_load_ant_tuner_cl: "
            "Antenna Tuner CL Control NV inactive" );
    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_ant_tuner_cl: "
            "Error reading Antenna Tuner CL Control NV from file system" );
    retval = FALSE;
  }

  if( retval == FALSE )
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_ant_tuner_cl: "
            "Error loading Antenna Tuner CL Control NV" );
  }

  modem_mem_free( ant_tuner_cl_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;

}/* rf_cdma_nv_load_ant_tuner_cl */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Predictive clipping enable/disable threshold
  for any band.

  @details
  Loads the Predictive clipping enable threshold from NV.
 
  If NV is not active, the enable/disable threshold is programmed as default
  based on systems recommendations -- 21.5dBm for enable. 19.5 dBm for disable.

  @param tx_static_struct
  Pointer to Static NV data structure

  @param en_thresh_item
  The NV item that contains the PC enable/disable threshold value.

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static boolean
rf_cdma_nv_load_pred_clip_en_thresh
(
  rf_cdma_tx_static_type* const tx_static_struct,

  const rfnv_item_id_enum_type en_thresh_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffer for intelliceiver cal data */
  nv_stat_enum_type          en_thresh_nv_status;
  rfnv_item_type             *en_thresh_buffer;
  rfnv_item_size_type        en_thresh_returned_size =
                                     sizeof(rfnv_item_type);
  rfnv_pred_clip_type*     en_thresh_nv_ptr;

  en_thresh_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                      MODEM_MEM_CLIENT_RFA );

  if ( en_thresh_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_pred_clip_en_thresh: Memory Alloc "
                      "failed" );
    return FALSE;
  }

  en_thresh_nv_status = 
    rfcommon_rfnv_get_item( en_thresh_item,
                            en_thresh_buffer,
                            &en_thresh_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( en_thresh_nv_status == NV_DONE_S )
  {
    /* Load the NV data into the pre-defined structure */
    en_thresh_nv_ptr = (rfnv_pred_clip_type *)en_thresh_buffer;

    tx_static_struct->pred_clip_nv.enable_threshold = 
                                             en_thresh_nv_ptr->enable_threshold;
    tx_static_struct->pred_clip_nv.disable_threshold = 
                                            en_thresh_nv_ptr->disable_threshold;

    retval = TRUE;
  } /* if ( ic_cal_nv_status == NV_DONE_S ) */
  else if ( en_thresh_nv_status == NV_NOTACTIVE_S )
  {
    RF_MSG( RF_HIGH,
            "rf_cdma_nv_load_pred_clip_en_thresh: NV inactive, using default" );

    /* Default values are provided from systems */
    tx_static_struct->pred_clip_nv.enable_threshold = 215;
    tx_static_struct->pred_clip_nv.disable_threshold = 195;

    retval = TRUE;
  } /* else if ( ic_cal_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_pred_clip_en_thresh: "
            "NV calibration state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( en_thresh_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;

} /* rf_cdma_nv_load_pred_clip_en_thresh() */

/*----------------------------------------------------------------------------*/
/*!
  @brief Helper function to check if delays values provided are
  within limits or not.
*/
boolean
rf_cdma_check_et_delay_validity 
(
   int32    arr[] ,
   size_t   arr_size ,
   int32    lower_valid_limit ,
   int32    upper_valid_limit        
)
{
  size_t count ; 
  boolean are_given_values_within_range = TRUE ;

  for ( count = 0 ; count < arr_size ; count++ )
  { /* Ascending Order */
    if ( ( arr[count] > upper_valid_limit ) || 
         ( arr[count] < lower_valid_limit ) )
    { /* If less than lower limit, fail and quit */

      are_given_values_within_range = FALSE ;
      RF_MSG_1( RF_ERROR, "rf_cdma_check_et_delay_validity - ET delay %d validity check failed",
                arr[count] );
      break ;
    }
  }
  return are_given_values_within_range ;
} /* rf_cdma_check_et_delay_validity */

/*----------------------------------------------------------------------------*/
/*!
  @brief Helper function to check if ET delays NV Bins are
  ordered in ascending frequency or not.
  If the bins are in ascending order of frequency TRUE is
  returned; FALSE otherwise.
*/
boolean
rf_cdma_check_et_channel_bin_validity
(
   uint32     freq_sorted_array[] ,
   size_t     arr_size ,
   const      rfm_cdma_band_class_type band 
)
{
  boolean success = TRUE ;
  /* First channel has to be valid. Rest of the array can have the same channel */
  if ( freq_sorted_array[0] != 0 )
  {
    size_t i = 1 ;
    /* Loop until the end of the channel list */
    for ( i = 1; i < arr_size && freq_sorted_array[i] != 0; i++ )
    {
      /* Insure that freq(x[i-1]) < freq(x[i]) */
      if ( rf_cdma_get_tx_carrier_freq( band, freq_sorted_array[i-1])
           >= rf_cdma_get_tx_carrier_freq( band, freq_sorted_array[i]) )
      {
        RF_MSG_1( RF_ERROR, "rf_cdma_check_et_channel_bin_validity: Non-monotonic "
                            "Tx characterization frequency list for Band %d",
                  band );
          success = FALSE ;
          break ;
      }
    }
  } /* if ( freq_sorted_array[0] != 0 ) */
  return success ;
} /* rf_cdma_check_et_channel_bin_validity */

/*----------------------------------------------------------------------------*/
/*!
  @brief Helper function to check if ET delays NV is usable or
  not.
*/
boolean 
rf_cdma_check_et_nv_validity 
(
   rf_cmda_et_nv_items_type* p_et_info_from_nv ,
   const rfm_cdma_band_class_type band 
)
{
  boolean success = TRUE;

  success &= ( ( p_et_info_from_nv->number_of_valid_entries > 0 ) && 
               ( p_et_info_from_nv->number_of_valid_entries <= MAX_FREQ_COMP_ET_PATH_DELAY_BINS ) );

  if(success == FALSE)
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_check_et_nv_validity - num valid entries %d is invalid",
              p_et_info_from_nv->number_of_valid_entries );
  }

  if ( TRUE == success )
  {
    success &= rf_cdma_check_et_delay_validity (
                 &(p_et_info_from_nv->et_path_delay_vs_freq[0] ) ,
                 p_et_info_from_nv->number_of_valid_entries ,
                 MIN_VALID_ET_PATH_DELAY_IN_NS , MAX_VALID_ET_PATH_DELAY_IN_NS ) ;
  }

  if ( TRUE == success )
  {
    success &= 
    rf_cdma_check_et_channel_bin_validity (&(p_et_info_from_nv->upper_bound_channel[0]) , 
                                           MAX_FREQ_COMP_ET_PATH_DELAY_BINS , band ) ;
    if ( TRUE == success )
    {  /* Copy the frequencies now that we that they are SO we dont have to 
      convert the channels to frequencies every look up. */
      size_t count ;
      for ( count = 0 ; count < MAX_FREQ_COMP_ET_PATH_DELAY_BINS ; count++ )
      {
        p_et_info_from_nv->upper_bound_freq_KHz[count] = 
          rf_cdma_get_tx_carrier_freq ( band, 
                                      p_et_info_from_nv->upper_bound_channel[count] ) ;
      }
    }
  }

  return success ;
} /* rf_cdma_check_et_nv_validity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx ET path delay data wrt frequency.

  @param tx_cal_struct
  The data structure that NV data must be loaded into.
 
  @param et_path_delay_nv_id
  The NV item that contains the Tx ET path delay data.
 
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
  TRUE if the ET delay Bin-data is successfully initialized,
  FALSE if there was a problem identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_et_path_delay_vs_freq_items
(
  rf_cdma_tx_cal_type* const tx_cal_struct,
  const rfnv_item_id_enum_type et_path_delay_nv_id,
  const rfm_cdma_band_class_type band ,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = FALSE ;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *et_path_delay_buffer;
  rfnv_item_size_type   et_path_delay_returned_size =
                                  sizeof(rfnv_item_type);

  /* Target : Internal CDMA Cal structure where data will be copied */
  rf_cmda_et_nv_items_type* p_et_info_from_nv = 
                    &(tx_cal_struct->et_path_delay_info) ;

  et_path_delay_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                       MODEM_MEM_CLIENT_RFA );
  if ( et_path_delay_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_et_path_delay_freq_comp_items: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
  }
  else
  {
    nv_status =
      rfcommon_rfnv_get_item( et_path_delay_nv_id,
                              et_path_delay_buffer,
                              &et_path_delay_returned_size,
                              caller_tcb_ptr,
                              task_nv_wait_sig,
                              task_wait_func_ptr ) ;

    if ( nv_status == NV_DONE_S )
    {
      /* Store items */
      size_t count = 0 ;
      /* Source :  Read from the NV */
      const rfnv_tx_et_path_delay_data_type* et_nv_data =
                    (rfnv_tx_et_path_delay_data_type*)(et_path_delay_buffer) ;

      p_et_info_from_nv->number_of_valid_entries = (et_nv_data->number_of_valid_entries & 0x7F);
      p_et_info_from_nv->is_delay_cald = ((et_nv_data->number_of_valid_entries & 0x80) != 0x0);
      for ( count = 0; count < MAX_FREQ_COMP_ET_PATH_DELAY_BINS ; count++ )
      {
        p_et_info_from_nv->et_path_delay_vs_freq[count] = et_nv_data->et_path_delay_vs_freq[count] ;
        p_et_info_from_nv->upper_bound_channel[count] = et_nv_data->upper_bound_channel[count] ;
      }

      /* Validate and if validation fails fail the NV loading process */
      retval = 
        rf_cdma_check_et_nv_validity ( p_et_info_from_nv , band ) ;
    }
    else if ( nv_status == NV_NOTACTIVE_S )
    {
      retval = TRUE ;
    }
    else
    {
      RF_MSG( RF_ERROR,
              "rf_cdma_nv_load_tx_et_path_delay_freq_comp_items: "
              "NV calibration state is inconsistent" );
      retval = FALSE;
    }
    modem_mem_free( et_path_delay_buffer, MODEM_MEM_CLIENT_RFA );
  }
  return retval;
} /* rf_cdma_nv_load_tx_et_path_delay_freq_comp_items() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx ET path delay offsets vs freq vs temp data.

  @param tx_cal_struct
  The data structure that NV data must be loaded into.
 
  @param rfnv_item_id
  The NV item that contains the Tx ET path delay offsets vs freq_vs temp data.
 
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
  TRUE if successful
  FALSE otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_et_path_delay_offsets_vs_freq_vs_temp_items
(
  rf_cdma_tx_cal_type* const tx_cal_struct,
  const rfnv_item_id_enum_type rfnv_item_id,
  const rfm_cdma_band_class_type band ,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  nv_stat_enum_type     nv_status;
  rfnv_item_type        *rfnv_item_data;
  rfnv_item_size_type   rfnv_item_returned_size = sizeof(rfnv_item_type);

  /* Target : Internal CDMA Cal structure where data will be copied */
  rf_cmda_et_nv_items_type* p_et_info_from_nv = 
                    &(tx_cal_struct->et_path_delay_info);

  /* Initialize to zeroes in case actual read fails */
  memset( &p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp, 0x0, 
          sizeof(p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp) );

  rfnv_item_data = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );
  if ( rfnv_item_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_et_path_delay_offsets_vs_freq_vs_temp_items: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
  }
  else
  {
    nv_status =
      rfcommon_rfnv_get_item( rfnv_item_id,
                              rfnv_item_data,
                              &rfnv_item_returned_size,
                              caller_tcb_ptr,
                              task_nv_wait_sig,
                              task_wait_func_ptr );

    if ( nv_status == NV_DONE_S )
    {
      /* Store items */
      uint8 i,j;
      /* Source :  Read from the NV */
      const rfnv_tx_delay_offsets_type* et_nv_data =
                    (rfnv_tx_delay_offsets_type*)(rfnv_item_data);

      p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp.channels_enabled = 
        et_nv_data->channels_enabled;

      for ( i = 0; i < RFCOM_TXLIN_NUM_CHANNELS; i++ )
      {
        p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp.tx_upper_bound_chan_list[i] = 
          et_nv_data->tx_upper_bound_chan_list[i];
        
        for ( j = 0; j < NV_TEMP_TABLE_SIZ; j++ )
        {
          p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp.delay_offset_vs_temp_vs_freq[j][i] = 
            et_nv_data->delay_offset_vs_temp_vs_freq[j][i];
        }
      }
    }
    else if ( nv_status == NV_NOTACTIVE_S )
    {
      /* If the NV item is inactive, load default of zero */
      memset( &(p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp), 0x0, 
              sizeof(p_et_info_from_nv->et_path_delay_offsets_vs_freq_vs_temp) );
    }
    else
    {
      RF_MSG( RF_ERROR,
              "rf_cdma_nv_load_tx_et_path_delay_offsets_vs_freq_vs_temp_items: "
              "NV calibration state is inconsistent" );
      retval = FALSE;
    }
    modem_mem_free( rfnv_item_data, MODEM_MEM_CLIENT_RFA );
  }

  return retval;
} /* rf_cdma_nv_load_tx_et_path_delay_offsets_vs_freq_vs_temp_items() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Lin vs Temp vs Freq NV data

  @details
  Loads the Lin vs Temp vs Freq Pin or Pout static NV item for the requested
  band.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param nv_item
  The NV item that contains Linearizer vs temp vs freq data

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pout
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     nv_status;
  rfnv_item_type        *nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                              sizeof(rfnv_item_type);
  rfnv_lin_vs_temp_vs_freq_type *lin_vs_temp_vs_freq_ptr;

  /* Iterator */
  int i, j, k;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );

  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pout: Could not "
                      "allocate buffer memory on heap" );
    load_success = FALSE;
    return load_success;
  }

  nv_status =
    rfcommon_rfnv_get_item( nv_item,
                            nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* Load the Tx static data for Lin Vs Temp Vs Freq NV if the load
    was successful */
    lin_vs_temp_vs_freq_ptr = (rfnv_lin_vs_temp_vs_freq_type *)nv_buffer;

    /* Populate new temp comp data structures for common temp comp 
    implementation */
    tx_static_struct->lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
    {
      for ( j = 0; j < RFCOM_TXLIN_TEMP_BIN_SIZE; j++ )
      {
        for ( k = 0; k < RFCOM_TXLIN_NUM_CHANNELS; k++ )
        {
          tx_static_struct->
                    lin_temp_comp_data.lin_vs_temp_vs_freq.pout[i][j][k] =
            lin_vs_temp_vs_freq_ptr->lin_vs_temp_vs_freq[i][j][k];
        }
      }
    }

  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* Populate new temp comp data structures for common temp comp 
    implementation */
    tx_static_struct->lin_temp_comp_data.has_pin_pout_temp_comp_nv = FALSE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
    {
      for ( j = 0; j < RFCOM_TXLIN_TEMP_BIN_SIZE; j++ )
      {
        for ( k = 0; k < RFCOM_TXLIN_NUM_CHANNELS; k++ )
        {
          tx_static_struct->
                    lin_temp_comp_data.lin_vs_temp_vs_freq.pout[i][j][k] = 0;
        }
      }
    }

    RF_MSG( RF_MED,
            "rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pout: "
            "Uninitialized NV. Loading Lim vs Temp as default" );
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pout: "
            "NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( nv_status == NV_DONE_S ) */

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return load_success;

} /* rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pout() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Lin vs Temp vs Freq NV data

  @details
  Loads the Lin vs Temp vs Freq Pin or Pout static NV item for the requested
  band.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param nv_item
  The NV item that contains Linearizer vs temp vs freq data

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pin
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     nv_status;
  rfnv_item_type        *nv_buffer;
  rfnv_item_size_type   nv_returned_size =
                              sizeof(rfnv_item_type);
  rfnv_lin_vs_temp_vs_freq_type *lin_vs_temp_vs_freq_ptr;

  /* Iterator */
  int i, j, k;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                               MODEM_MEM_CLIENT_RFA );

  if ( nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pin: Could not "
                      "allocate buffer memory on heap" );
    load_success = FALSE;
    return load_success;
  }

  nv_status =
    rfcommon_rfnv_get_item( nv_item,
                            nv_buffer,
                            &nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( nv_status == NV_DONE_S )
  {
    /* Load the Tx static data for Lin Vs Temp Vs Freq NV if the load
    was successful */
    lin_vs_temp_vs_freq_ptr = (rfnv_lin_vs_temp_vs_freq_type *)nv_buffer;

    /* Populate new temp comp data structures for common temp comp 
    implementation */
    tx_static_struct->lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
    {
      for ( j = 0; j < RFCOM_TXLIN_TEMP_BIN_SIZE; j++ )
      {
        for ( k = 0; k < RFCOM_TXLIN_NUM_CHANNELS; k++ )
        {
          tx_static_struct->
                    lin_temp_comp_data.lin_vs_temp_vs_freq.pin[i][j][k] =
            lin_vs_temp_vs_freq_ptr->lin_vs_temp_vs_freq[i][j][k];
        }
      }
    }

  } /* if ( nv_status == NV_DONE_S ) */
  else if ( nv_status == NV_NOTACTIVE_S )
  {
    /* Populate new temp comp data structures for common temp comp 
    implementation */
    tx_static_struct->lin_temp_comp_data.has_pin_pout_temp_comp_nv = FALSE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++ )
    {
      for ( j = 0; j < RFCOM_TXLIN_TEMP_BIN_SIZE; j++ )
      {
        for ( k = 0; k < RFCOM_TXLIN_NUM_CHANNELS; k++ )
        {
          tx_static_struct->
                    lin_temp_comp_data.lin_vs_temp_vs_freq.pin[i][j][k] = 0;
        }
      }
    }

    RF_MSG( RF_MED,
            "rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pin: "
            "Uninitialized NV. Loading Lin vs Temp as default" );
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pin: "
            "NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( nv_status == NV_DONE_S ) */

  modem_mem_free( nv_buffer, MODEM_MEM_CLIENT_RFA );
  return load_success;

} /* rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pin() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Lin vs Temp vs Freq NV data

  @details
  Loads the Lin vs Temp vs Freq Pin or Pout static NV item for the requested
  band.

  @param tx_static_struct
  Pointer to the CDMA static NV data structure

  @param nv_item
  The NV item that contains Linearizer vs temp vs freq data

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
  TRUE if the NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
static
boolean
rf_cdma_nv_load_tx_lin_vs_temp_vs_freq
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type pout_nv_item,
  const rfnv_item_id_enum_type pin_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pout( 
                                          tx_static_struct,
                                          pout_nv_item,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_tx_lin_vs_temp_vs_freq_pin( 
                                          tx_static_struct,
                                          pin_nv_item,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_tx_lin_vs_temp_vs_freq() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx characterization channel list for any band.

  @details
  Checks that the NV item for this device's and band's char_list_buffer channel
  list is loaded, and if so, initalizes the channel_list and freq_list
  members of the static structure.

  Up to 16 channels may be loaded from NV into the configuration data
  structure.  The channel list is terminated by the first index that
  contains the value 0.  No further channels are loaded from NV once a
  zero is found, and the channel_list and freq_list data members are
  filled with 0s in the remaining entries.  The number of valid channel
  entries is stored in the freq_list_size member of the configuration
  data structure.

  The channel list must be stored in ascending frequency sorted order,
  otherwise the NV is considered invalid.

  @param static_struct
  The Tx static structure that the channel list data is to be
  loaded in to.

  @param tx_char_chan_nv_item
  The NV item that contains the list of Tx calibration channels.

  @param band
  Frequency computation needs to know what band the channels are in, so
  this is passed as a parameter.

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
  TRUE if the channel list data is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_tx_char_chan_list
(
  rf_cdma_tx_static_type* const tx_static_struct,
  const rfnv_item_id_enum_type tx_char_chan_nv_item,
  const rfm_cdma_band_class_type band,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type   char_list_nv_status;
  nv_item_type        char_list_buffer;
  rfnv_item_size_type char_list_returned_size =
                                sizeof(nv_item_type);

  /* Array iterator */
  int i;

  char_list_nv_status =
    rfcommon_rfnv_get_item( tx_char_chan_nv_item,
                            (rfnv_item_type*)&char_list_buffer,
                            &char_list_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( char_list_nv_status == NV_DONE_S )
  {
    /* Verify the NV item contains a valid sorted channel list */
    /* If the first item isn't 0, there is at least one channel in the list
       and the sort order needs to be checked. */
    if ( char_list_buffer.c0_bc0_tx_cal_chan[0] != 0 )
    {
      /* Loop until the end of the channel list */
      for ( i = 1;
            i < NV_FREQ_TABLE_SIZ && char_list_buffer.c0_bc0_tx_cal_chan[i] != 0;
            i++ )
      {
        /* Insure that freq(x[i-1]) < freq(x[i]) */
        if ( rf_cdma_get_tx_carrier_freq(band, 
                                     char_list_buffer.c0_bc0_tx_cal_chan[i-1])
             >= rf_cdma_get_tx_carrier_freq(band, 
                                     char_list_buffer.c0_bc0_tx_cal_chan[i]) )
        {
          RF_MSG_1( RF_ERROR, "rf_cdma_nv_load_tx_char_chan_list: Non-monotonic "
                              "Tx characterization frequency list for Band %d",
                  band );
          return FALSE;
        }
      }
    } /* if ( char_list_buffer.c0_bc0_tx_cal_chan[0] != 0 ) */

    /* If reach here, the channel list format is valid.  Load it into memory. */
    for ( i = 0;
          i < NV_FREQ_TABLE_SIZ && char_list_buffer.c0_bc0_tx_cal_chan[i] != 0;
          i++ )
    {
      tx_static_struct->lin_temp_comp_data.tx_char_chan[i] =
                                        char_list_buffer.c0_bc0_tx_cal_chan[i];
      tx_static_struct->lin_temp_comp_data.tx_char_freq[i] =
         rf_cdma_get_tx_carrier_freq(band, 
                                      char_list_buffer.c0_bc0_tx_cal_chan[i]);
    } /* for each valid channel in the channel list */

    /* Record how many valid values are in the frequency list. */
    tx_static_struct->lin_temp_comp_data.tx_char_chan_size = i;

    /* Fill out the rest of the cal structure with 0's */
    while ( i < NV_FREQ_TABLE_SIZ )
    {
      tx_static_struct->lin_temp_comp_data.tx_char_chan[i] = 65535;
      tx_static_struct->lin_temp_comp_data.tx_char_freq[i] = 0;
      i++;
    } /* while not reach the end of the array */
  } /* if ( chan_list_nv_status == NV_DONE_S ) */
  else
  {
    for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
    {
      tx_static_struct->lin_temp_comp_data.tx_char_chan[i] = 65535;
      tx_static_struct->lin_temp_comp_data.tx_char_freq[i] = 0;
    }
    tx_static_struct->lin_temp_comp_data.tx_char_chan_size = 0;
  } /* else: if ( char_list_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rf_cdma_nv_load_tx_char_chan_list() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx PA static NV items for any band

  @details
  Checks that the NV item for this device's and band's PA static NV data 
  is loaded, and if so, initalizes the following CDMA PA static NV items.
  - PA range map
  - Switchpoint data
  - Lin vs temp vs Freq static data

  @param static_struct
  The Tx static structure that the PA static data is to be
  loaded in to.

  @param tx_pa_static_nv_item
  The NV item that contains the list of Tx calibration channels.

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
  TRUE if the PA static data is loaded and is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_tx_pa_static
(
  rf_cdma_tx_static_type* const tx_static_struct,
  rf_1x_tx_static_type* tx_1x_ptr,
  rf_hdr_tx_static_type* tx_hdr_ptr,
  const rfnv_item_id_enum_type tx_pa_static_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean ret_val = TRUE;
  rfcommon_nv_pa_static_data_type *pa_static_data;
  rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;
  uint8 lin_index;
  uint8 freq_index;
  uint8 temp_index;

  /* Allocate memory into which PA static data will be loaded */
  pa_static_data = (rfcommon_nv_pa_static_data_type*)
                     modem_mem_alloc( sizeof(rfcommon_nv_pa_static_data_type),
                                      MODEM_MEM_CLIENT_RFA );
  
  if ( pa_static_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_pa_static: "
                      "Failed to allocate pa_static_data on heap" );
    return FALSE;
  }

  ret_val = rfcommon_load_pa_static_nv( RFCOM_1X_MODE, pa_static_data, 
                                        tx_pa_static_nv_item,
                                        caller_tcb_ptr, task_nv_wait_sig,
                                        task_wait_func_ptr );
  
  if ( ret_val == TRUE )
  {
    /* PA static NV version */
    tx_static_struct->pa_static_nv_version = 
                             pa_static_data->cmn_pa.header[PA_STATIC_VERSION];

    if ( tx_static_struct->pa_static_nv_version == RFCMN_NV_PA_STATIC_VER_1 )
    {
      /* Number of active PA states as defined by NV */
      tx_static_struct->num_active_pa_states =
                         pa_static_data->cmn_pa.header[PA_STATIC_NUM_ACTIVE_PA];

      /* Load PA R Map data */
      for ( pa_state = RFM_PA_GAIN_STATE_0; 
            pa_state < tx_static_struct->num_active_pa_states; 
            pa_state++ )
      {
        tx_static_struct->pa_r_map[pa_state] = 
                                   pa_static_data->cmn_pa.pa_range_map[pa_state];
      }
      /* For remaining unactive PA states, initialize Range map to invalid */
      for ( ; pa_state < RFM_PA_GAIN_STATE_NUM; pa_state++ )
      {
        tx_static_struct->pa_r_map[pa_state] = 255;
      }

      /* Load PA switchpoints */

      /* --------------------------- 1x Tx Voice ----------------------------- */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->voice_swpts.fall_hold_time =
                    pa_static_data->cmn_pa.pa_fall_sw[PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->voice_swpts.fall[pa_state] = 
                                     pa_static_data->cmn_pa.pa_fall_sw[pa_state];
        tx_1x_ptr->voice_swpts.rise[pa_state] = 
                                     pa_static_data->cmn_pa.pa_rise_sw[pa_state];
      }

      /* --------------------------- 1x Tx Access ----------------------------- */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->access_swpts.fall_hold_time =
        pa_static_data->cdma_pa.cdma_access_pa_fall_sw[PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->access_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->access_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->access_swpts.fall[pa_state] = 
                         pa_static_data->cdma_pa.cdma_access_pa_fall_sw[pa_state];
        tx_1x_ptr->access_swpts.rise[pa_state] = 
                         pa_static_data->cdma_pa.cdma_access_pa_rise_sw[pa_state];
      }

      /* --------------------------- 1x Tx Data ----------------------------- */
      /* Copy switchpoint data for all reverse link data rates */
      /* Rate 1 */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->data_swpts_rsch[0].fall_hold_time =
        pa_static_data->cdma_pa.cdma_data_rate1_pa_fall_sw
                                                    [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->data_swpts_rsch[0].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[0].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[0].fall[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate1_pa_fall_sw[pa_state];
        tx_1x_ptr->data_swpts_rsch[0].rise[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate1_pa_rise_sw[pa_state];
      }
      /* Rate 2 */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->data_swpts_rsch[1].fall_hold_time =
        pa_static_data->cdma_pa.cdma_data_rate2_pa_fall_sw
                                                    [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->data_swpts_rsch[1].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[1].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[1].fall[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate2_pa_fall_sw[pa_state];
        tx_1x_ptr->data_swpts_rsch[1].rise[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate2_pa_rise_sw[pa_state];
      }
      /* Rate 3 */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->data_swpts_rsch[2].fall_hold_time =
        pa_static_data->cdma_pa.cdma_data_rate3_pa_fall_sw
                                                    [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->data_swpts_rsch[2].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[2].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[2].fall[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate3_pa_fall_sw[pa_state];
        tx_1x_ptr->data_swpts_rsch[2].rise[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate3_pa_rise_sw[pa_state];
      }
      /* Rate 4 */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->data_swpts_rsch[3].fall_hold_time =
        pa_static_data->cdma_pa.cdma_data_rate4_pa_fall_sw
                                                    [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->data_swpts_rsch[3].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[3].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[3].fall[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate4_pa_fall_sw[pa_state];
        tx_1x_ptr->data_swpts_rsch[3].rise[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate4_pa_rise_sw[pa_state];
      }
      /* Rate 5 */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->data_swpts_rsch[4].fall_hold_time =
        pa_static_data->cdma_pa.cdma_data_rate5_pa_fall_sw
                                                    [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->data_swpts_rsch[4].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[4].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[4].fall[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate5_pa_fall_sw[pa_state];
        tx_1x_ptr->data_swpts_rsch[4].rise[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate5_pa_rise_sw[pa_state];
      }
      /* Rate 6 */
      /* Initialize the timer hysteresis value */
      tx_1x_ptr->data_swpts_rsch[5].fall_hold_time =
        pa_static_data->cdma_pa.cdma_data_rate6_pa_fall_sw
                                                    [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_1x_ptr->data_swpts_rsch[5].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_1x_ptr->data_swpts_rsch[5].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_1x_ptr->data_swpts_rsch[5].fall[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate6_pa_fall_sw[pa_state];
        tx_1x_ptr->data_swpts_rsch[5].rise[pa_state] = 
          pa_static_data->cdma_pa.cdma_data_rate6_pa_rise_sw[pa_state];
      }

      /* --------------------------- HDR Tx DO_R0 ---------------------------- */
      /* Copy switchpoint data for all reverse link data rates */
      /* RRI 0 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[0].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri0_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[0].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[0].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[0].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri0_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[0].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri0_pa_rise_sw[pa_state];
      }
      /* RRI 1 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[1].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri1_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[1].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[1].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[1].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri1_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[1].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri1_pa_rise_sw[pa_state];
      }
      /* RRI 1 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[1].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri1_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[1].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[1].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[1].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri1_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[1].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri1_pa_rise_sw[pa_state];
      }
      /* RRI 2 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[2].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri2_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[2].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[2].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[2].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri2_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[2].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri2_pa_rise_sw[pa_state];
      }
      /* RRI 3 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[3].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri3_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[3].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[3].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[3].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri3_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[3].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri3_pa_rise_sw[pa_state];
      }
      /* RRI 4 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[4].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri4_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[4].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[4].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[4].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri4_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[4].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri4_pa_rise_sw[pa_state];
      }
      /* RRI 5 */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dor0_swpts_rri[5].fall_hold_time =
       pa_static_data->cdma_pa.hdr_dor0_rri5_pa_fall_sw
                                                  [PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dor0_swpts_rri[5].fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dor0_swpts_rri[5].rise[RFM_PA_GAIN_STATE_0] = -700;

      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM; 
            pa_state++ )
      {
        tx_hdr_ptr->dor0_swpts_rri[5].fall[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri5_pa_fall_sw[pa_state];
        tx_hdr_ptr->dor0_swpts_rri[5].rise[pa_state] =
          pa_static_data->cdma_pa.hdr_dor0_rri5_pa_rise_sw[pa_state];
      }


      /* --------------------------- HDR Tx Access --------------------------- */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->access_swpts.fall_hold_time =
        pa_static_data->cdma_pa.hdr_access_pa_fall_sw[PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->access_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->access_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_hdr_ptr->access_swpts.fall[pa_state] = 
                         pa_static_data->cdma_pa.hdr_access_pa_fall_sw[pa_state];
        tx_hdr_ptr->access_swpts.rise[pa_state] = 
                         pa_static_data->cdma_pa.hdr_access_pa_rise_sw[pa_state];
      }

      /* --------------------------- HDR Tx DO_RA ---------------------------- */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dorA_swpts.fall_hold_time =
        pa_static_data->cdma_pa.hdr_dora_pa_fall_sw[PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dorA_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dorA_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_hdr_ptr->dorA_swpts.fall[pa_state] = 
                         pa_static_data->cdma_pa.hdr_dora_pa_fall_sw[pa_state];
        tx_hdr_ptr->dorA_swpts.rise[pa_state] = 
                         pa_static_data->cdma_pa.hdr_dora_pa_rise_sw[pa_state];
      }

      /* -------------------------- HDR Tx DO_RB MC -------------------------- */
      /* Initialize the timer hysteresis value */
      tx_hdr_ptr->dorB_mc_swpts.fall_hold_time =
        pa_static_data->
                       cdma_pa.hdr_dorb_mc_pa_fall_sw[PA_STATIC_TIMER_HYS_INDEX];
      /* Initialize lowest PA state switchpoints to min power */
      tx_hdr_ptr->dorB_mc_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
      tx_hdr_ptr->dorB_mc_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;
      for ( pa_state = RFM_PA_GAIN_STATE_1; pa_state < RFM_PA_GAIN_STATE_NUM;
            pa_state++ )
      {
        tx_hdr_ptr->dorB_mc_swpts.fall[pa_state] = 
                      pa_static_data->cdma_pa.hdr_dorb_mc_pa_fall_sw[pa_state];
        tx_hdr_ptr->dorB_mc_swpts.rise[pa_state] = 
                      pa_static_data->cdma_pa.hdr_dorb_mc_pa_rise_sw[pa_state];
      }

      /* ----------------------------- Pin/Pout ----------------------------- */
      /* Populate temp comp data structures for common temp comp 
      implementation */
      tx_static_struct->lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;
      for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_LINEARIZERS; 
            lin_index++ )
      {
        for ( temp_index = 0; temp_index < RFCOM_TXLIN_TEMP_BIN_SIZE; 
              temp_index++ )
        {
          for ( freq_index = 0; freq_index < RFCOM_TXLIN_NUM_CHANNELS; 
                freq_index++ )
          {
            tx_static_struct->
                      lin_temp_comp_data.lin_vs_temp_vs_freq.pin[lin_index]
                                                                [temp_index]
                                                                [freq_index] =
              pa_static_data->cmn_pa.tx_lin_pin[temp_index]
                                               [freq_index]
                                               [lin_index];
        
            tx_static_struct->
                      lin_temp_comp_data.lin_vs_temp_vs_freq.pout[lin_index]
                                                                 [temp_index]
                                                                 [freq_index] =
              pa_static_data->cmn_pa.tx_lin_pout[temp_index]
                                                [freq_index]
                                                [lin_index];
          }/* for ( freq_index = 0; freq_index < RFCOM_TXLIN_NUM_CHANNELS; .. */
        }/* for ( temp_index = 0; temp_index < RFCOM_TXLIN_TEMP_BIN_SIZE; .. */
      }/* for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_LINEARIZERS; .. */

    }/* if( tx_static_struct->pa_static_nv_version == RFCMN_NV_PA_STATIC_VER_1 )*/
    /* .. Add subsequent versions of PA static NV here ........................ */
    else
    {
      /* PA static NV is present on the phone but the version is not 
      supported. Hence, return FALSE and load legacy NVs */
      
      /* Set PA static NV version to 4-state PA */
      tx_static_struct->pa_static_nv_version = RFCMN_NV_LEGACY_FOUR_STATE_PA;

      /* Set num active PA states to 4 active PA states */
      tx_static_struct->num_active_pa_states = 4;

      ret_val = FALSE;
    }
  } /* if ( ret_val == TRUE ) */
  else
  {
    /* PA static NV is not present on the phone. Hence, return FALSE 
    and load legacy NVs */

    /* Set PA static NV version to 4-state PA */
    tx_static_struct->pa_static_nv_version = RFCMN_NV_LEGACY_FOUR_STATE_PA;

    /* Set num active PA states to 4 active PA states */
    tx_static_struct->num_active_pa_states = 4;

    ret_val = FALSE;
  }

  /* Free allocated memory */
  if ( pa_static_data != NULL )
  {
    modem_mem_free( pa_static_data, MODEM_MEM_CLIENT_RFA );
  }

  return ret_val;
} /* rf_cdma_nv_load_tx_pa_static() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Static NV data for all chains for all bands.

  @details
  Performs validation on CDMA NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The CDMA band for which NV data must be loaded on this chain
 
  @param nv_path
  The CDMA NV path for which NV must be loaded
 
  @param index_entry
  The 1x and HDR path+band index entry that contains the pointers to the
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
  TRUE if the configuration was successfully loaded from NV, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_static_items
(
  const rfm_cdma_band_class_type band,
  const rf_cdma_nv_path_type nv_path,
  rf_cdma_config_band_index_type* const index_entry,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  rf_1x_config_type* const rf_1x_config_data = index_entry->ptr_1x;
  rf_hdr_config_type* const rf_hdr_config_data = index_entry->ptr_hdr;
  boolean retval = TRUE;
  rf_cdma_config_type* rf_common_config_ptr;
  rf_cdma_static_nv_items_map_type *static_nv_tbl 
                                         = NULL; /* Pointer to Static NV */
  rf_cdma_cal_nv_items_map_type *cal_nv_tbl = NULL; /* Pointer to Cal NV */
  rfm_device_enum_type device;
  boolean rx_only_nv_path;

  if ( rf_1x_config_data == NULL ||
       rf_hdr_config_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_static_items: "
                      "Invalid NULL pointer for the config data" );
    return FALSE;
  }

  rf_common_config_ptr = rf_1x_config_data->cdma_common_ptr;

  /* Get Table for Static NV items */
  static_nv_tbl = rf_cdma_nv_get_static_nv_table( band,
                                                  nv_path );
  if ( static_nv_tbl == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_static_items: "
              "Static NV data structure error for Band %d, NV path %d",
              band, nv_path );
    return FALSE;
  }

  /* Get Table for Calibration NV items */
  cal_nv_tbl = rf_cdma_nv_get_calibration_table( band,
                                                 nv_path );
  if ( cal_nv_tbl == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_static_items: "
              "Cal NV data structure error for Band %d, NV path %d",
              band, nv_path );
    return FALSE;
  }

  /* Identify whether we're loading NV for a TxRx path or
  Rx only path */
  device = rf_cdma_nv_map_nv_path_to_device( nv_path );

  /* Check with RFC if Tx is supported on this device, if yes then its not
  an Rx only device */
  if ( rfc_cdma_is_tx_device( device ) )
  {
    rx_only_nv_path = FALSE;
  }
  else
  {
    rx_only_nv_path = TRUE;
  }

  /* -------------------------------- Common ---------------------------------*/
  /* Load common Tx PA Static NV data */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->pa_static_nv_z;

    retval = rf_cdma_nv_load_tx_pa_static( &rf_common_config_ptr->tx_static,
                                           &rf_1x_config_data->tx_static,
                                           &rf_hdr_config_data->tx_static,
                                           static_nv_tbl->pa_static_nv_z,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );

    /* Read the uncompressed PA static nv if the compressed nv does not exist */
    if (retval == FALSE ) {
       index_entry->last_nv_item_read = static_nv_tbl->pa_static_nv;
       retval = rf_cdma_nv_load_tx_pa_static( &rf_common_config_ptr->tx_static,
                                              &rf_1x_config_data->tx_static,
                                              &rf_hdr_config_data->tx_static,
                                              static_nv_tbl->pa_static_nv,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );

    }
    /* If generic PA static NV loading fails, load the legacy CDMA static NV
    items */
    if ( retval == FALSE )
    {
      /* Load PA Range Map */
      /* This item is important to load first for RF cal.  Even if there is not
      static or cal data, we still need this item to do PA-state mapped Tx
      calibration. */
      index_entry->last_nv_item_read = static_nv_tbl->bc_pa_r_map;
      retval = rf_cdma_nv_load_tx_pa_r_map( &rf_common_config_ptr->tx_static,
                                            static_nv_tbl->bc_pa_r_map,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );

      /* Load 1x Tx Switchpoints */
      if ( retval == TRUE && !rx_only_nv_path )
      {
        index_entry->last_nv_item_read = static_nv_tbl->access_1x_swpts;
        retval = rf_cdma_nv_load_1x_tx_swpts( band,
                                              &rf_1x_config_data->tx_static,
                                              static_nv_tbl,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
      }

      /* Load HDR Tx Switchpoints */
      if ( retval == TRUE && !rx_only_nv_path )
      {
        index_entry->last_nv_item_read = static_nv_tbl->access_hdr_swpts;
        retval = rf_cdma_nv_load_hdr_tx_swpts( &rf_1x_config_data->tx_static,
                                               &rf_hdr_config_data->tx_static,
                                               static_nv_tbl,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
      }

      if ( retval == TRUE && !rx_only_nv_path )
      {
        index_entry->last_nv_item_read = static_nv_tbl->lin_vs_temp;
        retval = rf_cdma_nv_load_tx_lin_temp( &rf_common_config_ptr->tx_static,
                                              static_nv_tbl->lin_vs_temp,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
      }

      if ( retval == TRUE && !rx_only_nv_path )
      {
        index_entry->last_nv_item_read = static_nv_tbl->lin_vs_temp_vs_freq_pin;
        retval = rf_cdma_nv_load_tx_lin_vs_temp_vs_freq( 
                                        &rf_common_config_ptr->tx_static,
                                        static_nv_tbl->lin_vs_temp_vs_freq_pout,
                                        static_nv_tbl->lin_vs_temp_vs_freq_pin,
                                        caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr );
      }
    }
  }

  /* Load the Enc BTF delay value */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->enc_btf_delay;
    retval = rf_cdma_nv_load_enc_btf_dly( rf_1x_config_data,
                                          static_nv_tbl->enc_btf_delay,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* -------------------------------- Rx -------------------------------------*/

  /* ---------------- Rx Switchpoint load -----------------------------------*/
  /* Rx switchpoint items for the 4 different 1x and HDR modes must be loaded
  in the sequence specified below. Default behavior for inactive items relies
  on this sequence. If 1x items are inactive, switchpoints are populated with
  zeros. HDR Rev0/RevA defaults to 1x switchpoints, HDR RevB SC defaults to
  HDR Rev0/RevA switchpoints, and HDR RevB MC defaults to HDR RevB SC */
  /* Load 1x Rx switchpoints */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = static_nv_tbl->rx_1x_swpts;
    retval = rf_cdma_nv_load_1x_rx_swpts( &rf_1x_config_data->rx_static,
                                          static_nv_tbl->rx_1x_swpts,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  /* Load HDR Rev0 RevA Rx switchpoints */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = static_nv_tbl->rx_dor0_a_swpts;
    retval = rf_cdma_nv_load_dor0_a_rx_swpts( &rf_1x_config_data->rx_static,
                                              &rf_hdr_config_data->rx_static,
                                              static_nv_tbl->rx_dor0_a_swpts,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
  }

  /* Load HDR RevB Single Carrier Rx switchpoints */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = static_nv_tbl->rx_dorb_sc_swpts;
    retval = rf_cdma_nv_load_dorb_sc_rx_swpts( &rf_hdr_config_data->rx_static,
                                               static_nv_tbl->rx_dorb_sc_swpts,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
  }

  /* Load HDR RevB MultiCarrier Rx switchpoints */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = static_nv_tbl->rx_dorb_mc_swpts;
    retval = rf_cdma_nv_load_dorb_mc_rx_swpts( &rf_hdr_config_data->rx_static,
                                               static_nv_tbl->rx_dorb_mc_swpts,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->intelliceiver_det_thresh;
    retval = rf_cdma_nv_load_intelliceiver_det_thresh( 
                                                &rf_1x_config_data->
                                                     cdma_common_ptr->ic_data,
                                                static_nv_tbl->
                                                     intelliceiver_det_thresh,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }

  /* Load Spur table for 1x */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = static_nv_tbl->onex_spur_table;
    retval = rf_cdma_nv_load_1x_spur_info( &rf_1x_config_data->rx_static,
                                           static_nv_tbl->onex_spur_table,
                                           caller_tcb_ptr, task_nv_wait_sig,
                                           task_wait_func_ptr );
  }

  /* Load Spur table for HDR */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = static_nv_tbl->hdr_spur_table;
    retval = rf_cdma_nv_load_hdr_spur_info( &rf_hdr_config_data->rx_static,
                                            static_nv_tbl->hdr_spur_table,
                                            caller_tcb_ptr, task_nv_wait_sig,
                                            task_wait_func_ptr );
  }

  /* -------------------------------- Tx -------------------------------------*/
  /* Load Tx MultiLin APT Adjust static NV data */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->multi_lin_apt_adj;
    retval = rf_cdma_nv_load_tx_apt_adj( &rf_1x_config_data->tx_static,
                                         &rf_hdr_config_data->tx_static,
                                         static_nv_tbl->multi_lin_apt_adj,
                                         caller_tcb_ptr,
                                         task_nv_wait_sig,
                                         task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    if ( nv_path == RF_CDMA_NV_PATH_0 )
    {
      index_entry->last_nv_item_read = RFNV_THERM_BINS_I;
      retval = rf_cdma_nv_load_tx_therm_bins( &rf_common_config_ptr->tx_cal,
                                              &rf_common_config_ptr->tx_static,
                                              RFNV_THERM_BINS_I,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
    }
    else if ( nv_path == RF_CDMA_NV_PATH_2 )
    {
      index_entry->last_nv_item_read = RFNV_C2_THERM_BINS_I;
      retval = rf_cdma_nv_load_tx_therm_bins( &rf_common_config_ptr->tx_cal,
                                              &rf_common_config_ptr->tx_static,
                                              RFNV_C2_THERM_BINS_I,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
    }
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->tx_gtr_thresh;
    retval = rf_cdma_nv_load_tx_gtr_thresh( &rf_common_config_ptr->tx_static,
                                            static_nv_tbl->tx_gtr_thresh,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->dpd_config_params;

    retval = rf_cdma_nv_load_dpd_cfg( &rf_common_config_ptr->tx_static,
                                      static_nv_tbl->dpd_config_params,
                                      caller_tcb_ptr,
                                      task_nv_wait_sig,
                                      task_wait_func_ptr );
  }

  /* Load HDR PWR Limit Data */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->hdr_tx_pwr_lim ;
    retval = rf_cdma_nv_load_hdr_pwr_limits( &rf_hdr_config_data->tx_static,
                                             band,
                                             static_nv_tbl->hdr_tx_pwr_lim,
                                             caller_tcb_ptr,
                                             task_nv_wait_sig,
                                             task_wait_func_ptr);
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->lim_vs_temp_sar_0;
    retval = rf_cdma_nv_load_tx_lim_static( &rf_common_config_ptr->tx_static,
                                            band,
                                            static_nv_tbl->lim_vs_temp_sar_0,
                                            static_nv_tbl->lim_vs_temp_sar_1_8,
                                            static_nv_tbl->lim_vs_temp_vs_freq,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->hdet_vs_temp;
    retval = rf_cdma_nv_load_tx_hdet_vs_temp( &rf_common_config_ptr->tx_static,
                                              static_nv_tbl->hdet_vs_temp,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->hdet_mod_range;
    retval = rf_cdma_nv_load_tx_hdet_mod_range( 
                                    &rf_common_config_ptr->tx_static,
                                    static_nv_tbl->hdet_mod_range,
                                    caller_tcb_ptr,
                                    task_nv_wait_sig,
                                    task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->tx_cal_chan_list;
    retval = rf_cdma_nv_load_tx_char_chan_list( 
                                           &rf_common_config_ptr->tx_static,
                                           cal_nv_tbl->tx_cal_chan_list,
                                           band,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->et_path_delay;
    retval = rf_cdma_nv_load_tx_et_path_delay( &rf_common_config_ptr->tx_cal,
                                               cal_nv_tbl->et_path_delay,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->et_path_delay_vs_freq;
    retval = rf_cdma_nv_load_tx_et_path_delay_vs_freq_items( 
                                           &rf_common_config_ptr->tx_cal,
                                                             cal_nv_tbl->et_path_delay_vs_freq,
                                                             band ,
                                                             caller_tcb_ptr,
                                                             task_nv_wait_sig,
                                                             task_wait_func_ptr );
  }
 
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = 
                            cal_nv_tbl->et_path_delay_offsets_vs_freq_vs_temp;
    retval = rf_cdma_nv_load_tx_et_path_delay_offsets_vs_freq_vs_temp_items( 
                             &rf_common_config_ptr->tx_cal,
                                                                             cal_nv_tbl->et_path_delay_offsets_vs_freq_vs_temp,
                                                                             band,
                                                                             caller_tcb_ptr,
                                                                             task_nv_wait_sig,
                                                                             task_wait_func_ptr );
  }


  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->xpt_dpd_scaling;
    retval = rfcommon_xpt_load_dpd_scaling_nv( 
                              cal_nv_tbl->xpt_dpd_scaling,
                                               &rf_common_config_ptr->tx_cal.xpt_dpd_scaling_data,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
  }

  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->ant_tuner_cl;
    retval = rf_cdma_nv_load_ant_tuner_cl( &rf_common_config_ptr->tx_static,
                                            static_nv_tbl->ant_tuner_cl,
                                            caller_tcb_ptr,
                                            task_nv_wait_sig,
                                            task_wait_func_ptr );
  }

  /* Load the Predictive Clipping Enable Threshold value */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = static_nv_tbl->pred_clip_pwr_thresh;
    retval = rf_cdma_nv_load_pred_clip_en_thresh(
                                          &rf_common_config_ptr->tx_static,
                                          static_nv_tbl->pred_clip_pwr_thresh,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_static_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Calibrated NV data for all chains for all bands.

  @details
  Performs validation on CDMA NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The CDMA band for which NV data must be loaded on this chain
 
  @param nv_path
  The CDMA NV path for which NV must be loaded

  @param index_entry
  The 1x and HDR path+band index entry that contains the pointers to the
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
  TRUE if the configuration was successfully loaded from NV, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_cal_items
(
  const rfm_cdma_band_class_type band,
  const rf_cdma_nv_path_type nv_path,
  rf_cdma_config_band_index_type* const index_entry,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  rf_1x_config_type* const rf_1x_config_data = index_entry->ptr_1x;
  rf_hdr_config_type* const rf_hdr_config_data = index_entry->ptr_hdr;
  boolean retval = TRUE;
  rf_cdma_config_type* rf_common_config_ptr;
  rf_cdma_cal_nv_items_map_type *cal_nv_tbl = NULL; /* Pointer to Cal NV */

  rfcommon_core_convert_freq_handler_type convert_chan_to_tx_freq
                                   = NULL; /* Tx Freq conversion handler */

  rfm_device_enum_type device;
  boolean rx_only_nv_path;

  if ( rf_1x_config_data == NULL ||
       rf_hdr_config_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_c0: Invalid NULL pointer for the config data" );
    return FALSE;
  }

  rf_common_config_ptr = rf_1x_config_data->cdma_common_ptr;

  /* Get Table for Calibration NV items */
  cal_nv_tbl = rf_cdma_nv_get_calibration_table( band, nv_path );
  if ( cal_nv_tbl == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_cal_items: "
              "Cal NV data structure error for Band %d, NV path %d",
              band, nv_path );
    return FALSE;
  }

  /* Get Freq Conversion Handler */
  convert_chan_to_tx_freq = rf_cdma_nv_get_chan_to_tx_freq_handler( band );
  if ( convert_chan_to_tx_freq == NULL )
  {
    RF_MSG_2 ( RF_ERROR,
               "rf_cdma_nv_load_cal_items: No Freq Conversion handler for "
               "Band %d, NV path %d",
               band, nv_path );
    return FALSE;
  }

  /* Identify whether we're loading NV for a TxRx path or
  Rx only path */
  device = rf_cdma_nv_map_nv_path_to_device( nv_path );

  /* Check with RFC if Tx is supported on this device, if yes then its not
  an Rx only device */
  if ( rfc_cdma_is_tx_device( device ) )
  {
    rx_only_nv_path = FALSE;
  }
  else
  {
    rx_only_nv_path = TRUE;
  }

  /* -------------------------------- Rx -------------------------------------*/
  /* Load calibration channel list and LNA and VGA cal data  */
  if ( retval == TRUE )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->rx_cal_data ;
    retval = rf_cdma_nv_load_unified_rx_cal ( &rf_common_config_ptr->rx_cal ,
                                              cal_nv_tbl->rx_cal_data ,
                                              band ,
                                              nv_path ,
                                              caller_tcb_ptr ,
                                              task_nv_wait_sig ,
                                              task_wait_func_ptr ) ;
  }

  if ( retval == FALSE )  /*  If the new NV has failed revert to the old one */
  {
    /* Load calibration channel list */
    index_entry->last_nv_item_read = cal_nv_tbl->rx_cal_chan_list;
    retval = rf_cdma_nv_load_rx_chan_list( &rf_common_config_ptr->rx_cal,
                                               cal_nv_tbl->rx_cal_chan_list,
                                               cal_nv_tbl->rx_cal_chan_lru_list,
                                               band,
                                               caller_tcb_ptr,
                                               task_nv_wait_sig,
                                               task_wait_func_ptr );
    /* Load G0 cal */
    if ( retval == TRUE )
    {
      index_entry->last_nv_item_read = cal_nv_tbl->dvga;
      retval = rf_cdma_nv_load_rx_cal( &rf_common_config_ptr->rx_cal,
                                       RFM_LNA_GAIN_STATE_0,
                                       cal_nv_tbl->dvga,
                                       cal_nv_tbl->dvga_vs_freq,
                                       3,
                                       caller_tcb_ptr,
                                       task_nv_wait_sig,
                                       task_wait_func_ptr );
    }
    /* Load G1 cal */
    if ( retval == TRUE )
    {
      index_entry->last_nv_item_read = cal_nv_tbl->lna_1;
      retval = rf_cdma_nv_load_rx_cal( &rf_common_config_ptr->rx_cal,
                                       RFM_LNA_GAIN_STATE_1,
                                       cal_nv_tbl->lna_1,
                                       cal_nv_tbl->lna_1_vs_freq,
                                       3,
                                       caller_tcb_ptr,
                                       task_nv_wait_sig,
                                       task_wait_func_ptr );
    }
    /* Load G2 cal */
    if ( retval == TRUE )
    {
      index_entry->last_nv_item_read = cal_nv_tbl->lna_2;
      retval = rf_cdma_nv_load_rx_cal( &rf_common_config_ptr->rx_cal,
                                       RFM_LNA_GAIN_STATE_2,
                                       cal_nv_tbl->lna_2,
                                       cal_nv_tbl->lna_2_vs_freq,
                                       3,
                                       caller_tcb_ptr,
                                       task_nv_wait_sig,
                                       task_wait_func_ptr );
    }
    /* Load G3 cal */
    if ( retval == TRUE )
    {
      index_entry->last_nv_item_read = cal_nv_tbl->lna_3;
      retval = rf_cdma_nv_load_rx_cal( &rf_common_config_ptr->rx_cal,
                                       RFM_LNA_GAIN_STATE_3,
                                       cal_nv_tbl->lna_3,
                                       cal_nv_tbl->lna_3_vs_freq,
                                       3,
                                       caller_tcb_ptr,
                                       task_nv_wait_sig,
                                       task_wait_func_ptr );
    }
    /* Load G4 cal */
    if ( retval == TRUE )
    {
      index_entry->last_nv_item_read = cal_nv_tbl->lna_4;
      retval = rf_cdma_nv_load_rx_cal( &rf_common_config_ptr->rx_cal,
                                       RFM_LNA_GAIN_STATE_4,
                                       cal_nv_tbl->lna_4,
                                       cal_nv_tbl->lna_4_vs_freq,
                                       3,
                                       caller_tcb_ptr,
                                       task_nv_wait_sig,
                                       task_wait_func_ptr );
    }
  } /* Use the old NVs end */

  /* Load Intelliceiver data */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->intelliceiver_cal;
    retval = rf_cdma_nv_load_intelliceiver_cal( &rf_1x_config_data->
                                                     cdma_common_ptr->ic_data,
                                                cal_nv_tbl->intelliceiver_cal,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
  }

  /* -------------------------------- Tx -------------------------------------*/
  /* Load Tx Multi-Linearizer NV data for all PA states */
  if (retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->multi_lin_v3;
	retval = rfcommon_core_dealloc_tx_band_cal( &rf_common_config_ptr->
                                                  tx_cal.tx_multi_lin_cal);
	if ( retval == TRUE )
	{
    retval = rfcommon_core_load_linearizer_from_nv( TX_MULTI_LIN_REV_3,
                                                    cal_nv_tbl->multi_lin_v3,
                                                    &rf_common_config_ptr->
                                                      tx_cal.tx_multi_lin_cal,
                                                    convert_chan_to_tx_freq );
	}

    /* If loading previous version failed... try this version */
    if ( retval == FALSE )
    {
      RF_MSG_3( RF_ERROR,
                "rf_cdma_nv_load_cal_items: Failed to load multilin V2 nv ID %d. "
                "Trying multilin V1 for band %d path %d.",
                index_entry->last_nv_item_read, band, nv_path );

    index_entry->last_nv_item_read = cal_nv_tbl->multi_lin;
    retval = rfcommon_core_load_linearizer_from_nv( TX_MULTI_LIN_REV_1,
                                                    cal_nv_tbl->multi_lin,
                                                    &rf_common_config_ptr->
                                                      tx_cal.tx_multi_lin_cal,
                                                    convert_chan_to_tx_freq );
    }

    /* Update the main multi_lin var if version-specific load succeeded */
    if ( retval == TRUE )
    {
      cal_nv_tbl->multi_lin = index_entry->last_nv_item_read;
      RF_MSG_3( RF_HIGH,
                "rf_cdma_nv_load_cal_items: Loaded multilin nv ID %d for band %d path %d.",
                index_entry->last_nv_item_read, band, nv_path );
    }

  }

  /* Load HDET cal data */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->lim_vs_freq;
    retval = rf_cdma_nv_load_tx_lim_cal( &rf_common_config_ptr->
                                                       tx_cal.tx_lim_cal,
                                         band,
                                         cal_nv_tbl->lim_vs_freq,
                                         cal_nv_tbl->hdet_vs_freq,
                                         cal_nv_tbl->hpm_exp_hdet_vs_agc,
                                         cal_nv_tbl->hpm_hdet_off,
                                         cal_nv_tbl->hpm_hdet_spn,
                                         cal_nv_tbl->lpm_hdet,
                                         caller_tcb_ptr,
                                         task_nv_wait_sig,
                                         task_wait_func_ptr );
  }

  /* Load pin_cal_vs_freq data */
  if ( retval == TRUE && !rx_only_nv_path )
  {
    index_entry->last_nv_item_read = cal_nv_tbl->pin_cal_vs_freq;
    retval = rf_cdma_nv_load_tx_pin_cal_vs_freq( 
                               &(rf_common_config_ptr->
                                 tx_static.lin_temp_comp_data.pin_cal_vs_freq),
                               cal_nv_tbl->pin_cal_vs_freq,
                               caller_tcb_ptr,
                               task_nv_wait_sig,
                               task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_cal_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return common NV List Table size

  @return
  Common NV List Table Size
*/
static uint16
rf_cdma_nv_get_common_item_list_size
(
  void
)
{
  return ( sizeof( (rf_cdma_nv_common_item_list) ) /
           sizeof( (rf_cdma_nv_common_item_list[0]) ) );
} /* rf_cdma_nv_get_common_item_list_size */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Copy common NV items to the NV Data structure

  @param item_code RF NV item enum
  @param rfnv_item_ptr Pointer to RFNV item
*/
static void
rf_cdma_nv_copy_common_items
(
  rfnv_item_id_enum_type  item_code,    /* Item to put */
  rfnv_item_type        *rfnv_item_ptr     /* Pointer where to get the item */
)
{
    word loop_ctr;        /* Loop Counter */

  nv_item_type* item_ptr = (nv_item_type*) rfnv_item_ptr;

  switch (item_code)
  {
    /*------------------------ New RF NV Items -----------------*/

#ifdef FEATURE_RFCAL_VERSION

    case RFNV_SYS_SW_VER_I:
      for( loop_ctr=0; loop_ctr<NV_SIZE_OF_VERSION; loop_ctr++ )
      {
        rf_cdma_nv_common_tbl.rfnv_sys_sw_ver[loop_ctr] =
          item_ptr->sys_sw_ver[loop_ctr] ;
      }
      break;
    case RFNV_RF_CAL_VER_I:
      for( loop_ctr=0; loop_ctr<NV_SIZE_OF_VERSION; loop_ctr++ )
      {
        rf_cdma_nv_common_tbl.rfnv_rf_cal_ver[loop_ctr] =
          item_ptr->rf_cal_ver[loop_ctr] ;
      }
      break;
    case RFNV_RF_CONFIG_VER_I:
      for( loop_ctr=0; loop_ctr<NV_SIZE_OF_VERSION; loop_ctr++ )
      {
        rf_cdma_nv_common_tbl.rfnv_rf_config_ver[loop_ctr] =
          item_ptr->rf_config_ver[loop_ctr] ;
      }
      break;
    case RFNV_RF_CAL_DATE_I:
      rf_cdma_nv_common_tbl.rfnv_rf_cal_date = item_ptr->rf_cal_date;
      break;
    case RFNV_RF_NV_LOADED_DATE_I:
      rf_cdma_nv_common_tbl.rfnv_rf_nv_loaded_date = item_ptr->rf_nv_loaded_date;
      break;
    case RFNV_RF_CAL_DAT_FILE_I:
      for( loop_ctr=0; loop_ctr<NV_QC_SERIAL_NUM_LEN; loop_ctr++ )
      {
        rf_cdma_nv_common_tbl.rfnv_rf_cal_dat_file[loop_ctr] =
          item_ptr->rf_cal_dat_file[loop_ctr] ;
      }
      break;

#endif //FEATURE_RFCAL_VERSION

    case RFNV_HDR_THERMAL_STATE_LIMITS_I:
      {
        /* Get NV which is defined in uint16 */
        uint16 temp[NV_TEMP_TABLE_SIZ];
        size_t data_length = sizeof(temp) ; 
        memscpy ( temp, data_length ,
                  rfnv_item_ptr, data_length ) ;
        /* Convert uint16 NV to int16 for driver to use */
        for (loop_ctr=0; loop_ctr < NV_TEMP_TABLE_SIZ; loop_ctr++)
        {
          rf_cdma_nv_common_tbl.rfnv_cdma_thermal_max_limit[loop_ctr] = (int16)(temp[loop_ctr]);
        }
      }
      break;

    default:
      MSG_ERROR( "RF NV item not accounted for: %d", item_code, 0,0 );
      break;
  }
} /* rf_cdma_nv_copy_common_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Load Common CDMA NV items
 
  @details
  This will Load all Common NV to memory.
 
  @param caller_tcb_ptr
  TCB of the Task to used during NV Load
  
  @param task_nv_wait_sig
  Signal to Wait for NV LOad
  
  @param task_wait_func_ptr
  Pointer to the function to wait on.
 
  @return
  Return the Status of NV Load
*/
static boolean
rf_cdma_nv_load_common_items
(
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{
  rfcommon_nv_status_type status; /* Status of NV load */
  rfcommon_rfnv_info_type nv_info; /* Structure to send NV Data */
  boolean ret_val; /* Return variable */

  /* Populate the structure with Common NV information */
  nv_info.rfnv_item_list_ptr = rf_cdma_nv_common_item_list;
  nv_info.get_rfnv_item_table = NULL;
  nv_info.get_rfnv_item_list_size = rf_cdma_nv_get_common_item_list_size;
  nv_info.rfnv_data_valid_flag = TRUE;
  nv_info.copy_rfnv_item = rf_cdma_nv_copy_common_items;

  /* Load Common NV */
  status = rfcommon_rfnv_retrieve_items( &nv_info, 1,
                                         caller_tcb_ptr, task_nv_wait_sig,
                                         task_wait_func_ptr);

  /* Determine the return value based on NV status */
  if ( status != RFCOMMON_NV_READ_SUCCESS )
  {
    ret_val = FALSE;
  }
  else
  {
    ret_val = TRUE;
  }

  /* return */
  return ret_val;

} /* rf_cdma_nv_load_common_items */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the NV data for Chain 0 for all bands.

  @details
  Performs validation on CDMA NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The CDMA band for which NV data must be loaded on this chain
 
  @param index_entry
  The 1x and HDR path+band index entry that contains the pointers to the
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
  TRUE if the configuration was successfully loaded from NV, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_c0
(
  const rfm_cdma_band_class_type band,
  rf_cdma_config_band_index_type* const index_entry,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  #ifdef T_RUMI_EMULATION
  
    retval= rf_cdma_nv_load_c0_fake_init(index_entry);
    return retval;
  
  #endif

  /* Load Static NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_static_items( band,
                                           RF_CDMA_NV_PATH_0,
                                           index_entry,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );
  }
  /* Load Calibrated NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_cal_items( band,
                                        RF_CDMA_NV_PATH_0,
                                        index_entry,
                                        caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_c0 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the NV data for Chain 1 for all bands.

  @details
  Performs validation on CDMA NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The CDMA band for which NV data must be loaded on this chain
 
  @param index_entry
  A pointer to the structure that contains the pointers to the 1x and HDR
  configuration data memory.
 
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
  TRUE if the configuration was successfully loaded from NV, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_c1
(
  const rfm_cdma_band_class_type band,
  rf_cdma_config_band_index_type* const index_entry,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  /* Load Static NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_static_items( band,
                                           RF_CDMA_NV_PATH_1,
                                           index_entry,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );
  }
  /* Load Calibrated NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_cal_items( band,
                                        RF_CDMA_NV_PATH_1,
                                        index_entry,
                                        caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr );
  }

  return retval;

} /* rf_cdma_nv_load_c1 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the NV data for Chain 2 for all bands.

  @details
  Performs validation on CDMA NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The CDMA band for which NV data must be loaded on this chain
 
  @param index_entry
  A pointer to the structure that contains the pointers to the 1x and HDR
  configuration data memory.
 
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
  TRUE if the configuration was successfully loaded from NV, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_c2
(
  const rfm_cdma_band_class_type band,
  rf_cdma_config_band_index_type* const index_entry,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  /* Load Static NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_static_items( band,
                                           RF_CDMA_NV_PATH_2,
                                           index_entry,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );
  }
  /* Load Calibrated NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_cal_items( band,
                                        RF_CDMA_NV_PATH_2,
                                        index_entry,
                                        caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_c2 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the NV data for Chain 3 for all bands.

  @details
  Performs validation on CDMA NV configuration data for the requested
  band, and if the values are consistent, loads them into the configuration
  data structure.
 
  @param band
  The CDMA band for which NV data must be loaded on this chain
 
  @param index_entry
  A pointer to the structure that contains the pointers to the 1x and HDR
  configuration data memory.
 
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
  TRUE if the configuration was successfully loaded from NV, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_c3
(
  const rfm_cdma_band_class_type band,
  rf_cdma_config_band_index_type* const index_entry,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;
  
  /* Load Static NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_static_items( band,
                                           RF_CDMA_NV_PATH_3,
                                           index_entry,
                                           caller_tcb_ptr,
                                           task_nv_wait_sig,
                                           task_wait_func_ptr );
  }
  /* Load Calibrated NV Items */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_cal_items( band,
                                        RF_CDMA_NV_PATH_3,
                                        index_entry,
                                        caller_tcb_ptr,
                                        task_nv_wait_sig,
                                        task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_c3 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Load CDMA NV
 
  @details
  This will Load CDMA NV to memory.
 
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
  Return the Status of NV Load
*/
boolean
rf_cdma_nv_load
(
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  ASSERT( caller_tcb_ptr == rex_self() );

  rf_cdma_nv_load_common_items( caller_tcb_ptr, task_nv_wait_sig,
                                task_wait_func_ptr );

  /* Initialize 1x configuration structure for primary chain */
  /* BC0 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC0 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC0 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c0( RFM_CDMA_BC0,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC1 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC1 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC1 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c0( RFM_CDMA_BC1,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC3 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC3 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC3 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC4 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC4 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC4 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c0( RFM_CDMA_BC4,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC5 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC5 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC5 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC6 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC6 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC6 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c0( RFM_CDMA_BC6,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC10 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC10 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC10 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c0( RFM_CDMA_BC10,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC14 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC14 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC14 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }
    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
    rf_cdma_nv_load_c0( RFM_CDMA_BC14,
                        index_entry,
                        caller_tcb_ptr,
                        task_nv_wait_sig,
                        task_wait_func_ptr );
  }
  /* BC15 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_0 ) & SYS_BAND_MASK_BC15 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_0, RFM_CDMA_BC15 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c0( RFM_CDMA_BC15,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }

  /* Initialize 1x configuration structure for secondary chain */
  /* BC0 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC0 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC0 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c1( RFM_CDMA_BC0,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC1 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC1 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC1 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c1( RFM_CDMA_BC1,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC3 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC3 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC3 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC4 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC4 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC4 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c1( RFM_CDMA_BC4,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC5 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC5 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC5 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC6 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC6 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC6 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c1( RFM_CDMA_BC6,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC10 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC10 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC10 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c1( RFM_CDMA_BC10,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC14 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC14 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC14 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
    rf_cdma_nv_load_c1( RFM_CDMA_BC14,
                        index_entry,
                        caller_tcb_ptr,
                        task_nv_wait_sig,
                        task_wait_func_ptr );
  }
  /* BC15 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_1 ) & SYS_BAND_MASK_BC15 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_1, RFM_CDMA_BC15 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c1( RFM_CDMA_BC15,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }

  /* Initialize 1x configuration structure for SV chain */
  /* BC0 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC0 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC0 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c2( RFM_CDMA_BC0,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC1 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC1 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC1 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c2( RFM_CDMA_BC1,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC3 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC3 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC3 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC4 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC4 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC4 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c2( RFM_CDMA_BC4,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC5 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC5 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC5 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC6 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC6 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC6 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c2( RFM_CDMA_BC6,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC10 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC10 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC10 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c2( RFM_CDMA_BC10,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC14 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC14 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC14 ) ;
    if ( index_entry == NULL )
    {
      return FALSE;
    }
    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
    rf_cdma_nv_load_c2( RFM_CDMA_BC14,
                        index_entry,
                        caller_tcb_ptr,
                        task_nv_wait_sig,
                        task_wait_func_ptr );
  }
  /* BC15 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_2 ) & SYS_BAND_MASK_BC15 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_2, RFM_CDMA_BC15 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c2( RFM_CDMA_BC15,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }


  /* Initialize 1x configuration structure for secondary chain */
  /* BC0 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC0 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC0 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c3( RFM_CDMA_BC0,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC1 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC1 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC1 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c3( RFM_CDMA_BC1,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC3 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC3 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC3 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC4 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC4 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC4 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c3( RFM_CDMA_BC4,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC5 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC5 )
  {
    if ( rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC5 ) == NULL )
    {
      return FALSE;
    }
  }
  /* BC6 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC6 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC6 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c3( RFM_CDMA_BC6,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC10 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC10 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC10 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c3( RFM_CDMA_BC10,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }
  /* BC14 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC14 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC14 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }
    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
    rf_cdma_nv_load_c3( RFM_CDMA_BC14,
                        index_entry,
                        caller_tcb_ptr,
                        task_nv_wait_sig,
                        task_wait_func_ptr );
  }
  /* BC15 */
  if ( rfcommon_nv_get_rf_bc_config( RF_PATH_3 ) & SYS_BAND_MASK_BC15 )
  {
    rf_cdma_config_band_index_type* index_entry;
    index_entry = rf_cdma_nv_alloc( RF_CDMA_NV_PATH_3, RFM_CDMA_BC15 );
    if ( index_entry == NULL )
    {
      return FALSE;
    }

    /* Store whether this band loaded successfully in the valid_data
       member of the index entry.  This will be used later to determine
       if it is safe to try tuning this band. */
    index_entry->data_valid = 
      rf_cdma_nv_load_c3( RFM_CDMA_BC15,
                          index_entry,
                          caller_tcb_ptr,
                          task_nv_wait_sig,
                          task_wait_func_ptr );
  }

  /* Load FBRX NV data, it is handled seperately, this api load all bands' NV */ 
  if( rfcommon_fbrx_nv_retrieve_1x_data( caller_tcb_ptr, 
                                       task_nv_wait_sig, 
                                       task_wait_func_ptr ) == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load: rfcommon_fbrx_nv_retrieve_1x_data returns FALSE " );
#ifndef T_RUMI_EMULATION	
    return FALSE;
#endif
  }

  return TRUE;
} /* rf_cdma_nv_load */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clean up all memory allcoations
 
*/
void
rf_cdma_nv_cleanup
(
  void
)
{
  rf_cdma_nv_path_type path;
  rfm_cdma_band_class_type band;

  /* Free the common pointers */
  while(rf_cdma_nv_common_config_storage.allocated > 0)
  {
    rf_cdma_nv_common_config_storage.allocated--;
	rfcommon_core_dealloc_tx_band_cal(&(rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated]->tx_cal.tx_multi_lin_cal) );
    modem_mem_free(rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated], MODEM_MEM_CLIENT_RFA );
    rf_cdma_nv_common_config_storage.buffers[rf_cdma_nv_common_config_storage.allocated]=NULL;
  }

  /* Free the hdr pointers */
  while(rf_cdma_nv_hdr_config_storage.allocated > 0)
  {
    rf_hdr_config_type* hdr_config_buffer;

    rf_cdma_nv_hdr_config_storage.allocated--;

    hdr_config_buffer = rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated];
    
    if ( hdr_config_buffer != NULL )
    {
      modem_mem_free(hdr_config_buffer->tx_static.tx_pwr_limit_data, MODEM_MEM_CLIENT_RFA); 
    }

    modem_mem_free(rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated], MODEM_MEM_CLIENT_RFA );
    rf_cdma_nv_hdr_config_storage.buffers[rf_cdma_nv_hdr_config_storage.allocated]=NULL;
  }

  /* Free the 1x pointers */
  while(rf_cdma_nv_1x_config_storage.allocated > 0)
  {
    rf_cdma_nv_1x_config_storage.allocated--;
    modem_mem_free(rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated], MODEM_MEM_CLIENT_RFA );
    rf_cdma_nv_1x_config_storage.buffers[rf_cdma_nv_1x_config_storage.allocated]=NULL;
  }

  // Set top most struct to default values
  for (path = RF_CDMA_NV_PATH_0; path < RF_CDMA_NV_PATH_NUM; path++)
  {
    for (band = RFM_CDMA_BC0; band < RFM_CDMA_MAX_BAND; band++)
    {
      rf_cdma_config_device_index[path].band_index[band].ptr_1x = NULL;
      rf_cdma_config_device_index[path].band_index[band].ptr_hdr = NULL;
      rf_cdma_config_device_index[path].band_index[band].last_nv_item_read = 0;
      rf_cdma_config_device_index[path].band_index[band].data_valid = FALSE;      
    } /* for each band */
  } /* for each path */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Rx calibration channel list for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the channel_list and freq_list
  members of the calibration structure.

  Up to 16 channels may be loaded from NV into the configuration data
  structure.  The channel list is terminated by the first index that
  contains the value 0.  No further channels are loaded from NV once a
  zero is found, and the channel_list and freq_list data members are
  filled with 0s in the remaining entries.  The number of valid channel
  entries is stored in the freq_list_size member of the configuration
  data structure.

  The channel list must be stored in ascending frequency sorted order,
  otherwise the NV is considered invalid, and the load function will fail.

  Cal channels are critical to radio operation, and must be active,
  otherwise the the function will return FALSE to mark this band as
  inoperable.
 
  @param cal_struct
  The Rx calibration structure that the channel list data is to be
  loaded in to.

  @param rx_channel_list_item
  The NV item that contains the list of Rx calibration channels.

  @param rx_channel_lru_list_item
  The NV item that contains the list of LRU (least-recently-used) 
  channel values.

  @param band
  Frequency computation needs to know what band the channels are in, so
  this is passed as a parameter.

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
  TRUE if the channel list data is valid, otherwise FALSE.
*/
boolean
rf_cdma_nv_load_rx_chan_list
(
  rf_cdma_rx_cal_type* const cal_struct,
  const rfnv_item_id_enum_type rx_channel_list_item,
  const rfnv_item_id_enum_type rx_channel_lru_list_item,
  const rfm_cdma_band_class_type band,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type   chan_list_nv_status;
  nv_item_type        chan_list_buffer;
  rfnv_item_size_type chan_list_returned_size =
                                sizeof(nv_item_type);
  nv_stat_enum_type   lru_chan_list_nv_status;
  nv_item_type        lru_chan_list_buffer;
  rfnv_item_size_type lru_chan_list_returned_size =
                                sizeof(nv_item_type);

  /* Array iterator */
  int i;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  chan_list_nv_status =
    rfcommon_rfnv_get_item( rx_channel_list_item,
                            (rfnv_item_type*)&chan_list_buffer,
                            &chan_list_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  lru_chan_list_nv_status =
    rfcommon_rfnv_get_item( rx_channel_lru_list_item,
                            (rfnv_item_type*)&lru_chan_list_buffer,
                            &lru_chan_list_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( chan_list_nv_status == NV_DONE_S )
  {
    /* Verify the NV item contains a valid sorted channel list */
    /* If the first item isn't 0, there is at least one channel in the list
       and the sort order needs to be checked. */
    if ( chan_list_buffer.c0_bc0_rx_cal_chan[0] != 0 )
    {
      /* Loop until the end of the channel list */
      for ( i = 1;
            i < NV_FREQ_TABLE_SIZ && chan_list_buffer.c0_bc0_rx_cal_chan[i] != 0;
            i++ )
      {
        /* Insure that freq(x[i-1]) < freq(x[i]) */
        if ( rf_cdma_get_rx_carrier_freq(band, 
                                         chan_list_buffer.c0_bc0_rx_cal_chan[i-1])
             >= rf_cdma_get_rx_carrier_freq(band, 
                                            chan_list_buffer.c0_bc0_rx_cal_chan[i]) )
        {
          return FALSE;
        }
      }
    } /* if ( chan_list_buffer.c0_bc0_rx_cal_chan[0] != 0 ) */

    /* If reach here, the channel list format is valid.  Load it into memory. */
    for ( i = 0;
          i < NV_FREQ_TABLE_SIZ && chan_list_buffer.c0_bc0_rx_cal_chan[i] != 0;
          i++ )
    {
      cal_struct->channel_list[i] = chan_list_buffer.c0_bc0_rx_cal_chan[i];
      cal_struct->freq_list[i]
        = rf_cdma_get_rx_carrier_freq(band, 
                                      chan_list_buffer.c0_bc0_rx_cal_chan[i]);
    } /* for each valid channel in the channel list */

    /* Record how many valid values are in the frequency list. */
    cal_struct->freq_list_size = i;

    /* Fill out the rest of the cal structure with 0's */
    while ( i < NV_FREQ_TABLE_SIZ )
    {
      cal_struct->channel_list[i] = 0;
      cal_struct->freq_list[i] = 0;
      i++;
    } /* while not reach the end of the array */
  } /* if ( chan_list_nv_status == NV_DONE_S ) */
  else
  {
    for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
    {
      cal_struct->channel_list[i] = 0;
      cal_struct->freq_list[i] = 0;
    }
    cal_struct->freq_list_size = 0;

    /* If the Rx cal channel list wasn't active, this should indicate a
       load failure, so this band can be marked as invalid. */
    load_success = FALSE;
  } /* else: if ( chan_list_nv_status == NV_DONE_S ) */

  if ( lru_chan_list_nv_status == NV_DONE_S )
  {
    /* Load the least-recently-used-channel list from NV */
    for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
    {
      cal_struct->lru_channel_list[i] = 
        lru_chan_list_buffer.c0_bc0_rx_cal_chan_lru[i];
    }
  } /* if (lru_chan_list_nv_status == NV_DONE_S) */
  else
  {
    /* Set all least-recently-used-channel values to zero */
    for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
    {
      cal_struct->lru_channel_list[i] = 0;
    }
  }

  return load_success;
} /* rf_cdma_nv_load_rx_chan_list() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to load the Rx calibration channel list for
  any band. if the channels are invalid an error is returned.
 
  @param cal_struct
  The Rx calibration structure that the gain cal data is to be loaded
  into.
 
  @param band
  Frequency computation needs to know what band the channels are in, so
  this is passed as a parameter.
 
  @param chan_list_arr
  List of channels retrieved from the NV.
 
  @return
  TRUE: Rx Cal load is successful
  FALSE: Something went wrong

*/
boolean
rf_cdma_load_rx_chan_list 
(
   const rfm_cdma_band_class_type   band ,
   rf_cdma_rx_cal_type* const       cal_struct ,
   uint16                           chan_list_arr[]
) 
{
  /* Array iterator */
 int i;
 boolean load_success = TRUE ;


  /* Verify the NV item contains a valid sorted channel list */
  /* If the first item isn't 0, there is at least one channel in the list
     and the sort order needs to be checked. */
  if ( chan_list_arr[0] != 0 )
  {
    /* Loop until the end of the channel list */
    for ( i = 1;
          ( (i < NV_FREQ_TABLE_SIZ) && (chan_list_arr[i] != 0) ) ;
          i++ )
    {
      /* Insure that freq(x[i-1]) < freq(x[i]) */
      if ( rf_cdma_get_rx_carrier_freq(band, chan_list_arr[i-1])
           >= rf_cdma_get_rx_carrier_freq(band, chan_list_arr[i]) )
      {
        load_success = FALSE ;
        break ;
      }
    }
  } /* if ( chan_list_arr[0] != 0 ) */

  if ( TRUE == load_success )
  {
    /* If we have reached here, the channel list format is valid.  Load it into memory. */
    for ( i = 0;
          i < NV_FREQ_TABLE_SIZ && chan_list_arr[i] != 0;
          i++ )
    {
      cal_struct->channel_list[i] = chan_list_arr[i] ;
      cal_struct->freq_list[i]
        = rf_cdma_get_rx_carrier_freq(band, chan_list_arr[i]) ;
    } /* for each valid channel in the channel list */

    /* Record how many valid values are in the frequency list. */
    cal_struct->freq_list_size = i;

    /* Fill out the rest of the cal structure with 0's */
    while ( i < NV_FREQ_TABLE_SIZ )
    {
      cal_struct->channel_list[i] = 0;
      cal_struct->freq_list[i] = 0;
      i++;
    } /* while not reach the end of the array */
  } /* load_success after sorted channel list */

  return load_success ;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to load the Rx gain calibration data for the
  requested gain state, for all IntelliCeiver states.

  @details
  Checks that the NV items for this device's and band's calibration data
  are loaded, and if so, initalizes the data in the entries of the
  rx_gain_calibration member of the calibration structure for the
  requested LNA gain state

  Rx gain data is considered critical to radio operation, but the
  number of gain states that are used by any radio is variable, so this
  function will not delcare the band invalid if the items are not
  loaded.
 
  @param cal_struct
  The Rx calibration structure that the gain cal data is to be loaded
  into.
 
  @param rx_gain_state
  LNA gain state for which Rx cal data must be loaded. This is required
  to identify which NV member to load data from.
 
  @param gain_offset_base_item
  The NV item that contains the base offset for the calibration data
  vs frequency for the requested gain state.

  @param gain_offset_freq_item
  The NV item that contains the 3x16 offsets that are added to the value
  in the base item to obtain the 16-frequency gain curve for each of
  the three IntelliCeiver states the driver supports for the
  requested gain state.

  @param num_power_modes
  The number of power mode states to load from NV.  This is required
  for chain 0 and chain 1 differences in NV storage capacity.

  @param lna_gain_vs_freq
  the LNA vs Freq table retrieved from the NV.

  @return
  TRUE if the gain configuration data for the requested
  gain state is valid and the gain state itself is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_rx_lna_data
(
  rf_cdma_rx_cal_type* const  cal_struct ,
  rfm_cdma_power_mode_type    power_mode ,
  int16*                      lna_gain_vs_freq[]    
)
{
  /* Data structure iterator */
  int freq_index ;
  boolean success = TRUE ;

  if ( lna_gain_vs_freq != NULL )   
  {
    rfm_lna_gain_state_type rx_gain_state = RFM_LNA_GAIN_STATE_0 ;
    for ( ; rx_gain_state < RFM_LNA_GAIN_STATE_NUM ; rx_gain_state++ )
    {
      int16* lna_gain_vs_freq_per_gain_state = 
        ( lna_gain_vs_freq[0] + (rx_gain_state * NV_FREQ_TABLE_SIZ) ) ;
      for ( freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
      {
        cal_struct->rx_gain_calibration[power_mode][rx_gain_state][freq_index] =
            lna_gain_vs_freq_per_gain_state[freq_index] ;
      }  /* for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ ) */
    }  /* rx_gain_state < RFM_LNA_GAIN_STATE_NUM */
  } /* lna_gain_vs_freq != NULL */
  else 
  {
    /* If NV tables were empty, zero out the calibration data. */
    memset ( &(cal_struct->rx_gain_calibration[0]) , 
             sizeof(cal_struct->rx_gain_calibration) , 0 ) ;
    success = FALSE ;
  }
  return success ;
} /* rf_cdma_nv_load_rx_lna_data() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to parse the NV tables and put it in a table.

  @details
  Helper function to parse the NV tables and put it in a table
  whose index corresponds to the PM. So the table will be
  arranged as such NV_LNA_OFFSETS[PM0] = PM0 LNA data,
  NV_LNA_OFFSETS[PM1] = PM1 LNA data and so on. If only one
  power mode is found it is copied to next higher PM mode
  because the SW needs 2 PMs to operate. 
 
  @param nv_cal_data [Input]
  The Rx NV calibration structure that the gain cal data is to
  be loaded into.
 
  @param num_of_fields [Input]
  Number of NV tables. Each PM will have its own NV tables.
 
  @param nv_path [Input]
  Chain for which items need to be retrieved.
 
  @param lna_offsets_vs_freq [Output]
  The NV data is parsed and copied into this pointer table.

  @param num_power_modes_found [Output]
  The number of power mode states found while loading from NV.

  @return
  TRUE if NV with 1 PM found
  otherwise FALSE.
*/
boolean
rf_cdma_get_lna_cal_tables
(
   rfnv_rx_cal_data_typeid_118_type *nv_cal_data ,
   int16* lna_offsets_vs_freq[] ,
   rfm_cdma_power_mode_type* power_modes_found 
)
{
  boolean success = FALSE ;

  if ( lna_offsets_vs_freq != NULL )
  {
    lna_offsets_vs_freq[0] = &(nv_cal_data->freq_comp_offset[0][0]) ;
    success = TRUE ;

    switch ( nv_cal_data->cal_type )
    {
      case RFNV_RX_CAL_NV_TYPE_CDMA_PM0 :
      {
        *power_modes_found =  RFM_CDMA_POWER_MODE_NORMAL ;
        break ;
      }

      case RFNV_RX_CAL_NV_TYPE_CDMA_PM1 :
      {
        *power_modes_found =  RFM_CDMA_POWER_MODE_1 ;
        break ;
      }

      case RFNV_RX_CAL_NV_TYPE_CDMA_PM2 :
      {
        *power_modes_found = RFM_CDMA_POWER_MODE_2 ;
        break ;
      }

      default:
      {
        /* This NV type doesnt contain LNA or PM mode info, fall through */
        lna_offsets_vs_freq = NULL ;
        *power_modes_found = RFM_CDMA_POWER_MODE_INVALID ;
        success = FALSE ;
        break ;
      } /* Default */
    } /* Switch */
  } /* lna_offsets_vs_freq != NULL */
  return success ;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Rx calibration channel list for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the channel_list and freq_list
  members of the calibration structure.

  Up to 16 channels may be loaded from NV into the configuration data
  structure.  The channel list is terminated by the first index that
  contains the value 0.  No further channels are loaded from NV once a
  zero is found, and the channel_list and freq_list data members are
  filled with 0s in the remaining entries.  The number of valid channel
  entries is stored in the freq_list_size member of the configuration
  data structure.

  The channel list must be stored in ascending frequency sorted order,
  otherwise the NV is considered invalid, and the load function will fail.

  Cal channels are critical to radio operation, and must be active,
  otherwise the the function will return FALSE to mark this band as
  inoperable.
 
  @param cal_struct
  The Rx calibration structure that the channel list data is to be
  loaded in to.

  @param rx_channel_list_item
  The NV item that contains the list of Rx calibration channels.

  @param rx_channel_lru_list_item
  The NV item that contains the list of LRU (least-recently-used) 
  channel values.

  @param band
  Frequency computation needs to know what band the channels are in, so
  this is passed as a parameter.
 
  @param nv_path
  Chain for which the NV needs to be loaded

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
  TRUE if the channel list data is valid, otherwise FALSE.
*/
boolean
rf_cdma_nv_load_unified_rx_cal
(
  rf_cdma_rx_cal_type* const cal_struct,
  const rfnv_item_id_enum_type rx_channel_list_item,
  const rfm_cdma_band_class_type band,
  const rf_cdma_nv_path_type nv_path,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type   nv_read_status ;
  rfnv_item_size_type chan_list_returned_size =
                                sizeof(rfnv_item_type);

  rfnv_item_type* compressed_nv_buffer = NULL ;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = FALSE ;
  rfnv_rx_cal_data_type rx_band_cal_data = { 0 } ;

  compressed_nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type) ,
                                        MODEM_MEM_CLIENT_RFA ) ;

  if ( compressed_nv_buffer != NULL )
  {
    nv_read_status =
      rfcommon_rfnv_get_item( rx_channel_list_item,
                              (rfnv_item_type*)compressed_nv_buffer,
                              &chan_list_returned_size,
                              caller_tcb_ptr,
                              task_nv_wait_sig,
                              task_wait_func_ptr );


    if ( ( nv_read_status == NV_DONE_S ) || ( nv_read_status == NV_NOTACTIVE_S ) ) 
    {
      /* NV Related to RX_GAIN_VS_FREQ  */

      /* Load the rx_band_cal_data with NV content.
         Also initialize load_success  */
      load_success = 
        rfcommon_populate_rx_gain_vs_freq_v2_nv (
          &rx_band_cal_data , (rfnv_item_type*) compressed_nv_buffer , 
          RFM_LNA_GAIN_STATE_NUM )  ;

      if ( TRUE == load_success )
      {
        /* Obtain new Rx Freq comp structures from NV Table pointer. */
        if ( rx_band_cal_data.variant_marker.num_elements > 0 )
        {
          uint16* chan_list_arr = NULL ;
          uint16 nv_path_index = 0 ;
          uint16 num_nv_items = rx_band_cal_data.variant_marker.num_elements ;
          rfnv_rx_cal_data_typeid_118_type *nv_cal_data = 
            rx_band_cal_data.cal_118_data ;

          /* Search for the right Path index */
          for ( nv_path_index = 0 ; 
                /* Iterate through the whole NV and match the path */  
                nv_path_index < num_nv_items ; nv_path_index++ )
          {
            /* Proceed if the path index matches the chain data 
               we are trying to load */
            if ( nv_cal_data[nv_path_index].path_idx == nv_path )
            {
              chan_list_arr = &(nv_cal_data[nv_path_index].channel_list[0]) ;

              /* Sanitze then copy the channels to Rx Cal struct */
              load_success =
                rf_cdma_load_rx_chan_list ( band , cal_struct, chan_list_arr ) ;

              if ( TRUE == load_success )
              {
                int16* lna_offsets_vs_freq[1] =  { NULL } ;
                rfm_cdma_power_mode_type pm_found = RFM_CDMA_POWER_MODE_INVALID ;

                /* Get the correct cal tables for a given chain from the NV */
                load_success = 
                  rf_cdma_get_lna_cal_tables ( &(nv_cal_data[nv_path_index]) , 
                                               lna_offsets_vs_freq , &pm_found ) ;
                if ( TRUE == load_success )
                {
                  load_success = 
                    rf_cdma_nv_load_rx_lna_data ( cal_struct , pm_found , 
                                                  &(lna_offsets_vs_freq[0]) ) ; 
                } /* Atleast 1 PM NV entires found  */
              } /* load_success after sorted channel list */
            } /* if ( nv_cal_data->path_idx == nv_path ) */
          } /* for ( path_index = 0 ; path_index < RF_CDMA_NV_PATH_MAX ; path_index++ ) */
        } /* If nos of elements are >= 1 */
        else
        {
          load_success = FALSE ;
        } /* load_success = rf_cdma_load_rx_chan_list() */
      } /* load_success == TRUE after rfcommon_populate_rx_gain_vs_freq_v2_nv() */
    } /* if ( chan_list_nv_status == NV_DONE_S ) */
    else
    {

      /* If the Rx cal channel list wasn't active, this should indicate a
         load failure, so this band can be marked as invalid. */
      load_success = FALSE ;
    } /* else: if ( chan_list_nv_status == NV_DONE_S ) */
  } /* if (  chan_list_buffer != NULL ) */
  else
  {
    load_success = FALSE ;
  }
  
  if ( compressed_nv_buffer != NULL )
  {
    modem_mem_free ( compressed_nv_buffer , MODEM_MEM_CLIENT_RFA ) ;
  }

  /* Now that the copying is done destroy the allocated memory. */
  if ( rx_band_cal_data.cal_118_data != NULL )
  {
    modem_mem_free ( rx_band_cal_data.cal_118_data, 
                         MODEM_MEM_CLIENT_RFA);
  }
  return load_success ;
} /* rf_cdma_nv_load_new_rx_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Rx gain calibration data for the requested 
  gain state, for all IntelliCeiver states.

  @details
  Checks that the NV items for this device's and band's calibration data
  are loaded, and if so, initalizes the data in the entries of the
  rx_gain_calibration member of the calibration structure for the
  requested LNA gain state

  Rx gain data is considered critical to radio operation, but the
  number of gain states that are used by any radio is variable, so this
  function will not delcare the band invalid if the items are not
  loaded.
 
  @param cal_struct
  The Rx calibration structure that the gain cal data is to be loaded
  into.
 
  @param rx_gain_state
  LNA gain state for which Rx cal data must be loaded. This is required
  to identify which NV member to load data from.
 
  @param gain_offset_base_item
  The NV item that contains the base offset for the calibration data
  vs frequency for the requested gain state.

  @param gain_offset_freq_item
  The NV item that contains the 3x16 offsets that are added to the value
  in the base item to obtain the 16-frequency gain curve for each of
  the three IntelliCeiver states the driver supports for the
  requested gain state.

  @param num_power_modes
  The number of power mode states to load from NV.  This is required
  for chain 0 and chain 1 differences in NV storage capacity.

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
  TRUE if the gain configuration data for the requested
  gain state is valid and the gain state itself is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_rx_cal
(
  rf_cdma_rx_cal_type* const cal_struct,
  const rfm_lna_gain_state_type rx_gain_state,
  const rfnv_item_id_enum_type gain_offset_base_item,
  const rfnv_item_id_enum_type gain_offset_freq_item,
  const int num_power_modes,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Buffers for base and frequency offsets for the requested gain
     state */
  nv_stat_enum_type   gain_offset_base_nv_status;
  nv_item_type        gain_offset_base_buffer;
  rfnv_item_size_type gain_offset_base_returned_size =
                                  sizeof(nv_item_type);
  nv_stat_enum_type   gain_offset_freq_nv_status;
  nv_item_type        gain_offset_freq_buffer;
  rfnv_item_size_type gain_offset_freq_returned_size =
                                  sizeof(nv_item_type);

  /* Data structure iterator */
  int i;

  gain_offset_base_nv_status =
    rfcommon_rfnv_get_item( gain_offset_base_item,
                            (rfnv_item_type*)&gain_offset_base_buffer,
                            &gain_offset_base_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  gain_offset_freq_nv_status =
    rfcommon_rfnv_get_item( gain_offset_freq_item,
                            (rfnv_item_type*)&gain_offset_freq_buffer,
                            &gain_offset_freq_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( gain_offset_base_nv_status == NV_DONE_S && 
       gain_offset_freq_nv_status == NV_DONE_S )
  {
    /* If both items were read successfully, configure the data structure */
    rfm_cdma_power_mode_type power_mode;

    /* Loop over the power modes that are valid in NV */
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          ( power_mode < num_power_modes &&
            power_mode < RFM_CDMA_POWER_MODE_MAX );
          power_mode++ )
    {
      for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
      {
        /* Load NV for the requested gain state */
        switch( rx_gain_state )
        {
        case RFM_LNA_GAIN_STATE_0:
          cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][i] =
            gain_offset_base_buffer.c0_bc0_vga_gain_offset
            + gain_offset_freq_buffer.c0_bc0_vga_gain_offset_vs_freq[
                                              power_mode*NV_FREQ_TABLE_SIZ + i];
          break;

        case RFM_LNA_GAIN_STATE_1:
          cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][i] =
            gain_offset_base_buffer.c0_bc0_lna_1_offset
            + gain_offset_freq_buffer.c0_bc0_lna_1_offset_vs_freq[
                                              power_mode*NV_FREQ_TABLE_SIZ + i];

          if ( cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][i] <= cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][i] )
          {
            RF_MSG( RF_ERROR, "rf_cdma_nv_load_rx_cal: Wrong LNA calibration value: G0 < G1" );
            return FALSE;
          }                       

          break;

        case RFM_LNA_GAIN_STATE_2:
          cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][i] =
            gain_offset_base_buffer.c0_bc0_lna_2_offset
            + gain_offset_freq_buffer.c0_bc0_lna_2_offset_vs_freq[
                                              power_mode*NV_FREQ_TABLE_SIZ + i];

          if ( cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][i] <= cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][i] )
          {
            RF_MSG( RF_ERROR, "rf_cdma_nv_load_rx_cal: Wrong LNA calibration value: G1 < G2" );
            return FALSE;   
          }             
          break;

        case RFM_LNA_GAIN_STATE_3:
          cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][i] =
            gain_offset_base_buffer.c0_bc0_lna_3_offset
            + gain_offset_freq_buffer.c0_bc0_lna_3_offset_vs_freq[
                                              power_mode*NV_FREQ_TABLE_SIZ + i];

          if ( cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][i] <= cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][i] )
          {
            RF_MSG( RF_ERROR, "rf_cdma_nv_load_rx_cal: Wrong LNA calibration value: G2 < G3" );
            return FALSE;   
          }              

          break;

        case RFM_LNA_GAIN_STATE_4:
          cal_struct->rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_4][i] =
            gain_offset_base_buffer.c0_bc0_lna_4_offset
            + gain_offset_freq_buffer.c0_bc0_lna_4_offset_vs_freq[
                                              power_mode*NV_FREQ_TABLE_SIZ + i];
          break;

        default:
          RF_MSG_1( RF_ERROR,
                    "rf_cdma_nv_load_rx_cal: Unsupported Rx Gain State: Gain G%d",
                    rx_gain_state );
          return FALSE;
        }
      }
    }

    /* Fill in the rest of the power modes with 0 */
    for ( power_mode = power_mode; /* Does nothing - from where we left off */
          power_mode < RFM_CDMA_POWER_MODE_MAX;
          power_mode++ )
    {
      for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
      {
        cal_struct->rx_gain_calibration[power_mode][rx_gain_state][i] = 0;
      }
    }
  } /* if ( gain_offset_base_nv_status == NV_DONE_S &&
            gain_offset_freq_nv_status == NV_DONE_S ) */
  else if ( gain_offset_base_nv_status == NV_NOTACTIVE_S && 
            gain_offset_freq_nv_status == NV_NOTACTIVE_S )
  {
    /* If both items were empty, zero out the calibration data. */
    rfm_cdma_power_mode_type power_mode;
    int freq_index;
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          power_mode < RFM_CDMA_POWER_MODE_MAX;
          power_mode++ )
    {
      for ( freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
      {
        cal_struct->rx_gain_calibration[power_mode][rx_gain_state][freq_index] = 0;
      }
    }
  } /* else if ( gain_offset_base_nv_status == NV_NOTACTIVE_S &&
                 gain_offset_freq_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_5( RF_ERROR,
              "rf_cdma_nv_load_rx_cal: NV calibration state is inconsistent: Gain G%d, base item %d status %d, offset item %d status %d",
              rx_gain_state,
              gain_offset_base_item, gain_offset_base_nv_status,
              gain_offset_freq_item, gain_offset_freq_nv_status );
    return FALSE;
  }

  return TRUE;
} /* rf_cdma_nv_load_rx_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to copy Rx switchpoint data from NV item buffer into the
  Rx switchpoint data structures.

  @details
  Initializes the data in the entries of the lna_info member of the static rx
  structure, which is passed in for the desired mode.
 
  Consolidated Rx switchpoint data consists of active/inactive status
  for a given linearity state. For active switchpoints, NV data is loaded
  for switchpoint type, rise/fall data for fixed switchpoints and hysteresis/
  backoff data for OLTA switchpoints, and time hysteresis data.
 
  For inactive linearity states, the switchpoint data is populated with zeros.

  @param rx_lna_swpt_info_ptr
  Reference to the LNA info that must be populated with the NV data. This 
  is part of the static NV data structures.
 
  @param rx_swpt_nv_ptr
  Reference to the buffer into which NV data is temporarily copied over
  
  @return
  TRUE if the switchpoint data is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_rx_swpts
(
  rfnv_rx_swpt_type* rx_swpt_nv_ptr,
  rf_cdma_lna_info_type* rx_lna_swpt_info_ptr
)
{
  rf_cdma_lna_lin_sp_info_type *lin_info_ptr;
  rf_cdma_lna_sp_info_type *sp_info_ptr;

  rfm_cdma_power_mode_type power_mode;
  for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
        power_mode < RFM_CDMA_POWER_MODE_MAX;
        power_mode++ )
  {
    /* Check whether switchpoints are active for this power_mode */
    lin_info_ptr = 
      &(rx_lna_swpt_info_ptr->lin_info[power_mode]);
    lin_info_ptr->lin_mode_active = 
      rx_swpt_nv_ptr->lin_data[power_mode].lin_mode_active;

    if ( lin_info_ptr->lin_mode_active ==
                   RF_CDMA_LNA_SWITCH_PT_ACTIVE )
    {
      rfm_lna_gain_step_type gain_step;
      /* Load active switchpoint data for all gain steps */
      for ( gain_step = RFM_LNA_GAIN_STEP_0_TO_1;
            gain_step < RFM_LNA_GAIN_STEP_NUM;
            gain_step ++ )
      {
        sp_info_ptr = 
           &(rx_lna_swpt_info_ptr->lin_info[power_mode].sp_info[gain_step]);

        /* Switchpoint type */
        sp_info_ptr->sp_mode = 
          rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].sp_mode;

        /* Switchpoint rise/fall data */
        if ( sp_info_ptr->sp_mode == 
             RF_CDMA_LNA_SWITCH_PT_MODE_OLTA )
        {
          sp_info_ptr->sp_data.olta.backoff = 
            rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].rise_backoff;
          sp_info_ptr->sp_data.olta.hysteresis = 
            rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].fall_hysteresis;
        }
        else if ( sp_info_ptr->sp_mode == 
                  RF_CDMA_LNA_SWITCH_PT_MODE_FIXED )
        {
          sp_info_ptr->sp_data.fixed.rise = 
            rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].rise_backoff;
          sp_info_ptr->sp_data.fixed.fall = 
            rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].fall_hysteresis;
        }
        else
        {
          RF_MSG_1( RF_ERROR,
                    "rf_cdma_nv_load_rx_swpts: "
                    "NV calibration state is inconsistent. "
                    "Invalid Rx switchpoint type: %d",
                    sp_info_ptr->sp_mode );
          return FALSE;
        }

        /* Switchpoint time hysteresis data */
        sp_info_ptr->post_fall_hold = 
          rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].fall_hold;
        sp_info_ptr->post_rise_hold = 
          rx_swpt_nv_ptr->lin_data[power_mode].sp_data[gain_step].rise_hold;
      }
    } /* if ( lin_info_ptr->lin_mode_active ==
                        RF_CDMA_LNA_SWITCH_PT_ACTIVE ) */
    else
    {
      rfm_lna_gain_step_type gain_step;
      for ( gain_step = RFM_LNA_GAIN_STEP_0_TO_1;
            gain_step < RFM_LNA_GAIN_STEP_NUM;
            gain_step ++ )
      {
        sp_info_ptr = 
             &(rx_lna_swpt_info_ptr->lin_info[power_mode].sp_info[gain_step]);

        /* Populate data with zeros for inactive linearity states */
        sp_info_ptr->sp_mode = 0;
        sp_info_ptr->sp_data.fixed.fall = 0;
        sp_info_ptr->sp_data.fixed.rise = 0;

        sp_info_ptr->post_fall_hold = 0;
        sp_info_ptr->post_rise_hold = 0;
      }
    }
  }

  return TRUE;
} /* rf_cdma_nv_load_rx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load 1x Rx switchpoint data for the requested NV item 
  across all linearity states.

  @details
  Checks that the NV item for this device's and band's switchpoint data
  is loaded, and if so, initializes the data in the entries of the
  lna_info member of the static rx structure, which is passed in for 
  the desired mode.
 
  Consolidated Rx switchpoint data consists of active/inactive status
  for a given linearity state. For active switchpoints, NV data is loaded
  for switchpoint type, rise/fall data for fixed switchpoints and hysteresis/
  backoff data for OLTA switchpoints, and time hysteresis data.
 
  For inactive linearity states, the switchpoint data is populated with zeros.
 
  For non-active NV item, the switchpoint data is populated with zeros.

  Rx switch points could be considered critical to radio operation, but
  a radio could be only configured for HDR (SVDO), and therefore it is
  not valid to fail NV loading just because 1x switch points aren't set.
 
  @param rx_1x_ptr
  Reference to the 1x static items Rx data structure.
 
  @param rx_swpt_nv_item
  The NV item that contains the consolidated Rx switchpoint data across 
  all linearity states.

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
  TRUE if the switchpoint data is valid and the NV item is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_1x_rx_swpts
(
  rf_1x_rx_static_type* rx_1x_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for 1x Rx switchpoint items */
  nv_stat_enum_type   rx_swpt_nv_status;
  rfnv_item_type      *rx_swpt_buffer;
  rfnv_item_size_type rx_swpt_returned_size =
                            sizeof(rfnv_item_type);
  rfnv_rx_swpt_type   *rx_swpt_nv_ptr;

  rx_swpt_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );
  if ( rx_swpt_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_1x_rx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  rx_swpt_nv_status =
    rfcommon_rfnv_get_item( rx_swpt_nv_item,
                            rx_swpt_buffer,
                            &rx_swpt_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( rx_swpt_nv_status == NV_DONE_S )
  {
    /* If rx switchpoints were read successfully, configure the data
    structure */
    rx_swpt_nv_ptr = (rfnv_rx_swpt_type *)rx_swpt_buffer;
    rf_cdma_nv_load_rx_swpts( rx_swpt_nv_ptr,
                              &rx_1x_ptr->lna_info );
    
    retval = TRUE;  
  } /* if ( rx_swpt_nv_status == NV_DONE_S ) */
  else if ( rx_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* If 1x swpts were empty/not-active, zero out the NV data. */
    rf_cdma_lna_lin_sp_info_type *lin_info_ptr;
    rf_cdma_lna_sp_info_type *sp_info_ptr;

    rfm_cdma_power_mode_type power_mode;
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          power_mode < RFM_CDMA_POWER_MODE_MAX;
          power_mode++ )
    {
      rfm_lna_gain_step_type gain_step;

      lin_info_ptr = 
        &(rx_1x_ptr->lna_info.lin_info[power_mode]);
      lin_info_ptr->lin_mode_active = 
        RF_CDMA_LNA_SWITCH_PT_INACTIVE;
      for ( gain_step = RFM_LNA_GAIN_STEP_0_TO_1;
            gain_step < RFM_LNA_GAIN_STEP_NUM;
            gain_step ++ )
      {
        sp_info_ptr = 
             &(rx_1x_ptr->lna_info.lin_info[power_mode].sp_info[gain_step]);

        sp_info_ptr->sp_mode = 0;
        sp_info_ptr->sp_data.fixed.fall = 0;
        sp_info_ptr->sp_data.fixed.rise = 0;

        sp_info_ptr->post_fall_hold = 0;
        sp_info_ptr->post_rise_hold = 0;
      }
    }

    retval = TRUE;
  } /* else if ( rx_swpt_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_1x_rx_swpts: NV state is inconsistent: Swpt item %d status %d",
              rx_swpt_nv_item, rx_swpt_nv_status );

    retval = FALSE;
  }

  modem_mem_free( rx_swpt_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;
} /* rf_cdma_nv_load_1x_rx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR Rev0/RevA Rx switchpoint data for the requested NV item 
  across all linearity states.

  @details
  Checks that the NV item for this device's and band's switchpoint data
  is loaded, and if so, initializes the data in the entries of the
  lna_info member of the static rx structure, which is passed in for 
  the desired mode.
 
  Consolidated Rx switchpoint data consists of active/inactive status
  for a given linearity state. For active switchpoints, NV data is loaded
  for switchpoint type, rise/fall data for fixed switchpoints and hysteresis/
  backoff data for OLTA switchpoints, and time hysteresis data.
 
  For inactive linearity states, the switchpoint data is populated with zeros.
 
  For non-active NV items, the Rev0/RevA switchpoint data is
  populated with the data that is provided for 1x operation.
 
  @param rx_1x_ptr
  Reference to the 1x static items Rx data structure.
 
  @param rx_hdr_ptr
  Reference to the HDR static items Rx data structure.
 
  @param rx_swpt_nv_item
  The NV item that contains the consolidated Rx switchpoint data across 
  all linearity states.

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
  TRUE if the switchpoint data is valid and the NV item is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_dor0_a_rx_swpts
(
  rf_1x_rx_static_type *rx_1x_ptr,
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR Rev0/RevA Rx switchpoint items */
  nv_stat_enum_type   rx_swpt_nv_status;
  rfnv_item_type      *rx_swpt_buffer;
  rfnv_item_size_type rx_swpt_returned_size =
                           sizeof(rfnv_item_type);
  rfnv_rx_swpt_type   *rx_swpt_nv_ptr;

  rx_swpt_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );
  if ( rx_swpt_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dor0_a_rx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  rx_swpt_nv_status =
    rfcommon_rfnv_get_item( rx_swpt_nv_item,
                            rx_swpt_buffer,
                            &rx_swpt_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( rx_swpt_nv_status == NV_DONE_S )
  {
    /* If rx switchpoints were read successfully, configure the data
    structure */
    rx_swpt_nv_ptr = (rfnv_rx_swpt_type *)rx_swpt_buffer;
    rf_cdma_nv_load_rx_swpts( rx_swpt_nv_ptr,
                              &rx_hdr_ptr->lna_info_dor0_a );

    retval = TRUE;
  } /* if ( rx_swpt_nv_status == NV_DONE_S ) */
  else if ( rx_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* If Rev0/RevA NV items are inactive, the switchpoint data
    must default to the data that is loaded for CDMA voice support i.e.
    1x Rx switchpoints */
    /* 1x Rx switchpoint pointers */
    rf_cdma_lna_lin_sp_info_type *lin_info_ptr;
    rf_cdma_lna_sp_info_type *sp_info_ptr;
    /* HDR Rev0/RevA Rx switchpoint pointers */
    rf_cdma_lna_lin_sp_info_type *dor0_a_lin_info_ptr;
    rf_cdma_lna_sp_info_type *dor0_a_sp_info_ptr;

    rfm_cdma_power_mode_type power_mode;
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          power_mode < RFM_CDMA_POWER_MODE_MAX;
          power_mode++ )
    {
      rfm_lna_gain_step_type gain_step;

      lin_info_ptr = 
        &(rx_1x_ptr->lna_info.lin_info[power_mode]);
      dor0_a_lin_info_ptr = 
        &(rx_hdr_ptr->lna_info_dor0_a.lin_info[power_mode]);

      dor0_a_lin_info_ptr->lin_mode_active = 
        lin_info_ptr->lin_mode_active;

      for ( gain_step = RFM_LNA_GAIN_STEP_0_TO_1;
            gain_step < RFM_LNA_GAIN_STEP_NUM;
            gain_step ++ )
      {
        sp_info_ptr = 
             &(rx_1x_ptr->lna_info.lin_info[power_mode].sp_info[gain_step]);
        dor0_a_sp_info_ptr = 
             &(rx_hdr_ptr->lna_info_dor0_a.lin_info[power_mode].sp_info[gain_step]);

        dor0_a_sp_info_ptr->sp_mode = sp_info_ptr->sp_mode;
        dor0_a_sp_info_ptr->sp_data.fixed.fall = sp_info_ptr->sp_data.fixed.fall;
        dor0_a_sp_info_ptr->sp_data.fixed.rise = sp_info_ptr->sp_data.fixed.rise;

        dor0_a_sp_info_ptr->post_fall_hold = sp_info_ptr->post_fall_hold;
        dor0_a_sp_info_ptr->post_rise_hold = sp_info_ptr->post_rise_hold;
      }
    }

    retval = TRUE;
  } /* else if ( rx_swpt_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_dor0_a_rx_swpts: NV state is inconsistent: Swpt item %d status %d",
              rx_swpt_nv_item, rx_swpt_nv_status );

    retval = FALSE;
  }

  modem_mem_free( rx_swpt_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_dor0_a_rx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR RevB SingleCarrier Rx switchpoint data for the
  requested NV item across all linearity states.

  @details
  Checks that the NV item for this device's and band's switchpoint data
  is loaded, and if so, initializes the data in the entries of the
  lna_info member of the static rx structure, which is passed in for 
  the desired mode.
 
  Consolidated Rx switchpoint data consists of active/inactive status
  for a given linearity state. For active switchpoints, NV data is loaded
  for switchpoint type, rise/fall data for fixed switchpoints and hysteresis/
  backoff data for OLTA switchpoints, and time hysteresis data.
 
  For inactive linearity states, the switchpoint data is populated with zeros.
 
  For non-active NV items, the RevB SingleCarrier switchpoint data is
  populated with the data that is provided for Rev0/RevA operation.

  @param rx_hdr_ptr
  Reference to the HDR static items Rx data structure.
 
  @param rx_swpt_nv_item
  The NV item that contains the consolidated Rx switchpoint data across 
  all linearity states.

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
  TRUE if the switchpoint data is valid and the NV item is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_dorb_sc_rx_swpts
(
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR RevB Single Carrier Rx switchpoint items */
  nv_stat_enum_type   rx_swpt_nv_status;
  rfnv_item_type      *rx_swpt_buffer;
  rfnv_item_size_type rx_swpt_returned_size =
                              sizeof(rfnv_item_type);
  rfnv_rx_swpt_type   *rx_swpt_nv_ptr;

  rx_swpt_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );
  if ( rx_swpt_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dorb_sc_rx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  rx_swpt_nv_status =
    rfcommon_rfnv_get_item( rx_swpt_nv_item,
                            rx_swpt_buffer,
                            &rx_swpt_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( rx_swpt_nv_status == NV_DONE_S )
  {
    /* If rx switchpoints were read successfully, configure the data
    structure */
    rx_swpt_nv_ptr = (rfnv_rx_swpt_type *)rx_swpt_buffer;
    rf_cdma_nv_load_rx_swpts( rx_swpt_nv_ptr,
                              &rx_hdr_ptr->lna_info_dorb_sc );

    retval = TRUE;
  } /* if ( rx_swpt_nv_status == NV_DONE_S ) */
  else if ( rx_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* If RevB SingleCarrier NV items are inactive, the switchpoint data
    must default to the data that is loaded for HDR Rev0/RevA switchpoints */

    /* HDR Rev0/RevA Rx switchpoint pointers */
    rf_cdma_lna_lin_sp_info_type *dor0_a_lin_info_ptr;
    rf_cdma_lna_sp_info_type *dor0_a_sp_info_ptr;
    /* HDR RevB Single Carrier Rx switchpoint pointers */
    rf_cdma_lna_lin_sp_info_type *dorb_sc_lin_info_ptr;
    rf_cdma_lna_sp_info_type *dorb_sc_sp_info_ptr;

    rfm_cdma_power_mode_type power_mode;
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          power_mode < RFM_CDMA_POWER_MODE_MAX;
          power_mode++ )
    {
      rfm_lna_gain_step_type gain_step;

      dor0_a_lin_info_ptr = 
        &(rx_hdr_ptr->lna_info_dor0_a.lin_info[power_mode]);
      dorb_sc_lin_info_ptr = 
        &(rx_hdr_ptr->lna_info_dorb_sc.lin_info[power_mode]);

      dorb_sc_lin_info_ptr->lin_mode_active = 
        dor0_a_lin_info_ptr->lin_mode_active;

      for ( gain_step = RFM_LNA_GAIN_STEP_0_TO_1;
            gain_step < RFM_LNA_GAIN_STEP_NUM;
            gain_step ++ )
      {
        dor0_a_sp_info_ptr = 
             &(rx_hdr_ptr->lna_info_dor0_a.lin_info[power_mode].sp_info[gain_step]);
        dorb_sc_sp_info_ptr = 
             &(rx_hdr_ptr->lna_info_dorb_sc.lin_info[power_mode].sp_info[gain_step]);

        dorb_sc_sp_info_ptr->sp_mode = dor0_a_sp_info_ptr->sp_mode;
        dorb_sc_sp_info_ptr->sp_data.fixed.fall = dor0_a_sp_info_ptr->sp_data.fixed.fall;
        dorb_sc_sp_info_ptr->sp_data.fixed.rise = dor0_a_sp_info_ptr->sp_data.fixed.rise;

        dorb_sc_sp_info_ptr->post_fall_hold = dor0_a_sp_info_ptr->post_fall_hold;
        dorb_sc_sp_info_ptr->post_rise_hold = dor0_a_sp_info_ptr->post_rise_hold;
      }
    }

    retval = TRUE;
  } /* else if ( rx_swpt_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_dorb_sc_rx_swpts: NV state is inconsistent: Swpt item %d status %d",
              rx_swpt_nv_item, rx_swpt_nv_status );

    retval = FALSE;
  }

  modem_mem_free( rx_swpt_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_dorb_sc_rx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load HDR RevB MultiCarrier Rx switchpoint data for the
  requested NV item across all linearity states.

  @details
  Checks that the NV item for this device's and band's switchpoint data
  is loaded, and if so, initializes the data in the entries of the
  lna_info member of the static rx structure, which is passed in for 
  the desired mode.
 
  Consolidated Rx switchpoint data consists of active/inactive status
  for a given linearity state. For active switchpoints, NV data is loaded
  for switchpoint type, rise/fall data for fixed switchpoints and hysteresis/
  backoff data for OLTA switchpoints, and time hysteresis data.
 
  For inactive linearity states, the switchpoint data is populated with zeros.
 
  For non-active NV items, the RevB MultiCarrier switchpoint data is
  populated with the data that is provided for RevB SingleCarrier operation.
 
  @param rx_hdr_ptr
  Reference to the HDR static items Rx data structure.
 
  @param rx_swpt_nv_item
  The NV item that contains the consolidated Rx switchpoint data across 
  all linearity states.

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
  TRUE if the switchpoint data is valid and the NV item is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_dorb_mc_rx_swpts
(
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR RevB Multi Carrier Rx switchpoint items */
  nv_stat_enum_type   rx_swpt_nv_status;
  rfnv_item_type      *rx_swpt_buffer;
  rfnv_item_size_type rx_swpt_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_rx_swpt_type   *rx_swpt_nv_ptr;

  rx_swpt_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );
  if ( rx_swpt_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_dorb_mc_rx_swpts: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  rx_swpt_nv_status =
    rfcommon_rfnv_get_item( rx_swpt_nv_item,
                            rx_swpt_buffer,
                            &rx_swpt_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( rx_swpt_nv_status == NV_DONE_S )
  {
    /* If rx switchpoints were read successfully, configure the data
    structure */
    rx_swpt_nv_ptr = (rfnv_rx_swpt_type *)rx_swpt_buffer;
    rf_cdma_nv_load_rx_swpts( rx_swpt_nv_ptr,
                              &rx_hdr_ptr->lna_info_dorb_mc );

    retval = TRUE;
  } /* if ( rx_swpt_nv_status == NV_DONE_S ) */
  else if ( rx_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* If RevB MultiCarrier NV items are inactive, the switchpoint data
    must default to the data that is loaded for HDR RevB SC switchpoints */

    /* HDR RevB Single Carrier Rx switchpoint pointers */
    rf_cdma_lna_lin_sp_info_type *dorb_sc_lin_info_ptr;
    rf_cdma_lna_sp_info_type *dorb_sc_sp_info_ptr;
    /* HDR RevB Multi Carrier Rx switchpoint pointers */
    rf_cdma_lna_lin_sp_info_type *dorb_mc_lin_info_ptr;
    rf_cdma_lna_sp_info_type *dorb_mc_sp_info_ptr;

    rfm_cdma_power_mode_type power_mode;
    for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
          power_mode < RFM_CDMA_POWER_MODE_MAX;
          power_mode++ )
    {
      rfm_lna_gain_step_type gain_step;

      dorb_sc_lin_info_ptr = 
        &(rx_hdr_ptr->lna_info_dorb_sc.lin_info[power_mode]);
      dorb_mc_lin_info_ptr = 
        &(rx_hdr_ptr->lna_info_dorb_mc.lin_info[power_mode]);

      dorb_mc_lin_info_ptr->lin_mode_active = 
        dorb_sc_lin_info_ptr->lin_mode_active;

      for ( gain_step = RFM_LNA_GAIN_STEP_0_TO_1;
            gain_step < RFM_LNA_GAIN_STEP_NUM;
            gain_step ++ )
      {
        dorb_sc_sp_info_ptr = 
             &(rx_hdr_ptr->lna_info_dorb_sc.lin_info[power_mode].sp_info[gain_step]);
        dorb_mc_sp_info_ptr = 
             &(rx_hdr_ptr->lna_info_dorb_mc.lin_info[power_mode].sp_info[gain_step]);

        dorb_mc_sp_info_ptr->sp_mode = dorb_sc_sp_info_ptr->sp_mode;
        dorb_mc_sp_info_ptr->sp_data.fixed.fall = dorb_sc_sp_info_ptr->sp_data.fixed.fall;
        dorb_mc_sp_info_ptr->sp_data.fixed.rise = dorb_sc_sp_info_ptr->sp_data.fixed.rise;

        dorb_mc_sp_info_ptr->post_fall_hold = dorb_sc_sp_info_ptr->post_fall_hold;
        dorb_mc_sp_info_ptr->post_rise_hold = dorb_sc_sp_info_ptr->post_rise_hold;
      }
    }

    retval = TRUE;
  } /* else if ( rx_swpt_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_dorb_mc_rx_swpts: NV state is inconsistent: Swpt item %d status %d",
              rx_swpt_nv_item, rx_swpt_nv_status );

    retval = FALSE;
  }

  modem_mem_free( rx_swpt_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_dorb_mc_rx_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx calibration channel list for any band.
  Currently NOT IN USE, saved for the future use.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the channel_list and freq_list
  members of the calibration structure.

  Up to 16 channels may be loaded from NV into the configuration data
  structure.  The channel list is terminated by the first index that
  contains the value 0.  No further channels are loaded from NV once a
  zero is found, and the channel_list and freq_list data members are
  filled with 0s in the remaining entries.  The number of valid channel
  entries is stored in the freq_list_size member of the configuration
  data structure.

  The channel list must be stored in ascending frequency sorted order,
  otherwise the NV is considered invalid, and the load function will fail.

  This NV item will only be loaded if the multi-linearizer data structure
  NV item is not active.  If we have fallen back to using old NV, the Tx
  cal channel list is critical to the operation of the radio, and the
  band will be marked invalid if this item is not active.

  @param cal_struct
  The Tx calibration structure that the channel list data is to be
  loaded in to.

  @param tx_channel_list_item
  The NV item that contains the list of Tx calibration channels.

  @param band
  Frequency computation needs to know what band the channels are in, so
  this is passed as a parameter.

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
  TRUE if the channel list data is valid, otherwise FALSE.
*/
boolean
rf_cdma_nv_load_tx_chan_list
(
  tx_band_cal_type* const cal_struct,
  const rfnv_item_id_enum_type tx_channel_list_item,
  const rfm_cdma_band_class_type band,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type   chan_list_nv_status;
  nv_item_type        chan_list_buffer;
  rfnv_item_size_type chan_list_returned_size =
                                sizeof(nv_item_type);

  /* Array iterator */
  int i;

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  chan_list_nv_status =
    rfcommon_rfnv_get_item( tx_channel_list_item,
                            (rfnv_item_type*)&chan_list_buffer,
                            &chan_list_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( chan_list_nv_status == NV_DONE_S )
  {
    /* Verify the NV item contains a valid sorted channel list */
    /* If the first item isn't 0, there is at least one channel in the list
       and the sort order needs to be checked. */
    if ( chan_list_buffer.c0_bc0_tx_cal_chan[0] != 0 )
    {
      /* Loop until the end of the channel list */
      for ( i = 1;
            i < NV_FREQ_TABLE_SIZ && chan_list_buffer.c0_bc0_tx_cal_chan[i] != 0;
            i++ )
      {
        /* Insure that freq(x[i-1]) < freq(x[i]) */
        if ( rf_cdma_get_tx_carrier_freq(band, 
                                     chan_list_buffer.c0_bc0_tx_cal_chan[i-1])
             >= rf_cdma_get_tx_carrier_freq(band, 
                                     chan_list_buffer.c0_bc0_tx_cal_chan[i]) )
        {
          RF_MSG_1( RF_ERROR, "rf_cdma_nv_load_tx_chan_list: Non-monotonic "
                              "Tx calibration frequency list for Band %d",
                  band );
          return FALSE;
        }
      }
    } /* if ( chan_list_buffer.c0_bc0_tx_cal_chan[0] != 0 ) */

    /* If reach here, the channel list format is valid.  Load it into memory. */
    for ( i = 0;
          i < NV_FREQ_TABLE_SIZ && chan_list_buffer.c0_bc0_tx_cal_chan[i] != 0;
          i++ )
    {
      cal_struct->tx_cal_chan[i] = chan_list_buffer.c0_bc0_tx_cal_chan[i];
      cal_struct->tx_cal_freq[i]
        = rf_cdma_get_tx_carrier_freq(band, 
                                      chan_list_buffer.c0_bc0_tx_cal_chan[i]);
    } /* for each valid channel in the channel list */

    /* Record how many valid values are in the frequency list. */
    cal_struct->tx_cal_chan_size = i;

    /* Fill out the rest of the cal structure with 0's */
    while ( i < NV_FREQ_TABLE_SIZ )
    {
      cal_struct->tx_cal_chan[i] = 65535;
      cal_struct->tx_cal_freq[i] = 0;
      i++;
    } /* while not reach the end of the array */
  } /* if ( chan_list_nv_status == NV_DONE_S ) */
  else
  {
    for ( i = 0; i < NV_FREQ_TABLE_SIZ; i++ )
    {
      cal_struct->tx_cal_chan[i] = 65535;
      cal_struct->tx_cal_freq[i] = 0;
    }
    cal_struct->tx_cal_chan_size = 0;

    /* If the Tx cal channel list wasn't active, this should indicate a
       load failure, so this band can be marked as invalid. */
    load_success = FALSE;
  } /* else: if ( chan_list_nv_status == NV_DONE_S ) */

  return load_success;
} /* rf_cdma_nv_load_tx_chan_list */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to compare values during qsort for increasing order sort
 
  @param p1
  Pointer to the first element to be compared
 
  @param p2
  Pointer to the second element to be compared
 
  @return
  Return -1 if p1 less than p2,
          0 if p1 equals p2,
          1 if p1 greater than p2
*/
static
int
qsort_compare_increasing_rgi
(
  const void* p1,
  const void* p2
)
{
  int ret_val;

  /* Typecast each element to the struct type */
  rgi_pwr_lin_data_type *rgi_pwr_1 = (rgi_pwr_lin_data_type *)p1;
  rgi_pwr_lin_data_type *rgi_pwr_2 = (rgi_pwr_lin_data_type *)p2;

  /* Perform a subtraction for the RGI values to
  compute greater than/less than/equals result */
  ret_val = (rgi_pwr_1->rgi) - (rgi_pwr_2->rgi);

  return ret_val;
} /* qsort_compare_increasing_rgi */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Tx RGI and linearizer cal data

  @details
  Checks that the NV items for this device's and band's calibration data are
  loaded in NV, and if so, initializes the internal data structures for the
  RGI and linearizer data for the requested band specific NV items and PA gain
  state. This data is collected at the reference channel.

  @param cal_struct
  The Tx calibration structure that the cal data is to be loaded
  into.
 
  @param pa_gain_state
  PA gain state for which Tx cal data must be loaded. This is required
  to identify which NV member to load data from.
 
  @param tx_rgi_nv_item
  The NV item that contains the RGIs for reference channel of the
  band for which this function is called.

  @param tx_lin_nv_item
  The NV item that contains the linearizer data i.e. measured power for 
  reference channel of the band for which this function is called.

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
  TRUE if the RGI and linearizer cal data for the requested gain state is valid 
  or inactive and the gain state is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_tx_rgi_lin_cal
(
  tx_band_cal_type* const cal_struct,
  const rfm_pa_gain_state_type pa_gain_state,
  const rfnv_item_id_enum_type tx_rgi_nv_item,
  const rfnv_item_id_enum_type tx_lin_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Buffers for RGI and lin cal data for the requested gain state */
  nv_stat_enum_type   tx_rgi_nv_status;
  nv_item_type        tx_rgi_nv_buffer;
  rfnv_item_size_type tx_rgi_nv_returned_size =
                             sizeof(nv_item_type);
  nv_stat_enum_type   tx_lin_nv_status;
  nv_item_type        tx_lin_nv_buffer;
  rfnv_item_size_type tx_lin_nv_returned_size =
                             sizeof(nv_item_type);

  /* Create a temporary holder for RGI and Lin Data loaded from
  NV. This data is then sorted using qsort */
  rgi_pwr_lin_data_type rgi_pwr_lin_data[RFCOM_TXLIN_NUM_SEGMENTS];

  /* Data structure iterator */
  int i;

  /* Check if the PA gain state is valid. Each gain state has a
     corresponding linearizer table entry */
  if ( pa_gain_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_rgi_lin_cal: Invalid PA Gain State %d",
              pa_gain_state );
    return FALSE;
  }

  tx_rgi_nv_status = 
    rfcommon_rfnv_get_item( tx_rgi_nv_item,
                            (rfnv_item_type*)&tx_rgi_nv_buffer,
                            &tx_rgi_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  tx_lin_nv_status = 
    rfcommon_rfnv_get_item( tx_lin_nv_item,
                            (rfnv_item_type*)&tx_lin_nv_buffer,
                            &tx_lin_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( tx_rgi_nv_status == NV_DONE_S && 
       tx_lin_nv_status == NV_DONE_S )
  {
    /* For legacy NV, only support the voice and MCDO linearizers */
    /* Tx cal data linearizer index given waveform type and PA range */
    tx_multi_linearizer_index_node_type* tx_lin_cal_data_index =
      &cal_struct->tx_pa_state_cal_data[pa_gain_state].
                                linearizer_index[TX_LINEARIZER_VOICE][0];
    tx_multi_linearizer_index_node_type* tx_lin_cal_data_index_data_type_2 =
      &cal_struct->tx_pa_state_cal_data[pa_gain_state].
                                linearizer_index[TX_LINEARIZER_DATA_TYPE_2][0];

    /* The channel-range item must be set to highest chan in order to indicate
    the use of this single linearizer as reference for all valid channels.
    Currently hard-coding the frequency value corresponding to this channel
    value to some high frequency value*/
    tx_lin_cal_data_index->upper_bound_freq = 2500000;
    tx_lin_cal_data_index_data_type_2->upper_bound_freq = 2500000;
    /* We must have at least one linearizer available to load. Allocate
    memory for it */
    tx_lin_cal_data_index->node_data.lin_data = 
                            (tx_linearizer_table_type*)
                            modem_mem_alloc( sizeof(tx_linearizer_table_type),
                                             MODEM_MEM_CLIENT_RFA );

    if ( tx_lin_cal_data_index->node_data.lin_data == NULL )
    {
      RF_MSG_1( RF_ERROR,
                "rf_cdma_nv_load_tx_rgi_lin_cal: Failed to allocate modem heap"
                "memory: PA State %d", pa_gain_state );
      return FALSE;
    }

    /* Assign the SAME linearizer to MCDO as well. For legacy cal, we
    support non-APT cal and the same cal data MUST be available for MCDO
    operation */
    tx_lin_cal_data_index_data_type_2->node_data.lin_data = 
                                 tx_lin_cal_data_index->node_data.lin_data;

    /* Load the data that may or may not be sorted based on NV writing
    functionality */
    for ( i = 0; i < RFCOM_TXLIN_NUM_SEGMENTS; i++ )
    {
      rgi_pwr_lin_data[i].rgi = 
        tx_rgi_nv_buffer.bc0_tx_pdm_0[i];
      rgi_pwr_lin_data[i].pwr =
        tx_lin_nv_buffer.bc0_tx_lin_0[i];
    }
    /* Sort by increasing RGI values */
    qsort( &rgi_pwr_lin_data, RFCOM_TXLIN_NUM_SEGMENTS,
           sizeof(rgi_pwr_lin_data_type), qsort_compare_increasing_rgi );

    /* Data is now sorted. Load the calibration data into the structure
    for the corresponding gain state */
    for ( i = 0; i < RFCOM_TXLIN_NUM_SEGMENTS; i++ )
    {
      /* Only the first linearizer is used */
      /* Populate RGI values */
      if ( rgi_pwr_lin_data[i].rgi != RF_CDMA_NV_LEGACY_TX_LUT_DELIMITER )
      {
        tx_lin_cal_data_index[0].node_data.lin_data->table[i].rgi = 
          rgi_pwr_lin_data[i].rgi;
      }
      else
      {
        /* Convert the invalid value to match common multi-lin invalid
        RGI value */
        tx_lin_cal_data_index[0].node_data.lin_data->table[i].rgi = 
          RF_CDMA_NV_TX_LUT_DELIMITER;
      }

      /* Populate Power values */
      if ( rgi_pwr_lin_data[i].pwr != RF_CDMA_NV_LEGACY_TX_PWR_DELIMITER )
      {
        tx_lin_cal_data_index[0].node_data.lin_data->table[i].power = 
          rgi_pwr_lin_data[i].pwr;
      }
      else
      {
        /* Convert the invalid value to match common multi-lin invalid
        Pwr value */
        tx_lin_cal_data_index[0].node_data.lin_data->table[i].power = 
          RF_CDMA_NV_TX_PWR_DELIMITER;
      }

      /* Populate APT values */
      tx_lin_cal_data_index[0].node_data.lin_data->table[i].apt =
        0xFFFF; /* 65535 - default bias */
    }
  } /* if ( tx_rgi_nv_status == NV_DONE_S && 
            tx_lin_nv_status == NV_DONE_S ) */
  else if ( tx_rgi_nv_status == NV_NOTACTIVE_S &&
            tx_lin_nv_status == NV_NOTACTIVE_S )
  {
    /* Only support the voice and MCDO linearizers for old NV */
    /* Tx cal data linearizer index given waveform type and PA range */
    tx_multi_linearizer_index_node_type* tx_lin_cal_data_index =
      &cal_struct->tx_pa_state_cal_data[pa_gain_state].
                                  linearizer_index[TX_LINEARIZER_VOICE][0];
    tx_multi_linearizer_index_node_type* tx_lin_cal_data_index_data_type_2 =
      &cal_struct->tx_pa_state_cal_data[pa_gain_state].
                                linearizer_index[TX_LINEARIZER_DATA_TYPE_2][0];

    /* NV Items for this PA state are inactive, don't allocate memory for
    linearizer table */
    tx_lin_cal_data_index->node_data.lin_data = NULL;
    tx_lin_cal_data_index->upper_bound_freq = 0;
    tx_lin_cal_data_index_data_type_2->node_data.lin_data = NULL;
    tx_lin_cal_data_index_data_type_2->upper_bound_freq = 0;
  } /* else if ( tx_rgi_nv_status == NV_NOTACTIVE_S &&
                 tx_lin_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_rgi_lin_cal: NV calibration state is "
              "inconsistent: PA State %d", pa_gain_state );
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_tx_rgi_lin_cal: RGI NV item %d status %d", 
              tx_rgi_nv_item, tx_rgi_nv_status );
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_tx_rgi_lin_cal: Lin NV item %d status %d", 
              tx_lin_nv_item, tx_lin_nv_status );
    return FALSE;
  }

  return TRUE;
} /* rf_cdma_nv_load_tx_rgi_lin_cal_new() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Tx frequency compensation NV data

  @details
  Checks that the NV items for this device's and band's calibration data are
  loaded in NV, and if so, initializes the internal data structures for the
  frequency comp data for the requested band and PA gain state. This data is 
  collected across upto 16 calibration channels in the Tx cal chan list.

  @param cal_struct
  The Tx calibration structure that the cal data is to be loaded
  into.
 
  @param pa_gain_state
  PA gain state for which Tx cal data must be loaded. This is required
  to identify which NV member to load data from.
 
  @param tx_lin_freq_nv_item
  The band-specific NV item that contains the frequency offsets for the 
  linearizer power across upto 16 channels in the Tx cal chan list.

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
  TRUE if the freq comp cal data for the requested gain state is valid 
  or inactive and the gain state is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_tx_lin_freq_cal
(
  tx_band_cal_type* const cal_struct,
  const rfm_pa_gain_state_type pa_gain_state,
  const rfnv_item_id_enum_type tx_lin_freq_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Buffers for freq comp data for the requested gain state */
  nv_stat_enum_type   tx_lin_freq_nv_status;
  nv_item_type        tx_lin_freq_nv_buffer;
  rfnv_item_size_type tx_lin_freq_nv_returned_size =
                                  sizeof(nv_item_type);

  /* Pointer to tx freq comp data */
  tx_freq_offset_table_row_type* tx_freq_cal_data_index;

  /* Data structure iterator */
  int i;

  /* Check if the PA gain state is valid. Each gain state has a
     corresponding linearizer table entry */
  if ( pa_gain_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_lin_freq_cal: Invalid PA Gain State %d",
              pa_gain_state );
    return FALSE;
  }

  tx_lin_freq_nv_status = 
    rfcommon_rfnv_get_item( tx_lin_freq_nv_item,
                            (rfnv_item_type*)&tx_lin_freq_nv_buffer,
                            &tx_lin_freq_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  tx_freq_cal_data_index =
    &(cal_struct->tx_pa_state_cal_data[pa_gain_state].tx_freq_offsets[TX_FREQCOMP_APT][0]);

  if ( tx_lin_freq_nv_status == NV_DONE_S )
  {
    /* Load the freq comp data into the structure for the corresponding
    gain state.
    Freq comp data is binned for all RGIs i.e. we use the same offsets
    for all RGI values with old NV implementation. The RGI threshold must be
    set to max value i.e. 128 */
    tx_freq_cal_data_index->rgi_threshold = RF_CDMA_NV_TX_LUT_MAX_VALUE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_CHANNELS; i++ )
    {
      tx_freq_cal_data_index->freq_comp[i] = 
        tx_lin_freq_nv_buffer.bc0_tx_pwr_comp0.nv_tx_pwr_comp_vs_freq[i];
    }
  } /* if ( tx_lin_freq_nv_status == NV_DONE_S ) */
  else if ( tx_lin_freq_nv_status == NV_NOTACTIVE_S )
  {
    /* Load default zero value into the internal data structure if the
    NV items are not active. The linearizer would not apply
    any frequency compensation in this case */
    tx_freq_cal_data_index->rgi_threshold = RF_CDMA_NV_TX_LUT_DELIMITER;
    for ( i = 0; i < RFCOM_TXLIN_NUM_CHANNELS; i++ )
    {
      tx_freq_cal_data_index->freq_comp[i] = 0;
    }
  } /* else if ( tx_lin_freq_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_lin_freq_cal: NV calibration state is inconsistent: PA State %d", 
              pa_gain_state );
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_tx_lin_freq_cal: Lin Freq NV item %d status %d", 
              tx_lin_freq_nv_item, tx_lin_freq_nv_status );
    return FALSE;
  }

  return TRUE;
} /* rf_cdma_nv_load_tx_lin_freq_cal_new() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Tx frequency compensation NV data

  @details
  Checks that the NV items for this device's and band's calibration data are
  loaded in NV, and if so, initializes the internal data structures for the
  frequency comp data for the requested band and PA gain state. This data is 
  collected across upto 16 calibration channels in the Tx cal chan list.

  @param cal_struct
  The Tx calibration structure that the cal data is to be loaded
  into.
 
  @param pa_gain_state
  PA gain state for which Tx cal data must be loaded. This is required
  to identify which NV member to load data from.
 
  @param tx_lin_freq_nv_item
  The band-specific NV item that contains the frequency offsets for the 
  linearizer power across upto 16 channels in the Tx cal chan list.

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
  TRUE if the freq comp cal data for the requested gain state is valid 
  or inactive and the gain state is valid, otherwise FALSE.
*/
static
boolean
rf_cdma_nv_load_tx_lin_freq_cal_sv
(
  tx_band_cal_type* const cal_struct,
  const rfm_pa_gain_state_type pa_gain_state,
  const rfnv_item_id_enum_type tx_lin_freq_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for freq comp data for the requested gain state */
  nv_stat_enum_type          tx_lin_freq_nv_status;
  rfnv_item_type             *tx_lin_freq_nv_buffer;
  rfnv_item_size_type        tx_lin_freq_nv_returned_size =
                                    sizeof(rfnv_item_type);
  rfnv_tx_pwr_freq_comp_type *tx_pwr_freq_comp;

  /* Pointer to tx freq comp data */
  tx_freq_offset_table_row_type* tx_freq_cal_data_index;

  /* Data structure iterator */
  int i;

  /* Check if the PA gain state is valid. Each gain state has a
     corresponding linearizer table entry */
  if ( pa_gain_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_lin_freq_cal_sv_new: "
              "Invalid PA Gain State %d",
              pa_gain_state );
    retval = FALSE;
    return retval;
  }

  tx_lin_freq_nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                           MODEM_MEM_CLIENT_RFA );
  if ( tx_lin_freq_nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lin_freq_cal_sv_new: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  tx_lin_freq_nv_status = 
    rfcommon_rfnv_get_item( tx_lin_freq_nv_item,
                            tx_lin_freq_nv_buffer,
                            &tx_lin_freq_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  tx_freq_cal_data_index =
    &cal_struct->tx_pa_state_cal_data[pa_gain_state].tx_freq_offsets[TX_FREQCOMP_APT][0];

  if ( tx_lin_freq_nv_status == NV_DONE_S )
  {
    /* Load the calibration data into the structure for the corresponding
       gain state */
    tx_pwr_freq_comp = (rfnv_tx_pwr_freq_comp_type *)tx_lin_freq_nv_buffer;

    /* Load the freq comp data into the structure for the corresponding
    gain state.
    Freq comp data is binned for all RGIs i.e. we use the same offsets
    for all RGI values with old NV implementation. The RGI threshold must be
    0xFFFF */

    tx_freq_cal_data_index->rgi_threshold = RF_CDMA_NV_TX_LUT_MAX_VALUE;
    for ( i = 0; i < RFCOM_TXLIN_NUM_CHANNELS; i++ )
    {
      tx_freq_cal_data_index->freq_comp[i] = 
        (int8)(tx_pwr_freq_comp->freq_comp[i]);
    }

    retval = TRUE;
  } /* if ( tx_lin_freq_nv_status == NV_DONE_S ) */
  else if ( tx_lin_freq_nv_status == NV_NOTACTIVE_S )
  {
    /* Load default zero value into the internal data structure if the
    NV items are not active. The linearizer would not apply
    any frequency compensation in this case */
    tx_freq_cal_data_index->rgi_threshold = RF_CDMA_NV_TX_LUT_DELIMITER;
    for ( i = 0; i < RFCOM_TXLIN_NUM_CHANNELS; i++ )
    {
      tx_freq_cal_data_index->freq_comp[i] = 0;
    }

    retval = TRUE;
  } /* else if ( tx_lin_freq_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_lin_freq_cal_sv_new: "
              "NV calibration state is inconsistent: PA State %d", 
              pa_gain_state );
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_tx_lin_freq_cal_sv_new: "
              "Lin Freq NV item %d status %d", 
              tx_lin_freq_nv_item, tx_lin_freq_nv_status );
    retval = FALSE;
  }

  modem_mem_free( tx_lin_freq_nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_tx_lin_freq_cal_sv_new() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Functionality that traverses the frequency compensation data for a given
  PA state and obtains the ref channel for the linearizer sweep
 
  @details
  Identifies the index of freq comp data that has zero offset. Then uses
  this index to obtain the channel from the calibration channel list,
  which should be the reference linearizer for the frequency compensation
  data. Uses the Tx carrier frequency of this reference channel
  and updates the calibration data that is passed into this function.
 
  @param cal_struct
  The Tx calibration structure that contains the freq comp data used to
  identify the reference channel and the structure that is updated with
  the ref channel frequency
 
  @param pa_gain_state
  PA gain state for which Tx reference channel must be identified.
 
  @return
  TRUE if the reference channel frequency was calculated successfully or
  the linearizer node for the requested gain state is not allocated, else
  FALSE.
*/
static
boolean
rf_cdma_nv_update_tx_ref_chan_freq
(
  tx_band_cal_type* const cal_struct,
  const rfm_pa_gain_state_type pa_gain_state
)
{
  /* Pointer to tx freq comp data */
  tx_freq_offset_table_row_type* tx_freq_cal_data_index;
  /* Pointer to linearizer data which must be updated */
  tx_multi_linearizer_index_node_type* tx_lin_cal_data_index;

  /* Data structure iterator */
  int i;

  /* Check if the PA gain state is valid. Each gain state has a
     corresponding linearizer table entry */
  if ( pa_gain_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_update_tx_ref_chan_freq: "
              "Invalid PA Gain State %d",
              pa_gain_state );
    return FALSE;
  }

  /* Update reference linearizer chan_freq information based on
  frequency compensation data collected */
  tx_lin_cal_data_index =
    &cal_struct->tx_pa_state_cal_data[pa_gain_state].
                                linearizer_index[TX_LINEARIZER_VOICE][0];
  if ( tx_lin_cal_data_index[0].node_data.lin_data != NULL )
  {
    tx_freq_cal_data_index =
      &(cal_struct->tx_pa_state_cal_data[pa_gain_state].tx_freq_offsets[TX_FREQCOMP_APT][0]);

    /* Scan through the frequency compensation offsets for this PA state.
    Identify the first index for which offset is zero. Use this index
    from the cal chan list to obtain ref channel frequency */
    for ( i = 0; i < RFCOM_TXLIN_NUM_CHANNELS; i++ )
    {
      if ( tx_freq_cal_data_index->freq_comp[i] == 0 )
      {
        tx_lin_cal_data_index[0].node_data.lin_data->chan_freq =
          cal_struct->tx_cal_freq[i];
      }
    }
  }
  else
  {
    RF_MSG_1 ( RF_HIGH, "rf_cdma_nv_update_tx_ref_chan_freq: "
                        "Linearizer node not allocated for PA State %d",
               pa_gain_state );
  }

  return TRUE;
} /* rf_cdma_nv_update_tx_ref_chan_freq() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx gain calibration data for the requested 
  PA gain state.
  Currently NOT IN USE, saved for the future use.

  @details
  Checks that the NV items for this device's and band's calibration data
  are loaded, and if so, initalizes the data in the entries of the
  tx cal members of the calibration structure for the requested PA gain state

  @param cal_struct
  The Tx calibration structure that the cal data is to be loaded
  into.
 
  @param pa_gain_state
  PA gain state for which Tx cal data must be loaded. This is required
  to identify which NV member to load data from.
 
  @param tx_rgi_nv_item
  The NV item that contains the rgi step data for reference channel of the
  band for which this function is called.

  @param tx_lin_nv_item
  The NV item that contains the linearizer data i.e. measured power for 
  reference channel of the band for which this function is called.

  @param tx_lin_freq_nv_item
  The band-specific NV item that contains the frequency offsets for the 
  linearizer power across upto 16 channels in the Tx cal chan list.

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
  TRUE if the linearizer data (rgi, lin, and freq comp) is valid or inactive,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_legacy_tx_cal
(
  rf_cdma_nv_path_type nv_path,
  tx_band_cal_type* const cal_struct,
  const rfm_pa_gain_state_type pa_gain_state,
  const rfnv_item_id_enum_type tx_rgi_nv_item,
  const rfnv_item_id_enum_type tx_lin_nv_item,
  const rfnv_item_id_enum_type tx_lin_freq_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  /* Load the Tx linearizer calibration data which includes RGI vs Pwr
     as well as APT information */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_tx_rgi_lin_cal( cal_struct,
                                             pa_gain_state,
                                             tx_rgi_nv_item,
                                             tx_lin_nv_item,
                                             caller_tcb_ptr,
                                             task_nv_wait_sig,
                                             task_wait_func_ptr );
  }
  /* Load the frequency compensation data for the linearizer */
  if ( retval == TRUE )
  {
    if ( nv_path == RF_CDMA_NV_PATH_0 )
    {
      retval = rf_cdma_nv_load_tx_lin_freq_cal( cal_struct,
                                                pa_gain_state,
                                                tx_lin_freq_nv_item,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
    }
    else if ( nv_path == RF_CDMA_NV_PATH_2 )
    {
      retval = rf_cdma_nv_load_tx_lin_freq_cal_sv( cal_struct,
                                                   pa_gain_state,
                                                   tx_lin_freq_nv_item,
                                                   caller_tcb_ptr,
                                                   task_nv_wait_sig,
                                                   task_wait_func_ptr );
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_nv_load_legacy_tx_cal: Requested Tx cal "
                          "load on invalid NV Path %d",
                nv_path );
      retval = FALSE;
    }
  }
  /* Update the reference channel frequency for linearizer data */
  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_update_tx_ref_chan_freq( cal_struct,
                                                 pa_gain_state );
  }

  return retval;
} /* rf_cdma_nv_load_legacy_tx_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Tx PA Range Map Value for any band.

  @details
  Checks that the NV item for this device's and band's calibration channel
  list is loaded, and if so, initalizes the PA Range Map data for that band.

  @param static_nv_struct
  The Tx static NV data structure that the PA range map value must be loaded
  into.

  @param bc_pa_r_map_item
  The NV item that contains the PA Range Map value.

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
  TRUE if the PA Range Map value is loaded, otherwise FALSE.
*/
boolean
rf_cdma_nv_load_tx_pa_r_map
(
  rf_cdma_tx_static_type* const static_nv_struct,
  const rfnv_item_id_enum_type bc_pa_r_map_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     bc_pa_r_map_nv_status;
  nv_item_type          bc_pa_r_map_buffer;
  rfnv_item_size_type   bc_pa_r_map_returned_size =
                                sizeof(nv_item_type);

  byte nv_pa_r_map; /* PA range map as read from NV */
  rfm_pa_range_type gain_range_mapped = RFM_PA_GAIN_RANGE_NUM;
  rfm_pa_gain_state_type pa_state;
  int8 nv_bit_idx = 0;

  bc_pa_r_map_nv_status =
    rfcommon_rfnv_get_item( bc_pa_r_map_item,
                            (rfnv_item_type*)&bc_pa_r_map_buffer,
                            &bc_pa_r_map_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( bc_pa_r_map_nv_status == NV_DONE_S )
  {
    /* Load the PA R Map value for the band into its data structure
    and set the value loaded flag to TRUE */
    static_nv_struct->tx_pa_r_map = bc_pa_r_map_buffer.bc0_pa_r_map;

    /* Following is the PA Range Map Bit definition
    -------------------------------------------------
    | b7  | b6  | b5  | b4  | b3  | b2  | b1  | b0  |
    -------------------------------------------------
    | PAState 3 | PAState 2 | PAState 1 | PAState 0 |
    -------------------------------------------------
    | High Gain | High Gain | Mid Gain  | Low Gain  |
    -------------------------------------------------*/

    nv_pa_r_map = static_nv_struct->tx_pa_r_map;
    for ( pa_state = RFM_PA_GAIN_STATE_0; pa_state < RFM_PA_GAIN_STATE_4; 
          pa_state++ )
    {
      nv_bit_idx = pa_state * 2;

      gain_range_mapped = (rfm_pa_range_type)( (nv_pa_r_map >> nv_bit_idx) & 0x3 );

      if ( gain_range_mapped >= RFM_PA_GAIN_RANGE_NUM )
      {
        RF_MSG_1( RF_ERROR, "rf_cdma_map_pa_state_to_gain_range: "
                            "Invalid PA Gain Range obtained from NV: %d",
                            gain_range_mapped);
      }
      /* Assign Mapped PA Range */
      static_nv_struct->pa_r_map[pa_state] = (uint16)gain_range_mapped;
    }
    /* For remaining PA states not supported by legacy NV, initialize the PA R 
    Map to invalid */
    for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM; 
          pa_state++ )
    {
      static_nv_struct->pa_r_map[pa_state] = 255;
    }
  } /* if ( bc_pa_r_map_nv_status == NV_DONE_S ) */
  else
  {
    /* The BC NV item for PA Range Map was not loaded successfully.
    Indicate NV loading failed as this is critical to CDMA operation */
    static_nv_struct->tx_pa_r_map = 0;
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_pa_r_map: Uninitialized PA R Map NV" );
    return FALSE;
  } /* else: if ( bc_pa_r_map_nv_status == NV_DONE_S ) */

  return TRUE;
} /* rf_cdma_nv_load_tx_pa_r_map() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx max power (lim) vs freq data from NV

  @details
  Checks that the Tx Lim vs Freq NV item for the requested device and band
  is loaded currently from NV, and initializes the data for use by the
  HDET algorithm.

  @param lim_nv_struct
  The Tx NV data structure that HDET calibration information must be loaded
  into

  @param lim_vs_freq_item
  The NV item that contains Tx Lim vs Freq data.

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
  TRUE if the Lim vs Freq data is loaded, FALSE if there was a problem
  identifying the NV read status.
*/
static
boolean
rf_cdma_nv_load_tx_lim_freq_cal
(
  rf_cdma_tx_lim_cal_type* const lim_nv_struct,
  const rfnv_item_id_enum_type lim_vs_freq_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     lim_vs_freq_nv_status;
  rfnv_item_type        *lim_vs_freq_buffer;
  rfnv_item_size_type   lim_vs_freq_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_tx_pwr_freq_comp_type* lim_freq_data;

  /* Data structure iterator */
  int i;

  lim_vs_freq_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                        MODEM_MEM_CLIENT_RFA );
  if ( lim_vs_freq_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lim_freq_cal: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  lim_vs_freq_nv_status =
    rfcommon_rfnv_get_item( lim_vs_freq_item,
                            lim_vs_freq_buffer,
                            &lim_vs_freq_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( lim_vs_freq_nv_status == NV_DONE_S )
  {
    /* Load the max power vs frequency compensation data across
    the list of calibration channels */
    lim_freq_data = (rfnv_tx_pwr_freq_comp_type*)lim_vs_freq_buffer;

    for ( i = 0; i < RF_CDMA_NV_FREQ_LIST_SIZE; i++ )
    {
      lim_nv_struct->hdet_vs_freq[i] = 
        (int8)(lim_freq_data->freq_comp[i]);
    }

    retval = TRUE;
  }
  else if ( lim_vs_freq_nv_status == NV_NOTACTIVE_S )
  {
    /* Initialize the freq comp data to zero */
    memset( &(lim_nv_struct->hdet_vs_freq), 0,
            sizeof(int8)*RF_CDMA_NV_FREQ_LIST_SIZE );

    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lim_freq_cal: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  modem_mem_free( lim_vs_freq_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_tx_lim_freq_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx HDET calibration data from NV

  @details
  Checks that the HDET calibration for normal High Power mode items is
  loaded correctly from NV, and populates internal data structures with the
  correct device and band specific information.

  @param lim_nv_struct
  The Tx NV data structure that HDET calibration information must be loaded
  into

  @param band
  The band number for this NV item.  This is required to compute
  the Tx power in dBm10 from the NV item when loading the internal data
  structures.

  @param exp_hdet_vs_agc_item
  The NV item that contains expected HDET vs AGC data.
 
  @param hdet_off_item
  The NV item that contains HDET offset.
 
  @param hdet_spn_item
  The NV item that contains HDET span.
 
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
  TRUE if all three HDET NV items are loaded correctly or are inactive,
  else FALSE if NV state is inconsistent.
*/
static
boolean
rf_cdma_nv_load_tx_hdet_cal
(
  rf_cdma_tx_lim_cal_type* const lim_nv_struct,
  const rfm_cdma_band_class_type band,
  const rfnv_item_id_enum_type exp_hdet_vs_agc_item,
  const rfnv_item_id_enum_type hdet_off_item,
  const rfnv_item_id_enum_type hdet_spn_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     exp_hdet_vs_agc_status;
  rfnv_item_type        *exp_hdet_vs_agc_buffer;
  rfnv_item_size_type   exp_hdet_vs_agc_returned_size =
                                 sizeof(rfnv_item_type);
  nv_stat_enum_type     hdet_off_status;
  nv_item_type          hdet_off_buffer;
  rfnv_item_size_type   hdet_off_returned_size =
                                 sizeof(nv_item_type);
  nv_stat_enum_type     hdet_spn_status;
  nv_item_type          hdet_spn_buffer;
  rfnv_item_size_type   hdet_spn_returned_size =
                                 sizeof(nv_item_type);

  /* Data Structure Iterator */
  int i;

  /* Typedef for hdet_vs_agc data */
  rfnv_tx_hdet_vs_agc *hdet_vs_agc_data;

  exp_hdet_vs_agc_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                            MODEM_MEM_CLIENT_RFA );
  if ( exp_hdet_vs_agc_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_hdet_cal: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  exp_hdet_vs_agc_status =
    rfcommon_rfnv_get_item( exp_hdet_vs_agc_item,
                            exp_hdet_vs_agc_buffer,
                            &exp_hdet_vs_agc_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );
  hdet_off_status =
    rfcommon_rfnv_get_item( hdet_off_item,
                            (rfnv_item_type*)&hdet_off_buffer,
                            &hdet_off_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );
  hdet_spn_status =
    rfcommon_rfnv_get_item( hdet_spn_item,
                            (rfnv_item_type*)&hdet_spn_buffer,
                            &hdet_spn_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( exp_hdet_vs_agc_status == NV_DONE_S )
  {
    hdet_vs_agc_data = (rfnv_tx_hdet_vs_agc*)exp_hdet_vs_agc_buffer;

    /* Expected HDET vs AGC data is valid */
    for ( i = 0; i < RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE; i++ )
    {
      lim_nv_struct->hpm_hdet.exp_hdet_vs_agc[i] =
              hdet_vs_agc_data->hdet_vs_agc[i];
    }
    /* Set HDET flag to enabled */
    lim_nv_struct->hpm_hdet.enabled = TRUE;

    retval = TRUE;
  }
  else if ( exp_hdet_vs_agc_status == NV_NOTACTIVE_S )
  {
    /* This is a condition under which HDET operation is
    disabled */
    lim_nv_struct->hpm_hdet.enabled = FALSE;

    /* Initialize the exp_hdet_vs_agc data to zero */
    memset( &(lim_nv_struct->hpm_hdet.exp_hdet_vs_agc), 0,
            sizeof(uint16)*RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE );

    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_hdet_cal: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  if ( hdet_off_status == NV_DONE_S && retval == TRUE )
  {
    /* Load HDET offset value */
    lim_nv_struct->hpm_hdet.offset = hdet_off_buffer.bc0_hdet_off_v2;

  }
  else if ( hdet_off_status == NV_NOTACTIVE_S && retval == TRUE )
  {
    /* Load zero HDET offset val if NV is inactive */
    lim_nv_struct->hpm_hdet.offset = 0;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_hdet_cal: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  if ( hdet_spn_status == NV_DONE_S && retval == TRUE )
  {
    /* Load HDET span value */
    lim_nv_struct->hpm_hdet.span = hdet_spn_buffer.bc0_hdet_spn_v2;
  }
  else if ( hdet_spn_status == NV_NOTACTIVE_S && retval == TRUE )
  {
    /* Load zero HDET span val if NV is inactive */
    lim_nv_struct->hpm_hdet.span = 0;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_hdet_cal: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  /* For Normal High Power Mode HDET, the start power level for the incremental
  exp_hdet_vs_AGC steps must be hardcoded based on the requested band */
  switch( band )
  {
  case RFM_CDMA_BC0:
  case RFM_CDMA_BC10:
    lim_nv_struct->hpm_hdet.start_pwr_lvl = 
                   RF_CDMA_NV_HPM_HDET_START_LVL_LOW_BANDS;
    break;
  case RFM_CDMA_BC1:
  case RFM_CDMA_BC4:
  case RFM_CDMA_BC6:
  case RFM_CDMA_BC14:
  case RFM_CDMA_BC15:
    lim_nv_struct->hpm_hdet.start_pwr_lvl =
                   RF_CDMA_NV_HPM_HDET_START_LVL_HIGH_BANDS;
    break;
  default:
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_load_tx_hdet_cal: "
              "Loading HDET High Power mode NV: Unsupported Band %d",
              band );
    /* Assign a default start power value */
    lim_nv_struct->hpm_hdet.start_pwr_lvl =
                   RF_CDMA_NV_HPM_HDET_START_LVL_LOW_BANDS;
    break;
  }

  modem_mem_free( exp_hdet_vs_agc_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_tx_hdet_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx HDET calibration data from NV for Low Power
  Mode HDET

  @details
  Checks that the HDET calibration for low Power mode items is
  loaded correctly from NV, and populates internal data structures with the
  correct device and band specific information.

  @param lim_nv_struct
  The Tx NV data structure that HDET calibration information must be loaded
  into

  @param lpm_hdet_item
  The NV item that contains Low Power Mode HDET data.
 
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
  TRUE if LPM HDET item is loaded correctly or is inactive,
  else FALSE if NV state is inconsistent.
*/
static
boolean
rf_cdma_nv_load_tx_lpm_hdet_cal
(
  rf_cdma_tx_lim_cal_type* const lim_nv_struct,
  const rfnv_item_id_enum_type lpm_hdet_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  nv_stat_enum_type     lpm_hdet_nv_status;
  rfnv_item_type        *lpm_hdet_buffer;
  rfnv_item_size_type   lpm_hdet_returned_size =
                                  sizeof(rfnv_item_type);
  rfnv_tx_lpm_hdet*     lpm_hdet_data;

  /* Data structure iterator */
  int i;

  lpm_hdet_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                     MODEM_MEM_CLIENT_RFA );
  if ( lpm_hdet_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_tx_lpm_hdet_cal: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  lpm_hdet_nv_status =
    rfcommon_rfnv_get_item( lpm_hdet_item,
                            lpm_hdet_buffer,
                            &lpm_hdet_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( lpm_hdet_nv_status == NV_DONE_S )
  {
    /* Low power mode HDET NV is active. Load all data
    corresponding to this mode of operation */
    lpm_hdet_data = (rfnv_tx_lpm_hdet*)lpm_hdet_buffer;

    /* LPM enabled/disabled */
    lim_nv_struct->lpm_hdet.enabled = lpm_hdet_data->enabled;
    /* LPM HDET offset */
    lim_nv_struct->lpm_hdet.offset = lpm_hdet_data->off;
    /* LPM HDET span */
    lim_nv_struct->lpm_hdet.span = lpm_hdet_data->spn;
    /* LPM Threshold */
    lim_nv_struct->lpm_threshold = lpm_hdet_data->thresh;

    /* Obtain the start power level for Low power mode HDET readings */
    lim_nv_struct->lpm_hdet.start_pwr_lvl = lpm_hdet_data->hdet_vs_agc[0];

    /* LPM hdet_vs_agc data */
    for( i = 0; i < RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE; i++ )
    {
      lim_nv_struct->lpm_hdet.exp_hdet_vs_agc[i] =
                      lpm_hdet_data->hdet_vs_agc[i+1];
    }

    retval = TRUE;
  }
  else if ( lpm_hdet_nv_status == NV_NOTACTIVE_S )
  {
    /* LPM HDET NV is inactive. Set the enabled flag to 0 and
    set NV values to default 0 */
    lim_nv_struct->lpm_hdet.enabled = FALSE;
    memset( &(lim_nv_struct->lpm_hdet), 0,
            sizeof(rf_cdma_tx_hdet_cal_type) );

    retval = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_tx_lpm_hdet_cal: "
            "NV calibration state is inconsistent" );
    retval = FALSE;
  }

  modem_mem_free( lpm_hdet_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_tx_lpm_hdet_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load Tx HDET/max power calibration data

  @details
  Checks whether HDET NV items for this device and band's calibration data
  are loaded. Loads lim vs. freq data, as well as normal (high power mode)
  and low power mode HDET data.

  @param cal_struct
  The Tx NV data structure that HDET calibration information must be loaded
  into

  @param band
  The band number for this NV item.  This is required to compute
  the Tx power in dBm10 from the NV item when loading the internal data
  structures.

  @param lim_vs_freq_item
  The NV item that contains Tx Lim vs Freq data.
 
  @param hdet_vs_freq_item
  The NV item that contains Tx HDET vs Freq data, which is the newer
  version of lim_vs_freq, but holds the same data
 
  @param exp_hdet_vs_agc_item
  The NV item that contains expected HDET vs AGC data.
 
  @param hdet_off_item
  The NV item that contains HDET offset.
 
  @param hdet_spn_item
  The NV item that contains HDET span.
 
  @param lpm_hdet_item
  The NV item that contains Low Power Mode HDET data.

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
  TRUE if lim_vs_freq data, and hdet calibration data for both power
  modes is initialized successfully, else FALSE.
*/
boolean
rf_cdma_nv_load_tx_lim_cal
(
  rf_cdma_tx_lim_cal_type* const cal_struct,
  const rfm_cdma_band_class_type band,
  const rfnv_item_id_enum_type lim_vs_freq_item,
  const rfnv_item_id_enum_type hdet_vs_freq_item,
  const rfnv_item_id_enum_type exp_hdet_vs_agc_item,
  const rfnv_item_id_enum_type hdet_off_item,
  const rfnv_item_id_enum_type hdet_spn_item,
  const rfnv_item_id_enum_type lpm_hdet_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval = TRUE;

  if ( retval == TRUE )
  {
    /* Try loading HDET vs Freq item */
    retval = rf_cdma_nv_load_tx_hdet_vs_freq( cal_struct,
                                              hdet_vs_freq_item,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );

    /* Fall back to Lim vs Freq item if above is unsuccessful */
    if ( retval == FALSE )
    {
      retval = rf_cdma_nv_load_tx_lim_freq_cal( cal_struct,
                                                lim_vs_freq_item,
                                                caller_tcb_ptr,
                                                task_nv_wait_sig,
                                                task_wait_func_ptr );
    }
  }

  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_tx_hdet_cal( cal_struct,
                                          band,
                                          exp_hdet_vs_agc_item,
                                          hdet_off_item,
                                          hdet_spn_item,
                                          caller_tcb_ptr,
                                          task_nv_wait_sig,
                                          task_wait_func_ptr );
  }

  if ( retval == TRUE )
  {
    retval = rf_cdma_nv_load_tx_lpm_hdet_cal( cal_struct,
                                              lpm_hdet_item,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );
  }

  return retval;
} /* rf_cdma_nv_load_tx_lim_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Intelliceiver calibration data

  @details
  Checks that the NV items for this device's and band's intelliceiver 
  calibration data are loaded in NV, and initializes the internal data 
  structures with this data if available. Intelliceiver data is 
  provided at reference channel and for all available power modes

  @param ic_struct
  Intelliceiver NV data structure
 
  @param ic_cal_nv_item
  The band-specific NV item that contains the intelliceiver calibration
  data at reference channel.

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
*/
boolean
rf_cdma_nv_load_intelliceiver_cal
(
  rf_cdma_intelliceiver_type* const ic_struct,
  const rfnv_item_id_enum_type ic_cal_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffer for intelliceiver cal data */
  nv_stat_enum_type          ic_cal_nv_status;
  rfnv_item_type             *ic_cal_nv_buffer;
  rfnv_item_size_type        ic_cal_nv_returned_size =
                                     sizeof(rfnv_item_type);
  rfnv_ic_cal_data_type*     ic_cal_data_ptr;

  /* Iterators */
  int i, j;

  ic_cal_nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                      MODEM_MEM_CLIENT_RFA );
  if ( ic_cal_nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_intelliceiver_cal: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  if ( ic_cal_nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_intelliceiver_cal: Memory Alloc "
                      "failed" );
    return FALSE;
  }

  ic_cal_nv_status = 
    rfcommon_rfnv_get_item( ic_cal_nv_item,
                            ic_cal_nv_buffer,
                            &ic_cal_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( ic_cal_nv_status == NV_DONE_S )
  {
    /* Load the calibration data into the pre-defined structure */
    ic_cal_data_ptr = (rfnv_ic_cal_data_type *)ic_cal_nv_buffer;

    ic_struct->ref_channel = ic_cal_data_ptr->channel;
    for ( i = RFM_CDMA_POWER_MODE_NORMAL;
          i < RFM_CDMA_POWER_MODE_MAX;
          i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_INTELLICEIVER_DET_COUNT; j++ )
      {
        ic_struct->calibration[i][j] = ic_cal_data_ptr->
                                             calibration[i][j];
      }
    }

    retval = TRUE;
  } /* if ( ic_cal_nv_status == NV_DONE_S ) */
  else if ( ic_cal_nv_status == NV_NOTACTIVE_S )
  {
    RF_MSG( RF_HIGH,
            "rf_cdma_nv_load_intelliceiver_cal: Calibration data inactive" );

    ic_struct->ref_channel = 0;
    for ( i = RFM_CDMA_POWER_MODE_NORMAL;
          i < RFM_CDMA_POWER_MODE_MAX;
          i++ )
    {
      for ( j = 0; j < RF_CDMA_NV_INTELLICEIVER_DET_COUNT; j++ )
      {
        ic_struct->calibration[i][j] = 0;
      }
    }

    retval = TRUE;
  } /* else if ( ic_cal_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_intelliceiver_cal: "
            "NV calibration state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( ic_cal_nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_intelliceiver_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Intelliceiver Jammer Detector Adjust Threshold NV

  @details
  Checks that the NV items for this device's and band's intelliceiver 
  jammer detector thresholds are loaded in NV, and initializes the
  internal data structure with this data if available.

  @param ic_struct
  Intelliceiver NV data structure
 
  @param ic_det_thresh_nv_item
  The band-specific NV item that contains the intelliceiver detector 
  adjust thresholds.

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
*/
boolean
rf_cdma_nv_load_intelliceiver_det_thresh
(
  rf_cdma_intelliceiver_type* const ic_struct,
  const rfnv_item_id_enum_type ic_det_thresh_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffer for intelliceiver jammer detector threshold data */
  nv_stat_enum_type              ic_det_thresh_nv_status;
  rfnv_item_type                 *ic_det_thresh_nv_buffer;
  rfnv_item_size_type            ic_det_thresh_nv_returned_size =
                                     sizeof(rfnv_item_type);
  rfnv_ic_det_adjust_data_type*  ic_det_thresh_nv_ptr;

  /* Iterators */
  int i, j;

  ic_det_thresh_nv_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                             MODEM_MEM_CLIENT_RFA );
  if ( ic_det_thresh_nv_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_intelliceiver_det_thresh: Could not "
                      "allocate buffer memory on heap" );
    retval = FALSE;
    return retval;
  }

  ic_det_thresh_nv_status = 
    rfcommon_rfnv_get_item( ic_det_thresh_nv_item,
                            ic_det_thresh_nv_buffer,
                            &ic_det_thresh_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( ic_det_thresh_nv_status == NV_DONE_S )
  {
    /* Load the calibration data into the pre-defined structure */
    ic_det_thresh_nv_ptr = (rfnv_ic_det_adjust_data_type *)
                                    ic_det_thresh_nv_buffer;

    for ( i = 0;
          i < RF_CDMA_NV_INTELLICEIVER_DET_COUNT;
          i++ )
    {
      for ( j = RFM_LNA_GAIN_STATE_0; 
            j < RFM_LNA_GAIN_STATE_NUM;
            j++ )
      {
        ic_struct->det_adjust[i][j] = ic_det_thresh_nv_ptr->adj_val[i][j];
      }
    }

    retval = TRUE;
  } /* if ( ic_det_thresh_nv_status == NV_DONE_S ) */
  else if ( ic_det_thresh_nv_status == NV_NOTACTIVE_S )
  {
    RF_MSG( RF_HIGH,
            "rf_cdma_nv_load_intelliceiver_det_thresh: "
            "Threshold Adjust NV inactive" );

    for ( i = 0;
          i < RF_CDMA_NV_INTELLICEIVER_DET_COUNT;
          i++ )
    {
      for ( j = RFM_LNA_GAIN_STATE_0; 
            j < RFM_LNA_GAIN_STATE_NUM;
            j++ )
      {
        ic_struct->det_adjust[i][j] = 0;
      }
    }

    retval = TRUE;
  } /* else if ( ic_det_thresh_nv_status == NV_NOTACTIVE_S ) */
  else
  {
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_intelliceiver_det_thresh: "
            "NV state is inconsistent" );

    retval = FALSE;
  }

  modem_mem_free( ic_det_thresh_nv_buffer, MODEM_MEM_CLIENT_RFA );
  return retval;
} /* rf_cdma_nv_load_intelliceiver_det_thresh() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the Enc BTF Value for any band.

  @details
  Loads the Enc BTF value from NV for the corresponding device and band. The
  baseband modulator contains a BTF counter that compensates for the
  delays in the phone.

  BTF is a critical parameter for Tx to work, so if it is not loaded, the
  function will return FALSE, and the band will be marked invalid in the
  driver.

  @param rf_1x_nv_ptr
  Pointer to the 1x NV data structure

  @param enc_btf_item
  The NV item that contains the Enc BTF Delay value.

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
  TRUE if the BTF NV item is loaded successfully or is inactive, FALSE
  otherwise.
*/
boolean
rf_cdma_nv_load_enc_btf_dly
(
  rf_1x_config_type* const rf_1x_nv_ptr,
  const rfnv_item_id_enum_type enc_btf_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     enc_btf_nv_status;
  nv_item_type          enc_btf_buffer;
  rfnv_item_size_type   enc_btf_returned_size =
                              sizeof(nv_item_type);

  /* Assume that loading will be successful, and set this to FALSE if
     something goes wrong. */
  boolean load_success = TRUE;

  enc_btf_nv_status =
    rfcommon_rfnv_get_item( enc_btf_item,
                            (rfnv_item_type*)&enc_btf_buffer,
                            &enc_btf_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( enc_btf_nv_status == NV_DONE_S )
  {
    /* Load the Enc BTF delay value for the current band into the
    NV data structure*/
     rf_1x_nv_ptr->enc_btf_delay = enc_btf_buffer.bc0_enc_btf;

  } /* if ( enc_btf_nv_status == NV_DONE_S ) */
  else if ( enc_btf_nv_status == NV_NOTACTIVE_S )
  {
    /* The BC NV item for BTF was not loaded successfully. Load a
    default typical value for BTF */
    rf_1x_nv_ptr->enc_btf_delay = RF_CDMA_NV_ENC_BTF_DELAY_DEFAULT;
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_enc_btf_dly: Uninitialized BTF NV" );

    /* If the BTF NV isn't set, it's not really safe to use the default,
       so this band should be marked as not valid to prevent 1x/HDR from
       trying to tune to it. */
    load_success = FALSE;
  }
  else
  {
    /* The NV state is not consistent. Return FALSE to indicate failure */
    rf_1x_nv_ptr->enc_btf_delay = 0;
    RF_MSG( RF_ERROR,
            "rf_cdma_nv_load_enc_btf_dly: NV state inconsistent" );
    load_success = FALSE;
  } /* else: if ( enc_btf_nv_status == NV_DONE_S ) */

  return load_success;
} /* rf_cdma_nv_load_enc_btf_dly() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load legacy Tx switchpoint values.

  @details
  Loads the PA Rise and Fall switchpoint values for the requested
  band and path. The values are loaded into the legacy NV tables that will
  be deprecated in the future, but are required for temporary switchpoint
  support. Chain 2 will currently use the same switchpoints as Chain 0
  with this update.
 
  @param band
  Band for which legacy PA switchpoints must be loaded
 
  @param tx_1x_ptr
  Pointer to the old NV table for the corresponding band

  @param pa_r1_rise_item
  The NV item that contains the PA R1 Rise value.
 
  @param pa_r1_fall_item
  The NV item that contains the PA R1 Fall value.
 
  @param pa_r2_rise_item
  The NV item that contains the PA R1 Rise value.
 
  @param pa_r2_fall_item
  The NV item that contains the PA R1 Fall value.

  @param pa_r3_rise_item
  The NV item that contains the PA R1 Rise value.
 
  @param pa_r3_fall_item
  The NV item that contains the PA R1 Fall value.
 
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
  TRUE always. If the Switchpoint NV is inactive, use the default
  values hardcoded in the old NV table.
*/
boolean
rf_cdma_nv_load_old_pa_swpts
(
  rfm_cdma_band_class_type band,
  rf_1x_tx_static_type* tx_1x_ptr, 
  const rfnv_item_id_enum_type pa_r1_rise_item,
  const rfnv_item_id_enum_type pa_r1_fall_item,
  const rfnv_item_id_enum_type pa_r2_rise_item,
  const rfnv_item_id_enum_type pa_r2_fall_item,
  const rfnv_item_id_enum_type pa_r3_rise_item,
  const rfnv_item_id_enum_type pa_r3_fall_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  nv_stat_enum_type     pa_swpt_nv_status;
  nv_item_type          pa_swpt_nv_buffer;
  rfnv_item_size_type   pa_swpt_nv_returned_size =
                              sizeof(nv_item_type);
  rfm_pa_gain_state_type pa_state;

  /* Set PA state 0 switchpoints to min power -70 dBm */
  tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_0] = -700;
  tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_0] = -700;

  /* PA R1 Rise/Fall */
  pa_swpt_nv_status =
    rfcommon_rfnv_get_item( pa_r1_rise_item,
                            (rfnv_item_type*)&pa_swpt_nv_buffer,
                            &pa_swpt_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_nv_status == NV_DONE_S )
  {
    /* Load the corresponding switchpoint */
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_1] =
      rf_cdma_covert_pa_switchpoint_to_dbm(
        (int16)pa_swpt_nv_buffer.bc0_pa_r1_rise, band);

  } /* if ( pa_swpt_nv_status == NV_DONE_S ) */
  else if ( pa_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* Do nothing. Allow default values in old table to hold good */
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_old_pa_swpts: Uninitialized PA Swpt NV. "
            "Using defaults" );
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_1] = 20;
  }

  pa_swpt_nv_status =
    rfcommon_rfnv_get_item( pa_r1_fall_item,
                            (rfnv_item_type*)&pa_swpt_nv_buffer,
                            &pa_swpt_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_nv_status == NV_DONE_S )
  {
    /* Load the corresponding switchpoint */
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_1] =
      rf_cdma_covert_pa_switchpoint_to_dbm(
        (int16)pa_swpt_nv_buffer.bc0_pa_r1_fall, band);

  } /* if ( pa_swpt_nv_status == NV_DONE_S ) */
  else if ( pa_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* Do nothing. Allow default values in old table to hold good */
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_old_pa_swpts: Uninitialized PA Swpt NV. "
            "Using defaults" );
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_1] = -20;
  }

  /* PA R2 Rise/Fall */
  pa_swpt_nv_status =
    rfcommon_rfnv_get_item( pa_r2_rise_item,
                            (rfnv_item_type*)&pa_swpt_nv_buffer,
                            &pa_swpt_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_nv_status == NV_DONE_S )
  {
    /* Load the corresponding switchpoint */
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_2] =
      rf_cdma_covert_pa_switchpoint_to_dbm(
        (int16)pa_swpt_nv_buffer.bc0_pa_r1_rise, band);

  } /* if ( pa_swpt_nv_status == NV_DONE_S ) */
  else if ( pa_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* Do nothing. Allow default values in old table to hold good */
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_old_pa_swpts: Uninitialized PA Swpt NV. "
            "Using defaults" );
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_2] = 120;
  }

  pa_swpt_nv_status =
    rfcommon_rfnv_get_item( pa_r2_fall_item,
                            (rfnv_item_type*)&pa_swpt_nv_buffer,
                            &pa_swpt_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_nv_status == NV_DONE_S )
  {
    /* Load the corresponding switchpoint */
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_2] =
      rf_cdma_covert_pa_switchpoint_to_dbm(
        (int16)pa_swpt_nv_buffer.bc0_pa_r1_fall, band);

  } /* if ( pa_swpt_nv_status == NV_DONE_S ) */
  else if ( pa_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* Do nothing. Allow default values in old table to hold good */
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_old_pa_swpts: Uninitialized PA Swpt NV. "
            "Using defaults" );
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_2] = 80;
  }

  /* PA R3 Rise/Fall */
  pa_swpt_nv_status =
    rfcommon_rfnv_get_item( pa_r3_rise_item,
                            (rfnv_item_type*)&pa_swpt_nv_buffer,
                            &pa_swpt_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_nv_status == NV_DONE_S )
  {
    /* Load the corresponding switchpoint */
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_3] =
      rf_cdma_covert_pa_switchpoint_to_dbm(
        (int16)pa_swpt_nv_buffer.bc0_pa_r1_rise, band);

  } /* if ( pa_swpt_nv_status == NV_DONE_S ) */
  else if ( pa_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* Do nothing. Allow default values in old table to hold good */
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_old_pa_swpts: Uninitialized PA Swpt NV. "
            "Using defaults" );
    tx_1x_ptr->voice_swpts.rise[RFM_PA_GAIN_STATE_3] = 390;
  }

  pa_swpt_nv_status =
    rfcommon_rfnv_get_item( pa_r3_fall_item,
                            (rfnv_item_type*)&pa_swpt_nv_buffer,
                            &pa_swpt_nv_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  if ( pa_swpt_nv_status == NV_DONE_S )
  {
    /* Load the corresponding switchpoint */
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_3] =
      rf_cdma_covert_pa_switchpoint_to_dbm(
        (int16)pa_swpt_nv_buffer.bc0_pa_r1_fall, band);

  } /* if ( pa_swpt_nv_status == NV_DONE_S ) */
  else if ( pa_swpt_nv_status == NV_NOTACTIVE_S )
  {
    /* Do nothing. Allow default values in old table to hold good */
    RF_MSG( RF_MED,
            "rf_cdma_nv_load_old_pa_swpts: Uninitialized PA Swpt NV. "
            "Using defaults" );
    tx_1x_ptr->voice_swpts.fall[RFM_PA_GAIN_STATE_3] = 390;
  }

  /* Set remaining PA states to max power 40 dBm */
  for ( pa_state = RFM_PA_GAIN_STATE_4; pa_state < RFM_PA_GAIN_STATE_NUM;
        pa_state++ )
  {
    tx_1x_ptr->access_swpts.fall[pa_state] = 400;
    tx_1x_ptr->access_swpts.rise[pa_state] = 400;
  }

  return TRUE;
} /* rf_cdma_nv_load_old_pa_swpts() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return NV table that is band-independent

  @details
  This function is the only accessor function to return the common NV table.

  @return
  Pointer to the common NV table
*/
rf_cdma_nv_common_items_type*
rf_cdma_nv_get_common_table
(
  void
)
{
  return (&rf_cdma_nv_common_tbl);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the 1x configuration data for a given path and band.

  @details
  This function will return a pointer to the rf_1x_config_type structure
  that contains the configuration data for the requested device when
  operating on the requested band.

  The function will return a NULL pointer if the device or band is not
  supported in the current system, either due to hardware limitations
  or NV loading failure.

  @param path
  An NV path.  NV paths do not necessarily correspond to logical RFM
  devices, and are numbered from 0..N where N is the maximum CDMA path
  that is defined in RFNV.

  @param band
  The CDMA band that the user wants the configuration data for.

  @return
  A pointer to a 1x configuration data structure that can be used to
  configure the modem hardware for the requested band and channel.

  @retval NULL
  Indicates the configuration data is uninitialized for the requested
  chain and band.
*/
const rf_1x_config_type*
rf_cdma_nv_get_1x_data
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  if ( path < RF_CDMA_NV_PATH_NUM && band < RFM_CDMA_MAX_BAND )
  {
    return rf_cdma_config_device_index[path].band_index[band].ptr_1x;
  }
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_get_1x_data: chain (%d) or band (%d) is out of bounds",
              path, band );
    return NULL;
  }
} /* rf_cdma_nv_get_1x_data() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the HDR configuration data for a given path and band.

  @details
  This function will return a pointer to the rf_hdr_config_type structure
  that contains the configuration data for the requested device when
  operating on the requested band.

  The function will return a NULL pointer if the device or band is not
  supported in the current system, either due to hardware limitations
  or NV loading failure.

  @param path
  An NV path.  NV paths do not necessarily correspond to logical RFM
  devices, and are numbered from 0..N where N is the maximum CDMA path
  that is defined in RFNV.

  @param band
  The CDMA band that the user wants the configuration data for.

  @return
  A pointer to a 1x configuration data structure that can be used to
  configure the modem hardware for the requested band and channel.

  @retval NULL
  Indicates the configuration data is uninitialized for the requested
  chain and band.
*/
const rf_hdr_config_type*
rf_cdma_nv_get_hdr_data
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  if ( path < RF_CDMA_NV_PATH_NUM && band < RFM_CDMA_MAX_BAND )
  {
    return rf_cdma_config_device_index[path].band_index[band].ptr_hdr;
  }
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_get_1x_data: chain (%d) or band (%d) is out of bounds",
              path, band );
    return NULL;
  }
} /* rf_cdma_nv_get_hdr_data() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns a pointer to a modifiable version of the common CDMA
  configuration data.

  @details
  This function will return a pointer to the rf_cdma_config_type
  structure that contains the configuration data for the requested
  device when operating on the requested band.

  The function will return a NULL pointer if the device or band is not
  supported in the current system, either due to hardware limitations
  or NV loading failure.

  This function is static to insure outside clients to not obtain
  a modifiable copy of the configuration data strucures.  Only
  internal NV routines should be modifying the data structure,
  in order to load from NV and performan calibration updates.

  @param chain
  An NV path.  NV paths do not necessarily correspond to logical RFM
  devices, and are numbered from 0..N where N is the maximum CDMA path
  that is defined in RFNV.

  @param band
  The CDMA band that the user wants the configuration data for.

  @return
  A pointer to a CDMA configuration data structure.   The contents of
  this structure may be modified.

  @retval NULL
  Indicates the configuration data is uninitialized for the requested
  chain and band.
*/
static
rf_cdma_config_type*
rf_cdma_nv_get_mutable_common_data
(
  rf_cdma_nv_path_type chain,
  rfm_cdma_band_class_type band
)
{
  if ( chain < RF_CDMA_NV_PATH_NUM && band < RFM_CDMA_MAX_BAND )
  {
    if (    rf_cdma_config_device_index[chain].band_index[band].ptr_1x != NULL
         && rf_cdma_config_device_index[chain].band_index[band].ptr_hdr != NULL
       )
    {
      if ( rf_cdma_config_device_index[chain].band_index[band].ptr_1x->cdma_common_ptr != 
           rf_cdma_config_device_index[chain].band_index[band].ptr_hdr->cdma_common_ptr )
      {
        RF_MSG_2( RF_ERROR,
                  "rf_cdma_nv_get_mutable_common_data: 1x and HDR data structures are inconsistent for chain %d, band %d",
                  chain, band );
        return NULL;
      }
      else
      {
        return rf_cdma_config_device_index[chain].band_index[band].ptr_1x->cdma_common_ptr;
      }
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_get_mutable_common_data: chain (%d) or band (%d) is out of bounds",
              chain, band );
    return NULL;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return band and path Specfic Calibration NV Table

  @details
  This function is the only accessor function to return the NV table for
  any RF Calibration NV items.

  @param band
  Band for which the NV table is requested
 
  @param nv_path
  RF NV path for which calibration items are requested

  @return
  NV table corresponding to the band and nv_path
*/
rf_cdma_cal_nv_items_map_type*
rf_cdma_nv_get_calibration_table
(
  rfm_cdma_band_class_type band,
  rf_cdma_nv_path_type nv_path
)
{
  rf_cdma_cal_nv_items_map_type *table_ptr = NULL; /* Pointer to NV Table */

  /* Based on Band call the Band specific func to get NV */
  switch( band )
  {
  case RFM_CDMA_BC0:
    table_ptr = rf_cdma_nv_get_bc0_calibration_table(nv_path);
    break;
  case RFM_CDMA_BC1:
    table_ptr = rf_cdma_nv_get_bc1_calibration_table(nv_path);
    break;
  case RFM_CDMA_BC4:
    table_ptr = rf_cdma_nv_get_bc4_calibration_table(nv_path);
    break;
  case RFM_CDMA_BC6:
    table_ptr = rf_cdma_nv_get_bc6_calibration_table(nv_path);
    break;
  case RFM_CDMA_BC10:
    table_ptr = rf_cdma_nv_get_bc10_calibration_table(nv_path);
    break;
  case RFM_CDMA_BC14:
    table_ptr = rf_cdma_nv_get_bc14_calibration_table(nv_path);
    break;
  case RFM_CDMA_BC15:
    table_ptr = rf_cdma_nv_get_bc15_calibration_table(nv_path);
    break;
  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_nv_get_calibration_table: "
                        "Not Supported for Band %d",
              band );
    table_ptr = NULL;
    break;
  }

  /* Return NV Table */
  return table_ptr;

} /* rf_cdma_nv_get_calibration_table */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return band Specfic Static NV Table

  @details
  This function is the only accessor function to return the NV table for
  any RF Static NV Table.

  @param band
  Band for which the NV table is requested
 
  @param nv_path
  RF NV path for which static items are requested

  @return
  NV table corresponding to the band
*/
rf_cdma_static_nv_items_map_type*
rf_cdma_nv_get_static_nv_table
(
  rfm_cdma_band_class_type band,
  rf_cdma_nv_path_type nv_path
)
{
  rf_cdma_static_nv_items_map_type *table_ptr = NULL; /* Pointer to NV Table */

  /* Based on Band call the Band specific func to get NV */
  switch( band )
  {
  case RFM_CDMA_BC0:
    table_ptr = rf_cdma_nv_get_bc0_static_nv_table(nv_path);
    break;
  case RFM_CDMA_BC1:
    table_ptr = rf_cdma_nv_get_bc1_static_nv_table(nv_path);
    break;
  case RFM_CDMA_BC4:
    table_ptr = rf_cdma_nv_get_bc4_static_nv_table(nv_path);
    break;
  case RFM_CDMA_BC6:
    table_ptr = rf_cdma_nv_get_bc6_static_nv_table(nv_path);
    break;
  case RFM_CDMA_BC10:
    table_ptr = rf_cdma_nv_get_bc10_static_nv_table(nv_path);
    break;
  case RFM_CDMA_BC14:
    table_ptr = rf_cdma_nv_get_bc14_static_nv_table(nv_path);
    break;
  case RFM_CDMA_BC15:
    table_ptr = rf_cdma_nv_get_bc15_static_nv_table(nv_path);
    break;
  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_nv_get_static_nv_table: "
                        "Not Supported for Band %d",
              band );
    table_ptr = NULL;
    break;
  }

  /* Return NV Table */
  return table_ptr;

} /* rf_cdma_nv_get_static_nv_table */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return band and path Specfic Calibration NV Table

  @details
  This function is the only accessor function to return the NV table for
  any RF Calibration NV items.

  @param band
  Band for which the NV table is requested
 
  @return
  NV table corresponding to the band and nv_path
*/
rfcommon_core_convert_freq_handler_type
rf_cdma_nv_get_chan_to_tx_freq_handler
(
  rfm_cdma_band_class_type band
)
{
  rfcommon_core_convert_freq_handler_type 
    chan_to_freq_handler = NULL;

  /* Based on Band, obtain the band-specific function pointer */
  switch( band )
  {
  case RFM_CDMA_BC0:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc0_convert_chan_to_tx_freq;
    break;
  case RFM_CDMA_BC1:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc1_convert_chan_to_tx_freq;
    break;
  case RFM_CDMA_BC4:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc4_convert_chan_to_tx_freq;
    break;
  case RFM_CDMA_BC6:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc6_convert_chan_to_tx_freq;
    break;
  case RFM_CDMA_BC10:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc10_convert_chan_to_tx_freq;
    break;
  case RFM_CDMA_BC14:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc14_convert_chan_to_tx_freq;
    break;
  case RFM_CDMA_BC15:
    chan_to_freq_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc15_convert_chan_to_tx_freq;
    break;
  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_nv_get_chan_to_tx_freq_handler: "
                        "Not Supported for Band %d",
              band );
    chan_to_freq_handler = NULL;
    break;
  }

  /* Return function pointer */
  return chan_to_freq_handler;

} /* rf_cdma_nv_get_chan_to_tx_freq_handler */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates the Rx gain calibration data in memory with a new value.

  @details
  During calibration, when a new gain offset reading is obtained,
  this function can be used to update the internal memory structures
  that store the gain cal values.

  The band must have been successfully configured in the config system,
  otherwise the update fails.  This requires the band class mask NV to
  be set for the path that is being configured.

  The channel to update must be in the current channel list, otherwise
  the update fails.  

  @param path
  The NV path to update.

  @param band
  The CDMA band class to update.

  @param channel
  The channel to update

  @param power_mode
  The power mode to update.

  @param gain_state
  The gain state to update.

  @param gain
  The new gain value.

  @return
  TRUE if the requested update could be done.  FALSE otherwise.
*/
boolean
rf_cdma_nv_update_gain_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band,
  int channel,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_state_type gain_state,
  int gain
)
{
  rf_cdma_config_type* cdma_config_ptr;
  int channel_index;

  /* First, try to find the common CDMA data structure for this path and band. */
  if (    rf_cdma_config_device_index[path].band_index[band].ptr_1x == NULL
       || rf_cdma_config_device_index[path].band_index[band].ptr_1x->cdma_common_ptr == NULL )
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_update_gain_cal: path %d band %d does not have a valid configuration structure",
              path, band );
    return FALSE;
  }
  cdma_config_ptr =
    rf_cdma_config_device_index[path].band_index[band].ptr_1x->cdma_common_ptr;

  for ( channel_index = 0; channel_index < NV_FREQ_TABLE_SIZ; channel_index++ )
  {
    if ( cdma_config_ptr->rx_cal.channel_list[channel_index] == channel )
    {
      break;
    }
  }
  if ( channel_index == NV_FREQ_TABLE_SIZ )
  {
    RF_MSG_3( RF_ERROR,
              "rf_cdma_nv_update_gain_cal: path %d band %d does not have channel %d in the chan list",
              path, band, channel );
    return FALSE;
  }

  if ( power_mode >= RFM_CDMA_POWER_MODE_MAX )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_update_gain_cal: Invalid power mode %d",
              power_mode );
    return FALSE;
  }
  if ( gain_state >= RFM_LNA_GAIN_STATE_NUM )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_update_gain_cal: Invalid gain state %d",
              gain_state );
    return FALSE;
  }

  cdma_config_ptr->rx_cal.rx_gain_calibration[power_mode]
                                             [gain_state]
                                             [channel_index] = 
              (int16)rf_limit_s32(gain, -32768, 32767);

  return TRUE;
} /* rf_cdma_nv_update_gain_cal() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the interpolated gain calibration value for the requested
  path, band, channel, and power mode.

  @details

  @param path
  The NV path to update.

  @param band
  The CDMA band class to update.

  @param channel
  The channel to update

  @param power_mode
  The power mode to update.

  @param gain_state
  The gain state to update.

  @return
  Signed 16-bit calibration value.
*/
int16
rf_cdma_nv_get_gain_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band,
  int channel,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_state_type gain_state
)
{
  const rf_cdma_config_type* cdma_config_ptr;
  int16 return_value;

  /* Gets a modifiable pointer, but stores it as a pointer to const
     to prevent changing anything. */
  cdma_config_ptr = rf_cdma_nv_get_mutable_common_data( path, band );
  if ( cdma_config_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_get_gain_cal: Path %d, band %d common NV structure isn't allocated.",
               path, band, 0 );
  }

  if ( power_mode >= RFM_CDMA_POWER_MODE_MAX )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_get_gain_cal: Invalid power mode %d",
              power_mode );
    return 0;
  }
  if ( gain_state >= RFM_LNA_GAIN_STATE_NUM )
  {
    RF_MSG_1( RF_ERROR,
              "rf_cdma_nv_get_gain_cal: Invalid gain state %d",
              gain_state );
    return 0;
  }

  if ( cdma_config_ptr->rx_cal.freq_list_size == 0 ||
       cdma_config_ptr->rx_cal.freq_list_size > RF_CDMA_NV_FREQ_LIST_SIZE )
  {
    /* If there is no calibration data at all or we have an
    invalid frequency list size, return 0. */
    return_value = 0;
  }
  else
  {
    return_value = 
      rf_lininterp_u32_s16( cdma_config_ptr->rx_cal.freq_list,
                            &cdma_config_ptr->
                              rx_cal.rx_gain_calibration[power_mode]
                                                        [gain_state]
                                                        [0],
                            cdma_config_ptr->rx_cal.freq_list_size,
                            rf_cdma_get_rx_carrier_freq(band, channel) );
  }

  return return_value;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns an immutable pointer to the tx calibration data.

  @details
  Tx calibration data is loaded into the common NV data structures. This 
  function returns an immutable pointer to the Tx data for access by modules 
  outside of NV.

  @param path
  The NV path to query.

  @param band
  The CDMA band class for which Tx cal data must be returned.

  @return
  Const pointer to the Tx cal data for the requested band.
*/
const
rf_cdma_tx_cal_type*
rf_cdma_nv_get_tx_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  const rf_cdma_config_type* cdma_config_ptr;

  /* Gets a modifiable pointer, but stores it as a pointer to const
     to prevent changing anything. */
  cdma_config_ptr = rf_cdma_nv_get_mutable_common_data( path, band );
  if ( cdma_config_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_get_gain_cal: Path %d, band %d common NV structure isn't allocated.",
               path, band, 0 );
  }

  return &cdma_config_ptr->tx_cal;
} /* rf_cdma_nv_get_tx_cal */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns an immutable pointer to the tx static calibration data.

  @details
  Tx static calibration data is loaded into the common NV data structures. This 
  function returns an immutable pointer to the Tx data for access by modules 
  outside of NV.

  @param path
  The NV path to query.

  @param band
  The CDMA band class for which Tx cal data must be returned.

  @return
  Const pointer to the Tx cal data for the requested band.
*/
const
rf_cdma_tx_static_type*
rf_cdma_nv_get_tx_static
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  const rf_cdma_config_type* cdma_config_ptr;

  /* Gets a modifiable pointer, but stores it as a pointer to const
     to prevent changing anything. */
  cdma_config_ptr = rf_cdma_nv_get_mutable_common_data( path, band );
  if ( cdma_config_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_get_tx_static: "
               "Path %d, band %d common NV structure isn't allocated.",
               path, band, 0 );
  }

  return &cdma_config_ptr->tx_static;
} /* rf_cdma_nv_get_tx_static */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns an immutable pointer to the tx multilinearizer calibration data.

  @details
  Tx calibration data is loaded into the common NV data structures. This 
  function returns an immutable pointer to the Tx data for access by modules 
  outside of NV.

  @param path
  The NV path to query.

  @param band
  The CDMA band class for which Tx cal data must be returned.

  @return
  Const pointer to the Tx cal data for the requested band.
*/
const
tx_band_cal_type*
rf_cdma_nv_get_tx_multilin_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  const rf_cdma_config_type* cdma_config_ptr;

  /* Gets a modifiable pointer, but stores it as a pointer to const
     to prevent changing anything. */
  cdma_config_ptr = rf_cdma_nv_get_mutable_common_data( path, band );
  if ( cdma_config_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_get_tx_multilin_cal: "
               "Path %d, band %d common NV structure isn't allocated.",
               path, band, 0 );
  }

  return &cdma_config_ptr->tx_cal.tx_multi_lin_cal;
} /* rf_cdma_nv_get_tx_multilin_cal */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Report the NV validity check that was done when loading NV for the
  requested path and band.
 
  @details
  Checks if 1x and HDR config structures have been allocated, and if so,
  if the initial sanity checks that were done on NV when it was loaded
  resulted in the NV being marked as valid.
 
  @param path
  An NV path.  NV paths do not necessarily correspond to logical RFM
  devices, and are numbered from 0..N where N is the maximum CDMA path
  that is defined in RFNV.

  @param band
  The CDMA band that the user wants the configuration data for.

  @return
  Indicates if the NV has passed an initial sanity check.
*/
boolean
rf_cdma_nv_device_data_is_valid
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  if ( path < RF_CDMA_NV_PATH_NUM && band < RFM_CDMA_MAX_BAND )
  {
    if ( rf_cdma_config_device_index[path].band_index[band].ptr_1x == NULL ||
         rf_cdma_config_device_index[path].band_index[band].ptr_hdr == NULL )
    {
      /* No structure allocated, then this band wasn't defined in the
         BC_MASK NV item.  It's reasonable to return FALSE with no extra
         information. */
      return FALSE;
    }

    if ( rf_cdma_config_device_index[path].band_index[band].data_valid == FALSE )
    {
      /* If there were NV structures allocated, but the validity checks
         failed, then it was expected that this band should work.  Report
         a message indicating what NV we think caused the problem. */
      RF_MSG_3( RF_HIGH,
                "rf_cdma_nv_device_data_is_valid() reports FALSE for chain %d, band %d due to item %d",
                path, band, rf_cdma_config_device_index[path].band_index[band].last_nv_item_read );
      return FALSE;
    }

    return TRUE;
  }
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_device_data_is_valid: chain (%d) or band (%d) is out of bounds",
              path, band );
    return FALSE;
  }
} /* rf_cdma_nv_device_data_is_valid() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Spur table for 1x

  @param rx_1x_ptr
  Reference to the HDR static items Rx data structure.
 
  @param spur_table_nv_item
  The NV item that contains the spur table info

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
  TRUE if the spur info is valid and the NV item is valid,
  otherwise FALSE.
*/
boolean
rf_cdma_nv_load_1x_spur_info
(
  rf_1x_rx_static_type *rx_1x_ptr,
  const rfnv_item_id_enum_type spur_table_nv_item,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffer for 1X spur table info */
  nv_stat_enum_type        spur_table_nv_status;
  rfnv_item_type           *spur_table_info_buffer;
  rfnv_item_size_type      spur_tbl_returned_size = sizeof(rfnv_item_type);
  rfnv_cdma_spur_info_type *spur_table_nv_ptr;

  spur_table_info_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );

  if ( spur_table_info_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_1x_spur_info: Could not "
                      "allocate buffer memory on heap" );
    return FALSE;
  }

  spur_table_nv_status =
    rfcommon_rfnv_get_item( spur_table_nv_item,
                            spur_table_info_buffer,
                            &spur_tbl_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  /* start with good value */
  retval = TRUE;

  if ( spur_table_nv_status == NV_DONE_S )
  {
    int tbl_entry;

    /* If spur table read successfully, load the data structure */
    spur_table_nv_ptr = (rfnv_cdma_spur_info_type *)spur_table_info_buffer;
    
    for ( tbl_entry = 0; tbl_entry < RFCOMMON_MAX_SPUR_ENTRIES; tbl_entry++ )
    {
      rx_1x_ptr->onex_spur_table.rfcommon_spur_tbl[tbl_entry].spur_at_freq_hz = 
                           spur_table_nv_ptr->spur_entry[tbl_entry].spur_at_freq_hz;

      rx_1x_ptr->onex_spur_table.rfcommon_spur_tbl[tbl_entry].notch_depth = 
                         spur_table_nv_ptr->spur_entry[tbl_entry].notch_depth;
    }

  }
  else if ( spur_table_nv_status == NV_NOTACTIVE_S )
  {
    /* Initialize the spur table data to zero */           
    memset(&(rx_1x_ptr->onex_spur_table), 0, sizeof(rfcommon_spur_table_type));   
  }  
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_1x_spur_info: NV state is inconsistent: "
              "Spur Table Item %d status %d",
              spur_table_nv_item, spur_table_nv_status );

    retval = FALSE;
  }

  modem_mem_free( spur_table_info_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;

} /* rf_cdma_nv_load_1x_spur_info */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to load Spur table for HDR

  @param rx_hdr_ptr
  Reference to the HDR static items Rx data structure.
 
  @param spur_table_nv_item
  The NV item that contains the spur table info

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
  TRUE if the spur info is valid and the NV item is valid,
  otherwise FALSE.

*/
boolean
rf_cdma_nv_load_hdr_spur_info
(
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type spur_table_nv_item,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean retval;

  /* Buffers for HDR Rev0/RevA Rx switchpoint items */
  nv_stat_enum_type        spur_table_nv_status;
  rfnv_item_type           *spur_table_info_buffer;
  rfnv_item_size_type      spur_tbl_returned_size = sizeof(rfnv_item_type);
  rfnv_cdma_spur_info_type *spur_table_nv_ptr;

  spur_table_info_buffer = modem_mem_alloc( sizeof(rfnv_item_type),
                                    MODEM_MEM_CLIENT_RFA );

  if ( spur_table_info_buffer == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_nv_load_hdr_spur_info: Could not "
                      "allocate buffer memory on heap" );
    return FALSE;
  }

  spur_table_nv_status =
    rfcommon_rfnv_get_item( spur_table_nv_item,
                            spur_table_info_buffer,
                            &spur_tbl_returned_size,
                            caller_tcb_ptr,
                            task_nv_wait_sig,
                            task_wait_func_ptr );

  /* start with good value */
  retval = TRUE;

  if ( spur_table_nv_status == NV_DONE_S )
  {
    int tbl_entry;

    /* If spur table read successfully, load the data structure */
    spur_table_nv_ptr = (rfnv_cdma_spur_info_type *)spur_table_info_buffer;
    
    for ( tbl_entry = 0; tbl_entry < RFCOMMON_MAX_SPUR_ENTRIES; tbl_entry++ )
    {
      rx_hdr_ptr->hdr_spur_table.rfcommon_spur_tbl[tbl_entry].spur_at_freq_hz = 
                           spur_table_nv_ptr->spur_entry[tbl_entry].spur_at_freq_hz;

      rx_hdr_ptr->hdr_spur_table.rfcommon_spur_tbl[tbl_entry].notch_depth = 
                         spur_table_nv_ptr->spur_entry[tbl_entry].notch_depth;
    }

  }
  else if ( spur_table_nv_status == NV_NOTACTIVE_S )
  {
    /* Initialize the spur table data to zero */           
    memset(&(rx_hdr_ptr->hdr_spur_table), 0, sizeof(rfcommon_spur_table_type));   
  }  
  else
  {
    RF_MSG_2( RF_ERROR,
              "rf_cdma_nv_load_hdr_spur_info: NV state is inconsistent: "
              "Spur Table Item %d status %d",
              spur_table_nv_item, spur_table_nv_status );

    retval = FALSE;
  }

  modem_mem_free( spur_table_info_buffer, MODEM_MEM_CLIENT_RFA );

  return retval;

} /* rf_cdma_nv_load_hdr_spur_info */

/*============================================================================*/
/*!
  @name EFS

  @brief
  This section includes list of functions used to write NV
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write Single NV Item

  @details
  This function will perform single NV write under cuurent Task Context. The
  specified NV item is updated if sucessful.

  @param nv_item
  NV Item to be written

  @param *data_ptr
  NV write Data

  @return
  NV Write Status
*/
boolean
rf_cdma_nv_write
(
  rfnv_item_id_enum_type nv_item,
  void *data_ptr
)
{
  nv_cmd_type nv_write_cmd; /* FTM NV Command */

  /* Prepare the NV write command buffer. */
  nv_write_cmd.item       = (nv_items_enum_type)nv_item; /* item to write */
  nv_write_cmd.cmd        = NV_WRITE_F; /* NV operation */
  nv_write_cmd.data_ptr   = data_ptr; /* buffer to store write data  */
  nv_write_cmd.tcb_ptr    = rex_self(); /* Notify this task when done */
  nv_write_cmd.sigs       = FS_OP_COMPLETE_SIG; /* Signal this when done */
  nv_write_cmd.done_q_ptr = NULL; /* Return cmd to NO Q when done */

  /* Write to NV by:
  1. Clearing the NV signal.
  2. Queuing the write command to the NV task.
  3. Waiting for the NV task to finish servicing the write command. Ensure that
     watchdog is being hit during this synchronous operation. */
  (void) rex_clr_sigs( rex_self(), nv_write_cmd.sigs );
  
  nv_cmd( &nv_write_cmd );

  /* If NV writes hang, the FTM task should indicate this with a task dog 
  timeout. Therefore, do not use ftm_sig_rex_wait, which will incorrectly send a 
  report periodically */
  (void) rex_wait(FS_OP_COMPLETE_SIG);

  if (nv_write_cmd.status != NV_DONE_S)
  {
    RF_MSG_2( RF_ERROR, "FTM error writing NV: item %d, status %d",
              nv_write_cmd.item, nv_write_cmd.status );
  }

  /* Return NV Command Status */
  if ( nv_write_cmd.status == NV_DONE_S )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* rf_cdma_nv_write */

/*! \} */


/*============================================================================*/
/*!
  @name Data Structure Update

  @brief
  This section includes list of functions used to update internal NV data
  structures.
*/
/*! \{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Copies RF RX NV from channel index i+1 into index i

  @details
  Copies RF RX NV from channel index i+1 into index i. RF RX NV is modified,
  and may be inconsistent until the caller has finished the entire move
  operation.

  @param cdma_cfg_ptr
  Pointer to CDMA config data structure containing the calibration data
  to adjust.

  @param path
  RF Path on which Channeling is to be done

  @param index
  Index
*/
static void
rf_cdma_nv_move_next_down
(
  rf_cdma_config_type* cdma_cfg_ptr,
  rf_cdma_nv_path_type path,
  int32 index
)
{
  rfm_cdma_power_mode_type power_mode;
  
  if ( !(index >= 0 && index < NV_FREQ_TABLE_SIZ-1) )
  {
    ERR_FATAL( "Inavlid Index" ,0 ,0, 0 );
  }

  cdma_cfg_ptr->rx_cal.channel_list[index] = 
    cdma_cfg_ptr->rx_cal.channel_list[index+1];
  cdma_cfg_ptr->rx_cal.lru_channel_list[index] =
    cdma_cfg_ptr->rx_cal.lru_channel_list[index+1];

  for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
        power_mode < RFM_CDMA_POWER_MODE_MAX; power_mode++ )
  {
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][index+1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][index+1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][index+1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][index+1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_4][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_4][index+1];
  }
} /* rf_cdma_nv_move_next_down */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Copies RF RX NV from channel index i-1 into index i

  @details
  Copies RF RX NV from channel index i-1 into index i. RF RX NV is modified,
  and may be inconsistent until the caller has finished the entire move
  operation.

  @param cdma_cfg_ptr
  Pointer to CDMA config data structure containing the calibration data
  to adjust.

  @param path
  RF Path on which Channeling is to be done

  @param index
  Index
*/
static void
rf_cdma_nv_move_prev_up
(
  rf_cdma_config_type* cdma_cfg_ptr,
  rf_cdma_nv_path_type path,
  int32 index
)
{
  rfm_cdma_power_mode_type power_mode;
  
  if ( !(index >= 1 && index < NV_FREQ_TABLE_SIZ) )
  {
    ERR_FATAL( "Inavlid Index" ,0 ,0, 0 );
  }


  cdma_cfg_ptr->rx_cal.channel_list[index] = 
    cdma_cfg_ptr->rx_cal.channel_list[index-1];
  cdma_cfg_ptr->rx_cal.lru_channel_list[index] =
    cdma_cfg_ptr->rx_cal.lru_channel_list[index-1];

  for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
        power_mode < RFM_CDMA_POWER_MODE_MAX;
        power_mode++ )
  {
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][index-1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][index-1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][index-1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][index-1];
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_4][index]
       = cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_4][index-1];
  }
} /* rf_cdma_nv_move_prev_up */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes a frequency calibration item

  @details

  Initializes a frequency calibration item at the specified index. Increments 
  all LRU items and sets the specified entry to the tail. Any data that must be 
  saved in the current entry must have already been saved.

  @param cdma_cfg_ptr
  Pointer to CDMA config data structure containing the calibration data
  to adjust.

  @param path
  RF Path on which Channeling is to be done

  @param index
  Index

  @param channel
  RF Channel
*/
static void
rf_cdma_nv_create_blank
(
  rf_cdma_config_type* cdma_cfg_ptr,
  rf_cdma_nv_path_type path,
  int32 index,
  uint16 channel
)
{
  int32 j;
  rfm_cdma_power_mode_type power_mode;
   
  if ( !(index >= 0 && index < NV_FREQ_TABLE_SIZ) )
  {
    ERR_FATAL( "Inavlid Index" ,0 ,0, 0 );
  }

  /* Increment all LRU tracking entries, the new item will be going
  at the end of the list. */
  for (j = 0; j < NV_FREQ_TABLE_SIZ; j++)
  {
    /* Stop if the list ends prematurely */
    if (cdma_cfg_ptr->rx_cal.lru_channel_list[j] == 0)
    {
      break;
    }
    cdma_cfg_ptr->rx_cal.lru_channel_list[j]++;
  }

  cdma_cfg_ptr->rx_cal.channel_list[index] = channel;
  cdma_cfg_ptr->rx_cal.lru_channel_list[index] = 1;

  for ( power_mode = RFM_CDMA_POWER_MODE_NORMAL;
        power_mode < RFM_CDMA_POWER_MODE_MAX;
        power_mode++ )
  {
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_0][index] = 0;
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_1][index] = 0;
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_2][index] = 0;
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_3][index] = 0;
    cdma_cfg_ptr->rx_cal.rx_gain_calibration[power_mode][RFM_LNA_GAIN_STATE_4][index] = 0;
  }
} /* rf_cdma_nv_create_blank */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Add a calibration channel

  @details
  Add a calibration channel to the internal RF NV data structures.
  If the requested channel is already in the calibration channel list, the
  correct index is returned.  If it is not present, a new entry is created
  and the location of the new entry is returned.

  RF calibration items may be re-arranged or lost, depending on the
  requested channel.

  @param nv_path
  RF NV path on which channeling is to be done
 
  @param band
  RF  Band to be added
 
  @param chan
  RF Channel to be added

  @return
  Index into the rfnv_digital_items_type.rfnv_rx_cal_chan array for the
  requested channel.
*/
int32
rf_cdma_nv_add_cal_chan
(
  rf_cdma_nv_path_type nv_path,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  int32 insert_index, delete_index, idx; /* Counters for Loop */
  rf_cdma_config_type* cdma_cfg_ptr;

  cdma_cfg_ptr = rf_cdma_nv_get_mutable_common_data( nv_path, band );
  if ( cdma_cfg_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_add_cal_chan: Cannot calibrate path %d, band %d because NV structure isn't allocated.  Insure NV_RF_BC_CONFIG_I and NV_RF_BC_CONFIG_DIV_I are set correctly.",
               nv_path, band, 0 );
  }

  if (    nv_path > RF_CDMA_NV_PATH_MAX
       || chan == 0
       || rf_cdma_get_rx_carrier_freq(band, chan) == 0 )
  {
    ERR_FATAL( "Bad parameters : Path - %d, Band - %d,  Channel - %d)",
               nv_path, band, chan );
  }

  /* Find the insertion index, the first channel bin in the list
  where the channel is larger (or equal in the event of a match)
  than the inserting channel */
  insert_index = 0;
  while ( insert_index < NV_FREQ_TABLE_SIZ &&
          cdma_cfg_ptr->rx_cal.channel_list[insert_index] != 0 &&
          cdma_cfg_ptr->rx_cal.lru_channel_list[insert_index] != 0 &&
          rf_cdma_get_rx_carrier_freq(band, chan) >
          rf_cdma_get_rx_carrier_freq(band, cdma_cfg_ptr->rx_cal.channel_list[insert_index])
        )
  {
    insert_index++;
  }

  /* If the channel at the insertion index matches the requested channel
  there is nothing to do, return the index and do not modify NV structures.
  Special case where insert_index == NV_FREQ_TABLE_SIZ which means we
  searched the entire list and didn't find a larger channel. */
  if ( insert_index < NV_FREQ_TABLE_SIZ &&
       chan ==
       cdma_cfg_ptr->rx_cal.channel_list[insert_index] &&
       cdma_cfg_ptr->rx_cal.lru_channel_list[insert_index] != 0 )
  {
    /* Update the LRU list if the current channel is not already at
    the tail of the list */
    if (cdma_cfg_ptr->rx_cal.lru_channel_list[insert_index] != 1)
    {
      for (idx = 0; idx < NV_FREQ_TABLE_SIZ; idx++)
      {
        /* Stop if the list ends prematurely */
        if (cdma_cfg_ptr->rx_cal.lru_channel_list[idx] == 0)
        {
          break;
        }
        
        /* Increment the LRU entries that are lower than the one that moves */
        if ( cdma_cfg_ptr->rx_cal.lru_channel_list[idx] <
             cdma_cfg_ptr->rx_cal.lru_channel_list[insert_index] )
        {
          cdma_cfg_ptr->rx_cal.lru_channel_list[idx]++;
        }
      }
      /* And move the current entry to the tail of the list */
      cdma_cfg_ptr->rx_cal.lru_channel_list[insert_index] = 1;
    }
    return insert_index;
  }

  /* Past here, a matching channel in the RX cal list was not found.
  A new entry will need to be inserted */

  /* Find the least recently used calibration channel, or an unused
  one if it exists */
  delete_index = 0;
  while ( delete_index < NV_FREQ_TABLE_SIZ &&
          cdma_cfg_ptr->rx_cal.lru_channel_list[delete_index] != 16 &&
          cdma_cfg_ptr->rx_cal.lru_channel_list[delete_index] != 0 )
  {
    delete_index++;
  }

  if (delete_index < insert_index)
  {
    /*
      The deletion occurs earler than the insertion:

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  | X3X |  4  |  5  |  6  |  7  |  8  |
      |     |     | XXX |     |     |     |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
       \   Stay  /  Del  \  Down   /^\    Stay       /
                                    | Insert    

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  |  4  |  5  | new |  6  |  7  |  8  |
      |     |     |     |     |     |     |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
                              ^
    */

    for (idx = delete_index; idx < insert_index - 1; idx++)
    {
      rf_cdma_nv_move_next_down(cdma_cfg_ptr, nv_path, idx);
    }
    insert_index = insert_index - 1;
    rf_cdma_nv_create_blank( cdma_cfg_ptr, nv_path, insert_index,
                             chan);
  }
  else
  {
    /*
      The deletion occurs later than the insertion:

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  |  3  |  4  |  5  | X6X |  7  |  8  |
      |     |     |     |     |     | XXX |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
       \   Stay  /^\      Up       /  Del  \  Stay   /
                  | Insert

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  | new |  3  |  4  |  5  |  7  |  8  |
      |     |     |     |     |     |     |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
                  ^
    */

    for (idx = delete_index; idx > insert_index; idx--)
    {
      rf_cdma_nv_move_prev_up(cdma_cfg_ptr, nv_path, idx);
    }

    rf_cdma_nv_create_blank(cdma_cfg_ptr, nv_path, insert_index,
                            chan);
  }

  return insert_index;

} /* rf_cdma_nv_add_cal_chan */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return function pointer for the corresponding band

  @details
  
  @param band
  Band for which the function pointer is requested
 
  @return
  Function Pointer for the band
*/
rfcommon_core_convert_chan_handler_type
rf_cdma_nv_get_tx_freq_to_chan_handler
(
  uint8 band
)
{
  rfcommon_core_convert_chan_handler_type 
    freq_to_chan_handler = NULL;

  /* Based on Band, obtain the band-specific function pointer */
  switch( band )
  {
  case RFM_CDMA_BC0:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc0_convert_tx_freq_to_chan;
    break;
  case RFM_CDMA_BC1:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc1_convert_tx_freq_to_chan;
    break;
  case RFM_CDMA_BC4:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc4_convert_tx_freq_to_chan;
    break;
  case RFM_CDMA_BC6:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc6_convert_tx_freq_to_chan;
    break;
  case RFM_CDMA_BC10:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc10_convert_tx_freq_to_chan;
    break;
  case RFM_CDMA_BC14:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc14_convert_tx_freq_to_chan;
    break;
  case RFM_CDMA_BC15:
    freq_to_chan_handler = (rfcommon_core_convert_freq_handler_type)rf_cdma_nv_bc15_convert_tx_freq_to_chan;
    break;
  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_nv_get_tx_freq_to_chan_handler: "
                        "Not Supported for Band %d", band );
    freq_to_chan_handler = NULL;
    break;
  }

  /* Return function pointer */
  return freq_to_chan_handler;

} /* rf_cdma_nv_get_tx_freq_to_chan_handler */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns an mutable pointer to the tx multilinearizer calibration data.

  @details
  Tx calibration data is loaded into the common NV data structures. This 
  function returns an mutable pointer to the Tx data for access by modules 
  outside of NV.

  @param path
  The NV path to query.

  @param band
  The CDMA band class for which Tx cal data must be returned.

  @return
  Pointer to the Tx cal data for the requested band.
*/
tx_band_cal_type*
rf_cdma_nv_get_mutable_tx_multilin_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  rf_cdma_config_type* cdma_config_ptr;

  /* Gets a modifiable pointer, stores it as a pointer. */
  cdma_config_ptr = rf_cdma_nv_get_mutable_common_data( path, band );
  if ( cdma_config_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_get_tx_multilin_cal: "
               "Path %d, band %d common NV structure isn't allocated.",
               path, band, 0 );
  }

  return &(cdma_config_ptr->tx_cal.tx_multi_lin_cal);
} /* rf_cdma_nv_get_mutable_tx_multilin_cal */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns an mutable pointer to the tx multilinearizer calibration data.

  @details
  Pin Cal data is loaded into the common NV data structures. This 
  function returns an mutable pointer to the Tx data for access by modules 
  outside of NV.

  @param path
  The NV path to query.

  @param band
  The CDMA band class for which Tx cal data must be returned.

  @return
  Pointer to the Pin Cal data for the requested band.
*/
rfcommon_core_txlin_pin_cal_vs_freq_type*
rf_cdma_nv_get_mutable_tx_pin_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
)
{
  rf_cdma_config_type* cdma_config_ptr;

  /* Gets a modifiable pointer, stores it as a pointer. */
  cdma_config_ptr = rf_cdma_nv_get_mutable_common_data( path, band );
  if ( cdma_config_ptr == NULL )
  {
    ERR_FATAL( "rf_cdma_nv_get_mutable_tx_pin_cal: "
               "Path %d, band %d common NV structure isn't allocated.",
               path, band, 0 );
  }

  return &(cdma_config_ptr->tx_static.lin_temp_comp_data.pin_cal_vs_freq);
} /* rf_cdma_nv_get_mutable_tx_pin_cal */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the Tx multilin global structure and NV Id based on Tx Int Cal band type

  @details
  This function gets the corresponding global TxLin data structure.
  It also returns the NV Id for the multilin Tx item for the corresponding band
 
  @param
  band - The band for which Tx Int Cal was done and results are returned
 
  @param
  device - The device for which Tx Int Cal was done and results are
           returned
  @param
  item_code - Argument for filling up the NV ID of multi lin cal data
 
  @param
  tx_band_cal_data - Argument to hold the address of the global Multi Lin Tx
                     structure
 
  @return
    TRUE or FALSE
*/
boolean
rf_cdma_nv_get_txlin_cal_data
(
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code,
  tx_band_cal_type **tx_band_cal_data
)
{ 
  rfm_cdma_band_class_type rfcom_cdma_band;
  rf_cdma_cal_nv_items_map_type *cal_nv_tbl = NULL; /* Pointer to Cal NV */

  /* Get the rfcom cdma band */
  rfcom_cdma_band = (rfm_cdma_band_class_type)band;
  RF_MSG_1( RF_HIGH, "Tx Internal Cal : RFCOM CDMA Band B%d", rfcom_cdma_band );
  if( rfcom_cdma_band == RFM_CDMA_MAX_BAND )
  {
      RF_MSG_1( RF_ERROR,"Incorrect band selected for CDMA Do Int Dev Cal", 0);
      return FALSE;
  }

  *tx_band_cal_data = rf_cdma_nv_get_mutable_tx_multilin_cal( (rf_cdma_nv_path_type)device, 
                                                              rfcom_cdma_band );
    
  /* Get Table for Calibration NV items */
  cal_nv_tbl = rf_cdma_nv_get_calibration_table( rfcom_cdma_band,
                                                 (rf_cdma_nv_path_type)device );
  /* KW fix*/
  if (cal_nv_tbl == NULL)
   {
      RF_MSG_1( RF_ERROR, "Tx Internal Cal Fail, NULL pointer returned for nv tbl ", 0);
      return FALSE; 
   }
  *item_code = cal_nv_tbl->multi_lin;
  RF_MSG_1( RF_HIGH, "Tx Internal Cal, Item Code : %d", *item_code );

  return TRUE;   
} /* rf_cdma_nv_get_txlin_cal_data */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the HDET Vs AGC NV ID

  @details
  This function returns the corresponding HDET Vs AGC NV Id.
 
  @param
  band - The band for which Tx Int Cal was done and results are returned
 
  @param
  device - The device for which Tx Int Cal was done and results are
           returned
  @param
  item_code - Argument for filling up the NV ID of HDET Vs AGC NV
 
  @return
    TRUE or FALSE
*/
boolean
rf_cdma_nv_get_hdet_vs_agc_nvid
(
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code
)
{ 
  rfm_cdma_band_class_type rfcom_cdma_band;
  rf_cdma_cal_nv_items_map_type *cal_nv_tbl = NULL; /* Pointer to Cal NV */

  /* Get the rfcom cdma band */
  rfcom_cdma_band = (rfm_cdma_band_class_type)band;
  RF_MSG_1( RF_HIGH, "Tx Internal Cal : RFCOM CDMA Band B%d", rfcom_cdma_band );
  if( rfcom_cdma_band == RFM_CDMA_MAX_BAND )
  {
      RF_MSG_1( RF_ERROR,"Incorrect band selected for CDMA Do Int Dev Cal", 0);
      return FALSE;
  }

  /* Get Table for Calibration NV items */
  cal_nv_tbl = rf_cdma_nv_get_calibration_table( rfcom_cdma_band,
                                                 (rf_cdma_nv_path_type)device );

  if (cal_nv_tbl == NULL)
   {
      RF_MSG_1( RF_ERROR, "Tx Internal Cal Fail, NULL pointer returned for nv tbl ", 0);
      return FALSE; 
   }
  *item_code = cal_nv_tbl->hpm_exp_hdet_vs_agc;
  RF_MSG_1( RF_HIGH, "Tx Internal Cal, Item Code : %d", *item_code );

  return TRUE;   
} /* rf_cdma_nv_get_txlin_cal_data */
#endif /* FEATURE_CDMA1X */
/*! \} */

/*! @} */
