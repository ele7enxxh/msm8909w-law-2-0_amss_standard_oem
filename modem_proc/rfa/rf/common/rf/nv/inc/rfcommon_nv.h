
#ifndef RFCOMMON_NV_H
#define RFCOMMON_NV_H
/*===========================================================================

                             R F   N V   M a n a g e r

DESCRIPTION
  This header file contains the data structure and function declarations to interface with
  the RF NV Manager.

Copyright (c) 1997 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/nv/inc/rfcommon_nv.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/16   cv      Adding support for RX Static NV
10/28/14   ac      function to retreive nv tuner items for DBDC
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
08/14/14    pv     Added support for PA swp adjust mechanism.
07/08/14   cd      Remove erroneous HDR RRI6 entry from PA static NV support
05/27/14   pl      Adding support for Common RX NV
05/07/14   npi     Enable notch threshold NV support
05/02/14   cd      8-PA state updates
12/16/13   aa      Move fbrx related struct and api's to rfcommon_fbrx_nv.h file 
12/05/13   aa      updated struct rfcommon_nv_fbrx_gain_vs_freq_type_v2
10/16/13   vs      Added support for tuner LTE CA NV
11/21/13   aa      Moved FBRX Cal NV retrieve api to rfcommon_fbrx_nv.h 
11/19/13   aa      Added api to retrieve fbrx_gain_nv_item per tech per band
11/12/13   aa      Added support for RFNV_DATA_TYPE_FBRX_GAIN_V2 data
08/27/13   vs      Added api for retrieving tuner NV item code per band per tech
08/08/13   sl/cri  Common implementation for DPD scaling   
07/29/13   aca     RL NV change us to ms
07/29/13   aca     RL NV version update
07/24/13   dbz     Add FBRX gain switch point NV support
07/24/13   vb      Support for adaptation_time_ms and aol_threshold params in RL NV
07/19/13   dbz     Add support for common API to intepret FBRX gain v.s. freq NV
07/19/13   ndb     Added a new flag "closed_loop_enable" in the CL NV's  
07/10/13    pv     Added support for Delay vs Temp vs Freq NVs
07/02/13   ndb     Change the name of CL ver enum ID
06/28/13   ndb     Added rfcommon_load_ant_tuner_cl_ctrl_info_from_nv()
06/19/13   aca     Added support for Tuner RL NV
03/28/13   ems     Added support for NV items larger than 4K 
02/09/12   pl      PACK the notch NV structure
01/26/12   sty     Added support to read Spur info from NV
08/10/11   tnt     Initial Ant Tuner support  
07/22/11   dw      Increase RFNV_DATA_SIZE_MAX to 8k
04/12/11   sty     Added rf_has_five_gainranges to this file
04/12/11   sty     reverted previous change
04/12/11   sty     Deleted rfcommon_core_types.h
03/19/11   bmg     Added C++ safety.
07/01/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
04/12/10   aki     RFNV modifications
08/20/09   tnt     Merge changes from depot...mdm9k
06/26/09   sr      Added prototype for rfcommon_nv_retrieve_items to fix warning 
03/06/09   sr      re-organized based on the new rf driver architecture.
12/16/07   dyc     BC10 support.
11/29/06   jfc     Conditionally define diversity NV modes
10/17/06   ycl     Modifications for initial MSM7600 build.
09/18/06   hkk     Removed rfnv_cdma1x_bc1.h from RF_HAS_GPS_NV_TABLE
01/10/06   sar     Fixed lint errors.
11/15/05   rmd     Added support for band class 11.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
07/01/05   sar      Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
06/13/05   dbc     Removed support for MFLO (RF_HAS_MFLO)
05/02/05   dbc     Added support for MFLO (RF_HAS_MFLO)
01/07/05   sar     Updated featurization in rfnv_supported_mode_type to match 
                   that of rfnv_supported_mode_info_table.
09/30/04   sar     Added functions rfnv_retrieve_nv_items(), rfnv_nv_read() and
                   rfnv_nv_write().
05/25/04   sar     Added support for legacy cdma and pcs bands. Updated feature 
                   flags for nv tables.
03/19/04   sar     Updated to add new band classes.
01/30/04   zg      Fixed "RF_HAS_GPS"
12/04/03   sar     Fixed typo in Featurization in definition of 
                   rfnv_supported_mode_type.
10/07/03   sar     Adapted for Jaguar. Modified structure rfnv_supported_mode_info_type
                   and updated rfnv_supported_mode_info_table for 1X. Added prototype
                   for rfnv_get_nv_table().
===========================================================================*/

/*===========================================================================

                                 INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "nv.h"
#include "customer.h"
#include "rfcom.h"
#include "rfnv_items.h"
#include "rfnv_item_types.h"
#include "rfcommon_core_txlin_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NV_ITEM_ATTRIB_CRTCL_BMSK  0x01     /* bit 0 = critical */
#define NV_ITEM_ATTRIB_NON_CRTCL_BMSK 0x00  /* bit 0 = critical */
#define NV_CS_PER_CORE_MAX  4               /* number of CS per core */

#define RFNV_FILE_PATH "/nv/item_files/rfnv/"   // Path to RFNV files in EFS
#define RFNV_MAX_PATH 48

#define RFCOMMON_MAX_TUNER_GAIN_STATES  8
#define RFCOMMON_MAX_TUNER_DETECTORS    6
#define RFCOM_NV_FBRX_SELFTEST_GAIN_SWITCH_PTS_NUM 10
#define RFCOMMON_NV_RX_GAIN_V2_NV_MAX_CONTAINERS 32



/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The max number of spur entries allowed in the spur table - this is per chain 
  and per tech */
#define RFCOMMON_MAX_SPUR_ENTRIES                                             30

/* used in rfnv_gsm files :other RF related features */
extern boolean rf_has_five_gainranges;

typedef enum
{
  RFCOMMON_NV_READ_SUCCESS,
  RFCOMMON_NV_READ_FAILED,
  RFCOMMON_NV_WRITE_SUCCESS,
  RFCOMMON_NV_WRITE_FAILED,
  RFCOMMON_NV_INACTIVE_ITEM,
  RFCOMMON_NV_INVALID_SIZE,
  RFCOMMON_NV_BAD_OFFSET,
  RFCOMMON_NV_FILE_OPEN_FAIL,
  RFCOMMON_NV_FILE_SEEK_FAIL,
  RFCOMMON_NV_FILE_NOT_FOUND
} rfcommon_nv_status_type;

typedef struct
{
  /* nv item identifier */
  nv_items_enum_type   nv_item_id;

  /* flag to indicate calibration item */
  byte              nv_item_attrib;

} rfcommon_nv_items_list_type;


typedef enum
{
  RFCOMMON_NV_ATUNER_CL_CTRL_INFO_V_A_ID   = 0x0A,
  RFCOMMON_NV_ATUNER_CL_CTRL_INFO_INVALID = 0xFF
}rfcommon_nv_ant_tuner_cl_ctrl_info_version_type;


/* function description for extracting nv item list size              */
typedef word (*rfcommon_rfnv_get_list_size_type)(void);

/* function description for accessing the pointer to the nv structure */
typedef void *(*rfcommon_rfnv_get_item_table_type)(void);

typedef struct
{
  /* pointer to an array of supported NV items for a particular mode  */
  const rfcommon_nv_items_list_type *nv_item_list_ptr;

  /* void pointer to the structure with the nv values for the band    */
  rfcommon_rfnv_get_item_table_type get_nv_item_table;

  /* size of the array of supported NV items for a particular mode    */
  rfcommon_rfnv_get_list_size_type get_nv_item_list_size;

  /* flag to indicate that the NV data is valid for that mode         */
  boolean             nv_data_valid_flag;

  /* function pointer to a function to copy NV item from NV item temp
     buffer to the NV table of a particular mode                      */
  void (*copy_nv_item) ( nv_items_enum_type nv_item_id,
                         nv_item_type *nv_item_buf_ptr);

} rfcommon_nv_info_type;



#define RFNV_DATA_SIZE_MAX 8192

typedef byte rfnv_item_type[RFNV_DATA_SIZE_MAX]; 
typedef word rfnv_item_size_type;

typedef struct
{

  rfnv_item_id_enum_type   rfnv_item_id;     /* nv item identifier */
  byte                     rfnv_item_attrib; /* flag to indicate calibration item */

} rfcommon_rfnv_items_list_type;

typedef void (*copy_rfnv_item_type) ( rfnv_item_id_enum_type nv_item_id, rfnv_item_type *nv_item_buf_ptr);

typedef struct
{
  /* pointer to an array of supported RFNV items for a particular mode  */
  const rfcommon_rfnv_items_list_type *rfnv_item_list_ptr;

  /* void pointer to the structure with the nv values for the band    */
  rfcommon_rfnv_get_item_table_type get_rfnv_item_table;

  /* size of the array of supported RFNV items for a particular mode    */
  rfcommon_rfnv_get_list_size_type get_rfnv_item_list_size;

  /* flag to indicate that the RFNV data is valid for that mode         */
  boolean             rfnv_data_valid_flag;

  /* function pointer to a function to copy NV item from NV item temp
     buffer to the NV table of a particular mode                      */
  copy_rfnv_item_type  copy_rfnv_item;

} rfcommon_rfnv_info_type;

/* Ant Tuner NV data structure is common for all tech/band. 
   Each tech/band needs to have its own NV                 */
typedef PACK(struct)
{
  uint8 device_enable[NV_CS_PER_CORE_MAX];         /* 1=codeword for device0 is valid, 0 ow*/
  uint8 device_cs[NV_CS_PER_CORE_MAX];             /* 0..3, */                                  
  uint16 channel_list[16];                         /* array of 16 rx channels in a band */
  uint32 code_words[64];                           /* 16 codewords for each device */
}rfcommon_nv_ant_tuner_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Tuner closed loop static NV definition.
  One per tech per band per chain.
  Tech teams pass NV to tuner manager on Tx configuration
*/

typedef PACK(struct)
{
  /*flag to control the closed loop measurement*/
  uint8 closed_loop_enable;
  
  /* How often the RL measurements occur */
  uint16 rl_meas_trigger_time_ms;

  /*Defines the time Tuner CL state machine needs to continue in a scenarion 
    after one complete iteration of CL statemachine loop. */
  uint16 adaptation_time_ms;

  /* Return loss measurement that stop corrections */  
  int16 rl_threshold_db10; /* 6 dB  = 60*/

  /*Defines the threshold in db*10 for AOL scenario or when to use to sensor 
  updates. Only if all the CL states fail this threshold then only sensor
  update will be entertained */
  int16 aol_threshold_db10; 

  /* Tx Power at which measurements start */
  int16 cl_tx_pwr_switch_pt_dbm10; /* 12 dBm  = 120*/

  /* Power levels at which gain states are switched */
  int16 detector_gain_switch_pt_dbm10[RFCOMMON_MAX_TUNER_DETECTORS][RFCOMMON_MAX_TUNER_GAIN_STATES];
  
}rfcommon_nv_ant_tuner_cl_ctrl_data_type_v_a;

typedef PACK(struct)
{
  /* Strucutre determined by version */
  uint8 version; 

  /* Determined by size of rfcommon_nv_ant_tuner_cl_ctrl_data_type (based on version) */
  void* payload; 
  
}rfcommon_nv_ant_tuner_cl_ctrl_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing the absoulute freq at which spur is present, and the  
  notch filter depth to be applied to supress the spur
*/ 
typedef PACK(struct)
{
  /*! Absolute frequency(Hz) at which spur is present */
  uint32 spur_at_freq_hz;

  /*! Notch depth (value of K0 in the notch FSM) to be used supress the spur */
  uint8 notch_depth;

} rfcommon_spur_entry_type_s;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Table that is to be populated from the NV for a given (chain and tech)
*/
typedef PACK(struct)
{
  /*! master table that contains all the spurs for a given chain and tech */
  rfcommon_spur_entry_type_s rfcommon_spur_tbl[RFCOMMON_MAX_SPUR_ENTRIES]; 

} rfcommon_spur_table_type;

#define RFCOM_NV_FBRX_NOISE_DATA_NUM 4

typedef struct
{
  uint16 utra1_neg; // for W, interpret this as ACLR-5
  uint16 utra1_pos; // ACLR+5
  uint16 utra2_neg; // ACLR-10
  uint16 utra2_pos; // ACLR+10
  uint16 eutra_neg; // invalid for W
  uint16 eutra_pos; // invalid for W
} rfcommon_nv_fbrx_noise_binsum_type;

typedef PACK (struct)
{
   /* flag to indicate if cal data is valid for index (index 0: W/LTE10, 1: LTE20) */
   boolean  data_valid[RFCOM_NV_FBRX_NOISE_DATA_NUM];
   
   /* table containing noise power per bin for given BW/Tech */
   rfcommon_nv_fbrx_noise_binsum_type rfcommon_nv_fbrx_noise_binsum[RFCOM_NV_FBRX_NOISE_DATA_NUM];

} rfcommon_nv_fbrx_noise_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Table with spur thresholds that is to be populated from the NV for a given chain and tech
*/
typedef PACK(struct)
{
  /*! master table that contains thresholds for the corresponding spurs */
  int32 spur_thresh[RFCOMMON_MAX_SPUR_ENTRIES]; 

} rfcommon_spur_thresh_table_type;

typedef struct
{
   rfcom_lte_band_type band_x;
   rfcom_lte_band_type band_y;
   rfnv_item_id_enum_type ca_item_code; 
} rfcommon_nv_lte_ca_info;

typedef struct
{
   rfcom_wcdma_band_type band_1;
   rfcom_wcdma_band_type band_2;
   rfnv_item_id_enum_type dbdc_item_code; 
} rfcommon_nv_wcdma_dbdc_info;

#define RFCOMMON_NV_MAX_PA_STATE     RFCOM_TXLIN_NUM_LINEARIZERS
#define RFCOMMON_NV_MAX_TEMP_BIN     RFCOM_TXLIN_TEMP_BIN_SIZE
#define RFCOMMON_NV_MAX_FREQ_BIN     RFCOM_TXLIN_NUM_CHANNELS
#define RFCOMMON_NV_RESERVED_ROW     5
#define RFCOMMON_NV_MAX_CDMA_RATE    6

#define PA_STATIC_TIMER_HYS_INDEX    0

typedef enum 
{
  /*!> Use legacy NVs for PA static items */
  RFCMN_NV_LEGACY_FOUR_STATE_PA = 0,

  /*!> CMN PA static NV Version 1 */
  RFCMN_NV_PA_STATIC_VER_1 = 1,                      

  /*!> MAX Num of PA static NV version */
  RFCMN_NV_PA_STATIC_VER_MAX,

  /*!> Invalid */  
  RFCMN_NV_PA_STATIC_VER_INVALID

} pa_static_version_type;

typedef enum
{
  PA_STATIC_VERSION = 0,
  PA_STATIC_NUM_ACTIVE_PA,
  PA_STATIC_RESERVED_2,
  PA_STATIC_RESERVED_3,
  PA_STATIC_RESERVED_4,
  PA_STATIC_RESERVED_5,
  PA_STATIC_RESERVED_6,
  PA_STATIC_RESERVED_7, 
} pa_static_header_type_e;

/* COMMON PA switchpoint structure - Used by all techs */
typedef PACK(struct)
{
  /* Header info: 0->Version | 1-> No.of Active PA states | rest -> reserved */
  int16 header[RFCOMMON_NV_MAX_PA_STATE];

  /* PA Range Map NV */
  int16 pa_range_map[RFCOMMON_NV_MAX_PA_STATE];

  /* PA Fall switchpoint: 0-> Timer Hystersis value | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* Tx Rotator Angle */
  int16 tx_rot_angle[RFCOMMON_NV_MAX_PA_STATE];

  /* PA Compensate UP -> used for ILPC SW Pt compensation */
  int16 pa_comp_up[RFCOMMON_NV_MAX_PA_STATE];

  /* PA Compensate Down -> used for ILPC SW Pt compensation */
  int16 pa_comp_dn[RFCOMMON_NV_MAX_PA_STATE];

  /* Pin NV: 8-temp bin | 16-freq bin | 8 PA states */
  int16 tx_lin_pin[RFCOMMON_NV_MAX_TEMP_BIN][RFCOMMON_NV_MAX_FREQ_BIN][RFCOMMON_NV_MAX_PA_STATE];

  /* Pout NV: 8-temp bin | 16-freq bin | 8 PA states */  
  int16 tx_lin_pout[RFCOMMON_NV_MAX_TEMP_BIN][RFCOMMON_NV_MAX_FREQ_BIN][RFCOMMON_NV_MAX_PA_STATE];

  /* PA Compensate UP vs FREQ -> used for ILPC SW Pt compensation */
  int16 pa_comp_up_vs_freq[RFCOMMON_NV_MAX_FREQ_BIN][RFCOMMON_NV_MAX_PA_STATE];

  /* PA Compensate Down vs FREQ -> used for ILPC SW Pt compensation */
  int16 pa_comp_dn_vs_freq[RFCOMMON_NV_MAX_FREQ_BIN][RFCOMMON_NV_MAX_PA_STATE];

  /* Reserved Rows can be used for future extentions */
  int16 reserved[RFCOMMON_NV_RESERVED_ROW][RFCOMMON_NV_MAX_PA_STATE];

} rfcommon_nv_cmn_pa_static_type;


/* CDMA Specific switchpoint structure - Not used by other techs */
typedef PACK(struct)
{
  /* CDMA ACCESS PA Fall switchpoint: 0-> Timer Hystersis value | 1-> R1 fall | 2 -> R2 fall etc */
  int16 cdma_access_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA ACCESS PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 cdma_access_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 1 Fall switchpoint */
  int16 cdma_data_rate1_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 1 Rise switchpoint */
  int16 cdma_data_rate1_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 2 Fall switchpoint */
  int16 cdma_data_rate2_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 2 Rise switchpoint */
  int16 cdma_data_rate2_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 3 Fall switchpoint */
  int16 cdma_data_rate3_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 3 Rise switchpoint */
  int16 cdma_data_rate3_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 4 Fall switchpoint */
  int16 cdma_data_rate4_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 4 Rise switchpoint */
  int16 cdma_data_rate4_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 5 Fall switchpoint */
  int16 cdma_data_rate5_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 5 Rise switchpoint */
  int16 cdma_data_rate5_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 6 Fall switchpoint */
  int16 cdma_data_rate6_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* CDMA DATA PA Rate 6 Rise switchpoint */
  int16 cdma_data_rate6_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR ACCESS PA Fall switchpoint: 0-> Timer Hystersis value | 1-> R1 fall | 2 -> R2 fall etc */
  int16 hdr_access_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR ACCESS PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 hdr_access_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Fall switchpoint RRI0 RATE */
  int16 hdr_dor0_rri0_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Rise switchpoint RRI0 RATE */
  int16 hdr_dor0_rri0_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Fall switchpoint RRI1 RATE */
  int16 hdr_dor0_rri1_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Rise switchpoint RRI1 RATE */
  int16 hdr_dor0_rri1_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Fall switchpoint RRI2 RATE */
  int16 hdr_dor0_rri2_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Rise switchpoint RRI2 RATE */
  int16 hdr_dor0_rri2_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Fall switchpoint RRI3 RATE */
  int16 hdr_dor0_rri3_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Rise switchpoint RRI3 RATE */
  int16 hdr_dor0_rri3_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Fall switchpoint RRI4 RATE */
  int16 hdr_dor0_rri4_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Rise switchpoint RRI4 RATE */
  int16 hdr_dor0_rri4_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Fall switchpoint RRI5 RATE */
  int16 hdr_dor0_rri5_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DOR0 PA Rise switchpoint RRI5 RATE */
  int16 hdr_dor0_rri5_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DORA PA Fall switchpoint: 0-> Timer Hystersis value | 1-> R1 fall | 2 -> R2 fall etc */
  int16 hdr_dora_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DORA PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 hdr_dora_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DORB MC PA Fall switchpoint: 0-> Timer Hystersis value | 1-> R1 fall | 2 -> R2 fall etc */
  int16 hdr_dorb_mc_pa_fall_sw[RFCOMMON_NV_MAX_PA_STATE];

  /* HDR DORB MC PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 hdr_dorb_mc_pa_rise_sw[RFCOMMON_NV_MAX_PA_STATE];
  
}rfcommon_nv_cdma_specific_pa_static_type;

/* Combined PA static NV used for all 3G and 4G tech */
typedef struct
{
  rfcommon_nv_cmn_pa_static_type cmn_pa;
  rfcommon_nv_cdma_specific_pa_static_type cdma_pa;
} rfcommon_nv_pa_static_data_type;

/*===========================================================================

FUNCTION rfcommon_rfnv_init

DESCRIPTION
  Creates config file for RFNV item backup

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  0 = Success

SIDE EFFECTS
  Overwrites config file if it already exists

===========================================================================*/
int32 rfcommon_rfnv_init(void);


/*===========================================================================

FUNCTION rfcommon_rfnv_set_item

DESCRIPTION
  This function sets a specific RFNV item to NV or EFS.
  Items with IDs greater than RFNV_USE_EFS_NV_CUTOFF will be stored to EFS.

DEPENDENCIES
  The NV task has been started and is running for NV items with IDs below
  the RFNV_USE_EFS_NV_CUTOFF

RETURN VALUE
  nv_stat_enum_type

SIDE EFFECTS
  While this function is running all other current task activities are
  suspended except for watchdog kicking, and until the NV item is
  written in case of ID is less than RFNV_USE_EFS_NV_CUTOFF.

===========================================================================*/
extern nv_stat_enum_type rfcommon_rfnv_set_item
(
  rfnv_item_id_enum_type  item_code,       /* Item to set */
  rfnv_item_type          *item_ptr,       /* Pointer to the item */
  rfnv_item_size_type     item_size,       /* Size of item in bytes */
  rex_tcb_type            *task_ptr,       /* Calling task's TCB pointer */
  rex_sigs_type           task_wait_sig,   /* Task signal to wait for when reading NV */
  void                    (*task_wait_handler)( rex_sigs_type ) /* Task's wait function to be called when reading NV */
);


/*===========================================================================

FUNCTION rfcommon_rfnv_get_item

DESCRIPTION
  This function gets a specific RFNV item from NV or EFS.
  Items with IDs greater than RFNV_USE_EFS_NV_CUTOFF will be stored to EFS.

DEPENDENCIES
  The NV task has been started and is running for NV items with IDs below
  the RFNV_USE_EFS_NV_CUTOFF

RETURN VALUE
  nv_stat_enum_type

SIDE EFFECTS
  While this function is running all other current task activities are
  suspended except for watchdog kicking, and until the NV item is
  written in case of ID is less than RFNV_USE_EFS_NV_CUTOFF.

===========================================================================*/
nv_stat_enum_type rfcommon_rfnv_get_item
(
  rfnv_item_id_enum_type  item_code,
  rfnv_item_type          *item_ptr,
  rfnv_item_size_type     *item_size,       
  rex_tcb_type            *task_ptr,
  rex_sigs_type           task_wait_sig,
  void                    (*task_wait_handler)( rex_sigs_type )
);



/*===========================================================================

FUNCTION rfcommon_rfnv_retrieve_items

DESCRIPTION
  This function retrieves NV items for each item specified in the nv_info_table.

PARAMETER
  Caller task info. see below.

DEPENDENCIES
  See rfcommon_rfnv_get_item

RETURN VALUE
  rfcommon_nv_status_type

SIDE EFFECTS
  It will block the caller task.

===========================================================================*/
rfcommon_nv_status_type rfcommon_rfnv_retrieve_items
(
  rfcommon_rfnv_info_type *nv_info_table, 
  int16                   rfnv_info_table_size, 
  rex_tcb_type            *task_ptr, 
  rex_sigs_type           task_wait_sig,
  void                    (*task_wait_handler)( rex_sigs_type )
);


/*===========================================================================

FUNCTION rfnv_retrieve_nv_items

DESCRIPTION
  This function retrieves NV items for each mode specified in the
  rfnv_supported_mode_info_table.

PARAMETER
  Caller task info. see below.

DEPENDENCIES
  NV task is started.

RETURN VALUE
  status of the NV read process.

SIDE EFFECTS
  It will block the caller task.

===========================================================================*/
rfcommon_nv_status_type rfcommon_nv_mm_retrive_items
(
  rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
  rex_sigs_type task_wait_sig,          /* Task signal to wait for when reading NV */
  void          (*task_wait_handler)( rex_sigs_type ) /* Task's wait function to be called when reading NV */
);

/*===========================================================================

MACRO RFNV_SATURATE

DESCRIPTION
  This macro returns the input (val) limited to the range specified by (min)
  and (max).  This is used for clipping the results of adjustment
  calculations where the newly calculated value may exceed the legal range
  for the control.  Since these calculations are typically closed loop
  integrators, this macro prevents "integrator windup" and/or "control
  overflow/underflow".

PARAMETERS
  val   the value to be limited (saturated) interpreted as a signed integer
  min   the minimum allowed value (e.g. 0)
  max   the maximum allowed value (e.g. 2^n-1)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RFNV_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )

/*===========================================================================

FUNCTION RFNV_SET_NV_ITEM

DESCRIPTION
  This function set a specific NV item to NV.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
None

SIDE EFFECTS
  While this function is running all other current task activities are
  suspended except for watchdog kicking, and until the NV item is
  written.

===========================================================================*/
extern nv_stat_enum_type rfcommon_nv_set_item
(
  nv_items_enum_type  item_code,       /* Item to set */
  nv_item_type        *item_ptr,       /* Pointer to the item */
  rex_tcb_type        *task_ptr,       /* Calling task's TCB pointer */
  rex_sigs_type       task_wait_sig,   /* Task signal to wait for when reading NV */
  void                (*task_wait_handler)( rex_sigs_type ) /* Task's wait function to be called when reading NV */
);

/*----------------------------------------------------------------------------*/

rfcommon_nv_status_type rfcommon_nv_retrieve_items
(
  rfcommon_nv_info_type *nv_info_table, 
  int16 nv_info_table_size, 
  rex_tcb_type *task_ptr, 
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)( rex_sigs_type )
);

/* This structure will provide Delay compensation over freq and Temp based on characterized data from NV */
typedef PACK(struct)
{
  /* This entry controls the number of channels to be used for Delay offset across 8 bin temperature. 
     We allow a max of 16 on this. Also this value needs to match the channels enabled for absolute Delay NV 
    which is unique for each band. If zero is the value set on this variable we will NOT use this feature.
   We can choose what Channels */
  uint8 channels_enabled;

  /* This is the physical channels which will be used to limit the offset being used for. */
  uint16 tx_upper_bound_chan_list[RFCOM_TXLIN_NUM_CHANNELS];

  /* 2D Matrix comprises of Delay offset vs Temp vs Freq characterization of Tx Chain. */
  int16 delay_offset_vs_temp_vs_freq[NV_TEMP_TABLE_SIZ][RFCOM_TXLIN_NUM_CHANNELS];

} rfnv_tx_delay_offsets_type;

/* This structure will provide PA Switchpoint adjust capability from NV */
typedef PACK(struct)
{
  /* This entry controls whether this feature is enabled or not */
  uint8 pa_swp_adj_enable;

  /* This is the value (+ve/-ve) to adjust the PA switchpoints */
  int8 pa_swp_adj_val[RFCOM_TXLIN_NUM_PA_STATES-1];

} rfnv_pa_swp_adj_val_type;

/* NV structure for XPT DPD scaling NV data */
typedef PACK(struct)
{
  int32 scaling_data[RFCOM_TXLIN_NUM_CHANNELS];
} xpt_dpd_scaling_v1_nv_type;

/* NV structure for SelfTest FBRx Gain Stage switch points*/
typedef PACK (struct)
{
   /*Number of valid entries in channel list per band */
   uint8  num_switch_pts;		
   /*Channel list , Max list size is 16*/
   int16 switch_pwr_dbm[RFCOM_NV_FBRX_SELFTEST_GAIN_SWITCH_PTS_NUM];
} rfcommon_nv_selftest_fbrx_gain_switch_pt_type;


/* WORST CASE SIZE = 70 bytes (from below calculation). 
   DO NOT decrease, only increase */
#define XPT_DPD_SCALING_V1_NV_SIZE_MAX ( 0                                                    \
                                       + sizeof( rf_nv_data_variant_marker_type )             \
                                       + (1 * ( RFNV_DATA_TYPE_ID_SIZE                        \
                                               + sizeof( xpt_dpd_scaling_v1_nv_type ) ) )  )

nv_stat_enum_type rfcommon_nv_get_item(nv_items_enum_type item_code,
                                       nv_item_type *item_ptr,
                                       rex_tcb_type *task_ptr,
                                       rex_sigs_type task_wait_sig,
                                       void (*task_wait_handler)( rex_sigs_type ));

boolean rfcommon_load_ant_tuner_cl_ctrl_info_from_nv(rfnv_item_type *rfnv_item_ptr,
                                                     rfcommon_nv_ant_tuner_cl_ctrl_type *ant_tuner_cl_ctrl_info);

/*----------------------------------------------------------------------------*/
/*Tuner relate NV API*/
boolean 
rfcommon_nv_get_tuner_item_code
( 
   rfcom_mode_enum_type mode,
   rfcom_band_type_u band,
   boolean is_this_rx_operation,
   rfnv_item_id_enum_type *item_code 
);

boolean 
rfcommon_nv_get_tuner_ca_item_code
(
  rfcom_lte_band_type band_1,
  rfcom_lte_band_type band_2,
  rfnv_item_id_enum_type *item_code  
);

boolean rfcommon_nv_get_tuner_dbdc_item_code
( 
   rfcom_wcdma_band_type band_1,
   rfcom_wcdma_band_type band_2,
   rfnv_item_id_enum_type *item_code  
);
/*----------------------------------------------------------------------------*/

boolean
rfcommon_nv_load_fbrx_noise_data_from_nv
(
  rfcommon_nv_fbrx_noise_data_type *fbrx_noise_nv_data,
  rfnv_item_id_enum_type fbrx_noise_data_item,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

boolean 
rfcommon_load_pa_static_nv
(
  rfcom_mode_enum_type mode,
  void *pa_static_nv_data,
  rfnv_item_id_enum_type pa_static_nv_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);


boolean
rfcommon_populate_rx_gain_vs_freq_v2_nv
(
  rfnv_rx_cal_data_type        *nv_tbl_type_ptr,
  void                         *rfnv_item_ptr,
  uint8                        num_gain_states
);

/*===========================================================================

FUNCTION RFNV_CHK_NV_ITEM_COMPRESSED

DESCRIPTION
  This function check a specific NV item whether compressed with the following.
  TRUE:    if the input nv_item is compressed
  FALSE:   if the input nv_item is uncompressed
                compressed_nv: if the input nv_item has a coresponding compressed nv
                compressed_nv == input nv_item otherwise           

DEPENDENCIES
  None.

RETURN VALUE
boolean

SIDE EFFECTS
  Harded checking based on the nv defination enum

===========================================================================*/
boolean rfcommon_rfnv_chk_item_compressed
(
  rfnv_item_id_enum_type  item_code              /* Item to check */
);


boolean
rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv
(
  rfcommon_nv_selftest_fbrx_gain_switch_pt_type *selftest_fbrx_gain_switch_pts_nv_data,
  rfnv_item_id_enum_type selftest_fbrx_gain_switch_pts_data_item,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_NV_H */

