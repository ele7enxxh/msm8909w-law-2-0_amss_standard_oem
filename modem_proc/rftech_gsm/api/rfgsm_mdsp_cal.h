#ifndef RFGSM_MDSP_CAL_H
#define RFGSM_MDSP_CAL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       R F G S M  C A L  F U N C T I O N S

GENERAL DESCRIPTION
  This is the FTM file which contains polar specific commands

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007, 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_mdsp_cal.h#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/15   sml     Fixing complier warning
03/19/14   hoh     Added support for 8 PA gain states
01/15/14   sc      Remove unused APIs using FW shared memory 
11/05/13   ggs     Various changes required to support Bolt modem
10/24/13   tsr     TX calibraion support for wtr3925
08/01/13   sc      Move mDSP CCS APIs to CCS-specific files
12/11/12   sb      Add hooks for GSM APT 
12/06/12   tws     Fix Klocwork errors.
11/27/12   tws     Add rfm device to FTM GSM. 
09/26/12   tc      Added rfm_dev parameter to replace hardcoded RFM_DEVICE_0 in   
                   preparation for DSDA.
08/09/12   ggs     Initial changes and cleanup for Dime support
06/25/12   jps     Add support for characterized predistortion cal  
01/28/12   sar     Feature reduction changes.
12/20/11   sn      Added support for QFE1320
06/08/11   sr      Moved the pa_range offset information to the where the script is.
07/01/11   rsr/sr  Pre-distortion cal related changes for NikeL.        
03/11/11   sr      Removed un-used featurization
01/25/11   aro      Featurized Genesis HWIO
01/25/11   sr      msm dependecy macros, needs a proper fix later.
01/24/11   sc      Moved all #defines from rfgsm_mdsp_cal.c to this file.
                   This required rfgsm_core_timing_consts.h to be included.
11/26/10   ggs     Added support for enhanced predistortion
08/03/10   ggs     Initial version based upon ftm_polar.h
04/15/10   ggs     Removed ftm_polar_init_target_dependent_settings(rf_hw_type target)
                   This is not used but was causing a compilation error due to missing
                   declaration of rf_hw_type. Alternative is to #include rfcom.h
02/16/10   twa     Change PACKED struct to PACK( struct ) for gcc compatibility.
07/27/09   rsr     Code cleanup.
04/08/08   dw      Removed obsolete targets support
03/04/08   dw      Removed constants definition for HWTC IQ Capture configuration 
03/03/08   dw      Made UMTS IQ Capture parameters MSM target specific
12/12/07   jfc     Corrected FTM_DMA_GSM_RX_IRQ to 14 for worldphone targets
12/06/07   jfc     Added featurization for MSM7500 and MSM6800
08/08/07   jfc     Mainlined T_MSM6280 feature
06/13/07   jfc     Added CS cal support with ftm_prog_tx_cs_sweep()
02/01/07   jfc     Create macro for DMA_GSM_RX_IRQ number
01/05/07   jfc     Define macros for DMA channel
10/17/06   ycl     Modifications for initial MSM7600 build.
04/13/06   rsr     Changed the resolution of step size in RSB sweep to symbols.
03/28/06   rsr     Lint fixes.
03/15/06   rsr     Added support for TX RSB calibration
02/07/06    ka     Added ftm_polar_init_target_dependent_settings().
05/04/05   rsr     Removed warnings.Cleanup
04/15/05    xw     Added extcal parameter in ftm_prog_polar_tx_on().
04/07/05   rsr     Added macros for the switchpoints and stepsize switchpoint 
03/25/05   rsr     Made the pmic function to turn on CALPATH configurable 
          per customer request.
03/11/05   ka      Added support for external polar calibration.
03/04/05   rsr     Cleaned up warnings.
12/09/04   rsr     Created
===========================================================================*/
#include "rfgsm.h"
#include "rfgsm_mdsp.h"
#include "pm.h"
#include "rfgsm_core_timing_consts.h" /*only because of RF_RAMP_WINDOW_QTR_SYMBOLS*/
#include "rfm_device_types.h" /* only because of rfm_device_enum_type rfm_dev */
#include "rf_hal_bus_types.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rflm_gsm_dm.h"

/*===========================================================================
                 DEFINES
=============================================================================*/
/* PA ramp length. For each entry 2 dwords will be used (addr, ramp value) */
#define PA_RAMP_LEN ((RF_RAMP_WINDOW_QTR_SYMBOLS*3)>>1)

/*  LEGACY: 375 is max EDGE DMA symbols, 625 is max DMA Symbol length of flat step. */
/* GENESIS/MDM9K DMA wordlength is 64 bits, compare to 16 bits for legacy targets */ 

#define MAX_EDGE_STEP_LEN_LIN_AMAM_SWP 157 /* = 625/4 = 156.25 DMA WORDS */
/* 157*20 SYMBOLS = 3140 SYMBOLS = 2.512 frames (1250 Symbols/frame)  = 2.512*4.615 ms = 11.6ms */

#define MAX_FLAT_STEP_LEN_LIN_AMAM_SWP 93 /* = 375/4 = 93.75 DMA WORDS */
/* 93*20 SYMBOLS = 1860 SYMBOLS = 1.488 frames (1250 Symbols/frame)  = 1.488*4.615 ms = 6.87ms */

#define MAX_DMA_SYMBOLS_LINEAR_AMAM_SWEEP (MAX_EDGE_STEP_LEN_LIN_AMAM_SWP+MAX_FLAT_STEP_LEN_LIN_AMAM_SWP)
/* Check:  93+157 = 250 DMA WORDS. 20 SYMBOLS per word. 250*20 SYMBOLS = 5000 SYMBOLS = 4 frames = 5000*48/13 = 18.46ms
 * Note that for the initial implementation we hardcode 2 frames worth of data for Pre-distaortion cal.
 * 2 frames = 125 DMA words */

#define MAX_DMA_SYMBOLS_PER_FRAME    64 /* Due to EMOD HW constraints, can only program 1 frame worth of data at a time */

#define NUM_SYMB_PER_BURST    156

#define MAX_ENV_VAL_EDGE_PREDIST_SYMS 54733

/* May need to increase these numbers in the future */
#define FTM_NUM_PRE_BURST 1
#define FTM_NUM_CLEAN_UP 1

#define FTM_PRE_BURST_SBI   RFGSM_TX_START_MAX_SSBI_TRANSACTIONS_TASKQ_A
#define FTM_PRE_BURST_NONSBI  RFGSM_TX_START_MAX_NON_SSBI_TRANSACTIONS_TASKQ_B 


#define FTM_PRE_BURST_WTR_RFFE   FTM_PRE_BURST_SBI

#define FTM_TX_SWEEP_SBI    5
#define FTM_TX_SWEEP_WTR_RFFE    FTM_TX_SWEEP_SBI 
#define FTM_TX_SWEEP_NONSBI 8
#define FTM_POST_BURST_SBI 10
#define FTM_POST_BURST_WTR_RFFE FTM_POST_BURST_SBI
#define FTM_POST_BURST_NONSBI 10
#define FTM_NUM_STEPS       325 /* Allow only up to 5 PA gain ranges for now */

#define FTM_MASTER_TABLE_SIZE   128
#define FTM_PI  3.141592653589793238


#define CAL_SBI_BUF_ARR_SIZ (FTM_PRE_BURST_SBI + FTM_TX_SWEEP_SBI*FTM_NUM_STEPS + FTM_POST_BURST_SBI)
#define CAL_SBI_BUF_MEM_SIZ (CAL_SBI_BUF_ARR_SIZ * sizeof(rf_hal_bus_sbi_type))
#define CAL_GRFC_BUF_ARR_SIZ (FTM_PRE_BURST_NONSBI + FTM_TX_SWEEP_NONSBI*FTM_NUM_STEPS + FTM_POST_BURST_NONSBI)
#define CAL_GRFC_BUF_MEM_SIZ (CAL_GRFC_BUF_ARR_SIZ * sizeof(rf_hal_bus_grfc_type))
#define CAL_MSM_REG_BUF_ARR_SIZ (FTM_PRE_BURST_NONSBI + FTM_TX_SWEEP_NONSBI*FTM_NUM_STEPS+ FTM_POST_BURST_NONSBI)
#define CAL_MSM_REG_BUF_MEM_SIZ (CAL_MSM_REG_BUF_ARR_SIZ * sizeof(rflm_gsm_msm_reg_script_unit))
#define CAL_STEP_BUF_ARR_SIZ (FTM_NUM_PRE_BURST + FTM_NUM_STEPS + FTM_NUM_CLEAN_UP + 1)
#define CAL_STEP_BUF_MEM_SIZ (CAL_STEP_BUF_ARR_SIZ * sizeof(rfgsm_mdsp_cal_step_info_type))
#define CAL_RFFE_BUF_ARR_SIZ (FTM_PRE_BURST_NONSBI + FTM_TX_SWEEP_NONSBI*FTM_NUM_STEPS+ FTM_POST_BURST_NONSBI)
#define CAL_RFFE_BUF_MEM_SIZ (CAL_RFFE_BUF_ARR_SIZ * sizeof(rf_hal_bus_rffe_type))
#define CAL_FW_FRAME_BUF_ARR_SIZ (FTM_NUM_PRE_BURST + FTM_NUM_STEPS + FTM_NUM_CLEAN_UP + 1)
#define CAL_FW_FRAME_MEM_SIZ (CAL_FW_FRAME_BUF_ARR_SIZ * sizeof(rfgsm_mdsp_cal_tx_struct_type))
/*WTR3925 Support*/
#define CAL_WTR_RFFE_BUF_ARR_SIZ (FTM_PRE_BURST_WTR_RFFE + FTM_TX_SWEEP_WTR_RFFE*FTM_NUM_STEPS + FTM_POST_BURST_WTR_RFFE)
#define CAL_WTR_RFFE_BUF_MEM_SIZ (CAL_WTR_RFFE_BUF_ARR_SIZ * sizeof(rf_hal_bus_rffe_type))
/*===========================================================================
                 DEFINITIONS
=============================================================================*/

typedef enum
{
  FTM_4XFMOD_SWP,
  FTM_DA_SWP,
  FTM_RSB_DC_SWP,
  FTM_MSM_CONFIG2_AMAM_SWP,
  FTM_MSM_CONFIG2_AMAM_SWP_V2, /* Enhanced AMAM sweep */
  FTM_CS_SWP,
  FTM_ENVDC_CAL_CS_SWP,
  FTM_MAX_SWP
} rfgsm_mdsp_cal_swp_type;

typedef PACK( struct )
{
   uint16 step_duration;
   uint8 rgi_value;
   uint8 gsm_sweep; /* 1: GMSK, 0: 8PSK, 240-255: 8PSK with env gain from NV_GSM_<band>_CHAR_PREDIST_ENV_GAIN */
   uint8 pa_range;
}ftm_linear_da_sweep_data_type;

typedef PACK( struct )
{
   uint16 step_duration;
   uint16 envdc_val;
}ftm_linear_envdc_cs_data_type;


typedef struct
{
  float  vramp[FTM_MASTER_TABLE_SIZE];
  float  amam[FTM_MASTER_TABLE_SIZE];
  float  ampm[FTM_MASTER_TABLE_SIZE];
  uint16 num_entry;
} ftm_table_type;

typedef PACK( struct )
{
  uint16 dac_value;
  uint16 length;
}ftm_gsm_sweep_waveform_type;


typedef PACK( struct )
{
  ftm_linear_envdc_cs_data_type * envdc_sweep;
  uint16 num_steps;
  uint16 trig_daclev;
}rfgsm_mdsp_cal_envdc_cs_sweep_param_type;

typedef PACK( struct )
{
  ftm_linear_da_sweep_data_type * da_sweep;
  uint16 trig_daclev;
  uint16 num_steps;
}rfgsm_mdsp_cal_da_sweep_param_type;

typedef PACK( struct )
{
  uint16 step_len[2];
  uint16 rgi_index;
}rfgsm_mdsp_cal_amam_sweep_param_type;

typedef PACK( struct )
{
  uint16 step_len[2];
  uint16 rgi_index;
  uint16 type; /* Additional placeholder */
}rfgsm_mdsp_cal_amam_sweep_v2_param_type;

typedef PACK( struct )
{
  uint16 pa_ramp_max;
  uint16 error_code;
}ftm_gsm_amam_sweep_return_type;


typedef PACK( struct )
{
  uint8  vga;
  uint8  detector_value;
  uint16 error_code;
} ftm_gsm_detector_cal_result_type;

/*! @struct rfgsm_mdsp_cal_cnt_type */
/*! @brief This structure holds sbi, grfc, rffe and msm count for each calibration step */
typedef struct
{
  /*! Number of sbi entries */
   uint16   sbi_cnt;

  /*! Number of wtr rffe entries */
   uint16   wtr_rffe_cnt;

   /*! Number of grfc entries */
   uint16   grfc_cnt;

   /*! Number of rffe entries */
   uint16   rffe_cnt;

   /*! Number of msm entries */
   uint16   msm_cnt;
} rfgsm_mdsp_cal_cnt_type;

/*===========================================================================*/

/*! @struct rfgsm_mdsp_cal_step_info_type */
/*! @brief This structure holds all of the Tx setup information       
      for one calibration step. 
      It replaces legacy mdsp_ftm_tx_cal_grfc_type with some
      additional data fields added */

/* Structure to hold the number of SBI and nonSBI per step */
typedef struct
{
   uint16            num_sbi;
   rf_hal_bus_sbi_type       *sbi_scripts;

   uint16            num_wtr_rffe;
   rf_hal_bus_rffe_type       *wtr_rffe_scripts;

   uint16             num_grfc;
   rf_hal_bus_grfc_type  *grfc_scripts;

   uint16             num_rffe;
   rf_hal_bus_rffe_type  *rffe_scripts;

   uint16             num_msm_reg;
   rflm_gsm_msm_reg_script_unit  *msm_reg_scripts;

} rfgsm_mdsp_cal_step_info_type;

/*===========================================================================*/

/*! @struct rfgsm_mdsp_cal_tx_struct_type */
/*! @brief This structure holds all of the Tx calibration step information       
      for one frame
      It replaces the legacy mdsp_ftm_tx_cal_struct with some
      additional data fields added */
typedef struct
{
  uint32                     step_duration;   /* The width of each step. */
  boolean                    last_tx_cal_cmd; /* Last command indicator */
  uint32                     start_offset;
  uint16                     num_steps;     /* Number of steps */
  rfgsm_mdsp_cal_step_info_type  *steps_buffer; /* Pointer that holds the sbi/grfc/rffe/msm_reg writes
buffer for number of steps */
  boolean                    first_tx_cal_cmd;
#ifndef FEATURE_BOLT_MODEM
  GfwTxCalDmaData64BitType   *dma_data;
#endif /* !FEATURE_BOLT_MODEM */
  uint16                     dma_length;
  boolean                    modulation_type;
  int16                      modulator_start_time_offset_adj;
  int16                      frame_number; /* Used for performing modulo 2 division of shared memory buffer indexes 
  (ping-pong buffers) to ensure that FW is not reading a buffer that is being written to by SW */
} rfgsm_mdsp_cal_tx_struct_type;


/*! @struct rfgsm_mdsp_cal_variables_s */
/*! @brief This structure holds all of the global and static variables
          required for GSM TX cal. The structure is allocated at runtime
          and is specific to a given RF device. This allows for parallel
          TX calibration */
typedef struct
{
  /*! @brief This holds EDGE DMA data used for AMAM V1 and V2 Calibration */
  uint64 dma_data[MAX_DMA_SYMBOLS_LINEAR_AMAM_SWEEP];

  /*! @brief Data memory pointer */
  rflm_gsm_tx_cal_data_type* dm_ptr;

  /*! Global variable to track sbi, grfc rffe and msm count for each calibration step */
  rfgsm_mdsp_cal_cnt_type cnt;

  /*   
    Start offset is no longer static and is adjusted based upon the sweep type and step size. 
    Need to ensure that the start offset is set such that the calibration step start time does not fall in to   
    the next frame. This would be a problem because SW would be writing to a CCS shared memory buffer that is being processed/scheduled by 
    FW. 
    For example, 25 cal steps per frame would mean a cal step duration of 5000QS/25 = 200QS, so the start offset would need 
    to be set to 200QS to ensure that the start last cal step for the frame is sent at the start of the next frame (t=0).
    A start frame offset of 200QS means that FW only has 200QS to perform all of the frame processing before the start of the 
    first calibration step. 
    200QS start time offset may not work because RF_ON start offset is around -250QS. 
    This should still work because GFW will receive and start to process the asynch tx cal command in the previous frame.
    Worst case, we may need to move from double to triple buffered to handle this corner case.  
    10 cal steps per frame would mean a cal step duration of 5000QS/10 = 500QS, so the start offset would need 
    to be set to 500QS to ensure that the start last cal step for the frame is sent at the start of the next frame (t=0). 
    */

  volatile uint16 frame_start_offset;

  volatile uint16 frame_start_offset_adj_val;

  uint16 num_frames;

  uint16 step_frame_counter;

  /* This flag is checked in ISR context, during ftm_da_cal_send_tx_cal_cmd */
  boolean da_cal_swp_enabled;
  boolean amam_swp_enabled;
  boolean envdc_cs_swp_enabled;

  boolean updated_pdm_flag;

  /* For pre-distortion debugging only */ 
  volatile uint32 set_tx_cond_env_gain_debug;
  volatile uint32 set_amam_pa_scale_debug;

  volatile boolean hardcode_dma_data_debug;

  /* FTM sweep parameters */
  uint32 tx_cond_edge_env_gain_val;

  uint32 amam_pa_scale;

  uint16 amamExtVal;

  volatile int16 sbi_start_time_adj_value;
  volatile int16 grfc_start_time_adj_value;
  volatile int16 rffe_start_time_adj_value;
  volatile int16 wtr_rffe_start_time_adj_value;

  /* DA cal variables */
  uint16 pa_range_val_to_set;

  /*! @todo Clean up these temp bring-up debug variables */
  volatile boolean do_tx_cal_isr_debug;
  volatile boolean send_tx_cal_fw_cmd_debug; 
  volatile boolean reset_buffers_debug;

  /* Debug variables for bring-up */
  volatile boolean override_mod_debug;
  volatile boolean send_cleanup_step_debug;

  /*! @todo Legacy definition. This should be cleaned up */
  rfdevice_tx_agc_info_type rfdevice_txagc_info;

  /*=============================================================================
                          MSM REG SCRIPT ENTRIES
  =============================================================================*/

  /* New Dime shared memory interface uses separate buffers for GRFC and       
   * modem registers ("fake grfcs")      
  */

  /*! @struct rfgsm_mdsp_cal_tx_cond_edge_env_gain */
  /*! @brief Settings in TX_CONDITIONING_EDGE_ENV_GAIN0 are applied at the start 
  *  time of EDGE_ENV_GAIN_TRANSITION_CTL0 
  */
  rflm_gsm_msm_reg_script_unit tx_cond_edge_env_gain;

  /*! @struct rfgsm_mdsp_cal_smps_pdm_val */
  /*! @brief  Legacy SMPS/APT PDM control */
  rflm_gsm_msm_reg_script_unit smps_pdm_val;

  /*! @struct rfgsm_mdsp_cal_ep_ovr_val */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit ep_ovr_val;

  /*! @struct rfgsm_mdsp_cal_ep_ovr */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit ep_ovr;

  /*! @struct rfgsm_mdsp_cal_pp_ovr_val */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit pp_ovr_val;

  /*! @struct rfgsm_mdsp_cal_pp_ovr */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit pp_ovr;

  /*! @struct rfgsm_mdsp_cal_pa_scale */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit pa_scale;

  /*! @struct rfgsm_mdsp_cal_tx_predist_enable */
  rflm_gsm_msm_reg_script_unit tx_predist_enable;

  /*! @struct rfgsm_mdsp_cal_tx_predist_tbl_program */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit tx_predist_tbl_program;

  /*! @struct rfgsm_mdsp_cal_txc_dcoc */
  /*! @brief  */
  rflm_gsm_msm_reg_script_unit txc_dcoc;

  /*=============================================================================
                          MSM REG SCRIPTS 
  =============================================================================*/
  
  /*=============================================================================
                          DA Calibration
  =============================================================================*/

  /*! @struct rfgsm_mdsp_cal_msm_reg_tx_script */
  rflm_gsm_msm_reg_script_unit* msm_reg_tx_script[4];

  /*=============================================================================
                          AMAM SWP V1 and V2
  =============================================================================*/

  /*! @struct rfgsm_mdsp_cal_msm_reg_tx_script_amam_swp */
  rflm_gsm_msm_reg_script_unit* msm_reg_tx_script_amam_swp[10];

  /*=============================================================================
                          ENV DC CS
  =============================================================================*/

  /*! @struct rfgsm_mdsp_cal_msm_reg_tx_script_envdc_cs_swp */
  rflm_gsm_msm_reg_script_unit* msm_reg_tx_script_envdc_cs_swp[7];


  /*=============================================================================
                          LOCAL CALIBRATION BUFFERS
  =============================================================================*/

  /* Local buffer to hold SBI: 60 + 5*300 + 10 = 1570 */
  rf_hal_bus_sbi_type  sbi_buf[CAL_SBI_BUF_ARR_SIZ];

  /* Local buffer to hold RFFE: 26 + 5*300 + 10 = 1536 */
  rf_hal_bus_rffe_type wtr_rffe_buf[CAL_WTR_RFFE_BUF_ARR_SIZ];

  /* Local buffer to hold GRFC: 26 + 5*300 + 10 = 1536 */
  rf_hal_bus_grfc_type  grfc_buf[CAL_GRFC_BUF_ARR_SIZ];

  /* Local buffer to hold MSM register: 26 + 5*300 + 10 = 1536 */
  rflm_gsm_msm_reg_script_unit  msm_reg_buf[CAL_MSM_REG_BUF_ARR_SIZ];

  /* Local buffer to hold RFFE: 26 + 5*300 + 10 = 1536 */
  rf_hal_bus_rffe_type rffe_buf[CAL_RFFE_BUF_ARR_SIZ];

  /* Local buffer to hold the information for each cal bration step.  This structure will contain
   * pointer to rfgsm_mdsp_cal_sbi_buf and rfgsm_mdsp_cal_nonsbi_buf
   */
  rfgsm_mdsp_cal_step_info_type step_buffer[CAL_STEP_BUF_ARR_SIZ];

  /* Local structure used to populate CCS scripts in shared memory. It contains a pointer to rfgsm_mdsp_cal_step_buffer for each step. */
  rfgsm_mdsp_cal_tx_struct_type  fw_frame_buffer[CAL_FW_FRAME_BUF_ARR_SIZ];

  /*! @brief rfgsm_mdsp_cal_event_handle used for pushing 
      Tx calibration event into shared memory */
  rfcommon_mdsp_event_handle* rfgsm_mdsp_cal_event_handle;


} rfgsm_mdsp_cal_variables_s;

/*===========================================================================
                FUNCTION PROTOTYPES
=============================================================================*/

void rfgsm_mdsp_cal_init_vars( rfgsm_mdsp_cal_variables_s * vars_ptr );

void ftm_gen_tx_da_sweep( rfm_device_enum_type rfm_dev,
                          ftm_linear_da_sweep_data_type * da_sweep_input, 
                          uint16 num_steps_input);


void rfgsm_mdsp_cal_sweep( rfm_device_enum_type rfm_dev, 
                           rfgsm_mdsp_cal_swp_type sweep_type, 
                           void* sweep_arguments );


uint16 rfgsm_mdsp_cal_gen_amam_linear_sweep( rfm_device_enum_type rfm_dev,
                                             uint16* sweep_args);

uint16 rfgsm_mdsp_cal_gen_amam_linear_sweep_v2( rfm_device_enum_type rfm_dev,
                                                uint16* sweep_args);


void ftm_gen_tx_envdc_cs_sweep( rfm_device_enum_type rfm_dev,
                                ftm_linear_envdc_cs_data_type * envdc_cs_input, 
                                uint16 num_steps_input);

void ftm_da_cal_send_tx_cal_cmd( rfm_device_enum_type rfm_dev ); 


#endif /* RFGSM_MDSP_CAL_H */

