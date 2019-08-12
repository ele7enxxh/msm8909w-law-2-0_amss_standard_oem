/**
  @file geran_gl1_api.h

  @brief Defines GL1 functions available to software outside GERAN.
*/

/*============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.

  Qualcomm Proprietary

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

============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_gl1_api.h#1 $ */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   -------- ---------------------------------------------
28/05/15   akh      Initial revision
===========================================================================*/

#ifndef GERAN_GL1_API_H
#define GERAN_GL1_API_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "ds3g_ext.h"
#include "sys.h"

/*===========================================================================

FUNCTION  gpl1_config_modem_power_stats_tx

DESCRIPTION
Enables/disables capturing of tx power stats

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void gpl1_config_modem_power_stats_tx(boolean en_pwr_stats, sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION  gpl1_get_modem_power_stats_tx

DESCRIPTION
Fetches the tx power stats and clears old stats

RETURN VALUE
Tx power stats in ms

SIDE EFFECTS
None

===========================================================================*/
void gpl1_get_modem_power_stats_tx(uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX], sys_modem_as_id_e_type as_id);

#endif /* GERAN_GL1_API_H */

