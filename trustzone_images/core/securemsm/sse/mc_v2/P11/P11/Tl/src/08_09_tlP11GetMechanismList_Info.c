/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlLog.h"
#include "tlP11Glob.h"
#include "tlP11Mechanisms.h"
#include "aes.h"
#include "des.h"
#include "common.h"
#include "generic.h"
#include "hmac.h"
#include "hotp.h"
#include "pbkd.h"
#include "rsa.h"
#include "sha.h"
#include "so.h"
#include "securityExt.h"
#include "ssl.h"
#include "tls.h"

tlp11_mechanism_t mechanisms[] = {
    /* RSA */
  {
    CKM_RSA_PKCS_KEY_PAIR_GEN,
    {1024,2048,CKF_GENERATE_KEY_PAIR},
    tlp11_type_rsa_public_key|tlp11_type_rsa_private_key,
    {
      NULL,         /* cipher_init */
      NULL,         /* cipher_update */
      NULL,         /* cipher_final */
      NULL,         /* digest_init */
      NULL,         /* digest_update */
      NULL,         /* digest_final */
      NULL,         /* sign_init */
      NULL,         /* sign_update */
      NULL,         /* sign_final */
      NULL,         /* generate */
      rsa_generate, /* generate pair */
      NULL,         /* wrap */
      NULL,         /* unwrap */
      NULL          /* derive */
    }
  },
  {
    CKM_RSA_PKCS,
    {1024,2048,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_rsa_public_key|tlp11_type_rsa_private_key,
    {
      rsa_cipher_init, /* cipher_init */
      common_cipher_update, /* cipher_update */
      common_cipher_final, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      rsa_wrap, /* wrap */
      rsa_unwrap, /* unwrap */
      NULL  /* derive */
    }
  },
  //{CKM_RSA_PKCS_OAEP,{512,2048,CKF_UNWRAP},tlp11_type_rsa_public_key|tlp11_type_rsa_private_key},
  {
    CKM_RSA_PKCS_PSS,
    {1024,2048,CKF_VERIFY},
    tlp11_type_rsa_public_key|tlp11_type_rsa_private_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      rsa_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      rsa_sign_verify_final, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  //{CKM_RSA_9796,{512,2048,CKF_UNWRAP},tlp11_type_rsa_public_key|tlp11_type_rsa_private_key},
  //CKM_RSA_X_509 too dangerous!!
  {
    CKM_SHA1_RSA_PKCS,
    {1024,2048,CKF_SIGN|CKF_VERIFY},
    tlp11_type_rsa_public_key|tlp11_type_rsa_private_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      rsa_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      rsa_sign_verify_final, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  //{CKM_SHA256_RSA_PKCS,{512,2048,0},tlp11_type_rsa_public_key|tlp11_type_rsa_private_key},
  {
    CKM_SHA1_RSA_PKCS_PSS,
    {1024,2048,CKF_VERIFY},
    tlp11_type_rsa_public_key|tlp11_type_rsa_private_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      rsa_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      rsa_sign_verify_final, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_SHA256_RSA_PKCS_PSS,
    {1024,2048,CKF_VERIFY},
    tlp11_type_rsa_public_key|tlp11_type_rsa_private_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      rsa_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      rsa_sign_verify_final, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  /* RC4 */
//    CKM_RC4_KEY_GEN,
//    {CKM_RC4,{40,2048,0}},
#ifdef ENABLE_DES
  /* DES */
  {
    CKM_DES_KEY_GEN,
    {64,64,CKF_GENERATE},
    tlp11_type_des_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      des_generate,  /* generate */
      NULL,          /* generate pair */
      NULL,          /* wrap */
      NULL,          /* unwrap */
      NULL           /* derive */
    }
  },
  {
    CKM_DES_CBC,
    {64,64,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_des_key,
    {
      des_cipher_init,      /* cipher_init */
      common_cipher_update, /* cipher_update */
      common_cipher_final,  /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      des_wrap,    /* wrap */
      des_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
//    {CKM_DES_MAC, NULL_INFO},
//    {CKM_DES_MAC_GENERAL,NULL_INFO},
//    CKM_DES_CBC_PAD,
  {
    CKM_DES_CBC_PAD,
    {64,64,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_des_key,
    {
      des_cipher_init,      /* cipher_init */
      common_cipher_update, /* cipher_update */
      common_cipher_final,  /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      des_wrap,    /* wrap */
      des_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
  /* DES3 */
  {
    CKM_DES3_KEY_GEN,
    {192,192,CKF_GENERATE},
    tlp11_type_des3_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      des_generate,  /* generate */
      NULL,          /* generate pair */
      NULL,          /* wrap */
      NULL,          /* unwrap */
      NULL           /* derive */
    }
  },
//    CKM_DES3_ECB,
  {
    CKM_DES3_CBC,
    {192,192,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_des3_key,
    {
      des_cipher_init,      /* cipher_init */
      common_cipher_update, /* cipher_update */
      common_cipher_final,  /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      des_wrap,    /* wrap */
      des_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
//    {CKM_DES3_MAC, NULL_INFO},
//    {CKM_DES3_MAC_GENERAL,NULL_INFO},
  {
    CKM_DES3_CBC_PAD,
    {192,192,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_des3_key,
    {
      des_cipher_init,      /* cipher_init */
      common_cipher_update, /* cipher_update */
      common_cipher_final,  /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      des_wrap,    /* wrap */
      des_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
  {
    CKM_DES3_CBC_ENCRYPT_DATA,
    {0,0,CKF_DERIVE},
    tlp11_type_des3_key,
    {
      NULL,        /* cipher_init */
      NULL,        /* cipher_update */
      NULL,        /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      NULL,        /* wrap */
      NULL,        /* unwrap */
      des_encrypt_data         /* derive */
    }
  },
  {
    CKM_DES_CBC_ENCRYPT_DATA,
    {0,0,CKF_DERIVE},
    tlp11_type_des_key,
    {
      NULL,        /* cipher_init */
      NULL,        /* cipher_update */
      NULL,        /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      NULL,        /* wrap */
      NULL,        /* unwrap */
      des_encrypt_data         /* derive */
    }
  },
#endif
  /* MD2 */
//    CKM_MD2,
  /* MD5 */
//    CKM_MD5,
  /* SHA1 */
  {
    CKM_SHA_1,
    {0,0,CKF_DIGEST},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      sha_digest_init, /* digest_init */
      sha_digest_update, /* digest_update */
      sha_digest_final, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_SHA_1_HMAC,
    {PASSWORD_MIN_LEN*8,GENERIC_KEY_MAX_LEN*8,CKF_SIGN|CKF_VERIFY},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      hmac_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      hmac_sign_verify_final, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_SHA_1_HMAC_GENERAL,
    {PASSWORD_MIN_LEN*8,GENERIC_KEY_MAX_LEN*8,CKF_SIGN|CKF_VERIFY},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      hmac_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      hmac_sign_verify_final, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  /* SHA256 */
  {
    CKM_SHA256,
    {0,0,CKF_DIGEST},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      sha_digest_init, /* digest_init */
      sha_digest_update, /* digest_update */
      sha_digest_final, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_SHA256_HMAC,
    {PASSWORD_MIN_LEN*8,GENERIC_KEY_MAX_LEN*8,CKF_SIGN|CKF_VERIFY},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      hmac_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      hmac_sign_verify_final, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_SHA256_HMAC_GENERAL,
    {PASSWORD_MIN_LEN*8,GENERIC_KEY_MAX_LEN*8,CKF_SIGN|CKF_VERIFY},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      hmac_sign_verify_init, /* sign_init */
      common_sign_verify_update, /* sign_update */
      hmac_sign_verify_final, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  /* Generic Secret Key*/
  {
    CKM_GENERIC_SECRET_KEY_GEN,
    {GENERIC_KEY_MIN_LEN*8,GENERIC_KEY_MAX_LEN*8,CKF_GENERATE},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      generic_generate, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_GENERIC_SECRET_KEY_GEN_KEYPAD,
    {PASSWORD_MIN_LEN*8,PASSWORD_MAX_LEN*8,CKF_GENERATE},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
#ifdef ENABLE_WEAK_DERIVE
  {
    CKM_CONCATENATE_BASE_AND_KEY,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      derive_base_key      /* derive */
    }
  },
  {
    CKM_CONCATENATE_BASE_AND_DATA,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      derive_data_base      /* derive */
    }
  },
  {
    CKM_CONCATENATE_DATA_AND_BASE,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      derive_data_base      /* derive */
    }
  },
  {
    CKM_XOR_BASE_AND_DATA,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      derive_data_base      /* derive */
    }
  },
  {
    CKM_EXTRACT_KEY_FROM_KEY,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      derive_extract      /* derive */
    }
  },
#endif //ENABLE_WEAK_DERIVE
  {
    CKM_SHA1_KEY_DERIVATION,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      sha_derive      /* derive */
    }
  },
  {
    CKM_SHA256_KEY_DERIVATION,
    {0,0,CKF_DERIVE},
    tlp11_type_secret_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      NULL, /* generate */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      sha_derive      /* derive */
    }
  },
  /* Password based key generation */
//    {CKM_PBE_MD5_DES_CBC, NULL_INFO},
//    CKM_PBE_SHA1_RC4_128,
//    CKM_PBE_SHA1_RC4_40,
//    CKM_PBE_SHA1_DES3_EDE_CBC,
  {
    CKM_PKCS5_PBKD2,
    {PASSWORD_MIN_LEN*8,PASSWORD_MAX_LEN*8,CKF_GENERATE},
    tlp11_type_secret_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      pkcs5_pbkd2,   /* generate */
      NULL,          /* generate pair */
      NULL,          /* wrap */
      NULL,          /* unwrap */
      NULL           /* derive */
    }
  },
  {
    CKM_PKCS5_PBKD2_GENERIC_SECRET_KEY,
    {PASSWORD_MIN_LEN*8,PASSWORD_MAX_LEN*8,CKF_GENERATE},
    tlp11_type_secret_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      pkcs5_pbkd2_generic_secret,   /* generate */
      NULL,          /* generate pair */
      NULL,          /* wrap */
      NULL,          /* unwrap */
      NULL           /* derive */
    }
  },
//    CKM_PBA_SHA1_WITH_SHA1_HMAC,
  /* AES */
  {
    CKM_AES_KEY_GEN,
    {128,256,CKF_GENERATE},
    tlp11_type_aes_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      aes_generate,  /* generate */
      NULL,          /* generate pair */
      NULL,          /* wrap */
      NULL,          /* unwrap */
      NULL           /* derive */
    }
  },
  {
    CKM_AES_ECB,
    {128,256,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_aes_key,
    {
      aes_cipher_init,        /* cipher_init */
      common_cipher_update,      /* cipher_update */
      common_cipher_final,       /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      aes_wrap,    /* wrap */
      aes_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
  {
    CKM_AES_CBC,
    {128,256,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_aes_key,
    {
      aes_cipher_init,        /* cipher_init */
      common_cipher_update,      /* cipher_update */
      common_cipher_final,       /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      aes_wrap,    /* wrap */
      aes_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
  {
    CKM_AES_CBC_PAD,
    {128,256,CKF_ENCRYPT|CKF_DECRYPT|CKF_WRAP|CKF_UNWRAP},
    tlp11_type_aes_key,
    {
      aes_cipher_init,        /* cipher_init */
      common_cipher_update,      /* cipher_update */
      common_cipher_final,       /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      aes_wrap,    /* wrap */
      aes_unwrap,  /* unwrap */
      NULL         /* derive */
    }
  },
//    CKM_AES_MAC_GENERAL,
  /* Key derivation by data encryption */
  {
    CKM_AES_ECB_ENCRYPT_DATA,
    {128,256,CKF_DERIVE},
    tlp11_type_aes_key,
    {
      NULL,        /* cipher_init */
      NULL,        /* cipher_update */
      NULL,        /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      NULL,        /* wrap */
      NULL,        /* unwrap */
      aes_encrypt_data         /* derive */
    }
  },
  {
    CKM_AES_CBC_ENCRYPT_DATA,
    {128,256,CKF_DERIVE},
    tlp11_type_aes_key,
    {
      NULL,        /* cipher_init */
      NULL,        /* cipher_update */
      NULL,        /* cipher_final */
      NULL,        /* digest_init */
      NULL,        /* digest_update */
      NULL,        /* digest_final */
      NULL,        /* sign_init */
      NULL,        /* sign_update */
      NULL,        /* sign_final */
      NULL,        /* generate */
      NULL,        /* generate pair */
      NULL,        /* wrap */
      NULL,        /* unwrap */
      aes_encrypt_data         /* derive */
    }
  },
  {
    CKM_SO_KEY_WRAP,
    {0,0,CKF_UNWRAP},
    tlp11_type_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      NULL,          /* generate */
      NULL,          /* generate pair */
      NULL,          /* wrap */
      so_unwrap,     /* unwrap */
      NULL           /* derive */
    }
  },
  {
    CKM_WRAPKEY_AES_CBC_PAD,
    {256,256,CKF_WRAP|CKF_UNWRAP},
    tlp11_type_aes_key,
    {
      NULL,          /* cipher_init */
      NULL,          /* cipher_update */
      NULL,          /* cipher_final */
      NULL,          /* digest_init */
      NULL,          /* digest_update */
      NULL,          /* digest_final */
      NULL,          /* sign_init */
      NULL,          /* sign_update */
      NULL,          /* sign_final */
      NULL,          /* generate */
      NULL,          /* generate pair */
      wrapkey_aes_cbc_pad_wrap,          /* wrap */
      wrapkey_aes_cbc_pad_unwrap,     /* unwrap */
      NULL           /* derive */
    }
  },
  // OTP
  {
    CKM_HOTP_KEY_GEN,
    {OTP_KEY_MIN_LEN*8,OTP_KEY_MAX_LEN*8,CKF_GENERATE},
    tlp11_type_otp_key,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      NULL, /* sign_init */
      NULL, /* sign_update */
      NULL, /* sign_final */
      generic_generate, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  {
    CKM_HOTP,
    {OTP_KEY_MIN_LEN*8,OTP_KEY_MAX_LEN*8,CKF_SIGN|CKF_VERIFY},
    0,
    {
      NULL, /* cipher_init */
      NULL, /* cipher_update */
      NULL, /* cipher_final */
      NULL, /* digest_init */
      NULL, /* digest_update */
      NULL, /* digest_final */
      hotp_sign_init, /* sign_init */
      NULL, /* sign_update */
      hotp_sign_final, /* sign_final */
      NULL, /* generate, ad-hoc handled */
      NULL, /* generate pair */
      NULL, /* wrap */
      NULL, /* unwrap */
      NULL  /* derive */
    }
  },
  //SSL
  {
      CKM_SSL3_PRE_MASTER_KEY_GEN,
      {(48*8),(48*8),CKF_GENERATE},
      0,
      {
        NULL, /* cipher_init */
        NULL, /* cipher_update */
        NULL, /* cipher_final */
        NULL, /* digest_init */
        NULL, /* digest_update */
        NULL, /* digest_final */
        NULL, /* sign_init */
        NULL, /* sign_update */
        NULL, /* sign_final */
        tls_pre_master_key, /* generate, ad-hoc handled */
        NULL, /* generate pair */
        NULL, /* wrap */
        NULL, /* unwrap */
        NULL  /* derive */
      }
  },
  {
    CKM_SSL3_MASTER_KEY_DERIVE,
    {(48*8),(48*8),CKF_DERIVE},
    0,
    {
        NULL, /* cipher_init */
        NULL, /* cipher_update */
        NULL, /* cipher_final */
        NULL, /* digest_init */
        NULL, /* digest_update */
        NULL, /* digest_final */
        NULL, /* sign_init */
        NULL, /* sign_update */
        NULL, /* sign_final */
        NULL, /* generate, ad-hoc handled */
        NULL, /* generate pair */
        NULL, /* wrap */
        NULL, /* unwrap */
        derive_master_key  /* derive */
    }
  },
  //TLS
  { CKM_TLS_PRE_MASTER_KEY_GEN,
    {(48*8),(48*8),CKF_GENERATE},
    0,
    {   NULL, /* cipher_init */
        NULL, /* cipher_update */
        NULL, /* cipher_final */
        NULL, /* digest_init */
        NULL, /* digest_update */
        NULL, /* digest_final */
        NULL, /* sign_init */
        NULL, /* sign_update */
        NULL, /* sign_final */
        tls_pre_master_key, /* generate, ad-hoc handled */
        NULL, /* generate pair */
        NULL, /* wrap */
        NULL, /* unwrap */
        NULL  /* derive */
    }
  },
  { CKM_TLS_MASTER_KEY_DERIVE,
    {(48*8),(48*8),CKF_DERIVE},
    0,
    {   NULL, /* cipher_init */
        NULL, /* cipher_update */
        NULL, /* cipher_final */
        NULL, /* digest_init */
        NULL, /* digest_update */
        NULL, /* digest_final */
        NULL, /* sign_init */
        NULL, /* sign_update */
        NULL, /* sign_final */
        NULL, /* generate, ad-hoc handled */
        NULL, /* generate pair */
        NULL, /* wrap */
        NULL, /* unwrap */
        tls_derive_master_secret  /* derive */
    }
  },

  { CKM_TLS_KEY_AND_MAC_DERIVE,
    {(48*8),(48*8),CKF_DERIVE},
    0,
    {   NULL, /* cipher_init */
        NULL, /* cipher_update */
        NULL, /* cipher_final */
        NULL, /* digest_init */
        NULL, /* digest_update */
        NULL, /* digest_final */
        NULL, /* sign_init */
        NULL, /* sign_update */
        NULL, /* sign_final */
        NULL, /* generate, ad-hoc handled */
        NULL, /* generate pair */
        NULL, /* wrap */
        NULL, /* unwrap */
        tls_derive_key_and_mac  /* derive */
    }
  },
  { CKM_TLS_PRF,
    {(48*8),(48*8),CKF_DERIVE},
    0,
    {   NULL, /* cipher_init */
        NULL, /* cipher_update */
        NULL, /* cipher_final */
        NULL, /* digest_init */
        NULL, /* digest_update */
        NULL, /* digest_final */
        NULL, /* sign_init */
        NULL, /* sign_update */
        NULL, /* sign_final */
        NULL, /* generate, ad-hoc handled */
        NULL, /* generate pair */
        NULL, /* wrap */
        NULL, /* unwrap */
        tls_pseudorandom_function  /* derive */
    }
  },
};

/** Process a GetMechanismList command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGetMechanismList(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  unsigned int i = 0;
  unsigned int j = 0;

  do {
    TL_LOG_I("processCmdGetMechanismList+");

    memset(tci_message->rspGetMechanismList.params.mechanisms,0xFF,MECHANISM_LIST_LEN*sizeof(CK_MECHANISM_TYPE));

    if (MECHANISM_LIST_LEN < LENGTH(mechanisms)) { /*lint !e506 !e774*/
      TL_LOG_W("Internal error: internal buffer for mechanism list too small!");
    }

    for(; i<min(LENGTH(mechanisms),MECHANISM_LIST_LEN); i++) { /*lint !e506*/
      if (mechanismIsSecure(mechanisms[i].mechanism)) {
        tci_message->rspGetMechanismList.params.mechanisms[j++] = mechanisms[i].mechanism;
      }
    }
    tci_message->rspGetMechanismList.params.mechanisms_len = j;

  } while(0);

	TL_LOG_I("processCmdGetMechanismList- ret:0x%08X",ret);
  return ret;
}

CK_RV getMechanismData(
    CK_MECHANISM_TYPE mechanism,
    tlp11_mechanism_t *pMech)
{
  CK_RV ret = CKR_ARGUMENTS_BAD;
  bool_t bFound = FALSE;
  TL_LOG_I("getMechanismInfo+");
  if (pMech != NULL) {
    unsigned int i = 0;
    for(i=0; i<LENGTH(mechanisms); i++) {
      if (mechanism == mechanisms[i].mechanism) {
        if (mechanismIsSecure(mechanism)) {
          *pMech = mechanisms[i];
          pMech->info.flags &= g_config.arg.enabledFunctions;
          if ((secureWrapOnly(NULL)) &&
              (mechanism != CKM_WRAPKEY_AES_CBC_PAD)) {
            TL_LOG_I("secureWrapOnly: removing wrapping capabilities to mechanism 0x%08X",mechanism);
            pMech->info.flags &= (~CKF_WRAP);
          }
          bFound = TRUE;
          ret = CKR_OK;
          TL_LOG_I("Mechanism 0x%08X found. {%d,%d,%d,%08X}",
              mechanism,
              pMech->info.ulMinKeySize,
              pMech->info.ulMaxKeySize,
              pMech->info.flags,
              mechanisms[i].compatibleType
              );
        } else {
          TL_LOG_E("Mechanism 0x%08X not secure, filtered",mechanism);
        }
        break;
      }
    }
    if (!bFound) {
      TL_LOG_E("Mechanism 0x%08X not supported",mechanism);
      ret = CKR_MECHANISM_INVALID;
    }
  }
  TL_LOG_I("getMechanismInfo- ret:0x%08X",ret);
  return ret;
}

/** Process a GetMechanismInfo command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGetMechanismInfo(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlp11_mechanism_t mech;

  do {
    TL_LOG_I("processCmdGetMechanismInfo+");

    ret = getMechanismData(
        tci_message->cmdGetMechanismInfo.params.mechanism,
        &mech);
    if (ret == CKR_OK)
      tci_message->rspGetMechanismInfo.params.info = mech.info;

  } while(0);

  TL_LOG_I("processCmdGetMechanismInfo- ret:0x%08X",ret);
  return ret;
}
