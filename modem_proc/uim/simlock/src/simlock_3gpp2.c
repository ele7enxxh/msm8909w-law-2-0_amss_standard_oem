/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   3 G P P 2   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_3gpp2.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/16   vdc     Extend full and expired service policies on any slot
09/07/16   vdc     Apply same config for VALID_CARD_MUST_ON_ANY_SLOT policy
06/22/16   bcho    Return from algo after setting reason in emergency mode
03/09/16   av      Support for autolock whitelist
07/01/15   bcho    Support for whitelist and blacklist codes in same category
06/08/15   stv     Populate retry values in run algo response
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
02/17/15   vv      Indicate the reason for invalid subscription
01/21/15   vv      Remove check for the IMSI class
01/14/15   bcho    Support for identifying test SIMs
09/29/14   vv      Added support for emergency only mode
08/29/14   vv      Added support for temporary unlock
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_3gpp2.h"
#include "simlock_util.h"
#include "simlock_category.h"
#include "simlock_timer.h"

#define SIMLOCK_PREFIX_MASK        0x00FFC000
#define SIMLOCK_THOUSANDS_MASK     0x00003C00
#define SIMLOCK_LAST_3_MASK        0x000003FF

#define SIMLOCK_PREFIX_SHIFT       14
#define SIMLOCK_THOUSANDS_SHIFT    10
#define SIMLOCK_LAST_3_SHIFT       0

const uint8 simlock_bcd_to_num[] =
{ 0xFF, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const uint8 simlock_min_to_num[] =
{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };


/*===========================================================================
FUNCTION SIMLOCK_3GPP2_DECODE_IMSI

DESCRIPTION
  This function decodes the mcc, mnc and msin

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_3gpp2_decode_imsi
(
 uint16                             * in_mcc_ptr,
 uint8                              * in_mnc_ptr,
 uint32                             * in_min1_ptr,
 uint16                             * in_min2_ptr,
 simlock_category_code_data_type    * out_data_ptr
)
{
  uint16  mcc_code     = 0;
  uint16  first_three  = 0;
  uint16  second_three = 0;
  uint16  last_three   = 0;
  uint8   mnc_code     = 0;
  uint8   thousands    = 0;
  uint8   digit        = 0;

  if((in_mcc_ptr == NULL)   ||
     (in_mnc_ptr == NULL)   ||
     (in_min1_ptr == NULL)  ||
     (in_min2_ptr == NULL)  ||
     (out_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  mcc_code = *in_mcc_ptr;
  if(mcc_code <= 999)
  {
    /* Converting Mobile Country Code */
    SIMLOCK_MSG_LOW_0("Converting Mobile Country Code");
    digit = simlock_min_to_num[mcc_code/100];
    out_data_ptr->mcc[0] = digit;
    mcc_code %= 100;

    digit = simlock_min_to_num[mcc_code / 10];
    out_data_ptr->mcc[1] = digit;

    digit = simlock_min_to_num[mcc_code % 10];
    out_data_ptr->mcc[2] = digit;
  }
  else
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Error in converting Mobile Country Code of IMSI */
    SIMLOCK_MSG_HIGH_0("Error in converting IMSI");
    return SIMLOCK_GENERIC_ERROR;
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
  }

  mnc_code = *in_mnc_ptr;
  if(mnc_code <= 99)
  {
    /* Convert Mobile Network Code */
    SIMLOCK_MSG_LOW_0("Converting Mobile Network Code");
    digit = simlock_min_to_num[mnc_code / 10];
    out_data_ptr->mnc.mnc_data[0] = digit;

    digit = simlock_min_to_num[mnc_code % 10];
    out_data_ptr->mnc.mnc_data[1] = digit;

    out_data_ptr->mnc.mnc_len = 2;
  }
  else
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Error in converting Mobile Network Code of IMSI */
    SIMLOCK_MSG_HIGH_0("Error in converting IMSI");
    return SIMLOCK_GENERIC_ERROR;
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
  }

  first_three = *in_min2_ptr;
  if (first_three <= 999)
  {
    /* Convert First Three Digits of IMSI */
    SIMLOCK_MSG_LOW_0("Converting First Three Digits of IMSI");
    digit = simlock_min_to_num[first_three / 100];
    out_data_ptr->msin.msin_data[0] = digit;
    first_three %= 100;

    digit = simlock_min_to_num[first_three / 10];
    out_data_ptr->msin.msin_data[1] = digit;

    digit = simlock_min_to_num[first_three % 10];
    out_data_ptr->msin.msin_data[2] = digit;

    out_data_ptr->msin.msin_len = 3;
  }
  else
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Error in converting First 3 Digits of IMSI */
    SIMLOCK_MSG_HIGH_0("Error in converting IMSI");
    return SIMLOCK_GENERIC_ERROR;
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
  }

  if (*in_min1_ptr > 0)
  {
    /* Convert MIN 1 Portion of IMSI */
    second_three = (uint16)((*in_min1_ptr & SIMLOCK_PREFIX_MASK) >> SIMLOCK_PREFIX_SHIFT);
    thousands = (uint16)((*in_min1_ptr & SIMLOCK_THOUSANDS_MASK) >> SIMLOCK_THOUSANDS_SHIFT);
    last_three = (uint16)((*in_min1_ptr & SIMLOCK_LAST_3_MASK) >> SIMLOCK_LAST_3_SHIFT);

    thousands = simlock_bcd_to_num[thousands];
    if ( (thousands != 0xFF)
    &&   (second_three <= 999)
    &&   (last_three <= 999) )
    {
      /* IMSI data is of the right format */
      SIMLOCK_MSG_LOW_0("Converting Last 7 Digits of IMSI");
      digit = simlock_min_to_num[second_three / 100];
      out_data_ptr->msin.msin_data[3] = digit;

      second_three %= 100;

      digit = simlock_min_to_num[second_three / 10];
      out_data_ptr->msin.msin_data[4] = digit;

      digit = simlock_min_to_num[second_three % 10];
      out_data_ptr->msin.msin_data[5] = digit;

      out_data_ptr->msin.msin_data[6] = thousands;

      digit = simlock_min_to_num[last_three / 100];
      out_data_ptr->msin.msin_data[7] = digit;

      last_three %= 100;

      digit = simlock_min_to_num[last_three / 10];
      out_data_ptr->msin.msin_data[8] = digit;

      digit = simlock_min_to_num[last_three % 10];
      out_data_ptr->msin.msin_data[9] = digit;

      out_data_ptr->msin.msin_len = 10;
    } /* end if - last 7 digits of IMSI are correct format */
  } /* end if MIN 1 is a valid number */

  return SIMLOCK_SUCCESS;
} /* simlock_3gpp2_decode_imsi */


/*===========================================================================
FUNCTION SIMLOCK_3GPP2_PARSE_IMSI

DESCRIPTION
  This function parses the 3GPP2 IMSI from the card and provides the data in
  a structured format

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_3gpp2_parse_imsi
(
  const simlock_imsi_type            * imsi_data_ptr,
  simlock_category_code_data_type    * out_data_ptr
)
{
  simlock_result_enum_type  simlock_status    = SIMLOCK_SUCCESS;
  uint32                    min1_in_bcd       = 0;
  uint16                    mcc_in_bcd        = 0;
  uint16                    min2_in_bcd       = 0;
  uint8                     mnc_in_bcd        = 0;

  if((imsi_data_ptr == NULL)        ||
     (out_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(imsi_data_ptr->imsi_len != 10)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  mcc_in_bcd = (uint16)(((imsi_data_ptr->imsi[9] & 0x03) << 8) +
                        imsi_data_ptr->imsi[8]);

  mnc_in_bcd = (imsi_data_ptr->imsi[6] & 0x7F);

  min1_in_bcd = (uint32)((imsi_data_ptr->imsi[5] << 16) +
                         (imsi_data_ptr->imsi[4] << 8)  +
                         (imsi_data_ptr->imsi[3]));

  min2_in_bcd = (uint16)((imsi_data_ptr->imsi[2] << 8) +
                         (imsi_data_ptr->imsi[1]));

  simlock_status = simlock_3gpp2_decode_imsi(&mcc_in_bcd, &mnc_in_bcd,
                                             &min1_in_bcd, &min2_in_bcd,
                                             out_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_3gpp2_parse_imsi */


/*===========================================================================
FUNCTION SIMLOCK_3GPP2_RUN_ALGORITHM_RUIM

DESCRIPTION
  This function validates the 3GPP2 RUIM code from the card and the
  lock data retrieved from the category file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_code_in_list_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_code_in_list_enum_type simlock_3gpp2_run_algorithm_ruim
(
  simlock_category_code_data_type         sim_code_data,
  simlock_slot_enum_type                  slot,
  simlock_category_file_data_type       * category_data_ptr
)
{
  simlock_result_enum_type           simlock_status    = SIMLOCK_SUCCESS;
  uint32                             code_index        = 0;
  simlock_code_in_list_enum_type     code_in_list      = SIMLOCK_CODE_IN_NOLIST;
  boolean                            code_in_whitelist = FALSE;
  boolean                            code_in_blacklist = FALSE;

  if(category_data_ptr == NULL)
  {
    return code_in_list;
  }

  SIMLOCK_MSG_MED_0("simlock_3gpp2_run_algorithm_ruim");

  SIMLOCK_MSG_LOW_3("From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2]);

  SIMLOCK_MSG_LOW_3("From IMSI - mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2]);

  SIMLOCK_MSG_LOW_4("From IMSI - msin[0]: 0x%x, msin[1]: 0x%x, msin[2]: 0x%x, msin[3]: 0x%x",
                    sim_code_data.msin.msin_data[0], sim_code_data.msin.msin_data[1], sim_code_data.msin.msin_data[2], sim_code_data.msin.msin_data[3]);

  SIMLOCK_MSG_LOW_4("From IMSI - msin[4]: 0x%x, msin[5]: 0x%x, msin[6]: 0x%x, msin[7]: 0x%x",
                    sim_code_data.msin.msin_data[4], sim_code_data.msin.msin_data[5], sim_code_data.msin.msin_data[6], sim_code_data.msin.msin_data[7]);

  SIMLOCK_MSG_LOW_2("From IMSI - msin[8]: 0x%x, msin[9]: 0x%x",
                    sim_code_data.msin.msin_data[8], sim_code_data.msin.msin_data[9]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* Compare MCCs/MNCs from the file and IMSI */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_3("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2]);

    SIMLOCK_MSG_LOW_3("From file - mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2]);

    SIMLOCK_MSG_LOW_4("From file - msin[0]: 0x%x, msin[1]: 0x%x, msin[2]: 0x%x, msin[3]: 0x%x",
                     category_data_ptr->code_data_ptr[code_index].msin.msin_data[0],category_data_ptr->code_data_ptr[code_index].msin.msin_data[1],
                     category_data_ptr->code_data_ptr[code_index].msin.msin_data[2],category_data_ptr->code_data_ptr[code_index].msin.msin_data[3]);

    SIMLOCK_MSG_LOW_4("From file - msin[4]: 0x%x, msin[5]: 0x%x, msin[6]: 0x%x, msin[7]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[4],category_data_ptr->code_data_ptr[code_index].msin.msin_data[5],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[6],category_data_ptr->code_data_ptr[code_index].msin.msin_data[7]);

    SIMLOCK_MSG_LOW_2("From file - msin[8]: 0x%x, msin[9]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[8],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[9]);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                     &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      if(simlock_util_check_if_msin_matches(category_data_ptr->code_data_ptr[code_index].msin,
                                            sim_code_data.msin))
      {
        if(category_data_ptr->code_data_ptr[code_index].code_type == SIMLOCK_CODE_WHITELIST)
        {
          code_in_list = SIMLOCK_CODE_IN_WHITELIST;
          code_in_whitelist = TRUE;
        }
        if(category_data_ptr->code_data_ptr[code_index].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          code_in_list = SIMLOCK_CODE_IN_BLACKLIST;
          code_in_blacklist = TRUE;
        }
      }
    }
  }

  /* If autolock is enabled and,
     - no codes were present during locking process, lock the device to
       the current sim data.
     - a valid list (whitelist) of codes were present during the locking
       process, the intention is to auto lock the device to the current sim
       data if matches with any of the codes present in the whitelist of
       auto-lock codes.
     In both these cases, we update the lock code data in the category data
     file */
  if(category_data_ptr->category_header.auto_lock  &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      code_in_whitelist))
  {
    SIMLOCK_MSG_MED_0("autolock enabled, updating code data");

    /* Format the category data to write to file */
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;

    /* write the lock codes from SIM */
    simlock_status = simlock_category_update_and_write_lock_data(&category_data_ptr->category_header,
                                                                 &sim_code_data,
                                                                 slot);
    code_in_list = (simlock_status == SIMLOCK_SUCCESS ? SIMLOCK_CODE_IN_WHITELIST : SIMLOCK_CODE_IN_NOLIST);
  }

  if(code_in_whitelist && code_in_blacklist)
  {
    code_in_list = SIMLOCK_CODE_IN_WHITELIST_AND_BLACKLIST;
  }

  SIMLOCK_MSG_MED_1("ruim code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp2_run_algorithm_ruim */


/*===========================================================================
FUNCTION SIMLOCK_3GPP2_RUN_ALGORITHM_NW_TYPE2

DESCRIPTION
  This function validates the 3GPP2 NW type2 code from the card and the
  lock data retrieved from the category file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_code_in_list_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_code_in_list_enum_type simlock_3gpp2_run_algorithm_nw_type2
(
  simlock_category_code_data_type         sim_code_data,
  simlock_slot_enum_type                  slot,
  simlock_category_file_data_type       * category_data_ptr
)
{
  simlock_result_enum_type           simlock_status    = SIMLOCK_SUCCESS;
  uint32                             code_index        = 0;
  simlock_code_in_list_enum_type     code_in_list      = SIMLOCK_CODE_IN_NOLIST;
  boolean                            code_in_whitelist = FALSE;
  boolean                            code_in_blacklist = FALSE;

  if(category_data_ptr == NULL)
  {
    return code_in_list;
  }

  SIMLOCK_MSG_MED_0("simlock_3gpp2_run_algorithm_nw_type2");

  SIMLOCK_MSG_LOW_4("From IMSI - irm[0]: 0x%x, irm[1]: 0x%x, irm[2]: 0x%x, irm[3]: 0x%x",
                     sim_code_data.msin.msin_data[0], sim_code_data.msin.msin_data[1],
                    sim_code_data.msin.msin_data[2],
                    sim_code_data.msin.msin_data[3]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* Compare IRM from the file and IMSI */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_4("From file - irm[0]: 0x%x, irm[1]: 0x%x, irm[2]: 0x%x, irm[3]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[0],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[1],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[2],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[3]);

    if(simlock_util_check_if_irm_matches(category_data_ptr->code_data_ptr[code_index].msin,
                                         sim_code_data.msin))
    {
      if(category_data_ptr->code_data_ptr[code_index].code_type == SIMLOCK_CODE_WHITELIST)
      {
        code_in_list = SIMLOCK_CODE_IN_WHITELIST;
        code_in_whitelist = TRUE;
      }
      if(category_data_ptr->code_data_ptr[code_index].code_type == SIMLOCK_CODE_BLACKLIST)
      {
        code_in_list = SIMLOCK_CODE_IN_BLACKLIST;
        code_in_blacklist = TRUE;
      }
    }
  }

  /* If autolock is enabled and,
     - no codes were present during locking process, lock the device to
       the current sim data.
     - a valid list (whitelist) of codes were present during the locking
       process, the intention is to auto lock the device to the current sim
       data if matches with any of the codes present in the whitelist of
       auto-lock codes.
     In both these cases, we update the lock code data in the category data
     file */
  if(category_data_ptr->category_header.auto_lock  &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      code_in_whitelist))
  {
    SIMLOCK_MSG_MED_0("autolock enabled, updating code data");

    /* format the category data to write to file */
    memset(sim_code_data.mcc, 0x7F, SIMLOCK_MCC_LEN);
    memset(sim_code_data.mnc.mnc_data, 0x7F, SIMLOCK_MNC_MAX);

    sim_code_data.mnc.mnc_len = 0;

    sim_code_data.msin.msin_len = SIMLOCK_IRM_LEN;
    memset(&sim_code_data.msin.msin_data[SIMLOCK_IRM_LEN], 0x7F,
            SIMLOCK_MSIN_MAX - SIMLOCK_IRM_LEN);

    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;

    /* write the lock codes from SIM */
    simlock_status = simlock_category_update_and_write_lock_data(&category_data_ptr->category_header,
                                                                 &sim_code_data,
                                                                 slot);
    code_in_list = (simlock_status == SIMLOCK_SUCCESS ? SIMLOCK_CODE_IN_WHITELIST : SIMLOCK_CODE_IN_NOLIST);
  }

  if(code_in_whitelist && code_in_blacklist)
  {
    code_in_list = SIMLOCK_CODE_IN_WHITELIST_AND_BLACKLIST;
  }

  SIMLOCK_MSG_MED_1("nw_type2 code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp2_run_algorithm_nw_type2 */


/*===========================================================================
FUNCTION SIMLOCK_3GPP2_RUN_ALGORITHM_NW_TYPE1

DESCRIPTION
  This function validates the 3GPP2 NW type1 code from the card and the
  lock data retrieved from the category file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_code_in_list_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_code_in_list_enum_type simlock_3gpp2_run_algorithm_nw_type1
(
  simlock_category_code_data_type         sim_code_data,
  simlock_slot_enum_type                  slot,
  simlock_category_file_data_type       * category_data_ptr
)
{
  simlock_result_enum_type           simlock_status    = SIMLOCK_SUCCESS;
  uint32                             code_index        = 0;
  simlock_code_in_list_enum_type     code_in_list      = SIMLOCK_CODE_IN_NOLIST;
  boolean                            code_in_whitelist = FALSE;
  boolean                            code_in_blacklist = FALSE;

  if(category_data_ptr == NULL)
  {
    return code_in_list;
  }

  SIMLOCK_MSG_MED_0("simlock_3gpp2_run_algorithm_nw_type1");

  SIMLOCK_MSG_LOW_3("From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2]);

  SIMLOCK_MSG_LOW_3("From IMSI - mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* compare MCCs/MNCs from the file and IMSI */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_3("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2]);

    SIMLOCK_MSG_LOW_3("From file - mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2]);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                 &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      if(category_data_ptr->code_data_ptr[code_index].code_type == SIMLOCK_CODE_WHITELIST)
      {
        code_in_list = SIMLOCK_CODE_IN_WHITELIST;
        code_in_whitelist = TRUE;
      }
      if(category_data_ptr->code_data_ptr[code_index].code_type == SIMLOCK_CODE_BLACKLIST)
      {
        code_in_list = SIMLOCK_CODE_IN_BLACKLIST;
        code_in_blacklist = TRUE;
      }
    }
  }

  /* If autolock is enabled and,
     - no codes were present during locking process, lock the device to
       the current sim data.
     - a valid list (whitelist) of codes were present during the locking
       process, the intention is to auto lock the device to the current sim
       data if matches with any of the codes present in the whitelist of
       auto-lock codes.
     In both these cases, we update the lock code data in the category data
     file */
  if(category_data_ptr->category_header.auto_lock  &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      code_in_whitelist))
  {
    SIMLOCK_MSG_MED_0("autolock enabled, updating code data");

    /* Format the category data to write to file */
    sim_code_data.msin.msin_len = 0;
    memset(sim_code_data.msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;

    /* write the lock codes from SIM */
    simlock_status = simlock_category_update_and_write_lock_data(&category_data_ptr->category_header,
                                                                 &sim_code_data,
                                                                 slot);
    code_in_list = (simlock_status == SIMLOCK_SUCCESS ? SIMLOCK_CODE_IN_WHITELIST : SIMLOCK_CODE_IN_NOLIST);
  }

  if(code_in_whitelist && code_in_blacklist)
  {
    code_in_list = SIMLOCK_CODE_IN_WHITELIST_AND_BLACKLIST;
  }

  SIMLOCK_MSG_MED_1("nw_type1 code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp2_run_algorithm_nw_type1 */


/*===========================================================================
FUNCTION SIMLOCK_3GPP2_RUN_ALGORITHM

DESCRIPTION
  This function validates the 3GPP2 subscription on a card by verifying the
  card data against the lock data. It checks the codes for each enabled
  category for validating the subscription. Also, it checks if a code is
  categorized as a blacklist or whitelist and accordingly updates the
  validity of the subscription.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_3gpp2_run_algorithm
(
  const simlock_subscription_data_type    * sim_data_ptr,
  simlock_slot_enum_type                    slot,
  simlock_config_data_type                * config_data_ptr,
  simlock_subscription_validity_info_type * subs_validity_ptr
)
{
  simlock_result_enum_type          simlock_status         = SIMLOCK_SUCCESS;
  simlock_category_enum_type        category               = SIMLOCK_CATEGORY_3GPP_NW;
  simlock_slot_policy_enum_type     slot_policy            = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  simlock_code_in_list_enum_type    code_in_list           = SIMLOCK_CODE_IN_NOLIST;
  simlock_category_file_data_type   category_file_data;
  simlock_category_code_data_type   sim_code_data;
  uint32                            num_max_retries        = 0;
  uint32                            current_retries        = 0;

  if((sim_data_ptr == NULL)    ||
     (config_data_ptr == NULL) ||
     (subs_validity_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&sim_code_data, 0x00, sizeof(simlock_category_code_data_type));

  if(simlock_util_check_slot(slot) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_3gpp2_run_algorithm");

  slot_policy = config_data_ptr->slot_policy;

  if((slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS ||
      slot_policy == SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_ANY_SLOT ||
      slot_policy == SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_ANY_VALID_SLOT ||
      slot_policy == SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_ALL_VALID_SLOTS_EXPIRED) &&
     (slot != SIMLOCK_SLOT_1))
  {
    slot = SIMLOCK_SLOT_1;
  }

  simlock_status = simlock_3gpp2_parse_imsi(&sim_data_ptr->imsi,
                                            &sim_code_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("failed to parse imsi");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* initialize the failure reason as none */
  subs_validity_ptr->failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE;

  /* check if the MCC/MNC belongs to the
     test MCC/MNC */
  if(simlock_util_check_is_test_mcc_mnc(sim_code_data.mcc,
                                        sim_code_data.mnc))
  {
    subs_validity_ptr->is_subscription_valid = TRUE;
    return SIMLOCK_SUCCESS;
  }

  /* if the device is in emergency only mode, no subscription is valid */
  if(simlock_util_is_device_in_emergency_mode(config_data_ptr))
  {
    subs_validity_ptr->is_subscription_valid = FALSE;
    subs_validity_ptr->failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_EMERGENCY_ONLY;
    return SIMLOCK_SUCCESS;
  }

  for(category = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      category <= SIMLOCK_CATEGORY_3GPP2_RUIM; category++)
  {
    memset(&category_file_data, 0x00,
          sizeof(simlock_category_file_data_type));

    /* not all 3gpp2 categories are supported */
    simlock_status = simlock_util_check_category_supported(category);
    if(simlock_status == SIMLOCK_UNSUPPORTED)
    {
      continue;
    }

    /* set the subscription validity to TRUE. In case, none of the categories
         on a slot are enabled, the subscription is valid */
    subs_validity_ptr->is_subscription_valid = TRUE;

    if(!simlock_util_check_category_enabled(config_data_ptr, category, slot))
    {
      continue;
    }

    /* if the category is temporarily unlocked, continue */
    if(simlock_timer_check_is_temporary_unlock_set(slot, category))
    {
      continue;
    }

    SIMLOCK_MSG_HIGH_1("category 0x%x enabled", category);

    simlock_status = simlock_category_read_lock_data(category, slot, &category_file_data);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_2("read lock data failed, category: 0x%x,"
                        " slot: 0x%x", category, slot);
      subs_validity_ptr->is_subscription_valid = FALSE;
      subs_validity_ptr->failed_category = category;

      /* this frees the code data in the category */
      simlock_category_free_code_data(&category_file_data);
      break;
    }

    switch(category)
    {
      case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
        code_in_list = simlock_3gpp2_run_algorithm_nw_type1(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
        code_in_list = simlock_3gpp2_run_algorithm_nw_type2(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP2_RUIM:
        code_in_list = simlock_3gpp2_run_algorithm_ruim(sim_code_data, slot, &category_file_data);
        break;

      default:
        break;
    }

    num_max_retries = category_file_data.category_header.num_retries_max;
    current_retries = category_file_data.category_header.curr_retries;

    /* If blacklisted codes for this category matched, then no need to check
       other categories. This is because if more than one category is enabled
       as blacklist, then for a subscription to be valid, the codes of the all
       the enabled blacklisted categories should not match */
    if(code_in_list == SIMLOCK_CODE_IN_BLACKLIST)
    {
      subs_validity_ptr->is_subscription_valid = FALSE;
      subs_validity_ptr->failed_category = category;

      /* this frees the code data in the category */
      simlock_category_free_code_data(&category_file_data);
      break;
    }

    /* check if the code is whitelist. If yes and the codes for this category
       didn't match, then no need to check other categories. This is because
       if more than one category is enabled, then for a subscription to be
       valid, the codes of the all the enabled categories should match */
    if(simlock_category_is_whitelist(&category_file_data.category_header) &&
       (code_in_list == SIMLOCK_CODE_IN_NOLIST))
    {
      subs_validity_ptr->is_subscription_valid = FALSE;
      subs_validity_ptr->failed_category = category;

      /* this frees the code data in the category */
      simlock_category_free_code_data(&category_file_data);
      break;
    }

    /* check if the overall code indicator is blacklist. If yes and codes for
       this category matched in white and blacklist, then no need to check
       other categories */
    if(!simlock_category_is_whitelist(&category_file_data.category_header) &&
       (code_in_list == SIMLOCK_CODE_IN_WHITELIST_AND_BLACKLIST))
    {
      subs_validity_ptr->is_subscription_valid = FALSE;
      subs_validity_ptr->failed_category = category;

      /* this frees the code data in the category */
      simlock_category_free_code_data(&category_file_data);
      break;
    }

    /* this frees the code data in the category */
    simlock_category_free_code_data(&category_file_data);
  }

  /* If the subscription is not valid, check if the CK is present or not */
  if(!subs_validity_ptr->is_subscription_valid)
  {
    simlock_ck_present_type     ck_status = SIMLOCK_CK_PRESENT;

    subs_validity_ptr->failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_LOCKED_WITH_CK;

    if(simlock_util_check_ck_status(config_data_ptr,
                                    subs_validity_ptr->failed_category,
                                    slot,
                                    &ck_status) == SIMLOCK_SUCCESS)
    {
      if(ck_status == SIMLOCK_CK_NOT_PRESENT)
      {
        subs_validity_ptr->failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_LOCKED_WITHOUT_CK;
      }
      else
      {
        subs_validity_ptr->num_retries_max = num_max_retries;
        subs_validity_ptr->curr_retries = current_retries;
      }
    }
  }

  return simlock_status;
} /* simlock_3gpp2_run_algorithm */

#endif /* FEATURE_SIMLOCK */
