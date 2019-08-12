#include "comdef.h"

#define TZ_MPSS_SEC_CHANNEL_FUSE_OFFSET 0x100

typedef struct mba_modem_range_tz_s
{
  /* Physical address of the starting point of modem */
  uint32 mss_start;
  /* Physical address of the end of the modem range */
  uint32 mss_end;
  /* some magic number to verify authenticity.
   */
  uint32 magic_number;
}mba_modem_range_tz_t;


void mba_pass_tz_modem_range(uint32 start,uint32 len);

/* Read the L1 key from TZ controlled memory */
void mba_get_l1_key(uint8 *key, uint32 keylen);
