/*****************************************************************************
*
* @file  secboot_hw.h (Secboot Hardware API)
*
* @brief Secboot Hardware API to get authentication information from the
*        Security Control fuses
*        
*
* Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secboot/hw/secctrl/mdm9x07/src/secboot_hw.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/10/12   mm      Honor the total number before the index
02/27/12   hw,mm   Add support for multiple root selection
03/12/12   sm      Use QC's OEM id of 1
02/10/12   sm      Rename macro, and ensure code_segment is not 0.
11/30/10   sm      Corrected check of functions return value
08/03/10   sm      Fixed comment to denote correct motorola root cert
05/27/10   sm      Created module
============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "secboot_hw.h"
#include "secboot_hw_i.h"
#ifdef USE_BOOT_UTILS
#include "boot_msmhwio.h"
#else
#include "HALhwio.h"
//#define SECURITY_CONTROL_BASE 0xe12b8000
#include "msmhwiobase.h"
#endif
#include "msmhwioreg.h"
#include "secboot.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifdef USE_BOOT_UTILS
#include "boot_util.h"
#else
#include <string.h>
#define MEMSET  memset
#define MEMCPY  memcpy
#define MEMCMP  memcmp
#define MEMMOVE memmove
#ifdef TZBSP_WITH_TBASE
extern boolean tzbsp_sysdbg_flat_mapped(void);
secboot_hw_etype secboot_hw_is_auth_enabled_flat_mapped
(
  uint32  code_segment,
  volatile uint32* auth_enabled_ptr
);
#endif
/*===========================================================================
FUNCTION check_fec_error

DESCRIPTION
  Checks the fuse sense FEC error. Clears error if seen.

PARAMETERS
  None

RETURN VALUE
  TRUE if FEC error seen, FALSE otherwise

===========================================================================*/
boolean check_fec_error (void){
  if ( HWIO_INF(FEC_ESR, ERR_SEEN) == 0x1 ){
    HWIO_OUTF(FEC_ESR, ERR_SEEN, 0x0);
    return TRUE;
  }
  else{
    return FALSE;
  }
}
#endif


#define BREAKIF(cond) { if (cond) break; }

/* Value to mask out the MSM HW Revision from JTAG_ID */
#define HW_REVISION_AUTHENTICATION_MASK 0x0FFFFFFFUL

/* check whether root_sel feature is enabled or not*/
#define SECBOOT_IS_ROOT_CERT_SEL_ENABLED(val) (((val) & 0xFF) != 0xF0)

/* Number of shifts required to format the MSM HW Revision */
/* number for the authentication code                      */
#define HW_REVISION_NUM_SHFT_BITS       32

/* Indicates whether the secure boot fuses have been pulled from the QC region     */
/* Fuses (QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB, or QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB)     */
/* or from the OEM region fuses (QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB, or              */
/* QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB).                                              */
/* i.e if QC has signed the image for a code segment, we will blow the QC region   */
/* fuses and the SECURE_BOOTn software fuse register's FUSE_SRC bit will indicate  */
/* 0 (QC) i.e the secure boot fuses values are pulled from QC region. 1 (OEM) to   */
/* indicate secure boot fuses values are pulled from OEM region                    */
#define QC_SECBOOT_REGION_FUSE_SRC       0

/* Qualcomm Root of Trust table containing SHA256 hashes of */
/* known root certificates. The root certificate at index 0 */
/* is the QC root certificate which is used when an image   */
/* is signed using CSMS. Some root certificates in this     */
/* table have been assigned (handed over) to customers      */

static const uint8 root_of_trust_pk_hash_table[][SHA256_HASH_LEN] = {
/* QCT Root CA 1 (QCT_Root_CA.cer) */
{
    0xcc, 0x31, 0x53, 0xa8, 0x02, 0x93, 0x93, 0x9b, 0x90, 0xd0, 0x2d, 0x3b,
    0xf8, 0xb2, 0x3e, 0x02, 0x92, 0xe4, 0x52, 0xfe, 0xf6, 0x62, 0xc7, 0x49,
    0x98, 0x42, 0x1a, 0xda, 0xd4, 0x2a, 0x38, 0x0f
},
  /* Motorola Root CA (SRK.QCA280.x509.cer) */
{
    0x0a, 0xf1, 0x65, 0xb1, 0x67, 0x52, 0xd3, 0xf9, 0xf7, 0x51, 0x60, 0x6f,
    0x6d, 0x6d, 0x0d, 0x43, 0x04, 0x3b, 0x8a, 0x25, 0xb2, 0x5b, 0x97, 0x79,
    0x11, 0x08, 0x9c, 0xdf, 0xcf, 0xc3, 0x70, 0x05
},
/* QCT OEM Root CA 1 (qct1.cer) */
{
    0xcb, 0x4e, 0xfe, 0x59, 0x5e, 0x49, 0x03, 0xb4, 0x92, 0x9f, 0x22, 0x1a, 
    0x5d, 0xbb, 0x3a, 0xd6, 0xaf, 0xf2, 0xaf, 0x4f, 0x37, 0x00, 0x27, 0xa0, 
    0xd6, 0x66, 0xf1, 0xbd, 0x04, 0x33, 0xaf, 0x2e
},
/* QCT OEM Root CA 2 (qct2.cer) */
{
    0xc2, 0x91, 0x79, 0x58, 0x06, 0xe3, 0x62, 0xc3, 0xa3, 0x81, 0x6f, 0x7c, 
    0x01, 0x1e, 0xe1, 0xfd, 0xe0, 0x72, 0x06, 0xfa, 0x38, 0xab, 0x1a, 0xcf, 
    0x6d, 0xc2, 0x64, 0x08, 0x52, 0x5b, 0xb2, 0x35
},
/* QCT OEM Root CA 3 (qct3.cer) */
{
    0x0c, 0xd8, 0xa0, 0xa5, 0x87, 0x63, 0x2f, 0x7f, 0x2e, 0xb2, 0x83, 0x5a, 
    0x91, 0x25, 0x78, 0x0a, 0x21, 0xbf, 0xa0, 0x69, 0xc4, 0x96, 0xb9, 0x01, 
    0x7c, 0x7d, 0xa4, 0xee, 0x2a, 0x97, 0xc3, 0x80
},
/* QCT OEM Root CA 4 (qct4.cer) */
{
    0x0f, 0x43, 0x24, 0x08, 0x92, 0xd0, 0x2f, 0x0d, 0xc9, 0x63, 0x13, 0xc8, 
    0x13, 0x51, 0xb4, 0x0f, 0xd5, 0x02, 0x9e, 0xd9, 0x8f, 0xf9, 0xec, 0x70, 
    0x74, 0xdd, 0xae, 0x8b, 0x05, 0xcd, 0xc8, 0xe1
},
/* QCT OEM Root CA 5 (qct5.cer) */
{
    0x67, 0xfc, 0x42, 0x3c, 0xd9, 0x59, 0x9e, 0x90, 0x9b, 0xa9, 0xe7, 0xca, 
    0x0b, 0x77, 0xaf, 0xe2, 0x68, 0x2a, 0x94, 0x79, 0x66, 0x97, 0xdc, 0x8c, 
    0x9b, 0x40, 0x7a, 0x34, 0x1e, 0x36, 0xef, 0x1c
},
/* QCT OEM Root CA 6 (qct6.cer) */
{
    0xa2, 0x98, 0xd1, 0xdd, 0x4e, 0x14, 0xa4, 0x0f, 0xab, 0x85, 0x65, 0x3b, 
    0xfc, 0x27, 0x70, 0xde, 0xb9, 0xf4, 0x1b, 0x60, 0x54, 0x56, 0xc2, 0x98, 
    0xaf, 0x23, 0x24, 0xf4, 0x80, 0x3e, 0xb9, 0x1d
},
/* QCT OEM Root CA 7 (qct7.cer) */
{
    0xe6, 0x04, 0xf8, 0xe7, 0xe1, 0xca, 0x54, 0x92, 0x73, 0x4d, 0xd8, 0x4a, 
    0x96, 0x5f, 0xe2, 0x14, 0x1f, 0x7e, 0xdb, 0x09, 0xfe, 0xed, 0x53, 0x85, 
    0xfb, 0x11, 0x40, 0x6b, 0xf4, 0x94, 0x02, 0x11
},
/* QCT OEM Root CA 8 (qct8.cer) */
{
    0xae, 0x9e, 0x52, 0xb1, 0x86, 0x91, 0x3c, 0xf1, 0xf1, 0x91, 0x98, 0x62, 
    0x06, 0x97, 0xc7, 0x77, 0x4e, 0x0c, 0x9e, 0x1e, 0x5e, 0xa7, 0xb9, 0xc9, 
    0x8a, 0x26, 0x11, 0x86, 0xe7, 0x3e, 0x43, 0x7c
},
/* QCT OEM Root CA 9 (qct9.cer) */
{
    0x99, 0x85, 0xaa, 0x32, 0xf4, 0xb1, 0x14, 0xc2, 0x73, 0x3d, 0xb4, 0xba, 
    0xd4, 0xb1, 0xe0, 0x28, 0x3e, 0x92, 0xa2, 0x1d, 0x06, 0xd5, 0xd6, 0x65, 
    0xea, 0x1d, 0x52, 0x60, 0xba, 0x31, 0x16, 0x34
},
/* QCT OEM Root CA 10 (qct10.cer)--MSA ROOT KEY-QC-RESERVED */
{
 0x56, 0x3d, 0x9c, 0xc7, 0x74, 0x8b, 0xc2, 0x47, 0x09, 0xaf, 0x9d, 0x2c, 
 0x8e, 0x49, 0x48, 0x9a, 0xe7, 0x49, 0xf6, 0xfb, 0xbc, 0x0f, 0xa9, 0x2f, 
 0x10, 0xa2, 0x0c, 0x35, 0x51, 0xb0, 0x4d, 0x20
}
};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
/**
 * @brief This function gets the OEM_ID or chip SERIAL_NUM to be used
 *        for authentication depending on whether the USE_SERIAL_NUM
 *        fuse is blown. If the USE_SERIAL_NUM fuse is blown, but the
 *        root of trust is QC root of trust (CSMS), then OEM_ID is returned
 *        as we MUST always use OEM_ID when QC is the root of trust.
 *        If Qualcomm has has signed the MSA (modem self authentication)
 *        images - MBA/MSS, then 0 is returned irrespective of what is
 *        blown in the OEM_ID fuses, as it is expected that Qualcomm
 *        will sign the MSA images with an OEM_ID value of 0.
 * 
 *
 * @param code_segment       [in] Segment of SECURE_BOOTn register holding
 *                                authentication information for the code
 * @param root_of_trust     [in] Root certificate's 32 byte hash value
 * @param auth_id_ptr       [in,out] Pointer to auth_id to populate
 *                         
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
static secboot_hw_etype secboot_hw_get_auth_id
(
  uint32 code_segment,
  const uint8 root_of_trust[SHA256_HASH_LEN],
  uint32* auth_id_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;
  uint32           use_serial_num = 0;
  uint32           fuse_src = 0;

  do
  {

    /* Sanity check */
    if ((NULL == auth_id_ptr) || (NULL == root_of_trust) || (0 == code_segment))
    {
      ret_val = E_SECBOOT_HW_INVALID_PARAM;
      break;
    }

    /* Check if QC has blown the fuses if so (which means QC has signed the image) */
    /* If so, use the hardcoded OEM_ID of 1 for QC */
    /* FUSE_SRC indicates whether the SECURE_BOOTn software register fuses */
    /* have been pulled by HW from QFPROM_RAW_QC_SEC_BOOT_ROWn fuses (which is for QC) */
    /* QFPROM_RAW_OEM_SEC_BOOT_ROWn fuses which is for OEM's */
    fuse_src = HWIO_INFI( SECURE_BOOTn, code_segment, FUSE_SRC );
    if (QC_SECBOOT_REGION_FUSE_SRC == fuse_src)
    {
      // QC has signed the image. We expect QC to sign production images using QC OEM
      // ID of 1 (upper 16 bits oem hw id=1, lower 16 bits oem_model_id=0)
      (*auth_id_ptr) = 0x00010000;
      break;
    }

    if ( MEMCMP(root_of_trust, &root_of_trust_pk_hash_table[0][0], SHA256_HASH_LEN ) == 0)
    {
      /* QCOM root of trust (CSMS), always use OEM_ID */
      (*auth_id_ptr) = HWIO_IN( OEM_ID );
    }
    else
    {
      /* OEM root of trust, check whether OEM_ID or serial num needs to be used */
      ret_val = secboot_hw_get_use_serial_num(code_segment, &use_serial_num);
      BREAKIF(E_SECBOOT_HW_SUCCESS != ret_val);

      if (use_serial_num)
      {
        /* use serial number in authentication */
        ret_val = secboot_hw_get_serial_num(auth_id_ptr);
        BREAKIF(E_SECBOOT_HW_SUCCESS != ret_val);
      }
      else
      {
        /* Use OEM ID */
        (*auth_id_ptr) = HWIO_IN( OEM_ID );
      }
    }
  }while (0);

  return ret_val;
}

/**
 * @brief This function checks to see if the root of trust hash
 *        is blowin in the OEM_PK_HASH fuses or is in the
 *        root_of_trust_pk_hash_table.
 *
 * @param code_segment       [in] Segment of SECURE_BOOTn register holding
 *                                authentication information for the code
 *
 * @param is_hash_in_fuse_ptr [in,out] Pointer to uint32 that will be
 *                                populated with the fuse value. 0 means
 *                                hash is in the Qualcomm root of trust
 *                                table.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
static secboot_hw_etype secboot_hw_get_is_hash_in_fuse
(
  uint32 code_segment,
  uint32* is_hash_in_fuse_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;

  /* Sanity check */
  if ((NULL == is_hash_in_fuse_ptr) || (0 == code_segment))
  {
    return E_SECBOOT_HW_INVALID_PARAM;
  }

  (*is_hash_in_fuse_ptr) = HWIO_INFI( SECURE_BOOTn, code_segment, PK_HASH_IN_FUSE );
  return ret_val;
}

/**
 * @brief This function retrieves the index in the Qualcomm Root of Trust 
 *        table, that needs to be used for authentication
 *
 * @param code_segment       [in] Segment of SECURE_BOOTn register holding
 *                                authentication information for the code
 *
 * @param hash_index_ptr     [in,out] Pointer to uint32 that will be
 *                                populated with the index. uint32 is
 *                                allocated by the caller
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
static secboot_hw_etype secboot_hw_get_pk_hash_index
(
  uint32 code_segment,
  uint32* hash_index_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;

  /* Sanity check */
  if ((NULL == hash_index_ptr) || (0 == code_segment))
  {
    return E_SECBOOT_HW_INVALID_PARAM;
  }

  (*hash_index_ptr) = HWIO_INFI( SECURE_BOOTn, code_segment, ROM_PK_HASH_INDEX );
  return ret_val;
}

/*=========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=========================================================================*/

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_is_auth_enabled
(
  uint32  code_segment,
  volatile uint32* auth_enabled_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;
#ifdef TZBSP_WITH_TBASE
  if(tzbsp_sysdbg_flat_mapped()) 
  {
    ret_val = secboot_hw_is_auth_enabled_flat_mapped(code_segment, auth_enabled_ptr);
  }
  else
  {
#endif
    if ((NULL == auth_enabled_ptr) || (0 == code_segment))
    {
      return E_SECBOOT_HW_INVALID_PARAM;
    }

    /* Read AUTH_EN fuses to determine if authentication is required */
    /* If blown (high), authentication is required                   */
    (*auth_enabled_ptr) = HWIO_INFI( SECURE_BOOTn, code_segment, AUTH_EN );

    /* Check if we've enforced that modem images must be authenticated */
    /* (force_msa_auth_en fuse). This means even if AUTH_EN was not blown */
    /* we still tell caller to authenticate the image, i.e image HAS to */
    /* be signed */
    if ( (0 == (*auth_enabled_ptr)) && 
       ( (SECBOOT_HW_MBA_CODE_SEGMENT == code_segment) ||
         (SECBOOT_HW_MSS_CODE_SEGMENT == code_segment) ) )
    {
      //Force msa auth enable overrides auth enable setting, so
      //we tells caller to authenticate the image.
      (*auth_enabled_ptr) = HWIO_INF(BOOT_CONFIG, FORCE_MSA_AUTH_EN );
    }
#ifdef TZBSP_WITH_TBASE
  }
#endif
  return ret_val;
}

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_get_root_of_trust
(
  uint32 code_segment,
  uint8 root_of_trust[SHA256_HASH_LEN]
)
{
  uint32 index = 0;
  uint32 is_hash_in_fuse = 0;
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;
  uint32 oem_pk_hash_row_bytes[2] = {0};
  do
  {
    if ((NULL == root_of_trust) || (0 == code_segment))
    {
      ret_val = E_SECBOOT_HW_INVALID_PARAM;
      break;
    }

    ret_val = secboot_hw_get_is_hash_in_fuse(code_segment, &is_hash_in_fuse);
    BREAKIF(E_SECBOOT_HW_SUCCESS != ret_val);

    if (0 == is_hash_in_fuse)
    {
      /* Qualcomm table - the root of trust hash is in the root_of_trust */
      /* table, at the index pointed to by the pk hash index fuse        */
      ret_val = secboot_hw_get_pk_hash_index(code_segment, &index);
      BREAKIF(E_SECBOOT_HW_SUCCESS != ret_val);

      if (index >= sizeof(root_of_trust_pk_hash_table)/SHA256_HASH_LEN)
      {
        return E_SECBOOT_HW_OUT_OF_RANGE;
      }
      MEMCPY(root_of_trust, &root_of_trust_pk_hash_table[index][0],SHA256_HASH_LEN);
    }
    else
    {
      /* OEM table */
      /* For OEM table, the root of trust hash is in the OEM PK Hash fuses     */
      /* Each oem_pk_hash row is read and stored into the passed in hash array */
      /*                                         Row */
      /*                     |______LSB______|    4  */
      /* |xxxxxMSB________|  |______LSB______|    3  */
      /* |xxxxxMSB________|  |______LSB______|    2  */
      /* |xxxxxMSB________|  |______LSB______|    1  */
      /* |xxxxxMSB________|  |______LSB______|    0  */
      /*         23       0  31              0       */
      /*                                bits         */
      /* hash_array[]=                               */
      /* { 7 bytes from row-0 (bits 0 through 55 of row 0), */
      /*   7 bytes from row-1 (bits 0 through 55 of row 1), */
      /*   7 bytes from row-2 (bits 0 through 55 of row 2), */
      /*   7 bytes from row-3 (bits 0 through 55 of row 3), */
      /*   4 bytes from row-4 (bits 0 through 31 of row 4) }*/
      for (index=0; index < 4; index++)
      {
        /* Read oem_pk_hash LSB and store it to temporary array's 0th index */
        oem_pk_hash_row_bytes[0] = HWIO_INI (QFPROM_CORR_PK_HASH_ROWn_LSB,
                                              index);

        /* Read oem_pk_hash MSB and store it to temporary array's 1st index */
        oem_pk_hash_row_bytes[1] = HWIO_INI (QFPROM_CORR_PK_HASH_ROWn_MSB,
                                              index);

        /* copy oem_pk_hash_row (7 bytes only: 4 bytes from LSB and 3 */
        /* bytes from MSB to hash_array pointing to oem_pk_hash. Must */
        /* use copy api which can handle byte copy with pointer not   */
        /* aligned to 32 bit address                                  */
        MEMCPY(&(root_of_trust[index*7]),(uint8*)&oem_pk_hash_row_bytes[0], 7);
      }

      /* Store 4 remaining bytes of hash value from LSB row4 to */
      /* hash array                                             */
      /* Must use copy api which can handle byte copy with      */
      /* pointer not aligned to 32 bit address*/
      oem_pk_hash_row_bytes[0] = HWIO_IN (QFPROM_CORR_PK_HASH_ROW4_LSB);
      MEMCPY((uint8*)&root_of_trust[28], (uint8*)&oem_pk_hash_row_bytes[0], 4);
    }

    //Check for any FEC issues
    if (TRUE == check_fec_error())
    {
      //An FEC error which was unrecoverable was detected during fuse reading.
      //The state of the fuse values cannot be relied upon, so fail authentication
      ret_val = E_SECBOOT_HW_FEC_ERROR;
      break;
    }
  }while (0);

  return ret_val;
}

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_get_msm_hw_id
(
  uint32 code_segment,
  const uint8 root_of_trust[SHA256_HASH_LEN],
  uint64* msm_hw_id_ptr
)
{
  uint32 auth_id = 0;
  secboot_hw_etype retVal = E_SECBOOT_HW_SUCCESS;

  do
  {
    /* Sanity check */
    if ((NULL == msm_hw_id_ptr) || (NULL == root_of_trust) || (0 == code_segment))
    {
      return E_SECBOOT_HW_INVALID_PARAM;
    }

    /* Get the MSM HW Revision number for authentication purposes.  The
       MSM part number is required, excluding the most significant 7 bits,
       which are used to identify different foundry IDs, and version IDs. */

    /* Read HW PART NUM and assign it to uint32 MSBs of msm_hw_id */
    (*msm_hw_id_ptr) = (uint64) HWIO_INM(JTAG_ID, HW_REVISION_AUTHENTICATION_MASK);

    /* Adjust the MSM HW ID so it is in the format required by the
       authentication SW.  Authentication SW expects the part of the
       HW_REVISION_NUMBER used for authentication to start at bit 32 */
     (*msm_hw_id_ptr) <<= HW_REVISION_NUM_SHFT_BITS;

     /* Get the value for bits 31:0 of the msm_hw_id */
      retVal = secboot_hw_get_auth_id(code_segment, root_of_trust, &auth_id);
      BREAKIF(retVal != E_SECBOOT_HW_SUCCESS)

      (*msm_hw_id_ptr) |= (uint64)auth_id;

  }while (0);

  return retVal;
}

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_get_use_serial_num
(
  uint32   code_segment,
  uint32*  use_serial_num_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;

  do
  {
    /* Sanity check */
    if ((NULL == use_serial_num_ptr) || (0 == code_segment))
    {
      ret_val = E_SECBOOT_HW_INVALID_PARAM;
      break;
    }

    (*use_serial_num_ptr) = HWIO_INFI( SECURE_BOOTn, code_segment, USE_SERIAL_NUM );

  }while (0);

  return ret_val;
}

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_get_serial_num
(
  uint32*  serial_num_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;
  do
  {
    /* Sanity check */
    if ( NULL == serial_num_ptr)
    {
      ret_val = E_SECBOOT_HW_INVALID_PARAM;
      break;
    }

    (*serial_num_ptr) = HWIO_INF( QFPROM_CORR_SERIAL_NUM, SERIAL_NUM );

  }while (0);

  return ret_val;
}

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_get_root_cert_info
(
  uint32  code_segment,
  uint32* oem_root_cert_sel_ptr, 
  uint32* is_root_cert_enabled_ptr,
  uint32* root_cert_total_num
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;
  uint32 is_hash_in_fuse = 0;

  do 
  {
    /* Sanity check of input params */
    if ((NULL == oem_root_cert_sel_ptr) || (NULL == is_root_cert_enabled_ptr)  ||
        (NULL == root_cert_total_num) || (0 == code_segment))  
    {
      ret_val = E_SECBOOT_HW_INVALID_PARAM;
      break;
    }

    ret_val = secboot_hw_get_is_hash_in_fuse(code_segment, &is_hash_in_fuse);
    BREAKIF(E_SECBOOT_HW_SUCCESS != ret_val);

    if (0 == is_hash_in_fuse)
    {
      /* Disable if cert in ROM */
      *is_root_cert_enabled_ptr = SECBOOT_ROOT_CERT_SEL_DISABLED;
      *oem_root_cert_sel_ptr = 0;
      *root_cert_total_num = 0;
      ret_val = E_SECBOOT_HW_SUCCESS;
      break;
    }

    /* Retrieve the total number of ROOT CERT NUM from the fuses*/
    *root_cert_total_num = HWIO_INF(QFPROM_CORR_OEM_CONFIG_ROW0_LSB, 
                                    ROOT_CERT_TOTAL_NUM) + 1;

    if (1 == *root_cert_total_num)
    {
      /* Disable if total num is 1 */
      *is_root_cert_enabled_ptr = SECBOOT_ROOT_CERT_SEL_DISABLED;
      *oem_root_cert_sel_ptr = 0;
      *root_cert_total_num = 0;
      ret_val = E_SECBOOT_HW_SUCCESS;
      break;
    }

    /* The function reads the 8 bit Root HASH INDEX and its complement   */
    /* from the fuses and propulate it to oem_root_cert_sel_ptr (IN/OUT) */
    ret_val = secboot_hw_get_root_cert_sel(oem_root_cert_sel_ptr);
    if (ret_val != E_SECBOOT_HW_SUCCESS)
    {
      break;
    }

    /* Check if the Efuses value is set to 0xF0 which indicates disable 
     * Root Cert Selection 
     */
    if (SECBOOT_IS_ROOT_CERT_SEL_ENABLED(*oem_root_cert_sel_ptr))
    {
      *is_root_cert_enabled_ptr = SECBOOT_ROOT_CERT_SEL_ENABLED;
    }
    else
    {
      /* If Root Cert is disabled, set all the root cert Efuse information 
       * to indicate that it is disable 
       * Default to the 0th Root Cert and return to caller 
       */
      *is_root_cert_enabled_ptr = SECBOOT_ROOT_CERT_SEL_DISABLED;
      *oem_root_cert_sel_ptr = 0;
      *root_cert_total_num = 0;
      ret_val = E_SECBOOT_HW_SUCCESS;
      break;
    }

    /* If Root Cert is enabled, then calculate the actual index of the Root Cert hash 
     * from the 4 least significant bits 
     */  
    if ((*is_root_cert_enabled_ptr) == SECBOOT_ROOT_CERT_SEL_ENABLED)
    {
      /* If the fuse value of the root_cert_index and the complement is not 0, then check 
       * the index complement is valid
       */
      if (*oem_root_cert_sel_ptr != 0)
      {
        /* Check if the values are indeed the complement by bit-wise XOR */
        if ((((*oem_root_cert_sel_ptr >> 4) ^ *oem_root_cert_sel_ptr) & 0xf) != 0xf)
        {
          *oem_root_cert_sel_ptr = 0;
          *root_cert_total_num = 0;
          ret_val = E_SECBOOT_HW_OUT_OF_RANGE;
          break;
        }

        /* check the 1-complement part and mask the root cert sel to the real numeric value */
        *oem_root_cert_sel_ptr &= 0x0F;
      }

      /* sanity check - Check that the root selection is not greater than or equal 
       * the total number of Root certs 
       */
      if ( *oem_root_cert_sel_ptr >= *root_cert_total_num)
      {
        *oem_root_cert_sel_ptr = 0;
        *root_cert_total_num = 0;
        ret_val = E_SECBOOT_HW_OUT_OF_RANGE;
        break;
      }
    }
  } while (0);

  return ret_val;
}

/**
 * @brief See documentation in public header
 *
 */
secboot_hw_etype secboot_hw_get_ftbl(secboot_hw_ftbl_type* ftbl_ptr)
{
  if (NULL == ftbl_ptr)
    return E_SECBOOT_HW_INVALID_PARAM;

  ftbl_ptr->secboot_hw_get_msm_hw_id = secboot_hw_get_msm_hw_id;
  ftbl_ptr->secboot_hw_get_root_of_trust = secboot_hw_get_root_of_trust;
  ftbl_ptr->secboot_hw_get_serial_num = secboot_hw_get_serial_num;
  ftbl_ptr->secboot_hw_get_use_serial_num = secboot_hw_get_use_serial_num;
  ftbl_ptr->secboot_hw_is_auth_enabled = secboot_hw_is_auth_enabled;
  ftbl_ptr->secboot_hw_get_root_cert_info = secboot_hw_get_root_cert_info;

  return E_SECBOOT_HW_SUCCESS;
}

#ifdef TZBSP_WITH_TBASE
/* Use the C pre-processor to re-define the below macros based on
 * physical addresses. Note that the new defines are valid until
 * the end of the current translation unit */
#undef  SECURITY_CONTROL_BASE
#define SECURITY_CONTROL_BASE                     SECURITY_CONTROL_BASE_PHYS

secboot_hw_etype secboot_hw_is_auth_enabled_flat_mapped
(
  uint32  code_segment,
  volatile uint32* auth_enabled_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;

  if ((NULL == auth_enabled_ptr) || (0 == code_segment))
  {
    return E_SECBOOT_HW_INVALID_PARAM;
  }

  /* Read AUTH_EN fuses to determine if authentication is required */
  /* If blown (high), authentication is required                   */
  (*auth_enabled_ptr) = HWIO_INFI( SECURE_BOOTn, code_segment, AUTH_EN );

  /* Check if we've enforced that modem images must be authenticated */
  /* (force_msa_auth_en fuse). This means even if AUTH_EN was not blown */
  /* we still tell caller to authenticate the image, i.e image HAS to */
  /* be signed */
  if ( (0 == (*auth_enabled_ptr)) && 
       ( (SECBOOT_HW_MBA_CODE_SEGMENT == code_segment) ||
         (SECBOOT_HW_MSS_CODE_SEGMENT == code_segment) ) )
  {
    //Force msa auth enable overrides auth enable setting, so
    //we tells caller to authenticate the image.
    (*auth_enabled_ptr) = HWIO_INF(BOOT_CONFIG, FORCE_MSA_AUTH_EN );
  }

  return ret_val;
}
#endif
