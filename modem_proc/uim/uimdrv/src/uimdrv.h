#ifndef UIMDRV_H
#define UIMDRV_H
/*===========================================================================

                 U I M   D E V I C E   L A Y E R   H E A D E R

DESCRIPTION
   This is the header file for the interface layer of the UIM device driver.
   Therefore, this header file provides the complete interface to the low level
   UIM device driver.

   Copyright (c) 2001 - 2015 by QUALCOMM Technologies, Inc. All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimdrv.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/15   ks      Add end state to uim_atr_pps_rx_mode_type
01/19/15   kr      Fixed Compilation warnings
11/07/14   sam     Added new RX state- UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS
11/10/14   akv     Support for user to customize LDO settings
10/27/14   ll      KW
09/18/14   akv     UIMDRV UT enhancements
06/11/14   ll      Add interruptible time delay and return value for update params
05/23/14   ks      Enabling software command response timer
04/15/14   ks      Reverting the changes of CR556030 that introduced sw wwr
04/10/14   ll      Remove deprecated pmapp_uicc header file
04/06/14   ks      Introducing software wwt running parallel to h/w wwt
04/01/14   av      NVRUIM headers cleanup
03/27/14   lxu     USB UICC modem implement
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
01/29/14   akv     RX ISR refactoring changes
12/12/13   ks      BTSAP in client mode, intial implementation
10/24/13   akv     Removed unnecessary feature flags
10/08/13   nmb     HW header file cleanup
09/26/13   ak      Remove unnecessary feature flags
09/20/13   ak      Hotswap debug message cleanup
09/19/13   akv     Reference CLK changes to support 4.8MHz
09/16/13   yk      Removed the code for old targets and for old features
08/22/13   ak      Remove un-needed #defines
08/13/31   js      UIMDRV Memory De-Couple changes
08/06/13   ak      Bolt RUMI code
07/12/13   nmb     Global Reorganization
06/24/13   js      TSTS changes
06/14/13   js      Fixed compilation error
05/31/13   spo     Updated number of driver slots from 2 to 3
03/31/13   tl      Set data size to match spec conformance
02/08/13   js      Updated UIM USB code for parallel processing
12/26/12   akv     Removal of T_MDM9x25 featurization
12/05/12   js      UIM Parallel Processing changes
11/01/12   akv     Core cpu vdd voting to avoid Q6 going into power collapse
10/26/12   akv     HW Enumeration changes (without features)
10/26/12   akv     HW Enumeration changes
09/19/12   rm      Changes for elan
08/27/12   akv     Set P3 in Get Resp to 0 only when SW1 warnings are received
                   for streaming APDU
08/26/12   ssr     Moving Hotswap timer_def from timer expire cb to hotswap init
08/09/12   js/rm   Fixes for UIM controller hotswap and hardware WWT features
05/15/12   rm      Fixes for compilation errors
05/01/12   av      Replaced FEATURE_UIM_SUPPORT_HOTSWAP with an nvitem
04/20/12   js      8930 bring up changes
03/30/12   av      Replace feature flags with NVITEM
03/20/12   rm      Changes for WWT and hotswap support
03/15/12   rm      Changes for DIME
03/02/12   js      Vote for CPU latency node
02/18/12   av      Cleaned up remaining ALWAYS OFF/ON feature flags
02/06/12   av      Remove always ON 1x features
02/03/12   js      Reverting UART DM fix
01/20/12   js      Changes needed for 8930 UIM bring up
01/16/12   nmb     Removal of ALWAYS ON FEATURE_AUTH
12/23/11   shr     Removed featurization for ALWAYS ON features
11/30/11   js      Register and vote for CFPB clock
11/16/11   rm      Fix to correct the work waiting time calculation based on
                   UIM clock frequency
11/3/11    ms      Fix to use small stale timeout value for default baud-rate
10/18/11   av      Removed features that are always OFF
09/09/11   ssr     Fix to allow the reset command when switch is un-pressed where
                   client want to disable the hotswap interrupt at runtime,
                   Added new API to return the real time card status,
                   Added new API to disable the hotswap interrupt
09/01/11   ak      GPIO hotswap support for 9x15
08/29/11   ssr     Fixed compilation warnings
08/24/11   ssr     Fixed NV Item support for hotswap configuration
08/19/11   ak      9x15 bring-up changes/DalClock API support
07/26/11   rm      Fix to resolve a linker error for MOB
07/18/11   rm      Catch-up with latest on Hot-swap
06/14/11   ssr     Optimize UIM notification code for card removed
06/06/11   ssr     Update the code to do the card insertion based on the
                   NV_UIM_FIRST_INST_CLASS_I NV item,
05/24/11   yb      Fix to use clock regime APIs instead of register HWIO for
                   clock register
04/25/11   yb      Adding support for hotswap
11/30/10   yt      Including private UIMDRV header
10/26/10   ps      Fixed uim_rx_state variable when accessed between ISR and UIM task
10/27/10   rm      Fix for a bus error due to UART register access
08/17/10   ps      Merge UIM server DSDS changes
07/02/10   yt      Fixed compilation errors in VIRTUIM
02/18/10   vs      Merged CSIM functionality
09/30/09   mib     Added PACKED_POST for Q6 compilation
09/25/09   yk      Add support for 7x30
09/11/09   rm      Handle TC1 byte in ATR for extra guard time
08/13/09   nb      Moved the declaration for uim_dev_init to uim.h
08/10/09   rm      Error handling for T=1 protocol
07/20/09   mib     Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
06/06/09   sk      Fix for issue with change pin functionality
05/29/09   sg      Fix to include virtuim_qtf.h for test framework
05/13/09   ps      Fix QTF Compilation
05/12/09   ps      Fix Lint errors
04/08/09   nb      Fix for Multiple selects on MF
04/03/09   yk      Bring SCMM drivers
03/23/09   sun     Added support for Virtuim in QTF
03/19/09   rm      Move platform specific cust files inclusions to custuim.h
03/02/09   sun     Added private header file
01/28/09   nb      Changes to support virtuim in QTF
01/06/09   yk      Added MSM7X27 specific cust file inclusion
12/04/08   nb      Fixed QTF compilation problem
12/03/08   nb      Fixed Compilation Error
11/19/08   vs      Allowing uim_sleep_okts_handle to be accessed as a global
11/13/08   rm      Use timer callback for command response timer to handle the
                   work waiting timer expiry immediately in the timer context.
10/27/08   as/nk   Included 6270 and 8x50 hardware specific file
10/07/08   vs      Fix to power up UIM LDO after card power down, vote TCXO off
                   permanently when IC-USB interface is selected
10/03/08   jk      Fixed warning
09/22/08   rm      Fix for parity errors in UART_DM
09/19/08   rm      Work waiting time fix for UART_DM
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
08/22/08   js      Removed featurization around MFLO APDU status types
08/08/08   vs      Fixed compile errors related to featurization
08/01/08   rm      Added a method to return the size of RX data
                   waiting in RX FIFO and packing buffer in the UART DM.
07/09/08   rm      Fix compilation error
06/18/08   jk      Added QSC11X0 specific cust file inclusion
06/11/08   jar     Added platform specific custuim inclusions
                   for 7225.
05/28/08   rm      TA1=0x97 feature support.
05/09/08   rm      Fix to not call PMIC MPP APIs in case of direct
                   connect to SIM or single chip solution
05/02/08   rm      T=1 protocol support for LCU.
04/22/08   rm      Remove the un-necessary UIM_RX_HANDLE_CASE2_BYTES state
04/30/08   vs      Fixed Klocwork error
04/16/08   rm      Fix for 6246 to not call PMIC MPP APIs in case of direct
                   connect to SIM or single chip solution
04/08/08   sun     Added support for OMA Bcast
10/03/08   rm      Fix for 6290 to not call PMIC MPP APIs in case of direct
                   connect to SIM or single chip solution
03/13/08   rm/jar  LCU UIM bringup in UART word mode with DM disabled
03/05/08   jar     Allow PMIC to put VREG RUIM into LPM MODE when TXCO
                   is off under feature:  FEATURE_UIM_ENABLE_VREG_UIM_LPM
10/09/07   vs/nk   Added EUIMID feature support
09/10/07   tml     Added MFLO support
10/08/07   nf      Added GPLT support
07/02/07   sun     Added support for ODD INS Class
04/17/07   sp      Featurize for LTK
09/28/06   pv      Change uim_response_byte_delay to unsigned.
07/31/06   pv      Add definition for GENERATE ASYMMETRIC KEY PAIR WIM
                   instruction to use it to avoid doine a get response
                   when 6200 is received.
06/12/06   pv      Add UIM_RX_UNKNOWN_PROC_BYTE_RCVD state
04/11/06   pv      Moved UIM_READ_STATUS macro to uimdrv.c and export
                   uim_read_status function to do the same operation.
04/03/06   pv      Lint Fixes
01/18/06   tml     added support for status word 6999
09/06/05   jk      Changes to support only allowing reading UART Status while
                   there is still power to UART
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
02/09/05   jk      Added parenthesis to UIM_READ_STATUS Macro
                   so that it evaluates properly in if condition setting.
01/12/05   jk      Added code to process Response properly for Global Platform/WIM
                   cards
01/12/05   pv      Externalized function definitions for Driver Timestamping
12/28/04   jk      Changed Streaming APDU size from 260 to 261.
12/16/04   jk      Fixed CR # 54476 - Streaming APDU size up to 260 bytes
12/08/04   pv      Raven merged to main-line
12/06/04   pv      Externalised variables required for logging timeout info
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
09/01/04   ck      Added support for CSIM.
08/10/04   pv      Brought in definitions for rx_state and tx_state
                   Changed the response byte delayt timeout to 150microsecs
07/18/04   pv      Brought in some definitions from driver source file
05/11/04   ts      Fixed the uim command buffer definition for accesses
                   greater than 256 bytes.
04/28/04   ts      Added a constant to support for > 256 byte writes to
                   transparent files.
01/12/04   ck      Added UIM_LOGICAL_CHANNEL_BIT_MASK
12/19/03   jk      Added interface for R-UIM 3GPD Services.
10/31/03   ts      Added constants to support accesses greater than the maximum
                   transaction size.
10/23/03   ck      Merged in changes to support 18etus between reception of a
10/10/03   ts      Removed featurization around #defs for status words.
                   Added definition for minimum guardtime for T=1.
09/02/03   ck      Removed the featurization around enum type for uim_slot_type.
08/21/03   ck      Merged in support for Saber and TCXO/5 for USIM clock.
08/14/03   ck      Merged in support for dual slots.
07/22/03   ts      Added a table that indicates the maximum BRAF for the CRCF.
06/03/03   ck      Added status type UIM_CONVENTION_ERROR.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
02/26/03   jk      Added UTIL and WIM support
01/20/03   ts      Added run-time support of 13MHz GSM clock.
12/20/02   ts      Fixed Lint issues
                   Added support for using 13MHz GSM clock.
11/26/02   ts      Determine tcxo frequency used and max UIM frequency
                   supported based on tcxo frequency.
11/21/02   ts      Added constants for a default PPS exchange.
11/13/02   djm     WCDMA PLT build w/o FEATURE_UIM does not build because
                   UIM_MAX_CHARS is referred to by other files when FEATURE_UIM
                   is not defined.  pull this section of defines in when
                   FEATURE_WCDMA_PLT is def'd
10/24/02   ts      Added reference to clock rate conversion factor table.
07/30/02   gr      Merged timetest fix that was featurized (lost in the 1.10.1.0)
05/21/02   ts      Added support for 256 bytes transfers from the UIM card.
03/18/02   ts      Moved the MSMU_OUT and MSMUW_OUT from this file to uimdrv.c
                   to avoid multi-definition problems with these macros.
01/23/02   ts      Added definitions for setting the UIM clock signal to a high
                   state and a low state through uim_update_op_params.
12/13/01   ts      Aligned some comments
12/04/01   ts      Added status word definitions.
11/13/01   ts      Added support for UIM Application Toolkit (UTK).
10/10/01   ts      Removed the SIO_NOP from the MSMU_OUT macro.
07/27/01   ts      Added test port debug macros and constants so that any part
                   of UIM server can use it.
06/26/01   ts      Changed the uim_clk_freq_type to indicate tcxo/4 and tcxo/8.
05/25/01   ck      Featurised the intruction codes for Pre-Addendum 1 codes
                   that clashed with ISO 7816 instrn codes.
05/24/01   ck      Added support for USIM.
01/15/01   ck      Created file from R-UIM driver version 1.3.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "qw.h"
#include "uim.h"
#include "uim_p.h"
#include "uim_v.h"
#include "nv.h"
#include "npa.h"
#include "DDIClock.h"
#include "uimdrv_clk.h"
#include "pm.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Set TX FIFO to 128 bytes for Instruction case 2 */
#define UIM_CASE_2_INS_BADR_VAL 0x20

/* use an extra guard time of 1 etu over the default guard time of 2 etus */
#define UIM_UART_DM_EXTRA_GUARD_TIME 1

/* Work-waiting time correction for 4 byte duration is needed in the case of
 * UART_DM as there could be 4 bytes in the unpacking buffer yet to be transmitted
 * to the card.
 *
 * Therefore, the work-waiting time correction for T=0 protocol is:
 *      (4 byte time) + (4* guard time between bytes )
 *      = (4 * 10) + (4 * 3) = 52 ETUS
 *
 * and, the work-waiting time correction for T=1 protocol is:
 *      (4 byte time) + (4* guard time between bytes )
 *      = (4 * 10) + (4 * 2) = 48 ETUS
 *
 *
 * Please note that this delay is not applicable to legacy UARTs.
 */
#define UIM_FOUR_BYTE_TIME_DELAY  ((UIM_T_0_PROTOCOL == uim_ptr->state.operating_protocol) ? 52:48)

/* Second to milliseconds macro */
#define UIM_1SEC                 1000

/* The work-waiting time correction in milliseconds */
#define UIM_UART_DM_WAITING_TIME_CORRECTION                                      \
                         ((uim_ptr->id < UIM_MAX_INSTANCES &&                    \
                           uim_ptr->state.DI_current < UIM_BRAF_SIZE &&          \
                           uim_ptr->state.FI_current < UIM_CRCF_SIZE)?           \
    (rex_timer_cnt_type) ( UIM_FOUR_BYTE_TIME_DELAY *                            \
                           UIM_1SEC * crcf_values[uim_ptr->state.FI_current]) /  \
                          (braf_values[uim_ptr->state.DI_current] *              \
                           uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq):          \
                          (rex_timer_cnt_type)0)

#define UIM_MICROSECONDS_PER_TWO_MILLISECONDS 2000

/* Speed enhancement logic for LCU doesn't aim at reducing the etu (elementary
   time unit) value as it is depends frequency applied to card and the baud-rate
   negotiated with the card.

   Speed enhancement logic for LCU is based on programming a proper stale
   timeout value and rx water mark levels. RX water mark level is to receive
   and process the data as fast as possible. Stale timeout is to determine the
   end of data transfer. For a high stale timeout value, we wait un-necessarily
   for that much *extra* time if our expected size of data has already been
   received but just lying in the RX FIFO and packing buffer of the UART.
   So, to enhance the speed, we need to program lesser values of stale timeout
   and RX water mark levels. We are only trying to not let the race conditions
   or timing issues happen by not programming very low values of stale timeout
   and RX water mark levels. The UART HW designers recommended not to use very
   low RX STALE timeout values. The below stale and rx water mark levels were
   determined only by experimentation. */


/* Default RX water mark level for LCU set to 36 words just
   enough for only one RXLEV interrupt to happen */
#define MSMU_DEF_RFWR_VAL 36

/* Macro for number of ack bytes */
#define NUM_ACK_BYTES 1

/* Macro for number of status bytes */
#define NUM_STATUS_BYTES 2

/* Macro for maximum RX FIFO size */
#define MAX_RXFIFO_SIZE 256

/* The DMRX register value should always be a multiple of 16.
   Since, the maximum size of SIM response is 256 + 1 (ACK) + 2 (status bytes),
   the DMRX register value is set to 272 */
#define MSMU_DEF_RX_INIT_VAL 272

/*
   These are UIM card constants
 */
#define INVERSE_CONVENTION_TS_DETECT 0x03
#define DIRECT_CONVENTION_TS_DETECT  0x23
#define MAX_NUM_ATR_BYTES 33
#define MAX_NUM_PPS_BYTES 6

/* These define the result of XORing the Ack byte with the INS byte          */
#define PAUSE_REMAINING_BYTES 0x00    /* Send the remaining bytes of the data*/
#define PROG_REMAINING_BYTES  0x01    /* Send the remaining bytes of the data*/
#define PAUSE_NEXT_BYTE       0xFF    /* Send only the next byte and wait for
                                         Ack                                 */
#define PROG_NEXT_BYTE        0xFE    /* Send only the next byte and wait for
                                         Ack                                 */

/* These constants define the receive watermark levels. */
#define UIM_RX_WATER_MARK_LEVEL_ZERO      0
#define UIM_RX_WATER_MARK_LEVEL_FIVE      5

/* Maximum time we want uim module to do a clk_busy_wait */
#define UIM_MAX_CLK_BUSY_WAIT 1000

#define UIM_UART_DM_INTERUPT_MAX_WAIT_CNT        0x03

/* Time it takes for 1 byte to get to the UART fifo from the start bit */
#define UIM_BYTE_TIME ( (int32)( 372 * 12 ))

/* Macro for ISO7816 defined maximum ATR wait UIM clock cycles */
#define UIM_ISO7816_MAX_ATRWAIT_UIMCYCLES (int32)(40000)

/* Frequency of the sleep clock from which we get the time stmaps */
#define SLEEP_CLK_FREQ 32768

/* Default delay based upon a 3.25MHz UIM clock and CRAF of 512 and BRAF of 8
   The baud rate is 50.781Kbps.  */
/* The default delay adds delay for 1.5 bits.  This is needed because the MSM
   interrupts the ARM half way within the first stop bit.  There are 1.5 bits
   left to wait before a response */
#define UIM_RESPONSE_BYTE_DELAY_DEFAULT 150
/* This delay works on cards that expect 6 bits delay after the procedure
   byte.  This is 7.5 bits after the procedure byte */

#define UIM_RESPONSE_BYTE_DELAY_MORE 150

#define UIM_RUIM_UIMID_MAX_SIZE    7

/* Max num of slots*/
#define UIM_NUM_DRV_SLOTS          3

/* Define the slots that are supported for this build.  It could be one of
   two slots or both */
/* IMPORTANT: The enumeration and typedef for UIM slots should not be combined
   as this introduces Klocwork errors*/

#define UIM_DEFAULT_CHANNEL UIM_CHANNEL0

/* The enum type describes the voltage classes supported on the UIM interface.
   The list must start at the lowest voltage and progress top the highest voltage
   in order. */
typedef enum {
  UIM_VOLTAGE_CLASS_INVALID,
  UIM_VOLTAGE_CLASS_C,  /* Use 1.8 volts */
  UIM_VOLTAGE_CLASS_B,  /* Use 3 volts */
} uim_voltage_class_type;

/*
  This is a conversion table that returns the number of bytes that follow the
  format byte.  Only the "presents of interface bytes" field is used in the
  index.  The table contains the number of bits set in the index.
 */
extern const byte uim_y_format_table[16];

/* This type defines the transport protocol used by the UIM server */
typedef enum {
  UIM_T_0_PROTOCOL = 0x00,
  UIM_T_1_PROTOCOL = 0x01,
  UIM_NO_PROTOCOL  = 0x02
} uim_transport_protocol_type;

/* These define the UIM specific interface bytes */

#define UIM_K_FIELD          0x0F      /* In T0; Number of historical bytes */
#define UIM_Y_FIELD_SHIFT       4      /* Number of bits to shift to get the
                                           Y field in the lsb                */
#define UIM_TD_PRESENT       0x80      /* TD byte is present in following
                                           bytes                             */
#define UIM_T_FIELD          0x0F      /* In TD; transmission protocol      */

#define UIM_MINIMUM_T_1_GUARDTIME 1    /* This is the minimum guardtime for
                                          the T=1 protocol */

/* Default number of bits used for guardtime */
#define UIM_DEFLT_GUARDTIME_BITS   (2 + (1*uim_ptr->setting.features_status_list.handle_tc1_byte_for_extra_guard_time))

  /* This is the minimum guardtime for the T=0 protocol */
  #define UIM_MINIMUM_T_0_GUARDTIME UIM_DEFLT_GUARDTIME_BITS

  /* This macro returns the minimum guard time for T=0 / T=1 protocols */
  #define UIM_MINIMUM_GUARD_TIME                                   \
         ((uim_ptr->state.operating_protocol == UIM_T_1_PROTOCOL) ?           \
         (UIM_MINIMUM_T_1_GUARDTIME):(UIM_MINIMUM_T_0_GUARDTIME))

  /* The max guard time supported by UART is 254etu */
  #define UIM_MAX_POSSIBLE_GUARD_TIME  254

typedef enum {
  UIM_DONE,                      /* Command completed without error.        */
  UIM_DATA_DOWNLOAD_ERROR,       /* An SMS-PP download error occurred       */
  UIM_TOOLKIT_BUSY,              /* The R-UIM toolkit is unable to perform the
                                    operation because it is currently busy
                                    working on another command              */
  UIM_SUCCESS_AFTER_MULTI_TRIES, /* The command is successful; but after
                                    using the internal update retry routine.*/
  UIM_MEMORY_PROBLEM,            /* Memory problem.                         */
  UIM_NO_EF_SELECTED,            /* No EF was selected                      */
  UIM_OUT_OF_RANGE,              /* Indicates an invalid address            */
  UIM_NOT_FOUND,                 /* Indicates file ID or pattern is not
                                    found.                                  */
  UIM_INCONSISTENT_WITH_COMMAND, /* Indicates the file is inconsistent with
                                    the command.                            */
  UIM_NO_CHV_INITIALIZED,        /* No CHV was initialized to allow this
                                    command                                 */
  UIM_ACCESS_NOT_FULFILLED,      /* Indicates the access condition for the
                                    command has not been fulfilled.
                                    Unsuccessful CHV verification, at least
                                    one attempt left
                                    Unsuccessful UNBLOCK CHV verification,
                                    at least one attempt left authentication
                                    failed                                  */
  UIM_CONTRADICTION_W_CHV_STATUS,/* Command is in contradiction with the CHV
                                    status                                  */
  UIM_CONTRADICTION_W_INVALID_STATUS,/* Command is in contradiction with the
                                        invalidation status                 */
  UIM_SSD_UPDATE_SEQ_PROBLEM,    /* Problem in the sequence of the command
                                    with respect to base station challange  */
  UIM_CHV_BLOCKED,               /* Unsuccessful CHV verification, no attempt
                                    left
                                    Unsuccessful UNBLOCK CHV verification, no
                                    attempt left
                                    CHV blocked
                                    UNBLOCK CHV blocked                     */
  UIM_MAX_VALUE_REACHED,         /* Indicates the increase operation cannot be
                                    performed because the maximum value has
                                    been reached.                           */
  UIM_INCORRECT_PARAM_P3,        /* P3 is an incorrect parameter            */
  UIM_INCORRECT_PARAM_P1_2,      /* P1 or P2 is an incorrect parameter      */
  UIM_UNKNOWN_INS,               /* Unknown instruction code given in the
                                    command (APDU)                          */
  UIM_WRONG_CLASS,               /* Wrong instruction class given in the
                                    command                                 */
  UIM_PROBLEM,                   /* Technical problem with no diagnostic
                                    given                                   */
  UIM_CMD_ERR_NO_INFO,           /* No information given                    */
  UIM_INVALID_NEW_PIN,           /* In case of Invalid Pin Entered          */

  /* Status byte coding - warnings */
  UIM_NO_INFO_AND_NV_UNCHANGED,  /* No information given, state of NV memory
                                    unchanged                               */
  UIM_PART_DATA_MAY_BE_CORRUPTED,/* Part of returned data may be corrupted  */
  UIM_EOF_BEFORE_READING_LE_BYTES, /* End of File/record reached before
                                      reading Le bytes                      */
  UIM_SELECTED_FILE_INVALIDATED, /* Selected file invalidated               */
  UIM_INCORRECT_FCI_FORMAT,      /* FCI not formatted according to sub clause
                                    11.1.1.3.1                              */
  UIM_NO_INFO_AND_NV_CHANGED,    /* No information given, state of NV memory
                                    changed                                 */
  UIM_FILE_FILLED_UP_BY_LAST_WRITE,/* File filled up by last write          */
  UIM_CMD_SUCCESS_WITH_X_RETRIES,  /* Command successful but after using an
                                    internal update retry routine X times.
                                    Verification failed, X retries remain   */
  UIM_MORE_DATA_AVAILABLE,         /* More Data Available */

 /* Execution Errors */
  UIM_EXEC_ERR_AND_NV_UNCHANGED, /* No information given, state of NV memory
                                    unchanged                               */
  UIM_EXEC_ERR_AND_NV_CHANGED,   /* No information given, state of NV memory
                                    changed                                 */
  UIM_EXEC_ERR_MEMORY_PROBLEM,   /* Memory Problem                          */

/* Functions in CLA not supported */
  UIM_CLA_ERR_NO_INFO,           /* No information given                    */
  UIM_LOGICAL_CHAN_NOT_SUP,      /* Logical Channel not supported           */
  UIM_SECURE_MESSAGING_NOT_SUP,  /* Secure Messaging not supported          */

/* Command not allowed */
  UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT,/* Command incompatible with file
                                           structure                        */
  UIM_SEC_STATUS_NOT_SATISFIED,  /* Security Status not satisfied           */
  UIM_AUTH_PIN_BLOCKED,          /* Authentication/PIN method blocked       */
  UIM_REF_DATA_INVALIDATED,      /* Referenced data invalidated             */
  UIM_USE_CONDNS_NOT_SATISFIED,  /* Conditions of used not satisified       */
  UIM_CMD_ERR_NO_EF_SELECTED,    /* Command not allowed                     */

/* Wrong parameters */
  UIM_INCORRECT_PARAM_IN_DATA_FIELD,/* Incorrect params in the data field   */
  UIM_FUNCTION_NOT_SUPPORTED,    /* Function not supported                  */
  UIM_FILE_NOT_FOUND,            /* File not found                          */
  UIM_RECORD_NOT_FOUND,          /* Record not found                        */
  UIM_LC_INCONSISITENT_WITH_TLV, /* Lc inconsistent with TLV structure      */
  UIM_INCORRECT_PARAMS_P1_P2,    /* Incorrect parameters P1-P2              */
  UIM_LC_INCONSISITENT_WITH_P1_P2,/* Lc inconsistent with P1-P2             */
  UIM_REF_DATA_NOT_FOUND,        /* Referenced data not found               */
  UIM_NOT_ENOUGH_MEMORY_SPACE,   /* Not enough memory space                 */

/* Application errors */
  UIM_APPLCN_SPECIFIC_AUTH_ERROR,/* Appln specific Authentication Error     */

/* Reference Problem */
  UIM_ALGORITHM_NOT_SUPPORTED,   /* Algorithm not supported */
  UIM_INVALID_KEY_CHECK_VALUE,   /* Invalid Key Check Value */
  UIM_SE_NOT_SET,

#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_T_1_BLOCK_RX_FAILURE,      /* T=1 block was not received properly     */
#endif /* FEATURE_UIM_T_1_SUPPORT */

/* Not an actual status word returned */
  UIM_PARITY,                     /* Parity Error                            */
  UIM_CONVENTION_ERROR,           /* Error in convention used by the driver  */
  UIM_REMOTE_STATUS,              /* Indicates status word is received from remote card */
  UIM_REMOTE_GET_RESPONSE_STATUS, /* Indicates GET_RESPONSE to be sent to remote card  */
  UIM_MAXIMUM_STATUS,
  UIM_MFLO_LOCKED_INIT_PARAM,         /* parameters already intialized */
  UIM_MFLO_APPLET_NOT_INIT,             /* parameters not initialized */
  UIM_MFLO_PUBLIC_KEY_OR_CERT_NOT_INIT, /* public key not initialized */
  UIM_MFLO_UCAST_AUTH_KEY_NOT_INIT,     /* unicast authentication key not initialized */
  UIM_MFLO_UCAST_DATA_BLOCK_NOT_INIT,   /* data blocks not recieved in order */
  UIM_MFLO_VERIFY_UCAST_FAILED          /* unicast signature did not match */

} uim_apdu_status_type;

typedef enum {

  UIM_WAITING_FOR_T0,    /* Expecting the T0 character
                             after skipping the num_bytes_to_skip */
  UIM_WAITING_FOR_TD,    /* Expecting a TD character
                             after skipping the num_bytes_to_skip */
  UIM_WAITING_FOR_TCK,   /* Expecting the TCK character
                             after skipping the num_bytes_to_skip */
  UIM_WAITING_FOR_END,    /* Expecting the last of the ATR with NO
                             TCK character expected.              */
  UIM_END                 /* All expected bytes of ATR/PPS received */
} uim_atr_pps_rx_mode_type;

/* These types defined the callback functions used by the UIM driver.
                                                                             */
typedef void (*uim_unsolict_resp_callback_type)( void );

typedef void (*uim_resp_callback_type)(uint32 uim_instance_id);

typedef enum {
  UIM_TRANSPARENT  = 0,            /* Transparent structure  */
  UIM_LINEAR_FIXED = 1,            /* Linear Fixed structure */
  UIM_CYCLIC       = 3,            /* Cyclic structure       */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  UIM_NO_FILE_STRUCT = 0xFF        /* Indicates that no file is selected
                                      and is used for internal processing
                                      only */
} uim_file_structure_type;

typedef enum {
  RFU = 0,               /* Reserved for Future Use */
  MF = 1,                /* Master File             */
  DF = 2,                /* Dedicated File          */
  EF = 4                 /* Elementary File         */
} uim_uim_file_type;

typedef PACKED struct PACKED_POST {
  byte           rfu1[2];         /* Reserved for Future Use (RFU) */
  byte           mem[2];          /* Amt of memory of selected directory which
                                     is not allocated to any of the DFs or EFs
                                     under the selected directory */
  byte           file_id[2];      /* File ID */
  uim_uim_file_type  file_type;   /* File Type */
  byte           rfu2[5];         /* RFU */
  byte           len_data;        /* Len of the following GSM Specific data */
} uim_sel_mf_df_rsp_type;

/* featurized it for LTK because LTK assigns 4 bytes for an enum
    where as ARM assigned 1 byte. UIM processes it as 1 byte, so need to
    change it to byte from uim enums.*/
typedef PACKED struct PACKED_POST {
  byte                  rfu[2];        /* RFU */
  byte                  file_size[2];  /* File Size */
  byte                  file_id[2];    /* File ID */
  uim_uim_file_type     file_type;     /* File Type */
  byte                  inc_allowed;   /* RFU/Inc Access for Cyclic File*/
  byte                  access[3];     /* Access Conditions */
  byte                  file_status;   /* File Status */
  byte                  len_data;      /* Length of following data */
  uim_file_structure_type file_struct; /* Structure of EF */
  byte                  rec_len;       /* Length of record */
} uim_sel_ef_rsp_type;

typedef PACKED struct PACKED_POST {
  byte usage_ind_conf;                 /* Usage Indicator Confirmation */
} uim_store_esn_rsp_type;

typedef PACKED struct PACKED_POST {
  byte result;                         /* Result of Operation */
} uim_commit_rsp_type;

typedef PACKED struct PACKED_POST {
  byte result;                         /* Result of Operation */
} uim_ms_key_rsp_type;

typedef PACKED struct PACKED_POST {
  byte result;                         /* Result of Operation */
  byte ms_result_len;                  /* Length of MS Result */
  byte ms_result[UIM_PARAM_P];         /* MS Result */
} uim_key_gen_rsp_type;

typedef PACKED struct PACKED_POST {
  byte result;                         /* Result of Operation */
} uim_serv_key_gen_rsp_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
} uim_validn_rsp_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                       /* Block id */
  byte block_length;                   /* Block length */
  byte result_code;                    /* Result Code */
  byte data[UIM_MAX_BYTE_SIZE - 3];       /* Data from the card */
} uim_cfg_rsp_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
} uim_download_rsp_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
  byte block_length;                   /* Block length */
  byte data[UIM_MAX_BYTE_SIZE - 3];       /* Data from the card */
} uim_sspr_cfg_rsp_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
  byte segment_offset[2];              /* Segment offset */
  byte segment_size;                   /* Segment size */
} uim_sspr_download_rsp_type;

typedef PACKED struct PACKED_POST {
  byte result;                         /* Result of Operation */
  byte nam_lock_ind;                   /* NAM Lock Indication */
  byte rand_otapa[4];                  /* Rand OTAPA */
} uim_otapa_rsp_type;

typedef PACKED struct PACKED_POST {
  byte sync_tag;
  byte data[UIM_MAX_CHARS];
} uim_aka_auth_rsp_type;

typedef union {
  uim_sel_mf_df_rsp_type mf_df;       /* Response to Select MF/DF */
  uim_sel_ef_rsp_type    ef;          /* Response to Select EF */
  byte                   data[UIM_MAX_CHARS]; /* Response Data */
  uim_store_esn_rsp_type store_esn;   /* Response to Store ESN */
  uim_commit_rsp_type    commit;      /* Response to Commit */
  uim_ms_key_rsp_type    ms_key;      /* Response to MS Key Request */
  uim_key_gen_rsp_type   key_gen;     /* Response to Key Gen Request */
  uim_serv_key_gen_rsp_type   serv_key_gen;/* Response to Service Key Gen Request */
  uim_validn_rsp_type    validate;    /* Response to Validation Request */
  uim_cfg_rsp_type       configuration;/* Response to CFG Rsp Type */
  uim_download_rsp_type  download;    /* Response to Download Request */
  uim_sspr_cfg_rsp_type  sspr_config; /* Response to SSPR CFG Rsp Type */
  uim_sspr_download_rsp_type sspr_download; /* Response to SSPR Download Req */
  uim_otapa_rsp_type     otapa;       /* Response to OTAPA Request */
  uim_aka_auth_rsp_type       aka_auth;   /* Response to AKA Auth Request */
} uim_rsp_type;

typedef struct {
  word                 cmd_rsp_size;  /* Command Response Size */
  uim_rsp_type         rsp;           /* Response Type */
  uim_apdu_status_type cmd_status;    /* Command Status */
  byte                 sw1;           /* Status Word 1 */
  byte                 sw2;           /* Status word 2 */
  uim_slot_type        slot;
} uim_rsp_buf_type;

typedef PACKED struct PACKED_POST {
  byte  high_byte;                    /* High byte of the File to be selected */
  byte  low_byte;                     /* Low byte of the File to be selected */
} uim_sel_mf_df_ef_req_type;

typedef PACKED struct PACKED_POST {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified*/
} uim_verify_chv_type;

typedef PACKED struct PACKED_POST {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* Unblock CHV */
  byte  new_chv[UIM_MAX_CHV_DIGITS];  /* New CHV to replace the old one */
} uim_unblock_chv_type;

typedef PACKED struct PACKED_POST {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified */
  byte  new_chv[UIM_MAX_CHV_DIGITS];  /* New CHV to replace the old one */
} uim_change_chv_type;

typedef PACKED struct PACKED_POST {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified*/
} uim_enable_chv_type;

typedef PACKED struct PACKED_POST {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified*/
} uim_disable_chv_type;

typedef PACKED struct PACKED_POST {
  byte len_and_usage;                 /* Length of usage and Usage */
  byte esn[7];                        /* ESN */
} uim_store_esn_type;

typedef PACKED struct PACKED_POST {
  byte len_and_usage;                 /* Length of usage and Usage */
  byte meid[UIM_RUIM_UIMID_MAX_SIZE]; /* MEID */
} uim_store_meid_type;

typedef PACKED struct PACKED_POST {
  byte randseed[UIM_RAND_SEED];       /* Random Seed for Key Generation */
  byte a_key_p_rev;                   /* A_KEY_P_REV */
  byte param_p_len;                   /* Param P Length */
  byte param_g_len;                   /* Param G Length */
  byte param_p[UIM_PARAM_P];          /* Param P */
  byte param_g[UIM_PARAM_G];          /* Param G */
} uim_ms_key_req_type;

typedef PACKED struct PACKED_POST {
  byte bs_result_len;                 /* Length of BS Result */
  byte bs_result[UIM_PARAM_P];        /* BS Result */
} uim_key_gen_req_type;

typedef PACKED struct PACKED_POST {
  byte key_id[UIM_SERV_KEY_GEN_ID_SIZE];  /* Key ID Bitmap*/
} uim_serv_key_gen_req_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                      /* Block ID of the Download Request */
  byte block_len;                     /* BLock length */
  byte data[UIM_MAX_TRANS_SIZE_OUT - 2];     /* Data buffer */
} uim_validation_req_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                      /* Block ID of the Config Request */
} uim_config_req_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                      /* Block ID of the Download Request */
  byte block_len;                     /* BLock length */
  byte data[UIM_MAX_TRANS_SIZE_OUT - 2];     /* Data buffer */
} uim_download_req_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                      /* Block ID of the Config Request */
  byte req_offset[2];                 /* Request Offset */
  byte req_max_size;                  /* Request Max Size */
} uim_sspr_config_req_type;

typedef PACKED struct PACKED_POST {
  byte block_id;                      /* Block ID of the Download Request */
  byte block_len;                     /* BLock length */
  byte data[UIM_MAX_TRANS_SIZE_OUT - 2];     /* Data buffer */
} uim_sspr_download_req_type;

typedef PACKED struct PACKED_POST {
  byte   start_stop;                   /* Length of BS Result */
  dword rand_seed;                    /* Random Seed */
} uim_otapa_req_type;

typedef PACKED struct PACKED_POST {
  byte   start_stop;                    /* Length of BS Result */
  dword rand_seed;                    /* Random Seed */
  byte   esn[UIM_RUIM_UIMID_MAX_SIZE];  /* ESN or pseudo ESN */
} uim_otapa_req_esn_type;

typedef PACKED struct PACKED_POST {
  byte  randa[UIM_AKA_AUTH_RANDA_LEN]; /* RANDA */
  byte  autn_len;   /* AUTN length */
  byte  autn[UIM_MAX_CHARS];  /* AUTN */
} uim_aka_auth_req_type;

typedef PACKED struct PACKED_POST {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
  byte new_reference_data[UIM_MAX_CHV_DIGITS]; /* New reference Data */
} uim_change_ref_in_chan_req_type;

typedef PACKED struct PACKED_POST {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
} uim_verify_in_chan_req_type;

typedef PACKED struct PACKED_POST {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
} uim_disable_verfn_in_chan_req_type;

typedef PACKED struct PACKED_POST {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
} uim_enable_verfn_in_chan_req_type;

typedef PACKED struct PACKED_POST {
  byte unblock_data[UIM_MAX_CHV_DIGITS];        /* Unblock Data */
  byte new_ref_data[UIM_MAX_CHV_DIGITS];        /* New Reference Data */
} uim_reset_retry_in_chan_req_type;

typedef union {
  uim_sel_mf_df_ef_req_type  mf_df_ef;          /* Select file */
  uim_verify_chv_type        verify_chv;        /* Verify CHV */
  uim_unblock_chv_type       unblk_chv;         /* Unblock CHV */
  uim_change_chv_type        change_chv;        /* Change CHV */
  uim_disable_chv_type       disable_chv;       /* Verify CHV */
  uim_enable_chv_type        enable_chv;        /* Verify CHV */
  byte                       data[UIM_MAX_TRANS_SIZE_OUT];
  uim_store_esn_type         store_esn;         /* Store ESN Type */
  uim_store_meid_type        store_meid;        /* Store MEID Type */
  uim_ms_key_req_type        ms_key;            /* MS Key Request */
  uim_key_gen_req_type       key_gen;           /* Key Gen Request */
  uim_serv_key_gen_req_type  serv_key_gen;      /* Service Key Gen Request */
  uim_validation_req_type    validate;          /* Validation Request */
  uim_config_req_type        config_req;        /* Configuration Request */
  uim_download_req_type      download_req;      /* Download Request */
  uim_sspr_config_req_type   sspr_cfg_req;      /* SSPR Cfg Request */
  uim_sspr_download_req_type sspr_download_req; /* SSPR Download Request */
  uim_otapa_req_type         otapa_req;         /* OTAPA Request */
  uim_otapa_req_esn_type     otapa_req_esn;     /* OTAPA Request with ESN/pESN */
  uim_aka_auth_req_type      aka_auth;          /* AKA Auth Request */
  uim_verify_in_chan_req_type      verification_in_chan;/* Verify */
  uim_disable_verfn_in_chan_req_type disable_verfn_in_chan;/* Disable Verfictn */
  uim_enable_verfn_in_chan_req_type  enable_verfn_in_chan; /* Enable Verfictn */
  uim_reset_retry_in_chan_req_type   retry_in_chan; /* Reset Retry Counter */
  uim_reset_retry_in_chan_req_type reset_retry_in_chan;/* Reset Retry Counter */
  uim_change_ref_in_chan_req_type  change_ref_in_chan; /* Change Ref Data */
} uim_req_type;

typedef PACKED struct PACKED_POST {
  byte                    uim_class;  /* UIM class of instruction           */
  uim_instrns_enum_type  instrn;      /* Instruction type                   */
  byte                    p1;         /* input parameter 1                  */
  byte                    p2;         /* input parameter 2                  */
  byte                    p3;         /* input parameter 3                  */
} uim_apdu_type;

typedef enum {
  UIM_INSTRN_CASE_1    = 0, /* Case 1 - No data exchanged */
  UIM_INSTRN_CASE_2    = 1, /* Case 2 - Response data only */
  UIM_INSTRN_CASE_3    = 2, /* Case 3 - Command data only */
  UIM_INSTRN_CASE_4    = 3, /* Case 4 - Response and command data */
  UIM_INSTRN_IFS_BLOCK = 4  /* Used to send IFS requests to the driver */
} uim_instrn_case_type;

typedef struct {
  uim_protocol_type protocol;          /* Protocol of the APDU */
  uim_apdu_type     apdu_hdr;          /* APDU header */
  uim_req_type      req;               /* Request */
  uim_rsp_buf_type  *rsp_ptr;          /* Response pointer */
  uim_instrn_case_type instrn_case;    /* instruction case */
} uim_req_buf_type;

/* This type defines how to change the operations parameters. */
typedef struct {
  boolean change_clk_freq;           /* This indicates whether to change the UIM
                                        clock frequency                       */

  uim_clock_control_type  clock_frequency; /* This holds the selected clock frequency */

  boolean change_baud_rate;          /* This indicates whether to change the UIM
                                        interface baudrate                    */

  /* These parameters determine the baudrate of the UIM interface.            */
  uim_clock_rate_conversion_factor_type FI;
                                    /* Holds the UIM clock rate conversion    */
  uim_baud_rate_adjustment_factor_type  DI;
                                    /* Holds the baudrate adjustment factor   */
  boolean change_guardtime;         /* This indicates whether to change the UIM
                                       guardtime                              */
  byte guardtime_bits;              /* Number of ETUs to increase the
                                       guardtime                              */
} uim_op_params_type;

/*
  This type defines the PPS request parameters.  The parameters contain the
  PPS request as well as where to put the PPS response.
 */
typedef struct {
  byte size_of_pps_request;   /* This is the size of the PPS request          */
  byte *request_buf;          /* This points to the PPS request.              */
  uim_rsp_buf_type  *rsp_ptr; /* This points to where to place the PPS
                                 response                                     */
} uim_pps_req_buf_type;

/* First instruction class for the command */
extern nv_uim_first_inst_class_type uim_hotswap_first_ins_class[UIM_MAX_INSTANCES];

/*
 * This type identifies the convention of the SIM card interface.
 */
typedef enum {
  /* Identifies direct convention on the SIM interface */
  UIM_DIRECT_CONVENTION,
  /* Identifies inverse convention on the SIM interface */
  UIM_INVERSE_CONVENTION
} uim_convention_type;

/* Structure type to hold clock ids for use with DAL clock APIs and whether that
   clock has been enabled */
typedef struct
{
  ClockIdType nClkId;
  boolean bEnabled;
}uim_dal_clk_info_type;

#if defined(FEATURE_UIM_TEST_FRAMEWORK) && !defined(T_QDSP6)
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK AND !T_QDSP6 */

/*===========================================================================

FUNCTION UIM_POWER_UP                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function turns on the power for the UIM.  It puts the I/O line in
  reception mode (stops sending break on the I/O line) and turns on the
  UIM clock.

DEPENDENCIES
  This function must be executed either after uim_dex_init or after
  uim_power_down.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is powered up and has a clock.

===========================================================================*/
void uim_power_up
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_RESET                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function asserts the reset line on the UIM.  Any data received by the
  phone will be flushed.  Any transfer from the phone will be terminated and
  the buffers flushed.

DEPENDENCIES
  This function must be executed after uim_dex_init.  The UIM should be powered
  up (via uim_power_up) for this function to have any effect.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver is expecting an ATR after this reset.

===========================================================================*/
void uim_reset
(
  uim_rsp_buf_type         *rsp_ptr,    /* Defines where to put the ATR  */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_SEND_PPS                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the transfer of a PPS request to the UIM.  This
  function starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts a PPS exchange.  This exchange can only be done
  immediately after an ATR is received.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the PPS request.  Then the
  driver will expect the PPS response.

===========================================================================*/
void uim_send_pps
(
  uim_pps_req_buf_type const *pps_request,  /* This points to the PPS request and
                                               the PPS response buffer.           */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_UPDATE_OP_PARAMS                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the operational parameters of the UIM interface.  It
  can change the baud rate of the UIM interface, change the UIM clock
  frequency or change the guardtime.

DEPENDENCIES
  This function changes the operational parameters of the UIM interface.
  Whatever changes are requested in this function shall apply as soon as
  possible.  The interface will reflect the changes.

RETURN VALUE
  Whether CLK stop is performed successfully. This value will be used in the
  caller to change CLK state.

SIDE EFFECTS
  All transfers over the UIM interface will occur at the frequency, baudrate,
  or guardtime as specified in this function call.

===========================================================================*/
boolean uim_update_op_params
(
  uim_op_params_type const *op_params,       /* Proposed operational parameters */
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_SEND_COMMAND                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the transfer of an APDU to the UIM.  This function
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
void uim_send_command
(
  uim_req_buf_type        *uim_req, /* This points to the APDU command buffer, the
                                 command data buffer, the response buffer, the
                                 APDU status and the callback function.       */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_IS_FI_DI_SUPPORTED                        EXTERNALIZED FUNCTION

DESCRIPTION
  This function tells if a {FI, DI} combination is supported by the
  platform or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_fi_di_supported
(
  uim_instance_global_type *uim_ptr,
  uim_clock_rate_conversion_factor_type  FI, /* CRCF index */
  uim_baud_rate_adjustment_factor_type   DI /* BRAF index */
);


/*===========================================================================

FUNCTION UIM_POWER_DOWN                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function deactivates the UIM so it does becomes inactive and does not
  use power.  The electrical lines to the UIM are all set low in the
  appropriate order.

DEPENDENCIES
  This function inactivates the UIM.  The uim_power_up and uim_reset
  functions must be called after this function to bring the UIM back up
  to use.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is power down and inactive.

===========================================================================*/
void uim_power_down
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_ISO_POWER_DOWN_BEFORE_USB_ATTACH

DESCRIPTION
  This function set turn off clk, reset and data line with vcc line stay high.
  The electrical lines to the UIM are all set low in the appropriate order.

DEPENDENCIES
  This function inactivates the UIM.  The uim_power_up and uim_reset
  functions must be called after this function to bring the UIM back up
  to use.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is power down and inactive.

===========================================================================*/
void uim_iso_power_down_before_usb_attach
(
  uim_instance_global_type *uim_ptr
);

#if defined( FEATURE_UIM_T_1_SUPPORT )
/*===========================================================================

FUNCTION UIM_T_1_SEND_I_BLOCK

DESCRIPTION
  This function builds a T=1 information block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the information block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The information block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
void uim_t_1_send_i_block
(
  byte                    node_address,
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_T_1_SEND_R_BLOCK

DESCRIPTION
  This function builds a T=1 receive ready block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the receive ready block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The receive ready block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
void uim_t_1_send_r_block
(
  byte                    node_address, /* Node address byte */
  byte                    r_block_code, /* R-block response code */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_T_1_SEND_S_BLOCK

DESCRIPTION
  This function builds a T=1 supervisory block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the supervisory block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The supervisory block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
void uim_t_1_send_s_block
(
  byte                     node_address,        /* Node address byte */
  byte                     s_block_code,        /* S-block code */
  byte                     *s_block_data,       /* S-block data */
  byte                     s_block_data_size,    /* S-block data size */
  uim_instance_global_type *uim_ptr
);
#endif /* FEATURE_UIM_T_1_SUPPORT */


/*===========================================================================

MACRO UIM_IS_UIM_CLK_ON


DESCRIPTION
  Checks to see if the UIM clock is on

DEPENDENCIES
  None

RETURN VALUE
  TRUE if Clock is on, or if FEATURE_UIM_TCXO_SLEEP is not defined
  FALSE is Clock is off (or if UIM has voted Clock off)

SIDE EFFECTS
  None

===========================================================================*/
#define UIM_IS_UIM_CLK_ON(uim_ptr)  uim_ptr->state.clock_is_on

typedef enum {
  UIM_RX_PROCESS_NONE,   /* Not expecting any data               */
  UIM_RX_PROCESS_IDLE,   /* Not expecting any data               */
  UIM_RX_WAIT_FOR_TXREADY, /* State to wait for the TX_READY state */
  UIM_RX_TXREADY,          /* State indicating TX_READY received */
  UIM_RX_WAIT_FOR_TXLEV,   /* State to wait for the TX_LEV state */
  UIM_RX_TXLEV,            /* State indicating TX_LEV received */
  UIM_RX_WAIT_FOR_TXCOMP,  /* State to wait for the TX_COMP interrupt */
  UIM_RX_TXCOMP,           /* State indicating TX_COMP received */
  UIM_RX_WAIT_FOR_RXSTALE, /* State to wait for the RX_STALE interrupt */
  UIM_RX_RXSTALE,          /* State indicating RX_STALE received */
  UIM_RX_WAIT_FOR_RXLEV,   /* State to wait for the RX_LEV interrupt */
  UIM_RX_RXLEV,            /* State indicating RX_LEV received */
  UIM_RX_RECEIVE_ATR,    /* Expecting the ATR                    */
  UIM_RX_ATR_RECEIVED_WITH_PARITY_ERRORS, /* State indicating ATR received with parity errors */
  UIM_T_1_RX_RECEIVED_WITH_PARITY_ERRORS, /* State indicating bytes received with parity errors */
  UIM_T_1_RX_LEV_RECEIVED_WITH_PARITY_ERRORS, /* State RX_LEV intterupt received with parity errors */
  UIM_RX_RECEIVE_PPS,    /* Expecting the PPS response           */
  UIM_RX_PROC_PRCDR_BYTES,/* Expecting procedure bytes from the UIM  */
  UIM_RX_OVERRUN,        /* UART is overrun */
  UIM_RX_SW1_NORM_END,   /* Expecting the second status word     */
  UIM_RX_SW1_FETCH_END,  /* Expecting the second status word     */
  UIM_RX_SW1_DLOAD_ERR,  /* Expecting the second status word     */
  UIM_RX_SW1_RESP_END,   /* Expecting the second status word     */
  UIM_RX_SW1_BUSY,       /* Expecting the second status word     */
  UIM_RX_SW1_RETRIES,    /* Expecting the second status word     */
  UIM_RX_SW1_REFERENCE,  /* Expecting the second status word     */
  UIM_RX_SW1_SECURITY,   /* Expecting the second status word     */
  UIM_RX_SW1_P3_BAD,     /* Expecting the second status word     */
  UIM_RX_SW1_P1_P2_BAD,  /* Expecting the second status word     */
  UIM_RX_SW1_INS_BAD,    /* Expecting the second status word     */
  UIM_RX_SW1_CLA_BAD,    /* Expecting the second status word     */
  UIM_RX_SW1_PROBLEM,    /* Expecting the second status word     */
  UIM_RX_PROC_RESP_DATA, /* Expecting the APDU response          */
  UIM_RX_SW1_WARNINGS1,  /* Expecting the second status word     */
  UIM_RX_SW1_WARNINGS2   /* Expecting the second status word     */

#if defined( FEATURE_UIM_T_1_SUPPORT )
  ,UIM_RX_T_1_RECV_T_1_BLOCK_DM  /* Receive the T=1 block for T=1         */
  ,UIM_RX_T_1_SEND_PROLOGUE_ST   /* Send the prologue field for T=1       */
  ,UIM_RX_T_1_SEND_INFO_ST       /* Send the information field for T=1    */
  ,UIM_RX_T_1_SEND_EPILOGUE_ST   /* Send the epilogue field for T=1       */
  ,UIM_RX_T_1_RX_PROLOGUE_ST     /* Receive the prologue field for T=1    */
  ,UIM_RX_T_1_RX_INFO_ST         /* Receive the information field for T=1 */
  ,UIM_RX_T_1_RX_EPILOGUE_ST     /* Receive the epilogue field for T=1    */
#endif /* FEATURE_UIM_T_1_SUPPORT */

  ,UIM_RX_SW1_EXECUTION_ERR2     /* Expecting the second status word     */
  ,UIM_RX_SW1_CMD_NOT_ALLOWED    /* Expecting the second status word     */
  ,UIM_RX_SW1_WRONG_PARAMS       /* Expecting the second status word     */
  ,UIM_RX_SW1_USIM_RESEND_APDU   /* Expecting the second status word     */
  ,UIM_RX_SW1_USIM_END_RESP      /* Expecting the second status word     */
  ,UIM_RX_SW1_EXECUTION_ERR1     /* Expecting the second status word     */
  ,UIM_RX_SW1_CLA_NOT_SUPPORTED  /* Expecting the second status word     */
  ,UIM_RX_SW1_SE_FAIL           /* Expecting the second status word    */
  ,UIM_RX_UNKNOWN_PROC_BYTE_RCVD /* Un-known procedure byte received */

} uim_rx_state_type;

/*
   The following defines the return status of the uim_process_guardtime
   function.
 */
typedef enum {
  UIM_TX_SEND_NONE,      /* Not sending any data               */
  UIM_TX_SEND_PPS,       /* Sending the PPS bytes                */
  UIM_TX_SEND_CMD_HDR,   /* Sending the APDU command header      */
  UIM_TX_SND_CMD_DATA    /* Sending the command data bytes       */
} uim_tx_state_type;

dword uim_read_status(uim_instance_global_type *uim_ptr);

/*===========================================================================

FUNCTION UIM_SWT_CMD_RSP_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the software command response timer. This call-back is called in the context of the
  high priority timer task as soon as the rex timer for the software work waiting time
  expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Power down of the SIM interface if the command response timeout expired.

===========================================================================*/
void uim_swt_cmd_rsp_timer_expiry_cb
(
  unsigned long instance_id
);
/*===========================================================================

FUNCTION UIM_CMD_RSP_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the command response timer. This call-back is called in the context of the
  high priority timer task as soon as the rex timer for the work waiting time
  expires.

  The logic in this call-back is as follows:
    - (for LCU) check if the count in the rx fifo + the packing buffer increased
        from the last check.
          - If the count increased, then re-start the wwt timer.
          - else, put driver’s uim_rx_state to IDLE, mask all interrupts, reset
            uart, set cmd_rsp_timeout signal and powerdown the sim interface

    - (for non-LCU) check if RXRDY bit is set in the status. If set,
        set cmd_rsp_timeout and return. If not set, put driver’s uim-rx-state to
        idle, mask all interrupts, reset uart, set cmd_rsp_timeout signal and
        power down the sim interface.

  NOTE: The powerdown sequence of the UIM (as in the uim_power_down() function)
        is not started within this call-back as it is a huge-piece of logic
        which might hog the CPU as this call-back is being executed in the
        context of the timer task. Hence, this call-back only signals the
        UIM task to initiate the powerdown sequence, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Power down of the SIM interface if the command response timeout expired.

===========================================================================*/
void uim_cmd_rsp_timer_expiry_cb
(
  unsigned long instance_id
);

/*===========================================================================

FUNCTION UIM_40K_ATR_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a callback associated with the expiry of a
  timer. When the timer expires and the callback is called the UIM CMD RSP
  timeout signal is set to power-down the interface, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_40k_atr_timer_expiry_cb
(
  timer_cb_data_type param
);

/*===========================================================================

FUNCTION UIM_UIMDRV_FLUSH_UART

DESCRIPTION
  This function flushes the bytes in the RX FIFO by reading the bytes/words
  from the UART one by one and resets the UART RX to reset the error flags in
  UART. This way of reading the byte/words from the UART might, most of the
  times, take care of flushing those bytes/words that have just entered the
  UART and/or entering into the UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_uimdrv_flush_uart
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_RX_ISR

DESCRIPTION
  Main interrupt handling method externed here
  to be registered in the interrupt controller

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_rx_isr(
  uim_instance_global_type *uim_ptr
);


/**
 * Reconfigures the UART_DM to direct convention.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_reconfig_uartdm(uim_instance_global_type *uim_ptr);


/**
*  This function is used to turn on the UIM related clocks.
*
*
*  @param uim_ptr Pointer to the global UIM instance
*/
void UIM_TCXO_MUST_BE_ON_SLOT(uim_instance_global_type *uim_ptr);
#endif /* UIMDRV_H */
