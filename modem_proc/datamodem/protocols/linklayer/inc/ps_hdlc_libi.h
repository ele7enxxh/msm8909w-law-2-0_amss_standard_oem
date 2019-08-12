#ifndef PS_HDLC_LIBI_H
#define PS_HDLC_LIBI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ H D L C _ L I B I . H


GENERAL DESCRIPTION
  Header file for the internal HDLC library functions.

Copyright (c)2005 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_hdlc_libi.h#1 $ $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/23/14    skc    Removing FEATURE_DATA_PS_HDLC_PP
01/31/11    sa     Made changes for Software decoupling.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
12/19/07    scb    Added protocol processor framer and deframer functionality.
09/27/07    ifk    Removed return_on_bad_crc config parameter.
04/20/06    ssh    Added "cached mode" under FEATURE_HDLC_SW_WORKAROUND, etc.
11/15/05    ssh    Removed hdlc_mode, added some defines and comments
10/20/05    ssh    Support for hardware framing
10/07/05    ssh    Added data_state to hdlc_instance_type to capture HW state
09/30/05    ssh    Review comments
09/14/05    ssh    created module.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_hdlc_lib.h"

#ifdef FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
#include "dsmutil.h"
#endif /* FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED */

#ifdef FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
#ifndef FEATURE_DSM_DYNAMIC_POOL_SELECTION
/* make sure feature FEATURE_DSM_DYNAMIC_POOL_SELECTION is defined when feature
   FEATURE_DATA_PS_HDLC_HW_OPTIMIZED is defined */
#error "Feature FEATURE_DSM_DYNAMIC_POOL_SELECTION (dsmutil.c/h) not defined"
#endif /* FEATURE_DSM_DYNAMIC_POOL_SELECTION */
#endif /* FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define HDLC_FLAG          0x7e   /* HDLC async start/stop flag            */
#define HDLC_ESC_FLAG      0x7d   /* HDLC transparency escape flag         */
#define HDLC_ESC_ASYNC     HDLC_ESC_FLAG
#define HDLC_ESC_COMPL     0x20   /* HDLC transparency bit complement mask */

#ifdef FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
#define HDLC_BURST_READ_SIZE 32   /* Number of bytes that can be read in 
                                     one burst */
#endif /* FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED */

#ifdef FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
/* macro to allocate DSM item in PS module
   pool_id: DSM pool id;
   size_requested: memory size reqested 
   fall_back: if large item is not available then fall back to SMALL item and vice-versa */
#define hdlci_allocate_buffer(pool_id, size_requested, fall_back) \
  dsm_new_buffer_flexible(pool_id, size_requested, fall_back, DSM_ITEM_SUBSYSTEM_PS)
#else /* FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED not defined */
#define hdlci_allocate_buffer(pool_id, size_requested, fall_back) \
  dsm_new_buffer(pool_id)
#endif /* FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE DATA STRUCTURES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
  TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF HDLC_F_PTR_TYPE

DESCRIPTION
  Type of function called for framing (or unframing).
  This function pointer is present in the HDLC instance, and gets set to
  either hardware or the software framing (or unframing) function depending 
  upon desired HDLC mode.
---------------------------------------------------------------------------*/
typedef int (*hdlc_f_ptr_type)
(
  void            * handle,        /* associated HDLC instance             */
  dsm_item_type  ** src,        /* addr of ptr to source pkt               */
  dsm_item_type  ** dest,        /* addr of ptr to destination pkt          */
  void           *user_data_ptr /*To hold information such as meta_info_ptr etc*/
);


/*---------------------------------------------------------------------------
TYPEDEF HDLC_INSTANCE_TYPE

DESCRIPTION
  This is the instance type of the HDLC engine.
  (1) UNFRAMED_PKT_PTR
      Points to the start of the unframed packet (DSM chain) being assembled.
  (2) UNFRAMED_PKT_TAIL_PTR 
      Points to the end of the unframed packet (DSM chain) being assembled.
  (3) ERRNO
      Stores errno associated with the current packet, look at 
      hdlc_errno_enum_type for the values this can take.
  (4) FRAME_CRC - Current CRC value for the unframed packet chain.
  (5) MAX_LEN - Max length of the unframed packet chain (in bytes).
      NOT SUPPORTED CURRENTLY.
  (6) LAST_BYTE_WAS_ESC -  Indicates if the last byte was an escape byte
      This actually stores the mask HDLC_ESC_COMPL (0x20)
  (7) RETURN_PKT_ON_BAD_CRC - Configurable param, indicates if the data in 
      an unframed packet with bad checksum will be returned to the client.
  (8) STRIP_FF03_IF_PRESENT - NOT SUPPORTED CURRENTLY.
  (9) STATE - Current state of unframing, FINDING_7E, FINDING_SOP etc.
  (10) FRAME_F - Function pointer to use for framing, can point to
       software or hardware function depending upon configuration.
  (11) UNFRAME_F - Function pointer to use for unframing, can point to
       software or hardware function depending upon configuration.
  (12) ACCM - The Async-Control-Character-Map to use for framing.
  (13) UNFRAMED_PKT_Q - Queue to store multiple packets returned by hardware
  (14) DATA_STATE - Data state returned by hardware (the actual contents 
       of this 32-bit register are abstracted in the hardware driver.
  (15) SW_MODE_REQUIRED - This is to enforce the HDLC client's desire to
       stay in software mode. It's used during the set accm ioctl to prevent
       transparent switching to hardware mode (if the new accm is acceptable
       for hardware). Known use case: PPP needs to be in software mode 
       when operating in pseudonet mode.
 
  State needed to continue previous deframing operation
  Software: frame_crc, last_byte_was_esc, state
  Hardware: frame_crc, data_state

  Rest are common and used by both hardware and software.
---------------------------------------------------------------------------*/

/* Needed for Data Services */
typedef struct 
{
  uint32 *src_reg_ptr;
  uint32 *dst_reg_ptr;
} cd_reg_s_type;

typedef struct __instance_s
{
  dsm_item_type            *unframed_pkt_ptr;
  dsm_item_type            *unframed_pkt_tail_ptr;
  hdlc_errno_enum_type      ps_errno;
  uint16                    frame_crc;
  uint16                    max_len;
  byte                      last_byte_was_esc;
  boolean                   strip_ff03_if_present;
  hdlc_unframe_state_type   state;
  hdlc_f_ptr_type           frame_f;
  hdlc_f_ptr_type           unframe_f;
  uint32                    accm;
  q_type                    unframed_pkt_q;
  uint32                    data_state;
  boolean                   sw_mode_required;
} hdlc_instance_type;

/*---------------------------------------------------------------------------
HDLC IOCTLS
  Description of supported HDLC ioctls:
  GET_ACCM       - Returns the ACCM for the specified HDLC instance
  SET_ACCM       - Sets the ACCM for the specified HDLC instance
  GET_HDLC_MODE  - Returns the HDLC mode for the specified HDLC instance
  SET_HDLC_MODE  - Sets the HDLC mode for the specified HDLC instance
  GET_UNFRAMER_STATE - Returns the current state the unframer is in.
  SET_UNFRAMER_STATE - Sets the current state of the unframer
---------------------------------------------------------------------------*/
typedef enum hdlc_ioctl_e_type
{
  HDLC_IOCTL_MIN                 = 0,
  HDLC_IOCTL_GET_ACCM            = HDLC_IOCTL_MIN,
  HDLC_IOCTL_SET_ACCM            = 1,
  HDLC_IOCTL_GET_HDLC_MODE       = 2,
  HDLC_IOCTL_SET_HDLC_MODE       = 3,
  HDLC_IOCTL_GET_UNFRAMER_STATE  = 4,
  HDLC_IOCTL_SET_UNFRAMER_STATE  = 5,
  HDLC_IOCTL_MAX
} hdlc_ioctl_e_type;

/*---------------------------------------------------------------------------
  HDLC ioctls data
---------------------------------------------------------------------------*/
typedef union hdlc_ioctl_u_type
{
  uint32                   accm;
  hdlc_mode_enum_type      hdlc_mode;
  hdlc_unframe_state_type  unframe_state;
} hdlc_ioctl_u_type;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION HDLC_POWERUP_INIT()

DESCRIPTION This function is called during the powerup of PS task. As a
            part of this powerup initialization function, the protocol
            processor module if present is initialized. 

PARAMETERS

RETURN VALUE
  Returns 0 : If success
          -1 : If failed

DEPENDENCIES
  This

SIDE EFFECTS
===========================================================================*/
int hdlc_powerup_init
(
  void
);
/*===========================================================================
FUNCTION PS_HDLC_LIB_INIT()

DESCRIPTION
  Initializes ps_hdlc_lib module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_hdlc_lib_init
(
  void
);

/*===========================================================================
FUNCTION HDLC_RESET()

DESCRIPTION
  This function will reset the state of the HDLC instance. This includes
  freeing up any partially unframed packets, trashing the unframed packet
  queue and resetting the state variables. It also resets the hardware
  framer and deframer. The parameters set originally via config (max_len,
  return_pkt_on_bad_crc, strip_ff03_if_present and accm) stay unchanged, as
  do the framing and unframing function pointers (frame_f and unframe_f).

PARAMETERS
  handle: pointer to the instance being freed.

RETURN VALUE
  0  : On Success
  -1 : On Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int hdlc_reset
(
  void * handle
);

/*===========================================================================
FUNCTION HDLC_FRAME()

DESCRIPTION
  This function will frame the incoming packet.
  The input packet pointer (unframed) should be the complete packet to be
  framed including HDLC header and protocol field (if we desire to send
  them), but without the CRC.

PARAMETERS
    handle: The HDLC instance to use
    unframed: Points to the packet that needs framing.
    framed  : Points to the framed packet, *framed must be NULL when called.
    user_data: Any additional user data to be passed on as a part of asynchronous
               processing by the protocol processor

DEPENDENCIES
  The ACCM must be set for this HDLC instance before calling this function.

RETURN VALUE
  0 on success
 -1 on failure - use hdlc_errno() for specifics of error.  The header file
                 has the definitions of the various errnos.

SIDE EFFECTS
  None
===========================================================================*/
int hdlc_frame
(
  void           * handle,
  dsm_item_type ** unframed,
  dsm_item_type ** framed,
  void           * user_data
);

/*===========================================================================
FUNCTION HDLC_IOCTL

DESCRIPTION
  Performs the required operation on an HDLC instance.
  Takes an HDLC instance, the type of ioctl and associated data. Depending on
  the IOCTL the data may be used to return or pass information.

PARAMETERS
      handle: The HDLC instance to use
  ioctl_type: The HDLC ioctl
  ioctl_data: Associated ioctl information
       errno: Error code returned, if any error

DEPENDENCIES
  None

RETURN VALUE
  -1 in case of error (e.g., invalid arguments)
   0 in case of success

SIDE EFFECTS
  The ACCM or HDLC mode for the specified instance may be changed.
===========================================================================*/
int hdlc_ioctl
(
  void                 * handle,
  hdlc_ioctl_e_type      ioctl_type,
  hdlc_ioctl_u_type    * ioctl_data,
  hdlc_errno_enum_type * ps_errno
);

#endif /* PS_HDLC_LIBI_H */
