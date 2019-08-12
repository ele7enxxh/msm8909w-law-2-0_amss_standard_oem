#ifndef MMGSDI_CPHS_H
#define MMGSDI_CPHS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

       C O M M O N   P C N   H A N D S E T   S P E C I F I C A T I O N S

                   ( C P H S )   P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the Common PCN Handset Specifications (CPHS)
  Function Prototypes.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2005,2007-2009,2011-2012 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or
software is regulated by the U.S. Government. Diversion contrary to U.S. law
prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_cphs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/12   bcho    mmgsdi_proc_cphs_post_pin1_init() modified
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Added support for Get Operator Name API
02/20/09   ssr     Removed CSP file offset macro
11/14/08   ssr     Fixed parsing of value added service group
02/21/08   sun     Added new parameters for CPHS Read
01/08/07   sp      Updated GSDI_CPHS_CSP_NUM_IDENTIFY_OFFSET value from
                   0xFF to 0x0F.
05/11/05   sst     Lint fixes
12/27/04   jk      Fixed Compiler Warnings
09/22/04   jar     Added CPHS Post PIN1 Initialization procedures and
                   #defines for use in CPHS Procedures.
09/07/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================*/

#include "mmgsdi.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ----------------------------------------------------------------------------
   CPHS Information EF-CPHSI as organized in the File
   --------------------------------------------------------------------------*/
#define MMGSDI_CPHS_INFO_PHASE_OFFSET          0x00
#define MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2    0x01
#define MMGSDI_CPHS_INFO_SERVICES_OFFSET_B3    0x02

/* ----------------------------------------------------------------------------
  CPHS Information EF-CPHSI Masks
  ---------------------------------------------------------------------------*/
#define MMGSDI_CPHS_INFO_CSP_MASK              0x03
#define MMGSDI_CPHS_INFO_SST_MASK              0x0C
#define MMGSDI_CPHS_INFO_MBN_MASK              0x30
#define MMGSDI_CPHS_INFO_OP_NAME_MASK          0xC0
#define MMGSDI_CPHS_INFO_INFO_NUM_MASK         0x03

/* ----------------------------------------------------------------------------
   CPHS Service Group Codes as defined for EF-CSP
   --------------------------------------------------------------------------*/
#define MMGSDI_CPHS_CSP_SVC_GROUP_1            0x01
#define MMGSDI_CPHS_CSP_SVC_GROUP_2            0x02
#define MMGSDI_CPHS_CSP_SVC_GROUP_3            0x03
#define MMGSDI_CPHS_CSP_SVC_GROUP_4            0x04
#define MMGSDI_CPHS_CSP_SVC_GROUP_5            0x05
#define MMGSDI_CPHS_CSP_SVC_GROUP_6            0x06
#define MMGSDI_CPHS_CSP_SVC_GROUP_7            0x07
#define MMGSDI_CPHS_CSP_SVC_GROUP_8            0x08
#define MMGSDI_CPHS_CSP_SVC_GROUP_9            0x09
#define MMGSDI_CPHS_CSP_SVC_GROUP_C0           0xC0
#define MMGSDI_CPHS_CSP_SVC_GROUP_D5           0xD5

/* ----------------------------------------------------------------------------
   CPHS Service Group Offset as defined for EF-CSP
   --------------------------------------------------------------------------*/
#define MMGSDI_CPHS_CSP_SVC_GRP1_OFFSET        0x00
#define MMGSDI_CPHS_CSP_SVC_GRP2_OFFSET        0x02
#define MMGSDI_CPHS_CSP_SVC_GRP3_OFFSET        0x04
#define MMGSDI_CPHS_CSP_SVC_GRP4_OFFSET        0x06
#define MMGSDI_CPHS_CSP_SVC_GRP5_OFFSET        0x08
#define MMGSDI_CPHS_CSP_SVC_GRP6_OFFSET        0x0A
#define MMGSDI_CPHS_CSP_SVC_GRP7_OFFSET        0x0C
#define MMGSDI_CPHS_CSP_SVC_GRP8_OFFSET        0x0E
#define MMGSDI_CPHS_CSP_SVC_GRP9_OFFSET        0x10
#define MMGSDI_CPHS_CSP_SVC_GRPC0_OFFSET       0x12
#define MMGSDI_CPHS_CSP_SVC_GRPD0_OFFSET       0x14

/* ----------------------------------------------------------------------------
   Customer Service Profile Service Masks
   --------------------------------------------------------------------------*/
/* Byte 1 */
#define MMGSDI_CPHS_CSP_CFU_MASK               0x80
#define MMGSDI_CPHS_CSP_CFB_MASK               0x40
#define MMGSDI_CPHS_CSP_CFNRy_MASK             0x20
#define MMGSDI_CPHS_CSP_CFNRc_MASK             0x10
#define MMGSDI_CPHS_CSP_CT_MASK                0x08
/* Byte 2 */
#define MMGSDI_CPHS_CSP_BOAC_MASK              0x80
#define MMGSDI_CPHS_CSP_BOIC_MASK              0x40
#define MMGSDI_CPHS_CSP_BOICexHC_MASK          0x20
#define MMGSDI_CPHS_CSP_BAIC_MASK              0x10
#define MMGSDI_CPHS_CSP_BIC_ROAM_MASK          0x08
/* Byte 3 */
#define MMGSDI_CPHS_CSP_MPTY_MASK              0x80
#define MMGSDI_CPHS_CSP_CUG_MASK               0x40
#define MMGSDI_CPHS_CSP_AOC_MASK               0x20
#define MMGSDI_CPHS_CSP_PREF_CUG_MASK          0x10
#define MMGSDI_CPHS_CSP_CUG_OA_MASK            0x08
/* Byte 4 */
#define MMGSDI_CPHS_CSP_HOLD_MASK              0x80
#define MMGSDI_CPHS_CSP_CW_MASK                0x40
#define MMGSDI_CPHS_CSP_CCBS_MASK              0x20
#define MMGSDI_CPHS_CSP_USER_SIG_MASK          0x10
/* Byte 5 */
#define MMGSDI_CPHS_CSP_SM_MT_MASK             0x80
#define MMGSDI_CPHS_CSP_SM_MO_MASK             0x40
#define MMGSDI_CPHS_CSP_SM_CB_MASK             0x20
#define MMGSDI_CPHS_CSP_REPLY_PATH_MASK        0x10
#define MMGSDI_CPHS_CSP_DEL_CONF_MASK          0x08
#define MMGSDI_CPHS_CSP_PROTOCOL_ID_MASK       0x04
#define MMGSDI_CPHS_CSP_VALIDITY_PER_MASK      0x02
/* Byte 6 */
#define MMGSDI_CPHS_CSP_ALS_MASK               0x80
/* Byte 7 */
#define MMGSDI_CPHS_CSP_RSVD_SST_PHASE_1_MASK  0x80
/* Byte 8 */
#define MMGSDI_CPHS_CSP_CLIP_MASK              0x80
#define MMGSDI_CPHS_CSP_CoLR_MASK              0x20
#define MMGSDI_CPHS_CSP_CoLP_MASK              0x10
#define MMGSDI_CPHS_CSP_MCI_MASK               0x08
#define MMGSDI_CPHS_CSP_CLI_SEND_MASK          0x02
#define MMGSDI_CPHS_CSP_CLI_BLOCK_MASK         0x01
/* Byte 9 */
#define MMGSDI_CPHS_CSP_GPRS_MASK              0x80
#define MMGSDI_CPHS_CSP_HSCSD_MASK             0x40
#define MMGSDI_CPHS_CSP_VOICE_GRP_CALL_MASK    0x20
#define MMGSDI_CPHS_CSP_VOICE_BRDCST_SVC_MASK  0x10
#define MMGSDI_CPHS_CSP_MULTI_SUB_PROFILE_MASK 0x08
#define MMGSDI_CPHS_CSP_MULTI_BAND_MASK        0x04
/* Byte 10 */
#define MMGSDI_CPHS_CSP_PLMN_MODE_MASK         0x80
#define MMGSDI_CPHS_CSP_VPS_MASK               0x40
#define MMGSDI_CPHS_CSP_SM_MO_PAGING_MASK      0x20
#define MMGSDI_CPHS_CSP_SM_MO_EMAIL_MASK       0x10
#define MMGSDI_CPHS_CSP_FAX_MASK               0x08
#define MMGSDI_CPHS_CSP_DATA_MASK              0x04
#define MMGSDI_CPHS_CSP_LANGUAGE_MASK          0x01
/* Byte 11 */
#define MMGSDI_CPHS_CSP_INFO_NUMBERS_MASK      0xFF


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
  STRUCTURE  MMGSDI_CPHS_READ_CPHS_EFS_INFO_TYPE

  DESCRIPTION:
    This structure contains flags to read cphs related uim files
------------------------------------------------------------------------------*/

typedef struct{
  boolean         read_cphsi;
  boolean         read_csp;
  boolean         read_ons;
  boolean         read_ons_short;
} mmgsdi_cphs_read_cphs_efs_info_type ;


/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/


/*===========================================================================
FUNCTION MMGSDI_PROC_CPHS_POST_PIN1_INIT

DESCRIPTION
  This function will run the CPHS Intialization procedures.  The purpose
  of this function is to cache the relevant CPHS Information for use
  by other Tas

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_proc_cphs_post_pin1_init(
  mmgsdi_session_id_type               session_id,
  mmgsdi_slot_id_enum_type             slot,
  mmgsdi_app_enum_type                 app_type,
  mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs
);

#endif /* MMGSDI_CPHS_H */
