#ifndef QHSUSB_DCI_HW_REGISTERS_H
#define QHSUSB_DCI_HW_REGISTERS_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    File Name : qhsusb_dci_hw_registers.h

 GENERAL DESCRIPTION

    This file defines the USB HS ChipIdea hardware registers.

  Copyright (c) 2008-2010 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci_hw_registers.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
12/30/13   shreyasr Replace Hard coded values from HWIO values
08/30/10   dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
07/14/08   amirs    First Draft

===========================================================================*/
#ifdef QHSUSB_TARGET_8909
#include "qhsusb_hwio_dependency_8909.h"
#endif

#ifdef QHSUSB_TARGET_8952
#include "qhsusb_hwio_dependency_8952.h"
#endif

#ifdef QHSUSB_TARGET_8956
#include "qhsusb_hwio_dependency_8956.h"
#endif

#ifndef QHSUSB_TARGET_8956
// ====================================
// Register offset address
// ====================================

#define USB_OTG_HS_ID                                      HWIO_USB2_HSIC_USB_OTG_HS_ID_ADDR 
#define USB_OTG_HS_HWGENERAL                               HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR
#define USB_OTG_HS_HWHOST                                  HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR
#define USB_OTG_HS_HWDEVICE                                HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR
#define USB_OTG_HS_HWTXBUF                                 HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR
#define USB_OTG_HS_HWRXBUF                                 HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR
#define USB_OTG_HS_SCRATCH_RAMn                            HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(0) 
#define USB_OTG_HS_GPTIMER0LD                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR
#define USB_OTG_HS_GPTIMER0CTRL                            HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR
#define USB_OTG_HS_GPTIMER1LD                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR
#define USB_OTG_HS_GPTIMER1CTRL                            HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR
#define USB_OTG_HS_AHB_BURST                               HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR
#define USB_OTG_HS_XTOR_STS                                HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR
#define USB_OTG_HS_AHB_MODE                                HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR
#define USB_OTG_HS_GEN_CONFIG                              HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR
#define USB_OTG_HS_GEN_CONFIG2                             HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR
#define USB_OTG_HS_CAPLENGTH                               HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR
#define USB_OTG_HS_HCSPARAMS                               HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR
#define USB_OTG_HS_HCCPARAMS                               HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR
#define USB_OTG_HS_DCIVERSION                              HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR
#define USB_OTG_HS_DCCPARAMS                               HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR
#define USB_OTG_HS_USBCMD                                  HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR
#define USB_OTG_HS_USBSTS                                  HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR
#define USB_OTG_HS_USBINTR                                 HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR
#define USB_OTG_HS_FRINDEX                                 HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR
#define USB_OTG_HS_PERIODICLISTBASE                        HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR
#define USB_OTG_HS_DEVICEADDR                              HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR
#define USB_OTG_HS_ASYNCLISTADDR                           HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR
#define USB_OTG_HS_ENDPOINTLISTADDR                        HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR
#define USB_OTG_HS_TTCTRL                                  HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR
#define USB_OTG_HS_BURSTSIZE                               HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR
#define USB_OTG_HS_TXFILLTUNING                            HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR
#define USB_OTG_HS_ULPI_VIEWPORT                           HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR
#define USB_OTG_HS_ENDPTNAK                                HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR
#define USB_OTG_HS_ENDPTNAKEN                              HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR
#define USB_OTG_HS_PORTSC                                  HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR
#define USB_OTG_HS_OTGSC                                   HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR
#define USB_OTG_HS_USBMODE                                 HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR
#define USB_OTG_HS_ENPDTSETUPSTAT                          HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR
#define USB_OTG_HS_ENDPTPRIME                              HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR
#define USB_OTG_HS_ENDPTFLUSH                              HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR
#define USB_OTG_HS_ENDPTSTAT                               HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR
#define USB_OTG_HS_ENDPTCOMPLETE                           HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR
#define USB_OTG_HS_ENDPTCTRL0                              HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR
#define USB_OTG_HS_ENDPTCTRLn                              HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(0)
// ====================================
// Registers bitmask and shift
// ====================================

#define USB_OTG_HS_ID_SHFT                                               HWIO_USB2_HSIC_USB_OTG_HS_ID_ID_SHFT
#define USB_OTG_HS_ID_REVISON_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_ID_REVISON_BMSK
#define USB_OTG_HS_ID_REVISON_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_ID_REVISON_SHFT
#define USB_OTG_HS_ID_NID_BMSK                                           HWIO_USB2_HSIC_USB_OTG_HS_ID_NID_BMSK
#define USB_OTG_HS_ID_NID_SHFT                                           HWIO_USB2_HSIC_USB_OTG_HS_ID_NID_SHFT
#define USB_OTG_HS_ID_ID_BMSK                                            HWIO_USB2_HSIC_USB_OTG_HS_ID_ID_BMSK
#define USB_OTG_HS_ID_ID_SHFT                                            HWIO_USB2_HSIC_USB_OTG_HS_ID_ID_SHFT

#define USB_OTG_HS_HWGENERAL_SHFT                                               0
#define USB_OTG_HS_HWGENERAL_SM_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_SM_BMSK
#define USB_OTG_HS_HWGENERAL_SM_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_SM_SHFT
#define USB_OTG_HS_HWGENERAL_PHYM_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYM_BMSK
#define USB_OTG_HS_HWGENERAL_PHYM_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYM_SHFT
#define USB_OTG_HS_HWGENERAL_PHYW_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYW_BMSK
#define USB_OTG_HS_HWGENERAL_PHYW_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYW_SHFT
#define USB_OTG_HS_HWGENERAL_BWT_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_BWT_BMSK
#define USB_OTG_HS_HWGENERAL_BWT_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_BWT_SHFT
#define USB_OTG_HS_HWGENERAL_CLCK_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_CLCK_BMSK
#define USB_OTG_HS_HWGENERAL_CLCK_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_CLCK_SHFT
#define USB_OTG_HS_HWGENERAL_RT_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_RT_BMSK
#define USB_OTG_HS_HWGENERAL_RT_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_RT_SHFT

#define USB_OTG_HS_HWHOST_SHFT                                                  0
#define USB_OTG_HS_HWHOST_TTPER_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTPER_BMSK
#define USB_OTG_HS_HWHOST_TTPER_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTPER_SHFT
#define USB_OTG_HS_HWHOST_TTASY_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTASY_BMSK
#define USB_OTG_HS_HWHOST_TTASY_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTASY_SHFT
#define USB_OTG_HS_HWHOST_NPORT_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_NPORT_BMSK
#define USB_OTG_HS_HWHOST_NPORT_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_NPORT_SHFT
#define USB_OTG_HS_HWHOST_HC_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_HC_BMSK
#define USB_OTG_HS_HWHOST_HC_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_HC_SHFT

#define USB_OTG_HS_HWDEVICE_SHFT                                                0
#define USB_OTG_HS_HWDEVICE_DEVEP_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DEVEP_BMSK
#define USB_OTG_HS_HWDEVICE_DEVEP_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DEVEP_SHFT
#define USB_OTG_HS_HWDEVICE_DC_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DC_BMSK
#define USB_OTG_HS_HWDEVICE_DC_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DC_SHFT

#define USB_OTG_HS_HWTXBUF_SHFT                                                 0
#define USB_OTG_HS_HWTXBUF_TXLCR_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXLCR_BMSK
#define USB_OTG_HS_HWTXBUF_TXLCR_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXLCR_SHFT
#define USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK                                HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK
#define USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT                                HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT
#define USB_OTG_HS_HWTXBUF_TXADD_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXADD_BMSK
#define USB_OTG_HS_HWTXBUF_TXADD_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXADD_SHFT
#define USB_OTG_HS_HWTXBUF_TCBURST_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXBURST_BMSK
#define USB_OTG_HS_HWTXBUF_TCBURST_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXBURST_SHFT

#define USB_OTG_HS_HWRXBUF_SHFT                                                 0
#define USB_OTG_HS_HWRXBUF_RX_ADD_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_ADD_BMSK
#define USB_OTG_HS_HWRXBUF_RX_ADD_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_ADD_SHFT
#define USB_OTG_HS_HWRXBUF_RX_BURST_BMSK                                 HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_BURST_BMSK
#define USB_OTG_HS_HWRXBUF_RX_BURST_SHFT                                 HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_BURST_SHFT

#define USB_OTG_HS_SCRATCH_RAMn_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_BMSK 
#define USB_OTG_HS_SCRATCH_RAMn_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_SHFT
 
#define USB_OTG_HS_GPTIMER0LD_SHFT                                              0
#define USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK                                 HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK
#define USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT                                 HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT

#define USB_OTG_HS_GPTIMER0CTRL_SHFT                                            0
#define USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT
#define USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT
#define USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT
#define USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT

#define USB_OTG_HS_GPTIMER1LD_SHFT                                              0
#define USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK                                 HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK
#define USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT                                 HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT

#define USB_OTG_HS_GPTIMER1CTRL_SHFT                                            0
#define USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT
#define USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT
#define USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT
#define USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT

#define USB_OTG_HS_AHB_BURST_SHFT                                               0
#define USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK
#define USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT

#define USB_OTG_HS_XTOR_STS_SHFT                                                0
#define USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK                            HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK
#define USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT                            HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT

#define USB_OTG_HS_AHB_MODE_SHFT                                                0
#define USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK
#define USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT
#define USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK                      HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK
#define USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT                      HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT
#define USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK
#define USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT

#define USB_OTG_HS_GEN_CONFIG_SHFT                                              0
#define USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK                HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_BMSK
#define USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT                HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_SHFT
#define USB_OTG_HS_GEN_CONFIG_DEV_PE_NAK_CTRL_BMSK                       HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_BMSK
#define USB_OTG_HS_GEN_CONFIG_DEV_PE_NAK_CTRL_SHFT                       HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_SHFT
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK                HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT                HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK               HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT               HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT

#define USB_OTG_HS_GEN_CONFIG2_SESS_VLD_CTRL_EN_BMSK                     HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_BMSK
#define USB_OTG_HS_GEN_CONFIG2_SESS_VLD_CTRL_EN_SHFT                     HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_SHFT

#define USB_OTG_HS_CAPLENGTH_SHFT                                               0
#define USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK                              HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK
#define USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT                              HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT

#define USB_OTG_HS_HCIVERSION_SHFT                                              0
#define USB_OTG_HS_HCIVERSION_HCIVERSION_BMSK                            HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_HCIVERSION_BMSK
#define USB_OTG_HS_HCIVERSION_HCIVERSION_SHFT                            HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_HCIVERSION_SHFT

#define USB_OTG_HS_HCSPARAMS_SHFT                                               0
#define USB_OTG_HS_HCSPARAMS_N_TT_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_TT_BMSK
#define USB_OTG_HS_HCSPARAMS_N_TT_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_TT_SHFT
#define USB_OTG_HS_HCSPARAMS_N_PTT_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PTT_BMSK
#define USB_OTG_HS_HCSPARAMS_N_PTT_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PTT_SHFT
#define USB_OTG_HS_HCSPARAMS_PI_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PI_BMSK
#define USB_OTG_HS_HCSPARAMS_PI_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PI_SHFT
#define USB_OTG_HS_HCSPARAMS_N_CC_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_CC_BMSK
#define USB_OTG_HS_HCSPARAMS_N_CC_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_CC_SHFT
#define USB_OTG_HS_HCSPARAMS_N_PCC_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PCC_BMSK
#define USB_OTG_HS_HCSPARAMS_N_PCC_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PCC_SHFT
#define USB_OTG_HS_HCSPARAMS_PPC_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PPC_BMSK
#define USB_OTG_HS_HCSPARAMS_PPC_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PPC_SHFT
#define USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK                                HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK
#define USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT                                HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT

#define USB_OTG_HS_HCCPARAMS_SHFT                                               0
#define USB_OTG_HS_HCCPARAMS_EECP_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_EECP_BMSK
#define USB_OTG_HS_HCCPARAMS_EECP_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_EECP_SHFT
#define USB_OTG_HS_HCCPARAMS_IST_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IST_BMSK
#define USB_OTG_HS_HCCPARAMS_IST_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IST_SHFT
#define USB_OTG_HS_HCCPARAMS_ASP_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ASP_BMSK
#define USB_OTG_HS_HCCPARAMS_ASP_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ASP_SHFT
#define USB_OTG_HS_HCCPARAMS_PFL_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PFL_BMSK
#define USB_OTG_HS_HCCPARAMS_PFL_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PFL_SHFT
#define USB_OTG_HS_HCCPARAMS_ADC_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADC_BMSK
#define USB_OTG_HS_HCCPARAMS_ADC_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADC_SHFT

#define USB_OTG_HS_DCIVERSION_SHFT                                              0
#define USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK                            HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK
#define USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT                            HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT

#define USB_OTG_HS_DCCPARAMS_SHFT                                               0
#define USB_OTG_HS_DCCPARAMS_HC_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_HC_BMSK
#define USB_OTG_HS_DCCPARAMS_HC_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_HC_SHFT
#define USB_OTG_HS_DCCPARAMS_DC_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DC_BMSK
#define USB_OTG_HS_DCCPARAMS_DC_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DC_SHFT
#define USB_OTG_HS_DCCPARAMS_DEN_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_BMSK
#define USB_OTG_HS_DCCPARAMS_DEN_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_SHFT

#define USB_OTG_HS_USBCMD_SHFT                                                  0
#define USB_OTG_HS_USBCMD_RST_CTRL_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_BMSK
#define USB_OTG_HS_USBCMD_RST_CTRL_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_SHFT
#define USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK
#define USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT
#define USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK                           HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK
#define USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT                           HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT
#define USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK
#define USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT
#define USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK
#define USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT
#define USB_OTG_HS_USBCMD_ITC_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ITC_BMSK
#define USB_OTG_HS_USBCMD_ITC_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ITC_SHFT
#define USB_OTG_HS_USBCMD_FS2_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS2_BMSK
#define USB_OTG_HS_USBCMD_FS2_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS2_SHFT
#define USB_OTG_HS_USBCMD_SUTW_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SUTW_BMSK
#define USB_OTG_HS_USBCMD_SUTW_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SUTW_SHFT
#define USB_OTG_HS_USBCMD_ATDTW_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ATDTW_BMSK
#define USB_OTG_HS_USBCMD_ATDTW_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ATDTW_SHFT
#define USB_OTG_HS_USBCMD_ASPE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASPE_BMSK
#define USB_OTG_HS_USBCMD_ASPE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASPE_SHFT
#define USB_OTG_HS_USBCMD_ASP_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASP_BMSK
#define USB_OTG_HS_USBCMD_ASP_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASP_SHFT
#define USB_OTG_HS_USBCMD_LR_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_LR_BMSK
#define USB_OTG_HS_USBCMD_LR_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_LR_SHFT
#define USB_OTG_HS_USBCMD_IAA_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_IAA_BMSK
#define USB_OTG_HS_USBCMD_IAA_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_IAA_SHFT
#define USB_OTG_HS_USBCMD_ASE_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASE_BMSK
#define USB_OTG_HS_USBCMD_ASE_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASE_SHFT
#define USB_OTG_HS_USBCMD_PSE_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_PSE_BMSK
#define USB_OTG_HS_USBCMD_PSE_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_PSE_SHFT
#define USB_OTG_HS_USBCMD_FS_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS_BMSK
#define USB_OTG_HS_USBCMD_FS_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS_SHFT
#define USB_OTG_HS_USBCMD_RST_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RST_BMSK
#define USB_OTG_HS_USBCMD_RST_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RST_SHFT
#define USB_OTG_HS_USBCMD_RS_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RS_BMSK
#define USB_OTG_HS_USBCMD_RS_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RS_SHFT

#define USB_OTG_HS_USBSTS_SHFT                                                  0
#define USB_OTG_HS_USBSTS_ULPI_INTR_BMSK                                 HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPI_INTR_BMSK
#define USB_OTG_HS_USBSTS_ULPI_INTR_SHFT                                 HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPI_INTR_SHFT
#define USB_OTG_HS_USBSTS_TI1_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI1_BMSK
#define USB_OTG_HS_USBSTS_TI1_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI1_SHFT
#define USB_OTG_HS_USBSTS_TI0_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI0_BMSK
#define USB_OTG_HS_USBSTS_TI0_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI0_SHFT
#define USB_OTG_HS_USBSTS_UPI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UPI_BMSK
#define USB_OTG_HS_USBSTS_UPI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UPI_SHFT
#define USB_OTG_HS_USBSTS_UAI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UAI_BMSK
#define USB_OTG_HS_USBSTS_UAI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UAI_SHFT
#define USB_OTG_HS_USBSTS_NAKI_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_NAKI_BMSK
#define USB_OTG_HS_USBSTS_NAKI_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_NAKI_SHFT
#define USB_OTG_HS_USBSTS_AS_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AS_BMSK
#define USB_OTG_HS_USBSTS_AS_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AS_SHFT
#define USB_OTG_HS_USBSTS_PS_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PS_BMSK
#define USB_OTG_HS_USBSTS_PS_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PS_SHFT
#define USB_OTG_HS_USBSTS_RCL_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_RCL_BMSK
#define USB_OTG_HS_USBSTS_RCL_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_RCL_SHFT
#define USB_OTG_HS_USBSTS_HCH_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_HCH_BMSK
#define USB_OTG_HS_USBSTS_HCH_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_HCH_SHFT
#define USB_OTG_HS_USBSTS_ULPII_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPII_BMSK
#define USB_OTG_HS_USBSTS_ULPII_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPII_SHFT
#define USB_OTG_HS_USBSTS_SLI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SLI_BMSK
#define USB_OTG_HS_USBSTS_SLI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SLI_SHFT
#define USB_OTG_HS_USBSTS_SRI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SRI_BMSK
#define USB_OTG_HS_USBSTS_SRI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SRI_SHFT
#define USB_OTG_HS_USBSTS_URI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_URI_BMSK
#define USB_OTG_HS_USBSTS_URI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_URI_SHFT
#define USB_OTG_HS_USBSTS_AAI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AAI_BMSK
#define USB_OTG_HS_USBSTS_AAI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AAI_SHFT
#define USB_OTG_HS_USBSTS_SEI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SEI_BMSK
#define USB_OTG_HS_USBSTS_SEI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SEI_SHFT
#define USB_OTG_HS_USBSTS_FRI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_FRI_BMSK
#define USB_OTG_HS_USBSTS_FRI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_FRI_SHFT
#define USB_OTG_HS_USBSTS_PCI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PCI_BMSK
#define USB_OTG_HS_USBSTS_PCI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PCI_SHFT
#define USB_OTG_HS_USBSTS_UEI_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UEI_BMSK
#define USB_OTG_HS_USBSTS_UEI_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UEI_SHFT
#define USB_OTG_HS_USBSTS_UI_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UI_BMSK
#define USB_OTG_HS_USBSTS_UI_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UI_SHFT

#define USB_OTG_HS_USBINTR_SHFT                                                 0
#define USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK
#define USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT
#define USB_OTG_HS_USBINTR_TIE1_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE1_BMSK
#define USB_OTG_HS_USBINTR_TIE1_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE1_SHFT
#define USB_OTG_HS_USBINTR_TIE0_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE0_BMSK
#define USB_OTG_HS_USBINTR_TIE0_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE0_SHFT
#define USB_OTG_HS_USBINTR_UPIE_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UPIE_BMSK
#define USB_OTG_HS_USBINTR_UPIE_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UPIE_SHFT
#define USB_OTG_HS_USBINTR_UAIE_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UAIE_BMSK
#define USB_OTG_HS_USBINTR_UAIE_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UAIE_SHFT
#define USB_OTG_HS_USBINTR_NAKE_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_NAKE_BMSK
#define USB_OTG_HS_USBINTR_NAKE_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_NAKE_SHFT
#define USB_OTG_HS_USBINTR_ULPIE_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPIE_BMSK
#define USB_OTG_HS_USBINTR_ULPIE_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPIE_SHFT
#define USB_OTG_HS_USBINTR_SLE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SLE_BMSK
#define USB_OTG_HS_USBINTR_SLE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SLE_SHFT
#define USB_OTG_HS_USBINTR_SRE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SRE_BMSK
#define USB_OTG_HS_USBINTR_SRE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SRE_SHFT
#define USB_OTG_HS_USBINTR_URE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_URE_BMSK
#define USB_OTG_HS_USBINTR_URE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_URE_SHFT
#define USB_OTG_HS_USBINTR_AAE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_AAE_BMSK
#define USB_OTG_HS_USBINTR_AAE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_AAE_SHFT
#define USB_OTG_HS_USBINTR_SEE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SEE_BMSK
#define USB_OTG_HS_USBINTR_SEE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SEE_SHFT
#define USB_OTG_HS_USBINTR_FRE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_FRE_BMSK
#define USB_OTG_HS_USBINTR_FRE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_FRE_SHFT
#define USB_OTG_HS_USBINTR_PCE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PCE_BMSK
#define USB_OTG_HS_USBINTR_PCE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PCE_SHFT
#define USB_OTG_HS_USBINTR_UEE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UEE_BMSK
#define USB_OTG_HS_USBINTR_UEE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UEE_SHFT
#define USB_OTG_HS_USBINTR_UE_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UE_BMSK
#define USB_OTG_HS_USBINTR_UE_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UE_SHFT

#define USB_OTG_HS_FRINDEX_SHFT                                                 0
#define USB_OTG_HS_FRINDEX_FRINDEX_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_FRINDEX_BMSK
#define USB_OTG_HS_FRINDEX_FRINDEX_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_FRINDEX_SHFT

#define USB_OTG_HS_PERIODICLISTBASE_SHFT                                        0
#define USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK                         HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK
#define USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT                         HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT

#define USB_OTG_HS_DEVICEADDR_SHFT                                              0
#define USB_OTG_HS_DEVICEADDR_USBADR_BMSK                                HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADR_BMSK
#define USB_OTG_HS_DEVICEADDR_USBADR_SHFT                                HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADR_SHFT
#define USB_OTG_HS_DEVICEADDR_USBADRA_BMSK                               HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADRA_BMSK
#define USB_OTG_HS_DEVICEADDR_USBADRA_SHFT                               HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADRA_SHFT

#define USB_OTG_HS_ASYNCLISTADDR_SHFT                                           0
#define USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK                            HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK
#define USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT                            HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT

#define USB_OTG_HS_ENDPOINTLISTADDR_SHFT                                        0
#define USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK                          HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK
#define USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT                          HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT

#define USB_OTG_HS_TTCTRL_SHFT                                                  0
#define USB_OTG_HS_TTCTRL_TTHA_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_TTHA_BMSK
#define USB_OTG_HS_TTCTRL_TTHA_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_TTHA_SHFT

#define USB_OTG_HS_BURSTSIZE_SHFT                                               0
#define USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK                               HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK
#define USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT                               HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT
#define USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK                               HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK
#define USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT                               HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT

#define USB_OTG_HS_TXFILLTUNING_SHFT                                            0
#define USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK                         HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK
#define USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT                         HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT
#define USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK                         HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK
#define USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT                         HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT
#define USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK
#define USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT

#define USB_OTG_HS_ULPI_VIEWPORT_SHFT                                           0
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK                            HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT                            HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK                             HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT                             HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK                           HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT                           HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK                           HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT                           HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK                          HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT                          HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK                          HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT                          HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT

#define USB_OTG_HS_ENDPTNAK_SHFT                                                0
#define USB_OTG_HS_ENDPTNAK_EPTN_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPTN_BMSK
#define USB_OTG_HS_ENDPTNAK_EPTN_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPTN_SHFT
#define USB_OTG_HS_ENDPTNAK_EPRN_BMSK                                    HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPRN_BMSK
#define USB_OTG_HS_ENDPTNAK_EPRN_SHFT                                    HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPRN_SHFT

#define USB_OTG_HS_ENDPTNAKEN_SHFT                                              0
#define USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK                                 HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK
#define USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT                                 HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT
#define USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK                                 HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK
#define USB_OTG_HS_ENDPTNAKEN_EPRNE_SHFT                                 HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK

#define USB_OTG_HS_PORTSC_SHFT                                                  0
#define USB_OTG_HS_PORTSC_PTS_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTS_BMSK
#define USB_OTG_HS_PORTSC_PTS_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTS_SHFT
#define USB_OTG_HS_PORTSC_STS_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_STS_BMSK
#define USB_OTG_HS_PORTSC_STS_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_STS_SHFT
#define USB_OTG_HS_PORTSC_PTW_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTW_BMSK
#define USB_OTG_HS_PORTSC_PTW_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTW_SHFT
#define USB_OTG_HS_PORTSC_PSPD_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PSPD_BMSK
#define USB_OTG_HS_PORTSC_PSPD_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PSPD_SHFT
#define USB_OTG_HS_PORTSC_PFSC_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PFSC_BMSK
#define USB_OTG_HS_PORTSC_PFSC_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PFSC_SHFT
#define USB_OTG_HS_PORTSC_PHCD_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PHCD_BMSK
#define USB_OTG_HS_PORTSC_PHCD_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PHCD_SHFT
#define USB_OTG_HS_PORTSC_WKOC_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKOC_BMSK
#define USB_OTG_HS_PORTSC_WKOC_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKOC_SHFT
#define USB_OTG_HS_PORTSC_WKDS_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKDS_BMSK
#define USB_OTG_HS_PORTSC_WKDS_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKDS_SHFT
#define USB_OTG_HS_PORTSC_WKCN_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKCN_BMSK
#define USB_OTG_HS_PORTSC_WKCN_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKCN_SHFT
#define USB_OTG_HS_PORTSC_PTC_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTC_BMSK
#define USB_OTG_HS_PORTSC_PTC_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTC_SHFT
#define USB_OTG_HS_PORTSC_PIC_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PIC_BMSK
#define USB_OTG_HS_PORTSC_PIC_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PIC_SHFT
#define USB_OTG_HS_PORTSC_PO_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PO_BMSK
#define USB_OTG_HS_PORTSC_PO_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PO_SHFT
#define USB_OTG_HS_PORTSC_PP_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PP_BMSK
#define USB_OTG_HS_PORTSC_PP_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PP_SHFT
#define USB_OTG_HS_PORTSC_LS_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_LS_BMSK
#define USB_OTG_HS_PORTSC_LS_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_LS_SHFT
#define USB_OTG_HS_PORTSC_HSP_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_HSP_BMSK
#define USB_OTG_HS_PORTSC_HSP_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_HSP_SHFT
#define USB_OTG_HS_PORTSC_PR_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PR_BMSK
#define USB_OTG_HS_PORTSC_PR_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PR_SHFT
#define USB_OTG_HS_PORTSC_SUSP_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_SUSP_BMSK
#define USB_OTG_HS_PORTSC_SUSP_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_SUSP_SHFT
#define USB_OTG_HS_PORTSC_FPR_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_FPR_BMSK
#define USB_OTG_HS_PORTSC_FPR_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_FPR_SHFT
#define USB_OTG_HS_PORTSC_OCC_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCC_BMSK
#define USB_OTG_HS_PORTSC_OCC_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCC_SHFT
#define USB_OTG_HS_PORTSC_OCA_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCA_BMSK
#define USB_OTG_HS_PORTSC_OCA_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCA_SHFT
#define USB_OTG_HS_PORTSC_PEC_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PEC_BMSK
#define USB_OTG_HS_PORTSC_PEC_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PEC_SHFT
#define USB_OTG_HS_PORTSC_PE_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PE_BMSK
#define USB_OTG_HS_PORTSC_PE_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PE_SHFT
#define USB_OTG_HS_PORTSC_CSC_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CSC_BMSK
#define USB_OTG_HS_PORTSC_CSC_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CSC_SHFT
#define USB_OTG_HS_PORTSC_CCS_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CCS_BMSK
#define USB_OTG_HS_PORTSC_CCS_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CCS_SHFT

#define USB_OTG_HS_OTGSC_SHFT                                                   0
#define USB_OTG_HS_OTGSC_DPIE_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIE_BMSK
#define USB_OTG_HS_OTGSC_DPIE_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIE_SHFT
#define USB_OTG_HS_OTGSC_1MSE_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSE_BMSK
#define USB_OTG_HS_OTGSC_1MSE_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSE_SHFT
#define USB_OTG_HS_OTGSC_BSEIE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIE_BMSK
#define USB_OTG_HS_OTGSC_BSEIE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIE_SHFT
#define USB_OTG_HS_OTGSC_BSVIE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIE_BMSK
#define USB_OTG_HS_OTGSC_BSVIE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIE_SHFT
#define USB_OTG_HS_OTGSC_ASVIE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIE_BMSK
#define USB_OTG_HS_OTGSC_ASVIE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIE_SHFT
#define USB_OTG_HS_OTGSC_AVVIE_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIE_BMSK
#define USB_OTG_HS_OTGSC_AVVIE_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIE_SHFT
#define USB_OTG_HS_OTGSC_IDIE_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIE_BMSK
#define USB_OTG_HS_OTGSC_IDIE_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIE_SHFT
#define USB_OTG_HS_OTGSC_DPIS_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIS_BMSK
#define USB_OTG_HS_OTGSC_DPIS_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIS_SHFT
#define USB_OTG_HS_OTGSC_1MSS_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSS_BMSK
#define USB_OTG_HS_OTGSC_1MSS_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSS_SHFT
#define USB_OTG_HS_OTGSC_BSEIS_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIS_BMSK
#define USB_OTG_HS_OTGSC_BSEIS_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIS_SHFT
#define USB_OTG_HS_OTGSC_BSVIS_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIS_BMSK
#define USB_OTG_HS_OTGSC_BSVIS_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIS_SHFT
#define USB_OTG_HS_OTGSC_ASVIS_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIS_BMSK
#define USB_OTG_HS_OTGSC_ASVIS_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIS_SHFT
#define USB_OTG_HS_OTGSC_AVVIS_BMSK                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIS_BMSK
#define USB_OTG_HS_OTGSC_AVVIS_SHFT                                      HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIS_SHFT
#define USB_OTG_HS_OTGSC_IDIS_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIS_BMSK
#define USB_OTG_HS_OTGSC_IDIS_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIS_SHFT
#define USB_OTG_HS_OTGSC_DPS_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPS_BMSK
#define USB_OTG_HS_OTGSC_DPS_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPS_SHFT
#define USB_OTG_HS_OTGSC_1MST_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MST_BMSK
#define USB_OTG_HS_OTGSC_1MST_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MST_SHFT
#define USB_OTG_HS_OTGSC_BSE_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSE_BMSK
#define USB_OTG_HS_OTGSC_BSE_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSE_SHFT
#define USB_OTG_HS_OTGSC_BSV_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSV_BMSK
#define USB_OTG_HS_OTGSC_BSV_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSV_SHFT
#define USB_OTG_HS_OTGSC_ASV_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASV_BMSK
#define USB_OTG_HS_OTGSC_ASV_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASV_SHFT
#define USB_OTG_HS_OTGSC_AVV_BMSK                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVV_BMSK
#define USB_OTG_HS_OTGSC_AVV_SHFT                                        HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVV_SHFT
#define USB_OTG_HS_OTGSC_ID_BMSK                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ID_BMSK
#define USB_OTG_HS_OTGSC_ID_SHFT                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ID_SHFT
#define USB_OTG_HS_OTGSC_HABA_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HABA_BMSK
#define USB_OTG_HS_OTGSC_HABA_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HABA_SHFT
#define USB_OTG_HS_OTGSC_HADP_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HADP_BMSK
#define USB_OTG_HS_OTGSC_HADP_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HADP_SHFT
#define USB_OTG_HS_OTGSC_IDPU_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDPU_BMSK
#define USB_OTG_HS_OTGSC_IDPU_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDPU_SHFT
#define USB_OTG_HS_OTGSC_DP_BMSK                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DP_BMSK
#define USB_OTG_HS_OTGSC_DP_SHFT                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DP_SHFT
#define USB_OTG_HS_OTGSC_OT_BMSK                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OT_BMSK
#define USB_OTG_HS_OTGSC_OT_SHFT                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OT_SHFT
#define USB_OTG_HS_OTGSC_HAAR_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HAAR_BMSK
#define USB_OTG_HS_OTGSC_HAAR_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HAAR_SHFT
#define USB_OTG_HS_OTGSC_VC_BMSK                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VC_BMSK
#define USB_OTG_HS_OTGSC_VC_SHFT                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VC_SHFT
#define USB_OTG_HS_OTGSC_VD_BMSK                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VD_BMSK
#define USB_OTG_HS_OTGSC_VD_SHFT                                         HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VD_SHFT

#define USB_OTG_HS_USBMODE_SHFT                                                 0
#define USB_OTG_HS_USBMODE_VBPS_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_VBPS_BMSK
#define USB_OTG_HS_USBMODE_VBPS_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_VBPS_SHFT
#define USB_OTG_HS_USBMODE_SDIS_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SDIS_BMSK
#define USB_OTG_HS_USBMODE_SDIS_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SDIS_SHFT
#define USB_OTG_HS_USBMODE_SLOM_BMSK                                     HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SLOM_BMSK
#define USB_OTG_HS_USBMODE_SLOM_SHFT                                     HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SLOM_SHFT
#define USB_OTG_HS_USBMODE_ES_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ES_BMSK
#define USB_OTG_HS_USBMODE_ES_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ES_SHFT
#define USB_OTG_HS_USBMODE_CM_BMSK                                       HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_CM_BMSK
#define USB_OTG_HS_USBMODE_CM_SHFT                                       HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_CM_SHFT

#define USB_OTG_HS_ENPDTSETUPSTAT_SHFT                                          0
#define USB_OTG_HS_ENPDTSETUPSTAT_ENDPTSETUPSTAT_BMSK                    HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_BMSK
#define USB_OTG_HS_ENPDTSETUPSTAT_ENDPTSETUPSTAT_SHFT                    HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHFT

#define USB_OTG_HS_ENDPTPRIME_SHFT                                              0
#define USB_OTG_HS_ENDPTPRIME_PETB_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PETB_BMSK
#define USB_OTG_HS_ENDPTPRIME_PETB_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PETB_SHFT
#define USB_OTG_HS_ENDPTPRIME_PERB_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PERB_BMSK
#define USB_OTG_HS_ENDPTPRIME_PERB_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PERB_SHFT

#define USB_OTG_HS_ENDPTFLUSH_SHFT                                              0
#define USB_OTG_HS_ENDPTFLUSH_FETB_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FETB_BMSK
#define USB_OTG_HS_ENDPTFLUSH_FETB_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FETB_SHFT
#define USB_OTG_HS_ENDPTFLUSH_FERB_BMSK                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FERB_BMSK
#define USB_OTG_HS_ENDPTFLUSH_FERB_SHFT                                  HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FERB_SHFT

#define USB_OTG_HS_ENDPTSTAT_SHFT                                               0
#define USB_OTG_HS_ENDPTSTAT_ETBR_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ETBR_BMSK
#define USB_OTG_HS_ENDPTSTAT_ETBR_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ETBR_SHFT
#define USB_OTG_HS_ENDPTSTAT_ERBR_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ERBR_BMSK
#define USB_OTG_HS_ENDPTSTAT_ERBR_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ERBR_SHFT

#define USB_OTG_HS_ENDPTCOMPLETE_SHFT                                           0
#define USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK                               HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK
#define USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT                               HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT
#define USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK                               HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK
#define USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT                               HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT

#define USB_OTG_HS_ENDPTCTRL0_SHFT                                              0
#define USB_OTG_HS_ENDPTCTRL0_TXE_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXE_BMSK
#define USB_OTG_HS_ENDPTCTRL0_TXE_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXE_SHFT
#define USB_OTG_HS_ENDPTCTRL0_TXT_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXT_BMSK
#define USB_OTG_HS_ENDPTCTRL0_TXT_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXT_SHFT
#define USB_OTG_HS_ENDPTCTRL0_TXS_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXS_BMSK
#define USB_OTG_HS_ENDPTCTRL0_TXS_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXS_SHFT
#define USB_OTG_HS_ENDPTCTRL0_RXE_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXE_BMSK
#define USB_OTG_HS_ENDPTCTRL0_RXE_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXE_SHFT
#define USB_OTG_HS_ENDPTCTRL0_RXT_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXT_BMSK
#define USB_OTG_HS_ENDPTCTRL0_RXT_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXT_SHFT
#define USB_OTG_HS_ENDPTCTRL0_RXS_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXS_BMSK
#define USB_OTG_HS_ENDPTCTRL0_RXS_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXS_SHFT

#define USB_OTG_HS_ENDPTCTRLn_SHFT                                              0
#define USB_OTG_HS_ENDPTCTRLn_TXE_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXE_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXE_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXE_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXR_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXR_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXR_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXR_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXI_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXI_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXI_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXI_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXT_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXT_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXT_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXT_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXD_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXD_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXD_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXD_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXS_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXS_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXS_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXS_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXE_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXE_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXE_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXE_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXR_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXR_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXR_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXR_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXI_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXI_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXI_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXI_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXT_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXT_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXT_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXT_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXD_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXD_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXD_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXD_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXS_BMSK                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXS_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXS_SHFT                                   HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXS_SHFT

#else /* for MSM8956 */
// ====================================
// Register offset address
// ====================================

#define USB_OTG_HS_ID                                      HWIO_USB2_HS_USB_OTG_HS_ID_ADDR 
#define USB_OTG_HS_HWGENERAL                               HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_ADDR
#define USB_OTG_HS_HWHOST                                  HWIO_USB2_HS_USB_OTG_HS_HWHOST_ADDR
#define USB_OTG_HS_HWDEVICE                                HWIO_USB2_HS_USB_OTG_HS_HWDEVICE_ADDR
#define USB_OTG_HS_HWTXBUF                                 HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_ADDR
#define USB_OTG_HS_HWRXBUF                                 HWIO_USB2_HS_USB_OTG_HS_HWRXBUF_ADDR
#define USB_OTG_HS_SCRATCH_RAMn                            HWIO_USB2_HS_USB_OTG_HS_SCRATCH_RAMn_ADDR(0) 
#define USB_OTG_HS_GPTIMER0LD                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0LD_ADDR
#define USB_OTG_HS_GPTIMER0CTRL                            HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_ADDR
#define USB_OTG_HS_GPTIMER1LD                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1LD_ADDR
#define USB_OTG_HS_GPTIMER1CTRL                            HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_ADDR
#define USB_OTG_HS_AHB_BURST                               HWIO_USB2_HS_USB_OTG_HS_AHB_BURST_ADDR
#define USB_OTG_HS_XTOR_STS                                HWIO_USB2_HS_USB_OTG_HS_XTOR_STS_ADDR
#define USB_OTG_HS_AHB_MODE                                HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_ADDR
#define USB_OTG_HS_GEN_CONFIG                              HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_ADDR
#define USB_OTG_HS_GEN_CONFIG2                             HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_2_ADDR
#define USB_OTG_HS_CAPLENGTH                               HWIO_USB2_HS_USB_OTG_HS_CAPLENGTH_ADDR
#define USB_OTG_HS_HCSPARAMS                               HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_ADDR
#define USB_OTG_HS_HCCPARAMS                               HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_ADDR
#define USB_OTG_HS_DCIVERSION                              HWIO_USB2_HS_USB_OTG_HS_DCIVERSION_ADDR
#define USB_OTG_HS_DCCPARAMS                               HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_ADDR
#define USB_OTG_HS_USBCMD                                  HWIO_USB2_HS_USB_OTG_HS_USBCMD_ADDR
#define USB_OTG_HS_USBSTS                                  HWIO_USB2_HS_USB_OTG_HS_USBSTS_ADDR
#define USB_OTG_HS_USBINTR                                 HWIO_USB2_HS_USB_OTG_HS_USBINTR_ADDR
#define USB_OTG_HS_FRINDEX                                 HWIO_USB2_HS_USB_OTG_HS_FRINDEX_ADDR
#define USB_OTG_HS_PERIODICLISTBASE                        HWIO_USB2_HS_USB_OTG_HS_PERIODICLISTBASE_ADDR
#define USB_OTG_HS_DEVICEADDR                              HWIO_USB2_HS_USB_OTG_HS_DEVICEADDR_ADDR
#define USB_OTG_HS_ASYNCLISTADDR                           HWIO_USB2_HS_USB_OTG_HS_ASYNCLISTADDR_ADDR
#define USB_OTG_HS_ENDPOINTLISTADDR                        HWIO_USB2_HS_USB_OTG_HS_ENDPOINTLISTADDR_ADDR
#define USB_OTG_HS_TTCTRL                                  HWIO_USB2_HS_USB_OTG_HS_TTCTRL_ADDR
#define USB_OTG_HS_BURSTSIZE                               HWIO_USB2_HS_USB_OTG_HS_BURSTSIZE_ADDR
#define USB_OTG_HS_TXFILLTUNING                            HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_ADDR
#define USB_OTG_HS_ULPI_VIEWPORT                           HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ADDR
#define USB_OTG_HS_ENDPTNAK                                HWIO_USB2_HS_USB_OTG_HS_ENDPTNAK_ADDR
#define USB_OTG_HS_ENDPTNAKEN                              HWIO_USB2_HS_USB_OTG_HS_ENDPTNAKEN_ADDR
#define USB_OTG_HS_PORTSC                                  HWIO_USB2_HS_USB_OTG_HS_PORTSC_ADDR
#define USB_OTG_HS_OTGSC                                   HWIO_USB2_HS_USB_OTG_HS_OTGSC_ADDR
#define USB_OTG_HS_USBMODE                                 HWIO_USB2_HS_USB_OTG_HS_USBMODE_ADDR
#define USB_OTG_HS_ENPDTSETUPSTAT                          HWIO_USB2_HS_USB_OTG_HS_ENDPTSETUPSTAT_ADDR
#define USB_OTG_HS_ENDPTPRIME                              HWIO_USB2_HS_USB_OTG_HS_ENDPTPRIME_ADDR
#define USB_OTG_HS_ENDPTFLUSH                              HWIO_USB2_HS_USB_OTG_HS_ENDPTFLUSH_ADDR
#define USB_OTG_HS_ENDPTSTAT                               HWIO_USB2_HS_USB_OTG_HS_ENDPTSTAT_ADDR
#define USB_OTG_HS_ENDPTCOMPLETE                           HWIO_USB2_HS_USB_OTG_HS_ENDPTCOMPLETE_ADDR
#define USB_OTG_HS_ENDPTCTRL0                              HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_ADDR
#define USB_OTG_HS_ENDPTCTRLn                              HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_ADDR(0)
// ====================================
// Registers bitmask and shift
// ====================================

#define USB_OTG_HS_ID_SHFT                                               HWIO_USB2_HS_USB_OTG_HS_ID_ID_SHFT
#define USB_OTG_HS_ID_REVISON_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_ID_REVISON_BMSK
#define USB_OTG_HS_ID_REVISON_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_ID_REVISON_SHFT
#define USB_OTG_HS_ID_NID_BMSK                                           HWIO_USB2_HS_USB_OTG_HS_ID_NID_BMSK
#define USB_OTG_HS_ID_NID_SHFT                                           HWIO_USB2_HS_USB_OTG_HS_ID_NID_SHFT
#define USB_OTG_HS_ID_ID_BMSK                                            HWIO_USB2_HS_USB_OTG_HS_ID_ID_BMSK
#define USB_OTG_HS_ID_ID_SHFT                                            HWIO_USB2_HS_USB_OTG_HS_ID_ID_SHFT

#define USB_OTG_HS_HWGENERAL_SHFT                                               0
#define USB_OTG_HS_HWGENERAL_SM_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_SM_BMSK
#define USB_OTG_HS_HWGENERAL_SM_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_SM_SHFT
#define USB_OTG_HS_HWGENERAL_PHYM_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_PHYM_BMSK
#define USB_OTG_HS_HWGENERAL_PHYM_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_PHYM_SHFT
#define USB_OTG_HS_HWGENERAL_PHYW_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_PHYW_BMSK
#define USB_OTG_HS_HWGENERAL_PHYW_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_PHYW_SHFT
#define USB_OTG_HS_HWGENERAL_BWT_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_BWT_BMSK
#define USB_OTG_HS_HWGENERAL_BWT_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_BWT_SHFT
#define USB_OTG_HS_HWGENERAL_CLCK_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_CLCK_BMSK
#define USB_OTG_HS_HWGENERAL_CLCK_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_CLCK_SHFT
#define USB_OTG_HS_HWGENERAL_RT_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_RT_BMSK
#define USB_OTG_HS_HWGENERAL_RT_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_HWGENERAL_RT_SHFT

#define USB_OTG_HS_HWHOST_SHFT                                                  0
#define USB_OTG_HS_HWHOST_TTPER_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_HWHOST_TTPER_BMSK
#define USB_OTG_HS_HWHOST_TTPER_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_HWHOST_TTPER_SHFT
#define USB_OTG_HS_HWHOST_TTASY_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_HWHOST_TTASY_BMSK
#define USB_OTG_HS_HWHOST_TTASY_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_HWHOST_TTASY_SHFT
#define USB_OTG_HS_HWHOST_NPORT_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_HWHOST_NPORT_BMSK
#define USB_OTG_HS_HWHOST_NPORT_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_HWHOST_NPORT_SHFT
#define USB_OTG_HS_HWHOST_HC_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_HWHOST_HC_BMSK
#define USB_OTG_HS_HWHOST_HC_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_HWHOST_HC_SHFT

#define USB_OTG_HS_HWDEVICE_SHFT                                                0
#define USB_OTG_HS_HWDEVICE_DEVEP_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HWDEVICE_DEVEP_BMSK
#define USB_OTG_HS_HWDEVICE_DEVEP_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HWDEVICE_DEVEP_SHFT
#define USB_OTG_HS_HWDEVICE_DC_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_HWDEVICE_DC_BMSK
#define USB_OTG_HS_HWDEVICE_DC_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_HWDEVICE_DC_SHFT

#define USB_OTG_HS_HWTXBUF_SHFT                                                 0
#define USB_OTG_HS_HWTXBUF_TXLCR_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXLCR_BMSK
#define USB_OTG_HS_HWTXBUF_TXLCR_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXLCR_SHFT
#define USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK                                HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK
#define USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT                                HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT
#define USB_OTG_HS_HWTXBUF_TXADD_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXADD_BMSK
#define USB_OTG_HS_HWTXBUF_TXADD_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXADD_SHFT
#define USB_OTG_HS_HWTXBUF_TCBURST_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXBURST_BMSK
#define USB_OTG_HS_HWTXBUF_TCBURST_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_HWTXBUF_TXBURST_SHFT

#define USB_OTG_HS_HWRXBUF_SHFT                                                 0
#define USB_OTG_HS_HWRXBUF_RX_ADD_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HWRXBUF_RX_ADD_BMSK
#define USB_OTG_HS_HWRXBUF_RX_ADD_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HWRXBUF_RX_ADD_SHFT
#define USB_OTG_HS_HWRXBUF_RX_BURST_BMSK                                 HWIO_USB2_HS_USB_OTG_HS_HWRXBUF_RX_BURST_BMSK
#define USB_OTG_HS_HWRXBUF_RX_BURST_SHFT                                 HWIO_USB2_HS_USB_OTG_HS_HWRXBUF_RX_BURST_SHFT

#define USB_OTG_HS_SCRATCH_RAMn_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_BMSK 
#define USB_OTG_HS_SCRATCH_RAMn_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_SHFT
 
#define USB_OTG_HS_GPTIMER0LD_SHFT                                              0
#define USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK                                 HWIO_USB2_HS_USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK
#define USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT                                 HWIO_USB2_HS_USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT

#define USB_OTG_HS_GPTIMER0CTRL_SHFT                                            0
#define USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT
#define USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT
#define USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK                             HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT                             HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT
#define USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK
#define USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT

#define USB_OTG_HS_GPTIMER1LD_SHFT                                              0
#define USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK                                 HWIO_USB2_HS_USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK
#define USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT                                 HWIO_USB2_HS_USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT

#define USB_OTG_HS_GPTIMER1CTRL_SHFT                                            0
#define USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT
#define USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT
#define USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK                             HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT                             HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT
#define USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK
#define USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT                              HWIO_USB2_HS_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT

#define USB_OTG_HS_AHB_BURST_SHFT                                               0
#define USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK                              HWIO_USB2_HS_USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK
#define USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT                              HWIO_USB2_HS_USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT

#define USB_OTG_HS_XTOR_STS_SHFT                                                0
#define USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK                            HWIO_USB2_HS_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK
#define USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT                            HWIO_USB2_HS_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT

#define USB_OTG_HS_AHB_MODE_SHFT                                                0
#define USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK                             HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK
#define USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT                             HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT
#define USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK                      HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK
#define USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT                      HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT
#define USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK                              HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK
#define USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT                              HWIO_USB2_HS_USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT

#define USB_OTG_HS_GEN_CONFIG_SHFT                                              0
#define USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK                HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_BMSK
#define USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT                HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_SHFT
#define USB_OTG_HS_GEN_CONFIG_DEV_PE_NAK_CTRL_BMSK                       HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_BMSK
#define USB_OTG_HS_GEN_CONFIG_DEV_PE_NAK_CTRL_SHFT                       HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_SHFT
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK                HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT                HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK               HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK
#define USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT               HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT

#define USB_OTG_HS_GEN_CONFIG2_SESS_VLD_CTRL_EN_BMSK                     HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_BMSK
#define USB_OTG_HS_GEN_CONFIG2_SESS_VLD_CTRL_EN_SHFT                     HWIO_USB2_HS_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_SHFT

#define USB_OTG_HS_CAPLENGTH_SHFT                                               0
#define USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK                              HWIO_USB2_HS_USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK
#define USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT                              HWIO_USB2_HS_USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT

#define USB_OTG_HS_HCIVERSION_SHFT                                              0
#define USB_OTG_HS_HCIVERSION_HCIVERSION_BMSK                            HWIO_USB2_HS_USB_OTG_HS_CAPLENGTH_HCIVERSION_BMSK
#define USB_OTG_HS_HCIVERSION_HCIVERSION_SHFT                            HWIO_USB2_HS_USB_OTG_HS_CAPLENGTH_HCIVERSION_SHFT

#define USB_OTG_HS_HCSPARAMS_SHFT                                               0
#define USB_OTG_HS_HCSPARAMS_N_TT_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_TT_BMSK
#define USB_OTG_HS_HCSPARAMS_N_TT_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_TT_SHFT
#define USB_OTG_HS_HCSPARAMS_N_PTT_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_PTT_BMSK
#define USB_OTG_HS_HCSPARAMS_N_PTT_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_PTT_SHFT
#define USB_OTG_HS_HCSPARAMS_PI_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_PI_BMSK
#define USB_OTG_HS_HCSPARAMS_PI_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_PI_SHFT
#define USB_OTG_HS_HCSPARAMS_N_CC_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_CC_BMSK
#define USB_OTG_HS_HCSPARAMS_N_CC_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_CC_SHFT
#define USB_OTG_HS_HCSPARAMS_N_PCC_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_PCC_BMSK
#define USB_OTG_HS_HCSPARAMS_N_PCC_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_PCC_SHFT
#define USB_OTG_HS_HCSPARAMS_PPC_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_PPC_BMSK
#define USB_OTG_HS_HCSPARAMS_PPC_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_PPC_SHFT
#define USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK                                HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK
#define USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT                                HWIO_USB2_HS_USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT

#define USB_OTG_HS_HCCPARAMS_SHFT                                               0
#define USB_OTG_HS_HCCPARAMS_EECP_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_EECP_BMSK
#define USB_OTG_HS_HCCPARAMS_EECP_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_EECP_SHFT
#define USB_OTG_HS_HCCPARAMS_IST_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_IST_BMSK
#define USB_OTG_HS_HCCPARAMS_IST_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_IST_SHFT
#define USB_OTG_HS_HCCPARAMS_ASP_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_ASP_BMSK
#define USB_OTG_HS_HCCPARAMS_ASP_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_ASP_SHFT
#define USB_OTG_HS_HCCPARAMS_PFL_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_PFL_BMSK
#define USB_OTG_HS_HCCPARAMS_PFL_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_PFL_SHFT
#define USB_OTG_HS_HCCPARAMS_ADC_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_ADC_BMSK
#define USB_OTG_HS_HCCPARAMS_ADC_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_HCCPARAMS_ADC_SHFT

#define USB_OTG_HS_DCIVERSION_SHFT                                              0
#define USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK                            HWIO_USB2_HS_USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK
#define USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT                            HWIO_USB2_HS_USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT

#define USB_OTG_HS_DCCPARAMS_SHFT                                               0
#define USB_OTG_HS_DCCPARAMS_HC_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_HC_BMSK
#define USB_OTG_HS_DCCPARAMS_HC_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_HC_SHFT
#define USB_OTG_HS_DCCPARAMS_DC_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_DC_BMSK
#define USB_OTG_HS_DCCPARAMS_DC_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_DC_SHFT
#define USB_OTG_HS_DCCPARAMS_DEN_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_DEN_BMSK
#define USB_OTG_HS_DCCPARAMS_DEN_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_DEN_SHFT

#define USB_OTG_HS_USBCMD_SHFT                                                  0
#define USB_OTG_HS_USBCMD_RST_CTRL_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_DEN_BMSK
#define USB_OTG_HS_USBCMD_RST_CTRL_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_DCCPARAMS_DEN_SHFT
#define USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK                             HWIO_USB2_HS_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK
#define USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT                             HWIO_USB2_HS_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT
#define USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK                           HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK
#define USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT                           HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT
#define USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK
#define USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT
#define USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK                             HWIO_USB2_HS_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK
#define USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT                             HWIO_USB2_HS_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT
#define USB_OTG_HS_USBCMD_ITC_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_ITC_BMSK
#define USB_OTG_HS_USBCMD_ITC_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_ITC_SHFT
#define USB_OTG_HS_USBCMD_FS2_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_FS2_BMSK
#define USB_OTG_HS_USBCMD_FS2_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_FS2_SHFT
#define USB_OTG_HS_USBCMD_SUTW_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBCMD_SUTW_BMSK
#define USB_OTG_HS_USBCMD_SUTW_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBCMD_SUTW_SHFT
#define USB_OTG_HS_USBCMD_ATDTW_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBCMD_ATDTW_BMSK
#define USB_OTG_HS_USBCMD_ATDTW_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBCMD_ATDTW_SHFT
#define USB_OTG_HS_USBCMD_ASPE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASPE_BMSK
#define USB_OTG_HS_USBCMD_ASPE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASPE_SHFT
#define USB_OTG_HS_USBCMD_ASP_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASP_BMSK
#define USB_OTG_HS_USBCMD_ASP_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASP_SHFT
#define USB_OTG_HS_USBCMD_LR_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_USBCMD_LR_BMSK
#define USB_OTG_HS_USBCMD_LR_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_USBCMD_LR_SHFT
#define USB_OTG_HS_USBCMD_IAA_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_IAA_BMSK
#define USB_OTG_HS_USBCMD_IAA_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_IAA_SHFT
#define USB_OTG_HS_USBCMD_ASE_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASE_BMSK
#define USB_OTG_HS_USBCMD_ASE_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_ASE_SHFT
#define USB_OTG_HS_USBCMD_PSE_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_PSE_BMSK
#define USB_OTG_HS_USBCMD_PSE_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_PSE_SHFT
#define USB_OTG_HS_USBCMD_FS_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_USBCMD_FS_BMSK
#define USB_OTG_HS_USBCMD_FS_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_USBCMD_FS_SHFT
#define USB_OTG_HS_USBCMD_RST_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_RST_BMSK
#define USB_OTG_HS_USBCMD_RST_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBCMD_RST_SHFT
#define USB_OTG_HS_USBCMD_RS_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_USBCMD_RS_BMSK
#define USB_OTG_HS_USBCMD_RS_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_USBCMD_RS_SHFT

#define USB_OTG_HS_USBSTS_SHFT                                                  0
#define USB_OTG_HS_USBSTS_ULPI_INTR_BMSK                                 HWIO_USB2_HS_USB_OTG_HS_USBSTS_ULPI_INTR_BMSK
#define USB_OTG_HS_USBSTS_ULPI_INTR_SHFT                                 HWIO_USB2_HS_USB_OTG_HS_USBSTS_ULPI_INTR_SHFT
#define USB_OTG_HS_USBSTS_TI1_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_TI1_BMSK
#define USB_OTG_HS_USBSTS_TI1_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_TI1_SHFT
#define USB_OTG_HS_USBSTS_TI0_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_TI0_BMSK
#define USB_OTG_HS_USBSTS_TI0_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_TI0_SHFT
#define USB_OTG_HS_USBSTS_UPI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_UPI_BMSK
#define USB_OTG_HS_USBSTS_UPI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_UPI_SHFT
#define USB_OTG_HS_USBSTS_UAI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_UAI_BMSK
#define USB_OTG_HS_USBSTS_UAI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_UAI_SHFT
#define USB_OTG_HS_USBSTS_NAKI_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBSTS_NAKI_BMSK
#define USB_OTG_HS_USBSTS_NAKI_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBSTS_NAKI_SHFT
#define USB_OTG_HS_USBSTS_AS_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_USBSTS_AS_BMSK
#define USB_OTG_HS_USBSTS_AS_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_USBSTS_AS_SHFT
#define USB_OTG_HS_USBSTS_PS_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_USBSTS_PS_BMSK
#define USB_OTG_HS_USBSTS_PS_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_USBSTS_PS_SHFT
#define USB_OTG_HS_USBSTS_RCL_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_RCL_BMSK
#define USB_OTG_HS_USBSTS_RCL_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_RCL_SHFT
#define USB_OTG_HS_USBSTS_HCH_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_HCH_BMSK
#define USB_OTG_HS_USBSTS_HCH_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_HCH_SHFT
#define USB_OTG_HS_USBSTS_ULPII_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBSTS_ULPII_BMSK
#define USB_OTG_HS_USBSTS_ULPII_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBSTS_ULPII_SHFT
#define USB_OTG_HS_USBSTS_SLI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_SLI_BMSK
#define USB_OTG_HS_USBSTS_SLI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_SLI_SHFT
#define USB_OTG_HS_USBSTS_SRI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_SRI_BMSK
#define USB_OTG_HS_USBSTS_SRI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_SRI_SHFT
#define USB_OTG_HS_USBSTS_URI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_URI_BMSK
#define USB_OTG_HS_USBSTS_URI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_URI_SHFT
#define USB_OTG_HS_USBSTS_AAI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_AAI_BMSK
#define USB_OTG_HS_USBSTS_AAI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_AAI_SHFT
#define USB_OTG_HS_USBSTS_SEI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_SEI_BMSK
#define USB_OTG_HS_USBSTS_SEI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_SEI_SHFT
#define USB_OTG_HS_USBSTS_FRI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_FRI_BMSK
#define USB_OTG_HS_USBSTS_FRI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_FRI_SHFT
#define USB_OTG_HS_USBSTS_PCI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_PCI_BMSK
#define USB_OTG_HS_USBSTS_PCI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_PCI_SHFT
#define USB_OTG_HS_USBSTS_UEI_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_UEI_BMSK
#define USB_OTG_HS_USBSTS_UEI_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBSTS_UEI_SHFT
#define USB_OTG_HS_USBSTS_UI_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_USBSTS_UI_BMSK
#define USB_OTG_HS_USBSTS_UI_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_USBSTS_UI_SHFT

#define USB_OTG_HS_USBINTR_SHFT                                                 0
#define USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK                             HWIO_USB2_HS_USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK
#define USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT                             HWIO_USB2_HS_USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT
#define USB_OTG_HS_USBINTR_TIE1_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_TIE1_BMSK
#define USB_OTG_HS_USBINTR_TIE1_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_TIE1_SHFT
#define USB_OTG_HS_USBINTR_TIE0_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_TIE0_BMSK
#define USB_OTG_HS_USBINTR_TIE0_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_TIE0_SHFT
#define USB_OTG_HS_USBINTR_UPIE_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_UPIE_BMSK
#define USB_OTG_HS_USBINTR_UPIE_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_UPIE_SHFT
#define USB_OTG_HS_USBINTR_UAIE_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_UAIE_BMSK
#define USB_OTG_HS_USBINTR_UAIE_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_UAIE_SHFT
#define USB_OTG_HS_USBINTR_NAKE_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_NAKE_BMSK
#define USB_OTG_HS_USBINTR_NAKE_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBINTR_NAKE_SHFT
#define USB_OTG_HS_USBINTR_ULPIE_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_USBINTR_ULPIE_BMSK
#define USB_OTG_HS_USBINTR_ULPIE_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_USBINTR_ULPIE_SHFT
#define USB_OTG_HS_USBINTR_SLE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_SLE_BMSK
#define USB_OTG_HS_USBINTR_SLE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_SLE_SHFT
#define USB_OTG_HS_USBINTR_SRE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_SRE_BMSK
#define USB_OTG_HS_USBINTR_SRE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_SRE_SHFT
#define USB_OTG_HS_USBINTR_URE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_URE_BMSK
#define USB_OTG_HS_USBINTR_URE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_URE_SHFT
#define USB_OTG_HS_USBINTR_AAE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_AAE_BMSK
#define USB_OTG_HS_USBINTR_AAE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_AAE_SHFT
#define USB_OTG_HS_USBINTR_SEE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_SEE_BMSK
#define USB_OTG_HS_USBINTR_SEE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_SEE_SHFT
#define USB_OTG_HS_USBINTR_FRE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_FRE_BMSK
#define USB_OTG_HS_USBINTR_FRE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_FRE_SHFT
#define USB_OTG_HS_USBINTR_PCE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_PCE_BMSK
#define USB_OTG_HS_USBINTR_PCE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_PCE_SHFT
#define USB_OTG_HS_USBINTR_UEE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_UEE_BMSK
#define USB_OTG_HS_USBINTR_UEE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_USBINTR_UEE_SHFT
#define USB_OTG_HS_USBINTR_UE_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBINTR_UE_BMSK
#define USB_OTG_HS_USBINTR_UE_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBINTR_UE_SHFT

#define USB_OTG_HS_FRINDEX_SHFT                                                 0
#define USB_OTG_HS_FRINDEX_FRINDEX_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_FRINDEX_FRINDEX_BMSK
#define USB_OTG_HS_FRINDEX_FRINDEX_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_FRINDEX_FRINDEX_SHFT

#define USB_OTG_HS_PERIODICLISTBASE_SHFT                                        0
#define USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK                         HWIO_USB2_HS_USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK
#define USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT                         HWIO_USB2_HS_USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT

#define USB_OTG_HS_DEVICEADDR_SHFT                                              0
#define USB_OTG_HS_DEVICEADDR_USBADR_BMSK                                HWIO_USB2_HS_USB_OTG_HS_DEVICEADDR_USBADR_BMSK
#define USB_OTG_HS_DEVICEADDR_USBADR_SHFT                                HWIO_USB2_HS_USB_OTG_HS_DEVICEADDR_USBADR_SHFT
#define USB_OTG_HS_DEVICEADDR_USBADRA_BMSK                               HWIO_USB2_HS_USB_OTG_HS_DEVICEADDR_USBADRA_BMSK
#define USB_OTG_HS_DEVICEADDR_USBADRA_SHFT                               HWIO_USB2_HS_USB_OTG_HS_DEVICEADDR_USBADRA_SHFT

#define USB_OTG_HS_ASYNCLISTADDR_SHFT                                           0
#define USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK                            HWIO_USB2_HS_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK
#define USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT                            HWIO_USB2_HS_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT

#define USB_OTG_HS_ENDPOINTLISTADDR_SHFT                                        0
#define USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK                          HWIO_USB2_HS_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK
#define USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT                          HWIO_USB2_HS_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT

#define USB_OTG_HS_TTCTRL_SHFT                                                  0
#define USB_OTG_HS_TTCTRL_TTHA_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_TTCTRL_TTHA_BMSK
#define USB_OTG_HS_TTCTRL_TTHA_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_TTCTRL_TTHA_SHFT

#define USB_OTG_HS_BURSTSIZE_SHFT                                               0
#define USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK                               HWIO_USB2_HS_USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK
#define USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT                               HWIO_USB2_HS_USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT
#define USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK                               HWIO_USB2_HS_USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK
#define USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT                               HWIO_USB2_HS_USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT

#define USB_OTG_HS_TXFILLTUNING_SHFT                                            0
#define USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK                         HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK
#define USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT                         HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT
#define USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK                         HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK
#define USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT                         HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT
#define USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK                             HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK
#define USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT                             HWIO_USB2_HS_USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT

#define USB_OTG_HS_ULPI_VIEWPORT_SHFT                                           0
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK                             HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT                             HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK                            HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT                            HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK                             HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT                             HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK                             HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT                             HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK                           HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT                           HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK                           HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT                           HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK                          HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT                          HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK                          HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK
#define USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT                          HWIO_USB2_HS_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT

#define USB_OTG_HS_ENDPTNAK_SHFT                                                0
#define USB_OTG_HS_ENDPTNAK_EPTN_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_ENDPTNAK_EPTN_BMSK
#define USB_OTG_HS_ENDPTNAK_EPTN_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_ENDPTNAK_EPTN_SHFT
#define USB_OTG_HS_ENDPTNAK_EPRN_BMSK                                    HWIO_USB2_HS_USB_OTG_HS_ENDPTNAK_EPRN_BMSK
#define USB_OTG_HS_ENDPTNAK_EPRN_SHFT                                    HWIO_USB2_HS_USB_OTG_HS_ENDPTNAK_EPRN_SHFT

#define USB_OTG_HS_ENDPTNAKEN_SHFT                                              0
#define USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK                                 HWIO_USB2_HS_USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK
#define USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT                                 HWIO_USB2_HS_USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT
#define USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK                                 HWIO_USB2_HS_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK
#define USB_OTG_HS_ENDPTNAKEN_EPRNE_SHFT                                 HWIO_USB2_HS_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK

#define USB_OTG_HS_PORTSC_SHFT                                                  0
#define USB_OTG_HS_PORTSC_PTS_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PTS_BMSK
#define USB_OTG_HS_PORTSC_PTS_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PTS_SHFT
#define USB_OTG_HS_PORTSC_STS_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_STS_BMSK
#define USB_OTG_HS_PORTSC_STS_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_STS_SHFT
#define USB_OTG_HS_PORTSC_PTW_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PTW_BMSK
#define USB_OTG_HS_PORTSC_PTW_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PTW_SHFT
#define USB_OTG_HS_PORTSC_PSPD_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_PSPD_BMSK
#define USB_OTG_HS_PORTSC_PSPD_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_PSPD_SHFT
#define USB_OTG_HS_PORTSC_PFSC_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_PFSC_BMSK
#define USB_OTG_HS_PORTSC_PFSC_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_PFSC_SHFT
#define USB_OTG_HS_PORTSC_PHCD_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_PHCD_BMSK
#define USB_OTG_HS_PORTSC_PHCD_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_PHCD_SHFT
#define USB_OTG_HS_PORTSC_WKOC_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_WKOC_BMSK
#define USB_OTG_HS_PORTSC_WKOC_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_WKOC_SHFT
#define USB_OTG_HS_PORTSC_WKDS_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_WKDS_BMSK
#define USB_OTG_HS_PORTSC_WKDS_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_WKDS_SHFT
#define USB_OTG_HS_PORTSC_WKCN_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_WKCN_BMSK
#define USB_OTG_HS_PORTSC_WKCN_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_WKCN_SHFT
#define USB_OTG_HS_PORTSC_PTC_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PTC_BMSK
#define USB_OTG_HS_PORTSC_PTC_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PTC_SHFT
#define USB_OTG_HS_PORTSC_PIC_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PIC_BMSK
#define USB_OTG_HS_PORTSC_PIC_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PIC_SHFT
#define USB_OTG_HS_PORTSC_PO_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PO_BMSK
#define USB_OTG_HS_PORTSC_PO_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PO_SHFT
#define USB_OTG_HS_PORTSC_PP_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PP_BMSK
#define USB_OTG_HS_PORTSC_PP_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PP_SHFT
#define USB_OTG_HS_PORTSC_LS_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_LS_BMSK
#define USB_OTG_HS_PORTSC_LS_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_LS_SHFT
#define USB_OTG_HS_PORTSC_HSP_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_HSP_BMSK
#define USB_OTG_HS_PORTSC_HSP_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_HSP_SHFT
#define USB_OTG_HS_PORTSC_PR_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PR_BMSK
#define USB_OTG_HS_PORTSC_PR_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PR_SHFT
#define USB_OTG_HS_PORTSC_SUSP_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_SUSP_BMSK
#define USB_OTG_HS_PORTSC_SUSP_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_PORTSC_SUSP_SHFT
#define USB_OTG_HS_PORTSC_FPR_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_FPR_BMSK
#define USB_OTG_HS_PORTSC_FPR_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_FPR_SHFT
#define USB_OTG_HS_PORTSC_OCC_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_OCC_BMSK
#define USB_OTG_HS_PORTSC_OCC_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_OCC_SHFT
#define USB_OTG_HS_PORTSC_OCA_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_OCA_BMSK
#define USB_OTG_HS_PORTSC_OCA_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_OCA_SHFT
#define USB_OTG_HS_PORTSC_PEC_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PEC_BMSK
#define USB_OTG_HS_PORTSC_PEC_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_PEC_SHFT
#define USB_OTG_HS_PORTSC_PE_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PE_BMSK
#define USB_OTG_HS_PORTSC_PE_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_PORTSC_PE_SHFT
#define USB_OTG_HS_PORTSC_CSC_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_CSC_BMSK
#define USB_OTG_HS_PORTSC_CSC_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_CSC_SHFT
#define USB_OTG_HS_PORTSC_CCS_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_CCS_BMSK
#define USB_OTG_HS_PORTSC_CCS_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_PORTSC_CCS_SHFT

#define USB_OTG_HS_OTGSC_SHFT                                                   0
#define USB_OTG_HS_OTGSC_DPIE_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_DPIE_BMSK
#define USB_OTG_HS_OTGSC_DPIE_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_DPIE_SHFT
#define USB_OTG_HS_OTGSC_1MSE_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_FIELD_1MSE_BMSK
#define USB_OTG_HS_OTGSC_1MSE_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_FIELD_1MSE_SHFT
#define USB_OTG_HS_OTGSC_BSEIE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSEIE_BMSK
#define USB_OTG_HS_OTGSC_BSEIE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSEIE_SHFT
#define USB_OTG_HS_OTGSC_BSVIE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSVIE_BMSK
#define USB_OTG_HS_OTGSC_BSVIE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSVIE_SHFT
#define USB_OTG_HS_OTGSC_ASVIE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_ASVIE_BMSK
#define USB_OTG_HS_OTGSC_ASVIE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_ASVIE_SHFT
#define USB_OTG_HS_OTGSC_AVVIE_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_AVVIE_BMSK
#define USB_OTG_HS_OTGSC_AVVIE_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_AVVIE_SHFT
#define USB_OTG_HS_OTGSC_IDIE_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_IDIE_BMSK
#define USB_OTG_HS_OTGSC_IDIE_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_IDIE_SHFT
#define USB_OTG_HS_OTGSC_DPIS_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_DPIS_BMSK
#define USB_OTG_HS_OTGSC_DPIS_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_DPIS_SHFT
#define USB_OTG_HS_OTGSC_1MSS_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_FIELD_1MSS_BMSK
#define USB_OTG_HS_OTGSC_1MSS_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_FIELD_1MSS_SHFT
#define USB_OTG_HS_OTGSC_BSEIS_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSEIS_BMSK
#define USB_OTG_HS_OTGSC_BSEIS_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSEIS_SHFT
#define USB_OTG_HS_OTGSC_BSVIS_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSVIS_BMSK
#define USB_OTG_HS_OTGSC_BSVIS_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSVIS_SHFT
#define USB_OTG_HS_OTGSC_ASVIS_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_ASVIS_BMSK
#define USB_OTG_HS_OTGSC_ASVIS_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_ASVIS_SHFT
#define USB_OTG_HS_OTGSC_AVVIS_BMSK                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_AVVIS_BMSK
#define USB_OTG_HS_OTGSC_AVVIS_SHFT                                      HWIO_USB2_HS_USB_OTG_HS_OTGSC_AVVIS_SHFT
#define USB_OTG_HS_OTGSC_IDIS_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_IDIS_BMSK
#define USB_OTG_HS_OTGSC_IDIS_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_IDIS_SHFT
#define USB_OTG_HS_OTGSC_DPS_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_DPS_BMSK
#define USB_OTG_HS_OTGSC_DPS_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_DPS_SHFT
#define USB_OTG_HS_OTGSC_1MST_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_FIELD_1MST_BMSK
#define USB_OTG_HS_OTGSC_1MST_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_FIELD_1MST_SHFT
#define USB_OTG_HS_OTGSC_BSE_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSE_BMSK
#define USB_OTG_HS_OTGSC_BSE_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSE_SHFT
#define USB_OTG_HS_OTGSC_BSV_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSV_BMSK
#define USB_OTG_HS_OTGSC_BSV_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_BSV_SHFT
#define USB_OTG_HS_OTGSC_ASV_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_ASV_BMSK
#define USB_OTG_HS_OTGSC_ASV_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_ASV_SHFT
#define USB_OTG_HS_OTGSC_AVV_BMSK                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_AVV_BMSK
#define USB_OTG_HS_OTGSC_AVV_SHFT                                        HWIO_USB2_HS_USB_OTG_HS_OTGSC_AVV_SHFT
#define USB_OTG_HS_OTGSC_ID_BMSK                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_ID_BMSK
#define USB_OTG_HS_OTGSC_ID_SHFT                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_ID_SHFT
#define USB_OTG_HS_OTGSC_HABA_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_HABA_BMSK
#define USB_OTG_HS_OTGSC_HABA_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_HABA_SHFT
#define USB_OTG_HS_OTGSC_HADP_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_HADP_BMSK
#define USB_OTG_HS_OTGSC_HADP_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_HADP_SHFT
#define USB_OTG_HS_OTGSC_IDPU_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_IDPU_BMSK
#define USB_OTG_HS_OTGSC_IDPU_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_IDPU_SHFT
#define USB_OTG_HS_OTGSC_DP_BMSK                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_DP_BMSK
#define USB_OTG_HS_OTGSC_DP_SHFT                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_DP_SHFT
#define USB_OTG_HS_OTGSC_OT_BMSK                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_OT_BMSK
#define USB_OTG_HS_OTGSC_OT_SHFT                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_OT_SHFT
#define USB_OTG_HS_OTGSC_HAAR_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_HAAR_BMSK
#define USB_OTG_HS_OTGSC_HAAR_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_OTGSC_HAAR_SHFT
#define USB_OTG_HS_OTGSC_VC_BMSK                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_VC_BMSK
#define USB_OTG_HS_OTGSC_VC_SHFT                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_VC_SHFT
#define USB_OTG_HS_OTGSC_VD_BMSK                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_VD_BMSK
#define USB_OTG_HS_OTGSC_VD_SHFT                                         HWIO_USB2_HS_USB_OTG_HS_OTGSC_VD_SHFT

#define USB_OTG_HS_USBMODE_SHFT                                                 0
#define USB_OTG_HS_USBMODE_VBPS_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBMODE_VBPS_BMSK
#define USB_OTG_HS_USBMODE_VBPS_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBMODE_VBPS_SHFT
#define USB_OTG_HS_USBMODE_SDIS_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBMODE_SDIS_BMSK
#define USB_OTG_HS_USBMODE_SDIS_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBMODE_SDIS_SHFT
#define USB_OTG_HS_USBMODE_SLOM_BMSK                                     HWIO_USB2_HS_USB_OTG_HS_USBMODE_SLOM_BMSK
#define USB_OTG_HS_USBMODE_SLOM_SHFT                                     HWIO_USB2_HS_USB_OTG_HS_USBMODE_SLOM_SHFT
#define USB_OTG_HS_USBMODE_ES_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBMODE_ES_BMSK
#define USB_OTG_HS_USBMODE_ES_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBMODE_ES_SHFT
#define USB_OTG_HS_USBMODE_CM_BMSK                                       HWIO_USB2_HS_USB_OTG_HS_USBMODE_CM_BMSK
#define USB_OTG_HS_USBMODE_CM_SHFT                                       HWIO_USB2_HS_USB_OTG_HS_USBMODE_CM_SHFT

#define USB_OTG_HS_ENPDTSETUPSTAT_SHFT                                          0
#define USB_OTG_HS_ENPDTSETUPSTAT_ENDPTSETUPSTAT_BMSK                    HWIO_USB2_HS_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_BMSK
#define USB_OTG_HS_ENPDTSETUPSTAT_ENDPTSETUPSTAT_SHFT                    HWIO_USB2_HS_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHFT

#define USB_OTG_HS_ENDPTPRIME_SHFT                                              0
#define USB_OTG_HS_ENDPTPRIME_PETB_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTPRIME_PETB_BMSK
#define USB_OTG_HS_ENDPTPRIME_PETB_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTPRIME_PETB_SHFT
#define USB_OTG_HS_ENDPTPRIME_PERB_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTPRIME_PERB_BMSK
#define USB_OTG_HS_ENDPTPRIME_PERB_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTPRIME_PERB_SHFT

#define USB_OTG_HS_ENDPTFLUSH_SHFT                                              0
#define USB_OTG_HS_ENDPTFLUSH_FETB_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTFLUSH_FETB_BMSK
#define USB_OTG_HS_ENDPTFLUSH_FETB_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTFLUSH_FETB_SHFT
#define USB_OTG_HS_ENDPTFLUSH_FERB_BMSK                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTFLUSH_FERB_BMSK
#define USB_OTG_HS_ENDPTFLUSH_FERB_SHFT                                  HWIO_USB2_HS_USB_OTG_HS_ENDPTFLUSH_FERB_SHFT

#define USB_OTG_HS_ENDPTSTAT_SHFT                                               0
#define USB_OTG_HS_ENDPTSTAT_ETBR_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTSTAT_ETBR_BMSK
#define USB_OTG_HS_ENDPTSTAT_ETBR_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTSTAT_ETBR_SHFT
#define USB_OTG_HS_ENDPTSTAT_ERBR_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTSTAT_ERBR_BMSK
#define USB_OTG_HS_ENDPTSTAT_ERBR_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTSTAT_ERBR_SHFT

#define USB_OTG_HS_ENDPTCOMPLETE_SHFT                                           0
#define USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK                               HWIO_USB2_HS_USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK
#define USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT                               HWIO_USB2_HS_USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT
#define USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK                               HWIO_USB2_HS_USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK
#define USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT                               HWIO_USB2_HS_USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT

#define USB_OTG_HS_ENDPTCTRL0_SHFT                                              0
#define USB_OTG_HS_ENDPTCTRL0_TXE_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_TXE_BMSK
#define USB_OTG_HS_ENDPTCTRL0_TXE_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_TXE_SHFT
#define USB_OTG_HS_ENDPTCTRL0_TXT_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_TXT_BMSK
#define USB_OTG_HS_ENDPTCTRL0_TXT_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_TXT_SHFT
#define USB_OTG_HS_ENDPTCTRL0_TXS_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_TXS_BMSK
#define USB_OTG_HS_ENDPTCTRL0_TXS_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_TXS_SHFT
#define USB_OTG_HS_ENDPTCTRL0_RXE_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_RXE_BMSK
#define USB_OTG_HS_ENDPTCTRL0_RXE_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_RXE_SHFT
#define USB_OTG_HS_ENDPTCTRL0_RXT_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_RXT_BMSK
#define USB_OTG_HS_ENDPTCTRL0_RXT_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_RXT_SHFT
#define USB_OTG_HS_ENDPTCTRL0_RXS_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_RXS_BMSK
#define USB_OTG_HS_ENDPTCTRL0_RXS_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRL0_RXS_SHFT

#define USB_OTG_HS_ENDPTCTRLn_SHFT                                              0
#define USB_OTG_HS_ENDPTCTRLn_TXE_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXE_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXE_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXE_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXR_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXR_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXR_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXR_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXI_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXI_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXI_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXI_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXT_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXT_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXT_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXT_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXD_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXD_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXD_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXD_SHFT
#define USB_OTG_HS_ENDPTCTRLn_TXS_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXS_BMSK
#define USB_OTG_HS_ENDPTCTRLn_TXS_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_TXS_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXE_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXE_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXE_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXE_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXR_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXR_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXR_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXR_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXI_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXI_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXI_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXI_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXT_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXT_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXT_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXT_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXD_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXD_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXD_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXD_SHFT
#define USB_OTG_HS_ENDPTCTRLn_RXS_BMSK                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXS_BMSK
#define USB_OTG_HS_ENDPTCTRLn_RXS_SHFT                                   HWIO_USB2_HS_USB_OTG_HS_ENDPTCTRLn_RXS_SHFT

#endif /*MSM8956*/

// ====================================
// Register Field Value
// ====================================
// -----------------------------------------------------------------
// Note: the defines that are missing in msmhwioreg.h in some tragets.
// -----------------------------------------------------------------
#define USB_OTG_HS_USBMODE_CM_DEVICE_CONTROLLER_DEFAULT_FOR_DEVICE_ONLY_CONTROLLER_FVAL 0x02
#define USB_OTG_HS_USBMODE_SLOM_SETUP_LOCKOUTS_OFF_FVAL                                 0x01
#define USB_OTG_HS_USBCMD_RS_RUN_FVAL                                                   0x01
#define USB_OTG_HS_USBCMD_RS_STOP_FVAL                                                  0x00
#define USB_OTG_HS_GPTIMER0CTRL_GTPRUN_TIMER_STOP_FVAL                                    0
#define USB_OTG_HS_GPTIMER0CTRL_GTPRUN_TIMER_RUN_FVAL                                   0x1
#define USB_OTG_HS_GPTIMER0CTRL_GPTMODE_ONE_SHOT_FVAL                                     0
#define USB_OTG_HS_GPTIMER0CTRL_GPTMODE_REPEAT_FVAL                                     0x1
#define USB_OTG_HS_OTGSC_IDPU_ON_FVAL                                                   0x1
#define USB_OTG_HS_OTGSC_IDPU_OFF_FVAL                                                  0x0
#define USB_OTG_HS_AHB_MODE_HPROT_ONLY_CTRL_XFER_POSTED_FVAL                            0x2
// -----------------------------------------------------------------       
#endif // QHSUSB_DCI_HW_REGISTERS_H
