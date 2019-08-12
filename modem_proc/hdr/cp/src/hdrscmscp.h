#ifndef HDRSCMSCP_H
#define HDRSCMSCP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

        S E S S I O N   C O N F I G U R A T I O N    P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
  Negotiation for Restor Prior Session Attribute of SessionConfigurationProtocol
    
EXTERNALIZED FUNCTIONS
    
  hdrscmscp_gaup_prior_session

  hdrscmscp_get_gauped_complex_attrib_value
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005,2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/07   wsh     Created Module for prior session

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#ifdef FEATURE_HDR_PS_GAUP
#error code not present
#endif /* FEATURE_HDR_PS_GAUP */
#endif /* HDRSCMSCP_H */
