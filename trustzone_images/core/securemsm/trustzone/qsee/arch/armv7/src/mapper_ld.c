/*============================================================================

                  Map Long-Descripter Translation Table Entries

GENERAL DESCRIPTION
  Functions for configuring the translation tables used by the MMU.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/


/*============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/src/mapper_ld.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/11/11   jct     Include test MMU PT definitions conditionally
04/20/11   jct     Conditional #include depending on inclusion/exclusion of
                   MobiCore
01/26/11   pre     Intial revision.
============================================================================*/


/*============================================================================

                            INCLUDE FILES FOR MODULE

============================================================================*/
#include <string.h>

#include "tt_ld.h"
#include "default_tt_map.h"
#include "mair.h"
#include "tzbsp_mem.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_errno.h"


/*============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

============================================================================*/

#define TZBSP_RETCHK(xx) if ((xx) != E_SUCCESS) {return (xx);}

#define KB2MB(xx)    ((xx) >> 10)

#define KB2B(xx)     ((xx) << 10)
#define MB2KB(xx)    ((xx) << 10)

#define B2L2TTE(xx)  (((xx) / L2_MAP_SIZE) & 0x1FF)
#define KB2L2TTE(xx) (((xx) / B2KB(L2_MAP_SIZE)) & 0x1FF)

#define B2L3TTE(xx)  (((xx) / L3_MAP_SIZE) & 0x1FF)
#define KB2L3TTE(xx) (((xx) / B2KB(L3_MAP_SIZE)) & 0x1FF)

#define B2NUML1TTES(xx)  ((xx) / L1_MAP_SIZE)
#define KB2NUML1TTES(xx) ((xx) / B2KB(L1_MAP_SIZE))
#define B2NUML2TTES(xx)  ((xx) / L2_MAP_SIZE)
#define KB2NUML2TTES(xx) ((xx) / B2KB(L2_MAP_SIZE))
#define B2NUML3TTES(xx)  ((xx) / L3_MAP_SIZE)
#define KB2NUML3TTES(xx) ((xx) / B2KB(L3_MAP_SIZE))

#define L1TTE2B(xx)  ((xx) * L1_MAP_SIZE)
#define L1TTE2KB(xx) ((xx) * B2KB(L1_MAP_SIZE))
#define L2TTE2B(xx)  ((xx) * L2_MAP_SIZE)
#define L3TTE2B(xx)  ((xx) * L3_MAP_SIZE)

/* @TODO: clean these up */
#define L1BLKADDRMSK(xx) ((xx) & 0xFFC0000000)
#define L1TBLADDRMSK(xx) ((xx) & 0xFFFFFFF000)
#define L2BLKADDRMSK(xx) ((xx) & 0xFFFFE00000)
#define L2TBLADDRMSK(xx) (L1TBLADDRMSK(xx))
#define L3BLKADDRMSK(xx) (L1TBLADDRMSK(xx))
#define L3TBLADDRMSK(xx) (L1TBLADDRMSK(xx))

#define ISL1TBL(xx) (((xx) & 0x3) == L1_TBL_DESC)
#define ISL2TBL(xx) ISL1TBL(xx)

typedef struct
{
  uint32 ii;
  uint32 lvl;
} tt_iter_t;

const uint32 mair0 = MAIR0;
const uint32 mair1 = MAIR1;

extern uint32 Image$$TZBSP_CODE$$Base;
extern uint32 Image$$TZBSP_CODE$$Length;
extern uint32 Image$$TZBSP_RO_DATA$$Length;
extern uint32 Image$$TZBSP_RO_DATA$$Base;
extern uint32 Image$$TZBSP_DP_HASH$$Length;
extern uint32 Image$$L1$$Base;
extern uint32 Image$$L1$$Length;
extern uint32 Image$$L2_POOL$$Length;
extern uint32 Image$$L3_POOL$$Length;

/* TODO: figure out alignment and tie it to TTBR0 somehow */
l1_tt_t __attribute__((section("L1_TT_MEM"),aligned(1024))) l1;
l2_tt_t __attribute__((section("L2_TT_MEM"),aligned(4096))) l2[NUM_L2_TTS];
l3_tt_t __attribute__((section("L3_TT_MEM"),aligned(4096))) l3[NUM_L3_TTS];

static int g_num_alloc_tts[3] = {0, 0, 0};

/*============================================================================

                              FUNCTION DEFINITIONS

============================================================================*/

/* TODO: this is the physical address of the L2 TT and may not
   necessarily correspond to the VA of the L2 TT.  This is broken
   for long descriptors since the PA is 64 bits and the VA is 32
   bits. */
static l2_tt_t* l1_walk(ld_tte_t l1_tte)
{
  return (l2_tt_t *) ((uint32) L1TBLADDRMSK(l1_tte));
}

/* TODO: this is the physical address of the L3 TT and may not
   necessarily correspond to the VA of the L3 TT.  This is broken
   for long descriptors since the PA is 64 bits and the VA is 32
   bits. */
static l3_tt_t* l2_walk(ld_tte_t l2_tte)
{
  return (l3_tt_t *) ((uint32) L2TBLADDRMSK(l2_tte));
}

static uint32 get_num_ttes(uint32 level, uint32 v_base, uint32 size_kb)
{
  if (0 == size_kb || level > 3)
  {
    return 0;
  }

  /* The first term has -1 in order to make the result [start, end)
     instead of [end, next_end - 1).  The +1 is change results from
     zero-based to 1-based. */
  switch (level)
  {
  case 1:
    return KB2NUML1TTES(B2KB(v_base) + size_kb - 1) - B2NUML1TTES(v_base) + 1;
  case 2:
    return KB2NUML2TTES(B2KB(v_base) + size_kb - 1) - B2NUML2TTES(v_base) + 1;
  case 3:
    return KB2NUML3TTES(B2KB(v_base) + size_kb - 1) - B2NUML3TTES(v_base) + 1;
  }

  return 0;
}

static uint64 tz_to_cmn_blk_cfg(uint32 tzcfg)
{
  uint64 xn   = (tzcfg & TZ_XN_MSK);
  uint64 pxn  = (tzcfg & TZ_PXN_MSK);
  uint64 ng   = BLK_G;
  uint64 af   = BLK_AF;
  uint64 sh   = (tzcfg & TZ_SH_MSK);
  uint64 ap   = (tzcfg & TZ_AP_MSK);
  uint64 ns   = (tzcfg & TZ_NS_MSK);
  uint64 memt = (tzcfg & TZ_MEMT_MSK);

  if      (TZ_NON_EXECUTABLE == xn) xn = BLK_XN;
  else                              xn = BLK_X;

  if      (TZ_PL1_NON_EXECUTABLE == pxn) pxn = BLK_PXN;
  else                                   pxn = BLK_PX;

  if      (TZ_OUT_SHAREABLE == sh) sh = BLK_OUT_SHAREABLE;
  else if (TZ_INR_SHAREABLE == sh) sh = BLK_INR_SHAREABLE;
  else                             sh = BLK_NOT_SHAREABLE;

  if      (TZ_READ_WRITE_PL1 == ap) ap = BLK_AP_PL1_RW;
  else if (TZ_READ_WRITE_PL0 == ap) ap = BLK_AP_RW;
  else if (TZ_READ_ONLY_PL1  == ap) ap = BLK_AP_PL1_RO;
  else if (TZ_READ_ONLY_PL0  == ap) ap = BLK_AP_RO;
  else                              ap = 0;

  if      (TZ_NON_SECURE == ns) ns = BLK_NS;
  else                          ns = BLK_S;

  if      (TZ_MEM_STRONGLY_ORDERED  == memt) memt = BLK_SO;
  else if (TZ_MEM_DEVICE            == memt) memt = BLK_DVC;
  else if (TZ_MEM_NON_CACHEABLE     == memt) memt = BLK_NC;
  else if (TZ_MEM_WRITE_THROUGH_WA  == memt) memt = BLK_WT_WA;
  else if (TZ_MEM_WRITE_THROUGH_NWA == memt) memt = BLK_WT_NWA;
  else if (TZ_MEM_WRITE_BACK_WA     == memt) memt = BLK_WB_WA;
  else if (TZ_MEM_WRITE_BACK_NWA    == memt) memt = BLK_WB_NWA;
  else if (TZ_MEM_NOT_USED          == memt) memt = BLK_NOT_USED;
  else                                       memt = BLK_SO;

  return xn | pxn | ng | af | sh | ap | ns | memt;
}

static ld_tte_t tz_to_l1_blk_cfg(uint32 tzcfg)
{
  uint64 invalid = ((tzcfg >> 11) & 0x1) ? 0 : L1_BLK_DESC;

  return (ld_tte_t) (tz_to_cmn_blk_cfg(tzcfg) | invalid);
}

static ld_tte_t tz_to_l2_blk_cfg(uint32 tzcfg)
{
  return tz_to_l1_blk_cfg(tzcfg);
}

static ld_tte_t tz_to_l3_blk_cfg(uint32 tzcfg)
{
  uint64 invalid = ((tzcfg >> 11) & 0x1) ? 0 : L3_BLK_DESC;

  return (ld_tte_t) (tz_to_cmn_blk_cfg(tzcfg) | invalid);
}

static ld_tte_t l1_blk_cfg_to_l1_tbl_cfg(uint64 l1tte)
{
  /* Not yet supporting table configuration bits */
  return 0;
}

static ld_tte_t l2_blk_cfg_to_l2_tbl_cfg(ld_tte_t l1tte)
{
  /* Not yet supporting table configuration bits */
  return 0;
}

static ld_tte_t l1_blk_cfg_to_l2_blk_cfg(ld_tte_t l1tte)
{
  return l1tte & 0xFFF0000000000FFF;
}

static ld_tte_t l2_blk_cfg_to_l3_blk_cfg(ld_tte_t l2tte)
{
  uint64 l2cfg = (uint64) l1_blk_cfg_to_l2_blk_cfg(l2tte);

  if (L2_BLK_DESC == (l2cfg & 3))
  {
    return (ld_tte_t) ((l2cfg & ~3) | L3_BLK_DESC);
  }
  else
  {
    return (ld_tte_t) (l2cfg & ~3);
  }
}

static void* tt_iter_init(int lvl, tt_iter_t* iter_ptr)
{
  iter_ptr->ii = 0;
  iter_ptr->lvl = lvl;

  switch (lvl)
  {
  case 1:
    return (void*) &l1;
  case 2:
    return (void*) &l2[0];
  case 3:
    return (void*) &l3[0];
  default:
    return NULL;
  }
}

/**
   @todo This may need to be extended to support non-contiguous l2tt
         memory given by secure applications.
 */
static void* tt_iter_next(tt_iter_t* iter_ptr)
{
  iter_ptr->ii++;

  switch (iter_ptr->lvl)
  {
  case 1:
    return NULL;
  case 2:
    if (iter_ptr->ii < TZBSP_ARRAY_SIZE(l2))
    {
      return &l2[iter_ptr->ii];
    }
    else
    {
      return NULL;
    }
  case 3:
    if (iter_ptr->ii < TZBSP_ARRAY_SIZE(l3))
    {
      return &l3[iter_ptr->ii];
    }
    else
    {
      return NULL;
    }
  default:
    return NULL;
  }
}

static boolean is_tt_free(void* tt_ptr, int lvl)
{
  switch (lvl)
  {
  case 2:
    return L2_UNUSED_DESC != ((ld_tte_t) (((l2_tt_t*) tt_ptr)->tt[0] & 0x3));
  case 3:
    return L3_UNUSED_DESC != ((ld_tte_t) (((l3_tt_t*) tt_ptr)->tt[0] & 0x3));
  default:
    return FALSE;
  }
}

static int num_free_tts(int lvl)
{
  switch (lvl)
  {
  case 2:
    return NUM_L2_TTS - g_num_alloc_tts[1];
  case 3:
    return NUM_L3_TTS - g_num_alloc_tts[2];
  default:
    return 0;
  }
}


static void free_tt(void* tt_ptr, int lvl)
{
  switch (lvl)
  {
  case 2:
  {
    l2_tt_t* l2_tt_ptr = (l2_tt_t*) tt_ptr;
    if ((ld_tte_t) L2_UNUSED_DESC != l2_tt_ptr->tt[0])
    {
      uint32 ii;

      /* A L2 TT may have table TTEs.  Those also should be freed. */
      for (ii = 0; ii < NUM_L2_TTES_PER_L1_TTE; ii++)
      {
        if (ISL2TBL(l2_tt_ptr->tt[ii]))
        {
          free_tt((void*) l2_walk(l2_tt_ptr->tt[ii]), 3);
        }
      }

      memset(l2_tt_ptr, 0, sizeof(*l2_tt_ptr));
      l2_tt_ptr->tt[0] = (ld_tte_t) L2_UNUSED_DESC;
      g_num_alloc_tts[1]--;
      tzbsp_dcache_clean_region(l2_tt_ptr, sizeof(*l2_tt_ptr));
    }
    break;
  }
  case 3:
  {
    l3_tt_t* l3_tt_ptr = (l3_tt_t*) tt_ptr;
    if ((ld_tte_t) L3_UNUSED_DESC != l3_tt_ptr->tt[0])
    {
      memset(l3_tt_ptr, 0, sizeof(*l3_tt_ptr));
      l3_tt_ptr->tt[0] = (ld_tte_t) L3_UNUSED_DESC;
      g_num_alloc_tts[2]--;
      tzbsp_dcache_clean_region(l3_tt_ptr, sizeof(*l3_tt_ptr));
    }
    break;
  }
  default:
    break;
  }
}

static void set_tt_allocated(void* tt_ptr, int lvl)
{
  switch(lvl)
  {
  case 2:
    ((l2_tt_t*) tt_ptr)->tt[0] = L2_INV_DESC;
    break;
  case 3:
    ((l3_tt_t*) tt_ptr)->tt[0] = L3_INV_DESC;
    break;
  default:
    break;
  }
}

/**
   @todo This may need to be extended to allocate pages from either
         IMEM or DDR depending on the security concerns.  Currently
         we're not allowing DDR based TTs.
 */
static int alloc_tt(void** tt_ptr_ptr, int lvl)
{
  void* tt_ptr;
  tt_iter_t iter;

  if (2 != lvl && 3 != lvl)
  {
    return -E_FAILURE;
  }

  if (0 == num_free_tts(lvl))
  {
    return -E_NO_MEMORY;
  }

  for (tt_ptr = (l2_tt_t*) tt_iter_init(lvl, &iter);
       tt_ptr != NULL;
       tt_ptr = tt_iter_next(&iter))
  {
    if (!is_tt_free(tt_ptr, lvl))
    {
      /* Found an open slot */
      *tt_ptr_ptr = tt_ptr;
      g_num_alloc_tts[lvl - 1]++;
      set_tt_allocated(tt_ptr, lvl);

      return E_SUCCESS;
    }
  }

  return -E_FAILURE;
}

static void tzbsp_check_l2_tte_consolidation(l2_tt_t* l2_tt_ptr, uint32 v_base)
{
  uint32 ii;

  for (ii = 1; ii < NUM_L2_TTES_PER_L1_TTE; ii++)
  {
    /* Mappings must have the exact same memory attributes and must
       map to continuous physical addresses */
    /* TODO: */
    /* if ((l2_tt_ptr[ii] & 0x00000FFF) !=  (l2_tt_ptr[0]      & 0x00000FFF) || */
    /*     (l2_tt_ptr[ii] & 0xFFFFF000) != ((l2_tt_ptr[ii - 1] & 0xFFFFF000) + L2TTE2B(1))) */
    /* { */
    return;
    /* } */
  }

  /* l1.tt[B2L1TTE(v_base)] = */
  /*   (l2_tt_ptr[0] & 0xFFF00000) | tzbsp_get_1mb_l1_cfg_from_4kb_l2_cfg(l2_tt_ptr[0]); */

  /* tzbsp_dcache_clean_region(&l1.tt[B2L1TTE(v_base)], sizeof(l1.tt[B2L1TTE(v_base)])); */

  /* tzbsp_free_l3_tt(l2_tt); */
}

static void tzbsp_check_l3_tte_consolidation(l3_tt_t* l3_tt_ptr, uint32 v_base)
{
  return;
}

static void inval_tt_lvl(uint32 lvl)
{
  l2_tt_t* tt_ptr;
  tt_iter_t iter;

  /* Level one translation tables are statically allocated. */
  if (1 == lvl)
  {
    memset(&l1, 0, sizeof(l1));
  }
  else if (2 == lvl || 3 == lvl)
  {
    for (tt_ptr = (l2_tt_t*) tt_iter_init(lvl, &iter);
         tt_ptr != NULL;
         tt_ptr = (l2_tt_t*) tt_iter_next(&iter))
    {
      free_tt(tt_ptr, lvl);
    }

    g_num_alloc_tts[lvl - 1] = 0;
  }
}

/**
   Zeros out all entries in the mmu translation table.  Having an
   entry of type 0 raises an exception.

   @retval None

   @par Side Effects
   All entries in the tt are invalidated.  If the mmu is enabled this
   will raise a section translation exception each time memory access is tried

   @dep l1_tt must be allocated
*/
static void map_all_ttes_as_invalid
(
  void
)
{
  inval_tt_lvl(1);
  inval_tt_lvl(2);
  inval_tt_lvl(3);
}

/**
   Maps a block descriptor into the translation table as l1 ttes

   @retval None

   @par Side Effects Existing entries in the tt will be replaced.

   @dep l1_tt must be allocated
*/
static void map_using_only_l1_ttes
(
  const mem_block_type * block
)
{
  const ld_tte_t cfg = tz_to_l1_blk_cfg(block->tz_cfg);
  const uint32 num_l1_ttes = get_num_ttes(1, block->v_base,
                                          block->size_in_kbytes);
  uint32 tt_ii, tt_limit, tt_base;
  uint64 p_addr;

  /* Convert our base address into an index into the translation table */
  tt_base = B2L1TTE(block->v_base);

  /* Upper limit of mapping range */
  tt_limit = tt_base + num_l1_ttes;

  /* Mask out any unimportant address bits */
  p_addr = L1BLKADDRMSK(block->p_base);

  for (tt_ii = tt_base; tt_ii < tt_limit; tt_ii++, p_addr += L1_MAP_SIZE)
  {
    /* If this L1 TTE points to a L2 TT, free the L2 TT */
    if (ISL1TBL(l1.tt[tt_ii]))
    {
      free_tt((void*) l1_walk(l1.tt[tt_ii]), 2);
    }

    /*  First-Level (L1) Block Format:
       ______________________________________________________________________
      |                        |                     |                       |
      |63--------------------52|51-----------------40|39-------------------32|
      | Upper block attributes |       UNK/SBZP      |    Output address     |
      |________________________|_____________________|_______________________|
      |                          |           |                           | | |
      |31----------------------30|29-------12|11------------------------2|1|0|
      | Output address (cont...) |  UNK/SBZP | Lower block attributes    |0|1|
      |__________________________|___________|___________________________|_|_|

      Upper Attributes:
       ______________________________________________________
      |          |                   |  |    |               |
      |63------59|58---------------55|54|-53-|------52-------|
      |  Ignored |  Reserved for SW  |XN|PXN |Contiguous hint|
      |__________|___________________|__|____|_______________|

      Lower Attributes:
       ______________________________________
      |  |  |       |       |  |             |
      |11|10|9-----8|7-----6|-5|4-----------2|
      |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|
      |__|__|_______|_______|__|_____________|
    */

    l1.tt[tt_ii] = p_addr | cfg;
  }

  /* Write changes out to DDR */
  tzbsp_dcache_clean_region(&l1.tt[tt_base],
                            (tt_limit - tt_base) * sizeof(l1.tt[tt_base]));

} /* map_using_only_l1_ttes */


/**
   For a given virtual address, this function will retrieve the level
   two translation table address, if the level one translation table
   was programmed to point to one, or it will return a newly allocated
   level two translation table and return the L1 translation table
   configuration, translated to L2 format and the old physical address
   mapping of the L1 translation table.

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully

   @pre l1.tt must be allocated
   @pre l2.tt must be allocated
*/
static int alloc_or_get_l2_tt
(
  uint64    v_addr,
  l2_tt_t** l2_tt_ptr_ptr,
  ld_tte_t* l2_cfg_from_l1,
  uint64*   p_addr_from_l1,
  boolean*  new_l2_tt
)
{
  /* Convert our virtual base address into an index into the L1
     translation table. */
  uint32 tt_ii = B2L1TTE(v_addr);

  /* Does this L1 TTE already point to an allocated L2 TTE? */
  if (ISL1TBL(l1.tt[tt_ii]))
  {
    /* Grab the level two translation table address directly */
    *l2_tt_ptr_ptr = l1_walk(l1.tt[tt_ii]);
    *l2_cfg_from_l1 = 0;
    *p_addr_from_l1 = 0;
    *new_l2_tt = FALSE;
  }
  else
  {
    uint64 cfg;

    /* Allocate a new translation table */
    if (alloc_tt((void**) l2_tt_ptr_ptr, 2) != E_SUCCESS)
    {
      return -E_NO_MEMORY;
    }

    *new_l2_tt = TRUE;
    *l2_cfg_from_l1 = l1_blk_cfg_to_l2_blk_cfg(l1.tt[tt_ii]);
    *p_addr_from_l1 = L1BLKADDRMSK(l1.tt[tt_ii]);
    cfg = l1_blk_cfg_to_l1_tbl_cfg(l1.tt[tt_ii]);

    /* First-Level (L1) Table Format:
       ______________________________________________________________________
       |                        |                     |                       |
       |-63--|62-61|-60--|-59---|58---52|51----40|39------------------------32|
       |NSTbl|APTbl|XNTbl|PxNTbl|Ignored|UNK/SBZP|  Next-level table address  |
       |________________________|_____________________|_______________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |  Next-level table address (cont...)  |       Ignored             |1|1|
       |______________________________________|___________________________|_|_|
    */

    /* Enter the L2 base address and mark the descriptor for L2
     * translation table and default L1 TTE for L2 TTE info.
     */
    l1.tt[tt_ii] = (((ld_tte_t) L1TBLADDRMSK((uint64) *l2_tt_ptr_ptr)) | cfg |
                    L1_TBL_DESC);

    tzbsp_dcache_clean_region(&l1.tt[tt_ii], sizeof(l1.tt[tt_ii]));
  }

  return E_SUCCESS;
}

static int alloc_or_get_l3_tt
(
  l2_tt_t*  l2_tt_ptr,
  uint32    l2_tt_ii,
  l3_tt_t** l3_tt_ptr_ptr,
  ld_tte_t* l3_cfg_from_l2,
  uint64*   p_addr_from_l2,
  boolean*  new_l3_tt
)
{
  /* Does this L2 TTE already point to an allocated L3 TTE? */
  if (ISL2TBL(l2_tt_ptr->tt[l2_tt_ii]))
  {
    /* Grab the L3 TTE address directly */
    *l3_tt_ptr_ptr = l2_walk(l2_tt_ptr->tt[l2_tt_ii]);
    *l3_cfg_from_l2 = 0;
    *p_addr_from_l2 = 0;
    *new_l3_tt = FALSE;
  }
  else
  {
    uint64 cfg;

    /* Allocate a new translation table */
    if (alloc_tt((void**) l3_tt_ptr_ptr, 3) != E_SUCCESS)
    {
      return -E_NO_MEMORY;
    }

    *new_l3_tt = TRUE;
    *l3_cfg_from_l2 = l2_blk_cfg_to_l3_blk_cfg(l2_tt_ptr->tt[l2_tt_ii]);
    *p_addr_from_l2 = L2BLKADDRMSK(l2_tt_ptr->tt[l2_tt_ii]);
    cfg = l2_blk_cfg_to_l2_tbl_cfg(l2_tt_ptr->tt[l2_tt_ii]);

    /* Second-Level (L2) Table Format:
       _______________________________________________________________________
       |     |     |     |      |       |        |                            |
       |-63--|62-61|-60--|-59---|58---52|51----40|39------------------------32|
       |NSTbl|APTbl|XNTbl|PxNTbl|Ignored|UNK/SBZP|  Next-level table address  |
       |_____|_____|_____|______|_______|________|____________________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |  Next-level table address (cont...)  |       Ignored             |1|1|
       |______________________________________|___________________________|_|_|
    */

    /* Enter the L2 base address and mark the descriptor for L2
     * translation table and default L1 TTE for L2 TTE info.
     */
    l2_tt_ptr->tt[l2_tt_ii] = (((ld_tte_t) L2TBLADDRMSK(
                                  (uint64) *l3_tt_ptr_ptr)) |
                               cfg | L2_TBL_DESC);

    tzbsp_dcache_clean_region(&l2_tt_ptr->tt[l2_tt_ii],
                              sizeof(l2_tt_ptr->tt[l2_tt_ii]));
  }

  return E_SUCCESS;
}

/**
   Checks the L1 entries at the given address to see how many already
   point to allocated level two translation tables and how many need
   new allocations

   @retval number of level two translation tables needed for the given
           virtual address and number of L2 TTEs
*/
static int tzbsp_get_num_l2_tts_needed
(
  const uint32 v_base,
  const uint32 num_l1_ttes
)
{
  uint32 l1_tte_ii, tt_ii;
  int    num_l2_tts_needed = 0;

  /* Check that there is space for all the newly needed l2 translation
     tables */
  for (l1_tte_ii = 0; l1_tte_ii < num_l1_ttes; l1_tte_ii++)
  {
    /* Convert our virtual base address into an index into the L1
       translation table. */
    tt_ii = B2L1TTE(v_base) + l1_tte_ii;

    /* Does this L1 TTE already point to an allocated L2 TTE? */
    if (!ISL1TBL(l1.tt[tt_ii]))
    {
      num_l2_tts_needed++;
    }
  }

  return num_l2_tts_needed;
}

static int tzbsp_get_num_l3_tts_needed
(
  const uint32 v_base,
  const uint32 num_l2_ttes
)
{
  uint32 l2_tte_ii, l1_tte_ii;
  int    num_l3_tts_needed = 0;
  l2_tt_t* l2_tt_ptr;

  l1_tte_ii = B2L1TTE(v_base);

  l2_tt_ptr = l1_walk(l1.tt[l1_tte_ii]);

  /* Check that there is space for all the newly needed l3 translation
     tables */
  for (l2_tte_ii = 0; l2_tte_ii < num_l2_ttes; l2_tte_ii++)
  {
    /* Does this L2 TTE already point to an allocated L3 TTE? */
    if (!ISL2TBL(l2_tt_ptr->tt[l2_tte_ii]))
    {
      num_l3_tts_needed++;
    }
  }

  return num_l3_tts_needed;
}

/**
   Maps second level translation table entries.

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully


   @par Side Effects
   Existing entries in the tt will be replaced.

   @pre l2_tt must be allocated
*/
static int map_using_only_l2_ttes
(
  const mem_block_type * block
)
{
  const uint32 num_l1_ttes = get_num_ttes(1, block->v_base,
                                          block->size_in_kbytes);
  const uint32 num_l2_ttes = get_num_ttes(2, block->v_base,
                                          block->size_in_kbytes);
  const ld_tte_t cfg = tz_to_l2_blk_cfg(block->tz_cfg);
  const uint32 num_l2_tts_req = tzbsp_get_num_l2_tts_needed(block->v_base,
                                                            num_l1_ttes);

  l2_tt_t* l2_tt_ptr;
  uint32  tt_ii, tt_limit, tt_offset, l1_tt_ii;
  uint64  p_base_from_l1;
  int64   p_offset = block->p_base - block->v_base;
  ld_tte_t l2_cfg_from_l1;
  boolean new_l2_tt;

  if (num_l2_tts_req > num_free_tts(2))
  {
    return -E_NO_MEMORY;
  }

  /* We'll have to do each step for every l1 translation table entry
     that gets replaced with a pointer to a l2 translation table
   */
  for (l1_tt_ii = 0; l1_tt_ii < num_l1_ttes; l1_tt_ii++)
  {
    int retval = E_SUCCESS;

    retval =
      alloc_or_get_l2_tt((block->v_base + L1TTE2B(l1_tt_ii)),
                         &l2_tt_ptr, &l2_cfg_from_l1,
                         &p_base_from_l1, &new_l2_tt);

    TZBSP_RETCHK(retval);

    /* There will only be an offset for the first L1 TT and a limit
       for the last L1 TT. */
    tt_offset = (l1_tt_ii == 0) ? B2L2TTE(block->v_base) : 0;
    tt_limit = (l1_tt_ii == (num_l1_ttes - 1)) ?
      B2L2TTE(block->v_base) + num_l2_ttes -
      NUM_L2_TTES_PER_L1_TTE * l1_tt_ii : NUM_L2_TTES_PER_L1_TTE;

    /* Second-Level (L2) Block format:
       ______________________________________________________________________
      |                        |                     |                       |
      |63--------------------52|51-----------------40|39-------------------32|
      | Upper block attributes |       UNK/SBZP      |    Output address     |
      |________________________|_____________________|_______________________|
      |                          |           |                           | | |
      |31----------------------21|20-------12|11------------------------2|1|0|
      | Output address (cont...) |  UNK/SBZP | Lower block attributes    |0|1|
      |__________________________|___________|___________________________|_|_|

      Upper Attributes:
       ______________________________________________________
      |          |                   |  |    |               |
      |63------59|58---------------55|54|-53-|------52-------|
      |  Ignored |  Reserved for SW  |XN|PXN |Contiguous hint|
      |__________|___________________|__|____|_______________|

      Lower Attributes:
       ______________________________________
      |  |  |       |       |  |             |
      |11|10|9-----8|7-----6|-5|4-----------2|
      |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|
      |__|__|_______|_______|__|_____________|
    */

    /* Iterate through all the L2 descriptors in the L1 descriptor */
    for (tt_ii = 0; tt_ii < NUM_L2_TTES_PER_L1_TTE; tt_ii++)
    {
      /* If the index lies between the base index and the limit, then
         this is the given l2tte we need to map */
      if ((tt_offset <= tt_ii) && (tt_ii < tt_limit))
      {
        /* If this L2 TTE points to a L3 TT, free the L3 TT */
        if (ISL2TBL(l2_tt_ptr->tt[tt_ii]))
        {
          free_tt((void*) l2_walk(l2_tt_ptr->tt[tt_ii]), 3);
        }

        l2_tt_ptr->tt[tt_ii] = ((ld_tte_t) (block->v_base & L1_ADDR_MSK) +
                                L1TTE2B(l1_tt_ii) + L2TTE2B(tt_ii) +
                                p_offset) | cfg;
      }
      /* If this is a newly mapped L2 TT, write the non-specified L2
         TTEs with the mapping retrieved from the L1 TTE */
      else if (new_l2_tt)
      {
        l2_tt_ptr->tt[tt_ii] = ((p_base_from_l1 + L2TTE2B(tt_ii)) |
                                l2_cfg_from_l1);
      }
    }

    if (new_l2_tt)
    {
      /* Write entire new L2TT to DDR */
      tzbsp_dcache_clean_region(l2_tt_ptr, sizeof(*l2_tt_ptr));
    }
    else
    {
      /* Write only changes out to DDR */
      tzbsp_dcache_clean_region(&l2_tt_ptr->tt[tt_offset],
                                (tt_limit - tt_offset) *
                                sizeof(l2_tt_ptr->tt[tt_offset]));
    }

    /* Determine if all L2TTEs in the given L2TT can be combined into
       one L1TTE */
    tzbsp_check_l2_tte_consolidation(l2_tt_ptr,
                                     (block->v_base + L1TTE2B(l1_tt_ii)));
  }

  return E_SUCCESS;
} /* map_using_only_l2_ttes */

static int map_using_only_l3_ttes
(
  const mem_block_type * block
)
{
  const ld_tte_t cfg = tz_to_l3_blk_cfg(block->tz_cfg);

  const uint32 num_l1_ttes = get_num_ttes(1, block->v_base,
                                          block->size_in_kbytes);
  const uint32 num_l2_ttes = get_num_ttes(2, block->v_base,
                                          block->size_in_kbytes);
  const uint32 num_l3_ttes = get_num_ttes(3, block->v_base,
                                          block->size_in_kbytes);

  const uint32 num_l2_tts_req = tzbsp_get_num_l2_tts_needed(block->v_base,
                                                            num_l1_ttes);
  const uint32 num_l3_tts_req = tzbsp_get_num_l3_tts_needed(block->v_base,
                                                            num_l2_ttes);

  l3_tt_t* l3_tt_ptr;
  l2_tt_t* l2_tt_ptr;
  uint32 l1_tt_ii;
  uint32 l2_tt_ii, l2_tt_limit, l2_tt_offset;
  uint32 l3_tt_ii, l3_tt_limit, l3_tt_offset;
  uint64 p_addr_from_l1, p_addr_from_l2;
  int64  p_offset = block->p_base - block->v_base;
  ld_tte_t l2_cfg_from_l1, l3_cfg_from_l2;
  boolean new_l2_tt, new_l3_tt;

  if (num_l2_tts_req > num_free_tts(2) || num_l3_tts_req > num_free_tts(3))
  {
    return -E_NO_MEMORY;
  }

  /* We'll have to do each step for every l1 translation table entry
     that gets replaced with a pointer to a l2 translation table
   */
  for (l1_tt_ii = 0; l1_tt_ii < num_l1_ttes; l1_tt_ii++)
  {
    int retval = E_SUCCESS;

    retval =
      alloc_or_get_l2_tt((block->v_base + L1TTE2B(l1_tt_ii)),
                         &l2_tt_ptr, &l2_cfg_from_l1,
                         &p_addr_from_l1, &new_l2_tt);

    TZBSP_RETCHK(retval);

    if (0 == l1_tt_ii)
    {
      l2_tt_offset = B2L2TTE(block->v_base);
    }
    else
    {
      l2_tt_offset = 0;
    }

    l2_tt_limit = (l1_tt_ii == (num_l1_ttes - 1)) ?
      B2L2TTE(block->v_base) + num_l2_ttes -
      NUM_L2_TTES_PER_L1_TTE * l1_tt_ii : NUM_L2_TTES_PER_L1_TTE;

    for (l2_tt_ii = 0; l2_tt_ii < NUM_L2_TTES_PER_L1_TTE; l2_tt_ii++)
    {
      /* Filling in L2 info from L1 info must be done before filling
         in L3 info so L1 configurations can properly propogate down
         to the L3 TTs. */
      if (new_l2_tt)
      {
        l2_tt_ptr->tt[l2_tt_ii] = ((ld_tte_t) (p_addr_from_l1 +
                                               L2TTE2B(l2_tt_ii)) |
                                   l2_cfg_from_l1);
      }

      if ((l2_tt_offset <= l2_tt_ii) && (l2_tt_ii < l2_tt_limit))
      {
        retval =
          alloc_or_get_l3_tt(l2_tt_ptr, l2_tt_ii,
                             &l3_tt_ptr, &l3_cfg_from_l2, &p_addr_from_l2,
                             &new_l3_tt);

        TZBSP_RETCHK(retval);

        /* For the first iteration the TTE index offset (VA offset) is
           taken from the virtual base parameter.  The physical address
           must include all bits of information from the physical base
           parameter. */
        if (l2_tt_offset == l2_tt_ii)
        {
          l3_tt_offset = B2L3TTE(block->v_base);
        }
        /* For each remaining iteration the TTE index offset (VA offset)
           will be zero and the non L1 entry length aligned bits must be
           removed from the physical base parameter. */
        else
        {
          l3_tt_offset = 0;
        }

        /* The last iteration TTE index limit (VA limit) may not be the
           last index. */
        if (l2_tt_ii == (l2_tt_limit - 1))
        {
          l3_tt_limit = (B2L3TTE(block->v_base) + num_l3_ttes -
                         NUM_L3_TTES_PER_L2_TTE * (l2_tt_ii - l2_tt_offset));
        }
        /* Every other iteration must end at the last index. */
        else
        {
          l3_tt_limit = NUM_L3_TTES_PER_L2_TTE;
        }

        /* Third-Level (L3) Page (Block) Format:
        ______________________________________________________________________
       |                        |                     |                       |
       |63--------------------52|51-----------------40|39-------------------32|
       | Upper page attributes  |       UNK/SBZP      |    Output address     |
       |________________________|_____________________|_______________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |      Output address (cont...)        |  Lower page attributes    |1|1|
       |______________________________________|___________________________|_|_|

       Upper Attributes:
        ______________________________________________________
       |          |                   |  |    |               |
       |63------59|58---------------55|54|-53-|------52-------|
       |  Ignored |  Reserved for SW  |XN|PXN |Contiguous hint|
       |__________|___________________|__|____|_______________|

       Lower Attributes:
        ______________________________________
       |  |  |       |       |  |             |
       |11|10|9-----8|7-----6|-5|4-----------2|
       |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|
       |__|__|_______|_______|__|_____________| */


        /* Iterate through all the L3 descriptors in the L2 descriptor */
        for (l3_tt_ii = 0; l3_tt_ii < NUM_L2_TTES_PER_L1_TTE;
             l3_tt_ii++)
        {
          /* If the index lies between the base index and the limit, then
             this is the area that needs mapping */
          if ((l3_tt_offset <= l3_tt_ii) && (l3_tt_ii < l3_tt_limit))
          {
            l3_tt_ptr->tt[l3_tt_ii] =
              ((ld_tte_t) (block->v_base & L1_ADDR_MSK) + L1TTE2B(l1_tt_ii) +
               L2TTE2B(l2_tt_ii) + L3TTE2B(l3_tt_ii) + p_offset) | cfg;
          }
          /* If this is a newly mapped L2 TT, write the non-specified L2
             TTEs with the mapping retrieved from the L1 TTE */
          else if (new_l3_tt)
          {
            l3_tt_ptr->tt[l3_tt_ii] = (((ld_tte_t) p_addr_from_l2 +
                                        L3TTE2B(l3_tt_ii)) | l3_cfg_from_l2);
          }
        }

        if (new_l3_tt)
        {
          /* Write entire new L2TT to DDR */
          tzbsp_dcache_clean_region(l3_tt_ptr, sizeof(*l3_tt_ptr));
        }
        else
        {
          /* Write only changes out to DDR */
          tzbsp_dcache_clean_region(&l3_tt_ptr->tt[l3_tt_offset],
                                    (l3_tt_limit - l3_tt_offset) *
                                    sizeof(l3_tt_ptr->tt[l3_tt_offset]));
        }

        /* Determine if all L2TTEs in the given L2TT can be combined into
           one L1TTE */
        tzbsp_check_l3_tte_consolidation(l3_tt_ptr,
                                         (block->v_base + L1TTE2B(l2_tt_ii)));

      }
    }

    if (new_l2_tt)
    {
      /* Write entire new L2TT to DDR */
      tzbsp_dcache_clean_region(l2_tt_ptr, sizeof(*l2_tt_ptr));
    }
    else
    {
      /* Write only changes out to DDR */
      tzbsp_dcache_clean_region(&l2_tt_ptr->tt[l2_tt_offset],
                                (l2_tt_limit - l2_tt_offset) *
                                 sizeof(l2_tt_ptr->tt[l2_tt_offset]));
    }

    /* Determine if all L2TTEs in the given L2TT can be combined into
       one L1TTE */
    tzbsp_check_l2_tte_consolidation(l2_tt_ptr,
                                     (block->v_base + L1TTE2B(l1_tt_ii)));

  }

  return E_SUCCESS;
} /* map_using_only_l3_ttes */

/**
   Maps both first and second level translation table entries

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully

   @par Side Effects Existing entries in the tt will be replaced.

   @pre l1.tt must be allocated
   @pre l2.tt must be allocated
 */
static int map_using_l1_ttes
(
  const mem_block_type * block
)
{
  const uint64 pre_l1_sz_kb =
    ((L1_SIZE_MSK & block->v_base) ?
     B2KB((L1_MAP_SIZE - (L1_SIZE_MSK & block->v_base))) : 0);

  const uint64 l1_sz_kb = ((block->size_in_kbytes - pre_l1_sz_kb) &
                           L1_KB_ADDR_MSK);

  const uint64 post_l1_sz_kb = ((block->size_in_kbytes - pre_l1_sz_kb) -
                                l1_sz_kb);

  const uint64 l1_off = KB2B(pre_l1_sz_kb);

  const uint64 post_l1_off = KB2B(pre_l1_sz_kb + l1_sz_kb);

  const uint32 num_pre_l1_l2s =
    tzbsp_get_num_l2_tts_needed(block->v_base,
                                (0 < pre_l1_sz_kb) ? 1 : 0);

  const uint32 num_post_l1_l2s =
    tzbsp_get_num_l2_tts_needed((block->v_base + post_l1_off),
                                (0 < post_l1_sz_kb) ? 1 : 0);

  const uint32 num_l2_tts_req = num_pre_l1_l2s + num_post_l1_l2s;

  const uint32 num_pre_l1_l3s =
    tzbsp_get_num_l3_tts_needed(block->v_base,
                                (B2KB(L2_MAP_SIZE) >
                                 (pre_l1_sz_kb & L2_KB_SIZE_MSK)) ? 1 : 0);

  const uint32 num_post_l1_l3s =
    tzbsp_get_num_l2_tts_needed((block->v_base + post_l1_off),
                                (B2KB(L2_MAP_SIZE) >
                                 (post_l1_sz_kb & L2_KB_SIZE_MSK)) ? 1 : 0);

  const uint32 num_l3_tts_req = num_pre_l1_l3s + num_post_l1_l3s;

  int retval;

  if (num_l2_tts_req > num_free_tts(2) || num_l3_tts_req > num_free_tts(3))
  {
    return -E_NO_MEMORY;
  }

  /* Don't call map and handle failures when the map size is zero */
  if (pre_l1_sz_kb > 0)
  {
    retval = tzbsp_mmu_map(block->p_base,
                           block->v_base,
                           pre_l1_sz_kb,
                           block->tz_cfg);

    TZBSP_RETCHK(retval);
  }

  /* Don't call map and handle failures when the map size is zero */
  if (post_l1_sz_kb > 0)
  {
    retval = tzbsp_mmu_map(block->p_base + post_l1_off,
                           block->v_base + post_l1_off,
                           post_l1_sz_kb,
                           block->tz_cfg);

    TZBSP_RETCHK(retval);
  }

  return tzbsp_mmu_map(block->p_base + l1_off,
                       block->v_base + l1_off,
                       l1_sz_kb,
                       block->tz_cfg);
}

static int map_using_l2_ttes
(
  const mem_block_type * block
)
{
  const uint64 pre_l2_sz_kb =
    ((L2_SIZE_MSK & block->v_base) ?
     B2KB((L2_MAP_SIZE - (L2_SIZE_MSK & block->v_base))) : 0);

  const uint64 l2_sz_kb = ((block->size_in_kbytes - pre_l2_sz_kb) &
                           L2_KB_ADDR_MSK);
  const uint64 post_l2_sz_kb = ((block->size_in_kbytes - pre_l2_sz_kb) -
                                l2_sz_kb);

  const uint64 l2_off = KB2B(pre_l2_sz_kb);
  const uint64 post_l2_off = KB2B(pre_l2_sz_kb + l2_sz_kb);

  const uint32 num_l3_tts_req =
    tzbsp_get_num_l3_tts_needed(block->v_base,
                                (pre_l2_sz_kb > 0) ? 1 : 0) +
    tzbsp_get_num_l3_tts_needed((block->v_base + post_l2_off),
                                (post_l2_sz_kb > 0) ? 1 : 0);

  int retval;

  if (num_l3_tts_req > num_free_tts(3))
  {
    return -E_NO_MEMORY;
  }

  /* Don't call map and handle failures when the map size is zero */
  if (pre_l2_sz_kb > 0)
  {
    retval = tzbsp_mmu_map(block->p_base,
                           block->v_base,
                           pre_l2_sz_kb,
                           block->tz_cfg);

    TZBSP_RETCHK(retval);
  }

  /* Don't call map and handle failures when the map size is zero */
  if (post_l2_sz_kb > 0)
  {
    retval = tzbsp_mmu_map(block->p_base + post_l2_off,
                           block->v_base + post_l2_off,
                           post_l2_sz_kb,
                           block->tz_cfg);

    TZBSP_RETCHK(retval);
  }

  return tzbsp_mmu_map(block->p_base + l2_off,
                       block->v_base + l2_off,
                       l2_sz_kb,
                       block->tz_cfg);
}

int tzbsp_get_default_region_tz_cfg(uint32 v_addr, uint32 sz_in_kb,
                                    uint32* tzcfg)
{
  uint32 ii;

  if (tzcfg == NULL)
  {
    return -E_FAILURE;
  }

  for (ii = (TZBSP_ARRAY_SIZE(default_memory_map) - 1); ii != 0; ii--)
  {
    uint32 in_s = v_addr;
    uint32 in_e = v_addr + KB2B(sz_in_kb);
    uint32 bl_s = default_memory_map[ii].v_base;
    uint32 bl_e = (default_memory_map[ii].v_base +
                   KB2B(default_memory_map[ii].size_in_kbytes));

    if ((bl_s <= in_s && in_s < bl_e) && (bl_s < in_e && in_e <= bl_e))
    {
      *tzcfg = default_memory_map[ii].tz_cfg;

      return E_SUCCESS;
    }
    else if ((bl_s <= in_s && in_s < bl_e) || (bl_s < in_e && in_e <= bl_e))
    {
      return -E_NOT_SUPPORTED;
    }
  }

  return -E_NOT_FOUND;
}

/**
   @copydoc tzbsp_mmu_map_block_default_region
 */
int tzbsp_mmu_map_block_default_region
(
  const mem_block_type  * map
)
{
  int retval = E_SUCCESS;
  retval = tzbsp_mmu_add_block_default_region(map);
  TZBSP_RETCHK(retval);
  retval = tzbsp_mmu_map_block(map);
	TZBSP_RETCHK(retval);

  return retval;
}

/**
   @copydoc tzbsp_mmu_add_block_default_region
 */
int tzbsp_mmu_add_block_default_region
(
  const mem_block_type  * map
)
{
  mem_block_type temp;
  int            ii, jj;
  boolean        swap = FALSE;

  if (map == NULL)
  {
    return -E_FAILURE;
  }

  /* Find end of default memory map */
  for (ii = 0;
       (ii < (TZBSP_ARRAY_SIZE(default_memory_map) - 1)) &&
         (default_memory_map[ii].p_base != TZ_VAS_LAST_ADDRESS);
       ii++);

  /* Add all block items to the end of the default memory map */
  for (jj = 0;
       (ii < (TZBSP_ARRAY_SIZE(default_memory_map) - 1)) &&
         (map[jj].p_base != TZ_VAS_LAST_ADDRESS);
       ii++, jj++)
  {
    memcpy(&default_memory_map[ii], &map[jj], sizeof(mem_block_type));
  }

  /* This would be very bad to hit */
  if (map[jj].p_base != TZ_VAS_LAST_ADDRESS)
  {
    /* The default configuration structure needs to be larger */
    volatile int aa = 1;
    while (aa);
  }

  /* Sort in order of physical address and range. Order is set first by
   * the physical address, then by the size. */
  for (ii = 0;ii < (TZBSP_ARRAY_SIZE(default_memory_map) - 1);ii ++)
  {
    /* Last valid entry check */
    if (default_memory_map[ii].p_base == TZ_VAS_LAST_ADDRESS)
    {
      break;
    }
    for (jj = (ii + 1);jj < (TZBSP_ARRAY_SIZE(default_memory_map) - 1);jj ++)
    {
      /* Last valid entry check */
      if (default_memory_map[jj].p_base == TZ_VAS_LAST_ADDRESS)
      {
        break;
      }

      /* Compare the two entries to determine if a swap is required */
      if (default_memory_map[jj].p_base <= default_memory_map[ii].p_base)
      {
        /* Start address implies a swap is required. Need to compare the size
         * only if the start addresses are equal. The assumption is that if
         * the start addresses are equal the larger map comes first in the
         * table to prevent the overwriting of smaller maps */
        if (default_memory_map[jj].p_base == default_memory_map[ii].p_base)
        {
          if (default_memory_map[ii].size_in_kbytes <
              default_memory_map[jj].size_in_kbytes)
          {
            swap = TRUE;
          }
        }
        else
        {
          swap = TRUE;
        }

        if (TRUE == swap)
        {
          /* Swap the two entries */
          temp = default_memory_map[ii];
          default_memory_map[ii] = default_memory_map[jj];
          default_memory_map[jj] = temp;
        }
        swap = FALSE;
      }
    }
  }

  return E_SUCCESS;
}

/**
   @copydoc tzbsp_mmu_map_default_region
 */
int tzbsp_mmu_map_default_region
(
  uint32 p_addr,
  uint32 v_addr,
  uint32 sz_in_kb,
  uint32 msk
)
{
  mem_block_type pt[2];

  pt[0].p_base         = p_addr;
  pt[0].v_base         = v_addr;
  pt[0].size_in_kbytes = sz_in_kb;
  pt[0].tz_cfg         = msk;

  pt[1].p_base         = TZ_VAS_LAST_ADDRESS;
  pt[1].v_base         = TZ_VAS_LAST_ADDRESS;

  return tzbsp_mmu_map_block_default_region(pt);
}

/**
   @copydoc tzbsp_mmu_add_default_region
 */
int tzbsp_mmu_add_default_region
(
  uint32 p_addr,
  uint32 v_addr,
  uint32 sz_in_kb,
  uint32 msk
)
{
  mem_block_type pt[2];

  pt[0].p_base         = p_addr;
  pt[0].v_base         = v_addr;
  pt[0].size_in_kbytes = sz_in_kb;
  pt[0].tz_cfg         = msk;

  pt[1].p_base         = TZ_VAS_LAST_ADDRESS;
  pt[1].v_base         = TZ_VAS_LAST_ADDRESS;

  return tzbsp_mmu_add_block_default_region(pt);
}

/**
   @copydoc tzbsp_mmu_map
 */
int tzbsp_mmu_map
(
  uint32 p_addr,
  uint32 v_addr,
  uint32 sz_in_kb,
  uint32 msk
)
{
  mem_block_type pt[2];

  pt[0].p_base         = p_addr;
  pt[0].v_base         = v_addr;
  pt[0].size_in_kbytes = sz_in_kb;
  pt[0].tz_cfg         = msk;

  pt[1].p_base         = TZ_VAS_LAST_ADDRESS;
  pt[1].v_base         = TZ_VAS_LAST_ADDRESS;

  return tzbsp_mmu_map_block(pt);
}

#if DEBUG_MAP_CALLS
/* begin logging after MMU is enabled */
extern boolean begin_logging;
extern uint32 Load$$TZBSP_DP_DDR$$Base;
extern uint32 Load$$TZBSP_DP_DDR$$Limit;
#endif

/**
   @copydoc tzbsp_mmu_map_block
*/
int tzbsp_mmu_map_block
(
  const mem_block_type  * map
)
{
  int retval = 0;

  if (map == NULL)
  {
    return -E_FAILURE;
  }

  while ( map->v_base != TZ_VAS_LAST_ADDRESS )
  {
    uint32 paddr  = map->p_base;
    uint32 vaddr  = map->v_base;
    uint32 len_kb = map->size_in_kbytes;

#if DEBUG_MAP_CALLS
    if (begin_logging)
    {
      if (vaddr < 0x80000000)
      {
        uint32 dp_base = ( (unsigned int)&Load$$TZBSP_DP_DDR$$Base );
        uint32 dp_end  = ( (unsigned int)&Load$$TZBSP_DP_DDR$$Limit );
        if (vaddr < dp_base || vaddr > dp_end)
        {
          if ((map->tz_cfg & TZ_INVALID_MSK) == TZ_TTE_VALID)
          {
            TZBSP_LOG(0, "mapping 0x%08x++0x%08x as valid", vaddr, len_kb<<10);
          }
          else
          {
            TZBSP_LOG(0, "mapping 0x%08x++0x%08x as invalid", vaddr, len_kb<<10);
          }
        }
      }
    }
#endif

    /* unaligned pointer */
    if ((TZBSP_FLOOR_4K(vaddr) != vaddr) || (TZBSP_FLOOR_4K(paddr) != paddr))
    {
      return -E_BAD_ADDRESS;
    }

    /* length must be a page multiple */
    if (0 != (len_kb & 0x3))
    {
      return -E_OUT_OF_RANGE;
    }

    /* Determine if this request can be handled with only L1 TTEs */
    if (0 == (L1_SIZE_MSK & vaddr) && 0 == (len_kb & L1_KB_SIZE_MSK))
    {
      map_using_only_l1_ttes(map);
    }

    /* Determine if part of this request can be handled with L1 TTEs */
    else if (((B2KB(vaddr) + len_kb) & L1_KB_ADDR_MSK) >
             ((B2KB(vaddr) + B2KB(L1_MAP_SIZE) - 1) & L1_KB_ADDR_MSK))
    {
      retval = map_using_l1_ttes(map);

      TZBSP_RETCHK(retval);
    }

    /* Determine if the request can be completed only with L2 TTEs */
    else if (0 == (L2_SIZE_MSK & vaddr) && 0 == (len_kb & L2_KB_SIZE_MSK))
    {
      retval = map_using_only_l2_ttes(map);

      TZBSP_RETCHK(retval);
    }

    /* Determine if part of this request can be handled with L2 TTEs */
    else if (((B2KB(vaddr) + len_kb) & L2_KB_ADDR_MSK) >
             ((B2KB(vaddr) + B2KB(L2_MAP_SIZE) - 1) & L2_KB_ADDR_MSK))
    {
      retval = map_using_l2_ttes(map);

      TZBSP_RETCHK(retval);
    }

    /* The request can only be completed with only L3 TTEs */
    else
    {
      retval = map_using_only_l3_ttes(map);

      TZBSP_RETCHK(retval);
    }

    map++;
  }

  return E_SUCCESS;
}

/**
   Populates the initial translation tables with the content of the
   global memory map

   @retval
   None

   @par Side Effects
   Existing entries in the pagetable will be replaced.

   @dep The global memory map must be configured
   @dep The DDR must have been already called to get the DDR size
*/
int tzbsp_init_tts
(
  void
)
{
  int retval = E_SUCCESS;
  int cdlen = ((uint32) &Image$$TZBSP_CODE$$Base +
               (uint32) &Image$$TZBSP_CODE$$Length -
               (uint32) TZBSP_CODE_BASE);
  int rolen = ((uint32) &Image$$TZBSP_RO_DATA$$Length +
               (uint32) &Image$$TZBSP_DP_HASH$$Length);
  uint32 tt_len = ((uint32) &Image$$L1$$Length +
                   (uint32) &Image$$L2_POOL$$Length +
                   (uint32) &Image$$L3_POOL$$Length);

  map_all_ttes_as_invalid();

  /* The default memory map does not need to be added to itself */
  retval = tzbsp_mmu_map_block(default_memory_map);
  TZBSP_RETCHK(retval);

  /* The length of the code regions of TZBSP cannot be asigned to
     initial values of a structure at compile time, so setting this
     region to read-only and executable is done here.
   */
  retval =
    tzbsp_mmu_map_default_region(
      TZBSP_CODE_BASE,
      TZBSP_CODE_BASE,
      B2KB(TZBSP_ROUNDUP(uint32, cdlen, MIN_MAP_SIZE)),
      (TZ_READ_ONLY_PL1 | TZ_MEM_WRITE_BACK_WA | TZ_INR_SHAREABLE |
       TZ_EXECUTABLE));

  TZBSP_RETCHK(retval);

  /* The length of the RO region of TZBSP cannot be asigned to initial
     values of a structure at compile time, so setting this region to
     read-only and non-executable is done here.
   */
  retval =
    tzbsp_mmu_map_default_region(
      (uint32) &Image$$TZBSP_RO_DATA$$Base,
      (uint32) &Image$$TZBSP_RO_DATA$$Base,
      B2KB(TZBSP_ROUNDUP(uint32, rolen, MIN_MAP_SIZE)),
      (TZ_READ_ONLY_PL1 | TZ_MEM_WRITE_BACK_WA | TZ_INR_SHAREABLE |
       TZ_NON_EXECUTABLE));

  TZBSP_RETCHK(retval);

  retval =
    tzbsp_mmu_map_default_region(
      (uint32) &Image$$L1$$Base,
      (uint32) &Image$$L1$$Base,
      B2KB(TZBSP_ROUNDUP(uint32, tt_len, MIN_MAP_SIZE)),
      (TZ_READ_WRITE_PL1 | TZ_MEM_WRITE_BACK_WA | TZ_INR_SHAREABLE |
       TZ_NON_EXECUTABLE));

  TZBSP_RETCHK(retval);

  /* retval = tzbsp_mmu_map_block(test_memory_map); */
  /* TZBSP_RETCHK(retval); */

  return retval;
}


/**
  Removes the TZBSP mapping from the SBL translation tables.  SBL1
  translation tables will always be short-descriptors.

  @param  sbl_l1_pt   Pointer to SBL level-1 pagetable

  @par Dependencies
    None

  @retval
    None

  @par Side Effects
    None
*/
void mmu_remove_tz_mapping
(
  uint32 *sbl_l1_pt
)
{
  uint32 tz_l1_idx = TZBSP_CODE_BASE >> 20;

  /* SBL pagetable will be short-descriptors */
  if ((sbl_l1_pt[tz_l1_idx] & 0x3) == 0x1)
  {
    uint32 *tz_l2_pt  = (uint32*)(sbl_l1_pt[tz_l1_idx] & 0xFFFFFC00);
    uint32 tz_l2_idx  = (((uint32)TZBSP_CODE_BASE) >> 12 & 0xFF);
    uint32 tz_num_4kb =
      TZBSP_ROUNDUP(uint32, TZBSP_MAX_IMAGE_SIZE, 0x1000) >> 12;

    memset(&tz_l2_pt[tz_l2_idx], 0, sizeof(uint32) * tz_num_4kb);
  }
} /* mmu_remove_tz_mapping */
