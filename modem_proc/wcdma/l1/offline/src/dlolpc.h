#ifndef DLOLPC_H
#define DLOLPC_H
/*============================================================================
                            D L O L P C . H
DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlolpc.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/23/14    sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
03/18/14    mit     F3 cleanup, enabling via NV's
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
01/14/14    pr      Reset DPCH OLPC windup params as part of DPCH
                    -FDPCH transition.
07/11/13    sad     Added support to Freeze TxAccum on sec for call spike and 
                    reconfigurations
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/03/13    kcm     Fixed an Null ptr access issue with dlolpc read write intf ptr
04/26/13    kcm     Windup mode support for F-DPCH and FDPCH UL TPC rejection algorithm
                    enhancements.
04/01/13    hk      Mainlined FEATURE_WCDMA_EFDPCH
01/02/12    pr      Memory Optimization changes for WCDMA.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/04/12    kcm     Trigger power balance pattern on DL TPC while performing
                    forced sync A procedure.
01/28/12    vs      Nikel feature cleanup.
11/06/11    ks      Added function to bump sir target
10/19/10    ks      Added support for outage OLPC for efdpch
06/21/10    ms      Replaced FEATURE_WCDMA_FDPCH with FEATURE_WCDMA_EFDPCH
04/06/10    ks      Added Enhanced FDPCH support
10/03/08    hk      Bringing in 7k Mailine fixes
11/28/07    nag     Moved static function declarations which were under the 
                    FEATURE_ADAPTIVE_STEP_SIZE_OLPC feature from dlolpc.h to 
                    dlolpc.c to fix compiler warnings. 
11/01/07    m       Added support for adaptive step size OLPC
03/06/07    m       Modifications to quickly enable power control
                    after configuration of DPCH
12/16/06    m       Fixed few lint errors
10/17/06    ms      Added support for FDPCH.
02/08/06    m       Added TFCI based OLPC changes
01/11/06    m       Fixed compiler warning by adding void to prototype
01/09/06    m       Read OLPC NV items when UE is powered-on instead of
                    every time in l1m_init
10/04/05    m       Remember if current CCTrCH has a specific TB size
09/08/05    m       Added a member to dl_olpc_trch_vars_struct_type
                    to keep track of succ crc errors on a trch
06/22/05    sk      Added extern winddown_detected for 
                    FEATURE_L1_POWER_CTR_PKT_WINDUP_MODE
11/01/04    vb      Added code under feature FEATURE_L1_MANAGE_WIND_DOWN to
                    freeze dl OLPC under wind down detect condition
03/29/04    gs      Added argument tf_index to function
                    dl_olpc_report_channel_crc_status
02/26/04    gs      Externalized variable for SIR floor adjust at
                    compressed mode activation.
11/12/03    m       Created the file to accommodate OLPC implementation.
                    Adds support to Dormant mode OLPC
===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "l1dlcctrchcfg.h"
#include "l1def.h"

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* power offset value between DPCCH and DPDCH received from RRC in the
 * format defined in spec (int value 0..24 offset varying in steps of
 * 0.25dB from 0dB to 6dB)
 */
extern uint32 dl_dch_po3;
extern uint16 dl_olpc_target_ebnt_hist[8];
/* DL OLPC target for F-DPCH in linear units */
extern uint16 dl_olpc_target_ebnt_fdpch[WL1_MAX_UL_CARR_IDX];

/*  The parameter indicates whether or not wind down is detected. */
extern  boolean winddown_detected;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* Outer loop power control related macros */
/* --------------------------------------- */

#define DL_BLER_QUAL_VAL_MAX 0
#define DL_BLER_QUAL_VAL_MIN -63
#define DL_BLER_QUAL_VAL_NA  -127
#define DL_OLPC_TGT_EBNT_MAX_VAL          0xFFFF

 /* Min/Max values of TPC command error rate target signalled from N/W */
 #define FDPCH_MIN_TPC_CMD_ERROR_RATE     1
 #define FDPCH_MAX_TPC_CMD_ERROR_RATE     10

#define  WINDUP_MODE_DET_CNTR             45
#define  WINDUP_MODE_CRITERION_CNTR       42


/* enum for outage point */ 
typedef enum {
   /* 6% outage point */
   FDPCH_OUTAGE_POINT_6_PCNT,
   /* 20% outage point */  
   FDPCH_OUTAGE_POINT_20_PCNT
 } dl_olpc_outage_rate_enum_type;
 
 /* enum for outage monitor window */
 typedef enum {
    /* 1 frame */
    FDPCH_OUTAGE_WINDOW_1=0,  
    /* 5 frames */
    FDPCH_OUTAGE_WINDOW_5,
    /* 10 frames */  
    FDPCH_OUTAGE_WINDOW_10  
  } dl_olpc_outage_window_enum_type;

/* structre for bookeeping outage values */
typedef struct {
  /* total accumulated outage slot count */
  uint8 fdpch_outage_slot_cnt;
  /*outage rate 6% or 20%*/
  dl_olpc_outage_rate_enum_type fdpch_outage_point;
  /* outage measurement window value 1 or 10 frames */
  dl_olpc_outage_window_enum_type fdpch_outage_window;
  /* accumulated valid slots */
  uint8 num_accum_valid_slots;
  /* current SIR target value */
  int32 fdpch_sir_target_value;
  /* this is the allowed outage slot count in a given run */
  uint8 allowed_outage_slot_count;
  /* number of frames in outage window */
  uint8 fdpch_outage_window_num_frame;
  /* this is to keep track of when to start outage OLPC */
  uint8 dl_fdpch_olpc_init_mode_timer;
} dl_fdpch_outage_struct_type;

#define DL_FDPCH_OLPC_NUM_OUTAGE_RATE 2  /* currently 6% OR 20% */
#define DL_FDPCH_OLPC_OUTAGE_WINDOW_LENGTH 3 /* currently window length 1/5/10 */

#define WL1_DLOLPC_MSG(str, a, b, c) \
if (dlolpc_debug_msg_enabled) \
{ \
  MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define WL1_DLOLPC_MSG_4(str, a, b, c, d) \
if (dlolpc_debug_msg_enabled) \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d);\
}\
else \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d);\
}

#define WL1_DLOLPC_MSG_5(str, a, b, c, d, e) \
if (dlolpc_debug_msg_enabled) \
{ \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e);\
}\
else \
{ \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e);\
}

#define WL1_DLOLPC_MSG_6(str, a, b, c, d, e, f) \
if (dlolpc_debug_msg_enabled) \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f);\
}\
else \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f);\
}

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================
FUNCTION dl_olpc_read_olpc_nv_items

DESCRIPTION
  Reads NV items on UE power-up
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_read_olpc_nv_items(void);

/*===========================================================================
FUNCTION dl_olpc_setup

DESCRIPTION
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_setup(void);

/*===========================================================================
FUNCTION dl_olpc_init

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_init
(
  /* CcTrCh table index */
  uint8 cctrcht_idx,
  /* CCTrCh info structure info pointer */
  const l1_dl_cctrch_info_struct_type *cctrch_info
  /* TFC info structure info pointer */
  ,const l1_dl_ctfc_info_struct_type   *ctfc_info
);

/*===========================================================================
FUNCTION dl_olpc_enable

DESCRIPTION
  Enables OLPC. The arguments spreading factor type and number of pilot
  bits are passed down to calculate m_val.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_enable
(
  /* Spreading factor of DPCH channel */
  l1_sf_enum_type sf_type,
  /* Number of pilot bits per frame in DPCH channel */
  uint16 n_pilot
);

/*===========================================================================
FUNCTION dl_olpc_disable

DESCRIPTION
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_disable(void);

/*===========================================================================
FUNCTION dl_olpc_check_for_windup

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_check_for_windup(void);

/*===========================================================================
FUNCTION dl_olpc_check_for_winddown

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_check_for_winddown(void);

/*===========================================================================
FUNCTION dl_olpc_in_reconfig

DESCRIPTION
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_in_reconfig(void);

/*===========================================================================
FUNCTION dl_olpc_report_channel_crc_status

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_report_channel_crc_status
(
  uint8 trch_idx,     /* TrCh index */
  uint8 tf_idx,       /* TF index */
  uint8 num_crc,      /* number of the CRC received */
  uint8 num_err       /* number of CRC errors */
);

/*===========================================================================
FUNCTION dl_olpc_update_sir_target

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dl_olpc_update_sir_target(void);

/*===========================================================================
FUNCTION dl_olpc_update_target_ebnt_btfd_only_one_tb_w_crc

DESCRIPTION
  This function updates the outer loop power control based on the criteria
  other than CRC of a BTFD TrCh that is normally not permitted to do OLPC
  based on just CRC.

  If the curent olpc stateis INIT mode, do nothing.
  If the current olpc state is ACQ mode, it is swtiched to TRK mode.

  This function is called if following criteria is satisfied
  - CCTrCh reference type is DCH
  - This TrCh belongs to CCTrCh that needs BTFD
  - There are 2 TF and only one TF has non zero TB size
  - TrCh jas CRC attached to TB with non-zero number of TB.
  - Symbol error rate is greater than RM or ZSB threshold

  This function checks folowing conditions
  - OuterLoop set point < (3 dB + PO3)

  If above condition is satisfied then outer loop setpoint is incremented

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dl_olpc_update_target_ebnt_btfd_only_one_tb_w_crc(void);

/*===========================================================================
FUNCTION dl_olpc_find_trch_dormancy_status

DESCRIPTION
  This function returns TRUE in case of the transport channel is under dormancy or if .
  Trch dormancy  Non_dormant_trch_exist  ret_val
         FALSE               FALSE                      TRUE (This case should never happen)
         FALSE               TRUE                       TRUE
         TRUE                FALSE                      TRUE
         TRUE                TRUE                       FALSE  
    
DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE or FALSE

SIDE EFFECTS
None
===========================================================================*/
extern boolean dl_olpc_find_dormancy_criteria(uint8 cctrch_index, uint8 trch_index);

/*===========================================================================
FUNCTION dl_olpc_compute_sir_target_from_cmd_error_rate

DESCRIPTION
  This function looks up the 'dl_olpc_fdpch_cmd_error_rate_to_sir_target_mapping'
  table to determine the SIR target based on the TPC command error rate.

DEPENDENCIES
  None

RETURN VALUE
  Corresponding SIR Target

SIDE EFFECTS
None
===========================================================================*/
uint16 dl_olpc_compute_sir_target_from_cmd_error_rate
(
  /* TPC command error rate target signaled by RRC */
  uint16 tpc_cmd_error_rate,
  /*Primary or Secondary carrier*/
  wl1_ul_carr_id_enum_type carr_idx
);

/*===========================================================================
FUNCTION        DL_FDPCH_OLPC_INIT

FILE NAME       dlolpc.c

DESCRIPTION     This function Initializes the fdpch outage based olpc parameters. 
                Outage point should be either 6 OR 20
                Outage window should be in range of 1-10

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void dl_olpc_fdpch_outage_init(void);

/*===========================================================================
FUNCTION        DL_FDPCH_OUTAGE_THRESH_INIT

FILE NAME       dlolpc.c

DESCRIPTION     This function writes the outage threshold value to FW

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void dl_olpc_fdpch_outage_thresh_init(uint8 ber_rate, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION        DL_FDPCH_COMPUTE_SIR_TARGET_FROM_OUTAGE

FILE NAME       dlolpc.c

DESCRIPTION     This function handles the sir target setting for fdpch. It determines
                the correct SIR target based on outage reported by FW every frame

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void dl_olpc_fdpch_compute_sir_target_from_outage(wl1_ul_carr_id_enum_type carr_idx);

/*===================================================================
FUNCTION dl_olpc_bump_sirtgt_post_evt_detection

DESCRIPTION
  This function is called by searcher on detecting an event that signals
  change of best cell

DEPENDENCIES
  None

RETURN VALUE
  Success or failure

SIDE EFFECTS
  New estimate of bler for the given tr ch is computed
===================================================================*/
extern void dl_olpc_bump_sirtgt_post_evt_detection(void);

/*===================================================================
FUNCTION dl_olpc_trigger_power_balance

DESCRIPTION
  This function ENABLES/DISABLE power balance

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  New estimate of bler for the given tr ch is computed
===================================================================*/
extern void dl_olpc_trigger_power_balance(boolean trigger,  wl1_ul_carr_id_enum_type carr_idx );

/*===================================================================
FUNCTION dl_olpc_vars_mem_alloc

DESCRIPTION
  This function Allocates memory for dl_olpc_vars

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  New estimate of bler for the given tr ch is computed
===================================================================*/
extern void dl_olpc_vars_mem_alloc(void);
/*===================================================================
FUNCTION dl_olpc_vars_mem_dealloc

DESCRIPTION
  This function Deallocates memory for dl_olpc_vars

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  New estimate of bler for the given tr ch is computed
===================================================================*/
extern void dl_olpc_vars_mem_dealloc(void);

/*===================================================================
FUNCTION dl_olpc_check_for_windup_efdpch

DESCRIPTION
  This function checks if windup is detected for F-DPCH call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================*/
extern void dl_olpc_check_for_windup_efdpch(void);

/*===================================================================
FUNCTION dl_olpc_get_windup_status_efdpch

DESCRIPTION
  This function returns windup mode status.

DEPENDENCIES
  None

RETURN VALUE
  Boolean  1- Windup detected.
           0- Windup mode not detected.

SIDE EFFECTS
  None
===================================================================*/
extern boolean dl_olpc_get_windup_status_efdpch(wl1_ul_carr_id_enum_type carr_idx);

/*===================================================================
FUNCTION dl_olpc_reset_windup_status_efdpch

DESCRIPTION
  Funtion to reset the counter used for windup mode detection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================*/
extern void dl_olpc_reset_windup_status_efdpch(wl1_ul_carr_id_enum_type carr_idx);

/*===================================================================
FUNCTION dl_olpc_params_init

DESCRIPTION
  Funtion to reset the initialize the dlolpc async read write
  interface ptr.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================*/
extern void dl_olpc_params_init(void);

/*===================================================================
FUNCTION dl_olpc_freeze_sec_pwr_ctrl

DESCRIPTION
  Funtion to freeze power control on secondary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================*/
extern void dl_olpc_freeze_sec_pwr_ctrl(void);

/*===================================================================
FUNCTION dl_olpc_unfreeze_sec_pwr_ctrl

DESCRIPTION
  Funtion to unfreeze power control on secondary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================*/
extern void dl_olpc_unfreeze_sec_pwr_ctrl(void);

/*===================================================================
FUNCTION dl_olpc_reset_windup_params

DESCRIPTION
  Funtion to reset the windup params used in DPCH for DPCH-FDPCH reconfig

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================*/

void dl_olpc_reset_windup_params(void);

 /*===========================================================================

FUNCTION dl_set_dlolpc_nv_values

DESCRIPTION
  Function controls enabling of debug F3s for dlolpc

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_set_dlolpc_nv_values(void);

#endif
