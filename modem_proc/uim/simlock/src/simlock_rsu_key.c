/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   R S U   K E Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock RSU key functions.



                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_rsu_key.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/16   stv     Remote simlock support
08/19/15   stv     Re-locate the public key file
06/29/15   stv     Prefer hardcoded key to EFS provisioing for pub key
05/17/15   stv     Fix compiler warnings
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#if defined (FEATURE_SIMLOCK) && (defined (FEATURE_SIMLOCK_RSU) || defined (FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock.h"
#include "simlock_rsu.h"
#include "simlock_file.h"
#include "simlock_efs.h"
#include "simlock_crypto.h"
#include "simlock_util.h"
#include "simlock_rsu_key.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
static char *simlock_efs_rsu_pubkey_file   =   "/safe/uim/rsu_pubkey";

#define SIMLOCK_RSU_PUBKEY_FILE_VERSION      1

#define SIMLOCK_RSU_INVALID_MOD_LEN          8

/* Additional 1 byte corresponds to: Version - 1 byte */
#define SIMLOCK_RSU_PUBKEY_FILE_SIZE_V1      (SIMLOCK_RSU_PUBKEY_MODULUS_LEN + SIMLOCK_RSU_PUBKEY_EXPONENT_LEN + 1)
#define SIMLOCK_RSU_KEY_MOD_FILE_SIZE        (SIMLOCK_RSU_KEY_MOD_LEN)

/* Initialize the first 8-bytes to 0xFF and the rest to zero.
   If the first 8-bytes are 0xFF, then the modulus is considered
   as invalid. If the modulus is configured in the binary, this
   variable is expected to be updated during the integration */
static const uint8 simlock_rsu_pubkey_modulus[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* If the exponent is configured in the binary, this variable
  is expected to be updated during the integration */
static const uint8 simlock_rsu_pubkey_exponent[] = {0x00, 0x00, 0x00};

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_IS_HARDCODED_PUBKEY_CONFIGURED

DESCRIPTION
  This function checks if the public key is set in the source file

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE  : If the public key is set in the source file
  FALSE : If otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean simlock_rsu_key_is_hardcoded_pubkey_configured
(
  void
)
{
  uint8  invalid_modulus_data[SIMLOCK_RSU_INVALID_MOD_LEN] = {0xFF, 0xFF, 0xFF, 0xFF,
                                                              0xFF, 0xFF, 0xFF, 0xFF};
  uint8  invalid_exponent_data[SIMLOCK_RSU_PUBKEY_EXPONENT_LEN] = {0x00, 0x00, 0x00};

  /* check if modulus is set */
  if(memcmp(simlock_rsu_pubkey_modulus, invalid_modulus_data,
                                          SIMLOCK_RSU_INVALID_MOD_LEN) == 0)
  {
    return FALSE;
  }

  /* check if the exponent is set */
  if(memcmp(simlock_rsu_pubkey_exponent, invalid_exponent_data,
                                          SIMLOCK_RSU_PUBKEY_EXPONENT_LEN) == 0)
  {
    return FALSE;
  }

  if((sizeof(simlock_rsu_pubkey_modulus)) != SIMLOCK_RSU_PUBKEY_MODULUS_LEN)
  {
    return FALSE;
  }

  if((sizeof(simlock_rsu_pubkey_exponent)) != SIMLOCK_RSU_PUBKEY_EXPONENT_LEN)
  {
    return FALSE;
  }

  return TRUE;
} /* simlock_rsu_key_is_hardcoded_pubkey_configured */


/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_CHECK_PUBKEY_FILE_IN_SFS

DESCRIPTION
  This function checks if the public key file is present in the
  SFS and is of the expected size

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_key_check_pubkey_file_in_sfs
(
  void
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;
  uint32                   sfs_file_len   = 0;
  simlock_filename_type    file_name;

  /* get the pubkey file location in SFS */
  simlock_status = simlock_util_get_file_path_from_enum(SIMLOCK_FILE_RSU_PUBKEY, file_name);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* the pubkey file size should be SIMLOCK_RSU_PUBKEY_FILE_SIZE_V1 */
  simlock_status = simlock_file_check(file_name, &sfs_file_len);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (sfs_file_len != SIMLOCK_RSU_PUBKEY_FILE_SIZE_V1))
  {
    SIMLOCK_MSG_ERR_2("pubkey file check failed, status: 0x%x, len: 0x%x",
                       simlock_status, sfs_file_len);
    return SIMLOCK_GENERIC_ERROR;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_rsu_key_check_pubkey_file_in_sfs */


/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_READ_AND_STORE_PUBKEY_FILE

DESCRIPTION
  This function reads the public key file from the EFS location and stores it
  in the SFS location. It also deletes file in the EFS location. This is
  expected to happen once at the first power-up

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_read_and_store_pubkey_file
(
  void
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;

  /* check, if the hardcoded key is configured. If yes, no need
     to check for the EFS file */
  if(simlock_rsu_key_is_hardcoded_pubkey_configured())
  {
    SIMLOCK_MSG_MED_0("pubkey is configured in source");
    return SIMLOCK_SUCCESS;
  }

  /* check if the file exists in the SFS. If yes, return */
  simlock_status = simlock_rsu_key_check_pubkey_file_in_sfs();
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    /* delete the file from EFS since it was already copied to SFS */
    (void)simlock_efs_delete_file(simlock_efs_rsu_pubkey_file);
    return SIMLOCK_SUCCESS;
  }

  return simlock_file_secure(simlock_efs_rsu_pubkey_file,
                             SIMLOCK_RSU_PUBKEY_FILE_SIZE_V1,
                             SIMLOCK_FILE_RSU_PUBKEY);
} /* simlock_rsu_key_read_and_store_pubkey_file */


/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_GET_HARDCODED_PUBKEY

DESCRIPTION
  This function gets the modulus and exponent cnfigured in the binary.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_key_get_hardcoded_pubkey
(
  simlock_rsu_pubkey_data_type   *  pubkey_data_ptr
)
{
  if(pubkey_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* Check if the public key is configured */
  if(!simlock_rsu_key_is_hardcoded_pubkey_configured())
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  pubkey_data_ptr->version = SIMLOCK_RSU_PUBKEY_FILE_VERSION;

  simlock_memscpy(pubkey_data_ptr->modulus,
                  SIMLOCK_RSU_PUBKEY_MODULUS_LEN,
                  simlock_rsu_pubkey_modulus,
                  SIMLOCK_RSU_PUBKEY_MODULUS_LEN);

  simlock_memscpy(pubkey_data_ptr->exponent,
                  SIMLOCK_RSU_PUBKEY_EXPONENT_LEN,
                  simlock_rsu_pubkey_exponent,
                  SIMLOCK_RSU_PUBKEY_EXPONENT_LEN);

  return SIMLOCK_SUCCESS;
} /* simlock_rsu_key_get_hardcoded_pubkey */


/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_READ_PUBKEY_DATA

DESCRIPTION
  This function reads the public key file and provides the data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_read_pubkey_data
(
  simlock_rsu_pubkey_data_type   *  pubkey_data_ptr
)
{
  simlock_result_enum_type    simlock_status       = SIMLOCK_SUCCESS;
  uint32                      pubkey_file_size     = 0;
  uint8                     * pubkey_file_data_ptr = NULL;

  if(pubkey_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* Get the pub key from the global */
  simlock_status = simlock_rsu_key_get_hardcoded_pubkey(pubkey_data_ptr);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    return SIMLOCK_SUCCESS;
  }

  /* If key is not present in global,
     read the entire file data. pbkey_file_data_ptr holds the
     raw key data */
  simlock_status = simlock_file_read(&pubkey_file_data_ptr,
                                     &pubkey_file_size,
                                     SIMLOCK_FILE_RSU_PUBKEY,
                                     SIMLOCK_SFS_LOCAL);
  if((simlock_status == SIMLOCK_SUCCESS) &&
     (pubkey_file_data_ptr != NULL))
  {
     /* if the file size is not of the expected size,
       then it is not valid */
    if(pubkey_file_size != SIMLOCK_RSU_PUBKEY_FILE_SIZE_V1)
    {
      SIMLOCK_MSG_ERR_1("pbkey file size is not as expected, 0x%x",
                        pubkey_file_size);
      SIMLOCK_MEM_FREE(pubkey_file_data_ptr);
      return SIMLOCK_GENERIC_ERROR;
    }

    simlock_memscpy((uint8 *)pubkey_data_ptr,
                    sizeof(simlock_rsu_pubkey_data_type),
                    pubkey_file_data_ptr,
                    pubkey_file_size);

    SIMLOCK_MEM_FREE(pubkey_file_data_ptr);
    return SIMLOCK_SUCCESS;
  }

  return SIMLOCK_GENERIC_ERROR;
} /* simlock_rsu_key_read_pubkey_data */


#ifdef FEATURE_SIMLOCK_RSU
/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_GET_RANDOM_KEY

DESCRIPTION
  This function gets the random key. If the random key file exists in the SFS
  location, it is read for the key. If not, it generates the random key.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_rsu_key_get_random_key
(
  simlock_rsu_keymod_type              random_key
)
{
  simlock_result_enum_type    simlock_status  = SIMLOCK_SUCCESS;

  /* read the random key from file */
  simlock_status = simlock_rsu_key_read_keymod_file(random_key);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    return SIMLOCK_SUCCESS;
  }

  /* file doesn't exist, generate the random key */
  simlock_status = simlock_crypto_get_random_data(random_key, sizeof(simlock_rsu_keymod_type));
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("key generation failed, status: 0x%x", simlock_status);
    return simlock_status;
  }

  /* write the random key to the file */
  simlock_status = simlock_file_write(random_key, sizeof(simlock_rsu_keymod_type),
                                      SIMLOCK_FILE_RSU_KEY_MOD, SIMLOCK_SFS_LOCAL);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("write key failed, status: 0x%x", simlock_status);
  }

  return simlock_status;
} /* simlock_rsu_key_get_random_key */


/*===========================================================================
FUNCTION SIMLOCK_RSU_KEY_READ_KEYMOD_FILE

DESCRIPTION
  This function reads the modem key file and provides the key data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_key_read_keymod_file
(
  simlock_rsu_keymod_type        keymod_data
)
{
  simlock_result_enum_type    simlock_status       = SIMLOCK_SUCCESS;
  uint32                      keymod_file_size     = 0;
  uint8                     * keymod_file_data_ptr = NULL;

  do
  {
    /* read the entire file data. keymod_file_data_ptr holds the
       raw key data */
    simlock_status = simlock_file_read(&keymod_file_data_ptr,
                                       &keymod_file_size,
                                       SIMLOCK_FILE_RSU_KEY_MOD,
                                       SIMLOCK_SFS_LOCAL);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (keymod_file_data_ptr == NULL))
    {
      SIMLOCK_MSG_ERR_1("read key mod file failed, status: 0x%x",
                        simlock_status);
      break;
    }

     /* if the file size is not of the expected size,
       then it is not valid */
    if(keymod_file_size != SIMLOCK_RSU_KEY_MOD_LEN)
    {
      SIMLOCK_MSG_ERR_1("keymod file size is not as expected, 0x%x",
                        keymod_file_size);
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

    simlock_memscpy(keymod_data,
                    SIMLOCK_RSU_KEY_MOD_LEN,
                    keymod_file_data_ptr,
                    keymod_file_size);
  }while (0);

  SIMLOCK_MEM_FREE(keymod_file_data_ptr);
  return simlock_status;
} /* simlock_rsu_key_read_keymod_file */


/*===========================================================================
FUNCTION SIMLOCK_RSU_KEYMOD_GENERATE_AND_ENCRYPT

DESCRIPTION
  This function generates the modem key for the RSU. It encrypts the key,
  with the public key provisioned in the device

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_keymod_generate_and_encrypt
(
  simlock_data_type          * encrypted_key_ptr
)
{
  simlock_result_enum_type        simlock_status  = SIMLOCK_SUCCESS;
  simlock_rsu_pubkey_data_type  * pubkey_data_ptr = NULL;
  simlock_rsu_keymod_type         key_data_buf;

  memset(key_data_buf, 0x00, SIMLOCK_RSU_KEY_MOD_LEN);

  SIMLOCK_MSG_MED_0("simlock_rsu_key_generate");

  if(encrypted_key_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(encrypted_key_ptr, 0x00, sizeof(simlock_data_type));

  /* get the random key */
  simlock_status = simlock_rsu_key_get_random_key(key_data_buf);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("get random key failed, status: 0x%x", simlock_status);
    return simlock_status;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(pubkey_data_ptr, sizeof(simlock_rsu_pubkey_data_type));
  if(pubkey_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* read the public key parameters - modulus and exponent */
  simlock_status = simlock_rsu_key_read_pubkey_data(pubkey_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("unable to get the pubkey");
    SIMLOCK_MEM_FREE(pubkey_data_ptr);
    return simlock_status;
  }

  /* check the version */
  if(pubkey_data_ptr->version != SIMLOCK_RSU_PUBKEY_FILE_VERSION)
  {
    SIMLOCK_MSG_ERR_1("pubkey file version, 0x%x not supported",
                      pubkey_data_ptr->version);
    SIMLOCK_MEM_FREE(pubkey_data_ptr);
    return simlock_status;
  }

  /* perform the key encryption using RSA */
  simlock_status = simlock_crypto_rsa(SIMLOCK_CRYPTO_RSA_ENCRYPT,
                                      key_data_buf,
                                      SIMLOCK_RSU_KEY_MOD_LEN,
                                      pubkey_data_ptr->modulus,
                                      SIMLOCK_RSU_PUBKEY_MODULUS_LEN,
                                      pubkey_data_ptr->exponent,
                                      SIMLOCK_RSU_PUBKEY_EXPONENT_LEN,
                                      encrypted_key_ptr);
  SIMLOCK_MEM_FREE(pubkey_data_ptr);

  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("key encryption failed, status: 0x%x", simlock_status);
    return simlock_status;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_rsu_key_generate */
#endif /* FEATURE_SIMLOCK_RSU */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */
