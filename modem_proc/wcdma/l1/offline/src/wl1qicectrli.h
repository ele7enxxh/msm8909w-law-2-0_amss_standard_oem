#ifndef WL1_QICE_CTRL_I_H
#define WL1_QICE_CTRL_I_H
/*===========================================================================
                 WCDMA L1 QICE Controller

GENERAL DESCRIPTION
  This file contains the code for controlling the operation of the QICE block

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2008-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1qicectrli.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------
10/12/15   rsr    VLC Feature Checkin.
10/12/15   qj     Change for 0x4189 version 9.
10/12/15   rsr    Initial Checkin for IRPM Feature.
07/08/15   svh    Not sending cme disable for an already disabled cell.
01/08/15   svh    Fixed compiler warnings/errors.
11/18/14   rsr    Added changes to Update the way QSET Tables are Indexed
                  Also added changes for TxD to be assumed across carriers.
11/06/14   rsr    Added Changes for Jolokia Modem for New QSET Size and 
                  Iteration Management.
08/26/14   svh    Added changes for CIR enhancement logging and NV.
08/06/14   sr/rsr Changes to handle nbr QICE cell drop for cell resel to that cell.
07/15/14   ac     NV reorganization
06/04/14   rsr    Added Change to Ensure that MCVS clocks are dropped only 
                  after CME reframes with No RxD State. 
05/15/14   rsr    Enhanced the wl1_cme_free_cell_db_idx() APi support.
03/17/14   mit    Changes to accommodate dynamic cell_db_idx allocation
11/18/13   rsr    Added QICE Multi-carrier Support.
11/14/13   rsr    Increased Size of CME Srch info database from 8 to 10 per
                  carrier. 
09/12/13   jd      Searcher redesign 
08/28/13   sad    Disable SCH-IC if there are multiple cells in ASET
05/23/13   geg    WSW decoupling (except for component of QCHAT)
01/24/13   sj     Added function to drop qice neighbor cell to free up cell id
07/24/12   geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/07/12   sj     Added NV support for QPM params
04/06/12   hk     Added external triggers to enable/disable CME
04/04/12   sj     Added ARD v2 support
01/29/12   vs     Feature cleanup.
01/17/12   vsr    Added ARD Algorithm Support
08/09/11   vsr    Reduced number of samples in 0x423E log packet
08/02/11   vsr    CME bringup
07/05/11   vsr    Mainlining Genesis CME features
04/28/11   vsr    CME changes for NikeL
03/25/11   vsr    Logging updates
01/13/11   vsr    (Genesis) Removed unused API
01/06/11   vsr    Added support for QICE debug log packet
12/20/10   vsr    Limited QSET neighbours to 2 per carrier
11/10/10   vsr    (Genesis) Updated QICE FFINV logging
10/20/10   vsr    Add QSET Neighbour Cell Support
08/27/10   vsr    EWC Reg log packet code changed from 0x4233 to 0x423E
07/15/10   vsr    (Genesis) Updated Reg EQ log packet code to 0x4238
06/30/10   vsr    Updated EQ logging for Dual Carrier
05/03/10   rvs    New SCHIC log packet (0x418A) and new version of the EQ
                  Reconfig log packet (0x4188) for Genesis.
03/08/10   rvs    QICE SFC log packet (0x4189) version 2.
02/11/10   rvs    Updates to support dual carrier.
11/11/09   rvs    EQ Reconfig log packet (0x4188) version 3. 
10/12/09   rvs    EQ Reconfig log packet (0x4188) version 2.
09/22/09   sup    Removed log code assignments and placed them in 
                  log_codes_wcdma_int_v.h
09/24/09   rvs    Split header file in wl1qicectrl.h and wl1qicctrli.h.
09/09/09   rvs    Added support for QICE F/Finv and QSET Update log packets.
07/15/09   rvs    Checked in initial version of file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#include "wl1qicectrl.h"
#include "l1const.h"
#include "l1def.h"
#include "mcalwcdma_cme.h"
#include "wfw_qice_swi.h"
#include "log_codes_wcdma_int.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  WL1_CME_INIT = 0,
  WL1_CME_ALLOC = 1,
  WL1_CME_WEAK = 2,
  WL1_CME_QSET = 3,
  WL1_CME_DECONFIGURE = 4,
  WL1_CME_NUM_STATES
} wl1_cme_nghbr_state_enum_type;

#define WL1_CME_SRCH_RESULT_NUM_CELLS  (10)

#define WL1_CME_MAX_NUM_PSET_CELLS  (WL1_CME_SRCH_RESULT_NUM_CELLS)

#define WL1_CME_MAX_NUM_NGHBRS  (9)
#define WL1_CME_MAX_NUM_NGHBRS_PER_CARRIER  (2)

/* -----------------------------------------------------------------------
** Enumerations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Structures/Databases
** ----------------------------------------------------------------------- */

/* CME Srch Cell Info  */
typedef struct
{
  uint8                   num_cells[WL1_CME_NUM_CARRIERS];
  srch_cell_struct_type * cell_ptr[WL1_CME_NUM_CARRIERS][WL1_CME_SRCH_RESULT_NUM_CELLS];
} wl1_cme_srch_info_struct_type;

/* TO BE MOVED */
typedef void CME_RXD_TRANS_REFRAME_CB_FUNC_TYPE(void);
typedef void SRCH_CME_CELL_INFO_CB_FUNC_TYPE(wl1_cme_srch_info_struct_type* srch_info);
typedef void SRCH_CME_TXD_CHANGE_CB_FUNC_TYPE(uint8 cell_db_idx, boolean cpich_td_state);
typedef void CME_PRI_CELL_REFRAME_RXD_CB_FUNC_TYPE(boolean);
typedef void WL1_CME_FREE_CELL_ID_CB_FUNC_TYPE(void);

/* -----------------------------------------------------------------------
** Global Data/pointer Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** EQ Logging related definitions  
** ----------------------------------------------------------------------- */


/* Invalid value for CME logging */
#define CME_UNUSED_VAL            0xFF

/* EQ Debug log packet */
/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_eq_log_header_struct_type;

/* Maximum number of samples per log packet */
/* in version 8 we log samples for all active carriers */
#ifdef FEATURE_WCDMA_CIR_ENHANCEMENT
 #define CME_CIRE_EN_NUM_CARRIERS  WL1_CME_NUM_CARRIERS
#else
 #define CME_CIRE_EN_NUM_CARRIERS  1
#endif

#define CME_PER_CARR_NUM_MAX_EQ_DBG_SAMPLES   25
#define CME_NUM_MAX_EQ_DBG_SAMPLES   (CME_PER_CARR_NUM_MAX_EQ_DBG_SAMPLES * CME_CIRE_EN_NUM_CARRIERS)
/* Number of W16 per sample */
#define CME_NUM_W16_PER_EQ_SAMPLE    (sizeof(WfwEwcInfoDebugLogStruct)/sizeof(uint16))
/* Number of EQ log packet buffer */
#define CME_NUM_EQ_LOG_BUFFERS       2

LOG_RECORD_DEFINE(CME_DC_DBG_EQ_0x423E_PKT)
uint8 version;
  uint8  num_samples;
  uint16 info[CME_NUM_MAX_EQ_DBG_SAMPLES][CME_NUM_W16_PER_EQ_SAMPLE];
LOG_RECORD_END
  


/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_schic_log_header_struct_type;

/* Maximum number of samples per log packet */
#define CME_NUM_MAX_SCHIC_DBG_SAMPLES   32
/* Number of W16 per sample */
#define CME_NUM_W16_PER_SCHIC_SAMPLE    14
/* Number of EQ log packet buffer */
#define CME_NUM_SCHIC_LOG_BUFFERS       2

/* EQ debug log packet */
LOG_RECORD_DEFINE(CME_DBG_SCHIC_0x418A_PKT)
  uint8  version;
  uint8  num_samples;
  uint16 info[CME_NUM_MAX_SCHIC_DBG_SAMPLES][CME_NUM_W16_PER_SCHIC_SAMPLE];
LOG_RECORD_END

#ifdef FEATURE_WCDMA_CME_QICE_SFC_LOGGING

/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_qice_sfc_log_header_struct_type;

/* Maximum number of samples to hold per cell in a 3-cell QSET scenario. */
#define WL1_CME_SFC_DBG_QICE_PKT_NUM_MAX_SAMPLES_PER_CELL  (40)

/* Maximum number of samples per log packet. */
#define WL1_CME_SFC_DBG_QICE_PKT_NUM_MAX_SAMPLES  (WL1_CME_SFC_DBG_QICE_PKT_NUM_MAX_SAMPLES_PER_CELL * 3)

/* Size of the sample type bitmask in bytes, rounded up to multiple of 4. */
#define WL1_CME_SFC_DBG_QICE_PKT_SAMPLE_TYPE_BMSK_SIZE  (((WL1_CME_SFC_DBG_QICE_PKT_NUM_MAX_SAMPLES + 31) >> 5) << 2)

/* Size of a non-primary cell sample in bytes. */
#define WL1_CME_SFC_DBG_QICE_PKT_NON_PRIMARY_CELL_SAMPLE_SIZE  (4)

/* Flag to signal sample is not for the primary cell. */
#define WL1_CME_SFC_DBG_QICE_PKT_NON_PRIMARY_SAMPLE_FLAG  (0)

/* Flag to signal sample is for the primary cell. */
#define WL1_CME_SFC_DBG_QICE_PKT_PRIMARY_SAMPLE_FLAG  (1)

/* Maximum size of the sample buffer in bytes.  Derived from 120 samples, 40 of
 * which are for the primary cell, 80 for non-primary cells.
 */
#define WL1_CME_SFC_DBG_QICE_PKT_SAMPLE_BUFFER_SIZE  ((WL1_CME_SFC_DBG_QICE_PKT_NUM_MAX_SAMPLES_PER_CELL * sizeof(WfwQiceSfcLogEntryStruct)) + \
                                                      (2 * WL1_CME_SFC_DBG_QICE_PKT_NUM_MAX_SAMPLES_PER_CELL * WL1_CME_SFC_DBG_QICE_PKT_NON_PRIMARY_CELL_SAMPLE_SIZE))

/* Number of QICE log packet buffers. */
#define WL1_CME_NUM_QICE_SFC_LOG_BUFFERS      (2)

typedef PACKED struct PACKED_POST
{
  uint32 commit_margin_cx2;
  uint32 sfc_energy_thresh;
  uint16 sfc_hw_sum_alpha;
  uint16 sfc_ratio_alpha;
  uint16 sfc_ratio_thresh;
  uint8  mmse_weight_diag[4];
  uint8  mmse_weight_cross[4];
  uint8  noise_inflation;
  uint8  iterations;
  uint8  bank_switch_delay;
  uint8  sfc_bpg_per_dump;
  uint8  sfc_dump_per_ratio;
  uint8  sfc_ratio_per_classify;
} wl1_cme_dbg_config_params_type;

typedef PACKED struct PACKED_POST
{
  uint32 fht256[8];
  uint16 fht16;
  uint8  eq_index;
  uint8  reserved;
} wl1_cme_dbg_fht_params_type;

typedef PACKED struct PACKED_POST
{
  uint16 misc_sf16_overrides;
  uint16 sf16_non_linear_override;
  uint16 sf16_non_linear_select;
  uint16 sf16_mode_override;
  uint16 sf16_mode_select;
  uint8  eq_index;
  uint8  reserved;
} wl1_cme_dbg_fw_params_type;

/* QICE SFC debug log packet. */
LOG_RECORD_DEFINE(CME_SFC_DBG_QICE_PKT)
  uint8                          version_number;
  uint8                          oride_params_bmsk;
  uint8                          num_samples;
  uint8                          reserved;
  uint8                          sample_type_bmsk[WL1_CME_SFC_DBG_QICE_PKT_SAMPLE_TYPE_BMSK_SIZE];
  wl1_cme_dbg_config_params_type config_params;
  wl1_cme_dbg_fht_params_type    fht_params[MCALWCDMA_CME_MAX_NUM_QSET_CELLS];
  wl1_cme_dbg_fw_params_type     fw_params[MCALWCDMA_CME_MAX_NUM_QSET_CELLS];
  uint8                          sample_buffer[WL1_CME_SFC_DBG_QICE_PKT_SAMPLE_BUFFER_SIZE];
LOG_RECORD_END

#endif /* FEATURE_WCDMA_CME_QICE_SFC_LOGGING */

#ifdef FEATURE_WCDMA_CME_QICE_FFINV_LOGGING
/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_qice_ffinv_log_header_struct_type;

/* Maximum number of samples per log packet. */
#define WL1_CME_NUM_MAX_QICE_FFINV_DBG_SAMPLES  (25)
/* Number of QICE log packet buffers. */
#define WL1_CME_NUM_QICE_FFINV_LOG_BUFFERS      (2)

/* QICE debug log packet. */
LOG_RECORD_DEFINE(CME_FFINV_DBG_QICE_PKT)
  uint8                      version_number;
  uint8                      num_samples;
  uint16                     reserved;
  WfwQiceFFinvLogEntryStruct samples[WL1_CME_NUM_MAX_QICE_FFINV_DBG_SAMPLES];
LOG_RECORD_END

#endif /* FEATURE_WCDMA_CME_QICE_FFINV_LOGGING */

/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_qice_debug_log_header_struct_type;

/* Maximum number of samples per log packet. */
#define WL1_CME_NUM_MAX_QICE_DEBUG_SAMPLES  (25)

/* Number of QICE log packet buffers. */
#define WL1_CME_NUM_QICE_DEBUG_LOG_BUFFERS      (2)

/* QICE debug log packet. */
LOG_RECORD_DEFINE(CME_SFC_DBG_QICE_PKT)
  uint8                      version_number;
  uint8                      num_samples;
  uint8                      num_carriers;
  uint8                      reserved;
  WfwQiceDebugLogStruct samples[WL1_CME_NUM_CARRIERS][WL1_CME_NUM_MAX_QICE_DEBUG_SAMPLES];
  wfwQiceDebugConfigLogStruct config_data;
LOG_RECORD_END


/* CME logging to stop accumulation */
extern boolean wl1_cme_stop_eq_recfg_logging;

/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_eq_recfg_log_header_struct_type;

/* Number of EQ_RECFG log packet buffers. */
#define WL1_CME_NUM_EQ_RECFG_LOG_BUFFERS      (2)

typedef PACKED struct PACKED_POST
{
  uint16 MiscDynQiceInfo1;
  uint32 MiscDynQiceInfo2;
  uint32 RcvrSchicInfo;
  uint32 IrpmInfo1;
  uint32 IrpmInfo2;
  uint32 VlcInfo1;
  uint32 VlcInfo2;
}eq_recfg_carr_info_struct_type;


typedef PACKED struct PACKED_POST
{
  uint32 EqCfg;
  uint32 EqMiscInfo;
}eq_info_struct_type;
/*
genesis:
miscDynQiceInfo1 format:
Bit 0     - RXD state
Bit 1     - SCHIC enabled
Bit 4:2   - HS receiver type
Bit 5     - PA temperature state
Bit 6     - current QICE enable value
Bit 7     - dynamic QICE enable value
Bit 12:8  - QICE timer value
But 15:13 - Reserved

taxis:
miscDynQiceInfo1 format:
Bit 0     - RXD state
Bit 3:1   - HS receiver type
Bit 4     - PA temperature state
Bit 5     - current QICE enable value
Bit 6     - dynamic QICE enable value
Bit 11:7  - QICE timer value
But 15:12 - Reserved

miscDynQiceInfo2 format:
Bit 15:0  - TX AGC val
Bit 23:16 - avg num locked fingers
Bit 30:24 - avg SCCH success rate
Bit 31    - HS enabled flag

eq?Cfg format:
Bit 0     - Enable
Bit 1     - Init
Bit 2     - Primary
Bit 3     - RXD
Bit 4     - QICE enable
Bit 5     - PSC enable
Bit 6     - SSC enable
Bit 7     - Reframe
Bit 11:8  - Reserved
Bit 14:12 - Cell index
Bit 31:15 - Start window PN position chip x 1

eq?MiscInfo format:
Bit 12:0  - Scrambling code index for cell 0
Bit 16:13 - Reserved
Bit 17    - S-CPICH enable
Bit 26:18 - S-CPICH OVSF
Bit 27    - CPICH STTD
Bit 28    - HS cell
Bit 29    - EQ? update
Bit 30    - SCH invert
Bit 31    - Reserved

genesis:
rcvrSchicInfo format:
Bit 0     - Reserved
Bit 1     - R99 Rake enabled
Bit 2     - R99 LEQ enabled
Bit 3     - R99 QICE enabled 
Bit 4     - HS Rake enabled
Bit 5     - HS LEQ enabled
Bit 6     - HS QICE enabled 
Bit 7     - Non-master Rake enabled
Bit 8     - Non-master LEQ enabled
Bit 9     - Non-master QICE enabled
Bit 10    - QRAKE enabled
Bit 11    - Demod path update
Bit 12    - SCHIC update
Bit 14:13 - HS-PDSCH Diversity
Bit 15    - MIMO
Bit 16    - OHCIC enabled for Rake 
Bit 17    - OHCIC enabled for LEQ 
Bit 18    - OHCIC enabled for QICE
Bit 20:19 - Reserved
Bit 21    - SCH TSTD
Bit 22    - PCCPCH STTD
Bit 31:23 - Reserved

taxis:
rcvrSchicInfo format:
Bit 0     - Reserved
Bit 1     - R99 Rake enabled
Bit 2     - R99 LEQ enabled
Bit 3     - R99 QICE enabled 
Bit 4     - HS-SCCH Rake enabled
Bit 5     - HS-SCCH LEQ enabled
Bit 6     - HS-SCCH QICE enabled 
Bit 7     - HS-PDSCH Rake enabled
Bit 8     - HS-PDSCH LEQ enabled
Bit 9     - HS-PDSCH QICE enabled
Bit 10    - Demod path update
Bit 11    - SCHIC update
Bit 13:12 - HS-PDSCH Diversity
Bit 14    - MIMO
Bit 15    - Reserved
Bit 16    - SCHIC enabled for Rake 
Bit 17    - SCHIC enabled for LEQ 
Bit 18    - SCHIC enabled for QICE
Bit 20:19 - SCHIC cell EQ index (3 is none)
Bit 21    - SCH TSTD
Bit 22    - PCCPCH STTD
Bit 31:23 - Reserved
*/

/* EQ_RECFG debug log packet. */
LOG_RECORD_DEFINE(CME_DBG_EQ_RECFG_PKT)
  uint8  version;
  uint8  num_of_carriers;
  uint8  num_qset_cells;
  eq_recfg_carr_info_struct_type eq_recfg_carr_info[WL1_CME_NUM_CARRIERS];
  eq_info_struct_type  eq_info[MCALWCDMA_CME_MAX_NUM_QSET_CELLS*WL1_CME_NUM_CARRIERS];
LOG_RECORD_END

/* CME logging to stop accumulation */
extern boolean wl1_cme_stop_qset_update_logging;

/* Defined in services\diag\log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} cme_qset_update_log_header_struct_type;

/* Number of QSET_UPDATE log packet buffers. */
#define WL1_CME_NUM_QSET_UPDATE_LOG_BUFFERS  (2)

typedef PACKED struct PACKED_POST
{
  uint16 psc;
  uint8  ecio;
  uint8  misc_info; /* bit 7: pri, bits 6-4: eq idx; bits: 3-0: cell db idx */
  uint8  pri_ctr;
  uint8  drop_ctr;
  uint16 pn_pos_cx1;
} wl1_cme_qset_update_log_pkt_qset_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint16 psc;
  uint8  ecio;
  uint8  misc_info; /* bits 7-4: pset idx; bits: 3-0: cell db idx */
  int16  metric[MCALWCDMA_CME_MAX_NUM_QSET_CELLS];
} wl1_cme_qset_update_log_pkt_pset_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8                                             add_threshold;
  uint8                                             drop_threshold;
  uint8                                             drop_ctr_threshold;
  uint8                                             primary_ctr_threshold;
  uint8                                             metric_threshold;
  uint8                                             pset_leak;
  uint8                                             num_pq_set_cells; /* bits 7-4: num qset cell, bits 3-0: num pset cells */
} wl1_cme_qset_update_log_pkt_carr_info_struct_type;

/* QSET_UPDATE debug log packet. */
LOG_RECORD_DEFINE(CME_DBG_QSET_UPDATE_PKT)
  uint8                                             version;
  uint8                                             num_carriers;
  wl1_cme_qset_update_log_pkt_carr_info_struct_type carr_info[WL1_CME_NUM_CARRIERS];
  wl1_cme_qset_update_log_pkt_qset_info_struct_type qset_info[MCALWCDMA_CME_MAX_NUM_QSET_CELLS*3];
  wl1_cme_qset_update_log_pkt_pset_info_struct_type pset_info[WL1_CME_MAX_NUM_PSET_CELLS];
LOG_RECORD_END


/*===========================================================================
FUNCTION wl1_cme_stop_logging

DESCRIPTION
  Stops all logging in progress and flushes the log packets.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_cme_stop_logging(void);


/* -----------------------------------------------------------------------
** Public Function Declarations
** ----------------------------------------------------------------------- */

/*===========================================================================
FUNCTION wl1_cme_init

DESCRIPTION
This function initializes all the one-time and control parameters.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_init(void);

/*===========================================================================
FUNCTION wl1_cme_nv_read

DESCRIPTION
Reads the CME NV value and sets the overrides appropriately.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_nv_read(void);

/*===========================================================================
FUNCTION wl1_cme_controller

DESCRIPTION
Top-level function responsible for:
1. Acting on all the client actions, eg, HS transition, search results, etc.
2. Performing all of the actions, eg, P/QSET update, receiver selection, etc.
3. Updating the FW with the new configuration.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_controller(void);

/*===========================================================================
FUNCTION wl1_cme_cell_drop

DESCRIPTION
Notifies CME that a cell has been dropped, causing CME to drop the cell from
the P/QSET if it is present.  If the cell is in either set, the SRCH callback 
is deregistered.  It is registered again when the cell drop done handler is
called.  

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_cell_drop(
  /* The cell db index of the cell being dropped. */
  uint16 cell_idx,
  /* Client cb after cell is deconfigured */
  WL1_CME_FREE_CELL_ID_CB_FUNC_TYPE *free_cell_id_cb);

/*===========================================================================
FUNCTION wl1_cme_free_cell_db_idx

DESCRIPTION

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_free_cell_db_idx(uint16 num_cells, WL1_CME_FREE_CELL_ID_CB_FUNC_TYPE *free_cell_id_cb);

/*===========================================================================
FUNCTION wl1_cme_flush_logging

DESCRIPTION
Submits any EQ/SCHIC/QICE log packets that might be partially full.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_flush_logging(void);

/*===========================================================================
FUNCTION wl1_cme_update_eq_logging

DESCRIPTION
Updates the current EQ log packet by getting data from the FW/SW interface.
Prepares to submit the log packet if it fills up or if EQ logging is stopped.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_update_eq_logging(void);

/*===========================================================================
FUNCTION wl1_cme_submit_eq_log

DESCRIPTION
Called when the local command is processed, submits the EQ log packet.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_submit_eq_log(void);

/*===========================================================================
FUNCTION wl1_cme_update_eq_reg_logging

DESCRIPTION
Updates the current EQ Regular log packet by getting data from the FW/SW interface.
Prepares to submit the log packet if it fills up or if EQ logging is stopped.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_update_eq_reg_logging(void);

/*===========================================================================
FUNCTION wl1_cme_submit_eq_reg_log

DESCRIPTION
Called when the local command is processed, submits the EQ Regular log packet.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_submit_eq_reg_log(void);

/*===========================================================================
FUNCTION wl1_cme_update_schic_logging

DESCRIPTION
Updates the current SCHIC log packet by getting data from the FW/SW
interface.  Prepares to submit the log packet if it fills up or if SCHIC
logging is stopped.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_update_schic_logging(void);

/*===========================================================================
FUNCTION wl1_cme_submit_schic_log

DESCRIPTION
Called when the local command is processed, submits the SCHIC log packet.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_submit_schic_log(void);


#ifdef FEATURE_WCDMA_CME_QICE_SFC_LOGGING
/*===========================================================================
FUNCTION wl1_cme_update_qice_sfc_logging

DESCRIPTION
Updates the current QICE SFC log packet by getting data from the FW/SW
interface.  Prepares to submit the log packet if it fills up or if QICE
logging is stopped.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_update_qice_sfc_logging(void);

/*===========================================================================
FUNCTION wl1_cme_submit_qice_sfc_log

DESCRIPTION
Called when the local command is processed, submits the QICE SFC log packet.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_submit_qice_sfc_log(void);
#endif /* FEATURE_WCDMA_CME_QICE_SFC_LOGGING */

#ifdef FEATURE_WCDMA_CME_QICE_FFINV_LOGGING
/*===========================================================================
FUNCTION wl1_cme_update_qice_ffinv_logging

DESCRIPTION
Updates the current QICE FFINV log packet by getting data from the FW/SW
interface.  Prepares to submit the log packet if it fills up or if QICE
logging is stopped.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_update_qice_ffinv_logging(void);

/*===========================================================================
FUNCTION wl1_cme_submit_qice_ffinv_log

DESCRIPTION
Called when the local command is processed, submits the QICE SFC log packet.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_submit_qice_ffinv_log(void);
#endif /* FEATURE_WCDMA_CME_FFINV_LOGGING */

/*===========================================================================
FUNCTION wl1_cme_update_qice_debug_logging

DESCRIPTION
Updates the current QICE DEBUG log packet by getting data from the FW/SW
interface.  Prepares to submit the log packet if it fills up or if QICE
logging is stopped.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_update_qice_debug_logging(void);

/*===========================================================================
FUNCTION wl1_cme_submit_qice_debug_log

DESCRIPTION
Called when the local command is processed, submits the QICE DEBUG log packet.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_submit_qice_debug_log(void);

/*===========================================================================
FUNCTION cme_register_rxd_trans_reframe_cb

DESCRIPTION
This Function registers the RxD Callback to Drop MCVS Clocks.

DEPENDENCIES
None

RETURN VALUE


SIDE EFFECTS
None
===========================================================================*/
extern void cme_register_rxd_trans_reframe_cb(CME_RXD_TRANS_REFRAME_CB_FUNC_TYPE *rxd_trans_cb);

/*===========================================================================
FUNCTION wl1_cme_is_qice_rx_enabled

DESCRIPTION
Returns the boolean QICE Rx (On either carrier) enabled state.

DEPENDENCIES
None

RETURN VALUE
TRUE is there is at least one cell in the QSET and QICE is selected in the
demod path.

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_qice_rx_enabled(void);

/*===========================================================================
FUNCTION wl1_cme_is_hs_rx_update_pending

DESCRIPTION
Returns the boolean HS Rx update pending state.

DEPENDENCIES
None

RETURN VALUE
TRUE if a Rx update cmd is pending for HS receiver

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_hs_rx_update_pending(void);

/*===========================================================================
FUNCTION wl1_cme_is_txd_enabled_in_qset

DESCRIPTION
Check if any cell in the QSET has TxD enabled

DEPENDENCIES
None

RETURN VALUE
TRUE if any cell in the QSET has TxD enabled

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_txd_enabled_in_qset(void);

/*===========================================================================
FUNCTION wl1_cme_register_primary_cell_reframe_rxd_cb_func

DESCRIPTION
Register rxd callback func for QICE primary cell reframe

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_register_primary_cell_reframe_rxd_cb_func(CME_PRI_CELL_REFRAME_RXD_CB_FUNC_TYPE *rxd_cb);

/*===========================================================================
FUNCTION wl1_cme_get_ta_log_eq_info_for_cell

DESCRIPTION
Gets the value for eq_info in the per_cell_info_struct_type structure of the
WCDMA_FINGER_TA_INFO_LOG_PACKET_type structure.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
uint8 wl1_cme_get_ta_log_eq_info_for_cell(
  /* Cell db index of cell to get TA log info for. */
  uint8 cell_idx);


#ifdef WL1_CME_UNIT_TEST
#error code not present
#endif /* WL1_CME_UNIT_TEST */


#endif /* WL1_QICE_CTRL_I_H */
