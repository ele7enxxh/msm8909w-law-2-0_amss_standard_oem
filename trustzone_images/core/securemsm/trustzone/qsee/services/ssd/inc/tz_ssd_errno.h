#ifndef SSD_ERRNO_H
#define SSD_ERRNO_H

/**
   @file ssd_errno.h
   @brief extend TZ error codes
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/inc/tz_ssd_errno.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/27/12   pre      Initial version.

===========================================================================*/


/*===========================================================================

                              DEFINES

===========================================================================*/

#include "tzbsp_errno.h"

#define SSD_B TZBSP_EC_SSD_ERROR_BASE /* 200 */

enum
{
  /* tzbsp_ssd_protect_keystore_ns */
  TZBSP_EC_SSD_PKS_IP_CHK              = SSD_B,
  TZBSP_EC_SSD_PKS_OVERFLOW_SPTR,
  TZBSP_EC_SSD_PKS_KS_LEN,
  TZBSP_EC_SSD_PKS_IP_CHK_OVERFLOW,
  TZBSP_EC_SSD_PKS_SPTR,
  TZBSP_EC_SSD_PKS_ALIGN,
  TZBSP_EC_SSD_PKS_XPU_FAILED,

  /* tzbsp_ssd_decrypt_img_ns */
  TZBSP_EC_SSD_DI_OVERFLOW             = SSD_B + 10,
  TZBSP_EC_SSD_DI_SPTR,
  TZBSP_EC_SSD_DI_OVERFLOW_ALIGN,
  TZBSP_EC_SSD_DI_SPTR_2,
  TZBSP_EC_SSD_DI_IP_CHK,

  /* tzbsp_ssd_decrypt_elf_seg_frag_ns */
  TZBSP_EC_SSD_DESF_OP_CHK             = SSD_B + 20,
  TZBSP_EC_SSD_DESF_OP_SPTR_OVERFLOW,
  TZBSP_EC_SSD_DESF_IP_CHK,
  TZBSP_EC_SSD_DESF_NO_MD_CTX_ID,
  TZBSP_EC_SSD_DESF_CTX_NOT_FOUND,

  /* tzbsp_ssd_parse_md_ns */
  TZBSP_EC_SSD_PMDNS_OP_CHK            = SSD_B + 30,
  TZBSP_EC_SSD_PMDNS_IP_CHK,

  /* tzbsp_ssd_decrypt_elf_seg_frag_s */
  TZBSP_EC_SSD_DESFS_IP_CHK            = SSD_B + 40,
  TZBSP_EC_SSD_DESFS_INVALID_CTX,
  TZBSP_EC_SSD_DESFS_DECRYPT_FAILED,

  /* tzbsp_ssd_parse_md_s */
  TZBSP_EC_SSD_PMDS_IP_CHK             = SSD_B + 50,

  /* tzbsp_ssd_decrypt_elf_seg_frag_i */
  TZBSP_EC_SSD_DESFI_LEN_ALIGN         = SSD_B + 60,
  TZBSP_EC_SSD_DESFI_DEC_FAILED,
  TZBSP_EC_SSD_DESFI_BAD_SEGMENT_NUMBER,
  TZBSP_EC_SSD_DESFI_KDF_IV,

  /* tzbsp_ssd_parse_md_i */
  TZBSP_EC_SSD_PMDI_PARSING_FAILED     = SSD_B + 70,
  TZBSP_EC_SSD_PMDI_SETUP_CIPHER_FAILED,

  /* ssd_tz_decrypt_segment */
  TZBSP_EC_SSD_DS_IP_CHK               = SSD_B + 80,
  TZBSP_EC_SSD_DS_SET_IV,
  TZBSP_EC_SSD_DS_FAILED,

  /* ssd_tz_setup_cipher */
  TZBSP_EC_SSD_SC_DEC_IEK               = SSD_B + 90,
  TZBSP_EC_SSD_SC_CE_INIT,
  TZBSP_EC_SSD_SC_UNSUPPORTED_CIPHER,
  TZBSP_EC_SSD_SC_CIPHER_INIT,
  TZBSP_EC_SSD_SC_SET_DIR,
  TZBSP_EC_SSD_SC_SET_MODE,
  TZBSP_EC_SSD_SC_SET_KEY,
  TZBSP_EC_SSD_SC_SET_IV,

  /* ssd_tz_parse_and_validate_md */
  TZBSP_EC_SSD_PAVMD_IP_CHK             = SSD_B + 100,
  TZBSP_EC_SSD_PAVMD_ALLOC,
  TZBSP_EC_SSD_PAVMD_PARSE,
  TZBSP_EC_SSD_PAVMD_SIG,

  /* ssd_tz_decrypt_in_place */
  TZBSP_EC_SSD_DIP_CE_ERROR             = SSD_B + 110,

  /* ssd_tz_verify_sig */
  TZBSP_EC_SSD_VS_IP_CHK                = SSD_B + 120,
  TZBSP_EC_SSD_VS_CE_HASH_INIT,
  TZBSP_EC_SSD_VS_CE_HASH_UPDATE,
  TZBSP_EC_SSD_VS_CE_HASH_FINAL,
  TZBSP_EC_SSD_VS_CE_HASH_DEINIT,
  TZBSP_EC_SSD_VS_KEY_ID,
  TZBSP_EC_SSD_VS_GET_OEM_PUB_KEY,
  TZBSP_EC_SSD_VS_BI_READ_MOD,
  TZBSP_EC_SSD_VS_BI_READ_EXP,
  TZBSP_EC_SSD_VS_BI_READ_SIG,
  TZBSP_EC_SSD_VS_BI_MODEXP,
  TZBSP_EC_SSD_VS_BI_TO_BIN,
  TZBSP_EC_SSD_VS_PKCS_CHK,

  /* ssd_tz_verify_rsa_pkcs_1_5_sig */
  TZBSP_EC_SSD_VPKCS_IP_CHK             = SSD_B + 140,
  TZBSP_EC_SSD_VPKCS_BEG_PAD,
  TZBSP_EC_SSD_VPKCS_PAD,
  TZBSP_EC_SSD_VPKCS_END_PAD,
  TZBSP_EC_SSD_VPKCS_SIG,

  /* ssd_tz_decrypt_iek */
  TZBSP_EC_SSD_DIEK_KEY_ID              = SSD_B + 150,

  /* ssd_tz_decrypt_rsa_2048 */
  TZBSP_EC_SSD_DR2048_PT_LEN            = SSD_B + 160,
  TZBSP_EC_SSD_DR2048_GET_MOD_EXP,
  TZBSP_EC_SSD_DR2048_BI_READ_MOD,
  TZBSP_EC_SSD_DR2048_BI_READ_EXP,
  TZBSP_EC_SSD_DR2048_BI_READ_CT,
  TZBSP_EC_SSD_DR2048_BI_MODEXP,
  TZBSP_EC_SSD_DR2048_BI_TO_BIN,

  /* ssd_tz_decrypt_aes_128 */
  TZBSP_EC_SSD_DA128_CI                 = SSD_B + 170,
  TZBSP_EC_SSD_DA128_SET_KEYLEN,
  TZBSP_EC_SSD_DA128_SET_IV,
  TZBSP_EC_SSD_DA128_SET_MODE,
  TZBSP_EC_SSD_DA128_SET_PAD,
  TZBSP_EC_SSD_DA128_DECRYPT,
  TZBSP_EC_SSD_DA128_OLEN,

  /* ssd_tz_get_rsa_pri_mod_exp */
  TZBSP_EC_SSD_GET_RSA_PUB_ME_KEYSZ     = SSD_B + 180,

  /* ssd_tz_get_oem_pub_mod_exp */
  TZBSP_EC_SSD_GET_RSA_PRI_ME_KEYSZ     = SSD_B + 190,

  /* ssd_tz_find_md_start */
  TZBSP_EC_SSD_FMDS_NO_START_TAG        = SSD_B + 200,
  TZBSP_EC_SSD_FMDS_NO_XML_TAG_SPACE,
  TZBSP_EC_SSD_FMDS_NO_XML_TAG,
  TZBSP_EC_SSD_FMDS_ALIGN,

  /* ssd_parse_md */
  TZBSP_EC_SSD_PMD_IP_CHK               = SSD_B + 210,
  TZBSP_EC_SSD_PMD_NO_XML_OPEN_TAG,
  TZBSP_EC_SSD_PMD_TAG_NOT_FOUND,
  TZBSP_EC_SSD_PMD_ALLOC,
  TZBSP_EC_SSD_PMD_IMG_ENC_ALGO_TAG,
  TZBSP_EC_SSD_PMD_IMG_ENC_PADDING_TYPE_TAG,
  TZBSP_EC_SSD_PMD_IMG_ENC_OPERATION_MODE_TAG,
  TZBSP_EC_SSD_PMD_IEK_ENC_ALGO_TAG,
  TZBSP_EC_SSD_PMD_IEK_ENC_AES_PADDING_TAG,
  TZBSP_EC_SSD_PMD_IEK_ENC_RSA_PADDING_TAG,
  TZBSP_EC_SSD_PMD_IEK_ENC_PUB_KEY_ID_TAG,
  TZBSP_EC_SSD_PMD_IEK_ENC_PUB_KEY_ID_MISMATCH,
  TZBSP_EC_SSD_PMD_IEK_ENC_IV_TAG,
  TZBSP_EC_SSD_PMD_IEK_CIPHER_VALUE_TAG,
  TZBSP_EC_SSD_PMD_MD_SIG_PADDING_TAG,
  TZBSP_EC_SSD_PMD_MD_SIG_ALGO_TAG,
  TZBSP_EC_SSD_PMD_SIG_DGST_ALGO_TAG,
  TZBSP_EC_SSD_PMD_MD_SIGNATURE_TAG,
  TZBSP_EC_SSD_PMD_IMG_ENC_IV_TAG,
  TZBSP_EC_SSD_PMD_SIG_OEM_PUB_KEY_ID_TAG,
  TZBSP_EC_SSD_PMD_OEM_KEY_ID_MISMATCH,

  /* get_xml_node */
  TZBSP_EC_SSD_GXN_IP_CHK               = SSD_B + 230,
  TZBSP_EC_SSD_GXN_TAG_LEN,
  TZBSP_EC_SSD_GXN_BEG_TAG,
  TZBSP_EC_SSD_GXN_END_TAG,
  TZBSP_EC_SSD_GXN_TAG_CHK,

  /* ssd_b64_decode */
  TZBSP_EC_SSD_B64D_INP_CHK             = SSD_B + 240,
  TZBSP_EC_SSD_B64D_STRLEN_CHK,
  TZBSP_EC_SSD_B64D_INVALID_CHAR_GROUPING,
  TZBSP_EC_SSD_B64D_INVALID_NUM_PAD,
  TZBSP_EC_SSD_B64D_INVALID_DECODED_LEN,

   /* tzbsp_ssd_encrypt_keystore_ns */
  TZBSP_EC_SSD_EKSNS_OVERFLOW          = SSD_B + 300,
  TZBSP_EC_SSD_EKSNS_SPTR,
  TZBSP_EC_SSD_EKSNS_ALIGN_OVERFLOW,
  TZBSP_EC_SSD_EKSNS_SPTR2,
  TZBSP_EC_SSD_EKSNS_NO_HDR,
  TZBSP_EC_SSD_EKSNS_KS_LEN,
  TZBSP_EC_SSD_EKSNS_ENC_FAILED,
  

  TZ_EC_SSD_MAX_ERRNO                   = SSD_B + 400
};

#endif
