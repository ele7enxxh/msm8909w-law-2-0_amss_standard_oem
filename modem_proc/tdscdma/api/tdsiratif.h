#ifndef TDSIRATIF_H
#define TDSIRATIF_H
/*===========================================================================

                    TDSCDMA IRAT INTERFACE

GENERAL DESCRIPTION
  This file contains the IRAT interface definitions for X to TDSCDMA.
 
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

 
  Copyright (c) 2000 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tdsiratif.h#1 $ 
$DateTime: 2016/12/13 07:58:19 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/05/11   bw      Initial version 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include "rfcom.h"
#include "rfmeas_types.h"
#include "rfmeas_lte.h"
#include "lte_irat_types.h"
#include "IxErrno.h"

#ifdef __cplusplus
}  // extern "C"
#endif


/*===========================================================================

                   MSGR UMID DEFINES

===========================================================================*/
#define MSGR_MODULE_IRAT                   0x09

#define MSGR_TDSCDMA_IRAT    MSGR_TECH_MODULE(MSGR_TECH_TDSCDMA, MSGR_MODULE_IRAT)

enum
{
  MSGR_DEFINE_UMID( TDSCDMA, IRAT, CMD, INIT, 0x0, 
                   tdsirat_init_cmd_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CNF, INIT, 0x0, 
                   tdsirat_init_cnf_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, REQ, STARTUP, 0x1, 
                   tdsirat_startup_req_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, REQ, ACQ, 0x2, 
                   tdsirat_acq_req_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, RSP, ACQ, 0x2, 
                    tdsirat_acq_rsp_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, REQ, MEAS, 0x3, 
                   tdsirat_meas_req_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, RSP, MEAS, 0x3, 
                   tdsirat_meas_rsp_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CMD, STOP, 0x4, 
                   tdsirat_stop_cmd_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CNF, STOP, 0x4, 
                   none),

  MSGR_DEFINE_UMID( TDSCDMA, IRAT, CMD, ABORT, 0x5, 
                   tdsirat_abort_cmd_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CNF, ABORT, 0x5, 
                   tdsirat_abort_cnf_type),


  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CMD, BUILD_RF_SCRIPTS, 0x0A, 
                   tdsirat_build_rf_scripts_cmd_type),


  MSGR_DEFINE_UMID( TDSCDMA, IRAT, CMD, L2T_INIT, 0x10, 
                   tdsirat_init_cmd_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CNF, L2T_INIT, 0x10, 
                   tdsirat_init_cnf_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, REQ, L2T_STARTUP, 0x11, 
                   tdsirat_startup_req_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, REQ, L2T_ACQ, 0x12, 
                   tdsirat_acq_req_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, RSP, L2T_ACQ, 0x12, 
                    tdsirat_acq_rsp_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, REQ, L2T_MEAS, 0x13, 
                   tdsirat_meas_req_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, RSP, L2T_MEAS, 0x13, 
                   tdsirat_meas_rsp_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CMD, L2T_STOP, 0x14, 
                   tdsirat_stop_cmd_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CNF, L2T_STOP, 0x14, 
                   none),

  MSGR_DEFINE_UMID( TDSCDMA, IRAT, CMD, L2T_ABORT, 0x15, 
                   tdsirat_abort_cmd_type),

  MSGR_DEFINE_UMID(TDSCDMA, IRAT, CNF, L2T_ABORT, 0x15, 
                   tdsirat_abort_cnf_type),

};


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define TDSIRAT_MAX_TDS_FREQ    1
#define TDSIRAT_MAX_MEAS_FREQ    2

#define TDSIRAT_MAX_TDS_CELLS_PER_FREQ 8
#define TDSIRAT_MAX_TDS_CELLS 128

typedef enum
{
  /*!0: initial state; XL1 should set this state to start ACQ on a TDS frequency */
  TDSIRAT_ACQ_NONE,
  /*!1: Final state. ACQ success on the corresponding frequency; 
  XL1 can start MEAS on this frequency.*/ 
  TDSIRAT_ACQ_SUCCESS,
  /*!2: Final state. ACQ failed on the corresponding frequency; 
  XL1 should not start MEAS on this frequency.*/ 
  TDSIRAT_ACQ_FAILURE,
  /*!3: Intermediate state. ACQ_step0 passed, 
  XL1 should pass this state back in the next ACQ_REQ to continue ACQ. */ 
  TDSIRAT_ACQ_STEP0_PASS,
  /*!4: Intermediate state. ACQ_step0 failed on the current LNA state, 
  XL1 should pass this state back in the next ACQ_REQ to continue ACQ on the next LNA state.*/ 
  TDSIRAT_ACQ_STEP0_FAIL,
  /*!5: Final state. RSCP measurment is completed on the corresponding frequency; 
  If this state is received in ACQ_RSP, it means ACQ is succeeded and RSCP measurment is done within the same gap.*/ 
  TDSIRAT_MEAS_DONE,
  /*!6: Intermediate init state. Collecting sample for ACQ_step1, 
  XL1 should set this state to start ACQ STEP1 after ACQ STEP0 pass. Only applicable in L2T PSHO */
  TDSIRAT_ACQ_COLLECT_SAMPLE,
  /*!7: Intermediate state. Processing sample for ACQ_step1, 
  XL1 should pass this state back in the next ACQ_REQ to continue ACQ STEP1 processing. Only applicable in L2T PSHO */
  TDSIRAT_ACQ_PROCESS_SAMPLE,
  /*!8: Intermediate init state. Collecting sample for RSCP measurement, 
  XL1 should set this state in MEAS_REQ to start RSCP measurement after ACQ success. Only applicable in L2T PSHO */
  TDSIRAT_MEAS_COLLECT_SAMPLE,
  /*!9: Intermediate state. Processing sample for RSCP measurement, 
  XL1 should pass this state back in the next MEAS_REQ to continue RSCP measurement processing on the same frequency. Only applicable in L2T PSHO */
  TDSIRAT_MEAS_PROCESS_SAMPLE,
  /*!10: Intermediate state. ACQ_step0 passed, but will try a new LNA state again
  XL1 should pass this state back in the next ACQ_REQ to continue ACQ. */ 
  TDSIRAT_ACQ_STEP0_PASS_TRYAGAIN,
} tdsirat_acq_state_e;

typedef enum
{
  TDSIRAT_RAT_GSM,
  TDSIRAT_RAT_LTE,
  TDSIRAT_RAT_NONE
} tdsirat_meas_rat_type_e;

/* TFW internal state info */
typedef struct{
    /*! \brief DC settling value to be passed from Step0 to Step1/2 */     
  struct 
  {
    /*! @brief To load EST1 I DC estimate value */
    uint32 est1_load_i;
    /*! @brief To load EST1 Q DC estimate value */
    uint32 est1_load_q;
    /*! @brief To load EST2 I DC estimate value */
    uint32 est2_load_i;
    /*! @brief To load EST2 Q DC estimate value */
    uint32 est2_load_q;
  }rxlmWbdcEst;
  /*! \brief To kickstart HW in settled state due to lack of time budget to allow settling */     
  uint32  hwinitDvgaBias; 
  /*! \brief Scaling used for converting samples in mempool from 20pcfl to linear format in lmem */
  uint32  dvgaBias;
  /*! \brief  Step1/2 sample activation time */
  uint32  activateTime;
  /*! \brief  LNA offset computed for rscp meas samples*/
  uint32  lnaOffset;
  /*! \brief Specifies the buffer - Ping or Pong - for processing measurement samples */
  uint8 bufferIndex; 
  /*! \brief 6 dB boost */
  uint8 vpeBiasFlag;
}tdsirat_tfw_state_info_type;

typedef struct
{

  /*! The source GSM frequency for the tuneback script */
  uint32 arfcn;

  /*! The source GSM band */
  rf_card_band_type band;
  
  /*! The rssi on the source frequency, in dBmx16 units */
  int32 rssi;
  //RF API change
  /*! RF serving rxlm buffer Index */
  uint32 serving_rxlm_buf_idx;
  uint32                     num_source_item;
  rfm_meas_param_type        source_param[MAX_SOURCE_ITEM]; 

} tdsirat_meas_gsm_rf_param_type;

typedef struct 
{
  lte_irat_l2x_tuneaway_param_s l2x_tuneaway_param;
  
  //rfm_meas_header_type rf_header;

  /*------------------ These will be removed ----------------*/

   /*!<  Device on which LTE will be tuned  */
   //rfcom_device_enum_type device; 

  /*!< Specify source technology band*/
   //uint32                source_band;

   /*!<  Channel on which LTE will be tuned, regardless of source or target tech  */
   //uint16                rx_channel;

   /*!<  Channel bandwidth on which LTE will use, regardless of source or target tech  */
   //rfcom_lte_bw_type     bw;  

  /*------------------------------------------------------------*/  

} tdsirat_meas_lte_rf_param_type;


typedef union 
{
  tdsirat_meas_gsm_rf_param_type    gsm_param;
  tdsirat_meas_lte_rf_param_type    lte_param;
} tdsirat_meas_rf_source_params_u;


/*! @brief Structure containing the RF parameters to populate to build
  the scripts
  */
typedef struct
{
  /*! source rat type */
  tdsirat_meas_rat_type_e src_rat;

  /*! RF startup buffer Index */
  uint16 startup_rf_idx;

  /*! RF startup buffer Index */
  uint16 cleanup_rf_idx;

  /*! Needs to be the appropriate RF type for tune back. */
  tdsirat_meas_rf_source_params_u source_parameters;

  /******************* RXLM Parameters ***************************/

  /*! Antenna 0 rxlm buffer for rxlm related targets 
   should be allocated with rxlm_allocate_buffer() and cast 
   to uint8
   */
  uint32 rxlm_rx_buffer;

} tdsirat_meas_rf_param_type;

typedef struct
{
  uint32                       uarfcn;
  uint16                       ts0_position;
  uint8                        last_lna_state;
  tdsirat_acq_state_e  last_acq_state;
  tdsirat_tfw_state_info_type tfw_state_info;
}tdsirat_acq_freq_type;

typedef struct
{
  tdsirat_acq_freq_type freq[TDSIRAT_MAX_TDS_FREQ];
  uint8                 num_of_freq;
} tdsirat_acq_list_type;

typedef struct
{

  msgr_hdr_struct_type hdr;

  tdsirat_acq_list_type acq_list;

} tdsirat_acq_req_type;

typedef struct
{
  uint16 cpid; /*ID TDS code 0..127*/
  int16 rscp; /* RSCP value of the cell */
  uint16 cell_position; /* not used */ 
} tdsirat_cell_type;

typedef struct
{
  uint32             uarfcn;
  boolean            ignore;
  uint32             measuredFreq; /* frequency Hz */
  int32              rssi; /* RSSI value on the frequency */
  uint8              num_of_cells; /* total number of cells returned */
  uint8              num_of_jds_cells; /* number of JDS cells for interference cancellation */
  uint8              lna_state; /* current LNA state */
  uint16             ts0_position; /* ts0 start position from step0 result */
  tdsirat_acq_state_e acq_state;
  tdsirat_cell_type  cell[TDSIRAT_MAX_TDS_CELLS_PER_FREQ];
  tdsirat_tfw_state_info_type tfw_state_info;
} tdsirat_freq_type;

typedef struct
{
  tdsirat_freq_type  freq[TDSIRAT_MAX_MEAS_FREQ];
  uint8              num_of_freq;
} tdsirat_freq_list_type;

typedef struct
{

  msgr_hdr_struct_type hdr;

  tdsirat_freq_list_type  freq_list;

} tdsirat_acq_rsp_type;


typedef struct
{

  msgr_hdr_struct_type hdr;
  
  tdsirat_freq_list_type freq_list;

} tdsirat_meas_req_type;


typedef struct
{

  msgr_hdr_struct_type hdr;
  
  tdsirat_freq_list_type  freq_list;

} tdsirat_meas_rsp_type;


typedef struct
{

  msgr_hdr_struct_type hdr;

} tdsirat_init_cmd_type;


typedef struct
{

  msgr_hdr_struct_type hdr;

  errno_enum_type err;

} tdsirat_init_cnf_type;


typedef struct
{

  msgr_hdr_struct_type hdr;

} tdsirat_stop_cmd_type;

typedef struct
{
  msgr_hdr_struct_type hdr;

  uint16 gap_start_subfn;
  uint16 gap_start_pos;    /*gap position in TDS Cx8*/

  uint16 gap_end_subfn;
  uint16 gap_end_pos; /*gap end in TDS Cx8 */

  tdsirat_meas_rf_param_type rf_param;

} tdsirat_startup_req_type;

typedef struct
{

  msgr_hdr_struct_type hdr;

} tdsirat_abort_cmd_type;


typedef struct
{

  msgr_hdr_struct_type hdr;

  errno_enum_type err;

} tdsirat_abort_cnf_type;

typedef struct
{
  msgr_hdr_struct_type hdr;

} tdsirat_build_rf_scripts_cmd_type;



#endif /* TDSIRATIF_H */

