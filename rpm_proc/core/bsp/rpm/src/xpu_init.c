/*=============================================================================

                                XPU Init

GENERAL DESCRIPTION
  This file contains the configuration for the RPM message RAM XPU.

EXTERNALIZED FUNCTIONS
  xpu_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be called before allowing SBL1 to proceed.

Copyright (c) 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.

 $History: $
 $DateTime: 2015/02/25 02:53:43 $
 $Author: pwbldsvc $

=============================================================================*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "rpm_hwio.h"
#include "HALhwio.h"
#include "image_layout.h"

#define SHARED_IMEM_BASE 0x68600000 

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

typedef enum
{
    NOBODY = 0,
    RPM,
    TZ,
    AP,
    MSS,
    LPASS,
    WCNSS,
    NUM_EES,
} EEType;

#ifndef MSM8x26_STUBS
static const unsigned static_vmid_map[] =
{
    [NOBODY] = 0,
    [RPM]    = 1,
    [TZ]     = 2,
    [AP]     = 3,
    [MSS]    = 4,
    [LPASS]  = 5,
    [WCNSS]  = 6,
};

static unsigned lookup_vmid(EEType ee) __pure
{
    return static_vmid_map[ee];
}

static unsigned translate_vmid_mask(unsigned ee_type_mask) __pure
{
    unsigned vmid_mask = 0;

    while(ee_type_mask)
    {
        EEType ee = (EEType)__clz(__rbit(ee_type_mask));
        vmid_mask |= (1 << lookup_vmid(ee));
        ee_type_mask &= ~(1 << ee);
    }

    return vmid_mask;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    bool   all_read_only;
    bool   all_read_write;
    EEType read_only_vmid;
    EEType read_write_vmid;
} SinglePermissionType;

typedef struct
{
    unsigned read_vmid_mask;
    unsigned write_vmid_mask;
} MultiPermissionType;

typedef struct
{
  unsigned             start_addr;
  unsigned             end_addr;
} MPUPermissionRange;

typedef void (*GetRangeFn)(MPUPermissionRange* range);

typedef struct
{
  GetRangeFn           rangeFn;
  SinglePermissionType perms;
} MPUPermissionType;

// The code RAM APU has a single partition.  This partition can individually mark which vmids have read or write access.
typedef struct
{
    unsigned            super_users_vmid_mask;
    MultiPermissionType perms[1];
} CodeRAMConfig;

// The data RAM MPU has configurable address partitions of single-permission type.
typedef struct
{
    unsigned          super_users_vmid_mask;
    unsigned          unmapped_read_vmid_mask;
    unsigned          unmapped_write_vmid_mask;
    MPUPermissionType perms[6];
} DataRAMConfig;

typedef struct
{
    unsigned             super_users_vmid_mask;
    SinglePermissionType perms[32];
} MessageRAMConfig;

#ifndef MSM8x26_STUBS
unsigned perms_mask(const SinglePermissionType *perms) __pure
{
    unsigned mask = 0;

    if(perms->all_read_only)
        mask |= (1 << 25);
    else if(perms->read_only_vmid)
        mask |= (1 << 24) | (lookup_vmid(perms->read_only_vmid) << 16);

    if(perms->all_read_write)
        mask |= (1 << 9);
    else if(perms->read_write_vmid)
        mask |= (1 << 8) | (lookup_vmid(perms->read_write_vmid));

    return mask;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

static const CodeRAMConfig cram_cfg =
{
    .super_users_vmid_mask = (1 << RPM),
    .perms =
    {
        {
            .read_vmid_mask  = (1 << RPM),
            .write_vmid_mask = (1 << RPM),
        },
    },
};

static void DataRAMConfig_ImageHeader(MPUPermissionRange* range)
{
    range->start_addr = (unsigned) rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_base;
    range->end_addr   = (unsigned) rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_end;
}

static void DataRAMConfig_RPMPeripheralsSpace(MPUPermissionRange* range)
{
    range->start_addr = 0x180000;
    range->end_addr   = 0x189000;
}

static void DataRAMConfig_InternalDataRAM(MPUPermissionRange* range)
{
    range->start_addr = (unsigned) rpm_image_section_array[RPM_IMAGE_INFO_HEADER].section_end;
    range->end_addr   = (unsigned) rpm_image_section_array[RPM_STACK].section_end;
}

static void DataRAMConfig_SharedDataRAM(MPUPermissionRange* range)
{
    range->start_addr = (unsigned) rpm_image_section_array[RPM_STACK].section_end;
    range->end_addr   = 0x1A0000;
}

static void DataRAMConfig_DefaultRange(MPUPermissionRange* range)
{
    range->start_addr = 0xFFFFFFFF;
    range->end_addr   = 0xFFFFFFFF;
}

static const DataRAMConfig dram_cfg =
{
    .super_users_vmid_mask    = (1 << RPM),
    .unmapped_read_vmid_mask  = 0,
    .unmapped_write_vmid_mask = 0,
    .perms =
    {
        {   // RPM internal peripherals space (csr, qtimer, mpu, apu, etc.)
            .rangeFn    = DataRAMConfig_RPMPeripheralsSpace,
            .perms      =
            {
                .all_read_only   = false,
                .all_read_write  = false,
                .read_only_vmid  = NOBODY,
                .read_write_vmid = RPM,
            },
        },
        {
            .rangeFn    = DataRAMConfig_ImageHeader,
            .perms      =
            {
                .all_read_only   = true,
                .all_read_write  = false,
                .read_only_vmid  = NOBODY,
                .read_write_vmid = RPM,
            },
        },
        {   // Internal data RAM
            .rangeFn    = DataRAMConfig_InternalDataRAM,
            .perms      =
            {
                .all_read_only   = false,
                .all_read_write  = false,
                .read_only_vmid  = NOBODY,
                .read_write_vmid = RPM,
            },
        },
        {   // Shared data RAM
            .rangeFn    = DataRAMConfig_SharedDataRAM,
            .perms      =
            {
                .all_read_only   = true,
                .all_read_write  = false,
                .read_only_vmid  = NOBODY,
                .read_write_vmid = RPM,
            },
        },
        //
        // Below this part is just closing out unused parts.
        //
        {
            .rangeFn    = DataRAMConfig_DefaultRange,
            .perms      =
            {
                .all_read_only   = false,
                .all_read_write  = false,
                .read_only_vmid  = NOBODY,
                .read_write_vmid = NOBODY,
            },
        },
        {
            .rangeFn    = DataRAMConfig_DefaultRange,
            .perms      =
            {
                .all_read_only   = false,
                .all_read_write  = false,
                .read_only_vmid  = NOBODY,
                .read_write_vmid = NOBODY,
            },
        },
    },
};

static const MessageRAMConfig mram_cfg =
{
    .super_users_vmid_mask = (1 << RPM),
    .perms =
    {
        {   // Apps channel header and vmpm
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = AP,
        },
        {   // Apps fifo #1
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = AP,
        },
        {   // Apps fifo #1 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = AP,
        },
        {   // Apps fifo #2
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = AP,
        },
        {   // Apps fifo #2 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = AP,
        },
        {   // MSS channel header and vmpm
            // temporaily allow access due to SSR 
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = MSS,
        },
        {   // MSS fifo #1
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = MSS,
        },
        {   // MSS fifo #1 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = MSS,
        },
        {   // MSS fifo #2
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = MSS,
        },
        {   // MSS fifo #2 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = MSS,
        },
        {   // LPASS channel header and vmpm
            // temporaily allow access due to SSR
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = LPASS,
        },
        {   // LPASS fifo #1
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = LPASS,
        },
        {   // LPASS fifo #1 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = LPASS,
        },
        {   // LPASS fifo #2
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = LPASS,
        },
        {   // LPASS fifo #2 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = LPASS,
        },
        {   // WCNSS channel header and vmpm
            // temporaily allow access due to SSR
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = WCNSS,
        },
        {   // WCNSS fifo #1
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = WCNSS,
        },
        {   // WCNSS fifo #1 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = WCNSS,
        },
        {   // WCNSS fifo #2
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = WCNSS,
        },
        {   // WCNSS fifo #2 (cont.)
            .all_read_only   = false,
            .all_read_write  = false,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = WCNSS,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
        {   // IMEM workaround region
            .all_read_only   = false,
            .all_read_write  = true,
            .read_only_vmid  = NOBODY,
            .read_write_vmid = NOBODY,
        },
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

static void code_ram_xpu_setup(void)
{
    HWIO_OUTI(RPM_APU_XPU_RPU_ACRn, 0, translate_vmid_mask(cram_cfg.super_users_vmid_mask));

    for(unsigned i = 0; i < ARRAY_SIZE(cram_cfg.perms); i++)
    {
        HWIO_OUTI2(RPM_APU_XPU_RGn_RACRm, i, 0, translate_vmid_mask(cram_cfg.perms[i].read_vmid_mask));
        HWIO_OUTI2(RPM_APU_XPU_RGn_WACRm, i, 0, translate_vmid_mask(cram_cfg.perms[i].write_vmid_mask));
    }

    HWIO_OUT(RPM_APU_XPU_CR,  0x11F);
}

static void data_ram_xpu_setup(void)
{
    HWIO_OUTI(RPM_MPU_XPU_RPU_ACRn, 0, translate_vmid_mask(dram_cfg.super_users_vmid_mask));

    for(unsigned i = 0; i < ARRAY_SIZE(dram_cfg.perms); i++)
    {
        MPUPermissionRange range;
        dram_cfg.perms[i].rangeFn(&range);
        HWIO_OUTI2(RPM_MPU_XPU_PRTn_RACRm, i, 0, perms_mask(&dram_cfg.perms[i].perms));
        HWIO_OUTI(RPM_MPU_XPU_PRTn_START0, i, range.start_addr);
        HWIO_OUTI(RPM_MPU_XPU_PRTn_END0, i, range.end_addr);
        HWIO_OUTI(RPM_MPU_XPU_PRTn_TRANS_COMM, i, 1);
    }

    HWIO_OUT(RPM_MPU_XPU_UMR_RACR, translate_vmid_mask(dram_cfg.unmapped_read_vmid_mask));
    HWIO_OUT(RPM_MPU_XPU_UMR_WACR, translate_vmid_mask(dram_cfg.unmapped_write_vmid_mask));

    HWIO_OUT(RPM_MPU_XPU_CR,  0x11F);
}

static void msg_ram_xpu_setup(void)
{
    HWIO_OUTI(RPM_MSG_RAM_XPU_RPU_ACRn, 0, translate_vmid_mask(mram_cfg.super_users_vmid_mask));

    for(unsigned i = 0; i < ARRAY_SIZE(mram_cfg.perms); i++)
    {
        HWIO_OUTI2(RPM_MSG_RAM_XPU_RGn_RACRm, i, 0, perms_mask(&mram_cfg.perms[i]));
    }

    HWIO_OUT(RPM_MSG_RAM_XPU_CR,  0x11F);
}
#endif

void xpu_init(void)
{
#ifndef MSM8x26_STUBS
    code_ram_xpu_setup();
    data_ram_xpu_setup();
    msg_ram_xpu_setup();
#endif

    /* Send notification to SBL that XPU's are locked. Magic value from boot team. */
    //TODO
    //do we need this in new Bear Boot architecture
    //if needed, cross check the offset before writing the magic value.
    unsigned *cookie = (unsigned *)(SHARED_IMEM_BASE + 0x1C);
    *cookie = 0x112C3B1C;
}


