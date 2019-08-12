/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          I S   8 0 1   T R A N S L A T I O N    S E R V I C E S

GENERAL DESCRIPTION
  This module contains the services which translate an IS 801 message between
  the internal format used by the Position Determination task and the 
  external format sent to the PDE.

EXTERNALIZED FUNCTIONS

  paix_xlate_int_msid_hdr
    Translates an MSID header from internal to external format.  

  paix_xlate_int_pddm_hdr
    Translates a PDDM header from internal to external format.
  
  paix_xlate_int_msi_resp                      
    Translates a MS Information response from internal to external format.
  
  paix_xlate_int_pr_part
    Translates a Pseudorange Measurments response part from internal to 
    external format.
  
  paix_xlate_int_ppm_part
    Translates a Pilot Phase Measurements response part from internal to 
    external format.

  paix_xlate_int_msto_resp
    Translates a Time Offset Measurement response from internal to
    external format.
  
  paix_xlate_int_cack_resp
    Translates a Cancellation Acknowledgement response from internal to
    external format.
  
  paix_xlate_int_rej_resp
    Translates a Reject response from internal to external format.
  
  paix_xlate_int_aa_req
    Translates an Acquisition Assistance request from internal to external
    format.
  
  paix_xlate_int_sa_req
    Translates a Sensitivity Assistance request from internal to external
    format.
  
  paix_xlate_int_lr_req
    Translates a Location Response request from internal to external format.
  
  paix_xlate_ext_pddm_hdr
    Translates a PDDM header from external to internal format.
  
  paix_xlate_ext_req_type
    Translates a Request header from external to internal format (to 
    determine what type of request it is).
  
  paix_xlate_ext_pr_req
    Translates a Pseudorange Measurements request from external to internal
    format.
  
  paix_xlate_ext_ppm_req
    Translates a Pilot Phase Measurements request from external to internal
    format.

  paix_xlate_ext_lr_req
    Translates a Location response request (PDE to MS) element from external
    to internal format.

  paix_xlate_ext_tom_req
    Translates a Time offset measurement request element from external to
    internal format.
  
  paix_xlate_ext_cancel_req
    Translates a Cancellation request from external to internal format.
  
  paix_xlate_ext_resp_type
    Translates a Response header from external to internal format (to 
    determine what type of response it is).
  
  paix_xlate_ext_aa_resp
    Translates an Acquisition Assistance response from external to internal 
    format.
  
  paix_xlate_ext_sa_resp
    Translates a Sensitivity Assistance response from external to internal
    format.
  
  paix_xlate_ext_lr_resp
    Translates a Locations Response response from external to internal format.
  
  paix_xlate_ext_rej_resp
    Translates a Reject response from external to internal format.
  

DESCRIPTION
  This function translates a Position Determination Data Message header from 
  internal to external format.

  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/src/paixlate.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/08   cl      Ignore unsupported responses to make it IS801-B compliant
11/15/06   ank     Changed prototype of paix_xlate_int_msid_hdr, paix_xlate_int_prdm_bsp_resp.
10/11/06   br      Removed feature GPSONE, and include pai.h and paii.h files.
10/14/05   rw      Initialize millisecond fields for MS-A fix in paix_xlate_ext_lr_resp()
05/24/05   cl      Make CLM a NV item
04/18/05   sk      Moved macro to paix.h
08/14/03   anr     Added feature to send Proprietary IS801 Base Lat/Long Msg.
06/04/03   ar      Added translation for SCU messages
08/15/02   ak      Fixed sv_code_ph_win and doppler_win translation
07/15/02   ak      Fixed bug in paix_xlate_int_aa_resp
07/18/02   ak      Removed changes made for the name of the union for IMSI type specific
                   subfields.
07/15/02   ak      Added function to translate a AA response 
                   record from internal to external format.
06/24/02   az      Changed the name of the union for IMSI type specific
                   subfields from class to class_type.
04/19/02   sk      Mainlined LR tx function and fixed a typo.
04/11/02   hxw     Mod's to allow LR to be send on R-Link.
03/29/02   ak      Mainlined FEATURE_IS801_ADD_PILOTS, FEATURE_IS801_BIT_CHECK
12/10/01    SK     Mod's for MSTO handling.
11/29/01   aar     Added a function to xlate MSTO resp msg from int to ext.
07/11/01   mai     Fix for 0 based num_add_pilots_p as opposed to num_pilots_p which is 1 based.
06/27/01   dd      Added support for translating GPS Almanac/Ephemeris requests and responses.
06/08/01   mai     Added code to support returning pilots in additional pilots field of PPM
                   response.
06/07/01   mai     Removed return of FALSE for REF_PN in AA response, just made it an error 
                   message, also now all fields are initialized in AA return structure, even
                   if PDE does not send them.
03/27/01   mai     Changed check from sess_start & sess_end to sess_start & sess_source, as
                   having start and end set is legal, but start and source is not, on msgs 
                   coming from PDE.
03/10/01   mai     Added a couple more checks under AA response.
03/05/01   mai     Added local feature FEATURE_IS801_BIT_CHECK, which checks incoming
                   IS801 data for bit-wise validity.
02/26/01   mai     Added FEATURE_GPS_MODE_IS801 wrapper around GPS parts.
01/31/01   mai     Updated to support IS801.1 changes.
12/06/00   mai     Enhancements for detecting bad data or packets.
10/24/00   mai     Fixed for non-zero based beginning value of height in IS801 
                   LR response message.
10/17/00   mai     Fixed decoding of various request types, and changed to support the
                   new common header type used inside of each response or request type.
09/14/00   mai     Added code to translate IMSI class 0, type 3 for TCP/IP custom 
                   header, also changed arrays used in AA decode to const static to
                   reduce RAM usage.
09/01/00   mai     Added decoding of DOPPLER_WIN and SV_CODE_PH_WIN in this 
                   module, fixed issue with sign_extend_b on ARM.
07/07/00   mai     Made fixes for signed IS801 values, added sign extend 
                   routines.
06/01/00   mai     Made fixes for zero based first element handling.
05/19/00   mai     Changed code to use new byte to bit and bit to byte macros.
05/12/00   mai     Fixed ppm response part creation routine.
05/11/00   mai     Cleaned up code.
05/03/00   mai     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "msg.h"
#include "qw.h"
#include "time_svc.h"
#include "tm_pai.h"
#include "tm_paii.h"
#include "paixlate.h"
#include "bit.h"
#include "tm_is801.h"

boolean paix_incl_msto = TRUE;


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define NUM_DOPPLER_WINS    5
  
static const uint16 doppler_win_table[NUM_DOPPLER_WINS] = 
      { 
          400, 200, 100, 50, 25 
      };
  
#define NUM_SV_CODE_PH_WINS 32
  
static const uint16 sv_code_ph_win_table[NUM_SV_CODE_PH_WINS] = 
      { 
          1023, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 22, 26, 30, 38, 46, 54, 
          62, 78, 94, 110, 126, 158, 190, 222, 254, 318, 382, 446, 512, 640 
      };

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                       MACROS                                            */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*
 * ADD_BYTES_NEEDED macro is used to guard against malformed PDDMs. A PDDM
 * field contains information about how long that PDDM is. To avoid reading
 * bytes that are not actually part of the PDDM, before unpacking each field
 * from a PDDM, we must make sure that the provided PDDM buffer is long enough
 * to read from. Since each Is801-1 information element inside a PDDM 
 * contains information about how much more bytes to read from the buffer, 
 * this must be done as the PDDM is being decoded.
 *
 * The number of *bits* required to unpack a field in a PDDM is tracked using
 * the header file tm_pai.h. This has a number of structures. Each structure
 * represents an Is801-1 information element (Ephemeris/Almanac/AA data etc).
 * Each field of the struct is a member of the IS801-1 information element
 * that corresponds to that struct. The number of bits may be obtained by
 * using         
 *                          FSIZ(struct, field)
 *
 * MACRO Parameters:
 * str, field: The structure and field name that is used to get the size of
 *             the unpacked fields incoming PDDM.
 * need_bits:  Number of bits needed to unpack the fields decoded so far.
 *             This is updated with the number of bits needed to unpack 
 *             the field indicated in the param 'field'
 * b_len    :  Available buffer length from which to read the incoming PDDM
 *
 * MACRO Sideeffects
 * 1) Updates need_bits parameter
 * 2) If the number of bits needed exceeds provided the buffer length,
 *    print a diag error msg and return.
 */
#define ADD_BYTES_NEEDED(need_bits, b_len, str, field)                     \
do{                                                                        \
  need_bits += FSIZ( str, field );                                         \
  if(b_len*8 < need_bits)                                                  \
  {                                                                        \
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",          \
                    b_len, 0, 0);                                          \
    return FALSE;                                                          \
  }                                                                        \
}while(0);


#define ADD_BYTES_NEEDED_2(need_bits, b_len, str, field1, field2)          \
do{                                                                        \
  need_bits += FSIZ( str, field1 );                                        \
  need_bits += FSIZ( str, field2 );                                        \
  if(b_len*8 < need_bits)                                                  \
  {                                                                        \
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",          \
                    b_len, 0, 0);                                          \
    return FALSE;                                                          \
  }                                                                        \
}while(0);

#define ADD_BYTES_NEEDED_3(need_bits, b_len, str, field1, field2, field3)  \
do{                                                                        \
  need_bits += FSIZ( str, field1 );                                        \
  need_bits += FSIZ( str, field2 );                                        \
  need_bits += FSIZ( str, field3 );                                        \
  if(b_len*8 < need_bits)                                                  \
  {                                                                        \
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",          \
                    b_len, 0, 0);                                          \
    return FALSE;                                                          \
  }                                                                        \
}while(0);

#define ADD_BYTES_NEEDED_4(need_bits, b_len, str, field1, field2, field3,  \
                           field4)                                         \
do{                                                                        \
  need_bits += FSIZ( str, field1 );                                        \
  need_bits += FSIZ( str, field2 );                                        \
  need_bits += FSIZ( str, field3 );                                        \
  need_bits += FSIZ( str, field4 );                                        \
  if(b_len*8 < need_bits)                                                  \
  {                                                                        \
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",          \
                    b_len, 0, 0);                                          \
    return FALSE;                                                          \
  }                                                                        \
}while(0);

#define ADD_BYTES_NEEDED_5(need_bits, b_len, str, field1, field2, field3,  \
                           field4, field5)                                 \
do{                                                                        \
  need_bits += FSIZ( str, field1 );                                        \
  need_bits += FSIZ( str, field2 );                                        \
  need_bits += FSIZ( str, field3 );                                        \
  need_bits += FSIZ( str, field4 );                                        \
  need_bits += FSIZ( str, field5 );                                        \
  if(b_len*8 < need_bits)                                                  \
  {                                                                        \
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",          \
                    b_len, 0, 0);                                          \
    return FALSE;                                                          \
  }                                                                        \
}while(0);

#define ADD_BYTES_NEEDED_8(need_bits, b_len, str, field1, field2, field3,  \
                           field4, field5, field6, field7, field8)         \
do{                                                                        \
  need_bits += FSIZ( str, field1 );                                        \
  need_bits += FSIZ( str, field2 );                                        \
  need_bits += FSIZ( str, field3 );                                        \
  need_bits += FSIZ( str, field4 );                                        \
  need_bits += FSIZ( str, field5 );                                        \
  need_bits += FSIZ( str, field6 );                                        \
  need_bits += FSIZ( str, field7 );                                        \
  need_bits += FSIZ( str, field8 );                                        \
  if(b_len*8 < need_bits)                                                  \
  {                                                                        \
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",          \
                    b_len, 0, 0);                                          \
    return FALSE;                                                          \
  }                                                                        \
}while(0);


/*===========================================================================

FUNCTION SIGN_EXTEND_B

DESCRIPTION
  This function extends the sign of a value whose length is less than that 
  of a byte so that the byte representation will be correctly signed.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void sign_extend_b
(
    byte*  val,
    word   len
)
{
    byte mask;

    mask = 0xFF;
    mask <<= (len - 1);
    mask = ~mask;

    if (*val & (0x1 << (len - 1)))
    {
      *val = - (((~(*val)) + 1) & mask);
    }
    
} /* sign_extend_b() */


/*===========================================================================

FUNCTION SIGN_EXTEND_W

DESCRIPTION
  This function extends the sign of a value whose length is less than that 
  of a word so that the dword representation will be correctly signed.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void sign_extend_w
(
    word*  val,
    word   len
)
{
    word mask;

    mask = 0xFFFF;
    mask <<= (len - 1);
    mask = ~mask;

    if (*val & (0x1 << (len - 1)))
    {
      *val = - (((~(*val)) + 1) & mask);
    }
} /* sign_extend_w() */


/*===========================================================================

FUNCTION SIGN_EXTEND_D

DESCRIPTION
  This function extends the sign of a value whose length is less than that 
  of a dword so that the dword representation will be correctly signed.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void sign_extend_d
(
    dword* val,
    word   len
)
{
    dword mask;

    mask = 0xFFFFFFFF;
    mask <<= (len - 1);
    mask = ~mask;

    if (*val & (0x1 << (len - 1)))
    {
      *val = - (((~(*val)) + 1) & mask); /*lint !e501 */  
    }
} /* sign_extend_d() */

  
/*===========================================================================

FUNCTION XLATE_EXT_PDDM_HDR

DESCRIPTION
  This function translates a PDDM message header from external to internal 
  format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_pddm_hdr
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  is801_pddm_hdr_type *int_ptr
    /* Pointer to place where translated message should be placed */
)
{
  boolean status;
    /* Status returned to calling procedure */
  word msg_pos = 0;
    /* Position of record in message */

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }

  int_ptr->sess_start = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, sess_start ));
  msg_pos += FSIZ( pai_pddm_hdr_type, sess_start );

  int_ptr->sess_end = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, sess_end ));
  msg_pos += FSIZ( pai_pddm_hdr_type, sess_end );

  int_ptr->sess_source = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, sess_source ));
  msg_pos += FSIZ( pai_pddm_hdr_type, sess_source );

  int_ptr->sess_tag = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, sess_tag ));
  msg_pos += FSIZ( pai_pddm_hdr_type, sess_tag );

  int_ptr->pd_msg_type = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, pd_msg_type ));
  msg_pos += FSIZ( pai_pddm_hdr_type, pd_msg_type );

  int_ptr->num_requests = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, num_requests ));
  msg_pos += FSIZ( pai_pddm_hdr_type, num_requests );

  int_ptr->num_responses = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pddm_hdr_type, num_responses ));
  msg_pos += FSIZ( pai_pddm_hdr_type, num_responses );


  if (int_ptr->sess_start && int_ptr->sess_source)
  {
    MSG_FATAL("SESS_START & SESS_SOURCE both set to 1",0,0,0);
    status = FALSE;
  }
  else if (int_ptr->sess_start && int_ptr->sess_end)
  {
    MSG_FATAL("SESS_START & SESS_END both set to 1",0,0,0);
    status = TRUE;  /* this case could be erroneous, but not necessarily */ /*lint !e506 */
  }
  else if (int_ptr->pd_msg_type != PD_MSG_TYPE)
  {
    MSG_FATAL("inv RECV'D PD_MSG_TYPE value",0,0,0);
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
    
  return (status);

} /* paix_xlate_ext_pddm_hdr() */


/*===========================================================================

FUNCTION XLATE_EXT_REQ_HEADER

DESCRIPTION
  This function translates a PDDM request element header from external to 
  internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_req_header
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  paii_gen_req_hdr *int_ptr,
    /* Pointer to place where translated message should be placed */
  uint16 length /* length of the buf_ptr */
)
{
  boolean status;
  word    msg_pos = 0;
  byte    reserved = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  if(length < REQ_HDR_LEN)
  {
    MSG_ERROR("Insufficient bytes to decode", 0, 0, 0);
    return FALSE;
  }

  reserved = 
    b_unpackb( buf_ptr, msg_pos, FSIZ(pai_req_hdr_type, reserved ));
  
  msg_pos += FSIZ( pai_req_hdr_type, reserved );

  if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("REQHDR RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos % 8)));
    return FALSE;
  }

  int_ptr->req_type = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_req_hdr_type, req_type ));
  msg_pos += FSIZ( pai_req_hdr_type, req_type );

  int_ptr->req_par_len = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_req_hdr_type, req_par_len));
  msg_pos += FSIZ( pai_req_hdr_type, req_par_len);

  if (int_ptr->req_type > MAX_PDE_TO_MS_REQ_CODE || 
      int_ptr->req_type == PDE_RESERVED_REQ_CODE)
  {
    MSG_FATAL("inv REQ_CODE value in REQ HDR",0,0,0);
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
  
  return (status);

} /* paix_xlate_ext_req_type() */

/*===========================================================================
  
  FUNCTION XLATE_EXT_PR_REQ
  
  DESCRIPTION
    This function translates a Pseudorange measurement request element from 
    external to internal format.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Errors that were encountered while translating the message.
  
  SIDE EFFECTS
    None
  
===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
static boolean paix_xlate_ext_pr_req
(
  byte *buf_ptr,
  is801_fwd_link_req_prm_type *int_ptr,
  uint16 length
)
{
  boolean status;
  word    msg_pos = 0;
  byte    reserved = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  /* Check for insufficient length in the buffer */
  if(length * 8 < PAI_PR_REQ_RPT_TYPE_SZ)
  {
     return FALSE;
  }


  int_ptr->pref_resp_qual =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pr_req_rpt_type, pref_resp_qual ));
  msg_pos += FSIZ( pai_pr_req_rpt_type, pref_resp_qual );
  
  int_ptr->num_fixes =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pr_req_rpt_type, num_fixes ));
  msg_pos += FSIZ( pai_pr_req_rpt_type, num_fixes );
  
  int_ptr->t_betw_fixes =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pr_req_rpt_type, t_betw_fixes ));
  msg_pos += FSIZ( pai_pr_req_rpt_type, t_betw_fixes );
  
  int_ptr->offset_req =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_pr_req_rpt_type, offset_req ));
  msg_pos += FSIZ( pai_pr_req_rpt_type, offset_req );
  
  reserved = 
    b_unpackb( buf_ptr, msg_pos, FSIZ(pai_pr_req_rpt_type, reserved ));
      
  if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("PR RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos % 8)));
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
    
  return (status);
  
} /* paix_xlate_ext_pr_req() */

/*===========================================================================

FUNCTION XLATE_EXT_PPM_REQ

DESCRIPTION
  This function translates a Pilot phase measurement request element from 
  external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
static boolean paix_xlate_ext_ppm_req
(
  byte   *buf_ptr,
  is801_fwd_link_req_ppm_type *int_ptr,
  uint16 length
)
{
  boolean status;
  word    msg_pos = 0;
  byte    reserved = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }

  /* Check for insufficient length in the buffer */
  if(length * 8 < PAI_PPM_REQ_RPT_TYPE_SZ)
  {
     return FALSE;
  }
  
  int_ptr->pref_resp_qual =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_ppm_req_rpt_type, pref_resp_qual ));
  msg_pos += FSIZ( pai_ppm_req_rpt_type, pref_resp_qual );

  int_ptr->num_fixes =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_ppm_req_rpt_type, num_fixes ));
  msg_pos += FSIZ( pai_ppm_req_rpt_type, num_fixes );

  int_ptr->t_betw_fixes =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_ppm_req_rpt_type, t_betw_fixes ));
  msg_pos += FSIZ( pai_ppm_req_rpt_type, t_betw_fixes );

  int_ptr->offset_req =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_ppm_req_rpt_type, offset_req ));
  msg_pos += FSIZ( pai_ppm_req_rpt_type, offset_req );

  int_ptr->desi_pil_ph_res =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_ppm_req_rpt_type, desi_pil_ph_res ));
  msg_pos += FSIZ( pai_ppm_req_rpt_type, desi_pil_ph_res );

  reserved = 
    b_unpackb( buf_ptr, msg_pos, FSIZ(pai_ppm_req_rpt_type, reserved ));
  
  if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("PPM RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos % 8)));
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
    
  return (status);

} /* paix_xlate_ext_ppm_req() */

/*===========================================================================

FUNCTION XLATE_EXT_LR_REQ

DESCRIPTION
  This function translates a Location response request (PDE to MS) element
  from external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
static boolean paix_xlate_ext_lr_req
(
  byte *buf_ptr,
  is801_fwd_link_req_lr_type *int_ptr,
  uint16 length
)
{
  boolean status = TRUE; /*lint !e506 */
  word    msg_pos = 0;
  byte    reserved = PAI_RESERVED_VAL;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }  

  /* Check for insufficient length in the buffer */
  if(length * 8 < PAI_LR_PDE_REQ_RPT_TYPE_SZ)
  {
     return FALSE;
  }
  
  int_ptr->pref_resp_qual =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, pref_resp_qual));
  msg_pos += FSIZ( pai_lr_pde_req_rpt_type, pref_resp_qual);

  int_ptr->num_fixes =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, num_fixes));
  msg_pos += FSIZ( pai_lr_pde_req_rpt_type, num_fixes);

  int_ptr->t_betw_fixes =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, t_betw_fixes));
  msg_pos += FSIZ( pai_lr_pde_req_rpt_type, t_betw_fixes);

  int_ptr->height_req =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, height_req));
  msg_pos += FSIZ( pai_lr_pde_req_rpt_type, height_req);

  int_ptr->clk_cor_gps_req =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, clk_cor_gps_req));
  msg_pos += FSIZ( pai_lr_pde_req_rpt_type, clk_cor_gps_req);

  int_ptr->velocity_req =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, velocity_req));
  msg_pos += FSIZ( pai_lr_pde_req_rpt_type, velocity_req);

  reserved =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_lr_pde_req_rpt_type, reserved));

  if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("LR_PDE_REQ RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos%8)));
    status = FALSE;
  }

  return (status);
} /* paix_xlate_ext_lr_req() */


/*===========================================================================

FUNCTION XLATE_EXT_TOM_REQ

DESCRIPTION
  This function translates a Time offset measurement request element from
  external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
static boolean paix_xlate_ext_tom_req
(
  byte *buf_ptr,
  is801_fwd_link_req_tom_type *int_ptr,
  uint16 length
)
{
  boolean status = TRUE; /*lint !e506 */
  word    msg_pos = 0;
  byte    reserved = PAI_RESERVED_VAL;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  /* Check for insufficient length in the buffer */
  if(length * 8 < PAI_TOM_REQ_RPT_TYPE_SZ)
  {
     return FALSE;
  }
  
  int_ptr->use_time =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_tom_req_rpt_type, use_time));
  msg_pos += FSIZ( pai_tom_req_rpt_type, use_time);

  if (int_ptr->use_time != 0x0)
  {
    int_ptr->action_time =
      b_unpackb( buf_ptr, msg_pos, FSIZ( pai_tom_req_rpt_type, action_time));
    msg_pos += FSIZ( pai_tom_req_rpt_type, action_time);
  }

  if ((msg_pos % 8)  != 0)
  {
    reserved =
      b_unpackb( buf_ptr, msg_pos, (8 - (msg_pos % 8)));
  }
  if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("TOM RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos%8)));
    status = FALSE;
  }

  return (status);

} /* paix_xlate_ext_tom_req() */

/*===========================================================================

FUNCTION XLATE_EXT_CANCEL_REQ

DESCRIPTION
  This function translates a Cancellation request element from external to 
  internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
static boolean paix_xlate_ext_cancel_req
(
  byte *buf_ptr,
  is801_fwd_link_req_cancel_type *int_ptr,
  uint16 length
)
{
  boolean status;
  word    msg_pos = 0;
  byte    reserved = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }

  /* Check for insufficient length in the buffer */
  if(length * 8 < PAI_CANCEL_REQ_RPT_TYPE_SZ)
  {
     return FALSE;
  }
  
  int_ptr->cancel_type =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_cancel_req_rpt_type, cancel_type ));
  msg_pos += FSIZ( pai_cancel_req_rpt_type, cancel_type );

  reserved = 
    b_unpackb( buf_ptr, msg_pos, FSIZ(pai_cancel_req_rpt_type, reserved ));
  
  if (int_ptr->cancel_type > MAX_PDE_TO_MS_REQ_CODE || 
      int_ptr->cancel_type == PDE_RESERVED_REQ_CODE )
  {
    MSG_FATAL("inv cancel_type value in CACK req",0,0,0);
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("CACK RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos % 8)));
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
    
  return (status);

} /* paix_xlate_ext_cancel_req() */


/*===========================================================================

FUNCTION XLATE_EXT_RESP_HEADER

DESCRIPTION
  This function translates a PDDM response element header from external to 
  internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_resp_header
(
  byte              *buf_ptr,
    /* Pointer to received message to be translated */
  paii_gen_resp_hdr *int_ptr,
    /* Pointer to place where translated message should be placed */
  uint16             length
)
{
  boolean status;
  word    msg_pos = 0;
  byte    reserved = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  if(length < RESP_HDR_LEN)
  {
    return FALSE;
  }

  reserved = 
    b_unpackb( buf_ptr, msg_pos, FSIZ(pai_resp_hdr_type, reserved ));
  
  msg_pos += FSIZ( pai_resp_hdr_type, reserved );

  if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("RESPHDR1 RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos % 8)));
    return FALSE;
  }
  
  int_ptr->unsol_resp =
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_resp_hdr_type, unsol_resp ));
  msg_pos += FSIZ( pai_resp_hdr_type, unsol_resp );
  
  int_ptr->resp_type = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_resp_hdr_type, resp_type ));
  msg_pos += FSIZ( pai_resp_hdr_type, resp_type );

  int_ptr->resp_par_len = 
    b_unpackb( buf_ptr, msg_pos, FSIZ( pai_resp_hdr_type, resp_par_len));
  msg_pos += FSIZ( pai_resp_hdr_type, resp_par_len);

  if (int_ptr->resp_type > MAX_PDE_TO_MS_RESP_CODE)
  {
    MSG_FATAL("RESPHDR2 RES=%d mp=%d, nb=%d", reserved, msg_pos, (8-(msg_pos % 8)));
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }

  return (status);

} /* paix_xlate_ext_resp_type() */


/*===========================================================================

FUNCTION XLATE_EXT_LR_RESP

DESCRIPTION
  This function translates a Location response response element from 
  external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_lr_resp
(
  byte               *buf_ptr,
  is801_lr_resp_type *int_ptr,
  byte               resp_par_length,
  uint16             buf_length  
)
{
  boolean status;
  word    offset = 0;
  word    reserved = 0;
  uint16  num_bits_needed = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  (void) time_get(int_ptr->timestamp);

  int_ptr->timestamp_msec = 0;
  int_ptr->msec_incl = 0;

  ADD_BYTES_NEEDED_8(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                         time_ref_cdma, 
                         latitude, 
                         longitude, 
                         loc_uncrtnty_ang, 
                         loc_uncrtnty_a, 
                         loc_uncrtnty_p, 
                         fix_type, 
                         velocity_incl);
  
  int_ptr->time_ref_cdma = 
    b_unpackw( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, time_ref_cdma ));
  offset += FSIZ( pai_lr_res_rpt_type, time_ref_cdma );

  int_ptr->latitude = 
    b_unpackd( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, latitude ));
  offset += FSIZ( pai_lr_res_rpt_type, latitude );

  sign_extend_d((dword*)&(int_ptr->latitude), 
                FSIZ( pai_lr_res_rpt_type, latitude));

  int_ptr->longitude = 
    b_unpackd( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, longitude ));
  offset += FSIZ( pai_lr_res_rpt_type, longitude );

  sign_extend_d((dword*)&(int_ptr->longitude), 
                FSIZ( pai_lr_res_rpt_type, longitude));

  int_ptr->loc_uncrtnty_ang = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_ang ));
  offset += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_ang );

  int_ptr->loc_uncrtnty_a = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_a ));
  offset += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_a );
  
  int_ptr->loc_uncrtnty_p = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_p ));
  offset += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_p );
  
  int_ptr->fix_type = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, fix_type ));
  offset += FSIZ( pai_lr_res_rpt_type, fix_type );
  
  int_ptr->velocity_incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, velocity_incl ));
  offset += FSIZ( pai_lr_res_rpt_type, velocity_incl );

  if (TRUE == int_ptr->velocity_incl) /*lint !e506 */
  {
    ADD_BYTES_NEEDED_2(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                       velocity_hor, 
                       heading);
          
    int_ptr->velocity_hor = 
      b_unpackw( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, velocity_hor ));
    offset += FSIZ( pai_lr_res_rpt_type, velocity_hor );

    int_ptr->heading = 
      b_unpackw( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, heading ));
    offset += FSIZ( pai_lr_res_rpt_type, heading );


    if (TRUE == int_ptr->fix_type) /*lint !e506 */
    {
      ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                       velocity_ver);
      int_ptr->velocity_ver = 
        b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, velocity_ver ));
      offset += FSIZ( pai_lr_res_rpt_type, velocity_ver );
  
      sign_extend_b((byte*)&(int_ptr->velocity_ver), 
                  FSIZ( pai_lr_res_rpt_type, velocity_ver));
    }      
  }
  
  ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                   clock_incl);

  int_ptr->clock_incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, clock_incl ));
  offset += FSIZ( pai_lr_res_rpt_type, clock_incl );

  if (TRUE == int_ptr->clock_incl) /*lint !e506 */
  {
    ADD_BYTES_NEEDED_2(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                       clock_bias, clock_drift);
         
    int_ptr->clock_bias = 
      b_unpackd( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, clock_bias )); 
    offset += FSIZ( pai_lr_res_rpt_type, clock_bias );

    sign_extend_d((dword*)&(int_ptr->clock_bias), 
                  FSIZ( pai_lr_res_rpt_type, clock_bias));

    int_ptr->clock_drift = 
      b_unpackw( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, clock_drift ));
    offset += FSIZ( pai_lr_res_rpt_type, clock_drift );

    sign_extend_w((word*)&(int_ptr->clock_drift), 
                  FSIZ( pai_lr_res_rpt_type, clock_drift));
  }

  ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                   height_incl);
  
  int_ptr->height_incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, height_incl ));
  offset += FSIZ( pai_lr_res_rpt_type, height_incl );

  if (TRUE == int_ptr->height_incl) /*lint !e506 */
  {
    ADD_BYTES_NEEDED_2(num_bits_needed, buf_length, pai_lr_res_rpt_type, 
                       height, loc_uncrtnty_v);
         
    int_ptr->height = 
      b_unpackw( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, height ));
    offset += FSIZ( pai_lr_res_rpt_type, height );

    sign_extend_w((word*)&(int_ptr->height), 
                  FSIZ( pai_lr_res_rpt_type, height));

    int_ptr->loc_uncrtnty_v = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_v ));
    offset += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_v );
  }
  
  if ( (offset % 8) != 0 ) 
  {
    reserved = 
      b_unpackb( buf_ptr, offset, (8 - (offset % 8)));
  }
    
  if ((offset + BYTE_PAD)/8 != resp_par_length) 
  {
    MSG_FATAL("decoded length mismatch in LR resp",0,0,0);
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("LR RES=%d mp=%d, nb=%d", reserved, offset, (8-(offset % 8)));
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
    
  return (status);

} /* paix_xlate_ext_lr_resp() */

/*===========================================================================
  
FUNCTION XLATE_EXT_AA_RESP
  
DESCRIPTION
  This function translates an Acquisition Assistance response element from 
  external to internal format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
static boolean paix_xlate_ext_aa_resp
(
  byte *buf_ptr,
  is801_aa_resp_type *int_ptr,
  byte               resp_par_length,
  uint16             buf_length
)
{
  int i;
  boolean status;
  word    offset = 0;
  word    reserved = 0;
  uint16  num_bits_needed = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  (void) time_get(int_ptr->timestamp);
  
  ADD_BYTES_NEEDED_3(num_bits_needed, buf_length, pai_aa_res_fix_type, num_sv, time_of_app, dopp_incl);

  int_ptr->fix.num_sv = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, num_sv )) + 1;  /* IS801 uses 0 based numbering */
  offset += FSIZ( pai_aa_res_fix_type, num_sv );
  
  int_ptr->fix.time_of_app = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, time_of_app ));
  offset += FSIZ( pai_aa_res_fix_type, time_of_app );
  
  int_ptr->fix.dopp_incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, dopp_incl ));
  offset += FSIZ( pai_aa_res_fix_type, dopp_incl );
  
  if (TRUE == int_ptr->fix.dopp_incl) /*lint !e506 */
  {
    ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_aa_res_fix_type, add_dopp_incl);

    int_ptr->fix.add_dopp_incl = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, add_dopp_incl ));
    offset += FSIZ( pai_aa_res_fix_type, add_dopp_incl );
  }
    
  ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_aa_res_fix_type, code_ph_par_incl);

  int_ptr->fix.code_ph_par_incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, code_ph_par_incl ));
  offset += FSIZ( pai_aa_res_fix_type, code_ph_par_incl );
  
  if (TRUE == int_ptr->fix.code_ph_par_incl) /*lint !e506 */
  {
    ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_aa_res_fix_type, reference_pn);

    int_ptr->fix.reference_pn = 
      b_unpackw( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, reference_pn ));
    offset += FSIZ( pai_aa_res_fix_type, reference_pn );

    if (int_ptr->fix.reference_pn > GPS_AA_MAX_REF_PN)
    {
      MSG_FATAL("inv REF_PN value in AA resp",0,0,0);
    }
  }
  
  ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_aa_res_fix_type, az_el_incl);

  int_ptr->fix.az_el_incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_fix_type, az_el_incl ));
  offset += FSIZ( pai_aa_res_fix_type, az_el_incl );
  
  for (i=0; i < int_ptr->fix.num_sv && i< MAX_NUM_SATS; i++) 
  {
    ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_aa_res_var_type, sv_prn_num);

    int_ptr->var[i].sv_prn_num = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, sv_prn_num )) + 1; /* IS801 uses 0 based numbering */
    offset += FSIZ( pai_aa_res_var_type, sv_prn_num );

    if (int_ptr->var[i].sv_prn_num > GPS_MAX_SV_PRN_VALUE)
    {
      MSG_FATAL("inv SV_PRN num in AA resp",0,0,0);
      return FALSE;
    }
    
    if (TRUE == int_ptr->fix.dopp_incl) /*lint !e506 */
    {
      ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_aa_res_var_type, doppler0);

      int_ptr->var[i].doppler0 = 
        b_unpackw( buf_ptr, offset, FSIZ( pai_aa_res_var_type, doppler0 ));
      offset += FSIZ( pai_aa_res_var_type, doppler0 );
  
      sign_extend_w((word*)&(int_ptr->var[i].doppler0), 
                    FSIZ( pai_aa_res_var_type, doppler0));
  
      if (TRUE == int_ptr->fix.add_dopp_incl) /*lint !e506 */
      {
        ADD_BYTES_NEEDED_2(num_bits_needed, buf_length, pai_aa_res_var_type, doppler1, doppler_win);
              
        int_ptr->var[i].doppler1 = 
          b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, doppler1 ));
        offset += FSIZ( pai_aa_res_var_type, doppler1 );
  
        sign_extend_b((byte*)&(int_ptr->var[i].doppler1), 
                      FSIZ( pai_aa_res_var_type, doppler1));
  
        int_ptr->var[i].doppler_win_0_5_hz = 
          b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, doppler_win ));
        offset += FSIZ( pai_aa_res_var_type, doppler_win );

        if (int_ptr->var[i].doppler_win_0_5_hz > GPS_AA_MAX_DOPPLER_WIN)
        {
          MSG_FATAL("inv DOPPLER_WIN value in AA resp",0,0,0);
          return FALSE;
        }
        else
        {
          int_ptr->var[i].doppler_win_0_5_hz = 
          doppler_win_table[ int_ptr->var[i].doppler_win_0_5_hz ];
        }
      }
    }
    else
    {
      int_ptr->var[i].doppler0 = GPS_AA_MAX_DOPP0_VAL;
      int_ptr->var[i].doppler1 = 0;
      int_ptr->var[i].doppler_win_0_5_hz = doppler_win_table[GPS_AA_MAX_DOPPLER_WIN];
    }
  
    if (TRUE == int_ptr->fix.code_ph_par_incl) /*lint !e506 */
    {
      ADD_BYTES_NEEDED_4(num_bits_needed, buf_length, pai_aa_res_var_type, 
                         sv_code_ph, 
                         sv_code_ph_int, 
                         gps_bit_num, 
                         sv_code_ph_win);

      int_ptr->var[i].sv_code_ph = 
        b_unpackw( buf_ptr, offset, FSIZ( pai_aa_res_var_type, sv_code_ph ));
      offset += FSIZ( pai_aa_res_var_type, sv_code_ph );

      if (int_ptr->var[i].sv_code_ph > GPS_AA_MAX_SV_CODE_PH)
      {
        MSG_FATAL("Inv sv_code_ph in AA resp",0,0,0);
        return FALSE;
      }
  
      int_ptr->var[i].sv_code_ph_int = 
          b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, sv_code_ph_int ));
      offset += FSIZ( pai_aa_res_var_type, sv_code_ph_int );

      if (int_ptr->var[i].sv_code_ph_int > GPS_AA_MAX_SV_CODE_PH_INT)
      {
        MSG_FATAL("Inv sv_code_ph_int in AA resp",0,0,0);
        return FALSE;
      }
      else
      {
        int_ptr->var[i].sv_code_ph_int = 
        GPS_AA_MAX_SV_CODE_PH_INT - int_ptr->var[i].sv_code_ph_int;
      }
  
      int_ptr->var[i].gps_bit_num = 
        b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, gps_bit_num ));
      offset += FSIZ( pai_aa_res_var_type, gps_bit_num );

    {
      uint16 sv_code_ph_win_i = b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, sv_code_ph_win )) ;
      if(sv_code_ph_win_i >= NUM_SV_CODE_PH_WINS)
      {
        MSG_FATAL("Inv sv_code_ph_win in AA resp",0,0,0);
        return FALSE;
      
      }
      
      int_ptr->var[i].sv_code_ph_win = sv_code_ph_win_table[sv_code_ph_win_i];
      offset += FSIZ( pai_aa_res_var_type, sv_code_ph_win );
     }
    }
    else
    {
      int_ptr->var[i].sv_code_ph = 0;
      int_ptr->var[i].sv_code_ph_int = 0;
      int_ptr->var[i].gps_bit_num = 0;
      int_ptr->var[i].sv_code_ph_win = sv_code_ph_win_table[NUM_SV_CODE_PH_WINS - 1]; //last element
    }
  
    if (TRUE == int_ptr->fix.az_el_incl) /*lint !e506 */
    {
      ADD_BYTES_NEEDED_2(num_bits_needed, buf_length, pai_aa_res_var_type, 
                         azimuth, 
                         elevation);

      int_ptr->var[i].azimuth = 
        b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, azimuth ));
      offset += FSIZ( pai_aa_res_var_type, azimuth );
  
      int_ptr->var[i].elevation = 
        b_unpackb( buf_ptr, offset, FSIZ( pai_aa_res_var_type, elevation ));
      offset += FSIZ( pai_aa_res_var_type, elevation );
    }
    else
    {
      int_ptr->var[i].azimuth = 0;
      int_ptr->var[i].elevation = 0;
    }
  }                            

  if ( (offset % 8) != 0 ) 
  {
    reserved = b_unpackb( buf_ptr, offset, (8 - (offset % 8)));
  }
  
  if ((offset + BYTE_PAD)/8 != resp_par_length) 
  {
    MSG_FATAL("decoded length mismatch in AA resp",0,0,0);
  
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("AA RES=%d mp=%d, nb=%d", reserved, offset, (8-(offset % 8)));
  
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
  
  return (status);
  
} /* paix_xlate_ext_aa_resp() */
  
  
/*===========================================================================
  
FUNCTION XLATE_EXT_SA_RESP
  
DESCRIPTION
  This function translates a Sensitivity Assistance response element from 
  external to internal format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_sa_resp
(
  byte               *buf_ptr,
  is801_sa_part_type *int_ptr,
  byte               resp_par_length,
  uint16             buf_length
)
{
  int i;
  int j;
  byte num_dr_p;
  boolean status;
  word    offset = 0;
  word    temp_pos;
  word    reserved = 0;
  uint16  num_bits_needed = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  (void) time_get(int_ptr->timestamp);

  ADD_BYTES_NEEDED_5(num_bits_needed, buf_length, pai_sa_res_fix_type, 
                         ref_bit_num, 
                         num_dr_p, 
                         dr_size, 
                         part_num, 
                         total_parts);
  
  int_ptr->fix.ref_bit_num = 
    b_unpackw( buf_ptr, offset, FSIZ( pai_sa_res_fix_type, ref_bit_num ));
  offset += FSIZ( pai_sa_res_fix_type, ref_bit_num );

  if (int_ptr->fix.ref_bit_num > GPS_SA_MAX_REF_BIT_VALUE)
  {
    MSG_FATAL("inv REF_BIT value in SA resp",0,0,0);
  
    return FALSE;
  }
  
  int_ptr->fix.num_dr_p = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_sa_res_fix_type, num_dr_p )) + 1;  /* IS801 uses 0 based numbering */
  offset += FSIZ( pai_sa_res_fix_type, num_dr_p );
  MSG_MED("num_dr_p=%d",int_ptr->fix.num_dr_p,0,0);
  
  int_ptr->fix.dr_size = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_sa_res_fix_type, dr_size ));
  offset += FSIZ( pai_sa_res_fix_type, dr_size );
  
  int_ptr->fix.part_num = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_sa_res_fix_type, part_num )) + 1; /* IS801 uses 0 based numbering */
  offset += FSIZ( pai_sa_res_fix_type, part_num );  
  
  int_ptr->fix.total_parts = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_sa_res_fix_type, total_parts )) + 1;  /* IS801 uses 0 based numbering */
  offset += FSIZ( pai_sa_res_fix_type, total_parts );

  if (int_ptr->fix.part_num > int_ptr->fix.total_parts)
  {
    MSG_FATAL("part num > total num parts SA resp",0,0,0);
  
    return FALSE;
  }
  
  if (int_ptr->fix.num_dr_p > MAX_DATA_RECORDS)
  {
    num_dr_p = MAX_DATA_RECORDS;
  } 
  else
  {
    num_dr_p = int_ptr->fix.num_dr_p;
  }

  if(MAX_NAV_BITS_IN_BYTES < DR_SIZE_IN_BYTES(int_ptr->fix.dr_size))
  {
    MSG_ERROR("Insufficient buffer %d to decode all SA bits %d",MAX_NAV_BITS_IN_BYTES, int_ptr->fix.dr_size, 0);
  }
  /* Iterate over the raw data, to obtain the number of data records (SA
   * data) indicated in the fixed part of SA Resp PDDM */
  for (i = 0; i < num_dr_p; i++) 
  {
    temp_pos = offset;
    num_bits_needed += DR_SIZE_IN_BYTES(int_ptr->fix.dr_size) * 8;
    if(buf_length * 8 < num_bits_needed)
    {
      MSG_ERROR("Insufficient number of bytes %d. need %d Malformed PDDM", 
                   buf_length, num_bits_needed/8, 0);
      return FALSE;
    }

    for (j = 0; j < DR_SIZE_IN_BYTES(int_ptr->fix.dr_size); j++ ) 
    {
      int_ptr->var[i].nav_msg_bits[j] = 
        b_unpackb( buf_ptr, temp_pos, NUM_BITS_IN_BYTE(byte));
      temp_pos += NUM_BITS_IN_BYTE(byte);
    }

    offset += int_ptr->fix.dr_size*DR_SIZE_MULTIPLIER;

    ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_sa_res_var2_type, num_sv_dr);
    
    int_ptr->var[i].num_sv_dr =                 /* IS801 uses 0 based numbering */
      b_unpackb( buf_ptr, offset, FSIZ( pai_sa_res_var2_type, num_sv_dr )) + 1; 
    offset += FSIZ( pai_sa_res_var2_type, num_sv_dr );
  
    for (j = 0; j < int_ptr->var[i].num_sv_dr; j++) 
    {
      ADD_BYTES_NEEDED(num_bits_needed, buf_length, pai_sa_res_var3_type, sv_prn_num);

      int_ptr->var[i].sv_prn_num[j] =         /* IS801 uses 0 based numbering */
        b_unpackb( buf_ptr, offset, FSIZ( pai_sa_res_var3_type, sv_prn_num )) + 1;
      offset += FSIZ( pai_sa_res_var3_type, sv_prn_num );

      if (int_ptr->var[i].sv_prn_num[j] > GPS_MAX_SV_PRN_VALUE)
      {
        MSG_FATAL("inv SV_PRN value in SA resp",0,0,0);
        return  FALSE;
      }
    }
  }

  if ( (offset % 8) != 0 ) 
  {
    reserved = 
      b_unpackb( buf_ptr, offset, (8 - (offset % 8)));
  }
  
  if (((offset) + BYTE_PAD)/8 != resp_par_length) 
  {
    MSG_FATAL("decoded length mismatch in SA resp",0,0,0);
    MSG_FATAL("mp=%d, rpl=%d", offset, resp_par_length, 0);
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("SA RES=%d mp=%d, nb=%d", reserved, offset, (8-(offset % 8)));
    status = FALSE;
  } 
  else
  {
    status = TRUE; /*lint !e506 */
  }
  return (status);
} /* paix_xlate_ext_sa_resp() */

/*===========================================================================
  
FUNCTION XLATE_EXT_ALM_RESP
  
DESCRIPTION
  This function translates a GPS Almanace response element from 
  external to internal format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_alm_resp
(
  byte                *buf_ptr,
  is801_alm_part_type *int_ptr,
  byte                resp_par_length,
  uint16              buf_length  
)
{
  int i;
  uint8 u_TotalSats;
  boolean status;
  word    offset = 0;
  word    reserved = 0;
  uint16  num_bits_needed = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  (void) time_get(int_ptr->timestamp);

  ADD_BYTES_NEEDED_5(num_bits_needed, buf_length, pai_alm_res_fix_type, 
                         num_sv_p, 
                         week_num, 
                         toa, 
                         part_num, 
                         total_parts);
  
  int_ptr->fix.u_TotalSats = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_alm_res_fix_type, num_sv_p )) + 1;	/* Number of SV's -1, 1 to 64 */
  offset += FSIZ( pai_alm_res_fix_type, num_sv_p );
  
  int_ptr->fix.u_WeekNum = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_alm_res_fix_type, week_num )) ;  /* Almanac Week Number, 0 to 255 */
  offset += FSIZ( pai_alm_res_fix_type, week_num );
 
  int_ptr->fix.u_Toa = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_alm_res_fix_type, toa )); /* Ref. Time of Almanac, 0 to 147 */
  offset += FSIZ( pai_alm_res_fix_type, toa );
  
  int_ptr->fix.u_Part_Num = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_alm_res_fix_type, part_num )) + 1; /* IS801 uses 0 based numbering */
  offset += FSIZ( pai_alm_res_fix_type, part_num );  
  
  int_ptr->fix.u_Total_Parts = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_alm_res_fix_type, total_parts )) + 1;  /* IS801 uses 0 based numbering */
  offset += FSIZ( pai_alm_res_fix_type, total_parts );

  if (int_ptr->fix.u_Part_Num > int_ptr->fix.u_Total_Parts)
  {
    MSG_FATAL("Almanac response part num > total parts",0,0,0); 
    return FALSE;
  }

  if (int_ptr->fix.u_TotalSats > MAX_NUM_ALM_SATS)
  {
    u_TotalSats = MAX_NUM_ALM_SATS;
  }  
  else
  {
    u_TotalSats = int_ptr->fix.u_TotalSats;
  }

  num_bits_needed += PAI_ALM_RES_VAR_TYPE_SZ * u_TotalSats;

  if(buf_length*8 < num_bits_needed)                              
  {                                                                      
    MSG_ERROR("Malformed packet. Insufficent bytes %d to decode",         
                 buf_length, 0, 0);                                        
    return FALSE;                                                       
  }                                                                    
  
  for (i = 0; i < u_TotalSats; i++) 
  {
          
    int_ptr->var[i].u_Sv_Prn_Num =  
         b_unpackb( buf_ptr, offset, FSIZ( pai_alm_res_var_type, sv_prn_num )) + 1;  /* 1 to 32 */
    offset += FSIZ( pai_alm_res_var_type, sv_prn_num );
  
    int_ptr->var[i].w_DeltaI =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_alm_res_var_type, delta_inclination )); 
    offset += FSIZ( pai_alm_res_var_type, delta_inclination );
       
    int_ptr->var[i].q_MZero =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_alm_res_var_type, m0 )); 
    offset += FSIZ( pai_alm_res_var_type, m0 );
  
    int_ptr->var[i].w_E =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_alm_res_var_type, eccentricity )); 
    offset += FSIZ( pai_alm_res_var_type, eccentricity );

    int_ptr->var[i].q_SqrtA =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_alm_res_var_type, a_sqrt )); 
    offset += FSIZ( pai_alm_res_var_type, a_sqrt );

    int_ptr->var[i].q_OmegaZero =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_alm_res_var_type, omega_0 )); 
    offset += FSIZ( pai_alm_res_var_type, omega_0 );

    int_ptr->var[i].q_Omega =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_alm_res_var_type, omega )); 
    offset += FSIZ( pai_alm_res_var_type, omega );

    int_ptr->var[i].w_OmegaDot =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_alm_res_var_type, omegadot )); 
    offset += FSIZ( pai_alm_res_var_type, omegadot );

    int_ptr->var[i].w_Af1 =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_alm_res_var_type, af1 )); 
    offset += FSIZ( pai_alm_res_var_type, af1 );

    int_ptr->var[i].w_Af0 =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_alm_res_var_type, af0 )); 
    offset += FSIZ( pai_alm_res_var_type, af0 );
  }

  if ( (offset % 8) != 0 ) 
  {
    reserved = b_unpackb( buf_ptr, offset, (8 - (offset % 8)));
  }
  
  if ((offset + BYTE_PAD)/8 != resp_par_length) 
  {
    MSG_FATAL("decoded length mismatch in Almanac resp",0,0,0);
    MSG_FATAL("offset = %d, rpl=%d", offset, resp_par_length, 0);
  
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("Almanac RES=%d offset=%d, nb=%d", reserved, offset, (8-(offset % 8)));
  
    status = FALSE;
  } 
  else
  {
    status = TRUE; /*lint !e506 */
  }      

  return (status);
} /* paix_xlate_ext_alm_resp() */


/*===========================================================================
  
  FUNCTION XLATE_EXT_EPH_RESP
  
  DESCRIPTION
    This function translates a GPS Ephemeris response element from 
    external to internal format.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Errors that were encountered while translating the message.
  
  SIDE EFFECTS
    None
  
===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_eph_resp
(
  byte                *buf_ptr,
  is801_eph_part_type *int_ptr,
  byte                resp_par_length,
  uint16              buf_length
)
{
  int i;
  uint8 u_TotalSats;
  boolean status;
  word    offset = 0;
  word    reserved = 0;
  uint16  num_bits_needed = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }
  
  (void) time_get(int_ptr->timestamp);
  
  ADD_BYTES_NEEDED_4(num_bits_needed, buf_length, pai_eph_res_fix_type, 
                     num_sv_p, 
                     part_num, 
                     total_parts, 
                     ab_par_incl);
  
  /* Num SV's in this part, 1 to 16, 0 based */
  int_ptr->fix.u_TotalSats = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, num_sv_p )) + 1;
  offset += FSIZ( pai_eph_res_fix_type, num_sv_p );

  /* Part Number, 1 to Total Parts, 0 based */
  int_ptr->fix.u_Part_Num = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, part_num )) + 1;
  offset += FSIZ( pai_eph_res_fix_type, part_num ); 

  /* Total Parts, 1 to 32, 0 based */
  int_ptr->fix.u_Total_Parts = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, total_parts )) + 1;  
  offset += FSIZ( pai_eph_res_fix_type, total_parts );
  
  /* Alpha/Beta Parameters included, 0 or 1 */
  int_ptr->fix.u_AB_Par_Incl = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, ab_par_incl )) ; 
  offset += FSIZ( pai_eph_res_fix_type, ab_par_incl );

  if (TRUE == int_ptr->fix.u_AB_Par_Incl) /*lint !e506 */
  {
    ADD_BYTES_NEEDED_4(num_bits_needed, buf_length, pai_eph_res_fix_type, 
                       alpha_0, alpha_1, alpha_2, alpha_3); 
          
    int_ptr->fix.u_Alpha_0 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, alpha_0 ));
    offset += FSIZ( pai_eph_res_fix_type, alpha_0 );
 
    int_ptr->fix.u_Alpha_1 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, alpha_1 ));
    offset += FSIZ( pai_eph_res_fix_type, alpha_1 );
 
    int_ptr->fix.u_Alpha_2 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, alpha_2 ));
    offset += FSIZ( pai_eph_res_fix_type, alpha_2 );
 
    int_ptr->fix.u_Alpha_3 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, alpha_3 ));
    offset += FSIZ( pai_eph_res_fix_type, alpha_3 );
 
    ADD_BYTES_NEEDED_4(num_bits_needed, buf_length, pai_eph_res_fix_type, 
                       beta_0, beta_1, beta_2, beta_3); 

    int_ptr->fix.u_Beta_0 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, beta_0 ));
    offset += FSIZ( pai_eph_res_fix_type, beta_0 );
 
    int_ptr->fix.u_Beta_1 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, beta_1 ));
    offset += FSIZ( pai_eph_res_fix_type, beta_1 );
  
    int_ptr->fix.u_Beta_2 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, beta_2 ));
    offset += FSIZ( pai_eph_res_fix_type, beta_2 );
  
    int_ptr->fix.u_Beta_3 = 
      b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_fix_type, beta_3 ));
    offset += FSIZ( pai_eph_res_fix_type, beta_3 );
  }

  if (int_ptr->fix.u_Part_Num > int_ptr->fix.u_Total_Parts)
  {
    MSG_FATAL("total parts > part num GPS Ephemeris response",0,0,0);
    return FALSE;
  }
  
  if (int_ptr->fix.u_TotalSats > MAX_NUM_EPH_SATS)
  {
    u_TotalSats = MAX_NUM_EPH_SATS;
  }
  else
  {
    u_TotalSats = int_ptr->fix.u_TotalSats;
  }

  /* For each SV, a fixed number of bits is needed to encode the 
   * ephemeris. If provided buffer is less than this length, bomb out */
  num_bits_needed += u_TotalSats * PAI_EPH_RES_VAR_TYPE_SZ;

  if(buf_length * 8 < num_bits_needed)
  {
    MSG_ERROR("Insufficient number of bytes %d. need %d Malformed PDDM", 
                 buf_length, num_bits_needed/8, 0);
    return FALSE;
  }

  for (i = 0; i < u_TotalSats; i++) 
  {

    /* Satellite PRN number, 1 to 32, 0 based. */
    int_ptr->var[i].u_Sv_Prn_Num =  
         b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_var_type, sv_prn_num )) + 1; 
    offset += FSIZ( pai_eph_res_var_type, sv_prn_num );

    int_ptr->var[i].u_Iode =  
         b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_var_type, iode )); 
    offset += FSIZ( pai_eph_res_var_type, iode );

    int_ptr->var[i].x_C_Rs =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, c_rs )); 
    offset += FSIZ( pai_eph_res_var_type, c_rs );

    int_ptr->var[i].x_Delta_N =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, delta_n )); 
    offset += FSIZ( pai_eph_res_var_type, delta_n );
       
    int_ptr->var[i].l_M0 =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, m0 )); 
    offset += FSIZ( pai_eph_res_var_type, m0 );

    int_ptr->var[i].x_C_Uc =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, c_uc )); 
    offset += FSIZ( pai_eph_res_var_type, c_uc );
  
    int_ptr->var[i].l_Eccentricity =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, eccentricity )); 
    offset += FSIZ( pai_eph_res_var_type, eccentricity );

    int_ptr->var[i].x_C_Us =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, c_us )); 
    offset += FSIZ( pai_eph_res_var_type, c_us );
  
    int_ptr->var[i].l_A_Sqrt =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, a_sqrt )); 
    offset += FSIZ( pai_eph_res_var_type, a_sqrt );
  
    int_ptr->var[i].x_Toe =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, toe )); 
    offset += FSIZ( pai_eph_res_var_type, toe );

    int_ptr->var[i].x_C_Ic =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, c_ic )); 
    offset += FSIZ( pai_eph_res_var_type, c_ic );

    int_ptr->var[i].l_Omega_0 =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, omega_0 )); 
    offset += FSIZ( pai_eph_res_var_type, omega_0 );

    int_ptr->var[i].x_C_Is =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, c_is )); 
    offset += FSIZ( pai_eph_res_var_type, c_is );

    int_ptr->var[i].l_Angle_Inclination =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, angle_inclination )); 
    offset += FSIZ( pai_eph_res_var_type, angle_inclination );

    int_ptr->var[i].x_C_Rc =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, c_rc )); 
    offset += FSIZ( pai_eph_res_var_type, c_rc );

    int_ptr->var[i].l_Omega =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, omega )); 
    offset += FSIZ( pai_eph_res_var_type, omega );

    int_ptr->var[i].l_Omegadot =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, omegadot )); 
    offset += FSIZ( pai_eph_res_var_type, omegadot );

    int_ptr->var[i].x_Idot =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, idot )); 
    offset += FSIZ( pai_eph_res_var_type, idot );

    int_ptr->var[i].x_Toc =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, toc )); 
    offset += FSIZ( pai_eph_res_var_type, toc );

    int_ptr->var[i].b_Af2 =  
         b_unpackb( buf_ptr, offset, FSIZ( pai_eph_res_var_type, af2 )); 
    offset += FSIZ( pai_eph_res_var_type, af2 );

    int_ptr->var[i].b_Af1 =  
         b_unpackw( buf_ptr, offset, FSIZ( pai_eph_res_var_type, af1 )); 
    offset += FSIZ( pai_eph_res_var_type, af1 );

    int_ptr->var[i].l_Af0 =  
         b_unpackd( buf_ptr, offset, FSIZ( pai_eph_res_var_type, af0 )); 
    offset += FSIZ( pai_eph_res_var_type, af0 );   
  }

  if ( (offset % 8) != 0 ) 
  {
    reserved = b_unpackb( buf_ptr, offset, (8 - (offset % 8)));
  }
  
  if ((offset + BYTE_PAD)/8 != resp_par_length) 
  {
    MSG_FATAL("decoded length mismatch in Ephemeris resp",0,0,0);
    MSG_FATAL("offset=%d,rpl=%d", offset,resp_par_length, 0);
  
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
      MSG_FATAL("Almanac RES=%d offset=%d, nb=%d", reserved, offset, (8-(offset % 8)));
  
      status = FALSE;
  } 
  else
  {
    status = TRUE; /*lint !e506 */
  }    

  return (status);
  
} /* paix_xlate_ext_eph_resp() */


/*===========================================================================

FUNCTION XLATE_EXT_REJ_RESP

DESCRIPTION
  This function translates a Reject response element from external to 
  internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
boolean paix_xlate_ext_rej_resp
(
  byte                *buf_ptr,
  is801_rej_resp_type *int_ptr,
  byte                resp_par_length,
  uint16              buf_length
)
{
  boolean status;
  word    offset = 0;
  word    reserved = 0;
  uint16  num_bits_needed = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }  

  if (resp_par_length != NUM_BYTES_IN_BIT(REJ_RESP_LEN))
  {
    MSG_FATAL("inv resp_par_len in REJ resp",0,0,0);
    return FALSE;
  }
  num_bits_needed += NUM_BYTES_IN_BIT(REJ_RESP_LEN);

  ADD_BYTES_NEEDED_3(num_bits_needed, buf_length, pai_rej_res_rpt_type, 
                     rej_req_type, 
                     rej_reason, reserved2);

  int_ptr->rej_req_type = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_rej_res_rpt_type, rej_req_type ));
  offset += FSIZ( pai_rej_res_rpt_type, rej_req_type );

  if (int_ptr->rej_req_type > MAX_MS_TO_PDE_REQ_CODE || 
      int_ptr->rej_req_type == MS_RESERVED_REQ_CODE ||
      int_ptr->rej_req_type == MS_RESERVED_REQ_CODE_2 )
  {
    MSG_FATAL("inv REQ_CODE in REJ resp",0,0,0);
    return FALSE;
  }

  int_ptr->rej_reason = 
    b_unpackb( buf_ptr, offset, FSIZ( pai_rej_res_rpt_type, rej_reason ));
  offset += FSIZ( pai_rej_res_rpt_type, rej_reason );

  reserved = 
    b_unpackb( buf_ptr, offset, FSIZ(pai_rej_res_rpt_type, reserved2 ));
  
  if (int_ptr->rej_reason > MAX_REJ_REASON_VALUE)
  {
    MSG_FATAL("inv REJ_REASON in REJ resp",0,0,0);
    status = FALSE;
  }
  else if (reserved != PAI_RESERVED_VAL)
  {
    MSG_FATAL("REJ RES=%d offset=%d, nb=%d", reserved, offset, (8-(offset % 8)));
    status = FALSE;
  }
  else
  {
    status = TRUE; /*lint !e506 */
  }
    
  return (status);

} /* paix_xlate_ext_rej_resp() */


/*===========================================================================

FUNCTION PAIX_XLATE_EXT_REQ_PAYLOAD

DESCRIPTION
  This function translates external PDDM request payload to internal format

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_ext_req_payload
(
  byte                      *start,
  is801_fwd_link_req_s_type *req_msg_ptr,
  uint16                     length /* length of bytes in buf_ptr */
)
{
  if((start == NULL) || (req_msg_ptr == NULL))
  {
    MSG_ERROR("Input pointer is NULL", 0, 0, 0);
    return FALSE;
  }

  switch(req_msg_ptr->request_type)
  {
    case IS801_FWD_LINK_REQ_MASK_MSI:
      /* MS INFO request contains no additional fields, so nothing to decode */
      MSG_MED( "MS Info Request.", 0, 0, 0);
      break;

    case IS801_FWD_LINK_REQ_MASK_AMWF:
      /* AMWF request contains no additional fields, so nothing to decode */
      MSG_MED( "AMWF Request.", 0, 0, 0);
      break;

    case IS801_FWD_LINK_REQ_MASK_PRM:
      MSG_MED( "PR Request.", 0, 0, 0);
      if(paix_xlate_ext_pr_req(start, &(req_msg_ptr->req_msg_payload.prm_req), length) != TRUE) /*lint !e506 */
      {
        return FALSE;
      }
      break;
      
    case IS801_FWD_LINK_REQ_MASK_PPM:
      MSG_MED( "PPM Request.", 0, 0, 0);
      if(paix_xlate_ext_ppm_req(start, &(req_msg_ptr->req_msg_payload.ppm_req), length) != TRUE) /*lint !e506 */
      {
        return FALSE;
      }
      break;

    case IS801_FWD_LINK_REQ_MASK_CAN:
      MSG_MED( "CACK Request.", 0, 0, 0);
      if(paix_xlate_ext_cancel_req(start, &(req_msg_ptr->req_msg_payload.cancel_req), length) != TRUE) /*lint !e506 */
      {
        return FALSE;
      }
      break;

    case IS801_FWD_LINK_REQ_MASK_LR:
      MSG_MED( "LR PDE Request.", 0, 0, 0);
      if(paix_xlate_ext_lr_req(start, &(req_msg_ptr->req_msg_payload.lr_req), length) != TRUE) /*lint !e506 */
      {
        return FALSE;
      }
      break;

    case IS801_FWD_LINK_REQ_MASK_TOM:
      MSG_MED( "TOM Request.", 0, 0, 0);
      if(paix_xlate_ext_tom_req(start, &(req_msg_ptr->req_msg_payload.tom_req), length) != TRUE) /*lint !e506 */
      {
        return FALSE;
      }
      break;

    default:
      /* unsupported request type from PDE */
      return FALSE;
    }
    
    return TRUE; /*lint !e506 */

}


/*===========================================================================

FUNCTION PAIX_XLATE_EXT_RESP_PAYLOAD

DESCRIPTION
  This function translates external PDDM response payload to internal format

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_ext_resp_payload
(
  byte                             *start,
  is801_fwd_link_resp_part_s_type  *resp_par_ptr,  
  byte                              resp_par_length,
  uint16                            buf_length
)
{
  boolean retVal = FALSE;
  
  if((start == NULL) || (resp_par_ptr == NULL))
  {
    MSG_ERROR("Input pointer is NULL", 0, 0, 0);
    return FALSE;
  }

  if(buf_length < resp_par_length)
  {
    MSG_ERROR("Malformed PDDM. Insufficient bytes", 0, 0, 0);
    return FALSE;
  }

  switch(resp_par_ptr->resp_type)
  {
    case IS801_FWD_LINK_RESP_MASK_REJ:
      MSG_ERROR( "Rej Response.", 0, 0, 0);
      retVal = paix_xlate_ext_rej_resp( start, &(resp_par_ptr->resp_msg_payload.rej_resp), resp_par_length, buf_length);
    break;

    case IS801_FWD_LINK_RESP_MASK_AA:
      MSG_MED( "AA Response.", 0, 0, 0);
      retVal = paix_xlate_ext_aa_resp(start, &(resp_par_ptr->resp_msg_payload.aa_resp), resp_par_length, buf_length);
    break;

    case IS801_FWD_LINK_RESP_MASK_SA:
      MSG_MED( "SA Response.", 0, 0, 0);
      retVal = paix_xlate_ext_sa_resp(start, &(resp_par_ptr->resp_msg_payload.sa_part), resp_par_length, buf_length);
    break;

    case IS801_FWD_LINK_RESP_MASK_ALM:
      MSG_MED( "ALM Response.", 0, 0, 0);
      retVal = paix_xlate_ext_alm_resp(start, &(resp_par_ptr->resp_msg_payload.alm_part), resp_par_length, buf_length); 
      break;

    case IS801_FWD_LINK_RESP_MASK_EPH:
      MSG_MED( "EPH Response.", 0, 0, 0);
      retVal = paix_xlate_ext_eph_resp(start, &(resp_par_ptr->resp_msg_payload.eph_part), resp_par_length, buf_length);
      break;

    case IS801_FWD_LINK_RESP_MASK_LR:
      MSG_HIGH( "LR Response.", 0, 0, 0);
      retVal = paix_xlate_ext_lr_resp( start, &(resp_par_ptr->resp_msg_payload.lr_resp), resp_par_length, buf_length);
      break;

    /* Following are not supported by current implementation */
    case IS801_FWD_LINK_RESP_MASK_BSC:
    case IS801_FWD_LINK_RESP_MASK_LASC:
    case IS801_FWD_LINK_RESP_MASK_LACC:
    case IS801_FWD_LINK_RESP_MASK_BSA:
    case IS801_FWD_LINK_RESP_MASK_NMB:
    case IS801_FWD_LINK_RESP_MASK_AC:
    case IS801_FWD_LINK_RESP_MASK_SHI:
    /* IS801-B handshake compliant */
      MSG_HIGH( "Unsupported response %d, ignore it", 0, 0, 0);
      retVal = TRUE;
    break;

    default:
      /* unsupported resp type from PDE */
      retVal = FALSE;
    }
    
    return retVal;
}	


/*===========================================================================
  
FUNCTION XLATE_INT_AA_REQ
  
DESCRIPTION
  This function translates a Acquisition Assistance request record from 
  internal to external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
boolean paix_xlate_int_aa_req
(
  byte              *buf_ptr,
  is801_aa_req_type *int_ptr,
  word              *msg_length_ptr /* in bits */    
)
{
  word    buffer_pos = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }    
    
  /* ----------------------------------------------------------------
  ** Write AA request header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_req_hdr_type, reserved );
  
  b_packb( GPS_AA_REQ_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_type ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_type );
  
  b_packb( AA_REQ_LEN, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_par_len ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_par_len );
  
  /* ----------------------------------------------------------------
  ** Write AA request record
  ** ---------------------------------------------------------------- */
  b_packb( int_ptr->dopp_req, buf_ptr, buffer_pos,
           FSIZ( pai_aa_req_rpt_type, dopp_req ));
  buffer_pos += FSIZ( pai_aa_req_rpt_type, dopp_req );
  
  b_packb( int_ptr->add_dopp_req, buf_ptr, buffer_pos,
           FSIZ( pai_aa_req_rpt_type, add_dopp_req ));
  buffer_pos += FSIZ( pai_aa_req_rpt_type, add_dopp_req );
  
  b_packb( int_ptr->code_ph_par_req, buf_ptr, buffer_pos,
           FSIZ( pai_aa_req_rpt_type, code_ph_par_req ));
  buffer_pos += FSIZ( pai_aa_req_rpt_type, code_ph_par_req );
  
  b_packb( int_ptr->az_el_req, buf_ptr, buffer_pos,
           FSIZ( pai_aa_req_rpt_type, az_el_req ));
  buffer_pos += FSIZ( pai_aa_req_rpt_type, az_el_req );
  
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_aa_req_rpt_type, reserved ));
  buffer_pos += FSIZ( pai_aa_req_rpt_type, reserved );
   
  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_aa_req() */


/*===========================================================================
  
FUNCTION XLATE_INT_SA_REQ
  
DESCRIPTION
  This function translates a Sensitivity Assistance request record from 
  internal to external format.
  
DEPENDENCIES
  None
 
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
boolean paix_xlate_int_sa_req
(
  byte              *buf_ptr,
  word              *msg_length_ptr /* in bits */      
)
{
  word    buffer_pos = 0;

  if(buf_ptr == NULL)
  {
    return FALSE;
  }   
    
  /* ----------------------------------------------------------------
  ** Write SA request header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_req_hdr_type, reserved );
  
  b_packb( GPS_SA_REQ_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_type ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_type );
  
  b_packb( SA_REQ_LEN, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_par_len ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_par_len );
  
  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_sa_req() */



/*===========================================================================

FUNCTION XLATE_INT_LR_REQ

DESCRIPTION
  This function translates a Location Response request record from internal 
  to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_lr_req
(
  byte              *buf_ptr,
  is801_lr_req_type *int_ptr,
  word              *msg_length_ptr /* in bits */        
)
{
  word    buffer_pos = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }    
    
  
  /* ----------------------------------------------------------------
  ** Write LR request header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_req_hdr_type, reserved );

  b_packb( LOC_RESP_REQ_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_type ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_type );

  b_packb( LR_REQ_LEN, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_par_len ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_par_len );

  /* ----------------------------------------------------------------
  ** Write LR request record
  ** ---------------------------------------------------------------- */
  b_packb( int_ptr->height_req, buf_ptr, buffer_pos,
           FSIZ( pai_lr_req_rpt_type, height_req ));
  buffer_pos += FSIZ( pai_lr_req_rpt_type, height_req );

  b_packb( int_ptr->clk_cor_gps_req, buf_ptr, buffer_pos,
           FSIZ( pai_lr_req_rpt_type, clk_cor_gps_req ));
  buffer_pos += FSIZ( pai_lr_req_rpt_type, clk_cor_gps_req );

  b_packb( int_ptr->velocity_req, buf_ptr, buffer_pos,
           FSIZ( pai_lr_req_rpt_type, velocity_req ));
  buffer_pos += FSIZ( pai_lr_req_rpt_type, velocity_req );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_lr_req_rpt_type, reserved ));
  buffer_pos += FSIZ( pai_lr_req_rpt_type, reserved );

  *msg_length_ptr = buffer_pos;  

  return TRUE; /*lint !e506 */
} /* paix_xlate_int_lr_req() */

/*===========================================================================
  
FUNCTION XLATE_INT_ALM_REQ
  
DESCRIPTION
  This function translates a GPS Almanac request record from internal to
  external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean paix_xlate_int_alm_req
(
  byte              *buf_ptr,
  word              *msg_length_ptr /* in bits */       
)
{
  word    buffer_pos = 0;

  if(buf_ptr == NULL)
  {
    return FALSE;
  }        
    
  /* ----------------------------------------------------------------
  ** Write ALM request header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_req_hdr_type, reserved );
  
  b_packb( GPS_ALM_REQ_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_type ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_type );
  
  b_packb( ALM_REQ_LEN, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_par_len ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_par_len );

  *msg_length_ptr = buffer_pos;  

  return TRUE; /*lint !e506 */
} /* paix_xlate_int_alm_req() */

/*===========================================================================
  
FUNCTION XLATE_INT_EPH_REQ
  
DESCRIPTION
  This function translates a GPS Ephemerise request record from internal to 
  external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean paix_xlate_int_eph_req
(
  byte              *buf_ptr,
  is801_eph_req_type *int_ptr,
  word              *msg_length_ptr /* in bits */       
)
{
  word    buffer_pos = 0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }  
  
    
  /* ----------------------------------------------------------------
  ** Write EPH request header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
             FSIZ( pai_req_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_req_hdr_type, reserved );
  
  b_packb( GPS_EPH_REQ_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_type ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_type );
  
  b_packb( EPH_REQ_LEN, buf_ptr, buffer_pos,
           FSIZ( pai_req_hdr_type, req_par_len ));
  buffer_pos += FSIZ( pai_req_hdr_type, req_par_len );

  /* ----------------------------------------------------------------
  ** Write EPH request record
  ** ---------------------------------------------------------------- */

  b_packb( int_ptr->ab_par_req, buf_ptr, buffer_pos,
     FSIZ( pai_eph_req_rpt_type, ab_par_req ));
  buffer_pos += FSIZ( pai_eph_req_rpt_type, ab_par_req );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
     FSIZ( pai_eph_req_rpt_type, reserved ));
  buffer_pos += FSIZ( pai_eph_req_rpt_type, reserved );

  *msg_length_ptr = buffer_pos;  
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_eph_req() */


/*===========================================================================

FUNCTION XLATE_INT_PDDM_HDR

DESCRIPTION
  This function translates a Position Determination Data Message header from 
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_pddm_hdr
(
  byte                *buf_ptr,
  is801_pddm_hdr_type *int_ptr
)
{
  word buffer_pos=0;

  if((buf_ptr == NULL) || (int_ptr == NULL))
  {
    return FALSE;
  }  
  
  /* ----------------------------------------------------------------
  ** Translate PDDM header from internal to external format
  ** ---------------------------------------------------------------- */
  b_packb( int_ptr->sess_start, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, sess_start ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, sess_start );

  b_packb( int_ptr->sess_end, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, sess_end ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, sess_end );

  b_packb( int_ptr->sess_source, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, sess_source ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, sess_source );

  b_packb( int_ptr->sess_tag, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, sess_tag ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, sess_tag );

  b_packb( int_ptr->pd_msg_type, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, pd_msg_type ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, pd_msg_type );

  b_packb( int_ptr->num_requests, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, num_requests ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, num_requests );

  b_packb( int_ptr->num_responses, buf_ptr, buffer_pos,
           FSIZ( pai_pddm_hdr_type, num_responses ));
  buffer_pos += FSIZ( pai_pddm_hdr_type, num_responses );

  return TRUE; /*lint !e506 */

} /* paix_xlate_int_pddm_hdr() */


/*===========================================================================

FUNCTION XLATE_INT_REJ_RESP

DESCRIPTION
  This function translates a Reject response record from internal to external 
  format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_rej_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */
)
{
  word buffer_pos=0;

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  }  
  
  /* ----------------------------------------------------------------
  ** Write REJ response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( REJ_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );

  b_packb( NUM_BYTES_IN_BIT(REJ_RESP_LEN), buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write REJ response record
  ** ---------------------------------------------------------------- */
  b_packb( int_ptr->resp_msg_payload.rej_resp.rej_req_type, buf_ptr, buffer_pos,
           FSIZ( pai_rej_res_rpt_type, rej_req_type ));
  buffer_pos += FSIZ( pai_rej_res_rpt_type, rej_req_type );

  b_packb( int_ptr->resp_msg_payload.rej_resp.rej_reason, buf_ptr, buffer_pos,
           FSIZ( pai_rej_res_rpt_type, rej_reason ));
  buffer_pos += FSIZ( pai_rej_res_rpt_type, rej_reason );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_rej_res_rpt_type, reserved2 ));
  buffer_pos += FSIZ( pai_rej_res_rpt_type, reserved2 );

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_rej_resp() */


/*===========================================================================

FUNCTION XLATE_INT_CACK_RESP

DESCRIPTION
  This function translates a Provide Cancellation Acknowledgement response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_cack_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
)
{
  word buffer_pos=0;

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 

  /* ----------------------------------------------------------------
  ** Write CACK response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( CACK_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );

  b_packb( NUM_BYTES_IN_BIT(CACK_RESP_LEN), buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write CACK response record
  ** ---------------------------------------------------------------- */
  b_packb( int_ptr->resp_msg_payload.cack_resp.cancel_type_ack, buf_ptr, buffer_pos,
           FSIZ( pai_cack_res_rpt_type, cancel_type_ack ));
  buffer_pos += FSIZ( pai_cack_res_rpt_type, cancel_type_ack );

  b_packb( int_ptr->resp_msg_payload.cack_resp.no_out_req, buf_ptr, buffer_pos,
           FSIZ( pai_cack_res_rpt_type, no_out_req ));
  buffer_pos += FSIZ( pai_cack_res_rpt_type, no_out_req );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_cack_res_rpt_type, reserved2 ));
  buffer_pos += FSIZ( pai_cack_res_rpt_type, reserved2 );

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_cack_resp() */


/*===========================================================================

FUNCTION XLATE_INT_MSI_RESP

DESCRIPTION
  This function translates a Provide MS Information response record from 
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_msi_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
)
{
  word buffer_pos=0;

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 

  /* ----------------------------------------------------------------
  ** Write MSI response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( MS_INFO_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );

  b_packb( NUM_BYTES_IN_BIT(MSI_RESP_LEN), buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write MSI response record
  ** ---------------------------------------------------------------- */
  b_packb( int_ptr->resp_msg_payload.msi_resp.ms_ls_rev, buf_ptr, buffer_pos,
           FSIZ( pai_msi_res_rpt_type, ms_ls_rev ));
  buffer_pos += FSIZ( pai_msi_res_rpt_type, ms_ls_rev );

  b_packb( int_ptr->resp_msg_payload.msi_resp.ms_mode, buf_ptr, buffer_pos,
           FSIZ( pai_msi_res_rpt_type, ms_mode ));
  buffer_pos += FSIZ( pai_msi_res_rpt_type, ms_mode );

  b_packb( int_ptr->resp_msg_payload.msi_resp.pilot_ph_cap, buf_ptr, buffer_pos,
           FSIZ( pai_msi_res_rpt_type, pilot_ph_cap ));
  buffer_pos += FSIZ( pai_msi_res_rpt_type, pilot_ph_cap );

  b_packw( int_ptr->resp_msg_payload.msi_resp.gps_acq_cap, buf_ptr, buffer_pos,
           FSIZ( pai_msi_res_rpt_type, gps_acq_cap ));
  buffer_pos += FSIZ( pai_msi_res_rpt_type, gps_acq_cap );

  b_packw( int_ptr->resp_msg_payload.msi_resp.loc_calc_cap, buf_ptr, buffer_pos,
           FSIZ( pai_msi_res_rpt_type, loc_calc_cap ));
  buffer_pos += FSIZ( pai_msi_res_rpt_type, loc_calc_cap );

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_msi_resp() */

/*===========================================================================

FUNCTION XLATE_INT_TOM_RESP

DESCRIPTION
  This function translates a Provide Time Offset Measurement response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_tom_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
)
{
  word buffer_pos=0;

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 

  /* ----------------------------------------------------------------
  ** Write MSTO response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( MSTO_MEAS_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );

  b_packb( NUM_BYTES_IN_BIT(MSTO_RESP_LEN), buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write MSTO response record
  ** ---------------------------------------------------------------- */
  b_packd( int_ptr->resp_msg_payload.tom_resp.time_ref_ms, buf_ptr, buffer_pos,
           FSIZ( pai_msto_res_rpt_type, time_ref_ms ));
  buffer_pos += FSIZ( pai_msto_res_rpt_type, time_ref_ms );

  b_packw( int_ptr->resp_msg_payload.tom_resp.ref_pn, buf_ptr, buffer_pos,
           FSIZ( pai_msto_res_rpt_type, ref_pn ));
  buffer_pos += FSIZ( pai_msto_res_rpt_type, ref_pn );

  b_packw( int_ptr->resp_msg_payload.tom_resp.mob_sys_t_offset, buf_ptr, buffer_pos,
           FSIZ( pai_msto_res_rpt_type, mob_sys_t_offset ));
  buffer_pos += FSIZ( pai_msto_res_rpt_type, mob_sys_t_offset );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_msto_res_rpt_type, reserved2 ));
  buffer_pos += FSIZ( pai_msto_res_rpt_type, reserved2 );

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_tom_resp() */

/*===========================================================================

FUNCTION XLATE_INT_LR_RESP

DESCRIPTION
  This function translates a Location response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_lr_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
)
{
  word buffer_pos=0;
  word len_pos;
  word res_size;
  word res_par_len;

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 
  
  /* ----------------------------------------------------------------
  ** Write LOC response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( LOC_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );
  
  /* Save the position where we are going to write the resp_par_len so
  ** that we can write resp_par_len later.
  */
  len_pos = buffer_pos;

  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write LOC response record
  ** ---------------------------------------------------------------- */
  b_packw( int_ptr->resp_msg_payload.lr_resp.time_ref_cdma, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, time_ref_cdma ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, time_ref_cdma );

  b_packd( int_ptr->resp_msg_payload.lr_resp.latitude, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, latitude ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, latitude );

  b_packd( int_ptr->resp_msg_payload.lr_resp.longitude, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, longitude ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, longitude );

  b_packb( int_ptr->resp_msg_payload.lr_resp.loc_uncrtnty_ang, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_ang ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_ang );

  b_packb( int_ptr->resp_msg_payload.lr_resp.loc_uncrtnty_a, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_a ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_a );

  b_packb( int_ptr->resp_msg_payload.lr_resp.loc_uncrtnty_p, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_p ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_p );

  b_packb( int_ptr->resp_msg_payload.lr_resp.fix_type, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, fix_type ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, fix_type );


  b_packb( int_ptr->resp_msg_payload.lr_resp.velocity_incl, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, velocity_incl ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, velocity_incl );

  if (int_ptr->resp_msg_payload.lr_resp.velocity_incl == TRUE) /*lint !e506 */
  {
    b_packw( int_ptr->resp_msg_payload.lr_resp.velocity_hor, buf_ptr, buffer_pos,
             FSIZ( pai_lr_res_rpt_type, velocity_hor ));
    buffer_pos += FSIZ( pai_lr_res_rpt_type, velocity_hor );

    b_packw( int_ptr->resp_msg_payload.lr_resp.heading, buf_ptr, buffer_pos,
             FSIZ( pai_lr_res_rpt_type, heading ));
    buffer_pos += FSIZ( pai_lr_res_rpt_type, heading );

    if (int_ptr->resp_msg_payload.lr_resp.fix_type == TRUE) /*lint !e506 */
    {
      b_packb( int_ptr->resp_msg_payload.lr_resp.velocity_ver, buf_ptr, buffer_pos,
               FSIZ( pai_lr_res_rpt_type, velocity_ver ));
      buffer_pos += FSIZ( pai_lr_res_rpt_type, velocity_ver );
    }
  }

  b_packb( int_ptr->resp_msg_payload.lr_resp.clock_incl, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, clock_incl ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, clock_incl );

  if (int_ptr->resp_msg_payload.lr_resp.clock_incl == TRUE) /*lint !e506 */
  {
    b_packd( int_ptr->resp_msg_payload.lr_resp.clock_bias, buf_ptr, buffer_pos,
             FSIZ( pai_lr_res_rpt_type, clock_bias ));
    buffer_pos += FSIZ( pai_lr_res_rpt_type, clock_bias );

    b_packw( int_ptr->resp_msg_payload.lr_resp.clock_drift, buf_ptr, buffer_pos,
             FSIZ( pai_lr_res_rpt_type, clock_drift ));
    buffer_pos += FSIZ( pai_lr_res_rpt_type, clock_drift );
  }

  b_packb( int_ptr->resp_msg_payload.lr_resp.height_incl, buf_ptr, buffer_pos,
           FSIZ( pai_lr_res_rpt_type, height_incl ));
  buffer_pos += FSIZ( pai_lr_res_rpt_type, height_incl );

  if (int_ptr->resp_msg_payload.lr_resp.height_incl == TRUE) /*lint !e506 */
  {
    b_packw( int_ptr->resp_msg_payload.lr_resp.height, buf_ptr, buffer_pos,
             FSIZ( pai_lr_res_rpt_type, height ));
    buffer_pos += FSIZ( pai_lr_res_rpt_type, height );

    b_packw( int_ptr->resp_msg_payload.lr_resp.loc_uncrtnty_v, buf_ptr, buffer_pos,
             FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_v ));
    buffer_pos += FSIZ( pai_lr_res_rpt_type, loc_uncrtnty_v );
  }

  /* Calculate the size of the reserved field needed and add it */
  res_size = RES_SIZE(buffer_pos);
  b_packb( PAI_RESERVED_VAL,
           buf_ptr, buffer_pos, res_size);
  buffer_pos += res_size;

  /* Calculate length of resp par record, note len_pos = size of header and buffer_pos = 
     total size so subtract header size to get record size. */
  res_par_len = ((buffer_pos - len_pos - 1) / NUM_BITS_IN_BYTE(byte));

  b_packb( res_par_len, buf_ptr, len_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION XLATE_INT_PPM_PART

DESCRIPTION
  This function translates a Provide Pilot Phase Measurements response record 
  from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_ppm_part
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  is801_ppm_fragment_info_s_type  *ppm_fragment_info_ptr,
  word                       *msg_length_ptr /* in bits */    
)
{
  uint16 i;
  word buffer_pos=0;
  word len_pos;
  word res_size;
  word res_par_len=0;
  byte start_pilot_arr_idx;
  byte end_pilot_arr_idx;
  
  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 
  
  start_pilot_arr_idx = ppm_fragment_info_ptr->pilot_arr_idx_to_resume;
  end_pilot_arr_idx   = start_pilot_arr_idx + ppm_fragment_info_ptr->num_pilot_in_this_pddm;
  
  /* ----------------------------------------------------------------
  ** Write PPM response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( PPM_MEAS_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );

  len_pos = buffer_pos;
  
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write PPM response part record
  ** ---------------------------------------------------------------- */
  
  b_packd ( int_ptr->resp_msg_payload.ppm_resp.fix.time_ref_ms, buf_ptr, buffer_pos, 
                FSIZ( pai_ppm_res_fix_type, time_ref_ms));
  buffer_pos += FSIZ( pai_ppm_res_fix_type, time_ref_ms);
                                               
  b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.offset_incl, buf_ptr, buffer_pos, 
                FSIZ( pai_ppm_res_fix_type, offset_incl ));
  buffer_pos += FSIZ( pai_ppm_res_fix_type, offset_incl );

  /* if Offset is included then add the appropriate fields */
  if (TRUE == int_ptr->resp_msg_payload.ppm_resp.fix.offset_incl) /*lint !e506 */
  {
    b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.mob_sys_t_offset, buf_ptr, buffer_pos,
                FSIZ( pai_ppm_res_fix_type, mob_sys_t_offset ) );
    buffer_pos += FSIZ( pai_ppm_res_fix_type, mob_sys_t_offset ) ;
  }

  b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.ref_pn,
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, ref_pn) );
  buffer_pos += FSIZ ( pai_ppm_res_fix_type, ref_pn );

  b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.ref_pilot_strength, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, ref_pilot_strength ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, ref_pilot_strength );
  
  b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.band_class, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, band_class ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, band_class );

  b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.cdma_freq, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, cdma_freq ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, cdma_freq );

  b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.base_id, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, base_id ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, base_id );

  b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.sid, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, sid ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, sid );

  b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.nid, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, nid ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, nid );

  b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.total_rx_pwr, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, total_rx_pwr ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, total_rx_pwr );

  b_packb ( ppm_fragment_info_ptr->part_num - 1,  /* IS801 uses 0 based numbering */ 
          buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, part_num ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, part_num );

  b_packb ( ppm_fragment_info_ptr->total_parts - 1,  /* IS801 uses 0 based numbering */
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, total_parts ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, total_parts );

  b_packb ( ppm_fragment_info_ptr->num_pilot_in_this_pddm,
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, num_pilots_p ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, num_pilots_p );

  /* For each pilot being reported include the following information */
  for (i = start_pilot_arr_idx; 
      ((i < int_ptr->resp_msg_payload.ppm_resp.fix.num_pilots_p) && (i < end_pilot_arr_idx)); 
      i++) 
  {
    b_packd ( int_ptr->resp_msg_payload.ppm_resp.var[i].pilot_pn_phase, 
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_var_type, pilot_pn_phase) ); 
    buffer_pos += FSIZ( pai_ppm_res_var_type, pilot_pn_phase);
    b_packb ( int_ptr->resp_msg_payload.ppm_resp.var[i].pilot_strength, 
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_var_type, pilot_strength) ); 
    buffer_pos += FSIZ( pai_ppm_res_var_type, pilot_strength);
    b_packb ( int_ptr->resp_msg_payload.ppm_resp.var[i].rms_err_phase, 
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_var_type, rms_err_phase) ); 
    buffer_pos += FSIZ( pai_ppm_res_var_type, rms_err_phase);
  }

  b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.add_pilots_incl, 
            buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, add_pilots_incl ) );
  buffer_pos += FSIZ( pai_ppm_res_fix_type, add_pilots_incl );

  if (TRUE == int_ptr->resp_msg_payload.ppm_resp.fix.add_pilots_incl) /*lint !e506 */
  {
    b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.add_band_class, 
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, add_band_class ) );
    buffer_pos += FSIZ( pai_ppm_res_fix_type, add_band_class );
    b_packw ( int_ptr->resp_msg_payload.ppm_resp.fix.add_cdma_freq, 
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, add_cdma_freq ) );
    buffer_pos += FSIZ( pai_ppm_res_fix_type, add_cdma_freq );
    b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.add_total_rx_pwr, 
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, add_total_rx_pwr ) );
    buffer_pos += FSIZ( pai_ppm_res_fix_type, add_total_rx_pwr );
    b_packb ( int_ptr->resp_msg_payload.ppm_resp.fix.num_add_pilots_p - 1, // IS801 uses zero based numbering
              buf_ptr, buffer_pos, FSIZ( pai_ppm_res_fix_type, num_add_pilots_p ) );
    buffer_pos += FSIZ( pai_ppm_res_fix_type, num_add_pilots_p );

    for (i = 0; i<int_ptr->resp_msg_payload.ppm_resp.fix.num_add_pilots_p; i++) 
    {
      b_packd ( int_ptr->resp_msg_payload.ppm_resp.var[i].pilot_pn_phase, 
                buf_ptr, buffer_pos, FSIZ( pai_ppm_res_var_type, pilot_pn_phase) ); 
      buffer_pos += FSIZ( pai_ppm_res_var_type, pilot_pn_phase);
      
      b_packb ( int_ptr->resp_msg_payload.ppm_resp.var[i].pilot_strength, 
                buf_ptr, buffer_pos, FSIZ( pai_ppm_res_var_type, pilot_strength) );  
      buffer_pos += FSIZ( pai_ppm_res_var_type, pilot_strength);
      
      b_packb ( int_ptr->resp_msg_payload.ppm_resp.var[i].rms_err_phase, 
                buf_ptr, buffer_pos, FSIZ( pai_ppm_res_var_type, rms_err_phase) ); 
      buffer_pos += FSIZ( pai_ppm_res_var_type, rms_err_phase);
    }
  }
  
  /* Calculate the size of the reserved field needed and add it */
  res_size = RES_SIZE(buffer_pos);
  b_packb( PAI_RESERVED_VAL,
           buf_ptr, buffer_pos, res_size);
  buffer_pos += res_size;

  /* Calculate length of resp par record, note len_pos = size of header and buffer_pos = 
     total size so subtract header size to get record size. */
  res_par_len = ( (buffer_pos - len_pos - 1) / NUM_BITS_IN_BYTE(byte));

  b_packb( res_par_len, buf_ptr, len_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_ppm_part() */

/*===========================================================================
  
FUNCTION XLATE_INT_PRM_PART
  
DESCRIPTION
  This function translates a Provide GPS Pseudorange response record from 
  internal to external format.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Errors that were encountered while translating the message.
  
SIDE EFFECTS
  None
  
===========================================================================*/  
boolean paix_xlate_int_prm_part
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  is801_prm_fragment_info_s_type  *prm_fragment_info_ptr,  
  word                       *msg_length_ptr /* in bits */    
)
{
  uint16 i;
  word buffer_pos=0;
  word len_pos;
  word res_size;
  word res_par_len;
  byte start_meas_arr_idx;
  byte end_meas_arr_idx;
  
  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL) || (prm_fragment_info_ptr == NULL))
  {
    return FALSE;
  } 
  
  start_meas_arr_idx = prm_fragment_info_ptr->sv_arr_idx_to_resume;
  end_meas_arr_idx   = start_meas_arr_idx + prm_fragment_info_ptr->num_meas_in_this_pddm;
  
  /* ----------------------------------------------------------------
  ** Write PR response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );
  
  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );
  
  b_packb( PR_MEAS_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );
  
  len_pos = buffer_pos;
  
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );
  
  /* ----------------------------------------------------------------
  ** Write PR response part record
  ** ---------------------------------------------------------------- */
  b_packw ( prm_fragment_info_ptr->part_num - 1,/* IS801 uses 0-7 instead of 1-8 */
            buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, part_num) );
  buffer_pos += FSIZ( pai_pr_res_fix_type, part_num);
    
  b_packb ( prm_fragment_info_ptr->total_parts - 1,/* IS801 uses 0-7 instead of 1-8 */ 
            buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, total_parts) );
  buffer_pos += FSIZ( pai_pr_res_fix_type, total_parts);
    
  b_packb ( prm_fragment_info_ptr->num_meas_in_this_pddm - 1,/* IS801 uses 0 based counting */ 
            buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, num_ps_ranges_p) );
  buffer_pos += FSIZ( pai_pr_res_fix_type, num_ps_ranges_p);
    
  b_packd ( int_ptr->resp_msg_payload.prm_resp.fix.time_ref, 
            buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, time_ref) );
  buffer_pos += FSIZ( pai_pr_res_fix_type, time_ref);
    
  b_packb ( int_ptr->resp_msg_payload.prm_resp.fix.time_ref_src, 
            buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, time_ref_src) );
  buffer_pos += FSIZ( pai_pr_res_fix_type, time_ref_src);   

  //<<<[+ by Liao] Re-visit
  /* If PPM is not including MSTO don't include MSTO in PRM.
  */
  //>>>[+ End]

  b_packb ( int_ptr->resp_msg_payload.prm_resp.fix.offset_incl, 
            buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, offset_incl) );
  buffer_pos += FSIZ( pai_pr_res_fix_type, offset_incl);

  /* If the offset is included add the appropriate fields */
  if (TRUE == int_ptr->resp_msg_payload.prm_resp.fix.offset_incl) /*lint !e506 */
  {
    b_packw ( int_ptr->resp_msg_payload.prm_resp.fix.ref_pn, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, ref_pn) );
    buffer_pos += FSIZ( pai_pr_res_fix_type, ref_pn );
  
    b_packw ( int_ptr->resp_msg_payload.prm_resp.fix.mob_sys_t_offset, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_fix_type, mob_sys_t_offset) );
    buffer_pos += FSIZ( pai_pr_res_fix_type, mob_sys_t_offset );
  }
    
  /* For each pseudorange included, add the corresponding fields */
  for (i = start_meas_arr_idx; 
      ((i < int_ptr->resp_msg_payload.prm_resp.fix.num_ps_ranges_p) && (i < end_meas_arr_idx));   
      i++) 
  {
    b_packb ( int_ptr->resp_msg_payload.prm_resp.var[i].sv_prn_num - 1, /* IS801 uses 0 based numbering */
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, sv_prn_num) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, sv_prn_num);
    b_packb ( int_ptr->resp_msg_payload.prm_resp.var[i].sv_cno, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, sv_cno) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, sv_cno);
    b_packw ( int_ptr->resp_msg_payload.prm_resp.var[i].ps_doppler, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, ps_doppler) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, ps_doppler);
    b_packw ( int_ptr->resp_msg_payload.prm_resp.var[i].sv_code_ph_wh, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, sv_code_ph_wh) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, sv_code_ph_wh);
    b_packw ( int_ptr->resp_msg_payload.prm_resp.var[i].sv_code_ph_fr, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, sv_code_ph_fr) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, sv_code_ph_fr);
    b_packb ( int_ptr->resp_msg_payload.prm_resp.var[i].mul_path_ind, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, mul_path_ind) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, mul_path_ind);
    b_packb ( int_ptr->resp_msg_payload.prm_resp.var[i].ps_range_rms_er, 
              buf_ptr, buffer_pos, FSIZ( pai_pr_res_var_type, ps_range_rms_er) ); 
    buffer_pos += FSIZ( pai_pr_res_var_type, ps_range_rms_er);
  }
  
  /* Calculate the size of the reserved field needed and add it */
  res_size = RES_SIZE(buffer_pos);
  b_packb( PAI_RESERVED_VAL,
           buf_ptr, buffer_pos, res_size);
  buffer_pos += res_size;
  
  /* Calculate length of resp par record, note len_pos = size of header and buffer_pos = 
     total size so subtract header size to get record size. */
  res_par_len = ((buffer_pos - len_pos - 1) / NUM_BITS_IN_BYTE(byte));
  
  b_packb( res_par_len, buf_ptr, len_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));  

  *msg_length_ptr = buffer_pos;
  
  return TRUE; /*lint !e506 */
} /* paix_xlate_int_pr_part() */


/*===========================================================================

FUNCTION XLATE_INT_MSID_HDR

DESCRIPTION
  This function translates a MSID header for Position Determination from 
  internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_msid_hdr
(
  byte *buf_ptr,
  paii_msid_hdr_type *int_ptr,
  word *msg_length_ptr /* in bits */  
)
{
  word buffer_pos = 0;
    /* Position to pack bits in message */
  word res_size;
  /* Size of the reserved field */

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 

  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* We only support IMSI class 0, type 3 right now */
  if ( int_ptr->msid_type != MSID_TYPE_IMSI || 
       int_ptr->msid.type2.imsi.imsi_class != 0 || 
       int_ptr->msid.type2.imsi.class_type.zero.imsi_class_0_type != 3 ) 
  {
      *msg_length_ptr = 0;
      return FALSE;
  }

  /* ----------------------------------------------------------------
  ** Translate MSID header from internal to external format
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_msid_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_msid_hdr_type, reserved );

  b_packb( int_ptr->msid_type, buf_ptr, buffer_pos,
           FSIZ( pai_msid_hdr_type, msid_type ));
  buffer_pos += FSIZ( pai_msid_hdr_type, msid_type );

  b_packb( int_ptr->msid_len, buf_ptr, buffer_pos,
           FSIZ( pai_msid_hdr_type, msid_len ));
  buffer_pos += FSIZ( pai_msid_hdr_type, msid_len );
  
  b_packb( int_ptr->msid.type2.imsi.imsi_class, buf_ptr, buffer_pos,
           FSIZ( cai_msid2_type, imsi_class ));
  buffer_pos += FSIZ( cai_msid2_type, imsi_class );

  b_packb( int_ptr->msid.type2.imsi.class_type.zero.imsi_class_0_type, buf_ptr, buffer_pos,
           FSIZ( cai_imsi_class0_type, imsi_class_0_type ));
  buffer_pos += FSIZ( cai_imsi_class0_type, imsi_class_0_type );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos, 
           FSIZ( cai_imsi_class0_type3_type, reserved ));
  buffer_pos += FSIZ( cai_imsi_class0_type3_type, reserved );

  b_packw( int_ptr->msid.type2.imsi.class_type.zero.type.three.mcc, buf_ptr, buffer_pos, 
           FSIZ( cai_imsi_class0_type3_type, mcc ));
  buffer_pos += FSIZ( cai_imsi_class0_type3_type, mcc );

  b_packb( int_ptr->msid.type2.imsi.class_type.zero.type.three.imsi_11_12, buf_ptr, buffer_pos, 
           FSIZ( cai_imsi_class0_type3_type, imsi_11_12 ));
  buffer_pos += FSIZ( cai_imsi_class0_type3_type, imsi_11_12 );
    
  b_packq( int_ptr->msid.type2.imsi.class_type.zero.type.three.imsi_s, buf_ptr, buffer_pos, 
         FSIZ( cai_imsi_class0_type3_type, imsi_s ));
  buffer_pos += FSIZ( cai_imsi_class0_type3_type, imsi_s );
  
  /* Calculate the size of the reserved field needed and add it */
  
  res_size = RES_SIZE(buffer_pos);
  b_packb( PAI_RESERVED_VAL,
           buf_ptr, buffer_pos, res_size);
  buffer_pos += res_size;

  *msg_length_ptr = buffer_pos;

  return TRUE; /*lint !e506 */

} /* paix_xlate_int_msid_hdr() */
/*===========================================================================

FUNCTION PAIX_XLATE_INT_PRDM_BSP_RESP

DESCRIPTION
  This function translates a proprietary BS System Parameter response 
  record from internal to external format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
boolean paix_xlate_int_prdm_bsp_resp
(
  byte                       *buf_ptr,
  is801_rev_link_resp_s_type *int_ptr,
  word                       *msg_length_ptr /* in bits */    
)
{
  word buffer_pos = 0;
    /* Position to pack bits in message */

  if((buf_ptr == NULL) || (int_ptr == NULL) || (msg_length_ptr == NULL))
  {
    return FALSE;
  } 
  
  /* ----------------------------------------------------------------
  ** Write BSP response header
  ** ---------------------------------------------------------------- */
  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, reserved ));
  buffer_pos += FSIZ( pai_resp_hdr_type, reserved );

  b_packb( int_ptr->unsolicited, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, unsol_resp ));
  buffer_pos += FSIZ( pai_resp_hdr_type, unsol_resp );

  b_packb( PRDM_BSP_RESP_CODE, buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_type ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_type );

  b_packb( NUM_BYTES_IN_BIT(PRDM_BSP_RESP_LEN), buf_ptr, buffer_pos,
           FSIZ( pai_resp_hdr_type, resp_par_len ));
  buffer_pos += FSIZ( pai_resp_hdr_type, resp_par_len );

  /* ----------------------------------------------------------------
  ** Write BSP response record
  ** ---------------------------------------------------------------- */

  b_packw( int_ptr->resp_msg_payload.prdm_bsp_resp.time_ref_cdma, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, time_ref_cdma ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, time_ref_cdma );

  b_packb( int_ptr->resp_msg_payload.prdm_bsp_resp.still_active, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, still_active ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, still_active );

  b_packb( int_ptr->resp_msg_payload.prdm_bsp_resp.band_class, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, band_class ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, band_class );

  b_packw( int_ptr->resp_msg_payload.prdm_bsp_resp.cdma_freq, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, cdma_freq ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, cdma_freq );

  b_packw( int_ptr->resp_msg_payload.prdm_bsp_resp.pilot_pn, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, pilot_pn ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, pilot_pn );

  b_packw( int_ptr->resp_msg_payload.prdm_bsp_resp.sid, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, sid ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, sid );

  b_packw( int_ptr->resp_msg_payload.prdm_bsp_resp.nid, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, nid ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, nid );

  b_packw( int_ptr->resp_msg_payload.prdm_bsp_resp.base_id, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, base_id ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, base_id );

  b_packd( int_ptr->resp_msg_payload.prdm_bsp_resp.base_lat, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, base_lat ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, base_lat );

  b_packd( int_ptr->resp_msg_payload.prdm_bsp_resp.base_long, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, base_long ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, base_long );

  b_packb( PAI_RESERVED_VAL, buf_ptr, buffer_pos,
           FSIZ( pai_prdm_bsp_res_rpt_type, reserved ));
  buffer_pos += FSIZ( pai_prdm_bsp_res_rpt_type, reserved );


  *msg_length_ptr = buffer_pos;

  return TRUE; /*lint !e506 */
} /* paix_xlate_int_prdm_bsp_resp() */



