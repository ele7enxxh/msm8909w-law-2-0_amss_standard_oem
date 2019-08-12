/*===========================================================================
                Unified Image Encryption (UIE) L1 Fuse Key 

GENERAL DESCRIPTION
  Chipset-specific key source retrieval

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include <string.h>
#include "HALhwio.h"
#include "msmhwiobase.h"
#include "uiehwioreg.h"
#include "secboot_hw.h"
#include "uie.h"
#include "uie_priv.h"
#include "uie_env.h"

#define BREAKIF_SETSTATUS(cond, status_code) \
  if (cond) { ret = status_code; break; }

#define UIE_KEY_IMAGE_ENCR_ENABLED (0x1)
#define UIE_LEN_4BYTES (0x4)

bool uie_is_image_encryption_fuse_enabled(void)
{
  return false;
}
int uie_get_l1_fuse_key(uie_key_src_t key_source, uint8 *key, uint32 keylen)
{
	return 0;
}
uint16 uie_get_fec()
{
  // Get the FEC out
  return 1;
}

void uie_clear_fec()
{
  //Do nothing
}
