#ifndef IPSEC_IFACE_HDLR_H
#define IPSEC_IFACE_HDLR_H

/*===========================================================================
                    I P S E C _ I F A C E _ H D L R . H

DESCRIPTION
  IPsec IFACE handler functions.

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/ifacectls/inc/ipsec_iface_hdlr.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/26/09    sp     Created module - Moved IPsec Mode Handler to Data
===========================================================================*/

#include "datamodem_variation.h"
#ifdef FEATURE_SEC_IPSEC

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION  IPSEC_IFACE_HDLR_IFACE_INIT

DESCRIPTION
  This function creates and initialises the IPsec Ifaces.
  This is called from the cdc_task() function.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipsec_iface_hdlr_iface_init
(
  void
);

#endif /* FEATURE_SEC_IPSEC */

#endif /* IPSEC_IFACE_HDLR_H */
