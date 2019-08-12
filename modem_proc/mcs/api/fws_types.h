/*!
  @file
  fws_types.h

  @brief
  FW services interface.

  @detail
  Defines, types, and shared memory interfaces for FW-SW communication.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/fws_types.h#1 $

===========================================================================*/

#ifndef FWS_TYPES_H
#define FWS_TYPES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/* Don't include comdef.h here - this file is shared by FW builds also. */


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Maximum number of arguments for a diagnostic "F3" message. 
*/
#define FW_DIAG_MAX_MSG_ARGS            ( 9 )

/*! @brief Maximum size of logging packet (including sub-header), in bytes.
*/
#define FW_DIAG_LOG_MAX_PACKET_SIZE     ( 4096 )


/*! @brief FW application IDs (bitmasks). 
    These are used in the MCS_FWS_APP_CONFIG message, and may be combined.
*/
#define FW_APP_NONE        0x00000000   /* No application */
#define FW_APP_CDMA1X      0x00000001   /* CDMA2000 1x */
#define FW_APP_C2KSRCH     0x00000002   /* Searcher for 1x and HDR */
#define FW_APP_HDR         0x00000004   /* HDR/1xEV-DO */
#define FW_APP_GERAN       0x00000008   /* GERAN */
#define FW_APP_WCDMA       0x00000010   /* WCDMA/UMTS */
#define FW_APP_LTE         0x00000020   /* LTE */
#define FW_APP_TDSCDMA     0x00000040   /* TDSCDMA */
#define FW_APP_RFCMD       0x00000080   /* Commands from RF SW to FW (FTM) */
#define FW_APP_MCS         0x00001000   /* MCS test app */
#define FW_APP_GERAN2      0x00002000   /* GERAN 2nd instance (DSDA) */
#define FW_APP_GERAN3      0x00004000   /* GERAN 3rd instance (TSTS) */
/* Bitmasks 0xFFFF0000 are reserved for FW internal use */


/*! @brief Fatal message SSID. Must match MSG_SSID_DFLT (assert in code to check). */
#define FWS_MSG_SSID_DFLT  0

/*! @brief Fatal message mask. Must match MSG_LEGACY_FATAL (assert in code to check). */
#define FWS_MSG_MASK_FATAL (0x00000010)


/*! @brief Firmware operating states.
*/
typedef enum
{
  FW_STATE_UNINIT = 0,        /*!< FW has not initialized yet */
  FW_STATE_STARTUP,           /*!< FW has started initialization process */
  FW_STATE_ACTIVE,            /*!< FW has completed initialization, is active */
  FW_STATE_SUSPEND,           /*!< FW is in suspended state */
  FW_STATE_WAKEUP,            /*!< FW is doing wakeup initializations */
  FW_STATE_HALTING,           /*!< Fatal error encountered, dump is in progress */
  FW_STATE_ERROR              /*!< DSP Error code is pending. See message for info. */
} fw_state_e;


/*! @brief Firmware commands.
*/
typedef enum
{
  FW_CMD_NONE = 0,            /*!< No pending command */
  FW_CMD_SUSPEND,             /*!< Enter SUSPEND state */
  FW_CMD_RESUME,              /*!< Resume from SUSPEND state */
  FW_CMD_ERROR                /*!< Perform error shutdown */
} fw_command_e;


/*! @brief Firmware processor sleep modes. 
    Full descriptions of these are in the fws_sleep_request() header.
*/
typedef enum
{
  FW_SLEEP_PROBE,             /*!< A sleep probe, not a true sleep command */
  FW_SLEEP_DEBUG,             /*!< Debug mode */
  FW_SLEEP_CLK_GATE,          /*!< Clock gating mode only */
  FW_SLEEP_VDDMIN,            /*!< Vdd minimization, memory retention */
  FW_SLEEP_PWRDN_CORE,        /*!< Power collapse only core, retain memory */
  FW_SLEEP_PWRDN_FULL         /*!< Powre collapse both core and memory */
} fw_sleep_mode_e;


/*! @brief Sleep memory retention configuration. 
*/
typedef struct
{
  uint32 mem_slp_cntl;  /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL */
  uint32 mem_slp_cntl2; /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL2 */
  uint32 spare[2];      /*!< Future use */
} fws_sleep_cfg_t;


/*! @brief FW services shared memory interface.
*/
typedef struct ALIGN(32)
{
  /*===== The following fields are read-only to SW =====*/

  /* FW image information */
  char            version[64];        /*!< Firmware version */
  char            date[16];           /*!< Compile date */
  char            time[16];           /*!< Compile time */

  fw_state_e      state;              /*!< FW current operating state */

  /* Fatal error information, when state == FW_STATE_HALTING or FW_STATE_ERROR */
  char            err_message[256];   /*!< Fatal error message ("printf" style) */
  int32           err_args[5];        /*!< Arguments to message */
  uint8           err_num_args;       /*!< Number of valid arguments (remainder are undefined) */
  char            err_filename[256];  /*!< Filename of fatal error */
  uint16          err_line;           /*!< Line number of fatal error */
  
  /* FW application information */
  uint32          apps_present;       /*!< Bitmask of applications available */
  uint32          apps_enabled;       /*!< Bitmask of applications currently enabled */


  /*===== The following fields are writable by SW =====*/

  /* FW command. Start at a new cache line. */
  fw_command_e    command    ALIGN(32);  /*!< Write command to FW */

  /* FW return parameters, read-only to SW. */
  fws_sleep_cfg_t sleep_cfg  ALIGN(32);  /*!< Sleep retention configuration */
  uint32          debug[60];             /*!< Debug data for FW internal use */

  /*===== FW internal configuration parameters =====*/  
  uint8           config[16384] ALIGN(32);

} fws_smem_intf_t;


/*! @brief Logging packet subheader. 
*/
typedef struct ALIGN(8)
{
  uint16 code;      /*!< AMSS log code */ 
  uint16 length;    /*!< Length of subpacket, not including subheader */
  uint32 reserved;  /*!< Pad to 8-bytes. Implied by alignment, but make sure. */

} fw_diag_log_sub_header_t;


/*! @brief Fatal message format, for error forwarding from FW to FWS.
    This must match err.h msg_const_type, redefined to avoid CoreBSP dependenices
    on FW (and checked by asserts in SW). */
typedef struct __attribute__((packed))
{
  uint16 line;          /*!< Line number in source file */
  const char *fmt;      /*!< Printf style format string */
  const char *fname;    /*!< Pointer to source file name */

  uint32 num_args;      /*!< Number of arguments. For FW standalone use, ignored by err. */

} fws_fatal_msg_t;


#endif /* FWS_TYPES_H */
