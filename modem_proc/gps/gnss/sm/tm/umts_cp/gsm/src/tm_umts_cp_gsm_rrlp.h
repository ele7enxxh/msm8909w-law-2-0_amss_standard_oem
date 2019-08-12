/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  G S M   C P   R R L P   M O D U L E

GENERAL DESCRIPTION
  This header file contains prototypes, data structures, etc. used
  in the process of decoding and encoding RRLP mesages.


  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/gps/ulp/tm/umts_cp/gsm/rel/08.21_OSYS/src/tm_umts_cp_gsm_rrlp.h#3 
  $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

===========================================================================*/

#include "customer.h"

#include "gps_variation.h"
#ifdef FEATURE_CGPS_UMTS_CP_GSM

#include "sm_api.h"
#include "tm_umts_common_utils.h"
#include "tm_rrlp_asn1.h"


void tm_umts_cp_gsm_parse_rrlp
( 
  uint8   *p_rrlp,
  uint16  w_length 
);

void tm_umts_cp_gsm_handle_prm_data 
( 
  sm_GpsMeasRptStructType *p_prm_data 
);

void tm_umts_cp_gsm_send_pos_rrlp
(
  gad_ellip_alt_unc_ellip_type  *p_gad_shape_pos,
  rrlp_FixType                  fix_type,
  uint32                        q_timestamp_ms 
);


void tm_umts_cp_gsm_post_cached_acq_assist
(  
  void
);


void tm_umts_cp_gsm_rrlp_send_cached_gps_meas
(
  void
);

#endif  /* FEATURE_CGPS_UMTS_CP_GSM */

