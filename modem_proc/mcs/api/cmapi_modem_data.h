#ifndef CMAPI_MODEM_DATA_H
#define CMAPI_MODEM_DATA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
  @file   cmapi_modem_data.h
  @brief  This header file contains all the definitions necessary for clients 
          to interface with the Common Modem API module for modem data 
          collection.
*/

/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The cmapi_mainpage.dox file contains the group/module descriptions that
      are displayed in the output PDF generated using Doxygen and LaTeX. To
      edit or update any of the group/module text in the PDF, edit the
      cmapi_mainpage.dox file or contact Tech Pubs.
=============================================================================*/

/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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
=============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"   /* Definition for basic types and macros */
#include "cmapi_types.h"

#include <amssassert.h>
#include "sys.h"
#include "qsh.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/** System mode enumeration to be used for Call End data reporting
*/
typedef enum
{
  CMAPI_MODEM_DATA_SYS_MODE_UNKNOWN = 0,  /**< Unknown. */
  CMAPI_MODEM_DATA_SYS_MODE_GSM     = 1,  /**< GSM mode. */
  CMAPI_MODEM_DATA_SYS_MODE_UTRAN   = 2,  /**< WCDMA mode. */   
  CMAPI_MODEM_DATA_SYS_MODE_CDMA    = 3,  /**< CDMA mode. */
  CMAPI_MODEM_DATA_SYS_MODE_LTE     = 4,  /**< LTE mode. */
  CMAPI_MODEM_DATA_SYS_MODE_TDS     = 5,  /**< TD-SCDMA mode. */
  CMAPI_MODEM_DATA_SYS_MODE_INVALID = 255   /**< Invalid mode. */
}cmapi_modem_data_sys_mode_e_type;

/** Serving Cell Information structure to be used for modem data reporting
*/
typedef struct
{
  /** PLMN Id of the cell at the time of call end
  */
  sys_plmn_id_s_type plmn;
  
  /** Active RAT at the time of call end
  */
  cmapi_modem_data_sys_mode_e_type  sys_mode;
  
  /** Area Code of the serving cell.
  * For GSM, WCDMA, TDS: LAC (location area code)
  * For LTE: TAC (tracking area code)
  * For CDMA: REG_ZONE
  */
  uint16 area_code;
  /** Routing Area Code of the serving cell.
  */
  uint8 routing_area_code;
  /** Global Cell Id of the Serving Cell.
  * For GSM, CDMA, WCDMA, TDS: Cell Id
  * For LTE: Global Cell Id
  */
  uint32 cgi;
  /** RAT specific Cell Identifier of the Serving Cell.
  * For GSM, CDMA: INVALID
  * For WCDMA: PSC
  * For TDS: CP_ID
  * For LTE: PID (physical cell id)
  */
  uint16 psc_pid;
  /** Downlink frequency of the Serving Cell
  */
  uint16 arfcn;  
} cmapi_modem_data_cell_info_s;

/** Registration Status Information 
*/
typedef struct 
{
  /** Roaming Status Information to be used for Call End data reporting
  */
  /* Valid when there is full service
  ROAM_STATUS_OFF                      = 0
  ROAM_STATUS_ON                       = 1
  ROAM_STATUS_UNKNOWN                  = 255,
  */  
  uint8 roam_status;
  
  /** Serving Status Information to be used for Call End data reporting
  */
  /*  
  SRV_STATUS_NO_SRV                         = 0
  SRV_STATUS_LIMITED                        = 1
  SRV_STATUS_SRV                            = 2
  SRV_STATUS_LIMITED_REGIONAL               = 3
  SRV_STATUS_PWR_SAVE                       = 4

  */
  uint8 srv_status;
  
  /** Domain Information to be used for Call End data reporting
  */
  /*
  SRV_DOMAIN_NONE         = -1
  SRV_DOMAIN_NO_SRV       = 0
  SRV_DOMAIN_CS_ONLY      = 1
  SRV_DOMAIN_PS_ONLY      = 2
  SRV_DOMAIN_CS_PS        = 3
  SRV_DOMAIN_CAMPED       = 4
  */
  uint8 srv_domain;
} cmapi_call_registration_status_s; 

/** Call Information structure to capture data for Call End report
*/  
typedef struct
{
  cmapi_call_type_e                        type;
  cmapi_call_state_e                       state;
  cmapi_call_registration_status_s         reg_status;
  sys_rab_status_e_type                    rab_status[4];
  sys_call_end_event_e_type                end_event;
  uint8                                    end_cause;
} cmapi_modem_data_call_info_s;

/** Structure for BLER information
*/
typedef struct
{
  uint32 num_blks_rcvd;
  uint32 num_err;
} cmapi_modem_data_bler_info_s;
  
/** GSM L1 information to be reported for Call End
*/
typedef struct
{
  struct {
    /*TCH RxLev Full and Sub on Primary Antenna in dBx16*/
    int32                 tch_pri_rxlev_full;
    int32                 tch_pri_rxlev_sub;
  } rssi0_dBm;

  struct {
    /*TCH RxLev Full and Sub on Secondary Antenna in dBx16*/
    int32                 tch_sec_rxlev_full;
    int32                 tch_sec_rxlev_sub;
  } rssi1_dBm;

  struct {
    /*TCH RxQual Full and Sub (Combined , not Antenna Specific)*/
    uint8                 tch_rxqual_full;
    uint8                 tch_rxqual_sub;
  } snr;

  int16                   tx_pwr;

} cmapi_modem_data_gsm_l1_info_s;


/** WCDMA L1 information to be reported for Call End
*/
typedef struct
{
  /** RSSI on primary antenna (in dBm)
  */
  int32 rssi0_dBm;
  
  /** RSSI on diversity antenna (in dBm)
  */
  int32 rssi1_dBm;
  
  /** RSCP on primary antenna (in dBm)
  */
  int32 rscp0_db;
  
  /** RSCP on diversity antenna (in dBm)
  */
  int32 rscp1_db;
  
  /** EcIo on primary antenna (Divide by two and take minus to get dB)
  */
  int32 ecio0_db;
  
  /** EcIo on diversity antenna (Divide by two and take minus to get dB)
  */
  int32 ecio1_db;
  
  /** Signal-to-Noise Ratio 
  * convert to dB (snr * log(2)/10)
  */
  int32 snr;
  
  /** BLER information
  */
  cmapi_modem_data_bler_info_s bler;
  
  /** Tx Power Information (in dBm)
  */
  int8 avg_tx_pwr;
} cmapi_modem_data_wcdma_l1_info_s;

/** CDMA L1 information to be reported for Call End
*/
typedef struct
{
  /** RSSI on primary antenna (in dBm)
  */
  uint16 rssi0_dBm;
  
  /** RSSI on diversity antenna (in dBm)
  */
  uint16 rssi1_dBm;
  
  /** EcIo on primary antenna (in dB)
  */
  uint16 ecio0_db;

  /** EcIo on diversity antenna (in dB)
  */
  uint16 ecio1_db;

  /** Tx Power Information (in dB)
  */
  int16 tx_pwr;
} cmapi_modem_data_cdma_l1_info_s;

/** LTE L1 information to be reported for Call End
*/
typedef struct
{
  /** RSSI on primary antenna (in dBm)
  */
  int32 rssi0_dBm;
  
  /** RSSI on diversity antenna (in dBm)
  */
  int32 rssi1_dBm;
  
  /** RSRP on primary antenna (in dB)
  */
  int32 rsrp0_db;
  
  /** RSRP on diversity antenna (in dB)
  */
  int32 rsrp1_db;
  
  /** RSRQ on primary antenna (in dB)
  */
  int32 rsrq0_db;
  
  /** RSRQ on diversity antenna (in dB)
  */
  int32 rsrq1_db;
  
  /** Signal-to-Noise Ratio on primary antenna
  */
  int32 snr0;
  
  /** Signal-to-Noise Ratio on primary antenna
  */
  int32 snr1;
  
  /** BLER information
  */
  cmapi_modem_data_bler_info_s bler;
  
  /** Tx Power Information (in dBm)
  */
  int8 avg_tx_pwr;
} cmapi_modem_data_lte_l1_info_s;


/** TDS L1 information to be reported for Call End
*/
typedef struct
{
  /** RSSI on primary antenna (in dBm)
  */
  int8 rssi0_dBm;
  
  /** RSCP on primary antenna (in dBm)
  */
  int16 rscp0_dBm;
  
  /** Signal-to-Noise Ratio on primary antenna (in 1/256 dB)
  */
  int16 snr0;
  
  /** BLER information
  */
  cmapi_modem_data_bler_info_s bler;
  
  /** Tx Power Information (in dBm)
  */
  int8 avg_tx_pwr;
} cmapi_modem_data_tds_l1_info_s;


/** Union to maintain L1 information for all RATs
  */
typedef union
{
  cmapi_modem_data_cdma_l1_info_s   cdma_l1;
  cmapi_modem_data_gsm_l1_info_s    gsm_l1;
  cmapi_modem_data_wcdma_l1_info_s  wcdma_l1;
  cmapi_modem_data_lte_l1_info_s    lte_l1;
  cmapi_modem_data_tds_l1_info_s    tds_l1;
} cmapi_modem_data_tech_info_u;

/** Data structure to gather information provided to the caller in 
    cmapi_modem_data_get())
  */
typedef struct
{
  cmapi_modem_data_cell_info_s cell_info;
  cmapi_modem_data_call_info_s call_info;
  cmapi_modem_data_tech_info_u tech_info;  
} cmapi_modem_data_call_end_s_type;

/** Enumeration for events that clients can register with CMAPI for.
  */
typedef enum
{
  CMAPI_MODEM_DATA_EVENT_CALL_END
} cmapi_modem_data_event_e_type;

/** Structure for parameters to be provided to client for registered events
  * when the callback is invoked.
  */
typedef struct
{
  sys_modem_as_id_e_type         subs_id;
  cmapi_modem_data_event_e_type  event;
} cmapi_modem_data_event_s_type;

/** Callback type for QMI to receive notification of events
*/
typedef void (*cmapi_modem_data_event_cb_t)(cmapi_modem_data_event_s_type *);

/** Data structure for call end input parameters to  
    cmapi_modem_data_cfg()
  */
typedef struct 
{
  /** Flag to enable/disable call end data collection in CMAPI.
    */
  boolean                                 enable;
  
  /** Memory location in client space to write the call end data. 
    * Client will manage this memory by allocating it when enabling
    * call end data collection and shall free it when disabling
    * the data collection.
    */
  cmapi_modem_data_call_end_s_type*       cb_data_write_ptr;
} cmapi_modem_data_cfg_call_end_params_s;

/** Overall data structure for input parameters to  
    cmapi_modem_data_cfg()
  */
typedef struct 
{
  cmapi_modem_data_event_cb_t            event_cb;
  cmapi_modem_data_cfg_call_end_params_s call_end_params;
  /* Note: For CMAPI internal use only */
  uint32                                 internal_use;
} cmapi_modem_data_cfg_params_s_type;

/*===========================================================================

                    FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
  FUNCTION:  cmapi_modem_data_cfg_init
===========================================================================*/
/** @ingroup cmapi_modem_data_cfg_init 
  Initializes the config parameter structure for modem data collect.

  @datatypes 
  #cmapi_modem_data_config_init_params_s * \n
  
  @params[in] pointer to the config structure to be initialized..
  
  @return
  None

  @dependencies
  None.
*/
void cmapi_modem_data_cfg_init
(
  cmapi_modem_data_cfg_params_s_type *params
);

/*===========================================================================
  FUNCTION:  cmapi_modem_data_cfg
===========================================================================*/
/** @ingroup cmapi_modem_data_cfg 
  Configures data collection across different RATs which can be reported back
  in cmapi_modem_data_get()

  @datatypes 
  #cmapi_modem_data_cfg_params_s_type \n
  
  @params[in] pointer to a structure containing the config params for 
              modem data collect.
  
  @return
  cmapi_err_e_type: Enum for error 

  @dependencies
  cmapi_modem_data_cfg_init() should be called to initialize the parameters
  provided.
  
  This API should be called only after the UE has acquired service after 
  power up.
  
  @notes
  Calling this function with enable = TRUE when data collection is already
  configured will return a CMAPI_INVALID_PARAM error.
  
*/
cmapi_err_e_type cmapi_modem_data_cfg
(
  cmapi_modem_data_cfg_params_s_type *params
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*CMAPI_MODEM_DATA_H*/