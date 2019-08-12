/*!
  @file
  gfw_rf_intf.h

  @brief
  Generic interface for RF script for IRAT

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/xx/11   NB      Added immediate GRFC interface
09/02/10   NB      Increase #SBI in interface, add values for invalid times 
03/05/10    NB     Change RF interface to include GERAN interface 
                   to support legacy GERAN scripts
===========================================================================*/

#ifndef GFW_RF_IRAT_INTF_H
#define GFW_RF_IRAT_INTF_H
/*===========================================================================
                      RF script format        
===========================================================================*/


/*! @brief Immediate programming of SBI or GRFC (no delta) */
#define FW_RF_IRAT_IMM_SBI_GRFC_DELTA 0

/*! @brief Invalid delta value (example if you want to leave GRFC on always) */
#define FW_RF_IRAT_INVALID_SBI_GRFC_DELTA 0x7FFF

typedef enum 
{
   GSM_TIME_REF,
   WCDMA_TIME_REF,
   CDMA_TIME_REF,
   LTE_TIME_REF
}rfmeas_time_ref_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Single SBI data unit for passing SBI infor to WCDMA, LTE, and DO FWs.

*/

/*!
  @brief
  Define the maximum number of separate SBI buffers used, one per unique TQ
  (for parallel scheduling of SBI tasks belonging to different SBI masters)
*/
#define MAX_NUM_SBI_BUFS                   1

/*!
  @brief
  Define the number of SBI units per buffers used, one per unique TQ
  (for parallel scheduling of SBI tasks belonging to different SBI masters)
*/
#define MAX_NUM_OF_SBI_BUF0                   230
#define MAX_NUM_OF_SBI_BUF1                   20

/*!
  @brief local macro for maximum of 2 static numbers
*/
#define FW_RF_IRAT_MAX_OF_2NOS(a,b)  ( ( (a)>(b) )? (a): (b) )


#define MAX_NUM_OF_NON_TIME_SBI    300      

typedef struct
{
   /*! @brief  SBI transaction time relative to the burst  */
  union
  {
    int16     start_delta;  //union of the types used by Software and firmware
    int16     startDelta;
  };
 
   /*! @brief  SBI register */
  union
  {
    uint8     data;
    uint8     regData;
    uint8     reg_data;
  };
 
   /*! @brief  SBI address */
  union
  {
    uint8  addr;
    uint8 regAddr;
    uint8 reg_addr;
  };
 
   /*! @brief  SBI mode control. May be used for the 2 msb of SSBI 2.0 on RFD targets */
  union
  {
   uint8     iq_mode;
   uint8     iq_mode_slave_id;
   uint8 sbiMode;   
  };
  
   /*! @brief  SBI  bus */
  union
  {
    uint8     sbi_bus;
    uint8     ctl_fifo;
    struct
    {
      uint8 sbiMasterId     :3;
      uint8 waitTimeRef    :1;
    };
  };
} rfmeas_sbi_type;


/*!
  @brief
  SBI data structure for passing SBI script techPtrs are use to match GFW SBI
  data structure. Gl1 can take this structure and cast as legacy GFW SBI structure 
  without any alignment issues.

*/
typedef struct
{
  uint16  sbi_counter;
  rfmeas_sbi_type  sbi_buffer[ MAX_NUM_OF_NON_TIME_SBI ];
} rfmeas_sbi_param_type;


/*!
  @brief
  GRFC data structure for passing immediate GRFC's as bit mask to FW.
*/
typedef struct 
{
  /*! @brief Bitmask of GRFC's to be activated immdieately */
  uint32 grfc_act_imm;
  /*! @brief Bitmask of GRFC's to be deactivated immdieately */
  uint32 grfc_deact_imm;
  /*! @brief Bitmask of RF GRFC's to be activated immdieately */
  uint32 grfc_rf_act_imm;
  /*! @brief Bitmask of RF GRFC's to be deactivated immdieately */
  uint32 grfc_rf_deact_imm;
} rfmeas_imm_grfc_param_type;

/*!
  @brief
  GRFC data structure for passing nonSBI script to FW.
  Warining:Note that the start and stop logic in the interface
  is not tested.
*/
typedef struct
{
  /*! @brief GRFC engine Id to be activated/de-activated */
  uint16 grfc_id;
  /*! @brief start time offset for activate*/
  uint16 start_delta;
  /*! @brief stop time offset for de-activate */
  uint16 stop_delta;
  /*! @brief type of action.Activate/deactivate */
  uint8 start_logic;
  /*! @brief type of action.Activate/deactivate */
  uint8 stop_logic;
} rfmeas_grfc_type;

#define RFMEAS_MAX_NUM_OF_GRFC  44

/*!
  @brief
  GRFC parameter data structure for FW.

*/
typedef struct
{
   
  /*! @brief  Number of GRFC */
   uint16  num_grfc;

  /*! @brief  union of GRFC time reference and techSpecific 
      pointer (currently used only by GFW) */
  union 
  {
    rfmeas_time_ref_type ref;
    void *techPtr1;
  };

  /*! @brief  GRFC buffer */
   rfmeas_grfc_type grfc_buffer[ RFMEAS_MAX_NUM_OF_GRFC ];
} rfmeas_grfc_param_type;

/*!
  @brief
  SPI data structure used in FW RF script.

*/
typedef struct
{
  uint32 spi_reg_addr;
  uint32 spi_reg_value;
} rfmeas_spi_type;

#define MAX_NUM_OF_SPI_REG     20
/*!
  @brief
  RXF data structure for passing nonSBI script to WCDMA, LTE, and DO FWs.

*/
typedef struct
{
  uint32 num_spi_reg;
  rfmeas_spi_type spi_reg[ MAX_NUM_OF_SPI_REG ];
} rfmeas_spi_param_type;

/*!
  @brief
  Single RXF data unit for passing nonSBI script to WCDMA, LTE, and DO FWs.

*/
typedef struct
{
  uint32 rxf_reg_addr;
  uint32 rxf_reg_value;
} rfmeas_rxf_type;

#define MAX_NUM_OF_RXF_REG     20
/*!
  @brief
  RXF data structure for passing nonSBI script to WCDMA, LTE, and DO FWs.

*/
typedef struct
{
  uint32 num_msm_reg;
  rfmeas_rxf_type rxf_reg[ MAX_NUM_OF_RXF_REG ];
} rfmeas_rxf_param_type;


#define MAX_NUM_OF_ANT_TUNE_SPI_DEV 4
/*!
  @brief
  Antenna tuner SPI information Data structure.
*/
typedef struct
{
  uint8 cs_n[MAX_NUM_OF_ANT_TUNE_SPI_DEV];             /* CS_N for each according codeword */
  uint32 tune_info[MAX_NUM_OF_ANT_TUNE_SPI_DEV];       /* array of total max of 4 devices */
} rfmeas_spi_ant_tuner_param_type; 

/*!
  @brief
  Data structure for passing nonSBI script to WCDMA, LTE, and DO FWs.
  FW should have the same structure. This structure is cache aligned
  for FW to clean cache before reading, and SW to flush its cache 
  after writing
*/

typedef struct
{
  rfmeas_imm_grfc_param_type imm_grfc_infor; /* Immediate GRFC mask */
  rfmeas_grfc_param_type   grfc_infor; /* GRFC mask and value */
  rfmeas_sbi_param_type    sbi_infor[MAX_NUM_SBI_BUFS];  /* SBI information */
  rfmeas_rxf_param_type    rxf_infor;  /* MSM reg and data */
  rfmeas_spi_param_type    spi_infor;  /* This is obsolete now.To be removed after approval from RF */
  rfmeas_spi_ant_tuner_param_type tuner_spi_infor; /*tuner info*/
} ALIGN(32) rfmeas_param_infor_type;


/*! @brief MACRO for converting buffer id to script address. Any module wanting
 * to use this API must also include modem_fw_memmap.h */
#define GFW_IRAT_SCRIPT_ADDR(buf_id) \
  (FW_SMEM_IRAT_SCRIPT_ADDR + buf_id*sizeof(rfmeas_param_infor_type))

/*! @brief MACRO for converting buffer id to script offset. Currently SW and 
 * FW memory maps are aligned and the shared memory is idempotent, in
 * case that changes, SW can still use just the offset API */
#define GFW_IRAT_SCRIPT_OFF(buf_id) \
  (buf_id*sizeof(rfmeas_param_infor_type))

#endif

