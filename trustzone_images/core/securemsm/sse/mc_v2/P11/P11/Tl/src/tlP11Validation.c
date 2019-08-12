/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "tlLog.h"
#include "tlP11Validation.h"
#include "tlP11Utils.h"
#include "tlP11Glob.h"
#include "securityExt.h"

#if 0

CK_RV validateDate(CK_DATE *date)
{
  CK_RV ret = CKR_OK;
  uint32_t year = 0;
  uint32_t month = 0;
  uint32_t day = 0;

  TL_LOG_I("validateDate+");
  do {

    if (!date) {
      ret =  CKR_GENERAL_ERROR;
      break;
    }

    if ((*((uint32_t*)date->year) == 0) &&
        (*((uint16_t*)date->month) == 0)  &&
        (*((uint16_t*)date->day) == 0)
        ) {
      ret = CKR_OK;
      break;
    }

    year = date->year[3]-'0';
    year += (date->year[2]-'0')*10;
    year += (date->year[1]-'0')*100;
    year += (date->year[0]-'0')*1000;
    month = date->month[1]-'0';
    month += (date->month[0]-'0')*10;
    day = date->day[1]-'0';
    day += (date->day[0]-'0')*10;

    if ((year == 0) && (month == 0) && (day ==0)) {
      ret = CKR_OK;
      break;
    }
    if ((year < 1900) || (year > 9999)) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((month < 1) || (month > 12)) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((day < 1) || (day > 31)) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  } while(0);
  TL_LOG_I("validateDate- ret:0x%08X",ret);
  return ret;;
}
#endif


CK_RV validateStorage(const tlp11_storage_t *storage)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateStorage+");
  do {
    if (!storage) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    if (storage->label_len > LABEL_LEN) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  }while(0);
  TL_LOG_I("validateStorage- ret:0x%08X",ret);
  return ret;
}

CK_RV validateData(const tlp11_data_t *data)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateData+");
  do {
    if (!data) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateStorage(&data->storage);
    if (ret != CKR_OK)
      break;
    if ((data->application_len > APPLICATION_LEN) ||
        (data->object_id_len > OBJECT_ID_MAX_LEN) ||
        (data->value_len > DATA_MAX_LEN)
        ){
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  }while(0);
  TL_LOG_I("validateData- ret:0x%08X",ret);
  return ret;
}

CK_RV validateKey(const tlp11_key_t *key)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateKey+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateStorage(&key->storage);
    if (ret != CKR_OK)
      break;
    if (key->id_len > ID_MAX_LEN) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((key->local == 0) &&
        (key->key_gen_mechanism != CK_UNAVAILABLE_INFORMATION)) {
      TL_LOG_E("!CKA_LOCAL & CKA_KEY_GEN_MECHANISM: 0x%08X",key->key_gen_mechanism);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
#if 0
    ret = validateDate(&key->start_date);
    if (ret != CKR_OK)
      break;
    ret = validateDate(&key->end_date);
    if (ret != CKR_OK)
      break;
#endif
  } while(0);
  TL_LOG_I("validateKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validatePublicKey(const tlp11_public_key_t *key)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validatePublicKey+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateKey(&key->key);
    if (ret != CKR_OK)
      break;
    if ((key->subject_len > SUBJECT_MAX_LEN)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  } while(0);
  TL_LOG_I("validatePublicKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validatePrivateKey(tlp11_private_key_t *key)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validatePrivateKey+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateKey(&key->key);
    if (ret != CKR_OK)
      break;
    if ((key->subject_len > SUBJECT_MAX_LEN)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((key->never_extractable == CK_TRUE) &&
        (key->extractable == CK_TRUE)) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((key->always_sensitive == CK_TRUE) &&
        (key->sensitive == CK_FALSE)) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (trustedWrapOnly(NULL)) {
       TL_LOG_I("Enforcing CKA_WRAP_WITH_TRUSTED");
       key->wrap_with_trusted = 1;
     }
  } while(0);
  TL_LOG_I("validatePrivateKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateRSAPublicKey(const tlp11_rsa_public_key_t *key)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateRSAPublicKey+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validatePublicKey(&key->public_key);
    if (ret != CKR_OK)
      break;
    if ((key->modulus_len > MODULUS_MAX_LEN) || (key->modulus_len == 0)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (((key->modulus_bits / 8) > MODULUS_MAX_LEN)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((key->public_exponent_len > PUBLIC_EXPONENT_LEN) || (key->public_exponent_len == 0)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  } while(0);
  TL_LOG_I("validateRSAPublicKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateRSAPrivateKey(tlp11_rsa_private_key_t *key)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateRSAPrivateKey+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validatePrivateKey(&key->private_key);
    if (ret != CKR_OK)
      break;
    if ((key->modulus_len > MODULUS_MAX_LEN) || (key->modulus_len == 0)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((key->public_exponent_len > PUBLIC_EXPONENT_LEN) || (key->public_exponent_len == 0)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((key->exponent_len > PRIVATE_EXPONENT_MAX_LEN) || (key->exponent_len == 0)
        ) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  } while(0);
  TL_LOG_I("validateRSAPrivateKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateSecretKey(tlp11_secret_key_t *secret)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateSecretKey+");
  do {
    if (!secret) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateStorage(&secret->key.storage);
    if (ret != CKR_OK)
      break;
    if ((secret->never_extractable == CK_TRUE) &&
        (secret->extractable == CK_TRUE)) {
      TL_LOG_E("never_extractable == TRUE && extractable == TRUE");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((secret->always_sensitive == CK_TRUE) &&
        (secret->sensitive == CK_FALSE)) {
      TL_LOG_E("always_sensitive == TRUE && sensitive == FALSE");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (conflictingAttributes(g_token_0_ptr)) {
      if ((secret->decrypt == 1) && (secret->wrap == 1)) {
        TL_LOG_E("CKA_DECRYPT=CKA_WRAP=1, security breach!");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      if ((secret->encrypt == 1) && (secret->unwrap == 1)) {
        TL_LOG_E("CKA_ENCRYPT=CKA_UNWRAP=1, security breach!");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
    if (trustedWrapOnly(NULL)) {
       TL_LOG_I("Enforcing CKA_WRAP_WITH_TRUSTED");
       secret->wrap_with_trusted = 1;
     }
  }while(0);
  TL_LOG_I("validateSecretKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateGenericSecretKey(tlp11_secret_key_simple_t *generic)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateGenericSecretKey+");
  do {
    if (!generic) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateSecretKey(&generic->secret);
    if (ret != CKR_OK)
      break;
    // can be used only for key derivation
    if ((generic->secret.encrypt == CK_TRUE) ||
        (generic->secret.verify == CK_TRUE) ||
        (generic->secret.decrypt == CK_TRUE) ||
        (generic->secret.sign == CK_TRUE) ||
        (generic->secret.wrap == CK_TRUE) ||
        (generic->secret.unwrap == CK_TRUE)
        ) {
      TL_LOG_E(" Not to be used for cryptographic operations besides derivation!");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (generic->secret.key.key_gen_mechanism == CKM_GENERIC_SECRET_KEY_GEN_KEYPAD) {
      if ((generic->len < PASSWORD_MIN_LEN) || (generic->len > PASSWORD_MAX_LEN)) {
        TL_LOG_E("Password not within length limits: %d",generic->len);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    } else { // any other case
      if ((generic->len < GENERIC_KEY_MIN_LEN) || (generic->len > GENERIC_KEY_MAX_LEN)) {
        TL_LOG_E("Key not within length limits: %d",generic->len);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
  }while(0);
  TL_LOG_I("validateGenericSecretKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateOTPSecretKey(tlp11_otp_secret_key_t *otp)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateOTPSecretKey+");
  do {
    if (!otp) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateSecretKey(&otp->secret);
    if (ret != CKR_OK)
      break;
    // can be used only for signatures generation and verification
    if ((otp->secret.encrypt == CK_TRUE) ||
        (otp->secret.decrypt == CK_TRUE) ||
        (otp->secret.wrap == CK_TRUE) ||
        (otp->secret.unwrap == CK_TRUE)
        ) {
      TL_LOG_E(" Not to be used for cryptographic operations besides signatures!");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((otp->challenge_requirement == CK_OTP_PARAM_MANDATORY) ||
        (otp->challenge_requirement == CK_OTP_PARAM_OPTIONAL) ||
        (otp->pin_requirement == CK_OTP_PARAM_MANDATORY) ||
        (otp->pin_requirement == CK_OTP_PARAM_OPTIONAL)) {
      TL_LOG_E(" Only T-OTP and H-OTP supported, challenge and PIN always ignored");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    //TODO: anything bad in allowing both on the same key?
//    if (((otp->time_requirement == CK_OTP_PARAM_MANDATORY) ||
//         (otp->time_requirement == CK_OTP_PARAM_OPTIONAL)) &&
//        ((otp->counter_requirement == CK_OTP_PARAM_MANDATORY) ||
//         (otp->counter_requirement == CK_OTP_PARAM_OPTIONAL))) {
//      TL_LOG_E(" T-OTP and H-OTP supported, but not combined");
//      ret = CKR_TEMPLATE_INCONSISTENT;
//      break;
//    }
    if (otp->time_requirement == CK_OTP_PARAM_OPTIONAL) {
      TL_LOG_E(" Token lacks internal clock, time if needed is MANDATORY");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((otp->len < OTP_KEY_MIN_LEN) || (otp->len > OTP_KEY_MAX_LEN)) {
      TL_LOG_E("Key not within length limits: %d",otp->len);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((otp->length < OTP_LENGTH_MIN) || (otp->length > OTP_LENGTH_MAX)) {
      TL_LOG_E("OTP LENGTH out of limits: %d",otp->length);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (otp->service_identifier_len > OTP_SERVICE_ID_LEN) {
      TL_LOG_E("service_identifier_len out of limits: %d",otp->service_identifier_len);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (otp->service_logo_len > OTP_SERVICE_LOGO_LEN) {
      TL_LOG_E("service_logo_len out of limits: %d",otp->service_logo_len);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (otp->service_logo_type_len > OTP_SERVICE_LOGO_TYPE_LEN) {
      TL_LOG_E("service_logo_type_len out of limits: %d",otp->service_logo_type_len);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (otp->format != CK_OTP_FORMAT_DECIMAL) {
      TL_LOG_E("Format 0x%08X not supported",otp->format);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (otp->user_friendly_mode == CK_TRUE) {
      TL_LOG_E("User-friendly mode not supported");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  }while(0);
  TL_LOG_I("validateOTPSecretKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateAESSecretKey(tlp11_secret_key_simple_t *aes)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateAESSecretKey+");
  do {
    if (!aes) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateSecretKey(&aes->secret);
    if (ret != CKR_OK)
      break;
    if ((aes->len != 16) &&
        (aes->len != 32)) {
      TL_LOG_E("Unsupported key length");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  }while(0);
  TL_LOG_I("validateAESSecretKey- ret:0x%08X",ret);
  return ret;
}

CK_RV validateDESSecretKey(tlp11_secret_key_simple_t *des)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateDESSecretKey+");
  do {
    if (!des) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    if ((des->secret.key.key_type == CKK_DES) &&
        (des->len != DES_KEY_LEN)) {
      TL_LOG_E("Wrong key length");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if ((des->secret.key.key_type == CKK_DES3) &&
        (des->len != DES3_KEY_LEN)) {
      TL_LOG_E("Wrong key length");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    ret = validateSecretKey(&des->secret);
    if (ret != CKR_OK)
      break;
  }while(0);
  TL_LOG_I("validateDESSecretKey- ret:0x%08X",ret);
  return ret;
}

/** Sanitise an X.509 certificate
 */
CK_RV validateCertificate(const tlp11_certificate_t *certificate)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateCertificate+");
  do {
    if (!certificate) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateStorage(&certificate->storage);
    if (ret != CKR_OK)
      break;
    if (certificate->category > 3){
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
  }while(0);
  TL_LOG_I("validateCertificate- ret:0x%08X",ret);
  return ret;
}

/** Sanitise an X.509 certificate
 */
CK_RV validateX509Certificate(tlp11_certificate_x509_t *x509)
{
  CK_RV ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  TL_LOG_I("validateX509Certificate+");
  do {
    if (!x509) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    ret = validateCertificate(&x509->certificate);
    if (ret != CKR_OK)
      break;
    if (x509->subject_len == 0){
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (x509->java_midp_security_domain > 3){
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    // they cannot be both null
    if ((x509->value_len == 0) && (x509->url_len == 0)) {
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    {
      uint8_t tmp[PUBLIC_KEY_HASH_LEN] = {0};
      if (((0 == memcmp(x509->hash_of_issuer_public_key,tmp,PUBLIC_KEY_HASH_LEN)) ||
          (0 == memcmp(x509->hash_of_subject_public_key,tmp,PUBLIC_KEY_HASH_LEN))) &&
          (x509->url_len != 0)){
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
    // if we have a value and not an URL, check the checksum
    if (x509->value_len != 0) {
      uint8_t checksum[3] = {0};
      result = computeChecksum(
          checksum,
          x509->value,
          x509->value_len,
          tlp11_type_certificate_x509
          );
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to to compute checksum");
        ret = TL2CK(result);
        break;
      }
      if (0 != memcmp(
          checksum,
          x509->certificate.check_value,
          sizeof(x509->certificate.check_value))) {
        TL_LOG_E("CKA_CHECK_VALUE does not match certificate: %02X %02X %02X vs %02X %02X %02X",
            checksum[0],
            checksum[1],
            checksum[2],
            x509->certificate.check_value[0],
            x509->certificate.check_value[1],
            x509->certificate.check_value[2]
            );
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
  }while(0);
  TL_LOG_I("validateX509Certificate- ret:0x%08X",ret);
  return ret;
}
/*
tlp11_type_certificate_x509 =  (0x00000004 | tlp11_type_certificate),
  tlp11_type_data =              (0x00000008 | tlp11_type_storage),
  tlp11_type_aes_key =           (0x00000200 | tlp11_type_secret_key),
  tlp11_type_des3_key =          (0x00000400 | tlp11_type_secret_key),
  tlp11_type_des_key =           (0x00000800 | tlp11_type_secret_key),
  tlp11_type_generic_key =       (0x00001000 | tlp11_type_secret_key),
  tlp11_type_rc4_key =           (0x00002000 | tlp11_type_secret_key),
  tlp11_type_rsa_public_key =    (0x00020000 | tlp11_type_public_key),
  tlp11_type_rsa_private_key =   (0x00200000 | tlp11_type_private_key),
*/
CK_RV validateObject(tlp11_container_object_t *object)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateObject+");
  do {
    if (!object) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    /*lint -e{788}*/
    switch (object->header.object_type) {
    case tlp11_type_data:
      ret = validateData(&object->data);
      break;
    case tlp11_type_aes_key:
      ret = validateAESSecretKey(&object->secret_key_simple);
      break;
    case tlp11_type_generic_key:
      ret = validateGenericSecretKey(&object->secret_key_simple);
      break;
    case tlp11_type_rsa_public_key:
      ret = validateRSAPublicKey(&object->rsa_public_key);
      break;
    case tlp11_type_rsa_private_key:
      ret = validateRSAPrivateKey(&object->rsa_private_key);
      break;
    case tlp11_type_des3_key:
      ret = validateDESSecretKey(&object->secret_key_simple);
      break;
    case tlp11_type_des_key:
      ret = validateDESSecretKey(&object->secret_key_simple);
      break;
    case tlp11_type_rc4_key:
    default:
      TL_LOG_E("Unsupported object/key type");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

  }while(0);
  TL_LOG_I("validateObject- ret:0x%08X",ret);
  return ret;
}

CK_RV validateSecureTemplateSecretKey(const tlp11_secret_key_t *key, uint32_t operation) {
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateSecureTemplate+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    if (!secureTemplates(g_token_0_ptr)) {
      TL_LOG_I("Secure Template extension not enabled");
      ret = CKR_OK;
      break;
    }
    // checks valid regardless of operation
    if (key->key.storage.modifiable == 1) {
      TL_LOG_E("Keys MUST NOT be modifiable");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (key->sensitive == 0){
      TL_LOG_E("Keys MUST be sensitive");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (operation == CKF_GENERATE) {
      if ((key->wrap == 1) &&
          (key->unwrap == 1) &&
          (key->encrypt == 0) &&
          (key->decrypt == 0)) {
        TL_LOG_I("Management key, generation approved");
      } else if ((key->wrap == 0) &&
                 (key->unwrap == 0) &&
                 (key->encrypt == 1) &&
                 (key->decrypt == 1)) {
        TL_LOG_I("Data key, generation approved");
      } else if ((key->wrap == 0) &&
                 (key->unwrap == 0) &&
                 (key->encrypt == 0) &&
                 (key->decrypt == 0)) {
        TL_LOG_I("Neither data, nor management, generation approved");
      } else {
        TL_LOG_I("Unsupported generation template, rejected");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    } else { // all other cases
      if (key->extractable == 1){
        TL_LOG_E("Imported keys MUST NOT be extractable");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      if ((key->wrap == 0) && (key->decrypt == 0) && (key->unwrap == key->encrypt)) {
        TL_LOG_I("CKA_WRAP=CKA_DECRYPT=0, CKA_UNWRAP=CKA_ENCRYPT, approved");
      } else {
        TL_LOG_I("Unsupported template for operation 0x%08X, rejected",operation);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
  }while(0);
  TL_LOG_I("validateSecureTemplate- ret:0x%08X",ret);
  return ret;
}

CK_RV validateSecureTemplateKeyPair(
    const tlp11_public_key_t *pub,
    const tlp11_private_key_t *prv,
    uint32_t operation) {
  CK_RV ret = CKR_OK;
  TL_LOG_I("validateSecureTemplateKeyPair+");
  do {
    if (!secureTemplates(g_token_0_ptr)) {
      TL_LOG_I("Secure Template extension not enabled");
      ret = CKR_OK;
      break;
    }
    if (prv && (prv->key.storage.modifiable == 1)) {
      TL_LOG_E("Private keys MUST NOT be modifiable");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (pub && (pub->key.storage.modifiable == 1)) {
      TL_LOG_E("Public keys MUST NOT be modifiable");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (prv && (prv->sensitive == 0)) {
      TL_LOG_E("Private keys MUST be sensitive");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (operation == CKF_GENERATE_KEY_PAIR) {
      // we need both of them
      if (!pub || !prv) {
        ret = CKR_GENERAL_ERROR;
        break;
      }
      if ((pub->wrap == 0) && // public is "public", so it could be used to inject anything
          (prv->unwrap == 0)) { // private must support decryption, else it's useless
        TL_LOG_I("CKA_WRAP=CKA_UNWRAP=0");
      } else {
        TL_LOG_I("Unsupported generation template, rejected");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    } else if (operation == CKF_UNWRAP) {
      // unwrapping a private key
      if (!prv) {
        ret = CKR_GENERAL_ERROR;
        break;
      }
      if (prv->extractable == 1){
        TL_LOG_E("Imported keys MUST NOT be extractable");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      if (prv->unwrap == 0) {
        TL_LOG_I("CKA_UNWRAP=0, approved");
      } else {
        TL_LOG_I("Unsupported template for operation 0x%08X, rejected",operation);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    } else { // CreateObject/CopyObject
      if (!pub) {
        ret = CKR_GENERAL_ERROR;
        break;
      }
      if (pub->wrap == 0) {
        TL_LOG_I("CKA_WRAP=0, approved");
      } else {
        TL_LOG_I("Unsupported template for operation 0x%08X, rejected",operation);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    }
  }while(0);
  TL_LOG_I("validateSecureTemplateKeyPair- ret:0x%08X",ret);
  return ret;
}

CK_RV cryptoValidateRSAPrivateKey(tlp11_rsa_private_key_t *key)
{
  CK_RV ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCipherAlg_t alg;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiRsaKey_t rsaKey = {0};
  tlApiCipherMode_t mode;
  tlApiCrSession_t sessionhandle;
  uint8_t dataIn[256];
  uint32_t dataInLen = 256;
  size_t randomLen = 0;
  uint8_t dataEnc[256];
  size_t dataEncLen = 256;
  uint8_t dataOut[256];
  size_t dataOutLen = 256;
  TL_LOG_I("cryptoValidateRSAPrivateKey+");
  do {
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    alg = TLAPI_ALG_RSA_PKCS1;
    // first encrypt with the public
    rsaKey.exponent.value = key->public_exponent;
    rsaKey.exponent.len = key->public_exponent_len;
    rsaKey.modulus.value = key->modulus;
    rsaKey.modulus.len = key->modulus_len;
    mcKey.rsaKey=&rsaKey;
    mode = TLAPI_MODE_ENCRYPT;
    // fill in input
    dataInLen = key->modulus_len >> 1;
    randomLen = dataInLen;
    result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,dataIn,&randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      break;
    }
    if (randomLen != dataInLen) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,dataInLen);
      result = E_TLAPI_UNKNOWN;
      break;
    }
    //DUMP_HEX("Pub Exp",rsaKey.exponent,rsaKey.exponentLen);
    //DUMP_HEX("Modulus",rsaKey.modulus,rsaKey.modulusLen);
    result = CRYPTO_OP(tlApiCipherInit(
          &sessionhandle,
          alg,
          mode,
          &mcKey));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      ret=TL2CK(result);
      break;
    }
    //DUMP_HEX("Input",dataIn,dataInLen);
    result = CRYPTO_OP(tlApiCipherDoFinal(
          sessionhandle,
          dataIn,
          dataInLen,
          dataEnc,
          &dataEncLen));
    if (IS_STUBBED(result)) {
      dataEncLen=min(dataEncLen,dataInLen);
      memscpy(dataEnc,dataEncLen,dataIn,dataEncLen);
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
      ret=TL2CK(result);
      break;
    }
    //DUMP_HEX("Output",dataEnc,dataEncLen);
    // now decrypt
    rsaKey.privateExponent.value = key->exponent;
    rsaKey.privateExponent.len = key->exponent_len;
    mcKey.rsaKey=&rsaKey;
    mode = TLAPI_MODE_DECRYPT;
    //DUMP_HEX("Prv Exp",rsaKey.exponent,rsaKey.exponentLen);
    //DUMP_HEX("Modulus",rsaKey.modulus,rsaKey.modulusLen);
    result = CRYPTO_OP(tlApiCipherInit(
          &sessionhandle,
          alg,
          mode,
          &mcKey));
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherInit: 0x%08X",result);
      ret=TL2CK(result);
      break;
    }
    //DUMP_HEX("dataEnc",dataEnc,dataEncLen);
    //DUMP_HEX("dataOut",dataOut,dataOutLen);
    result = CRYPTO_OP(tlApiCipherDoFinal(
          sessionhandle,
          dataEnc,
          dataEncLen,
          dataOut,
          &dataOutLen));
    //DUMP_HEX("dataOut",dataOut,dataOutLen);
    if (IS_STUBBED(result)) {
      dataOutLen=min(dataOutLen,dataEncLen);
      memscpy(dataOut,dataOutLen,dataEnc,dataOutLen);
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
      ret=TL2CK(result);
      break;
    }
    if (dataInLen != dataOutLen) {
      TL_LOG_E("Lengths do not match: %d vs %d",dataOutLen,dataInLen);
      ret = CKR_DATA_INVALID;
      break;
    }
    if (0 != memcmp(dataIn,dataOut,dataInLen)) {
      TL_LOG_E("Content does not match!");
      DUMP_HEX("Expected",dataIn,dataInLen);
      DUMP_HEX("Decrypted",dataOut,dataInLen);
      ret = CKR_DATA_INVALID;
      break;
    }
  } while(0);
  TL_LOG_I("cryptoValidateRSAPrivateKey- ret:0x%08X",ret);
  return ret;
}
