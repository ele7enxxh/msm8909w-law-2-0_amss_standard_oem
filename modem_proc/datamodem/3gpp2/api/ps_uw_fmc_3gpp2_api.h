#ifndef PS_UW_FMC_3GPP2_API_H
#define PS_UW_FMC_3GPP2_API_H
/*===========================================================================
  @file ps_uw_fmc_3gpp2_api.h

  This file defines the external API for UW FMC sub module within 3GPP2

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/api/ps_uw_fmc_3gpp2_api.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                       LOCAL DATA DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_UW_FMC_IFACE_HDLR_INIT()

DESCRIPTION
  This function is called at power up by PS to initialize UW FMC module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_uw_fmc_iface_hdlr_init
(
  void
);

#endif /* PS_UW_FMC_3GPP2_API_H */
