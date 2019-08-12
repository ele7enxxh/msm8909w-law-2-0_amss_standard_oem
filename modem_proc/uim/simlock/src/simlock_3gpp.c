/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   3 G P P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock 3gpp functions.


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_3gpp.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
09/07/16   vdc     Extend full and expired service policies on any slot
09/07/16   vdc     Apply same config for VALID_CARD_MUST_ON_ANY_SLOT policy
06/22/16   bcho    Return from algo after setting reason in emergency mode
06/17/16   vdc     Added support for SPN based SIMLOCK feature
03/09/16   av      Support for autolock whitelist
10/16/15   ar      Adding MCCs in list which have default MNC len as 3 digits
07/09/15   bcho    Support for whitelist and blacklist codes in same category
06/08/15   stv     Populate retry values in run algo response
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
02/17/15   vv      Indicate the reason for invalid subscription
01/14/15   bcho    Support for identifying test SIMs
01/08/15   vv      Fix for parsing the non-standard IMSIs
09/29/14   vv      Added support for emergency only mode
08/29/14   vv      Added support for temporary unlock
04/14/14   vv      Added support for wild char in lock codes
02/14/14   vv      Initial revision
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_3gpp.h"
#include "simlock_util.h"
#include "simlock_category.h"
#include "simlock_timer.h"

#define SIMLOCK_IMSI_BYTE_0          0x00
#define SIMLOCK_IMSI_BYTE_1          0x01
#define SIMLOCK_IMSI_BYTE_2          0x02
#define SIMLOCK_IMSI_BYTE_3          0x03
#define SIMLOCK_IMSI_BYTE_4          0x04
#define SIMLOCK_IMSI_BYTE_5          0x05

#define SIMLOCK_IMSI_LOW_BYTE        0x0F
#define SIMLOCK_IMSI_HIGH_BYTE       0xF0

#define SIMLOCK_MNC_2_DIGITS         0x02
#define SIMLOCK_MNC_3_DIGITS         0x03
#define SIMLOCK_MNC_LEN_BYTE         0x03

#define SIMLOCK_IS_VALID_MSIN_CHAR(c)              \
  ((c >= '0' && c <= '9') ? TRUE : FALSE)

/*===========================================================================
FUNCTION SIMLOCK_3GPP_GET_NUM_OF_MNC_DIGITS

DESCRIPTION
  This function determines the number of MNC digits

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  Number of MNC digits

SIDE EFFECTS
  NONE
===========================================================================*/
static uint8 simlock_3gpp_get_num_of_mnc_digits
(
  const simlock_ad_type      * ad_data_ptr
)
{
  /* Old SIM cards had only 2 digits MNC and EF-AD with 3 bytes only.
     Default to 2 digits to keep backward compatibility */
  uint8  num_mnc_digits        = SIMLOCK_MNC_2_DIGITS;

  if(ad_data_ptr == NULL)
  {
    return SIMLOCK_MNC_2_DIGITS;
  }

  /* EF-AD 4th byte will tell the MNC len which has been populated correctly
     inside mmgsdi_card_init_update_ad_mnc_len during initialization */
  if (ad_data_ptr->ad_len > SIMLOCK_MNC_LEN_BYTE &&
      ad_data_ptr->ad[SIMLOCK_MNC_LEN_BYTE] != 0xFF)
  {
    if ( ((ad_data_ptr->ad[SIMLOCK_MNC_LEN_BYTE] & 0x0F) == SIMLOCK_MNC_2_DIGITS ) ||
         ((ad_data_ptr->ad[SIMLOCK_MNC_LEN_BYTE] & 0x0F) == SIMLOCK_MNC_3_DIGITS ) )
    {
      num_mnc_digits = ad_data_ptr->ad[SIMLOCK_MNC_LEN_BYTE] & 0x0F;
    }
  }
  else
  {
    SIMLOCK_MSG_MED_1("EF-AD has invalid MNC length: 0x%x",
                      ad_data_ptr->ad[SIMLOCK_MNC_LEN_BYTE]);
  }

  return num_mnc_digits;
} /* simlock_3gpp_get_num_of_mnc_digits */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_PARSE_ICCID

DESCRIPTION
  This function parses the ICCID from the card and provides the data in
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
static simlock_result_enum_type simlock_3gpp_parse_iccid
(
  const simlock_iccid_file_type           * iccid_data_ptr,
  simlock_iccid_type                      * out_data_ptr
)
{
  uint8 index     = 0;
  uint8 digit_cnt = 0;

  if(iccid_data_ptr == NULL ||
     iccid_data_ptr->iccid_len == 0 ||
     iccid_data_ptr->iccid_len > SIMLOCK_ICCID_LEN ||
     out_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  for(index = 0; index < iccid_data_ptr->iccid_len; index++)
  {
    if((iccid_data_ptr->iccid_data[index] & 0x0F) <= 9)
    {
      out_data_ptr->iccid_data[digit_cnt] = (iccid_data_ptr->iccid_data[index]& 0x0F) + '0';
    }
    else
    {
      /* For invalid digit, use character 'a' */
      out_data_ptr->iccid_data[digit_cnt] = 'a';
    }
    digit_cnt++;

    if(((iccid_data_ptr->iccid_data[index] >> 4) & 0x0F) <= 9)
    {
      out_data_ptr->iccid_data[digit_cnt] = ((iccid_data_ptr->iccid_data[index] >> 4)& 0x0F) + '0';
    }
    else
    {
      /* For invalid digit, use character 'a' */
      out_data_ptr->iccid_data[digit_cnt] = 'a';
    }
    digit_cnt++;
  }

  out_data_ptr->iccid_len = digit_cnt;
  return SIMLOCK_SUCCESS;
} /* simlock_3gpp_parse_iccid */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_PARSE_IMSI

DESCRIPTION
  This function parses the 3GPP IMSI from the card and provides the data in
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
static simlock_result_enum_type simlock_3gpp_parse_imsi
(
  const simlock_imsi_type            * imsi_data_ptr,
  const simlock_ad_type              * ad_data_ptr,
  simlock_category_code_data_type    * out_data_ptr
)
{
  uint8                     num_mnc_digits    = 0;
  uint8                     imsi_len          = 0;
  uint8                     msin_len          = 0;
  uint8                     msin_offset       = 0;

  if((imsi_data_ptr == NULL)           ||
     (imsi_data_ptr->imsi_len <= 0)    ||
     (ad_data_ptr == NULL)             ||
     (out_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* first byte in the buffer is the length */
  imsi_len = imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_0];

  if(imsi_len < SIMLOCK_MIN_VALID_IMSI_LEN ||
     imsi_len > SIMLOCK_MAX_VALID_IMSI_LEN ||
     imsi_len > (imsi_data_ptr->imsi_len + 1))
  {
    SIMLOCK_MSG_ERR_0("imsi length invalid");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Get the number of mnc digits */
  num_mnc_digits = simlock_3gpp_get_num_of_mnc_digits(ad_data_ptr);

  SIMLOCK_MSG_MED_1("number of MNC digits, 0x%x", num_mnc_digits);

  /* -----------------------------------------------------------
    Parse MCC/MNC and MSIN from the IMSI:
    B0     B1       B2       B3       B4     B5   B6   B7    B8
    LEN  MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3  D3D2 D5D4 D7D6  D9D8

    2-digit MNC
    B0     B1       B2       B3       B4     B5   B6   B7    B8
    LEN  MCC1PB   MCC3MCC2 MNC2MNC1  D2D1   D4D3 D6D5 D8D7  D10D9
  -------------------------------------------------------------*/

  if(imsi_data_ptr->imsi_len > SIMLOCK_IMSI_BYTE_3)
  {
    /* Decode MCC digits in ASCII */
    out_data_ptr->mcc[0] = (((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_1] & SIMLOCK_IMSI_HIGH_BYTE) >> 4) + '0');
    out_data_ptr->mcc[1] = ((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_2] & SIMLOCK_IMSI_LOW_BYTE) + '0');
    out_data_ptr->mcc[2] = (((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_2] & SIMLOCK_IMSI_HIGH_BYTE) >> 4) + '0');

    /* Decode MNC digits in ASCII */
    out_data_ptr->mnc.mnc_data[0] = ((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_3] & SIMLOCK_IMSI_LOW_BYTE) + '0');
    out_data_ptr->mnc.mnc_data[1] = (((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_3] & SIMLOCK_IMSI_HIGH_BYTE) >> 4) + '0');
  }

  if(num_mnc_digits == 2)
  {
    out_data_ptr->mnc.mnc_len = 2;
  }
  else
  {
    out_data_ptr->mnc.mnc_len = SIMLOCK_MNC_MAX;
    if(imsi_data_ptr->imsi_len > SIMLOCK_IMSI_BYTE_4)
    {
      out_data_ptr->mnc.mnc_data[2] = ((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_4] & SIMLOCK_IMSI_LOW_BYTE) + '0');
    }
  }

  /* Decode d6 and d7 in ASCII. Irrespective of MNC length (2 or 3 digits),
     msin_data[0] is considered d6
     msin_data[1] is considered d7 */

  /* Decode MSIN in ASCII */
  do
  {
    char c = '0';

    if(imsi_data_ptr->imsi_len > SIMLOCK_IMSI_BYTE_4)
    {
      if(num_mnc_digits == 2)
      {
        c = ((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_4] & SIMLOCK_IMSI_LOW_BYTE) + '0');
        if(!SIMLOCK_IS_VALID_MSIN_CHAR(c))
        {
          break;
        }
        out_data_ptr->msin.msin_data[msin_len++] = c;

        c = (((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_4] & SIMLOCK_IMSI_HIGH_BYTE) >> 4) + '0');
        if(!SIMLOCK_IS_VALID_MSIN_CHAR(c))
        {
          break;
        }
        out_data_ptr->msin.msin_data[msin_len++] = c;
      }
      else
      {
        c = (((imsi_data_ptr->imsi[SIMLOCK_IMSI_BYTE_4] & SIMLOCK_IMSI_HIGH_BYTE) >> 4) + '0');
        if(!SIMLOCK_IS_VALID_MSIN_CHAR(c))
        {
          break;
        }
        out_data_ptr->msin.msin_data[msin_len++] = c;
      }
    }

    if(imsi_data_ptr->imsi_len > SIMLOCK_IMSI_BYTE_5)
    {
      for(msin_offset = SIMLOCK_IMSI_BYTE_5; msin_offset <= imsi_len; msin_offset++)
      {
        c = ((imsi_data_ptr->imsi[msin_offset] & SIMLOCK_IMSI_LOW_BYTE) + '0');
        if(!SIMLOCK_IS_VALID_MSIN_CHAR(c))
        {
          break;
        }

        if((msin_len + 1) > SIMLOCK_MSIN_MAX)
        {
          SIMLOCK_MSG_ERR_1("Invalid MSIN length: 0x%x", msin_len);
          return SIMLOCK_GENERIC_ERROR;
        }

        out_data_ptr->msin.msin_data[msin_len++] = c;

        c = (((imsi_data_ptr->imsi[msin_offset] & SIMLOCK_IMSI_HIGH_BYTE) >> 4) + '0');
        if(!SIMLOCK_IS_VALID_MSIN_CHAR(c))
        {
          break;
        }

        if((msin_len + 1) > SIMLOCK_MSIN_MAX)
        {
          SIMLOCK_MSG_ERR_1("Invalid MSIN length: 0x%x", msin_len);
          return SIMLOCK_GENERIC_ERROR;
        }

        out_data_ptr->msin.msin_data[msin_len++] = c;
      }
    }
  } while(0);

  out_data_ptr->msin.msin_len = msin_len;
  return SIMLOCK_SUCCESS;
} /* simlock_3gpp_parse_imsi */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_SIM

DESCRIPTION
  This function validates the 3GPP SIM code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_sim
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

  SIMLOCK_MSG_LOW_6("simlock_3gpp_run_algorithm_sim: From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2]);

  SIMLOCK_MSG_LOW_6("From IMSI - msin[0]: 0x%x, msin[1]: 0x%x, msin[2]: 0x%x, msin[3]: 0x%x, msin[4]: 0x%x, msin[5]: 0x%x",
                    sim_code_data.msin.msin_data[0], sim_code_data.msin.msin_data[1], sim_code_data.msin.msin_data[2],
                    sim_code_data.msin.msin_data[3], sim_code_data.msin.msin_data[4], sim_code_data.msin.msin_data[5]);

  SIMLOCK_MSG_LOW_4("From IMSI - msin[6]: 0x%x, msin[7]: 0x%x, msin[8]: 0x%x, msin[9]: 0x%x",
                    sim_code_data.msin.msin_data[6], sim_code_data.msin.msin_data[7],
                    sim_code_data.msin.msin_data[8], sim_code_data.msin.msin_data[9]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* Compare the IMSI from the file and SIM data */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_6("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2]);

    SIMLOCK_MSG_LOW_6("From file - msin[0]: 0x%x, msin[1]: 0x%x, msin[2]: 0x%x, msin[3]: 0x%x, msin[4]: 0x%x, msin[5]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[0],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[1],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[2],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[3],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[4],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[5]);

    SIMLOCK_MSG_LOW_4("From file - , msin[6]: 0x%x, msin[7]: 0x%x, msin[8]: 0x%x, msin[9]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[6],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[7],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[8],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[9]);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                 &&
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
       data if it matches with any of the codes present in the whitelist of
       auto-lock codes.
     In both these cases, we update the lock code data in the category data
     file */
  if(category_data_ptr->category_header.auto_lock  &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      code_in_whitelist))
  {
    SIMLOCK_MSG_MED_0("autolock enabled, updating code data");

    /* format the category data to write to file */
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;
    sim_code_data.spn.spn_valid = FALSE;
    memset(sim_code_data.spn.spn_data, SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_SPN_LEN);
    memset(&sim_code_data.iccid, 0x00, sizeof(sim_code_data.iccid));
    sim_code_data.iccid.iccid_len = 0;
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

  SIMLOCK_MSG_MED_1("sim code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_sim */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_CP

DESCRIPTION
  This function validates the 3GPP CP code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_cp
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

  SIMLOCK_MSG_LOW_8("simlock_3gpp_run_algorithm_cp: From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x, From SIM - GID1 Len: 0x%x, GID2 Len: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2],
                    sim_code_data.gid1.gid_len, sim_code_data.gid2.gid_len);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* compare MCCs/MNCs, gid1 and gid2 from the file and IMSI */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_8("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x, From file - gid1 Len: 0x%x, gid2 Len: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2],
                      category_data_ptr->code_data_ptr[code_index].gid1.gid_len,
                      category_data_ptr->code_data_ptr[code_index].gid2.gid_len);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                  &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      /* the card should be configured to have a valid gid1
         and gid2 (any value between, 0x00 - 0xFE). If not,
         the card is not valid */
      if((sim_code_data.gid1.gid_len > 0) &&
         (sim_code_data.gid2.gid_len > 0))
      {
        if(simlock_util_check_if_gid_matches(category_data_ptr->code_data_ptr[code_index].gid1,
                                             sim_code_data.gid1)                                &&
           simlock_util_check_if_gid_matches(category_data_ptr->code_data_ptr[code_index].gid2,
                                             sim_code_data.gid2))
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

    if((sim_code_data.gid1.gid_len == 0) ||
       (sim_code_data.gid2.gid_len == 0))
    {
      SIMLOCK_MSG_ERR_2("autolock is enabled, but gid1 : 0x%x or"
                        " gid2 :0x%x is invalid",
                         sim_code_data.gid1.gid_data[0], sim_code_data.gid2.gid_data[0]);
      return SIMLOCK_CODE_IN_NOLIST;
    }

    /* format the category data to write to file.
       set the GID1 and GID2 lengths explicitly to 1 since
       in autolock case, only 1 byte for GIDs is supported
       as per the 22.022 Spec */
    sim_code_data.msin.msin_len = 0;
    memset(sim_code_data.msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
    sim_code_data.spn.spn_valid = FALSE;
    memset(sim_code_data.spn.spn_data, SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_SPN_LEN);
    memset(&sim_code_data.iccid, 0x00, sizeof(sim_code_data.iccid));
    sim_code_data.iccid.iccid_len = 0;
    sim_code_data.gid1.gid_len = SIMLOCK_MIN_GID_LEN;
    sim_code_data.gid2.gid_len = SIMLOCK_MIN_GID_LEN;

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

  SIMLOCK_MSG_MED_1("cp code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_cp */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_SP

DESCRIPTION
  This function validates the 3GPP SP code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_sp
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

  SIMLOCK_MSG_LOW_7("simlock_3gpp_run_algorithm_sp: From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x, From SIM - gid1 Len: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2],
                    sim_code_data.gid1.gid_len);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* compare MCCs/MNCs and gid1 from the file and IMSI */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_7("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x, gid1 Len: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2],
                      category_data_ptr->code_data_ptr[code_index].gid1.gid_len);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                  &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      /* the card should be configured to have a valid gid1
        (any value between, 0x00 - 0xFE). If not,
        the card is not valid */
      if(sim_code_data.gid1.gid_len > 0)
      {
        if(simlock_util_check_if_gid_matches(category_data_ptr->code_data_ptr[code_index].gid1,
                                             sim_code_data.gid1))
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

    if(sim_code_data.gid1.gid_len == 0)
    {
      SIMLOCK_MSG_ERR_1("autolock is enabled, but gid1 : 0x%x is invalid",
                        sim_code_data.gid1.gid_data[0]);
      return code_in_list;
    }

    /* format the category data to write to file.
       set the GID1 length explicitly to 1, since in
       autolock case, only 1 byte for GID is supported,
       as per the 22.022 Spec */
    sim_code_data.msin.msin_len = 0;
    memset(sim_code_data.msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
    sim_code_data.gid1.gid_len = SIMLOCK_MIN_GID_LEN;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;
    sim_code_data.spn.spn_valid = FALSE;
    memset(sim_code_data.spn.spn_data, SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_SPN_LEN);
    memset(&sim_code_data.iccid, 0x00, sizeof(sim_code_data.iccid));
    sim_code_data.iccid.iccid_len = 0;

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

  SIMLOCK_MSG_MED_1("sp code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_sp */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_NS

DESCRIPTION
  This function validates the 3GPP NS code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_ns
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

  SIMLOCK_MSG_LOW_8("simlock_3gpp_run_algorithm_ns: From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x, d6: 0x%x, d7: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2],
                    sim_code_data.msin.msin_data[0], sim_code_data.msin.msin_data[1]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* compare MCCs/MNCs, imsi_d6 and imsi_d7 from the file and IMSI */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_8("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x, d6: 0x%x, d7: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[0],
                      category_data_ptr->code_data_ptr[code_index].msin.msin_data[1]);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                  &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      if(simlock_util_check_if_imsi_digit_matches(category_data_ptr->code_data_ptr[code_index].msin.msin_data[0],
                                                  sim_code_data.msin.msin_data[0])                        &&
         simlock_util_check_if_imsi_digit_matches(category_data_ptr->code_data_ptr[code_index].msin.msin_data[1],
                                                  sim_code_data.msin.msin_data[1]))
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

    /* format the category data to write to file */
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;
    sim_code_data.spn.spn_valid = FALSE;
    memset(sim_code_data.spn.spn_data, SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_SPN_LEN);
    memset(&sim_code_data.iccid, 0x00, sizeof(sim_code_data.iccid));
    sim_code_data.iccid.iccid_len = 0;

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

  SIMLOCK_MSG_MED_1("ns code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_ns */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_NW

DESCRIPTION
  This function validates the 3GPP NW code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_nw
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

  SIMLOCK_MSG_LOW_6("simlock_3gpp_run_algorithm_nw: From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
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
    SIMLOCK_MSG_LOW_6("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2]);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                  &&
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

    /* format the category data to write to file */
    sim_code_data.msin.msin_len = 0;
    memset(sim_code_data.msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;
    sim_code_data.spn.spn_valid = FALSE;
    memset(sim_code_data.spn.spn_data, SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_SPN_LEN);
    memset(&sim_code_data.iccid, 0x00, sizeof(sim_code_data.iccid));
    sim_code_data.iccid.iccid_len = 0;

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

  SIMLOCK_MSG_MED_1("nw code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_nw */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_SPN

DESCRIPTION
  This function validates the 3GPP SPN code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_spn
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

  SIMLOCK_MSG_LOW_6("simlock_3gpp_run_algorithm_spn: From IMSI - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* Compare the IMSI from the file and SIM data */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    SIMLOCK_MSG_LOW_6("From file - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                      category_data_ptr->code_data_ptr[code_index].mcc[0],
                      category_data_ptr->code_data_ptr[code_index].mcc[1],
                      category_data_ptr->code_data_ptr[code_index].mcc[2],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[0],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[1],
                      category_data_ptr->code_data_ptr[code_index].mnc.mnc_data[2]);

    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                 &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      if(simlock_util_check_if_spn_matches(&category_data_ptr->code_data_ptr[code_index].spn,
                                           &sim_code_data.spn))
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
       data if it matches with any of the codes present in the whitelist of
       auto-lock codes.
     In both these cases, we update the lock code data in the category data
     file */
  if(category_data_ptr->category_header.auto_lock  &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      code_in_whitelist))
  {
    SIMLOCK_MSG_MED_0("autolock enabled, updating code data");

    /* format the category data to write to file */
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;
    sim_code_data.msin.msin_len = 0;
    memset(sim_code_data.msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
    memset(&sim_code_data.iccid, 0x00, sizeof(sim_code_data.iccid));
    sim_code_data.iccid.iccid_len = 0;

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

  SIMLOCK_MSG_MED_1("sim code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_spn */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM_ICCID

DESCRIPTION
  This function validates the 3GPP ICCID code from the card and the
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
static simlock_code_in_list_enum_type simlock_3gpp_run_algorithm_iccid
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

  SIMLOCK_MSG_LOW_6("run_algo_iccid: From sim - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                    sim_code_data.mcc[0], sim_code_data.mcc[1], sim_code_data.mcc[2],
                    sim_code_data.mnc.mnc_data[0], sim_code_data.mnc.mnc_data[1], sim_code_data.mnc.mnc_data[2]);

  /* if autolock is not enabled, atleast one valid code is expected */
  if(!category_data_ptr->category_header.auto_lock &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      category_data_ptr->code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("no valid code data");
    return code_in_list;
  }

  /* Compare MCC, MNC and ICCID from the file and SIM data */
  for (code_index = 0; code_index < category_data_ptr->category_header.num_of_codes;
       code_index++)
  {
    if(simlock_util_check_if_mcc_matches(category_data_ptr->code_data_ptr[code_index].mcc,
                                         sim_code_data.mcc)                                 &&
       simlock_util_check_if_mnc_matches(category_data_ptr->code_data_ptr[code_index].mnc,
                                         sim_code_data.mnc))
    {
      if(simlock_util_check_if_iccid_matches(category_data_ptr->code_data_ptr[code_index].iccid,
                                             sim_code_data.iccid))
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
       data if it matches with any of the codes present in the whitelist of
       auto-lock codes.
     In both these cases, we update the lock code data in the category data
     file */
  if(category_data_ptr->category_header.auto_lock  &&
     (category_data_ptr->category_header.num_of_codes == 0 ||
      code_in_whitelist) &&
      sim_code_data.iccid.iccid_len > 0)
  {
    SIMLOCK_MSG_MED_0("autolock enabled, updating code data");

    /* format the category data to write to file */
    memset(sim_code_data.gid1.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid1.gid_len = 0;
    memset(sim_code_data.gid2.gid_data, SIMLOCK_LOCK_CODE_WILD_DIGIT,
           SIMLOCK_MAX_GID_LEN);
    sim_code_data.gid2.gid_len = 0;
    sim_code_data.msin.msin_len = 0;
    memset(sim_code_data.msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
    sim_code_data.spn.spn_valid = FALSE;
    memset(sim_code_data.spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);

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

  SIMLOCK_MSG_MED_1("iccid code in list type: 0x%x", code_in_list);
  return code_in_list;
} /* simlock_3gpp_run_algorithm_iccid */


/*===========================================================================
FUNCTION SIMLOCK_3GPP_RUN_ALGORITHM

DESCRIPTION
  This function validates the 3GPP subscription on a card by verifying the
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
simlock_result_enum_type simlock_3gpp_run_algorithm
(
  const simlock_subscription_data_type    * sim_data_ptr,
  simlock_slot_enum_type                    slot,
  simlock_config_data_type                * config_data_ptr,
  simlock_subscription_validity_info_type * subs_validity_ptr
)
{
  simlock_result_enum_type          simlock_status          = SIMLOCK_SUCCESS;
  simlock_category_enum_type        category                = SIMLOCK_CATEGORY_3GPP_NW;
  simlock_slot_policy_enum_type     slot_policy             = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  simlock_code_in_list_enum_type    code_in_list            = SIMLOCK_CODE_IN_NOLIST;
  simlock_category_file_data_type   category_file_data;
  simlock_category_code_data_type   sim_code_data;
  uint32                            num_max_retries         = 0;
  uint32                            current_retries         = 0;

  if((sim_data_ptr == NULL)    ||
     (config_data_ptr == NULL) ||
     (subs_validity_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&sim_code_data, 0x00, sizeof(simlock_category_code_data_type));

  /* check the slot */
  if(simlock_util_check_slot(slot) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_3gpp_run_algorithm");

  /* check the slot policy */
  slot_policy = config_data_ptr->slot_policy;

  /* if the slot policy is same on all the available slots or valid card must 
     on any slot, then it is expected to have one category file per each
     enabled category i.e. all the codes for that category are in one file */
  if((slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS ||
      slot_policy == SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_ANY_SLOT ||
      slot_policy == SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_ANY_VALID_SLOT ||
      slot_policy == SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_ALL_VALID_SLOTS_EXPIRED) &&
     (slot != SIMLOCK_SLOT_1))
  {
    slot = SIMLOCK_SLOT_1;
  }

  simlock_status = simlock_3gpp_parse_imsi(&sim_data_ptr->imsi,
                                           &sim_data_ptr->ad,
                                           &sim_code_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("failed to parse imsi");
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_3gpp_parse_iccid(&sim_data_ptr->iccid, 
                                            &sim_code_data.iccid);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_HIGH_0("failed to parse ICCID");
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

  sim_code_data.gid1 = sim_data_ptr->gid1;
  sim_code_data.gid2 = sim_data_ptr->gid2;
  sim_code_data.spn  = sim_data_ptr->spn;

  /* if the device is in emergency only mode, no subscription is valid */
  if(simlock_util_is_device_in_emergency_mode(config_data_ptr))
  {
    subs_validity_ptr->is_subscription_valid = FALSE;
    subs_validity_ptr->failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_EMERGENCY_ONLY;
    return SIMLOCK_SUCCESS;
  }

  for(category = SIMLOCK_CATEGORY_3GPP_NW; category <= SIMLOCK_CATEGORY_3GPP_SIM;
      category++)
  {
    memset(&category_file_data, 0x00,
            sizeof(simlock_category_file_data_type));

    /* set the subscription validity to TRUE. In case, none of the categories
       on a slot are enabled, the subscription is valid */
    subs_validity_ptr->is_subscription_valid = TRUE;

    /* if the category is not enabled, nothing to do */
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

    /* read the category data */
    simlock_status = simlock_category_read_lock_data(category, slot,
                                                     &category_file_data);
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
      case SIMLOCK_CATEGORY_3GPP_NW:
        code_in_list = simlock_3gpp_run_algorithm_nw(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP_NS:
        code_in_list = simlock_3gpp_run_algorithm_ns(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP_SP:
        code_in_list = simlock_3gpp_run_algorithm_sp(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP_CP:
        code_in_list = simlock_3gpp_run_algorithm_cp(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP_SIM:
        code_in_list = simlock_3gpp_run_algorithm_sim(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP_SPN:
        code_in_list = simlock_3gpp_run_algorithm_spn(sim_code_data, slot, &category_file_data);
        break;

      case SIMLOCK_CATEGORY_3GPP_ICCID:
        code_in_list = simlock_3gpp_run_algorithm_iccid(sim_code_data, slot, &category_file_data);
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
} /* simlock_run_algorithm_3gpp */

#endif /* FEATURE_SIMLOCK */
