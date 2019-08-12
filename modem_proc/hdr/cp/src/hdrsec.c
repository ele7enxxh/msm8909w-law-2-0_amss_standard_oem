/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             G E N E R I C   S E C U R I T Y   P R O T O C O L   

GENERAL DESCRIPTION
   This module contains the procedures for the generic security protocol 
   as defined in the security layer of IS-856.

EXTERNALIZED FUNCTIONS (Global)

EXTERNALIZED FUNCTIONS (Regional)

IDLEIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrsec.c_v   1.2   18 Mar 2003 22:06:16   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsec.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/13/09   wsh     Fixed security header included when Auth type is default
10/23/08   wsh     Fixed critical/high lint warnings
04/28/06   pba     Fixed compiler warnings
04/07/06   hal     Changed dsm_pushdown() to use hdrutil_pushdown_head()
03/24/05   pba     Lint cleanup
03/18/03   mpa     Fixed compiler warning
12/06/01   om      Fixed some MSG_HIGH to MSG_LOW
11/29/01   om      First integrated version
10/19/01   dna     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qw.h"
#include "hdrdebug.h"
#include "err.h"

#include "hdrts.h"
#include "hdrscp.h"

#include "hdrsec.h"
#include "hdrutil.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

/* <EJECT> */ 
/*===========================================================================

                    FOWARD DECLARATIONS FOR MODULE

===========================================================================*/
uint16 hdrauth_get_pkt
(
  uint16                    max_bytes,
  hdrhai_channel_enum_type  channel,
  uint16                    time_since_prev_req,
  dsm_item_type**           pkt_ptr,
  boolean*                  is_fmt_b_ptr
);

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Static storage for the cryptosync */
uint8   hdrsec_cryptosync[HDRSEC_CRYPTOSYNC_LEN];
uint8*  hdrsec_cryptosync_short = hdrsec_cryptosync;
uint16  hdrsec_cryptosync_len = 0;

/* <EJECT> */
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
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( cryptosync, sizeof(uint8) * HDRSEC_CRYPTOSYNC_LEN, 
           hdrsec_cryptosync, hdrsec_cryptosync_len );
#else
  memcpy( cryptosync, hdrsec_cryptosync, hdrsec_cryptosync_len );
#endif /* FEATURE_MEMCPY_REMOVAL */

  return hdrsec_cryptosync_len;

} /* hdrsec_get_cryptosync */

/* <EJECT> */
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
)
{
  uint16   pkt_len;
  boolean  include_header;
  qword    sys_time;
  int      index;
  uint16   auth_subtype;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( hdrscp_get_current_subtype( HDRHAI_SECURITY_PROTOCOL ) )
  {
    case HDRSCP_DEFAULT_SEC:
      /* The default cryptosync is zero */
      memset( hdrsec_cryptosync, 0, HDRSEC_CRYPTOSYNC_LEN );
      hdrsec_cryptosync_len = HDRSEC_CRYPTOSYNC_LEN;
      include_header = FALSE;
      *is_secured = FALSE;
      break;

    case HDRSCP_GENERIC_SECURITY:
      auth_subtype = 
        hdrscp_get_current_subtype( HDRHAI_AUTHENTICATION_PROTOCOL );

      if ( auth_subtype != HDRSCP_DEFAULT_AUTH )
      {
        /* Update the cryptosync for this packet and change to network order */
        hdrts_get_80ms_time( sys_time );
    
        for (index = 0; index <= 7; index++)
        {
          hdrsec_cryptosync[index] = QW_BYTE( sys_time, 7 - index );
        }
    
        hdrsec_cryptosync_short = hdrsec_cryptosync + 6;
        hdrsec_cryptosync_len = HDRSEC_CRYPTOSYNC_LEN;
        max_bytes -= HDRSEC_CRYPTOSYNC_SHORT_LEN;
        include_header = TRUE;
        *is_secured = TRUE;
      }
      else
      {
        memset( hdrsec_cryptosync, 0, HDRSEC_CRYPTOSYNC_LEN );
        hdrsec_cryptosync_len = HDRSEC_CRYPTOSYNC_LEN;
        include_header = FALSE;
        *is_secured = FALSE;
      }
      break;

    default:
      ERR( "SEC: Unknown Prot. Subtype %d", 
           hdrscp_get_current_subtype( HDRHAI_SECURITY_PROTOCOL ), 0, 0 );
      hdrsec_cryptosync_len = 0;
      include_header = FALSE;
      *is_secured = FALSE;
      break;
  }

  /* Get the authentication layer payload */
  pkt_len = hdrauth_get_pkt( max_bytes,
                             channel,
                             time_since_prev_req,
                             pkt_ptr,
                             is_fmt_b_ptr );
  HDR_MSG_PROT_2 (MSG_LEGACY_LOW,  "SEC: Rcvd %d/%d bytes from AUTH", 
                  pkt_len, dsm_length_packet( *pkt_ptr ));

  /* Prepend the SEC header if there is one */
  if ( include_header && pkt_len )
  {
    (void) hdrutil_pushdown_head( pkt_ptr,
                           hdrsec_cryptosync_short,
                           HDRSEC_CRYPTOSYNC_SHORT_LEN );

    pkt_len += HDRSEC_CRYPTOSYNC_SHORT_LEN;
  }
  
  return pkt_len;

} /* hdrsec_get_pkt */
