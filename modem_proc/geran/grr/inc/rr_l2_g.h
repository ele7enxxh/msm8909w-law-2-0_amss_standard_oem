/** @file rr_l2_g.h */

/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2013-2014 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_l2_g.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13/07/01   tjw     New file

===========================================================================*/
#ifndef rr_l2_g_h
#define rr_l2_g_h

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_type.h"
#include "geran_dual_sim.h"

/**********************************************************/
/* DL_DATA_MUX_IND primitive :                            */
/* FROM :  RR                                             */
/* TO   :  L2                                             */
/* DESCRIPTION :                                          */
/* Provides GL2 with a set of callback functions that can */
/* be used for demultiplexing (of CCCH or CBCH blocks)    */
/**********************************************************/
typedef enum
{
  L2_MUX_CCCH,
  L2_MUX_CBCH,
  L2_MUX_NONE = 0xFF
} l2_mux_data_t;

typedef boolean (*l2_mux_cb_fn_t)(const l2_mux_data_t rcvd_type, ARFCN_T channel, const gas_id_t gas_id);

typedef struct
{
  IMH_T          message_header;
  l2_mux_cb_fn_t ccch_callback;
  l2_mux_cb_fn_t cbch_callback;
  gas_id_t       gas_id;
} dl_data_mux_ind_T;


#endif /* rr_l2_g_h */

/* EOF */

