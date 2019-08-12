#ifndef sys_stru_v_h
#define sys_stru_v_h
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_stru.h_v   1.14   18 Jul 2002 11:42:26   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/sys_stru_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/01   jca     Fixed LINT errors.
05/03/01   jca     Added location_area_code_T and routing_area_id_T.
08/01/01   cd      Removed IMH_T definition and included nas_exp.h
12/06/01   kwa     CR19220 - Modified PLMN_MATCH macro.
02/04/02   kwa     Moved PLMN_id_T to sys_PLMN.h.
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
10/28/02   ks      Added sgsnr indicator in mm_gprs_sys_info_T for Revision
                   level indication in GPRS.
10/22/03   jca     Moved network_op_mode_type from mm.h to sys_stru.h.
                   Added NETWORK_OP_MODE_3 to network_op_mode_type.
                   mm_gprs_sys_info_T now uses network_op_mode_type rather than byte.
07/22/05   up      Change sys_PLMN.h to small case.
09/30/05   up      Modified classmark_3_ie_T to change the size of classmark 3 to 14.
12/07/05   npr     Added FEATURE_GSM_DTM to mm_gprs_sys_info_T
07/16/05   abhi   Fixing klocwork defect
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rex.h"
#include "sys_stru.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * Macro to compare two PLMN ids, taking into account the number of digits
 * in the network code. As currently implemented, plmn_2 is the ID of the
 * PLMN broadcast by the network and plmn_1 is the ID of the PLMN that it
 * is being compared against (e.g., the HPLMN stored on the SIM).  If the
 * third MNC digit in the broadcast PLMN id is 0xF the broadcast PLMN id
 * is assumed to contain a two digit MNC so the third MNC digit is not
 * compared.  Otherwise, all three MNC digits are compared.
 */

#define PLMN_MATCH( plmn_1, plmn_2 ) \
(\
  ( (plmn_1[0] & 0x0F) == (plmn_2[0] & 0x0F) ) && \
  ( (plmn_1[0] / 0x10) == (plmn_2[0] / 0x10) ) && \
  ( (plmn_1[1] & 0x0F) == (plmn_2[1] & 0x0F) ) && \
  ( ( (plmn_2[1] / 0x10) == 0x0F) \
  ? ( \
      ( (plmn_1[2] & 0x0F) == (plmn_2[2] & 0x0F) ) && \
      ( (plmn_1[2] / 0x10) == (plmn_2[2] / 0x10) ) \
    ) \
  : ( \
      ( (plmn_1[2] & 0x0F) == (plmn_2[2] & 0x0F) ) && \
      ( (plmn_1[2] / 0x10) == (plmn_2[2] / 0x10) ) && \
      ( (plmn_1[1] / 0x10) == (plmn_2[1] / 0x10) ) \
    ) \
  ) \
) \

typedef struct
{
   dword  numerator;
   dword  denominator;
} fraction_T;


typedef struct ef_kc_tag
{
   byte     kc[8];
   byte     cipher_key_sequence_number;
} ef_kc_T;

#define NAS_INIT_CRIT_SECT(a)  rex_init_crit_sect(&a)

#define NAS_ENTER_CRIT_SECT(a) rex_enter_crit_sect(&a)
#define NAS_EXIT_CRIT_SECT(a)  rex_leave_crit_sect(&a)

typedef rex_crit_sect_type nas_crit_sect_type;


#endif /* sys_stru_h */
