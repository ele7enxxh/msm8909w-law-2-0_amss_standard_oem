#ifndef ESTK_SSPARSER_H
#define ESTK_SSPARSER_H

/*===========================================================================
FILE:  estk_ssparser.h

GENERAL DESCRIPTION:
  Interface to the Supplementary Service String parser used by ESTK

  Copyright (c) 2008, 2010 - 2011, 2013QUALCOMM Technologies, Inc(QTI). 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_ssparser.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   gs      Removed the ESTK featurization
08/06/13   vr      Centralized Call Control changes
06/20/13   vr      Triton TSTS changes
01/12/11   adp     Temp Hack till CM team fixes the code through CCB 
02/15/10   nb      Moved Dual Slot Changes
11/03/08   sk      Featurized ESTK based on external interface support
09/10/08   adg/sk  Send SS Fixes    
07/29/08   adg     Initial version
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_ESTK

#if !defined(TEST_FRAMEWORK)
#include "sys.h"
#include "sys_gw.h"
#else
#error code not present
#endif
#include "cm.h"
#include "estk.h"

/*===========================================================================
                        DEFINITIONS AND TYPES
===========================================================================*/

/* Enum to denote the operation field extracted from a SS string */
typedef estk_sups_ss_operation_enum_type  ssparser_op_enum_type;
#define SSPARSER_OP_ACTIVATE              ESTK_ACTIVATE_SS
#define SSPARSER_OP_DEACTIVATE            ESTK_DEACTIVATE_SS
#define SSPARSER_OP_INTERROGATE           ESTK_INTERROGATE_SS
#define SSPARSER_OP_REGISTER              ESTK_REGISTER_SS
#define SSPARSER_OP_ERASE                 ESTK_ERASE_SS
#define SSPARSER_OP_INVALID               ESTK_NULL_SS_OPERATION


/* Enum to denote the service code field extracted from a SS string */
/* Values match ss_operation_code_T in mn_cm_exp.h ,so can be used  */
/* directly in CM function calls.                                   */
typedef ss_operation_code_T ssparser_sc_enum_type;

#define SSPARSER_SC_CD                    cd
#define SSPARSER_SC_CLIP                  clip
#define SSPARSER_SC_CLIR                  clir
#define SSPARSER_SC_CoLP                  colp
#define SSPARSER_SC_CoLR                  colr
#define SSPARSER_SC_CFU                   cfu
#define SSPARSER_SC_CFB                   cfb
#define SSPARSER_SC_CFNRy                 cfnry
#define SSPARSER_SC_CFNRc                 cfnrc
#define SSPARSER_SC_CFAll                 allForwardingSS
#define SSPARSER_SC_CFAllCond             allCondForwardingSS
#define SSPARSER_SC_CW                    cw
#define SSPARSER_SC_BAOC                  baoc
#define SSPARSER_SC_BOIC                  boic
#define SSPARSER_SC_BOICexHC              boicExHC
#define SSPARSER_SC_BAIC                  baic
#define SSPARSER_SC_BICRoam               bicRoam
#define SSPARSER_SC_BarAll                allCallRestrictionSS
#define SSPARSER_SC_BarOut                barringOfOutgoingCalls
#define SSPARSER_SC_BarInc                barringOfIncomingCalls
#define SSPARSER_SC_ECT                   ect
#define SSPARSER_SC_CCBS                  ccbs
#define SSPARSER_SC_CNAP                  cnap
#define SSPARSER_SC_INVALID           0xFFFFFFFF


/*===========================================================================
  
  STRUCT: SSPARSER_SI_TYPE_ENUM_TYPE

  PARAMETERS: 
    SSPARSER_SI_NONE    : SI not used 
    SSPARSER_SI_TELNUM  : SI is a telephone number
    SSPARSER_SI_BS      : SI is a basic service code
    SSPARSER_SI_BYTE    : SI is a byte
    SSPARSER_SI_PWD     : SI is a password
  
  PURPOSE:
    Enum to denote the data types of SIA, SIB, and SIC .
===========================================================================*/
typedef enum
{
  SSPARSER_SI_NONE    = 0x00,
  SSPARSER_SI_TELNUM,
  SSPARSER_SI_BS,
  SSPARSER_SI_BYTE,
  SSPARSER_SI_PWD,
  SSPARSER_SI_MAX     = 0x7FFFFFFF
}
ssparser_si_type_enum_type;


/* Enum to denote a basic service type extracted from a SS string */
/* Values match ss_operation_code_T enum, so can be used directly */
/* in CM function calls.                                          */
typedef bsg_code_type                     ssparser_bs_type_enum_type;
#define SSPARSER_BST_TELE                 TS_CODE
#define SSPARSER_BST_BEARER               BS_CODE


/* Values to denote TON and NPI */
/* Values match values in header cm.h, so can be used directly in CM function calls. */
#define SSPARSER_TON_UNKNOWN              CM_TON_UNKNOWN
#define SSPARSER_TON_INTERNATIONAL        CM_TON_INTERNATIONAL
#define SSPARSER_TON_NATIONAL             CM_TON_NATIONAL
#define SSPARSER_TON_NW_SPECIFIC          CM_TON_NETWORK_SPECIFIC
#define SSPARSER_NPI_UNKNOWN              CM_NPI_UNKNOWN
#define SSPARSER_NPI_ISDN                 CM_NPI_ISDN


/*===========================================================================
  
  STRUCT: SSPARSER_SS_STRING_INFO_TYPE

  PARAMETERS: 
  op:          The operation field
  sc:          The service code
  ton;         Number type
  npi;         Number presentation
  op_ptr:      ointer to op within SS string
  op_length:   op length
  sc_ptr:      Pointer to sc within SS string
  sc_length:   sc length
  sia_type:    Data type of SIA
  sia_ptr:     Pointer to 1st supp info within SS string
  sia_length:  1st supp info length
  sib_type:    Data type of SIB
  sib_ptr:     Pointer to 2nd supp info within SS string
  sib_length:  2nd supp info length
  sic_type:    Data type of SIC
  sic_ptr:     Pointer to 3rd supp info within SS string
  sic_length:  3rd supp info length

  PURPOSE:
     Structure to denote a parsed SS string
==========================================================================*/
typedef struct
{
  ssparser_op_enum_type       op;
  ssparser_sc_enum_type       sc;

  uint8                       ton;
  uint8                       npi;

  const uint8                *op_ptr;
  uint32                      op_length;

  const uint8                *sc_ptr;
  uint32                      sc_length;

  ssparser_si_type_enum_type  sia_type;
  const uint8                *sia_ptr;
  uint32                      sia_length;

  ssparser_si_type_enum_type  sib_type;
  const uint8                *sib_ptr;
  uint32                      sib_length;

  ssparser_si_type_enum_type  sic_type;
  const uint8                *sic_ptr;
  uint32                      sic_length;
}
ssparser_ss_string_info_type;

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
extern void ssparser_init(void);

/*===========================================================================

FUNCTION: ssparser_parse

DESCRIPTION:
  Parse the SS string given into the structure given.

PARAMETERS:
  ss_string_ptr:   [Input] Pointer to the SS string
  ss_length:       [Input] The SS string length, in characters
  ss_sc_info_ptr:  [Input] Pointer to a struct to receive the parsed output

DEPENDENCIES:
  None

RETURN VALUE:
  The output structure pointer if successful, otherwise NULL.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
extern ssparser_ss_string_info_type *ssparser_parse
(
  const uint8                  *ss_string,
  uint32                        ss_length,
  gstk_ton_enum_type            ss_ton,
  gstk_npi_enum_type            ss_npi,
  ssparser_ss_string_info_type *ss_info_ptr
);

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
extern boolean ssparser_parse_uint8
(
  const uint8 *str_ptr,
  uint32       str_length,
  uint8       *result_ptr
);

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
extern boolean ssparser_parse_basic_service
(
  const uint8                *bs_ptr,
  uint32                      bs_length,
  ssparser_bs_type_enum_type *bs_type_ptr,
  uint8                      *bs_code_ptr
);

/*===========================================================================

FUNCTION: ssparser_parse_bcd_telnum

DESCRIPTION:
  Parse the telephone number string into BCD form.

PARAMETERS:
  tel_str_ptr:        [Input] Pointer to the tel number string
  tel_str_length:     [Input] The tel number string length, in characters
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
extern boolean ssparser_parse_bcd_telnum
(
  const uint8       *tel_str_ptr,
  uint32             tel_str_length,
  uint8              ton,
  uint8              npi,
  uint8             *tel_bcd_ptr,
  uint32             tel_bcd_size,
  uint8             *tel_bcd_length_ptr
);

#endif /* FEATURE_ESTK */
#endif /* ESTK_SSPARSER_H */

