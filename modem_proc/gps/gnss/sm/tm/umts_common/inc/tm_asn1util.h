#ifndef LSMASN1UTIL_H
#define LSMASN1UTIL_H
/*===========================================================================
                          TM_ASN1UTIL.H

DESCRIPTION
  This header file contains function prototypes for the functions in 
  tm_asn1util.h.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

   Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/inc/tm_asn1util.h#1 $
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
#include "comdef.h"
#include "tm_asn1_common.h"
#include "tm_osysinit.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* must be a value not claimed by rtxErrCode.h */
#define C_ASN1_DECODE_BUFFER_NOT_ALLOCATED 100   

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*! @brief This structure is used to get ASN.1 encoding status when there is any
encoding failure
*/
typedef struct
{
  int32  encode_status;      /*!< Encoding failure cause       */ 
  int32  pdu_num;            /*!< PDU on which encoding failed */
  void   *unencoded_pdu_ptr;  /*!< Unencoded pdu ptr            */
} tm_osys_encode_info_s;

/*! @brief This structure is used to get ASN.1 decoding status when there is any
decoding failure
*/
typedef struct
{
  int32  decode_status;      /*!< Decoding failure cause       */ 
  int32  pdu_num;            /*!< PDU on which Decoding failed */
  int32  pdu_size;           /*!< Size of the encoded string   */
  void   *encoded_pdu_ptr;    /*!< ptr to encoded string        */
  int32  decode_length;      /*!< Decoded length incase pre-allocated
                                   decode buffer is used        */
} tm_osys_decode_info_s;


/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  uint8 placeholder;
} tm_osys_asn1util_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  /*! Run-time context structure.  This structure is a container structure 
      that holds all working variables involved in encoding or decoding a
      message
  */
  OSCTXT tm_asn1_ctxt; 

  /*! @brief Used to get ASN.1 encoding status in case of encoding failure
  */
  tm_osys_encode_info_s enc_info;

  /*! @brief Used to get ASN.1 decoding status in case of decoding failure
  */
  tm_osys_decode_info_s dec_info;


} tm_osys_asn1util_dynamic_data_s;

/*! @brief ASN1Util Private data sturctured definition
*/
typedef struct
{
  /*!< Static part of private data */
  //tm_osys_asn1util_static_data_s *sd_ptr;
  
  /*!< Dynamic part of private data */
  tm_osys_asn1util_dynamic_data_s *dd_ptr;
  
}tm_osys_asn1util_s;



void tm_asn1_osys_printf
(
  void *stream, const char *format, va_list args 
);


int tm_osys_asn1_encode_pdu ( void      *unencoded_pdu_ptr,
                              tm_pdu_buf_type *pdu_buf_ptr,
                              int pdu_num );
                              
                              
void tm_osys_asn1_decode_pdu(void *encoded_pdu_ptr,                         
                              size_t encoded_pdu_size,
                              void *p_pdu_decoded,
                              int pdu_num,
                              int *decode_status_ptr);
                              
void tm_asn1_free_osys_buf
(
  void *buf, int pdu_num  
);

void tm_osys_asn1_free_pdu
(
  void *buf, int pdu_num  
);

void tm_asn1_err_print(int pdu_num);


#endif /* LSMASN1UTIL_H */
