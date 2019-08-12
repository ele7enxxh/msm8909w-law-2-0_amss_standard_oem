#ifndef LIB_PROT_H
#define LIB_PROT_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/libprot.h_v   1.5   17 Jul 2002 11:52:14   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/libprot.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/01   jca     Added missing includes.
06/11/01   TS      Added GSM build support.
07/10/02   rmc     Prototypes for classmark functions
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "environ.h"
#include "sys_stru.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * Function prototypes
 */

void gs_read_coded_classmark_2(
  classmark_2_ie_T  *classmark_2_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99
);
byte gs_get_classmark_1(boolean gsm_supported, boolean wcdma_in_use, boolean MSCR99);
boolean gs_read_coded_classmark_3(
  classmark_3_ie_T *classmark_3_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99
);

typedef struct
{
  boolean                    gsm_supported;
  boolean                    wcdma_supported;
  boolean                    wcdma_in_use;
  boolean                    MSCR99;

  sys_modem_as_id_e_type     as_id;
  sys_radio_access_tech_e_type  rat;

} classmark_params_T;


void gs_read_coded_classmark_2_per_subs(
  classmark_2_ie_T     *classmark_2_ptr,
  classmark_params_T   *params_ptr
);

boolean gs_read_coded_classmark_3_per_subs(
  classmark_3_ie_T     *classmark_3_ptr,
  classmark_params_T   *params_ptr
);


byte gs_get_classmark_1_with_rat(
  classmark_params_T    *params_ptrs
);

void gs_read_coded_classmark_2_with_rat(
  classmark_2_ie_T     *classmark_2_ptr,
  classmark_params_T   *params_ptr
);

boolean gs_read_coded_classmark_3_with_rat(
  classmark_3_ie_T     *classmark_3_ptr,
  classmark_params_T   *params_ptr
);


typedef struct
{
byte    speech_caps_ms_capability;
boolean speech_caps_v1_supported;
boolean speech_caps_v2_supported;
boolean speech_caps_v3_supported;
boolean speech_caps_v5_supported;
} speech_caps_T;


VOID_FUNC   read_imeisv(byte *);
VOID_FUNC   normalise_mnc(byte * net, byte num_nets);
VOID_FUNC   pack_net_list(byte *      net_list_bytes,
                          PLMN_id_T * net_list_struct,
                          word        num_nets);
VOID_FUNC unpack_net_list(byte *      net_list_bytes,
                          PLMN_id_T * net_list_struct,
                          word        num_nets);

boolean gs_read_coded_classmark_3_sglte(
  classmark_3_ie_T     *classmark_3_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99,
  sys_radio_access_tech_e_type current_rat
);

void gs_read_coded_classmark_2_sglte(
  classmark_2_ie_T *classmark_2_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported,
  boolean           wcdma_in_use,
  boolean           MSCR99,
  sys_radio_access_tech_e_type current_rat
);

byte gs_get_classmark_1_sglte(
boolean gsm_supported, 
boolean wcdma_in_use, 
boolean MSCR99,
sys_radio_access_tech_e_type current_rat
);
#endif
