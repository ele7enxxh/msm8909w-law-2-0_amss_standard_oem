/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "tlLog.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlP11Mechanisms.h"
#include "asn1.h"
#include "tlP11Glob.h"

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_UNWRAP_FUNC(so_unwrap)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  size_t soLen = 0;
  TL_LOG_I("so_unwrap+");
  do {
    if (!newKey) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    // check the parameters
    if (!params || (params->header.valid == 0)) {
      TL_LOG_E(" NULL input!");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->SO_KEY_WRAP_PARAMS.soLen == 0) {
      TL_LOG_E(" Bad soLen");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    // unwrap the secure object
    tlp11_so_container_t *so_container_0_ptr = (tlp11_so_container_t *)params->SO_KEY_WRAP_PARAMS.so;
    soLen = sizeof(tlp11_so_container_t);
    {
      result = CRYPTO_OP(tlApiUnwrapObjectExt(
            (mcSoHeader_t *)params->SO_KEY_WRAP_PARAMS.so,
            sizeof(params->SO_KEY_WRAP_PARAMS.so),
            so_container_0_ptr,
            &soLen,
            TLAPI_UNWRAP_PERMIT_CONTEXT_MASK));
    }
    if (IS_STUBBED(result)) {
      memscpy(
          so_container_0_ptr,
          sizeof(tlp11_so_container_t),
          params->SO_KEY_WRAP_PARAMS.so+sizeof(mcSoHeader_t),
          min(soLen,params->SO_KEY_WRAP_PARAMS.soLen-sizeof(mcSoHeader_t)));
      soLen = params->SO_KEY_WRAP_PARAMS.soLen - sizeof(mcSoHeader_t) ;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("Failed to unwrap data: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (so_container_0_ptr->object.id_len > ID_MAX_LEN) {
      TL_LOG_E("SO ID is too long, cannot fit in newKey");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    // trusted implies not modifiable
    newKey->storage.modifiable = 0;
    newKey->storage.token = 1;
    // now check what key we got and if that matches with the newKey
    if (so_container_0_ptr->object.cls == CKO_SECRET_KEY) {
      newKey->object.cls = CKO_SECRET_KEY;
      newKey->key.key_type = CKK_AES;
      newKey->header.object_type = tlp11_type_aes_key;
      // copy over the value
      if (so_container_0_ptr->secret.len > SECRET_KEY_MAX_LEN) {
        TL_LOG_E("SO key is too long, cannot fit in newKey");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      newKey->key.id_len = so_container_0_ptr->object.id_len;
      memscpy(newKey->key.id,ID_MAX_LEN,so_container_0_ptr->object.id,newKey->key.id_len);
      newKey->secret_key_simple.len = so_container_0_ptr->secret.len;
      memscpy(newKey->secret_key_simple.value,SECRET_KEY_MAX_LEN,so_container_0_ptr->secret.value,so_container_0_ptr->secret.len);
      result = computeChecksum(
        newKey->secret_key.check_value,
        newKey->secret_key_simple.value,
        newKey->secret_key_simple.len,
        newKey->header.object_type);
      if (!IS_OK(result)) {
        TL_LOG_E(" Failed to compute checksum");
        rv = TL2CK(result);
        break;
      }
      newKey->secret_key.trusted = 1;
      newKey->secret_key.extractable = 0;
      newKey->secret_key.sensitive = 1;
      // go through the flags
      newKey->secret_key.encrypt = ((so_container_0_ptr->key.flags & CKF_ENCRYPT) != 0);
      newKey->secret_key.decrypt = ((so_container_0_ptr->key.flags & CKF_DECRYPT) != 0);
      newKey->secret_key.wrap = ((so_container_0_ptr->key.flags & CKF_WRAP) != 0);
      newKey->secret_key.unwrap = ((so_container_0_ptr->key.flags & CKF_UNWRAP) != 0);
      newKey->secret_key.sign = ((so_container_0_ptr->key.flags & CKF_SIGN) != 0);
      newKey->secret_key.verify = ((so_container_0_ptr->key.flags & CKF_VERIFY) != 0);
      newKey->key.derive = ((so_container_0_ptr->key.flags & CKF_DERIVE) != 0);
      // clear wrap and unwrap template
      zeroAttributeTemplate(&newKey->secret_key.wrap_template);
      zeroAttributeTemplate(&newKey->secret_key.unwrap_template);
      rv = validateAESSecretKey(&newKey->secret_key_simple);
    } else if (so_container_0_ptr->object.cls == CKO_OTP_KEY) {
      newKey->object.cls = CKO_OTP_KEY;
      newKey->key.key_type = CKK_HOTP;
      newKey->header.object_type = tlp11_type_otp_key;
      // copy over the value
      if (so_container_0_ptr->secret.len > OTP_KEY_MAX_LEN) {
        TL_LOG_E("SO key is too long, cannot fit in newKey");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      newKey->key.id_len = so_container_0_ptr->object.id_len;
      memscpy(newKey->key.id,ID_MAX_LEN,so_container_0_ptr->object.id,newKey->key.id_len);
      newKey->otp.len = so_container_0_ptr->secret.len;
      memscpy(newKey->otp.value,OTP_KEY_MAX_LEN,so_container_0_ptr->secret.value,so_container_0_ptr->secret.len);
      result = computeChecksum(
        newKey->secret_key.check_value,
        newKey->otp.value,
        newKey->otp.len,
        newKey->header.object_type);
      if (!IS_OK(result)) {
        TL_LOG_E(" Failed to compute checksum");
        rv = TL2CK(result);
        break;
      }
      newKey->secret_key.trusted = 1;
      newKey->secret_key.extractable = 0;
      newKey->secret_key.sensitive = 1;
      // go through the flags
      newKey->secret_key.encrypt = ((so_container_0_ptr->key.flags & CKF_ENCRYPT) != 0);
      newKey->secret_key.decrypt = ((so_container_0_ptr->key.flags & CKF_DECRYPT) != 0);
      newKey->secret_key.wrap = ((so_container_0_ptr->key.flags & CKF_WRAP) != 0);
      newKey->secret_key.unwrap = ((so_container_0_ptr->key.flags & CKF_UNWRAP) != 0);
      newKey->secret_key.sign = ((so_container_0_ptr->key.flags & CKF_SIGN) != 0);
      newKey->secret_key.verify = ((so_container_0_ptr->key.flags & CKF_VERIFY) != 0);
      newKey->key.derive = ((so_container_0_ptr->key.flags & CKF_DERIVE) != 0);
      // clear wrap and unwrap template
      zeroAttributeTemplate(&newKey->secret_key.wrap_template);
      zeroAttributeTemplate(&newKey->secret_key.unwrap_template);
      // copy over OTP specific fields: counter
      memscpy(newKey->otp.counter,OTP_COUNTER_LEN,so_container_0_ptr->otp.counter,sizeof(newKey->otp.counter));
      // service identifier
      if (so_container_0_ptr->otp.service_identifier_len > OTP_SERVICE_ID_LEN) {
        TL_LOG_E("OTP Service Identifier too long, cannot fit in newKey");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      newKey->otp.service_identifier_len = so_container_0_ptr->otp.service_identifier_len;
      memscpy(newKey->otp.service_identifier,OTP_SERVICE_ID_LEN,so_container_0_ptr->otp.service_identifier,newKey->otp.service_identifier_len);
      // service logo
      if (so_container_0_ptr->otp.service_logo_len > OTP_SERVICE_LOGO_LEN) {
        TL_LOG_E("OTP Service Logo too long, cannot fit in newKey");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      newKey->otp.service_logo_len = so_container_0_ptr->otp.service_logo_len;
      memscpy(newKey->otp.service_logo,OTP_SERVICE_LOGO_LEN,so_container_0_ptr->otp.service_logo,newKey->otp.service_logo_len);
      // service logo type
      if (so_container_0_ptr->otp.service_logo_type_len > OTP_SERVICE_LOGO_TYPE_LEN) {
        TL_LOG_E("OTP Service Logo Type too long, cannot fit in newKey");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      newKey->otp.service_logo_type_len = so_container_0_ptr->otp.service_logo_type_len;
      memscpy(newKey->otp.service_logo_type,OTP_SERVICE_LOGO_TYPE_LEN,so_container_0_ptr->otp.service_logo_type,newKey->otp.service_logo_type_len);
      // user identifier
      if (so_container_0_ptr->otp.user_identifier_len > OTP_USER_ID_LEN) {
        TL_LOG_E("OTP User Identifier too long, cannot fit in newKey");
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      newKey->otp.user_identifier_len = so_container_0_ptr->otp.user_identifier_len;
      memscpy(newKey->otp.user_identifier,OTP_USER_ID_LEN,so_container_0_ptr->otp.user_identifier,newKey->otp.user_identifier_len);
      // fix a couple of defaults
      if ((newKey->otp.counter_requirement == CK_OTP_PARAM_IGNORED) &&
          (newKey->otp.time_requirement == CK_OTP_PARAM_IGNORED)) {
        newKey->otp.counter_requirement = CK_OTP_PARAM_OPTIONAL;
      }
      if (newKey->otp.length == 0) {
        newKey->otp.length = OTP_LENGTH_MAX;
      }

      rv = validateOTPSecretKey(&newKey->otp);
    } else if (so_container_0_ptr->object.cls == CKO_PUBLIC_KEY) {
      // this is possible only with this mechanism, as a way to insert trusted
      // public keys in the token
      newKey->object.cls = CKO_PUBLIC_KEY;
      newKey->key.key_type = CKK_RSA;
      newKey->header.object_type = tlp11_type_rsa_public_key;

      newKey->key.id_len = so_container_0_ptr->object.id_len;
      memscpy(newKey->key.id,ID_MAX_LEN,so_container_0_ptr->object.id,newKey->key.id_len);
      // proceed to decode
      newKey->rsa_public_key.modulus_len=MODULUS_MAX_LEN;
      newKey->rsa_public_key.public_exponent_len=PUBLIC_EXPONENT_LEN;
      result = DER_decode_RSAPublicKey(
          so_container_0_ptr->pub.derData,
          so_container_0_ptr->pub.derDataLen,
          newKey->rsa_public_key.public_exponent,
          &newKey->rsa_public_key.public_exponent_len,
          newKey->rsa_public_key.modulus,
          &newKey->rsa_public_key.modulus_len);

      if (!IS_OK(result)) {
        TL_LOG_E("Failed to decode data!");
        rv = CKR_GENERAL_ERROR;
        break;
      }
      newKey->rsa_public_key.modulus_bits = 8*newKey->rsa_public_key.modulus_len;
      newKey->public_key.trusted = 1;
      // go through the flags
      newKey->public_key.encrypt = ((so_container_0_ptr->key.flags & CKF_ENCRYPT) != 0);
      newKey->public_key.wrap = ((so_container_0_ptr->key.flags & CKF_WRAP) != 0);
      newKey->public_key.verify = ((so_container_0_ptr->key.flags & CKF_VERIFY) != 0);
      newKey->public_key.verify_recovery = ((so_container_0_ptr->key.flags & CKF_VERIFY_RECOVER) != 0);
      newKey->key.derive = ((so_container_0_ptr->key.flags & CKF_DERIVE) != 0);
      zeroAttributeTemplate(&newKey->public_key.wrap_template);
      rv = validateRSAPublicKey(&newKey->rsa_public_key);
    } else if (so_container_0_ptr->object.cls == CKO_PRIVATE_KEY) {
      newKey->object.cls = CKO_PRIVATE_KEY;
      newKey->key.key_type = CKK_RSA;
      newKey->header.object_type = tlp11_type_rsa_private_key;

      newKey->key.id_len = so_container_0_ptr->object.id_len;
      memscpy(newKey->key.id,ID_MAX_LEN,so_container_0_ptr->object.id,newKey->key.id_len);
      // decode the content
      newKey->rsa_private_key.modulus_len=MODULUS_MAX_LEN;
      newKey->rsa_private_key.public_exponent_len=PUBLIC_EXPONENT_LEN;
      newKey->rsa_private_key.exponent_len=PRIVATE_EXPONENT_MAX_LEN;
      newKey->rsa_private_key.prime1_len=PRIME1_MAX_LEN;
      newKey->rsa_private_key.prime2_len=PRIME2_MAX_LEN;
      newKey->rsa_private_key.exponent1_len=EXPONENT1_MAX_LEN;
      newKey->rsa_private_key.exponent2_len=EXPONENT2_MAX_LEN;
      newKey->rsa_private_key.coefficient_len=COEFFICIENT_MAX_LEN;
      result = DER_decode_PrivateKeyInfo(
          so_container_0_ptr->pub.derData,
          so_container_0_ptr->pub.derDataLen,
          &newKey->rsa_private_key
          );
      if (!IS_OK(result)) {
        TL_LOG_E("DER_decode_PrivateKeyInfo: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      rv = cryptoValidateRSAPrivateKey(&newKey->rsa_private_key);
      if (rv != CKR_OK) {
        TL_LOG_E("New key failed validation!");
        break;
      }

      newKey->private_key.extractable = 0;
      newKey->private_key.sensitive = 1;
      // go through the flags
      newKey->private_key.decrypt = ((so_container_0_ptr->key.flags & CKF_DECRYPT) != 0);
      newKey->private_key.unwrap = ((so_container_0_ptr->key.flags & CKF_UNWRAP) != 0);
      newKey->private_key.sign = ((so_container_0_ptr->key.flags & CKF_SIGN) != 0);
      newKey->private_key.sign_recovery = ((so_container_0_ptr->key.flags & CKF_SIGN_RECOVER) != 0);
      newKey->key.derive = ((so_container_0_ptr->key.flags & CKF_DERIVE) != 0);
      zeroAttributeTemplate(&newKey->private_key.unwrap_template);
      rv = validateRSAPrivateKey(&newKey->rsa_private_key);
    } else if (so_container_0_ptr->object.cls == CKO_CERTIFICATE) {
      newKey->object.cls = CKO_CERTIFICATE;
      newKey->certificate.type = CKC_X_509;
      newKey->header.object_type = tlp11_type_certificate_x509;
      newKey->certificate.trusted = 1;
      // copy over all fields
      newKey->x509.id_len = so_container_0_ptr->object.id_len;
      memscpy(newKey->x509.id,ID_MAX_LEN,so_container_0_ptr->object.id,newKey->x509.id_len);
      memscpy(&newKey->certificate.start_date,sizeof(newKey->certificate.start_date),&so_container_0_ptr->x509.start_date,sizeof(newKey->certificate.start_date));
      memscpy(&newKey->certificate.end_date,sizeof(newKey->certificate.end_date),&so_container_0_ptr->x509.end_date,sizeof(newKey->certificate.end_date));
      if ((so_container_0_ptr->x509.subject_len > SUBJECT_MAX_LEN) ||
          (so_container_0_ptr->x509.issuer_len > ISSUER_MAX_LEN) ||
          (so_container_0_ptr->x509.serial_number_len > SERIAL_NUMBER_MAX_LEN) ||
          (so_container_0_ptr->x509.value_len > X509_VALUE_MAX_LEN)) {
        TL_LOG_E("Unsupported lengths for an X.509 certificate");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      newKey->x509.subject_len = so_container_0_ptr->x509.subject_len;
      newKey->x509.issuer_len = so_container_0_ptr->x509.issuer_len;
      newKey->x509.serial_number_len = so_container_0_ptr->x509.serial_number_len;
      newKey->x509.value_len = so_container_0_ptr->x509.value_len;
      memscpy(newKey->x509.subject,SUBJECT_MAX_LEN,so_container_0_ptr->x509.subject,newKey->x509.subject_len);
      memscpy(newKey->x509.issuer,ISSUER_MAX_LEN,so_container_0_ptr->x509.issuer,newKey->x509.issuer_len);
      memscpy(newKey->x509.serial_number,SERIAL_NUMBER_MAX_LEN,so_container_0_ptr->x509.serial_number,newKey->x509.serial_number_len);
      memscpy(newKey->x509.value,X509_VALUE_MAX_LEN,so_container_0_ptr->x509.value,newKey->x509.value_len);
      if ((so_container_0_ptr->x509.check_value[0] == 0) &&
          (so_container_0_ptr->x509.check_value[1] == 0) &&
          (so_container_0_ptr->x509.check_value[2] == 0)) {
        // overwrite it
        result = computeChecksum(
            so_container_0_ptr->x509.check_value,
            so_container_0_ptr->x509.value,
            so_container_0_ptr->x509.value_len,
            tlp11_type_certificate_x509
            );
        if (!IS_OK(result)) {
          TL_LOG_E("Failed to to compute checksum");
          rv = TL2CK(result);
          break;
        }
      }
      memscpy(newKey->certificate.check_value,
          CHECK_VALUE_LEN,
          so_container_0_ptr->x509.check_value,
          sizeof(newKey->certificate.check_value));
      rv = validateX509Certificate(&newKey->x509);
    } else {
      // unsupported key type
      TL_LOG_E(" Unsupported pbject class: %08X",so_container_0_ptr->object.cls);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
  } while(0);
  TL_LOG_I("so_unwrap-: %08X",rv);
  return rv;
}
