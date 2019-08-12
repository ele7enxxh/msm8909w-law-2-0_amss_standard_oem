/**
@file mba_bimc.c
@brief BIMC related functions.

*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/src/bimc/mba_bimc_nonicb.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
08/10/12   tk      Initial version
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <IxErrno.h>
#include <comdef.h>
#include <HALhwio.h>
#include "mba_bimc.h"
#include "msmhwiobase.h"
#include "mba_hwio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/** Memory including this address and above is peripheral memory. */
#define MBA_DDR_MAX     0xf8000000

#define MBA_4GB                       0x100000000
#define MBA_2GB                       0x80000000
#define MBA_1GB                       0x40000000
#define MBA_512MB                     0x20000000
#define MBA_256MB                     0x10000000
#define MBA_128MB                     0x8000000
#define MBA_64MB                      0x4000000
#define MBA_32MB                      0x2000000
#define MBA_16MB                      0x1000000

#define MBA_ARRAY_SIZE(xx) (sizeof(xx) / sizeof((xx)[0]))

typedef enum
{
  MBA_CH0_RANK0 = 0,
  MBA_CH0_RANK1 = 1,
  MBA_NUM_RANKS = 2,
} mba_rank_index_t;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct mba_rank_s
{
  uint32  start;
  uint32  size;
  boolean interleaved;
} mba_rank_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/**
 * Storage for the DDR regions variables
 */
mba_ddr_region_t g_mba_ddr_regions[MBA_NUM_RANKS];
uint32 g_mba_num_ddr_regions = 0;

/**
 * DDR rank layout is stored here. This needs to be separate from the DDR
 * regions as those can be coalesced.
 */
static mba_rank_t g_mba_ranks[MBA_NUM_RANKS];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 * SCMO_ADDR_BASE_Csn[ADDR_BASE] represents bits 31:26 which are bits 5:0 here,
 * hence the shifting to produce the actual physical address.
 *
 * @param [in] base SCMO_ADDR_BASE_Csn[ADDR_BASE] at [5:0]
 *
 * @return The real physical base address of a DDR rank.
 */
static uint32 mba_rank_base_to_real_base(uint32 base)
{
  return base << 26;
}

/**
 * Converts SCMO_ADDR_MASK_Csn[ADDR_MASK] to bytes.
 *
 * @param [in] mask   Field SCMO_ADDR_MASK_Csn[ADDR_MASK].
 *
 * @return The size of the DDR rank in bytes.
 */
static uint32 mba_rank_addr_mask_to_bytes(uint32 mask)
{
  switch( mask ) {
    case 0x20: return MBA_2GB;
    case 0x30: return MBA_1GB;
    case 0x38: return MBA_512MB;
    case 0x3C: return MBA_256MB;
    case 0x3E: return MBA_128MB;
    case 0x3F: return MBA_64MB;
    default: return 0;
  }
}

/**
 * Retrieves the layout of a DDR rank.
 *
 * @param [out] rank  Pointer to a rank structure to store the rank layout.
 * @param [in] index  Index of the DDR rank.
 */
static void mba_grab_rank(mba_rank_t* rank, mba_rank_index_t index)
{
  rank->start = 0;
  rank->size  = 0;
  rank->interleaved = FALSE;

  switch(index)
  {
    case MBA_CH0_RANK0:
      if(HWIO_INFI(BIMC_S_DDR0_SCMO_ADDR_MAP_CSn, 0, RANK_EN))
      {
        rank->start = HWIO_INFI(BIMC_S_DDR0_SCMO_ADDR_BASE_CSn, 0, ADDR_BASE);
        rank->size  = HWIO_INFI(BIMC_S_DDR0_SCMO_ADDR_MASK_CSn, 0, ADDR_MASK);
        rank->interleaved = HWIO_INF(BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG,
                                     INTERLEAVE_CS0);
      }
      break;
    case MBA_CH0_RANK1:
      if(HWIO_INFI(BIMC_S_DDR0_SCMO_ADDR_MAP_CSn, 1, RANK_EN))
      {
        rank->start = HWIO_INFI(BIMC_S_DDR0_SCMO_ADDR_BASE_CSn, 1, ADDR_BASE);
        rank->size  = HWIO_INFI(BIMC_S_DDR0_SCMO_ADDR_MASK_CSn, 1, ADDR_MASK);
        rank->interleaved = HWIO_INF(BIMC_S_DDR0_SCMO_SLV_INTERLEAVE_CFG,
                                     INTERLEAVE_CS1);
      }
      break;
    default:
      break;
  }

  /* Convert the intermediate values to real values. */
  rank->start = mba_rank_base_to_real_base(rank->start);
  rank->size  = mba_rank_addr_mask_to_bytes(rank->size);
}

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Returns the EBI channel for the address range, which can fall on both
 * channels, one channel or no channel, depending on EBI memory interleaving
 * configuration and the address range. Interleaving configuration can be
 * partial (some ranks are interleaved and others non-interleaved at the same
 * time).
 *
 * @param[in] start   Range start address.
 * @param[in] end     Range end address.
 *
 * @return The EBI channel for the address range [\c start, \c end), one of \c
 *         MBA_EBI_CH_*.
 */

/**
 * Returns the EBI channel for the address range, which can fall on both
 * channels, one channel or no channel, depending on EBI memory interleaving
 * configuration and the address range. Interleaving configuration can be
 * partial (some ranks are interleaved and others non-interleaved at the same
 * time).
 *
 * @param[in] start   Range start address.
 * @param[in] end     Range end address.
 *
 * @return The EBI channel for the address range [\c start, \c end), one of \c
 *         MBA_EBI_CH_*.
 */
uint8 mba_ebi_channel_for_addr(uint32 start, uint32 end)
{
  mba_rank_index_t i;
  uint32 size;

  /* Check that we are in DDR range. */
  if(end < start || MBA_DDR_MAX <= end)
  {
    return MBA_EBI_CH_NONE;
  }
  size = end - start;

  /* We are always in the 32 bit range with start/end as the maximum address of
   * DDR memory is below the 4GB limit. */
  for(i=MBA_CH0_RANK0; i < MBA_NUM_RANKS; i++)
  {
    /* Check if [start, range) is contained within a DDR rank. */
    if(g_mba_ranks[i].start <= start &&
       size < g_mba_ranks[i].size)
    {
      if(g_mba_ranks[i].interleaved)
      {
        return MBA_EBI_CH_BOTH;
      }
      else /* Non interleaved. */
      {
        return (MBA_CH0_RANK0 == i || MBA_CH0_RANK1 == i) ?
                MBA_EBI_CH_0: MBA_EBI_CH_1;
      }
    }
  }

  /* The memory range doesn't fall on any EBI channel. */
  return MBA_EBI_CH_NONE;
}

/* @copydoc mba_coalesce_ddr_regions */
uint32 mba_coalesce_ddr_regions( mba_ddr_region_t *regions, uint32 num_regions )
{
  uint32 i, j;
  uint32 total_regions = 0;
  
  if( num_regions < 2 ) 
  {
    /* Nothing to do */
    return num_regions;
  }

  /* Compare each DDR region against the others, attempting to coalesce */
  for( j = 0; j < num_regions; /*incremented below*/ ) 
  {
    uint64 cstart = (uint64)regions[j].start; /* 64-bit cast to prevent overflow errors */
    uint64 csize  = (uint64)regions[j].size;
    
    for( i = (j + 1); i < num_regions; i++ ) 
    {
      uint64 istart = (uint64)regions[i].start;
      uint64 isize  = (uint64)regions[i].size;
      if( isize == 0 ) 
      { 
        continue;
      }
      else if( csize == 0 ) 
      {
        regions[j].start = (uint32)istart;  /* Since the regions are modified in-place, the seed 
                                             * for the next coalesce needs to be done dynamically */
      }
      if( ((istart + isize) >= cstart) && (istart <= (cstart + csize)) ) 
      {
        /* Iterator region is contained within, or adjacent to, current region; coalesce this region */
        regions[j].start = ( istart < cstart ) ? (uint32)istart : (uint32)cstart;
        regions[j].size += (uint32)isize;
        regions[i].start = 0;
        regions[i].size = 0;
        break;
      }
    }
    
    /* Since each merge operation could allow other previous unmergeable
     * regions to be coalesced, the outer loop is only iterated once the entire
     * array is scanned in a single pass 
     */
    if( i == num_regions ) 
    {
      if( regions[j].size != 0 ) 
      {
        total_regions++;
      }
      j++;
    }
  }

  return total_regions;
}


/**
 * Sets the DDR Size, at runtime, to account for various HW configurations.
 * NOTE: This function will cause a data abort if called with the MMU enabled.
 */
int mba_read_ddr_size( void )
{
  mba_rank_index_t i;

  for(i=MBA_CH0_RANK0; i < MBA_NUM_RANKS; i++)
  {
    mba_grab_rank(&g_mba_ranks[i], i);

    g_mba_ddr_regions[i].start       = g_mba_ranks[i].start;
    g_mba_ddr_regions[i].size        = g_mba_ranks[i].size;
    g_mba_ddr_regions[i].interleaved = g_mba_ranks[i].interleaved;
  }

  g_mba_num_ddr_regions = mba_coalesce_ddr_regions( g_mba_ddr_regions,
                                        MBA_ARRAY_SIZE(g_mba_ddr_regions) );
  return E_SUCCESS;
}


