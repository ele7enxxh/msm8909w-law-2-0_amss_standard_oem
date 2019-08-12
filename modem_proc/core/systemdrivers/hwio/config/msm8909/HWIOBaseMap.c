
/*
===========================================================================
*/
/**
  @file HWIOBaseMap.c
  @brief Auto-generated HWIO Device Configuration base file.

  DESCRIPTION:
    This file contains Device Configuration data structures for mapping
    physical and virtual memory for HWIO blocks.
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hwio/config/msm8909/HWIOBaseMap.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DalHWIO.h"
#include "msmhwiobase.h"


/*=========================================================================
      Data Definitions
==========================================================================*/

static HWIOModuleType HWIOModules_PRNG_PRNG[] =
{
  { "PRNG_PRNG",                                   0x00000000, 0x00000140 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB[] =
{
  { "XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB",       0x00000000, 0x00000400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SPDM_WRAPPER_TOP[] =
{
  { "SPDM_SPDM_CREG",                              0x00000000, 0x00000120 },
  { "SPDM_SPDM_OLEM",                              0x00001000, 0x0000015c },
  { "SPDM_SPDM_RTEM",                              0x00002000, 0x00000318 },
  { "SPDM_SPDM_SREG",                              0x00004000, 0x00000120 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SECURITY_CONTROL[] =
{
  { "SECURITY_CONTROL_CORE",                       0x00000000, 0x00007000 },
  { "SEC_CTRL_APU_APU1132_10",                     0x00007000, 0x000006d8 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_RPM_SS_MSG_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PDM_PERPH_WEB[] =
{
  { "PDM_WEB_TCXO4",                               0x00000000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BOOT_ROM[] =
{
  { "BOOT_ROM_MPU1032_3_M20L12_AHB",               0x000ff000, 0x00000380 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_RPM[] =
{
  { "RPM_DEC",                                     0x00080000, 0x00002000 },
  { "RPM_QTMR_AC",                                 0x00082000, 0x00001000 },
  { "RPM_F0_QTMR_V1_F0",                           0x00083000, 0x00001000 },
  { "RPM_F1_QTMR_V1_F1",                           0x00084000, 0x00001000 },
  { "RPM_APU",                                     0x00087000, 0x00000300 },
  { "RPM_VMIDMT",                                  0x00088000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BIMC[] =
{
  { "BIMC_MISC",                                   0x00000000, 0x00001000 },
  { "BIMC_GLOBAL1",                                0x00001000, 0x00001000 },
  { "BIMC_GLOBAL2",                                0x00002000, 0x00001000 },
  { "BIMC_PERFMON",                                0x00003000, 0x00001000 },
  { "BIMC_DTE",                                    0x00006000, 0x0000005c },
  { "BIMC_M_APP_MPORT",                            0x00008000, 0x00001000 },
  { "BIMC_M_APP_PROF",                             0x00009000, 0x00001000 },
  { "BIMC_M_DSP_MPORT",                            0x0000c000, 0x00001000 },
  { "BIMC_M_DSP_PROF",                             0x0000d000, 0x00001000 },
  { "BIMC_M_GPU_MPORT",                            0x00010000, 0x00001000 },
  { "BIMC_M_GPU_PROF",                             0x00011000, 0x00001000 },
  { "BIMC_M_SYS0_MPORT",                           0x00014000, 0x00001000 },
  { "BIMC_M_SYS0_PROF",                            0x00015000, 0x00001000 },
  { "BIMC_M_SYS1_MPORT",                           0x00018000, 0x00001000 },
  { "BIMC_M_SYS1_PROF",                            0x00019000, 0x00001000 },
  { "BIMC_M_TCU0_MPORT",                           0x0001c000, 0x00001000 },
  { "BIMC_M_TCU0_PROF",                            0x0001d000, 0x00001000 },
  { "BIMC_M_TCU1_MPORT",                           0x00020000, 0x00001000 },
  { "BIMC_M_TCU1_PROF",                            0x00021000, 0x00001000 },
  { "BIMC_S_SYS_SWAY",                             0x00050000, 0x00001000 },
  { "BIMC_S_DEFAULT_SWAY",                         0x00058000, 0x00001000 },
  { "BIMC_S_DDR0_ARB",                             0x00049000, 0x00001000 },
  { "BIMC_S_SYS_ARB",                              0x00051000, 0x00001000 },
  { "BIMC_S_DEFAULT_ARB",                          0x00059000, 0x00001000 },
  { "BIMC_S_DDR0_SCMO",                            0x00048000, 0x00001000 },
  { "BIMC_S_DDR0",                                 0x0004a000, 0x00001080 },
  { "BIMC_S_DDR0_DPE",                             0x0004c000, 0x00001000 },
  { "BIMC_S_DDR0_SHKE",                            0x0004d000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MPM2_MPM[] =
{
  { "MPM2_MPM",                                    0x00000000, 0x00001000 },
  { "MPM2_G_CTRL_CNTR",                            0x00001000, 0x00001000 },
  { "MPM2_G_RD_CNTR",                              0x00002000, 0x00001000 },
  { "MPM2_SLP_CNTR",                               0x00003000, 0x00001000 },
  { "MPM2_QTIMR_AC",                               0x00004000, 0x00001000 },
  { "MPM2_QTIMR_V1",                               0x00005000, 0x00001000 },
  { "MPM2_TSYNC",                                  0x00006000, 0x00001000 },
  { "MPM2_APU",                                    0x00007000, 0x00000780 },
  { "MPM2_TSENS",                                  0x00008000, 0x00001000 },
  { "MPM2_TSENS_TM",                               0x00009000, 0x00001000 },
  { "MPM2_WDOG",                                   0x0000a000, 0x00000020 },
  { "MPM2_PSHOLD",                                 0x0000b000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_DEHR_BIMC_WRAPPER[] =
{
  { "DEHR_BIMC",                                   0x00002000, 0x00002000 },
  { "DEHR_XPU",                                    0x00000000, 0x00000300 },
  { "DEHR_VMIDMT",                                 0x00001000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PC_NOC[] =
{
  { "PC_NOC",                                      0x00000000, 0x00010080 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SYSTEM_NOC[] =
{
  { "SYSTEM_NOC",                                  0x00000000, 0x00012080 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CRYPTO0_CRYPTO_TOP[] =
{
  { "CRYPTO0_CRYPTO",                              0x0003a000, 0x00006000 },
  { "CRYPTO0_CRYPTO_BAM",                          0x00004000, 0x00020000 },
  { "CRYPTO0_CRYPTO_BAM_XPU2_BAM",                 0x00002000, 0x00002000 },
  { "CRYPTO0_CRYPTO_BAM_VMIDMT_BAM",               0x00000000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QDSS_QDSS[] =
{
  { "QDSS_APB",                                    0x00000000, 0x00030000 },
  { "QDSS_DAPROM",                                 0x00000000, 0x00001000 },
  { "QDSS_QDSSCSR",                                0x00001000, 0x00001000 },
  { "QDSS_CXSTM_2_32_32_TRUE",                     0x00002000, 0x00001000 },
  { "QDSS_CTI0_CTI0_CSCTI",                        0x00010000, 0x00001000 },
  { "QDSS_CTI1_CTI1_CSCTI",                        0x00011000, 0x00001000 },
  { "QDSS_CTI2_CTI2_CSCTI",                        0x00012000, 0x00001000 },
  { "QDSS_CTI3_CTI3_CSCTI",                        0x00013000, 0x00001000 },
  { "QDSS_CTI4_CTI4_CSCTI",                        0x00014000, 0x00001000 },
  { "QDSS_CTI5_CTI5_CSCTI",                        0x00015000, 0x00001000 },
  { "QDSS_CTI6_CTI6_CSCTI",                        0x00016000, 0x00001000 },
  { "QDSS_CTI7_CTI7_CSCTI",                        0x00017000, 0x00001000 },
  { "QDSS_CTI8_CTI8_CSCTI",                        0x00018000, 0x00001000 },
  { "QDSS_CSTPIU_CSTPIU_CSTPIU",                   0x00020000, 0x00001000 },
  { "QDSS_IN_FUN0_IN_FUN0_CXATBFUNNEL_128W8SP",    0x00021000, 0x00001000 },
  { "QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP",     0x00024000, 0x00001000 },
  { "QDSS_ETFETB_ETFETB_CXTMC_F128W8K",            0x00025000, 0x00001000 },
  { "QDSS_ETR_ETR_CXTMC_R64W32D",                  0x00026000, 0x00001000 },
  { "QDSS_AHB",                                    0x00070000, 0x00030000 },
  { "QDSS_NDPBAM_NDPBAM_BAM_NDP_TOP_AUTO_SCALE_V2_0", 0x00080000, 0x00019000 },
  { "QDSS_NDPBAM_BAM",                             0x00084000, 0x00015000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TLMM[] =
{
  { "TLMM_MPU1132_16_M22L12_AHB",                  0x00300000, 0x00000a00 },
  { "TLMM_CSR",                                    0x00000000, 0x00300000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CLK_CTL[] =
{
  { "GCC_CLK_CTL_REG",                             0x00000000, 0x00080000 },
  { "GCC_RPU_RPU1132_32_L12",                      0x00080000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CORE_TOP_CSR[] =
{
  { "TCSR_TCSR_MUTEX",                             0x00005000, 0x00020000 },
  { "TCSR_REGS_APU1132_16",                        0x00036000, 0x00000a00 },
  { "TCSR_TCSR_REGS",                              0x00037000, 0x00021000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PMIC_ARB[] =
{
  { "SPMI_CFG_TOP",                                0x00000000, 0x0000d000 },
  { "SPMI_GENI_CFG",                               0x0000a000, 0x00000700 },
  { "SPMI_CFG",                                    0x0000a700, 0x00001a00 },
  { "SPMI_PIC",                                    0x01800000, 0x00200000 },
  { "PMIC_ARB_MPU1132_18_M25L12_AHB",              0x0000e000, 0x00000b00 },
  { "PMIC_ARB_CORE",                               0x0000f000, 0x00001000 },
  { "PMIC_ARB_CORE_REGISTERS",                     0x00400000, 0x00400000 },
  { "PMIC_ARB_CORE_REGISTERS_OBS",                 0x00c00000, 0x00400000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MSS_TOP[] =
{
  { "MSS_APU0132_5",                               0x00000000, 0x00000480 },
  { "MSS_RMB",                                     0x00020000, 0x00010000 },
  { "MSS_BAM_NDP_AUTO_SCALE_V2_0",                 0x00044000, 0x00019000 },
  { "MSS_QDSP6SS_PUB",                             0x00080000, 0x00004040 },
  { "QDSP6SS_PRIVATE",                             0x00100000, 0x00080000 },
  { "QDSP6SS_CSR",                                 0x00100000, 0x00008028 },
  { "QDSP6SS_L2VIC",                               0x00110000, 0x00001000 },
  { "QDSP6SS_QTMR_AC",                             0x00120000, 0x00001000 },
  { "QDSP6SS_QTMR_F0_0",                           0x00121000, 0x00001000 },
  { "QDSP6SS_QTMR_F1_1",                           0x00122000, 0x00001000 },
  { "QDSP6SS_QTMR_F2_2",                           0x00123000, 0x00001000 },
  { "QDSP6SS_SAW2",                                0x00130000, 0x00000ff0 },
  { "MSS_PERPH",                                   0x00180000, 0x0000f020 },
  { "MSS_UIM0_UART_DM",                            0x00190000, 0x000001c0 },
  { "MSS_UIM1_UART_DM",                            0x00198000, 0x000001c0 },
  { "MSS_UIM2_UART_DM",                            0x001a8000, 0x000001c0 },
  { "MSS_CONF_BUS_TIMEOUT",                        0x001b0000, 0x00001000 },
  { "MSS_COMBODAC_COMP",                           0x001b2000, 0x00001000 },
  { "COMBODAC_CSR",                                0x001b2000, 0x00000800 },
  { "COMBODAC_REGARRAY",                           0x001b2800, 0x00000800 },
  { "MSS_MGPI",                                    0x001b3000, 0x00000128 },
  { "MSS_CRYPTO_TOP",                              0x001c0000, 0x00040000 },
  { "MSS_CRYPTO",                                  0x001fa000, 0x00006000 },
  { "MSS_CRYPTO_BAM",                              0x001c4000, 0x00015000 },
  { "MSS_NAV",                                     0x00200000, 0x000f888d },
  { "MODEM_TOP",                                   0x00300000, 0x00090000 },
  { "TX_TOP",                                      0x00380000, 0x00010000 },
  { "TX",                                          0x00380000, 0x00001000 },
  { "TX_UNIFIED_TOP",                              0x00381000, 0x00001000 },
  { "TX_UNIFIED",                                  0x00381000, 0x00000800 },
  { "TX_UNIFIED_MEM",                              0x00381800, 0x00000800 },
  { "TX_MEM",                                      0x00382000, 0x00001000 },
  { "TX_BRDG",                                     0x00383000, 0x00000050 },
  { "TXR_A0",                                      0x00385000, 0x00001000 },
  { "TXC_A1",                                      0x00386000, 0x00001000 },
  { "TXR_A1",                                      0x00387000, 0x00001000 },
  { "TXC_A0",                                      0x00384000, 0x00001000 },
  { "TXC_MEM",                                     0x00388000, 0x00001800 },
  { "O_TX",                                        0x0038a000, 0x00000200 },
  { "O_TX_WMORE_TS_TRIF",                          0x0038a200, 0x00000400 },
  { "O_TX_ENC_TS_TRIF",                            0x0038a600, 0x00000200 },
  { "O_TX_MOD_TS_TRIF",                            0x0038a800, 0x00000700 },
  { "RXFE",                                        0x00360000, 0x00020000 },
  { "RXFE_TOP_CFG",                                0x00360000, 0x00001000 },
  { "RXFE_ADC_ADC0",                               0x00362000, 0x00000100 },
  { "RXFE_ADC_ADC1",                               0x00362100, 0x00000100 },
  { "RXFE_ADC_ADC2",                               0x00362200, 0x00000100 },
  { "RXFE_ADC_ADC3",                               0x00362300, 0x00000100 },
  { "RXFE_WB_WB0",                                 0x00364000, 0x00001000 },
  { "RXFE_WB_WB1",                                 0x00365000, 0x00001000 },
  { "RXFE_WB_WB2",                                 0x00366000, 0x00001000 },
  { "RXFE_WB_WB3",                                 0x00367000, 0x00001000 },
  { "RXFE_NB_NB0",                                 0x00370000, 0x00001000 },
  { "RXFE_NB_NB1",                                 0x00371000, 0x00001000 },
  { "RXFE_NB_NB2",                                 0x00372000, 0x00001000 },
  { "RXFE_NB_NB3",                                 0x00373000, 0x00001000 },
  { "RXFE_NB_NB4",                                 0x00374000, 0x00001000 },
  { "RXFE_NB_NB5",                                 0x00375000, 0x00001000 },
  { "RXFE_BRDG",                                   0x0037f000, 0x00000100 },
  { "DEMBACK_TOP",                                 0x00340000, 0x00020000 },
  { "DEMBACK_COMMON",                              0x00340000, 0x00000100 },
  { "DEMBACK_BRDG",                                0x00340100, 0x00000100 },
  { "LTE_DEMBACK",                                 0x00341000, 0x00000050 },
  { "UMTS_DEMBACK",                                0x00342000, 0x00000300 },
  { "TDS_DEMBACK",                                 0x00342400, 0x00000100 },
  { "CDMA_DEINT",                                  0x00342600, 0x00000100 },
  { "CDMA_WIDGET",                                 0x00342900, 0x00000100 },
  { "HDR_DEINT",                                   0x00342c00, 0x00000100 },
  { "DB_BUF",                                      0x00344000, 0x00004000 },
  { "SVD_TBVD",                                    0x00348000, 0x00000300 },
  { "LTE_DEMBACK_SCH_TRIF",                        0x00349000, 0x00000300 },
  { "LTE_DEMBACK_CCH_TRIF",                        0x0034a000, 0x00000c00 },
  { "LTE_REENC_TS_TRIF",                           0x0034b000, 0x00000400 },
  { "W_DBACK_HS_TRIF",                             0x0034c000, 0x00000400 },
  { "W_DBACK_NONHS_TRIF",                          0x0034d000, 0x00000400 },
  { "T_DBACK_TRIF",                                0x0034e000, 0x00000500 },
  { "HDR_DEINT_TS_TRIF",                           0x0034f000, 0x00000300 },
  { "TBVD_CCH_TRIF",                               0x00350000, 0x00000400 },
  { "DECOB",                                       0x00350500, 0x0000b901 },
  { "DB_BUF_PAGE",                                 0x0035c000, 0x00004000 },
  { "TDEC_TOP",                                    0x0033c000, 0x00004000 },
  { "TDEC",                                        0x0033c000, 0x000000a8 },
  { "TD_CFG_TRIF",                                 0x0033d000, 0x00000a00 },
  { "TD_TRIF",                                     0x0033e000, 0x00000900 },
  { "TDECIB_MEM",                                  0x0033f000, 0x00001000 },
  { "MTC_TOP",                                     0x00300000, 0x0002fffd },
  { "CCS",                                         0x00300000, 0x00020000 },
  { "PDMEM",                                       0x00300000, 0x00010000 },
  { "CONTROL",                                     0x0031fc00, 0x00000400 },
  { "MTC_CLK",                                     0x00320000, 0x00000400 },
  { "MCDMA",                                       0x00320400, 0x00000400 },
  { "A2",                                          0x00320800, 0x00000400 },
  { "DBG",                                         0x00320c00, 0x00000400 },
  { "MTC_BRDG",                                    0x00321000, 0x00000400 },
  { "A2_MEM",                                      0x00322000, 0x00002000 },
  { "MCDMA_TS_TRIF",                               0x00324000, 0x00000600 },
  { "DBG_TS_TRIF",                                 0x00324800, 0x00000200 },
  { "STMR_EXPY",                                   0x00324c00, 0x00000400 },
  { "ENCRYPT",                                     0x00325000, 0x000000fd },
  { "O_STMR",                                      0x00328000, 0x00000400 },
  { "TDS_STMR",                                    0x00328400, 0x00000400 },
  { "UNIV_STMR",                                   0x00328800, 0x00000400 },
  { "UMTS_STMR",                                   0x00329000, 0x00000800 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_ULTAUDIO_CORE[] =
{
  { "ULTAUDIO_CSR",                                0x00000000, 0x00003000 },
  { "ULTAUDIO_LPASS_TCSR",                         0x00000000, 0x00000100 },
  { "LPASS_LPASS_CSR",                             0x00000100, 0x00000f00 },
  { "LPASS_LPASS_CSR_VFR",                         0x00001000, 0x00001000 },
  { "LPASS_LPASS_CSR_IO",                          0x00002000, 0x00001000 },
  { "LPASS_AHBI_TIME",                             0x00005000, 0x00001000 },
  { "LPASS_LPASS_SYNC_WRAPPER",                    0x00006000, 0x00000ffd },
  { "LPASS_AVTIMER",                               0x00006000, 0x00000100 },
  { "LPASS_LPASS_AUDSYNC_WRAPPER",                 0x00007000, 0x000001fd },
  { "LPASS_LPA_IF",                                0x00008000, 0x00010000 },
  { "LPASS_LPASS_LPM",                             0x00018000, 0x00001000 },
  { "LPASS_CODEC_CORE",                            0x0001c000, 0x00000314 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BLSP1_BLSP[] =
{
  { "BLSP1_BLSP_BAM",                              0x00004000, 0x00023000 },
  { "BLSP1_BLSP_BAM_XPU2",                         0x00002000, 0x00002000 },
  { "BLSP1_BLSP_BAM_VMIDMT",                       0x00000000, 0x00001000 },
  { "BLSP1_BLSP_UART0_UART0_DM",                   0x0002f000, 0x00000200 },
  { "BLSP1_BLSP_UART1_UART1_DM",                   0x00030000, 0x00000200 },
  { "BLSP1_BLSP_QUP0",                             0x00035000, 0x00000600 },
  { "BLSP1_BLSP_QUP1",                             0x00036000, 0x00000600 },
  { "BLSP1_BLSP_QUP2",                             0x00037000, 0x00000600 },
  { "BLSP1_BLSP_QUP3",                             0x00038000, 0x00000600 },
  { "BLSP1_BLSP_QUP4",                             0x00039000, 0x00000600 },
  { "BLSP1_BLSP_QUP5",                             0x0003a000, 0x00000600 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QPIC_QPIC[] =
{
  { "QPIC_QPIC_BAM_LITE_TOP_QPIC",                 0x00000000, 0x0001e001 },
  { "QPIC_QPIC_BAM",                               0x00004000, 0x0001a000 },
  { "QPIC_QPIC_XPU2",                              0x00002000, 0x00002000 },
  { "QPIC_QPIC_VMIDMT",                            0x00000000, 0x00001000 },
  { "QPIC_EBI2CR",                                 0x00020000, 0x00008000 },
  { "QPIC_EBI2ND",                                 0x00030000, 0x00001000 },
  { "QPIC_QPIC_MPU_MPU1132_8_M39L16_AHB_40",       0x00031000, 0x00000600 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_WCSS_WCSS[] =
{
  { "WCSS_A_BT_SYSCLK",                            0x00440000, 0x00000138 },
  { "WCSS_A_BT",                                   0x003ff000, 0x00000400 },
  { "WCSS_A_CRYPTO4",                              0x00400000, 0x00010000 },
  { "WCSS_A_QTA_BT",                               0x004c0000, 0x00000024 },
  { "WCSS_A_BT_SSBI2_CFG",                         0x004afff0, 0x00000010 },
  { "WCSS_A_SSBI2_CFG",                            0x004a0000, 0x00000200 },
  { "WCSS_A_SSBI2_CMD",                            0x00480000, 0x00000010 },
  { "WCSS_A_XPU_APU",                              0x0021f000, 0x00000300 },
  { "WCSS_A_PMU",                                  0x0021b000, 0x00003000 },
  { "WCSS_A_AOAHB",                                0x0021a000, 0x00000094 },
  { "WCSS_A_SAW2_WCN_SAW2",                        0x00219000, 0x00001000 },
  { "WCSS_A_QTMR_AC",                              0x00211000, 0x00001000 },
  { "WCSS_A_QTMR_V1_T0",                           0x00212000, 0x00001000 },
  { "WCSS_A_QTMR_V1_T1",                           0x00213000, 0x00001000 },
  { "WCSS_A_QTMR_V1_T2",                           0x00214000, 0x00001000 },
  { "WCSS_A_QTMR_V1_T3",                           0x00215000, 0x00001000 },
  { "WCSS_A_QTMR_V1_T4",                           0x00216000, 0x00001000 },
  { "WCSS_A_QGIC2",                                0x0020a000, 0x00005020 },
  { "WCSS_A_LTE_CXM",                              0x00208000, 0x000000c8 },
  { "WCSS_A_SPDM",                                 0x00203000, 0x00000138 },
  { "WCSS_A_DXE_0",                                0x00202000, 0x00000600 },
  { "WCSS_A_CMEM",                                 0x00201000, 0x00000400 },
  { "WCSS_A_CCAHB",                                0x00200000, 0x00000094 },
  { "WCSS_A_FM_SSBI2_CFG",                         0x001e0000, 0x00000200 },
  { "WCSS_A_FM_SSBI2_CMD",                         0x001c0000, 0x00000010 },
  { "WCSS_A_FM",                                   0x00180000, 0x00020100 },
  { "WCSS_A_A2XB",                                 0x00100400, 0x00000030 },
  { "WCSS_A_ADU",                                  0x00082800, 0x00000400 },
  { "WCSS_A_AGC",                                  0x00013c00, 0x000003d0 },
  { "WCSS_A_BBANA",                                0x00014000, 0x00000034 },
  { "WCSS_A_BMU",                                  0x00080000, 0x00000274 },
  { "WCSS_A_BTCF",                                 0x00014800, 0x00000018 },
  { "WCSS_A_CAHB",                                 0x00082c00, 0x00000094 },
  { "WCSS_A_CAL",                                  0x00008000, 0x000014f8 },
  { "WCSS_A_CCU",                                  0x00204000, 0x000020d0 },
  { "WCSS_A_CCU_UART",                             0x00207000, 0x000000ac },
  { "WCSS_A_CDAHB",                                0x00100000, 0x00000094 },
  { "WCSS_A_DAHB",                                 0x000c0000, 0x00000094 },
  { "WCSS_A_DPU",                                  0x00081800, 0x00000168 },
  { "WCSS_A_FDAHB",                                0x000c0400, 0x00000094 },
  { "WCSS_A_FFT",                                  0x0000c000, 0x00001008 },
  { "WCSS_A_MCU",                                  0x00080c00, 0x000003c0 },
  { "WCSS_A_MPI",                                  0x00013400, 0x00000060 },
  { "WCSS_A_MTU",                                  0x00081400, 0x00000400 },
  { "WCSS_A_PHYDBG",                               0x00004000, 0x00004000 },
  { "WCSS_A_PHYINT",                               0x00015c00, 0x00000010 },
  { "WCSS_A_PMI",                                  0x00013800, 0x00000070 },
  { "WCSS_A_QTA",                                  0x00080fc0, 0x00000024 },
  { "WCSS_A_RACTL",                                0x0000f000, 0x000001d4 },
  { "WCSS_A_RBAPB",                                0x00014400, 0x00000074 },
  { "WCSS_A_RFIF",                                 0x00015000, 0x00000bf8 },
  { "WCSS_A_RPE",                                  0x00082400, 0x00000154 },
  { "WCSS_A_RXACLKCTRL",                           0x00016000, 0x00000014 },
  { "WCSS_A_RXCLKCTRL",                            0x00016400, 0x0000002c },
  { "WCSS_A_RXP",                                  0x00080800, 0x000003e4 },
  { "WCSS_A_TACTL",                                0x00012400, 0x0000005c },
  { "WCSS_A_TAQAM",                                0x00012c00, 0x00000034 },
  { "WCSS_A_TBAPB",                                0x00012800, 0x00000018 },
  { "WCSS_A_TDC",                                  0x00014c00, 0x00000038 },
  { "WCSS_A_TPC",                                  0x00010000, 0x00000d00 },
  { "WCSS_A_TPE",                                  0x00082000, 0x00000200 },
  { "WCSS_A_TXCLKCTRL",                            0x00013000, 0x0000000c },
  { "WCSS_A_TXCTL",                                0x00012000, 0x000000b0 },
  { "WCSS_A_TXFIR",                                0x00018000, 0x00001800 },
  { "WCSS_A_TXP",                                  0x00080400, 0x00000400 },
  { NULL, 0, 0 }
};

HWIOPhysRegionType HWIOBaseMap[] =
{
  {
    "PRNG_PRNG",
    (DALSYSMemAddr)0x00022000,
    0x00001000,
    (DALSYSMemAddr)PRNG_PRNG_BASE,
    HWIOModules_PRNG_PRNG
  },
  {
    "XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB",
    (DALSYSMemAddr)0x0002d000,
    0x00001000,
    (DALSYSMemAddr)XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE,
    HWIOModules_XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB
  },
  {
    "SPDM_WRAPPER_TOP",
    (DALSYSMemAddr)0x00040000,
    0x00005000,
    (DALSYSMemAddr)SPDM_WRAPPER_TOP_BASE,
    HWIOModules_SPDM_WRAPPER_TOP
  },
  {
    "SECURITY_CONTROL",
    (DALSYSMemAddr)0x00058000,
    0x00008000,
    (DALSYSMemAddr)SECURITY_CONTROL_BASE,
    HWIOModules_SECURITY_CONTROL
  },
  {
    "RPM_SS_MSG_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x00060000,
    0x00005000,
    (DALSYSMemAddr)RPM_SS_MSG_RAM_START_ADDRESS_BASE,
    HWIOModules_RPM_SS_MSG_RAM_START_ADDRESS
  },
  {
    "PDM_PERPH_WEB",
    (DALSYSMemAddr)0x00068000,
    0x00004000,
    (DALSYSMemAddr)PDM_PERPH_WEB_BASE,
    HWIOModules_PDM_PERPH_WEB
  },
  {
    "BOOT_ROM",
    (DALSYSMemAddr)0x00100000,
    0x00100000,
    (DALSYSMemAddr)BOOT_ROM_BASE,
    HWIOModules_BOOT_ROM
  },
  {
    "RPM",
    (DALSYSMemAddr)0x00200000,
    0x00089000,
    (DALSYSMemAddr)RPM_BASE,
    HWIOModules_RPM
  },
  {
    "BIMC",
    (DALSYSMemAddr)0x00400000,
    0x0005a000,
    (DALSYSMemAddr)BIMC_BASE,
    HWIOModules_BIMC
  },
  {
    "MPM2_MPM",
    (DALSYSMemAddr)0x004a0000,
    0x0000c000,
    (DALSYSMemAddr)MPM2_MPM_BASE,
    HWIOModules_MPM2_MPM
  },
  {
    "DEHR_BIMC_WRAPPER",
    (DALSYSMemAddr)0x004b0000,
    0x00004000,
    (DALSYSMemAddr)DEHR_BIMC_WRAPPER_BASE,
    HWIOModules_DEHR_BIMC_WRAPPER
  },
  {
    "PC_NOC",
    (DALSYSMemAddr)0x00500000,
    0x00011000,
    (DALSYSMemAddr)PC_NOC_BASE,
    HWIOModules_PC_NOC
  },
  {
    "SYSTEM_NOC",
    (DALSYSMemAddr)0x00580000,
    0x00013000,
    (DALSYSMemAddr)SYSTEM_NOC_BASE,
    HWIOModules_SYSTEM_NOC
  },
  {
    "CRYPTO0_CRYPTO_TOP",
    (DALSYSMemAddr)0x00700000,
    0x00040000,
    (DALSYSMemAddr)CRYPTO0_CRYPTO_TOP_BASE,
    HWIOModules_CRYPTO0_CRYPTO_TOP
  },
  {
    "QDSS_QDSS",
    (DALSYSMemAddr)0x00800000,
    0x00099000,
    (DALSYSMemAddr)QDSS_QDSS_BASE,
    HWIOModules_QDSS_QDSS
  },
  {
    "TLMM",
    (DALSYSMemAddr)0x01000000,
    0x00301000,
    (DALSYSMemAddr)TLMM_BASE,
    HWIOModules_TLMM
  },
  {
    "CLK_CTL",
    (DALSYSMemAddr)0x01800000,
    0x00082000,
    (DALSYSMemAddr)CLK_CTL_BASE,
    HWIOModules_CLK_CTL
  },
  {
    "CORE_TOP_CSR",
    (DALSYSMemAddr)0x01900000,
    0x00058000,
    (DALSYSMemAddr)CORE_TOP_CSR_BASE,
    HWIOModules_CORE_TOP_CSR
  },
  {
    "PMIC_ARB",
    (DALSYSMemAddr)0x02000000,
    0x01906000,
    (DALSYSMemAddr)PMIC_ARB_BASE,
    HWIOModules_PMIC_ARB
  },
  {
    "MSS_TOP",
    (DALSYSMemAddr)0x04000000,
    0x0038b000,
    (DALSYSMemAddr)MSS_TOP_BASE,
    HWIOModules_MSS_TOP
  },
  {
    "ULTAUDIO_CORE",
    (DALSYSMemAddr)0x07700000,
    0x0001d000,
    (DALSYSMemAddr)ULTAUDIO_CORE_BASE,
    HWIOModules_ULTAUDIO_CORE
  },
  {
    "BLSP1_BLSP",
    (DALSYSMemAddr)0x07880000,
    0x0003b000,
    (DALSYSMemAddr)BLSP1_BLSP_BASE,
    HWIOModules_BLSP1_BLSP
  },
  {
    "QPIC_QPIC",
    (DALSYSMemAddr)0x07980000,
    0x00032000,
    (DALSYSMemAddr)QPIC_QPIC_BASE,
    HWIOModules_QPIC_QPIC
  },
  {
    "WCSS_WCSS",
    (DALSYSMemAddr)0x0a000000,
    0x004c1000,
    (DALSYSMemAddr)WCSS_WCSS_BASE,
    HWIOModules_WCSS_WCSS
  },
  { NULL, 0, 0, 0, NULL }
};

