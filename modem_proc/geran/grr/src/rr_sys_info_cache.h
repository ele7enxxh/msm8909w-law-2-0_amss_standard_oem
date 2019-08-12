#ifndef __RR_SI_CACHE__
#define __RR_SI_CACHE__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR System Information Cache Module

GENERAL DESCRIPTION
   This module contains functions to handle system information message processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 by QUALCOMM Technologies, Inc. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_cache.h#1 $ 
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
20/01/2014  SJW     Added blacklisting interface
13/05/2013  SJW     Ported to QDSP6
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "sys_type.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "rr_l2_l1.h"
#include "rr_l1.h"
#include "rr_l2.h"
#include "rr_gprs_defs.h"

/* Only 8 SIs are cached currently. This is specifically used for QXDM logging */
#define MAX_CSI_PER_CGI 10

#define CACHED_SYS_INFO_TC 0xCC

typedef struct
{
  uint8   msg_type;
  ARFCN_T si_arfcn;
  rr_csi_cgi_t cgi;
  uint8  *octet_ptr;
  uint16  counter; /*indicates order in which entries are added*/
} csi_entry_t;

typedef enum
{
  CSI_FLUSH_CACHE_FULL = 0,
  CSI_FLUSH_ARFCN_CHANGE,
  CSI_FLUSH_SI_CHANGE,
  CSI_FLUSH_CACHE_INIT,
  CSI_FLUSH_CONTENTION_FAILURE,
  CSI_FLUSH_DIFF_CELL_ON_ARFCN,
  CSI_FLUSH_NO_CAUSE = 255
} rr_si_cache_flush_cause_T;

typedef enum
{
  CSI_ENTRY_UPDATED = 0,
  CSI_ENTRY_ADDED,
  CSI_ENTRY_NO_STATUS = 255
} rr_si_cache_update_status_T;

typedef enum
{
  CSI_RETRIEVE_FOR_FAST_ACQUISITION = 0,
  CSI_RETRIEVE_ALL_SIS
} rr_si_cache_SIs_to_retrieve_T;

extern void rr_csi_init_cache(void);
extern void rr_csi_flush_data(LAI_T   *lai_ptr,
                                uint16  *ci_ptr,
                                const rr_si_cache_flush_cause_T csi_flush_cause,
                                const gas_id_t gas_id);

extern void rr_csi_flush_arfcn_data(const ARFCN_T arfcn,
                                    const rr_si_cache_flush_cause_T csi_flush_cause,
                                    const gas_id_t gas_id);

extern boolean rr_csi_update_entry(uint8            msg_type,
                                   ARFCN_T             arfcn,
                                   dl_unit_data_ind_T *msg_ptr,
                                   LAI_T              *lai_ptr,
                                   uint16             *ci_ptr,
                                   const gas_id_t      gas_id);

extern rr_si_cache_SIs_to_retrieve_T rr_csi_is_retrieval_for_fast_acquisition(const gas_id_t gas_id);

extern uint16 rr_csi_retrieve_all_SIs(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr, rr_si_cache_SIs_to_retrieve_T SIs_to_retrieve, const gas_id_t gas_id);

extern boolean rr_csi_retrieve_SI3_4(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr, const gas_id_t gas_id);

extern void rr_csi_copy_pend_store_to_cache(ARFCN_T             arfcn,
                                            LAI_T              *lai_ptr,
                                            uint16             *ci_ptr,
                                            const gas_id_t      gas_id);

extern void rr_csi_deactivate_pend_store(const gas_id_t gas_id);

extern void rr_csi_activate_pend_store(const gas_id_t gas_id);

extern void rr_csi_get_si_index(csi_entry_t *log_entry_ptr,uint8 *si_index);

extern void rr_csi_get_si_cache_summary(csi_entry_t *log_entry_ptr,
  uint8     *si_length,
  uint8     *cached_si_list,
  uint16    *SI_2quater_indices,
  uint16    *SI_2ter_indices);

#endif /*  __RR_SI_CACHE__ */
