#ifndef RR_IRAT_OVERLAP_DETECT_H
#define RR_IRAT_OVERLAP_DETECT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_irat_overlap_detect.h

GENERAL DESCRIPTION
   This module contains functions for handling overlap detection logic for LTE EARFCN's and WCDMA UARFCNs.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_irat_overlap_detect.h#1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_LTE
void rr_detect_and_remove_overlap_earfcn(
	   mph_wcdma_cell_update_list_req_T* idle_msg_ptr, 
	   mph_dedicated_wcdma_cell_update_list_req_T* ded_msg_ptr, 
	   const gas_id_t gas_id );
#endif

#ifdef FEATURE_WCDMA
void rr_detect_and_remove_overlap_uarfcn_in_idle( 
	   mph_wcdma_cell_update_list_req_T* idle_msg_ptr, 
	   const gas_id_t gas_id );


void rr_detect_and_remove_overlap_uarfcn_in_dedicated( 
	   mph_dedicated_wcdma_cell_update_list_req_T* ded_msg_ptr, 
	   const gas_id_t gas_id );

#endif


#endif
