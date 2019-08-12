#ifndef MDSP_CMD_H
#define MDSP_CMD_H
/*===========================================================================
                          M D S P _ C M D . H


GENERAL DESCRIPTION
  This header file contains macros and definitions for the mDSP driver
  commands.
  *** This file is an internal file to the mDSP driver ***

  Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmdsp6/inc/mdsp_cmd.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when         who     what, where, why
----------  -------- ------------------------------------------------------
10/03/13     ka      CR628885 Remove unused functions
22/02/13     cja     CR455527 Remove featurisation from files in API directory
04/01/13     cja     CR437603 Remove use of AsyncImmediateCmd, always use AsyncCmd
20/06/12     ky      Added support of using new CCS interface and passing new CCS
                     buffer pointers in RF task messages
08/06/12     cja     CR363269 Do not clear IRAT SBI/NonSBI buffers
16/01/12     ky      CR 336812 mdsp_nv_recovery_restart_type is defined and this structure is pouplated after
                     reading nv item from efs. if the recovery restart is enabled then mdsp drivers
                     ignore the errors other wise it does error fatal and also dump the mdsp memory.
                     mdspsvc provided api to update this nv item read value. so updated this value
                     during mdsp_init
27/03/12     cja     CR338055 Recovery for Nikel RxFE Stuck
26/03/12     jj      CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
09/09/11     jj      CR 304819 correction
02/09/11     jj      CR 304819 Add interface to update smps pdm to dynamic tx buffer
31/08/11     jj      CR 304393 Dynamic tx path delay set interface for rf driver
20/04/11     cja     Nikel remove PA transition ramp
11/04/11     pg      CR282109: Ensure AEQ PH2 switching only occurs at a block boundary
02/02/11     cja     Changes for Nikel
05/04/11     ky      282111: For sequence num mismatch if recovery nv item is set then ignore it other
                     wise trigger halt
11/01/11     ky      MDSP Halt recovery fixes after testing on target
22/11/10     ky      mdsp_nv_recovery_restart_type is defined and this structure is pouplated after
                     reading nv item from efs. if the recovery restart is enabled then mdsp drivers
                     ignore the errors other wise it does error fatal and also dump the mdsp memory.
                     mdspsvc provided api to update this nv item read value. so updated this value
                     during mdsp_init
14/10/10     ws      FEATURE_GSM_MDSP_AEQ_8PSK - support AEQ Ph2
03/09/10     ws      Added FEATURE_GSM_MDSP_AEQ_SCH for QDSP6 targets
10/05/10     cja     Optimise shared memory buffers to allow 38 SBI transactions in compressed mode.
11-03-10     ky      CR 229455-L1 to print out mDSP PC info when mDSP stall
04/02/09     ws      Removed leagcy Q4 MDSP services and image for Q6 targets
09/11/09     ky      Added new function for q6 logging commands
30/09/09     ws      Renamed mdsp_issue_command2 to mdsp_issue_qdsp6_command
05/08/09     ws      Removed FEATURE_GSM_MDSP_DTM it's now mainlined
26/05/09     cs      CR167848 fix for expanded afc_freq I/F to mDSP
29/05/09     ws      FEATURE_GSM_MDSP_AEQ phase 1 for all GMSK bursts
30/03/09     ws      Added FEATURE_GSM_GPRS_QDSP6 and FEATURE_GSM_GPRS_POSEIDON
                     to replace FEATURE_GSM_GPRS_MUSTANG
26/03/09     ws      Added PACKED_POST for QDSP6 targets
24/11/08     og      FEATURE_GSM_GPRS_MSC33 support.
09/09/08     agv     Added GtW interface change for MUSTANG.
01/08/08     agv     Changes for LCU G2W Interface Change for HW clock bug.
19/03/08     agv     LCU RF Spur Interface Support.
18/01/08     agv     LCU RF Linear PA Interface Support.
14/01/08     agv     New feature for Switched Mode Power Supply which will
                     provide better talk time.
22/10/07     ws      Add support for ESAIC NV 2508 item control
03/23/06     ws      Added Support for SDA algorithm SAIC Disable Algorithm
02/20/06     gfr     Mainline command and apps logging features, add
                     support for mDSP services apps logging
01/19/06     gfr     Increase number of parameters to mdsp_process_error,
                     add new mDSP error types
12/09/05     gfr     Added mDSP NV item structure to hold loading NV item data
11/07/05     gfr     Added internal functions to replace global variables
10/17/05     gfr     Add mdsp_saic flag
10/18/05     gfr     Add support for having the mDSP do all ciphering
10/11/05     gfr     Added mdsp_dm_pdtch_decode_done to sanity check the counter
09/23/05     gfr     Optimize DM active period to just during PDTCH decoding
09/22/05     gfr     Removed FEATURE_SBI_MDSP_CONTROLLER
08/19/05     gfr     Added mdsp_burst_current_buffer
08/17/05     gfr     Added access functions for mdsp_cmd_len_struct
08/05/05     gfr     Added SAIC control words to DTM rx command
07/29/05     gfr     DTM support
06/09/05     gfr     Add mdsp_log_grfc_write
05/25/05     gfr     Update prototype for mdsp_log_cmds
05/18/05     gfr     Clean up includes, remove old DTM code
05/10/05     gfr     Mainline FEATURE_GSM_MDSP_EER
05/04/05     yh      Add mdsp warning error type
04/28/05     gfr     Add support for the DM active interrupt
04/08/04     gfr     Added mdsp_fw_version
03/22/05     gfr     Renamed FEATURE_POLAR_RF to FEATURE_POLAR
03/14/05     yh      Support for mdsp apps log.
03/08/05     gfr     Add prototype for mdsp_allow_reset
01/28/05     gfr     Add configure_IR_window command.
01/28/05     yh      Deleted the signals MDSP_DOWNLOAD_WAIT_SIG and
                     MDSP_GSM_DOWNLOAD_TIMEOUT_SIG as they will be passed
                     in by GPL1
01/25/05     gfr     Add support for the polar PA transition profile
01/24/05     gfr     General cleanup and restructure.
12/09/04     yhong   Added GPRS TX struct to log into a trace buffer
12/07/04     gfr     Change signals being used to avoid conflict with FTM.
12/06/04     gfr     Featurize the mDSP SBI controller
11/30/04     gfr     Support for polar RF and EER programming.
09/08/04     gfr     Make the length of the debug cmd trace a define.
07/26/04     gfr     General cleanup, fixed rx burst offset.
06/10/04     gfr     Removed unused AMR commands.
05/20/04     gfr     Increased command trace data length to 30 to fit all cmds.
03/18/04      jc     Increased  MDSP_SBI_RF_ON_NUM_TRANS to 19, more bbrx settings
03/18/04      bm     Increased  MDSP_SBI_RF_ON_NUM_TRANS to 18, due to new bbrx settings
03/05/04      gfr    Lint cleanup and removed uwire code.
02/17/04      gfr    GSM rotators support.
1203/03       jc     Bring in PSBI offset to 4250 qs.
11/05/03      jcw    Added a pass/fail return value to mdsp_check_sbi_semaphores()
08/06/03      gw     Moved #define of MDSP_GEN_CONFIG_MAX_BUF_LEN to mdsp_intf.h.
07/02/03      jtn    Moved SBI #defines out of this file into mdsp_intf.h so RF code is
                     not dependent on this file for those #defines.
06/29/03      jtn    Removed the #define for FEATURE_RF_MULTIBYTE_SBI so it can be
                     enabled or disabled in the appropriate file, not here
06/29/03      jtn    Added MULTI defines for SBI multibyte transfers enabled when
                     FEATURE_RF_MULTIBYTE_SBI is defined, as it is now.
06/03/03      gfr    Added offset parameter to sbi access command.
05/21/03      gw     Added definitions for clear_deinterleaver command.
05/12/03      gfr    Added tx_alpha parameter to transmit command.
05/08/03      jc     Changes for RF restructure.
05/02/03      hg     Added GSM AMR support.
03/24/03      jc     Changed MAX_SBI_TRANSACTIONS from 6 to 8.
                     Changed MAX_NONSBI_TRANSACTIONS from 10 to 8.
03/04/03      gfr    Increased size of cmd trace log structure.
03/04/03      jc     Fix the stop_tx cmd for phoneT.
12/13/02      sd     Increased RF start and stop SBI transaction count
                     constants for GTOW.
12/11/02      thh    Renamed QDSP_CMD_SUCCESS/FAILED to MDSP_CMD_SUCCESS/FAILED.
12/11/02      ih     Featurized MDSP_SBI_CTL_FIFO_TIMED to support multi-byte
                     SBI transactions for RF bursts
11/12/02      jc     Added keyword PACKED to unions.
11/03/02      tb     Added support for INTERRAT_GTOW
10/28/02      gw     Moved around some #defines relating to buffer sizes.
10/14/02      jc     Added MAX_CM_RSSI_MEASUREMENTS.
10/03/02      ih     Added MDSP_UWIRE_CTL_FIFO_IMM MDSP_UWIRE_CTL_FIFO_TIMED
                     to support both access and burst uwire transactions.
09/25/02      ih     Added structure for SBI status buffer
09/09/02      gw/ih  Added defintions to support static SBI/nonSBI buffers.
08/22/02      jc     Put stop_test_tx back.  Was a needed phoneT command.
08/19/02      jc     Removed unused phoneT structs.
08/12/02      thh    Changed MDSP_POWER_TIME_OFFSET_3 to 3450 to accomodate the
                     long synthesizer settling time for the B1 revision.
08/07/02      jc     Add sbi_buffer to cm_startup cmd. Increase MAX_SBI_TRANACTIONS
                     to 6.
06/05/02      gw     Moved #define of MDSP_SYNC_RX_FIFO_LEN here from
                     mdsp_intf.c so it can be used in mdsp_gprs.c too.
05/20/02      jc     Added support for compressed mode
05/08/02      thh    Added structure definition for new SBI results buffer.
04/11/02      jc     Added MDSP_MEM_SEG_x.
03/04/02      thh    Added support for Txprofile and SBI's new API.
                     Also, increase duration between uwire writes to 100 qs.
                     This needs to be reduced when there is time.
02/04/02      jc     Some cleanup.
02/04/02      mk     Increased value for _OFFSET_3.
10/30/01      jc     Initial version.
===========================================================================*/

#include "geran_variation.h"

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "mdsp_intf.h"
#include "mdsp_intf_g.h"
#include "nv.h"

/****************************************************************************
****************************************************************************
****************************************************************************
                          DEFINITIONS AND ENUMERATIONS

****************************************************************************
****************************************************************************
***************************************************************************/

/* default number of quarter symbols before start of burst */
#define PHONET_DEFAULT_TX_TIME_OFFSET               1875
#define PHONET_DEFAULT_RX_TIME_OFFSET               1280
#define MDSP_DEFAULT_TX_TIME_OFFSET                 1875

/* default number of symbols the mdsp will use as a window around the burst */
#define MDSP_DEFAULT_RX_SEARCH_WIDTH                12

/* Number of quarter symbols before center of burst */
#define MDSP_RX_BURST_SIZE                          150
#define MDSP_DEFAULT_RX_TIME_OFFSET   (FRAME_TICK_SETUP_MARGIN_QS + ((MDSP_RX_BURST_SIZE * 4) / 2))

/* Definitions associated with downloading the firmware image */
#define MDSP_MAX_DOWNLOAD_TIME_MS       1000

/* TX data mode for various mdsp transmit commands */
typedef enum
{
  TX_DATA_FROM_BUFFER,
  TX_DATA_PSEUDO_RANDOM,
  TX_DATA_TONE,
  FORCEWORD_TX_DATA_MODE_TYPE = 0xffff
} tx_data_mode_type;


#define MAX_FW_SETUP_TRANSACTIONS   1

typedef PACKED struct PACKED_POST
{
  uint16            id;
  uint16            data;
} fw_setup_ctl_unit;

/* Structure to be filled when used with MDSP_POWER_MEASURE_CMD */
typedef PACKED struct PACKED_POST
{
  uint16              num_transactions;
  fw_setup_ctl_unit   fw[MAX_FW_SETUP_TRANSACTIONS];
} fw_setup_request_struct;


/* Stored Tx power control information from the RF driver */
typedef struct
{
#ifdef FEATURE_POLAR
   /* Polar RF PA ramp table */
   mdsp_pa_ramp_type *pa_ramp_table_polar;
   boolean new_pa_ramp_table_polar;

   /* Polar RF AM/AM and AM/PM tables */
   mdsp_amam_ampm_tables_type *amam_ampm_tables;
   boolean                    new_am_am_table;
   boolean                    new_am_pm_table;
#else
   /* Linear RF PA ramp table */
   mdsp_pa_ramp_type *pa_ramp_table_linear;
   boolean new_pa_ramp_table_linear;
   uint16 pa_index[4];
#endif
   uint16  mdsp_linear_tx_gain_buff_updated;
   boolean tx_path_delay_updated;
   int16   tx_path_delay_val;
   boolean tx_smps_pdm_value_updated;
   int16   tx_smps_pdm_value;
} mdsp_tx_power_control_struct;



/****************************************************************************
 ****************************************************************************
 ****************************************************************************
                           MDSP COMMANDS

 ****************************************************************************
 ****************************************************************************
 ***************************************************************************/
#define MAX_MDSP_CMD_TYPES       MDSP_CMD_invalid_cmd_type


/****************************************************************************
                            PRIVATE SHARED TYPES
 ***************************************************************************/
/* Different types of mdsp errors - used for logging */
typedef enum
{
   MDSP_GSM_ERROR_FORCED                = 0,
   MDSP_GSM_ERROR_HALTED                = 1,
   MDSP_GSM_ERROR_CMD_NOT_PROCESSED     = 2,
   MDSP_GSM_ERROR_RECOVER               = 3,
   MDSP_GSM_ERROR_SBI_NOT_CLEARED       = 4,  /* obselete */
   MDSP_GSM_ERROR_NONSBI_NOT_CLEARED    = 5,  /* obselete */
   MDSP_GSM_ERROR_INVALID_POWER         = 6,
   MDSP_GSM_ERROR_INVALID_METRICS       = 7,
   MDSP_GSM_ERROR_INVALID_DATA          = 8,
   MDSP_GSM_ERROR_INVALID_SCH           = 9,
   MDSP_GSM_ERROR_INVALID_FCCH          = 10,
   MDSP_GSM_ERROR_SLEEP                 = 11,
   MDSP_GSM_ERROR_WARNING               = 12,
   MDSP_GSM_ERROR_BURST_BUF_NOT_CLEARED = 13,
   MDSP_GSM_ERROR_GRFC_BUF_NOT_CLEARED  = 14,
   MDSP_GSM_ERROR_NONSBI_BUF_OVERFLOW   = 15,
   MDSP_GSM_ERROR_SBI_BUFFER_OVERFLOW   = 16,
   MDSP_GSM_ERROR_SEQ_NUM_MISMATCH      = 17,
   MDSP_GSM_ERROR_RF_EVENT_SBI_TRANSITION_BUF_NOT_CLEARED = 18,
   MDSP_GSM_ERROR_RF_EVENT_NONSBI_TRANSITION_BUF_NOT_CLEARED = 19,
   MDSP_GSM_ERROR_RF_EVENT_SBI_STOP_BUF_NOT_CLEARED = 20,
   MDSP_GSM_ERROR_RF_EVENT_NONSBI_STOP_BUF_NOT_CLEARED = 21,
   MDSP_GSM_ERROR_RF_EVENT_SBI_START_BUF_NOT_CLEARED = 22,
   MDSP_GSM_ERROR_RF_EVENT_NONSBI_START_BUF_NOT_CLEARED = 23
} mdsp_gsm_error_type;


/* Structure used to hold the results of loading mDSP NV item - this is
   used since NV items must be loaded in mdsp_download_and_startup but
   processed in mdsp_init. */
typedef struct
{
  nv_stat_enum_type  status;
  nv_item_type       item;
} mdsp_nv_item_type;

/****************************************************************************
                            PRIVATE SHARED VARIABLES
 ***************************************************************************/
extern uint16 mdsp_sync_rx_time_offset;
extern uint16 mdsp_sync_tx_time_offset;

extern boolean mdsp_compressed_mode;
#ifdef FEATURE_GSM_MDSP_IR
extern int16  mdsp_dm_pdtch_decodes;
#endif
extern boolean mdsp_saic;

#ifdef FEATURE_GSM_MDSP_ESAIC
extern boolean mdsp_esaic;
#endif

extern boolean mdsp_aeq;

#ifdef FEATURE_GSM_MDSP_AEQ_8PSK
extern boolean mdsp_aeq_8psk;
#ifdef FEATURE_GSM_DYNAMIC_AEQ_8PSK
extern boolean mdsp_aeq_8psk_dynamic;
#endif /* FEATURE_GSM_DYNAMIC_AEQ_8PSK */
#endif

#ifdef FEATURE_GSM_MDSP_AEQ_SCH
extern boolean mdsp_aeq_sch;
#endif
extern mdsp_nv_item_type mdsp_nv_mem_dump_enabled;

/****************************************************************************
                            PRIVATE FUNCTION PROTOTYPES
 ***************************************************************************/

void mdsp_issue_qdsp6_command (GfwCmdBufType *cmd, GfwMdspCmdType  cmdId , gas_id_t gas_id );

uint16 mdsp_get_current_seq_num (gas_id_t gas_id);
void mdsp_apps_log (void);
boolean mdsp_allow_reset(void);
void mdsp_sleep_init(gas_id_t gas_id );
void mdsp_sleep_inact(void);
void mdsp_set_cmd_len (uint16 cmd, uint16 len);
uint16 mdsp_cmd_len (uint16 cmd);
void mdsp_clear_cmd_len (void);
uint8 mdsp_burst_current_buffer (mdsp_burst_type burst , gas_id_t gas_id );
#ifdef FEATURE_GSM_MDSP_IR
void mdsp_dm_disable (gas_id_t gas_id );
void mdsp_dm_pdtch_decode_done (boolean completed);
#endif
uint16 mdsp_ftsm (gas_id_t gas_id);

#if defined(FEATURE_GSM_MDSP_IR)
void mdsp_init_dm (void);
#endif
uint32 gmdsp_get_pc(void);

/* ---------- Results validation functions ---------- */
boolean mdsp_validate_rx_metrics (GfwBurstMetricsStruct *metrics , gas_id_t gas_id );
boolean mdsp_validate_fcch_results (GfwFcchResultStruct*data , gas_id_t gas_id );
boolean mdsp_validate_decoded_data (GfwDecodedDataStruct *data, boolean validate_rx_qual , gas_id_t gas_id );

boolean mdsp_validate_power_measure (uint32 num_rssi, uint32 *rssi , gas_id_t gas_id );
boolean mdsp_validate_sch_results (mdsp_sch_results_struct *data , gas_id_t gas_id  );

#endif /* MDSP_CMD_H */
