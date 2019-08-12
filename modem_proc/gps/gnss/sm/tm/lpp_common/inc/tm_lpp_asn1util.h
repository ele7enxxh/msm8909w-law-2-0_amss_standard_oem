#ifndef TM_LPP_ASN1UTIL_H
#define TM_LPP_ASN1UTIL_H
/*===========================================================================
                          TM_LPP_ASN1UTIL.H

DESCRIPTION
  This header file contains function prototypes for the functions in 
  tm_asn1util.h.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/inc/tm_lpp_asn1util.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $  
  
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/5/2011  lt      Initial OSYS based version.
  
  
  
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "gps_variation.h"
#include "comdef.h"
#include "tm_osysinit.h"
#include "tm_asn1util.h"


#include "tm_supl_asn1.h"
#include "tm_lpp_asn1.h"

/*===========================================================================
  tm_lpp_asn1_osys_print_char
    
  Description:
    This function accumulates characters until it has a complete line, then it
    prints the line.

  Return
    None
===========================================================================*/
void tm_lpp_asn1_osys_print_char
(
  char ch /*!< char to print out */
);


/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_printf

  Description 
    This is the printf() function for objective systems use.

  Returns:
    None
===========================================================================*/
void tm_lpp_asn1_osys_printf
(
  void *stream, const char *format, va_list args 
);




/*===========================================================================
  tm_lpp_osys_asn1_encode_pdu
    
  Description:
    This function ASN.1 encodes a passed signalling message. 
    This function returns a pdu_buf_type via an output parameter. If encoding
    failed, pdu_buf.value will be NULL and pdu_buf.length is meaningless. If
    encoding succeeded, pdu_buf.value will be non-NULL and pdu_buf.length will
    contain the number of bytes in the encoded PDU.

    
   Parameters:
    unencoded_pdu_ptr - Pointer to the unencoded PDU
    encoded_pdu - This is a variable of type pdu_buf_type and needs to
      be declared by the caller and passed here as a parameter. If the
      encoding operation succeeds, then the value field of this structure will
      point to the encoded PDU and the length field will indicate the size of
      the encoded PDU in bytes
    param pdu_num - Type of the message being decoded. 
 
    Returns:

    int - status of the encode operation
=========================================================================*/

int tm_lpp_osys_asn1_encode_pdu ( void      *unencoded_pdu_ptr,
                              tm_pdu_buf_type *pdu_buf_ptr);


/* ===========================================================================
  tm_lpp_osys_asn1_decode_pdu
 
  Description: 
    This function ASN.1 decodes a passed signalling message. 

    The pointer to decoded PDU is set to NULL if an error occurs, otherwise
    points to the decoded PDU
 
 
  Parameters 
    encoded_pdu_ptr - Pointer to the encoded PDU
    encoded_pdu_size - Size in bytes of the encoded message
    decoded_pdu_ptr - A NULL pointer declared to be type of the
      message being decoded, and must be passed in by the caller
    pdu_num - Type of the message being decoded. Must be one of
      the 'OSYS PDU Types' defined in lte_rrc_osys_asn1util.h
 
  Returns: 
    int - status of the decode operation
===========================================================================*/
void  tm_lpp_osys_asn1_decode_pdu(void *encoded_pdu_ptr,                         
                              size_t encoded_pdu_size,
                              void *p_pdu_decoded,
                              int *decode_status_ptr);



/*===========================================================================

  FUNCTION:  tm_osys_asn1_free_buf

  Description:
    This function frees memory that was allocated by
    tm_osys_asn1_encode_pdu()

 
  Returns: 
    None
===========================================================================*/
void tm_lpp_asn1_free_osys_buf
(
  void *buf
);
  


/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_IE_free

  Description:

    This function is called by the Objective Systems ASN.1 libraries in order
    to free dynamically allocated memory for individual elements within the PDU

  Returns:
    void
===========================================================================*/
void tm_lpp_asn1_osys_IE_free
(
  void **ptr    /*!< Pointer to memory location to be freed */
);


/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_IE_realloc

  Description
    This function is called by the Objective Systems ASN.1 libraries in order
    to allocate dynamic memory for individual elements within the PDU while a
    PDU is being constructed by hand.

  Returns:
    void
=========================================================================*/
void * tm_lpp_asn1_osys_IE_realloc
(
  void *ptr,    /*!< Pointer to memory location to reallocate */
  size_t size   /*!< Size in bytes to be allocated */
);

/*===========================================================================

  FUNCTION:  tm_lpp_asn1_osys_IE_malloc

  Descrpition:
    This function is called by the Objective Systems ASN.1 libraries in order
    to allocate dynamic memory for individual elements within the PDU while a
    PDU is being constructed by hand.

  Returns:
    void
=========================================================================*/
void * tm_lpp_asn1_osys_IE_malloc
(
  size_t size   /*!< Size in bytes to be allocated */
);


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
);

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
);

void tm_asn1_free_lpp_message_IEs(LPP_Message *p_lpp_pdu, int pdu_num);

void tm_lpp_asn1_osys_alloc_lpppayload_array(supl_Ver2_PosPayLoad_extension_lPPPayload *elem);

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
);

void tm_lpp_init_encoding_context(void);


#endif /* LSMASN1UTIL_H */
