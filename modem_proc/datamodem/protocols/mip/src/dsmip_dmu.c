/*===========================================================================

                  M O B I L E   I P   D Y N A M I C 

                  K E Y   U P D A T E   M O D U L E 

GENERAL DESCRIPTION

  This module contains all functions required to implement the 
  Mobile IP Dynamic Key Update (version 1.3, rev 6) protocol.  

  This module makes use of the security/crypto and EFS API's.

  This module is called from the PS task context.

  The DMSS DMU implementation implements the following encryption only:
  - RSA-1024 Key data encryption (ATV=1) for DMU version 1.3 (DMUV=0)
  - Cleartext operation (DMUV=7) for interoperability testing support

  The mobile is provisioned with at least one RSA-1024 public key, stored 
  as an EFS file.  (The file format is described in the appropriate read 
  function's comment block)

  In the future, public keys may be stored in compliance with the 
  Distinguished Encoding Rules (DER) format. 

EXTERNALIZED FUNCTIONS

  dmu_init()                   - initialize the DMU module
  dmu_gen_key_data             - generate temporary random keys 
  dmu_pkoid_supported          - Check if a public key exists for given PKOID 
  dmu_encrypt_key_data         - encrypt the temporary random keys 
  dmu_rand_sig_handler         - perform DMU random generation on signal
  dmu_commit_key_data          - commit provided shared secrets to NV

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dmu_init must be called before any of the other external API's.

Copyright (c) 2001-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/dsmip_dmu.c_v   1.10   14 Feb 2003 17:35:44   jeffd  $                      
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_dmu.c#1 $                       $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
when        who    what, where, why
-------     ---    -----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
06/03/13    sd     ATCoP decoupling changes
07/22/11    dvk    Global Variable Cleanup
07/15/11    rp     QShrink2 changes: MSG_SPRINTF is used instead of MSG buffers.
05/20/11    op     Migrated to MSG 2.0 macros
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/11/11    ack    Global Variable Cleanup
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
06/16/10    ms     Migrating to new EFS APIs.
03/31/10    gc     Added efs_sync() call
10/15/09    vs     SU Level Api Defeaturization
03/11/09    ls     DMU directory is always set to "/DMU"
07/16/08    ac     Change is to have DMU register for secapi_register_wait_func 
                   (psi_wait) which will allow the DS watchdog to be petted while 
				   waiting on the SEC RSA software encryption to finish.
06/11/07    as     Change HDR AN password when committing DMU keys
04/10/07    as     Removed featurized code that is not used any more for DMU
                   Using secapi_get_random to start preencrypt dmu keys
11/09/06    as     Added new definition of DMUEFSI_PK_DIR for 7500 4.2
05/10/06   ssh     Update Simple IP password in PPP control block after DMU
                   completes.
10/28/05   as/vrk  added #include "dsmip_metasm.h"
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
01/19/05    jd     Require encrypted PPP authentication after DMU update
04/28/04    jd     Linted the file
02/02/04    jd     Changed DMU encrypt signal handler to a command handler
01/23/04    jd     Removed support for FEATURE_SEC_DEPRICATED
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
01/20/04    jd     dmui_pre_encrypt_all_sessions returns operation result.
                   Only set dmu_rand_ready flag if pre-encryption succeeds. 
01/19/04    jd     Free the crypto instance after encryption is complete, so
                   security services will free the DSP for other clients to 
                   use.  Made encryption synchronous as well for simplicity.
07/31/03    mvl    Re-added jd's change from 05/20/03
05/29/03    jay    Changes for new crypto API
05/20/03    jd     Now write CHAP PPP_PASSWORD_I for multimode builds too
02/14/03    jay    Handle encryption failure during DMU
12/29/02    jd     Fixed includes for multimode builds
                   Added params to dmu_sig_handler to be compatible with 
                   ps_set_sig_handler()
12/29/02    jd     Added dmu_rand_sig_handler, moving DMU random signal code
                   out of psmgr and into dmu source
12/23/02    jd     Added dmu_commit_key_data
12/10/02    jd     Cleaned up comments, 
                   removed unused param from dmui_get_pkoid_from_nv()
10/02/02    jay    Added a function to read PKOID from NV and other fixes
                   based on DMU inetrop in SD.
09/20/02    jay    Read MN_Auth from NV and copy to dmu_info structure.
05/22/02    jay    Added functions to generate random data and encrypt them.
05/08/02    jd     Created file

===========================================================================*/


/*===========================================================================

             Include Files for Module 

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsmip_api.h"
#include "data_msg.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU

#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "amssassert.h"
#include "fs_public.h"
#include "dsmip_dmu.h"
#include "nv.h"
#include "secutil.h"

#include "secapi.h"
#include "pstimer.h"
#include "dsat707mipctab.h"
#include "ps_utils.h"
#include "ps_ppp.h"
#include "ds_1x_profile.h"
#include "ds707_data_session_profile.h"
#include "dsmip_metasm.h"


#include <stringl/stringl.h>
#include "psi.h"

#include "ps_system_heap.h"
#include "dsat707mip.h"
#include "mcfg_fs.h"

/* ======================================================================= */
/*                       External Data                                     */
/* ======================================================================= */

/*---------------------------------------------------------------------------
  Temporary MIP keys.  

  These keys are pre-generated at system startup, then pre-encrypted with 
  the public key corresponding to the PKOID indicated in the active MIP user 
  profile. 
---------------------------------------------------------------------------*/
static dmu_info_type  mip_dmu_info[MIP_MAX_SESSIONS];

/*---------------------------------------------------------------------------
  Flag to indicate if searcher has started and random data will be available
  for key generation and encryption processes.
---------------------------------------------------------------------------*/
static boolean        mip_dmu_rand_ready = FALSE;


/* ======================================================================= */
/*                       Local Data & Definitions                          */
/* ======================================================================= */

/*---------------------------------------------------------------------------
  Macros to convert between dmu_info pointer and dmu_info array index
---------------------------------------------------------------------------*/
#define MIP_DMU_PTR_2_INDEX(dmu_ptr)  abs((dmu_ptr) - mip_dmu_info)
#define INDEX_2_DMU_INFO_PTR(index)   (mip_dmu_info + (index))

/*---------------------------------------------------------------------------
  DMU Public Key directory name (subdirectory in EFS root directory)
---------------------------------------------------------------------------*/
#define DMUEFSI_PK_DIR  "DMU"

/*---------------------------------------------------------------------------
  Maximum length of public key filename, i.e. sizeof("/DMU/255.key\0") 
---------------------------------------------------------------------------*/
#define DMUEFSI_PK_FNAME_LEN  (sizeof(DMUEFSI_PK_DIR)+sizeof("/###.key")+1)

/*---------------------------------------------------------------------------
  buffer for full path public key filename
---------------------------------------------------------------------------*/
LOCAL char  dmui_public_key_filename[DMUEFSI_PK_FNAME_LEN];

static byte  ds_1x_active_profile_id = DS_1X_INVALID_ACTIVE_PROFILE_ID;


/* ======================================================================= */
/*                       Internal Functions                                */
/* ======================================================================= */
LOCAL byte dmui_get_active_mip_profile_id (void);

/* = = = = = = = = = = = = = = = = = = = = = = = =  = = = = = = = = = = =  */
/*                       NV & EFS function                                 */
/* = = = = = = = = = = = = = = = = = = = = = = = =  = = = = = = = = = = =  */


/*===========================================================================

FUNCTION DMUI_GET_PKOID

DESCRIPTION

  Get the provisioned pkoid from NV for the active MIP profile 

PARAMETERS

  None 

RETURN VALUE
  -1          - couldn't read key
  0-255       - PKOID read from NV
  256..65534  - should never happen

DEPENDENCIES
  None
===========================================================================*/
LOCAL sint15 dmui_get_pkoid_from_nv
(
  void
)
{
  nv_stat_enum_type  nv_status;
  nv_item_type      *ps_nv_item_ptr;
  sint15             pkoid;
/*- -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Read the pkoid for this session
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr->ds_mip_dmu_pkoid.index = dmui_get_active_mip_profile_id();

  nv_status = ps_get_nv_item (NV_DS_MIP_DMU_PKOID_I, ps_nv_item_ptr);
  if (nv_status != NV_DONE_S)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Couldn't read pkoid for profile %d",
                  ps_nv_item_ptr->ds_mip_dmu_pkoid.index);
    PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
    return -1;
  }
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Read pkoid %d for profile %d", 
           ps_nv_item_ptr->ds_mip_dmu_pkoid.pkoid, 
           ps_nv_item_ptr->ds_mip_dmu_pkoid.index);

  pkoid = (sint15)ps_nv_item_ptr->ds_mip_dmu_pkoid.pkoid;
  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
  return ( pkoid );

} /* dmui_get_pkoid_from_nv() */



/*===========================================================================

FUNCTION DMUI_EFSI_READ_KEY_RSA_1024

DESCRIPTION
  Read the RSA-1024 public key value from the specified EFS file (opened 
  previously) to the specified buffer.

  Note there is no bounds checking on public_key parameter.

  RSA-1024 public key format:

    +---------------+
    | public_key    +  
    | |- exponent   +  (128 bytes)
    | +- modulus    +  (128 bytes)
    +---------------+

PARAMETERS

  fd              - EFS File Descriptor to read from (input) - must be open already 
  public_exponent - public key exponent value (output)
  modulus         - public key modulus value (output)

RETURN VALUE
  Length of the key, in bytes.
  -1 = couldn't read key

DEPENDENCIES
  None

===========================================================================*/
LOCAL boolean dmui_efsi_read_eky_rsa_1024
(
  int *             fd,
  byte *            public_exponent,
  byte *            modulus
)
{

  /*-------------------------------------------------------------------------
    Read the exponent into the given buffer.
    Note that the efs_read call is blocking.
  -------------------------------------------------------------------------*/
  if (mcfg_fread(*fd, 
                 public_exponent, 
                 MIP_DMU_RSA1024_KEY_LEN, 
                 MCFG_FS_TYPE_EFS ) == -1)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,  "efs_read failed");
    return (FALSE);
  }

  /*-------------------------------------------------------------------------
    Read the modulus into the given buffer
    Note that the efs_read call is blocking.
  -------------------------------------------------------------------------*/
  if (mcfg_fread( *fd, modulus, MIP_DMU_RSA1024_KEY_LEN, MCFG_FS_TYPE_EFS) 
      == -1)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,  "efs_read failed");
    return (FALSE);
  }

  return (TRUE);
} /* dmui_efsi_read_eky_rsa_1024() */

/*===========================================================================

FUNCTION DMUI_GEN_PK_FILENAME

DESCRIPTION
  Generate the EFS path to the public key file for the specified PKOID.

PARAMETERS

  pkoid  - pkoid of key for which filename is generated

RETURN VALUE
  pointer to the static char array where filename is stored

DEPENDENCIES
  Function is non-reentrant, always call from the same task.
  Filename storage is global - never 


===========================================================================*/
LOCAL char * dmui_gen_pk_filename
(
  byte  pkoid
)
{
/*-  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /*-------------------------------------------------------------------------
    Build the filename, based on pkoid
  -------------------------------------------------------------------------*/
  (void) strlcpy(
      dmui_public_key_filename, 
      DMUEFSI_PK_DIR, 
      DMUEFSI_PK_FNAME_LEN);

  (void) snprintf(
      &dmui_public_key_filename[sizeof(DMUEFSI_PK_DIR) - 1], 
      DMUEFSI_PK_FNAME_LEN - sizeof(DMUEFSI_PK_DIR) + 1,
      "/%d.key", 
      pkoid);

  DATA_MSG_SPRINTF_1(MSG_LEGACY_MED,"Public key filename is %s",
                     dmui_public_key_filename);

  return( dmui_public_key_filename );

} /* dmui_gen_pk_filename() */



/*===========================================================================

FUNCTION DMUI_EFS_READ_PK

DESCRIPTION

  Read the public key from the EFS file to the specified buffer.

  Public keys are stored in the directory:   /DMU
  Filenames are assumed to be of the format: <pkoid>.key
   where pkoid is the text string of PKOID as a decimal number

  Index file format:

    +---------------+
    | pk_info       +
    | |- pkoid      +  (1 byte)  
    | |- pkoi       +  (1 byte)
    | |- pk_Expansn +  (1 byte)  (set to dflt = 0xff, for dmuv 0 (v1.3))
    | |- atv        +  (4 bits)  (currently only support 1 (RSA-1024))
    | +- dmuv       +  (4 bits)  (this implementation = 0x00 (DMU v1.3))
    +---------------+
    | public_key    +  (variable) public key value, format dependent on atv
    +---------------+

  RSA-1024 public key file format:

    +---------------+
    | public_key    +  
    | |- exponent   +  (128 byte)
    | +- modulus    +  (128 byte)
    +---------------+

  Future:  read public keys at startup (dmu_efs_init) to verify they are ok,
           so provisioners will have some indication at startup if they did 
           their job right.

PARAMETERS

  pkoid           - public key organization identifier (input)
  pk_info         - public key identifier (output)
  public_exponent - public key exponent value (output)
  modulus         - public key modulus value (output)

RETURN VALUE
  Length of the key, in bytes.
  -1 = couldn't read key

DEPENDENCIES
  None

===========================================================================*/
LOCAL boolean dmui_efs_read_pk
(
  byte           pkoid,
  dmu_info_type *dmu_info
)
{
  int                fd  = -1,length = 0;
  boolean            retval = FALSE;
/*-  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  DATA_MIP_MSG0(MSG_LEGACY_MED, "Retrieve DMU public key");

  /*-------------------------------------------------------------------------
    Generate the public key filename
  -------------------------------------------------------------------------*/
  (void) dmui_gen_pk_filename( pkoid );

  /*-------------------------------------------------------------------------
    Open the public key file
  -------------------------------------------------------------------------*/
  fd = mcfg_fopen(dmui_public_key_filename, 
                  O_RDONLY,0, 
                  MCFG_FS_TYPE_EFS,
                  MCFG_FS_SUBID_NONE);
  if( fd == -1 ) 
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,"Cannot open file for read");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Read Public Key Identifier info
  -------------------------------------------------------------------------*/
  length = mcfg_fread(fd,         
                      &(dmu_info->pk_info), 
                      sizeof (pki_type), 
                      MCFG_FS_TYPE_EFS);
  if ( length > 0 )
  {
    /*-----------------------------------------------------------------------
      Force filename pkoid over pk info.pkoid in the file itself
      (to allow duplicating a key by copying the file and renaming)
      // JD - add this to DMU design doc & customer guide
    -----------------------------------------------------------------------*/
    if (dmu_info->pk_info.pkoid != pkoid)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Warning: pkoid in file != filename!");
      dmu_info->pk_info.pkoid = pkoid;
    }

    /*-----------------------------------------------------------------------
      Read the key based on algorithm type
    -----------------------------------------------------------------------*/
    switch (dmu_info->pk_info.av)
    {
      /*---------------------------------------------------------------------
        RSA-1024 type supported
      ---------------------------------------------------------------------*/
      case MIP_DMU_AV_RSA_1024:
        retval = dmui_efsi_read_eky_rsa_1024 (&fd,  
                                              dmu_info->pub_exp,  
                                              dmu_info->modulus);
        break;
  
      /*---------------------------------------------------------------------
        Otherwise, not supported, return
      ---------------------------------------------------------------------*/
      default:
        DATA_MIP_MSG1(MSG_LEGACY_ERROR,  "ATV type (%d) not supported", dmu_info->pk_info.av);
        break;
    }
  }
  else if( length == 0 )
  {
    DATA_MIP_MSG0(MSG_LEGACY_MED,  "EFS file reached end of file ");
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,  "efs_read failed ");
  }
 
  /*-------------------------------------------------------------------------
    Close the public key file
  -------------------------------------------------------------------------*/
  if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) == -1)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,  "efs_close failed ");
  }
  return (retval);

} /* dmui_efs_read_pk() */



/* = = = =  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  */
/*                        Encryption functions                             */
/* = = = =  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  */


/*============================================================================
FUNCTION DMUI_GET_CRYPTO_INSTANCE

DESCRIPTION
  Get a crypto instance and set it up to do encryption for DMU

PARAMETERS
  dmu_info:  pointer to dmu_info block where allocated crypto handle will be
             stored

RETURN VALUE
  FALSE:  Couldn't allocate or configure crypto instance for dmu
  TRUE:   Crypto instance successfully allocated and configured for dmu
  None
    
DEPENDENCIES
  crypto instance must be freed by caller

SIDE EFFECTS
  None
============================================================================*/
LOCAL boolean dmui_get_crypto_instance
( 
  dmu_info_type * dmu_info
)
{
  secapi_pkx_param_data_type  rsa_param;
  secerrno_enum_type          sec_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*--------------------------------------------------------------------------
    Get a crypto handle for this session. 
  --------------------------------------------------------------------------*/
  sec_errno = secapi_new(&(dmu_info->crypto_handle), SECAPI_RSA);

  if(sec_errno != E_SUCCESS)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Error creating crypto handler (%d)",sec_errno);
    return FALSE;
  }
   
  /*--------------------------------------------------------------------------
    Do the encryption in synchronous mode to avoid lots of commanding.

    This is pretty fast on newer targets (< 50ms), but might be slow on <6100
  --------------------------------------------------------------------------*/
  rsa_param.common.exec_mode = SECAPI_SYNC_EXEC;
  sec_errno = secapi_pkx_set_param( dmu_info->crypto_handle,
                                    SECAPI_RSA,
                                    SECAPI_EXEC_MODE,
                                    &rsa_param );
  if( sec_errno != E_SUCCESS )
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Err couldn't set RSA to async mode (%d)", sec_errno);
    return FALSE;
  }

  return TRUE;

} /* dmui_get_crypto_instance() */



/*============================================================================
FUNCTION DMUI_FREE_CRYPTO_INSTANCE

DESCRIPTION
  Frees a crypto instance associated with specified DMU session info

PARAMETERS
  dmu_info:  pointer to dmu_info block containing crypto handle to free

RETURN VALUE
  None
    
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL void dmui_free_crypto_instance
(
  dmu_info_type *dmu_info
)
{
  secerrno_enum_type  sec_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*--------------------------------------------------------------------------
    Free allocated crypto handle.  crypto_handle is nulled in sec*_delete()
  --------------------------------------------------------------------------*/
  if (dmu_info->crypto_handle != NULL)
  {
    sec_errno = secapi_delete (&(dmu_info->crypto_handle));
    if (sec_errno != E_SUCCESS)
    {
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Error deleting crypto handler (%d)",sec_errno);
    }
  }
} /* dmui_free_crypto_instance() */



/*============================================================================
FUNCTION DMU_CONFIG_PUB_KEY

DESCRIPTION
  Configure the crypto instance for the specified session to use the key 
  file named by the pkoid in the NV item corresponding to this session 

PARAMETERS
  pkoid    - pkoid to configure public key for or 
             load pkoid specified in active MIP profile if PKOID_WILD_CARD
  dmu_info - session for which to configure public key

RETURN VALUE
  Success or Failure 

DEPENDENCIES
  None

SIDE EFFECTS
  DMU info is filled with public key and pk info

============================================================================*/
LOCAL boolean dmui_config_pub_key
(
  dmu_info_type * dmu_info
)
{
  sint15                      pkoid;
  secerrno_enum_type          sec_errno;
  secapi_pkx_param_data_type  rsa_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /*--------------------------------------------------------------------------
    If requested, read pkoid from profile info
  --------------------------------------------------------------------------*/
  pkoid = dmu_info->pk_info.pkoid;
  if (pkoid == PKOID_UNLOADED)
  {
    pkoid = dmui_get_pkoid_from_nv();
    if (pkoid == -1)
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Error while reading pkoid from NV");
      return FALSE;
    }
    dmu_info->pk_info.pkoid = (byte) pkoid;
  }

  /*--------------------------------------------------------------------------
    Read the modulus and exponent value from efs
  --------------------------------------------------------------------------*/
  if (!dmui_efs_read_pk( (byte) pkoid, dmu_info))
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Error while reading PK from EFS");
    return FALSE;
  }
  
  /*--------------------------------------------------------------------------
    Set the modulus value in the crypto instance configuration
  --------------------------------------------------------------------------*/
  rsa_param.rsa.key.modulus_ptr  = dmu_info->modulus;
  rsa_param.rsa.key.modulus_size = MIP_DMU_RSA1024_KEY_LEN;
  sec_errno = secapi_pkx_set_param( dmu_info->crypto_handle,
                                    SECAPI_RSA,
                                    SECAPI_PKX_RSA_MODULUS,
                                    &rsa_param );
  if(sec_errno != E_SUCCESS)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Error setting modulus (%d)",sec_errno);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Set the exponent e value in the crypto instance configuration
  --------------------------------------------------------------------------*/
  rsa_param.rsa.key.public_key_ptr = dmu_info->pub_exp;
  rsa_param.rsa.key.key_size       = MIP_DMU_RSA1024_KEY_LEN;
  sec_errno = secapi_pkx_set_param( dmu_info->crypto_handle,
                                    SECAPI_RSA,
                                    SECAPI_PKX_RSA_PUBLIC_KEY,
                                    &rsa_param );
  if(sec_errno != E_SUCCESS)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Error setting exp_e (%d)",sec_errno);
    return FALSE;
  }

  return TRUE;
} /* dmui_config_pub_key() */



/*============================================================================
FUNCTION DMUI_START_ENCRYPTION

DESCRIPTION
  Start encrypting the shared secrets (key data)

PARAMETERS
  dmu_info:  pointer to dmu_info block containing key_data to be encrypted

RETURN VALUE
  FALSE:  Error starting encryption
  TRUE:   encryption has started
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL boolean dmui_start_encryption
(
  dmu_info_type *dmu_info
)
{
  secerrno_enum_type  sec_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /*--------------------------------------------------------------------------
    Set the key data length.  
 
    With the old crypto API, this is in bits.
    With the new crypto API, this is in bytes.  
  --------------------------------------------------------------------------*/
  dmu_info->key_data_len = sizeof(dmu_key_data_type);

  /*--------------------------------------------------------------------------
    Start the encryption process
  --------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------
    Initialize encrypted_key_data_len to the buffer length provided to 
    crypto API.  Will be set to actual length of encrypted data on return.
  --------------------------------------------------------------------------*/
  dmu_info->encrypted_key_data_len = MIP_DMU_ENCRYPTED_KEY_DATA_LEN;
  sec_errno = secapi_pkx_encrypt(  dmu_info->crypto_handle,
                                   SECAPI_RSA,
                                   SECAPI_PKX_RSA_PADDING_ENC_PKCS1_V15,
                                   (byte *) &(dmu_info->key_data),
                                   dmu_info->key_data_len,
                                   dmu_info->encrypted_key_data,
                                   &(dmu_info->encrypted_key_data_len) );
  if (sec_errno != E_SUCCESS)
  {
    /*------------------------------------------------------------------------
      Future:  If this fails, perhaps this can be handled at runtime.  
    ------------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Err during encryption request (%d)",sec_errno);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Some sanity checks
  --------------------------------------------------------------------------*/
  if( dmu_info->encrypted_key_data_len != MIP_DMU_ENCRYPTED_KEY_DATA_LEN )
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Malformed encryption data %d",
                  dmu_info->encrypted_key_data_len); 
    return FALSE;
  }
  
  return TRUE; 

} /* dmui_start_encryption() */
 


/*============================================================================
FUNCTION DMUI_ENCRYPT_KEY_DATA

DESCRIPTION
  Encrypts the shared secrets using RSA - 1024

PARAMETERS
  dmu_info:  pointer to dmu_info block containing key_data to be encrypted
             and pkoid indicating which public key to load

RETURN VALUE
  FALSE:  Error before encryption was started
  TRUE:   Shared secret encryption has started
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL boolean dmui_encrypt_key_data
(
  dmu_info_type *dmu_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /*--------------------------------------------------------------------------
    Clear the encrypted flag
  --------------------------------------------------------------------------*/
  dmu_info->encrypted = FALSE;

  /*--------------------------------------------------------------------------
    Get a crypto instance and control of the QDSP
    - this MUST be freed by the caller immediately after return
  --------------------------------------------------------------------------*/
  if (!dmui_get_crypto_instance(dmu_info))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Couldn't allocate crypto handles - "
                  "can't do dmu!");
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Configure the crypto instance with the public key
  --------------------------------------------------------------------------*/
  if (!dmui_config_pub_key(dmu_info))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Couldn't configure public key - "
                  "can't do dmu!");
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Encrypt the key data
  --------------------------------------------------------------------------*/
  if (!dmui_start_encryption(dmu_info))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Couldn't start key encryption!");
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Set the flag to indicate that temporary dmu keys are encrypted now
  --------------------------------------------------------------------------*/
  dmu_info->encrypted = TRUE;

  return TRUE;

} /* dmui_encrypt_key_data() */

/*============================================================================
FUNCTION DMUI_GET_ACTIVE_MIP_PROFILE_ID

DESCRIPTION
  This function returns the active MIP profile id. If ds_1x_runtime_profile
  is populated then we use MIP profile from there. Else we read the active
  MIP profile from NV. If this NV read fails then we use MIP profile from
  ds707_mip_static_profile

PARAMETERS
  None

RETURN VALUE
  byte: Active MIP profile id
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL byte dmui_get_active_mip_profile_id
(
  void
)
{
  byte                 active_mip_profile_id;
  nv_stat_enum_type    nv_status;
  nv_item_type        *ps_nv_item_ptr;
  ds707_mip_profile_type     *ds_static_profile_ptr = NULL;
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
  }

  if(mip_meta_sm_open())
  {
    active_mip_profile_id = (byte) ds707_get_mip_profile_index();
  }
  else /* read the active profile id from nv if not already read */
  {
    if (ds_1x_active_profile_id != DS_1X_INVALID_ACTIVE_PROFILE_ID)
    {
      active_mip_profile_id = ds_1x_active_profile_id;
    }
    else
    {
      nv_status = ps_get_nv_item(NV_DS_MIP_ACTIVE_PROF_I, ps_nv_item_ptr);
      if( nv_status == NV_DONE_S )
      {
        ds_1x_active_profile_id = (byte) ps_nv_item_ptr->ds_mip_active_prof;
        DATA_MIP_MSG1(MSG_LEGACY_HIGH, "Active MIP profile after nv read is: %d", ds_1x_active_profile_id);
        active_mip_profile_id = ds_1x_active_profile_id;
      }
      else /* if nv read for active profile id 
              fails then use ds707_mip_static_profile */
      { 
        ds_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
        if( ds_static_profile_ptr == NULL )
        {
          DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
        }
		
        active_mip_profile_id = (byte) 
                                 ds_static_profile_ptr->mip.ds_qcmipp_val;
      }
    } 
  }
  DATA_MIP_MSG1(MSG_LEGACY_HIGH, "Using current MIP profile as: %d", active_mip_profile_id);

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
  return active_mip_profile_id;
} /* dmui_get_active_mip_profile_id */


/* ======================================================================== */
/*                       External Functions                                 */
/* ======================================================================== */

/*============================================================================
FUNCTION DMU_INIT

DESCRIPTION
  Initialize DMU module

PARAMETERS
  None

RETURN VALUE
  None
    
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
void dmu_init
( 
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    Clear the DMU session info block
  --------------------------------------------------------------------------*/
  memset (mip_dmu_info, 0, sizeof (mip_dmu_info));

} /* dmu_init() */



/*============================================================================

FUNCTION DMU_GEN_KEY_DATA

DESCRIPTION
  Generates new temporary keys which will be used for during next DMU.

PARAMETERS
  dmu_info:  pointer to dmu_info block where temporary keys will be stored

RETURN VALUE
  None

DEPENDENCIES
  None

============================================================================*/
void dmu_gen_key_data
( 
  dmu_info_type *dmu_info  
)
{
  secerrno_enum_type sec_errno;
  nv_stat_enum_type  nv_status;
  nv_item_type      *ps_nv_item_ptr;
  byte               i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return;
  }

  /*--------------------------------------------------------------------------
    Generate random keys using the RFC 1750 compliant source of random 
    data (provided by security services).
  --------------------------------------------------------------------------*/
  sec_errno = secapi_get_random( SECAPI_SECURE_RANDOM,
                                 (byte *)&(dmu_info->key_data),
                                 sizeof(dmu_key_data_type) );
  /*--------------------------------------------------------------------------
    Random data generation failed, return FAILURE
  --------------------------------------------------------------------------*/
  if(sec_errno != E_SUCCESS)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Error generating random numbers");
    PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
    return;
  }

  /*--------------------------------------------------------------------------
    Retrieve MN Authenticator from NV, overwriting the random value 
    generated above.
  --------------------------------------------------------------------------*/
  ps_nv_item_ptr->ds_mip_dmu_mn_auth.index = dmui_get_active_mip_profile_id();

  nv_status = ps_get_nv_item(NV_DS_MIP_DMU_MN_AUTH_I,ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "DMU MN Auth unreadable");
    PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
    return;
  }
   
  /*--------------------------------------------------------------------------
    Copy mn_auth into dmu_info, clear mn auth after msg is built.
    Since NV structure is PACKED copy byte by byte.
  --------------------------------------------------------------------------*/
  for( i=0; i<MIP_DMU_MN_AUTH_LEN; i++)
  {
    dmu_info->key_data.mn_auth[i] = ps_nv_item_ptr->ds_mip_dmu_mn_auth.mn_auth[i];
  }

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
} /* dmu_gen_key_data() */



/*============================================================================
FUNCTION DMU_PKOID_SUPPORTED

DESCRIPTION
  Check if a public key exists for the given PKOID. 

PARAMETERS
  pkoid - pkoid of the key to look for

RETURN VALUE
  FALSE:  No key for this pkoid
  TRUE:   key present for this pkoid

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
boolean dmu_pkoid_supported
(
  byte pkoid
)
{
   struct fs_stat file_stat;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /*--------------------------------------------------------------------------
    Check if a readable file exists for this pkoid

    - note that we do not verify that the public key contained therein is 
      valid (i.e. length check, successfully configure crypto with key).
  --------------------------------------------------------------------------*/
  if(mcfg_fs_stat(dmui_gen_pk_filename( pkoid ), 
                  &file_stat,
                  MCFG_FS_TYPE_EFS,
                  MCFG_FS_SUBID_NONE) != MCFG_FS_STATUS_OK)
  {
    DATA_MIP_MSG1(MSG_LEGACY_HIGH, "Coudn't nametest public key file for pkoid = %d!", pkoid);
    return FALSE;
  }
  
  return TRUE;
} /* dmu_pkoid_supported() */

/*============================================================================
FUNCTION DMU_ENCRYPT_KEY_DATA

DESCRIPTION
  Encrypts the shared secrets using RSA - 1024

PARAMETERS
  None

RETURN VALUE
  FALSE:  Error before encryption was started
  TRUE:   Shared secret encryption has started
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
boolean dmu_encrypt_key_data
(
  dmu_info_type *dmu_info
)
{
  boolean  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /*--------------------------------------------------------------------------
    Encrypt the key_data
  --------------------------------------------------------------------------*/
  result = dmui_encrypt_key_data(dmu_info);

  /*--------------------------------------------------------------------------
    Free the crypto instance regardless of the result
  --------------------------------------------------------------------------*/
  dmui_free_crypto_instance(dmu_info);

  DATA_MIP_MSG0(MSG_LEGACY_MED, "Key encryption complete");
  return result;

} /* dmu_encrypt_key_data() */



/*============================================================================

FUNCTION DMU_PRE_ENCRYPT_ALL_SESSIONS

DESCRIPTION
  Generates new temporary keys and encrypt for all the sessions

PARAMETERS
  None

RETURN VALUE
  FALSE:  Pre-encryption failed
  TRUE:   Pre-encryption succeeded

DEPENDENCIES
  This is only called once shortly after startup

============================================================================*/
LOCAL boolean dmui_pre_encrypt_all_sessions
(
  void 
)
{
  dmu_info_type * dmu_info;
  byte            loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dmu_info = INDEX_2_DMU_INFO_PTR(0);

  for (loop=0; loop < MIP_MAX_SESSIONS; loop++, dmu_info++)
  {
    /*------------------------------------------------------------------------
      Generate new key data for this session
    ------------------------------------------------------------------------*/
    dmu_gen_key_data(dmu_info);

    /*------------------------------------------------------------------------
      Encrypt key data
    ------------------------------------------------------------------------*/
    if (!dmu_encrypt_key_data(dmu_info))
    {
      /*----------------------------------------------------------------------
        Future: encryption could not complete.  Handle this case.
      ----------------------------------------------------------------------*/
      DATA_MIP_MSG1(MSG_LEGACY_ERROR,"Couldn't encrypt key data for session %d", \
                    loop);
      return FALSE;
    }
  } /* for all sessions */

  return TRUE;
} /* dmui_pre_encrypt_all_sessions() */



/*============================================================================

FUNCTION DMU_COMMIT_KEY_DATA

DESCRIPTION
  Write shared secrets from provided key_data extension into NV

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

============================================================================*/
void dmu_commit_key_data
(
  dmu_key_data_type * key_data
)
{
  nv_ds_mip_ss_user_prof_type *  ss_profile;
  nv_stat_enum_type              nv_status;
  nv_item_type                  *ps_nv_item_ptr;
  byte *                         d;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return;
  }

  /*--------------------------------------------------------------------------
    Write MN-HA and MN-AAA shared secrets to active MIP profile

    Future:  decouple atcop dependency here
  --------------------------------------------------------------------------*/
  ss_profile = &ps_nv_item_ptr->ds_mip_ss_user_prof;
  ss_profile->index = dmui_get_active_mip_profile_id();

  d = (byte *) ss_profile->mn_ha_shared_secret;
  memscpy (d, 
           NV_MAX_MN_HA_SHARED_SECRET_LEN, 
           key_data->mn_ha_key, 
           MIP_DMU_MN_HA_KEY_LEN);
  ss_profile->mn_ha_shared_secret_length = MIP_DMU_MN_HA_KEY_LEN;

  d = (byte *) ss_profile->mn_aaa_shared_secret;
  memscpy (d, 
           NV_MAX_MN_AAA_SHARED_SECRET_LEN, 
           key_data->mn_aaa_key, 
           MIP_DMU_MN_AAA_KEY_LEN);
  ss_profile->mn_aaa_shared_secret_length = MIP_DMU_MN_AAA_KEY_LEN;

  nv_status = ps_put_nv_item (NV_DS_MIP_SS_USER_PROF_I, ps_nv_item_ptr);
  if (nv_status != NV_DONE_S)
  {
    /* should really pre-commit keys and not do DMU if pre-committing fails */
    ERR_FATAL("Error in writing DMU updated shared secret data to NV!",0,0,0);
  }

#ifdef FEATURE_DATA_MOBILE_IP_DMU_MODIFIES_AN_AUTH
  /*--------------------------------------------------------------------------
    Write A12 CHAP shared secret to PPP password NV item
  --------------------------------------------------------------------------*/
  d = (byte *) ps_nv_item_ptr->hdr_an_auth_passwd_long.passwd;
  memscpy (d, 
           NV_MAX_AN_AUTH_PASSWD_LONG_LENGTH, 
           key_data->chap_key, 
           MIP_DMU_CHAP_KEY_LEN);
  ps_nv_item_ptr->hdr_an_auth_passwd_long.passwd_len = MIP_DMU_CHAP_KEY_LEN;

  nv_status = ps_put_nv_item (NV_HDR_AN_AUTH_PASSWD_LONG_I, ps_nv_item_ptr);
  if (nv_status != NV_DONE_S)
  {
    /* should really pre-commit keys and not do DMU if pre-committing fails */
    ERR_FATAL("Couldn't write DMU updated password to NV!",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Update the password in PPP, so re-authentication mid-call can succeed.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_AN_AUTH
  if( ppp_update_password( PPP_UM_AN_DEV, 
                           key_data->chap_key,
                           MIP_DMU_CHAP_KEY_LEN ) != 0 )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "Couldn't update AN PPP password with new DMU password");
  }
#endif
#endif /* FEATURE_DATA_MOBILE_IP_DMU_MODIFIES_AN_AUTH */

  /*--------------------------------------------------------------------------
    Write Simple IP CHAP shared secret to PPP password NV item
  --------------------------------------------------------------------------*/
  d = (byte *) ps_nv_item_ptr->ppp_password.password;
  memscpy (d, 
           NV_MAX_PPP_PASSWORD_LENGTH, 
           key_data->chap_key, 
           MIP_DMU_CHAP_KEY_LEN);
  ps_nv_item_ptr->ppp_password.password_len = MIP_DMU_CHAP_KEY_LEN;

  nv_status = ps_put_nv_item (NV_PPP_PASSWORD_I, ps_nv_item_ptr);
  if (nv_status != NV_DONE_S)
  {
    /* should really pre-commit keys and not do DMU if pre-committing fails */
    ERR_FATAL("Error in writing DMU updated shared secret data to NV!",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Update the password in PPP, so re-authentication mid-call can succeed.
  -------------------------------------------------------------------------*/
  if( ppp_update_password( PPP_UM_SN_DEV, 
                           key_data->chap_key,
                           MIP_DMU_CHAP_KEY_LEN ) != 0 )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "Error in writing DMU updated shared secret data to PPP");
  }
  
  /*-------------------------------------------------------------------------
    Enforce that passwords are encrypted after DMU update
    (to avoid cloning data service access parameters
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr->auth_require_password_encryption = TRUE;
  nv_status = ps_put_nv_item( NV_AUTH_REQUIRE_PASSWORD_ENCRYPTION_I,
                              ps_nv_item_ptr );
  if (NV_DONE_S != nv_status)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "Changing password encryption (required) NV item failed!");
  }

 /* ------------------------------------------------------------------------
   * Sync the cached nv items to backing store
   * ----------------------------------------------------------------------*/

  if ( -1 == efs_sync("/"))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "efs_sync() failed");
  }
    

  /*-------------------------------------------------------------------------
    Update the runtime setting for auth on Um device.

    Note that the authentication encryption setting currently is global,
    hence this call will affect all devices (i.e. AN and Rm as well).

    If the DMU code is modified to update other device(s) passwords besides 
    Um (e.g. AN), add another line to require encrypted auth on those 
    devices as well.
  -------------------------------------------------------------------------*/
  ppp_update_auth_setting( PPP_UM_SN_DEV, TRUE );

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
} /* dmu_commit_key_data() */

/*============================================================================
FUNCTION MIP_GET_DMU_INFO_PTR_BY_INDEX

DESCRIPTION
  Access function for mip_dmu_info array.

PARAMETERS
  Index: Element index of the array.

RETURN VALUE
  Pointer to MIP DMU Info element.

DEPENDENCIES
  None

============================================================================*/
dmu_info_type* mip_get_dmu_info_ptr_by_index
(
  int index
)
{
  return &mip_dmu_info[index];
}

/*============================================================================
FUNCTION MIP_IS_DMU_RAND_READY

DESCRIPTION
  Access function for mip_dmu_rand_ready flag.

PARAMETERS
  None

RETURN VALUE
  mip_dmu_rand_ready flag value.

DEPENDENCIES
  None

============================================================================*/
boolean mip_is_dmu_rand_ready(void)
{
  return mip_dmu_rand_ready;
}

#endif /* FEATURE_DS_MOBILE_IP_DMU */

/*============================================================================

FUNCTION DMU_RAND_SIG_HANDLER

DESCRIPTION
  Handle random data ready signal from 
  i)  Security services at startup to indicate that random data is avaiable
      for random key generation
  ii) PS task if qcmipp is changed, so random keys can be re-generated (for 
      safety, in case keys were captured from an incomplete DMU update with 
      the previous profile during a key request spoof), public key for new
      profile is read and news keys encrypted.

PARAMETERS
  Ignored

RETURN VALUE
  TRUE - tells PS that signal handling is complete

DEPENDENCIES
  None

============================================================================*/
void dmu_preencrypt_cmd_hdlr
(
  ps_cmd_enum_type  cmd,                    /* Command from issuer          */
  void *            data_ptr                /* user data provided (ignored) */
)
{
#if defined(FEATURE_DS_MOBILE_IP_DMU)
  dmu_info_type * dmu_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    Check mip_dmu_rand_ready, 
    - first time called: initialize, generate and pre-encrypt all sessions.
    - after: profile has changed, regenerate and pre-encrypt affected session
  --------------------------------------------------------------------------*/
  if (!mip_dmu_rand_ready)
  {
    if (dmui_pre_encrypt_all_sessions())
    {
      mip_dmu_rand_ready = TRUE;
    }
    return;
  }

  /*--------------------------------------------------------------------------
    For affected profile, generate new temporary keys and pre-encrypt them
    to prepare for DMU.

    For now, hard code this to session 0.  If multiple sessions need to 
    be supported, need to pass session index/pointer into the function.
  --------------------------------------------------------------------------*/
  DATA_MIP_MSG0(MSG_LEGACY_MED, "Pre-encrypt new DMU keys");
  dmu_info = &mip_dmu_info[0];

  /*------------------------------------------------------------------------
    Generate new random key data
  ------------------------------------------------------------------------*/
  dmu_gen_key_data (dmu_info);

  /*------------------------------------------------------------------------
    Reconfigure with the public key for the newly selected MIP profile
  ------------------------------------------------------------------------*/
  dmu_info->pk_info.pkoid = PKOID_UNLOADED;

  /*------------------------------------------------------------------------
    Encrypt key data with the newly loaded public key
  ------------------------------------------------------------------------*/
  if (!dmu_encrypt_key_data(dmu_info))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Couldn't encrypt key data!");
  }
  return;
#else
  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "dmu_preencrypt_cmd_hdlr() not supported");
  return;
#endif


} /* dmu_preencrypt_cmd_hdlr() */
