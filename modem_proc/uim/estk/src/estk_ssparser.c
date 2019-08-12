/*===========================================================================
FILE:  estk_ssparser.c


GENERAL DESCRIPTION:ENHANCED STK layer 
  Implements the Supplementary Services String parser for ESTK

  Copyright (c) 2008, 2010 - 2014 QUALCOMM Technologies, Inc(QTI). 
  All Rights Reserved

  QUALCOMM Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies, Inc(QTI) and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission ofQUALCOMM Technologies, Inc(QTI).

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_ssparser.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   gs      Removed the ESTK featurization
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
10/04/13   gm      F3 message reduction
10/19/12   hk      Memory Optimization in ESTK through F3 removal
05/04/11   adp     Fixing Compiler errors in UIM main/latest MOB COPPER 
02/15/10   nb      Moved Dual Slot Changes
11/03/08   sk      Featurized ESTK based on external interface support
09/14/08   sk      Fixed lint errors
09/10/08   adg/sk  Send SS fixes
07/29/08   adg     Initial version

===========================================================================*/

/*===========================================================================
                                INCLUDE FILES
===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"

#ifdef FEATURE_ESTK
#include <string.h>
#include "estk_ssparser.h"

/*===========================================================================
                        INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/* Alternative SS sytring character representations */
#define SSPARSER_ALT_STAR             0x0A
#define SSPARSER_ALT_HASH             0x0B

/* Minimum valid SS string length - op + sc + terminator */
#define SSPARSER_SS_STRING_MIN_LENGTH (1 + 2 + 1)

/* Extract one of the optional SI fields */
#define SSPARSER_EXTRACT_SI_FIELD(remaining, next_ptr, si_ptr, si_length)             \
if ( ((remaining) != 0) &&                                                            \
     ((*(next_ptr) == '*') || (*(next_ptr) == SSPARSER_ALT_STAR)) )                   \
{                                                                                     \
  (si_ptr) = ++next_ptr;                                                           \
  --(remaining);                                                                      \
                                                                                      \
  while ( ((remaining) != 0) &&                                                       \
          (*(next_ptr) != '*') && (*(next_ptr) != '#') &&                             \
          (*(next_ptr) != SSPARSER_ALT_STAR) && (*(next_ptr) != SSPARSER_ALT_HASH) )  \
  {                                                                                   \
    ++(next_ptr);                                                                     \
    --(remaining);                                                                    \
    ++(si_length);                                                                    \
  }                                                                                   \
                                                                                      \
  if ((si_length) == 0)                                                               \
    (si_ptr) = NULL;                                                                  \
}

/* Structure type for SC parse table */
typedef struct
{
  uint32                      sc_raw;
  ssparser_sc_enum_type       sc_enum;
  const char                 *sc_name;
  ssparser_si_type_enum_type  sia_type;
  ssparser_si_type_enum_type  sib_type;
  ssparser_si_type_enum_type  sic_type;
}
ssparser_sc_parse_table_entry_type;

/* Structure type for conversion of tele- and bearer-service codes */
typedef struct
{
  uint8                      bscode_raw;
  uint8                      bscode_cooked;
  ssparser_bs_type_enum_type bscode_type;
}
ssparser_bscode_parse_table_entry_type;


/*===========================================================================
                               LOCAL VARIABLES
===========================================================================*/

static const ssparser_sc_parse_table_entry_type ssparser_sc_parse_table[] =
{
  {  66, SSPARSER_SC_CD,        "CD",               SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  30, SSPARSER_SC_CLIP,      "CLIP",             SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  31, SSPARSER_SC_CLIR,      "CLIR",             SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  76, SSPARSER_SC_CoLP,      "CoLP",             SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  77, SSPARSER_SC_CoLR,      "CoLR",             SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  21, SSPARSER_SC_CFU,       "CFU",              SSPARSER_SI_TELNUM, SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  {  67, SSPARSER_SC_CFB,       "CFB",              SSPARSER_SI_TELNUM, SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  {  61, SSPARSER_SC_CFNRy,     "CFNRy",            SSPARSER_SI_TELNUM, SSPARSER_SI_BS,     SSPARSER_SI_BYTE },
  {  62, SSPARSER_SC_CFNRc,     "CFNRc",            SSPARSER_SI_TELNUM, SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  {   2, SSPARSER_SC_CFAll,     "CF (All)",         SSPARSER_SI_TELNUM, SSPARSER_SI_BS,     SSPARSER_SI_BYTE },
  {   4, SSPARSER_SC_CFAllCond, "CF (Conditional)", SSPARSER_SI_TELNUM, SSPARSER_SI_BS,     SSPARSER_SI_BYTE },
  {  43, SSPARSER_SC_CW,        "CW",               SSPARSER_SI_BS,     SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  33, SSPARSER_SC_BAOC,      "BAOC",             SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  { 331, SSPARSER_SC_BOIC,      "BOIC",             SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  { 332, SSPARSER_SC_BOICexHC,  "BOIC (Exc Home)",  SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  {  35, SSPARSER_SC_BAIC,      "BAIC",             SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  { 351, SSPARSER_SC_BICRoam,   "BIC (Roaming)",    SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  { 330, SSPARSER_SC_BarAll,    "All Barring",      SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  { 333, SSPARSER_SC_BarOut,    "Outgoing Barring", SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  { 353, SSPARSER_SC_BarInc,    "Incoming Barring", SSPARSER_SI_PWD,    SSPARSER_SI_BS,     SSPARSER_SI_NONE },
  {  96, SSPARSER_SC_ECT,       "ECT",              SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  {  37, SSPARSER_SC_CCBS,      "CCBS",             SSPARSER_SI_BYTE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
  { 300, SSPARSER_SC_CNAP,      "CNAP",             SSPARSER_SI_NONE,   SSPARSER_SI_NONE,   SSPARSER_SI_NONE },
};

#define SSPARSER_SC_PARSE_TABLE_ENTRIES     (sizeof(ssparser_sc_parse_table) / \
                                             sizeof(ssparser_sc_parse_table[0]))


static const ssparser_bscode_parse_table_entry_type ssparser_bscode_parse_table[] =
{
  { 10, allTeleservices,                  SSPARSER_BST_TELE   },
  { 11, telephony,                        SSPARSER_BST_TELE   },
  { 12, allDataTeleservices,              SSPARSER_BST_TELE   },
  { 13, allFacsimileTransmissionServices, SSPARSER_BST_TELE   },
  { 16, allShortMessageServices,          SSPARSER_BST_TELE   },
  { 19, allTeleservices_ExeptSMS,         SSPARSER_BST_TELE   },

  { 50, allPLMN_specificTS,               SSPARSER_BST_TELE   },
  { 51, plmn_specificTS_1,                SSPARSER_BST_TELE   },
  { 52, plmn_specificTS_2,                SSPARSER_BST_TELE   },
  { 53, plmn_specificTS_3,                SSPARSER_BST_TELE   },
  { 54, plmn_specificTS_4,                SSPARSER_BST_TELE   },
  { 55, plmn_specificTS_5,                SSPARSER_BST_TELE   },
  { 56, plmn_specificTS_6,                SSPARSER_BST_TELE   },
  { 57, plmn_specificTS_7,                SSPARSER_BST_TELE   },
  { 58, plmn_specificTS_8,                SSPARSER_BST_TELE   },
  { 59, plmn_specificTS_9,                SSPARSER_BST_TELE   },
  { 60, plmn_specificTS_A,                SSPARSER_BST_TELE   },
  { 61, plmn_specificTS_B,                SSPARSER_BST_TELE   },
  { 62, plmn_specificTS_C,                SSPARSER_BST_TELE   },
  { 63, plmn_specificTS_D,                SSPARSER_BST_TELE   },
  { 64, plmn_specificTS_E,                SSPARSER_BST_TELE   },
  { 65, plmn_specificTS_F,                SSPARSER_BST_TELE   },

  { 20, allBearerServices,                SSPARSER_BST_BEARER },
  { 21, allAsynchronousServices,          SSPARSER_BST_BEARER },
  { 22, allSynchronousServices,           SSPARSER_BST_BEARER },
  { 24, allDataCircuitSynchronous,        SSPARSER_BST_BEARER },
  { 25, allDataCircuitAsynchronous,       SSPARSER_BST_BEARER },

  { 70, allPLMN_specificTS,               SSPARSER_BST_BEARER },
  { 71, plmn_specificBS_1,                SSPARSER_BST_BEARER },
  { 72, plmn_specificBS_2,                SSPARSER_BST_BEARER },
  { 73, plmn_specificBS_3,                SSPARSER_BST_BEARER },
  { 74, plmn_specificBS_4,                SSPARSER_BST_BEARER },
  { 75, plmn_specificBS_5,                SSPARSER_BST_BEARER },
  { 76, plmn_specificBS_6,                SSPARSER_BST_BEARER },
  { 77, plmn_specificBS_7,                SSPARSER_BST_BEARER },
  { 78, plmn_specificBS_8,                SSPARSER_BST_BEARER },
  { 79, plmn_specificBS_9,                SSPARSER_BST_BEARER },
  { 80, plmn_specificBS_A,                SSPARSER_BST_BEARER },
  { 81, plmn_specificBS_B,                SSPARSER_BST_BEARER },
  { 82, plmn_specificBS_C,                SSPARSER_BST_BEARER },
  { 83, plmn_specificBS_D,                SSPARSER_BST_BEARER },
  { 84, plmn_specificBS_E,                SSPARSER_BST_BEARER },
  { 85, plmn_specificBS_F,                SSPARSER_BST_BEARER }
};

#define SSPARSER_BSCODE_PARSE_TABLE_ENTRIES (sizeof(ssparser_bscode_parse_table) / \
                                             sizeof(ssparser_bscode_parse_table[0]))


#if defined(SSPARSER_DEBUG)
/*===========================================================================

FUNCTION: ssparser_show_result

DESCRIPTION:
  Show the result of the SS parser.

PARAMETERS:
  ss_string_ptr: [Input] Pointer to the original SS string
  ss_length:     [Input] Length of the original ss string
  ss_info_ptr:   [Input] Pointer to the resulting parsed form

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void ssparser_show_result(
  const uint8                        *ss_string_ptr,
  uint32                              ss_length,
  const ssparser_ss_string_info_type *ss_info_ptr)
{
  const char *op_name  = "Unknown";
  const char *si_name  = "Unknown";
  uint32      sc_index = 0;

  if (ss_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("ss_info_ptr NULL");
    return;
  }

  UIM_MSG_HIGH_2("ssparser_show_result Input: \"%.*s\"",
                 	 ss_length, ss_string_ptr );

  switch (ss_info_ptr->op)
  {
    case SSPARSER_OP_ACTIVATE:    op_name = "ACTIVATE";    break;
    case SSPARSER_OP_DEACTIVATE:  op_name = "DEACTIVATE";  break;
    case SSPARSER_OP_INTERROGATE: op_name = "INTERROGATE"; break;
    case SSPARSER_OP_REGISTER:    op_name = "REGISTER";    break;
    case SSPARSER_OP_ERASE:       op_name = "ERASE";       break;
    case SSPARSER_OP_INVALID:     op_name = "INVALID";     break;
  }
  UIM_MSG_HIGH_3("  Operation: %s (%.*s)",
                 op_name, ss_info_ptr->op_length, ss_info_ptr->op_ptr);

  for (sc_index = 0; sc_index < SSPARSER_SC_PARSE_TABLE_ENTRIES; ++sc_index)
  {
    if (ssparser_sc_parse_table[sc_index].sc_enum == ss_info_ptr->sc)
    {
      break;
    }
  }
  UIM_MSG_HIGH_3("  Service Code: %s (%.*s)",
                 ssparser_sc_parse_table[sc_index].sc_name,
                 ss_info_ptr->sc_length,
                 ss_info_ptr->sc_ptr);

  switch (ss_info_ptr->sia_type)
  {
    case SSPARSER_SI_NONE:   si_name = "None";       break;
    case SSPARSER_SI_TELNUM: si_name = "Tel Number"; break;
    case SSPARSER_SI_BS:     si_name = "Basic Svc";  break;
    case SSPARSER_SI_BYTE:   si_name = "Byte";       break;
    case SSPARSER_SI_PWD:    si_name = "Password";   break;
    default:                 si_name = "Invalid";    break;
  }
  if (ss_info_ptr->sia_length == 0)
  {
    UIM_MSG_HIGH_1("  Supplementary Info A: %s, ABSENT", si_name);
  }
  else
  {
    UIM_MSG_HIGH_3("  Supplementary Info A: %s, \"%.*s\"",
                   si_name, ss_info_ptr->sia_length, ss_info_ptr->sia_ptr);
  }

  switch (ss_info_ptr->sib_type)
  {
    case SSPARSER_SI_NONE:   si_name = "None";       break;
    case SSPARSER_SI_TELNUM: si_name = "Tel Number"; break;
    case SSPARSER_SI_BS:     si_name = "Basic Svc";  break;
    case SSPARSER_SI_BYTE:   si_name = "Byte";       break;
    case SSPARSER_SI_PWD:    si_name = "Password";   break;
    default:                 si_name = "Invalid";    break;
  }
  if (ss_info_ptr->sib_length == 0)
  {
    UIM_MSG_HIGH_1("  Supplementary Info B: %s, ABSENT", si_name);
  }
  else
  {
    UIM_MSG_HIGH_3("  Supplementary Info B: %s, \"%.*s\"",
                   si_name, ss_info_ptr->sib_length, ss_info_ptr->sib_ptr);
  }

  switch (ss_info_ptr->sic_type)
  {
    case SSPARSER_SI_NONE:   si_name = "None";       break;
    case SSPARSER_SI_TELNUM: si_name = "Tel Number"; break;
    case SSPARSER_SI_BS:     si_name = "Basic Svc";  break;
    case SSPARSER_SI_BYTE:   si_name = "Byte";       break;
    case SSPARSER_SI_PWD:    si_name = "Password";   break;
    default:                 si_name = "Invalid";    break;
  }
  if (ss_info_ptr->sic_length == 0)
  {
    UIM_MSG_HIGH_1("  Supplementary Info C: %s, ABSENT", si_name);
  }
  else
  {
    UIM_MSG_HIGH_3("  Supplementary Info C: %s, \"%.*s\"",
                   si_name, ss_info_ptr->sic_length, ss_info_ptr->sic_ptr);
  }

} /* ssparser_show_result */
#endif /* defined(SSPARSER_DEBUG) */

/*===========================================================================

FUNCTION: ssparser_isdigit

DESCRIPTION:
  Determine whether the input is a decimal digit or not.

PARAMETERS:
  input:  [Input] input the potential digit to be tested

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE  - if it is a digit
  FALSE - otherwise

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static boolean ssparser_isdigit(uint8 input)
{
  if ((input >= '0') && (input <= '9'))
    return TRUE;

  if (input <= 0x09)
    return TRUE;

  return FALSE;

} /* ssparser_isdigit */

/*===========================================================================

FUNCTION: ssparser_extract_op

DESCRIPTION:
  Extract the operation field from the SS string. Valid operation strings 
  are defined in 22.030

PARAMETERS:
  ss_string_ptr:  [Input] Pointer to the SS string
  ss_op_ptr:      [Input] Pointer to an enum to receive the operation

DEPENDENCIES:
  None

RETURN VALUE:
  A pointer to the first character in the SS string following the operation 
  field, of NULL on failure.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static const uint8 *ssparser_extract_op(
  const uint8           *ss_string_ptr,
  ssparser_op_enum_type *ss_op_ptr)
{
  ssparser_op_enum_type ss_op = SSPARSER_OP_INVALID;

  UIM_MSG_HIGH_0("ssparser_extract_op");

  if ((ss_op_ptr == NULL) || (ss_string_ptr == NULL))
  {
    UIM_MSG_ERR_0("ss_string_ptr or ss_op_ptr was NULL");
    return NULL;
  }

  switch (*ss_string_ptr)
  {
    case '*': case SSPARSER_ALT_STAR:
      switch (*++ss_string_ptr)
      {
        case '*': case SSPARSER_ALT_STAR:
          ss_op = SSPARSER_OP_REGISTER;
          ++ss_string_ptr;
          break;

        case '#': case SSPARSER_ALT_HASH:
          ss_op = SSPARSER_OP_INTERROGATE;
          ++ss_string_ptr;
          break;

        default:
          ss_op = SSPARSER_OP_ACTIVATE;
          break;
      }
      break;

    case '#': case SSPARSER_ALT_HASH:
      switch (*++ss_string_ptr)
      {
        case '#': case SSPARSER_ALT_HASH:
          ss_op = SSPARSER_OP_ERASE;
          ++ss_string_ptr;
          break;

        default:
          ss_op = SSPARSER_OP_DEACTIVATE;
          break;
      }
      break;

    default:
      break;
  }

  *ss_op_ptr = ss_op;

  return ((ss_op == SSPARSER_OP_INVALID) ? NULL : ss_string_ptr);

} /* ssparser_extract_op */

/*===========================================================================

FUNCTION: ssparser_find_sc_info

DESCRIPTION:
  Locate the information table entry for this service code.Valid service 
  codes are defined in 22.030.

PARAMETERS:
  ss_string_ptr:   [Input] Pointer to the SS string
  ss_sc_info_ptr:  [Input] Pointer to receive the table entry address

DEPENDENCIES:
  None

RETURN VALUE:
  A pointer to the first character in the SS string following the
  operation field, of NULL on failure.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static const uint8 *ssparser_find_sc_info(
  const uint8                               *ss_string_ptr,
  const ssparser_sc_parse_table_entry_type **ss_sc_info_ptr)
{
  uint32 sc_temp  = 0;
  uint32 sc_index = 0;

  UIM_MSG_HIGH_0("ssparser_find_sc_info");

  if ((ss_sc_info_ptr == NULL) || (ss_string_ptr == NULL))
  {
    UIM_MSG_ERR_0("ss_string_ptr or ss_sc_info_ptr was NULL");
    return NULL;
  }

  /* Accumulate the service code digits */
  while ( ss_string_ptr && ssparser_isdigit(*ss_string_ptr) )
  {
    sc_temp = (sc_temp * 10) + (*ss_string_ptr & 0x0F);
    ++ss_string_ptr;
  }

  /* Find the table entry, if the service code is valid */
  *ss_sc_info_ptr = NULL;
  for (sc_index = 0; sc_index < SSPARSER_SC_PARSE_TABLE_ENTRIES; ++sc_index)
  {
    if (ssparser_sc_parse_table[sc_index].sc_raw == sc_temp)
    {
      *ss_sc_info_ptr = &ssparser_sc_parse_table[sc_index];
      break;
    }
  }

  return ((*ss_sc_info_ptr == NULL) ? NULL : ss_string_ptr);

} /* ssparser_find_sc_info */

/*===========================================================================

FUNCTION: ssparser_init

DESCRIPTION:
  Initialise the SS String parser.

PARAMETERS:
  ss_string_ptr:   [Input] Pointer to the SS string
  ss_sc_info_ptr:  [Input] Pointer to receive the table entry address

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void ssparser_init(void)
{
  UIM_MSG_HIGH_0("ssparser_init");
} /* ssparser_init */

/*===========================================================================

FUNCTION: ssparser_parse

DESCRIPTION:
  Parse the SS string given into the structure given.

PARAMETERS:
  ss_string_ptr:   [Input] Pointer to the SS string
  ss_length:       [Input] The SS string length, in characters
  ss_ton:          [Input] International or Domestic number
  ss_npi:          [Input] Number plan Indicator
  ss_info_ptr:     [Input] Pointer to a struct to receive the parsed output

DEPENDENCIES:
  None

RETURN VALUE:
  The output structure pointer if successful, otherwise NULL.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
ssparser_ss_string_info_type *ssparser_parse
(
  const uint8                  *ss_string_ptr,
  uint32                        ss_length,
  gstk_ton_enum_type            ss_ton,
  gstk_npi_enum_type            ss_npi,
  ssparser_ss_string_info_type *ss_info_ptr
)
{
  /* Start of next field in SS string */  
  const uint8                              *ss_next_ptr    = NULL;
  /* Pointer to service code info */
  const ssparser_sc_parse_table_entry_type *ss_sc_info_ptr = NULL; 
  /* Number of characters still to parse */     
  uint32                                    remaining      = ss_length; 

  UIM_MSG_HIGH_0("ssparser_parse");

  /* Check that we have somewhere to parse into */
  if (ss_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("No output structure given");
    return NULL;
  }
  memset(ss_info_ptr, 0, sizeof(*ss_info_ptr));

  /* Ensure that we have something that might be a valid SS string */
  if ((ss_string_ptr == NULL) || (ss_length < SSPARSER_SS_STRING_MIN_LENGTH))
  {
    UIM_MSG_ERR_0("Invalid SS string");
    return NULL;
  }

  /* Extract the operation field and verify it was OK */
  ss_info_ptr->op_ptr = ss_string_ptr;
  ss_next_ptr = ssparser_extract_op(ss_info_ptr->op_ptr, &ss_info_ptr->op);
  if (ss_next_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid operation field");
    return NULL;
  }
  ss_info_ptr->op_length = int32touint32(ss_next_ptr - ss_info_ptr->op_ptr);
  remaining -= ss_info_ptr->op_length;

  /* Extract the service code field and verify it was OK */
  ss_info_ptr->sc_ptr = ss_next_ptr;
  ss_next_ptr = ssparser_find_sc_info(ss_info_ptr->sc_ptr, &ss_sc_info_ptr);
  if (ss_next_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid service code field");
    return NULL;
  }
  ss_info_ptr->sc_length = int32touint32(ss_next_ptr - ss_info_ptr->sc_ptr);
  remaining -= ss_info_ptr->sc_length;

  ss_info_ptr->sc = ss_sc_info_ptr->sc_enum;
  ss_info_ptr->sia_type = ss_sc_info_ptr->sia_type;
  ss_info_ptr->sib_type = ss_sc_info_ptr->sib_type;
  ss_info_ptr->sic_type = ss_sc_info_ptr->sic_type;

  /* Sort out the TON */
  switch (ss_ton)
  {
    case GSTK_INTERNATIONAL_NUM:    ss_info_ptr->ton = SSPARSER_TON_INTERNATIONAL;
                                    break;
    case GSTK_NATIONAL_NUM:         ss_info_ptr->ton = SSPARSER_TON_NATIONAL;
                                    break;
    case GSTK_NETWORK_SPECIFIC_NUM: ss_info_ptr->ton = SSPARSER_TON_NW_SPECIFIC;
                                    break;
    case GSTK_UNKNOWN_TON:
    default:                        ss_info_ptr->ton = SSPARSER_TON_UNKNOWN;
                                    break;
  }

  /* Sort out the NPI */
  if (ss_npi == GSTK_ISDN_TELEPHONY_NPI)
    ss_info_ptr->npi = SSPARSER_NPI_ISDN;
  else
    ss_info_ptr->npi = SSPARSER_NPI_UNKNOWN;

  /* Extract 1st optional supplementary info (SIA) if present */
  SSPARSER_EXTRACT_SI_FIELD(remaining,
                            ss_next_ptr,
                            ss_info_ptr->sia_ptr,
                            ss_info_ptr->sia_length);

  /* Extract 2nd optional supplementary info (SIB) if present */
  SSPARSER_EXTRACT_SI_FIELD(remaining,
                            ss_next_ptr,
                            ss_info_ptr->sib_ptr,
                            ss_info_ptr->sib_length);

  /* Extract 3rd optional supplementary info (SIC) if present */
  SSPARSER_EXTRACT_SI_FIELD(remaining,
                            ss_next_ptr,
                            ss_info_ptr->sic_ptr,
                            ss_info_ptr->sic_length);

  #if defined(SSPARSER_DEBUG)
  ssparser_show_result(ss_string_ptr, ss_length, ss_info_ptr);
  #endif /* defined(SSPARSER_DEBUG) */

  return ss_info_ptr;

} /* ssparser_parse */

/*===========================================================================

FUNCTION: ssparser_parse_uint8

DESCRIPTION:
  Parse the uint8 decimal integer value given.

PARAMETERS:
  str_ptr:     [Input] Pointer to the uint8 string
  str_length:  [Input] The uint8 string length, in characters
  result_ptr:  [Input] Pointer to a uint8 to receive the value

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if parsing successful, otherwise FALSE.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean ssparser_parse_uint8
(
  const uint8 *str_ptr,
  uint32       str_length,
  uint8       *result_ptr
)
{
  uint8 result = 0;

  UIM_MSG_HIGH_3("ssparser_parse_uint8() str_ptr : 0x%x str_length : %d  result_ptr : %d",
                 str_ptr, str_length, result_ptr);

  if ( (str_ptr == NULL) || (str_length == 0) )
  {
    /*No uint8 string given*/
    return FALSE;
  }

  if (result_ptr == NULL)
  {
    /*uint8 value pointer missing*/
    return FALSE;
  }

  while ( (str_length > 0) && ssparser_isdigit(*str_ptr) )
  {
    result = (uint8)((result * 10) + (*str_ptr & 0x0F));
    ++str_ptr;
    --str_length;
  }

  *result_ptr = result;

  return TRUE;

} /* ssparser_parse_uint8 */

/*===========================================================================

FUNCTION: ssparser_parse_basic_service

DESCRIPTION:
  Parse the SS Basic Service given.

PARAMETERS:
  s_ptr:       [Input] Pointer to the basic service string
  bs_length:   [Input] The basic service string length, in characters
  bs_type_ptr: [Input] Pointer to a enum to receive the service type
  bs_code_ptr: [Input] Pointer to a uint8 to receive the service code

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if parsing successful, otherwise FALSE.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean ssparser_parse_basic_service
(
  const uint8                *bs_ptr,
  uint32                      bs_length,
  ssparser_bs_type_enum_type *bs_type_ptr,
  uint8                      *bs_code_ptr
)
{
  uint8                                         bs_code      = 0x00;
  uint32                                        bs_index     = 0x00;
  const ssparser_bscode_parse_table_entry_type *bs_table_ptr = NULL;

  UIM_MSG_HIGH_2("ssparser_parse_basic_service() bs_ptr : 0x%x bs_length : %d",
                 bs_ptr, bs_length);

  if ( (bs_ptr == NULL) || (bs_length != 2) )
  {
    /*No Basic Service string given*/
    return FALSE;
  }

  if ( (bs_type_ptr == NULL) || (bs_code_ptr == NULL) )
  {
    UIM_MSG_ERR_0("Basic Service type/code pointer missing");
    return FALSE;
  }

  while ( (bs_length > 0) && ssparser_isdigit(*bs_ptr) )
  {
    bs_code = (uint8)((bs_code * 10) + (*bs_ptr & 0x0F));
    ++bs_ptr;
    --bs_length;
  }

  for (bs_index = 0; bs_index < SSPARSER_BSCODE_PARSE_TABLE_ENTRIES; ++bs_index)
  {
    if (ssparser_bscode_parse_table[bs_index].bscode_raw == bs_code)
    {
      bs_table_ptr = &ssparser_bscode_parse_table[bs_index];
      break;
    }
  }

  if (bs_table_ptr == NULL)
  {
    UIM_MSG_ERR_1("Don't recognise Basic Service code 0x%x", bs_code);
    return FALSE;
  }

  *bs_code_ptr = bs_table_ptr->bscode_cooked;
  *bs_type_ptr = bs_table_ptr->bscode_type;

  return TRUE;

} /* ssparser_parse_basic_service */

/*===========================================================================

FUNCTION: ssparser_parse_bcd_telnum

DESCRIPTION:
  Parse the telephone number string into BCD form.

PARAMETERS:
  tel_str_ptr:        [Input] Pointer to the tel number string
  tel_str_length:     [Input] The tel number string length, in characters
  ton:                [Input] Internation or Domestic number
  npi:                [Input] Number plan indicator
  tel_bcd_ptr:        [Input] Pointer to a byte array to receive the BCD
  tel_bcd_size:       [Input] Size of the BCD buffer in bytes
  tel_bcd_length_ptr: [Input] Pointer to receive the BCD length

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if parsing successful, otherwise FALSE.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean ssparser_parse_bcd_telnum
(
  const uint8       *tel_str_ptr,
  uint32             tel_str_length,
  uint8              ton,
  uint8              npi,
  uint8             *tel_bcd_ptr,
  uint32             tel_bcd_size,
  uint8             *tel_bcd_length_ptr
)
{
  uint32 tel_bcd_length = 0x00;
  uint8  bcd_next       = 0x00;

  UIM_MSG_HIGH_2("ssparser_parse_bcd_telnum() tel_str_ptr : 0x%x tel_str_length: %d",
                 tel_str_ptr, tel_str_length);

  if ( (tel_str_ptr == NULL) || (tel_str_length == 0) )
  {
   /*No telephone number string given*/
    return FALSE;
  }

  if ( (tel_bcd_ptr == NULL) || (tel_bcd_size == 0) || (tel_bcd_length_ptr == NULL) )
  {
    UIM_MSG_ERR_0("No BCD telephone number buffer given");
    return FALSE;
  }

  if (tel_str_ptr[0] == '+')
  {
    UIM_MSG_HIGH_0("Telephone number is in international format");
    ton = SSPARSER_TON_INTERNATIONAL;
    ++tel_str_ptr;
    --tel_str_length;
  }

  tel_bcd_length = 1 + ((tel_str_length + 1) / 2);
  if (tel_bcd_length > tel_bcd_size)
  {
    UIM_MSG_ERR_0("BCD telephone number buffer too small");
    return FALSE;
  }

  *tel_bcd_length_ptr = (uint8) tel_bcd_length;
  *tel_bcd_ptr++ = 0x80 | ton | npi;

  while (tel_str_length != 0)
  {
    bcd_next = *tel_str_ptr & 0x0F;
    ++tel_str_ptr;
    --tel_str_length;

    if (tel_str_length == 0)
    {
      bcd_next |= 0xF0;
    }
    else
    {
      bcd_next |= ((*tel_str_ptr & 0x0F) << 4);
      ++tel_str_ptr;
      --tel_str_length;
    }

    *tel_bcd_ptr++ = bcd_next;
  }

  return TRUE;

} /* ssparser_parse_bcd_telnum */

#endif /* FEATURE_ESTK */

