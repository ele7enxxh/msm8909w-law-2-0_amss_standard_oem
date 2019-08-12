/*!
  @file
  rflm_api_cmn.h

  @brief
  RFLM API common types and defines
  
  @detail
  This file contains common data types and definitions for exposed RFLM API
  with FW.  There should be no include dependencies with RFSW, AMSS SW, 
  and SW build other than comdef.h (as this is for use in FW builds)

*/

/*==============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_cmn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/15   ag      Auto-pin Acq/Trk mode changes
10/06/14   bsh     Add more enums to the error type
09/10/14   pl      Add support for dynamic notch 
07/31/14   jmf     Add API to get current self test mode to pass to FW
07/29/14   ggs     Introduce new enum value for invalid script type
07/23/14   dbz     Added support for selftest VSWR split capture
07/17/14   ggs     Introduce new enum for wrong CCS event status
05/22/14   oei     New TQP manager functions: reserve_dev, disassoc_device
05/07/14   sc      Add state machine err return
03/19/13   cri     Split sample capture initial revision
03/03/14   gvn     Change in TQP allocation and release implementation
01/23/14   jc      Added new FW APIs for coex
12/09/13   sar     Added prototype for rflm_cmn_get_tq_pair.
10/02/13   hdz     Added cmn API version
09/30/13   HDZ     Added rflm_dtr_tx_xfer_list_s related code
09/03/13   hdz     Used rflm.h instead of comdef.h
07/24/13   sty     added rflm_tech_id_t
07/18/13   hdz     Initial creation
==============================================================================*/

#ifndef RFLM_API_CMN_H
#define RFLM_API_CMN_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "comdef.h"

/*! @brief Version of the rflm cmn module.
*/
#define RFLM_API_CMN_MAJOR_VERSION            0
#define RFLM_API_CMN_MINOR_VERSION            1
#define RFLM_API_CMN_VERSION    \
              ((RFLM_API_CMN_MAJOR_VERSION << 16) | RFLM_API_CMN_MINOR_VERSION )

/*! @brief Maximum number of block data to be DMA transferred */
#define RFLM_DTR_TX_XFER_MAX_NUM                                              9

/*! @brief Number of bins of modem settings needed 
  to satisfy split sample capture
  1. Default values
  2. Const ENV values
  3. Const IQ values */
#define RFLM_DTR_SPLIT_CAPT_SETTINGS_MAX_NUM                                  3

/*! @brief Number of dedicated Notch per WB */
#define RFLM_DTR_DEDICATED_NOTCH_PER_WB_MAX_NUM                               4          

/*==============================================================================

  Common datatypes used throughout LM API
  
==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!@brief Defines total number of NB chains supported in AGC configuration 
          interface. todo - use RxLM #define */
#define RFLM_CMN_RXAGC_MAX_NB_PER_WB                                           4


/*! @brief 
    Opaque handle for uniquely identifying rx channel subscription in 
    LM API calls.  Handles are passed to FW via L1 prior to channel activation.
    The handle remains valid until channel deactivation.
*/
typedef uint32 rflm_handle_rx_t;


/*! @brief 
    Opaque handle for uniquely identifying tx channel subscription in 
    LM API calls.  Handles are passed to FW via L1 prior to channel activation.
    The handle remains valid until channel deactivation.
*/
typedef uint32 rflm_handle_tx_t;


/*! @brief 
    Enum of common LM API error return codes.
*/
typedef enum 
{
  RFLM_ERR_NONE                     = 0,
  RFLM_ERR_IGNORED                  = 1,  
  RFLM_ERR_API_FAILED               = 2,
  RFLM_ERR_API_TxAGC_ORPHAN         = 3,   ///< TxAGC update is called without a corresponding TxAGC calc
  RFLM_ERR_API_TXAGC_FIXUP_SKIP     = 4,   ///< TxAGC update fixup is skipped due to insufficeint time
  RFLM_ERR_API_STATE_MACHINE        = 5,   ///< FW to RFLM call-flow error
  RFLM_ERR_CCS_EVENT_STATUS         = 6,
  RFLM_ERR_INVALID_SCRIPT           = 7,

  /*Specifying the error type*/
  RFLM_ERR_INVALID_INPUT_INF0       = 100,  ///< Any input information that is wrong/invalid. Handle ID, carrier index, path. 
  RFLM_ERR_NULL_PARAMS              = 101,  ///< INPUT and/or OUTPUT structure are/is NULL. 
  RFLM_ERR_DM_NOT_READY             = 102,  ///< DM scripts or data is unavailable. This happens when rx config is still active
  RFLM_ERR_ACTION_TIME_IN_PAST      = 103,  ///< The action time passed by FW is in past, beyond allowed limits
  RFLM_ERR_ACTION_TIME_IN_FUTURE    = 104,  ///< The action time passed by FW is in future, beyond allowed limits

  RFLM_ERR_PAD= 0x7FFFFFFF
} rflm_err_t;


typedef enum
{
	RFLM_COEX_HANDLE_RX=0,
	RFLM_COEX_HANDLE_TX=1,
	RFLM_COEX_HANDLE_ASYNC=2,
	RFLM_COEX_HANDLE_PAD=0x7FFFFFFF
}rflm_coex_handle_t;


/*! @brief 
  Enum for ADC low power states
*/
typedef enum
{
  RFLM_DTR_RX_LPS_LTE_5_10 = 0,
  RFLM_DTR_RX_LPS_LTE_15_20,
  RFLM_DTR_RX_LPS_LTE_MAX,
  RFLM_DTR_RX_LPS_LTE_NUM = RFLM_DTR_RX_LPS_LTE_MAX
}rflm_dtr_rx_adc_low_pwr_state_enum;


/*! @brief 
    type for HW UNIV_STMR time value
*/
typedef uint32 rflm_ustmr_t;


/*! @brief 
    Power in 0.01 dB units
*/
typedef int16  rflm_dbm100_t;

/*! @brief 
    Power in db10 units
    Dynamic Range -512 to +511
*/
typedef int16  rflm_db10_t;

typedef int16 rflm_pin_polarity;

/*! @brief
    DTR input config parameters 
*/
typedef struct
{
  int32 reserved;                 ///< placeholder, no input needed
} rflm_drif_in_cfg_t;

/*! @brief Structure for TX DMA transfer */
typedef struct
{
  /*! @brief Start offset in words from DTR TX intf structure for starting the transfer */
  uint32 start_ofs_in_words;
  /*! @brief Transfer length in words */
  uint32 length_in_words;
  /*! @brief Destion address for writing the data */
  uint32 dest_addr;
} rflm_dtr_tx_xfer_s;

/*! @brief Number of block data to be DMA transferred */
typedef struct
{
  /*! @brief Number of block data need to be DMA transferred */
  uint32 num_xfer;
  /*! @brief DMA transfer block data array */
  rflm_dtr_tx_xfer_s tx_xfer_data[RFLM_DTR_TX_XFER_MAX_NUM];
} rflm_dtr_tx_xfer_list_s;


/*! @brief
    DTR output config parameters (for DRIF chunks and LUTs)
    Note: FW needs to update the DRIF buffer specified after the FED API
    call that outputs this structure.  
*/
typedef struct
{
  struct rflm_dtr_update_en_dac_idx_s
  {
    uint32  update_enable :   8;     ///< Update enable flag   
    uint32  dac_idx :         8;     /// dac indx (0/1)
    uint32  reserved :        16;    /// reserved for future
  }update_en_dac_idx;
                                  
  rflm_ustmr_t update_time;       ///< ustmr time for update
                                  ///< note: FW needs to convert to sample
                                  ///< & block number for actual HW intf
  
  rflm_ustmr_t log_start_time;    ///< ustmr time to start tx sample capture
                                  ///< for FBRx/dbg/cal
  
  rflm_ustmr_t log_stop_time;     ///< ustmr time to stop tx sample capture

  rflm_ustmr_t dac_iref_update_start_time; ///< ustmr time to start iref  update

    
  rflm_ustmr_t dac_iref_update_stop_time;     
                                  ///< ustmr time to stop iref update

  uint32 tx_dac_conf2_reg;        /// used for iref update

  uint32 tx_dac_conf3_reg;       /// used for iref update                           
  
                                  
  uint32       cluster_mask;      ///< Mask indicating which rflm owned cluster needs 
                                  ///< update.  non-RF cluster bits are set to 0.                                                                
  rflm_dtr_tx_xfer_list_s xfer_list;
                                  ///< List of LMEM blocks to DMA to DRIF.
                                  ///< This includes DRIF shadow reg and LUTs.
                                  ///< (Updated EVT LMEM is not included.)
} rflm_drif_out_cfg_t;

/*! @brief
    enums for tech ID 
*/
typedef enum
{
  RFLM_TECH_1X,
  RFLM_TECH_HDR,
  RFLM_TECH_LTE,
  RFLM_TECH_WCDMA,
  RFLM_TECH_TDSCDMA,
  RFLM_TECH_GSM,
  RFLM_MAX_TECH
}rflm_tech_id_t;

/*! @brief
   structure of api versions for all header files included by FW
*/
typedef struct
{
  uint32 rflm_api_cmn_version;
  uint32 rflm_api_c2k_version;
  uint32 rflm_api_wcdma_version;
  uint32 rflm_api_lte_version;
  uint32 rflm_api_gsm_version;
  uint32 rflm_api_tds_version;
  uint32 rflm_api_fbrx_version;
  uint32 rflm_api_dtr_rx_version;
  uint32 rflm_api_dtr_tx_version;  
}rflm_api_version_t;


/*! @brief
   structure coex api 
*/
typedef struct
{
  uint32   handle_select;    /*! 0 – rx handle, 1 - tx handle or neither(>=2) - swport*/
  uint32   trig_time;        /*! The trigger time in ustmr ticks */  
  uint16   level;            /*! The value to write */  
  uint16   grfc_id;          /*! The grfc id task */  
} rflm_api_cmn_coex_ctrl_t;
 
typedef struct
{
  rflm_handle_tx_t handle_id;
}rflm_set_et_split_cal_in_t;

typedef struct
{
  rflm_drif_out_cfg_t drif_set[RFLM_DTR_SPLIT_CAPT_SETTINGS_MAX_NUM];
}rflm_set_et_split_cal_out_t;

typedef enum
{
  RFLM_CMN_FBRX_COUPLER_INPUT_DIRECTION_FOWRARD,
  RFLM_CMN_FBRX_COUPLER_INPUT_DIRECTION_REVERSE,

} rflm_cmn_fbrx_coupler_input_direction_type;

typedef struct
{
  uint32 notch_I_q18;
  uint32 notch_Q_q18;
  uint32 notch_depth;
}rflm_dyn_notch_t;

typedef struct
{
  uint32 num_notch;
  rflm_dyn_notch_t settings[RFLM_DTR_DEDICATED_NOTCH_PER_WB_MAX_NUM];
}rflm_set_dyn_notch_in_t;

typedef struct
{
  uint32 num_notch_configured;
}rflm_set_dyn_notch_out_t;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief get api versions for all header files included by FW
*/

rflm_api_version_t 
rflm_cmn_get_api_version
(
  void
);


/*==============================================================================

FUNCTION:  rflm_api_cmn_coex_ctrl

==============================================================================*/
/*!
  @brief
  Issues GRFC control on behalf of coexistence manager
 
  @return
  rflm error code
*/
/*============================================================================*/
extern rflm_err_t rflm_api_cmn_coex_ctrl ( rflm_handle_rx_t rx_handle_id,
                                           rflm_handle_tx_t tx_handle_id,
                                           rflm_api_cmn_coex_ctrl_t *input_param );

/*----------------------------------------------------------------------------*/
/*!
  @brief get TQ based on RFM device 
*/
boolean
rflm_cmn_get_tq_pair 
( 
   uint8                       device  ,
   rflm_tech_id_t              tech    ,
   uint8*                      tq_pair 
) ;


/*----------------------------------------------------------------------------*/
/*!
  @brief Releases the TQP before going to sleep.
*/
boolean
rflm_cmn_release_tq_pair 
( 
   uint8                   device  ,
   rflm_tech_id_t          tech    
) ;


/*----------------------------------------------------------------------------*/
/*! 
  @brief Releases the TQP before going to sleep, with option to reserve device
    s.t. next request for the same device returns the same TQP.
*/
boolean
rflm_cmn_release_tq_pair_reserve_device
( 
   uint8                   device  ,
   rflm_tech_id_t          tech    ,
   boolean                 reserve_device    
) ;


/*----------------------------------------------------------------------------*/
/*! 
  @brief Unreserves device associated with TQPs NOT in use (intended for
    SLEEP -> EXIT transitions)
*/
void
rflm_cmn_disassociate_tq_pair_device
(
   rflm_tech_id_t          tech
);


/*----------------------------------------------------------------------------*/
/*! 
  @brief Get the TQP currently used by the source IRAT tech.
*/
boolean
rflm_cmn_get_irat_tq_pair 
( 
   uint8                   device  ,
   rflm_tech_id_t          tech    ,
   uint8*                  tq_pair 
) ;
/*----------------------------------------------------------------------------*/
/*! 
  @brief Enquire status of tqp usage
*/
boolean rflm_cmn_enquire_tqp_usage
( 
   uint8 device, 
   uint8 *tqpair_idx  
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief Interface for common FW to set up a split sample capture for ET
*/
rflm_err_t 
rflm_cmn_setup_et_split_cal
(
   rflm_set_et_split_cal_in_t *in,
   rflm_set_et_split_cal_out_t *out
);

void rflm_cmn_set_fbrx_coupler_input_directon(rflm_cmn_fbrx_coupler_input_direction_type coupler_direction);

boolean rflm_get_sefltest_capture_flag(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_CMN_H */
