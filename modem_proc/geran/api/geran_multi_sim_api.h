/**
  @file geran_multi_sim_api.h

  @brief Defines Multi-SIM functions available to software outside GERAN.
*/

/*============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies, Inc.

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/geran_multi_sim_api.h#1 $ */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/14   mko     Creating the GERAN API to provide the GL1 queue id
31/05/14   sjw     Initial revision
===========================================================================*/

#ifndef GERAN_MULTISIM_API_H
#define GERAN_MULTISIM_API_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "sys_type.h"
#include "gs.h"

typedef enum
{
  GERAN_API_CLIENT_EDTM,
  GERAN_API_CLIENT_NACC,
  GERAN_API_CLIENT_GMM,
  GERAN_API_CLIENT_PDP,
  GERAN_API_CLIENT_RLC,
  GERAN_API_CLIENT_GL1,
  GERAN_API_CLIENT_G2X,
  GERAN_API_CLIENT_CMUI,   /* for use by Call Manager or User Interface */
  GERAN_API_CLIENT_RESET,  /* used by panic reset mechanisms */
  GERAN_API_CLIENT_UNKNOWN /* last entry, do not add new clients after this line */
} geran_api_client_id_t;

extern boolean geran_api_get_tuneaway_blocked(const sys_modem_as_id_e_type);
extern void    geran_api_set_tuneaway_blocked(const geran_api_client_id_t, const boolean, const sys_modem_as_id_e_type);
extern gs_queue_id_T geran_api_get_l1_queue_id(const sys_modem_as_id_e_type);

#endif /* GERAN_MULTISIM_API_H */

