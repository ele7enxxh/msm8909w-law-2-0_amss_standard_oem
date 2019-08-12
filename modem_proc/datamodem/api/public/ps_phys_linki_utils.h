#ifndef PS_PHYS_LINKI_UTILS_H
#define PS_PHYS_LINKI_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   P S _ P H Y S _ L I N K I _ U T I L S . H

DESCRIPTION
  Header file defining internal macros used by PS to manipulate a phys link.
  As validation is not performed what so ever, all input parameters are
  assumed to be valid.

EXTERNAL FUNCTIONS
  None

Copyright (c) 2005-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_phys_linki_utils.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/09    pp     CMI De-featurization.
12/14/08    pp     Common Modem Interface: Public/Private API split.
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
05/02/05    msr    Wrapped macros in parenthesis.
04/17/05    msr    Created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                              MACRO DEFINITIONS

===========================================================================*/
/*===========================================================================
MACRO PS_PHYS_LINKI_GET_STATE()

DESCRIPTION
  This macro returns the state of the phys link that was passed in.

PARAMETERS
  phys_link_ptr : pointer to the phys link whose state is requested

RETURN VALUE
  the state of the passed in phys link.
===========================================================================*/
#define PS_PHYS_LINKI_GET_STATE(phys_link_ptr)                           \
  ((phys_link_ptr)->phys_private.state)

/*===========================================================================
MACRO PS_PHYS_LINKI_SET_STATE()

DESCRIPTION
  This macro sets the state of the phys link that was passed in

PARAMETERS
  phys_link_ptr : pointer to the phys link whose state needs to be set
  new_state     : phys_link's new state

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINKI_SET_STATE(phys_link_ptr, new_state)                \
  ((phys_link_ptr)->phys_private.state = new_state)

/*===========================================================================
MACRO PS_PHYS_LINKI_GET_INST()

DESCRIPTION
  Returns a phys link's instance

PARAMETERS
  phys link_ptr : pointer to the phys link

RETURN VALUE
  phys link's instance
===========================================================================*/
#define PS_PHYS_LINKI_GET_INST(phys_link_ptr)                            \
  ((phys_link_ptr)->phys_private.instance)

/*===========================================================================
MACRO PS_PHYS_LINKI_FLOW_ENABLED()

DESCRIPTION
  Checks whether the phys link is flow enabled.

PARAMETERS
  phys_link_ptr : pointer to the phys link

RETURN VALUE
  TRUE  : if flow is enabled
  FALSE : otherwise
===========================================================================*/
#define PS_PHYS_LINKI_FLOW_ENABLED(phys_link_ptr)                        \
  ((phys_link_ptr)->phys_private.tx_flow_mask == ALL_FLOWS_ENABLED)

/*===========================================================================
MACRO PS_PHYS_LINKI_GET_REF_CNT()

DESCRIPTION
  Returns the number of flows associated with a phys link.

PARAMETERS
  phys_link_ptr : pointer to phys link

RETURN VALUE
  Number of flows
===========================================================================*/
#define PS_PHYS_LINKI_GET_REF_CNT(phys_link_ptr)                         \
  ((phys_link_ptr)->phys_private.ref_cnt)

/*===========================================================================
MACRO PS_PHYS_LINKI_INC_REF_CNT()

DESCRIPTION
  Increments number of flows associated with a phys link

PARAMETERS
  phys_link_ptr : pointer to phys link

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINKI_INC_REF_CNT(phys_link_ptr)                         \
  ((phys_link_ptr)->phys_private.ref_cnt++)

/*===========================================================================
MACRO PS_PHYS_LINKI_DEC_REF_CNT()

DESCRIPTION
  Decrements number of flows associated with a phys link

PARAMETERS
  phys_link_ptr : pointer to phys link

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINKI_DEC_REF_CNT(phys_link_ptr)                         \
  ((phys_link_ptr)->phys_private.ref_cnt--)

/*===========================================================================
MACRO PS_PHYS_LINKI_SET_CAPABILITY()

DESCRIPTION
  Sets a capability of a phys link.

  NOTE : Only one capability can be set each time.

PARAMETERS
  phys_link_ptr : pointer to phys link
  capability    : capability which needs to be set

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINKI_SET_CAPABILITY(phys_link_ptr, capability)          \
  ((phys_link_ptr)->phys_private.capability_mask |= (0x1 << (uint8)(capability)))

/*===========================================================================
MACRO PS_PHYS_LINKI_RESET_CAPABILITY()

DESCRIPTION
  Resets a capability of a phys link.

  NOTE : Only one capability can be reset each time.

PARAMETERS
  phys_link_ptr : pointer to phys link
  capability    : capability which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINKI_RESET_CAPABILITY(phys_link_ptr, capability)        \
  ((phys_link_ptr)->phys_private.capability_mask &= ~(0x1 << (uint8)(capability)))

/*===========================================================================
MACRO PS_PHYS_LINKI_GET_CAPABILITY()

DESCRIPTION
  Returns TRUE if a capability of a phys link is turned on.

  NOTE : Only one capability can be fetched each time.

PARAMETERS
  phys_link_ptr : pointer to phys link
  capability    : capability which needs to be tested

RETURN VALUE
  TRUE  : if capability is turned on
  FALSE : otherwise
===========================================================================*/
#define PS_PHYS_LINKI_GET_CAPABILITY(phys_link_ptr, capability)          \
  ((phys_link_ptr)->phys_private.capability_mask & (0x1 << (uint8)(capability)))

#endif /* PS_PHYS_LINKI_UTILS_H */
