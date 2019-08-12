#ifndef mm_lsm_h
#define mm_lsm_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2004 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mm_lsm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/04   cd      LCS support. Initial version
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
09/02/04   cd      Removed references to lcs_abort_ind. These are now included 
                   in a LSM file.
03/13/06   abe     Added new type RESET_UE_POSITIONING_STORED_INFORMATION
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sys_stru.h"
#include "sys_type.h"
#include "sys_cnst.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*****************************************************/
/* The following enumeration defines the message ids */
/* of the messages in transit between MM and LSM     */
/*****************************************************/
typedef enum
{ 
   /* MM<-LSM primitives */
   LCS_ACTIVE_IND                  = 0x01
} mm_lsm_message_id_enum_T;



/*************************************************************/
/* LCS_ACTIVE_IND primitive                                  */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Indicates the start or end of an LCS session in LSM       */
/*************************************************************/
typedef struct
{
  IMH_T                    message_header;
  boolean                  lcs_in_progress;
} lcs_active_ind_T;

/* Added new type for handling UE POSITIONING INFO */
#define RESET_UE_POSITIONING_STORED_INFORMATION 0x48




typedef enum 
{
  MM_CGPS_MSG_EVENT_ABORT,
  MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO
} mm_cgps_event_e_type;


typedef struct
{
  uint8*          p_data;
  uint32          length;
} mm_cgps_msg_data_s_type;

typedef union /* for possible future expansion */
{
  mm_cgps_msg_data_s_type    z_mm_cgps_msg_data;
} mm_cgps_event_data_union_type;

typedef struct
{
  mm_cgps_event_e_type            e_event_type;
  mm_cgps_event_data_union_type   u;
} mm_cgps_event_data_s_type;

typedef boolean (* mm_cgps_event_cb_type ) ( mm_cgps_event_data_s_type* event_data_ptr );



/* defined in mm_lsm.c, for CGPS_UMTS_CP to register the MM event call back function */
boolean mm_register_cgps_event_cb ( mm_cgps_event_cb_type event_cb_fp );









#endif // mm_lsm_h
