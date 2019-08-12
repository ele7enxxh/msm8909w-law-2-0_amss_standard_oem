#ifndef HDRSLPRX_H
#define HDRSLPRX_H
/*===========================================================================

            H D R   S I G N A L I N G   L A Y E R   P R O T O C O L
             R E C E I V I N G   C H A I N   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Signaling Layer Protocol (SLP)
  receiving chain.

Copyright (c) 2000, 2001, 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrslprx.h_v   1.4   25 Sep 2001 16:50:38   omichael  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrslprx.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/06   sju     Fixed typo in commenting reassmebly buffers.
06/05/05   dna     Allow Broadcast to use separeate seqno from unicast
09/25/01   om      Removed FEATURE_HDR_HAI_X4
02/22/00   om      Created Module

===========================================================================*/

#ifndef COMDEF_H
  #error Please include <comdef.h> first
#endif
#ifndef DSM_H
  #error Please include <dsm.h> first
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Check for brdcst / unicast SDU's */
#define HDRSLP_RXA_BUF( msg_ptr )    HDRDSM_GET_BCAST_BIT( msg_ptr )

typedef struct
{
  dsm_item_type*  buffer_ptr;
  uint8           length;
  uint8           frg_seqno;
  boolean         last_frg;
} hdrslp_reassembly_buffer_struct_type;

/* Two reassembly buffers - [1] for brdcst and [0] for unicast fragments */
extern hdrslp_reassembly_buffer_struct_type  hdrslp_reassembly_buffer[2];
extern uint8                                 hdrslp_seqno_expected;
extern uint8                                 hdrslp_seqno_received;

/* <EJECT> */ 
/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSLP_PROCESS_RCVD_SIGNALING

DESCRIPTION
  This function processes received signaling data:
    1. Reassemble SDU fragments
    2. Do sanity checks on received SDUs
    3. Maintain the bit array of the sequence space
    4. Update list of SLP ACKs to be sent w/ next frame

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the SDU

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrslp_process_rcvd_signaling( dsm_item_type* msg_ptr );

#endif /* HDRSLP_H */
