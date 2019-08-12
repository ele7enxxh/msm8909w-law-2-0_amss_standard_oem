#ifndef __HAL_CLK_TEST_H__
#define __HAL_CLK_TEST_H__


/*
==============================================================================

FILE:         HALclkTest.h

DESCRIPTION:
  This file contains the ids for configuring the debug clock circuitry
  for the clock HAL.


==============================================================================

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x07/inc/HALclkTest.h#3 $

==============================================================================
            Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Defines
** ==========================================================================*/

/*
 * Masks used during test clock frequency calculation setup.
 */
#define HAL_CLK_TEST_TYPE_MASK    (0xF0000)
#define HAL_CLK_TEST_SEL_MASK     (0x07FFF)


/*
 * GCC debug clocks
 */
#define HAL_CLK_GCC_TEST                                              (2  << 0x10)

#define HAL_CLK_GCC_TEST_GCC_PCNOC_AHB_CLK                          (HAL_CLK_GCC_TEST | 0x008)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_RPM_AHB_CLK                      (HAL_CLK_GCC_TEST | 0x00A)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_TIC_CLK                          (HAL_CLK_GCC_TEST | 0x00B)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_AT_CLK                           (HAL_CLK_GCC_TEST | 0x00C)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_QDSS_STM_AXI_CLK                 (HAL_CLK_GCC_TEST | 0x00D)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_APSS_AHB_CLK                     (HAL_CLK_GCC_TEST | 0x00E)
#define HAL_CLK_GCC_TEST_GCC_GP1_CLK                                (HAL_CLK_GCC_TEST | 0x010)
#define HAL_CLK_GCC_TEST_GCC_GP2_CLK                                (HAL_CLK_GCC_TEST | 0x011)
#define HAL_CLK_GCC_TEST_GCC_GP3_CLK                                (HAL_CLK_GCC_TEST | 0x012)
#define HAL_CLK_GCC_TEST_GCC_NOC_CONF_XPU_AHB_CLK                   (HAL_CLK_GCC_TEST | 0x018)
#define HAL_CLK_GCC_TEST_GCC_IMEM_AXI_CLK                           (HAL_CLK_GCC_TEST | 0x020)
#define HAL_CLK_GCC_TEST_GCC_IMEM_CFG_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x021)
#define HAL_CLK_GCC_TEST_GCC_MSS_CFG_AHB_CLK                        (HAL_CLK_GCC_TEST | 0x030)
#define HAL_CLK_GCC_TEST_GCC_MSS_Q6_BIMC_AXI_CLK                    (HAL_CLK_GCC_TEST | 0x031)
#define HAL_CLK_GCC_TEST_MSS_GCC_DBG_CLK                            (HAL_CLK_GCC_TEST | 0x032)
#define HAL_CLK_GCC_TEST_GCC_RPM_CFG_XPU_CLK                        (HAL_CLK_GCC_TEST | 0x038)
#define HAL_CLK_GCC_TEST_GCC_QDSS_DAP_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x040)
#define HAL_CLK_GCC_TEST_GCC_QDSS_CFG_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x041)
#define HAL_CLK_GCC_TEST_GCC_QDSS_AT_CLK                            (HAL_CLK_GCC_TEST | 0x042)
#define HAL_CLK_GCC_TEST_GCC_QDSS_ETR_USB_CLK                       (HAL_CLK_GCC_TEST | 0x043)
#define HAL_CLK_GCC_TEST_GCC_QDSS_STM_CLK                           (HAL_CLK_GCC_TEST | 0x044)
#define HAL_CLK_GCC_TEST_GCC_QDSS_TRACECLKIN_CLK                    (HAL_CLK_GCC_TEST | 0x045)
#define HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV2_CLK                    (HAL_CLK_GCC_TEST | 0x046)
#define HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV3_CLK                    (HAL_CLK_GCC_TEST | 0x048)
#define HAL_CLK_GCC_TEST_GCC_QDSS_DAP_CLK                           (HAL_CLK_GCC_TEST | 0x049)
#define HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV4_CLK                    (HAL_CLK_GCC_TEST | 0x04A)
#define HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV8_CLK                    (HAL_CLK_GCC_TEST | 0x04B)
#define HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV16_CLK                   (HAL_CLK_GCC_TEST | 0x04C)
#define HAL_CLK_GCC_TEST_GCC_APSS_TCU_CLK                           (HAL_CLK_GCC_TEST | 0x050)
#define HAL_CLK_GCC_TEST_GCC_MSS_TBU_AXI_CLK                        (HAL_CLK_GCC_TEST | 0x055)
#define HAL_CLK_GCC_TEST_GCC_MSS_TBU_GSS_AXI_CLK                    (HAL_CLK_GCC_TEST | 0x056)
#define HAL_CLK_GCC_TEST_GCC_MSS_TBU_Q6_AXI_CLK                     (HAL_CLK_GCC_TEST | 0x057)
#define HAL_CLK_GCC_TEST_GCC_SMMU_XPU_CLK                           (HAL_CLK_GCC_TEST | 0x059)
#define HAL_CLK_GCC_TEST_GCC_SMMU_CFG_CLK                           (HAL_CLK_GCC_TEST | 0x05B)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_TBU_CLK                          (HAL_CLK_GCC_TEST | 0x05D)
#define HAL_CLK_GCC_TEST_GCC_SMMU_CATS_CLK                          (HAL_CLK_GCC_TEST | 0x05F)
#define HAL_CLK_GCC_TEST_GCC_USB_HS_SYSTEM_CLK                      (HAL_CLK_GCC_TEST | 0x060)
#define HAL_CLK_GCC_TEST_GCC_USB_HS_AHB_CLK                         (HAL_CLK_GCC_TEST | 0x061)
#define HAL_CLK_GCC_TEST_GCC_USB_HS_INACTIVITY_TIMERS_CLK           (HAL_CLK_GCC_TEST | 0x062)
#define HAL_CLK_GCC_TEST_GCC_USB2A_PHY_SLEEP_CLK                    (HAL_CLK_GCC_TEST | 0x063)
#define HAL_CLK_GCC_TEST_GCC_USB_HS_PHY_CFG_AHB_CLK                 (HAL_CLK_GCC_TEST | 0x064)
#define HAL_CLK_GCC_TEST_GCC_SDCC1_APPS_CLK                         (HAL_CLK_GCC_TEST | 0x068)
#define HAL_CLK_GCC_TEST_GCC_SDCC1_AHB_CLK                          (HAL_CLK_GCC_TEST | 0x069)
#define HAL_CLK_GCC_TEST_GCC_SDCC2_APPS_CLK                         (HAL_CLK_GCC_TEST | 0x070)
#define HAL_CLK_GCC_TEST_GCC_SDCC2_AHB_CLK                          (HAL_CLK_GCC_TEST | 0x071)
#define HAL_CLK_GCC_TEST_GCC_QPIC_CLK                               (HAL_CLK_GCC_TEST | 0x078)
#define HAL_CLK_GCC_TEST_GCC_QPIC_AHB_CLK                           (HAL_CLK_GCC_TEST | 0x079)
#define HAL_CLK_GCC_TEST_GCC_QPIC_SYSTEM_CLK                        (HAL_CLK_GCC_TEST | 0x07A)
#define HAL_CLK_GCC_TEST_GCC_BIMC_NOC_MPU_AHB_CLK                   (HAL_CLK_GCC_TEST | 0x080)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_AHB_CLK                          (HAL_CLK_GCC_TEST | 0x088)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_SLEEP_CLK                        (HAL_CLK_GCC_TEST | 0x089)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP1_SPI_APPS_CLK                (HAL_CLK_GCC_TEST | 0x08A)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP1_I2C_APPS_CLK                (HAL_CLK_GCC_TEST | 0x08B)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART1_APPS_CLK                   (HAL_CLK_GCC_TEST | 0x08C)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART1_SIM_CLK                    (HAL_CLK_GCC_TEST | 0x08D)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP2_SPI_APPS_CLK                (HAL_CLK_GCC_TEST | 0x08E)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP2_I2C_APPS_CLK                (HAL_CLK_GCC_TEST | 0x090)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART2_APPS_CLK                   (HAL_CLK_GCC_TEST | 0x091)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART2_SIM_CLK                    (HAL_CLK_GCC_TEST | 0x092)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP3_SPI_APPS_CLK                (HAL_CLK_GCC_TEST | 0x093)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP3_I2C_APPS_CLK                (HAL_CLK_GCC_TEST | 0x094)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART3_APPS_CLK                   (HAL_CLK_GCC_TEST | 0x095)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART3_SIM_CLK                    (HAL_CLK_GCC_TEST | 0x096)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP4_SPI_APPS_CLK                (HAL_CLK_GCC_TEST | 0x098)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP4_I2C_APPS_CLK                (HAL_CLK_GCC_TEST | 0x099)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART4_APPS_CLK                   (HAL_CLK_GCC_TEST | 0x09A)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART4_SIM_CLK                    (HAL_CLK_GCC_TEST | 0x09B)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP5_SPI_APPS_CLK                (HAL_CLK_GCC_TEST | 0x09C)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP5_I2C_APPS_CLK                (HAL_CLK_GCC_TEST | 0x09D)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART5_APPS_CLK                   (HAL_CLK_GCC_TEST | 0x09E)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART5_SIM_CLK                    (HAL_CLK_GCC_TEST | 0x0A0)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP6_SPI_APPS_CLK                (HAL_CLK_GCC_TEST | 0x0A1)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_QUP6_I2C_APPS_CLK                (HAL_CLK_GCC_TEST | 0x0A2)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART6_APPS_CLK                   (HAL_CLK_GCC_TEST | 0x0A3)
#define HAL_CLK_GCC_TEST_GCC_BLSP1_UART6_SIM_CLK                    (HAL_CLK_GCC_TEST | 0x0A4)
#define HAL_CLK_GCC_TEST_GCC_PRNG_XPU_CFG_AHB_CLK                   (HAL_CLK_GCC_TEST | 0x0C8)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_MPU_CFG_AHB_CLK                  (HAL_CLK_GCC_TEST | 0x0C9)
#define HAL_CLK_GCC_TEST_GCC_PDM_AHB_CLK                            (HAL_CLK_GCC_TEST | 0x0D0)
#define HAL_CLK_GCC_TEST_GCC_PDM_XO4_CLK                            (HAL_CLK_GCC_TEST | 0x0D1)
#define HAL_CLK_GCC_TEST_GCC_PDM2_CLK                               (HAL_CLK_GCC_TEST | 0x0D2)
#define HAL_CLK_GCC_TEST_GCC_PRNG_AHB_CLK                           (HAL_CLK_GCC_TEST | 0x0D8)
#define HAL_CLK_GCC_TEST_GCC_APSS_TCU_ASYNC_CLK                     (HAL_CLK_GCC_TEST | 0x0E9)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_SEC_PCM_DATA_OE_CLK           (HAL_CLK_GCC_TEST | 0x0EA)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_TER_PCM_DATA_OE_CLK           (HAL_CLK_GCC_TEST | 0x0EB)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_EXT_MCLK2_CLK                 (HAL_CLK_GCC_TEST | 0x0EC)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_PCM_DATA_OE_CLK               (HAL_CLK_GCC_TEST | 0x0ED)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CLK     (HAL_CLK_GCC_TEST | 0x0EE)
#define HAL_CLK_GCC_TEST_GCC_TCSR_AHB_CLK                           (HAL_CLK_GCC_TEST | 0x0F0)
#define HAL_CLK_GCC_TEST_GCC_BOOT_ROM_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x0F8)
#define HAL_CLK_GCC_TEST_GCC_MSG_RAM_AHB_CLK                        (HAL_CLK_GCC_TEST | 0x100)
#define HAL_CLK_GCC_TEST_GCC_TLMM_AHB_CLK                           (HAL_CLK_GCC_TEST | 0x108)
#define HAL_CLK_GCC_TEST_GCC_TLMM_CLK                               (HAL_CLK_GCC_TEST | 0x109)
#define HAL_CLK_GCC_TEST_GCC_MPM_AHB_CLK                            (HAL_CLK_GCC_TEST | 0x110)
#define HAL_CLK_GCC_TEST_GCC_RPM_PROC_FCLK                          (HAL_CLK_GCC_TEST | 0x118)
#define HAL_CLK_GCC_TEST_GCC_RPM_PROC_HCLK                          (HAL_CLK_GCC_TEST | 0x119)
#define HAL_CLK_GCC_TEST_GCC_RPM_BUS_AHB_CLK                        (HAL_CLK_GCC_TEST | 0x11A)
#define HAL_CLK_GCC_TEST_GCC_RPM_SLEEP_CLK                          (HAL_CLK_GCC_TEST | 0x11B)
#define HAL_CLK_GCC_TEST_GCC_RPM_TIMER_CLK                          (HAL_CLK_GCC_TEST | 0x11C)
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_ACC_CLK                       (HAL_CLK_GCC_TEST | 0x120)
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x121)
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_CLK                           (HAL_CLK_GCC_TEST | 0x122)
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_SENSE_CLK                     (HAL_CLK_GCC_TEST | 0x123)
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_BOOT_ROM_PATCH_CLK            (HAL_CLK_GCC_TEST | 0x124)
#define HAL_CLK_GCC_TEST_GCC_SPMI_SER_CLK                           (HAL_CLK_GCC_TEST | 0x128)
#define HAL_CLK_GCC_TEST_GCC_SPMI_PCNOC_AHB_CLK                     (HAL_CLK_GCC_TEST | 0x129)
#define HAL_CLK_GCC_TEST_GCC_SPMI_AHB_CLK                           (HAL_CLK_GCC_TEST | 0x12A)
#define HAL_CLK_GCC_TEST_GCC_SPDM_CFG_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x130)
#define HAL_CLK_GCC_TEST_GCC_SPDM_MSTR_AHB_CLK                      (HAL_CLK_GCC_TEST | 0x131)
#define HAL_CLK_GCC_TEST_GCC_SPDM_FF_CLK                            (HAL_CLK_GCC_TEST | 0x132)
#define HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_CY_CLK                       (HAL_CLK_GCC_TEST | 0x133)
#define HAL_CLK_GCC_TEST_GCC_SPDM_PCNOC_CY_CLK                      (HAL_CLK_GCC_TEST | 0x135)
#define HAL_CLK_GCC_TEST_GCC_SPDM_RPM_CY_CLK                        (HAL_CLK_GCC_TEST | 0x136)
#define HAL_CLK_GCC_TEST_GCC_CRYPTO_CLK                             (HAL_CLK_GCC_TEST | 0x138)
#define HAL_CLK_GCC_TEST_GCC_CRYPTO_AXI_CLK                         (HAL_CLK_GCC_TEST | 0x139)
#define HAL_CLK_GCC_TEST_GCC_CRYPTO_AHB_CLK                         (HAL_CLK_GCC_TEST | 0x13A)
#define HAL_CLK_GCC_TEST_GCC_AHB_CLK                                (HAL_CLK_GCC_TEST | 0x148)
#define HAL_CLK_GCC_TEST_GCC_XO_CLK                                 (HAL_CLK_GCC_TEST | 0x149)
#define HAL_CLK_GCC_TEST_GCC_XO_DIV4_CLK                            (HAL_CLK_GCC_TEST | 0x14A)
#define HAL_CLK_GCC_TEST_GCC_IM_SLEEP_CLK                           (HAL_CLK_GCC_TEST | 0x14B)
#define HAL_CLK_GCC_TEST_GCC_BIMC_XO_CLK                            (HAL_CLK_GCC_TEST | 0x150)
#define HAL_CLK_GCC_TEST_GCC_BIMC_CFG_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x151)
#define HAL_CLK_GCC_TEST_GCC_BIMC_SLEEP_CLK                         (HAL_CLK_GCC_TEST | 0x152)
#define HAL_CLK_GCC_TEST_GCC_BIMC_PCNOC_AXI_CLK                     (HAL_CLK_GCC_TEST | 0x153)
#define HAL_CLK_GCC_TEST_GCC_BIMC_CLK                               (HAL_CLK_GCC_TEST | 0x154)
#define HAL_CLK_GCC_TEST_GCC_BIMC_APSS_AXI_CLK                      (HAL_CLK_GCC_TEST | 0x155)
#define HAL_CLK_GCC_TEST_GCC_DDR_DIM_CFG_CLK                        (HAL_CLK_GCC_TEST | 0x158)
#define HAL_CLK_GCC_TEST_GCC_BIMC_TCU_CLK                           (HAL_CLK_GCC_TEST | 0x159)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_DDR_CFG_CLK                      (HAL_CLK_GCC_TEST | 0x15A)
#define HAL_CLK_GCC_TEST_GCC_DDR_DIM_SLEEP_CLK                      (HAL_CLK_GCC_TEST | 0x15B)
#define HAL_CLK_GCC_TEST_GCC_BIMC_MCDMA_AXI_CLK                     (HAL_CLK_GCC_TEST | 0x15C)
#define HAL_CLK_GCC_TEST_GCC_BIMC_NAV_AXI_CLK                       (HAL_CLK_GCC_TEST | 0x15D)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_PCNOC_MPORT_CLK               (HAL_CLK_GCC_TEST | 0x162)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_PCNOC_SWAY_CLK                (HAL_CLK_GCC_TEST | 0x163)
#define HAL_CLK_GCC_TEST_GCC_APSS_AHB_CLK                           (HAL_CLK_GCC_TEST | 0x168)
#define HAL_CLK_GCC_TEST_GCC_APSS_AXI_CLK                           (HAL_CLK_GCC_TEST | 0x169)
#define HAL_CLK_GCC_TEST_APSS_GCC_DBG_CLK                           (HAL_CLK_GCC_TEST | 0x16A)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT0_AHB_CLK             (HAL_CLK_GCC_TEST | 0x178)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT1_AHB_CLK             (HAL_CLK_GCC_TEST | 0x179)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT2_AHB_CLK             (HAL_CLK_GCC_TEST | 0x17A)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT3_AHB_CLK             (HAL_CLK_GCC_TEST | 0x17B)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT4_AHB_CLK             (HAL_CLK_GCC_TEST | 0x17C)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT5_AHB_CLK             (HAL_CLK_GCC_TEST | 0x180)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT6_AHB_CLK             (HAL_CLK_GCC_TEST | 0x181)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT7_AHB_CLK             (HAL_CLK_GCC_TEST | 0x182)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT8_AHB_CLK             (HAL_CLK_GCC_TEST | 0x183)
#define HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT9_AHB_CLK             (HAL_CLK_GCC_TEST | 0x184)
#define HAL_CLK_GCC_TEST_GCC_DEHR_CLK                               (HAL_CLK_GCC_TEST | 0x188)
#define HAL_CLK_GCC_TEST_GCC_RBCPR_CLK                              (HAL_CLK_GCC_TEST | 0x190)
#define HAL_CLK_GCC_TEST_GCC_RBCPR_AHB_CLK                          (HAL_CLK_GCC_TEST | 0x191)
#define HAL_CLK_GCC_TEST_GCC_USB_HSIC_AHB_CLK                       (HAL_CLK_GCC_TEST | 0x198)
#define HAL_CLK_GCC_TEST_GCC_USB_HSIC_SYSTEM_CLK                    (HAL_CLK_GCC_TEST | 0x199)
#define HAL_CLK_GCC_TEST_GCC_USB_HSIC_CLK                           (HAL_CLK_GCC_TEST | 0x19A)
#define HAL_CLK_GCC_TEST_GCC_USB_HSIC_IO_CAL_CLK                    (HAL_CLK_GCC_TEST | 0x19B)
#define HAL_CLK_GCC_TEST_GCC_USB_HSIC_IO_CAL_SLEEP_CLK              (HAL_CLK_GCC_TEST | 0x19C)
#define HAL_CLK_GCC_TEST_USB2_PHY_GCC_ATEST_ULPI_0_CLK              (HAL_CLK_GCC_TEST | 0x1A0)
#define HAL_CLK_GCC_TEST_USB2_PHY_GCC_ATEST_ULPI_1_CLK              (HAL_CLK_GCC_TEST | 0x1A8)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CLK        (HAL_CLK_GCC_TEST | 0x1B0)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CLK     (HAL_CLK_GCC_TEST | 0x1B1)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_LPAIF_PRI_I2S_CLK             (HAL_CLK_GCC_TEST | 0x1B2)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_LPAIF_SEC_I2S_CLK             (HAL_CLK_GCC_TEST | 0x1B3)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_LPAIF_AUX_I2S_CLK             (HAL_CLK_GCC_TEST | 0x1B4)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AVSYNC_XO_CLK                 (HAL_CLK_GCC_TEST | 0x1B5)
#define HAL_CLK_GCC_TEST_GCC_ULTAUDIO_STC_XO_CLK                    (HAL_CLK_GCC_TEST | 0x1B6)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_AXI_CLK                         (HAL_CLK_GCC_TEST | 0x1B8)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_AHB_CLK                         (HAL_CLK_GCC_TEST | 0x1B9)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_SYS_25M_CLK                     (HAL_CLK_GCC_TEST | 0x1BA)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_TX_CLK                          (HAL_CLK_GCC_TEST | 0x1BB)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_125M_CLK                        (HAL_CLK_GCC_TEST | 0x1BC)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_RX_CLK                          (HAL_CLK_GCC_TEST | 0x1BD)
#define HAL_CLK_GCC_TEST_GCC_EMAC_0_SYS_CLK                         (HAL_CLK_GCC_TEST | 0x1BE)
#define HAL_CLK_GCC_TEST_MPM_GCC_TEMP_SENSOR_RINGOSC_CLK            (HAL_CLK_GCC_TEST | 0x1C8)
#define HAL_CLK_GCC_TEST_APSS_GCC_RINGOSC_CLK                       (HAL_CLK_GCC_TEST | 0x1D0)
#define HAL_CLK_GCC_TEST_GCC_DCD_XO_CLK                             (HAL_CLK_GCC_TEST | 0x1E0)


#endif // __HAL_CLK_TEST_H__

