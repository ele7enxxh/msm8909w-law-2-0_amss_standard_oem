#ifndef AKAALGOKEYGEN_H
#define AKAALGOKEYGEN_H

/** 
  @file AKAAlgoKeyGen.h
  @brief SHA1-based f functions. 
  This file contains the definitions of the constants, data structures, and 
  interfaces that provide the SHA1-based Authentication and Key Agreement (AKA) 
  algorithm. 
 */

/*===========================================================================
  Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/akaalgo/AKAAlgoKeyGen.h#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/10   ds/sd   (Tech Pubs) Edited Doxygen markup and comments.
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.

===========================================================================*/



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "comdef.h"



/**
   @addtogroup akaalgo_SHA1_crypto_apis 
   @{
*/

/*=============================================================================

Function: f0

=============================================================================*/
/**
  Generates RAND values to be used in authentication vectors. 

  The f0 function is a pseudo random generator algorithm that is probably as 
  difficult as Secure Hash Algorithm 1 (SHA-1). The function is presented with
  the random secret seed and a counter parameter. The random secret seed is 
  chosen by the operator. The counter parameter is initialized to 0 at the 
  Authentication Center (AuC) once and is incremented every time the function is 
  called. The procedure returns 64 pseudo random bits every time it is invoked. 
  The calling process is responsible for incrementing the counter and repeatedly 
  calling the function in order to generate the required number of pseudo random 
  bits. 

  @param[in] seed[]  Seed for generating random number.
  @param[in] fi      Type identifier.
  @param[in] Fmk[]   Family key.
  @param[in] buff[]  Random number.

  @return
   None.

  @dependencies
   None.
*/
void f0(uint8 seed[],uint8 fi,uint8 Fmk[],uint8 buff[]);

/*=============================================================================

Function: f1

=============================================================================*/
/**
  Network authentication function. 

  The f1 function is used to compute the message authentication code (MAC) that 
  authenticates a 19 authentication vector to the universal input method (UIM).
     @code
   f1: 	(K; SQN, RAND, AMF) -> MAC-A (or XMAC-A)
     @endcode
  f1 is a MAC function. It is computationally infeasible to derive K from 
  knowledge of RAND, SQN, AMF, and MAC-A (or XMAC-A).

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ..., K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber authentication
                     key K is a long-term secret key stored in the USIM and AuC.
  @param[in] fi      Type identifier.

  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[in] SQN[]   Sequence number. \n
                     @code
                     SQN[0], SQN[1], ... SQN[47]
                     @endcode
                     The length of SQN is 48 bits. The AuC must include a 
                     fresh sequence number in each authentication token. The 
                     verification of the freshness of the sequence number by
                     the USIM constitutes authentication of the network to the 
                     user.

  @param[in] AMF[]   Authentication management field. \n
                     @code
                     AMF[0], AMF[1], ... AMF[15] 
                     @endcode
                     The length of AMF is 16 bits. The use of AMF is not 
                     standardized. Example uses of the AMF are provided in 
                     Annex F of TS 33.102.

  @param[out] MACA[] Message authentication code used for authentication
                     of the network to the user. \n
                     @code
                     MAC-A[0], MAC-A[1], ... MAC-A[63] 
                     @endcode
                     The length of MAC-A is 64 bits. MAC-A authenticates the 
                     data integrity and the data origin of RAND, SQN, and AMF.
                     The verification of MAC-A by the USIM constitutes to 
                     entity authentication of the network to the user.

  @return
   None.

  @dependencies
   None.
*/
void f1(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 SQN[],uint8 AMF[],uint8 MACA[]);

/*=============================================================================

Function: f1star

=============================================================================*/
/**
  Re-synchronization message authentication function. 

  The f1* function is used to compute the MAC that authenticates a 
resynchronization message to the Authentication Center.
     @code
   f1*:	(K; SQN, RAND, AMF) -> MAC-S (or XMAC-S)
     @endcode
  f1* is a MAC function. It is computationally infeasible to 
  derive K from knowledge of RAND, SQN, AMF and MAC-S (or XMAC-S).

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber authentication
                     key K is a long-term secret key stored in the USIM and the 
                     AuC.

  @param[in] fi      Type identifier.

  @param[in] *RAND[] Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[in] SQN[]   Sequence number. \n
                     @code
                     SQN[0], SQN[1], ... SQN[47] 
                     @endcode
                     The length of SQN is 48 bits. The AuC should include a 
                     fresh sequence number in each authentication token. The 
                     verification of the freshness of the sequence number by 
                     the USIM constitutes to entity authentication of the 
                     network to the user.

  @param[in] AMF[]   Authentication management field. \n
                     @code
                     AMF[0], AMF[1], ... AMF[15] 
                     @endcode
                     The length of AMF is 16 bits. The use of AMF is not 
                     standardized. Example uses of the AMF are provided in 
                     Annex F of TS 33.102.

  @param[out] MACA[] Message authentication code used for authentication of the 
                     network to the user. \n
                     @code
                     MAC-A[0], MAC-A[1], ... MAC-A[63] 
                     @endcode
                     The length of MAC-A is 64 bits. MAC-A authenticates the 
                     data integrity and the data origin of RAND, SQN and AMF. 
                     The verification of MAC-A by the USIM constitutes to 
                     entity authentication of the network to the user.

  @return
   None.

  @dependencies
   None.
*/
void f1star(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 SQN[],uint8 AMF[],uint8 MACA[]);

/*=============================================================================

Function: f2

=============================================================================*/
/**
  User authentication function. 

  The f2 function is used to compute the challenge response returned from the UIM 
  when an authentication vector is processed. 
     @code
   f2: 		(K; RAND)-> RES (or XRES)
     @endcode
  f2 is a MAC function. It is computationally infeasible to derive K from 
  knowledge of RAND and RES (or XRES).

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber 
                     authentication key K is a long term secret key stored 
                     in the USIM and the AuC.

  @param[in] fi      Type identifier.

  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.
  @param[out] RES[]  User response. \n
                     @code
                     RES[0], RES[1], ... RES[n 1] 
                     @endcode
                     The length n of RES and XRES is at most 128 bits and at 
                     least 32 bits, and shall be a multiple of 8 bits. RES 
                     and XRES constitute to entity authentication of the user 
  to the network.

  @param[in] l_res[] Length of RES.

  @return
   None.

  @dependencies
   None.
*/
void f2(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 RES[],int l_res);

/*=============================================================================

Function: f3

=============================================================================*/
/**
  Cipher key (CK) derivation function. 

  The f3 function is a pseudo random function used to generate a ciphering key. 
  The output can be used as the CK key in an AKA authentication vector, or for 
  other purposes.
     @code
   f3: 		(K; RAND) -> CK
     @endcode
  f3 is a key derivation function. It is computationally infeasible to derive K 
  from knowledge of RAND and CK.

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber 
                     authentication key K is a long term secret key stored in 
                     the USIM and the AuC.

  @param[in] fi      Type identifier. 
 
  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[out] CK[]   Cipher key. \n
                     @code
                     CK[0], CK[1], ... CK[127] 
                     @endcode
                     The length of CK is 128 bits. If the effective key length
                     is to be smaller than 128 bits, the most significant bits 
                     of CK will carry the effective key information, whereas 
                     the remaining least significant bits will be set to zero.

  @return
  None.

  @dependencies
   None.
*/
void f3(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 *CK);

/*=============================================================================

Function: f4

=============================================================================*/
/**
  Integrity key derivation function. 

  The f4 function is a pseudo random function used to generate the integrity key 
  (IK). 
     @code
   f4: 		(K; RAND) -> IK
     @endcode
  f4 is a key derivation function. It is computationally infeasible to derive
  K from knowledge of RAND and IK.

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber authentication
                     key K is a long-term secret key stored in the USIM and 
                     the AuC.
 
  @param[in] fi      Type identifier.
   
  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[out] IK[]   Integrity key. \n
                     @code
                     IK[0], IK[1], ... IK[127] 
                     @endcode
                     The length of IK is 128 bits. If the effective key 
                     length is to be smaller than 128 bits, the 
                     most significant bits of IK will carry the effective key
                     information, whereas the remaining least significant 
                     bits will be set zero.

  @return
   None.

  @dependencies
   None.
*/
void f4(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 *IK);

/*=============================================================================

Function: f5

=============================================================================*/
/**
  Anonymity key (AK) derivation function for normal operation. 

  The f5 function is used to compute the AK, which protects the 
  authentication vector sequence numbers.
     @code
   f5: 		(K; RAND) -> AK
     @endcode
  f5 is a key derivation function. It is computationally infeasible to derive K 
  from knowledge of RAND and AK. The use of f5 is optional. 

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber authentication
                     key K is a long term secret key stored in the USIM and the 
                     AuC.
 
  @param[in] fi      Type identifier.
   
  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[out] AK[]   Anonymity key. \n
                     @code
                     AK[0], AK[1], ... AK[47] 
                     @endcode
   The length of AK is 48 bits. It equals the length of SQN.

  @return
   None.

  @dependencies
   None.
*/
void f5(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 AK[]);	

/*=============================================================================

Function: f5star

=============================================================================*/
/**
  AK derivation function for re-synchronization. 

  The f5* function is used to compute an AK, which protects authentication 
  vector sequence numbers in the resynchronization message. 
     @code
	f5*: (K; RAND) -> AK
     @endcode
   f5* is a key derivation function. It is computationally
   infeasible to derive K from knowledge of RAND and AK.
   The use of f5* is optional.

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber authentication
                     key K is a long-term secret key stored in the USIM and the 
                     AuC.
 
  @param[in] fi      Type identifier.
   
  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[out] AKS[]  Anonymity key. \n
                     @code
                     AK[0], AK[1], ... AK[47] 
                     @endcode
   The length of AK is 48 bits. It equals the length of SQN.

  @return
   None.

  @dependencies
   None.
*/
void f5star(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 AKS[]);

/*=============================================================================

Function: f11

=============================================================================*/
/**
  Universal message authentication code (UMAC) key generating function. 

  The f11 function is used to compute the UIM authentication key, which protects 
  against rogue shell attacks. Since UAK resides in the UIM, it can be used to 
  ensure that the UIM is present in subsequent MAC operations.
  The function f11 must be called prior to calling the UMAC_Generation function 
  (see 2.1.2.2 of S.S0078).

  @param[in] K[]     Subscriber authentication key. \n
                     @code
                     K[0], K[1], ... K[127] 
                     @endcode
                     The length of K is 128 bits. The subscriber authentication
                     key K is a long-term secret key stored in the USIM and the 
                     AuC.
 
  @param[in] fi      Type identifier.
   
  @param[in] RAND[]  Random challenge. \n
                     @code
                     RAND[0], RAND[1], ... RAND[127] 
                     @endcode
   The length of RAND is 128 bits.

  @param[in] Fmk[]   Family key.

  @param[in] *UAK    UMAC key.

  @return
   None.

  @dependencies
   None.
*/
void f11(uint8 K[],uint8 fi,uint8 *RAND,uint8 Fmk[],uint8 *UAK);

/** @} */  /* end_addtogroup akaalgo_SHA1_crypto_apis */ /*  End of API */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* AKAALGOKEYGEN_H */
