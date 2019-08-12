/*============================================================================
  @file gnss_serdes.c

                  GNSS Serialization/Deserialization Module

GENERAL DESCRIPTION

  This file contains routines that can be used to SERIALIZE or DESERIALIZE
  a data structure

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gnss_serdes.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/10   kgj     Original

=============================================================================*/


/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "comdef.h"
#include "amssassert.h"
#include "gnss_serdes.h"


#define GNSS_ASSERT(assertion)  ASSERT(assertion)


/* ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 *                          PUBLIC FUNCTIONS
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 *                          gnss_serialize
 * ---------------------------------------------------------------------------*/
/**
@brief This function serializes a data structure

@param p_scattered [in] scattered memory descriptor table

@param w_numScattered [in] number of primitives in the p_scatterd array

@param p_serialized [out] serialization buffer

@param w_serialSz [in] size of serialization buffer

@param b_serializedLittleEndian [in] TRUE if serial data is little endian

@return
    Returns the number of bytes serialized into the p_serialized buffer
*/
uint16 gnss_serialize(
  const gnss_serializePrimitiveT *  p_scattered,
  uint16                            w_numScattered,
  void *                            p_serialized,
  uint16                            w_serialSz,
  boolean                           b_serializeLittleEndian
)
{
  int             i;
  uint8 *         p_dst = (uint8 *) p_serialized;
  const uint8 *   p_src;
  uint32          q_sz;
  uint32          q_cnt;

  GNSS_ASSERT(p_scattered);
  GNSS_ASSERT(p_serialized);
  GNSS_ASSERT(b_serializeLittleEndian);

  /*
   *  Count the number of bytes to be copied according to the scatter table
   *  IF the serial buffer is not large enough to hold ALL of the bytes THEN
   *    return 0 to the caller (indicating an error)
   *  ENDIF
   */
  q_cnt = 0;
  for( i=0; i<w_numScattered; i++ )
  {
    q_sz = (uint32) p_scattered[i].sz;
    q_sz *= p_scattered[i].cnt;

    q_cnt += q_sz;
  }
 
  if( q_cnt > (uint32) w_serialSz )
  {
    return 0;
  }

  /*
   *  Scatter-copy the bytes now (ignore endianness for now)
   */
  for( i=0; i<w_numScattered; i++ )
  {
    p_src = p_scattered[i].p_primitive;
    q_sz  = p_scattered[i].sz;
    q_sz *= p_scattered[i].cnt ;

    while( q_sz > 0 )
    {
      *p_dst++ = *p_src++;
      q_sz--;
    }
  }

  return (uint16) q_cnt;
}



/* ---------------------------------------------------------------------------
 *                          gnss_deserialize
 * ---------------------------------------------------------------------------*/
/**
@brief This function deserializes a data structure

@param p_serialized [in] serialization buffer

@param w_serialSz [in] number of bytes in the serialization buffer

@param p_scattered [out] scattered memory descriptor table

@param w_numScattered [in] number of primitives in the p_scatterd array

@param b_serializedLittleEndian [in] TRUE if serial data is little endian

@return
    Returns the number of bytes deserialized from the p_serialized buffer
*/
uint16 gnss_deserialize(
  const void *                        p_serialized,
  uint16                              w_serialSz,
  const gnss_deserializePrimitiveT *  p_scattered,
  uint16                              w_numScattered,
  boolean                             b_serializeLittleEndian
)
{
  int             i;
  const uint8 *   p_src = (uint8 *) p_serialized;
  uint8 *         p_dst;
  uint32          q_sz;
  uint32          q_cnt;

  GNSS_ASSERT(p_scattered);
  GNSS_ASSERT(p_serialized);
  GNSS_ASSERT(b_serializeLittleEndian);

  /*
   *  Count the number of bytes to be copied according to the scatter table
   *  IF the serial buffer is not contain enough bytes THEN
   *    return 0 to the caller (indicating an error)
   *  ENDIF
   */
  q_cnt = 0;
  for( i=0; i<w_numScattered; i++ )
  {
    q_sz = (uint32) p_scattered[i].sz;
    q_sz *= p_scattered[i].cnt;

    q_cnt += q_sz;
  }
  
  if( q_cnt > (uint32) w_serialSz )
  {
    return 0;
  }

  /*
   *  Scatter-copy the bytes now (ignore endianness for now)
   */
  for( i=0; i<w_numScattered; i++ )
  {
    p_dst = p_scattered[i].p_primitive;
    q_sz  = p_scattered[i].sz;
    q_sz *= p_scattered[i].cnt ;

    while( q_sz > 0 )
    {
      *p_dst++ = *p_src++;
      q_sz--;
    }
  }

  return (uint16) q_cnt;
}
