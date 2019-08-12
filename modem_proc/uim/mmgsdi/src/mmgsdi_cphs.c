/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

       C O M M O N   P C N   H A N D S E T   S P E C I F I C A T I O N S

                   ( C P H S )   P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the Common PCN Handset Specifications (CPHS)
  procedures.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009, 2011-2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_cphs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
05/22/16   vdc     Remove F3 messages for memory allocation failure
08/29/14   hh      Fix compliation warnings
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv      Remove unused data types
07/15/14   yt      Support for NV refresh
01/10/14   df      Use v2.0 diag macros
07/23/13   tl      Remove read ONS during post pin1 as a dependency on CPHSI
06/24/13   vdc     Added support for triple SIM
05/16/13   vdc     Replace memcpy with safer version memscpy
05/06/13   spo     Modified CPHS variables to array of pointers
04/16/13   tl      Clean up of radio access features from MMGSDI code
03/20/13   tl      Optional CHPS fields only read if activated and allocated
01/04/13   tl      EF-CPS/EF-ONS only read if EF-CHPSI is present
03/16/12   yt      Remove inclusion of tmc.h
01/30/12   bcho    mmgsdi_proc_cphs_post_pin1_init() modified
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Added support for Get Operator Name API
02/18/11   yt      Fixed length check for CPHSI data
09/26/09   kp      ZI memory reduction changes
08/24/09   shr     Fixed compliation errors (for CDMA only)
07/08/09   shr     Fixed Lint warnings
04/29/09   js      Added prototypes for local functions
02/19/09   ssr     Removed comment from CSP data
12/15/08   ssr     Fixed Lint Error
12/08/08   ssr     Fixed parsing of CSP data
11/14/08   ssr     Fixed parsing of value added service group
04/29/08   sun     Added support for invalid 3G configuration
02/21/08   sun     Added new parameters for individual reads
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
07/07/06   sun     Removed Reference to mc.h
05/11/05   sst     Lint fixes
11/15/04   jar     Code clean up to use Debug MACROS
09/23/04   jar     Added retrieval of CPHS Information and parsing of data.
09/07/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

/* System and Interfaces */
#include "uim_msg.h"
#include "err.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"
#include "mmgsdi_cphs.h"
#include "mmgsdiutil.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_nv.h"
#include "uim.h"
#include "intconv.h"
#include "uim_selective_logging.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================
FUNCTION MMGSDI_CPHS_PARSE_CPHSI_DATA

DESCRIPTION
  This function is a local function used to parse the CPHSI Data and store
  the results in gsdi_data (The global Data Structure).

  The information stored in the Global Data Structure will be utilized
  by the CPHS GET INFO API.

INPUT:
  mmgsdi_data_type *  data:  The pointer to the buffer used to store the
                             read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cphs_parse_cphsi_data(
  mmgsdi_data_type              data,
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_cphs_int_info_type *cphs_int_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(data.data_ptr);
  if(data.data_len <= MMGSDI_CPHS_INFO_SERVICES_OFFSET_B3)
  {
    UIM_MSG_ERR_1("CPHSI return data len 0x%x too short", data.data_len);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_cphs_info_ptr(session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(cphs_int_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     CPHS Information was read which indicates that CPHS is supported to some
     degree.  It is up to the (EF-CSP) Customer Service Profile to indicate
     exactly which services are supported.
     Logically, as far as GSDI is concerned, reading CPHS Information is an
     indication that the card is CPHS Capable.
     ------------------------------------------------------------------------*/
  cphs_int_info_ptr->cphs_info.cphs_capable = TRUE;

  /* --------------------------------------------------------------------------
     Store the CPHS Phase Information
     ------------------------------------------------------------------------*/
  cphs_int_info_ptr->cphs_info.cphs_service.phase = data.data_ptr[MMGSDI_CPHS_INFO_PHASE_OFFSET];

  /* --------------------------------------------------------------------------
     Now parse the CPHS Information Data to determine which "Services" are
     allocated and activated
     ------------------------------------------------------------------------*/
  cphs_int_info_ptr->cphs_info.cphs_service.csp =
    (mmgsdi_current_service_enum_type)(data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                     & MMGSDI_CPHS_INFO_CSP_MASK);
  cphs_int_info_ptr->cphs_info.cphs_service.sst =
    (mmgsdi_current_service_enum_type)((data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                      & MMGSDI_CPHS_INFO_SST_MASK) >> 2);
  cphs_int_info_ptr->cphs_info.cphs_service.mbn =
    (mmgsdi_current_service_enum_type)((data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                      & MMGSDI_CPHS_INFO_MBN_MASK) >> 4);
  cphs_int_info_ptr->cphs_info.cphs_service.ops =
    (mmgsdi_current_service_enum_type)((data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                      & MMGSDI_CPHS_INFO_OP_NAME_MASK) >> 6);
  cphs_int_info_ptr->cphs_info.cphs_service.ifn =
    (mmgsdi_current_service_enum_type)(data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B3]
                                     & MMGSDI_CPHS_INFO_INFO_NUM_MASK);
  return MMGSDI_SUCCESS;
} /* mmgsdi_cphs_parse_cphsi_data */


/*===========================================================================
FUNCTION MMGSDI_CPHS_PARSE_CSP_DATA

DESCRIPTION
  This function is a local function used to parse the CSP Data and store
  the results in gsdi_data (The global Data Structure).

  The information stored in the Global Data Structure will be utilized
  by the CPHS GET INFO API.

INPUT:
  mmgsdi_data_type  data:  The pointer to the buffer used to store the
                           read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cphs_parse_csp_data(
  mmgsdi_data_type              data,
  mmgsdi_session_type_enum_type session_type
)
{
  uint32                      iteration     = 0x0000;
  uint32                      svcs_index    = 0x0000;
  mmgsdi_cphs_int_info_type *cphs_int_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(data.data_ptr);

  if(mmgsdi_util_get_cphs_info_ptr(session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(cphs_int_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* -------------------------------------------------------------------------
   In case if Mandatory Data Objects are not present in CSP File then
   we will pick the default value.
   For example default value of PLMN mode is ON, So in case where
   the PLMN bit is not present in file then we will allow to show manual option
  ----------------------------------------------------------------------------*/

  /* Default value is ON for Value Added Service group */
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_PLMN_MODE    = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_VPS          = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_PAGING = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_EMAIL  = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_FAX          = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_DATA         = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.valued_added_Language    = MMGSDI_CSP_IND_ON;

  /* --------------------------------------------------------------------------
     Iterate through the Service Group offsets, and set the Global
     data Structure Information appropriately
     ------------------------------------------------------------------------*/
  while ( (iteration + 1) < int32touint32(data.data_len))
  {
    /* Service Group offset and its data should be there.*/
    svcs_index = iteration + 1;
    switch ( data.data_ptr[iteration] )
    {
    case MMGSDI_CPHS_CSP_SVC_GROUP_1:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFU_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFU = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFU = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFB_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFB = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFB = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFNRy_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRy = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRy = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFNRc_MASK)
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRc = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRc = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CT_MASK)
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CT = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CT = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_2:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BOAC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOAC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOAC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BOIC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BOICexHC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC_exHC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC_exHC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BAIC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BAIC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BAIC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BIC_ROAM_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BIC_Roam = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BIC_Roam = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_3:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MPTY_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_MPTY = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_MPTY = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CUG_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_AOC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_AoC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_AoC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_PREF_CUG_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_Pref_CUG = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_Pref_CUG = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CUG_OA_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG_OA = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG_OA = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_4:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_HOLD_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_HOLD = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_HOLD = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CW_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CW = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CW = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CCBS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CCBS = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CCBS = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_USER_SIG_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_User_Signalling = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_User_Signalling = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_5:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MT_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MT = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MT = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MO_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MO = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MO = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_CB_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_CB = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_CB = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_REPLY_PATH_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Reply_Path = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Reply_Path = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_DEL_CONF_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Del_Conf = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Del_Conf = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_PROTOCOL_ID_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Protocol_ID = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Protocol_ID = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VALIDITY_PER_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Validity_Period = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Validity_Period = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_6:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_ALS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_teleservices_als = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_teleservices_als = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_7:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_RSVD_SST_PHASE_1_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_features_SST = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_features_SST = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_8:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CLIP_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLIP = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLIP = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CoLR_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLR = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLR = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CoLP_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLP = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLP = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MCI_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_MCI = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_MCI = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CLI_SEND_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_send = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_send = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CLI_BLOCK_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_block = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_block = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_9:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_GPRS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_GPRS = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_GPRS = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_HSCSD_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_HSCSD = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_HSCSD = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VOICE_GRP_CALL_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Group_call = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Group_call = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VOICE_BRDCST_SVC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Broadcast_Svc = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Broadcast_Svc = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MULTI_SUB_PROFILE_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Muli_Sub_Profile = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Muli_Sub_Profile = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MULTI_BAND_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Multiband = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Multiband = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_C0:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_PLMN_MODE_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_PLMN_MODE = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_PLMN_MODE = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VPS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_VPS = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_VPS = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MO_PAGING_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_PAGING = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_PAGING = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MO_EMAIL_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_EMAIL = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_EMAIL = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_FAX_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_FAX = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_FAX = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_DATA_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_DATA = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_DATA = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_LANGUAGE_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.valued_added_Language = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.valued_added_Language = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_D5:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_INFO_NUMBERS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.information_numbers = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.information_numbers = MMGSDI_CSP_IND_OFF;

      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR_2("Bad Service Group: 0x%x Iteration: 0x%x",
                               data.data_ptr[iteration], iteration);
    }
    iteration += 2;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_cphs_parse_csp_data */


/*===========================================================================
FUNCTION MMGSDI_PROC_CPHS_POST_PIN1_INIT

DESCRIPTION
  This function will run the CPHS Intialization procedures.  The purpose
  of this function is to cache the relevant CPHS Information for use
  by other Tasks for later usage.

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
)
{
  mmgsdi_file_enum_type       chpsi_filename          = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type       csp_filename            = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type       chps_ons_filename       = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type       cphs_ons_short_filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_return_enum_type     read_cphsi_status       = MMGSDI_ERROR;
  mmgsdi_return_enum_type     mmgsdi_status           = MMGSDI_ERROR;
  mmgsdi_session_info_type   *session_info_ptr        = NULL;
  mmgsdi_data_type            mmgsdi_data_buf;
  mmgsdi_cphs_int_info_type  *cphs_int_info_ptr       = NULL;

  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /* --------------------------------------------------------------------------
     Perform Paramater Checks and allocate memory for the cphs Requests
     ------------------------------------------------------------------------*/
  if((slot != MMGSDI_SLOT_1) &&
     (slot != MMGSDI_SLOT_2) &&
     (slot != MMGSDI_SLOT_3))
  {
    MMGSDI_DEBUG_MSG_ERROR_1("CPHS: Bad MMGSDI Slot ID: 0x%x", slot);
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch(app_type)
  {
    case MMGSDI_APP_USIM:
      chpsi_filename            = MMGSDI_USIM_CPHSI;
      csp_filename              = MMGSDI_USIM_CSP;
      chps_ons_filename         = MMGSDI_USIM_ONS;
      cphs_ons_short_filename   = MMGSDI_USIM_ONS_SHORT;
      break;
    case MMGSDI_APP_SIM:
      chpsi_filename            = MMGSDI_GSM_CPHSI;
      csp_filename              = MMGSDI_GSM_CSP;
      chps_ons_filename         = MMGSDI_GSM_ONS;
      cphs_ons_short_filename   = MMGSDI_GSM_ONS_SHORT;
      break;
    default:
      UIM_MSG_HIGH_1("CPHS not valid for non SIM/USIM app type 0x%x", app_type );
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_cphs_info_ptr(session_info_ptr->session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(cphs_int_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(read_cphs_efs.read_cphsi)
  {
    /* --------------------------------------------------------------------------
       First Determine if CPHS is supported on this SIM/USIM by trying to
       read EF CPHS Information Files to access the CPHS Service Table
       ------------------------------------------------------------------------*/
    read_cphsi_status = mmgsdi_card_init_cache_binary( session_id,
                                                       app_type,
                                                       slot,
                                                       chpsi_filename,
                                                       &mmgsdi_data_buf);
    if (read_cphsi_status == MMGSDI_SUCCESS )
    {
      mmgsdi_status = mmgsdi_cphs_parse_cphsi_data(mmgsdi_data_buf,
                                                   session_info_ptr->session_type);
      if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR_1("Failure parsing CPHSI: 0x%x, CPHSI Not Valid",
                                 mmgsdi_status);
        read_cphs_efs.read_csp       = FALSE;
        read_cphs_efs.read_ons_short = FALSE;
      }
      else
      {
        /* Optional CHPS data-fields should only be read or updated
           if indicated as being both allocated and activated */
        if (cphs_int_info_ptr->cphs_info.cphs_service.csp != MMGSDI_ACTIVATED_AND_ALLOCATED)
        {
          read_cphs_efs.read_csp       = FALSE;
        }
        if (cphs_int_info_ptr->cphs_info.cphs_service.ops != MMGSDI_ACTIVATED_AND_ALLOCATED)
        {
          read_cphs_efs.read_ons_short = FALSE;
        }
      }
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Could not read CPHSI: 0x%x", mmgsdi_status);
      /*If Mode is USIM, then the File may not be present*/
      if(app_type == MMGSDI_APP_SIM)
      {
        /* EF-CPS and EF-ONS short are dependent on CPHS Service Table */
        read_cphs_efs.read_csp       = FALSE;
        read_cphs_efs.read_ons_short = FALSE;
      }
      else if(app_type == MMGSDI_APP_USIM)
      {
        if (!mmgsdi_nv_is_ens_enabled(slot))
        {
          /* EF-CPS and EF-ONS are dependent on CPHS Service Table
             and 3G cards with ENS flag enabled do not use CPHS
             Service Table so continue with CSP and ONS short read */
          read_cphs_efs.read_csp       = FALSE;
          read_cphs_efs.read_ons_short = FALSE;
        }
      }
    }
  }

  if(read_cphs_efs.read_csp)
  {
    /* --------------------------------------------------------------------------
       Next Read and Cache the Customer Service Profile File:
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                  app_type,
                                                  slot,
                                                  csp_filename,
                                                  &mmgsdi_data_buf);
    if (mmgsdi_status == MMGSDI_SUCCESS )
    {
      /* --------------------------------------------------------------------------
         At this point, we will Parse the CSP File in order to save the information
         to the gsdi_data type for future use as needed.
         ------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_cphs_parse_csp_data(mmgsdi_data_buf,
                                                 session_info_ptr->session_type);
      if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR_1("Failure parsing CSP: 0x%x, CSP Not Valid",
                                 mmgsdi_status);
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);

      /*-------------------------------------------------------------------------
        2G CARD Configuration:
        DF GSM -> EF CPHSI, EF CSP

        3g Card Configuration
        ADF-> EF CSP
        DF_GSM -> EF CPHSI

        With 3G cards ,the CPHSI file is only present under DF GSM and not under
        ADF, hence the read of the USIM CPHSI will result in an error. Since CSP
        is still present in the ADF , it implies that the card supports CPHS.

        cphs_capable suggest if the card supports CPHS or not.

        Set the CPHS Available to TRUE so that the applications can read the
        other information from cphs_info globals
        -----------------------------------------------------------------------*/
      if(read_cphsi_status != MMGSDI_SUCCESS &&
         app_type == MMGSDI_APP_USIM)
      {
        cphs_int_info_ptr->cphs_info.cphs_capable = TRUE;
        cphs_int_info_ptr->cphs_info.cphs_service.csp =
          MMGSDI_ACTIVATED_AND_ALLOCATED;
        cphs_int_info_ptr->cphs_info.cphs_service.ops =
          MMGSDI_ACTIVATED_AND_ALLOCATED;
      }
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Failure reading CSP: 0x%x, CSP Not Supported",
                               mmgsdi_status);
    }
  }

  if(read_cphs_efs.read_ons)
  {
    /* --------------------------------------------------------------------------
       Next Read and Cache the CPHS ONS File:
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_card_init_cache_binary( session_id,
                                                   app_type,
                                                   slot,
                                                   chps_ons_filename,
                                                   &mmgsdi_data_buf);
    if (mmgsdi_status == MMGSDI_SUCCESS )
    {
      if(mmgsdi_data_buf.data_len > 0 && mmgsdi_data_buf.data_ptr != NULL)
      {
        cphs_int_info_ptr->cphs_ons_len = mmgsdi_data_buf.data_len;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cphs_int_info_ptr->cphs_ons_ptr);
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cphs_int_info_ptr->cphs_ons_ptr,
          cphs_int_info_ptr->cphs_ons_len);

        if (cphs_int_info_ptr->cphs_ons_ptr != NULL)
        {
          mmgsdi_memscpy((void *)(cphs_int_info_ptr->cphs_ons_ptr),
                         cphs_int_info_ptr->cphs_ons_len,
                         (void *)mmgsdi_data_buf.data_ptr,
                         cphs_int_info_ptr->cphs_ons_len);
        }
        else
        {
          cphs_int_info_ptr->cphs_ons_len = 0;
        }
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Failure reading ONS: 0x%x, ONS Not Supported",
                               mmgsdi_status);
    }
  }

  if(read_cphs_efs.read_ons_short)
  {
    /* --------------------------------------------------------------------------
       Next Read and Cache the CPHS ONS SHORT File:
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_card_init_cache_binary( session_id,
                                                   app_type,
                                                   slot,
                                                   cphs_ons_short_filename,
                                                   &mmgsdi_data_buf);
    if (mmgsdi_status == MMGSDI_SUCCESS )
    {
      if(mmgsdi_data_buf.data_len > 0 && mmgsdi_data_buf.data_ptr != NULL)
      {
        cphs_int_info_ptr->cphs_ons_short_len = mmgsdi_data_buf.data_len;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cphs_int_info_ptr->cphs_ons_short_ptr);
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          cphs_int_info_ptr->cphs_ons_short_ptr,
          cphs_int_info_ptr->cphs_ons_short_len);

        if (cphs_int_info_ptr->cphs_ons_short_ptr != NULL)
        {
          mmgsdi_memscpy((void *)(cphs_int_info_ptr->cphs_ons_short_ptr),
                         cphs_int_info_ptr->cphs_ons_short_len,
                         (void *)mmgsdi_data_buf.data_ptr,
                         cphs_int_info_ptr->cphs_ons_short_len);
        }
        else
        {
          cphs_int_info_ptr->cphs_ons_short_len = 0;
        }
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Failure reading ONS Short: 0x%x, ONS Short Not Supported",
                               mmgsdi_status);
    }
  }

  /* --------------------------------------------------------------------------
     Cleanup procedures to free up any memory allocated for this function
     ------------------------------------------------------------------------*/
  return MMGSDI_SUCCESS;
} /* mmgsdi_proc_cphs_post_pin1_init */
