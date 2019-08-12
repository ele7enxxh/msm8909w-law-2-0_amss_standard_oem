/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  M M G S D I   E O N S  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to process EONS
  and EONS initalization.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2011 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_eons.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
06/10/16   rp      Reduce repeated F3 logging in EONS
05/24/16   bcho    EONS F3 reduction
05/22/16   vdc     Remove F3 messages for memory allocation failure
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
09/07/15   av      Migrate to uim_common EFS APIs
05/04/15   bcho    Copy additional name also in API cnf
12/03/14   bcho    featurize support to use stored rplmn name across operators
09/09/14   kk      Enhancements in NV handling
08/08/14   bcho    Copy additional names to API CNF
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
08/06/14   bcho    Display SPN flag is reset in roaming when display bit is 1
07/15/14   yt      Support for NV refresh
07/04/14   bcho    Ignore display condition in handling API call in limited srv
03/28/14   ar      Enabled N.A. PLMN comparison feature by default
03/11/14   bcho    Don't run EONS algo if OPL-PNN not cached
02/10/14   ar      Replace session checks with utility functions
02/06/14   hh      For API calls, populate ONS when EF-SPN is absent or SPN is disabled 
02/03/13   av      Duplicate long/short names if either one is empty
01/29/14   vdc     Added support for PLMN name source
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
12/19/13   yt      Support for new CM SS event interface
12/18/13   hh      For API calls, populate SPN and ONS only when condition allows
12/05/13   bcho    Re-run the EONS algorithm when PLMNID/LAC/RAT is changed
10/22/13   bcho    Use roaming status received in CM SS event in EONS algorithm
10/17/13   am      Added new item for plmn comparison in the nvitem 67211
10/01/13   am      Fix update of display operator in using display byte
08/28/13   av      Support for Chinese language PLMN names
08/27/13   vv      Fix for a memory leak
08/22/13   vdc     Added support for TSTS
08/21/13   rp      Compare IMSI PLMN after copying n/w plmn in globals
08/16/13   bcho    SIM-BCCH PLMN comparison function updated for APADT TCs
07/15/13   vs      EONS updates to support single baseband SGLTE
07/04/13   bcho    Modify length check for EF-SPDI data
07/03/13   bcho    PLMN compare fn modified according to Annex A 3GPP TS 23122
07/01/13   at      Support for additional info in EONS APIs
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/24/13   vdc     Added support for triple SIM
06/19/13   tl      Reintroducing featurization for dual slot
06/18/13   vv      Fix for copying HNB name length
06/17/13   bcho    Modify check for Service provider PLMN list tag in EF-SPDI
05/28/13   spo     Replaced instances of memcpy with memscpy
05/16/13   vdc     Replace memcpy with safer version memscpy
05/15/13   bcho    Perform OPL-PNN lookup for Non RPLMN ID under NV feature
05/08/13   bcho    Added support to provide rplmn-spn display bit and network
                   info in get operator name API cnf and in EONS event
05/06/13   spo     Modified CPHS variables to array of pointers
05/02/13   at      Klockwork fixes for SE13 related APIs
05/02/13   bcho    Use rplmn name stored in EF-CPHS-ONS only if RPLMN is HPLMN
05/01/13   bcho    For ICC cards provide seek direction to search pattern in EF
04/05/13   at      Support for reading SE13 binary file from EFS
04/16/13   bcho    Use rplmn name stored in EFS in first full srv after bootup
04/16/13   tl      Remove obsoleted featurization
04/16/13   bcho    SE13 table updated to 28 March, 2013 GSMA published list
04/10/13   tkl     Updated memcpy function with strlen for security reason
04/10/13   bcho    SE13 table updated to 26 Feb, 2013 GSMA published list
04/04/13   bcho    Update SE13 table and modify its lookup algorithm
04/04/13   bcho    Don't perform OPL-PNN lookup for Non registered PLMN ID
03/21/13   bcho    Send EONS event to session who registered late
03/11/13   abd     SE13 table updated to 28 January, 2013 GSMA published list
03/11/13   abd     SE13 table updated to November 5th 2012 GSMA published list
02/07/13   vv      EONS support for sys mode SYS_SYS_MODE_TDS
01/29/13   yt      Fixed Klocwork issue
01/20/13   vv      Introduced new UCS2_PACKED encoding scheme
01/20/13   vv      For RPLMN name request, check CSG ID with NITZ CSG ID
01/20/13   vv      Reset status in case HNB name is not retrieved
01/20/13   vv      Added EONS support for the CSG
01/11/13   bcho    Free allocated memory if EONS algo returns error code
01/10/13   bcho    Update boundary checks for index in SE13 table binary search
12/12/12   at      Fixed critical KW warnings
12/12/12   av      Fixed compiler critical warnings
11/12/12   bcho    Read EF-AD to determine EF-IMSI has 2-digit or 3-digit MNC
11/11/12   bcho    SE13 table updated to October 23rd 2012 GSMA published list
11/08/12   bcho    SE13 table updated to October 5th 2012 GSMA published list
11/05/12   vv      Intialize local variable to prevent possible crash
11/05/12   av      Skip checking security attributes during OPL/PNN EF select
10/30/12   bcho    Copy RAT type with PLMN ID in get operator name cnf,
                   compare RAT type with camped NW type to decide plmn is rplmn
10/29/12   bcho    Return PLMN name from SE13 table if there is only entry for
                   plmn id though RAT type is not same.
10/19/12   bcho    Boundary checks for SE13 table index during binary search
10/17/12   bcho    Handle scenario when TAC LAC CHANGED MASK is set in SS event
10/15/12   bcho    Handle SS event/get operator name API call in No Card and/or
                   Limited Service scenario
10/12/12   abg     Do not send provisioning events to non-prov sessions
10/11/12   spo     Removed unnecessary F3 messages
10/03/12   abg     Removed Queuing related F3 messages
10/03/12   bcho    Check status of EF-OPL/EF-PNN in svc table before caching
09/24/12   bcho    Add LTE network type in SE13 table entries for Fido
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/11/12   bcho    SPN-RPLMN display condition ignored if client requests
09/07/12   bcho    Check all bytes of SPN/ONS/ONS SHORT data buffer is not 0xFF
                   and validate buffer when encoding type is GSM 7 BIT UNPACKED
09/07/12   bcho    PLMN comparision function modified
09/03/12   bcho    NULL check before de-reference plmn list ptr
09/03/12   bcho    EF-OPL and EF-PNN attributes read and search pattern made
                   sync, Send eons cache ready event
08/13/12   bcho    Use RAT type with PLMN ID in SE13 table lookup
08/13/12   bcho    Add LTE names into SE13 table based on new SE13 release
08/13/12   bcho    Check for MCC-MNC valid digits added in get_mnc_mcc_str()
08/09/12   bcho    SE13 table lookup modified
08/09/12   bcho    Add LTE network type in SE13 table entries for AT&T
06/07/12   bcho    Redundant Memory allocation removed
05/04/12   bcho    Use TAC at place of LAC while copying LTE network SS info
05/04/12   bcho    Trigger EONS event when service status is changed to FULL
04/12/12   bcho    EONS Event(triggered by SS Evt) supported on test framework
01/30/12   bcho    Implementation of get operator name API, EONS event modified
12/27/11   bcho    To resolve memory corruption while reading EFS file.
12/21/11   bcho    Initial version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "mmgsdi_evt.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_se13.h"
#include "mmgsdi_eons_nitz.h"
#include "mmgsdi_eons_opl_pnn.h"
#include "mmgsdi_eons_cdma.h"
#include "uim_selective_logging.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_EONS_PLMN_MATCH_FUNC_TYPE
-----------------------------------------------------------------------------*/
/** A function pointer type that when used allows a caller to use particular
    type of plmn matching function depending on the plmn being matched
*/
typedef boolean (*mmgsdi_eons_plmn_match_func_type) (mmgsdi_plmn_id_type plmn_id_1,
                                                     mmgsdi_plmn_id_type plmn_id_2);

/*==========================================================================
FUNCTION MMGSDI_EONS_PLMN_GET_MCC_MNC

DESCRIPTION
  Function to get MCC and MNC values from PLMN ID

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_plmn_get_mcc_mnc(
  mmgsdi_plmn_id_type   plmn,
  uint16              * mcc_ptr,
  uint16              * mnc_ptr
)
{
  uint16 mcc_digit_1 = 0;
  uint16 mcc_digit_2 = 0;
  uint16 mcc_digit_3 = 0;

  uint16 mnc_digit_1 = 0;
  uint16 mnc_digit_2 = 0;
  uint16 mnc_digit_3 = 0;

  if(mcc_ptr  == NULL ||
     mnc_ptr  == NULL ||
     (plmn.plmn_id_val[0] == 0XFF &&
      plmn.plmn_id_val[1] == 0XFF &&
      plmn.plmn_id_val[2] == 0XFF ))
  {
    return MMGSDI_ERROR;
  }

  mcc_digit_1 = plmn.plmn_id_val[0] & 0x0F;
  mcc_digit_2 = plmn.plmn_id_val[0] / 0x10;
  mcc_digit_3 = plmn.plmn_id_val[1] & 0x0F;

  mnc_digit_1 = plmn.plmn_id_val[2] & 0x0F;
  mnc_digit_2 = plmn.plmn_id_val[2] / 0x10;
  mnc_digit_3 = plmn.plmn_id_val[1] / 0x10;

  *mcc_ptr = (100 * mcc_digit_1) + (10 * mcc_digit_2) + mcc_digit_3;

  if (mnc_digit_3 == 0xF)
  {
    *mnc_ptr = (10 * mnc_digit_1) + mnc_digit_2;
  }
  else
  {
    *mnc_ptr = (100 * mnc_digit_1) + (10 * mnc_digit_2) + mnc_digit_3;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_plmn_get_mcc_mnc */


/*==========================================================================
FUNCTION MMGSDI_EONS_COMPARE_SIM_PLMN_WITH_NW_PLMN

DESCRIPTION
  Function to compare SIM PLMN ID with PLMN ID received from network.
  This follows Annex A 3GPP TS 23122 for PLMN comparison which says:

  MS shall compare using all 3 digits of SIM-MCC with that of BCCH-MCC.
  If the values matches, then MS shall read the 3rd digit of the BCCH-MNC.
  If the 3rd digit is Hex F, then MS shall determine if the BCCH-MCC lies
  in the range 310-316. If yes, then MS shall compare the 3rd digit of
  SIM-MNC with ‘0’. If yes, MS shall compare using just the 1st 2 digits
  of the SIM-MNC with that of BCCH-MNC. If the values match, then PLMN match
  succeeds.

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_compare_sim_plmn_with_nw_plmn(
  mmgsdi_plmn_id_type sim_plmn,
  mmgsdi_plmn_id_type nw_plmn)
{
  uint16 mcc = 0;
  uint16 mnc = 0;

  /* First compare sim_plmn's MCC with that of nw_plmn */
  if(((sim_plmn.plmn_id_val[0] & 0x0F) == (nw_plmn.plmn_id_val[0] & 0x0F)) &&
     ((sim_plmn.plmn_id_val[0] & 0xF0) == (nw_plmn.plmn_id_val[0] & 0xF0)) &&
     ((sim_plmn.plmn_id_val[1] & 0x0F) == (nw_plmn.plmn_id_val[1] & 0x0F)))
  {
    if(MMGSDI_SUCCESS != mmgsdi_eons_plmn_get_mcc_mnc(nw_plmn, &mcc, &mnc))
    {
      return FALSE;
    }

    /* To follow Annex A 3GPP TS 23122 (see function description) NW PLMN ID's
       MNC is updated as below.
       If NW PLMNID's MCC is in range of 310-316 (BCCH MCC) and MNC's third
       digit is 'F', replace 'F' by '0'. */
    if(mcc >= 310 && mcc <= 316 && ((nw_plmn.plmn_id_val[1] & 0xF0) == 0xF0))
    {
      nw_plmn.plmn_id_val[1] = nw_plmn.plmn_id_val[1] & 0x0F;
    }

    /* Annex A 3GPP TS 23122 says that if 3rd digit of SIM MNC is not '0’
       then PLMN match fails but to let card which violating Annex A work
       properly it is required to replace 3rd digit of SIM MNC with '0' if
       it is 'F' and SIM PLMNID's MCC is in range of 310-316. */

    if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_NORTH_AMERICAN_PLMN_COMPARISON, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
              MMGSDI_FEATURE_ENABLED)
    {
      if(MMGSDI_SUCCESS != mmgsdi_eons_plmn_get_mcc_mnc(sim_plmn, &mcc, &mnc))
      {
        return FALSE;
      }

      /* Though Annex A 3GPP TS 23122 says that if 3rd digit of SIM MNC is not
         ‘0’ then PLMN match fails, it is required to replace 3rd digit of SIM
         MNC with '0' if it is 'F' and SIM PLMNID's MCC is in range of 310-316
         for ADAPT test cases. */
      if(mcc >= 310 && mcc <= 316 && ((sim_plmn.plmn_id_val[1] & 0xF0) == 0xF0))
      {
        sim_plmn.plmn_id_val[1] = sim_plmn.plmn_id_val[1] & 0x0F;
      }
    }

    /* Compare 1. two digits of sim_plmn's MNC with that of nw_plmn if both have
       only two digits or 2. all three digits */
    if((((nw_plmn.plmn_id_val[1] & 0xF0) == 0xF0) && ((sim_plmn.plmn_id_val[1] & 0xF0) == 0xF0))
           ?(((sim_plmn.plmn_id_val[2] & 0x0F) == (nw_plmn.plmn_id_val[2] & 0x0F)) &&
              ((sim_plmn.plmn_id_val[2] & 0xF0) == (nw_plmn.plmn_id_val[2] & 0xF0)))
           :(((sim_plmn.plmn_id_val[2] & 0x0F) == (nw_plmn.plmn_id_val[2] & 0x0F)) &&
              ((sim_plmn.plmn_id_val[2] & 0xF0) == (nw_plmn.plmn_id_val[2] & 0xF0)) &&
              ((sim_plmn.plmn_id_val[1] & 0xF0) == (nw_plmn.plmn_id_val[1] & 0xF0))))
    {
      return TRUE;
    }
  }
  return FALSE;
}/* mmgsdi_eons_compare_sim_plmn_with_nw_plmn */


/*==========================================================================
FUNCTION MMGSDI_EONS_PLMN_MATCH

DESCRIPTION
  Function to compare two network PLMN IDs. If a PLMN ID from a SIM has to
  be compared with a NW PLMN ID then the function
  mmgsdi_eons_compare_sim_plmn_with_nw_plmn should be used

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_plmn_match(
  mmgsdi_plmn_id_type plmn_id_1,
  mmgsdi_plmn_id_type plmn_id_2)
{
  return (((plmn_id_1.plmn_id_val[0] & 0x0F) == (plmn_id_2.plmn_id_val[0] & 0x0F)) &&
           ((plmn_id_1.plmn_id_val[0] / 0x10) == (plmn_id_2.plmn_id_val[0] / 0x10)) &&
           ((plmn_id_1.plmn_id_val[1] & 0x0F) == (plmn_id_2.plmn_id_val[1] & 0x0F)) &&
           ((((plmn_id_2.plmn_id_val[1] / 0x10) == 0x0F) && ((plmn_id_1.plmn_id_val[1] / 0x10) == 0x0F))
           ?(((plmn_id_1.plmn_id_val[2] & 0x0F) == (plmn_id_2.plmn_id_val[2] & 0x0F)) &&
              ((plmn_id_1.plmn_id_val[2] / 0x10) == (plmn_id_2.plmn_id_val[2] / 0x10)))
           :(((plmn_id_1.plmn_id_val[2] & 0x0F) == (plmn_id_2.plmn_id_val[2] & 0x0F)) &&
              ((plmn_id_1.plmn_id_val[2] / 0x10) == (plmn_id_2.plmn_id_val[2] / 0x10)) &&
              ((plmn_id_1.plmn_id_val[1] / 0x10) == (plmn_id_2.plmn_id_val[1] / 0x10)))
           ));
}/* mmgsdi_eons_plmn_match */


/*==========================================================================
FUNCTION MMGSDI_EONS_COPY_PLMN_NAME_TO_EFS_DATA

DESCRIPTION
  Function to copy plmn name to efs data buffer

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
void mmgsdi_eons_copy_plmn_name_to_efs_data(
  const mmgsdi_eons_nitz_data_type  * nitz_info_ptr,
  uint8                             * buffer_ptr,
  uint32                              buffer_len
)
{
  uint8 full_name_length =
    (uint8)nitz_info_ptr->long_name.plmn_name.eons_data.data_len;
  uint8 short_name_length =
    (uint8)nitz_info_ptr->short_name.plmn_name.eons_data.data_len;

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "copy plmn name to efs buffer");

  /*copy full name available flag*/
  buffer_ptr[MMGSDI_EONS_FILE_DATA_NAME_FLAG_OFFSET] =
    nitz_info_ptr->long_name_available;

  /*copy full name country initials*/
  buffer_ptr[MMGSDI_EONS_FILE_DATA_NAME_CI_OFFSET] =
    nitz_info_ptr->long_name.plmn_name_ci;

  /*copy full name spare bits*/
  buffer_ptr[MMGSDI_EONS_FILE_DATA_SPARE_BIT_OFFSET] =
    nitz_info_ptr->long_name.plmn_name_spare_bits;

  /*copy full name encoding*/
  mmgsdi_memscpy(&buffer_ptr[MMGSDI_EONS_FILE_DATA_NAME_ENC_OFFSET],
                 buffer_len,
                 &nitz_info_ptr->long_name.plmn_name.eons_encoding,
                 sizeof(mmgsdi_eons_encoding_enum_type));

  /*copy full name length*/
  buffer_ptr[MMGSDI_EONS_FILE_DATA_NAME_LEN_OFFSET] = full_name_length;

  if(nitz_info_ptr->long_name.plmn_name.eons_data.data_len > 0)
  {
    /*copy full name data buffer*/
    mmgsdi_memscpy(&buffer_ptr[MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET],
                   buffer_len,
                   nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
                   nitz_info_ptr->long_name.plmn_name.eons_data.data_len);
  }

  /*copy short name available flag*/
  buffer_ptr[full_name_length + MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET] =
    nitz_info_ptr->short_name_available;

  /*copy short name country initials*/
  buffer_ptr[full_name_length + MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET +
                                MMGSDI_EONS_FILE_DATA_NAME_CI_OFFSET] =
    nitz_info_ptr->short_name.plmn_name_ci;

  /*copy short name spare bits*/
  buffer_ptr[full_name_length + MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET +
                                MMGSDI_EONS_FILE_DATA_SPARE_BIT_OFFSET] =
    nitz_info_ptr->short_name.plmn_name_spare_bits;

  /*copy short name encoding*/
  mmgsdi_memscpy(&buffer_ptr[full_name_length + MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET +
                                        MMGSDI_EONS_FILE_DATA_NAME_ENC_OFFSET],
                 buffer_len,
                 &nitz_info_ptr->short_name.plmn_name.eons_encoding,
                 sizeof(mmgsdi_eons_encoding_enum_type));

  /*copy short name length*/
  buffer_ptr[full_name_length + MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET +
                                MMGSDI_EONS_FILE_DATA_NAME_LEN_OFFSET]
    = short_name_length;

  if(nitz_info_ptr->short_name.plmn_name.eons_data.data_len > 0)
  {
    /*copy short name data buffer*/
    mmgsdi_memscpy(&buffer_ptr[full_name_length + MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET +
                                          MMGSDI_EONS_FILE_DATA_NAME_POS_OFFSET],
                 buffer_len,
                 nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                 nitz_info_ptr->short_name.plmn_name.eons_data.data_len);
  }
}/* mmgsdi_eons_copy_plmn_name_to_efs_data */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_CPHS_ONS

DESCRIPTION
  Function to get operator name string from cphs file caches

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_eons_get_cphs_ons(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_plmn_info_type         * plmn_info_ptr)
{
  boolean                     is_ons_retrieved  = FALSE;
  mmgsdi_cphs_int_info_type * cphs_int_info_ptr = NULL;

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "Get cphs ons");

  if(plmn_info_ptr == NULL)
  {
    return is_ons_retrieved;
  }

  if(mmgsdi_util_get_cphs_info_ptr(session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return is_ons_retrieved;
  }
  if(cphs_int_info_ptr == NULL)
  {
    return is_ons_retrieved;
  }

  /*read CPHS ONS cache*/
  if(cphs_int_info_ptr->cphs_ons_ptr != NULL &&
     cphs_int_info_ptr->cphs_ons_len > 0)
  {
    uint8 len = 0;

    /*get first occurrence of 0xFF in CPHS ONS Buffer*/
    unsigned char *cphs_ons_buff_ptr =
        (unsigned char *)memchr (cphs_int_info_ptr->cphs_ons_ptr,
                                0xFF, cphs_int_info_ptr->cphs_ons_len);

    /*if pointer is not NULL, an occurrence of 0xFF is found*/
    if(cphs_ons_buff_ptr)
    {
      /*Calculate length of CPHS ONS buffer before ocurrence of 0xFF*/
      len = (uint8)(cphs_ons_buff_ptr - (cphs_int_info_ptr->cphs_ons_ptr));
    }
    else
    {
      /*CPHS ONS Buffer does not have 0xFF*/
      len = (uint8)cphs_int_info_ptr->cphs_ons_len;
    }

    if(len)
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "cphs ons valid buffer len: 0x%x", len);

      /*copy long name length*/
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = len;

      /*copy long name encoding*/
      plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
        MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
        len);

      if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
      {
        /*copy long name data buffer*/
        mmgsdi_memscpy(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
                       len,
                       cphs_int_info_ptr->cphs_ons_ptr,
                       len);
        is_ons_retrieved = TRUE;
      }
      else
      {
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = 0;

        UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS, "Memory allocation failed for plmn_long_name eons_data.data_ptr");
      }
    }
  }

  /*read CPHS ONS SHORT cache*/
  if(cphs_int_info_ptr->cphs_ons_short_ptr != NULL &&
     cphs_int_info_ptr->cphs_ons_short_len > 0)
  {
    uint8 len = 0;

    /*get first occurrence of 0xFF in CHPS ONS SHORT Buffer*/
    unsigned char *cphs_ons_short_ptr =
        (unsigned char *)memchr (cphs_int_info_ptr->cphs_ons_short_ptr,
                                0xFF, cphs_int_info_ptr->cphs_ons_short_len);

    /*if pointer is not NULL, an occurrence of 0xFF is found*/
    if(cphs_ons_short_ptr)
    {
      /*Calculate length of CHPS ONS SHORT buffer before ocurrence of 0xFF*/
      len = (uint8)(cphs_ons_short_ptr - (cphs_int_info_ptr->cphs_ons_short_ptr));
    }
    else
    {
      /*CPHS ONS SHORT Buffer does not have 0xFF*/
      len = (uint8)cphs_int_info_ptr->cphs_ons_short_len;
    }

    if(len)
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "cphs ons short valid buffer len: 0x%x", len);

      /*copy short name length*/
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = len;

      /*copy short name encoding*/
      plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding =
        MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
        len);

      if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr != NULL)
      {
        /*copy short name data buffer*/
        mmgsdi_memscpy(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
                       len,
                       cphs_int_info_ptr->cphs_ons_short_ptr,
                       len);
        is_ons_retrieved = TRUE;
      }
      else
      {
        plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = 0;

        UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS, "Memory allocation failed for plmn_short_name eons_data.data_ptr");
      }
    }
  }
  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "cphs ons data long len: 0x%x, short len: 0x%x",
                 cphs_int_info_ptr->cphs_ons_len,
                 cphs_int_info_ptr->cphs_ons_short_len);

  return is_ons_retrieved;
}/*mmgsdi_eons_get_cphs_ons*/


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_MNC_MCC_STR

DESCRIPTION
  Function to convert MNC MCC into char string

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_eons_get_mnc_mcc_str(
  mmgsdi_plmn_info_type * plmn_info_ptr)
{
  uint32 index                                = 0;
  uint32 mcc_digit_1                          = 0;
  uint32 mcc_digit_2                          = 0;
  uint32 mcc_digit_3                          = 0;
  uint32 mnc_digit_1                          = 0;
  uint32 mnc_digit_2                          = 0;
  uint32 mnc_digit_3                          = 0;
  uint8  ons_str[MMGSDI_EONS_MCC_MNC_STR_LEN] = {0};

  if(plmn_info_ptr == NULL)
  {
    return FALSE;
  }

  if(plmn_info_ptr->plmn_id.plmn_id_val[0] == 0XFF &&
     plmn_info_ptr->plmn_id.plmn_id_val[1] == 0XFF &&
     plmn_info_ptr->plmn_id.plmn_id_val[2] == 0XFF )
  {
    return FALSE;
  }

  mcc_digit_1 = plmn_info_ptr->plmn_id.plmn_id_val[0] & 0x0F;
  mcc_digit_2 = plmn_info_ptr->plmn_id.plmn_id_val[0] / 0x10;
  mcc_digit_3 = plmn_info_ptr->plmn_id.plmn_id_val[1] & 0x0F;

  mnc_digit_1 = plmn_info_ptr->plmn_id.plmn_id_val[2] & 0x0F;
  mnc_digit_2 = plmn_info_ptr->plmn_id.plmn_id_val[2] / 0x10;
  mnc_digit_3 = plmn_info_ptr->plmn_id.plmn_id_val[1] / 0x10;

  if((mcc_digit_1 > 9) ||
     (mcc_digit_2 > 9) ||
     (mcc_digit_3 > 9) ||
     (mnc_digit_1 > 9) ||
     (mnc_digit_2 > 9))
  {
    UIM_MSG_MED_0("MCC MNC digits are not valid");
    return FALSE;
  }

  /*assumimg mcc and mnc are max 3 digit numbers*/
  ons_str[index] = (uint8)(mcc_digit_1 + '0');
  index++;

  ons_str[index] = (uint8)(mcc_digit_2 + '0');
  index++;

  ons_str[index] = (uint8)(mcc_digit_3 + '0');
  index++;

  ons_str[index] = ' ';
  index++;

  ons_str[index] = (uint8)(mnc_digit_1 + '0');
  index++;

  ons_str[index] = (uint8)(mnc_digit_2 + '0');
  index++;

  if(mnc_digit_3 <= 9)
  {
    ons_str[index] = (uint8)(mnc_digit_3 + '0');
    index++;
  }

  ons_str[index] = 0x00;

  plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = index;

  plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
    MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

  if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len > 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len);

    if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len,
        ons_str,
        strlen((const char *)ons_str));
    }
    else
    {
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = 0;
    }
  }

  plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = index;

  plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding =
    MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

  if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len > 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len);

    if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
        plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len,
        ons_str,
        strlen((const char *)ons_str));
    }
    else
    {
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = 0;
    }
  }
  return TRUE;
}/* mmgsdi_eons_get_mnc_mcc_str */


/*===========================================================================
  FUNCTION MMGSDI_EONS_MATCH_PLMN_ID_TO_RPLMN_ID

DESCRIPTION
  Function to compare the plmn id with the rplmn id and determine if they match

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_eons_match_plmn_id_to_rplmn_id(
  const mmgsdi_eons_info_type       * eons_info_ptr,
  boolean                             plmn_id_from_sim,
  const mmgsdi_plmn_id_type         * plmn_id_ptr,
  boolean                             compare_rat,
  mmgsdi_eons_call_stack_enum_type  * stack_type_ptr
)
{
  boolean                             first_stack_rplmn_avail  = FALSE;
  const mmgsdi_plmn_id_type         * first_stack_rplmn_id_ptr = NULL;
  boolean                             sec_stack_rplmn_avail    = FALSE;
  const mmgsdi_plmn_id_type         * sec_stack_rplmn_id_ptr   = NULL;
  mmgsdi_eons_plmn_match_func_type    plmn_match_func          = NULL;

  if (plmn_id_ptr == NULL || eons_info_ptr == NULL)
  {
    return FALSE;
  }

  if (stack_type_ptr != NULL)
  {
    *stack_type_ptr = MMGSDI_EONS_MAX_CALL_STACK;
  }

  first_stack_rplmn_avail =
    eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_available;

  first_stack_rplmn_id_ptr =
    &(eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_id);

  sec_stack_rplmn_avail   =
    eons_info_ptr->rplmn_info.sec_stack_rplmn.rplmn_available;

  sec_stack_rplmn_id_ptr =
    &(eons_info_ptr->rplmn_info.sec_stack_rplmn.rplmn_id);

  if (plmn_id_from_sim)
  {
    plmn_match_func = mmgsdi_eons_compare_sim_plmn_with_nw_plmn;
  }
  else
  {
    plmn_match_func = mmgsdi_eons_plmn_match;
  }

  if(first_stack_rplmn_avail)
  {
    if (plmn_match_func(*plmn_id_ptr, *first_stack_rplmn_id_ptr))
    {
      if (compare_rat)
      {
        if(eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_id.rat ==
             plmn_id_ptr->rat)
        {
          if (stack_type_ptr != NULL)
          {
            *stack_type_ptr = MMGSDI_EONS_FIRST_CALL_STACK;
          }
          return TRUE;
        }
      }
      else
      {
        if (stack_type_ptr != NULL)
        {
          *stack_type_ptr = MMGSDI_EONS_FIRST_CALL_STACK;
        }
        return TRUE;
      }
    }
  }

  if(sec_stack_rplmn_avail)
  {
    if (plmn_match_func(*plmn_id_ptr, *sec_stack_rplmn_id_ptr))
    {
      if (compare_rat)
      {
        if(eons_info_ptr->rplmn_info.sec_stack_rplmn.rplmn_id.rat ==
             plmn_id_ptr->rat)
        {
          if (stack_type_ptr != NULL)
          {
            *stack_type_ptr = MMGSDI_EONS_SEC_CALL_STACK;
          }
          return TRUE;
        }
      }
      else
      {
        if (stack_type_ptr != NULL)
        {
          *stack_type_ptr = MMGSDI_EONS_SEC_CALL_STACK;
        }
        return TRUE;
      }
    }
  }
  return FALSE;
} /* mmgsdi_eons_match_plmn_id_to_rplmn_id */


/*===========================================================================
  FUNCTION MMGSDI_EONS_COPY_RPLMN_LAC

DESCRIPTION
  Function to copy RPLMN LAC if available

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_copy_rplmn_lac(
  mmgsdi_eons_call_stack_enum_type   stack_type,
  const mmgsdi_eons_info_type      * eons_info_ptr,
  mmgsdi_plmn_info_type            * plmn_info_ptr
)
{
  if(eons_info_ptr == NULL || plmn_info_ptr == NULL ||
     (stack_type != MMGSDI_EONS_FIRST_CALL_STACK &&
      stack_type != MMGSDI_EONS_SEC_CALL_STACK))
  {
    return;
  }

  if(stack_type == MMGSDI_EONS_FIRST_CALL_STACK &&
     eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_available &&
     eons_info_ptr->rplmn_info.first_stack_rplmn.lac_available)
  {
    plmn_info_ptr->lac = eons_info_ptr->rplmn_info.first_stack_rplmn.lac;
  }
  else if(eons_info_ptr->rplmn_info.sec_stack_rplmn.rplmn_available &&
          eons_info_ptr->rplmn_info.sec_stack_rplmn.lac_available)
  {
    plmn_info_ptr->lac = eons_info_ptr->rplmn_info.sec_stack_rplmn.lac;
  }
} /* mmgsdi_eons_copy_rplmn_lac */


/*===========================================================================
  FUNCTION MMGSDI_EONS_COPY_RPLMN_NAME

DESCRIPTION
  Function to copy RPLMN name if available

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if RPLMN name is copied

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_eons_copy_rplmn_name(
  mmgsdi_eons_call_stack_enum_type   stack_type,
  const mmgsdi_eons_info_type      * eons_info_ptr,
  mmgsdi_plmn_info_type            * plmn_info_ptr
)
{
  const mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr  = NULL;
  boolean                                        name_copied           = FALSE;

  if (eons_info_ptr == NULL || plmn_info_ptr == NULL ||
      (stack_type != MMGSDI_EONS_FIRST_CALL_STACK &&
       stack_type != MMGSDI_EONS_SEC_CALL_STACK))
  {
    return FALSE;
  }

  if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
  {
    rplmn_info_ptr = &(eons_info_ptr->rplmn_info.first_stack_rplmn);
  }
  else
  {
    rplmn_info_ptr = &(eons_info_ptr->rplmn_info.sec_stack_rplmn);
  }

  if (rplmn_info_ptr->rplmn_available == FALSE)
  {
    return FALSE;
  }

  /* Copy Marco cell name if available */
  if(plmn_info_ptr->plmn_id.csg_id == MMGSDI_EONS_INVALID_CSG_ID)
  {
    if (rplmn_info_ptr->long_name_available)
    {
      if (MMGSDI_SUCCESS == mmgsdi_eons_copy_plmn_name_info(
                              &(rplmn_info_ptr->long_name),
                              &(plmn_info_ptr->plmn_long_name),
                              TRUE))
      {
        name_copied = TRUE;

        if (uim_selective_logging[UIM_LOG_EONS])
        {
          mmgsdi_util_print_data_to_diag("Long name data buffer",
            plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
            (uint16)plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len);
        }
      }
    }

    if (rplmn_info_ptr->short_name_available)
    {
      if (MMGSDI_SUCCESS == mmgsdi_eons_copy_plmn_name_info(
                              &(rplmn_info_ptr->short_name),
                              &(plmn_info_ptr->plmn_short_name),
                              TRUE))
      {
        name_copied = TRUE;

        if (uim_selective_logging[UIM_LOG_EONS])
        {
          mmgsdi_util_print_data_to_diag("Short name data buffer",
            plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
            (uint16)plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len);
        }
      }
    }
    /* Copy PLMN name source from RPLMN */
    plmn_info_ptr->plmn_name_source = rplmn_info_ptr->rplmn_name_source;
  }

  /* Check if CSG id is valid and HNB name is available copy that as the plmn name */
  if((plmn_info_ptr->plmn_id.csg_id != MMGSDI_EONS_INVALID_CSG_ID) &&
     (plmn_info_ptr->plmn_id.csg_id == rplmn_info_ptr->csg_info.csg_id))
  {

    if((plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len > 0) &&
       (plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len <= SYS_HNB_NAME_LEN))
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "RPLMN name is available in CSG EONS info");

      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len =
        rplmn_info_ptr->csg_info.hnb_name.length;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len);

      if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
      {
        mmgsdi_memscpy(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
                       plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len,
                       rplmn_info_ptr->csg_info.hnb_name.name,
                       rplmn_info_ptr->csg_info.hnb_name.length);

        /* Encoding type: UCS2_PACKED or UCS2 normal */
        if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr[MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS] ==
             MMGSDI_UCS2_ENCODING_TYPE_1 ||
           plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr[MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS] ==
             MMGSDI_UCS2_ENCODING_TYPE_2 ||
           plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr[MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS] ==
             MMGSDI_UCS2_ENCODING_TYPE_3)
        {
          plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_UCS2_PACKED;
        }
        else
        {
          plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
            MMGSDI_EONS_ENC_UCS2;
        }
        name_copied = TRUE;
      }
    }
  }

  /* Point to the additional language PLMN names, if present */
  if(rplmn_info_ptr->num_additional_names > 0)
  {
    uint8 index = 0;

    plmn_info_ptr->num_plmn_additional_names = rplmn_info_ptr->num_additional_names;

    if(plmn_info_ptr->num_plmn_additional_names > MMGSDI_ADDITIONAL_NAMES_MAX)
    {
      plmn_info_ptr->num_plmn_additional_names = MMGSDI_ADDITIONAL_NAMES_MAX;
    }

    for(index = 0; index < plmn_info_ptr->num_plmn_additional_names; index++)
    {
      plmn_info_ptr->plmn_additional_names[index] =
        rplmn_info_ptr->additional_names[index];
    }
    name_copied = TRUE;
  }

  /* Copy additional info if available */
  if (rplmn_info_ptr->additional_info.info_encoding !=
        MMGSDI_EONS_ENC_INVALID &&
      rplmn_info_ptr->additional_info.info_data.data_len > 0  &&
      rplmn_info_ptr->additional_info.info_data.data_ptr != NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       plmn_info_ptr->plmn_additional_info.info_data.data_ptr,
       rplmn_info_ptr->additional_info.info_data.data_len);

    if (plmn_info_ptr->plmn_additional_info.info_data.data_ptr != NULL)
    {
      plmn_info_ptr->plmn_additional_info.info_data.data_len = 
        rplmn_info_ptr->additional_info.info_data.data_len;

      plmn_info_ptr->plmn_additional_info.info_encoding =
        rplmn_info_ptr->additional_info.info_encoding;

      mmgsdi_memscpy(plmn_info_ptr->plmn_additional_info.info_data.data_ptr,
                     plmn_info_ptr->plmn_additional_info.info_data.data_len,
                     rplmn_info_ptr->additional_info.info_data.data_ptr,
                     rplmn_info_ptr->additional_info.info_data.data_len);

      name_copied = TRUE;
    }
  }
  return name_copied;
} /* mmgsdi_eons_copy_rplmn_name */


/*===========================================================================
  FUNCTION MMGSDI_EONS_BUILD_PLMN_ID_LIST_WITH_RPLMN

DESCRIPTION
  Function to build PLMN ID List with RPLMN IDs

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_build_plmn_id_list_with_rplmn(
  boolean                         first_stack_rplmn_run_algo,
  boolean                         sec_stack_rplmn_run_algo,
  mmgsdi_eons_rplmn_info_type   * rplmn_info_ptr,
  mmgsdi_plmn_id_list_type      * plmn_id_list_ptr,
  mmgsdi_list_plmn_name_type    * plmn_name_array_ptr
)
{
  uint32                   size          = 0;

  /* If both 1st and 2nd stack RPLMNs have been updated and
     algo needs to be run then PLMN ID list shall contain 2
     entries else 1 */
  if (first_stack_rplmn_run_algo && sec_stack_rplmn_run_algo)
  {
    plmn_id_list_ptr->num_of_plmn_ids = 2;
  }
  else
  {
    plmn_id_list_ptr->num_of_plmn_ids = 1;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    plmn_id_list_ptr->plmn_list_ptr,
    (sizeof(mmgsdi_plmn_id_type) * plmn_id_list_ptr->num_of_plmn_ids));

  if (plmn_id_list_ptr->plmn_list_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* If the first stack is active and algo needs to be run copy the
     RPLMN into the plmn id temp variable and the 0th entry of the plmn id list.
     Increment the count of plmn ids and the size to be allocated
     for the name array pointer*/
  /* If the second stack is active we need to check if size is non-zero.
     This indicates whether there is already a first stack RPLMN to be
     processed. If so the second stack RPLMN is copied into the 1st entry
     of the plmn id list and count of plmn ids and size are suitably updated.
     If however there is no first stack RPLMN then simply plmn id is updated */
  if (first_stack_rplmn_run_algo)
  {
    mmgsdi_memscpy(&(plmn_id_list_ptr->plmn_list_ptr[0]),
                   sizeof(mmgsdi_plmn_id_type),
                   &(rplmn_info_ptr->first_stack_rplmn.rplmn_id),
                   sizeof(mmgsdi_plmn_id_type));

    plmn_name_array_ptr->num_of_plmn_ids++;

    size = sizeof(mmgsdi_plmn_info_type);
  }

  if (sec_stack_rplmn_run_algo)
  {
    if (size > 0 )
    {
      mmgsdi_memscpy(&(plmn_id_list_ptr->plmn_list_ptr[1]),
                     sizeof(mmgsdi_plmn_id_type),
                     &(rplmn_info_ptr->sec_stack_rplmn.rplmn_id),
                     sizeof(mmgsdi_plmn_id_type));
    }
    else
    {
      mmgsdi_memscpy(&(plmn_id_list_ptr->plmn_list_ptr[0]),
                     sizeof(mmgsdi_plmn_id_type),
                     &(rplmn_info_ptr->sec_stack_rplmn.rplmn_id),
                     sizeof(mmgsdi_plmn_id_type));
    }

    plmn_name_array_ptr->num_of_plmn_ids++;

    size += sizeof(mmgsdi_plmn_info_type);
  }

  /* Allocate memory for plmn name */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(plmn_name_array_ptr->plmn_info_ptr, size);
  if (plmn_name_array_ptr->plmn_info_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_eons_build_plmn_id_list_with_rplmn */


/*===========================================================================
  FUNCTION MMGSDI_EONS_UPDATE_RPLMN_NAME_AND_METHOD

DESCRIPTION
  Function to update RPLMN name based on name retrieved from running
  the EONS algorithm and the method used

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_update_rplmn_name_and_method(
  mmgsdi_eons_method_enum_type              eons_method,
  mmgsdi_eons_call_stack_enum_type          stack_type,
  boolean                                   rplmn_is_hplmn_spdi_plmn,
  uint8                                     display_byte,
  const mmgsdi_plmn_info_type             * plmn_info_ptr,
  mmgsdi_eons_info_type                   * eons_info_ptr
)
{
  mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr = NULL;
  mmgsdi_return_enum_type                  mmgsdi_status  = MMGSDI_SUCCESS;


  if (plmn_info_ptr == NULL || eons_info_ptr == NULL ||
      (stack_type != MMGSDI_EONS_FIRST_CALL_STACK &&
       stack_type != MMGSDI_EONS_SEC_CALL_STACK))
  {
    return MMGSDI_ERROR;
  }

  if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
  {
    rplmn_info_ptr = &(eons_info_ptr->rplmn_info.first_stack_rplmn);
  }
  else
  {
    rplmn_info_ptr = &(eons_info_ptr->rplmn_info.sec_stack_rplmn);
  }

  rplmn_info_ptr->eons_method = eons_method;
  rplmn_info_ptr->rplmn_name_source = plmn_info_ptr->plmn_name_source;

  if (rplmn_is_hplmn_spdi_plmn)
  {
    rplmn_info_ptr->is_home_network = TRUE;
  }
  else
  {
    rplmn_info_ptr->is_home_network = FALSE;
  }

  /* Do not display RPLMN name if rplmn_is_hplmn_spdi_plmn && bit1 == 0 */
  if (rplmn_is_hplmn_spdi_plmn                                               &&
      ((display_byte & MMGSDI_EONS_DISP_COND_RPLMN_BIT_POS) == 0))
  {
    rplmn_info_ptr->display_operator_info = FALSE;
  }
  else
  {
    rplmn_info_ptr->display_operator_info = TRUE;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    rplmn_info_ptr->long_name.plmn_name.eons_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    rplmn_info_ptr->short_name.plmn_name.eons_data.data_ptr);

  rplmn_info_ptr->long_name_available  = FALSE;
  rplmn_info_ptr->short_name_available = FALSE;
  rplmn_info_ptr->num_additional_names = 0;

  /* Reset additional info data in rplmn info */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    rplmn_info_ptr->additional_info.info_data.data_ptr);
  rplmn_info_ptr->additional_info.info_encoding =
    MMGSDI_EONS_ENC_INVALID;
  rplmn_info_ptr->additional_info.info_data.data_len =
    0;

  mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(
                    &(plmn_info_ptr->plmn_long_name),
                    &(rplmn_info_ptr->long_name),
                    TRUE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (rplmn_info_ptr->long_name.plmn_name.eons_data.data_len > 0 &&
      rplmn_info_ptr->long_name.plmn_name.eons_data.data_ptr != NULL)
  {
    rplmn_info_ptr->long_name_available = TRUE;
  }

  mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(
                       &(plmn_info_ptr->plmn_short_name),
                       &(rplmn_info_ptr->short_name),
                       TRUE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (rplmn_info_ptr->short_name.plmn_name.eons_data.data_len > 0 &&
      rplmn_info_ptr->short_name.plmn_name.eons_data.data_ptr != NULL)
  {
    rplmn_info_ptr->short_name_available = TRUE;
  }


  if (plmn_info_ptr->plmn_additional_info.info_encoding !=
        MMGSDI_EONS_ENC_INVALID &&
      plmn_info_ptr->plmn_additional_info.info_data.data_len > 0  &&
      plmn_info_ptr->plmn_additional_info.info_data.data_ptr != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
       rplmn_info_ptr->additional_info.info_data.data_ptr);

    rplmn_info_ptr->additional_info.info_encoding =
      plmn_info_ptr->plmn_additional_info.info_encoding;

    rplmn_info_ptr->additional_info.info_data.data_len =
      plmn_info_ptr->plmn_additional_info.info_data.data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       rplmn_info_ptr->additional_info.info_data.data_ptr,
       rplmn_info_ptr->additional_info.info_data.data_len);

    if (rplmn_info_ptr->additional_info.info_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(rplmn_info_ptr->additional_info.info_data.data_ptr,
                     rplmn_info_ptr->additional_info.info_data.data_len,
                     plmn_info_ptr->plmn_additional_info.info_data.data_ptr,
                     plmn_info_ptr->plmn_additional_info.info_data.data_len);
    }
    else
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /* Point to the additional language PLMN names, if present */
  if(plmn_info_ptr->num_plmn_additional_names > 0)
  {
    uint8 index = 0;

    rplmn_info_ptr->num_additional_names = plmn_info_ptr->num_plmn_additional_names;

    if(rplmn_info_ptr->num_additional_names > MMGSDI_ADDITIONAL_NAMES_MAX)
    {
      rplmn_info_ptr->num_additional_names = MMGSDI_ADDITIONAL_NAMES_MAX;
    }

    for(index = 0; index < rplmn_info_ptr->num_additional_names; index++)
    {
      rplmn_info_ptr->additional_names[index] =
        plmn_info_ptr->plmn_additional_names[index];
    }
  }

  return mmgsdi_status;
} /* mmgsdi_eons_update_rplmn_name_and_method */


/*===========================================================================
  FUNCTION MMGSDI_EONS_GET_HPLMN

DESCRIPTION
  Function to return HPLMN from EF-IMSI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_get_hplmn(
  mmgsdi_session_id_type            session_id,
  mmgsdi_app_enum_type              app_type,
  mmgsdi_plmn_id_type             * hplmn_id_ptr
)
{
  mmgsdi_data_type                  imsi_data;
  mmgsdi_data_type                  ad_data;
  mmgsdi_access_type                ad_file_access;
  mmgsdi_access_type                imsi_file_access;
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  uint8                             num_of_mnc_digit     = 0;

  if (hplmn_id_ptr == NULL)
  {
    return;
  }

  memset(&imsi_data,   0x00, sizeof(mmgsdi_data_type));
  memset(&ad_data,     0x00, sizeof(mmgsdi_data_type));

  ad_file_access.access_method     = MMGSDI_EF_ENUM_ACCESS;
  imsi_file_access.access_method   = MMGSDI_EF_ENUM_ACCESS;

  switch (app_type)
  {
    case MMGSDI_APP_SIM:
      ad_file_access.file.file_enum     = MMGSDI_GSM_AD;
      imsi_file_access.file.file_enum   = MMGSDI_GSM_IMSI;
      break;
    case MMGSDI_APP_USIM:
      ad_file_access.file.file_enum     = MMGSDI_USIM_AD;
      imsi_file_access.file.file_enum   = MMGSDI_USIM_IMSI;
      break;
    default:
      return;
  }

  /* Read EF-AD to determine HPLMN has 2-digit MNC or 3-digit MNC*/
  (void)mmgsdi_util_read_cache_file(session_id, &ad_file_access, &ad_data);

  /* Read EF-IMSI from cache*/
  mmgsdi_status = mmgsdi_util_read_cache_file(
                    session_id, &imsi_file_access, &imsi_data);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_get_num_of_mnc_digits(
                      imsi_data, ad_data, &num_of_mnc_digit);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Fill MCC Digit 1 and MCC Digit 2 */
      hplmn_id_ptr->plmn_id_val[0] =
        (byte)(((imsi_data.data_ptr[1] & 0xF0 ) / 0x10) +
               ((imsi_data.data_ptr[2] & 0x0F ) * 0x10));

      /* Fill MCC Digit 3 and if MNC has third digit fill MNC Digit 3 */
      if(num_of_mnc_digit == MMGSDIUTIL_MNC_3_DIGITS)
      {
        hplmn_id_ptr->plmn_id_val[1]
          = (byte)(((imsi_data.data_ptr[2] & 0xF0 ) / 0x10) +
                   ((imsi_data.data_ptr[4] & 0x0F ) * 0x10));
      }
      else
      {
        hplmn_id_ptr->plmn_id_val[1]
          = (byte)(((imsi_data.data_ptr[2] & 0xF0 ) / 0x10) | 0xF0);
      }

      /* Fill MNC Digit 1 and MNC Digit 2 */
      hplmn_id_ptr->plmn_id_val[2] = imsi_data.data_ptr[3];

      UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                               "HPLMN from IMSI, AD: 0x%x 0x%x 0x%x",
                               hplmn_id_ptr->plmn_id_val[0],
                               hplmn_id_ptr->plmn_id_val[1],
                               hplmn_id_ptr->plmn_id_val[2]);
    }
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(ad_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(imsi_data.data_ptr);
} /* mmgsdi_eons_get_hplmn */


/*===========================================================================
  FUNCTION MMGSDI_EONS_CHECK_IF_RPLMN_IS_HPLMN

DESCRIPTION
  Function to determine if RPLMN is HPLMN i.e. user is in home network

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_eons_check_if_rplmn_is_hplmn(
  mmgsdi_session_id_type            session_id,
  const mmgsdi_int_app_info_type  * app_info_ptr,
  mmgsdi_eons_call_stack_enum_type  stack_type
)
{
  mmgsdi_data_type                  spdi_data;
  mmgsdi_access_type                spdi_file_access;
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_app_enum_type              app_type             = MMGSDI_APP_NONE;
  boolean                           is_hplmn             = FALSE;

  if(app_info_ptr == NULL)
  {
    return FALSE;
  }

  app_type = app_info_ptr->app_data.app_type;
  memset(&spdi_data, 0x00, sizeof(mmgsdi_data_type));

  spdi_file_access.access_method   = MMGSDI_EF_ENUM_ACCESS;

  switch (app_type)
  {
    case MMGSDI_APP_SIM:
      spdi_file_access.file.file_enum   = MMGSDI_GSM_SPDI;
      break;
    case MMGSDI_APP_USIM:
      spdi_file_access.file.file_enum   = MMGSDI_USIM_SPDI;
      break;
    default:
      return FALSE;
  }

  /* To determine whether it is in home network or not,
    1. Check roaming status received in CM SS event.
    2. If raoming status is ON, rplmn is compared with
       plmns read from EF-SPDI */
  if(((stack_type == MMGSDI_EONS_FIRST_CALL_STACK) &&
      (app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.rplmn_available == FALSE)) ||
     ((stack_type == MMGSDI_EONS_SEC_CALL_STACK) &&
      (app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn.rplmn_available == FALSE)))
  {
    return FALSE;
  }

  /* If the NW ROAM Status is OFF, device is in home network, return */
  if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK &&
      app_info_ptr->eons_info.ss_info.first_stack_ss.roam_status == SYS_ROAM_STATUS_OFF)
  {
    return TRUE;
  }
  else if(stack_type == MMGSDI_EONS_SEC_CALL_STACK &&
          app_info_ptr->eons_info.ss_info.sec_stack_ss.roam_status == SYS_ROAM_STATUS_OFF)
  {
    return TRUE;
  }

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "NW ROAM Status is not OFF, trying spdi");

  mmgsdi_status = mmgsdi_util_read_cache_file(
                    session_id, &spdi_file_access, &spdi_data);

  if(mmgsdi_status == MMGSDI_SUCCESS && spdi_data.data_len > 0)
  {
    uint8                loop_count    = 0;
    uint16               index         = 0;
    uint16               disp_info_len = 0;
    uint16               plmn_list_len = 0;
    mmgsdi_plmn_id_type  nw_plmn_id;

    /* EFSPDI data is a constructed TLV coded according to
       ISO/IEC 8825-1. TLV with Tag 0x80 (Service provider
       PLMN list tag) is encapsulated inside TLV with Tag
       0xA3 (Service provider display information Tag).
       Length of TLV data can be greater than 127. If MSB
       of length byte is set to 0, remaining 7 bits denotes
       the length of TLV data bytes. If MSB of length byte
       is set to 1, remaining 7 bits denotes number of
       length octets next to it. We only support one length
       octet */

    do
    {
      /* Check if EF-SPDI first byte is 0xA3 */
      if(spdi_data.data_ptr[index] !=
         MMGSDI_EONS_SPDI_DISP_INFO_TAG)
      {
        UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                                "Invalid SPDI Tag : 0x%x at index : 0x%x",
                                spdi_data.data_ptr[index], index);
        break;
      }
      index++;

      /* Retrieve display info len */
      if (spdi_data.data_ptr[index] < 0x80)
      {
        disp_info_len = spdi_data.data_ptr[index];
        index++;
      }
      else if ((spdi_data.data_len > (index + 1)) &&
               (spdi_data.data_ptr[index] == 0x81))
      {
        /* 0x81 means next octet contains the length of TLV
           data bytes */
        disp_info_len = spdi_data.data_ptr[index + 1];
        index += 2;
      }
      else
      {
        /* If display info length octet is other than 0x81, we
           dont support */
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                                "SPDI display info length octet : 0x%x",
                                spdi_data.data_ptr[index]);
        break;
      }

      /* Check for spdi data length */
      if(spdi_data.data_len < (index + disp_info_len))
      {
        break;
      }

      /* Check if next byte in EF-SPDI is 0x80 */
      if(spdi_data.data_ptr[index] != MMGSDI_EONS_SPDI_PLMN_LIST_TAG)
      {
        UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                                "Invalid SPDI Tag : 0x%x at index : 0x%x",
                                spdi_data.data_ptr[index], index);
        break;
      }
      index++;

      /* Check for spdi data length */
      if(spdi_data.data_len <= index)
      {
        break;
      }

      /* Retrieve plmn list len */
      if (spdi_data.data_ptr[index] < 0x80)
      {
        plmn_list_len = spdi_data.data_ptr[index];
        index++;
      }
      else if ((spdi_data.data_len > (index + 1)) &&
               (spdi_data.data_ptr[index] == 0x81))
      {
        /* 0x81 means next octet contains the length of TLV
           data bytes */
        plmn_list_len = spdi_data.data_ptr[index + 1];
        index += 2;
      }
      else
      {
        /* If plmn list length octet is other than 0x81,
           we dont support */
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                                "SPDI plmn list length octet : 0x%x",
                                spdi_data.data_ptr[index]);
        break;
      }

      /* Plmn list len should be multiple of MMGSDI_PLMN_ID_SIZE */
      if((plmn_list_len % MMGSDI_PLMN_ID_SIZE) != 0)
      {
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                                "Invalid SPDI plmn list len: 0x%x",
                                plmn_list_len);
        break;
      }

      /* Check for spdi data length */
      if(spdi_data.data_len < (index + plmn_list_len))
      {
        break;
      }

      if(stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
      {
        nw_plmn_id =
          app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.rplmn_id;
      }
      else
      {
        nw_plmn_id =
          app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn.rplmn_id;
      }

      for(loop_count = 0;
          loop_count < (plmn_list_len / MMGSDI_PLMN_ID_SIZE);
          loop_count++)
      {
        mmgsdi_plmn_id_type temp_plmn;

        mmgsdi_memscpy(temp_plmn.plmn_id_val,
          sizeof(temp_plmn.plmn_id_val),
          &spdi_data.data_ptr[(loop_count * MMGSDI_PLMN_ID_SIZE) + index],
          MMGSDI_PLMN_ID_SIZE);

        /* If an spdi plmn matches with rplmn set is_home_network to TRUE */
        if (mmgsdi_eons_compare_sim_plmn_with_nw_plmn(temp_plmn, nw_plmn_id))
        {
          UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                                   "spdi record at index 0x%x matches with rplmn: home network",
                                   loop_count);
          is_hplmn = TRUE;
          break;
        }
      }
    }while(0);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(spdi_data.data_ptr);

  return is_hplmn;
} /* mmgsdi_eons_check_if_rplmn_is_hplmn */


/*===========================================================================
  FUNCTION MMGSDI_EONS_POPULATE_SPN_AND_DISPLAY_INFO

DESCRIPTION
  Function to read EF-SPN and populate SPN and display information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_populate_spn_and_display_info(
  mmgsdi_session_id_type                   session_id,
  boolean                                  fs_rplmn_is_hplmn_spdi_plmn,
  boolean                                  ss_rplmn_is_hplmn_spdi_plmn,
  mmgsdi_int_app_info_type               * app_info_ptr,
  uint8                                  * display_byte_ptr
)
{
  mmgsdi_data_type          spn_data;
  mmgsdi_access_type        spn_file_access;
  uint8                     spn_ef_len       = 0;
  unsigned char           * spn_ef_ptr       = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_app_enum_type      app_type         = MMGSDI_APP_NONE;
  mmgsdi_eons_info_type   * eons_info_ptr    = NULL;

  if (display_byte_ptr == NULL || app_info_ptr == NULL)
  {
    return;
  }

  app_type      = app_info_ptr->app_data.app_type;
  eons_info_ptr = &(app_info_ptr->eons_info);
  memset(&spn_data, 0x00, sizeof(mmgsdi_data_type));

  /*Read EF-SPN from cache*/
  spn_file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  if(app_type == MMGSDI_APP_SIM)
  {
    spn_file_access.file.file_enum = MMGSDI_GSM_SPN;
  }
  else
  {
    spn_file_access.file.file_enum = MMGSDI_USIM_SPN;
  }

  mmgsdi_status = mmgsdi_util_read_cache_file(
                    session_id, &spn_file_access, &spn_data);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (spn_data.data_len == 0))
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(spn_data.data_ptr);
    return;
  }

  *display_byte_ptr = spn_data.data_ptr[MMGSDI_EONS_SPN_DISP_COND_POS];

  /* Display SPN if:
     RPLMN matches with HPLMN or SPDI PLMN
     or
     display byte bit2 is 0
     */
  if((*display_byte_ptr & MMGSDI_EONS_DISP_COND_SPN_BIT_POS) == FALSE)
  {
    eons_info_ptr->display_spn = TRUE;
    eons_info_ptr->rplmn_info.first_stack_rplmn.display_spn_info = TRUE;
    eons_info_ptr->rplmn_info.sec_stack_rplmn.display_spn_info = TRUE;
  }
  else
  {
    if(fs_rplmn_is_hplmn_spdi_plmn == TRUE)
    {
      /* display_spn will be deprecated in the future as it is not stack specific
         for now only populated based on 1st stack info */
      eons_info_ptr->display_spn = TRUE;
      eons_info_ptr->rplmn_info.first_stack_rplmn.display_spn_info = TRUE;
    }
    else
    {
      eons_info_ptr->display_spn = FALSE;
      eons_info_ptr->rplmn_info.first_stack_rplmn.display_spn_info = FALSE;
    }

    if(ss_rplmn_is_hplmn_spdi_plmn == TRUE)
    {
      eons_info_ptr->rplmn_info.sec_stack_rplmn.display_spn_info = TRUE;
    }
    else
    {
      eons_info_ptr->rplmn_info.sec_stack_rplmn.display_spn_info = FALSE;
    }
  }

  eons_info_ptr->spn_display_bit =
    (*display_byte_ptr & MMGSDI_EONS_DISP_COND_SPN_BIT_POS) ?
      MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_1 : MMGSDI_EONS_SPN_DISPLAY_BIT_VAL_0;

  eons_info_ptr->rplmn_display_bit =
    (*display_byte_ptr & MMGSDI_EONS_DISP_COND_RPLMN_BIT_POS) ?
      MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_1 : MMGSDI_EONS_RPLMN_DISPLAY_BIT_VAL_0;

  /* Copy SPN data to app info*/
  spn_ef_ptr = (unsigned char *)memchr(
                                 &spn_data.data_ptr[MMGSDI_EONS_SPN_DATA_BUF_POS],
                                 0xFF, MMGSDI_EONS_SPN_DATA_LEN);


  /*if pointer is not NULL, an occurrence of 0xFF is found*/
  if(spn_ef_ptr)
  {
    /*Calculate length of SPN buffer before ocurrence of 0xFF*/
    spn_ef_len = (uint8)(spn_ef_ptr - (&spn_data.data_ptr[MMGSDI_EONS_SPN_DATA_BUF_POS]));
  }
  else
  {
    /*SPN Buffer does not have 0xFF, valid buffer length is 16*/
    spn_ef_len = MMGSDI_EONS_SPN_DATA_LEN;
  }

  if(spn_ef_len)
  {
    /*SPN Encoding*/
    if(spn_data.data_ptr[MMGSDI_EONS_SPN_DATA_BUF_POS] == MMGSDI_UCS2_ENCODING_TYPE_1 ||
      spn_data.data_ptr[MMGSDI_EONS_SPN_DATA_BUF_POS] == MMGSDI_UCS2_ENCODING_TYPE_2 ||
      spn_data.data_ptr[MMGSDI_EONS_SPN_DATA_BUF_POS] == MMGSDI_UCS2_ENCODING_TYPE_3 )
    {
      eons_info_ptr->spn.eons_encoding      = MMGSDI_EONS_ENC_UCS2_PACKED;
      eons_info_ptr->spn.eons_data.data_len = (uint8)MMGSDI_EONS_SPN_DATA_LEN;
    }
    else
    {
      eons_info_ptr->spn.eons_encoding      = MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;
      eons_info_ptr->spn.eons_data.data_len = spn_ef_len;
    }

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(eons_info_ptr->spn.eons_data.data_ptr);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      eons_info_ptr->spn.eons_data.data_ptr,
      (sizeof(uint8) * eons_info_ptr->spn.eons_data.data_len));

    /*SPN Data*/
    if(eons_info_ptr->spn.eons_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(eons_info_ptr->spn.eons_data.data_ptr,
                     eons_info_ptr->spn.eons_data.data_len,
                     &spn_data.data_ptr[MMGSDI_EONS_SPN_DATA_BUF_POS],
                     eons_info_ptr->spn.eons_data.data_len);
    }
    else
    {
      eons_info_ptr->spn.eons_data.data_len = 0;
    }
  }
  else
  {
    eons_info_ptr->spn.eons_encoding      = MMGSDI_EONS_ENC_INVALID;
    eons_info_ptr->spn.eons_data.data_len = 0;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(eons_info_ptr->spn.eons_data.data_ptr);
  }

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "spn display byte: 0x%x spn length: 0x%x encoding: 0x%x",
                           *display_byte_ptr,
                           eons_info_ptr->spn.eons_data.data_len,
                           eons_info_ptr->spn.eons_encoding);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(spn_data.data_ptr);
} /* mmgsdi_eons_populate_spn_and_display_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_RAT_TYPE_TO_SYS_MODE

DESCRIPTION
  Function to map  radio access technology to system mode

DEPENDENCIES
  None

RETURN VALUE
  SYS_SYS_MODE_E_TYPE

SIDE EFFECTS
  None
===========================================================================*/
sys_sys_mode_e_type mmgsdi_eons_map_rat_type_to_sys_mode(
  mmgsdi_rat_enum_type              rat_type
)
{
  sys_sys_mode_e_type  sys_mode = SYS_SYS_MODE_MAX;

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "map rat type 0x%x to sys mode",
                           rat_type);

  switch(rat_type)
  {
    case MMGSDI_RAT_GSM:
      sys_mode = SYS_SYS_MODE_GSM;
      break;

    case MMGSDI_RAT_UMTS:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;

    case MMGSDI_RAT_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;

    case MMGSDI_RAT_TDS:
      sys_mode = SYS_SYS_MODE_TDS;
      break;

    default:
      sys_mode = SYS_SYS_MODE_NONE;
      break;
  }
  return sys_mode;
}/* mmgsdi_eons_map_rat_type_to_sys_mode */


/*===========================================================================
FUNCTION MMGSDI_EONS_POPULATE_OPERATOR_INFO

DESCRIPTION
  Function to print PLMN INFO names

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_print_plmn_info_names(
  mmgsdi_plmn_info_type   * plmn_info_ptr
)
{
  if (plmn_info_ptr == NULL)
  {
    return;
  }

  if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len > 0 &&
     uim_selective_logging[UIM_LOG_EONS] != 0x00)
  {
    /* print full name data buffer*/
    mmgsdi_util_print_data_to_diag("plmn long name data buffer",
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
      (uint16)plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len);
  }

  /* This log was kept intentionally */
  UIM_MSG_HIGH_4("plmn full name length: 0x%x encoding: 0x%x short name length: 0x%x encoding: 0x%x",
                 plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len,
                 plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding,
                 plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len,
                 plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding);

  if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len > 0 &&
     uim_selective_logging[UIM_LOG_EONS] != 0x00)
  {
    /* print short name data buffer*/
    mmgsdi_util_print_data_to_diag("plmn short name data buffer",
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
      (uint16)plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len);
  }
}/* mmgsdi_eons_print_plmn_info_names */


/*==========================================================================
FUNCTION: MMGSDI_EONS_POPULATE_GET_OPERATOR_NAME_CNF

DESCRIPTION
  Function populates data for mmgsdi_session_get_operator_name_cnf_type:
  1. If there is only 1 PLMN ID is in the list and the PLMN ID is RPLMN,
     popluate the following:
      spn_display_bit:   value from the card
      rplmn_display_bit: value from the card
      roaming_status:
      SPN:  Populated when the condition specified by MMGSDI_EONS_POPULATE_SPN allows, i.e.
            (1) PLMN ID is NOT RPLMN
            or
            (2) combined condition of ignore_disp_cond and b2 value in EFSPN byte 1
  2. Populate PLMN name if condition specified by MMGSDI_EONS_POPULATE_PLMN_NAME allows, i.e.
            (1) PLMN ID is NOT RPLMN
            or
            (2) combined condition of ignore_disp_cond and b1 value in EFSPN byte 1
     Copy from RPLMN directly if available, o/w, return FALSE to run algorithm

PARAMETERS
  ignore_disp_cond:      mmgsdi_eons_ignore_disp_cond_enum_type
  eons_info_ptr:         points to mmgsdi_int_app_info_type.eons_info
  spn_ptr:               points to mmgsdi_session_get_operator_name_cnf_type.spn
  lang_ind_ptr:          points to mmgsdi_session_get_operator_name_cnf_type.spn_lang_ind
  disp_info_ptr:         points to mmgsdi_eons_display_info_type that is used to build
                         the spn_display_bit, rplmn_display_bit, and roaming_status in
                         mmgsdi_session_get_operator_name_cnf_type
  plmn_name_array_ptr:   points to mmgsdi_session_get_operator_name_cnf_type.plmn_list
  plmn_name_array_index: index in plmn_name_array_ptr->plmn_info_ptr
  stack_type:            stack where the RPLMN that matches to the PLMN ID at
                         plmn_name_array_index resides
  is_rplmn:              whether the PLMN ID at plmn_name_array_index is RPLMN
  is_hplmn_spdi:         whether the PLMN ID at plmn_name_array_index is either HPLMN
                         or SPDI PLMN

DEPENDENCIES
  None

RETURN VALUE
  TRUE:   information obtained, continue to next PLMN ID
  FALSE:  fall through and run the algorithm to get PLMN names

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_eons_populate_get_operator_name_cnf(
  mmgsdi_eons_ignore_disp_cond_enum_type   ignore_disp_cond,
  mmgsdi_eons_info_type                   *eons_info_ptr,
  mmgsdi_eons_name_type                   *spn_ptr,
  uint8                                   *lang_ind_ptr,
  mmgsdi_eons_display_info_type           *disp_info_ptr,
  mmgsdi_list_plmn_name_type              *plmn_name_array_ptr,
  uint32                                   plmn_name_array_index,
  mmgsdi_eons_call_stack_enum_type         stack_type,
  boolean                                  is_rplmn,
  boolean                                  is_hplmn_spdi
)
{
  if(!eons_info_ptr || !plmn_name_array_ptr)
  {
    return FALSE;
  }

  if(lang_ind_ptr != NULL)
  {
    *lang_ind_ptr = 0;
  }

  /* Populate mmgsdi_eons_display_info_type */
  if(disp_info_ptr && is_rplmn && (1 == plmn_name_array_ptr->num_of_plmn_ids))
  {
    disp_info_ptr->rplmn_display_bit = eons_info_ptr->rplmn_display_bit;
    disp_info_ptr->spn_display_bit   = eons_info_ptr->spn_display_bit;
    disp_info_ptr->roaming_status    = is_hplmn_spdi?
                                          MMGSDI_EONS_IN_HOME_NW:
                                          MMGSDI_EONS_IN_ROAMING_NW;
  }

  /* Populate SPN if:
     1. Only 1 PLMN ID is provided
     and
     2. Condition specified in MMGSDI_EONS_POPULATE_SPN allows SPN to be populated
     */
  if(spn_ptr                                     &&
     (1 == plmn_name_array_ptr->num_of_plmn_ids) &&
     (TRUE == MMGSDI_EONS_POPULATE_SPN(is_rplmn,
                                       is_hplmn_spdi,
                                       ignore_disp_cond,
                                       eons_info_ptr->spn_display_bit)))
  {
    spn_ptr->eons_encoding      = eons_info_ptr->spn.eons_encoding;
    spn_ptr->eons_data.data_len = eons_info_ptr->spn.eons_data.data_len;

    if(eons_info_ptr->spn.eons_encoding != MMGSDI_EONS_ENC_INVALID &&
       spn_ptr->eons_data.data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(spn_ptr->eons_data.data_ptr,
                                         spn_ptr->eons_data.data_len);

      if(spn_ptr->eons_data.data_ptr != NULL)
      {
        mmgsdi_memscpy(spn_ptr->eons_data.data_ptr,
                       spn_ptr->eons_data.data_len,
                       eons_info_ptr->spn.eons_data.data_ptr,
                       spn_ptr->eons_data.data_len);

        if (uim_selective_logging[UIM_LOG_EONS])
        {
          /* print spn data buffer*/
          mmgsdi_util_print_data_to_diag("spn data buffer",
            spn_ptr->eons_data.data_ptr,
            (uint16)spn_ptr->eons_data.data_len);
        }
      }
      else
      {
        spn_ptr->eons_data.data_len = 0;
      }
    }
  }/* Populate SPN */

  /* Populate PLMN name */
  if(TRUE == MMGSDI_EONS_POPULATE_PLMN_NAME(is_rplmn,
                                            is_hplmn_spdi,
                                            ignore_disp_cond,
                                            eons_info_ptr->rplmn_display_bit))
  {
    /* If EONS is retrieved for the RPLMN (either macro or femto cell),
       copy from stored RPLMN name and go to the next PLMN ID
       In case it is not retrieved, run the respective algorithms
       (macro or femto cell) and retrieve the EONS */
    if(is_rplmn)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "RPLMN, copy name directly");
      return mmgsdi_eons_copy_rplmn_name(
                    stack_type,
                    eons_info_ptr,
                    &(plmn_name_array_ptr->plmn_info_ptr[plmn_name_array_index]));

    }
    else
    {
      /* Not RPLMN, run algorithm to retrieve names */
      return FALSE;
    }
  }/* Populate PLMN name */

  /* Continue to next PLMN ID */
  return TRUE;
}/* mmgsdi_eons_populate_eons_cnf_data */


/*==========================================================================
FUNCTION MMGSDI_EONS_CHECK_AND_POPULATE_EMPTY_NAME

DESCRIPTION
  Function to check if either of the long/short names is empty and if it is,
  copies the non-empty name to the empty name buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
static void mmgsdi_eons_check_and_populate_empty_name(
  mmgsdi_plmn_info_type * plmn_info_ptr)
{
  mmgsdi_plmn_name_type     *src_plmn_name_ptr = NULL;
  mmgsdi_plmn_name_type     *dst_plmn_name_ptr = NULL;

  if(plmn_info_ptr == NULL)
  {
    return;
  }

  if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len == 0 &&
     plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len > 0)
  {
    /* longname is empty... we will be copying shortname to longname */
    src_plmn_name_ptr = &plmn_info_ptr->plmn_short_name;
    dst_plmn_name_ptr = &plmn_info_ptr->plmn_long_name;
  }
  else if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len == 0 &&
          plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len > 0)
  {
    /* shortname is empty... we will be copying longname to shortname */
    src_plmn_name_ptr = &plmn_info_ptr->plmn_long_name;
    dst_plmn_name_ptr = &plmn_info_ptr->plmn_short_name;
  }
  else
  {
    /* Both short and long names are either both empty or are both non-empty...
       no action to perform in either case */
    return;
  }

  /* Ensure data_ptr is NULL before allocating it in
     mmgsdi_eons_copy_plmn_name_info()*/
  dst_plmn_name_ptr->plmn_name.eons_data.data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(dst_plmn_name_ptr->plmn_name.eons_data.data_ptr);

  (void) mmgsdi_eons_copy_plmn_name_info(src_plmn_name_ptr,
                                         dst_plmn_name_ptr,
                                         TRUE);
} /* mmgsdi_eons_check_and_populate_empty_name */


/*==========================================================================
FUNCTION MMGSDI_EONS_RUN_ALGO

DESCRIPTION
  Function provides service provider name and operator name string.
  For service provider name it reads EF-SPN.
  For operator name string it runs enhanced operator name lookup algorithm.
  EONS algorithm has following priority of name source.
  1. EF-OPL and EF-PNN Look up
  2. EF-ONS and EF-ONS-SHORT Look up  as per CPHS specification (B.4.1.2)
  3. NITZ information Look up, NITZ info is received in CM SS Event
  4. Search UI database which contains mapping between MCC-MNC and Network name
  5. Provide MCC-MNC ascii string as operator name string.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo(
  mmgsdi_session_id_type                         session_id,
  mmgsdi_eons_trigger_enum_type                  eons_trigger,
  mmgsdi_plmn_id_list_type                       plmn_id_list,
  mmgsdi_eons_ignore_disp_cond_enum_type         ignore_display_condition,
  mmgsdi_eons_name_type                        * spn_ptr,
  uint8                                        * lang_ind_ptr,
  mmgsdi_list_plmn_name_type                   * plmn_name_array_ptr,
  mmgsdi_eons_display_info_type                * display_info_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status                = MMGSDI_ERROR;
  mmgsdi_int_app_info_type         * int_app_info_ptr             = NULL;
  mmgsdi_eons_info_type            * eons_info_ptr                = NULL;
  mmgsdi_session_type_enum_type      session_type                 = MMGSDI_MAX_SESSION_TYPE_ENUM;
  uint8                              display_byte                 = 0xFF;
  uint32                             loop_count                   = 0;
  boolean                            rplmn_is_hplmn_spdi_plmn     = FALSE;
  boolean                            fs_rplmn_is_hplmn_spdi_plmn  = FALSE;
  boolean                            ss_rplmn_is_hplmn_spdi_plmn  = FALSE;
  mmgsdi_plmn_id_type                hplmn_id;
  boolean                            first_stack_rplmn_run_algo   = FALSE;
  boolean                            sec_stack_rplmn_run_algo     = FALSE;
  boolean                            fs_plmn_id_lac_rat_changed   = FALSE;
  boolean                            ss_plmn_id_lac_rat_changed   = FALSE;
  boolean                            allow_run_algo               = TRUE;
  mmgsdi_feature_status_enum_type    opl_pnn_lookup_for_rplmn     = MMGSDI_FEATURE_DISABLED;

  /* This log is kept intentionally */
  UIM_MSG_HIGH_1("Run EONS algo for trigger 0x%x", eons_trigger);

  if(eons_trigger != MMGSDI_EONS_TRIGGER_REFRESH_FCN &&
     eons_trigger != MMGSDI_EONS_TRIGGER_SS_EVENT &&
     eons_trigger != MMGSDI_EONS_TRIGGER_API_CALL &&
     eons_trigger != MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE)
  {
    return MMGSDI_ERROR;
  }

  if((MMGSDI_SESSION_ID_UNASSIGNED == mmgsdi_util_is_session_id_valid(session_id)) ||
     (plmn_id_list.num_of_plmn_ids > 0 &&
      (plmn_id_list.plmn_list_ptr == NULL)))
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(session_id, &session_type, NULL);

  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "Session type: 0x%x, status: 0x%x",
                           session_type, mmgsdi_status);

  if((MMGSDI_SUCCESS != mmgsdi_status) || 
     (!mmgsdi_util_is_prov_session(session_type)))
  {
    return MMGSDI_ERROR;
  }

  /* Get EONS info handle */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    NULL,
                    NULL);

  if(int_app_info_ptr != NULL && MMGSDI_SUCCESS == mmgsdi_status)
  {
    eons_info_ptr = &(int_app_info_ptr->eons_info);
  }
  else
  {
    eons_info_ptr =
      &(mmgsdi_generic_data_ptr->mmgsdi_eons_global_info);
  }

  if(session_type == MMGSDI_1X_PROV_PRI_SESSION ||
     session_type == MMGSDI_1X_PROV_SEC_SESSION ||
     session_type == MMGSDI_1X_PROV_TER_SESSION)
  {
    if (int_app_info_ptr == NULL)
    {
      UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS,
                              "No active 1x session. Cannot run eons algo");
      return MMGSDI_ERROR;
    }

    /* For SS event trigger copy over service status and roaming status */
    if (eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT)
    {
      mmgsdi_eons_update_cdma_serving_sys_info(eons_info_ptr);
    }

    if (plmn_name_array_ptr != NULL)
    {
      plmn_name_array_ptr->num_of_plmn_ids = 0;
    }

    return mmgsdi_eons_run_algo_for_cdma(
             session_id,
             eons_info_ptr,
             int_app_info_ptr->app_data.app_type,
             ignore_display_condition,
             spn_ptr,
             lang_ind_ptr,
             display_info_ptr);
  }

  /* Rest of the code is for running EONS algo for GW sessions */

  /* Check if algo is triggered with valid plmn name array ptr */
  if(plmn_name_array_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Check all valid OPL PNN records are cached or not if APP state is READY.
     Because OPL-PNN caching starts only when APP state moves to READY */
  if(int_app_info_ptr != NULL &&
     int_app_info_ptr->app_state == MMGSDI_APP_STATE_READY)
  {
    allow_run_algo = mmgsdi_eons_opl_pnn_cache_ready(session_type);
  }

  opl_pnn_lookup_for_rplmn =
    mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_OPL_PNN_LOOKUP_FOR_NON_RPLMN, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  /* If the algorithm is being run due to an SS event trigger then
     we do the following:
     1. Check that at least one of the stacks has valid RPLMN info
     2. If the RPLMN identifiers such as PLMN ID or LAC or CSG ID
        have changed copy them
     3. Check whether algorithm needs to be run again or name is
        already available, event needs to be sent or not necessary
     4. Update the service status of the RPLMN info
     5. If the algorithm has to be run build a PLMN ID list
        based on the PLMN IDs of valid RPLMNs */
  if (eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT)
  {
    boolean first_stack_rplmn_updated   = FALSE;
    boolean sec_stack_rplmn_updated     = FALSE;
    boolean first_stack_rplmn_send_evt  = FALSE;
    boolean sec_stack_rplmn_send_evt    = FALSE;

    /* If SS event has been triggered for 3GPP RAT and neither 1st nor 2nd
       stack has an RPLMN and LAC available there is nothing to do */
    if (((eons_info_ptr->ss_info.first_stack_ss.is_valid &&
          eons_info_ptr->ss_info.first_stack_ss.nitz_info.rplmn_available &&
          eons_info_ptr->ss_info.first_stack_ss.nitz_info.lac_available) ||
         (eons_info_ptr->ss_info.sec_stack_ss.is_valid &&
          eons_info_ptr->ss_info.sec_stack_ss.nitz_info.rplmn_available &&
          eons_info_ptr->ss_info.sec_stack_ss.nitz_info.lac_available)) == FALSE)
    {
      UIM_MSG_MED_0("SS event trigger with no valid NITZ info. Skip algo");
      plmn_name_array_ptr->num_of_plmn_ids = 0;
      plmn_name_array_ptr->plmn_info_ptr = NULL;
      return MMGSDI_ERROR;
    }

    /* Copy over any change in RPLMN ids */
    mmgsdi_eons_nitz_update_rplmn_identifiers(
      eons_info_ptr,
      &first_stack_rplmn_updated,
      &sec_stack_rplmn_updated,
      &fs_plmn_id_lac_rat_changed,
      &ss_plmn_id_lac_rat_changed);

    /* Determine if algo needs to be run, event needs to be sent
       and update the first event sent and service status */
    if (eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_available)
    {
      mmgsdi_eons_nitz_check_run_algo_send_evt_needed(
         &(eons_info_ptr->ss_info.first_stack_ss),
         first_stack_rplmn_updated,
         &(eons_info_ptr->rplmn_info.first_stack_rplmn),
         &first_stack_rplmn_run_algo,
         &first_stack_rplmn_send_evt);

      eons_info_ptr->rplmn_info.first_stack_rplmn.srv_status =
        eons_info_ptr->ss_info.first_stack_ss.srv_status;
    }

    if (eons_info_ptr->rplmn_info.sec_stack_rplmn.rplmn_available)
    {
      mmgsdi_eons_nitz_check_run_algo_send_evt_needed(
         &(eons_info_ptr->ss_info.sec_stack_ss),
         sec_stack_rplmn_updated,
         &(eons_info_ptr->rplmn_info.sec_stack_rplmn),
         &sec_stack_rplmn_run_algo,
         &sec_stack_rplmn_send_evt);

      eons_info_ptr->rplmn_info.sec_stack_rplmn.srv_status =
        eons_info_ptr->ss_info.sec_stack_ss.srv_status;
    }

    /* If OPL PNN cache is not ready, return MMGSDI NOT INIT */
    if(allow_run_algo == FALSE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "Do not run EONS algo as OPL PNN cache is not ready");
      return MMGSDI_NOT_INIT;
    }

    if (first_stack_rplmn_run_algo == FALSE &&
        sec_stack_rplmn_run_algo   == FALSE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "SS event trigger with no change in RPLMN ids. Skip algo");
      plmn_name_array_ptr->num_of_plmn_ids = 0;
      plmn_name_array_ptr->plmn_info_ptr = NULL;

      if (first_stack_rplmn_send_evt || sec_stack_rplmn_send_evt)
      {
        return MMGSDI_SUCCESS;
      }
      else
      {
        return MMGSDI_ERROR;
      }
    }

    mmgsdi_status = mmgsdi_eons_build_plmn_id_list_with_rplmn(
                      first_stack_rplmn_run_algo,
                      sec_stack_rplmn_run_algo,
                      &(eons_info_ptr->rplmn_info),
                      &(plmn_id_list),
                      plmn_name_array_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(plmn_id_list.plmn_list_ptr);
      return MMGSDI_ERROR;
    }
  }
  /* If the algorithm is being run due to an OPL PNN cache or FCN trigger
     we do the following:
   1. Check that at least one of the stacks has valid RPLMN info
   2. Check whether algorithm needs to be run again or if the name is
      already available from the OPL PNN method if so return
   3. If the algorithm has to be run build a PLMN ID list
      based on the PLMN IDs of valid RPLMNs */
  else if(eons_trigger == MMGSDI_EONS_TRIGGER_REFRESH_FCN ||
          eons_trigger == MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE)
  {
    first_stack_rplmn_run_algo =
      eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_available;

    sec_stack_rplmn_run_algo =
      eons_info_ptr->rplmn_info.sec_stack_rplmn.rplmn_available;

    if (first_stack_rplmn_run_algo == FALSE &&
        sec_stack_rplmn_run_algo   == FALSE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "RPLMN not available so algo not run");
      plmn_name_array_ptr->num_of_plmn_ids = 0;
      plmn_name_array_ptr->plmn_info_ptr = NULL;
      return MMGSDI_ERROR;
    }

    /* If OPL PNN cache is not ready, return MMGSDI NOT INIT */
    if(allow_run_algo == FALSE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "Do not run EONS algo as OPL PNN cache is not ready");
      return MMGSDI_NOT_INIT;
    }

    /*If eons algo is triggered after completion of opl pnn cache and previously
      followed eons method is OPL PNN lookup, algo is not run again */
    if (first_stack_rplmn_run_algo &&
        eons_trigger == MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE &&
        eons_info_ptr->rplmn_info.first_stack_rplmn.eons_method ==
          MMGSDI_EONS_METHOD_OPL_PNN_LOOKUP)
    {
      first_stack_rplmn_run_algo = FALSE;
    }

    if(sec_stack_rplmn_run_algo &&
       eons_trigger == MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE &&
       eons_info_ptr->rplmn_info.sec_stack_rplmn.eons_method ==
         MMGSDI_EONS_METHOD_OPL_PNN_LOOKUP)
    {
      sec_stack_rplmn_run_algo = FALSE;
    }

    if (first_stack_rplmn_run_algo == FALSE &&
        sec_stack_rplmn_run_algo   == FALSE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "Last method was OPL-PNN lookup. Algo not run");
      return MMGSDI_ERROR;
    }

    mmgsdi_status = mmgsdi_eons_build_plmn_id_list_with_rplmn(
                      first_stack_rplmn_run_algo,
                      sec_stack_rplmn_run_algo,
                      &(eons_info_ptr->rplmn_info),
                      &(plmn_id_list),
                      plmn_name_array_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(plmn_id_list.plmn_list_ptr);
      return MMGSDI_ERROR;
    }
  }
  /* If the algorithm is being run due to an API call trigger
     we ensure input has non-zero PLMN ID list size and run algo */
  else
  {
    if(plmn_id_list.plmn_list_ptr == NULL ||
       plmn_id_list.num_of_plmn_ids == 0)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                               "API trigger with no plmn id list so algo not run");
      plmn_name_array_ptr->num_of_plmn_ids = 0;
      plmn_name_array_ptr->plmn_info_ptr = NULL;
      return MMGSDI_ERROR;
    }

    if(allow_run_algo == FALSE)
    {
      /* Return ERROR if MMGSDI_FEATURE_OPL_PNN_LOOKUP_FOR_NON_RPLMN enabled :
         If feature is enabled, we use OPL-PNN lookup for non-rplmn ids also.
         And if cache is not ready we would be giving name from SE13 table which
         might be retrieved from PNN cache if it is ready */
      if(opl_pnn_lookup_for_rplmn == MMGSDI_FEATURE_ENABLED)
      {
        plmn_name_array_ptr->num_of_plmn_ids = 0;
        plmn_name_array_ptr->plmn_info_ptr = NULL;
        UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                                 "Do not run EONS algo as OPL PNN cache is not ready");
        return MMGSDI_NOT_INIT;
      }

      /* Return ERROR if any PLMN ID is matching with RPLMN ID because OPL-PNN
         cache is required in OPL-PNN lookup for RPLMN ID */
      for (loop_count = 0; loop_count < plmn_id_list.num_of_plmn_ids; loop_count++)
      {
        if(mmgsdi_eons_match_plmn_id_to_rplmn_id(
             eons_info_ptr,
             FALSE,
             &(plmn_id_list.plmn_list_ptr[loop_count]),
             TRUE,
             NULL))
        {
          break;
        }
      }

      if(loop_count != plmn_id_list.num_of_plmn_ids)
      {
        plmn_name_array_ptr->num_of_plmn_ids = 0;
        plmn_name_array_ptr->plmn_info_ptr = NULL;
        UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                                 "Do not run EONS algo as OPL PNN cache is not ready");
        return MMGSDI_NOT_INIT;
      }
    }

    plmn_name_array_ptr->num_of_plmn_ids = plmn_id_list.num_of_plmn_ids;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      plmn_name_array_ptr->plmn_info_ptr,
      (sizeof(mmgsdi_plmn_info_type) * plmn_id_list.num_of_plmn_ids));

    if(plmn_name_array_ptr->plmn_info_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  memset(hplmn_id.plmn_id_val, 0xFF, MMGSDI_PLMN_ID_SIZE);
  /* First we check if the RPLMN is an HPLMN. To determine if the RPLMN is
     an HPLMN, we first check roam status received in CM SS event. If roam
     status is ON, RPLMN is compared with PLMN IDs stored in EF-SPDI.
     If one of them matches then it is a home network and this information
     along with the display conditions provided by the caller are used
     determine the display conditions of the SPN and operator name */
  if(int_app_info_ptr != NULL)
  {
    mmgsdi_eons_get_hplmn(session_id, int_app_info_ptr->app_data.app_type,
                          &hplmn_id);
    fs_rplmn_is_hplmn_spdi_plmn = mmgsdi_eons_check_if_rplmn_is_hplmn(
                                    session_id,
                                    int_app_info_ptr,
                                    MMGSDI_EONS_FIRST_CALL_STACK);
    UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                             "Is first stack rplmn matches with hplmn or SPDI plmn: 0x%x",
                             fs_rplmn_is_hplmn_spdi_plmn);

    ss_rplmn_is_hplmn_spdi_plmn = mmgsdi_eons_check_if_rplmn_is_hplmn(
                                    session_id,
                                    int_app_info_ptr,
                                    MMGSDI_EONS_SEC_CALL_STACK);
    UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                             "Is second stack rplmn matches with hplmn or SPDI plmn: 0x%x",
                             ss_rplmn_is_hplmn_spdi_plmn);

    mmgsdi_eons_populate_spn_and_display_info(
      session_id,
      fs_rplmn_is_hplmn_spdi_plmn,
      ss_rplmn_is_hplmn_spdi_plmn,
      int_app_info_ptr,
      &display_byte);
  }

  mmgsdi_status = MMGSDI_SUCCESS;

  /* For each PLMN ID in the list check:
     1. If it's an RPLMN and a name is available if so copy it over
     2. If a name is not available run through the EONS algo and
        find a name from the best possible source */

  for (loop_count = 0; loop_count < plmn_id_list.num_of_plmn_ids; loop_count++)
  {
    boolean                              is_ons_retrieved     = FALSE;
    boolean                              is_plmn_rplmn        = FALSE;
    mmgsdi_eons_method_enum_type         eons_method          = MMGSDI_EONS_METHOD_MAX;
    mmgsdi_eons_call_stack_enum_type     stack_type           = MMGSDI_EONS_MAX_CALL_STACK;

    UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                             "Get ONS for plmn id: 0x%x, 0x%x, 0x%x",
                             plmn_id_list.plmn_list_ptr[loop_count].plmn_id_val[0],
                             plmn_id_list.plmn_list_ptr[loop_count].plmn_id_val[1],
                             plmn_id_list.plmn_list_ptr[loop_count].plmn_id_val[2]);

    mmgsdi_memscpy(&(plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id),
                   sizeof(mmgsdi_plmn_id_type),
                   &(plmn_id_list.plmn_list_ptr[loop_count]),
                   sizeof(mmgsdi_plmn_id_type));

    plmn_name_array_ptr->plmn_info_ptr[loop_count].lac = 0xFFFF;

    if(mmgsdi_eons_match_plmn_id_to_rplmn_id(
         eons_info_ptr,
         FALSE,
         &(plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id),
         TRUE,
         &stack_type))
    {
      is_plmn_rplmn = TRUE;

      if(stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
      {
        rplmn_is_hplmn_spdi_plmn = fs_rplmn_is_hplmn_spdi_plmn;
      }
      else if(stack_type == MMGSDI_EONS_SEC_CALL_STACK)
      {
        rplmn_is_hplmn_spdi_plmn = ss_rplmn_is_hplmn_spdi_plmn;
      }

      mmgsdi_eons_copy_rplmn_lac(
         stack_type,
         eons_info_ptr,
         &(plmn_name_array_ptr->plmn_info_ptr[loop_count]));
    }

    if(eons_trigger == MMGSDI_EONS_TRIGGER_API_CALL)
    {
      /* For API call:
         1. Populate SPN, display bits, and language indicator
         2. If the name for the PLMN ID needs to be populated and the PLMN is
            RPLMN, populate directly.
            If name is not found, proceed to execute the algorithm
      */

      /* In limited/limited regional service we will not identify PLMN present
         in CM SS event as RPLMN, we will always return RPLMN/SPN name in API
         cnf. It is same as ignoring RPLMN-SPN display conditions */
      if((stack_type == MMGSDI_EONS_FIRST_CALL_STACK &&
          (eons_info_ptr->ss_info.first_stack_ss.srv_status == SYS_SRV_STATUS_LIMITED ||
           eons_info_ptr->ss_info.first_stack_ss.srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL)) ||
         (stack_type == MMGSDI_EONS_SEC_CALL_STACK &&
          (eons_info_ptr->ss_info.sec_stack_ss.srv_status == SYS_SRV_STATUS_LIMITED ||
           eons_info_ptr->ss_info.sec_stack_ss.srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL)) )
      {
        ignore_display_condition = MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND;
      }

      if(mmgsdi_eons_populate_get_operator_name_cnf(
                                           ignore_display_condition,
                                           eons_info_ptr,
                                           spn_ptr,
                                           lang_ind_ptr,
                                           display_info_ptr,
                                           plmn_name_array_ptr,
                                           loop_count,
                                           stack_type,
                                           is_plmn_rplmn,
                                           rplmn_is_hplmn_spdi_plmn))
      {
        continue;
      }
    }

    /* Check if the HNB name has to be provided */
    if(plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id.csg_id !=
         MMGSDI_EONS_INVALID_CSG_ID)
    {
      mmgsdi_status = mmgsdi_eons_run_algo_csg(
                        stack_type,
                        session_id,
                        eons_trigger,
                        &(plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id),
                        plmn_name_array_ptr,
                        loop_count,
                        (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)?
                        fs_plmn_id_lac_rat_changed:ss_plmn_id_lac_rat_changed);

      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                               "mmgsdi_eons_run_algo_csg, status 0x%x",
                               mmgsdi_status);

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* If we could only retrieve one of the names (long or short) via the
           look-up method for this CSG plmn-id and the other one is empty, copy
           over the non-empty long/short name as both a long and a short name.
           This is done to avoid sending, say, a non-empty longname but an empty
           shortname to MMGSDI clients such as QMINAS */
        mmgsdi_eons_check_and_populate_empty_name(
          &(plmn_name_array_ptr->plmn_info_ptr[loop_count]));
        continue;
      }
      else
      {
        /* Reset the status and fall back to EONS corresponding to macro cell */
        mmgsdi_status = MMGSDI_SUCCESS;
      }
    }

    /*---------------------------------------------------------------------
      Method OPL PNN Lookup: Perform OPL-PNN lookup only when PNN record(s)
      are available and plmn equals to rplmn
      a. Compare plmn with opl records' plmn if comparision succeeds:
         1. Compare LAC. If comparsion succeeds, read PNN record from index
            associated with opl record.
      b. If plmn -opl records'plmn comparision or LAC comparision fails
          1. Compare plmn with hplmn (extracted from EF-IMSI),
              if comparision succeeds, read PNN record at index 0.
          2. Compare plmn with ehplmns (read from EF-EHPLMN)
              if comparision succeeds, read PNN record at index 0.
    ---------------------------------------------------------------------*/
    if (int_app_info_ptr != NULL &&
        (is_plmn_rplmn ||
         opl_pnn_lookup_for_rplmn == MMGSDI_FEATURE_ENABLED))
    {
      if (TRUE == mmgsdi_eons_get_opl_pnn_ons(
                    session_id,
                    session_type,
                    int_app_info_ptr,
                    stack_type,
                    is_plmn_rplmn,
                    hplmn_id,
                    &(plmn_name_array_ptr->plmn_info_ptr[loop_count])))
      {
        is_ons_retrieved = TRUE;
        eons_method = MMGSDI_EONS_METHOD_OPL_PNN_LOOKUP;
        plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_name_source =
          MMGSDI_EONS_NAME_SOURCE_OPL_PNN;
        /* This log is kept intentionally */
        UIM_MSG_HIGH_0("ONS retrieved from OPL-PNN lookup");
      }
    }

    if(int_app_info_ptr != NULL &&
       is_ons_retrieved == FALSE && is_plmn_rplmn &&
       ((stack_type == MMGSDI_EONS_FIRST_CALL_STACK &&
         int_app_info_ptr->eons_info.ss_info.first_stack_ss.roam_status == SYS_ROAM_STATUS_OFF) ||
        (stack_type == MMGSDI_EONS_SEC_CALL_STACK &&
         int_app_info_ptr->eons_info.ss_info.sec_stack_ss.roam_status == SYS_ROAM_STATUS_OFF)))
    {
      /*---------------------------------------------------------------------
      Method CPHS ONS Lookup:
      Copy plmn names from CPHS ONS or ONS SHORT if available
      CPHS ONS and CPHS ONS SHORT is cached while CPHS Intialization.
      CPHS ONS SHORT is only cached if its service is allocated and activated
    ---------------------------------------------------------------------*/
      if(TRUE == mmgsdi_eons_get_cphs_ons(
                   session_type,
                   &(plmn_name_array_ptr->plmn_info_ptr[loop_count])))
      {
        is_ons_retrieved = TRUE;
        eons_method = MMGSDI_EONS_METHOD_CPHS_ONS_LOOKUP;
        plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_name_source =
          MMGSDI_EONS_NAME_SOURCE_CPHS_ONS;
        /* This log is kept intentionally */
        UIM_MSG_HIGH_0("ONS retrieved from CPHS-ONS lookup");
      }
    }

    if((is_ons_retrieved == FALSE) && is_plmn_rplmn)
    {
      /*-------------------------------------------------------------------
        Method NITZ Lookup:
        1. Copy plmn names from NITZ info if full name or short
           name is present in nitz info and update rplmn_info.txt
        2. If full name or short name is not present in nitz info,
           read rplmn_info.txt and compare stored plmn with rplmn
           if matches copy plmn names from rplmn_info.txt.
      -------------------------------------------------------------------*/
      if(TRUE == mmgsdi_eons_get_nitz_ons(
                   stack_type,
                   session_type,
                   eons_info_ptr,
                   &(plmn_name_array_ptr->plmn_info_ptr[loop_count]),
                   hplmn_id))
      {
        is_ons_retrieved = TRUE;
        eons_method = MMGSDI_EONS_METHOD_NITZ_LOOKUP;
        plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_name_source =
          MMGSDI_EONS_NAME_SOURCE_NITZ;
        /* This log is kept intentionally */
        UIM_MSG_HIGH_0("ONS retrieved from NITZ lookup");
      }
    }

    if(is_ons_retrieved == FALSE)
    {
      sys_sys_mode_e_type    sys_mode = SYS_SYS_MODE_MAX;

      /* Method SE13 lookup: Copy plmn names from static network info
         table if MCC/MNC and sys mode match with any record of table*/
      sys_mode = mmgsdi_eons_map_rat_type_to_sys_mode(
                   plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id.rat);

      if(TRUE == mmgsdi_eons_get_se13_table_ons(
                   &(plmn_name_array_ptr->plmn_info_ptr[loop_count]),
                   plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id,
                   sys_mode))
      {
        is_ons_retrieved = TRUE;
        eons_method = MMGSDI_EONS_METHOD_STATIC_TABLE_LOOKUP;
        plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_name_source =
          MMGSDI_EONS_NAME_SOURCE_SE13;
        /* This log is kept intentionally */
        UIM_MSG_HIGH_2("ONS retrieved from SE13 static table lookup, full name len: 0x%x short name len: 0x%x",
                       plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_len,
                       plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_len);
      }

      /* Copy the PLMN names from additional languages static table, if
         applicable (that is, if MCC, MNC and sys mode match).
         It is very possible that there was no English name found in the
         regular hardcoded SE13 table above but the additional language
         name was found in the addl_lang_SE13 hardcoded table. In that case,
         English names will be compiled from MCC-MNC whereas other language
         names will be from the hardcoded additional language SE13 table */
      (void) mmgsdi_eons_get_se13_addl_lang_table_ons(
               &(plmn_name_array_ptr->plmn_info_ptr[loop_count]),
               plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_id,
               sys_mode);
    }

    if(is_ons_retrieved == FALSE)
    {
      /*Method MCC MNC string copy: Create MCC-MNC string and copy to plmn names */
      is_ons_retrieved = mmgsdi_eons_get_mnc_mcc_str(
                           &(plmn_name_array_ptr->plmn_info_ptr[loop_count]));
      if(is_ons_retrieved)
      {
        /* This log is kept intentionally */
        UIM_MSG_HIGH_1("ONS retrieved from MNC-MCC string, full & short name len: 0x%x",
                       plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_len);
        eons_method = MMGSDI_EONS_METHOD_MNC_MCC_STR;
        plmn_name_array_ptr->plmn_info_ptr[loop_count].plmn_name_source =
          MMGSDI_EONS_NAME_SOURCE_MCC_MNC;
      }
    }

    /* If we could only retrieve one of the names (long or short) via the
       look-up method for this plmn-id and the other one is empty, copy
       over the non-empty long/short name as both a long and a short name.
       This is done to avoid sending, say, a non-empty longname but an empty
       shortname to MMGSDI clients such as QMINAS. Per our current EONS logic,
       for every look-up method, we bail out as soon as we find even one of
       the names in that source. This can result in one of the names sent
       to MMGSDI EONS clients to be empty and UI can end up displaying a empty
       name/string */
    mmgsdi_eons_check_and_populate_empty_name(
      &(plmn_name_array_ptr->plmn_info_ptr[loop_count]));

    /*if the plmn is an rplmn, update eons method and name */
    if(is_plmn_rplmn)
    {
      mmgsdi_status = mmgsdi_eons_update_rplmn_name_and_method(
                        eons_method,
                        stack_type,
                        rplmn_is_hplmn_spdi_plmn,
                        display_byte,
                        &(plmn_name_array_ptr->plmn_info_ptr[loop_count]),
                        eons_info_ptr);
    }

    mmgsdi_eons_print_plmn_info_names(
       &(plmn_name_array_ptr->plmn_info_ptr[loop_count]));
  }

  /* In the case of SS event or FCN or OPL PNN the PLMN ID list was
     created while running the algorithm based on the active RPLMNs
     available. Hence the list is freed for those cases */
  if(eons_trigger == MMGSDI_EONS_TRIGGER_REFRESH_FCN ||
     eons_trigger == MMGSDI_EONS_TRIGGER_SS_EVENT ||
     eons_trigger == MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(plmn_id_list.plmn_list_ptr);
  }

  return mmgsdi_status;
}/* mmgsdi_eons_run_algo */


/*===========================================================================
FUNCTION MMGSDI_EONS_POPULATE_OPERATOR_INFO

DESCRIPTION
  Function to populate operator info in the EONS event payload

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_populate_operator_info(
  const mmgsdi_eons_per_stack_rplmn_info_type   * rplmn_info_ptr,
  mmgsdi_plmn_info_type                         * op_info_ptr
)
{
  if (rplmn_info_ptr == NULL || op_info_ptr == NULL)
  {
    return;
  }

  if (rplmn_info_ptr->rplmn_available)
  {
    mmgsdi_memscpy(&(op_info_ptr->plmn_id),
                   sizeof(mmgsdi_plmn_id_type),
                   &(rplmn_info_ptr->rplmn_id),
                   sizeof(mmgsdi_plmn_id_type));
  }

  if (rplmn_info_ptr->lac_available)
  {
    op_info_ptr->lac = rplmn_info_ptr->lac;
  }

  if (rplmn_info_ptr->long_name_available)
  {
    (void)mmgsdi_eons_copy_plmn_name_info(
            &(rplmn_info_ptr->long_name),
            &(op_info_ptr->plmn_long_name),
            TRUE);
  }
  if (rplmn_info_ptr->short_name_available)
  {
    (void)mmgsdi_eons_copy_plmn_name_info(
            &(rplmn_info_ptr->short_name),
            &(op_info_ptr->plmn_short_name),
            TRUE);
  }

  /* Copy the PLMN name source to EONS info payload */
  op_info_ptr->plmn_name_source = rplmn_info_ptr->rplmn_name_source;

  if (rplmn_info_ptr->additional_info.info_encoding !=
        MMGSDI_EONS_ENC_INVALID &&
      rplmn_info_ptr->additional_info.info_data.data_len > 0  &&
      rplmn_info_ptr->additional_info.info_data.data_ptr != NULL)
  {
    op_info_ptr->plmn_additional_info.info_encoding =
      rplmn_info_ptr->additional_info.info_encoding;

    op_info_ptr->plmn_additional_info.info_data.data_len =
      rplmn_info_ptr->additional_info.info_data.data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       op_info_ptr->plmn_additional_info.info_data.data_ptr,
       op_info_ptr->plmn_additional_info.info_data.data_len);

    if (op_info_ptr->plmn_additional_info.info_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(op_info_ptr->plmn_additional_info.info_data.data_ptr,
                     op_info_ptr->plmn_additional_info.info_data.data_len,
                     rplmn_info_ptr->additional_info.info_data.data_ptr,
                     rplmn_info_ptr->additional_info.info_data.data_len);
    }
  }

  /* Point to the additional language PLMN names, if present */
  if(rplmn_info_ptr->num_additional_names > 0)
  {
    uint8 index = 0;

    op_info_ptr->num_plmn_additional_names = rplmn_info_ptr->num_additional_names;

    if(op_info_ptr->num_plmn_additional_names > MMGSDI_ADDITIONAL_NAMES_MAX)
    {
      op_info_ptr->num_plmn_additional_names = MMGSDI_ADDITIONAL_NAMES_MAX;
    }

    for(index = 0; index < op_info_ptr->num_plmn_additional_names; index++)
    {
      op_info_ptr->plmn_additional_names[index].plmn_lang.lang_len =
        rplmn_info_ptr->additional_names[index].plmn_lang.lang_len;

      mmgsdi_memscpy(op_info_ptr->plmn_additional_names[index].plmn_lang.lang_id,
                     MMGSDI_LANG_ID_LEN_MAX,
                     rplmn_info_ptr->additional_names[index].plmn_lang.lang_id,
                     rplmn_info_ptr->additional_names[index].plmn_lang.lang_len);

      /* Short name in additional language */
      op_info_ptr->plmn_additional_names[index].plmn_short_name.plmn_name.eons_data.data_len =
        rplmn_info_ptr->additional_names[index].plmn_short_name.plmn_name.eons_data.data_len;

      op_info_ptr->plmn_additional_names[index].plmn_short_name.plmn_name.eons_data.data_ptr =
        rplmn_info_ptr->additional_names[index].plmn_short_name.plmn_name.eons_data.data_ptr,

      op_info_ptr->plmn_additional_names[index].plmn_short_name.plmn_name.eons_encoding =
        rplmn_info_ptr->additional_names[index].plmn_short_name.plmn_name.eons_encoding;

      /* Long name in additional language */
      op_info_ptr->plmn_additional_names[index].plmn_long_name.plmn_name.eons_data.data_len =
        rplmn_info_ptr->additional_names[index].plmn_long_name.plmn_name.eons_data.data_len;

      op_info_ptr->plmn_additional_names[index].plmn_long_name.plmn_name.eons_data.data_ptr =
        rplmn_info_ptr->additional_names[index].plmn_long_name.plmn_name.eons_data.data_ptr,

      op_info_ptr->plmn_additional_names[index].plmn_long_name.plmn_name.eons_encoding =
        rplmn_info_ptr->additional_names[index].plmn_long_name.plmn_name.eons_encoding;
    }
  }
} /* mmgsdi_eons_populate_operator_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_POPULATE_EVT_INFO

DESCRIPTION
  Function to populate EONS event payload

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_populate_evt_info(
  mmgsdi_session_type_enum_type     session_type,
  const mmgsdi_eons_info_type     * eons_info_ptr,
  mmgsdi_eons_evt_info_type       * evt_eons_ptr
)
{
  const mmgsdi_eons_per_stack_rplmn_info_type   * rplmn_info_ptr = NULL;
  mmgsdi_plmn_info_type                         * op_info_ptr    = NULL;

  if (eons_info_ptr == NULL || evt_eons_ptr == NULL ||
      (!mmgsdi_util_is_prov_session(session_type)))
  {
    return;
  }

  /* Regardless of session type the event payload shall contain SPN info, SPN
     display bit and rule regarding display of spn based on home network etc. */
  evt_eons_ptr->spn_display_bit        = eons_info_ptr->spn_display_bit;
  evt_eons_ptr->display_spn            = eons_info_ptr->display_spn;
  evt_eons_ptr->spn.eons_encoding      = eons_info_ptr->spn.eons_encoding;

  if(eons_info_ptr->spn.eons_encoding != MMGSDI_EONS_ENC_INVALID &&
     eons_info_ptr->spn.eons_data.data_len > 0)
  {
    evt_eons_ptr->spn.eons_data.data_len =
      eons_info_ptr->spn.eons_data.data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      evt_eons_ptr->spn.eons_data.data_ptr,
      evt_eons_ptr->spn.eons_data.data_len);

    if(evt_eons_ptr->spn.eons_data.data_ptr != NULL)
    {
      mmgsdi_memscpy(evt_eons_ptr->spn.eons_data.data_ptr,
                     evt_eons_ptr->spn.eons_data.data_len,
                     eons_info_ptr->spn.eons_data.data_ptr,
                     eons_info_ptr->spn.eons_data.data_len);

      if (uim_selective_logging[UIM_LOG_EONS])
      {
        /* print spn data buffer*/
        mmgsdi_util_print_data_to_diag("spn data buffer",
          evt_eons_ptr->spn.eons_data.data_ptr,
          (uint16)evt_eons_ptr->spn.eons_data.data_len);
      }
    }
    else
    {
      evt_eons_ptr->spn.eons_data.data_len = 0;
    }
  }

  /* Additionally for CDMA sessions the event shall contain language indication
     and roaming status */
  if (session_type == MMGSDI_1X_PROV_PRI_SESSION ||
      session_type == MMGSDI_1X_PROV_SEC_SESSION)
  {
    evt_eons_ptr->spn_lang_ind = eons_info_ptr->lang_ind;

    /* Note: For CDMA there is no 2nd stack supported hence only first stack
       content is copied */
    if (eons_info_ptr->rplmn_info.first_stack_rplmn.rplmn_available)
    {
      evt_eons_ptr->first_stack_info.is_valid       = TRUE;

      evt_eons_ptr->first_stack_info.roaming_status =
        (eons_info_ptr->rplmn_info.first_stack_rplmn.is_home_network) ?
          MMGSDI_EONS_IN_HOME_NW : MMGSDI_EONS_IN_ROAMING_NW;
    }
    return;
  }

  /* Additionally for GW sessions the event can contain:
     1. The RPLMN display bit,
     2. The roaming status, LAC and operator info if service status is FULL service */
  evt_eons_ptr->rplmn_display_bit = eons_info_ptr->rplmn_display_bit;

  rplmn_info_ptr = &(eons_info_ptr->rplmn_info.first_stack_rplmn);
  op_info_ptr    = &(evt_eons_ptr->first_stack_info.operator_info);

  if (rplmn_info_ptr->rplmn_available &&
      rplmn_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
  {
    evt_eons_ptr->first_stack_info.is_valid = TRUE;

    evt_eons_ptr->first_stack_info.roaming_status =
      (rplmn_info_ptr->is_home_network) ?
        MMGSDI_EONS_IN_HOME_NW : MMGSDI_EONS_IN_ROAMING_NW;

    evt_eons_ptr->first_stack_info.display_operator_info =
      rplmn_info_ptr->display_operator_info;
    evt_eons_ptr->first_stack_info.display_spn_info =
      rplmn_info_ptr->display_spn_info;

    mmgsdi_eons_populate_operator_info(rplmn_info_ptr,
                                       op_info_ptr);
  }

  rplmn_info_ptr = &(eons_info_ptr->rplmn_info.sec_stack_rplmn);
  op_info_ptr    = &(evt_eons_ptr->sec_stack_info.operator_info);

  if (rplmn_info_ptr->rplmn_available &&
      rplmn_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
  {
    evt_eons_ptr->sec_stack_info.is_valid = TRUE;

    evt_eons_ptr->sec_stack_info.roaming_status =
      rplmn_info_ptr->is_home_network ?
        MMGSDI_EONS_IN_HOME_NW : MMGSDI_EONS_IN_ROAMING_NW;

    evt_eons_ptr->sec_stack_info.display_operator_info =
      rplmn_info_ptr->display_operator_info;
    evt_eons_ptr->sec_stack_info.display_spn_info =
      rplmn_info_ptr->display_spn_info;

    mmgsdi_eons_populate_operator_info(rplmn_info_ptr,
                                       op_info_ptr);
  }
} /* mmgsdi_eons_populate_evt_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_LAST_EVENT

DESCRIPTION
  Function to send EONS event to provisioning session which registered late
  i.e. after session registration check whether EONS event is sent to this session type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_last_event
(
  mmgsdi_session_id_type        session_id
)
{
  mmgsdi_return_enum_type         mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type   session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_int_app_info_type      * int_app_info_ptr = NULL;
  mmgsdi_eons_info_type         * eons_info_ptr    = NULL;
  mmgsdi_eons_evt_info_type     * evt_eons_ptr     = NULL;
  mmgsdi_event_data_type        * evt_info_ptr     = NULL;

  mmgsdi_status = mmgsdi_util_get_session_type(
                     session_id,
                     &session_type,
                     NULL);

  /*Send eons event only when session type is GW PRI, GW SEC or 1X PRI*/
  if(MMGSDI_SUCCESS != mmgsdi_status ||
     (!mmgsdi_util_is_prov_session(session_type)))
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    NULL,
                    NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     int_app_info_ptr == NULL)
  {
    return;
  }

  eons_info_ptr = &(int_app_info_ptr->eons_info);

  /* check if EONS algo has run and service status is FULL service*/
  if((eons_info_ptr->rplmn_info.first_stack_rplmn.eons_method <=
        MMGSDI_EONS_METHOD_NONE ||
      eons_info_ptr->rplmn_info.first_stack_rplmn.eons_method >=
        MMGSDI_EONS_METHOD_MAX ||
      eons_info_ptr->rplmn_info.first_stack_rplmn.srv_status
        != SYS_SRV_STATUS_SRV) &&
     (eons_info_ptr->rplmn_info.sec_stack_rplmn.eons_method <=
        MMGSDI_EONS_METHOD_NONE ||
      eons_info_ptr->rplmn_info.sec_stack_rplmn.eons_method >=
        MMGSDI_EONS_METHOD_MAX ||
      eons_info_ptr->rplmn_info.sec_stack_rplmn.srv_status
        != SYS_SRV_STATUS_SRV))
  {
    return;
  }

  /*send eons event to all sessions of session_type*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return;
  }

  memset(evt_info_ptr, 0x0, sizeof(mmgsdi_event_data_type));

  evt_info_ptr->evt   = MMGSDI_EONS_EVT;
  evt_eons_ptr        = &(evt_info_ptr->data.eons);
  eons_info_ptr       = &(int_app_info_ptr->eons_info);

  mmgsdi_eons_populate_evt_info(session_type,
                                &(int_app_info_ptr->eons_info),
                                evt_eons_ptr);

  mmgsdi_evt_notify_specific_session_id(session_id, evt_info_ptr);

  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
}/* mmgsdi_eons_send_last_event */


/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_EVENT

DESCRIPTION
  Function to send EONS event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_event(
  mmgsdi_session_id_type        session_id,
  mmgsdi_eons_trigger_enum_type eons_trigger,
  boolean                       csg_ef_refreshed)
{
  mmgsdi_return_enum_type         mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type   session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_int_app_info_type      * int_app_info_ptr = NULL;
  mmgsdi_slot_id_enum_type        slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_list_plmn_name_type      plmn_name_list;
  mmgsdi_plmn_id_list_type        plmn_id_list;
  mmgsdi_eons_evt_info_type     * evt_eons_ptr = NULL;
  mmgsdi_event_data_type        * evt_info_ptr  = NULL;

  mmgsdi_status = mmgsdi_util_get_session_type(
                     session_id,
                     &session_type,
                     NULL);

  /*Send eons event only when session type is GW PRI, GW SEC or 1X PRI*/
  if(MMGSDI_SUCCESS != mmgsdi_status ||
     (session_type != MMGSDI_GW_PROV_PRI_SESSION &&
      session_type != MMGSDI_GW_PROV_SEC_SESSION &&
      session_type != MMGSDI_GW_PROV_TER_SESSION &&
      session_type != MMGSDI_1X_PROV_PRI_SESSION &&
      session_type != MMGSDI_1X_PROV_SEC_SESSION ))
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    NULL,
                    NULL);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    return;
  }

  if(eons_trigger == MMGSDI_EONS_TRIGGER_REFRESH_FCN)
  {
    /* In case UE is on a macrocell and CSG related EF gets REFRESHED, do not
       send the event. Note: No CSG data defined for a 2nd active stack so only
       checking against first stack ss information */
    if((csg_ef_refreshed &&
        int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.rplmn_available &&
        int_app_info_ptr->eons_info.rplmn_info.first_stack_rplmn.csg_info.csg_id ==
          MMGSDI_EONS_INVALID_CSG_ID))
    {
      UIM_MSG_LOW_0("FCN trigger but CSG id invalid hence no algo run or evt sent");
      return;
    }
  }

  /*send eons event to all sessions of session_type*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return;
  }

  memset(&plmn_id_list, 0x0, sizeof(mmgsdi_plmn_id_list_type));
  memset(&plmn_name_list, 0x0, sizeof(mmgsdi_list_plmn_name_type));

  evt_info_ptr->evt              = MMGSDI_EONS_EVT;
  evt_eons_ptr                  = &(evt_info_ptr->data.eons);

  mmgsdi_status = mmgsdi_eons_run_algo(
                    session_id,
                    eons_trigger,
                    plmn_id_list,
                    MMGSDI_EONS_IGNORE_DISPLAY_COND_NONE,
                    NULL,
                    NULL,
                    &plmn_name_list,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_eons_populate_evt_info(session_type,
                                  &(int_app_info_ptr->eons_info),
                                  evt_eons_ptr);

    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, int_app_info_ptr, TRUE);
  }

  /* free memory allocated to plmn info ptr while running EONS algo */
  if(plmn_name_list.plmn_info_ptr != NULL)
  {
    uint8 loop_count = 0;
    for (loop_count = 0; loop_count < plmn_name_list.num_of_plmn_ids; loop_count++)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        plmn_name_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        plmn_name_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_ptr);
    }
    MMGSDIUTIL_TMC_MEM_FREE(plmn_name_list.plmn_info_ptr);
  }
  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
}/* mmgsdi_eons_send_event */


/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_SYS_MODE_TO_RAT_TYPE

DESCRIPTION
  Function to map system mode to radio access technology

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RAT_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_rat_enum_type mmgsdi_eons_map_sys_mode_to_rat_type(
  sys_sys_mode_e_type              sys_mode
)
{
  mmgsdi_rat_enum_type  rat_type = MMGSDI_RAT_NONE;

  switch(sys_mode)
  {
    case SYS_SYS_MODE_GSM:
      rat_type = MMGSDI_RAT_GSM;
      break;

    case SYS_SYS_MODE_WCDMA:
      rat_type = MMGSDI_RAT_UMTS;
      break;

    case SYS_SYS_MODE_LTE:
      rat_type = MMGSDI_RAT_LTE;
      break;

    case SYS_SYS_MODE_TDS:
      rat_type = MMGSDI_RAT_TDS;
      break;

    default:
      rat_type = MMGSDI_RAT_NONE;
      break;
  }
  return rat_type;
}/* mmgsdi_eons_map_sys_mode_to_rat_type */


/*===========================================================================
FUNCTION MMGSDI_EONS_MAP_NETWORK_CODING_SCHEME

DESCRIPTION
  Function to map network coding scheme to mmgsdi operator name coding scheme

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_OPERATOR_NAME_ENCODING_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_eons_encoding_enum_type mmgsdi_eons_map_network_coding_scheme(
  sys_network_name_coding_scheme_type_e_type network_encoding
)
{
  mmgsdi_eons_encoding_enum_type encoding;

  switch(network_encoding)
  {
    case SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET:
      encoding = MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_PACKED;
      break;

    case SYS_NW_NAME_CODING_SCHEME_UCS2:
      encoding = MMGSDI_EONS_ENC_UCS2;
      break;

    default:
      encoding = MMGSDI_EONS_ENC_INVALID;
      break;
  }

  return encoding;
}/* mmgsdi_eons_map_network_coding_scheme */


/*===========================================================================
FUNCTION MMGSDI_EONS_FREE_RPLMN_INFO_PTRS

DESCRIPTION
  Function to free pointers in RPLMN info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_free_rplmn_info_ptrs(
  mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr
)
{
  uint8 index = 0;

  if (rplmn_info_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
     rplmn_info_ptr->long_name.plmn_name.eons_data.data_ptr);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
     rplmn_info_ptr->short_name.plmn_name.eons_data.data_ptr);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
     rplmn_info_ptr->additional_info.info_data.data_ptr);

  for(index = 0; index < rplmn_info_ptr->num_additional_names; index ++)
  {
    rplmn_info_ptr->additional_names[index].plmn_long_name.plmn_name.eons_data.data_ptr = NULL;
    rplmn_info_ptr->additional_names[index].plmn_short_name.plmn_name.eons_data.data_ptr = NULL;
  }
  rplmn_info_ptr->num_additional_names = 0;
} /* mmgsdi_eons_free_rplmn_info_ptrs */


/*===========================================================================
  FUNCTION MMGSDI_EONS_COPY_PLMN_NAME_INFO

DESCRIPTION
  Utility function to copy PLMN name info

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_copy_plmn_name_info(
  const mmgsdi_plmn_name_type  * in_plmn_ptr,
  mmgsdi_plmn_name_type        * out_plmn_ptr,
  boolean                        allocate_mem
)
{
  if(in_plmn_ptr == NULL ||
     out_plmn_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  out_plmn_ptr->plmn_name.eons_data.data_len = 0;

  if (allocate_mem &&
      in_plmn_ptr->plmn_name.eons_encoding != MMGSDI_EONS_ENC_INVALID &&
      in_plmn_ptr->plmn_name.eons_data.data_len > 0  &&
      in_plmn_ptr->plmn_name.eons_data.data_ptr != NULL)
  {
    /* Ensure data_ptr is NULL, before allocating */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(out_plmn_ptr->plmn_name.eons_data.data_ptr);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       out_plmn_ptr->plmn_name.eons_data.data_ptr,
       in_plmn_ptr->plmn_name.eons_data.data_len);
    if (out_plmn_ptr->plmn_name.eons_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  if(in_plmn_ptr->plmn_name.eons_encoding != MMGSDI_EONS_ENC_INVALID &&
     in_plmn_ptr->plmn_name.eons_data.data_len > 0 &&
     out_plmn_ptr->plmn_name.eons_data.data_ptr != NULL &&
     in_plmn_ptr->plmn_name.eons_data.data_ptr != NULL)
  {
    out_plmn_ptr->plmn_name_ci                 = in_plmn_ptr->plmn_name_ci;
    out_plmn_ptr->plmn_name_spare_bits         = in_plmn_ptr->plmn_name_spare_bits;
    out_plmn_ptr->plmn_name.eons_encoding      = in_plmn_ptr->plmn_name.eons_encoding;

    out_plmn_ptr->plmn_name.eons_data.data_len = in_plmn_ptr->plmn_name.eons_data.data_len;

    mmgsdi_memscpy(out_plmn_ptr->plmn_name.eons_data.data_ptr,
      int32touint32(out_plmn_ptr->plmn_name.eons_data.data_len),
      in_plmn_ptr->plmn_name.eons_data.data_ptr,
      int32touint32(in_plmn_ptr->plmn_name.eons_data.data_len));
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_eons_copy_plmn_name_info */

