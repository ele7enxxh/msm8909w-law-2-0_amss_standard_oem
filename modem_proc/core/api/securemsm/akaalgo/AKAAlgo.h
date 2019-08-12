#ifndef AKAALGO_H
#define AKAALGO_H

/**
@file AKAAlgo.h 
@brief AKA Core APIs.\ This file contains the definitions of the constants, data structures and  
interfaces that provide the Authentication and Key Agreement (AKA) mechanism. 
*/

/*===========================================================================
  Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY 

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/akaalgo/AKAAlgo.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
06/23/10   ds/sd   (Tech Pubs) Edited Doxygen comments.
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
                
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
@addtogroup akaalgo_core_apis 
@{
*/

/**
@name AKAALGO Request Status
The following constants are grouped as Authentication Request processing status types.
@{
*/
#define  AKAALGO_SUCCESS    	(0) /**< AKA was successful. */
#define  AKAALGO_ERR_SQN       	(1) /**< SQN freshness test failure. */
#define  AKAALGO_ERR_MACA       (2) /**< MAC-A validation failure (invalid AUTN). */
#define  AKAALGO_STATUS_NONE    (0xFF) /**< Initial status. */
/** @} */  /* end_name_AKAALGO Request Status */

/**
@name AKA Alogorithm Type
@{
*/
#define  AKAALGO_ALGO_SHA1      	(0x0001) /**< SHA1-based AKA algorithm set. */
#define  AKAALGO_ALGO_MILENAGE 		(0x0002) /**< MILENAGE-based AKA algorithm set. */
/** @} */  /* end_name_AKA Alogorithm Type */

/**
@name Other Parameters 
@{
*/
#define L_KEY              (16)    /**< Sets the L key to 128 bits per 3G 
                                        TS 33.105.v.3.0(2000-03). */
#define SEQ_MS_ARR_SIZE_MAX   (32) /**< Recommended in 3GPP TS 33.102. */
/** @} */  /* end_name_Other parameters */

/** @} */  /* end_addtogroup akaalgo_core_apis */
 

/**
@addtogroup akaalgo_core_apis 
   @{
*/

/*=============================================================================

Function: AKAAlgo_Init

=============================================================================*/

/**
Initializes the AKAAlgo library and sets the configuration parameter. 
This function is used to set the Algorithm type and the operator-specific 
configuration parameter. It should be called before AKAAlgo_Perform_AKA().

@param[in] algo_id   Supports two types of algorithms that are defined as the 
                     following macros:
   - #define AKAALGO_ALGO_SHA1 (0x0001)
                     - #define AKAALGO_ALGO_MILENAGE (0x0002).
@param[in] op_ptr    Operator-specific configuration parameter.
@param[in] op_len    Length of the operation.

@return
None.

  @dependencies
- The op_ptr parameter should be pointing to a buffer of at least 16 bytes
   if AKAALGO_MILENAGE is used.
- The op_ptr parameter is ignored when AKAALGO_ALGO_SHA1 is used and can be NULL.
*/
void AKAAlgo_Init 
( 
  uint32   algo_id,                              
  uint8*   op_ptr,       
  uint32   op_len        
);

/*=============================================================================

Function: AKAAlgo_Perform_AKA

=============================================================================*/

/**
  Performs the AKA procedure.
This function performs the AKA procedure by validating the AUTN, testing the 
SQN freshness, and generating the CK and IK.
 
@param[in] auth_req_ptr Authentication request buffer AUTN.\n
@verbatim
Length in bytes  6              2    8  
Field            Concealed SQN  AMF  MAC 
@endverbatim
 
@param[in] rand_ptr     Random challenge (RAND). \n
@verbatim
Length in bytes  16  
Field            RAND 
@endverbatim
 
@param[out] resp_ptr    AUTS or Auth response buffer.
                        The AKA processing status is included in the first byte 
                        of the resp_ptr. \n
@verbatim 
AKAALGO_ERR_MACA
If validation of AUTN fails, the resp_ptr will be: 
Length in bytes  1   
Field            Status = AKAALGO_ERR_MACA  

AKAALGO_ERR_SQN
If there is a synchronization error, the resp_ptr will be: 
Length in bytes  1                         6                 8   
Field            Status = AKAALGO_ERR_SQN  Concealed SQN-MS  MAC-S  

AKAALGO_SUCCESS
If AKA processing succeeds, the resp_ptr will be: 
Length in bytes  1                         1        8 or 16  1       16  1       16  
Field            Status = AKAALGO_SUCCESS  RES_LEN  RES      CK_LEN  CK  IK_LEN  IK  
@endverbatim 

@note The RES field will be 8 bytes in length for 
MILENAGE (TS 35.206.v8.0.0.0(2008-12)) and 16 bytes for 3GPP2 AKA 
(TS 33.105.v.3.0(2000-03)).

@return
None.

@dependencies
-# Call AKAAlgo_Set_Subscriber_Key to set key K.
-# Call AKAAlgo_Set_SEQ to load the array of stored sequence numbers.
-# Call AKAAlgo_Get_SEQ to store the updated array of sequence numbers 
if the AKA processing status is AKAALGO_SUCCESS.
*/
void AKAAlgo_Perform_AKA
(
  uint8*  auth_req_ptr,   
  uint8*  rand_ptr,       
  uint8*  resp_ptr        
);

/*=============================================================================

Function: AKAAlgo_Set_Subscriber_Key

=============================================================================*/

/**
Sets the subscriber key.
This function sets the subscriber key.

@param[in] sub_key Subscriber key.

@param[in] key_len Key length.
                   The key_len parameter is of length L_KEY.
       
@return
None.

@dependencies
  None.
*/
void AKAAlgo_Set_Subscriber_Key
(
  uint8* sub_key,        
  uint8  key_len         
);

/*=============================================================================
 
Function: AKAAlgo_Set_SEQ
 
=============================================================================*/
/**
Sets the array of previously accepted sequence numbers.
This function sets the array of previously accepted sequence numbers.
The first SEQ_MS_ARR_SIZE_MAX elements of the array are copied into working 
buffers.

@param[in] seq_array_ptr   Pointer to the array containing previously accepted 
                           sequence numbers.
 
@param[in] seq_array_size  Size of the above array.
                           The seq_array_size parameter must be at least 
                           SEQ_MS_ARR_SIZE_MAX in size.
  
@return
None.
      
@dependencies
  None.
*/
void AKAAlgo_Set_SEQ
(
  uint64* seq_array_ptr, 
  uint32  seq_array_size 
);

/*=============================================================================

Function: AKAAlgo_Get_SEQ
      
=============================================================================*/

/**
Gets the updated array of sequence numbers.
This function is used to get the updated array of sequence numbers. This 
function is called after a successful AKA operation so that the updated 
sequence numbers can be stored in non-volatile memory.

@param[out] seq_array_ptr  Pointer to the array that can hold the updated 
                           sequence numbers.

@param[in] seq_array_size  Size of the above array.
                           The SEQ_MS_ARR_SIZE_MAX number of sequence numbers 
                           are copied so the size must be at least 
                           SEQ_MS_ARR_SIZE_MAX.

@return
None.

@dependencies
AKAAlgo_Perform_AKA() must be called earlier, and the status must be 
AKAALGO_SUCCESS.
*/
void AKAAlgo_Get_SEQ
(
  uint64*  seq_array_ptr,
  uint32   seq_array_size
);

/** @} */  /* end_addtogroup_akaalgo_core_apis */  /*  End of API */

#ifdef __cplusplus
}
#endif // __cplusplus



#endif /* AKAALGO_H */
