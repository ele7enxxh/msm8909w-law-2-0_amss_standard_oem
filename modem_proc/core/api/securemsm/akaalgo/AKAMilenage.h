#ifndef AKAMILENAGE_H
#define AKAMILENAGE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef unsigned char u8;

#define MILENAGE_OP_LEN		16


/*=============================================================================

Function: AKA_milenage_f1

=============================================================================*/

/**
  Algorithm f1.
Computes network authentication code MAC-A from key K, random
challenge RAND, sequence number SQN and authentication management
field AMF.
 
@param[in] K		128 bits. Subscriber key \n
@param[in] RAND		128 bits. Random challenge (RAND). \n
@param[in] SQN		48 bits. Sequence number. \n
@param[in] AMF		16 bits. Authentication management field. \n
@param[out] MAC-A	64 bits. network authentication code. \n
@param[in] OP		128 bits. Operator Variant Algorithm Configuration Field. \n 

@return
None.

@dependencies
None
*/
extern void AKA_milenage_f1 ( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2], u8 mac_a[8], u8 op[16] );

/*=============================================================================

Function: AKA_milenage_f2345

=============================================================================*/

/**
  Algorithm f2345.
  Takes key K and random challenge RAND, and returns response RES,
  confidentiality key CK, integrity key IK and anonymity key AK.
 
@param[in] K		128 bits. Subscriber key \n
@param[in] RAND		128 bits. Random challenge (RAND). \n
@param[out] RES		48 bits. Sequence number. \n
@param[out] CK		128 bits. Authentication management field. \n
@param[out] IK  	128 bits. network authentication code. \n
@param[out] AK  	48 bits. Anonymity key. \n
@param[in] OP		128 bits. Operator Variant Algorithm Configuration Field. \n 
@param[in] encrypt_xor	indicate use OP or OPc (encrypted OP) as components of f function. \n 

@return
None.

@dependencies
None
*/
extern void AKA_milenage_f2345 ( u8 k[16], u8 rand[16], u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6], u8 op[16], boolean encrypt_xor );

/*=============================================================================

Function: AKA_milenage_f1star

=============================================================================*/
/**
  Algorithm f1*.
Computes resynch authentication code MAC-S from key K, random
challenge RAND, sequence number SQN and authentication management
field AMF.
 
@param[in] K		128 bits. Subscriber key \n
@param[in] RAND		128 bits. Random challenge (RAND). \n
@param[in] SQN		48 bits. Sequence number. \n
@param[in] AMF		16 bits. Authentication management field. \n
@param[out] MAC-S	64 bits. network authentication code. \n
@param[in] OP		128 bits. Operator Variant Algorithm Configuration Field. \n 

@return
None.

@dependencies
None
*/
extern void AKA_milenage_f1star( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2], u8 mac_s[8], u8 op[16] );


/*=============================================================================

Function: AKA_milenage_f5star

=============================================================================*/

/**
  Algorithm f5*.
  Takes key K and random challenge RAND, and returns resynch anonymity key AK.
 
@param[in] K		128 bits. Subscriber key \n
@param[in] RAND		128 bits. Random challenge (RAND). \n
@param[out] AK  	48 bits. Anonymity key. \n
@param[in] OP		128 bits. Operator Variant Algorithm Configuration Field. \n 

@return
None.

@dependencies
None
*/
extern void AKA_milenage_f5star( u8 k[16], u8 rand[16], u8 ak[6], u8 op[16] );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //#define AKAMILENAGE_H

