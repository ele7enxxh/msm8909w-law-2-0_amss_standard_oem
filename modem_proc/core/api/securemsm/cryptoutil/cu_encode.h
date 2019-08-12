#ifndef CU_ENCODE_H
#define CU_ENCODE_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/cryptoutil/cu_encode.h#1 $
  $DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/2010 dm	   Crypto API files rearchitecture
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "stddef.h"

/*!

  @brief 
  Base64 encode the source string and output to the destination string.

  @details
  Base64 encode the source string and output to the destination string.
  The base64 encoding is modified to use only FS-compatible characters.

  @dependencies
  None

  @param[in] *src_ptr            - Pointer to source string
  @param[in] src_len            - Length of source string
  @param[out] *des_ptr            - Pointer to destination string

  @return
  Length of destination string.

  @sideeffects
  None
*/
size_t cu_b64fs_encode( uint8 *src_ptr, size_t src_len, uint8 *des_ptr );


size_t cu_b64_encode( uint8 *src_ptr, size_t src_len, uint8 *des_ptr );

#endif
                           
