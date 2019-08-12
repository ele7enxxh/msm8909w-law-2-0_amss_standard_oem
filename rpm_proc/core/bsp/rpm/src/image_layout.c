/*===========================================================================
                              image_layout.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

The data RAM is laid out as follows...
For sections that don't have a fixed address, they will at least appear in the
order listed below

--------------------------------------------------------------------
|    SECTION NAME   |  ADDRESS  |   SIZE   | READABLE FROM MASTERS |
====================================================================
|                   |           |          |                       |
| IMAGE_INFO_HEADER | 0x90000   |   0x40   |         YES           |
|                   |           |          |                       |
--------------------------------------------------------------------
|                   |           |          |                       |
|  DATA_RAM RW/ZI   | 0x90040   | VARIABLE |         NO            |
|                   |           |          |                       |
--------------------------------------------------------------------
|                   |           |          |                       |
|       HEAP        | VARIABLE  |MAX AVAIL |         NO            |
|                   |           |          |                       |
--------------------------------------------------------------------
|                   |           |          |                       |
|      STACK        | VARIABLE  |  0x800   |         NO            |
|                   |           |          |                       |
--------------------------------------------------------------------
|                   |           | VARIABLE |                       |
|     CPR STATS     | VARIABLE  |   PER    |         YES           |
|                   |           |  TARGET  |                       |
--------------------------------------------------------------------
|                   |           |          |                       |
|    SLEEP STATS    | 0x9DBA0   |   0x60   |         YES           |
|                   |           |          |                       |
--------------------------------------------------------------------
|                   |           |          |                       |
|      RPM LOG      | 0x9DC00   |  0x2400  |         YES           |
|                   |           |          |                       |
--------------------------------------------------------------------

===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <string.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "image_layout.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

#if defined(MSM8909_IMAGE_LAYOUT)
#define RPM_CPR_STATS_SIZE              0x2F0
#elif defined(MDM9x45_IMAGE_LAYOUT)
#define RPM_CPR_STATS_SIZE              0x1F0 //This is the size on 9x45.
#elif defined(MDM9x07_IMAGE_LAYOUT)
#define RPM_CPR_STATS_SIZE              0x3F0 //This is the size on 9x07.
#else
#define RPM_CPR_STATS_SIZE              0x100 //This is the size on 8916.
#endif

//Utility macros.
#define __ALLIGN_UP_TO_16(a)            (((a)+0xF)&0xFFFFFFF0)
#define __ROUND_DOWN_TO_NEAREST_64(a)   ((a)&0xFFFFFFC0)
#define RPM_COMPILE_ASSERT(x)           int __dummy[(int)x]

//Start defining the requirements and addresses for each of the image sections.
#if defined(MDM9x45_IMAGE_LAYOUT)
#define RPM_LOG_END                     0xA4000
#else
#define RPM_LOG_END                     0xA0000
#endif

#define RPM_LOG_RESERVED_SPACE          0x2400
#define RPM_LOG_START                   (RPM_LOG_END-RPM_LOG_RESERVED_SPACE)
#if defined(MDM9x45_IMAGE_LAYOUT)
RPM_COMPILE_ASSERT(RPM_LOG_START==0xA1C00);     //Required since RPM clients expect this on 9x45
#else
RPM_COMPILE_ASSERT(RPM_LOG_START==0x9DC00);     //Required since RPM clients already expect this
#endif

#define RPM_SLEEP_STATS_END             RPM_LOG_START
#define RPM_SLEEP_STATS_RESERVED_SPACE  0x60
#define RPM_SLEEP_STATS_START           (RPM_SLEEP_STATS_END - RPM_SLEEP_STATS_RESERVED_SPACE)
#if defined(MDM9x45_IMAGE_LAYOUT)
RPM_COMPILE_ASSERT(RPM_SLEEP_STATS_START==0xA1BA0);     //Required since RPM clients expect this on 9x45
#else
RPM_COMPILE_ASSERT(RPM_SLEEP_STATS_START==0x9DBA0);     //Required since RPM clients already expect this
#endif

#define RPM_CPR_STATS_END             (RPM_SLEEP_STATS_START)
#define RPM_CPR_STATS_RESERVED_SPACE  __ALLIGN_UP_TO_16(RPM_CPR_STATS_SIZE)
#define RPM_CPR_STATS_START           (RPM_CPR_STATS_END - RPM_CPR_STATS_RESERVED_SPACE)

#define RPM_STACK_END                 __ROUND_DOWN_TO_NEAREST_64(RPM_CPR_STATS_START)
#define RPM_STACK_SIZE                0x800   //2K
#define RPM_STACK_START               (RPM_STACK_END-RPM_STACK_SIZE)

#define RPM_HEAP_LIMIT                RPM_STACK_START

#define RPM_IMAGE_DATA_RAM_START      0x90000
#define RPM_IMAGE_INFO_HEADER_START   RPM_IMAGE_DATA_RAM_START
#define RPM_IMAGE_HEADER_MAX_SIZE     0x40
#define RPM_IMAGE_INFO_HEADER_END     RPM_IMAGE_INFO_HEADER_START + RPM_IMAGE_HEADER_MAX_SIZE

//const int rpm_reset_stack_base __attribute__((at(0x0))) = RPM_STACK_END;
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct
{
  unsigned reserved_space[0x10];
} rpm_image_info_header_reserved_space;

typedef struct
{
  unsigned reserved[4];
  unsigned cpr_stats_address;
  unsigned sleep_stats_address;
  unsigned rpm_log_address;
  int rpm_free_heap_space;
} rpm_image_info_header;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
static bool initialized = false;

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
extern char Image$$OTHER_SECTIONS$$ZI$$Base[];

const rpm_image_section_info rpm_image_section_array[RPM_IMAGE_SECTIONS_COUNT] =
  {
    {   //RPM_IMAGE_INFO_HEADER
      .section_base = (char*)RPM_IMAGE_INFO_HEADER_START,
      .section_end  = (char*)RPM_IMAGE_INFO_HEADER_END,
    },
    { //RPM_HEAP
      .section_base = Image$$OTHER_SECTIONS$$ZI$$Base,
      .section_end  = (char*)RPM_HEAP_LIMIT,
    },
    { //RPM_STACK
      .section_base = (char*)RPM_STACK_START,
      .section_end  = (char*)RPM_STACK_END,
    },
    { //RPM_CPR_STATS
      .section_base = (char*)RPM_CPR_STATS_START,
      .section_end  = (char*)RPM_CPR_STATS_END,
    },
    { //RPM_SLEEP_STATS
      .section_base = (char*)RPM_SLEEP_STATS_START,
      .section_end  = (char*)RPM_SLEEP_STATS_END,
    },
    { //RPM_LOG
      .section_base = (char*)RPM_LOG_START,
      .section_end  = (char*)RPM_LOG_END,
    },
  };


//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static rpm_image_info_header_reserved_space image_info_header_buffer __attribute__((at(RPM_IMAGE_DATA_RAM_START))) = {1};

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================


/*===========================================================================
FUNCTION: populate_image_header

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void populate_image_header(void)
{
  const uint32 build_id = RPM_BUILD_ID;

  rpm_image_info_header* image_header = (rpm_image_info_header*)rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_base;
  CORE_VERIFY((char*)&image_header[1] <= rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_end);

  memcpy(image_header->reserved, &build_id, sizeof(uint32));
  image_header->cpr_stats_address=(unsigned)rpm_image_section_array[RPM_CPR_STATS].section_base;
  image_header->sleep_stats_address=(unsigned)rpm_image_section_array[RPM_SLEEP_STATS].section_base;
  image_header->rpm_log_address=(unsigned)rpm_image_section_array[RPM_LOG].section_base;
}

void rpm_adjust_free_heap_space(int heap_adjust)
{
  rpm_image_info_header* image_header = (rpm_image_info_header*)rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_base;
  image_header->rpm_free_heap_space += heap_adjust;
}

int get_free_heap_space(void)
{
    rpm_image_info_header* image_header = (rpm_image_info_header*)rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_base;
    return image_header->rpm_free_heap_space;
}

void rpm_free_heap_space_init(void)
{
    if(!initialized)
    {
        rpm_image_info_header* image_header = (rpm_image_info_header*)rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_base;
        image_header->rpm_free_heap_space = 0;
        initialized = true;
    }
}

