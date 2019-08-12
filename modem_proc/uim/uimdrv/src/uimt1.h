#ifndef UIMT1_H
#define UIMT1_H
/*===========================================================================

        E X T E R N A L   U I M   T = 1   T R A N S P O R T   L A Y E R
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM T=1 protocol transport layer.

Copyright (c) 2001,2002,2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimt1.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/14   ll      Remove of return_ptr and remove/change uim_free to UIM_FREE
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
12/05/12   js      UIM Parallel Processing changes
10/26/12   akv     HW Enumeration changes (without features)
09/21/12   ms      Added code for error handling during recovery
03/20/12   rm      Changes for WWT and hotswap support
09/01/10   ps      Merge UIM server DSDS changes
09/30/09   mib     Added PACKED_POST for Q6 compilation
10/06/08   rm      Cached select support for T=1 protocol
11/15/04   pv      Added more bit definitions to check the max values allowed,
                   added NAD to block_params type,
                   added extern uim_last_tx_i_block_length
02/24/03   ts      Created T=1 module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"

#ifdef FEATURE_UIM_T_1_SUPPORT
#include "comdef.h"
#include "uimi.h"
#include "uim.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* This defines the T=1 protocol states of operation */
typedef enum {
  UIM_T_1_FAILED_CMD_ST,            /* The current command has failed */
  UIM_T_1_CACHED_SEL_RSP_ST,        /* The SELECT response was already cached */
  UIM_T_1_SENT_APDU_ST,             /* The command has been completely sent */
  UIM_T_1_SENT_CHAINED_APDU_ST,     /* The command is being sent */
  UIM_T_1_SENT_SUPERVISORY_BLOCK_ST /* An S-block has been sent */
} uim_t_1_protocol_states_type;

/* This holds the current T=1 protocol state */
extern uim_t_1_protocol_states_type uim_t_1_protocol_state[UIM_MAX_INSTANCES];

/* These are used for ATR processing */
#define UIM_T_1_CWI_FIELD          0x0F
#define UIM_T_1_BWI_FIELD          0xF0
#define UIM_T_1_BWI_FIELD_SHIFT    4
#define UIM_T_1_EDC_FIELD          0x01

/* These are the PCB definitions */
/* Information block */
#define UIM_T_1_PCB_I_BLOCK        0x00
#define UIM_T_1_PCB_SEND_SEQ_NUM   0x40
#define UIM_T_1_PCB_MORE_DATA_BIT  0x20
/* Receive ready block */
#define UIM_T_1_PCB_R_BLOCK        0x80
#define UIM_T_1_PCB_NEXT_SEQ_NUM   0x10
#define UIM_T_1_PCB_ERROR_FREE     0x00
#define UIM_T_1_PCB_EDC_ERROR      0x01
#define UIM_T_1_PCB_OTHER_ERROR    0x02
#define UIM_T_1_PCB_ERR_CODE_MASK  0x0F
/* Supervisory block */
#define UIM_T_1_PCB_S_BLOCK        0xC0
#define UIM_T_1_PCB_S_RSP          0x20
#define UIM_T_1_PCB_RESYNC_REQ     0x00
#define UIM_T_1_PCB_RESYNC_RSP     0x20
#define UIM_T_1_PCB_IFS_REQ        0x01
#define UIM_T_1_PCB_IFS_RSP        0x21
#define UIM_T_1_PCB_ABORT_REQ      0x02
#define UIM_T_1_PCB_ABORT_RSP      0x22
#define UIM_T_1_PCB_WTX_REQ        0x03
#define UIM_T_1_PCB_WTX_RSP        0x23
#define UIM_T_1_PCB_VPP_STATE_ERR  0x24

#define UIM_T_1_SBLK_B1_B5_MASK    0x1F
#define UIM_T_1_SBLK_B1_B5_MAX_VAL 0x03

#define UIM_T_1_RBLK_B1_B6_MASK    0x2F
#define UIM_T_1_RBLK_B1_B6_MAX_VAL 0x02


#define UIM_T_1_NUM_BYTES_PROLOGUE 0x03

/* This type defines the T=1 block type */
typedef enum {
  UIM_T_1_BLOCK_TYPE_I,
  UIM_T_1_BLOCK_TYPE_R,
  UIM_T_1_BLOCK_TYPE_S
} uim_t_1_block_type_type;

/* This is the definition of the prologue field used for the T=1 protocol */
typedef PACKED struct PACKED_POST {
  byte nad;     /* Node Address byte */
  byte pcb;     /* Protocol control byte */
  byte len;     /* Length byte */
} uim_prologue_field_type;

/* This structure defines the epilogue field of a T=1 block */
typedef union {
  word crc;
  byte lrc;
} uim_epilogue_field_type;

/* This defines the parameters of an I-block */
typedef struct {
  byte send_seq_num;
  byte more_bit;
} uim_i_block_param_type;

/* This defines the parameters of an R-block */
typedef struct{
  byte send_seq_num;
  byte error_code;
}uim_r_block_param_type;

/* This defines the parameters of an S-block */
typedef enum {
  UIM_S_BLOCK_RESYNCH_REQUEST  = 0x00,
  UIM_S_BLOCK_IFS_REQUEST      = 0x01,
  UIM_S_BLOCK_ABORT_REQUEST    = 0x02,
  UIM_S_BLOCK_WTX_REQUEST      = 0x03,
  UIM_S_BLOCK_RESYNCH_RESPONSE = 0x20,
  UIM_S_BLOCK_IFS_RESPONSE     = 0x21,
  UIM_S_BLOCK_ABORT_RESPONSE   = 0x22,
  UIM_S_BLOCK_WTX_RESPONSE     = 0x23,
  UIM_S_BLOCK_VPP_ERROR        = 0x24
} uim_s_block_type;

/* This defines the different parameters from a received T=1 block */
typedef union {
  uim_i_block_param_type i_block;
  uim_r_block_param_type r_block;
  uim_s_block_type s_block;
} uim_t_1_block_params_type;

/* This structure defines the format of the parsed T=1 blocks */
typedef struct {
  byte node_address_byte;
  uim_t_1_block_type_type block_type;
  uim_t_1_block_params_type block_params;
} uim_block_params_type;

/* This defines the available Error Detection types */
typedef enum {
  UIM_T_1_EDC_LRC,
  UIM_T_1_EDC_CRC
} uim_t_1_edc_type;


/* Default T=1 BWI */
#define UIM_T_1_DEFAULT_BWI 4
/* Default T=1 CWI */
#define UIM_T_1_DEFAULT_CWI 13

/* Default T=1 IFS */
#define UIM_T_1_DEFAULT_IFS 32
/* Max and Min values */
#define UIM_T_1_MAX_IFS    254
#define UIM_T_1_MIN_IFS    1

/* T=1 initial sequence number */
#define UIM_T_1_INIT_SEQ_NUM 0
#define UIM_T_1_CHANGE_SEQ_NUM_MASK 0x1

/* T=1 EDC field sizes */
#define UIM_T_1_LRC_SIZE 1
#define UIM_T_1_CRC_SIZE 2


/*===========================================================================

FUNCTION UIM_PROCESS_T_1_COMMAND

DESCRIPTION
  This function processes a request to send an APDU to the UIM card while
  operating in T=1 transport protocol.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.  This
  function uses global variables to set up the UIM driver and server transfers.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the T=1 block response.  Additional blocks are sent out when processing
  the block responses from the card.

===========================================================================*/
void uim_process_t_1_command
(
  uim_req_buf_type *uim_req, /* This points to the APDU command buffer, the
                               command data buffer, the response buffer, the
                               APDU status and the callback function.      */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_PROCESS_T_1_BLOCK

DESCRIPTION
  This function processes the block response from a T=1 protocol transaction.
  a request to send an APDU to the UIM card while
  operating in T=1 transport protocol.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.  This
  function uses global variables to set up the UIM driver and server transfers.

RETURN VALUE
  None.

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the T=1 block response.  Additional blocks are sent out when processing
  the block responses from the card.

===========================================================================*/
void uim_process_t_1_block
(
  uim_rsp_buf_type         *rsp_ptr,
  rex_sigs_type            *i_mask,  /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_PROCESS_T_1_TIMEOUT

DESCRIPTION
  This function processes the timeouts that happen when running the T=1
  protocol.

DEPENDENCIES
  This function uses global variables to determine how to process the timout.

RETURN VALUE
  A boolean value that indicates whether the timout processing is fully
  handled within the T=1 protocol timeout processing.
  FALSE - timeout is not fully handled, task timeout needs to be processed.
  TRUE - timeout is fully handled within T=1 processing.

SIDE EFFECTS
  Either another request is sent to the card or this function indicates
  to the task level timeout processing to process the timeout.

===========================================================================*/
boolean uim_process_t_1_timeout
(
  uim_instance_global_type *uim_ptr
);

#endif /* FEATURE_UIM_T_1_SUPPORT */
#endif /* UIMT1_H */
