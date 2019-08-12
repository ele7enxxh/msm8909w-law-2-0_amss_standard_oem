#ifndef mm_iei_h
#define mm_iei_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_iei.h_v   1.3   22 Aug 2001 10:46:26   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/mm_iei.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/01   ab      Renamed CM to CNM.
08/22/01   jca     Removed errant CNM renames.
11/14/02   mks     Added definitions for IEs in MM_INFORMATION message
04/03/03   cd      Added definition for Equivalent PLMN list IEI
04/15/03   cd      Now including sys.h
06/06/05   abe     Added code for CCBS support
09/16/05   ajt     Added definition IEI_EMERGENCY_NUM_LIST
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "sys.h"
#include "sys_v.h"
#include "mm_v.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/*
 * Local  Defines
 * --------------
 */

#define  IEI_RAND                   0x00      /* value not defined in 4.08   */
#define  IEI_SRES                   0x01      /* value not defined in 4.08   */
#define  IEI_CM_SERVICE_TYPE        0x02      /* value not defined in 4.08   */
#define  IEI_IDENTITY               0x03      /* value not defined in 4.08   */
#define  IEI_LU_TYPE                0x04      /* value not defined in 4.08   */
#define  IEI_REJECT_CAUSE           0x05      /* value not defined in 4.08   */

#ifdef FEATURE_CCBS 
#define IEI_PD_AND_SAPI             0x06      /* value not defined in 24.008 */
#endif 
#define  IEI_FOLLOW_ON_PROCEED      0xa1

#define  IEI_AUTN                   0x20      /* value not defined in 24.008   */

#define  IEI_NETWORK_FULL_NAME      0x43      /* Defined in TS24.008 */
#define  IEI_NETWORK_SHORT_NAME     0x45      /* Defined in TS24.008 */
#define  IEI_TIME_ZONE              0x46      /* Defined in TS24.008 */
#define  IEI_TIME_ZONE_AND_TIME     0x47      /* Defined in TS24.008 */
#define  IEI_LSA_ID                 0x48      /* Defined in TS24.008 */
#define  IEI_DAYLIGHT_SAVING_TIME   0x49      /* Defined in TS24.008 */
#define  IEI_EQUIV_PLMN             0x4a
#define  IEI_EMERGENCY_NUM_LIST     0x34      /* Defined in TS24.008 */

#define IEI_CLASSMARK2_UMTS 0x33
#define IEI_ADDITIONAL_UPDATE_PARAM 0x0c
#define CLASSMARK_2_TLV_SIZE 5
#ifdef FEATURE_NAS_REL10
#define IEI_UL_NW_FTR_SUPPORT       0xE0
#endif



typedef struct ie_autn_tag
{
   byte  iei;
   byte  length;
   byte  autn[16];
} ie_autn_T;



/*
 * RAND IE
 */

typedef struct ie_rand_tag
{
   byte  iei;
   byte  rand[16];
} ie_rand_T;

/*
 * SRES IE
 */

typedef struct ie_sres_tag
{
   byte  iei;
   byte  sres[4];
} ie_sres_T;

/*
 * cm_service_type IE
 */

typedef struct ie_cm_service_type_tag
{
   byte  service_type;
} ie_cm_service_type_T;

/*
 * IDENTITY IE
 */

typedef struct ie_identity_tag
{
   byte  identity;
} ie_identity_T;

/*
 * LU_TYPE IE
 */

typedef struct ie_lu_type_tag
{
   byte  lu_type;
} ie_lu_type_T;

/*
 * REJECT CAUSE IE
 */

typedef struct ie_reject_cause_tag
{
   byte  iei;
   byte  reject_cause_value;
} ie_reject_cause_T;

/*
 * FOLLOW ON PROCEED IE
 */

typedef struct ie_follow_on_proceed_tag
{
   byte  iei;
} ie_follow_on_proceed_T;

typedef struct ie_equiv_plmn_tag
{
   byte                    iei;
   equiv_plmn_list_s_type  equiv_plmn_list;    
}ie_equiv_plmn_T;

#endif
