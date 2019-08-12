#ifndef UIMGEN_H
#define UIMGEN_H
/*===========================================================================

        E X T E R N A L   U I M  G E N E R I C   S T A T E   M A C H I N E
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM generic state machine.

Copyright (c) 2001,2002,2008-2014, 2016 by QUALCOMM Technologies, Inc. All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimgen.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/16   dd      Enhancing the voltage supply indication to QMI feature
04/11/16   ks      Code changes needed for LPA support
11/26/15   sam     Add logic to update EF-TST in recovery
09/22/15   ll      Remove unused #define UIM_5V_UIM_UICC
09/18/14   akv     UIMDRV UT enhancements
07/31/14   ak      Feature flags cleanup
07/25/14   sam     Fix to send Status command when Apps are reopened during recovery 
04/02/14   lxu     Send USB PPS request without PPS1 byte
03/27/14   lxu     USB UICC modem initial implement
02/02/14   nmb     SFI initial engg revision
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
01/14/14   nmb     Remove ACCESS_F and related unused functionality
12/21/13   am      Silent Recover Feature Implementation
10/24/13   akv     Removed unnecessary feature flags
10/22/13   ll      Add UIM BUSY Response feature
10/07/13   ak      Remove dead code - uimtk.c, uimgnusr.c
09/26/13   ak      Remove unnecessary feature flags
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
07/25/12   ms      Flushing manage channel commands for ICC recovery
                   Smart recovery for extended logical channels
05/21/12   yb      Robust fix to handle 61 XX chained response
03/18/12   av      Replace feature flags with NVITEM
03/08/12   nmb     Notify card of logical channel support
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/30/11   shr     Removed featurization for ALWAYS ON features
12/28/11   ms      Enter into recovery in case of braf_value equal to zero
                   to avoid divide by 0 crash
08/30/11   yb      Added support for complete recovery solution
09/01/10   ps      Merge UIM Driver DSDS changes
07/01/10   nmb     Updates for JCDMA CSIM support
03/12/10   rm      Fix to handle TA4 for clock stop level
02/18/10   vs      Merged CSIM functionality
10/08/09   yb      In case of stream apdu:
                   1. Report bad p3 status words to the clients.
                   2. Do not perform resend apdu internally.
08/03/09   yb      Fix to do get response for seek command if a GCF test case
                   is running or NV_ENS_ENABLED_I NV item is enabled.
07/28/09   yb      Fixed Compilation error after enabling feature
                   UIM_FEATURE_ALLOW_CHV2_DISABLE
04/15/09   ssr     Fixed limited Length TERMINAL PROFILE for non-spec
                   compliant cards
03/23/09   sun     Added support for virtuim in QTF
01/28/09   nb      Changes to support virtuim in QTF
12/17/08   nb      Common Modem Interface
09/12/08   nb      Added support to handle warnings returned by Envelope Command
08/08/08   vs      Removed featurization
08/05/08   vs      Renamed start and stop clock functions appropriately
07/17/08   vs      Added support for UIM USB UICC ICCD interface
06/03/08   rm      TA1=0x97 support
06/12/06   pv      Get rid of FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN and
                   add states for UIM_RECOVERY_COMPLETE_F
04/03/06   pv      Lint Fixes
03/09/06   pv      Added new state to clear the response cache
11/01/05   tml     Combine last sel EF with last sel Dir
10/19/05   pv      Added definition for UIM_DELAY_AFTER_ATR_ST for introducing
                   delay between the reception of the ATR and PPS request.
06/16/05   sk      Fixed UIM_CSIM_APDU_MIN_SIZE value
06/13/05   pv      Added support for new UIM commands with malloc
05/10/05   pv      BT sim access changes - Added POWER_DOWN_ST
02/03/05   jk      Changed FEATURE_UIM_RUN_TIME_ENABLE to FEATURE_UIM_RUIM
                   around CHECK_FOR_CDMA_DF to allow CDMA to function okay
                   in 1x builds without RUN_TIME_ENABLE
01/12/05   pv      Externalized variables required for timeout logging and for
                   Driver time stamping mechanism
10/11/04   pv      Added code to parse the ATR for voltage class information
09/01/04   ck      Added support for CSIM.
05/17/04   ck      Added definitions to support for ISO 7816 streaming API.
10/17/03   ck      Added APDU_STREAM_ST to support streaming APDU.
09/19/03   ck      Changed the flag uim_atr_recd to uim_atr_pps_done.
08/14/03   ck      Added support for dual slots.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/22/03   jk      Added UTIL and WIM support
03/04/03   ck      Removed the constants used for processing a select command
02/13/03   wli     Added support to GSTK.
11/21/02   ts      Added a new state to check for the CDMA DF for RTRE.
11/21/02   ts      Added constants for default PPS.
10/29/02   ck      Externalized uim_atr_recd flag.
10/02/02   ts      Include definitions for checking TC1 (extra guardtime).
09/05/02   wli     Put parenthesis around definition of the UIM_CLOCK_STOP_ALL_BITS.
07/18/02   wli     Increased UIM_MAX_NUMBER_GEN_STATES to 4
07/09/02   ck      Modified the prototype for the function uim_stop_uim_clock()
                   to return a boolean type.
04/09/02   ck      Modified the featuization around clock stop functions to
                   include FEATURE_UIM_USIM.
03/27/02   ck      Modified the definition for 3V Tech bit for GSM and added a
                   similar one for USIM.
02/14/02   ts      Removed definitions for DFs as they are already defined in
                   uimi.h.  Explicitly include uimi.h.
01/22/02   ts      Added support for processing file characteristics.  This is
                   used to determine clock stop preferences.
11/13/01   ts      Added support for UIM Application Toolkit (UTK).
07/27/01   ts      Added constants to support processing the ATR and PPS for
                   changing operational parameters.
07/26/01   wli     Moved some definitions to uim.h
05/28/01   ck      Added support for multi mode server.
01/17/01   ts      Created module.  This module contains the generic state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "comdef.h"
#include "uimi.h"
#include "uim.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_CHV_PAD_CHAR         0xFF
#define UIM_MAX_PPS_CHARS        6
#define UIM_PPS_DEFAULT_PPSS     0xFF
#define UIM_PPS_DEFAULT_PPS0     0
#define UIM_PPS_DEFAULT_PCK      0xFF
#define UIM_PPS_DEFAULT_REQ_SIZE 3
#define UIM_PPS_PPS1_PPS0        0x10
#define UIM_TA1_INDEX            2
#define UIM_PPS_REQ_SIZE         4
#define UIM_FI_SHIFT_OF_TA1      4
#define UIM_DI_MASK_OF_TA1       0x0F
#define UIM_PPS_USB_PPS0         0x2F
#define UIM_PPS_USB_PPS2         0xC0

/* Definitions to check if we need to send a PPS based on the ATR */
#define UIM_ATR_T0_BYTE          1
#define UIM_ATR_TA_PRESENT       0x10
#define UIM_ATR_TB_PRESENT       0x20
#define UIM_ATR_TC_PRESENT       0x40
#define UIM_ATR_TD_PRESENT       0x80
#define UIM_ATR_TA1_BYTE         2
#define UIM_ATR_TA2_IND          0x10
#define UIM_NO_PPS_VALUE1        0x11
#define UIM_NO_PPS_VALUE2        0x01
#define UIM_TC1_IS_0             0x00
#define UIM_TC1_IS_255           0xFF

/* Definitions to check the bits in the 8th and 5th position are set.*/
#define UIM_BIT8_CHECK           0x80
#define UIM_BIT5_CHECK           0x10

#define UIM_P2_NEXT_RECORD_MODE  2
#define UIM_P2_PREV_RECORD_MODE  3
#define UIM_P2_ABS_RECORD_MODE   4

#define UIM_MAX_NUMBER_GEN_STATES 3

/* This is the index of the file characteristics byte in the response
   buffer of selecting the GSM directory. */
#define UIM_FILE_CHAR_INDEX      13

#define UIM_VOLTAGE_CLASS_BITS   0x70

#define UIM_3V_SIM_GSM           0x10
#define UIM_1_8_SIM_GSM          0x20
/* Defines the supported voltages for SIM cards */
#define UIM_VOLTAGES_SUPPORTED_GSM (UIM_3V_SIM_GSM | UIM_1_8_SIM_GSM)

#define UIM_3V_UIM_UICC          0x20
#define UIM_1_8_UIM_UICC         0x40
/* Defines the supported voltages for UICC cards */
#define UIM_VOLTAGES_SUPPORTED_UICC (UIM_3V_UIM_UICC | UIM_1_8_UIM_UICC)

#define UIM_UI_MASK              0x3F
#define UIM_UI_3V_UIM_UICC       0x02
#define UIM_UI_1_8_UIM_UICC      0x04

#define UIM_3V_TECH_BIT_GSM      0x10
#define UIM_3V_TECH_BIT_UICC     0x20
#define UIM_CLOCK_STOP_B1        0x01
#define UIM_CLOCK_STOP_B3        0x04
#define UIM_CLOCK_STOP_B4        0x08
#define UIM_CLOCK_STOP_ALL_BITS  (UIM_CLOCK_STOP_B1 | \
                                 UIM_CLOCK_STOP_B3 | \
                                 UIM_CLOCK_STOP_B4)

/* Macros for clock stop related bits in TA4 byte of ATR */
#define UIM_TA4_CLOCK_STOP_NOT_SUPPORTED 0x00
#define UIM_TA4_CLOCK_STOP_LOW           0x01
#define UIM_TA4_CLOCK_STOP_HIGH          0x02
#define UIM_TA4_CLOCK_STOP_NO_PREF       0x03
#define UIM_TA4_CLOCK_STOP_FIELD_SHFT    0x06

/* Define how the UIM clock is to be controlled */
typedef enum {
  UIM_CLOCK_STOP_NOT_ALLOWED, /* Stopping the UIM clock is not allowed */
  UIM_CLOCK_STOP_HIGH,        /* The UIM clock can be stopped high */
  UIM_CLOCK_STOP_LOW          /* The UIM clock can be stopped low */
} uim_clock_stop_cntrl_type;

/* This is the ICCID EF file ID */
#define UIM_ICCID_EF_ID          0x2FE2
#define UIM_ICCID_OFFSET         0
#define UIM_ICCID_SIZE           10

#define UIM_MINIMUM_TP_LENGTH    0x09

#define UIM_FCP_TEMPLATE_TAG       0x62
#define UIM_FILE_DESCRIPTOR_TAG    0x82
#define UIM_FILE_ID_TAG            0x83
#define UIM_DF_NAME_TAG            0x84
#define UIM_PROPRIETARY_TAG        0xA5
#define UIM_SUPPORTED_CMD_TAG      0x87
#define UIM_CHARACTERISTICS_TAG    0x80
#define UIM_SECURITY_ATTR_TAG1     0x86
#define UIM_SECURITY_ATTR_TAG2     0x8B
#define UIM_SECURITY_ATTR_TAG3     0x8C
#define UIM_SECURITY_ATTR_TAG4     0xAB
#define UIM_LIFE_CYCLE_STATUS_TAG  0x8A
#define UIM_FILE_SIZE_TAG          0x80
#define UIM_TOTAL_FILE_SIZE_TAG    0x81
#define UIM_SFI_TAG                0x88

#define UIM_UICC_TRANSPARENT_EF    0x01
#define UIM_UICC_LINEAR_EF         0x02
#define UIM_UICC_CYCLIC_EF         0x06
#define UIM_UICC_EF_STRUCT_MASK    0x07
#define UIM_TERMINAL_CAP_BYTE_MASK 0x01

#define UIM_SFI_NOT_USED_BR_MASK 0x80
#define UIM_SFI_NOT_USED_RR_MASK 0xF8

#define UIM_SFI_BITS_B1_TO_B5_MASK   0x1F
#define UIM_SFI_USED_BITS_B6_TO_B8   0x80

#define UIM_SFI_BITS_B4_TO_B8_MASK   0xF8
#define UIM_SHIFT_SFI_BITS_TO_B4_TO_B8 0x03

#define UIM_PIN1_APP1_VALUE     0x01
#define UIM_PIN1_APP2_VALUE     0x02
#define UIM_PIN1_APP3_VALUE     0x03
#define UIM_PIN1_APP4_VALUE     0x04
#define UIM_PIN1_APP5_VALUE     0x05
#define UIM_PIN1_APP6_VALUE     0x06
#define UIM_PIN1_APP7_VALUE     0x07
#define UIM_PIN1_APP8_VALUE     0x08

#define UIM_PIN2_APP1_VALUE     0x81
#define UIM_PIN2_APP2_VALUE     0x82
#define UIM_PIN2_APP3_VALUE     0x83
#define UIM_PIN2_APP4_VALUE     0x84
#define UIM_PIN2_APP5_VALUE     0x85
#define UIM_PIN2_APP6_VALUE     0x86
#define UIM_PIN2_APP7_VALUE     0x87
#define UIM_PIN2_APP8_VALUE     0x88
#define UIM_UNIVERSAL_PIN_VALUE 0x11

#define UIM_SW1_SECURITY        0x98

#define UIM_USB_SUPPORTED       0xC0
#define UIM_EUICC_SUPPORTED     0x82


/*

Refeerence from ETSI TS 102 221 and 102 261

SWP is the interface between the UICC and the CLF

The UICC indicates support of SWP interface in the 
Global Interface bytes of the ATR as defined in TS 102 221

Table 6.7: Coding of the first TBi (i > 2) after T = 15 of the ATR

===============              ========
b8...........b1              Meaning
===============              ========

0 0 0 0 0 0 0 0         No additional global interface parameters supported

1 - - 1 - - - -         Low Impedance drivers and protocol available on the I/O line

1 1 - - - - - -         Inter-Chip USB UICC-Terminal interface supported as defined

1 - 1 - - - - -         UICC-CLF interface supported as defined in TS 102 613 [19]


*/

/* Does UICC support swp ? */
#define UIM_UICC_CLF_INTERFACE_SUPPORTED       0xA0


typedef enum {
  UIM_POWER_UP_ST,                     /* Power Up state */
  UIM_RESET_ST,                        /* Reset State */
  UIM_DELAY_AFTER_ATR_ST,              /* Delay after receiving the ATR */
  UIM_PPS_ST,                          /* PPS State */
  UIM_SEND_STATUS_ST,                  /* Send Status to determine ICC or UICC */
  UIM_UPDATE_OP_PARAMS_ST,             /* Update Operational Parameters State */
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,                      /* Send IFS negotiation for IFSD */
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_CHECK_CHARACTERISTICS_ST,        /* Check clock stop preferences */
  UIM_TERMINAL_CAPABILITY_ST,          /* Send the terminal capabilities */
  UIM_CHECK_FOR_CDMA_DF,               /* Select CDMA DF to determine if R-UIM
                                          card */
  UIM_READ_ICCID_ST,                   /* Select ICCID EF */
  UIM_SELECT_ST,                       /* Select State */
  UIM_SELECT_FAILED_ST,                /* Cleanup for failed selects */
  UIM_STREAM_SELECT_ST,                /* Handling for stream apdu SELECT cmds */
  UIM_STATUS_ST,                       /* Status State */
  UIM_READ_BIN_MEM_ST,                 /* Read binary with uim allocated memory */
  UIM_UPDATE_BIN_MEM_ST,               /* Update binary not writing to cmd_ptr */
  UIM_READ_REC_MEM_ST,                 /* Read record with uim allocated memory */
  UIM_UPDATE_REC_MEM_ST,               /* Update binary not writing to cmd_ptr */
  UIM_INC_MEM_ST,                      /* INC with uim allocated memory */
  UIM_SEEK_ST,                         /* Seek State */
  UIM_VERIFY_CHV1_ST,                  /* Verify CHV 1 State */
  UIM_VERIFY_CHV2_ST,                  /* Verify CHV 2 State */
  UIM_UNBLOCK_CHV1_ST,                 /* Unblock CHV 1 State */
  UIM_UNBLOCK_CHV2_ST,                 /* Unblock CHV 2 State */
  UIM_CHANGE_CHV1_ST,                  /* Change CHV 1 State */
  UIM_CHANGE_CHV2_ST,                  /* Change CHV 2 State */
  UIM_ENABLE_CHV1_ST,                  /* Enable CHV 1 State */
  UIM_ENABLE_CHV2_ST,                  /* Enable CHV 2 State */
  UIM_DISABLE_CHV1_ST,                 /* Disable CHV 1 State */
  UIM_DISABLE_CHV2_ST,                 /* Disable CHV 2 State */
  UIM_INVALIDATE_ST,                   /* Invalidate State */
  UIM_REHABILITATE_ST,                 /* Rehab state */
  UIM_TERMINAL_PROFILE_ST,             /* Term profile State */
  UIM_ENVELOPE_ST,                     /* Envelope State */
  UIM_FETCH_ST,                        /* Fetch State */
  UIM_TERMINAL_RESPONSE_ST,            /* Term response State */
  UIM_POLLING_INTERVAL_ST,             /* Polling state */
  UIM_STREAM_APDU_ST,                  /* Stream APDU state */
  UIM_POWER_DOWN_ST,                   /* Power Down the INTERFACE */
  UIM_RECOVERY_COMPLETE_ST,            /* Recovery completed by GSDI */
  UIM_USB_POWER_OFF_ST,                /* Send ICCD POWER OFF to USB */
  UIM_USB_POWER_ON_ST,                 /* Send ICCD POWER ON to USB */
  UIM_USB_GET_ATR_ST,                  /* Get ATR data block from USB */
  UIM_RECOVERY_PERFORM_TP_ST,               /* Send Terminal Profile during recovery */
  UIM_RECOVERY_FLUSH_MANAGE_CHANNEL_ST,     /* Flush Manage channel for ICC during recovery */
  UIM_RECOVERY_MANAGE_CHANNEL_FOR_UICC_ST,  /* Send Manage channel for UICC during recovery */
  UIM_RECOVERY_CLOSE_CHANNEL_FOR_UICC_ST,   /* Send Manage channel to close channels during recovery */
  UIM_RECOVERY_CHECK_FOR_SELECT_AID_ST,     /* Check if select AID is required and set corresponding path */
  UIM_RECOVERY_VERIFY_CHV1_ST,              /* Pin1 verification during recovery */
  UIM_RECOVERY_VERIFY_CHV2_ST,              /* Pin2 verification during recovery */
  UIM_RECOVERY_SEND_STATUS_COMMAND_ST,       /* Send status indicating completion of app initalisation*/
  UIM_RECOVERY_VERIFY_UPIN_FOR_UICC_ST ,    /* UPIN verification during recovery */
  UIM_RECOVERY_CHECK_REHAB_REQD_ST,         /* Check if rehabilitation required during recovery */
  UIM_RECOVERY_REHAB_ST,                    /* Rehabilitate the file during recovery */
  UIM_RECOVERY_CHECK_EF_TST_UPDATE_REQD_ST, /* check if EF_TST update is required*/
  UIM_RECOVERY_SELECT_EF_TST_ST,            /* select EF-TST file*/
  UIM_RECOVERY_WRITE_EF_TST_ST,             /* update EF-tst during recovery*/
  UIM_GET_RESPONSE_ST,                      /* Send Get Response to card for stream APDUs */
  UIM_SILENT_RECOVER_UICC_ST,               /* SILENT RECOVERY is meant to recover SWP interface for NCF */
  UIM_MANAGE_CHANNEL_ST,                     /* Manage Channel */
  UIM_SELECT_ICCID_ST,                      /* Select the ICCID during power-up */
  UIM_DONE_ST,                              /* Done State */
/* Debug states - ONLY USED FOR TESTING */
  UIM_SIMULATE_NULL_ST                     /* SIM BUSY simulation state */
} uim_generic_state_type;


typedef struct {
  boolean         use_chv;              /* indicate if CHV should be used */
  byte            chv[UIM_MAX_CHV_DIGITS];      /* CHV digits */
} uim_chv_struct_type;

/* Type to store the terminal profile data */
typedef struct {
  byte   num_bytes;         /* Number of bytes in profile */
  byte   data[UIM_TERMINAL_PROFILE_DATA_LENGTH]; /* Terminal Profile */
}uim_int_terminal_profile_data_type;

/* Type to store the protocols supported by the card */
typedef struct {
  boolean t_0;  /* T = 0 support */
#if defined( FEATURE_UIM_T_1_SUPPORT )
  boolean t_1;  /* T = 1 support */
#endif /* FEATURE_UIM_T_1_SUPPORT */
} uim_prot_sup_by_card_type;


/*===========================================================================

FUNCTION UIM_SEND_GENERIC_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested. We pass cmd_ptr explicitly here since we may be sending err
  report for am unprocessed command that was present in command queue while
  processing hotswap card removal or card error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_generic_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_COMMAND

DESCRIPTION
  This procedure processes a generic command sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM Generic state variable amongst others.

===========================================================================*/
boolean uim_process_generic_command
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_generic_state.

===========================================================================*/

uim_cmd_status_type uim_generic_command_response
(
 uim_rsp_buf_type         *rsp_ptr,
 uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND

DESCRIPTION
  This procedure is the main state machine for Generic UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

void uim_generic_command
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_SET_CMD_STATE

DESCRIPTION
  This procedure sets the UIM state and is called during custom command
  processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the state of ruim_state variable in auth task.

===========================================================================*/
void uim_set_cmd_state
(
  uim_generic_state_type state
);

/*========================================================================

FUNCTION UIM_PROCESSING_ENVELOPE_APDU

DESCRIPTION
  This function returns whether UIM is processing an envelope command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : UIM processing EVVELOPE COMMAND
  FALSE: UIM not processing ENVELOPE COMMAND

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_processing_envelope_apdu
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================
FUNCTION UIM_IS_TEST_ICCID

DESCRIPTION
  This function checks if the current ICCID is GCF ICCID or not.

RETURN VALUE
  Boolean
    TRUE : If current iccid is same as iccid used in GCF test cases
    FALSE: If current iccid is NOT same as iccid used in GCF test cases

DEPENDENCIES
  None
===========================================================================*/
boolean uim_is_test_iccid
(
  uim_instance_global_type *uim_ptr
);
#endif /* UIMGEN_H */
