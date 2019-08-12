#ifndef TDSL1UTILS_H
#define TDSL1UTILS_H
/*===========================================================================

                    L 1   U T I L I T I E S

DESCRIPTION
  This file contains global declarations and external references
  for the L1 utilities.

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2010 by Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1utils.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/12   owen    change normalize() to tds_normalize
04/04/12   mr      Mainlined TDSL1_F3_REDESIGN and added TDSL1_L1M_MASK 
03/23/12   mr      Enabling the tds module based F3 macros
03/23/12   mr      Added new feature TDSL1_F3_REDESIGN 
05/21/10   yh      Featurized sample server config for TAXIS only
04/09/10   ms      Added support for dynamic memory allocation from Modem Heap.
03/30/10   rvs     IRAT sample-server config changes for Chetak and
                   general clean-up.
10/06/09   yh      Add function to toggle HW register when entering/exiting RxD.
09/17/09   yh      Use MCS API to configure SS to avoid GPS concurrency issues.
09/14/09   sv      Removed TDS_INTLOCK's.
08/20/09   ms      Added prototype for l1_copy_to_word function
07/17/09   yh      Add support for IRAT phase 2
04/17/09   ks      Added prototype for num_set_bitsXX functions
04/10/09   scm     Add TDSL1_TRK_LO_PDM_MIDPOINT definition.
03/06/09   yh      For IRAT phase 1, store the current mode of sample server 1.
11/05/08   scm     Utility functions for TCXO freeze and freq hypothesis usage.
10/03/08   hk      Bringing in 7k Mailine fixes
08/29/08   ks      Added wl1_swfw_sync()
08/08/08   ks      Added WL1 bit manipulation function prototype
01/29/08   scm     Move normalize_sfn() macro from tdsdlstates.h to tdsl1utils.h.
09/04/07   rgn     Added support for stable GSTMR reads at frame boundary
05/30/07   rmak    Corrected typo in tdsl1_deregister_devman_client extern
05/22/07   rmak    MDSP Low speed optimization support
04/10/06   mg      Power Collapse Support
11/15/06   mg      Extern tdsl1_enable_tx_and_clock / tdsl1_disable_tx_and_clock
08/16/05   mg      Remove wl1_gsm_clock_halted()
03/15/06   scm     Extern tdsl1_convert_ppm_to_freq_err().
03/07/06   scm     Extern tdsl1_convert_freq_err_to_ppm().
02/07/06   bbd     Added support for GSM clock shutdown voting mechanism.
12/16/05   mg      Add more wrapper functions
12/16/05   mg      Created L1 wrapper functions for enabling/disabling fee/modem clks
11/29/05   bbd     Created L1 wrapper functions for enabling/disabling GSM clock.
01/28/04   scm     Add extern to tdsl1_get_mdsp_debug_data().
07/30/03   m       Created file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tdscdma_variation.h"
#include "customer.h"
#include "tdsl1const.h"
#include "tdsmathutil.h"
#include "nv.h"
#include "tcxomgr.h"
#include "msg_mask.h"
#include "stringl.h"


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* GSM clock enable state */
extern boolean tdsl1_util_gsm_clk_ena_state;

/*===========================================================================

                     MACROS

===========================================================================*/

#define UNNORMALIZE_CFN(this_cfn, w_r_t_cfn) \
  UNNORMALIZE((this_cfn), (w_r_t_cfn), MAX_CFN_COUNT)

#define normalize_sfn(a) ((uint16)tds_normalize((int16)(a), MAX_SFN_COUNT))

#define TDSL1_UTIL_INTERF_BMSK      0x01
#define TDSL1_UTIL_GSM_MEAS_BMSK    0x02
#define TDSL1_UTIL_ALL_MEAS_BMSK    0x03

/* Various votes for deciding MDSP clk speed (medium or low) */
#define TDSL1_NON_DCH_OR_VOICE_ONLY       0x01 /* If L1 is not DCH or call is not voice only */
#define TDSL1_DCH_GSM_NEIGHBOURS          0x02 /* If GSM Neighbours are present */
#define TDSL1_DCH_HIGH_NUM_ACTIVE_FINGERS 0x04 /* If assigned fingers are more than MSMHW_MAX_NUM_FINGERS_FOR_LOW_MDSP_CLK_SPEED */
#define TDSL1_DCH_CLTD                    0x08 /* If CLTD is enabled */
#define TDSL1_MDSP_OPTIMIZATION_ALL_VOTES 0x0f
#define MOD_8_MASK                      0x7

#if defined(FEATURE_BOLT_MODEM) || defined(FEATURE_THOR_MODEM) || defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
#define TDS_MAX_USTMR_TIME  (0xFFFFFF)
#else
#define TDS_MAX_USTMR_TIME  (0x7FFFFFF)
#endif

/* Macro to print F3 msgs */
extern    uint32    tdsl1_msg_mask;
extern    boolean   TDSL1_LOG_F3_ENABLED;
extern    uint32    tdsl1_msg_level_mask;

/* EFS filename for F3 msg mask */
#define TDSL1_F3_MASK_EFS_FILENAME "/nv/item_files/modem/tdscdma/l1/f3_mask"

/* Add modules whose F3s are allowed by default to this mask.
   The allowed mask can also be controlled by EFS file "f3_mask". */ 
#define TDSL1_MSG_MASK_DEFAULT 0

/* Macro to check if a given meesage is HIGH or ERROR*/
#define TDSL1_MSG_HIGH_ERROR(mask) ((mask)& tdsl1_msg_level_mask)

/* Macro to check if a given meesage is allowed */
#define TDSL1_MSG_MASK_ALLOWED( mask )  \
  ((tdsl1_msg_mask & (mask)) || (TDSL1_MSG_HIGH_ERROR(mask)))

/* NV configuration for F3 messages using EFS files */
typedef struct
{
  /* F3 Mask cfg */
  uint32                             f3_mask_cfg;
}tdsl1_nv_cfg_s;


/****************************************************************************
** WL1_MSGx - Macros only take non-string parameters (like %d, not %s).
**
** Avoiding string support (%s) here because of excessive CPU overhead.
**
** Options for LEVEL:
** - LOW, MED, HIGH, ERROR, FATAL
**
** Example:
** - TDSL1_MSG2(ERROR, "delta_vco_temp (%d) > 0.5ppm, bounded to %d",
**            ABS(delta_vco_temp), tcxomgr.vco_temp_update_thresh);
**************************************************************************/

/*! Mask for TDL1 Error Messages 
*/
//#define TDSL1_ERROR_MASK MSG_LEGACY_ERROR



#define TDSL1_L1M_MASK  MSG_MASK_11
#define TDSL1_L1M_LOW   ( TDSL1_L1M_MASK | MSG_LEGACY_LOW   )
#define TDSL1_L1M_MED   ( TDSL1_L1M_MASK | MSG_LEGACY_MED   )
#define TDSL1_L1M_DEBUG ( TDSL1_L1M_MASK )

/* Mask for TDS SRCH*/
#define TDSL1_SRCH_MASK  MSG_MASK_5
#define TDSL1_SRCH_LOW   ( TDSL1_SRCH_MASK | MSG_LEGACY_LOW   )
#define TDSL1_SRCH_MED   ( TDSL1_SRCH_MASK | MSG_LEGACY_MED   )
#define TDSL1_SRCH_DEBUG ( TDSL1_SRCH_MASK )

/* MASK for TDS HSDPA */
#define TDSL1_HSDPA_MASK  MSG_MASK_6
#define TDSL1_HSDPA_LOW   ( TDSL1_HSDPA_MASK | MSG_LEGACY_LOW   )
#define TDSL1_HSDPA_MED   ( TDSL1_HSDPA_MASK | MSG_LEGACY_MED   )
#define TDSL1_HSDPA_DEBUG ( TDSL1_HSDPA_MASK )

#define TDSL1_HSUPA_MASK  MSG_MASK_7
#define TDSL1_HSUPA_LOW   ( TDSL1_HSUPA_MASK | MSG_LEGACY_LOW   )
#define TDSL1_HSUPA_MED   ( TDSL1_HSUPA_MASK | MSG_LEGACY_MED   )
#define TDSL1_HSUPA_DEBUG ( TDSL1_HSUPA_MASK )


#define TDSL1_UL_MASK  MSG_MASK_8
#define TDSL1_UL_LOW   ( TDSL1_UL_MASK | MSG_LEGACY_LOW   )
#define TDSL1_UL_MED   ( TDSL1_UL_MASK | MSG_LEGACY_MED   )
#define TDSL1_UL_DEBUG ( TDSL1_UL_MASK )

#define TDSL1_DL_MASK  MSG_MASK_9
#define TDSL1_DL_LOW   ( TDSL1_DL_MASK | MSG_LEGACY_LOW   )
#define TDSL1_DL_MED   ( TDSL1_DL_MASK | MSG_LEGACY_MED   )
#define TDSL1_DL_DEBUG ( TDSL1_DL_MASK )

#define TDSL1_SEQ_MASK  MSG_MASK_10
#define TDSL1_SEQ_LOW   ( TDSL1_SEQ_MASK | MSG_LEGACY_LOW   )
#define TDSL1_SEQ_MED   ( TDSL1_SEQ_MASK | MSG_LEGACY_MED   )
#define TDSL1_SEQ_DEBUG ( TDSL1_SEQ_MASK )

#define TDSL1_MSG0(LEVEL, FMT) \
  if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
  {                                                       \
    MSG(MSG_SSID_TDSCDMA_L1,MSG_LEGACY_ ## LEVEL, FMT); \
  } 

#define TDSL1_MSG1(LEVEL, FMT, A) \
  if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
  {                                          \
    MSG_1(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A);\
  } 

#define TDSL1_MSG2(LEVEL, FMT, A, B) \
  if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
  {                                          \
    MSG_2(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B);\
  } 
        
#define TDSL1_MSG3(LEVEL, FMT, A, B, C) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
        {                                          \
           MSG_3(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C);\
        } 

#define TDSL1_MSG4(LEVEL, FMT, A, B, C, D) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
        {                                          \
           MSG_4(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C, D);\
        } 

#define TDSL1_MSG5(LEVEL, FMT, A, B, C, D, E) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
        {                                          \
           MSG_5(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C, D, E);\
        } 

#define TDSL1_MSG6(LEVEL, FMT, A, B, C, D, E, F) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
        {                                          \
           MSG_6(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C, D, E, F);\
        } 

#define TDSL1_MSG7(LEVEL, FMT, A, B, C, D, E, F, G) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
        {                                          \
           MSG_7(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C, D, E, F, G);\
        } 

#define TDSL1_MSG8(LEVEL, FMT, A, B, C, D, E, F, G, H) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL) )  \
        {                                          \
           MSG_8(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C, D, E, F, G, H);\
        } 

#define TDSL1_MSG9(LEVEL, FMT, A, B, C, D, E, F, G, H, I) \
        if ( TDSL1_MSG_MASK_ALLOWED(MSG_LEGACY_ ## LEVEL))  \
        {                                          \
           MSG_9(MSG_SSID_TDSCDMA_L1, MSG_LEGACY_ ## LEVEL, FMT, A, B, C, D, E, F, G, H, I);\
        } 

/* NEW sample F3 messages */
#define TDSL1_MODULE_MSG0(msg_mask, FMT) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
          MSG(MSG_SSID_TDSCDMA_L1,msg_mask, FMT); \
        } 

#define TDSL1_MODULE_MSG1(msg_mask, FMT, A) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
          MSG_1(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A);\
        } 

#define TDSL1_MODULE_MSG2(msg_mask, FMT, A, B) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_2(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B);\
        } 

#define TDSL1_MODULE_MSG3(msg_mask, FMT, A, B, C) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_3(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C);\
        } 

#define TDSL1_MODULE_MSG4(msg_mask, FMT, A, B, C, D) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_4(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C, D);\
        } 

#define TDSL1_MODULE_MSG5(msg_mask, FMT, A, B, C, D, E) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_5(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C, D, E);\
        } 

#define TDSL1_MODULE_MSG6(msg_mask, FMT, A, B, C, D, E, F) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_6(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C, D, E, F);\
        } 

#define TDSL1_MODULE_MSG7(msg_mask, FMT, A, B, C, D, E, F, G) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_7(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C, D, E, F, G);\
        } 

#define TDSL1_MODULE_MSG8(msg_mask, FMT, A, B, C, D, E, F, G, H) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask) )  \
        {                                          \
           MSG_8(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C, D, E, F, G, H);\
        } 

#define TDSL1_MODULE_MSG9(msg_mask, FMT, A, B, C, D, E, F, G, H, I) \
        if ( TDSL1_MSG_MASK_ALLOWED(msg_mask))  \
        {                                          \
           MSG_9(MSG_SSID_TDSCDMA_L1, msg_mask, FMT, A, B, C, D, E, F, G, H, I);\
        } 

/** Enum for the mode to set the sample server */
typedef enum
{
  /* Mode GSM */
  TDSL1_SAMPLE_SERVER_MODE_GSM,
  /* Mode QICE */
  TDSL1_SAMPLE_SERVER_MODE_QICE,
  /* Mode Diversity */
  TDSL1_SAMPLE_SERVER_MODE_DIVERSITY,
  /* Mode Max */
  TDSL1_SAMPLE_SERVER_MODE_MAX
} tdsl1_sample_server_mode_enum_type;

/** Enum for the RxD mode to set the sample server */
typedef enum
{
  /* Entering RxD */
  TDSL1_SAMPLE_SERVER_ENTER_RXD,
  /* Exiting RxD */
  TDSL1_SAMPLE_SERVER_EXIT_RXD,
} tdsl1_sample_server_rxd_mode_enum_type;
 
/* the types of snapshots available and a bitmask for specifying a group of 
   those types */
typedef uint8 tdsl1_snapshot_selection_bmsk_type;
typedef enum
{
  TDSL1_SNAP_G,
  TDSL1_SNAP_TDS_RXTIME,
  TDSL1_SNAP_TDS_WALLTIME,
  TDSL1_SNAP_L,
  TDSL1_SNAP_USTMR,
} tdsl1_snapshot_types;


/* stores time on GSM multiframe timeline */
typedef struct
{
  uint16 qs;
  uint32 gfn;  
} tds_gfn_struct_type;

/* Used to store a timing snapshot between the 3 RAT types (GSM/TDSCDMA/LTE) */
typedef struct
{
  /* determines which of the snapshot fields are valid in the rest of this structure */
  tdsl1_snapshot_selection_bmsk_type valid_bmsk;

  /* stores GSM time taken at snapshot */
  tds_gfn_struct_type g;
  /* stores the tds rx time taken at snapshot. */
  tdsl1_time_info_struct_type tds_rxtime;
  tdsl1_time_info_struct_type tds_walltime;
  /* stores the lte ostmr time taken at the snapshot */
  uint32 l;
  /* stores the universal stmr time taken at the snapshot */
  uint32 ustmr_cnt;
} tdsl1_gtl_snapshot_type;

/* The 12-bit, unsigned representation of trk_lo varies from 0 to 4095, so
** this is its midpoint. */
#define TDSL1_TRK_LO_PDM_MIDPOINT  2048

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION TDSL1_MEM_ALLOC

DESCRIPTION
  Allocates memory dynamically from the Modem HEAP.
  
DEPENDENCIES
  None

RETURN VALUE
  Pointer to the allocated memory chunk

SIDE EFFECTS
  None
  
===========================================================================*/
extern void* tdsl1_mem_alloc
(
  size_t               size       /*!< Number of bytes to allocate */
);

/*===========================================================================

FUNCTION TDSL1_MEM_FREE

DESCRIPTION
  De-allocates the dynamically allocated memory from the heap
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void tdsl1_mem_free
(
  void                *ptr        /*!< Memory to free */
);

/*===========================================================================

FUNCTION TDSL1_F3_MASK_EFS_CFG

DESCRIPTION
  Prepare the mask based on user input through QXDM
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

extern void tdsl1_f3_mask_efs_cfg(uint32 efs_f3_mask_cfg);

/*===========================================================================

FUNCTION TDSL1_F3_MASK_EFS_CFG

DESCRIPTION
  Reads F3 mask configuration from EFS
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

extern void tdsl1_common_nv_f3_mask_cfg(void);

/*===========================================================================

FUNCTION tdsl1_nv_read

DESCRIPTION
  Read a item from nv. Note that  the NV read is done in a synchronous
  fashion.  That is,  this function  only  returns after  the NV  task
  completes the read  operation. During this time, it  kicks the watch
  dog periodically.

DEPENDENCIES
  NV Task running

RETURN VALUE
  nv_stat_enum_type - nv read status is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
nv_stat_enum_type tdsl1_nv_read
(
  /* NV item to read. */
  nv_items_enum_type         nv_item,

  /* Pointer to a data buffer where to store the data that is
   * associated with the NV item.
   */
  nv_item_type              *data_ptr
);

/*===========================================================================

FUNCTION tdsl1_nv_write

DESCRIPTION
  Write a item to nv.
  
  Note that the NV write is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the write operation.
  During this time, it kicks the watch dog periodically.

DEPENDENCIES
  None

RETURN VALUE
  nv_stat_enum_type - nv write status is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean tdsl1_nv_write 
(
  /* NV item to write. */
  nv_items_enum_type         nv_item,

  /* Pointer to a data buffer where to read the data that is
   * associated with the NV item to write.
   */
  nv_item_type              *data_ptr
);

/*===========================================================================
FUNCTION     TDSL1_NV_PROCESS_NV_CMD_SIG

DESCRIPTION
  Processes TDSL1M_NV_CMD_SIG from NV

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1_nv_process_nv_cmd_sig(void);


/*===========================================================================
FUNCTION     TDSL1_GSM_CLK_CTRL_LOCK_MUTEX

DESCRIPTION
  This function locks the "wl1_gsm_clk_ctrl_crit_sect" mutex

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1_gsm_clk_ctrl_lock_mutex( void );

/*===========================================================================
FUNCTION     TDSL1_GSM_CLK_CTRL_UNLOCK_MUTEX

DESCRIPTION
  This function unlocks the "wl1_gsm_clk_ctrl_crit_sect" mutex

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1_gsm_clk_ctrl_unlock_mutex( void );

/*===========================================================================
FUNCTION     TDSL1_GET_MDSP_DEBUG_DATA

DESCRIPTION
  This function is called to retrieve debug information from the mDSP.  It
  would generally be invoked during error processing to inspect mDSP state.

DEPENDENCIES
  None.

PARAMETERS
  *mdsp_debug_buffer_ptr - Specifies debug buffer of interest:
                           MDSP_debugSleepBuf or MDSP_debugWakeupBuf.
  *tdsl1m_mdsp_pc               - Returns mDSP PC to caller.
  *tdsl1m_mdsp_nest_level       - Returns mDSP nest level to caller.
  *tdsl1m_mdsp_int_mask         - Returns mDSP int mask to caller.

RETURN VALUE
  Three values returned through function parameters.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1_get_mdsp_debug_data(uint16 *mdsp_debug_buffer_ptr,
                                    uint32 *tdsl1m_mdsp_pc, uint32 *tdsl1m_mdsp_nest_level,
                                    uint32 *tdsl1m_mdsp_int_mask);


/*===========================================================================
FUNCTION     TDSL1_MDSP_CLK_SPEED_VOTE_IS_VALID

DESCRIPTION
  Confirm if client bitmask is valid
  (i.e. has only one bit set and is one of the valid clients)

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  uint8 vote

RETURN VALUE
  TRUE  if vote is valid
  FALSE if vote is invalid

SIDE EFFECTS
  none
===========================================================================*/
extern boolean tdsl1_mdsp_clk_speed_vote_is_valid(uint8 vote);

/*===========================================================================
FUNCTION     TDSL1_REQUIRE_MDSP_MED_SPEED

DESCRIPTION
  Add Vote for MDSP to run at Medium speed

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  uint8 vote

RETURN VALUE
  TRUE  if vote was successfully added
  FALSE if vote was not recognized or not needed

SIDE EFFECTS
  May increase MDSP clk speed
===========================================================================*/
extern boolean tdsl1_require_mdsp_med_speed(uint8 vote);

/*===========================================================================
FUNCTION     TDSL1_RELINQUISH_MDSP_MED_SPEED

DESCRIPTION
  Remove Vote for MDSP to run at Medium Speed

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  uint8 vote

RETURN VALUE
  TRUE  if vote was successfully removed
  FALSE if vote was not recognized or not already set

SIDE EFFECTS
  May decrease MDSP clk speed
===========================================================================*/
extern boolean tdsl1_relinquish_mdsp_med_speed(uint8 vote);

/*===========================================================================
FUNCTION     TDSL1_DISABLE_GSM_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to disable GSM clock.
  Checks GSTMR to see if GFN rollover is about to happen.  If so, we
  need to delay the clock shutdown to ensure MDSP isn't still using
  anything.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  GSM clock is disabled.
===========================================================================*/
extern void tdsl1_disable_gsm_clock(uint8 module_bmsk);

/*===========================================================================
FUNCTION     TDSL1_ENABLE_GSM_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to enable GSM clock.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  GSM clock is enabled.
===========================================================================*/
extern void tdsl1_enable_gsm_clock(uint8 module_bmsk);

/*===========================================================================
FUNCTION     TDSL1_QUERY_GSM_CLOCK_STATE

DESCRIPTION
  Returns current state of GSM clock

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  GSM clock enabled (TRUE) or disabled (FALSE).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1_query_gsm_clock_state(void);


/*===========================================================================
FUNCTION     TDSL1_ENTERING_SLEEP

DESCRIPTION
  Wrapper function for turning TCXO and TRK_LO off.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void tdsl1_entering_sleep(void);

/*===========================================================================
FUNCTION     TDSL1_EXITING_SLEEP

DESCRIPTION
  Wrapper function for turning TCXO and TRK_LO on.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void tdsl1_exiting_sleep(void);

/*===========================================================================
FUNCTION     TDSL1_ENABLE_CLOCKS_EXITING_SLEEP

DESCRIPTION
  Wrapper function for  calling clock regime APIs to enable clocks when 
  coming out of sleep.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void tdsl1_enable_clocks_exiting_sleep(void);

/*===========================================================================
FUNCTION     TDSL1_DISABLE_CLOCKS_ENTERING_SLEEP

DESCRIPTION
  Wrapper function for  calling clock regime APIs to disable clocks when 
  entering sleep.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void tdsl1_disable_clocks_entering_sleep(void);

/*===========================================================================
FUNCTION     tdsl1_write_fbiber_threshold

DESCRIPTION
  Function to write to mdsp variable fbiBER_Threshold

DEPENDENCIES

PARAMETERS
  uint16 threshold - value to write to mdsp variable fbiBER_Threshold.

RETURN VALUE
  boolean - returns whether write was successful (TRUE) or not (FALSE)

SIDE EFFECTS

===========================================================================*/
extern boolean tdsl1_write_fbiber_threshold(uint16 threshold);

/*===========================================================================
FUNCTION     tdsl1_read_fbiber_threshold

DESCRIPTION
  Function to read from mdsp variable fbiBER_Threshold

DEPENDENCIES

PARAMETERS
  uint16 *threshold - pointer to store value read from mdsp variable 
                               fbiBER_Threshold.

RETURN VALUE
  boolean - returns whether read was successful (TRUE) or not (FALSE)

SIDE EFFECTS

===========================================================================*/
extern boolean tdsl1_read_fbiber_threshold(uint16 *threshold);

/*===========================================================================
FUNCTION     TDSL1_CONVERT_FREQ_ERR_TO_PPM

DESCRIPTION
  This function takes the VCTCXO frequency error in units of 7.3242 Hz as a
  parameter.  Then, based upon the current carrier frequency, it converts
  that error to units of 2^-10 ppm.

DEPENDENCIES
  None.

PARAMETERS
  raw_freq_error - From mDSP freqCommon register, units of 7.3242 Hz.

RETURN VALUE
  int16 - VCTCXO frequency error in units of 2^-10 ppm.

SIDE EFFECTS
  None.
===========================================================================*/
extern int16 tdsl1_convert_freq_err_to_ppm(uint16 freq, int32 freq_error_q8);

/*===========================================================================
FUNCTION     TDSL1_CONVERT_PPM_TO_FREQ_ERR

DESCRIPTION
  This function receives a parameter in units of (ppm * 100).  So for 3.5 ppm,
  the caller should pass 350.  Then, based upon the current carrier frequency,
  it converts it to units of 7.3242 Hz.  This corresponds to units used by
  the mDSP to manage VCTCXO.

DEPENDENCIES
  None.

PARAMETERS
  int16 ppm_times_one_hundred (actual ppm value * 100, i.e. 2.25 ppm -> 225).

RETURN VALUE
  int16 - VCTCXO frequency error in units of 7.3242 Hz for the mDSP.

SIDE EFFECTS
  None.
===========================================================================*/
extern int32 tdsl1_convert_ppm_to_freq_err(uint16 freq, int16 ppm);


/*===========================================================================
FUNCTION     TDSL1_UPDATE_RGS_VALUES

DESCRIPTION
  This function is called periodically when we have service to update Recent
  Good System data in TCXO Manager.  It handles any tweaking or converting
  of the data to get it to a format that TCXO Manager expects.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1_update_rgs_values(void);

/*===========================================================================
FUNCTION     TDSL1_GET_STORED_VCO

DESCRIPTION
  Function called to get trk_lo and rotator values from TCXO MGR.  Applies
  only to FEATURE_WCDMA_TCXOMGR30, where TCXO MGR handles Recent Good System
  validation, RGS aging, temperature table access, etc.  WL1 then uses these
  values when attempting acquisition.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  tcxomgr_vco_info_type - TCXO MGR VCTCXO information structure.

SIDE EFFECTS
  None.
===========================================================================*/
extern tcxomgr_vco_info_type tdsl1_get_stored_vco(void);


/*===========================================================================
FUNCTION     TDSL1_CHECK_GSTMR_RD_STATUS

DESCRIPTION
  This function does two successive gstmr reads and makes sure that the counters
  are being read when gstmr is stable

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Quarter symbol count from gstmr

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 tdsl1_check_gstmr_rd_status(void);

/*===========================================================================
FUNCTION tdsl1_utils_init

DESCRIPTION
  Function initializes the global variables in l1utils
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  Initializes global variables
  
===========================================================================*/
extern void tdsl1_utils_init (void);

/*===========================================================================
FUNCTION     __lmbd

DESCRIPTION
  This function finds the leftomost occurence(MSB) of "bit_val_to_match" in "datum". It does comparision till stop_pos bit

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  position of leftmost bit pattern. check return type for -ve value.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 tds_lmbd64(uint64 datum, uint8 stop_pos);

uint8 tds_lmbd32(uint32 datum, uint8 stop_pos);

uint8 tds_lmbd16(uint16 datum, uint8 stop_pos);

uint8 tds_lmbd8(uint8 datum, uint8 stop_pos);




/*===========================================================================
FUNCTION     __rmbd

DESCRIPTION
  This function finds the rightmost occurence of "bit_val_to_match" in "datum". It does comparision starting from bit pos
  start_pos 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  position of leftmost bit pattern. check return type for -ve value.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 tds_rmbd64(uint64 datum, uint8 start_pos);
  
uint8 tds_rmbd32(uint32 datum, uint8 start_pos);

uint8 tds_rmbd16(uint16 datum, uint8 start_pos);

uint8 tds_rmbd8(uint8 datum, uint8 start_pos);

/*===========================================================================
FUNCTION     NUM_SET_BITSX   x: 64/32/16/8

DESCRIPTION
  This function counts number of bits set in a given "datum"

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
 number of set bits in the datum

SIDE EFFECTS
  None.
===========================================================================*/
 uint8 tdsnum_set_bits64(uint64 datum);
 uint8 tdsnum_set_bits32(uint32 datum);
 uint8 tdsnum_set_bits16(uint16 datum);
 uint8 tdsnum_set_bits8(uint8 datum);

 /*===========================================================================

FUNCTION L1_COPY_to_WORD

DESCRIPTION
  This function copies data from uint8 pointer into a word(uint32) byte-by-byte
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 tdsl1_copy_to_word(uint8 *src_ptr);

/*===========================================================================

FUNCTION tdsl1_copy_byte_to_word

DESCRIPTION
  This function copies data from byte into a word.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 tdsl1_copy_byte_to_word(uint8 *src_ptr);
#if 0
#if defined(FEATURE_TDSCDMA_OFFLINE_MODEM_W2G) && defined(FEATURE_WCDMA_TAXIS_SW)
/*===========================================================================
FUNCTION     tdsl1_config_sample_server  

DESCRIPTION
  This function sets the sample server setting according to the mode
  requested. Current supported modes are GSM, QICE and RxD. GSM and QICE
  will configure sample server 0 while RxD configures sample server 1.
  Makes use of MCS to configure TDM tables.

DEPENDENCIES
  None.

PARAMETERS
  Mode to configure sample server.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1_config_sample_server(tdsl1_sample_server_mode_enum_type mode);

/*===========================================================================
FUNCTION     tdsl1_config_sample_server_for_rxd

DESCRIPTION
  These functions asks MCS to toggle the TDM_CTL HW register when
  entering and exiting RxD. This is needed to have a sync between
  both sample servers on RxD mode.

DEPENDENCIES
  None.

PARAMETERS
  TDSL1_SAMPLE_SERVER_ENTER_RXD - activating RxD.
  TDSL1_SAMPLE_SERVER_EXIT_RXD - deactivating RxD.
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1_config_sample_server_for_rxd(tdsl1_sample_server_rxd_mode_enum_type mode);

/*===========================================================================
FUNCTION     tdsl1_config_sample_server_arm_ctl

DESCRIPTION
  This function configures the sample server setting to be ARM controlled
  and point GSM to SSRV 0. This function controls shared control mode
  between SS0 and SS1.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1_config_sample_server_arm_ctl(void);
#endif /* defined(FEATURE_TDSCDMA_OFFLINE_MODEM_W2G) && defined(FEATURE_WCDMA_TAXIS_SW) */
#endif
/*===========================================================================
FUNCTION     tdsl1_stmr_has_passed

DESCRIPTION
  This function is to check whether the target time has passed, or is yet to pass

  If there were no rollovers, the only check would be 
     whether target_time > current_time or not.
  because of 5 ms (or 8192 sub-frames) rollover , the code is a little
  more complicated.
  Assumption is that no more than 5 ms (or 8192 sub-frames) has passed in any case.
  This will work only for short durations.
 
DEPENDENCIES
  None.

PARAMETERS
  initial_time --- baseline time, current_time and target_time are after initial_time.
  current_time --- current time.
  target_time  --- target time.

  initial_time/current_time/target_time must be in the same unit and stmr type.
  e.g. 
  if initial_time is rx-time sub-frame number, current_time/target_time
  must be rx-time sub-frame number; 
  if initial_time is wall-time cx8 number, current_time/target_time
  must be wall-time cx8 number; 

RETURN VALUE
  TRUE --- the target time has passed
  FALSE -- the target time is not passed

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1_stmr_has_passed
(
  uint32 initial_time, 
  uint32 current_time, 
  uint32 target_time
);

extern void tdsl1_init_tcb(void);

extern rex_tcb_type* tdsl1_get_tcb(void);

extern void tdsl1_init_ftm_tcb(void);

extern rex_tcb_type* tdsl1_get_ftm_tcb(void);

extern void tdsl1_get_irat_snapshot
(
  tdsl1_snapshot_selection_bmsk_type selections,
  tdsl1_gtl_snapshot_type *snapshot
);


extern void tdsl1_log_power_info_for_mpcm();

#if defined(FEATURE_BOLT_MODEM) && defined(FEATURE_TDSCDMA_EUL_MEMPOOL_LOGGING)
/*=========================================================================

FUNCTION tdseul_wav_ref_log_trigger

DESCRIPTION
  This function sets parameters to log WAV/REF.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/ 
void tdseul_wav_ref_log_trigger (boolean enable_flag, uint32 nStartSf, uint32 reflogTS);
#endif

#ifdef FEATURE_TDSCDMA_SW_F8_DECIPHER
/********************** Start F8 support functions ******************/

/********************** Start F8 support  ******************/

/* 16 bit rotate left */
#define ROL16(a,b) (uint16)((a<<b)|(a>>(16-b)))

typedef union {
  uint32 b32[2];
  uint16 b16[4];
  uint8  b8[8];
} tdsl1util_REGISTER64;

typedef union {
  uint32 b32;
  uint16 b16[2];
  uint8  b8[4];
} tdsl1util_MY_DWORD;

typedef union {
  uint16 b16;
  uint8  b8[2];
} tdsl1util_MY_WORD;

typedef struct{
    uint32 LFSR_S0;   //!< left shift register \f$S_0\f$
    uint32 LFSR_S1;   //!< left shift register \f$S_1\f$
    uint32 LFSR_S2;   //!< left shift register \f$S_2\f$
    uint32 LFSR_S3;   //!< left shift register \f$S_3\f$
    uint32 LFSR_S4;   //!< left shift register \f$S_4\f$
    uint32 LFSR_S5;   //!< left shift register \f$S_5\f$
    uint32 LFSR_S6;   //!< left shift register \f$S_6\f$
    uint32 LFSR_S7;   //!< left shift register \f$S_7\f$
    uint32 LFSR_S8;   //!< left shift register \f$S_8\f$
    uint32 LFSR_S9;   //!< left shift register \f$S_9\f$
    uint32 LFSR_S10;  //!< left shift register \f$S_{10}\f$
    uint32 LFSR_S11;  //!< left shift register \f$S_{11}\f$
    uint32 LFSR_S12;  //!< left shift register \f$S_{12}\f$
    uint32 LFSR_S13;  //!< left shift register \f$S_{13}\f$
    uint32 LFSR_S14;  //!< left shift register \f$S_{14}\f$
    uint32 LFSR_S15;  //!< left shift register \f$S_{15}\f$
    uint32 FSM_R1;   //!< finite state machine register \f$R_1\f$
    uint32 FSM_R2;   //!< finite state machine register \f$R_2\f$
    uint32 FSM_R3;   //!< finite state machine register \f$R_3\f$
    boolean willCont;
    uint32 lastKS;
    int32 lastBitLength;
    }tdsl1util_Snow3GRegisterType;

void tdsl1util_f8_kasumi(uint8 *key, uint32 count, uint16 bearer, uint8 dir, uint8 *data, int32 length);

void tdsl1util_f8_snow3G( uint8 *key, int32 count, int32 bearer, int32 dir, uint8 *data, int32 length);

/********************** End F8 support  ******************/
#endif






#endif /* L1UTILS_H */
