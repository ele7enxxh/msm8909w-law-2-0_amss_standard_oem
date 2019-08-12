#ifndef UIMI_H
#define UIMI_H
/*===========================================================================

                        I N T E R N A L   R U I M
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the internal declarations for the RUIM which runs
  in the context of the AUTH Task. It also contains the interface
  between the custom ruim modules and the MAIN RUIM state machine.

Copyright (c) 2000-2016 by QUALCOMM Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimi.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   dd      F3 Optimization on curr path
05/09/16   sam     Added function to queue an internal open channel command
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
10/16/15   sam     Added support to queue Explicit ADF selection
07/15/15   ks      Consolidating the F3s that print the directory path
05/13/15   ll      Splitting the slot specific NVs from MCFG refreshed NVs
01/09/15   akv     Adding power supported indication to the TERMINAL_CAPABILITY
11/17/14   nmb     Remove dead directory maintenance code
10/27/14   ll      KW
09/18/14   akv     UIMDRV UT enhancements
08/28/14   akv     UIMDRV-QMI interactions - Indications and acks for LDO events
08/25/14   ll      Fix compile warnings medium
08/14/14   sam     Support for extended recovery
08/08/14   ak      Feature flags cleanup
08/06/14   js      Handle SW 6280, 63C1 for non-telecom ADFs for activate ADF
07/15/14   ll      NV refresh upon hotswap feature
06/16/14   ll      Switch to new string based TLMM APIs
06/04/14   na/js   Handling of case 4 commands for non telecom apps
04/11/14   na      Added support for SWP NV item to encode SWP in Terminal Capability
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
03/03/14   nmb     Directory maintenance I&T bug fixes
02/18/14   sam     Usage of newer style diag macros
02/02/14   akv     SFI initial engg revision
01/29/14   akv     RX ISR refactoring changes
01/14/14   ak      Uim task signal handling re-factor/cleanup
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
12/27/13   ks      Subscription manager review fixes
12/18/13   ll      Old NULL Byte timer logic clean up for SIM BUSY feature
11/11/13   js      Updated handling of task stop signal
10/24/13   akv     Removed unnecessary feature flags
10/22/13   ll      Add UIM BUSY Response feature
10/22/13   ll      Check UIM GPIOs as inputs upon timeout w/recovery disabled
10/07/13   ll      Disable polling for internal power measurement
10/01/13   ak      Remove unnecessary feature flags
09/30/13   akv     Removal of multiple efs reads performed by uim tasks
09/26/13   yk      Clean up signals
09/24/13   nmb     Remove extra uim_free param
09/23/13   nmb     Remove GET_CURR_INSTANCE
09/19/13   akv     Reference CLK changes to support 4.8MHz
09/16/13   yk      Removed the code for old targets and for old features
08/21/13   sam     Power down UIM on technical problems through NV control
08/14/13   ks      More time proactive command handling
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
08/1/13    sam     APDU logging mask variable moved to uim_instance_global_type
07/25/13   akv     Resetting necessary UIM globals after we notify our clients
                   of an error in uim_notify_error.
07/12/13   nmb     Global Reorganization
07/09/13   js      Fix for extra modem wake up while UE is camped on CDMA.
07/04/13   na      NULL Byte timer implementation
06/24/13   js      TSTS changes
05/16/13   na      Secure memory copy
02/12/13   js      General F3 macro cleanup
01/24/13   akv     ICCID Cached and sent along with link est data to GSDI
12/12/12   av      Replace FEATURE_UIM_DEPRECATE_TMC with FEATURE_MODEM_RCINIT
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
10/26/12   akv     HW Enumeration changes
10/19/12   ssr     Fix Stream APDU ST check for Generic Command only
10/04/12   js      Fixed linker error
09/26/12   av      Make UIM_MAX_NUM_NULL_PROC_BYTES count run time configurable
09/21/12   ms      Added code for error handling during recovery
08/13/12   av      Removed RPC support along with RPC style comments
08/13/12   js      Fix for supporting proprietary streaming APDU
08/09/12   js/rm   Fixes for UIM controller hotswap and hardware WWT features
07/24/12   nmb     Fix to not retry the same command on receiving 6F 00
05/21/12   nmb     Dual Sim Subscription Manager enablement
05/01/12   av      Replaced FEATURE_UIM_SUPPORT_HOTSWAP with an nvitem
03/30/12   av      Remove feature flags with NVITEM
03/30/12   ssr     Empty AID selection support
03/20/12   rm      Changes for WWT and hotswap support
03/15/12   rm      Changes for DIME
03/08/12   nmb     Notify card of logical channel support
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/24/12   yb      Fixes to save power impact due to delay before and after
                   clock stop in idle mode
01/06/12   ssr     Added delay before doing clock stop and after clock start
01/06/12   ms      Fix for polling happening in less than 28 sec after last command
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   ms      Modified procedure for card error notification w.r.t addition of MMGSDI API interface
10/18/11   av      Removed features that are always OFF
10/04/11   yk      Change deferrable timers to non-deferrable timers
09/07/11   ssr     Added uim_hotswap_reset_slot api
08/30/11   yb      Added support for complete recovery solution
                   Added support for caching channel information slot wise
                   Fixed crash on card removal and inserting new card
08/11/11   nmb     Expanded Logical Channels
08/09/11   ak      Merge "reset poll timer after Refresh Reset"
08/01/11   vs      Added support for session open with select response
05/16/11   ssr     Added UIM thread safe support
04/26/11   yt      Replaced references to TMC heap with Modem heap
04/25/11   yb      Adding support for hotswap
04/22/11   yk      Fix for a card issue leading to card error during STATUS
                   command
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/18/10   yb      Fixed compilation error for DSDS logging
11/10/10   yt      Memory reduction changes
10/22/10   js      Make uim to malloc on modem heap always
09/27/10   yb      Added support to send aid TLV length as p3 in status
                   command to card
09/01/10   ps      Merge UIM server DSDS changes
08/31/10   yb      Added bit in UIM Config NV to ignore RX break errors in idle
08/16/10   yb      Adding UIM Config NV parameters
03/29/10   ssr     Removed FEATURE_DS_MOBILE_IP
03/25/10   kp      Backedout virtuim changes from mainline
03/21/10   mib     Compilation fix for C2K only builds
02/23/10   yb      Fixed compilation error
02/18/10   vs      Merged CSIM functionality
02/18/10   nb      Changed FEATURE_UIM_SUPPORT_CSIM to FEATURE_UIM_SUPPORT_ATCSIM
01/13/10   rm      Fixes for 40K ATR issue
10/08/09   kp      Fix for the error fatal
10/08/09   kp      Replacing FEATURE_MODEM_HEAP with
                   FEATURE_UIM_ZI_MEMORY_REDUCTION
09/30/09   mib     Added PACKED_POST for Q6 compilation
09/26/09   kp      ZI memory reduction changes
08/10/09   rm      Fixes to have separate count for repeated technical problems
08/03/09   yb      Fix to do get response for seek command if a GCF test case
                   is running or NV_ENS_ENABLED_I NV item is enabled.
04/15/09   ssr     Fixed limited Length TERMINAL PROFILE for non-spec
                   compliant cards
04/15/09   rm      Fix to add 1ms delay between APDUs
04/07/09   js      Support for accessing EF-by-path with proprietary DFs
03/23/09   sun     Added support for VIRTUIM in QTF
02/17/09   nb      Fixed Compilation Warning
01/29/09   sun     Removed MBMS Featurization
01/28/09   nb      Changes to support virtuim in QTF
12/17/08   nb      Common Modem Interface
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/17/08   nb      Removed redundant extern declaration for uim_atr_pps_done,
                   already declared in uimgen.h
10/07/08   vs      Fix to power up UIM LDO after card power down
09/19/08   tml     Re-select MF to force directory maintainance to MF when
                   received a sw1 warning with normal return
08/26/08   kk      Few comments from uim directory management review
08/22/08   vs      Added USB UICC related poll status
08/14/08   kk      Added handling for SELECT EFs with non-generic file ids
08/08/08   vs      Removed feature UIM_CLOCK_STOP_ALLOWED
07/17/08   vs      Added uim_pet_and_wait() prototype
09/10/07   tml     Added MFLO support
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
07/18/07   sp      Featurize uim_free_reset_auth_info()
07/05/07   sun     Added support for ODD INS Class
05/21/07   jk      Move recovery data structure to uim.h so it can be exported
02/13/07   pv      Localaze uim_poll_timer and uim_poll_time_delay and
                   provide an API uim_set_poll_timer instead.
02/12/07   pv      Rename clear_timeout to clear_timeout_on_response to be
                   more clear as to what it means.
12/11/06   tml     Add uim_reset_dir_for_slot_channel
11/13/06   pv      Externalize uim_poll_timer and uim_poll_time_delay.
                   Bring the definition for UIM_UICC_INSTRUCTION_CLASS1 out of
                   conditional compilation.
09/29/06   pv      Move uim_clk_busy_wait out of FEATURE_UIM_LOG_APDU_TO_EFS
09/28/06   pv      Externalize uim_process_generic_response uim_me_powerup to
                   be used in uim.c
09/25/06   pv      Export uim_clk_busy_wait which is a wrapper around
                   clk_busy_wait.
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
07/17/06   jk      Support for APDU logging to EFS
06/12/06   pv      Re-define macros to use the uim_config_param.
                   Define uim_mode_type.
04/24/06   pv      Externalize uim_config_params to be used by any UIM module.
04/19/06   pv      Removed INLINE for uim_get_file_path
                   Featurize out declarations relating to UIM_DEBUG_LOG.
                   Get UIM_PATH_TO_SEL_MF_INDEX out of feature FEATURE_UIM_UICC.
                   Get uim_poll_pending out of FEATURE_UIM_UICC
03/30/06   pv      Externalize bunch of debug variables to keep track of
                   the general health condition of the uim errors
03/09/06   pv      Externlaize uim_get_file_path
03/01/06   pv      Externalize uim_clear_get_response_cache.
02/15/06   pv      changed the signature uim_malloc to use dword instead of
                   int32.  Corrected the value for UIM_USIM_RETURN_NO_DATA
02/14/06   nk      Merge for: Use heap_small when the size of get response
                   allocation is less than 64 bytes and External variables
                   for UIM chaching mechanism
11/14/05   pv      Lint Fixes
10/27/05   pv      Code to turn off the interface if no ATR is received whthin
                   40,000 UIM clock cycles
11/01/05   tml     Combine last sel EF with last sel Dir
09/13/05   jk      Added definition for minimum interface powerdown time
                   between voltage switch
09/06/05   jk      Changes to support only allowing reading UART Status while
                   there is still power to UART
07/27/05   jk      Made work waiting time array for dual slots
06/13/05   pv      Code to support UIM commands with allocated memory.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/10/05   pv      BT Sim access changes.  Externalized variables and functions
                   to handle power-up, power-down commands
02/02/05   pv      Increased Max Parity Error count from 10 to 100.
01/21/05   wli     Extend work waiting timer value for an out of spec RUIM card.
12/28/04   pv      externalized variables required for time-stamp based timeout
                   mechanism
11/15/04   pv      Added uim_t_1_bgt variable for block guard time
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
09/01/04   ck      Added support for CSIM.
05/17/04   ck      Moved some definitions to support additional information
                   in GSDI link established message
                   Added definitions to support streaming
04/16/04   ts      Fixed problems with error condition processing due to bad
                   merge.
03/26/04   ts      Added max receive error conditions and max receive error
                   flags.  Added a transaction timer for excessive null
                   procedure bytes.
03/15/04   ts      Made uim_toggle_instrn_class a global variable so that uim.c
                   can change it when timing out on a USIM instruction class.
01/27/04   ck      Externalized the function uim_gsdi_link_est
12/19/03   jk      Added definitions for R-UIM 3GPD Services.
09/19/03   ck      Added the definitions for start character in direct
                   convention and inverse convention.
09/02/03   ck      Changed the return type for the funtcion uim_set_cdma_slot.
08/14/03   ck      Added support for dual slots.
06/19/03   ck      Externalised the variable uim_overrun_error.
06/03/03   ck      Added support for status UIM_INSTRN_CHANGE and
                   UIM_CONVENTION_CHANGE.
                   Externalised the counters uim_convention_change_count and
                   uim_instrn_toggle_count.
05/01/03   ck      Externalised the static command buffer
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
02/26/03   jk      Added WIM and UTIL support
03/04/03   ck      Moved the constants used for processing a select command
                   from uimgen.c
10/24/02   ts      Added references to work waiting time.
10/11/02   ck      Externalized the variable uim_first_inst_class.
08/12/02   ck      Externalized the function uim_reset_dir and added definition
                   to support select by path reference and added definitions to
                   support select by path reference.
07/09/02   ck      Added curr_app member to last selected directory struct.
02/01/02   ts      Added a new status indicating the card does not support 3V
                   technology.
12/19/01   ck      Removed CDMA and GSM DF definitions.
12/04/01   ts      Added support for UIM toolkit.
05/28/01   ck      Added support for R-UIM, GSM and USIM modules.
01/15/01   ck      Created Module from authi.h rev 1.1 from Common archive.

===========================================================================*/

/* re-define lint based on the lint flag.  Could be put to comdef.h */
#include "uim_variation.h"
#ifdef _lint
  #ifdef LOCAL
    #undef LOCAL
  #endif

  #define LOCAL static
#endif
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "uim.h"
#include "uimdrv.h"
#include "nv.h"
#include "task.h"
#include "modem_mem.h"
#include "rex.h"
#include "log.h"
#include "nv_items.h"
#include "uim_logging.h"
#include "err.h"
#include "mcfg_refresh.h"
#include "mcfg_common.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif /* FEATURE_MODEM_RCINIT */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================

                          DEFINITIONS

===========================================================================*/
#define UIM_FREE(ptr)                                     \
  if(NULL != ptr)                                              \
  {                                                            \
    modem_mem_free((void*)(ptr),MODEM_MEM_CLIENT_UIM);         \
    ptr = NULL;                                                \
  }
#define UIM_ENTER_CRIT_SECTION(crit_sec_ptr, uim_ptr)          \
 if (FALSE == uim_ptr->flag.task_dereg_from_rcinit)            \
  {                                                            \
    rex_enter_crit_sect(crit_sec_ptr);                         \
  }
#define UIM_LEAVE_CRIT_SECTION(crit_sec_ptr, uim_ptr)          \
 if (FALSE == uim_ptr->flag.task_dereg_from_rcinit)            \
  {                                                            \
    rex_leave_crit_sect(crit_sec_ptr);                         \
  }

#define RETURN_IF_INSTANCE_INVALID(instance)                               \
  if (instance >= UIM_MAX_INSTANCES)                                       \
  {                                                                        \
    UIM_MSG_ERR_1("Invalid input param instance id 0x%x", instance);       \
    return;                                                                \
  }

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_PATH_DF1_INDEX       0
#define UIM_PATH_DF2_INDEX       1
#define UIM_PATH_EF_INDEX        2
#define UIM_PATH_DF3_INDEX       2
#define UIM_PATH_EF2_INDEX       3
#define UIM_NO_FILE_SEL          0xFFFF
#define UIM_APP_SELECTION        0xFFFF

/* DF3 and EF2 indices are applicable in case where
   we have UIM path to be selected of length 4 with EF
   at path index 3 for example
   path - 0x7FFF, 0x5Fxx, 0x4Fxx, 8Fxx */

/* 2 bits contain logical channel information */
#define UIM_LOGICAL_CHANNEL_MASK 0x03
/* Mask for get response to remove
   bit 8 and 5 from last sent APDU */
#define UIM_GET_RESPONSE_CLA_MASK 0x6F

/* These define constants used for processing a select command. */
#define UIM_FILE_ID_LEVEL_MASK   0xF000
#define UIM_FILE_ID_LEVEL_SHIFT  12
#define UIM_MF_LEVEL             3
#define UIM_DF_LEVEL_1           7
#define UIM_DF_LEVEL_2           5
#define UIM_EF_UNDER_MF          2
#define UIM_EF_UNDER_1ST_DF      6
#define UIM_EF_UNDER_2ND_DF      4

/* constants for special category of FILES */
#define UIM_FILE_ID_LEVEL_MASK2       0xFF00
#define UIM_FILE_ID_LEVEL_SHIFT2      8
#define UIM_MF_LEVEL_BYTE             0x3F00
#define UIM_DF_LEVEL_1_BYTE           0x7F00
#define UIM_DF_LEVEL_2_BYTE           0x5F00
#define UIM_EF_UNDER_MF_BYTE          0x2F00
#define UIM_EF_UNDER_1ST_DF_BYTE      0x6F00
#define UIM_EF_UNDER_2ND_DF_BYTE      0x4F00
#define UIM_ADF_LEVEL_BYTE            0x1100

#define UIM_DIRECTORY_PATH_LENGTH_1 0x01
#define UIM_DIRECTORY_PATH_LENGTH_2 0x02
#define UIM_DIRECTORY_PATH_LENGTH_3 0x03
#define UIM_DIRECTORY_PATH_LENGTH_4 0x04
#define UIM_DIRECTORY_PATH_LENGTH_5 0x05

#define UIM_MF_ID                0x3F00

/* Class of instruction (ISO/IEC 7816-3) */
#define UIM_INSTRUCTION_CLASS    0xA0
#define UIM_INSTRUCTION_CLASS2   0x80

#define UIM_ICC_INSTRUCTION_CLASS 0xA0

/* This is the main instruction class for UTIL commands.  The logical channel
number (either 1, 2, or 3) is added to this CLA byte while the command is
being built */
#define UIM_UTIL_INSTRUCTION_CLASS1       0x80

/* This is the instruction class for manage channel commands in the util module */
#define UIM_UTIL_INSTRUCTION_CLASS2       0x00

#define UIM_SEL_ADF                     0x1111

#define UIM_PATH_TO_SEL_MF_INDEX        0
#define UIM_UICC_INSTRUCTION_CLASS1     0x00

/* Extended Logical Channel Class */
#define UIM_UICC_EXTENDED_LC_CLASS      0x40

#define UIM_UICC_INSTRUCTION_CLASS2     0x80
#define UIM_SEL_AID_BY_FILE_ID          0x7FFF

#define UIM_USIM_SELECT_BY_FILE_ID      0x00
#define UIM_USIM_SELECT_CHILD_DF        0x01
#define UIM_USIM_SELECT_PARENT_DF       0x03
#define UIM_USIM_SELECT_BY_DF_NAME      0x04
#define UIM_USIM_SELECT_BY_PATH_FROM_MF 0x08
#define UIM_USIM_SELECT_BY_PATH_FROM_DF 0x09

#define UIM_USIM_SESSION_ACTIVATION     0x00
#define UIM_USIM_SESSION_TERMINATION    0x40
#define UIM_USIM_SESSION_NONE           0x60

#define UIM_UICC_SELECT_FIRST_OCCURENCE 0x00
#define UIM_UICC_SELECT_LAST_OCCURENCE  0x01
#define UIM_UICC_SELECT_NEXT_OCCURENCE  0x02
#define UIM_UICC_SELECT_PREV_OCCURENCE  0x03

#define UIM_UICC_RETURN_NO_DATA         0x0C
#define UIM_UICC_RETURN_FCP_DATA        0x04
#define UIM_UICC_RETURN_FMD_DATA        0x08
#define UIM_UICC_RETURN_FCI_DATA        0x00
#define UIM_UICC_RETURN_FCI_W_INTERFACE 0x10


#define UIM_ATR_TS_BYTE_INDEX           0
#define UIM_ATR_TS_DIRECT_CONV          0x3B
#define UIM_ATR_TS_INVERSE_CONV         0x3F

#define UIM_MAX_PARITY_ERR_COUNT(uim_ptr)        ( 2 + uim_ptr->setting.config_params.additional_parity_err_cnt )
#define UIM_MAX_RX_BREAK_ERR_COUNT(uim_ptr)      ( 0 + uim_ptr->setting.config_params.additional_rx_break_err_cnt )

#define UIM_MAX_OVERRUN_ERR_COUNT       10

/* Macro for maximum repeated technical problems */
#define UIM_MAX_REPEATED_TECH_PROBLEMS  3

/* Macro for AID TLV length offset */
#define UIM_AID_TLV_LEN_OFFSET          2

/* The following macros are used for AID decoding/ comparison */
#define UIM_AID_RID_LEN                 0x05
#define UIM_TELECOM_AID_NUM             0x02

/* Sizes for the data of APDU blocks */
#define UIM_CMPT_IP_AUTH_MAX_MN_HA_BLK_SIZE 254
#define UIM_CMPT_IP_AUTH_MAX_HASH_BLK_SIZE  247
#define UIM_CMPT_IP_AUTH_MAX_MN_AAA_CHALLENGE_SIZE 238

/* These are the P1 codes to specify the option in the APDU */
#define UIM_CMPT_IP_AUTH_P1_CHAP            0

#define UIM_CMPT_IP_AUTH_P1_MN_HA           0x1
#define UIM_CMPT_IP_AUTH_P1_MIP_RRQ         0x2
#define UIM_CMPT_IP_AUTH_P1_MN_AAA          0x3
#define UIM_CMPT_IP_AUTH_P1_HRPD            0x4

/* P2 codes to specify which block is being sent */
#define UIM_CMPT_IP_AUTH_P2_FIRST_BLK       0x0
#define UIM_CMPT_IP_AUTH_P2_NEXT_BLK        0x1
#define UIM_CMPT_IP_AUTH_P2_SNGL_BLK        0x2
#define UIM_CMPT_IP_AUTH_P2_LAST_BLK        0x3

/* The following is the number of milliseconds the interface should remain
powered down when switching voltage classes */

#define UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY 12
/* 10 Milliseconds per ISO, adding 2 to compensate
Milliseconds for any VCC Rampdown issues. */

/*
 * Macros and Variables used for card specific error - card
 * loses its context during activation. So, a STATUS command
 * following this is resulting in a poll error.
 */
#define UIM_CSIM_IMSI_M_ADDR_OFFSET    0x7

#define UIM_USIM_IMSI_HIGH_BYTE_MASK   0xF0
#define UIM_USIM_IMSI_LOW_BYTE_MASK    0x0F

#define UIM_USIM_CARD_IMSI_MCC1_VAL     0x3
#define UIM_USIM_CARD_IMSI_MCC2_VAL     0x1
#define UIM_USIM_CARD_IMSI_MCC3_VAL     0x1

#define UIM_USIM_CARD_IMSI_MNC1_VAL     0x4
#define UIM_USIM_CARD_IMSI_MNC2_VAL     0x8
#define UIM_MAX_WAIT_LOCAL_CARD_PUP     0x0A

/* Multiplication factors */
#define UIM_EXTRA_BWT_MUL_FACTOR                                10 /* Multiplication factor with extra BWT */
#define UIM_EXTRA_BGT_MUL_FACTOR                                10 /* Multiplication factor with extra BGT */
#define UIM_EXTRA_CWT_MUL_FACTOR                                10 /* Multiplication factor with extra CWT */
/* Delay before doing clock stop */
#define UIM_DELAY_BEFORE_CLOCK_STOP                             30

/* Delay after clock start */
#define UIM_DELAY_AFTER_CLOCK_RESTART                            5

/* Additional Interface Support (UICC - CLF) TAG */
#define UIM_ADDITIONAL_INTERFACE_SUPPORTED_TAG                    0x82
/* Additional Interface Support (UICC - CLF) LENGTH */
#define UIM_ADDITIONAL_INTERFACE_SUPPORTED_LENGTH                 0x01
/* Additional Interface Support (UICC - CLF) VALUE */
#define UIM_ADDITIONAL_INTERFACE_SUPPORTED                        0x01
/* Additional Interface NOT Support (UICC - CLF) Value */
#define UIM_ADDITIONAL_INTERFACE_NOT_SUPPORTED                    0x00
/* Terminal capability length byte offset */
#define UIM_TERM_CAPABILITY_LEN_BYTE_OFFSET                       0x02

/*The delay between recoveries is a multiple of this macro*/
#define UIM_RECOVERY_DELAY_FACTOR   250

/* Macro to get the command mode type for a command */
#define UIM_GET_CMD_MODE(command) ((command >> 8) & 0x0F)

/* Macro to identify if the command is allowed while UIM is in passive mode */
#define UIM_IS_CMD_VALID_IN_MODE(cmdptr, uim_ptr)                                    \
  (((((int)((cmdptr)->hdr.slot)-1) < (int)UIM_NUM_DRV_SLOTS)                         \
    && (((int)((cmdptr)->hdr.slot)-1) >= 0))?                                        \
    ((uim_ptr->command.mode == UIM_PASSIVE_MODE)?                                    \
                       (((cmdptr)->hdr.command == UIM_RESET_F) ||                    \
                       ((cmdptr)->hdr.command == UIM_RESET_WITH_TP_F) ||             \
                       ((cmdptr)->hdr.command == UIM_RESET_SWITCH_UIM_PASSIVE_F) ||  \
                       ((cmdptr)->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F) ||      \
                       ((cmdptr)->hdr.command == UIM_POWER_DOWN_F) ||                \
                       ((cmdptr)->hdr.command == UIM_STREAM_ISO7816_APDU_F)) :       \
                       (TRUE)) :                                                     \
                       (FALSE))

/* -------------------------------------------------------------------------
** The following data declarations are representations of the CDMA messages
** found in IS-95A (see sections 6.7 and 7.7).  Each bit in the message is
** represented by a byte in the data structure.
** ------------------------------------------------------------------------ */

typedef PACKED struct PACKED_POST
{
  byte randssd[56];     /* Randssd */
  byte proc_control[8]; /* Process Control bits */
  byte esn[32];
  byte esn_pad1[16];
  byte esn_pad2[8];
} uim_update_ssd_req_siz_type;

typedef PACKED struct PACKED_POST
{
  byte rands[32];       /* Rands */
} uim_bs_chal_req_siz_type;

typedef PACKED struct PACKED_POST
{
  byte authbs[24];      /* Randbs */
} uim_conf_ssd_req_siz_type;

typedef PACKED struct PACKED_POST
{
  byte rand_type[8];     /* rand type */
  byte rand[32];         /* rand */
  byte randu[24];        /* rand for unique challenge */
  byte dig_len[8];       /* number of digits */
  byte digits[8];        /* arrray of digits */
  byte proc_control[8];  /* Process control bits */
  byte esn[32];          /* ESN */
  byte esn_pad1[16];     /* padding for ESN */
  byte esn_pad2[8];      /* padding for ESN */
} uim_run_cave_req_siz_type;

typedef PACKED struct PACKED_POST {
  byte auth_sig[24];     /* auth signature */
} uim_run_cave_rsp_siz_type;

typedef PACKED struct PACKED_POST {
  byte randbs[32];       /* RANDBS */
} uim_bs_chal_rsp_siz_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      UIM_FEATURE_BUSY_RESPONSE_TYPE
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8    version;
  /* version 0: NEW FEATURE ITEMS = 2; TOTAL CURRENT FEATURE ITEMS = 2. */
  uint32 uim_busy_ind_timer_val_ms;
  uint32 uim_trans_timer_val_ms;
  /* version 0: RESERVED_FOR_FUTURE_USE size is 32 - 9 = 23 */
  uint8    reserved_for_future_use[23];
} uim_feature_busy_response_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_TYPE
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8    version;
  /* version 0: NEW FEATURE ITEMS = 2; TOTAL CURRENT FEATURE ITEMS = 2. */
  boolean uim_disable_simulate_null;
  uint8 uim_simulate_null_ins;
  /* version 0: RESERVED_FOR_FUTURE_USE size is 5 - 3 = 2 */
  uint8    reserved_for_future_use[2];
} uim_feature_busy_response_simulate_null_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      UIM_FEATURE_EXTENDED_RECOVERY_NV_TYPE
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8       version;
  /* version 0: NEW FEATURE ITEMS = 3; TOTAL CURRENT FEATURE ITEMS = 3. */
  uint8       no_of_extended_powerups;
  uint32      delay_value;
  boolean     incremental_delay;
  /* version 0: RESERVED_FOR_FUTURE_USE size is 13 - 3 = 10 */
  uint16      reserved_for_future_use[10];
} uim_feature_extended_recovery_nv_type;

typedef enum {
  UIM_NO_SUCH_CMD_STATUS = 0,      /* No such command status */
  UIM_CMD_SUCCESS,                 /* Transaction was a success */
  UIM_CMD_ERR,                     /* Transaction was not successful */
  UIM_CMD_ERR_WITH_SFI,            /* Transaction was not successful using SFI */
  UIM_CMD_FETCH,                   /* Command is completed; A proactive command
                                      is pending */
  UIM_CMD_COMPLETED,               /* Command is completed */
  UIM_ICCID_MISMATCH,              /* ICCID mismatch */
  UIM_POLL_ERROR,                  /* Error in polling the card */
  UIM_VOLTAGE_MISMATCH,            /* The card does not support 3V tech */
  UIM_SWITCH_VOLTAGE,              /* Reset the interface and bring up new
                                      voltage */
  UIM_INSTRN_CHANGE,               /* Change instruction class */
  UIM_CONVENTION_CHANGE,           /* Change the convnetion used in the driver */
  UIM_CMD_USB_UICC_REPOLL,
  UIM_CLK_FREQ_CHANGE_NEEDED,
  UIM_MAXIMUM_CMD
} uim_cmd_status_type;

typedef enum {
  UIM_NO_SUCH_CMD = 0x0,
  UIM_GENERIC_CMD = 0x1,
  UIM_GSM_CMD     = 0x2,
  UIM_CDMA_CMD    = 0x3,
  UIM_USIM_CMD    = 0x4,
  UIM_ISIM_CMD    = 0x5
} uim_command_mode_type;

/* Additional Information required for read binary to handle more than 256 */
typedef struct {
  word  num_bytes; /* number of bytes to be read */
  byte *data_ptr; /* ptr to hold the read data */
  word  offset; /* current offset uim would read */
  byte *alloc_mem;
  word  num_bytes_rsp;
} uim_rd_wr_info_type;

/* Structure to hold the get response information that we receive
 * back from the card
 */
typedef struct {
  boolean  is_cached;
  word     data_size;
  byte   * data;
} uim_get_rsp_cache_type;

/*Global strurture to store all information required for Subscription manager configuration*/
typedef struct{
   boolean                   is_sm_data_initialised;
   uint8                     num_physical_slots;  /* Number of physical slots in the target*/
   uim_slot_type             new_active_slot;          /* Current Active slot */
   boolean                   sm_ap_mode_on;
   uim_phy_slot_status_type  uim_slot_info[UIM_NUM_PHY_SLOTS];/*Data of all physical slots*/
   uim_evt_callback_type     *evt_cb_ptr;           /* QMI callback configured with UIM */
   rex_crit_sect_type        uim_sm_data_crit_sect;
}uim_sm_data_type;

 /* Structure to store NV data for active slot configuration */
typedef struct{
  uint8                      version;
  uim_slot_type              active_slot;
  uint8                      RFU[5];
}uim_active_slot_cfg_type;

 /* Structure to store NV for power down on technical problem */
typedef struct {
  boolean to_pdown;
  uint8 cnt_tech_problem;
}uim_pdown_on_tech_problem_type;

typedef enum {
  UIM_JCDMA_SERVICE_DISABLED =0, /* JCDMA Service disabled on ME */
  UIM_JCDMA_SERVICE_ENABLED =1 /* JCDMA Service enabled on ME */
}uim_jcdma_service_status_enum;

typedef struct {
  uim_jcdma_service_status_enum  service; /* JCDMA ME Capability */
}uim_jcdma_me_capability_type;

/* Structure to store NV for UIM connection mode and waiting timer value
   for remote connection request */
typedef PACKED struct PACKED_POST{
  uint8  connection_mode;
  uint8  delay_time_insec;
}uim_last_mode_and_config_delay_for_remote_connect_type;


/* Shadow vars for UART DM registers used internally in the hardware global */
typedef struct
{
  int32 nMR1;
  int32 nMR2;
  int32 nCSR;
  int32 nCR;
  int32 nMISR;
  int32 nIMR;
  int32 nIPR;
  int32 nTFWR;
  int32 nRFWR;
  int32 nRXINIT;
  int32 nIRDA;
  int32 nDMEM;
  int32 nTXNUM;
  int32 nBADR;
  int32 nTXFIFO;
  int32 nRXFIFO;
}uim_uart_shadow_type;

/* The UIM BUSY Indication timer is in one of the following states */
typedef enum {
  UIM_BUSY_RESPONSE_TIMER_NOT_USED = 0,
  UIM_BUSY_RESPONSE_TIMER_NOT_STARTED,
  UIM_BUSY_RESPONSE_TIMER_ACTIVE,
  UIM_BUSY_RESPONSE_TIMER_EXPIRED
}uim_busy_response_timer_state_type;

typedef enum {
  UIM_BUSY_RESPONSE_EVT_NV_INIT,
  UIM_BUSY_RESPONSE_EVT_SEND_APDU,
  UIM_BUSY_RESPONSE_EVT_T_1_SEND_I_BLOCK,
  UIM_BUSY_RESPONSE_EVT_T_1_SEND_R_BLOCK,
  UIM_BUSY_RESPONSE_EVT_T_1_SEND_S_BLOCK,
  UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR,
  UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG,
  UIM_BUSY_RESPONSE_EVT_BUSY_IND_TIMER_EXP_SIG,
  UIM_BUSY_RESPONSE_EVT_TRANS_TIMER_EXP_SIG,
  UIM_BUSY_RESPONSE_EVT_SIMULATE_NULL_TIMER_EXP_SIG,
  UIM_BUSY_RESPONSE_EVT_POWER_DOWN
}uim_busy_response_timer_event_type;

typedef enum 
{
  UIM_EXTENDED_RECOVERY_NO_EVT,            /*default event*/
  UIM_EXTENDED_RECOVERY_EVT_NV_INIT,       /*evt to intialise the extended recovery globals after NV read*/
  UIM_EXTENDED_RECOVERY_EVT_TIMER_START,   /*evt to kick start the timer for the next fresh powerup */
  UIM_EXTENDED_RECOVERY_EVT_TIMER_EXP,     /*evt to prepare UIM for a fresh powerup*/
  UIM_EXTENDED_RECOVERY_EVT_TERMINATE      /*evt to terminate etended recovery*/
}uim_enhanced_recovery_evt_type;

void uim_force_recovery(uim_instance_global_type *uim_ptr);

/*4 bitfields each to save the 4 UIM GPIO values*/
typedef struct{
  uint8 detect:1;
  uint8 reset:1;
  uint8 clk:1;
  uint8 data:1;
}uim_gpio_value_type;

/* uim_gpio_and_clock_info_struct */
/* This structure contains gpio and clock register state */
/* to be saved in a global upon command response timeout so */
/* that the values can be inspected in a crash dump */
typedef struct{
  boolean isInternalCmd; /* Timed out on internal or external command*/
  uint32  num_timeouts;   /* Number of total timeouts since last device power cycle */

  DalTlmm_GpioConfigIdType  reset_config; /* GPIO_CFGn GPIO configuration for GPIO 1 (RESET) */
  DalTlmm_GpioConfigIdType  clk_config; /* GPIO_CFGn GPIO configuration for GPIO 2 (CLK) */
  DalTlmm_GpioConfigIdType  data_config; /* GPIO_CFGn GPIO configuration for GPIO 3 (DATA) */

  /* 4 bitfields each to save the 4 UIM GPIO values */
  uim_gpio_value_type gpio_input;
  uim_gpio_value_type gpio_input_pull_up;

}uim_gpio_info_struct;

typedef struct
{
  uim_path_type path;
  boolean       is_ef;
  uim_aid_type  curr_app;
  boolean       empty_aid_is_active;
}uim_current_path_type;

void uim_rx_isr_1(void);
void uim_rx_isr_2(void);
void uim_rx_isr_3(void);


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/

/*===========================================================================

FUNCTION UIM_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of MMGSDI features as defined by the NVITEM
  'uim_features_status_list'.
  The global, "uim_features_status_list", used to hold the NVITEM, is
  expandable, yet the size is fixed. See the struct definition for more
  detailed doc on that. The global struct has a "version" item that is updated
  everytime we add new items to this global struct (starting with version 0
  in the first implementation)
  The uimdrv code has full control over version of NVITEM. Taking an example,
  if in future, in version 2 of this struct(that has 10 items), the delta is 3
  items (from version 0 that had 7 items), and this build is loaded onto the
  device that already had the items set using version 0 of this NVITEM, then,
  on boot-up after loading the build, uimdrv will validate the version it read
  from NV. If it is not 2, it will update it to 2 and write it back to NV. At
  this point, however, the value of features/items 8 through 10 will be set to
  their default values by uimdrv code.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, uimdrv will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).

===========================================================================*/
void uim_nv_init_features_status_nv_lookup
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_COMMAND_RESPONSE_CALLBACK

DESCRIPTION
  This procedure initiates the processing of the response that has been recd
  from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the UIM_CMD_RSP_SIG.

===========================================================================*/
void uim_command_response_callback
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_RESET_DIR

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure.

===========================================================================*/
void uim_reset_dir
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_RESET_DIR_FOR_SLOT_CHANNEL

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults for the respective slot on the respective channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure for the slot specified.

===========================================================================*/
void uim_reset_dir_for_slot_channel
(
  uim_channel_type       channel,
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_GSDI_LINK_EST

DESCRIPTION
  This procedure sends a link established command to GSDI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_gsdi_link_est
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_GSDI_LINK_EST

DESCRIPTION
  This procedure can be used to report error for the commands still
  remaining in the UIM command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_flush_command
(
  uim_flush_type         flush_type,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_NOTIFY_ERRROR

DESCRIPTION
  This function notifies the higher layer (GSDI) if UIM encounters an
  unexpected error during power up or an internal wake up and passes the
  status of the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_notify_error
(
  uim_status_type         uim_status_param,
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_process_generic_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_SET_POLL_TIMER

DESCRIPTION
  This function should be used to set the time to trigger a poll.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Sets the uim_poll_time_delay to the value passed in.
  Sets a second timer based on feature definition to ensure
  wake-up.

===========================================================================*/
void uim_set_poll_timer(
  rex_timer_cnt_type        time,
  rex_timer_cnt_type        wake_up_poll_offset,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_FREE_RESET_AUTH_INFO

DESCRIPTION
  Frees the auth info memory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_free_reset_auth_info
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_NV_INIT

DESCRIPTION
  This procedure is called to initialize all UIMDRV NV items.
  It is called all UIM task but the NV's are read only once by the task
  that initializes first.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void uim_nv_init
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_READ_MCFG_REFRESHED_NV

DESCRIPTION
  This procedure is called to read all the carrier dependent MCFG refresh slot based 
  UIMDRV NV items.
  And inform MCGF once the read is finished.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void uimdrv_read_mcfg_refreshed_nv
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_HANDLE_MCGF_NV_REFRESH_CB

DESCRIPTION
  This procedure a MCGF callback. it is called to set UIM_MCGF_NV_REFRESH_SIG

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean uimdrv_handle_mcgf_nv_refresh_cb
(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
);


/*===========================================================================

FUNCTION UIM_PET_AND_WAIT

DESCRIPTION
  This procedure accepts rex_sigs_type mask and keeps waiting for any of
  the signals in the mask specified.  Also, if it is time to report to
  the dog, the dog would be pet.

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type uim_pet_and_wait
(
  rex_sigs_type          sigs_to_wait_for,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_CLEAR_POLL_TIMER

DESCRIPTION
  This function clears any pending poll timer(s).

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Clears all the timers relating to a poll.
===========================================================================*/
void uim_clear_poll_timer(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_RESET_POLL_TIMER_VAL

DESCRIPTION
  This function resets UIM POLL timer value to UIM_POLL_TIME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_reset_poll_timer_val
(
  uim_instance_global_type *uim_ptr
);


void uim_prg_imr_mask_all(uim_instance_global_type *uim_ptr);
/*===========================================================================

FUNCTION UIM_PROCESS_CARD_RESPONSE

DESCRIPTION
  This procedure processes the response received from the UIM card and changes
  the UIM server control variables based upon the processing status.

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  None.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/
void uim_process_card_response
(
 rex_sigs_type            *i_mask,   /* Mask of signals interested in */
 uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_DEV_INIT

DESCRIPTION
  Initialize the clock regime, configures the GPIOs, MSM UART2 serial
  interface.  It places the UIM power line low, the UIM reset line low,
  the I/O line low and the clock line low.

DEPENDENCIES
  The routine MUST be called before any other UIM driver function.  It must
  only be called once on power up of the phone.

RETURN VALUE
  None

SIDE EFFECTS
  This function leaves ALL the UIM lines low after it executes.

===========================================================================*/
void uim_dev_init
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

  FUNCTION UIM_CLEAR_CMD_RSP_TIMER

  DESCRIPTION
   This procedure will clear the command response timer

  DEPENDENCIES


  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
void uim_clear_cmd_rsp_timer
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_BUSY_IND_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_busy_ind_timer,
  when some event happens.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_busy_ind_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type busy_timer_event
);


/*===========================================================================

FUNCTION UIM_TRANS_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_trans_timer,
  when some event happens.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_trans_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type trans_timer_event
);


/*===========================================================================

FUNCTION UIM_SIMULATE_NULL_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_simulate_null_timer,
  when some event happens. This function is only for SIM BUSY Feature testing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type simulate_null_timer_event
);


/*===========================================================================

FUNCTION UIM_SEND_ERR_RPT

DESCRIPTION
  This function, based on the command mode calls the module that sends an
  error report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The command buffer is released.

===========================================================================*/
void uim_send_err_rpt
(
  uim_cmd_type             *cmd_ptr,  /* the command to queue up */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_PROCESS_COMMAND

DESCRIPTION
  This procedure dispatches the command to the respective module based on the
  type of the command if the path to execute the command is correct. If not,
  this function queues the external command back into the head of the command
  queue and issues an internal select to the required path.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_process_command
(
  rex_sigs_type             *mask,
  /* rex signals type mask */
  uim_instance_global_type  *uim_ptr
);


/*===========================================================================

FUNCTION UIM_PROCESS_CMD_ERR

DESCRIPTION
  This procedure processes the error response received from the UIM card

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  cmd_ptr value.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/
void uim_process_cmd_err
(
  rex_sigs_type            *iMask_ptr,   /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_RESET_UIM

DESCRIPTION
  This procedure builds the static command for power up and issues it. It
  issues different command based on whether the reset is for a wake up or
  a power up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_reset_uim
(
  rex_sigs_type            *mask,
  boolean                   me_powerup,
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_GET_UART_STATUS()

DESCRIPTION
  This procedure safely calls the uim_read_status() by first checking to make
  sure that the power and clock for the appropriate UIM is still on.
  If it isn't, it'll return 0 instead.  It always checks for the slot currently
  in use.

DEPENDENCIES
  None

RETURN VALUE
  Either Read Status or 0.

SIDE EFFECTS
  None

===========================================================================*/
dword uim_get_uart_status
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_RESET_UART_SLOT                         INTERNAL FUNCTION

DESCRIPTION
  This function resets the UART used to communicate with the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The UART will no longer be transmitting nor receiving data.  The receive
  and transmit FIFOs will be flushed.

===========================================================================*/
void uim_reset_uart_slot
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================
  FUNCTION UIM_START_CMD_RSP_TIMER

  DESCRIPTION
   This procedure will start the command response timer.

  DEPENDENCIES


  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
void uim_start_cmd_rsp_timer
(
  dword                     cmd_rsp_time_val,
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_SEND_APDU                          INTERNAL FUNCTION

DESCRIPTION
  This function starts the transfer of an APDU to the UIM.  This function
  starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the APDU response.  Some commands solicit a GET_RESPONSE or a FETCH command
  to be sent after the APDU.

===========================================================================*/
void uim_send_apdu
(
  uim_req_buf_type         *uim_req, /* This points to the APDU command buffer, the
                                        command data buffer, the response buffer
                                        the APDU status and the callback function */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_RESET_RECEIVER                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function resets the receiver and places the rx ISR to the idle
  state.

DEPENDENCIES
  This function inactivates the UART receiver.  Any data received while
  in idle are processed as errors.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UART receiver is disabled and the
  Rx ISR is in the idle state.

===========================================================================*/
void uim_reset_receiver
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_IS_GET_RESPONSE_ALLOWED

DESCRIPTION
  This procedure determines if get response should be allowed or not in case
  of warnings status words for CASE 4 instruction

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if SEEK command and ens is enabled or gcf testing is going on
  FALSE  if SEEK command and both ens or gcf testing flags are off.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_get_response_allowed
(
  uim_instance_global_type *uim_ptr,
  uint8                     sw1,
  uint8                     sw2
);


/*===========================================================================
FUNCTION UIMDRV_WAIT_FOR_QMI_ACKNOWLEDGEMENT

DESCRIPTION
  Waits on the required acknowledgement from QMI_UIM that is inturn waiting on
  the REQ to come in from the AP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uimdrv_wait_for_qmi_acknowledgement 
(
  uim_instance_global_type     *uim_ptr, 
  uimdrv_qmi_acknowledgements_type  required_ack
);


boolean sfi_parse_fcp_for_is_ef
(
  uim_instance_global_type *uim_ptr,
  byte const               *data,
  word                      num_bytes,
  uim_channel_type          curr_ch
);

void uim_update_current_dir_uicc
(
  uim_instance_global_type *uim_ptr,
  boolean                   is_ef,
  uim_select_cmd_type      *sel_cmd_ptr,
  uim_channel_type          curr_ch
);

void uim_update_current_dir_sfi
(
  uim_instance_global_type *uim_ptr,
  uim_path_type            *path,
  uim_channel_type          curr_ch
);

void uim_update_current_dir_icc
(
  uim_instance_global_type *uim_ptr,
  boolean                   is_ef,
  uim_path_type            *path,
  uint8                     path_position
);

void uim_print_curr_path
(
 const uim_path_type      *path,
 uim_instance_global_type *uim_ptr
);

void uim_print_curr_path_ext
(
 const uim_path_type      *path,
 uim_instance_global_type *uim_ptr

);

void uim_clear_current_path
(
  uim_instance_global_type *uim_ptr,
  uim_channel_type          curr_ch
);

void uim_explicit_reselect_mf_aid
(
  uim_instance_global_type  *uim_ptr
);

void uim_extended_recovery_event_handler
(
  uim_instance_global_type *uim_ptr,
  uim_enhanced_recovery_evt_type  evt
);

void uim_queue_internal_open_channel_command
(
  uim_instance_global_type  *uim_ptr
);
#endif /* UIMI_H */
