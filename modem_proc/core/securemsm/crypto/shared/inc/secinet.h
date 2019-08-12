#ifndef SECINET_H
#define SECINET_H
/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/inc/secinet.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 07:59:23 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/01/11    qxu     Created module
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

MACRO SEC_HTONL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572, ps_htonl) The bit shifting is correct*/
#define sec_htonl(x)                                                      \
  (((((uint32)(x) & 0x000000FFU) << 24) |                                \
  (((uint32)(x) & 0x0000FF00U) <<  8) |                                  \
  (((uint32)(x) & 0x00FF0000U) >>  8) |                                  \
  (((uint32)(x) & 0xFF000000U) >> 24)))

/*===========================================================================

MACRO SEC_HTONLL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572, ps_htonll) The bit shifting is correct*/
#define sec_htonll(x)                                                     \
  (((((uint64)(x) & 0x00000000000000FFULL) << 56)   |                    \
  (((uint64)(x) & 0x000000000000FF00ULL) << 40)   |                      \
  (((uint64)(x) & 0x0000000000FF0000ULL) << 24)   |                      \
  (((uint64)(x) & 0x00000000FF000000ULL) << 8)    |                      \
  (((uint64)(x) & 0x000000FF00000000ULL) >> 8)    |                      \
  (((uint64)(x) & 0x0000FF0000000000ULL) >> 24)   |                      \
  (((uint64)(x) & 0x00FF000000000000ULL) >> 40)   |                      \
  (((uint64)(x) & 0xFF00000000000000ULL) >> 56)))

/*===========================================================================

MACRO SEC_HTONS()

DESCRIPTION
 Converts host-to-network short integer.  Handles potential byte order
 differences between different computer architectures and different network
 protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572, ps_htons) The bit shifting is correct*/
#define sec_htons(x)                                                      \
  (((((uint16)(x) & 0x00FF) << 8) | (((uint16)(x) & 0xFF00) >> 8)))

/*===========================================================================

MACRO SEC_NTOHL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572, ps_ntohl) The bit shifting is correct*/
#define sec_ntohl(x)                                                      \
  (((((uint32)(x) & 0x000000FFU) << 24) |                                \
  (((uint32)(x) & 0x0000FF00U) <<  8) |                                  \
  (((uint32)(x) & 0x00FF0000U) >>  8) |                                  \
  (((uint32)(x) & 0xFF000000U) >> 24)))

/*===========================================================================

MACRO SEC_NTOHLL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572, ps_ntohll) The bit shifting is correct*/
#define sec_ntohll(x)                                                     \
  (((((uint64)(x) & 0x00000000000000FFULL) << 56)   |                    \
  (((uint64)(x) & 0x000000000000FF00ULL) << 40)   |                      \
  (((uint64)(x) & 0x0000000000FF0000ULL) << 24)   |                      \
  (((uint64)(x) & 0x00000000FF000000ULL) << 8)    |                      \
  (((uint64)(x) & 0x000000FF00000000ULL) >> 8)    |                      \
  (((uint64)(x) & 0x0000FF0000000000ULL) >> 24)   |                      \
  (((uint64)(x) & 0x00FF000000000000ULL) >> 40)   |                      \
  (((uint64)(x) & 0xFF00000000000000ULL) >> 56)))

/*===========================================================================

MACRO SEC_NTOHS()

DESCRIPTION
 Converts network-to-host short integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572, ps_ntohs) The bit shifting is correct*/
#define sec_ntohs(x)                                                      \
  (((((uint16)(x) & 0x00FF) << 8) | (((uint16)(x) & 0xFF00) >> 8)))

#endif /* SECINET_H */

