#ifndef SECPKX_H
#define SECPKX_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
            P U B L I C   K E Y   E X C H A N G E   R O U T I N E S


DESCRIPTION
  This is the header file for the Securoty Services PKX module.
  The PKX module provides functions to execute public key exchange
  operations and signing / authentication procedures using the 
  RSA algorithm.

EXTERNALIZED FUNCTIONS

  secpkx_set_param()        - Set instance-specific parameters
  secpkx_modexp()           - Do a basic modulo exponentiation
  secpkx_create_key()       - Create a key during a Diffie-Hellman key 
                              exchange (Sequence: PRIVATE - PUBLIC - SECRET)
  secpkx_encrypt()          - Encrypt a message using the peer's public
                              RSA key
  secpkx_decrypt()          - Decrypt a message using one's private RSA key
  secpkx_sign()             - Sign a message using one's private RSA key
  secpkx_authenticate()     - Authenticate an RSA signature using the
                              originator's public RSA key

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secpkx.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
06/10/09   ejt     Removed CS_PUBKEYRSA & seccrypt.h
05/14/07   rv      CS IPubKey integration
02/25/05   sb      Added support for Rsa signing, secure modmath operations
11/12/04   rv      changes to correct LINT errors
02/18/04   rwh     Replaced literal values in modmath parameter size defs.
09/04/03   om      Added padded_param_size struct for ARM and DSP usage.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"
#include "secapi.h"
#include "seccrypti.h"

#include "secpkxarm.h"
#include "secpkxhw.h"

/*===========================================================================

                        DEFINITIONS AND CONSTANTS

===========================================================================*/

/*---------------------------------------------------------------------------
  Bit masks that represent each PKX parameter. These are used to
  set and determine the status of a PKX instance.
---------------------------------------------------------------------------*/
#define SECPKX_MODEXP_PARAMETERS            0x00800000
#define SECPKX_MODEXP_BASE_PARAMETER        0x00000001
#define SECPKX_MODEXP_MODULUS_PARAMETER     0x00000002
#define SECPKX_MODEXP_EXPONENT_PARAMETER    0x00000004

#define SECPKX_DH_PARAMETERS                0x00400000
#define DH_GROUP_PARAMETER                  0x00000001

#define SECPKX_RSA_ENC_PARAMETERS           0x00200000
#define SECPKX_RSA_DEC_PARAMETERS           0x00100000
#define SECPKX_RSA_BASE_PARAMETER           0x00000001
#define SECPKX_RSA_MODULUS_PARAMETER        0x00000002
#define SECPKX_RSA_PRIVATE_KEY_PARAMETER    0x00000004
#define SECPKX_RSA_PUBLIC_KEY_PARAMETER     0x00000008
#define SECPKX_RSA_PRIV_KEY_IV_PARAMETER    0x00000010

/* Instance doing a basic modulo exponentiation */
#define SECPKX_MODEXP_INST_READY_MASK   (SECPKX_MODEXP_PARAMETERS           \
                                         | SECPKX_MODEXP_BASE_PARAMETER     \
                                         | SECPKX_MODEXP_MODULUS_PARAMETER  \
                                         | SECPKX_MODEXP_EXPONENT_PARAMETER)

/* Instance doing a Diffie-Hellman key exchange */
#define SECPKX_DH_INST_READY_MASK       ( DH_GROUP_PARAMETER )

/* Instance doing an RSA key exchange */
#define SECPKX_RSA_INST_ENC_READY_MASK  (SECPKX_RSA_MODULUS_PARAMETER     \
                                         | SECPKX_RSA_PUBLIC_KEY_PARAMETER)
                                         
#define SECPKX_RSA_INST_DEC_READY_MASK  (SECPKX_RSA_MODULUS_PARAMETER     \
                                         | SECPKX_RSA_PRIVATE_KEY_PARAMETER)

/* Instance doing an RSA signature operation */
#define SECPKX_RSA_INST_SIGN_READY_MASK  SECPKX_RSA_INST_DEC_READY_MASK

/* Instance doing an RSA signature operation with Protected private key */
#define SECPKX_RSA_INST_PROT_SIGN_READY_MASK  (SECPKX_RSA_INST_DEC_READY_MASK \
                                               | SECPKX_RSA_PRIV_KEY_IV_PARAMETER)	

/* Instance doing an RSA signature verification */
#define SECPKX_RSA_INST_AUTH_READY_MASK  SECPKX_RSA_INST_ENC_READY_MASK


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for a PKX instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct secpkx_instance_s
{
  struct __pkx_common_info_s
  {
    /* Keep track whether this instance is currently executing */
    boolean busy;

    /* Keep track of execution time */
    //seccrypt_exec_time_type time;

    /* Padding type */
    secapi_pkx_padding_enum_type  padding;

    /* The ARM implementation needs all parameters 32-bit aligned, */
    /* so we do a dummy filler here to guarantee that.             */
    uint32 dummy;

    union
    {
      /* Generic parameters for a modulo exponentiation */
      struct __pkx_mod_exp_storage_s
      {
        uint8  base    [SECAPI_PKX_MAX_BASE_SIZE];
        uint8  modulus [SECAPI_PKX_MAX_MOD_SIZE];
        uint8  exponent[SECAPI_PKX_MAX_EXP_SIZE];
        uint8  result  [SECAPI_PKX_MAX_EXP_SIZE];		
		uint8  priv_key_iv[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
		uint8  priv_key_devkey[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
      } mod_exp;

      /* Parameters for a Diffie-Hellman operation */
      struct __pkx_dh_storage_s
      {
        struct
        {
          uint8  base   [SECAPI_PKX_MAX_BASE_SIZE];
          uint8  modulus[SECAPI_PKX_MAX_MOD_SIZE];
        } group;

        uint8  my_private [SECAPI_PKX_MAX_EXP_SIZE];
        uint8  my_public  [SECAPI_PKX_MAX_MOD_SIZE];
        uint8  peer_public[SECAPI_PKX_MAX_MOD_SIZE];
        uint8  secret     [SECAPI_PKX_MAX_MOD_SIZE];
      } dh;

      /* Parameters for an RSA operation */
      struct __pkx_rsa_storage_s
      {
        uint8  modulus    [SECAPI_PKX_MAX_MOD_SIZE];
        uint8  padded_data[SECAPI_PKX_MAX_BASE_SIZE];
        uint8  private_key[SECAPI_PKX_MAX_EXP_SIZE];
        uint8  public_key [SECAPI_PKX_MAX_EXP_SIZE];
        uint8  cipher_data[SECAPI_PKX_MAX_MOD_SIZE];
		uint8  priv_key_iv[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
		uint8  priv_key_devkey[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
	  } rsa;

      /* Parameters for an ECC operation */
      struct __pkx_ecc_storage_s                                        
      {
        secerrno_enum_type  sec_errnum;
        uint8   public_key_x[SECAPI_PKX_ECC_MAX_PUBLIC_KEY_LEN];      
        uint8   public_key_y[SECAPI_PKX_ECC_MAX_PUBLIC_KEY_LEN];      
        uint32  public_key_size;       
        uint8   private_key[SECAPI_PKX_ECC_MAX_PRIVATE_KEY_LEN];       
        uint32  private_key_size;      
        char    m[SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN];  
        char    a[SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN];  
        char    b[SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN];
        char    G_x[SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN];  
        char    G_y[SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN];  
        char    n[SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN];  
      } ecc;

    } param_storage;

    /* References to the mod-math parameters of the current operation */
    struct
    {
      uint8*  base;
      uint8*  exponent;
      uint8*  modulus;
      uint8*  result;
	  uint8*  exponent_iv;
	} param_reference;
    
    /* Sizes of the parameters of the current operation */
    struct
    {
      uint16 base;
      uint16 exponent;
      uint16 modulus;
	  uint16 exponent_iv;
	} param_size;
    
    /* ------------------------------------------------------------ */
    /* Shared section with dsp                                      */
    /* ------------------------------------------------------------ */
    struct
    {
      uint16 base;
      uint16 exponent;
      uint16 modulus;
    } padded_param_size;
    
  } common;

  /* All implementation-specific data is in here */
  union
  {
    secpkxarm_instance_type  arm;
    secpkxhw_instance_type   hw;
  } platform;

};


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/

/* ---------------------------------------------------------------------- */
/* PKX State Machine transition function tables                           */
/* ---------------------------------------------------------------------- */
extern seccrypts_state_x_funcs_table_type    secpkx_x_func_table;
extern seccrypts_state_x_allowed_table_type  secpkx_x_allowed_table;


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECPKX_INITIALIZE_INSTANCE

DESCRIPTION
  This function sets the default parameters for the passed-in 
  PKX instance.

DEPENDENCIES
  None

PARAMETERS
  instance_ptr  - Pointer to PKX instance

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secpkx_initialize_instance
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_SET_PARAM()

DESCRIPTION
  This function allows to set instance-specific parameters.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  E_SUCCESS      if the function executes successfully.
  E_INVALID_ARG  if a paremeter is out of range
  E_FAILURE      if any other failures.

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_set_param
(
  seccrypt_instance_type*      instance_ptr,
  secapi_param_enum_type       param,
  secapi_pkx_param_data_type*  param_data_ptr
);

/*===========================================================================

FUNCTION SECPKX_ABORT()

DESCRIPTION
  This function 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_abort
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_CREATE_KEY()

DESCRIPTION
  This function will create a key using the Diffie-Helman algorithm. The
  key created is dependent on the passed key type: Private, Public, or
  Secret. The Key will be written to the passed address.

  This function will use previously stored values for the base, modulus,
  and key information needed by the Diffie-Helman algorithm. Note if none
  of these values have been set (via "secpkx_dh_set_param") then default
  values will be used.

  For ASYNC mode operation there is a callback function invoked upon
  completion of the Diffie-Helman key.

DEPENDENCIES
  The passed Crypto-instance must be initialized to perform the Diffie-Helman
  algorithm
  The Maximum calculation time stored for Diffie-Helman must have a non-zero
  value.

PARAMETERS

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  If a Private key is requested the generated key will be stored as the
  Diffie-Helman Key for the passed instance of Diffie-Helman
===========================================================================*/
extern secerrno_enum_type secpkx_create_key
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_MODEXP()

DESCRIPTION
  This function 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_modexp
(
  seccrypt_instance_type*     instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_ENCRYPT()

DESCRIPTION
  This function 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_encrypt
(
  seccrypt_instance_type*     instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_DECRYPT()

DESCRIPTION
  This function 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_decrypt
(
  seccrypt_instance_type*     instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_SIGN()

DESCRIPTION
  This function 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_sign
(
  seccrypt_instance_type*     instance_ptr
);

/*===========================================================================

FUNCTION SECPKX_AUTHENTICATE()

DESCRIPTION
  This function 

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secpkx_authenticate
(
  seccrypt_instance_type*     instance_ptr
);


/*===========================================================================

FUNCTION SECPKX_FINISH_OP()

DESCRIPTION
  This function does common completion work for a PKX instance.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Writes data to diag log.
===========================================================================*/
void secpkx_finish_op
(
  seccrypt_instance_type*  instance_ptr
);

#endif /* SECPKX_H */
