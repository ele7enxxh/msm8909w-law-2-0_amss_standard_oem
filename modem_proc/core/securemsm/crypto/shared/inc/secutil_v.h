#ifndef SECUTIL_V_H
#define SECUTIL_V_H

/** 
   @file secutil_v.h
   @brief Cryptographic Utility Library - string manipulation.
   This file contains the definitions of the constants, data structures
   and interfaces contained in the Cryptographic Utility Library. 
*/

/*===========================================================================
  Copyright (c) 2000 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/inc/secutil_v.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/10   dm      Crypto API files rearchitecture
09/27/10   dm	   Removed the #if 0 section and the feature 'FEATURE_DSM_MEM_CHK'
08/12/10   ejt     Clean up removed TARGET_OS_UNIX  
06/25/10   AF      (Tech Pubs) Edited/added Doxygen comments.
06/18/10   yh      FEATURE_DSM_MEM_CHK clean up
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
05/21/07   avm     corrected RPC meta comments
04/04/07   avm     defined secutil rnd buf for compatibility with w/1x
02/27/07   df      new function secutil_common_wait()
12/18/06   df      support for dual PD, all rnd funcs now in secrnd
12/13/06   avm     fixed calls to banned API's
12/07/06   avm     Fixed Lint error type 650, 661, 662.
04/21/06   df      added secutil_b64fs_encode(), secutil_get/put_nv_item()
03/27/06   df      more changes to support secrnd task
03/22/06   df      changes to support FEATURE_SECRND
12/12/05   df      include stddef to resolve size_t
10/17/05   df      changed to "on demand" seeding
10/03/05   df      added mutex primitives for csprng
09/16/05   df      Added support for csprng periodic seeding
08/18/05   rv      added secutil_strnicmp (String compare Ignore Case)
11/12/04   rv      changes to correct LINT errors
02/18/04   rwh     Moved some char to long macros from DES code to secutil.h.
                   Added seek_write().
03/24/03   rwh     Provide QXDM output based on a run-time message level.
                   Added macros to handle pointer values between ARM/QDSP.
03/21/03   rwh     Removed a compile warning on an unsigned compare.
03/13/03   rwh     Commented out directives used in custssl.h file.
12/11/02   rwh     Added extern for seccrypt_initialized (CR 26278).
07/25/02   rwh     Modified the random data dump function to use a circular 
                   buffer, moved #def to custssl.h.
06/05/02   om      Added fuzzy string match prototypes
04/18/02   rwh     Added include of secerrno.h.
12/04/01   rwh     Changed log_opaque to log_opaque_level.
12/03/01   rwh     Added prototype for secutil_accept_random_data.
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "secerrno.h"
#include "nv.h"
#include "rex.h"
#include "ixipc.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
/*lint -e760 */  /*Redundant macro Symbol defined identically at Location */

/** @addtogroup crypto_secutilapi 
  @{ */


/**
   Copies four consecutive characters to a long. 
*/
#define c2l(c,l)	(/*lint -e616 */l  = ((uint32)(*((c)++)))      , \
			             l |= ((uint32)(*((c)++))) <<  8, \
			             l |= ((uint32)(*((c)++))) << 16, \
			             l |= ((uint32)(*((c)++))) << 24  /*lint -e616 */)

/** 
  Copies one to seven consecutive characters to two longs.
  @note c is not incremented as per c2l. 
*/
#define c2ln(c,l1,l2,n)	{/*lint -e616*/ \
                        c += n;      \
			l1 = l2 = 0; \
			switch (n) { \
			case 7: l2 |= ((uint32)(*(--(c)))) << 16; \
			case 6: l2 |= ((uint32)(*(--(c)))) <<  8; \
			case 5: l2 |= ((uint32)(*(--(c))))      ; \
			case 4: l1 |= ((uint32)(*(--(c)))) << 24; \
			case 3: l1 |= ((uint32)(*(--(c)))) << 16; \
			case 2: l1 |= ((uint32)(*(--(c)))) <<  8; \
			case 1: l1 |= ((uint32)(*(--(c))))      ; \
			default: break;} \
		/*lint +e616 */}

/** 
  Copies a long value to four consecutive characters. 
*/
#define l2c(l,c)	(/*lint -e616 */*((c)++) = (uint8)(((l)      ) & 0xff), \
			             *((c)++) = (uint8)(((l) >>  8) & 0xff), \
			             *((c)++) = (uint8)(((l) >> 16) & 0xff), \
			             *((c)++) = (uint8)(((l) >> 24) & 0xff)/*lint +e616 */)

/**
  Copies two long values to one to seven consecutive characters. 
  @note c is not incremented as per l2c.
*/
#define l2cn(l1,l2,c,n)	{/*lint -e616 */ \
                        c += n; \
			switch (n) { \
			case 7: *(--(c)) = (uint8)(((l2) >> 16) & 0xff); \
			case 6: *(--(c)) = (uint8)(((l2) >>  8) & 0xff); \
			case 5: *(--(c)) = (uint8)(((l2)      ) & 0xff); \
			case 4: *(--(c)) = (uint8)(((l1) >> 24) & 0xff); \
			case 3: *(--(c)) = (uint8)(((l1) >> 16) & 0xff); \
			case 2: *(--(c)) = (uint8)(((l1) >>  8) & 0xff); \
			case 1: *(--(c)) = (uint8)(((l1)      ) & 0xff); \
            default: break;  } \
		/*lint -e616 */}

/** @} */  /** end_addtogroup crypto_secutilapi */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/**
  @addtogroup crypto_secutilapi 
  @{
*/

/**
  Reads the requested item from NV.
   
  @param[in] item      ID of the item to get.
  @param[in] data_ptr  Pointer to the buffer to receive the item value.
  @param[in] data_len  Length of the buffer to receive the item value.

  @return
  Status returned from the NV read request. An ERR is logged if 
  status is other than: \n 
  NV_DONE_S       - Request done. \n 
  NV_NOTACTIVE_S  - Item was not active.
  
  @dependencies
  This task is started; NV task is started.

  @sideeffects
  While this function is running, all other task activities are suspended 
  except for watchdog kicking until the NV item is read.
*/
secerrno_enum_type secutil_get_nv_item
( 
  nv_items_enum_type             item,           
  uint8*                         data_ptr,
  uint16                         data_len
);

/**
  Writes an item to NV memory.  

  Wait until write is complete.

  @param[in] item     ID of the item to put.
  @param[in] data_ptr Pointer to the data source. 

  @return
  Status returned from the NV read request. An ERR is logged if the 
  status is other than NV_DONE_S - Request done.
  
  @dependencies
  This task is started; NV task is started.

  @sideeffects
  While this function is running, all other task activities are
  suspended, except for watchdog kicking, until the NV item is written.
*/
secerrno_enum_type secutil_put_nv_item
( 
  nv_items_enum_type             item,           
  uint8*                         data_ptr
);

/** 

  @param[in] rnd_num_ptr - Buffer to be filled in with the random data.
  @param[in] rnd_num_len - Amount of Random Data

  @dependencies    

  @return
  E_SUCCESS     if success
  E_BAD_ADDRESS if NULL pointer was passed

  @sideeffects  
  None
*/
secerrno_enum_type secutil_get_pkcs_random
(
  uint8*  random_ptr,
  uint16  random_len
);

/*!
  @}
*/
/* end_addtogroup_crypto_secutilapi */

#endif /* SECUTIL_V_H */
/*lint +e760 */
