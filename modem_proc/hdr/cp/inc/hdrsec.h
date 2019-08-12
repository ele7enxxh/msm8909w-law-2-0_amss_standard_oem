#ifndef HDRSEC_H
#define HDRSEC_H
/*===========================================================================

             G E N E R I C   S E C U R I T Y   P R O T O C O L   
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the generic security protocol.

Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrsec.h_v   1.0   29 Nov 2001 17:33:36   omichael  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrsec.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/01   om      First integrated version
10/19/01   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Length in octets of the cryptosync (short) header */
#define HDRSEC_CRYPTOSYNC_LEN         8
#define HDRSEC_CRYPTOSYNC_SHORT_LEN   2

/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSEC_GET_CRYPTOSYNC

DESCRIPTION
  This function provides the cryptosync.

DEPENDENCIES
  None

PARAMETERS
  cryptosync - Byte array to write the cryptosync to
  
RETURN VALUE
  Length of cryptosync in bytes, zero if not available

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrsec_get_cryptosync
(
  uint8*  cryptosync
);

/*===========================================================================

FUNCTION HDRSEC_GET_PKT

DESCRIPTION
  This function takes an authentication protocol payload 
  and prepends the cryptosync header if security is enabled.

DEPENDENCIES
  None

PARAMETERS
  max_bytes           - Maximum payload size the MAC layer can accommodate
  channel             - Channel payload is sent on
  time_since_prev_req - Number of 26ms intervals since last get_pkt() call
  pkt_ptr             - Pointer to payload (as dsm item chain)
  is_fmt_b_ptr        - Filled in by PCP to specify frame format
  is_secured          - Filled in by SEC to specify security

RETURN VALUE
  Length of the security layer payload in bytes

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrsec_get_pkt
(
  uint16                    max_bytes,
  hdrhai_channel_enum_type  channel,
  uint16                    time_since_prev_req,
  dsm_item_type**           pkt_ptr,
  boolean*                  is_fmt_b_ptr,
  boolean*                  is_secured
);

#endif /* HDRSEC_H */
