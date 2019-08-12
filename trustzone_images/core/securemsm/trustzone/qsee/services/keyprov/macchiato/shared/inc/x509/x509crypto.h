#ifndef X509_CRYPTO_H
#define X509_CRYPTO_H
#include "secx509.h"

secerrno_enum_type CalcSignature
(
secx509_cert_type*  cert_ptr,
uint8*              cert_data
);

secerrno_enum_type VerifySignature
(
secx509_cert_type*  cert_ptr,
secx509_pubkey_type*  pkey_ptr
);

#endif //X509_CRYPTO_H
