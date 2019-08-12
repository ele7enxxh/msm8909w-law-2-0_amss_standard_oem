/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlLog.h"
#include "tlP11Attributes.h"
#include "tlP11Glob.h"
#include "securityExt.h"

#define BB_ANY ((CK_BBOOL)-1)

static CK_RV setBool(CK_BBOOL *target, const CK_BBOOL value) {
  if (!target)
    return CKR_GENERAL_ERROR;
  if ((value == 0) || (value == 1)) {
    *target = value;
    return CKR_OK;
  } else {
    return CKR_ATTRIBUTE_VALUE_INVALID;
  }
}

static CK_RV setStickyConfBool(CK_BBOOL *target, const CK_BBOOL value, const CK_BBOOL sticky_value) {
  if (!target)
    return CKR_GENERAL_ERROR;
  if (sticky(g_token_0_ptr)) {
    if (*target == value)
      return CKR_OK;
    else if ((*target == sticky_value) || (sticky_value == BB_ANY))
      return CKR_ATTRIBUTE_READ_ONLY;
    else
      return setBool(target,value);
  } else {
    return setBool(target,value);
  }
}

static CK_RV setStickyBool(CK_BBOOL *target, const CK_BBOOL value, const CK_BBOOL sticky_value) {
  if (!target)
    return CKR_GENERAL_ERROR;
  if (*target == value)
    return CKR_OK;
  else if ((*target == sticky_value) || (sticky_value == BB_ANY))
    return CKR_ATTRIBUTE_READ_ONLY;
  else
    return setBool(target,value);
}

#define getValue(src,value) \
    *(value) = (src); \
    rv = CKR_OK


// we effectively need to return only parameters which are encrypted!
CK_RV getULongAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    CK_ULONG *value)
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  if (!value || !object)
    return CKR_GENERAL_ERROR;

  do {
    if (IS_TYPE(object,tlp11_type_secret_key)) {
      // a symmetric secret key
      if ((object->secret_key.sensitive) || (object->secret_key.extractable == 0)) {
        rv = CKR_ATTRIBUTE_SENSITIVE;
        break;
      }
      switch (attribute) {
      case CKA_VALUE_LEN:
        if (IS_TYPE(object,tlp11_type_otp_key))
          *value = object->otp.len;
        else
          *value = object->secret_key_simple.len;
        rv = CKR_OK;
        break;
      default:
        break;
      }

      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }

      if (IS_TYPE(object,tlp11_type_otp_key)) {
        switch (attribute) {
        case CKA_OTP_TIME:
          *value = object->otp.time;
          rv = CKR_OK;
          break;
        default:
          break;
        }
      }
    }



  } while(0);
  return rv;
}

CK_RV getArrayAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    uint8_t *value,
    uint32_t *value_len)
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  if (!value || !value_len || !object)
    return CKR_GENERAL_ERROR;

  do {
    // x509 certificates are all in clear
    if (IS_TYPE(object,tlp11_type_data)) {
      // DATA
      switch (attribute) {
      case CKA_VALUE:
        if (*value_len < object->data.value_len) {
          *value_len = object->data.value_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->data.value_len,object->data.value,object->data.value_len);
          *value_len = object->data.value_len;
          rv = CKR_OK;
        }
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_rsa_private_key)) {
      if ((object->private_key.sensitive) || (object->private_key.extractable == 0)) {
          rv = CKR_ATTRIBUTE_SENSITIVE;
          break;
      }
      // RSA Private Key
      switch (attribute) {
      case CKA_PRIVATE_EXPONENT:
        if (*value_len < object->rsa_private_key.exponent_len) {
          *value_len = object->rsa_private_key.exponent_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->rsa_private_key.exponent_len,object->rsa_private_key.exponent,object->rsa_private_key.exponent_len);
          *value_len = object->rsa_private_key.exponent_len;
          rv = CKR_OK;
        }
        break;
      case CKA_PRIME_1:
        if (*value_len < object->rsa_private_key.prime1_len) {
          *value_len = object->rsa_private_key.prime1_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->rsa_private_key.prime1_len,object->rsa_private_key.prime1,object->rsa_private_key.prime1_len);
          *value_len = object->rsa_private_key.prime1_len;
          rv = CKR_OK;
        }
        break;
      case CKA_PRIME_2:
        if (*value_len < object->rsa_private_key.prime2_len) {
          *value_len = object->rsa_private_key.prime2_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->rsa_private_key.prime2_len,object->rsa_private_key.prime2,object->rsa_private_key.prime2_len);
          *value_len = object->rsa_private_key.prime2_len;
          rv = CKR_OK;
        }
        break;
      case CKA_EXPONENT_1:
        if (*value_len < object->rsa_private_key.exponent1_len) {
          *value_len = object->rsa_private_key.exponent1_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->rsa_private_key.exponent1_len,object->rsa_private_key.exponent1,object->rsa_private_key.exponent1_len);
          *value_len = object->rsa_private_key.exponent1_len;
          rv = CKR_OK;
        }
        break;
      case CKA_EXPONENT_2:
        if (*value_len < object->rsa_private_key.exponent2_len) {
          *value_len = object->rsa_private_key.exponent2_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->rsa_private_key.exponent2_len,object->rsa_private_key.exponent2,object->rsa_private_key.exponent2_len);
          *value_len = object->rsa_private_key.exponent2_len;
          rv = CKR_OK;
        }
        break;
      case CKA_COEFFICIENT:
        if (*value_len < object->rsa_private_key.coefficient_len) {
          *value_len = object->rsa_private_key.coefficient_len;
          rv = CKR_BUFFER_TOO_SMALL;
        } else {
          memscpy(value,object->rsa_private_key.coefficient_len,object->rsa_private_key.coefficient,object->rsa_private_key.coefficient_len);
          *value_len = object->rsa_private_key.coefficient_len;
          rv = CKR_OK;
        }
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_secret_key)) {
      // a symmetric secret key
      if ((object->secret_key.sensitive) || (object->secret_key.extractable == 0)) {
        rv = CKR_ATTRIBUTE_SENSITIVE;
        break;
      }
      switch (attribute) {
      case CKA_VALUE:
      {
        const uint8_t *src;
        uint32_t src_len;
        if (IS_TYPE(object,tlp11_type_otp_key)) {
          src = object->otp.value;
          src_len = object->otp.len;
        } else {
          src = object->secret_key_simple.value;
          src_len = object->secret_key_simple.len;
        }
        if (*value_len < src_len) {
          *value_len = src_len;
          rv = CKR_BUFFER_TOO_SMALL;
          break;
        }
        *value_len = src_len;
        memscpy(value,src_len,src,src_len);
        rv = CKR_OK;

        break;
      }
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }


      if (IS_TYPE(object,tlp11_type_otp_key)) {
        switch (attribute) {
        case CKA_OTP_COUNTER:
          if (*value_len < OTP_COUNTER_LEN) {
            *value_len = OTP_COUNTER_LEN;
            rv = CKR_BUFFER_TOO_SMALL;
          } else {
            memscpy(value,OTP_COUNTER_LEN,object->otp.counter,OTP_COUNTER_LEN);
            *value_len = OTP_COUNTER_LEN;
            rv = CKR_OK;
          }
          break;
        case CKA_OTP_USER_IDENTIFIER:
          if (*value_len < object->otp.user_identifier_len) {
            *value_len = object->otp.user_identifier_len;
            rv = CKR_BUFFER_TOO_SMALL;
          } else {
            memscpy(value,OTP_USER_ID_LEN,object->otp.user_identifier,object->otp.user_identifier_len);
            *value_len = object->otp.user_identifier_len;
            rv = CKR_OK;
          }
          break;
        case CKA_OTP_SERVICE_IDENTIFIER:
          if (*value_len < object->otp.service_identifier_len) {
            *value_len = object->otp.service_identifier_len;
            rv = CKR_BUFFER_TOO_SMALL;
          } else {
            memscpy(value,OTP_SERVICE_ID_LEN,object->otp.service_identifier,object->otp.service_identifier_len);
            *value_len = object->otp.service_identifier_len;
            rv = CKR_OK;
          }
          break;
        case CKA_OTP_SERVICE_LOGO:
          if (*value_len < object->otp.service_logo_len) {
            *value_len = object->otp.service_logo_len;
            rv = CKR_BUFFER_TOO_SMALL;
          } else {
            memscpy(value,OTP_SERVICE_LOGO_LEN,object->otp.service_logo,object->otp.service_logo_len);
            *value_len = object->otp.service_logo_len;
            rv = CKR_OK;
          }
          break;
        case CKA_OTP_SERVICE_LOGO_TYPE:
          if (*value_len < object->otp.service_logo_type_len) {
            *value_len = object->otp.service_logo_type_len;
            rv = CKR_BUFFER_TOO_SMALL;
          } else {
            memscpy(value,OTP_SERVICE_LOGO_TYPE_LEN,object->otp.service_logo_type,object->otp.service_logo_type_len);
            *value_len = object->otp.service_logo_type_len;
            rv = CKR_OK;
          }
          break;
        default:
          break;
        }
      }
    }
  } while(0);
  return rv;
}

CK_RV setAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    tlp11_container_object_t *object,
    const uint32_t value)
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  if (!object)
    return CKR_GENERAL_ERROR;
  do {
    if (attribute == CKA_CLASS) {
      rv = CKR_ATTRIBUTE_READ_ONLY;
      break;
    }

    if (IS_TYPE(object,tlp11_type_storage)) {
      if (object->storage.modifiable == 0) {
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_storage)) {

      switch (attribute) {
      case CKA_TOKEN:
      case CKA_PRIVATE:
      case CKA_MODIFIABLE:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_certificate)) {
      switch (attribute) {
      case CKA_CERTIFICATE_TYPE:
      case CKA_CERTIFICATE_CATEGORY:
      case CKA_TRUSTED:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_certificate_x509)) {
      switch (attribute) {
      case CKA_JAVA_MIDP_SECURITY_DOMAIN:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_key)) {
      switch (attribute) {
      case CKA_KEY_TYPE:
      case CKA_LOCAL:
      case CKA_KEY_GEN_MECHANISM:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      case CKA_DERIVE:
        rv = setBool(&object->key.derive,(CK_BBOOL)value);
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_public_key)) {
      switch (attribute) {
      case CKA_TRUSTED:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      case CKA_ENCRYPT:
        rv = setStickyConfBool(&object->public_key.encrypt,(CK_BBOOL)value,BB_ANY);
        break;
      case CKA_VERIFY:
        //rv = setStickyConfBool(&object->public_key.verify,value,BB_ANY);
        rv = setBool(&object->public_key.verify,(CK_BBOOL)value);
        break;
      case CKA_VERIFY_RECOVER:
        //rv = setStickyConfBool(&object->public_key.verify_recovery,value,BB_ANY);
        rv = setBool(&object->public_key.verify_recovery,(CK_BBOOL)value);
        break;
      case CKA_WRAP:
        rv = setStickyConfBool(&object->public_key.wrap,(CK_BBOOL)value,BB_ANY);
        break;
      default:
        break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_private_key)) {
      switch (attribute) {
      case CKA_ALWAYS_AUTHENTICATE:
      case CKA_ALWAYS_SENSITIVE:
      case CKA_NEVER_EXTRACTABLE:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      case CKA_SENSITIVE:
        rv = setStickyBool(&object->private_key.sensitive,(CK_BBOOL)value,1);
        break;
      case CKA_DECRYPT:
        rv = setStickyConfBool(&object->private_key.decrypt,(CK_BBOOL)value,BB_ANY);
        break;
      case CKA_SIGN:
        //rv = setStickyConfBool(&object->private_key.sign,value,BB_ANY);
        rv = setBool(&object->private_key.sign,(CK_BBOOL)value);
        break;
      case CKA_SIGN_RECOVER:
        //rv = setStickyConfBool(&object->private_key.sign_recovery,value,BB_ANY);
        rv = setBool(&object->private_key.sign_recovery,(CK_BBOOL)value);
        break;
      case CKA_UNWRAP:
        rv = setStickyConfBool(&object->private_key.unwrap,(CK_BBOOL)value,BB_ANY);
        break;
      case CKA_EXTRACTABLE:
        rv = setStickyBool(&object->private_key.extractable,(CK_BBOOL)value,0);
        break;
      case CKA_WRAP_WITH_TRUSTED:
        rv = setStickyBool(&object->private_key.wrap_with_trusted,(CK_BBOOL)value,1);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_secret_key)) {
      switch (attribute) {
      case CKA_ALWAYS_SENSITIVE:
      case CKA_NEVER_EXTRACTABLE:
      case CKA_VALUE_LEN:
      case CKA_TRUSTED:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      case CKA_SENSITIVE:
        rv = setStickyBool(&object->secret_key.sensitive,(CK_BBOOL)value,1);
        break;
      case CKA_ENCRYPT:
        rv = setStickyConfBool(&object->secret_key.encrypt,(CK_BBOOL)value,1);
        break;
      case CKA_DECRYPT:
        rv = setStickyConfBool(&object->secret_key.decrypt,(CK_BBOOL)value,1);
        break;
      case CKA_SIGN:
        //rv = setStickyConfBool(&object->secret_key.sign,value,BB_ANY);
        rv = setBool(&object->secret_key.sign,(CK_BBOOL)value);
        break;
      case CKA_VERIFY:
        //rv = setStickyConfBool(&object->secret_key.verify,value,BB_ANY);
        rv = setBool(&object->secret_key.verify,(CK_BBOOL)value);
        break;
      case CKA_WRAP:
        rv = setStickyConfBool(&object->secret_key.wrap,(CK_BBOOL)value,1);
        break;
      case CKA_UNWRAP:
        rv = setStickyConfBool(&object->secret_key.unwrap,(CK_BBOOL)value,1);
        break;
      case CKA_EXTRACTABLE:
        rv = setStickyBool(&object->secret_key.extractable,(CK_BBOOL)value,0);
        break;
      case CKA_WRAP_WITH_TRUSTED:
        rv = setStickyBool(&object->secret_key.wrap_with_trusted,(CK_BBOOL)value,1);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
    if (IS_TYPE(object,tlp11_type_otp_key)) {
      switch (attribute) {
      case CKA_OTP_FORMAT:
      case CKA_OTP_LENGTH:
      case CKA_OTP_TIME:
      case CKA_OTP_USER_FRIENDLY_MODE:
      case CKA_OTP_CHALLENGE_REQUIREMENT:
      case CKA_OTP_TIME_REQUIREMENT:
      case CKA_OTP_COUNTER_REQUIREMENT:
      case CKA_OTP_PIN_REQUIREMENT:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }


  } while (0);
  return rv;
}

CK_RV setArrayAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    tlp11_container_object_t *object,
    const uint8_t *value,
    const uint32_t value_len
    )
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  if (!object || !value)
    return CKR_GENERAL_ERROR;

  do {
    if (IS_TYPE(object,tlp11_type_storage)) {
      if (object->storage.modifiable == 0) {
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      }
    }
    if (IS_TYPE(object,tlp11_type_storage)) {
      switch (attribute) {
      case CKA_LABEL:
        if (value_len > LABEL_LEN) {
          rv = CKR_ATTRIBUTE_VALUE_INVALID;
        } else {
          memscpy(object->storage.label,LABEL_LEN,value,value_len);
          object->storage.label_len = value_len;
          rv = CKR_OK;
        }
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    // none of the field in DATA can be changed according to specs
    // following specs, even if they look odd...
    if (IS_TYPE(object,tlp11_type_data)) {
      switch (attribute) {
      case CKA_APPLICATION:
      case CKA_OBJECT_ID:
      case CKA_VALUE:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    // same story for certificates, nothing can be changed
    if (IS_TYPE(object,tlp11_type_certificate)) {
      switch (attribute) {
      case CKA_CHECK_VALUE:
      case CKA_START_DATE:
      case CKA_END_DATE:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
    // same story for x.509 certificates
    if (IS_TYPE(object,tlp11_type_certificate_x509)) {
      switch (attribute) {
      case CKA_SUBJECT:
      case CKA_ID:
      case CKA_ISSUER:
      case CKA_SERIAL_NUMBER:
      case CKA_VALUE:
      case CKA_URL:
      case CKA_HASH_OF_SUBJECT_PUBLIC_KEY:
      case CKA_HASH_OF_ISSUER_PUBLIC_KEY:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_key)) {
      switch (attribute) {
      case CKA_ALLOWED_MECHANISMS:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      case CKA_ID:
        if (sticky(g_token_0_ptr)) {
          rv = CKR_ATTRIBUTE_READ_ONLY;
          break;
        }
        if (value_len > ID_MAX_LEN) {
          rv = CKR_ATTRIBUTE_VALUE_INVALID;
        } else {
          memscpy(object->key.id,ID_MAX_LEN,value,value_len);
          object->key.id_len = value_len;
          rv = CKR_OK;
        }
        break;
      case CKA_START_DATE:
        if (value_len != sizeof(CK_DATE)) {
          rv = CKR_ATTRIBUTE_VALUE_INVALID;
        } else {
          memscpy(&object->key.start_date,sizeof(CK_DATE),value,value_len);
          rv = CKR_OK;
        }
        break;
      case CKA_END_DATE:
        if (value_len != sizeof(CK_DATE)) {
          rv = CKR_ATTRIBUTE_VALUE_INVALID;
        } else {
          memscpy(&object->key.end_date,sizeof(CK_DATE),value,value_len);
          rv = CKR_OK;
        }
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_public_key)) {
      switch (attribute) {
      case CKA_SUBJECT:
        if (value_len > SUBJECT_MAX_LEN) {
          rv = CKR_ATTRIBUTE_VALUE_INVALID;
        } else {
          memscpy(object->public_key.subject,SUBJECT_MAX_LEN,value,value_len);
          object->public_key.subject_len = value_len;
          rv = CKR_OK;
        }
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
    if (IS_TYPE(object,tlp11_type_private_key)) {
      switch (attribute) {
      case CKA_SUBJECT:
        if (value_len > SUBJECT_MAX_LEN) {
          rv = CKR_ATTRIBUTE_VALUE_INVALID;
        } else {
          memscpy(object->private_key.subject,SUBJECT_MAX_LEN,value,value_len);
          object->private_key.subject_len = value_len;
          rv = CKR_OK;
        }
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_rsa_public_key)) {
      switch (attribute) {
      case CKA_MODULUS:
      case CKA_PUBLIC_EXPONENT:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
    if (IS_TYPE(object,tlp11_type_rsa_private_key)) {
      switch (attribute) {
      case CKA_MODULUS:
      case CKA_PUBLIC_EXPONENT:
      case CKA_PRIVATE_EXPONENT:
      case CKA_PRIME_1:
      case CKA_PRIME_2:
      case CKA_EXPONENT_1:
      case CKA_EXPONENT_2:
      case CKA_COEFFICIENT:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_secret_key)) {
      switch (attribute) {
      case CKA_CHECK_VALUE:
      case CKA_VALUE:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
    if (IS_TYPE(object,tlp11_type_otp_key)) {
      switch (attribute) {
      case CKA_OTP_COUNTER:
      case CKA_OTP_USER_IDENTIFIER:
      case CKA_OTP_SERVICE_IDENTIFIER:
      case CKA_OTP_SERVICE_LOGO:
      case CKA_OTP_SERVICE_LOGO_TYPE:
        rv = CKR_ATTRIBUTE_READ_ONLY;
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

  } while(0);
  return rv;
}

/**
 * This is an internal function, so we require the input buffer to be
 * the max we can ever need
 */
CK_RV getTemplateArrayAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    tlp11_container_object_t *object,
    uint8_t *value,
    uint32_t *value_len
    )
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  uint8_t* src = NULL;
  if (!object || !value || !value_len)
    return CKR_GENERAL_ERROR;

  TL_LOG_I("+getArrayAttributeValue(0x%08X)",attribute);

  do {
    if (*value_len < SUBJECT_MAX_LEN) {
      *value_len = SUBJECT_MAX_LEN;
      rv = CKR_BUFFER_TOO_SMALL;
      break;
    }

    if (!(IS_TYPE(object,tlp11_type_secret_key) || IS_TYPE(object,tlp11_type_private_key))) {
      TL_LOG_E("This function can be called only on keys that can be wrapped.");
      rv = CKR_KEY_HANDLE_INVALID;
      break;
    }

    if (IS_TYPE(object,tlp11_type_storage)) {
      switch (attribute) {
      case CKA_LABEL:
        *value_len = object->storage.label_len;
        src = (uint8_t*)(object->storage.label);
        break;
      default: break;
      }
      if (src != NULL) break;
    }

    if (IS_TYPE(object,tlp11_type_key)) {
      switch (attribute) {
      case CKA_ALLOWED_MECHANISMS:
        *value_len = sizeof(object->key.allowed_mechanisms);
        src = (uint8_t*)(object->key.allowed_mechanisms);
        break;
      case CKA_ID:
        *value_len = object->key.id_len;
        src = object->key.id;
        break;
      case CKA_START_DATE:
        *value_len = sizeof(object->key.start_date);
        src = (uint8_t*)(&object->key.start_date);
        break;
      case CKA_END_DATE:
        *value_len = sizeof(object->key.end_date);
        src = (uint8_t*)(&object->key.end_date);
        break;
      default: break;
      }
      if (src != NULL) break;
    }

    if (IS_TYPE(object,tlp11_type_private_key)) {
      switch (attribute) {
      case CKA_SUBJECT:
        *value_len = object->private_key.subject_len;
        src = (uint8_t*)(object->private_key.subject);
        break;
      default: break;
      }
      if (src != NULL) break;
    }

    if (IS_TYPE(object,tlp11_type_rsa_private_key)) {
      switch (attribute) {
      case CKA_MODULUS:
        *value_len = object->rsa_private_key.modulus_len;
        src = object->rsa_private_key.modulus;
        break;
      case CKA_PUBLIC_EXPONENT:
        *value_len = object->rsa_private_key.public_exponent_len;
        src = object->rsa_private_key.public_exponent;
        break;
      default: break;
      }
      if (src != NULL) break;
    }

    if (IS_TYPE(object,tlp11_type_secret_key)) {
      switch (attribute) {
      case CKA_CHECK_VALUE:
        *value_len = sizeof(object->secret_key.check_value);
        src = object->secret_key.check_value;
        break;
      default: break;
      }
      if (src != NULL) break;
    }
    if (IS_TYPE(object,tlp11_type_otp_key)) {
      switch (attribute) {
      case CKA_OTP_COUNTER:
        *value_len = sizeof(object->otp.counter);
        src = object->otp.counter;
        break;
      case CKA_OTP_TIME:
        *value_len = sizeof(object->otp.time);
        src = (uint8_t *)(object->otp.time);
        break;
      case CKA_OTP_USER_IDENTIFIER:
        *value_len = sizeof(object->otp.user_identifier);
        src = (uint8_t *)(object->otp.user_identifier);
        break;
      case CKA_OTP_SERVICE_IDENTIFIER:
        *value_len = sizeof(object->otp.service_identifier);
        src = (uint8_t *)(object->otp.service_identifier);
        break;
      case CKA_OTP_SERVICE_LOGO:
        *value_len = object->otp.service_logo_len;
        src = object->otp.service_logo;
        break;
      case CKA_OTP_SERVICE_LOGO_TYPE:
        *value_len = sizeof(object->otp.service_logo_type);
        src = (uint8_t *)(object->otp.service_logo_type);
        break;
      default: break;
      }
      if (src != NULL) break;
    }
  } while(0);
  if (src != NULL) {
    memscpy(value,*value_len,src,*value_len);
    rv = CKR_OK;
  } else {
    TL_LOG_E("Array attribute 0x%08X invalid for a wrap template",attribute);
    rv = CKR_ATTRIBUTE_TYPE_INVALID;
  }
  TL_LOG_I("-getArrayAttributeValue(0x%08X): 0x%08X",attribute,rv);
  return rv;
}

CK_RV getTemplateBoolAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    CK_BBOOL *value)
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  if (!object || !value)
    return CKR_GENERAL_ERROR;
  TL_LOG_I("+getTemplateBoolAttributeValue(0x%08X)",attribute);

  do {
    if (!(IS_TYPE(object,tlp11_type_secret_key) || IS_TYPE(object,tlp11_type_private_key))) {
      TL_LOG_E("This function can be called only on keys that can be wrapped.");
      rv = CKR_KEY_HANDLE_INVALID;
      break;
    }

    if (IS_TYPE(object,tlp11_type_storage)) {
      switch (attribute) {
      case CKA_TOKEN:
        getValue(object->storage.token,value);
        break;
      case CKA_PRIVATE:
        getValue(object->storage.prvt,value);
        break;
      case CKA_MODIFIABLE:
        getValue(object->storage.modifiable,value);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_key)) {
      switch (attribute) {
      case CKA_LOCAL:
        getValue(object->key.local,value);
        break;
      case CKA_DERIVE:
        getValue(object->key.derive,value);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_private_key)) {
      switch (attribute) {
      case CKA_ALWAYS_AUTHENTICATE:
        getValue(object->private_key.always_authenticate,value);
        break;
      case CKA_ALWAYS_SENSITIVE:
        getValue(object->private_key.always_sensitive,value);
        break;
      case CKA_NEVER_EXTRACTABLE:
        getValue(object->private_key.never_extractable,value);
        break;
      case CKA_SENSITIVE:
        getValue(object->private_key.sensitive,value);
        break;
      case CKA_DECRYPT:
        getValue(object->private_key.decrypt,value);
        break;
      case CKA_SIGN:
        getValue(object->private_key.sign,value);
        break;
      case CKA_SIGN_RECOVER:
        getValue(object->private_key.sign_recovery,value);
        break;
      case CKA_UNWRAP:
        getValue(object->private_key.unwrap,value);
        break;
      case CKA_EXTRACTABLE:
        getValue(object->private_key.extractable,value);
        break;
      case CKA_WRAP_WITH_TRUSTED:
        getValue(object->private_key.wrap_with_trusted,value);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_secret_key)) {
      switch (attribute) {
      case CKA_ALWAYS_SENSITIVE:
        getValue(object->secret_key.always_sensitive,value);
        break;
      case CKA_NEVER_EXTRACTABLE:
        getValue(object->secret_key.never_extractable,value);
        break;
      case CKA_SENSITIVE:
        getValue(object->secret_key.sensitive,value);
        break;
      case CKA_ENCRYPT:
        getValue(object->secret_key.encrypt,value);
        break;
      case CKA_DECRYPT:
        getValue(object->secret_key.decrypt,value);
        break;
      case CKA_SIGN:
        getValue(object->secret_key.sign,value);
        break;
      case CKA_VERIFY:
        getValue(object->secret_key.verify,value);
        break;
      case CKA_WRAP:
        getValue(object->secret_key.wrap,value);
        break;
      case CKA_UNWRAP:
        getValue(object->secret_key.unwrap,value);
        break;
      case CKA_EXTRACTABLE:
        getValue(object->secret_key.extractable,value);
        break;
      case CKA_WRAP_WITH_TRUSTED:
        getValue(object->secret_key.wrap_with_trusted,value);
        break;
      case CKA_TRUSTED:
        getValue(object->secret_key.trusted,value);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
    if (IS_TYPE(object,tlp11_type_otp_key)) {
      switch (attribute) {
      case CKA_OTP_USER_FRIENDLY_MODE:
        getValue(object->otp.user_friendly_mode,value);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
  } while (0);
  TL_LOG_I("-getTemplateBoolAttributeValue(0x%08X): 0x%08X",attribute,rv);
  return rv;
}

CK_RV getTemplateULongAttributeValue(
    const CK_ATTRIBUTE_TYPE attribute,
    const tlp11_container_object_t *object,
    CK_ULONG *value)
{
  CK_RV rv = CKR_ATTRIBUTE_TYPE_INVALID;
  if (!object || !value)
    return CKR_GENERAL_ERROR;
  TL_LOG_I("+getTemplateULongAttributeValue(0x%08X)",attribute);

  do {
    if (!(IS_TYPE(object,tlp11_type_secret_key) || IS_TYPE(object,tlp11_type_private_key))) {
      TL_LOG_E("This function can be called only on keys that can be wrapped.");
      rv = CKR_KEY_HANDLE_INVALID;
      break;
    }

    if (attribute == CKA_CLASS) {
      getValue(object->object.cls,value);
      break;
    }

    if (IS_TYPE(object,tlp11_type_key)) {
      switch (attribute) {
      case CKA_KEY_TYPE:
        getValue(object->key.key_type,value);
        break;
      case CKA_KEY_GEN_MECHANISM:
        getValue(object->key.key_gen_mechanism,value);
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }

    if (IS_TYPE(object,tlp11_type_secret_key)) {
      switch (attribute) {
      case CKA_VALUE_LEN:
        if (IS_TYPE(object,tlp11_type_otp_key)) {
          getValue(object->otp.len,value);
        } else {
          getValue(object->secret_key_simple.len,value);
        }
        break;
      default: break;
      }
      if (rv != CKR_ATTRIBUTE_TYPE_INVALID) {
        break;
      }
    }
  } while (0);
  TL_LOG_I("-getTemplateULongAttributeValue(0x%08X): 0x%08X",attribute,rv);
  return rv;
}
