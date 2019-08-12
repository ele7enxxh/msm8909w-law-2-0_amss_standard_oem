/*===========================================================================
                         TM  A S N 1  U T I L

DESCRIPTION
  This file contains wrapper functions that provide an interface to the OSS
  ASN.1 subsystem.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The memory manager and the ASN.1 subsystem must be initialized before the 
  decode or encode functions can be utilized.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/src/tm_lpp_asn1util.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ------------------------------------------------------
  03/05/14   sj      Fixed LPP CP Fuzz crash
  12/14/11   gk      Lint and other fixes
  10/27/11   gk      initial version
===========================================================================*/




/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

#include "gps_variation.h"
#include "customer.h"

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "tm_asn1_common.h"
#include "tm_osysinit.h"
#include "msg.h"
#include "err.h"
#include "tm_lpp.h"
#include "tm_asn1util.h"
#include "tm_lpp_asn1.h"
#include "tm_lpp_asn1util.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
#define     TM_OSYS_ASN1_MAX_LINE_LENGTH 38

/*! @brief OSYS temp buffer size
*/
#define     TM_OSYS_ASN1_TEMP_BUF_LENGTH 1024

static char buffer[TM_OSYS_ASN1_MAX_LINE_LENGTH+1];
static int buf_index;

extern struct tm_osys_asn1util_s                 tm_lpp_osys_asn1util;
extern struct tm_osys_asn1util_dynamic_data_s    tm_lpp_osys_asn1util_dynamic_data;
extern tm_osys_asn1util_s *tm_lpp_osys_priv_ptr ;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================*/
/*!
    @brief
    This function accumulates characters until it has a complete line, then it
    prints the line.

    @return
    None
*/
/*=========================================================================*/
void tm_lpp_asn1_osys_print_char
(
  char ch /*!< char to print out */
)
{

  if (ch == '\n')
  {
    if (buf_index == 0)
    {
      /* newline at the start of a line -- toss it
       */
      return;
    }

    /* newline at not the end of a line -- print the line
     */
    buffer[buf_index] = 0;      // Null terminate the line
    buf_index = 0;
    /* print_line(); */
    MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", buffer);
    return;
  }

  /* Just a plain old character. Add it to the line to be printed.
   */
  buffer[buf_index++] = ch;

  /* Print the line if we have reached/exceeded our max line length
   */
  if (buf_index >= TM_OSYS_ASN1_MAX_LINE_LENGTH)
  {
    buffer[buf_index] = 0;      // Null terminate the line
    buf_index = 0;
    /* print_line(); */
    MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", buffer); 
  }

} /* tm_lpp_asn1_osys_print_char() */

/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_printf

===========================================================================*/
/*!
    @brief
    This is the printf() function for objective systems use.

    @return
    None
*/
/*=========================================================================*/
void tm_lpp_asn1_osys_printf
(
  void *stream, const char *format, va_list args 
)
{

  
} /* tm_lpp_asn1_osys_printf() */




/* ===========================================================================*/
/*!
    @brief
    This function ASN.1 encodes a passed signalling message. 
    This function returns a pdu_buf_type via an output parameter. If encoding
    failed, pdu_buf.value will be NULL and pdu_buf.length is meaningless. If
    encoding succeeded, pdu_buf.value will be non-NULL and pdu_buf.length will
    contain the number of bytes in the encoded PDU.

    @param unencoded_pdu_ptr - Pointer to the unencoded PDU
    @param encoded_pdu - This is a variable of type pdu_buf_type and needs to
      be declared by the caller and passed here as a parameter. If the
      encoding operation succeeds, then the value field of this structure will
      point to the encoded PDU and the length field will indicate the size of
      the encoded PDU in bytes
    @param pdu_num - Type of the message being decoded. 
 
    @return
    int - status of the encode operation
*/
/*=========================================================================*/

int tm_lpp_osys_asn1_encode_pdu ( void      *unencoded_pdu_ptr,
                              tm_pdu_buf_type *pdu_buf_ptr)
{
  int encode_status = 0;

  /* Set the allocation of the encoded ptr as Dynamic. SO that obj only allocated ptr.
  Note for Obj Sys a sttaic address also can be provided*/

  encode_status = pu_setBuffer (&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),
                                  NULL, 
                                  0, 
                                  FALSE);
                                  
                                  
  if ( (encode_status =asn1PE_LPP_Message(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt), 
                                    (LPP_Message*) unencoded_pdu_ptr)
       ) != 0 
     )
  {
    MSG_ERROR("TM_LPP_ASN_UTIL: ASN1 LPP encoding error: stat = %u", encode_status, 0, 0);
    return encode_status;
  }                                
                                  
  pdu_buf_ptr->value = 
    pe_GetMsgPtr(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),
                 (OSINT32*)&(pdu_buf_ptr->length));

  if(encode_status != 0)
  {
    MSG_HIGH("TM_LPP_ASN_UTIL: Encoding failed. Status = %d", encode_status, 0, 0);
    rtxErrPrint(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt));
  }
  if ( encode_status == 0 )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
      "TM_LPP_ASN_UTIL:LPP encode buf alloc: %lx", pdu_buf_ptr->value);
  }
  return encode_status;

}

void tm_lpp_init_encoding_context()
{
  /* release all dynamically allocated buffers, so there is no carried over
     memory */
  MSG_HIGH("TM_LPP_ASNUTIL:**********Init LPP context**********", 0,0,0);
  rtxMemReset(&tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt);
}

/* ===========================================================================*/
/*!
    @brief
    This function ASN.1 decodes a passed signalling message. 

    The pointer to decoded PDU is set to NULL if an error occurs, otherwise
    points to the decoded PDU
 
    @param encoded_pdu_ptr - Pointer to the encoded PDU
    @param encoded_pdu_size - Size in bytes of the encoded message
    @param decoded_pdu_ptr - A NULL pointer declared to be type of the
      message being decoded, and must be passed in by the caller
    @param pdu_num - Type of the message being decoded. Must be one of
      the 'OSYS PDU Types' defined in lte_rrc_osys_asn1util.h
 
    @return
    int - status of the decode operation
*/                                                                           
/*=========================================================================*/
void  tm_lpp_osys_asn1_decode_pdu(void *encoded_pdu_ptr,                         
                              size_t encoded_pdu_size,
                              void *p_pdu_decoded,
                              int *decode_status_ptr)
{
  OSUINT32   decoded_length;

  *decode_status_ptr = pu_setBuffer(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),
                               (OSOCTET *)encoded_pdu_ptr, 
                               encoded_pdu_size, 
                               FALSE);
  if(*decode_status_ptr != 0)
  {
    MSG_ERROR("TM_LPP_ASN1_UTIL:Decode Error. Could not initialize buffer", 0, 0, 0);
    *decode_status_ptr = C_ASN1_DECODE_BUFFER_NOT_ALLOCATED;
    return;
  }
  
  /* Invoke the ASN.1 decode function depending on type of PDU passed in*/
  *decode_status_ptr =
    asn1PD_LPP_Message( &(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),
                          (LPP_Message *)p_pdu_decoded );
                          
  if (*decode_status_ptr != 0)
  {
    MSG_ERROR("TM_LPP_ASN_UTIL: Decoding failure: %d", *decode_status_ptr, 0, 0);
    rtxErrPrint(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt));
    if (0 != rtxErrReset(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt)))
    {
      MSG_ERROR("TM_LPP_ASN_UTIL: rtxErrReset failure", 0, 0, 0);
    }
    //*decode_status_ptr = C_ASN1_DECODE_FAILURE;
  }
  else
  {
    decoded_length = pu_getMsgLen(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt));
  
    MSG_MED("TM_LPP_ASN_UTIL: Decoding status %d, length: %d", 
            *decode_status_ptr, decoded_length, 0);
  }
  

}



/*===========================================================================

  FUNCTION:  tm_osys_asn1_free_buf

===========================================================================*/
/*!
    @brief
    This function frees memory that was allocated by
    tm_osys_asn1_encode_pdu()

    @return
    None
*/
/*=========================================================================*/
void tm_lpp_asn1_free_osys_buf
(
  void *buf
)
{
  if (buf != NULL)
  {
    rtxMemFreePtr(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt), buf);
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
            "TM_LPP_ASN_UTIL:LPP encode buf free: %lx", buf);

  }  
  buf = NULL;
} 

void tm_lpp_asn1_err_print( void )
{
  rtxErrPrint(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt));
  if (0 != rtxErrReset(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt)))
  {
    MSG_ERROR("TM_LPP_ASN_UTIL: rtxErrReset failure", 0, 0, 0);
  }
    
}


/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_lpppayload_free

===========================================================================*/
/*!
    @brief
    This function is called by the Objective Systems ASN.1 libraries in order
    to free dynamically allocated memory for individual elements within the PDU

    @return
    void
*/
/*=========================================================================*/
void tm_lpp_asn1_osys_lpppayload_free
(
  supl_Ver2_PosPayLoad_extension_lPPPayload *ptr    /*!< Pointer to memory location to be freed */
)
{
  asn1Free_supl_Ver2_PosPayLoad_extension_lPPPayload( &(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),
     ptr);
  ptr = NULL;
  return;
} /* tm_lpp_asn1_osys_lpppayload_free() */

/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_IE_free

===========================================================================*/
/*!
    @brief
    This function is called by the Objective Systems ASN.1 libraries in order
    to free dynamically allocated memory for individual elements within the PDU

    @return
    void
*/
/*=========================================================================*/
void tm_lpp_asn1_osys_IE_free
(
  void **ptr    /*!< Pointer to memory location to be freed */
)
{
  
  if ( ptr != NULL) 
  {
    if ( *ptr != NULL )
    {
      rtxMemFreePtr(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt), *ptr);
    }
    else
    {
      MSG_ERROR("Null Osys ptr 0x%x", ptr, 0, 0);
    }
    *ptr = NULL;
  }
  else
  {
    MSG_ERROR("Null Osys ptr ptr", 0, 0, 0);
  }
  return;
} /* tm_lpp_asn1_osys_IE_free() */



/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_IE_realloc

===========================================================================*/
/*!
    @brief
    This function is called by the Objective Systems ASN.1 libraries in order
    to allocate dynamic memory for individual elements within the PDU while a
    PDU is being constructed by hand.

    @return
    void
*/
/*=========================================================================*/
void * tm_lpp_asn1_osys_IE_realloc
(
  void *ptr,    /*!< Pointer to memory location to reallocate */
  size_t size   /*!< Size in bytes to be allocated */
)
{
  return rtxMemRealloc(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt), ptr, size);
} /* lte_rrc_asn1_osys_IE_realloc() */

/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_IE_malloc

===========================================================================*/
/*!
    @brief
    This function is called by the Objective Systems ASN.1 libraries in order
    to allocate dynamic memory for individual elements within the PDU while a
    PDU is being constructed by hand.

    @return
    void
*/
/*=========================================================================*/
void * tm_lpp_asn1_osys_IE_malloc
(
  size_t size   /*!< Size in bytes to be allocated */
)
{
  return rtxMemAllocZ(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt), size);
} /* tm_lpp_asn1_osys_IE_malloc() */


/*===========================================================================

  FUNCTION:  tm_asn1_free_lpp_message_IEs

===========================================================================*/
/*!
    @brief
    This function is called by the Objective Systems ASN.1 libraries in order
    to free the dynamic memory for individual elements within the PDU while a
    PDU is being constructed by hand.

    @return
    void
*/
/*=========================================================================*/

void tm_asn1_free_lpp_message_IEs(LPP_Message *p_lpp_pdu, int pdu_num)
{
  
  if ( p_lpp_pdu == NULL )
  {                 
    MSG_HIGH("TM_LPP_ASN_UTIL: Nothing to free. PDU is null %d", pdu_num, 0,0);
    return;
  }
  
  MSG_HIGH("TM_LPP_ASN_UTIL: Free LPP PDU  %d", pdu_num, 0,0);
  asn1Free_LPP_Message(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),
                       p_lpp_pdu);
  p_lpp_pdu = NULL;
}

/*===========================================================================*/
/*!
    @brief
    This function is called by the Objective Systems ASN.1 libraries in order
    to allocate dynamic memory for individual elements within the PDU while a
    PDU is being constructed by hand.

    @return
    void
*/
/*=========================================================================*/

void tm_lpp_asn1_osys_alloc_lpppayload_array(supl_Ver2_PosPayLoad_extension_lPPPayload *elem)
{
  ALLOC_ASN1ARRAY1(&(tm_lpp_osys_priv_ptr->dd_ptr->tm_asn1_ctxt),elem,OSDynOctStr); 
}
 
/*===========================================================================*/
/*!
    @brief
    The OSDynOctStr is a structure containing a length field in bits (numocts)
    and a pointer to an array of one or more bytes (data). The bytes are ordered
    from most significant to least significant. It is possible that the number
    of bits is not divisible by 8. If this is the case, the remaining bits
    ( in the last byte ) will reside in the upper bits of the byte.  

    @return
    Returns integer containing contents of OSDynOctStr.
*/
/*=========================================================================*/

uint32 tm_lpp_osys_asn1_bitstring_to_int
(
  size_t max_bytes,           /*!< Maximum number of bytes that are available 
                                   to hold result - use sizeof(uint32), 
                                   sizeof(uint16), etc */
  ASN1BitStr32 *bitstring_ptr  /*!< Pointer to the Objective Systems bitstring*/
)
{
  uint32 result = 0;
  uint32 num_bits = 0;
  uint8  *data_ptr = NULL;

  if(bitstring_ptr == NULL)
  {
    MSG_ERROR("TM_LPP_ASN1UTIL: Null data ptr",0,0,0);
    return result;
  }

  num_bits = (uint32) bitstring_ptr->numbits;
  data_ptr = (uint8 *) bitstring_ptr->data;


  /* While there are bits remaining, build the int */
  while (num_bits)
  {
    if (num_bits >= 8)
    {
      /* More than 8 bits remaining so shift the next byte in */
      result <<= 8;
      result |= *data_ptr++;
      num_bits -= 8;
    }
    else
    {
      /* Less than 8 bits remaining. Shift the remaining bits in */ 
      result <<= num_bits;
      result |= ((*data_ptr++) >> (8 - num_bits));
      num_bits = 0;
    }
  }

  return ( result );
} /*  */

/*===========================================================================*/
/*!
    @brief
    The OSDynOctStr is a structure containing a length field in bits (numocts)
    and a pointer to an array of one or more bytes (data). The bytes are ordered
    from most significant to least significant. It is possible that the number of
    bits is not divisible by 8. If this is the case, the remaining bits ( in the
    last byte ) will reside in the upper bits of the byte.  

    Calling function shall fill in valid length field, and also allocate
    memory for bitstring value.

    @return
    Returns TRUE/FALSE.

*/
/*=========================================================================*/


boolean tm_lpp_osys_asn1_int_to_bitstring
(
  uint32 int_value,           /*!< Integer value to be converted into Objective 
                                   Systems bitstring */
  ASN1BitStr32 *bitstring_ptr /*!< Pointer to the Objective Systems bitstring - 
                                  length field should be filled up correctly or 
                                  else API yields a bad result */
)
{
  int32 octet_shift = 0;
  uint8 bit_shift = 0;   
  uint8  *data_ptr = NULL;


  if (bitstring_ptr->numbits <= 0) 
  {
    return FALSE;
  }

  octet_shift = (int32) (bitstring_ptr->numbits/8);
  bit_shift = bitstring_ptr->numbits % 8;

  /* Memory must have been allocated from calling procedure */
  data_ptr = (uint8 *) bitstring_ptr->data;

  /* start building oss bitstring with MSB first */
  while (octet_shift > 0) 
  {
    octet_shift--;
    /* More than 8 bits remaining so shift the next byte in */
    *data_ptr++ = (uint8) (int_value >> ((octet_shift*8) + bit_shift)); 
  }

  if (bit_shift > 0) 
  {
    /* Get the last n bits  - These bits ( in the last byte ) will reside in
       the upper bits of the byte - so do left shift by (8 - bit_shift) */
    *data_ptr++ = 
      (uint8) ((int_value % (uint32)(1 << bit_shift)) << (uint8)(8 - bit_shift));   
  }

  return TRUE;
} /*  */
