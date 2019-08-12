#ifndef __HAL_HALTLMMHWIO_H__
#define __HAL_HALTLMMHWIO_H__
/* ===========================================================================

              H A L   T L M M   H W I O   I N T E R F A C E

==============================================================================

 ADDRESS FILE VERSION: (unknown)

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved

==============================================================================
 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/tlmm/hw/v1/HALtlmmHWIO.h#1 $
=========================================================================== */

/*------------------------------------------------------------------------------
 * MODULE: tlmm
 *------------------------------------------------------------------------------*/


#define HWIO_TLMM_RPU_RGn_ACR_ADDR(nBase, n)                                         ((nBase)      + 00000000 + 0x4 * (n))
#define HWIO_TLMM_RPU_RGn_ACR_RMSK                                                 0x31f
#define HWIO_TLMM_RPU_RGn_ACR_MAXn                                                  0xac
#define HWIO_TLMM_RPU_RGn_ACR_INI(nBase,n) \
        in_dword(HWIO_TLMM_RPU_RGn_ACR_ADDR(nBase, n))
#define HWIO_TLMM_RPU_RGn_ACR_INMI(nBase,n,mask) \
        in_dword_masked(HWIO_TLMM_RPU_RGn_ACR_ADDR(nBase, n), mask)
#define HWIO_TLMM_RPU_RGn_ACR_OUTI(nBase,n,val) \
        out_dword(HWIO_TLMM_RPU_RGn_ACR_ADDR(nBase, n),val)
#define HWIO_TLMM_RPU_RGn_ACR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_RPU_RGn_ACR_ADDR(nBase, n),mask,val,HWIO_TLMM_RPU_RGn_ACR_INI(nBase,n));
#define HWIO_TLMM_RPU_RGn_ACR_RWGE_BMSK                                            0x200
#define HWIO_TLMM_RPU_RGn_ACR_RWGE_SHFT                                              0x9
#define HWIO_TLMM_RPU_RGn_ACR_RWE_BMSK                                             0x100
#define HWIO_TLMM_RPU_RGn_ACR_RWE_SHFT                                               0x8
#define HWIO_TLMM_RPU_RGn_ACR_RWVMID_BMSK                                           0x1f
#define HWIO_TLMM_RPU_RGn_ACR_RWVMID_SHFT                                              0

#define HWIO_TLMM_SPARE_WACR_ADDR(nBase)                                            ((nBase)      + 0x00000344)
#define HWIO_TLMM_SPARE_WACR_RMSK                                                  0x31f
#define HWIO_TLMM_SPARE_WACR_IN(nBase)                                              \
        in_dword_masked(HWIO_TLMM_SPARE_WACR_ADDR(nBase), HWIO_TLMM_SPARE_WACR_RMSK)
#define HWIO_TLMM_SPARE_WACR_INM(nBase,m)                                           \
        in_dword_masked(HWIO_TLMM_SPARE_WACR_ADDR(nBase), m)
#define HWIO_TLMM_SPARE_WACR_OUT(nBase,v)                                           \
        out_dword(HWIO_TLMM_SPARE_WACR_ADDR(nBase),v)
#define HWIO_TLMM_SPARE_WACR_OUTM(nBase,m,v)                                        \
        out_dword_masked_ns(HWIO_TLMM_SPARE_WACR_ADDR(nBase),m,v,HWIO_TLMM_SPARE_WACR_IN);
#define HWIO_TLMM_SPARE_WACR_RWGE_BMSK                                             0x200
#define HWIO_TLMM_SPARE_WACR_RWGE_SHFT                                               0x9
#define HWIO_TLMM_SPARE_WACR_RWE_BMSK                                              0x100
#define HWIO_TLMM_SPARE_WACR_RWE_SHFT                                                0x8
#define HWIO_TLMM_SPARE_WACR_RWVMID_BMSK                                            0x1f
#define HWIO_TLMM_SPARE_WACR_RWVMID_SHFT                                               0

#define HWIO_TLMM_SPARE_RACR_ADDR(nBase)                                             ((nBase)      + 0x00000348)
#define HWIO_TLMM_SPARE_RACR_RMSK                                                  0x31f
#define HWIO_TLMM_SPARE_RACR_IN(nBase)                                              \
        in_dword_masked(HWIO_TLMM_SPARE_RACR_ADDR(nBase), HWIO_TLMM_SPARE_RACR_RMSK)
#define HWIO_TLMM_SPARE_RACR_INM(nBase,m)                                           \
        in_dword_masked(HWIO_TLMM_SPARE_RACR_ADDR(nBase), m)
#define HWIO_TLMM_SPARE_RACR_OUT(nBase,v)                                           \
        out_dword(HWIO_TLMM_SPARE_RACR_ADDR(nBase),v)
#define HWIO_TLMM_SPARE_RACR_OUTM(nBase,m,v)                                        \
        out_dword_masked_ns(HWIO_TLMM_SPARE_RACR_ADDR(nBase),m,v,HWIO_TLMM_SPARE_RACR_IN);
#define HWIO_TLMM_SPARE_RACR_RGE_BMSK                                              0x200
#define HWIO_TLMM_SPARE_RACR_RGE_SHFT                                                0x9
#define HWIO_TLMM_SPARE_RACR_RE_BMSK                                               0x100
#define HWIO_TLMM_SPARE_RACR_RE_SHFT                                                 0x8
#define HWIO_TLMM_SPARE_RACR_RVMID_BMSK                                             0x1f
#define HWIO_TLMM_SPARE_RACR_RVMID_SHFT                                                0

#define HWIO_RGB_IF_SEL_WACR_ADDR(nBase)                                             ((nBase)      + 0x00000360)
#define HWIO_RGB_IF_SEL_WACR_RMSK                                                  0x31f
#define HWIO_RGB_IF_SEL_WACR_IN(nBase)                                              \
        in_dword_masked(HWIO_RGB_IF_SEL_WACR_ADDR(nBase), HWIO_RGB_IF_SEL_WACR_RMSK)
#define HWIO_RGB_IF_SEL_WACR_INM(nBase,m)                                           \
        in_dword_masked(HWIO_RGB_IF_SEL_WACR_ADDR(nBase), m)
#define HWIO_RGB_IF_SEL_WACR_OUT(nBase,v)                                           \
        out_dword(HWIO_RGB_IF_SEL_WACR_ADDR(nBase),v)
#define HWIO_RGB_IF_SEL_WACR_OUTM(nBase,m,v)                                        \
        out_dword_masked_ns(HWIO_RGB_IF_SEL_WACR_ADDR(nBase),m,v,HWIO_RGB_IF_SEL_WACR_IN);
#define HWIO_RGB_IF_SEL_WACR_RWGE_BMSK                                             0x200
#define HWIO_RGB_IF_SEL_WACR_RWGE_SHFT                                               0x9
#define HWIO_RGB_IF_SEL_WACR_RWE_BMSK                                              0x100
#define HWIO_RGB_IF_SEL_WACR_RWE_SHFT                                                0x8
#define HWIO_RGB_IF_SEL_WACR_RWVMID_BMSK                                            0x1f
#define HWIO_RGB_IF_SEL_WACR_RWVMID_SHFT                                               0

#define HWIO_RGB_IF_SEL_RACR_ADDR(nBase)                                             ((nBase)      + 0x00000364)
#define HWIO_RGB_IF_SEL_RACR_RMSK                                                  0x31f
#define HWIO_RGB_IF_SEL_RACR_IN(nBase)                                              \
        in_dword_masked(HWIO_RGB_IF_SEL_RACR_ADDR(nBase), HWIO_RGB_IF_SEL_RACR_RMSK)
#define HWIO_RGB_IF_SEL_RACR_INM(nBase,m)                                           \
        in_dword_masked(HWIO_RGB_IF_SEL_RACR_ADDR(nBase), m)
#define HWIO_RGB_IF_SEL_RACR_OUT(nBase,v)                                           \
        out_dword(HWIO_RGB_IF_SEL_RACR_ADDR(nBase),v)
#define HWIO_RGB_IF_SEL_RACR_OUTM(nBase,m,v)                                        \
        out_dword_masked_ns(HWIO_RGB_IF_SEL_RACR_ADDR(nBase),m,v,HWIO_RGB_IF_SEL_RACR_IN);
#define HWIO_RGB_IF_SEL_RACR_RGE_BMSK                                              0x200
#define HWIO_RGB_IF_SEL_RACR_RGE_SHFT                                                0x9
#define HWIO_RGB_IF_SEL_RACR_RE_BMSK                                               0x100
#define HWIO_RGB_IF_SEL_RACR_RE_SHFT                                                 0x8
#define HWIO_RGB_IF_SEL_RACR_RVMID_BMSK                                             0x1f
#define HWIO_RGB_IF_SEL_RACR_RVMID_SHFT                                                0

#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_ADDR(nBase)                                  ((nBase)      + 0x00000368)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RMSK                                       0x31f
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_IN(nBase)                                   \
        in_dword_masked(HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_ADDR(nBase), HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RMSK)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_INM(nBase,m)                                \
        in_dword_masked(HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_ADDR(nBase), m)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_OUT(nBase,v)                                \
        out_dword(HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_ADDR(nBase),v)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_ADDR(nBase),m,v,HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_IN);
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RWGE_BMSK                                  0x200
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RWGE_SHFT                                    0x9
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RWE_BMSK                                   0x100
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RWE_SHFT                                     0x8
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RWVMID_BMSK                                 0x1f
#define HWIO_GP_PDM0_GPIO_PORT_SEL_WACR_RWVMID_SHFT                                    0

#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_ADDR(nBase)                                  ((nBase)      + 0x0000036c)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RMSK                                       0x31f
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_IN(nBase)                                   \
        in_dword_masked(HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_ADDR(nBase), HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RMSK)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_INM(nBase,m)                                \
        in_dword_masked(HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_ADDR(nBase), m)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_OUT(nBase,v)                                \
        out_dword(HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_ADDR(nBase),v)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_ADDR(nBase),m,v,HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_IN);
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RGE_BMSK                                   0x200
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RGE_SHFT                                     0x9
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RE_BMSK                                    0x100
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RE_SHFT                                      0x8
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RVMID_BMSK                                  0x1f
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RACR_RVMID_SHFT                                     0

#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_ADDR(nBase)                                  ((nBase)      + 0x00000370)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RMSK                                       0x31f
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_IN(nBase)                                   \
        in_dword_masked(HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_ADDR(nBase), HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RMSK)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_INM(nBase,m)                                \
        in_dword_masked(HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_ADDR(nBase), m)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_OUT(nBase,v)                                \
        out_dword(HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_ADDR(nBase),v)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_ADDR(nBase),m,v,HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_IN);
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RWGE_BMSK                                  0x200
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RWGE_SHFT                                    0x9
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RWE_BMSK                                   0x100
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RWE_SHFT                                     0x8
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RWVMID_BMSK                                 0x1f
#define HWIO_GP_PDM1_GPIO_PORT_SEL_WACR_RWVMID_SHFT                                    0

#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_ADDR(nBase)                                  ((nBase)      + 0x00000374)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RMSK                                       0x31f
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_IN(nBase)                                   \
        in_dword_masked(HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_ADDR(nBase), HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RMSK)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_INM(nBase,m)                                \
        in_dword_masked(HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_ADDR(nBase), m)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_OUT(nBase,v)                                \
        out_dword(HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_ADDR(nBase),v)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_ADDR(nBase),m,v,HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_IN);
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RGE_BMSK                                   0x200
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RGE_SHFT                                     0x9
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RE_BMSK                                    0x100
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RE_SHFT                                      0x8
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RVMID_BMSK                                  0x1f
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RACR_RVMID_SHFT                                     0

#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_ADDR(nBase)                                  ((nBase)      + 0x00000378)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RMSK                                       0x31f
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_IN(nBase)                                   \
        in_dword_masked(HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_ADDR(nBase), HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RMSK)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_INM(nBase,m)                                \
        in_dword_masked(HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_ADDR(nBase), m)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_OUT(nBase,v)                                \
        out_dword(HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_ADDR(nBase),v)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_ADDR(nBase),m,v,HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_IN);
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RWGE_BMSK                                  0x200
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RWGE_SHFT                                    0x9
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RWE_BMSK                                   0x100
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RWE_SHFT                                     0x8
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RWVMID_BMSK                                 0x1f
#define HWIO_GP_PDM2_GPIO_PORT_SEL_WACR_RWVMID_SHFT                                    0

#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_ADDR(nBase)                                  ((nBase)      + 0x0000037c)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RMSK                                       0x31f
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_IN(nBase)                                   \
        in_dword_masked(HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_ADDR(nBase), HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RMSK)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_INM(nBase,m)                                \
        in_dword_masked(HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_ADDR(nBase), m)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_OUT(nBase,v)                                \
        out_dword(HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_ADDR(nBase),v)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_ADDR(nBase),m,v,HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_IN);
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RGE_BMSK                                   0x200
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RGE_SHFT                                     0x9
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RE_BMSK                                    0x100
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RE_SHFT                                      0x8
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RVMID_BMSK                                  0x1f
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RACR_RVMID_SHFT                                     0

#define HWIO_RESOUT_HDRV_CTL_WACR_ADDR(nBase)                                        ((nBase)      + 0x00000380)
#define HWIO_RESOUT_HDRV_CTL_WACR_RMSK                                             0x31f
#define HWIO_RESOUT_HDRV_CTL_WACR_IN(nBase)                                         \
        in_dword_masked(HWIO_RESOUT_HDRV_CTL_WACR_ADDR(nBase), HWIO_RESOUT_HDRV_CTL_WACR_RMSK)
#define HWIO_RESOUT_HDRV_CTL_WACR_INM(nBase,m)                                      \
        in_dword_masked(HWIO_RESOUT_HDRV_CTL_WACR_ADDR(nBase), m)
#define HWIO_RESOUT_HDRV_CTL_WACR_OUT(nBase,v)                                      \
        out_dword(HWIO_RESOUT_HDRV_CTL_WACR_ADDR(nBase),v)
#define HWIO_RESOUT_HDRV_CTL_WACR_OUTM(nBase,m,v)                                   \
        out_dword_masked_ns(HWIO_RESOUT_HDRV_CTL_WACR_ADDR(nBase),m,v,HWIO_RESOUT_HDRV_CTL_WACR_IN);
#define HWIO_RESOUT_HDRV_CTL_WACR_RWGE_BMSK                                        0x200
#define HWIO_RESOUT_HDRV_CTL_WACR_RWGE_SHFT                                          0x9
#define HWIO_RESOUT_HDRV_CTL_WACR_RWE_BMSK                                         0x100
#define HWIO_RESOUT_HDRV_CTL_WACR_RWE_SHFT                                           0x8
#define HWIO_RESOUT_HDRV_CTL_WACR_RWVMID_BMSK                                       0x1f
#define HWIO_RESOUT_HDRV_CTL_WACR_RWVMID_SHFT                                          0

#define HWIO_RESOUT_HDRV_CTL_RACR_ADDR(nBase)                                        ((nBase)      + 0x00000384)
#define HWIO_RESOUT_HDRV_CTL_RACR_RMSK                                             0x31f
#define HWIO_RESOUT_HDRV_CTL_RACR_IN(nBase)                                         \
        in_dword_masked(HWIO_RESOUT_HDRV_CTL_RACR_ADDR(nBase), HWIO_RESOUT_HDRV_CTL_RACR_RMSK)
#define HWIO_RESOUT_HDRV_CTL_RACR_INM(nBase,m)                                      \
        in_dword_masked(HWIO_RESOUT_HDRV_CTL_RACR_ADDR(nBase), m)
#define HWIO_RESOUT_HDRV_CTL_RACR_OUT(nBase,v)                                      \
        out_dword(HWIO_RESOUT_HDRV_CTL_RACR_ADDR(nBase),v)
#define HWIO_RESOUT_HDRV_CTL_RACR_OUTM(nBase,m,v)                                   \
        out_dword_masked_ns(HWIO_RESOUT_HDRV_CTL_RACR_ADDR(nBase),m,v,HWIO_RESOUT_HDRV_CTL_RACR_IN);
#define HWIO_RESOUT_HDRV_CTL_RACR_RGE_BMSK                                         0x200
#define HWIO_RESOUT_HDRV_CTL_RACR_RGE_SHFT                                           0x9
#define HWIO_RESOUT_HDRV_CTL_RACR_RE_BMSK                                          0x100
#define HWIO_RESOUT_HDRV_CTL_RACR_RE_SHFT                                            0x8
#define HWIO_RESOUT_HDRV_CTL_RACR_RVMID_BMSK                                        0x1f
#define HWIO_RESOUT_HDRV_CTL_RACR_RVMID_SHFT                                           0

#define HWIO_JTAG_HDRV_CTL_WACR_ADDR(nBase)                                          ((nBase)      + 0x00000388)
#define HWIO_JTAG_HDRV_CTL_WACR_RMSK                                               0x31f
#define HWIO_JTAG_HDRV_CTL_WACR_IN(nBase)                                           \
        in_dword_masked(HWIO_JTAG_HDRV_CTL_WACR_ADDR(nBase), HWIO_JTAG_HDRV_CTL_WACR_RMSK)
#define HWIO_JTAG_HDRV_CTL_WACR_INM(nBase,m)                                        \
        in_dword_masked(HWIO_JTAG_HDRV_CTL_WACR_ADDR(nBase), m)
#define HWIO_JTAG_HDRV_CTL_WACR_OUT(nBase,v)                                        \
        out_dword(HWIO_JTAG_HDRV_CTL_WACR_ADDR(nBase),v)
#define HWIO_JTAG_HDRV_CTL_WACR_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_JTAG_HDRV_CTL_WACR_ADDR(nBase),m,v,HWIO_JTAG_HDRV_CTL_WACR_IN);
#define HWIO_JTAG_HDRV_CTL_WACR_RWGE_BMSK                                          0x200
#define HWIO_JTAG_HDRV_CTL_WACR_RWGE_SHFT                                            0x9
#define HWIO_JTAG_HDRV_CTL_WACR_RWE_BMSK                                           0x100
#define HWIO_JTAG_HDRV_CTL_WACR_RWE_SHFT                                             0x8
#define HWIO_JTAG_HDRV_CTL_WACR_RWVMID_BMSK                                         0x1f
#define HWIO_JTAG_HDRV_CTL_WACR_RWVMID_SHFT                                            0

#define HWIO_JTAG_HDRV_CTL_RACR_ADDR(nBase)                                          ((nBase)      + 0x0000038c)
#define HWIO_JTAG_HDRV_CTL_RACR_RMSK                                               0x31f
#define HWIO_JTAG_HDRV_CTL_RACR_IN(nBase)                                           \
        in_dword_masked(HWIO_JTAG_HDRV_CTL_RACR_ADDR(nBase), HWIO_JTAG_HDRV_CTL_RACR_RMSK)
#define HWIO_JTAG_HDRV_CTL_RACR_INM(nBase,m)                                        \
        in_dword_masked(HWIO_JTAG_HDRV_CTL_RACR_ADDR(nBase), m)
#define HWIO_JTAG_HDRV_CTL_RACR_OUT(nBase,v)                                        \
        out_dword(HWIO_JTAG_HDRV_CTL_RACR_ADDR(nBase),v)
#define HWIO_JTAG_HDRV_CTL_RACR_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_JTAG_HDRV_CTL_RACR_ADDR(nBase),m,v,HWIO_JTAG_HDRV_CTL_RACR_IN);
#define HWIO_JTAG_HDRV_CTL_RACR_RGE_BMSK                                           0x200
#define HWIO_JTAG_HDRV_CTL_RACR_RGE_SHFT                                             0x9
#define HWIO_JTAG_HDRV_CTL_RACR_RE_BMSK                                            0x100
#define HWIO_JTAG_HDRV_CTL_RACR_RE_SHFT                                              0x8
#define HWIO_JTAG_HDRV_CTL_RACR_RVMID_BMSK                                          0x1f
#define HWIO_JTAG_HDRV_CTL_RACR_RVMID_SHFT                                             0

#define HWIO_PMIC_HDRV_PULL_CTL_WACR_ADDR(nBase)                                     ((nBase)      + 0x00000390)
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RMSK                                          0x31f
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_IN(nBase)                                      \
        in_dword_masked(HWIO_PMIC_HDRV_PULL_CTL_WACR_ADDR(nBase), HWIO_PMIC_HDRV_PULL_CTL_WACR_RMSK)
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_INM(nBase,m)                                   \
        in_dword_masked(HWIO_PMIC_HDRV_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_OUT(nBase,v)                                   \
        out_dword(HWIO_PMIC_HDRV_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_PMIC_HDRV_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_PMIC_HDRV_PULL_CTL_WACR_IN);
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RWGE_BMSK                                     0x200
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RWGE_SHFT                                       0x9
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RWE_BMSK                                      0x100
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RWE_SHFT                                        0x8
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RWVMID_BMSK                                    0x1f
#define HWIO_PMIC_HDRV_PULL_CTL_WACR_RWVMID_SHFT                                       0

#define HWIO_PMIC_HDRV_PULL_CTL_RACR_ADDR(nBase)                                     ((nBase)      + 0x00000394)
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RMSK                                          0x31f
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_IN(nBase)                                      \
        in_dword_masked(HWIO_PMIC_HDRV_PULL_CTL_RACR_ADDR(nBase), HWIO_PMIC_HDRV_PULL_CTL_RACR_RMSK)
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_INM(nBase,m)                                   \
        in_dword_masked(HWIO_PMIC_HDRV_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_OUT(nBase,v)                                   \
        out_dword(HWIO_PMIC_HDRV_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_PMIC_HDRV_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_PMIC_HDRV_PULL_CTL_RACR_IN);
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RGE_BMSK                                      0x200
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RGE_SHFT                                        0x9
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RE_BMSK                                       0x100
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RE_SHFT                                         0x8
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RVMID_BMSK                                     0x1f
#define HWIO_PMIC_HDRV_PULL_CTL_RACR_RVMID_SHFT                                        0

#define HWIO_RF_HDRV_PULL_CTL_WACR_ADDR(nBase)                                       ((nBase)      + 0x00000398)
#define HWIO_RF_HDRV_PULL_CTL_WACR_RMSK                                            0x31f
#define HWIO_RF_HDRV_PULL_CTL_WACR_IN(nBase)                                        \
        in_dword_masked(HWIO_RF_HDRV_PULL_CTL_WACR_ADDR(nBase), HWIO_RF_HDRV_PULL_CTL_WACR_RMSK)
#define HWIO_RF_HDRV_PULL_CTL_WACR_INM(nBase,m)                                     \
        in_dword_masked(HWIO_RF_HDRV_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_RF_HDRV_PULL_CTL_WACR_OUT(nBase,v)                                     \
        out_dword(HWIO_RF_HDRV_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_RF_HDRV_PULL_CTL_WACR_OUTM(nBase,m,v)                                  \
        out_dword_masked_ns(HWIO_RF_HDRV_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_RF_HDRV_PULL_CTL_WACR_IN);
#define HWIO_RF_HDRV_PULL_CTL_WACR_RWGE_BMSK                                       0x200
#define HWIO_RF_HDRV_PULL_CTL_WACR_RWGE_SHFT                                         0x9
#define HWIO_RF_HDRV_PULL_CTL_WACR_RWE_BMSK                                        0x100
#define HWIO_RF_HDRV_PULL_CTL_WACR_RWE_SHFT                                          0x8
#define HWIO_RF_HDRV_PULL_CTL_WACR_RWVMID_BMSK                                      0x1f
#define HWIO_RF_HDRV_PULL_CTL_WACR_RWVMID_SHFT                                         0

#define HWIO_RF_HDRV_PULL_CTL_RACR_ADDR(nBase)                                       ((nBase)      + 0x0000039c)
#define HWIO_RF_HDRV_PULL_CTL_RACR_RMSK                                            0x31f
#define HWIO_RF_HDRV_PULL_CTL_RACR_IN(nBase)                                        \
        in_dword_masked(HWIO_RF_HDRV_PULL_CTL_RACR_ADDR(nBase), HWIO_RF_HDRV_PULL_CTL_RACR_RMSK)
#define HWIO_RF_HDRV_PULL_CTL_RACR_INM(nBase,m)                                     \
        in_dword_masked(HWIO_RF_HDRV_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_RF_HDRV_PULL_CTL_RACR_OUT(nBase,v)                                     \
        out_dword(HWIO_RF_HDRV_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_RF_HDRV_PULL_CTL_RACR_OUTM(nBase,m,v)                                  \
        out_dword_masked_ns(HWIO_RF_HDRV_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_RF_HDRV_PULL_CTL_RACR_IN);
#define HWIO_RF_HDRV_PULL_CTL_RACR_RGE_BMSK                                        0x200
#define HWIO_RF_HDRV_PULL_CTL_RACR_RGE_SHFT                                          0x9
#define HWIO_RF_HDRV_PULL_CTL_RACR_RE_BMSK                                         0x100
#define HWIO_RF_HDRV_PULL_CTL_RACR_RE_SHFT                                           0x8
#define HWIO_RF_HDRV_PULL_CTL_RACR_RVMID_BMSK                                       0x1f
#define HWIO_RF_HDRV_PULL_CTL_RACR_RVMID_SHFT                                          0

#define HWIO_SDC4_HDRV_PULL_CTL_WACR_ADDR(nBase)                                     ((nBase)      + 0x000003b8)
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RMSK                                          0x31f
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_IN(nBase)                                      \
        in_dword_masked(HWIO_SDC4_HDRV_PULL_CTL_WACR_ADDR(nBase), HWIO_SDC4_HDRV_PULL_CTL_WACR_RMSK)
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_INM(nBase,m)                                   \
        in_dword_masked(HWIO_SDC4_HDRV_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_OUT(nBase,v)                                   \
        out_dword(HWIO_SDC4_HDRV_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_SDC4_HDRV_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_SDC4_HDRV_PULL_CTL_WACR_IN);
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RWGE_BMSK                                     0x200
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RWGE_SHFT                                       0x9
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RWE_BMSK                                      0x100
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RWE_SHFT                                        0x8
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RWVMID_BMSK                                    0x1f
#define HWIO_SDC4_HDRV_PULL_CTL_WACR_RWVMID_SHFT                                       0

#define HWIO_SDC4_HDRV_PULL_CTL_RACR_ADDR(nBase)                                     ((nBase)      + 0x000003bc)
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RMSK                                          0x31f
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_IN(nBase)                                      \
        in_dword_masked(HWIO_SDC4_HDRV_PULL_CTL_RACR_ADDR(nBase), HWIO_SDC4_HDRV_PULL_CTL_RACR_RMSK)
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_INM(nBase,m)                                   \
        in_dword_masked(HWIO_SDC4_HDRV_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_OUT(nBase,v)                                   \
        out_dword(HWIO_SDC4_HDRV_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_SDC4_HDRV_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_SDC4_HDRV_PULL_CTL_RACR_IN);
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RGE_BMSK                                      0x200
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RGE_SHFT                                        0x9
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RE_BMSK                                       0x100
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RE_SHFT                                         0x8
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RVMID_BMSK                                     0x1f
#define HWIO_SDC4_HDRV_PULL_CTL_RACR_RVMID_SHFT                                        0

#define HWIO_SDC3_HDRV_PULL_CTL_WACR_ADDR(nBase)                                     ((nBase)      + 0x000003c0)
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RMSK                                          0x31f
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_IN(nBase)                                      \
        in_dword_masked(HWIO_SDC3_HDRV_PULL_CTL_WACR_ADDR(nBase), HWIO_SDC3_HDRV_PULL_CTL_WACR_RMSK)
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_INM(nBase,m)                                   \
        in_dword_masked(HWIO_SDC3_HDRV_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_OUT(nBase,v)                                   \
        out_dword(HWIO_SDC3_HDRV_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_SDC3_HDRV_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_SDC3_HDRV_PULL_CTL_WACR_IN);
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RWGE_BMSK                                     0x200
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RWGE_SHFT                                       0x9
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RWE_BMSK                                      0x100
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RWE_SHFT                                        0x8
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RWVMID_BMSK                                    0x1f
#define HWIO_SDC3_HDRV_PULL_CTL_WACR_RWVMID_SHFT                                       0

#define HWIO_SDC3_HDRV_PULL_CTL_RACR_ADDR(nBase)                                     ((nBase)      + 0x000003c4)
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RMSK                                          0x31f
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_IN(nBase)                                      \
        in_dword_masked(HWIO_SDC3_HDRV_PULL_CTL_RACR_ADDR(nBase), HWIO_SDC3_HDRV_PULL_CTL_RACR_RMSK)
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_INM(nBase,m)                                   \
        in_dword_masked(HWIO_SDC3_HDRV_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_OUT(nBase,v)                                   \
        out_dword(HWIO_SDC3_HDRV_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_SDC3_HDRV_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_SDC3_HDRV_PULL_CTL_RACR_IN);
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RGE_BMSK                                      0x200
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RGE_SHFT                                        0x9
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RE_BMSK                                       0x100
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RE_SHFT                                         0x8
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RVMID_BMSK                                     0x1f
#define HWIO_SDC3_HDRV_PULL_CTL_RACR_RVMID_SHFT                                        0

#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_ADDR(nBase)                               ((nBase)      + 0x000003c8)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RMSK                                    0x31f
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_IN(nBase)                                \
        in_dword_masked(HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_ADDR(nBase), HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RMSK)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_INM(nBase,m)                             \
        in_dword_masked(HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_OUT(nBase,v)                             \
        out_dword(HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_OUTM(nBase,m,v)                          \
        out_dword_masked_ns(HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_IN);
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RWGE_BMSK                               0x200
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RWGE_SHFT                                 0x9
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RWE_BMSK                                0x100
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RWE_SHFT                                  0x8
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RWVMID_BMSK                              0x1f
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_WACR_RWVMID_SHFT                                 0

#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_ADDR(nBase)                               ((nBase)      + 0x000003cc)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RMSK                                    0x31f
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_IN(nBase)                                \
        in_dword_masked(HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_ADDR(nBase), HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RMSK)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_INM(nBase,m)                             \
        in_dword_masked(HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_OUT(nBase,v)                             \
        out_dword(HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_OUTM(nBase,m,v)                          \
        out_dword_masked_ns(HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_IN);
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RGE_BMSK                                0x200
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RGE_SHFT                                  0x9
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RE_BMSK                                 0x100
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RE_SHFT                                   0x8
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RVMID_BMSK                               0x1f
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RACR_RVMID_SHFT                                  0

#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_ADDR(nBase)                                ((nBase)      + 0x000003d0)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RMSK                                     0x31f
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_IN(nBase)                                 \
        in_dword_masked(HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_ADDR(nBase), HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RMSK)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_INM(nBase,m)                              \
        in_dword_masked(HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_OUT(nBase,v)                              \
        out_dword(HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_OUTM(nBase,m,v)                           \
        out_dword_masked_ns(HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_IN);
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RWGE_BMSK                                0x200
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RWGE_SHFT                                  0x9
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RWE_BMSK                                 0x100
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RWE_SHFT                                   0x8
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RWVMID_BMSK                               0x1f
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_WACR_RWVMID_SHFT                                  0

#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_ADDR(nBase)                                ((nBase)      + 0x000003d4)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RMSK                                     0x31f
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_IN(nBase)                                 \
        in_dword_masked(HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_ADDR(nBase), HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RMSK)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_INM(nBase,m)                              \
        in_dword_masked(HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_OUT(nBase,v)                              \
        out_dword(HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_OUTM(nBase,m,v)                           \
        out_dword_masked_ns(HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_IN);
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RGE_BMSK                                 0x200
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RGE_SHFT                                   0x9
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RE_BMSK                                  0x100
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RE_SHFT                                    0x8
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RVMID_BMSK                                0x1f
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RACR_RVMID_SHFT                                   0

#define HWIO_MODE_PULL_CTL_WACR_ADDR(nBase)                                          ((nBase)      + 0x000003d8)
#define HWIO_MODE_PULL_CTL_WACR_RMSK                                               0x31f
#define HWIO_MODE_PULL_CTL_WACR_IN(nBase)                                           \
        in_dword_masked(HWIO_MODE_PULL_CTL_WACR_ADDR(nBase), HWIO_MODE_PULL_CTL_WACR_RMSK)
#define HWIO_MODE_PULL_CTL_WACR_INM(nBase,m)                                        \
        in_dword_masked(HWIO_MODE_PULL_CTL_WACR_ADDR(nBase), m)
#define HWIO_MODE_PULL_CTL_WACR_OUT(nBase,v)                                        \
        out_dword(HWIO_MODE_PULL_CTL_WACR_ADDR(nBase),v)
#define HWIO_MODE_PULL_CTL_WACR_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_MODE_PULL_CTL_WACR_ADDR(nBase),m,v,HWIO_MODE_PULL_CTL_WACR_IN);
#define HWIO_MODE_PULL_CTL_WACR_RWGE_BMSK                                          0x200
#define HWIO_MODE_PULL_CTL_WACR_RWGE_SHFT                                            0x9
#define HWIO_MODE_PULL_CTL_WACR_RWE_BMSK                                           0x100
#define HWIO_MODE_PULL_CTL_WACR_RWE_SHFT                                             0x8
#define HWIO_MODE_PULL_CTL_WACR_RWVMID_BMSK                                         0x1f
#define HWIO_MODE_PULL_CTL_WACR_RWVMID_SHFT                                            0

#define HWIO_MODE_PULL_CTL_RACR_ADDR(nBase)                                          ((nBase)      + 0x000003dc)
#define HWIO_MODE_PULL_CTL_RACR_RMSK                                               0x31f
#define HWIO_MODE_PULL_CTL_RACR_IN(nBase)                                           \
        in_dword_masked(HWIO_MODE_PULL_CTL_RACR_ADDR(nBase), HWIO_MODE_PULL_CTL_RACR_RMSK)
#define HWIO_MODE_PULL_CTL_RACR_INM(nBase,m)                                        \
        in_dword_masked(HWIO_MODE_PULL_CTL_RACR_ADDR(nBase), m)
#define HWIO_MODE_PULL_CTL_RACR_OUT(nBase,v)                                        \
        out_dword(HWIO_MODE_PULL_CTL_RACR_ADDR(nBase),v)
#define HWIO_MODE_PULL_CTL_RACR_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_MODE_PULL_CTL_RACR_ADDR(nBase),m,v,HWIO_MODE_PULL_CTL_RACR_IN);
#define HWIO_MODE_PULL_CTL_RACR_RGE_BMSK                                           0x200
#define HWIO_MODE_PULL_CTL_RACR_RGE_SHFT                                             0x9
#define HWIO_MODE_PULL_CTL_RACR_RE_BMSK                                            0x100
#define HWIO_MODE_PULL_CTL_RACR_RE_SHFT                                              0x8
#define HWIO_MODE_PULL_CTL_RACR_RVMID_BMSK                                          0x1f
#define HWIO_MODE_PULL_CTL_RACR_RVMID_SHFT                                             0

#define HWIO_GPIO_INTR_CFG_SUn_ADDR(nBase, n)                                        ((nBase)      + 0x00000400 + 0x4 * (n))
#define HWIO_GPIO_INTR_CFG_SUn_RMSK                                                  0xf
#define HWIO_GPIO_INTR_CFG_SUn_MAXn                                                 0xac
#define HWIO_GPIO_INTR_CFG_SUn_INI(nBase,n) \
        in_dword(HWIO_GPIO_INTR_CFG_SUn_ADDR(nBase, n))
#define HWIO_GPIO_INTR_CFG_SUn_INMI(nBase,n,mask) \
        in_dword_masked(HWIO_GPIO_INTR_CFG_SUn_ADDR(nBase, n), mask)
#define HWIO_GPIO_INTR_CFG_SUn_OUTI(nBase,n,val) \
        out_dword(HWIO_GPIO_INTR_CFG_SUn_ADDR(nBase, n),val)
#define HWIO_GPIO_INTR_CFG_SUn_OUTMI(nBase,n,mask,val) \
        out_dword_masked_ns(HWIO_GPIO_INTR_CFG_SUn_ADDR(nBase, n),mask,val,HWIO_GPIO_INTR_CFG_SUn_INI(nBase,n));
#define HWIO_GPIO_INTR_CFG_SUn_DIR_CONN_EN_BMSK                                      0x8
#define HWIO_GPIO_INTR_CFG_SUn_DIR_CONN_EN_SHFT                                      0x3
#define HWIO_GPIO_INTR_CFG_SUn_TARGET_PROC_BMSK                                      0x7
#define HWIO_GPIO_INTR_CFG_SUn_TARGET_PROC_SHFT                                        0

#define HWIO_DIR_CONN_INTR_CFG_SUn_ADDR(nBase, n)                                    ((nBase)      + 0x00000700 + 0x4 * (n))
#define HWIO_DIR_CONN_INTR_CFG_SUn_RMSK                                            0xfff
#define HWIO_DIR_CONN_INTR_CFG_SUn_MAXn                                              0xf
#define HWIO_DIR_CONN_INTR_CFG_SUn_INI(nBase,n) \
        in_dword(HWIO_DIR_CONN_INTR_CFG_SUn_ADDR(nBase, n))
#define HWIO_DIR_CONN_INTR_CFG_SUn_INMI(nBase,n,mask) \
        in_dword_masked(HWIO_DIR_CONN_INTR_CFG_SUn_ADDR(nBase, n), mask)
#define HWIO_DIR_CONN_INTR_CFG_SUn_OUTI(nBase,n,val) \
        out_dword(HWIO_DIR_CONN_INTR_CFG_SUn_ADDR(nBase, n),val)
#define HWIO_DIR_CONN_INTR_CFG_SUn_OUTMI(nBase,n,mask,val) \
        out_dword_masked_ns(HWIO_DIR_CONN_INTR_CFG_SUn_ADDR(nBase, n),mask,val,HWIO_DIR_CONN_INTR_CFG_SUn_INI(nBase,n));
#define HWIO_DIR_CONN_INTR_CFG_SUn_POLARITY_BMSK                                   0x800
#define HWIO_DIR_CONN_INTR_CFG_SUn_POLARITY_SHFT                                     0xb
#define HWIO_DIR_CONN_INTR_CFG_SUn_GPIO_SEL_BMSK                                   0x7f8
#define HWIO_DIR_CONN_INTR_CFG_SUn_GPIO_SEL_SHFT                                     0x3
#define HWIO_DIR_CONN_INTR_CFG_SUn_TARGET_PROC_BMSK                                  0x7
#define HWIO_DIR_CONN_INTR_CFG_SUn_TARGET_PROC_SHFT                                    0

#define HWIO_TLMM_CLK_GATE_EN_SU_ADDR(nBase)                                         ((nBase)      + 0x00000800)
#define HWIO_TLMM_CLK_GATE_EN_SU_RMSK                                               0x3f
#define HWIO_TLMM_CLK_GATE_EN_SU_IN(nBase)                                          \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_SU_ADDR(nBase), HWIO_TLMM_CLK_GATE_EN_SU_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_SU_INM(nBase,m)                                       \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_SU_ADDR(nBase), m)
#define HWIO_TLMM_CLK_GATE_EN_SU_OUT(nBase,v)                                       \
        out_dword(HWIO_TLMM_CLK_GATE_EN_SU_ADDR(nBase),v)
#define HWIO_TLMM_CLK_GATE_EN_SU_OUTM(nBase,m,v)                                    \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_SU_ADDR(nBase),m,v,HWIO_TLMM_CLK_GATE_EN_SU_IN);
#define HWIO_TLMM_CLK_GATE_EN_SU_AHB_HCLK_EN_BMSK                                   0x20
#define HWIO_TLMM_CLK_GATE_EN_SU_AHB_HCLK_EN_SHFT                                    0x5
#define HWIO_TLMM_CLK_GATE_EN_SU_DBG_STATUS_EN_BMSK                                 0x10
#define HWIO_TLMM_CLK_GATE_EN_SU_DBG_STATUS_EN_SHFT                                  0x4
#define HWIO_TLMM_CLK_GATE_EN_SU_CRCI_EN_BMSK                                        0x8
#define HWIO_TLMM_CLK_GATE_EN_SU_CRCI_EN_SHFT                                        0x3
#define HWIO_TLMM_CLK_GATE_EN_SU_JTAG_XTRIG_EN_BMSK                                  0x4
#define HWIO_TLMM_CLK_GATE_EN_SU_JTAG_XTRIG_EN_SHFT                                  0x2
#define HWIO_TLMM_CLK_GATE_EN_SU_SUMMARY_INTR_EN_BMSK                                0x2
#define HWIO_TLMM_CLK_GATE_EN_SU_SUMMARY_INTR_EN_SHFT                                0x1
#define HWIO_TLMM_CLK_GATE_EN_SU_CRIF_READ_EN_BMSK                                   0x1
#define HWIO_TLMM_CLK_GATE_EN_SU_CRIF_READ_EN_SHFT                                     0

#define HWIO_TLMM_IE_CTRL_DISABLE_SU_ADDR(nBase)                                     ((nBase)      + 0x00000810)
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_RMSK                                            0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_IN(nBase)                                      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_SU_ADDR(nBase), HWIO_TLMM_IE_CTRL_DISABLE_SU_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_INM(nBase,m)                                   \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_SU_ADDR(nBase), m)
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_OUT(nBase,v)                                   \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_SU_ADDR(nBase),v)
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_SU_ADDR(nBase),m,v,HWIO_TLMM_IE_CTRL_DISABLE_SU_IN);
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_IE_CTRL_DISABLE_BMSK                            0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_SU_IE_CTRL_DISABLE_SHFT                              0

#define HWIO_PSHOLD_CTL_SU_ADDR(nBase)                                               ((nBase)      + 0x00000820)
#define HWIO_PSHOLD_CTL_SU_RMSK                                                      0x1
#define HWIO_PSHOLD_CTL_SU_IN(nBase)                                                \
        in_dword_masked(HWIO_PSHOLD_CTL_SU_ADDR(nBase), HWIO_PSHOLD_CTL_SU_RMSK)
#define HWIO_PSHOLD_CTL_SU_INM(nBase,m)                                             \
        in_dword_masked(HWIO_PSHOLD_CTL_SU_ADDR(nBase), m)
#define HWIO_PSHOLD_CTL_SU_OUT(nBase,v)                                             \
        out_dword(HWIO_PSHOLD_CTL_SU_ADDR(nBase),v)
#define HWIO_PSHOLD_CTL_SU_OUTM(nBase,m,v)                                          \
        out_dword_masked_ns(HWIO_PSHOLD_CTL_SU_ADDR(nBase),m,v,HWIO_PSHOLD_CTL_SU_IN);
#define HWIO_PSHOLD_CTL_SU_PSHOLD_VALUE_BMSK                                         0x1
#define HWIO_PSHOLD_CTL_SU_PSHOLD_VALUE_SHFT                                           0

#define HWIO_CDC_CLK_TEST_SEL_SU_ADDR(nBase)                                         ((nBase)      + 0x00000830)
#define HWIO_CDC_CLK_TEST_SEL_SU_RMSK                                             0xffff
#define HWIO_CDC_CLK_TEST_SEL_SU_IN(nBase)                                          \
        in_dword_masked(HWIO_CDC_CLK_TEST_SEL_SU_ADDR(nBase), HWIO_CDC_CLK_TEST_SEL_SU_RMSK)
#define HWIO_CDC_CLK_TEST_SEL_SU_INM(nBase,m)                                       \
        in_dword_masked(HWIO_CDC_CLK_TEST_SEL_SU_ADDR(nBase), m)
#define HWIO_CDC_CLK_TEST_SEL_SU_OUT(nBase,v)                                       \
        out_dword(HWIO_CDC_CLK_TEST_SEL_SU_ADDR(nBase),v)
#define HWIO_CDC_CLK_TEST_SEL_SU_OUTM(nBase,m,v)                                    \
        out_dword_masked_ns(HWIO_CDC_CLK_TEST_SEL_SU_ADDR(nBase),m,v,HWIO_CDC_CLK_TEST_SEL_SU_IN);
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_3_BMSK                                    0x8000
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_3_SHFT                                       0xf
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_2_BMSK                                    0x4000
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_2_SHFT                                       0xe
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_1_BMSK                                    0x2000
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_1_SHFT                                       0xd
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_0_BMSK                                    0x1000
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CA_0_SHFT                                       0xc
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CK_BMSK                                       0x800
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CK_SHFT                                         0xb
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CS_N_0_BMSK                                   0x400
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CS_N_0_SHFT                                     0xa
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_RAS_N_BMSK                                    0x200
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_RAS_N_SHFT                                      0x9
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CAS_N_BMSK                                    0x100
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CAS_N_SHFT                                      0x8
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_WE_N_BMSK                                      0x80
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_WE_N_SHFT                                       0x7
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CKE_0_BMSK                                     0x40
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_CKE_0_SHFT                                      0x6
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_OE_BMSK                                        0x20
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_OE_SHFT                                         0x5
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_IE_BMSK                                        0x10
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_IE_SHFT                                         0x4
#define HWIO_CDC_CLK_TEST_SEL_SU_CDC_TEST_ENA_SLV_BMSK                               0xc
#define HWIO_CDC_CLK_TEST_SEL_SU_CDC_TEST_ENA_SLV_SHFT                               0x2
#define HWIO_CDC_CLK_TEST_SEL_SU_CDC_OSC_TEST_EN_BMSK                                0x2
#define HWIO_CDC_CLK_TEST_SEL_SU_CDC_OSC_TEST_EN_SHFT                                0x1
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_MODE_BMSK                                       0x1
#define HWIO_CDC_CLK_TEST_SEL_SU_SMT_MODE_SHFT                                         0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_ADDR(nBase)                                 ((nBase)      + 0x00000840)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_RMSK                                  0xfffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_IN(nBase)                                  \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_ADDR(nBase), HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_INM(nBase,m)                               \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_ADDR(nBase), m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_OUT(nBase,v)                               \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_ADDR(nBase),v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_OUTM(nBase,m,v)                            \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_ADDR(nBase),m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_IN);
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_162_BMSK                         0x8000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_162_SHFT                              0x1b
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_160_BMSK                         0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_160_SHFT                              0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_146_BMSK                         0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_146_SHFT                              0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_144_BMSK                         0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_144_SHFT                              0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_117_BMSK                          0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_117_SHFT                              0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_105_BMSK                          0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_105_SHFT                              0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_99_BMSK                           0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_99_SHFT                               0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_96_BMSK                           0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_96_SHFT                               0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_91_BMSK                            0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_91_SHFT                               0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_90_BMSK                            0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_90_SHFT                               0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_89_BMSK                            0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_89_SHFT                               0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_88_BMSK                            0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_88_SHFT                               0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_87_BMSK                             0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_87_SHFT                                0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_71_BMSK                             0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_71_SHFT                                0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_67_BMSK                             0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_67_SHFT                                0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_63_BMSK                             0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_63_SHFT                                0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_61_BMSK                              0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_61_SHFT                                0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_58_BMSK                              0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_58_SHFT                                0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_54_BMSK                              0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_54_SHFT                                0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_50_BMSK                              0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_50_SHFT                                0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_46_BMSK                               0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_46_SHFT                                0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_42_BMSK                               0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_42_SHFT                                0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_38_BMSK                               0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_38_SHFT                                0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_34_BMSK                               0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_GPIO_34_SHFT                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC3_DATA_3_BMSK                            0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC3_DATA_3_SHFT                            0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC3_DATA_1_BMSK                            0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC3_DATA_1_SHFT                            0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC4_DATA_3_BMSK                            0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC4_DATA_3_SHFT                            0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC4_DATA_1_BMSK                            0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SU_SDC4_DATA_1_SHFT                              0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_ADDR(nBase)                                 ((nBase)      + 0x00000844)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_RMSK                                  0x3ffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_IN(nBase)                                  \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_ADDR(nBase), HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_INM(nBase,m)                               \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_ADDR(nBase), m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_OUT(nBase,v)                               \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_ADDR(nBase),v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_OUTM(nBase,m,v)                            \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_ADDR(nBase),m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_IN);
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_129_BMSK                         0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_129_SHFT                              0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_57_BMSK                          0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_57_SHFT                               0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_172_BMSK                          0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_172_SHFT                              0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_128_BMSK                          0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_128_SHFT                              0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_127_BMSK                          0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_127_SHFT                              0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_126_BMSK                          0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_126_SHFT                              0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_125_BMSK                           0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_125_SHFT                              0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_124_BMSK                           0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_124_SHFT                              0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_123_BMSK                           0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_123_SHFT                              0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_115_BMSK                           0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_115_SHFT                              0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_106_BMSK                            0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_106_SHFT                               0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_104_BMSK                            0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_104_SHFT                               0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_103_BMSK                            0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_103_SHFT                               0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_94_BMSK                             0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_94_SHFT                                0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_93_BMSK                              0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_93_SHFT                                0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_73_BMSK                              0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_73_SHFT                                0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_52_BMSK                              0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_52_SHFT                                0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_51_BMSK                              0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_51_SHFT                                0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_45_BMSK                               0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_45_SHFT                                0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_41_BMSK                               0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_41_SHFT                                0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_40_BMSK                               0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_40_SHFT                                0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_37_BMSK                               0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_37_SHFT                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_31_BMSK                                0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_31_SHFT                                0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_30_BMSK                                0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_30_SHFT                                0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_29_BMSK                                0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_GPIO_29_SHFT                                0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_SRST_N_BMSK                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SU_SRST_N_SHFT                                   0

#define HWIO_TLMM_RPU_CR_ADDR(nBase)                                                 ((nBase)      + 0x00000f80)
#define HWIO_TLMM_RPU_CR_RMSK                                                        0xf
#define HWIO_TLMM_RPU_CR_IN(nBase)                                                  \
        in_dword_masked(HWIO_TLMM_RPU_CR_ADDR(nBase), HWIO_TLMM_RPU_CR_RMSK)
#define HWIO_TLMM_RPU_CR_INM(nBase,m)                                               \
        in_dword_masked(HWIO_TLMM_RPU_CR_ADDR(nBase), m)
#define HWIO_TLMM_RPU_CR_OUT(nBase,v)                                               \
        out_dword(HWIO_TLMM_RPU_CR_ADDR(nBase),v)
#define HWIO_TLMM_RPU_CR_OUTM(nBase,m,v)                                            \
        out_dword_masked_ns(HWIO_TLMM_RPU_CR_ADDR(nBase),m,v,HWIO_TLMM_RPU_CR_IN);
#define HWIO_TLMM_RPU_CR_DCDEE_BMSK                                                  0x8
#define HWIO_TLMM_RPU_CR_DCDEE_SHFT                                                  0x3
#define HWIO_TLMM_RPU_CR_RPUEIE_BMSK                                                 0x4
#define HWIO_TLMM_RPU_CR_RPUEIE_SHFT                                                 0x2
#define HWIO_TLMM_RPU_CR_RPUERE_BMSK                                                 0x2
#define HWIO_TLMM_RPU_CR_RPUERE_SHFT                                                 0x1
#define HWIO_TLMM_RPU_CR_RPUE_BMSK                                                   0x1
#define HWIO_TLMM_RPU_CR_RPUE_SHFT                                                     0

#define HWIO_TLMM_RPU_EAR_ADDR(nBase)                                                ((nBase)      + 0x00000f84)
#define HWIO_TLMM_RPU_EAR_RMSK                                                   0xfffff
#define HWIO_TLMM_RPU_EAR_IN(nBase)                                                 \
        in_dword_masked(HWIO_TLMM_RPU_EAR_ADDR(nBase), HWIO_TLMM_RPU_EAR_RMSK)
#define HWIO_TLMM_RPU_EAR_INM(nBase,m)                                              \
        in_dword_masked(HWIO_TLMM_RPU_EAR_ADDR(nBase), m)
#define HWIO_TLMM_RPU_EAR_PA_BMSK                                                0xfffff
#define HWIO_TLMM_RPU_EAR_PA_SHFT                                                      0

#define HWIO_TLMM_RPU_ESR_ADDR(nBase)                                                ((nBase)      + 0x00000f88)
#define HWIO_TLMM_RPU_ESR_RMSK                                                0x80000003
#define HWIO_TLMM_RPU_ESR_IN(nBase)                                                 \
        in_dword_masked(HWIO_TLMM_RPU_ESR_ADDR(nBase), HWIO_TLMM_RPU_ESR_RMSK)
#define HWIO_TLMM_RPU_ESR_INM(nBase,m)                                              \
        in_dword_masked(HWIO_TLMM_RPU_ESR_ADDR(nBase), m)
#define HWIO_TLMM_RPU_ESR_OUT(nBase,v)                                              \
        out_dword(HWIO_TLMM_RPU_ESR_ADDR(nBase),v)
#define HWIO_TLMM_RPU_ESR_OUTM(nBase,m,v)                                           \
        out_dword_masked_ns(HWIO_TLMM_RPU_ESR_ADDR(nBase),m,v,HWIO_TLMM_RPU_ESR_IN);
#define HWIO_TLMM_RPU_ESR_MULTIERROR_BMSK                                     0x80000000
#define HWIO_TLMM_RPU_ESR_MULTIERROR_SHFT                                           0x1f
#define HWIO_TLMM_RPU_ESR_CLIENT_BMSK                                                0x2
#define HWIO_TLMM_RPU_ESR_CLIENT_SHFT                                                0x1
#define HWIO_TLMM_RPU_ESR_CFG_BMSK                                                   0x1
#define HWIO_TLMM_RPU_ESR_CFG_SHFT                                                     0

#define HWIO_TLMM_RPU_ESRRESTORE_ADDR(nBase)                                         ((nBase)      + 0x00000f8c)
#define HWIO_TLMM_RPU_ESRRESTORE_RMSK                                         0x80000003
#define HWIO_TLMM_RPU_ESRRESTORE_IN(nBase)                                          \
        in_dword_masked(HWIO_TLMM_RPU_ESRRESTORE_ADDR(nBase), HWIO_TLMM_RPU_ESRRESTORE_RMSK)
#define HWIO_TLMM_RPU_ESRRESTORE_INM(nBase,m)                                       \
        in_dword_masked(HWIO_TLMM_RPU_ESRRESTORE_ADDR(nBase), m)
#define HWIO_TLMM_RPU_ESRRESTORE_OUT(nBase,v)                                       \
        out_dword(HWIO_TLMM_RPU_ESRRESTORE_ADDR(nBase),v)
#define HWIO_TLMM_RPU_ESRRESTORE_OUTM(nBase,m,v)                                    \
        out_dword_masked_ns(HWIO_TLMM_RPU_ESRRESTORE_ADDR(nBase),m,v,HWIO_TLMM_RPU_ESRRESTORE_IN);
#define HWIO_TLMM_RPU_ESRRESTORE_MULTIERROR_BMSK                              0x80000000
#define HWIO_TLMM_RPU_ESRRESTORE_MULTIERROR_SHFT                                    0x1f
#define HWIO_TLMM_RPU_ESRRESTORE_CLIENT_BMSK                                         0x2
#define HWIO_TLMM_RPU_ESRRESTORE_CLIENT_SHFT                                         0x1
#define HWIO_TLMM_RPU_ESRRESTORE_CFG_BMSK                                            0x1
#define HWIO_TLMM_RPU_ESRRESTORE_CFG_SHFT                                              0

#define HWIO_TLMM_RPU_ESYNR0_ADDR(nBase)                                             ((nBase)      + 0x00000f90)
#define HWIO_TLMM_RPU_ESYNR0_RMSK                                               0x1f0000
#define HWIO_TLMM_RPU_ESYNR0_IN(nBase)                                              \
        in_dword_masked(HWIO_TLMM_RPU_ESYNR0_ADDR(nBase), HWIO_TLMM_RPU_ESYNR0_RMSK)
#define HWIO_TLMM_RPU_ESYNR0_INM(nBase,m)                                           \
        in_dword_masked(HWIO_TLMM_RPU_ESYNR0_ADDR(nBase), m)
#define HWIO_TLMM_RPU_ESYNR0_AVMID_BMSK                                         0x1f0000
#define HWIO_TLMM_RPU_ESYNR0_AVMID_SHFT                                             0x10

#define HWIO_TLMM_RPU_ESYNR1_ADDR(nBase)                                             ((nBase)      + 0x00000f94)
#define HWIO_TLMM_RPU_ESYNR1_RMSK                                             0xc0000100
#define HWIO_TLMM_RPU_ESYNR1_IN(nBase)                                              \
        in_dword_masked(HWIO_TLMM_RPU_ESYNR1_ADDR(nBase), HWIO_TLMM_RPU_ESYNR1_RMSK)
#define HWIO_TLMM_RPU_ESYNR1_INM(nBase,m)                                           \
        in_dword_masked(HWIO_TLMM_RPU_ESYNR1_ADDR(nBase), m)
#define HWIO_TLMM_RPU_ESYNR1_DCD_BMSK                                         0x80000000
#define HWIO_TLMM_RPU_ESYNR1_DCD_SHFT                                               0x1f
#define HWIO_TLMM_RPU_ESYNR1_AC_BMSK                                          0x40000000
#define HWIO_TLMM_RPU_ESYNR1_AC_SHFT                                                0x1e
#define HWIO_TLMM_RPU_ESYNR1_AWRITE_BMSK                                           0x100
#define HWIO_TLMM_RPU_ESYNR1_AWRITE_SHFT                                             0x8

#define HWIO_TLMM_RPU_REV_ADDR(nBase)                                                ((nBase)      + 0x00000ff4)
#define HWIO_TLMM_RPU_REV_RMSK                                                      0xff
#define HWIO_TLMM_RPU_REV_IN(nBase)                                                 \
        in_dword_masked(HWIO_TLMM_RPU_REV_ADDR(nBase), HWIO_TLMM_RPU_REV_RMSK)
#define HWIO_TLMM_RPU_REV_INM(nBase,m)                                              \
        in_dword_masked(HWIO_TLMM_RPU_REV_ADDR(nBase), m)
#define HWIO_TLMM_RPU_REV_MAJOR_BMSK                                                0xf0
#define HWIO_TLMM_RPU_REV_MAJOR_SHFT                                                 0x4
#define HWIO_TLMM_RPU_REV_MINOR_BMSK                                                 0xf
#define HWIO_TLMM_RPU_REV_MINOR_SHFT                                                   0

#define HWIO_TLMM_RPU_IDR_ADDR(nBase)                                                ((nBase)      + 0x00000ff8)
#define HWIO_TLMM_RPU_IDR_RMSK                                                  0x1f3cff
#define HWIO_TLMM_RPU_IDR_IN(nBase)                                                 \
        in_dword_masked(HWIO_TLMM_RPU_IDR_ADDR(nBase), HWIO_TLMM_RPU_IDR_RMSK)
#define HWIO_TLMM_RPU_IDR_INM(nBase,m)                                              \
        in_dword_masked(HWIO_TLMM_RPU_IDR_ADDR(nBase), m)
#define HWIO_TLMM_RPU_IDR_LSB_BMSK                                              0x1f0000
#define HWIO_TLMM_RPU_IDR_LSB_SHFT                                                  0x10
#define HWIO_TLMM_RPU_IDR_XPUT_BMSK                                               0x3000
#define HWIO_TLMM_RPU_IDR_XPUT_SHFT                                                  0xc
#define HWIO_TLMM_RPU_IDR_PT_BMSK                                                  0x800
#define HWIO_TLMM_RPU_IDR_PT_SHFT                                                    0xb
#define HWIO_TLMM_RPU_IDR_MV_BMSK                                                  0x400
#define HWIO_TLMM_RPU_IDR_MV_SHFT                                                    0xa
#define HWIO_TLMM_RPU_IDR_NRG_BMSK                                                  0xff
#define HWIO_TLMM_RPU_IDR_NRG_SHFT                                                     0

#define HWIO_TLMM_RPU_ACR_ADDR(nBase)                                                ((nBase)      + 0x00000ffc)
#define HWIO_TLMM_RPU_ACR_RMSK                                                0xffffffff
#define HWIO_TLMM_RPU_ACR_IN(nBase)                                                 \
        in_dword_masked(HWIO_TLMM_RPU_ACR_ADDR(nBase), HWIO_TLMM_RPU_ACR_RMSK)
#define HWIO_TLMM_RPU_ACR_INM(nBase,m)                                              \
        in_dword_masked(HWIO_TLMM_RPU_ACR_ADDR(nBase), m)
#define HWIO_TLMM_RPU_ACR_OUT(nBase,v)                                              \
        out_dword(HWIO_TLMM_RPU_ACR_ADDR(nBase),v)
#define HWIO_TLMM_RPU_ACR_OUTM(nBase,m,v)                                           \
        out_dword_masked_ns(HWIO_TLMM_RPU_ACR_ADDR(nBase),m,v,HWIO_TLMM_RPU_ACR_IN);
#define HWIO_TLMM_RPU_ACR_RWE_BMSK                                            0xffffffff
#define HWIO_TLMM_RPU_ACR_RWE_SHFT                                                     0

#define HWIO_GPIO_CFGn_ADDR(nBase, n)                                                ((nBase)      + 0x00001000 + 0x10 * (n))
#define HWIO_GPIO_CFGn_RMSK                                                        0x3ff
#define HWIO_GPIO_CFGn_MAXn                                                         0xac
#define HWIO_GPIO_CFGn_INI(nBase,n) \
        in_dword(HWIO_GPIO_CFGn_ADDR(nBase, n))
#define HWIO_GPIO_CFGn_INMI(nBase,n,mask) \
        in_dword_masked(HWIO_GPIO_CFGn_ADDR(nBase, n), mask)
#define HWIO_GPIO_CFGn_OUTI(nBase,n,val) \
        out_dword(HWIO_GPIO_CFGn_ADDR(nBase, n),val)
#define HWIO_GPIO_CFGn_OUTMI(nBase,n,mask,val) \
        out_dword_masked_ns(HWIO_GPIO_CFGn_ADDR(nBase, n),mask,val,HWIO_GPIO_CFGn_INI(nBase,n));
#define HWIO_GPIO_CFGn_GPIO_OE_BMSK                                                0x200
#define HWIO_GPIO_CFGn_GPIO_OE_SHFT                                                  0x9
#define HWIO_GPIO_CFGn_DRV_STRENGTH_BMSK                                           0x1c0
#define HWIO_GPIO_CFGn_DRV_STRENGTH_SHFT                                             0x6
#define HWIO_GPIO_CFGn_FUNC_SEL_BMSK                                                0x3c
#define HWIO_GPIO_CFGn_FUNC_SEL_SHFT                                                 0x2
#define HWIO_GPIO_CFGn_GPIO_PULL_BMSK                                                0x3
#define HWIO_GPIO_CFGn_GPIO_PULL_SHFT                                                  0

#define HWIO_GPIO_IN_OUTn_ADDR(nBase, n)                                             ((nBase)      + 0x00001004 + 0x10 * (n))
#define HWIO_GPIO_IN_OUTn_RMSK                                                       0x3
#define HWIO_GPIO_IN_OUTn_MAXn                                                      0xac
#define HWIO_GPIO_IN_OUTn_INI(nBase,n) \
        in_dword(HWIO_GPIO_IN_OUTn_ADDR(nBase, n))
#define HWIO_GPIO_IN_OUTn_INMI(nBase, n,mask) \
        in_dword_masked(HWIO_GPIO_IN_OUTn_ADDR(nBase, n), mask)
#define HWIO_GPIO_IN_OUTn_OUTI(nBase,n,val) \
        out_dword(HWIO_GPIO_IN_OUTn_ADDR(nBase, n),val)
#define HWIO_GPIO_IN_OUTn_OUTMI(nBase,n,mask,val) \
        out_dword_masked_ns(HWIO_GPIO_IN_OUTn_ADDR(nBase, n),mask,val,HWIO_GPIO_IN_OUTn_INI(nBase,n));
#define HWIO_GPIO_IN_OUTn_GPIO_OUT_BMSK                                              0x2
#define HWIO_GPIO_IN_OUTn_GPIO_OUT_SHFT                                              0x1
#define HWIO_GPIO_IN_OUTn_GPIO_IN_BMSK                                               0x1
#define HWIO_GPIO_IN_OUTn_GPIO_IN_SHFT                                                 0

#define HWIO_GPIO_INTR_CFGn_ADDR(nBase, n)                                           ((nBase)      + 0x00001008 + 0x10 * (n))
#define HWIO_GPIO_INTR_CFGn_RMSK                                                     0xf
#define HWIO_GPIO_INTR_CFGn_MAXn                                                    0xac
#define HWIO_GPIO_INTR_CFGn_INI(nBase,n) \
        in_dword(HWIO_GPIO_INTR_CFGn_ADDR(nBase, n))
#define HWIO_GPIO_INTR_CFGn_INMI(nBase,n,mask) \
        in_dword_masked(HWIO_GPIO_INTR_CFGn_ADDR(nBase, n), mask)
#define HWIO_GPIO_INTR_CFGn_OUTI(nBase,n,val) \
        out_dword(HWIO_GPIO_INTR_CFGn_ADDR(nBase, n),val)
#define HWIO_GPIO_INTR_CFGn_OUTMI(nBase,n,mask,val) \
        out_dword_masked_ns(HWIO_GPIO_INTR_CFGn_ADDR(nBase, n),mask,val,HWIO_GPIO_INTR_CFGn_INI(nBase,n));
#define HWIO_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                                  0x8
#define HWIO_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                                  0x3
#define HWIO_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                                       0x4
#define HWIO_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                                       0x2
#define HWIO_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                                        0x2
#define HWIO_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                                        0x1
#define HWIO_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                                         0x1
#define HWIO_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                                           0

#define HWIO_GPIO_INTR_STATUSn_ADDR(nBase, n)                                        ((nBase)      + 0x0000100c + 0x10 * (n))
#define HWIO_GPIO_INTR_STATUSn_RMSK                                                  0x1
#define HWIO_GPIO_INTR_STATUSn_MAXn                                                 0xac
#define HWIO_GPIO_INTR_STATUSn_INI(nBase,n) \
        in_dword(HWIO_GPIO_INTR_STATUSn_ADDR(nBase, n))
#define HWIO_GPIO_INTR_STATUSn_INMI(nBase,n,mask) \
        in_dword_masked(HWIO_GPIO_INTR_STATUSn_ADDR(nBase, n), mask)
#define HWIO_GPIO_INTR_STATUSn_OUTI(nBase,n,val) \
        out_dword(HWIO_GPIO_INTR_STATUSn_ADDR(nBase, n),val)
#define HWIO_GPIO_INTR_STATUSn_OUTMI(nBase,n,mask,val) \
        out_dword_masked_ns(HWIO_GPIO_INTR_STATUSn_ADDR(nBase, n),mask,val,HWIO_GPIO_INTR_STATUSn_INI(nBase,n));
#define HWIO_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                                      0x1
#define HWIO_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                                        0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR(nBase)                                           ((nBase)      + 0x00002000)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                                0x3ff
#define HWIO_TLMM_INT_JTAG_CTL_IN(nBase)                                            \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR(nBase), HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(nBase,m)                                         \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR(nBase), m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(nBase,v)                                         \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR(nBase),v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(nBase,m,v)                                      \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR(nBase),m,v,HWIO_TLMM_INT_JTAG_CTL_IN);
#define HWIO_TLMM_INT_JTAG_CTL_TOTAL_RTCK_ENABLE_SOFT_BMSK                         0x200
#define HWIO_TLMM_INT_JTAG_CTL_TOTAL_RTCK_ENABLE_SOFT_SHFT                           0x9
#define HWIO_TLMM_INT_JTAG_CTL_RTCK_SEL_SOFT_BMSK                                  0x180
#define HWIO_TLMM_INT_JTAG_CTL_RTCK_SEL_SOFT_SHFT                                    0x7
#define HWIO_TLMM_INT_JTAG_CTL_PPSS_TAP_ENA_SOFT_BMSK                               0x40
#define HWIO_TLMM_INT_JTAG_CTL_PPSS_TAP_ENA_SOFT_SHFT                                0x6
#define HWIO_TLMM_INT_JTAG_CTL_MSSA11_TAP_ENA_SOFT_BMSK                             0x20
#define HWIO_TLMM_INT_JTAG_CTL_MSSA11_TAP_ENA_SOFT_SHFT                              0x5
#define HWIO_TLMM_INT_JTAG_CTL_RPM_TAP_ENA_SOFT_BMSK                                0x10
#define HWIO_TLMM_INT_JTAG_CTL_RPM_TAP_ENA_SOFT_SHFT                                 0x4
#define HWIO_TLMM_INT_JTAG_CTL_ADSP6_TAP_ENA_SOFT_BMSK                               0x8
#define HWIO_TLMM_INT_JTAG_CTL_ADSP6_TAP_ENA_SOFT_SHFT                               0x3
#define HWIO_TLMM_INT_JTAG_CTL_SCMP_TAP_ENA_SOFT_BMSK                                0x4
#define HWIO_TLMM_INT_JTAG_CTL_SCMP_TAP_ENA_SOFT_SHFT                                0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SOFT_BMSK                                 0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SOFT_SHFT                                 0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SOFT_BMSK                                 0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SOFT_SHFT                                   0

#define HWIO_TLMM_AUX_JTAG_ADDR(nBase)                                               ((nBase)      + 0x00002004)
#define HWIO_TLMM_AUX_JTAG_RMSK                                                      0x7
#define HWIO_TLMM_AUX_JTAG_IN(nBase)                                                \
        in_dword_masked(HWIO_TLMM_AUX_JTAG_ADDR(nBase), HWIO_TLMM_AUX_JTAG_RMSK)
#define HWIO_TLMM_AUX_JTAG_INM(nBase,m)                                             \
        in_dword_masked(HWIO_TLMM_AUX_JTAG_ADDR(nBase), m)
#define HWIO_TLMM_AUX_JTAG_OUT(nBase,v)                                             \
        out_dword(HWIO_TLMM_AUX_JTAG_ADDR(nBase),v)
#define HWIO_TLMM_AUX_JTAG_OUTM(nBase,m,v)                                          \
        out_dword_masked_ns(HWIO_TLMM_AUX_JTAG_ADDR(nBase),m,v,HWIO_TLMM_AUX_JTAG_IN);
#define HWIO_TLMM_AUX_JTAG_AUX_JTAG_SEL_BMSK                                         0x7
#define HWIO_TLMM_AUX_JTAG_AUX_JTAG_SEL_SHFT                                           0

#define HWIO_TLMM_ETM_MODE_ADDR(nBase)                                               ((nBase)      + 0x00002034)
#define HWIO_TLMM_ETM_MODE_RMSK                                                      0xf
#define HWIO_TLMM_ETM_MODE_IN(nBase)                                                \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR(nBase), HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(nBase,m)                                             \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR(nBase), m)
#define HWIO_TLMM_ETM_MODE_OUT(nBase,v)                                             \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR(nBase),v)
#define HWIO_TLMM_ETM_MODE_OUTM(nBase,m,v)                                          \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR(nBase),m,v,HWIO_TLMM_ETM_MODE_IN);
#define HWIO_TLMM_ETM_MODE_ETM_MODE_B_BMSK                                           0xc
#define HWIO_TLMM_ETM_MODE_ETM_MODE_B_SHFT                                           0x2
#define HWIO_TLMM_ETM_MODE_ETM_MODE_A_BMSK                                           0x3
#define HWIO_TLMM_ETM_MODE_ETM_MODE_A_SHFT                                             0

#define HWIO_TLMM_ETM_XTRIG_0_ADDR(nBase)                                            ((nBase)      + 0x00002008)
#define HWIO_TLMM_ETM_XTRIG_0_RMSK                                            0x3fffffff
#define HWIO_TLMM_ETM_XTRIG_0_IN(nBase)                                             \
        in_dword_masked(HWIO_TLMM_ETM_XTRIG_0_ADDR(nBase), HWIO_TLMM_ETM_XTRIG_0_RMSK)
#define HWIO_TLMM_ETM_XTRIG_0_INM(nBase,m)                                          \
        in_dword_masked(HWIO_TLMM_ETM_XTRIG_0_ADDR(nBase), m)
#define HWIO_TLMM_ETM_XTRIG_0_OUT(nBase,v)                                          \
        out_dword(HWIO_TLMM_ETM_XTRIG_0_ADDR(nBase),v)
#define HWIO_TLMM_ETM_XTRIG_0_OUTM(nBase,m,v)                                       \
        out_dword_masked_ns(HWIO_TLMM_ETM_XTRIG_0_ADDR(nBase),m,v,HWIO_TLMM_ETM_XTRIG_0_IN);
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_AD6_BMSK                                  0x20000000
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_AD6_SHFT                                        0x1d
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_AD6_BMSK                         0x10000000
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_AD6_SHFT                               0x1c
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_AD6_BMSK                             0x8000000
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_AD6_SHFT                                  0x1b
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_AD6_BMSK                             0x4000000
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_AD6_SHFT                                  0x1a
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_AD6_BMSK                              0x2000000
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_AD6_SHFT                                   0x19
#define HWIO_TLMM_ETM_XTRIG_0_A11_TO_AD6_BMSK                                  0x1000000
#define HWIO_TLMM_ETM_XTRIG_0_A11_TO_AD6_SHFT                                       0x18
#define HWIO_TLMM_ETM_XTRIG_0_SC1_TO_AD6_BMSK                                   0x800000
#define HWIO_TLMM_ETM_XTRIG_0_SC1_TO_AD6_SHFT                                       0x17
#define HWIO_TLMM_ETM_XTRIG_0_SC0_TO_AD6_BMSK                                   0x400000
#define HWIO_TLMM_ETM_XTRIG_0_SC0_TO_AD6_SHFT                                       0x16
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_A11_BMSK                                    0x200000
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_A11_SHFT                                        0x15
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_A11_BMSK                           0x100000
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_A11_SHFT                               0x14
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_A11_BMSK                               0x80000
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_A11_SHFT                                  0x13
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_A11_BMSK                               0x40000
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_A11_SHFT                                  0x12
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_A11_BMSK                                0x20000
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_A11_SHFT                                   0x11
#define HWIO_TLMM_ETM_XTRIG_0_AD6_TO_A11_BMSK                                    0x10000
#define HWIO_TLMM_ETM_XTRIG_0_AD6_TO_A11_SHFT                                       0x10
#define HWIO_TLMM_ETM_XTRIG_0_SC1_TO_A11_BMSK                                     0x8000
#define HWIO_TLMM_ETM_XTRIG_0_SC1_TO_A11_SHFT                                        0xf
#define HWIO_TLMM_ETM_XTRIG_0_SC0_TO_A11_BMSK                                     0x4000
#define HWIO_TLMM_ETM_XTRIG_0_SC0_TO_A11_SHFT                                        0xe
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_SC1_BMSK                                      0x2000
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_SC1_SHFT                                         0xd
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_SC1_BMSK                             0x1000
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_SC1_SHFT                                0xc
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_SC1_BMSK                                 0x800
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_SC1_SHFT                                   0xb
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_SC1_BMSK                                 0x400
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_SC1_SHFT                                   0xa
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_SC1_BMSK                                  0x200
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_SC1_SHFT                                    0x9
#define HWIO_TLMM_ETM_XTRIG_0_AD6_TO_SC1_BMSK                                      0x100
#define HWIO_TLMM_ETM_XTRIG_0_AD6_TO_SC1_SHFT                                        0x8
#define HWIO_TLMM_ETM_XTRIG_0_A11_TO_SC1_BMSK                                       0x80
#define HWIO_TLMM_ETM_XTRIG_0_A11_TO_SC1_SHFT                                        0x7
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_SC0_BMSK                                        0x40
#define HWIO_TLMM_ETM_XTRIG_0_LA_TO_SC0_SHFT                                         0x6
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_SC0_BMSK                               0x20
#define HWIO_TLMM_ETM_XTRIG_0_DAYTONA_FAB_TO_SC0_SHFT                                0x5
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_SC0_BMSK                                  0x10
#define HWIO_TLMM_ETM_XTRIG_0_APPS_FAB_TO_SC0_SHFT                                   0x4
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_SC0_BMSK                                   0x8
#define HWIO_TLMM_ETM_XTRIG_0_MMSS_FAB_TO_SC0_SHFT                                   0x3
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_SC0_BMSK                                    0x4
#define HWIO_TLMM_ETM_XTRIG_0_SYS_FAB_TO_SC0_SHFT                                    0x2
#define HWIO_TLMM_ETM_XTRIG_0_AD6_TO_SC0_BMSK                                        0x2
#define HWIO_TLMM_ETM_XTRIG_0_AD6_TO_SC0_SHFT                                        0x1
#define HWIO_TLMM_ETM_XTRIG_0_A11_TO_SC0_BMSK                                        0x1
#define HWIO_TLMM_ETM_XTRIG_0_A11_TO_SC0_SHFT                                          0

#define HWIO_TLMM_ETM_XTRIG_1_ADDR(nBase)                                            ((nBase)      + 0x0000200c)
#define HWIO_TLMM_ETM_XTRIG_1_RMSK                                            0xffffffff
#define HWIO_TLMM_ETM_XTRIG_1_IN(nBase)                                             \
        in_dword_masked(HWIO_TLMM_ETM_XTRIG_1_ADDR(nBase), HWIO_TLMM_ETM_XTRIG_1_RMSK)
#define HWIO_TLMM_ETM_XTRIG_1_INM(nBase,m)                                          \
        in_dword_masked(HWIO_TLMM_ETM_XTRIG_1_ADDR(nBase), m)
#define HWIO_TLMM_ETM_XTRIG_1_OUT(nBase,v)                                          \
        out_dword(HWIO_TLMM_ETM_XTRIG_1_ADDR(nBase),v)
#define HWIO_TLMM_ETM_XTRIG_1_OUTM(nBase,m,v)                                       \
        out_dword_masked_ns(HWIO_TLMM_ETM_XTRIG_1_ADDR(nBase),m,v,HWIO_TLMM_ETM_XTRIG_1_IN);
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_MMSS_FAB_STOP_BMSK                        0x80000000
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_MMSS_FAB_STOP_SHFT                              0x1f
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_MMSS_FAB_STOP_BMSK               0x40000000
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_MMSS_FAB_STOP_SHFT                     0x1e
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_MMSS_FAB_STOP_BMSK                  0x20000000
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_MMSS_FAB_STOP_SHFT                        0x1d
#define HWIO_TLMM_ETM_XTRIG_1_SYS_FAB_TO_MMSS_FAB_STOP_BMSK                   0x10000000
#define HWIO_TLMM_ETM_XTRIG_1_SYS_FAB_TO_MMSS_FAB_STOP_SHFT                         0x1c
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_MMSS_FAB_STOP_BMSK                        0x8000000
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_MMSS_FAB_STOP_SHFT                             0x1b
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_MMSS_FAB_STOP_BMSK                        0x4000000
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_MMSS_FAB_STOP_SHFT                             0x1a
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_MMSS_FAB_STOP_BMSK                        0x2000000
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_MMSS_FAB_STOP_SHFT                             0x19
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_MMSS_FAB_STOP_BMSK                        0x1000000
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_MMSS_FAB_STOP_SHFT                             0x18
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_MMSS_FAB_START_BMSK                         0x800000
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_MMSS_FAB_START_SHFT                             0x17
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_MMSS_FAB_START_BMSK                0x400000
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_MMSS_FAB_START_SHFT                    0x16
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_MMSS_FAB_START_BMSK                   0x200000
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_MMSS_FAB_START_SHFT                       0x15
#define HWIO_TLMM_ETM_XTRIG_1_SYS_FAB_TO_MMSS_FAB_START_BMSK                    0x100000
#define HWIO_TLMM_ETM_XTRIG_1_SYS_FAB_TO_MMSS_FAB_START_SHFT                        0x14
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_MMSS_FAB_START_BMSK                         0x80000
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_MMSS_FAB_START_SHFT                            0x13
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_MMSS_FAB_START_BMSK                         0x40000
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_MMSS_FAB_START_SHFT                            0x12
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_MMSS_FAB_START_BMSK                         0x20000
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_MMSS_FAB_START_SHFT                            0x11
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_MMSS_FAB_START_BMSK                         0x10000
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_MMSS_FAB_START_SHFT                            0x10
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_SYS_FAB_STOP_BMSK                             0x8000
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_SYS_FAB_STOP_SHFT                                0xf
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_SYS_FAB_STOP_BMSK                    0x4000
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_SYS_FAB_STOP_SHFT                       0xe
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_SYS_FAB_STOP_BMSK                       0x2000
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_SYS_FAB_STOP_SHFT                          0xd
#define HWIO_TLMM_ETM_XTRIG_1_MMSS_FAB_TO_SYS_FAB_STOP_BMSK                       0x1000
#define HWIO_TLMM_ETM_XTRIG_1_MMSS_FAB_TO_SYS_FAB_STOP_SHFT                          0xc
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_SYS_FAB_STOP_BMSK                             0x800
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_SYS_FAB_STOP_SHFT                               0xb
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_SYS_FAB_STOP_BMSK                             0x400
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_SYS_FAB_STOP_SHFT                               0xa
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_SYS_FAB_STOP_BMSK                             0x200
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_SYS_FAB_STOP_SHFT                               0x9
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_SYS_FAB_STOP_BMSK                             0x100
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_SYS_FAB_STOP_SHFT                               0x8
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_SYS_FAB_START_BMSK                              0x80
#define HWIO_TLMM_ETM_XTRIG_1_LA_TO_SYS_FAB_START_SHFT                               0x7
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_SYS_FAB_START_BMSK                     0x40
#define HWIO_TLMM_ETM_XTRIG_1_DAYTONA_FAB_TO_SYS_FAB_START_SHFT                      0x6
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_SYS_FAB_START_BMSK                        0x20
#define HWIO_TLMM_ETM_XTRIG_1_APPS_FAB_TO_SYS_FAB_START_SHFT                         0x5
#define HWIO_TLMM_ETM_XTRIG_1_MMSS_FAB_TO_SYS_FAB_START_BMSK                        0x10
#define HWIO_TLMM_ETM_XTRIG_1_MMSS_FAB_TO_SYS_FAB_START_SHFT                         0x4
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_SYS_FAB_START_BMSK                              0x8
#define HWIO_TLMM_ETM_XTRIG_1_AD6_TO_SYS_FAB_START_SHFT                              0x3
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_SYS_FAB_START_BMSK                              0x4
#define HWIO_TLMM_ETM_XTRIG_1_A11_TO_SYS_FAB_START_SHFT                              0x2
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_SYS_FAB_START_BMSK                              0x2
#define HWIO_TLMM_ETM_XTRIG_1_SC1_TO_SYS_FAB_START_SHFT                              0x1
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_SYS_FAB_START_BMSK                              0x1
#define HWIO_TLMM_ETM_XTRIG_1_SC0_TO_SYS_FAB_START_SHFT                                0

#define HWIO_TLMM_ETM_XTRIG_2_ADDR(nBase)                                            ((nBase)      + 0x00002010)
#define HWIO_TLMM_ETM_XTRIG_2_RMSK                                            0xffffffff
#define HWIO_TLMM_ETM_XTRIG_2_IN(nBase)                                             \
        in_dword_masked(HWIO_TLMM_ETM_XTRIG_2_ADDR(nBase), HWIO_TLMM_ETM_XTRIG_2_RMSK)
#define HWIO_TLMM_ETM_XTRIG_2_INM(nBase,m)                                          \
        in_dword_masked(HWIO_TLMM_ETM_XTRIG_2_ADDR(nBase), m)
#define HWIO_TLMM_ETM_XTRIG_2_OUT(nBase,v)                                          \
        out_dword(HWIO_TLMM_ETM_XTRIG_2_ADDR(nBase),v)
#define HWIO_TLMM_ETM_XTRIG_2_OUTM(nBase,m,v)                                       \
        out_dword_masked_ns(HWIO_TLMM_ETM_XTRIG_2_ADDR(nBase),m,v,HWIO_TLMM_ETM_XTRIG_2_IN);
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_DAYTONA_FAB_STOP_BMSK                     0x80000000
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_DAYTONA_FAB_STOP_SHFT                           0x1f
#define HWIO_TLMM_ETM_XTRIG_2_APPS_FAB_TO_DAYTONA_FAB_STOP_BMSK               0x40000000
#define HWIO_TLMM_ETM_XTRIG_2_APPS_FAB_TO_DAYTONA_FAB_STOP_SHFT                     0x1e
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_DAYTONA_FAB_STOP_BMSK               0x20000000
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_DAYTONA_FAB_STOP_SHFT                     0x1d
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_DAYTONA_FAB_STOP_BMSK                0x10000000
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_DAYTONA_FAB_STOP_SHFT                      0x1c
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_DAYTONA_FAB_STOP_BMSK                     0x8000000
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_DAYTONA_FAB_STOP_SHFT                          0x1b
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_DAYTONA_FAB_STOP_BMSK                     0x4000000
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_DAYTONA_FAB_STOP_SHFT                          0x1a
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_DAYTONA_FAB_STOP_BMSK                     0x2000000
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_DAYTONA_FAB_STOP_SHFT                          0x19
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_DAYTONA_FAB_STOP_BMSK                     0x1000000
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_DAYTONA_FAB_STOP_SHFT                          0x18
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_DAYTONA_FAB_START_BMSK                      0x800000
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_DAYTONA_FAB_START_SHFT                          0x17
#define HWIO_TLMM_ETM_XTRIG_2_APPS_FAB_TO_DAYTONA_FAB_START_BMSK                0x400000
#define HWIO_TLMM_ETM_XTRIG_2_APPS_FAB_TO_DAYTONA_FAB_START_SHFT                    0x16
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_DAYTONA_FAB_START_BMSK                0x200000
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_DAYTONA_FAB_START_SHFT                    0x15
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_DAYTONA_FAB_START_BMSK                 0x100000
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_DAYTONA_FAB_START_SHFT                     0x14
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_DAYTONA_FAB_START_BMSK                      0x80000
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_DAYTONA_FAB_START_SHFT                         0x13
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_DAYTONA_FAB_START_BMSK                      0x40000
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_DAYTONA_FAB_START_SHFT                         0x12
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_DAYTONA_FAB_START_BMSK                      0x20000
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_DAYTONA_FAB_START_SHFT                         0x11
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_DAYTONA_FAB_START_BMSK                      0x10000
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_DAYTONA_FAB_START_SHFT                         0x10
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_APPS_FAB_STOP_BMSK                            0x8000
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_APPS_FAB_STOP_SHFT                               0xf
#define HWIO_TLMM_ETM_XTRIG_2_DAYTONA_FAB_TO_APPS_FAB_STOP_BMSK                   0x4000
#define HWIO_TLMM_ETM_XTRIG_2_DAYTONA_FAB_TO_APPS_FAB_STOP_SHFT                      0xe
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_APPS_FAB_STOP_BMSK                      0x2000
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_APPS_FAB_STOP_SHFT                         0xd
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_APPS_FAB_STOP_BMSK                       0x1000
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_APPS_FAB_STOP_SHFT                          0xc
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_APPS_FAB_STOP_BMSK                            0x800
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_APPS_FAB_STOP_SHFT                              0xb
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_APPS_FAB_STOP_BMSK                            0x400
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_APPS_FAB_STOP_SHFT                              0xa
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_APPS_FAB_STOP_BMSK                            0x200
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_APPS_FAB_STOP_SHFT                              0x9
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_APPS_FAB_STOP_BMSK                            0x100
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_APPS_FAB_STOP_SHFT                              0x8
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_APPS_FAB_START_BMSK                             0x80
#define HWIO_TLMM_ETM_XTRIG_2_LA_TO_APPS_FAB_START_SHFT                              0x7
#define HWIO_TLMM_ETM_XTRIG_2_DAYTONA_FAB_TO_APPS_FAB_START_BMSK                    0x40
#define HWIO_TLMM_ETM_XTRIG_2_DAYTONA_FAB_TO_APPS_FAB_START_SHFT                     0x6
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_APPS_FAB_START_BMSK                       0x20
#define HWIO_TLMM_ETM_XTRIG_2_MMSS_FAB_TO_APPS_FAB_START_SHFT                        0x5
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_APPS_FAB_START_BMSK                        0x10
#define HWIO_TLMM_ETM_XTRIG_2_SYS_FAB_TO_APPS_FAB_START_SHFT                         0x4
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_APPS_FAB_START_BMSK                             0x8
#define HWIO_TLMM_ETM_XTRIG_2_AD6_TO_APPS_FAB_START_SHFT                             0x3
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_APPS_FAB_START_BMSK                             0x4
#define HWIO_TLMM_ETM_XTRIG_2_A11_TO_APPS_FAB_START_SHFT                             0x2
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_APPS_FAB_START_BMSK                             0x2
#define HWIO_TLMM_ETM_XTRIG_2_SC1_TO_APPS_FAB_START_SHFT                             0x1
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_APPS_FAB_START_BMSK                             0x1
#define HWIO_TLMM_ETM_XTRIG_2_SC0_TO_APPS_FAB_START_SHFT                               0

#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_ADDR(nBase)                                  ((nBase)      + 0x00002018)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RMSK                                  0x3fffffff
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_IN(nBase)                                   \
        in_dword_masked(HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_ADDR(nBase), HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RMSK)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_INM(nBase,m)                                \
        in_dword_masked(HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_ADDR(nBase), m)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_OUT(nBase,v)                                \
        out_dword(HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_ADDR(nBase),v)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_ADDR(nBase),m,v,HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_IN);
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_PPSS_BMSK                    0x20000000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_PPSS_SHFT                          0x1d
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_PPSS_BMSK                      0x10000000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_PPSS_SHFT                            0x1c
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_PPSS_BMSK                       0x8000000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_PPSS_SHFT                            0x1b
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_PPSS_BMSK                       0x4000000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_PPSS_SHFT                            0x1a
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_PPSS_BMSK                       0x2000000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_PPSS_SHFT                            0x19
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_PPSS_BMSK                       0x1000000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_PPSS_SHFT                            0x18
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_A11_BMSK                       0x800000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_A11_SHFT                           0x17
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_A11_BMSK                         0x400000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_A11_SHFT                             0x16
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_A11_BMSK                        0x200000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_A11_SHFT                            0x15
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_A11_BMSK                         0x100000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_A11_SHFT                             0x14
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_A11_BMSK                          0x80000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_A11_SHFT                             0x13
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_A11_BMSK                          0x40000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_A11_SHFT                             0x12
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_RPM_BMSK                        0x20000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_RPM_SHFT                           0x11
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_RPM_BMSK                          0x10000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_RPM_SHFT                             0x10
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_RPM_BMSK                          0x8000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_RPM_SHFT                             0xf
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_RPM_BMSK                           0x4000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_RPM_SHFT                              0xe
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_RPM_BMSK                           0x2000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_RPM_SHFT                              0xd
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_RPM_BMSK                           0x1000
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_RPM_SHFT                              0xc
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_SC1_BMSK                          0x800
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_SC1_SHFT                            0xb
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_SC1_BMSK                            0x400
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_SC1_SHFT                              0xa
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_SC1_BMSK                           0x200
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_SC1_SHFT                             0x9
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_SC1_BMSK                            0x100
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_SC1_SHFT                              0x8
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_SC1_BMSK                             0x80
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_SC1_SHFT                              0x7
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_SC1_BMSK                             0x40
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC0_TO_SC1_SHFT                              0x6
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_SC0_BMSK                           0x20
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_QDMAP_TO_SC0_SHFT                            0x5
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_SC0_BMSK                             0x10
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_AD6_TO_SC0_SHFT                              0x4
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_SC0_BMSK                             0x8
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_PPSS_TO_SC0_SHFT                             0x3
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_SC0_BMSK                              0x4
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_A11_TO_SC0_SHFT                              0x2
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_SC0_BMSK                              0x2
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_RPM_TO_SC0_SHFT                              0x1
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_SC0_BMSK                              0x1
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_0_SC1_TO_SC0_SHFT                                0

#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_ADDR(nBase)                                  ((nBase)      + 0x0000201c)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_RMSK                                        0x3f
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_IN(nBase)                                   \
        in_dword_masked(HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_ADDR(nBase), HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_RMSK)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_INM(nBase,m)                                \
        in_dword_masked(HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_ADDR(nBase), m)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_OUT(nBase,v)                                \
        out_dword(HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_ADDR(nBase),v)
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_ADDR(nBase),m,v,HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_IN);
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_QDMAP_TO_AD6_BMSK                           0x20
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_QDMAP_TO_AD6_SHFT                            0x5
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_A11_TO_AD6_BMSK                             0x10
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_A11_TO_AD6_SHFT                              0x4
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_PPSS_TO_AD6_BMSK                             0x8
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_PPSS_TO_AD6_SHFT                             0x3
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_RPM_TO_AD6_BMSK                              0x4
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_RPM_TO_AD6_SHFT                              0x2
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_SC1_TO_AD6_BMSK                              0x2
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_SC1_TO_AD6_SHFT                              0x1
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_SC0_TO_AD6_BMSK                              0x1
#define HWIO_TLMM_DBG_HALT_XTRIG_PERM_1_SC0_TO_AD6_SHFT                                0

#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_ADDR(nBase)                                  ((nBase)      + 0x00002020)
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RMSK                                    0xffffff
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_IN(nBase)                                   \
        in_dword_masked(HWIO_TLMM_DBG_RESUME_XTRIG_PERM_ADDR(nBase), HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RMSK)
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_INM(nBase,m)                                \
        in_dword_masked(HWIO_TLMM_DBG_RESUME_XTRIG_PERM_ADDR(nBase), m)
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_OUT(nBase,v)                                \
        out_dword(HWIO_TLMM_DBG_RESUME_XTRIG_PERM_ADDR(nBase),v)
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_OUTM(nBase,m,v)                             \
        out_dword_masked_ns(HWIO_TLMM_DBG_RESUME_XTRIG_PERM_ADDR(nBase),m,v,HWIO_TLMM_DBG_RESUME_XTRIG_PERM_IN);
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_AD6_BMSK                       0x800000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_AD6_SHFT                           0x17
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_A11_TO_AD6_BMSK                         0x400000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_A11_TO_AD6_SHFT                             0x16
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_AD6_BMSK                        0x200000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_AD6_SHFT                            0x15
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_AD6_BMSK                         0x100000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_AD6_SHFT                             0x14
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC1_TO_AD6_BMSK                          0x80000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC1_TO_AD6_SHFT                             0x13
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC0_TO_AD6_BMSK                          0x40000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC0_TO_AD6_SHFT                             0x12
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_A11_BMSK                        0x20000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_A11_SHFT                           0x11
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_AD6_TO_A11_BMSK                          0x10000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_AD6_TO_A11_SHFT                             0x10
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_A11_BMSK                          0x8000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_A11_SHFT                             0xf
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_A11_BMSK                           0x4000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_A11_SHFT                              0xe
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC1_TO_A11_BMSK                           0x2000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC1_TO_A11_SHFT                              0xd
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC0_TO_A11_BMSK                           0x1000
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC0_TO_A11_SHFT                              0xc
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_SC1_BMSK                          0x800
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_SC1_SHFT                            0xb
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_AD6_TO_SC1_BMSK                            0x400
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_AD6_TO_SC1_SHFT                              0xa
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_SC1_BMSK                           0x200
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_SC1_SHFT                             0x9
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_A11_TO_SC1_BMSK                            0x100
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_A11_TO_SC1_SHFT                              0x8
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_SC1_BMSK                             0x80
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_SC1_SHFT                              0x7
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC0_TO_SC1_BMSK                             0x40
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC0_TO_SC1_SHFT                              0x6
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_SC0_BMSK                           0x20
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_QDMAP_TO_SC0_SHFT                            0x5
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_AD6_TO_SC0_BMSK                             0x10
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_AD6_TO_SC0_SHFT                              0x4
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_SC0_BMSK                             0x8
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_PPSS_TO_SC0_SHFT                             0x3
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_A11_TO_SC0_BMSK                              0x4
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_A11_TO_SC0_SHFT                              0x2
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_SC0_BMSK                              0x2
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_RPM_TO_SC0_SHFT                              0x1
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC1_TO_SC0_BMSK                              0x1
#define HWIO_TLMM_DBG_RESUME_XTRIG_PERM_SC1_TO_SC0_SHFT                                0

#define HWIO_TLMM_DBG_STATUS_ADDR(nBase)                                             ((nBase)      + 0x00002028)
#define HWIO_TLMM_DBG_STATUS_RMSK                                                  0x3ff
#define HWIO_TLMM_DBG_STATUS_IN(nBase)                                              \
        in_dword_masked(HWIO_TLMM_DBG_STATUS_ADDR(nBase), HWIO_TLMM_DBG_STATUS_RMSK)
#define HWIO_TLMM_DBG_STATUS_INM(nBase,m)                                           \
        in_dword_masked(HWIO_TLMM_DBG_STATUS_ADDR(nBase), m)
#define HWIO_TLMM_DBG_STATUS_AD6_MCD_BREAKEVENT_BMSK                               0x200
#define HWIO_TLMM_DBG_STATUS_AD6_MCD_BREAKEVENT_SHFT                                 0x9
#define HWIO_TLMM_DBG_STATUS_A11_DBGACK_BMSK                                       0x100
#define HWIO_TLMM_DBG_STATUS_A11_DBGACK_SHFT                                         0x8
#define HWIO_TLMM_DBG_STATUS_PPSS_JTAG_DBGACK_BMSK                                  0x80
#define HWIO_TLMM_DBG_STATUS_PPSS_JTAG_DBGACK_SHFT                                   0x7
#define HWIO_TLMM_DBG_STATUS_RPM_DBG_ACK_BMSK                                       0x40
#define HWIO_TLMM_DBG_STATUS_RPM_DBG_ACK_SHFT                                        0x6
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU1_ACK_BMSK                                   0x20
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU1_ACK_SHFT                                    0x5
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU1_DONE_BMSK                                  0x10
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU1_DONE_SHFT                                   0x4
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU1_TRIGGER_BMSK                                0x8
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU1_TRIGGER_SHFT                                0x3
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU0_ACK_BMSK                                    0x4
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU0_ACK_SHFT                                    0x2
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU0_DONE_BMSK                                   0x2
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU0_DONE_SHFT                                   0x1
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU0_TRIGGER_BMSK                                0x1
#define HWIO_TLMM_DBG_STATUS_SC_DBG_CPU0_TRIGGER_SHFT                                  0

#define HWIO_CHIP_MODE_ADDR(nBase)                                                   ((nBase)      + 0x00002048)
#define HWIO_CHIP_MODE_RMSK                                                          0x3
#define HWIO_CHIP_MODE_IN(nBase)                                                    \
        in_dword_masked(HWIO_CHIP_MODE_ADDR(nBase), HWIO_CHIP_MODE_RMSK)
#define HWIO_CHIP_MODE_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_CHIP_MODE_ADDR(nBase), m)
#define HWIO_CHIP_MODE_MODE1_PIN_BMSK                                                0x2
#define HWIO_CHIP_MODE_MODE1_PIN_SHFT                                                0x1
#define HWIO_CHIP_MODE_MODE0_PIN_BMSK                                                0x1
#define HWIO_CHIP_MODE_MODE0_PIN_SHFT                                                  0

#define HWIO_TLMM_SPARE_ADDR(nBase)                                                  ((nBase)      + 0x0000204c)
#define HWIO_TLMM_SPARE_RMSK                                                        0xff
#define HWIO_TLMM_SPARE_IN(nBase)                                                   \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR(nBase), HWIO_TLMM_SPARE_RMSK)
#define HWIO_TLMM_SPARE_INM(nBase,m)                                                \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR(nBase), m)
#define HWIO_TLMM_SPARE_OUT(nBase,v)                                                \
        out_dword(HWIO_TLMM_SPARE_ADDR(nBase),v)
#define HWIO_TLMM_SPARE_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR(nBase),m,v,HWIO_TLMM_SPARE_IN);
#define HWIO_TLMM_SPARE_MISC_BMSK                                                   0xff
#define HWIO_TLMM_SPARE_MISC_SHFT                                                      0

#define HWIO_HW_REVISION_NUMBER_ADDR(nBase)                                          ((nBase)      + 0x00002054)
#define HWIO_HW_REVISION_NUMBER_RMSK                                          0xffffffff
#define HWIO_HW_REVISION_NUMBER_IN(nBase)                                           \
        in_dword_masked(HWIO_HW_REVISION_NUMBER_ADDR(nBase), HWIO_HW_REVISION_NUMBER_RMSK)
#define HWIO_HW_REVISION_NUMBER_INM(nBase,m)                                        \
        in_dword_masked(HWIO_HW_REVISION_NUMBER_ADDR(nBase), m)
#define HWIO_HW_REVISION_NUMBER_VERSION_ID_BMSK                               0xf0000000
#define HWIO_HW_REVISION_NUMBER_VERSION_ID_SHFT                                     0x1c
#define HWIO_HW_REVISION_NUMBER_PARTNUM_BMSK                                   0xffff000
#define HWIO_HW_REVISION_NUMBER_PARTNUM_SHFT                                         0xc
#define HWIO_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                               0xffe
#define HWIO_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                                 0x1
#define HWIO_HW_REVISION_NUMBER_START_BIT_BMSK                                       0x1
#define HWIO_HW_REVISION_NUMBER_START_BIT_SHFT                                         0

#define HWIO_RGB_IF_SEL_ADDR(nBase)                                                  ((nBase)      + 0x00002064)
#define HWIO_RGB_IF_SEL_RMSK                                                         0x7
#define HWIO_RGB_IF_SEL_IN(nBase)                                                   \
        in_dword_masked(HWIO_RGB_IF_SEL_ADDR(nBase), HWIO_RGB_IF_SEL_RMSK)
#define HWIO_RGB_IF_SEL_INM(nBase,m)                                                \
        in_dword_masked(HWIO_RGB_IF_SEL_ADDR(nBase), m)
#define HWIO_RGB_IF_SEL_OUT(nBase,v)                                                \
        out_dword(HWIO_RGB_IF_SEL_ADDR(nBase),v)
#define HWIO_RGB_IF_SEL_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_RGB_IF_SEL_ADDR(nBase),m,v,HWIO_RGB_IF_SEL_IN);
#define HWIO_RGB_IF_SEL_DSUB_LCDC_CGC_EN_BMSK                                        0x4
#define HWIO_RGB_IF_SEL_DSUB_LCDC_CGC_EN_SHFT                                        0x2
#define HWIO_RGB_IF_SEL_RGB_SEL_BMSK                                                 0x3
#define HWIO_RGB_IF_SEL_RGB_SEL_SHFT                                                   0

#define HWIO_GP_PDM0_GPIO_PORT_SEL_ADDR(nBase)                                       ((nBase)      + 0x00002068)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_RMSK                                              0x1
#define HWIO_GP_PDM0_GPIO_PORT_SEL_IN(nBase)                                        \
        in_dword_masked(HWIO_GP_PDM0_GPIO_PORT_SEL_ADDR(nBase), HWIO_GP_PDM0_GPIO_PORT_SEL_RMSK)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_INM(nBase,m)                                     \
        in_dword_masked(HWIO_GP_PDM0_GPIO_PORT_SEL_ADDR(nBase), m)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_OUT(nBase,v)                                     \
        out_dword(HWIO_GP_PDM0_GPIO_PORT_SEL_ADDR(nBase),v)
#define HWIO_GP_PDM0_GPIO_PORT_SEL_OUTM(nBase,m,v)                                  \
        out_dword_masked_ns(HWIO_GP_PDM0_GPIO_PORT_SEL_ADDR(nBase),m,v,HWIO_GP_PDM0_GPIO_PORT_SEL_IN);
#define HWIO_GP_PDM0_GPIO_PORT_SEL_GP_PDM0_SEL_BMSK                                  0x1
#define HWIO_GP_PDM0_GPIO_PORT_SEL_GP_PDM0_SEL_SHFT                                    0

#define HWIO_GP_PDM1_GPIO_PORT_SEL_ADDR(nBase)                                       ((nBase)      + 0x0000206c)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_RMSK                                              0x1
#define HWIO_GP_PDM1_GPIO_PORT_SEL_IN(nBase)                                        \
        in_dword_masked(HWIO_GP_PDM1_GPIO_PORT_SEL_ADDR(nBase), HWIO_GP_PDM1_GPIO_PORT_SEL_RMSK)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_INM(nBase,m)                                     \
        in_dword_masked(HWIO_GP_PDM1_GPIO_PORT_SEL_ADDR(nBase), m)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_OUT(nBase,v)                                     \
        out_dword(HWIO_GP_PDM1_GPIO_PORT_SEL_ADDR(nBase),v)
#define HWIO_GP_PDM1_GPIO_PORT_SEL_OUTM(nBase,m,v)                                  \
        out_dword_masked_ns(HWIO_GP_PDM1_GPIO_PORT_SEL_ADDR(nBase),m,v,HWIO_GP_PDM1_GPIO_PORT_SEL_IN);
#define HWIO_GP_PDM1_GPIO_PORT_SEL_GP_PDM1_SEL_BMSK                                  0x1
#define HWIO_GP_PDM1_GPIO_PORT_SEL_GP_PDM1_SEL_SHFT                                    0

#define HWIO_GP_PDM2_GPIO_PORT_SEL_ADDR(nBase)                                       ((nBase)      + 0x00002070)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_RMSK                                              0x1
#define HWIO_GP_PDM2_GPIO_PORT_SEL_IN(nBase)                                        \
        in_dword_masked(HWIO_GP_PDM2_GPIO_PORT_SEL_ADDR(nBase), HWIO_GP_PDM2_GPIO_PORT_SEL_RMSK)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_INM(nBase,m)                                     \
        in_dword_masked(HWIO_GP_PDM2_GPIO_PORT_SEL_ADDR(nBase), m)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_OUT(nBase,v)                                     \
        out_dword(HWIO_GP_PDM2_GPIO_PORT_SEL_ADDR(nBase),v)
#define HWIO_GP_PDM2_GPIO_PORT_SEL_OUTM(nBase,m,v)                                  \
        out_dword_masked_ns(HWIO_GP_PDM2_GPIO_PORT_SEL_ADDR(nBase),m,v,HWIO_GP_PDM2_GPIO_PORT_SEL_IN);
#define HWIO_GP_PDM2_GPIO_PORT_SEL_GP_PDM2_SEL_BMSK                                  0x1
#define HWIO_GP_PDM2_GPIO_PORT_SEL_GP_PDM2_SEL_SHFT                                    0

#define HWIO_RESOUT_HDRV_CTL_ADDR(nBase)                                             ((nBase)      + 0x00002080)
#define HWIO_RESOUT_HDRV_CTL_RMSK                                                    0x7
#define HWIO_RESOUT_HDRV_CTL_IN(nBase)                                              \
        in_dword_masked(HWIO_RESOUT_HDRV_CTL_ADDR(nBase), HWIO_RESOUT_HDRV_CTL_RMSK)
#define HWIO_RESOUT_HDRV_CTL_INM(nBase,m)                                           \
        in_dword_masked(HWIO_RESOUT_HDRV_CTL_ADDR(nBase), m)
#define HWIO_RESOUT_HDRV_CTL_OUT(nBase,v)                                           \
        out_dword(HWIO_RESOUT_HDRV_CTL_ADDR(nBase),v)
#define HWIO_RESOUT_HDRV_CTL_OUTM(nBase,m,v)                                        \
        out_dword_masked_ns(HWIO_RESOUT_HDRV_CTL_ADDR(nBase),m,v,HWIO_RESOUT_HDRV_CTL_IN);
#define HWIO_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                                      0x7
#define HWIO_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                                        0

#define HWIO_JTAG_HDRV_CTL_ADDR(nBase)                                               ((nBase)      + 0x00002084)
#define HWIO_JTAG_HDRV_CTL_RMSK                                                     0x3f
#define HWIO_JTAG_HDRV_CTL_IN(nBase)                                                \
        in_dword_masked(HWIO_JTAG_HDRV_CTL_ADDR(nBase), HWIO_JTAG_HDRV_CTL_RMSK)
#define HWIO_JTAG_HDRV_CTL_INM(nBase,m)                                             \
        in_dword_masked(HWIO_JTAG_HDRV_CTL_ADDR(nBase), m)
#define HWIO_JTAG_HDRV_CTL_OUT(nBase,v)                                             \
        out_dword(HWIO_JTAG_HDRV_CTL_ADDR(nBase),v)
#define HWIO_JTAG_HDRV_CTL_OUTM(nBase,m,v)                                          \
        out_dword_masked_ns(HWIO_JTAG_HDRV_CTL_ADDR(nBase),m,v,HWIO_JTAG_HDRV_CTL_IN);
#define HWIO_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                            0x38
#define HWIO_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                             0x3
#define HWIO_JTAG_HDRV_CTL_RTCK_HDRV_BMSK                                            0x7
#define HWIO_JTAG_HDRV_CTL_RTCK_HDRV_SHFT                                              0

#define HWIO_PMIC_HDRV_PULL_CTL_ADDR(nBase)                                          ((nBase)      + 0x0000208c)
#define HWIO_PMIC_HDRV_PULL_CTL_RMSK                                              0xffff
#define HWIO_PMIC_HDRV_PULL_CTL_IN(nBase)                                           \
        in_dword_masked(HWIO_PMIC_HDRV_PULL_CTL_ADDR(nBase), HWIO_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_PMIC_HDRV_PULL_CTL_INM(nBase,m)                                        \
        in_dword_masked(HWIO_PMIC_HDRV_PULL_CTL_ADDR(nBase), m)
#define HWIO_PMIC_HDRV_PULL_CTL_OUT(nBase,v)                                        \
        out_dword(HWIO_PMIC_HDRV_PULL_CTL_ADDR(nBase),v)
#define HWIO_PMIC_HDRV_PULL_CTL_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_PMIC_HDRV_PULL_CTL_ADDR(nBase),m,v,HWIO_PMIC_HDRV_PULL_CTL_IN);
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC1_PULL_BMSK                              0xc000
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC1_PULL_SHFT                                 0xe
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC2_PULL_BMSK                              0x3000
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC2_PULL_SHFT                                 0xc
#define HWIO_PMIC_HDRV_PULL_CTL_CXO_EN_PA_ON_HDRV_BMSK                             0xe00
#define HWIO_PMIC_HDRV_PULL_CTL_CXO_EN_PA_ON_HDRV_SHFT                               0x9
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC_FWD_CLK_HDRV_BMSK                        0x1c0
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC_FWD_CLK_HDRV_SHFT                          0x6
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC1_HDRV_BMSK                                0x38
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC1_HDRV_SHFT                                 0x3
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC2_HDRV_BMSK                                 0x7
#define HWIO_PMIC_HDRV_PULL_CTL_SSBI_PMIC2_HDRV_SHFT                                   0

#define HWIO_RF_HDRV_PULL_CTL_ADDR(nBase)                                            ((nBase)      + 0x00002090)
#define HWIO_RF_HDRV_PULL_CTL_RMSK                                             0x7ffffff
#define HWIO_RF_HDRV_PULL_CTL_IN(nBase)                                             \
        in_dword_masked(HWIO_RF_HDRV_PULL_CTL_ADDR(nBase), HWIO_RF_HDRV_PULL_CTL_RMSK)
#define HWIO_RF_HDRV_PULL_CTL_INM(nBase,m)                                          \
        in_dword_masked(HWIO_RF_HDRV_PULL_CTL_ADDR(nBase), m)
#define HWIO_RF_HDRV_PULL_CTL_OUT(nBase,v)                                          \
        out_dword(HWIO_RF_HDRV_PULL_CTL_ADDR(nBase),v)
#define HWIO_RF_HDRV_PULL_CTL_OUTM(nBase,m,v)                                       \
        out_dword_masked_ns(HWIO_RF_HDRV_PULL_CTL_ADDR(nBase),m,v,HWIO_RF_HDRV_PULL_CTL_IN);
#define HWIO_RF_HDRV_PULL_CTL_SSBI1_RTR_PULL_BMSK                              0x6000000
#define HWIO_RF_HDRV_PULL_CTL_SSBI1_RTR_PULL_SHFT                                   0x19
#define HWIO_RF_HDRV_PULL_CTL_SSBI2_RTR_PULL_BMSK                              0x1800000
#define HWIO_RF_HDRV_PULL_CTL_SSBI2_RTR_PULL_SHFT                                   0x17
#define HWIO_RF_HDRV_PULL_CTL_SSBI1_RTR_HDRV_BMSK                               0x700000
#define HWIO_RF_HDRV_PULL_CTL_SSBI1_RTR_HDRV_SHFT                                   0x14
#define HWIO_RF_HDRV_PULL_CTL_SSBI2_RTR_HDRV_BMSK                                0xe0000
#define HWIO_RF_HDRV_PULL_CTL_SSBI2_RTR_HDRV_SHFT                                   0x11
#define HWIO_RF_HDRV_PULL_CTL_TX_ON_HDRV_BMSK                                    0x1c000
#define HWIO_RF_HDRV_PULL_CTL_TX_ON_HDRV_SHFT                                        0xe
#define HWIO_RF_HDRV_PULL_CTL_PA_RANGE1_HDRV_BMSK                                 0x3800
#define HWIO_RF_HDRV_PULL_CTL_PA_RANGE1_HDRV_SHFT                                    0xb
#define HWIO_RF_HDRV_PULL_CTL_PA_RANGE0_HDRV_BMSK                                  0x700
#define HWIO_RF_HDRV_PULL_CTL_PA_RANGE0_HDRV_SHFT                                    0x8
#define HWIO_RF_HDRV_PULL_CTL_GSM_TX_CLK_PULL_BMSK                                  0xc0
#define HWIO_RF_HDRV_PULL_CTL_GSM_TX_CLK_PULL_SHFT                                   0x6
#define HWIO_RF_HDRV_PULL_CTL_GSM_TX_PHASE_D_HDRV_BMSK                              0x38
#define HWIO_RF_HDRV_PULL_CTL_GSM_TX_PHASE_D_HDRV_SHFT                               0x3
#define HWIO_RF_HDRV_PULL_CTL_GSM_TX_LB_CLK_HDRV_BMSK                                0x7
#define HWIO_RF_HDRV_PULL_CTL_GSM_TX_LB_CLK_HDRV_SHFT                                  0

#define HWIO_SDC4_HDRV_PULL_CTL_ADDR(nBase)                                          ((nBase)      + 0x000020a0)
#define HWIO_SDC4_HDRV_PULL_CTL_RMSK                                              0x1fff
#define HWIO_SDC4_HDRV_PULL_CTL_IN(nBase)                                           \
        in_dword_masked(HWIO_SDC4_HDRV_PULL_CTL_ADDR(nBase), HWIO_SDC4_HDRV_PULL_CTL_RMSK)
#define HWIO_SDC4_HDRV_PULL_CTL_INM(nBase,m)                                        \
        in_dword_masked(HWIO_SDC4_HDRV_PULL_CTL_ADDR(nBase), m)
#define HWIO_SDC4_HDRV_PULL_CTL_OUT(nBase,v)                                        \
        out_dword(HWIO_SDC4_HDRV_PULL_CTL_ADDR(nBase),v)
#define HWIO_SDC4_HDRV_PULL_CTL_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_SDC4_HDRV_PULL_CTL_ADDR(nBase),m,v,HWIO_SDC4_HDRV_PULL_CTL_IN);
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_CMD_PULL_BMSK                                0x1800
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_CMD_PULL_SHFT                                   0xb
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_DATA_PULL_BMSK                                0x600
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_DATA_PULL_SHFT                                  0x9
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_CLK_HDRV_BMSK                                 0x1c0
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_CLK_HDRV_SHFT                                   0x6
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_CMD_HDRV_BMSK                                  0x38
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_CMD_HDRV_SHFT                                   0x3
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_DATA_HDRV_BMSK                                  0x7
#define HWIO_SDC4_HDRV_PULL_CTL_SDC4_DATA_HDRV_SHFT                                    0

#define HWIO_SDC3_HDRV_PULL_CTL_ADDR(nBase)                                          ((nBase)      + 0x000020a4)
#define HWIO_SDC3_HDRV_PULL_CTL_RMSK                                              0x1fff
#define HWIO_SDC3_HDRV_PULL_CTL_IN(nBase)                                           \
        in_dword_masked(HWIO_SDC3_HDRV_PULL_CTL_ADDR(nBase), HWIO_SDC3_HDRV_PULL_CTL_RMSK)
#define HWIO_SDC3_HDRV_PULL_CTL_INM(nBase,m)                                        \
        in_dword_masked(HWIO_SDC3_HDRV_PULL_CTL_ADDR(nBase), m)
#define HWIO_SDC3_HDRV_PULL_CTL_OUT(nBase,v)                                        \
        out_dword(HWIO_SDC3_HDRV_PULL_CTL_ADDR(nBase),v)
#define HWIO_SDC3_HDRV_PULL_CTL_OUTM(nBase,m,v)                                     \
        out_dword_masked_ns(HWIO_SDC3_HDRV_PULL_CTL_ADDR(nBase),m,v,HWIO_SDC3_HDRV_PULL_CTL_IN);
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_CMD_PULL_BMSK                                0x1800
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_CMD_PULL_SHFT                                   0xb
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_DATA_PULL_BMSK                                0x600
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_DATA_PULL_SHFT                                  0x9
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_CLK_HDRV_BMSK                                 0x1c0
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_CLK_HDRV_SHFT                                   0x6
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_CMD_HDRV_BMSK                                  0x38
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_CMD_HDRV_SHFT                                   0x3
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_DATA_HDRV_BMSK                                  0x7
#define HWIO_SDC3_HDRV_PULL_CTL_SDC3_DATA_HDRV_SHFT                                    0

#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_ADDR(nBase)                                    ((nBase)      + 0x000020a8)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_RMSK                                        0x1fff
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_IN(nBase)                                     \
        in_dword_masked(HWIO_CODEC_SPKR_HDRV_PULL_CTL_ADDR(nBase), HWIO_CODEC_SPKR_HDRV_PULL_CTL_RMSK)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_INM(nBase,m)                                  \
        in_dword_masked(HWIO_CODEC_SPKR_HDRV_PULL_CTL_ADDR(nBase), m)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_OUT(nBase,v)                                  \
        out_dword(HWIO_CODEC_SPKR_HDRV_PULL_CTL_ADDR(nBase),v)
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_OUTM(nBase,m,v)                               \
        out_dword_masked_ns(HWIO_CODEC_SPKR_HDRV_PULL_CTL_ADDR(nBase),m,v,HWIO_CODEC_SPKR_HDRV_PULL_CTL_IN);
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_SCK_PULL_BMSK                    0x1800
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_SCK_PULL_SHFT                       0xb
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_WS_PULL_BMSK                      0x600
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_WS_PULL_SHFT                        0x9
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_SCK_HDRV_BMSK                     0x1c0
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_SCK_HDRV_SHFT                       0x6
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_WS_HDRV_BMSK                       0x38
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_WS_HDRV_SHFT                        0x3
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_DOUT_HDRV_BMSK                      0x7
#define HWIO_CODEC_SPKR_HDRV_PULL_CTL_CODEC_SPKR_DOUT_HDRV_SHFT                        0

#define HWIO_CODEC_MIC_HDRV_PULL_CTL_ADDR(nBase)                                     ((nBase)      + 0x000020ac)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_RMSK                                          0xfff
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_IN(nBase)                                      \
        in_dword_masked(HWIO_CODEC_MIC_HDRV_PULL_CTL_ADDR(nBase), HWIO_CODEC_MIC_HDRV_PULL_CTL_RMSK)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_INM(nBase,m)                                   \
        in_dword_masked(HWIO_CODEC_MIC_HDRV_PULL_CTL_ADDR(nBase), m)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_OUT(nBase,v)                                   \
        out_dword(HWIO_CODEC_MIC_HDRV_PULL_CTL_ADDR(nBase),v)
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_OUTM(nBase,m,v)                                \
        out_dword_masked_ns(HWIO_CODEC_MIC_HDRV_PULL_CTL_ADDR(nBase),m,v,HWIO_CODEC_MIC_HDRV_PULL_CTL_IN);
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_SCK_PULL_BMSK                       0xc00
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_SCK_PULL_SHFT                         0xa
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_WS_PULL_BMSK                        0x300
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_WS_PULL_SHFT                          0x8
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_DIN0_PULL_BMSK                       0xc0
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_DIN0_PULL_SHFT                        0x6
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_SCK_HDRV_BMSK                        0x38
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_SCK_HDRV_SHFT                         0x3
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_WS_HDRV_BMSK                          0x7
#define HWIO_CODEC_MIC_HDRV_PULL_CTL_CODEC_MIC_WS_HDRV_SHFT                            0

#define HWIO_MODE_PULL_CTL_ADDR(nBase)                                               ((nBase)      + 0x000020b4)
#define HWIO_MODE_PULL_CTL_RMSK                                                      0xf
#define HWIO_MODE_PULL_CTL_IN(nBase)                                                \
        in_dword_masked(HWIO_MODE_PULL_CTL_ADDR(nBase), HWIO_MODE_PULL_CTL_RMSK)
#define HWIO_MODE_PULL_CTL_INM(nBase,m)                                             \
        in_dword_masked(HWIO_MODE_PULL_CTL_ADDR(nBase), m)
#define HWIO_MODE_PULL_CTL_OUT(nBase,v)                                             \
        out_dword(HWIO_MODE_PULL_CTL_ADDR(nBase),v)
#define HWIO_MODE_PULL_CTL_OUTM(nBase,m,v)                                          \
        out_dword_masked_ns(HWIO_MODE_PULL_CTL_ADDR(nBase),m,v,HWIO_MODE_PULL_CTL_IN);
#define HWIO_MODE_PULL_CTL_MODE_1_PULL_BMSK                                          0xc
#define HWIO_MODE_PULL_CTL_MODE_1_PULL_SHFT                                          0x2
#define HWIO_MODE_PULL_CTL_MODE_0_PULL_BMSK                                          0x3
#define HWIO_MODE_PULL_CTL_MODE_0_PULL_SHFT                                            0

#define HWIO_GPIO_OUT_0_ADDR(nBase)                                                  ((nBase)      + 0x00003000)
#define HWIO_GPIO_OUT_0_RMSK                                                  0xffffffff
#define HWIO_GPIO_OUT_0_IN(nBase)                                                   \
        in_dword_masked(HWIO_GPIO_OUT_0_ADDR(nBase), HWIO_GPIO_OUT_0_RMSK)
#define HWIO_GPIO_OUT_0_INM(nBase,m)                                                \
        in_dword_masked(HWIO_GPIO_OUT_0_ADDR(nBase), m)
#define HWIO_GPIO_OUT_0_OUT(nBase,v)                                                \
        out_dword(HWIO_GPIO_OUT_0_ADDR(nBase),v)
#define HWIO_GPIO_OUT_0_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_GPIO_OUT_0_ADDR(nBase),m,v,HWIO_GPIO_OUT_0_IN);
#define HWIO_GPIO_OUT_0_GPIO_OUT_BMSK                                         0xffffffff
#define HWIO_GPIO_OUT_0_GPIO_OUT_SHFT                                                  0

#define HWIO_GPIO_OUT_1_ADDR(nBase)                                                  ((nBase)      + 0x00003004)
#define HWIO_GPIO_OUT_1_RMSK                                                  0xffffffff
#define HWIO_GPIO_OUT_1_IN(nBase)                                                   \
        in_dword_masked(HWIO_GPIO_OUT_1_ADDR(nBase), HWIO_GPIO_OUT_1_RMSK)
#define HWIO_GPIO_OUT_1_INM(nBase,m)                                                \
        in_dword_masked(HWIO_GPIO_OUT_1_ADDR(nBase), m)
#define HWIO_GPIO_OUT_1_OUT(nBase,v)                                                \
        out_dword(HWIO_GPIO_OUT_1_ADDR(nBase),v)
#define HWIO_GPIO_OUT_1_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_GPIO_OUT_1_ADDR(nBase),m,v,HWIO_GPIO_OUT_1_IN);
#define HWIO_GPIO_OUT_1_GPIO_OUT_BMSK                                         0xffffffff
#define HWIO_GPIO_OUT_1_GPIO_OUT_SHFT                                                  0

#define HWIO_GPIO_OUT_2_ADDR(nBase)                                                  ((nBase)      + 0x00003008)
#define HWIO_GPIO_OUT_2_RMSK                                                  0xffffffff
#define HWIO_GPIO_OUT_2_IN(nBase)                                                   \
        in_dword_masked(HWIO_GPIO_OUT_2_ADDR(nBase), HWIO_GPIO_OUT_2_RMSK)
#define HWIO_GPIO_OUT_2_INM(nBase,m)                                                \
        in_dword_masked(HWIO_GPIO_OUT_2_ADDR(nBase), m)
#define HWIO_GPIO_OUT_2_OUT(nBase,v)                                                \
        out_dword(HWIO_GPIO_OUT_2_ADDR(nBase),v)
#define HWIO_GPIO_OUT_2_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_GPIO_OUT_2_ADDR(nBase),m,v,HWIO_GPIO_OUT_2_IN);
#define HWIO_GPIO_OUT_2_GPIO_OUT_BMSK                                         0xffffffff
#define HWIO_GPIO_OUT_2_GPIO_OUT_SHFT                                                  0

#define HWIO_GPIO_OUT_3_ADDR(nBase)                                                  ((nBase)      + 0x0000300c)
#define HWIO_GPIO_OUT_3_RMSK                                                  0xffffffff
#define HWIO_GPIO_OUT_3_IN(nBase)                                                   \
        in_dword_masked(HWIO_GPIO_OUT_3_ADDR(nBase), HWIO_GPIO_OUT_3_RMSK)
#define HWIO_GPIO_OUT_3_INM(nBase,m)                                                \
        in_dword_masked(HWIO_GPIO_OUT_3_ADDR(nBase), m)
#define HWIO_GPIO_OUT_3_OUT(nBase,v)                                                \
        out_dword(HWIO_GPIO_OUT_3_ADDR(nBase),v)
#define HWIO_GPIO_OUT_3_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_GPIO_OUT_3_ADDR(nBase),m,v,HWIO_GPIO_OUT_3_IN);
#define HWIO_GPIO_OUT_3_GPIO_OUT_BMSK                                         0xffffffff
#define HWIO_GPIO_OUT_3_GPIO_OUT_SHFT                                                  0

#define HWIO_GPIO_OUT_4_ADDR(nBase)                                                  ((nBase)      + 0x00003010)
#define HWIO_GPIO_OUT_4_RMSK                                                  0xffffffff
#define HWIO_GPIO_OUT_4_IN(nBase)                                                   \
        in_dword_masked(HWIO_GPIO_OUT_4_ADDR(nBase), HWIO_GPIO_OUT_4_RMSK)
#define HWIO_GPIO_OUT_4_INM(nBase,m)                                                \
        in_dword_masked(HWIO_GPIO_OUT_4_ADDR(nBase), m)
#define HWIO_GPIO_OUT_4_OUT(nBase,v)                                                \
        out_dword(HWIO_GPIO_OUT_4_ADDR(nBase),v)
#define HWIO_GPIO_OUT_4_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_GPIO_OUT_4_ADDR(nBase),m,v,HWIO_GPIO_OUT_4_IN);
#define HWIO_GPIO_OUT_4_GPIO_OUT_BMSK                                         0xffffffff
#define HWIO_GPIO_OUT_4_GPIO_OUT_SHFT                                                  0

#define HWIO_GPIO_OUT_5_ADDR(nBase)                                                  ((nBase)      + 0x00003014)
#define HWIO_GPIO_OUT_5_RMSK                                                      0x1fff
#define HWIO_GPIO_OUT_5_IN(nBase)                                                   \
        in_dword_masked(HWIO_GPIO_OUT_5_ADDR(nBase), HWIO_GPIO_OUT_5_RMSK)
#define HWIO_GPIO_OUT_5_INM(nBase,m)                                                \
        in_dword_masked(HWIO_GPIO_OUT_5_ADDR(nBase), m)
#define HWIO_GPIO_OUT_5_OUT(nBase,v)                                                \
        out_dword(HWIO_GPIO_OUT_5_ADDR(nBase),v)
#define HWIO_GPIO_OUT_5_OUTM(nBase,m,v)                                             \
        out_dword_masked_ns(HWIO_GPIO_OUT_5_ADDR(nBase),m,v,HWIO_GPIO_OUT_5_IN);
#define HWIO_GPIO_OUT_5_GPIO_OUT_BMSK                                             0x1fff
#define HWIO_GPIO_OUT_5_GPIO_OUT_SHFT                                                  0

#define HWIO_GPIO_OUT_CLR_0_ADDR(nBase)                                              ((nBase)      + 0x00003020)
#define HWIO_GPIO_OUT_CLR_0_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_CLR_0_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_CLR_0_ADDR(nBase),v)
#define HWIO_GPIO_OUT_CLR_0_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_CLR_0_ADDR(nBase),m,v,HWIO_GPIO_OUT_CLR_0_shadow)
#define HWIO_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                                          0

#define HWIO_GPIO_OUT_CLR_1_ADDR(nBase)                                              ((nBase)      + 0x00003024)
#define HWIO_GPIO_OUT_CLR_1_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_CLR_1_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_CLR_1_ADDR(nBase),v)
#define HWIO_GPIO_OUT_CLR_1_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_CLR_1_ADDR(nBase),m,v,HWIO_GPIO_OUT_CLR_1_shadow)
#define HWIO_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                                          0

#define HWIO_GPIO_OUT_CLR_2_ADDR(nBase)                                              ((nBase)      + 0x00003028)
#define HWIO_GPIO_OUT_CLR_2_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_CLR_2_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_CLR_2_ADDR(nBase),v)
#define HWIO_GPIO_OUT_CLR_2_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_CLR_2_ADDR(nBase),m,v,HWIO_GPIO_OUT_CLR_2_shadow)
#define HWIO_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                                          0

#define HWIO_GPIO_OUT_CLR_3_ADDR(nBase)                                              ((nBase)      + 0x0000302c)
#define HWIO_GPIO_OUT_CLR_3_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_CLR_3_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_CLR_3_ADDR(nBase),v)
#define HWIO_GPIO_OUT_CLR_3_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_CLR_3_ADDR(nBase),m,v,HWIO_GPIO_OUT_CLR_3_shadow)
#define HWIO_GPIO_OUT_CLR_3_GPIO_OUT_CLR_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_CLR_3_GPIO_OUT_CLR_SHFT                                          0

#define HWIO_GPIO_OUT_CLR_4_ADDR(nBase)                                              ((nBase)      + 0x00003030)
#define HWIO_GPIO_OUT_CLR_4_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_CLR_4_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_CLR_4_ADDR(nBase),v)
#define HWIO_GPIO_OUT_CLR_4_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_CLR_4_ADDR(nBase),m,v,HWIO_GPIO_OUT_CLR_4_shadow)
#define HWIO_GPIO_OUT_CLR_4_GPIO_OUT_CLR_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_CLR_4_GPIO_OUT_CLR_SHFT                                          0

#define HWIO_GPIO_OUT_CLR_5_ADDR(nBase)                                              ((nBase)      + 0x00003034)
#define HWIO_GPIO_OUT_CLR_5_RMSK                                                  0x1fff
#define HWIO_GPIO_OUT_CLR_5_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_CLR_5_ADDR(nBase),v)
#define HWIO_GPIO_OUT_CLR_5_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_CLR_5_ADDR(nBase),m,v,HWIO_GPIO_OUT_CLR_5_shadow)
#define HWIO_GPIO_OUT_CLR_5_GPIO_OUT_CLR_BMSK                                     0x1fff
#define HWIO_GPIO_OUT_CLR_5_GPIO_OUT_CLR_SHFT                                          0

#define HWIO_GPIO_OUT_SET_0_ADDR(nBase)                                              ((nBase)      + 0x00003040)
#define HWIO_GPIO_OUT_SET_0_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_SET_0_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_SET_0_ADDR(nBase),v)
#define HWIO_GPIO_OUT_SET_0_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_SET_0_ADDR(nBase),m,v,HWIO_GPIO_OUT_SET_0_shadow)
#define HWIO_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                                          0

#define HWIO_GPIO_OUT_SET_1_ADDR(nBase)                                              ((nBase)      + 0x00003044)
#define HWIO_GPIO_OUT_SET_1_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_SET_1_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_SET_1_ADDR(nBase),v)
#define HWIO_GPIO_OUT_SET_1_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_SET_1_ADDR(nBase),m,v,HWIO_GPIO_OUT_SET_1_shadow)
#define HWIO_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                                          0

#define HWIO_GPIO_OUT_SET_2_ADDR(nBase)                                              ((nBase)      + 0x00003048)
#define HWIO_GPIO_OUT_SET_2_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_SET_2_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_SET_2_ADDR(nBase),v)
#define HWIO_GPIO_OUT_SET_2_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_SET_2_ADDR(nBase),m,v,HWIO_GPIO_OUT_SET_2_shadow)
#define HWIO_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                                          0

#define HWIO_GPIO_OUT_SET_3_ADDR(nBase)                                              ((nBase)      + 0x0000304c)
#define HWIO_GPIO_OUT_SET_3_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_SET_3_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_SET_3_ADDR(nBase),v)
#define HWIO_GPIO_OUT_SET_3_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_SET_3_ADDR(nBase),m,v,HWIO_GPIO_OUT_SET_3_shadow)
#define HWIO_GPIO_OUT_SET_3_GPIO_OUT_SET_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_SET_3_GPIO_OUT_SET_SHFT                                          0

#define HWIO_GPIO_OUT_SET_4_ADDR(nBase)                                              ((nBase)      + 0x00003050)
#define HWIO_GPIO_OUT_SET_4_RMSK                                              0xffffffff
#define HWIO_GPIO_OUT_SET_4_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_SET_4_ADDR(nBase),v)
#define HWIO_GPIO_OUT_SET_4_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_SET_4_ADDR(nBase),m,v,HWIO_GPIO_OUT_SET_4_shadow)
#define HWIO_GPIO_OUT_SET_4_GPIO_OUT_SET_BMSK                                 0xffffffff
#define HWIO_GPIO_OUT_SET_4_GPIO_OUT_SET_SHFT                                          0

#define HWIO_GPIO_OUT_SET_5_ADDR(nBase)                                              ((nBase)      + 0x00003054)
#define HWIO_GPIO_OUT_SET_5_RMSK                                                  0x1fff
#define HWIO_GPIO_OUT_SET_5_OUT(nBase,v)                                            \
        out_dword(HWIO_GPIO_OUT_SET_5_ADDR(nBase),v)
#define HWIO_GPIO_OUT_SET_5_OUTM(nBase,m,v)                                         \
        out_dword_masked(HWIO_GPIO_OUT_SET_5_ADDR(nBase),m,v,HWIO_GPIO_OUT_SET_5_shadow)
#define HWIO_GPIO_OUT_SET_5_GPIO_OUT_SET_BMSK                                     0x1fff
#define HWIO_GPIO_OUT_SET_5_GPIO_OUT_SET_SHFT                                          0

#define HWIO_GPIO_IN_0_ADDR(nBase)                                                   ((nBase)      + 0x00003060)
#define HWIO_GPIO_IN_0_RMSK                                                   0xffffffff
#define HWIO_GPIO_IN_0_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_IN_0_ADDR(nBase), HWIO_GPIO_IN_0_RMSK)
#define HWIO_GPIO_IN_0_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_IN_0_ADDR(nBase), m)
#define HWIO_GPIO_IN_0_GPIO_IN_BMSK                                           0xffffffff
#define HWIO_GPIO_IN_0_GPIO_IN_SHFT                                                    0

#define HWIO_GPIO_IN_1_ADDR(nBase)                                                   ((nBase)      + 0x00003064)
#define HWIO_GPIO_IN_1_RMSK                                                   0xffffffff
#define HWIO_GPIO_IN_1_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_IN_1_ADDR(nBase), HWIO_GPIO_IN_1_RMSK)
#define HWIO_GPIO_IN_1_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_IN_1_ADDR(nBase), m)
#define HWIO_GPIO_IN_1_GPIO_IN_BMSK                                           0xffffffff
#define HWIO_GPIO_IN_1_GPIO_IN_SHFT                                                    0

#define HWIO_GPIO_IN_2_ADDR(nBase)                                                   ((nBase)      + 0x00003068)
#define HWIO_GPIO_IN_2_RMSK                                                   0xffffffff
#define HWIO_GPIO_IN_2_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_IN_2_ADDR(nBase), HWIO_GPIO_IN_2_RMSK)
#define HWIO_GPIO_IN_2_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_IN_2_ADDR(nBase), m)
#define HWIO_GPIO_IN_2_GPIO_IN_BMSK                                           0xffffffff
#define HWIO_GPIO_IN_2_GPIO_IN_SHFT                                                    0

#define HWIO_GPIO_IN_3_ADDR(nBase)                                                   ((nBase)      + 0x0000306c)
#define HWIO_GPIO_IN_3_RMSK                                                   0xffffffff
#define HWIO_GPIO_IN_3_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_IN_3_ADDR(nBase), HWIO_GPIO_IN_3_RMSK)
#define HWIO_GPIO_IN_3_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_IN_3_ADDR(nBase), m)
#define HWIO_GPIO_IN_3_GPIO_IN_BMSK                                           0xffffffff
#define HWIO_GPIO_IN_3_GPIO_IN_SHFT                                                    0

#define HWIO_GPIO_IN_4_ADDR(nBase)                                                   ((nBase)      + 0x00003070)
#define HWIO_GPIO_IN_4_RMSK                                                   0xffffffff
#define HWIO_GPIO_IN_4_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_IN_4_ADDR(nBase), HWIO_GPIO_IN_4_RMSK)
#define HWIO_GPIO_IN_4_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_IN_4_ADDR(nBase), m)
#define HWIO_GPIO_IN_4_GPIO_IN_BMSK                                           0xffffffff
#define HWIO_GPIO_IN_4_GPIO_IN_SHFT                                                    0

#define HWIO_GPIO_IN_5_ADDR(nBase)                                                   ((nBase)      + 0x00003074)
#define HWIO_GPIO_IN_5_RMSK                                                       0x1fff
#define HWIO_GPIO_IN_5_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_IN_5_ADDR(nBase), HWIO_GPIO_IN_5_RMSK)
#define HWIO_GPIO_IN_5_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_IN_5_ADDR(nBase), m)
#define HWIO_GPIO_IN_5_GPIO_IN_BMSK                                               0x1fff
#define HWIO_GPIO_IN_5_GPIO_IN_SHFT                                                    0

#define HWIO_GPIO_OE_0_ADDR(nBase)                                                   ((nBase)      + 0x00003080)
#define HWIO_GPIO_OE_0_RMSK                                                   0xffffffff
#define HWIO_GPIO_OE_0_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_OE_0_ADDR(nBase), HWIO_GPIO_OE_0_RMSK)
#define HWIO_GPIO_OE_0_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_OE_0_ADDR(nBase), m)
#define HWIO_GPIO_OE_0_OUT(nBase,v)                                                 \
        out_dword(HWIO_GPIO_OE_0_ADDR(nBase),v)
#define HWIO_GPIO_OE_0_OUTM(nBase,m,v)                                              \
        out_dword_masked_ns(HWIO_GPIO_OE_0_ADDR(nBase),m,v,HWIO_GPIO_OE_0_IN);
#define HWIO_GPIO_OE_0_GPIO_OE_BMSK                                           0xffffffff
#define HWIO_GPIO_OE_0_GPIO_OE_SHFT                                                    0

#define HWIO_GPIO_OE_1_ADDR(nBase)                                                   ((nBase)      + 0x00003084)
#define HWIO_GPIO_OE_1_RMSK                                                   0xffffffff
#define HWIO_GPIO_OE_1_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_OE_1_ADDR(nBase), HWIO_GPIO_OE_1_RMSK)
#define HWIO_GPIO_OE_1_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_OE_1_ADDR(nBase), m)
#define HWIO_GPIO_OE_1_OUT(nBase,v)                                                 \
        out_dword(HWIO_GPIO_OE_1_ADDR(nBase),v)
#define HWIO_GPIO_OE_1_OUTM(nBase,m,v)                                              \
        out_dword_masked_ns(HWIO_GPIO_OE_1_ADDR(nBase),m,v,HWIO_GPIO_OE_1_IN);
#define HWIO_GPIO_OE_1_GPIO_OE_BMSK                                           0xffffffff
#define HWIO_GPIO_OE_1_GPIO_OE_SHFT                                                    0

#define HWIO_GPIO_OE_2_ADDR(nBase)                                                   ((nBase)      + 0x00003088)
#define HWIO_GPIO_OE_2_RMSK                                                   0xffffffff
#define HWIO_GPIO_OE_2_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_OE_2_ADDR(nBase), HWIO_GPIO_OE_2_RMSK)
#define HWIO_GPIO_OE_2_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_OE_2_ADDR(nBase), m)
#define HWIO_GPIO_OE_2_OUT(nBase,v)                                                 \
        out_dword(HWIO_GPIO_OE_2_ADDR(nBase),v)
#define HWIO_GPIO_OE_2_OUTM(nBase,m,v)                                              \
        out_dword_masked_ns(HWIO_GPIO_OE_2_ADDR(nBase),m,v,HWIO_GPIO_OE_2_IN);
#define HWIO_GPIO_OE_2_GPIO_OE_BMSK                                           0xffffffff
#define HWIO_GPIO_OE_2_GPIO_OE_SHFT                                                    0

#define HWIO_GPIO_OE_3_ADDR(nBase)                                                   ((nBase)      + 0x0000308c)
#define HWIO_GPIO_OE_3_RMSK                                                   0xffffffff
#define HWIO_GPIO_OE_3_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_OE_3_ADDR(nBase), HWIO_GPIO_OE_3_RMSK)
#define HWIO_GPIO_OE_3_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_OE_3_ADDR(nBase), m)
#define HWIO_GPIO_OE_3_OUT(nBase,v)                                                 \
        out_dword(HWIO_GPIO_OE_3_ADDR(nBase),v)
#define HWIO_GPIO_OE_3_OUTM(nBase,m,v)                                              \
        out_dword_masked_ns(HWIO_GPIO_OE_3_ADDR(nBase),m,v,HWIO_GPIO_OE_3_IN);
#define HWIO_GPIO_OE_3_GPIO_OE_BMSK                                           0xffffffff
#define HWIO_GPIO_OE_3_GPIO_OE_SHFT                                                    0

#define HWIO_GPIO_OE_4_ADDR(nBase)                                                   ((nBase)      + 0x00003090)
#define HWIO_GPIO_OE_4_RMSK                                                   0xffffffff
#define HWIO_GPIO_OE_4_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_OE_4_ADDR(nBase), HWIO_GPIO_OE_4_RMSK)
#define HWIO_GPIO_OE_4_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_OE_4_ADDR(nBase), m)
#define HWIO_GPIO_OE_4_OUT(nBase,v)                                                 \
        out_dword(HWIO_GPIO_OE_4_ADDR(nBase),v)
#define HWIO_GPIO_OE_4_OUTM(nBase,m,v)                                              \
        out_dword_masked_ns(HWIO_GPIO_OE_4_ADDR(nBase),m,v,HWIO_GPIO_OE_4_IN);
#define HWIO_GPIO_OE_4_GPIO_OE_BMSK                                           0xffffffff
#define HWIO_GPIO_OE_4_GPIO_OE_SHFT                                                    0

#define HWIO_GPIO_OE_5_ADDR(nBase)                                                   ((nBase)      + 0x00003094)
#define HWIO_GPIO_OE_5_RMSK                                                       0x1fff
#define HWIO_GPIO_OE_5_IN(nBase)                                                    \
        in_dword_masked(HWIO_GPIO_OE_5_ADDR(nBase), HWIO_GPIO_OE_5_RMSK)
#define HWIO_GPIO_OE_5_INM(nBase,m)                                                 \
        in_dword_masked(HWIO_GPIO_OE_5_ADDR(nBase), m)
#define HWIO_GPIO_OE_5_OUT(nBase,v)                                                 \
        out_dword(HWIO_GPIO_OE_5_ADDR(nBase),v)
#define HWIO_GPIO_OE_5_OUTM(nBase,m,v)                                              \
        out_dword_masked_ns(HWIO_GPIO_OE_5_ADDR(nBase),m,v,HWIO_GPIO_OE_5_IN);
#define HWIO_GPIO_OE_5_GPIO_OE_BMSK                                               0x1fff
#define HWIO_GPIO_OE_5_GPIO_OE_SHFT                                                    0

#define HWIO_GPIO_OE_CLR_0_ADDR(nBase)                                               ((nBase)      + 0x00003100)
#define HWIO_GPIO_OE_CLR_0_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_CLR_0_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_CLR_0_ADDR(nBase),v)
#define HWIO_GPIO_OE_CLR_0_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_CLR_0_ADDR(nBase),m,v,HWIO_GPIO_OE_CLR_0_shadow)
#define HWIO_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                            0

#define HWIO_GPIO_OE_CLR_1_ADDR(nBase)                                               ((nBase)      + 0x00003104)
#define HWIO_GPIO_OE_CLR_1_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_CLR_1_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_CLR_1_ADDR(nBase),v)
#define HWIO_GPIO_OE_CLR_1_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_CLR_1_ADDR(nBase),m,v,HWIO_GPIO_OE_CLR_1_shadow)
#define HWIO_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                            0

#define HWIO_GPIO_OE_CLR_2_ADDR(nBase)                                               ((nBase)      + 0x00003108)
#define HWIO_GPIO_OE_CLR_2_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_CLR_2_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_CLR_2_ADDR(nBase),v)
#define HWIO_GPIO_OE_CLR_2_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_CLR_2_ADDR(nBase),m,v,HWIO_GPIO_OE_CLR_2_shadow)
#define HWIO_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                            0

#define HWIO_GPIO_OE_CLR_3_ADDR(nBase)                                               ((nBase)      + 0x0000310c)
#define HWIO_GPIO_OE_CLR_3_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_CLR_3_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_CLR_3_ADDR(nBase),v)
#define HWIO_GPIO_OE_CLR_3_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_CLR_3_ADDR(nBase),m,v,HWIO_GPIO_OE_CLR_3_shadow)
#define HWIO_GPIO_OE_CLR_3_GPIO_OE_CLR_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_CLR_3_GPIO_OE_CLR_SHFT                                            0

#define HWIO_GPIO_OE_CLR_4_ADDR(nBase)                                               ((nBase)      + 0x00003110)
#define HWIO_GPIO_OE_CLR_4_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_CLR_4_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_CLR_4_ADDR(nBase),v)
#define HWIO_GPIO_OE_CLR_4_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_CLR_4_ADDR(nBase),m,v,HWIO_GPIO_OE_CLR_4_shadow)
#define HWIO_GPIO_OE_CLR_4_GPIO_OE_CLR_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_CLR_4_GPIO_OE_CLR_SHFT                                            0

#define HWIO_GPIO_OE_CLR_5_ADDR(nBase)                                               ((nBase)      + 0x00003114)
#define HWIO_GPIO_OE_CLR_5_RMSK                                                   0x1fff
#define HWIO_GPIO_OE_CLR_5_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_CLR_5_ADDR(nBase),v)
#define HWIO_GPIO_OE_CLR_5_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_CLR_5_ADDR(nBase),m,v,HWIO_GPIO_OE_CLR_5_shadow)
#define HWIO_GPIO_OE_CLR_5_GPIO_OE_CLR_BMSK                                       0x1fff
#define HWIO_GPIO_OE_CLR_5_GPIO_OE_CLR_SHFT                                            0

#define HWIO_GPIO_OE_SET_0_ADDR(nBase)                                               ((nBase)      + 0x00003120)
#define HWIO_GPIO_OE_SET_0_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_SET_0_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_SET_0_ADDR(nBase),v)
#define HWIO_GPIO_OE_SET_0_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_SET_0_ADDR(nBase),m,v,HWIO_GPIO_OE_SET_0_shadow)
#define HWIO_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                            0

#define HWIO_GPIO_OE_SET_1_ADDR(nBase)                                               ((nBase)      + 0x00003124)
#define HWIO_GPIO_OE_SET_1_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_SET_1_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_SET_1_ADDR(nBase),v)
#define HWIO_GPIO_OE_SET_1_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_SET_1_ADDR(nBase),m,v,HWIO_GPIO_OE_SET_1_shadow)
#define HWIO_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                            0

#define HWIO_GPIO_OE_SET_2_ADDR(nBase)                                               ((nBase)      + 0x00003128)
#define HWIO_GPIO_OE_SET_2_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_SET_2_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_SET_2_ADDR(nBase),v)
#define HWIO_GPIO_OE_SET_2_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_SET_2_ADDR(nBase),m,v,HWIO_GPIO_OE_SET_2_shadow)
#define HWIO_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                            0

#define HWIO_GPIO_OE_SET_3_ADDR(nBase)                                               ((nBase)      + 0x0000312c)
#define HWIO_GPIO_OE_SET_3_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_SET_3_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_SET_3_ADDR(nBase),v)
#define HWIO_GPIO_OE_SET_3_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_SET_3_ADDR(nBase),m,v,HWIO_GPIO_OE_SET_3_shadow)
#define HWIO_GPIO_OE_SET_3_GPIO_OE_SET_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_SET_3_GPIO_OE_SET_SHFT                                            0

#define HWIO_GPIO_OE_SET_4_ADDR(nBase)                                               ((nBase)      + 0x00003130)
#define HWIO_GPIO_OE_SET_4_RMSK                                               0xffffffff
#define HWIO_GPIO_OE_SET_4_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_SET_4_ADDR(nBase),v)
#define HWIO_GPIO_OE_SET_4_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_SET_4_ADDR(nBase),m,v,HWIO_GPIO_OE_SET_4_shadow)
#define HWIO_GPIO_OE_SET_4_GPIO_OE_SET_BMSK                                   0xffffffff
#define HWIO_GPIO_OE_SET_4_GPIO_OE_SET_SHFT                                            0

#define HWIO_GPIO_OE_SET_5_ADDR(nBase)                                               ((nBase)      + 0x00003134)
#define HWIO_GPIO_OE_SET_5_RMSK                                                   0x1fff
#define HWIO_GPIO_OE_SET_5_OUT(nBase,v)                                             \
        out_dword(HWIO_GPIO_OE_SET_5_ADDR(nBase),v)
#define HWIO_GPIO_OE_SET_5_OUTM(nBase,m,v)                                          \
        out_dword_masked(HWIO_GPIO_OE_SET_5_ADDR(nBase),m,v,HWIO_GPIO_OE_SET_5_shadow)
#define HWIO_GPIO_OE_SET_5_GPIO_OE_SET_BMSK                                       0x1fff
#define HWIO_GPIO_OE_SET_5_GPIO_OE_SET_SHFT                                            0


#endif /* __HAL_HALTLMMHWIO_H__ */

