/*==============================================================================

FILE:      icbcfg_query.c

DESCRIPTION: This file implements the ICB driver for querying DDR memory map.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/icbcfg_query.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2013/10/29  tk     This file created for DDR memory map query functions
2013/10/21  sds    Add additional method to attempt to map BRIC segments
2013/10/10  sds    BIMC remap support now checks for hardware support explicitly
2013/09/24  sds    Refactor code for better reuse and code size.
2013/09/23  sds    Fixes for BIMC remapper support
2013/08/30  sds    Added support for BIMC remap operations
2013/05/17  sds    Fixed the handling of non-power of two regions
2013/02/26  sds    Changed how available DDR regions are handled.
2012/08/07  sy     Fixed Linux compilation Error
2012/02/07  av     Created

        Copyright (c) 2012-2013 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg.h"
#include "icbcfgi.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "HALbimc.h"

/*============================================================================
                          DEFINES/MACROS
============================================================================*/

/*============================================================================
                          Type definitions
============================================================================*/
/* Device configuration storage for Segments_Init() and Remap() */
typedef struct
{
  uint32                     num_channels;
  uint32                     num_segments;
  uint32                     map_ddr_region_count;
  icbcfg_mem_region_type *   map_ddr_regions;
  HAL_bimc_InfoType *        bimc_hal_info;
  uint32 *                   channel_map;
  HAL_bimc_SlaveSegmentType *safe_reset_seg;
} icbcfg_device_config_type;

/* Memory region definition */
#define LAST_REGION 3

/* A more generic implementation friendly interpretation of the ddr_size_info struct. */
#define MAX_DDR_CHANNELS 2
#define MAX_CS           2
typedef struct
{
  uint32 sdram_size[MAX_DDR_CHANNELS][MAX_CS];
  uint64 sdram_addr[MAX_DDR_CHANNELS][MAX_CS];
} icb_ddr_size_info_type;

/*============================================================================
                          Helper functions
============================================================================*/

//*============================================================================*/
/**
@brief
    Fetch the items we have in device config

@param[in]  dev         DAL string device ID.
@param[in]  dev_config  The structure to store the device configuration in

@return
    Whether or not the operation was successful.
*/
/*============================================================================*/
bool get_device_configuration
(
   const char *            dev,
   icbcfg_device_config_type *dev_config
)
{
   bool ret_val = false;

   do
   {
      DALSYS_PROPERTY_HANDLE_DECLARE(hICBProp);
      DALSYSPropertyVar ICBPropVar;

      if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev, hICBProp))
      {
         break;
      }

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                  "num_channels", 0, &ICBPropVar)))
      {
         break;
      }
      dev_config->num_channels = ICBPropVar.Val.dwVal;

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                  "num_segments", 0, &ICBPropVar)))
      {
         break;
      }
      dev_config->num_segments = ICBPropVar.Val.dwVal;

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                   "map_ddr_region_count", 0, &ICBPropVar)))
      {
         break;
      }
      dev_config->map_ddr_region_count = ICBPropVar.Val.dwVal;

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                   "map_ddr_regions", 0, &ICBPropVar)))
      {
         break;
      }
      dev_config->map_ddr_regions = (icbcfg_mem_region_type *)ICBPropVar.Val.pStruct;

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                  "bimc_hal_info", 0, &ICBPropVar)))
      {
         break;
      }

      dev_config->bimc_hal_info = (HAL_bimc_InfoType *)ICBPropVar.Val.pStruct;

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                  "channel_map", 0, &ICBPropVar)))
      {
         break;
      }
      dev_config->channel_map = (uint32 *)ICBPropVar.Val.pStruct;

      if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                  "safe_reset_seg", 0, &ICBPropVar)))
      {
         break;
      }
      dev_config->safe_reset_seg = (HAL_bimc_SlaveSegmentType *)ICBPropVar.Val.pStruct;

      ret_val = true;
   } while( false );

   return ret_val;
}

//*============================================================================*/
/**
@brief
    Compress a region struct to minimal regions and apply constraints

@param[in]     dev_config   a pointer to the device configuration data to use
@param[in]     constrain    whether or not to constrain the minimal region sweeping
                            to avoid our safe reset/hardware reserved region
@param[inout]  ddr_regions  the regions descriptor to sweep together

*/
//*============================================================================*/
void compress_regions
(
  icbcfg_device_config_type *dev_config,
  bool                       constrain,
  icb_mem_map_type *         ddr_regions
)
{
   uint64 map_ddr_start_addr = 0, map_ddr_end_addr = 0;
   uint32 channel;

   /* Sweep together segments in place, if possible. */
   for( channel = 0; channel < dev_config->num_channels; channel++ )
   {
      region_type *channel_regions = ddr_regions->channels[channel].regions;
      uint32 region, comp_region;

      /* Skip the first region, since we are doing an in-place sweep. */
      for( comp_region = 0, region = 0; region < MAX_REGIONS; region++ )
      {
         if( !channel_regions[region].size )
         {
            continue;
         }

         /* Do no "sweeping together" for the first region/comp region pair.
          * They're the same thing, but we still need to apply constraints
          * if requested to do so. */
         if( 0 == region &&  0 == comp_region )
         {
           /* Nothing to do here. */
         }
         /* Regions are only considered contiguous regions if they
         * share the same interleaving mode, and are address contiguous. */
         else if( !((channel_regions[region].interleaved ==
                     channel_regions[comp_region].interleaved) &&
                     ( /* This region is contiguous after the previous region */
                       (channel_regions[region].base_addr==
                       (channel_regions[comp_region].base_addr +
                        channel_regions[comp_region].size)) ||
                       /* This region is contiguous before the previous region */
                       (channel_regions[comp_region].base_addr ==
                       (channel_regions[region].base_addr +
                        channel_regions[region].size))
                     )))
         {
            /* Create a new region. We're either the first, or a new one. */
            comp_region++;
            channel_regions[comp_region].interleaved = channel_regions[region].interleaved;
            channel_regions[comp_region].base_addr = channel_regions[region].base_addr;
            channel_regions[comp_region].size = channel_regions[region].size;
         }
         /* Not the first region, and extendable. */
         else
         {
            /* Add to existing region. */
            channel_regions[comp_region].size += channel_regions[region].size;

            /* If the merged region is under the existing region, use the merged region's start address. */
            if( channel_regions[comp_region].base_addr > channel_regions[region].base_addr )
            {
               channel_regions[comp_region].base_addr = channel_regions[region].base_addr;
            }
         }

         /* Constrain segment addresses/sizes, if required. */
         if( constrain )
         {
            /* First find the appropriate region to match to, since we need to limit
             * sweeping together of regions that aren't valid.
             * If we are below the start or DDR space, make it the start of DDR.
             * If we are past the end of DDR space, adjust the size accordingly. */
            uint32 ridx;
            for( ridx = 0; ridx < dev_config->map_ddr_region_count; ridx++ )
            {
               /* If the start address is bounded by this region, select it. */
               if( (channel_regions[comp_region].base_addr >= dev_config->map_ddr_regions[ridx].start_addr &&
                    channel_regions[comp_region].base_addr < dev_config->map_ddr_regions[ridx].end_addr) )
               {
                  map_ddr_start_addr = dev_config->map_ddr_regions[ridx].start_addr;
                  map_ddr_end_addr   = dev_config->map_ddr_regions[ridx].end_addr;
                  break;
               }
            }

            /* If we didn't find a region that matches, skip it. */
            if( ridx == dev_config->map_ddr_region_count )
            {
               continue;
            }

            /* Enforce memory map limits on DDR space. */
            if( channel_regions[comp_region].base_addr < map_ddr_start_addr )
            {
               channel_regions[comp_region].base_addr = map_ddr_start_addr;
            }
            if( channel_regions[comp_region].base_addr + channel_regions[comp_region].size > map_ddr_end_addr )
            {
               channel_regions[comp_region].size = map_ddr_end_addr - channel_regions[comp_region].base_addr;
            }
         }
      }

      /* Check to see if we exceed the number of segments allowed. */
      if( comp_region > dev_config->num_segments )
      {
        break;
      }

      /* Clear unused regions. */
      for( comp_region++; comp_region < MAX_REGIONS; comp_region++ )
      {
         channel_regions[comp_region].interleaved = false;
         channel_regions[comp_region].base_addr = 0;
         channel_regions[comp_region].size = 0;
      }
   }
}

//*============================================================================*/
/**
@brief
    Get the current system memory map from the hardware, inserting reserved
    region to the memory map if it is determined we removed it earlier,
    and we are asked to do so.

@param[in]  hICBProp       the property handle to use
@param[in]  add_reserved   whether or not to insert the safe reset/hw reserved region
                           back into the memory map
@param[in]  sys_map        the structure to store the memory map in

*/
//*============================================================================*/
void get_system_memory_map
(
  icbcfg_device_config_type *dev_config,
  bool                       add_reserved,
  icb_mem_map_type *         sys_map
)
{
   uint32 channel;
   icb_mem_map_type sub_map;

   /* Clear the map structure passed to us. */
   memset(sys_map, 0, sizeof(icb_mem_map_type));

   for( channel = 0; channel < dev_config->num_channels; channel++ )
   {
      uint32 region, pass, seg, sub_seg = 0, add_seg = 0;
      memset(&sub_map, 0, sizeof(sub_map));

      /* Go through and collect any enabled segments. */
      for( region = 0; region < dev_config->num_segments; region++ )
      {
         HAL_bimc_SlaveSegmentType hal_seg;
         region_type *channel_regions;
         HAL_bimc_GetSlaveSegment( dev_config->bimc_hal_info,
                                   dev_config->channel_map[channel],
                                   region, &hal_seg );

         if( hal_seg.bEnable )
         {
            if( hal_seg.type == BIMC_SEGMENT_TYPE_ADDITIVE )
            {
               /* Additive segments go to the sys_map passed to us.
                * We'll deal with any subtractive mappings later. */
               channel_regions = sys_map->channels[channel].regions;
               seg = add_seg++;
            }
            else
            {
               /* Subtractive segments go to the sub_map we're keeping.
                * We'll use these to shrink the sys_map after we've collected them.
                * If we are expected to add the reserved region to the map,
                * skip any subtractive segments that look like it. */
               if( add_reserved &&
                   hal_seg.uStartAddr   == dev_config->safe_reset_seg->uStartAddr &&
                   hal_seg.uSegmentSize == dev_config->safe_reset_seg->uSegmentSize )
               {
                 continue;
               }
               channel_regions = sub_map.channels[channel].regions;
               seg = sub_seg++;
            }

            channel_regions[seg].base_addr   = hal_seg.uStartAddr;
            channel_regions[seg].size        = hal_seg.uSegmentSize;
            channel_regions[seg].interleaved =
              (hal_seg.interleave == BIMC_INTERLEAVE_EVEN || hal_seg.interleave == BIMC_INTERLEAVE_ODD);
         }
      }

      /* Shrink sys_map regions, if we found any subtractive mappings.
       * We will, at most, have to make as many passes through the additive
       * segments as we have subtractive segments.
       * It's more work to sweep together the subtractive segments than it is
       * to just brute force check after every modification. */
      for( pass = 0; pass < sub_seg; pass++ )
      {
         for( seg = 0; seg < sub_seg; seg++ )
         {
            region_type *sub_region = &sub_map.channels[channel].regions[seg];
            if( sub_region->size == 0 ) continue;

            for( region = 0; region < dev_config->num_segments; region++ )
            {
              region_type *add_region = &sys_map->channels[channel].regions[region];
              if( add_region->size == 0 ) continue;

               /* Check here if subtractive segment will stack at the end. */
               if( (add_region->base_addr + add_region->size) ==
                   (sub_region->base_addr + sub_region->size) )

               {
                  /* We found a match, so shrink the additive region.
                   * Also clear the subtractive region for any subsequent passes. */
                  add_region->size      -= sub_region->size;
                  sub_region->base_addr  = 0;
                  sub_region->size       = 0;
               }
               /* Check here if the subtractive segment is contained by region. */
               else if( (add_region->base_addr < sub_region->base_addr) &&
                        ((add_region->base_addr + add_region->size) >
                         (sub_region->base_addr + sub_region->size)))
               {
                  /* We found a match, so split the additive region.
                   * First, copy the additive region we're working on to the next slot.
                   * Resize the first half of the split. */
                  sys_map->channels[channel].regions[region+1] = *add_region;
                  add_region->size       = sub_region->base_addr - add_region->base_addr;

                  /* Now, adjust the second half. */
                  add_region = &sys_map->channels[channel].regions[region+1];
                  add_region->size      -= (sub_region->base_addr - add_region->base_addr) + sub_region->size;
                  add_region->base_addr  = sub_region->base_addr + sub_region->size;

                  /* Finally, clear the subtractive region for any subsequent passes. */
                  sub_region->base_addr  = 0;
                  sub_region->size       = 0;
               }
               /* Check here if the subtractive segment will stack at the start. */
               else if( (add_region->base_addr == sub_region->base_addr &&
                         add_region->size > sub_region->size) )
               {
                  /* We found a match, so shrink the additive region.
                   * Also clear the subtractive region for any subsequent passes. */
                  add_region->base_addr += sub_region->size;
                  add_region->size      -= sub_region->size;
                  sub_region->base_addr  = 0;
                  sub_region->size       = 0;
               }
            }
         }
      }
   }
}

/*============================================================================
                          External functions
============================================================================*/

//*============================================================================*/
/**
@brief
      Returns the current memory map.

@param[in]    dev  DAL string device ID.
@param[inout] info The information about the DDR configuration

@return
      icbcfg_error_type indicating success or error.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
icbcfg_error_type ICB_Get_Memmap( const char * dev, icb_mem_map_type * info )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR;

   do
   {
      icbcfg_device_config_type  dev_config;

      /* Validate config pointers. */
      if (!dev || !info)
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      /* Ensure BIMC HAL has been initialized. */
      HAL_bimc_Init(dev_config.bimc_hal_info);

      /* Get the system memory map, holes and all. */
      get_system_memory_map(&dev_config, false, info);
      compress_regions(&dev_config, true, info);

      ret_val = ICBCFG_SUCCESS;
   } while( false );

   return ret_val;
}
/* vim: set ts=3:sts=3:sw=3:et */
