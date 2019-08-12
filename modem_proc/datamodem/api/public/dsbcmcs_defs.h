#ifndef DSBCMCS_DEFS_H
#define DSBCMCS_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D S B C M C S _ D E F S . H

GENERAL DESCRIPTION
  This file contains the externalized structures and definitions needed to 
  access the BCMCS database module and should be included by all modules
  that need to access BCDBAPP. Structures and definitions used internally
  by BCDBAPP should not be in this file.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

 Copyright (c) 2004 - 2010 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsbcmcs_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

-----------------------------------------------------------------------------
when        who    what, where, why
--------    ---    ------------------------------------------------------- 
11/02/10    sa     Removed meta comments.
04/04/06    kvd    Added new enum types DSBCMCS_FRAMING_NOT_SET &
                   DSBCMCS_PROTOCOL_NOT_SET used by db_update.
03/11/04    vr     Initial revision.
===========================================================================*/
/*===========================================================================
                      INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "hdraddr.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  The BCMCS flow id type. This can be different for 1X and HDR
---------------------------------------------------------------------------*/
//VR - move this to MC files before checkin
//typedef uint32 dsbcmcs_hdr_flow_id_type;

/*---------------------------------------------------------------------------
  BCDB zone type - it will be a SUBNET for HDR or a PZID for 1X. In the 
  future it can be expanded to support other technologies like UMTS, etc. 

  When requesting for the link layer information for an {IP, port},
  the calling module must specify the zone since the information may  be 
  different for different zones.
---------------------------------------------------------------------------*/
typedef enum
{
  DSBCMCS_ZONE_1X   = 0,
  DSBCMCS_ZONE_HDR  = 1,

  DSBCMCS_ZONE_MAX
} dsbcmcs_zone_enum_type;

typedef struct dsbcmcs_zone
{
  dsbcmcs_zone_enum_type type;

  union
  {
    hdraddr_type subnet;
    /*-----------------------------------------------------------------------
       Expand here for 1X, UMTS, etc. 
       Making this an union since the "zone" may not be 32-bits for other
       technolgies.
    -----------------------------------------------------------------------*/
  } zone;

} dsbcmcs_zone_type; 

/*---------------------------------------------------------------------------
  The upper layer protocol obtained after deframing the BCMCS packet.
---------------------------------------------------------------------------*/
typedef enum
{
  DSBCMCS_PROTOCOL_NOT_SET = -1,
                             /* field not set by user-so default value
                                is stored in the internal databse for 
                                that particualr entry when DB_UPDATE API
                                is called. */
  DSBCMCS_PROTOCOL_PPP  = 0, /*the packet is PPP encapsulated (there are one
                             or two protocol bytes preceding the IP packet)*/
  DSBCMCS_PROTOCOL_IPv4 = 1, /*the packet obtained after deframing is an IPv4 
                             packet without any further encapsulation.     */
  DSBCMCS_PROTOCOL_IPv6 = 2, /*the packet obtained after deframing is an IPv6 
                             packet without any further encapsulation.     */

  /* Add other protocols in future if needed */
  DSBCMCS_PROTOCOL_MAX
} dsbcmcs_protocol_enum_type;


/*---------------------------------------------------------------------------
  BCMCS framing type
---------------------------------------------------------------------------*/
typedef enum
{
  DSBCMCS_FRAMING_NOT_SET = -1,
                                  /* field not set by user-so default value
                                     is stored in the internal databse for 
                                     that particualr entry when DB_UPDATE API
                                     is called. */
  DSBCMCS_FRAMING_SEGMENT = 0,    /* Segment based framing                 */
  DSBCMCS_FRAMING_HDLC    = 1,    /* HDLC-like framing                     */
  DSBCMCS_FRAMING_MAX
} dsbcmcs_framing_enum_type;


#endif /* DSBCMCS_DEFS_H */
