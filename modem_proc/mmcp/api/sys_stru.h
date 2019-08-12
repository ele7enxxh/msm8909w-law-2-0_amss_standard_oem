#ifndef sys_stru_h
#define sys_stru_h
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_stru.h_v   1.14   18 Jul 2002 11:42:26   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_stru.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
#include "comdef.h"
#include "nas_exp.h"
#include "sys_plmn.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define MAX_MESSAGE_DATA_LEN           MAX_OCTETS_L3_MESSAGE
/* 
** Spec. 24.008, sec - 10.5.1.7 (Rel - 7 and 8)
** Size of Classmark 3 is increased to 15 octets.
*/
#define MAX_CLASSMARK3_DATA_SIZE     34

/*
 * Message header for used for CM peer messages
 */

typedef struct msg_head_tag
{
   IMH_T IMH;              /* Intertask Message Header                    */
   byte  ti_pd;            /* Transaction identifier, protocol discrim.   */
   byte  type;             /* Message type                                */
} msg_head_T;

typedef enum
{
   NETWORK_OP_MODE_1,
   NETWORK_OP_MODE_2,
   NETWORK_OP_MODE_3,
   PS_DOMAIN_SYS_INFO_UNAVAILABLE
} network_op_mode_type;



/*
 * Definition of LAI for use in intertask messages - Definition consists
 * entirely of bytes
 */

typedef struct
{
  PLMN_id_T PLMN_id;
  byte location_area_code[2];
} inter_task_lai_T;



typedef byte routing_area_code_T;



typedef struct
{
  network_op_mode_type  nmo;
  routing_area_code_T   rac;
  boolean               sgsnr;

  boolean               dtm_supported;

  boolean               egprs_supported;
} mm_gprs_sys_info_T;



/*********************************************************************
 * mobile station classmark 2 IE
 */

typedef struct classmark_2_ie_tag
{
   byte        length;
   byte        data[3];
} classmark_2_ie_T;

/*
** Spec. 24.008, sec - 10.5.1.7 (Rel - 5)
** Size of Classmark 3 is increased to 14 octets.
*/

typedef struct classmark_3_ie_tag
{
   byte        length;
   byte        data[MAX_CLASSMARK3_DATA_SIZE];
} classmark_3_ie_T;

typedef struct
{
   byte lac[2];
} location_area_code_T;

typedef struct
{
   PLMN_id_T            plmn_id;
   location_area_code_T location_area_code;
   routing_area_code_T  routing_area_code;
} routing_area_id_T;

#define GET_IMH_LEN(len,IMHP) \
        len = (((IMH_T *)IMHP)->message_len_lsb \
    + (0x100 * ((IMH_T *)IMHP)->message_len_msb));

#define PUT_IMH_LEN(len,IMHP) \
        ((IMH_T *)IMHP)->message_len_msb = (unsigned char)((len) / 0x100), \
        ((IMH_T *)IMHP)->message_len_lsb = (unsigned char)((len) % 0x100);



typedef struct 
{
  word length;
  byte value[52];
} gmm_ie_LV_ms_radio_access_caps_T; 


extern gmm_ie_LV_ms_radio_access_caps_T get_ms_radio_access_capability
(
  boolean gprs_pkt_res_req_ms_ra_cap
);

#endif /* sys_stru_h */
