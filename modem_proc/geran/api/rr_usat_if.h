#ifndef RR_USAT_IF_H
#define RR_USAT_IF_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_usat_if.h

GENERAL DESCRIPTION
   This module contains functions accessed by usat

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_usat_if.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
19/10/10   SJW     Added support for FEATURE_DUAL_SIM
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
12/12/02   JAC     started
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_cnst.h"
#include "timers.h"
#include "geran_dual_sim.h"
#include "sys.h"

#define RR_USAT_SIZE_BCCH_LIST ((BA_MAX_LEN*10)/8+1)

typedef struct
{
  boolean timing_advance_valid;
  byte  timing_advance;
} geran_grr_current_timing_advance_type;

#define current_timing_advance_T geran_grr_current_timing_advance_type

typedef struct
{
  uint8 length;
  uint8 bcch_list[RR_USAT_SIZE_BCCH_LIST];
} rr_bcch_list_type;

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



boolean rr_get_nmr_bcch(uint8 *nmr, rr_bcch_list_type *bcch_list);
boolean rr_api_get_nmr_bcch(uint8 *nmr, uint8 len_of_nmr, rr_bcch_list_type *bcch_list);
boolean rr_get_cell_id(uint8 *cell_id);
boolean rr_get_timing_advance(uint8 *me_status, uint8 *timing_advance);

boolean rr_ds_get_nmr_bcch(uint8 *nmr, 
                           rr_bcch_list_type *bcch_list,
                           sys_modem_as_id_e_type as_id);

boolean rr_ds_api_get_nmr_bcch(uint8 *nmr, 
                           uint8 len_of_nmr,
                           rr_bcch_list_type *bcch_list,
                           sys_modem_as_id_e_type as_id);

boolean rr_ds_get_cell_id(uint8                  *cell_id,
                          sys_modem_as_id_e_type  as_id);
boolean rr_ds_get_timing_advance(uint8                  *me_status, 
                                 uint8                  *timing_advance,
                                 sys_modem_as_id_e_type  as_id);

#endif

