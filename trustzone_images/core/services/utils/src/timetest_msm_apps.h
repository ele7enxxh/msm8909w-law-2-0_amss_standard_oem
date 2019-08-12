#ifndef TIMETEST_MSM_APPS_H
#define TIMETEST_MSM_APPS_H
/*===========================================================================

           APPLICATION PROCESSOR T I M E T E S T   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 2005, 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/timetest_msm_apps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/07   gfr     Added HSU tasks.
07/05/07   gfr     Added TIMETEST_QIPCALL_TASK_ID
07/03/07   gfr     Added TIMETEST_SECTESTAPP_TASK_ID
06/21/07   gfr     Added TIMETEST_STA_FORK_TASK_ID
05/25/07   gfr     Support for WLAN task IDs.
04/11/07   gfr     Support for BT task IDs.
03/07/07   gfr     Added PDCOMMTCP_TASK_ID.
02/28/07   gfr     Cleanup for 7600 and friends
10/10/06   kvk     Added DS & USB task IDs
08/21/06    dp     Timetest changes
04/25/06    dp     Integrated Ian's timetest changes
01/25/06   ddh     Added support for SLEEP task on aARM.
10/12/05   ddh     Added support for DEM task on aARM.
07/15/05    aw     Added support for SND task on aARM.
01/31/05    cr     Add support for 2 CPU. Created timetest_m7500.h and 
                   timetest_a7500.h to hold the values for ISR and TASK IDs
                   depending on which processor is compiling.
                   Added a bit in LED port to indicate which processor wrote
                   the LED port.
===========================================================================*/

/* ----------------------------------------------------------------------- * 
 *  services/tmc/tmc_apps.c                                                *
 * ----------------------------------------------------------------------- */

#define TIMETEST_TMC_TASK_ID                     0x01
#define TIMETEST_DOG_TASK_ID                     0x02
#define TIMETEST_FS_TASK_ID                      0x03
#define TIMETEST_SFAT_TASK_ID                    0x04
#define TIMETEST_NV_TASK_ID                      0x05
#define TIMETEST_SMEM_PLAYER_TASK_ID             0x06
#define TIMETEST_UI_TASK_ID                      0x07
#define TIMETEST_DIAG_TASK_ID                    0x08
#define TIMETEST_ONCRPC_TASK_ID                  0x09
#define TIMETEST_STA_TASK_ID                     0x0a
#define TIMETEST_STA_REMOTE_TASK_ID              0x0b
#define TIMETEST_HS_TASK_ID                      0x0c
#define TIMETEST_PS_TASK_ID                      0x0d
#define TIMETEST_QDSP_RTOS_TASK_ID               0x0e
#define TIMETEST_GRAPH_TASK_ID                   0x0f
#define TIMETEST_VS_TASK_ID                      0x10
#define TIMETEST_SMD_TASK_ID                     0x11
#define TIMETEST_MP4_TASK_ID                     0x12
#define TIMETEST_MP4_PV_FIRST_TASK_ID            0x21  /* base PV task     */
#define TIMETEST_MP4_PV_PARSER_ID                0x21  /* ensure sync with */
#define TIMETEST_MP4_PV_CPVTEXT_ID               0x22  /* zrex2rex.c and   */
#define TIMETEST_MP4_PV_VIDEO_ID                 0x23  /* qtvplayer.h      */
#define TIMETEST_MP4_PV_RENDERER_ID              0x24
#define TIMETEST_MP4_PV_STREAMER_ID              0x25
#define TIMETEST_MP4_PV_AUDIO_ID                 0x26
#define TIMETEST_MP4_PV_PLAYER_ID                0x27
#define TIMETEST_MP4_PV_DSPTASK_ID               0x28
#define TIMETEST_MP4_PV_LAST_TASK_ID             0x28 /* last of PV tasks */
#define TIMETEST_TIMER_TASK_ID                   0x29
#define TIMETEST_SND_TASK_ID                     0x30
#define TIMETEST_DISP_TASK_ID                    0x31
#define TIMETEST_DEM_TASK_ID                     0x32
#define TIMETEST_SLEEP_TASK_ID                   0x33
#define TIMETEST_QDIAG_TASK_ID                   0x34
#define TIMETEST_JOYST_TASK_ID                   0x35
#define TIMETEST_QVP_AUDIO_TASK_ID               0x37
#define TIMETEST_QVP_MPEG4_TASK_ID               0x38
#define TIMETEST_QVP_APP_TASK_ID                 0x39  
#define TIMETEST_QVP_RTP_TASK_ID                 0x3A
#define TIMETEST_QVP_SIPT_TASK_ID                0x3B
#define TIMETEST_QVP_IO_TASK_ID                  0x3C
#define TIMETEST_QVP_PLAYER_TASK_ID              0x3D
#define TIMETEST_QVP_VFE_TASK_ID                 0x3E
#define TIMETEST_SEC_TASK_ID                     0x3F
#define TIMETEST_SECCRYPTARM_TASK_ID             0x40
#define TIMETEST_SECSSL_TASK_ID                  0x41 
#define TIMETEST_SECTESTAPP_TASK_ID              0x42
#define TIMETEST_DS_TASK_ID                      0x43
#define TIMETEST_DS_CS_UL_TASK_ID                0x44
#define TIMETEST_DS_CS_DL_TASK_ID                0x45
#define TIMETEST_USBDC_TASK_ID                   0x46
#define TIMETEST_PDCOMMTCP_TASK_ID               0x47
#define TIMETEST_BT_TASK_ID                      0x48
#define TIMETEST_BT_EXTPF_TASK_ID                0x49
#define TIMETEST_WLAN_CP_TASK_ID                 0x4a
#define TIMETEST_WLAN_TRP_TASK_ID                0x4b
#define TIMETEST_WLAN_SEC_TASK_ID                0x4c
#define TIMETEST_STA_FORK_TASK_ID                0x4d
#define TIMETEST_QIPCALL_TASK_ID                 0x4e
#define TIMETEST_HSU_HOST_TASK_ID                0x4f
#define TIMETEST_HSU_PERIPHERAL_TASK_ID          0x50
#define TIMETEST_HSU_AL_TASK_ID                  0x51
#define TIMETEST_HSU_AL_MS_TASK_ID               0x52

#define TIMETEST_MAX_TASK_ID                     0x53


/* ----------------------------------------------------------------------- * 
 *  services/oncrpc/oncrpc_proxy.c                                         *
 * ----------------------------------------------------------------------- */

#define TIMETEST_ONCRPC_PROXY_TASK_ID            0x60


/* ----------------------------------------------------------------------- * 
 *  services/rexl4/rexl4.c                                                 *
 * ----------------------------------------------------------------------- */

#define TIMETEST_DPC_TASK_ID                     0x61
#define TIMETEST_REX_TASK_ID                     0x62


/* ----------------------------------------------------------------------- * 
 *  drivers/tramp/tramp_msm_apps.c                                         *
 * ----------------------------------------------------------------------- */

#define TIMETEST_TRAMP_UART2_DM_RX_ISR           0x80
#define TIMETEST_TRAMP_UART2_DM_ISR              0x81
#define TIMETEST_TRAMP_SDC4_1_ISR                0x82
#define TIMETEST_TRAMP_SDC4_0_ISR                0x83
#define TIMETEST_TRAMP_SDC3_1_ISR                0x84
#define TIMETEST_TRAMP_SDC3_0_ISR                0x85
#define TIMETEST_TRAMP_USB_HS_ISR                0x86
#define TIMETEST_TRAMP_UART1_DM_RX_ISR           0x87
#define TIMETEST_TRAMP_UART1_DM_ISR              0x88
#define TIMETEST_TRAMP_TSIF_ISR                  0x89
#define TIMETEST_TRAMP_ARM11_DMA_ISR             0x8a
#define TIMETEST_TRAMP_ARM11_PM_ISR              0x8b
#define TIMETEST_TRAMP_AD_HSSD_ISR               0x8c
#define TIMETEST_TRAMP_AXI_MPU_EBI1_ISR          0x8d
#define TIMETEST_TRAMP_AXI_MPU_SMI_ISR           0x8e
#define TIMETEST_TRAMP_PBUS_ARM11_ISR            0x8f
#define TIMETEST_TRAMP_NAND_OP_DONE_ISR          0x90
#define TIMETEST_TRAMP_NAND_WR_ER_DONE_ISR       0x91
#define TIMETEST_TRAMP_SOFTRESET_ISR             0x92
#define TIMETEST_TRAMP_PWB_I2C_ISR               0x93
#define TIMETEST_TRAMP_GPIO_GROUP2_ISR           0x94
#define TIMETEST_TRAMP_GPIO_GROUP_ISR            0x95
#define TIMETEST_TRAMP_TCHSCRN2_ISR              0x96
#define TIMETEST_TRAMP_TCHSCRN1_ISR              0x97
#define TIMETEST_TRAMP_TCHSCRN_SSBI_ISR          0x98
#define TIMETEST_TRAMP_KEYSENSE_ISR              0x99
#define TIMETEST_TRAMP_SDC2_1_ISR                0x9a
#define TIMETEST_TRAMP_SDC2_0_ISR                0x9b
#define TIMETEST_TRAMP_SDC1_1_ISR                0x9c
#define TIMETEST_TRAMP_SDC1_0_ISR                0x9d
#define TIMETEST_TRAMP_ADSP_A9A11_ISR            0x9e
#define TIMETEST_TRAMP_ADSP_A11_ISR              0x9f
#define TIMETEST_TRAMP_ADM_AARM_ISR              0xa0 
#define TIMETEST_TRAMP_GRAPHICS_ISR              0xa1 
#define TIMETEST_TRAMP_MDP_ISR                   0xa2 
#define TIMETEST_TRAMP_MDDI_CLIENT_ISR           0xa3 
#define TIMETEST_TRAMP_MDDI_EXT_ISR              0xa4 
#define TIMETEST_TRAMP_MDDI_PRI_ISR              0xa5 
#define TIMETEST_TRAMP_USB_OTG_ISR               0xa6 
#define TIMETEST_TRAMP_UART3_RX_ISR              0xa7 
#define TIMETEST_TRAMP_UART2_RX_ISR              0xa8 
#define TIMETEST_TRAMP_UART1_RX_ISR              0xa9 
#define TIMETEST_TRAMP_UART3_ISR                 0xaa 
#define TIMETEST_TRAMP_UART2_ISR                 0xab 
#define TIMETEST_TRAMP_UART1_ISR                 0xac 
#define TIMETEST_TRAMP_DEBUG_TIMER_ISR           0xad 
#define TIMETEST_TRAMP_GP_TIMER_ISR              0xae 
#define TIMETEST_TRAMP_M2A_6_ISR                 0xaf 
#define TIMETEST_TRAMP_M2A_5_ISR                 0xb0
#define TIMETEST_TRAMP_M2A_4_ISR                 0xb1
#define TIMETEST_TRAMP_M2A_3_ISR                 0xb2
#define TIMETEST_TRAMP_M2A_2_ISR                 0xb3
#define TIMETEST_TRAMP_M2A_1_ISR                 0xb4
#define TIMETEST_TRAMP_M2A_0_ISR                 0xb5

#define TIMETEST_MAX_ISR_ID                      0xb6

#endif /* TIMETEST_MSM_APPS_H */

