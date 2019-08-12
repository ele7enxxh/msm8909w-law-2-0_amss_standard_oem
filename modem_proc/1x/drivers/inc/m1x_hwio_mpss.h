#ifndef M1X_HWIO_MPSS_H
#define M1X_HWIO_MPSS_H
/*===========================================================================

        M O D E M    H A R D W A R E    I N P U T / O U T P U T

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM Modem/MSS/Nav hardware for MDM9x35 chipset.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/m1x_hwio_mpss.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/15   srk     Remove modem names.
08/14/13   cjb     Moved to MCS version of HWIO definitions
08/14/13   cjb     Copied CORE HWIO file for MDM
04/22/13   cjb     Replace MSS_TOP_BASE with MODEM_TOP_BASE
04/22/13   cjb     Same as MDM

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*
 * Basic HWIO definitions.
 */
#include "msmhwio.h"

/*
 * Chipset specific base addresses.
 */
#include "msmhwiobase.h"

/*
 * Modem HWIO definitions from MCS
 */
#include "mcs_hwio_modem.h"

#endif /* M1X_HWIO_MPSS_H */
