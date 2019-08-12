/*!
  @file
  fws_msg.h

  @brief
  Modem firmware services: message router messages.

  @detail
  UMID and message definitions to communicate with the firmware DSP.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/fws_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/13   rjg     Removed obsolete messages - phase 2.
05/01/12   rjg     Removed obsolete messages for unicore - phase 1.
04/25/11   rjg     Added SMEM_REGION_REQ/RSP messages.
04/15/11   rjg     Added SAW_CONFIG and modified suspend command.
10/01/10   rjg     Added PMU messages
09/12/10   ckk     Added WDOG_REPORT_TMRI messages
06/14/10   rjg     Added SUSPEND_CMD and SUSPEND_RSP messages.
10/07/09   rjg     Initial version.
===========================================================================*/

#ifndef FWS_MSG_H
#define FWS_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "msgr.h"
#include "fws_types.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief FWS UMID definitions.
 */
enum {
  MSGR_DEFINE_UMID( MCS, FWS, CMD,  APP_CONFIG,     0x00, fws_app_config_cmd_t    ),
  MSGR_DEFINE_UMID( MCS, FWS, CMD,  SUSPEND,        0x01, fws_suspend_cmd_t       ),
  MSGR_DEFINE_UMID( MCS, FWS, CMD,  WAKEUP,         0x02, fws_wakeup_cmd_t        ),

  MSGR_DEFINE_UMID( MCS, FWS, RSP,  APP_CONFIG,     0x00, fws_app_config_rsp_t    ),
  MSGR_DEFINE_UMID( MCS, FWS, RSP,  SUSPEND,        0x01, fws_suspend_rsp_t       ),
  MSGR_DEFINE_UMID( MCS, FWS, RSP,  WAKEUP,         0x02, fws_wakeup_rsp_t        ),

  MSGR_DEFINE_UMID( MCS, FWS, IND,  DIAG_MSG,       0x00, fws_diag_msg_ind_t      ),
  MSGR_DEFINE_UMID( MCS, FWS, IND,  DIAG_LOG,       0x01, fws_diag_log_ind_t      ),

  MSGR_DEFINE_UMID( MCS, FWS, TMRI, WDOG_REPORT,    0x00, fws_wdog_report_tmri_t  ),

  MSGR_DEFINE_UMID( MCS, FWS, SPR,  SHUTDOWN,       0x00, fws_shutdown_spr_t      ),
};


/*! @brief Application configuration command message.

    "Enabling" an application refers to initializing a technology-specific
    FW module to a state where it can respond to SW commands through its own 
    interface, e.g. registering its messages, or mapping a shared memory buffer. 
    Once a configuration response has been received, SW may then issue further 
    commands through the application-specific interface.

    "Disabling" an application will free up any owned resources, including
    its SW interface. A disabled application will not respond to any 
    application-specific commands.

    This message specifies zero or more FW applications to disable,
    and zero or more FW applications to enable.
    All of the application disables are performed before any of the 
    enables.

    A MCS_FWS_APP_CONFIG_RSP message is returned once all the enables and 
    disables have completed, and contains the updated mask of enabled applications.
    @see fw_app_config_rsp_t

    If the disable mask specifies an application that has already been 
    disabled, or the enable mask specifies an application that has already
    been enabled, then that application will be silently ignored.
    If either mask specifies an application that does not exist in the image,
    then an error status is returned.
    If an application fails enable or disable (for example, if resources
    are not available), then its status will not change and an error status
    will be returned. 

    e.g.
    disable_mask = FW_APP_NONE; enable_mask = FW_APP_CDMA1X;
    - enables the CDMA1X FW application
    disable_mask = FW_APP_CDMA1X; enable_mask = FW_APP_HDR | FW_APP_C2KSRCH
    - disables the CDMA1X FW application, then enables the HDR and C2KSRCH
      applications
*/
typedef struct
{
  msgr_hdr_s   hdr;           /*!< MSGR header */

  uint32       disable_mask;  /*!< Bitmask of FW_APP_x to disable */
  uint32       enable_mask;   /*!< Bitmask of FW_APP_x to enable */

} fws_app_config_cmd_t;



/*! @brief Application configuration response message.

    This message is always sent in response to a MCS_FWS_APP_CONFIG_CMD
    message.

    The enabled_mask field reflects all the currently enabled applications, after
    applying the disable/enable commands. Note that this includes applications
    that were previously enabled, not just the ones in the most recent command.

    The error_mask bitmask is non-zero for any technology which caused an error,
    either because it was not present, or because it returned an error status
    during enable/disable (if resources were not available, for example).

    The bitmask of applications present in the image and which are currently
    enabled is also available through the FWS shared-memory interface.
    @see fw_app_smem_intf_t

    SW should treat any error response as a serious error, indicating that 
    there is a mismatch between SW/FW (SW trying to enable an app that is 
    not present), or a configuration is not valid (an app fails disable because
    it is still in traffic mode, for example).

    e.g.
    Command: disable_mask = FW_APP_NONE; enable_mask = FW_APP_CDMA1X;
    Response: enabled_mask = FW_APP_CDMA1X; error_mask = FW_APP_NONE;
    - CDMA1X application was successfully enabled, no errors
    Command: disable_mask = FW_APP_NONE; enable_mask = FW_APP_HDR | FW_APP_C2KSRCH
    Response: enabled_mask = FW_APP_CDMA1X | FW_APP_C2KSRCH; error_mask = FW_APP_HDR;
    - C2KSRCH successfully was enabled, HDR is either not present or generated
      an error condition during enable. CDMA1X was previously enabled, and 
      remains enabled.
    Command: disable_mask = FW_APP_CDMA1X; enable_mask = FW_APP_NONE;
    Response: enabled_mask = FW_APP_CDMA1X | FW_APP_C2KSRCH; error_mask = FW_APP_CDMA1X;
    - CDMA1X generated an error when trying to disable. It remains in the bitmask
      of enabled applications.
*/
typedef struct
{
  msgr_hdr_s   hdr;           /*!< MSGR header */

  uint32       enabled_mask;  /*!< Bitmask of currently enabled apps */
  uint32       error_mask;    /*!< Bitmask of applications which caused
                                   an error during enable or disable. */

} fws_app_config_rsp_t;


/*! @brief Firmware suspend command.

    This message requests the firmware to prepare for sleep state. 

    Firmware may reject the command if it is unable to enter the requested
    state (for example, if it is currently using resources which require clocks).

    This command may be sent even if firmware apps are enabled, as long
    as those applications have released resources which allow sleep.

    The MCS_FWS_SUSPEND_RSP message is always sent in response to this command, with
    and indication of whether it was successful.

*/
typedef struct
{
  msgr_hdr_s       hdr;        /*!< MSGR header */

  fw_sleep_mode_e  mode;       /*!< Indicates the sleep mode */

} fws_suspend_cmd_t;


/*! @brief Firmware suspend response message.

    This message is always sent in response to an MCS_FWS_SUSPEND_CMD message.

    The firmware may reject the suspend command if it is unable to transition
    to the requested state, such as if it is currently using resources which
    require clocks.

    If the firmware successfully transitioned to the requested mode, it
    returns error=0. 
    Otherwise, error is non-zero, and error_info contains additional information
    (actual values are for internal debug only).
*/
typedef struct
{
  msgr_hdr_s   hdr;           /*!< MSGR header */

  uint32       error;         /*!< error=0 for successful transition.
                                   Other values indicate the suspend command
                                   was rejected. */
  uint32       error_info;    /*!< Additional  information for when error
                                   is set. */

} fws_suspend_rsp_t;


/*! @brief Firmware wakeup command.

    This message requests the firmware to wake up from sleep state. 

    The MCS_FWS_WAKEUP_RSP message is always sent in response to this command, 
    when the wakeup process is complete.
*/
typedef struct
{
  msgr_hdr_s       hdr;        /*!< MSGR header */

} fws_wakeup_cmd_t;


/*! @brief Firmware wakeup response message.

    This message is always sent in response to an MCS_FWS_WAKEUP_CMD message,
    after the wakeup process is complete.
*/
typedef struct
{
  msgr_hdr_s   hdr;           /*!< MSGR header */

} fws_wakeup_rsp_t;


/*! @brief Shared memory region request message.
 
    This is a request for the physical address and size of the FW processor's
    shared memory region.
 
    The response will be MCS_FWS_SMEM_REGION_RSP.
*/
typedef struct
{
  msgr_hdr_s      hdr;        /*!< MSGR header */

} fws_smem_region_req_t;


/*! @brief Shared memory region response message. 
 
    This message is sent in response to a MCS_FWS_SMEM_REGION_REQ message.
    It contains the physical address and size of the FW processor's shared
    memory region.
*/
typedef struct
{
  msgr_hdr_s      hdr;        /*!< MSGR header */

  uint64          phys_addr;  /*!< Physical address of start of shared memory */
  uint32          size;       /*!< Size of shared memory region */

} fws_smem_region_rsp_t;


/*! @brief Formatted "F3" message MSGR indication, from FW DSP to FWS
*/
typedef struct
{
  msgr_hdr_s           hdr;      /*!< MSGR header */

  uint8                type;     /*!< Message type */
  uint8                ssid;     /*!< Message subsystem ID */
  uint16               line;     /*!< Line number of message */
  uint8                num_args; /*!< Number of arguments [0,FW_DIAG_MAX_MSG_ARGS] */      
  uint8                fn_ofs;   /*!< Offset from start of "strings" for filename */
  uint8                fmt_ofs;  /*!< Offset from start of "strings" for format */
  uint8                reserved; /*!< Padding for 8-byte alignment */

  /*! @brief Variable-length data: 
      - "num_args" words of arguments
      - filename is nul terminated and located at strings[fn_ofs]. Only the
        base filename+extension, no path.
      - format string is nul terminated and located at strings[fmt_ofs].
      - filename and format strings will never overlap with each other or
        with the valid arguments. 
      - there may be padding/unused space in the packet, compared to the 
        size indicated in the header. 
   */
  union {
    /*! Message arguments: zero to FWS_MAX_DIAG_MSG_ARGS */
    int32 args[FW_DIAG_MAX_MSG_ARGS];    

    /*! Filename and format strings stored at fn_ofs and fmt_ofs. 
        We allocate 128 characters for the concatenation of format and 
        filename strings. If there are fewer than max # arguments, that
        space can be used for strings also. */
    char strings[FW_DIAG_MAX_MSG_ARGS*sizeof(int32) + 128];
  } data;

} fws_diag_msg_ind_t;


/*! @brief Binary log MSGR indication, from FW DSP to FWS.
*/
typedef struct
{
  msgr_hdr_s    hdr;      /*!< MSGR header */

  /*! Variable-length data:
     - a fw_diag_log_sub_header_t subheader, 8-byte aligned
       - "code" is the FW logging code (NOT AMSS log code - needs translation)
       - "length" is the length of the following data in bytes (not including subheader)
     - "length" bytes of logging packet data
     - 0 to 7 bytes of padding, to reach 8-byte alignment
     - done when current offset == MSGR packet length
    */
  uint8 data[FW_DIAG_LOG_MAX_PACKET_SIZE];  

} fws_diag_log_ind_t;


/*! @brief Shutdown message for FWS task.
    FWS task always runs, only expected to be used for QTF testing.
*/
typedef struct
{
  msgr_hdr_s   hdr;      /*!< MSGR header */

  /* No data */
} fws_shutdown_spr_t;


/*! @brief WATCHDOG timer report message for FWS task.
    FWS task needs to report preiodically to WATCHDOG services.
*/
typedef struct
{
  msgr_hdr_s   hdr;      /*!< MSGR header */

  /* No data */
} fws_wdog_report_tmri_t;


#endif /* FWS_MSG_H */
