#ifndef AKAALGOMILENAGE_H
#define AKAALGOMILENAGE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef unsigned char u8;

#define MILENAGE_OP_LEN		16

/*--------------------------- prototypes --------------------------*/
void milenage_init ( u8* op_ptr, uint32 op_len );
void milenage_f1 ( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2], u8 mac_a[8] );
void milenage_f2345 ( u8 k[16], u8 rand[16], u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6] );
void milenage_f1star( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2], u8 mac_s[8] );
void milenage_f5star( u8 k[16], u8 rand[16], u8 ak[6] );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //#define AKAALGOMILENAGE_H

