#ifndef __GNSS_SERDES_H__
#define __GNSS_SERDES_H__

/*============================================================================
  @file gnss_serdes.h

                  GNSS Serialization/Deserialization Module

GENERAL DESCRIPTION

  This module can be used to SERIALIZE or DESERIALIZE a data structure

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gnss_serdes.h#1 $
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

/* ---------------------------------------------------------------------------
 * Data Types
 * ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 *  SERDES is short for SERIALIZE/DESERIALIZE.
 *
 *  The following set of data structures is used to create
 *  SERDES descriptor tables.  These tables are used by serialization
 *  and deserialization methods to serialize/deserialize data structures.
 *
 *  The gnss_serdesPrimitiveT structure describes a "primitive".  A "primitive"
 *  describes a "C" language primitive like uint8, int16, float,
 *  double, etc.   It contains a pointer to the "C" language primitive,
 *  the size of the primitive (in bytes), and a count of the number 
 *  of primitives pointed to by the pointer (in the case of an array
 *  of primitives). 
 *
 *  The gnss_serializePrimitiveT structure is just a renamed version
 *  of the gnss_serdesPrimitiveT structure.  Both have a const
 *  pointer to the "unpacked" data item location, which allows for
 *  serialization but not deserialization
 *
 *  The gnss_deserializePrimitiveT structure is identical to the
 *  other 2 already mentioned, except that its data pointer is
 *  a non-const pointer which allows for deserialization
 *
 *  The gnss_serdesSerializeTblT structure describes an array of 
 *  serialization primitives.
 * 
 *  The gnss_serdesDeserializeTblT structure describes an array of 
 *  deserialization primitives.
 * 
 *  The gnss_serdesUnionT is a union of all 5 of the structures
 *  listed above. 
 *
 *  It is VITAL that these 5 structures overlay on top of each other
 *  in a convenient way, because software will be cleverly designed
 *  to allow mixing of Primitive items and lists of Primitive items.
 *  In particular, the "sz" field is "special".  It is used to
 *  identify the difference between a "primitive" structure and
 *  a "primitive Table" structure.  The "sz" field must be non-zero
 *  for a "primitive" structure.  The "sz" field must be 0 for
 *  a "primitive Table" structure.
 *
 *  DO NOT change the format of these structures without EXTREME CARE!
 *  ANY changes would require serious analysis of code that uses
 *  these structures.  Most importantly, all 5 structures must
 *  meet the following requirements:
 *    1) The first field must be a pointer
 *    2) The second field must be a 16-bit "sz" field or "reserved"
 *    3) The third field must be a 16-bit "cnt" or "w_numPrimitives"
 ---------------------------------------------------------------------------*/
typedef struct
{
  const void *  p_primitive;  /* Pointer to primitive */
  uint16  sz;                 /* Size of primitive (in bytes) */
  uint16  cnt;                /* Number of primitives (must be >= 1) */
} gnss_serdesPrimitiveT;

typedef gnss_serdesPrimitiveT gnss_serializePrimitiveT;

typedef struct
{
  void *  p_primitive;        /* Pointer to non-serialized primitive */
  uint16  sz;                 /* Size of primitive (in bytes) */
  uint16  cnt;                /* Number of primitives (must be >= 1) */
} gnss_deserializePrimitiveT;

typedef struct
{
  /* Pointer to an array of serialization primitives */
  const gnss_serializePrimitiveT *  p_primitiveArray;
    
  uint16        w_reserved;         /* Reserved Field - set to 0 */
  uint16        w_numPrimitives;    /* primitiveArray length */
} gnss_serdesSerializeTblT;

typedef struct
{
  /* Pointer to an array of deserialization primitives */
  const gnss_deserializePrimitiveT *  p_primitiveArray;
    
  uint16        w_reserved;         /* Reserved Field - set to 0 */
  uint16        w_numPrimitives;    /* primitiveArray length */
} gnss_serdesDeserializeTblT;

typedef union
{
  gnss_serdesPrimitiveT         primitive;
  gnss_deserializePrimitiveT    deserializePrimitive;
  gnss_serdesDeserializeTblT    deserializeTbl;
  gnss_serializePrimitiveT      serializePrimitive;
  gnss_serdesSerializeTblT      serializeTbl;
} gnss_serdesUnionT;


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
);


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
);

#endif  /* __GNSS_SERDES_H__ */
