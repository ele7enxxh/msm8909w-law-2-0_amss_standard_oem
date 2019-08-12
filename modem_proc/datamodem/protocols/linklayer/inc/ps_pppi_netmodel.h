#ifndef PS_PPPI_NETMODEL_H
#define PS_PPPI_NETMODEL_H
/*===========================================================================

                      P S _ P P P I _ N E T M O D E L . H

DESCRIPTION

 The PPP Network model internal header file.

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.h_v   1.3   15 Nov 2002 23:37:30   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi_netmodel.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/09/09   mga     Merged Compiler warnings fix
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort. [Split from ps_ppp_netmodel.h]
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_defs.h"
#include "dsm.h"
#include "ps_ppp_snoop.h"


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_STA_PPP
/*---------------------------------------------------------------------------
  Enum for netmdl_getconst
---------------------------------------------------------------------------*/
typedef enum
{
  NETMDL_CONST_META_STATE,
  NETMDL_CONST_MAX_PARAM                  /* max value - array sizing      */
} netmdl_const_enum_type;
#endif /* FEATURE_STA_PPP */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION NETMDL_RESET_PPP()

DESCRIPTION
  This function will spoof Term Requests over both the Um adn Rm interfaces.

PARAMETERS
  kill_ppp: if this is TRUE tear down the call, otherwise spoof a C-Req to
  reset the connection.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_reset_ppp
(
  boolean kill_ppp
);

/*===========================================================================
  FUNCTION NETMDL_COMPARE_LCP_OPTS()

  DESCRIPTION
    Compares the LCP options on the Rm and Um.

  PARAMETERS
    None

  RETURN VALUE
    TRUE - Rm and Um LCP options match
    FALSE - Rm and Um LCP options differ

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean netmdl_compare_lcp_opts
(
  void
);

#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
  FUNCTION NETMDL_COMPARE_ICPC_OPTS()

  DESCRIPTION
    Compares the ICPC options on the Rm and Um.

  PARAMETERS
    TRUE - Rm and Um ICPC options match
    FALSE - Rm and Um ICPC options differ

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean netmdl_compare_ipcp_opts
(
  void
);


#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION NETMDL_GETCONST()

DESCRIPTION
  This function returns netmodel-specific constants for use in the PPP STA.

PARAMETERS
  netmdl_enum  Enum value to select which constant is returned.

RETURN VALUE
  The desired constant.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int netmdl_getconst
(
  netmdl_const_enum_type netmdl_enum
);

#endif /* FEATURE_STA_PPP */
#endif /* PS_PPPI_NETMODEL_H */
