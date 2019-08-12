#ifndef TIMETST_MSM_MODEM_H
#define TIMETST_MSM_MODEM_H
/*===========================================================================

           APPLICATION PROCESSOR T I M E T E S T   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 1993, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/timetest_msm_modem.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/07   gfr     Added HSU tasks.
07/03/07   gfr     Added TIMETEST_SECTESTAPP_TASK_ID
06/18/07    ry     Added TIMETEST_SNS_TASK_ID.
05/25/07   gfr     Added new task ids.
03/06/07   gfr     Removed obselete task ids.
02/28/07   gfr     Cleanup for 7600 and friends
02/01/07   gsc     Added ID for FC TASK
08/21/06    dp     Timetest changes
04/25/06    dp     Integrated Ian's timetest changes
01/20/06   ddh     Added DEM TASK ID.
08/04/05   jhs     Added GRAPHICS_ISR id.
07/21/05   jhs     Added task id for GPSSRCH_DEMOD_TASK.
03/15/05   ptm     Add task id for PBM.
01/31/05    cr     Add support for 2 CPU. Created timetest_m7500.h and 
                   timetest_a7500.h to hold the values for ISR and TASK IDs
                   depending on which processor is compiling.
                   Added a bit in LED port to indicate which processor wrote
                   the LED port.
===========================================================================*/

/* ----------------------------------------------------------------------- * 
 *  services/tmc/tmc.c                                                     *
 * ----------------------------------------------------------------------- */

#define TIMETEST_TMC_TASK_ID              0x01
#define TIMETEST_SLEEP_TASK_ID            0x02
#define TIMETEST_DOG_TASK_ID              0x03
#define TIMETEST_QDSP_TASK_ID             0x04
#define TIMETEST_QDSP_RTOS_TASK_ID        0x05
#define TIMETEST_VOC_TASK_ID              0x06
#define TIMETEST_SND_TASK_ID              0x07
#define TIMETEST_HS_TASK_ID               0x08
#define TIMETEST_TDSO_TASK_ID             0x09
#define TIMETEST_IDS_TASK_ID              0x0a
#define TIMETEST_FS_TASK_ID               0x0b
#define TIMETEST_SFAT_TASK_ID             0x0c
#define TIMETEST_NV_TASK_ID               0x0d
#define TIMETEST_MC_TASK_ID               0x0e
#define TIMETEST_TX_TASK_ID               0x0f
#define TIMETEST_RX_TASK_ID               0x10
#define TIMETEST_RXTX_TASK_ID             0x11
#define TIMETEST_SRCH_TASK_ID             0x12
#define TIMETEST_CM_TASK_ID               0x14
#define TIMETEST_MMOC_TASK_ID             0x15
#define TIMETEST_HDR_MC_TASK_ID           0x16
#define TIMETEST_HDR_SRCH_TASK_ID         0x17
#define TIMETEST_HDR_RX_TASK_ID           0x18
#define TIMETEST_HDR_TX_TASK_ID           0x19
#define TIMETEST_HDR_BCASTPROT_TASK_ID    0x1a
#define TIMETEST_CD_TASK_ID               0x1b
#define TIMETEST_NF_TASK_ID               0x1c
#define TIMETEST_COMP_TASK_ID             0x1d
#define TIMETEST_BT_TASK_ID               0x1e
#define TIMETEST_BT_RC2SIO_TASK_ID        0x1f
#define TIMETEST_BT_EXTPF_TASK_ID         0x20
#define TIMETEST_MAC_HS_DL_TASK_ID        0x21
#define TIMETEST_DEM_TASK_ID              0x22
#define TIMETEST_UI_TASK_ID               0x23
#define TIMETEST_FC_TASK_ID               0x24
#define TIMETEST_PP_TASK_ID               0x25
#define TIMETEST_GPSFFT_TASK_ID           0x26
#define TIMETEST_LM_TASK_ID               0x27
#define TIMETEST_SM_TM_TASK_ID            0x28
#define TIMETEST_DIAG_TASK_ID             0x29
#define TIMETEST_ONCRPC_TASK_ID           0x2a
#define TIMETEST_STA_TASK_ID              0x2b
#define TIMETEST_CC_TASK_ID               0x2c
#define TIMETEST_SEC_TASK_ID              0x2d
#define TIMETEST_SECCRYPTARM_TASK_ID      0x2e
#define TIMETEST_SECIPSEC_TASK_ID         0x2f
#define TIMETEST_SECSSL_TASK_ID           0x30
#define TIMETEST_SECDRM_TASK_ID           0x31
#define TIMETEST_JOYST_TASK_ID            0x32
#define TIMETEST_TCPTEST_TASK_ID          0x33
#define TIMETEST_UDPTEST_TASK_ID          0x34
#define TIMETEST_QMIP_TASK_ID             0x35
#define TIMETEST_AUTH_TASK_ID             0x36
#define TIMETEST_UIM_TASK_ID              0x37
#define TIMETEST_GSDI_TASK_ID             0x38
#define TIMETEST_VIRTSAT_TASK_ID          0x39
#define TIMETEST_GSTK_TASK_ID             0x3a
#define TIMETEST_DH_TASK_ID               0x3b
#define TIMETEST_GRAPH_TASK_ID            0x3c
#define TIMETEST_VS_TASK_ID               0x3d
#define TIMETEST_USBDC_TASK_ID            0x3e
#define TIMETEST_GSM_L1_TASK_ID           0x3f
#define TIMETEST_GSM_L2_TASK_ID           0x40
#define TIMETEST_GSM_RR_TASK_ID           0x41
#define TIMETEST_GSM_GPRS_MAC_TASK_ID     0x42
#define TIMETEST_GSM_GPRS_RLC_UL_TASK_ID  0x43
#define TIMETEST_GSM_GPRS_RLC_DL_TASK_ID  0x44
#define TIMETEST_GSM_GPRS_LLC_TASK_ID     0x45
#define TIMETEST_MM_TASK_ID               0x46
#define TIMETEST_REG_TASK_ID              0x47
#define TIMETEST_MN_CNM_TASK_ID           0x48
#define TIMETEST_SM_TASK_ID               0x49
#define TIMETEST_GSM_CB_TASK_ID           0x4a
#define TIMETEST_WMS_TASK_ID              0x4b
#define TIMETEST_MDSP_TASK_ID             0x4c
#define TIMETEST_GSDI_UIM_TASK_ID         0x4d
#define TIMETEST_L1_TASK_ID               0x4f
#define TIMETEST_L2_UL_TASK_ID            0x50
#define TIMETEST_L2_DL_TASK_ID            0x51
#define TIMETEST_RRC_TASK_ID              0x52
#define TIMETEST_AL1_TASK_ID              0x53
#define TIMETEST_MENU_UT_TASK_ID          0x54
#define TIMETEST_TC_TASK_ID               0x55
#define TIMETEST_RRC_UT_TASK_ID           0x56
#define TIMETEST_WPLT_TASK_ID             0x57
#define TIMETEST_PS_TASK_ID               0x58
#define TIMETEST_DS_TASK_ID               0x59
#define TIMETEST_GCSD_TASK_ID             0x5a
#define TIMETEST_DS_CS_UL_TASK_ID         0x5b
#define TIMETEST_DS_CS_DL_TASK_ID         0x5c
#define TIMETEST_MGPMC_TASK_ID            0x5d
#define TIMETEST_FTM_TASK_ID              0x5e
#define TIMETEST_BCDB_TASK_ID             0x5f
#define TIMETEST_PBM_TASK_ID              0x60
#define TIMETEST_SMD_TASK_ID              0x61
#define TIMETEST_TIMER_TASK_ID            0x62
#define TIMETEST_QDIAG_TASK_ID            0x63
#define TIMETEST_PDCOMMWMS_TASK_ID        0x64
#define TIMETEST_SECRND_TASK_ID           0x65
#define TIMETEST_SFS_TASK_ID              0x66
#define TIMETEST_SECTESTAPP_TASK_ID       0x67
#define TIMETEST_HSU_HOST_TASK_ID         0x68
#define TIMETEST_HSU_PERIPHERAL_TASK_ID   0x69
#define TIMETEST_HSU_AL_TASK_ID           0x6a
#define TIMETEST_HSU_AL_MS_TASK_ID        0x6b
#define TIMETEST_PDCOMMTCP_TASK_ID        0x6c

/* ----------------------------------------------------------------------- * 
 *  services/oncrpc/oncrpc_proxy.c                                         *
 * ----------------------------------------------------------------------- */

#define TIMETEST_ONCRPC_PROXY_TASK_ID     0x6d


/* ----------------------------------------------------------------------- * 
 *  services/rexl4/rexl4.c                                                 *
 * ----------------------------------------------------------------------- */

#define TIMETEST_DPC_TASK_ID              0x6e
#define TIMETEST_REX_TASK_ID              0x6f

/* ----------------------------------------------------------------------- * 
 *  services/sensors/sns.c                                                 *
 * ----------------------------------------------------------------------- */

#define TIMETEST_SNS_TASK_ID              0x70 

/* ----------------------------------------------------------------------- * 
 *  GAN related ids                                                                   *
 * ----------------------------------------------------------------------- */
#define TIMETEST_UKCC_TASK_ID             0x71

/* ----------------------------------------------------------------------- * 
 *  Location middleware task ID                                                 *
 * ----------------------------------------------------------------------- */

#define TIMETEST_LOC_MIDDLEWARE_TASK_ID   0x72 

/* ----------------------------------------------------------------------- * 
 *  Max task ID                                                            *
 * ----------------------------------------------------------------------- */

#define TIMETEST_FTM_CAMERA_TASK_ID       0x73

#define TIMETEST_MAX_TASK_ID              0x74


/* ----------------------------------------------------------------------- * 
 *  Modem events                                                           *
 * ----------------------------------------------------------------------- */

#define TIMETEST_ACQ_PULLIN_EVENT                0x75
#define TIMETEST_TX_GEN_BUILD_FRAME_EVENT        0x76
#define TIMETEST_TX_GEN_GET_DATA_EVENT           0x77
#define TIMETEST_TX_GEN_TEST_EVENT               0x78
#define TIMETEST_TX_GEN_SET_MOD_TIMING_EVENT     0x79
#define TIMETEST_RX_GEN_DEC_DATA_READ_EVENT      0x7a
#define TIMETEST_RX_GEN_SRCH_HB_EVENT            0x7b
#define TIMETEST_RX_GEN_DEC_TEST_EVENT           0x7c
#define TIMETEST_RX_GEN_DEC_HDR_READ_EVENT       0x7d
#define TIMETEST_RX_GEN_DL_MAINT_EVENT           0x7e
#define TIMETEST_PHYCHAN_COMB_CHAN_EVENT         0x7f


/* ----------------------------------------------------------------------- * 
 *  drivers/tramp/tramp_msm_modem.c                                        *
 * ----------------------------------------------------------------------- */

#define TIMETEST_TRAMP_GRAPHICS_ISR              0x80
#define TIMETEST_TRAMP_A2M_6_CTL_ISR             0x81
#define TIMETEST_TRAMP_A2M_5_CTL_ISR             0x82
#define TIMETEST_TRAMP_A2M_4_CTL_ISR             0x83
#define TIMETEST_TRAMP_A2M_3_CTL_ISR             0x84
#define TIMETEST_TRAMP_A2M_2_CTL_ISR             0x85
#define TIMETEST_TRAMP_A2M_1_CTL_ISR             0x86
#define TIMETEST_TRAMP_A2M_0_CTL_ISR             0x87
#define TIMETEST_TRAMP_MWDOG_ISR                 0x88
#define TIMETEST_TRAMP_BUS_ERR_ISR               0x89
#define TIMETEST_TRAMP_CRYPTO_ENGINE_CTL_ISR     0x8a
#define TIMETEST_TRAMP_ADM_MOD_INT_ISR           0x8b
#define TIMETEST_TRAMP_TS_APPS_CTL_ISR           0x8c
#define TIMETEST_TRAMP_MDP_ISR                   0x8d
#define TIMETEST_TRAMP_MDDI_EXT_ISR              0x8e
#define TIMETEST_TRAMP_MDDI_PRI_ISR              0x8f
#define TIMETEST_TRAMP_CHIP_SBI_ISR              0x90
#define TIMETEST_TRAMP_MPM_WAKEUP_ISR            0x91
#define TIMETEST_TRAMP_GPIO_GROUP1_ISR           0x92
#define TIMETEST_TRAMP_GPIO_GROUP0_ISR           0x93
#define TIMETEST_TRAMP_MDDI_CLIENT_ISR           0x94
#define TIMETEST_TRAMP_COMB_USB_ISR              0x95
#define TIMETEST_TRAMP_ADSP_ISR                  0x96
#define TIMETEST_TRAMP_NAND_ISR                  0x97
#define TIMETEST_TRAMP_UI_ISR                    0x98
#define TIMETEST_TRAMP_I2CC_ISR                  0x99
#define TIMETEST_TRAMP_UART_ISR                  0x9a
#define TIMETEST_TRAMP_SDC2_ISR                  0x9b
#define TIMETEST_TRAMP_SDC1_ISR                  0x9c
#define TIMETEST_TRAMP_SLEEP_TIME_TICK_ISR       0x9d
#define TIMETEST_TRAMP_SLEEP_TIMER_1_ISR         0x9e
#define TIMETEST_TRAMP_SLEEP_TIMER_0_ISR         0x9f
#define TIMETEST_TRAMP_TSIF_ISR                  0xa0
#define TIMETEST_TRAMP_BT_WAKEUP_ISR             0xa1
#define TIMETEST_TRAMP_SBI_ISR                   0xa2
#define TIMETEST_TRAMP_MDM_INT_ISR               0xa3
#define TIMETEST_TRAMP_MDSP_INT_ISR              0xa4
#define TIMETEST_TRAMP_SYS_ONLINE_INT2_ISR       0xa5
#define TIMETEST_TRAMP_SYS_ONLINE_INT1_ISR       0xa6
#define TIMETEST_TRAMP_SYS_ONLINE_INT0_ISR       0xa7
#define TIMETEST_TRAMP_SLEEP_FEE_ISR             0xa8
#define TIMETEST_TRAMP_SLEEP_INT3_ISR            0xa9
#define TIMETEST_TRAMP_SLEEP_INT2_ISR            0xaa
#define TIMETEST_TRAMP_SLEEP_INT1_ISR            0xab
#define TIMETEST_TRAMP_WAKEUP_ISR                0xac
#define TIMETEST_TRAMP_GSM_MICRO_ISR             0xad
#define TIMETEST_TRAMP_EDGE_ENCRYPT_DONE_ISR     0xae
#define TIMETEST_TRAMP_BACKUP_ISR                0xaf
#define TIMETEST_TRAMP_UMTS_STMR2_ISR            0xb0
#define TIMETEST_TRAMP_UMTS_STMR1_ISR            0xb1
#define TIMETEST_TRAMP_CDMA_STMR0_ISR            0xb2
#define TIMETEST_TRAMP_UMTS_STMR_EVENT_ISR       0xb3
#define TIMETEST_TRAMP_OFFLINE_DONE_ISR          0xb4
#define TIMETEST_TRAMP_QUICK_PAGING_ISR          0xb5
#define TIMETEST_TRAMP_DEC_DONE_ISR              0xb6
#define TIMETEST_TRAMP_GPS_BC_MICRO_ISR          0xb7
#define TIMETEST_TRAMP_DEM_TIME_1_ISR            0xb8
#define TIMETEST_TRAMP_GPS_CMP_ISR               0xb9
#define TIMETEST_TRAMP_RTC_ROLLOVER_ISR          0xba
#define TIMETEST_TRAMP_RTC_1XHDR_ISR             0xbb

#define TIMETEST_TRAMP_RTC_COMPARE_1_ISR         0xbc     
#define TIMETEST_TRAMP_RTC_COMPARE_2_ISR         0xbd
#define TIMETEST_TRAMP_SYS_TIME_GP_COMPARE_ISR   0xbe
#define TIMETEST_TRAMP_PN_ROLLOVER_1X_ISR        0xbf
#define TIMETEST_TRAMP_PN_ROLLOVER_HDR_ISR       0xc0
#define TIMETEST_TRAMP_125_MS_ISR                0xc1
#define TIMETEST_TRAMP_PER_SLOT_ISR              0xc2
#define TIMETEST_TRAMP_HDR_FRAME_ISR             0xc3
#define TIMETEST_TRAMP_1X_TX_FR_ISR              0xc4
#define TIMETEST_TRAMP_SVD_DONE_ISR              0xc5
#define TIMETEST_TRAMP_TD_DONE_ISR               0xc6
#define TIMETEST_TRAMP_SDC1_0_ISR                0xc7
#define TIMETEST_TRAMP_SDC1_1_ISR                0xc8
#define TIMETEST_TRAMP_SDC3_0_ISR                0xc9
#define TIMETEST_TRAMP_SDC3_1_ISR                0xca
#define TIMETEST_TRAMP_SDC2_0_ISR                0xcb
#define TIMETEST_TRAMP_SDC2_1_ISR                0xcc
#define TIMETEST_TRAMP_SDC4_0_ISR                0xcd
#define TIMETEST_TRAMP_SDC4_1_ISR                0xce
#define TIMETEST_TRAMP_ADSP_0_ISR                0xcf
#define TIMETEST_TRAMP_ADSP_2_ISR                0xd0
#define TIMETEST_TRAMP_UART1_ISR                 0xd1
#define TIMETEST_TRAMP_UART2_ISR                 0xd2
#define TIMETEST_TRAMP_UART3_ISR                 0xd3
#define TIMETEST_TRAMP_DP_RX_DATA1_ISR           0xd4
#define TIMETEST_TRAMP_DP_RX_DATA2_ISR           0xd5
#define TIMETEST_TRAMP_DP_RX_DATA3_ISR           0xd6
#define TIMETEST_TRAMP_DM_UART_ISR               0xd7
#define TIMETEST_TRAMP_DM_DP_RX_DATA_ISR         0xd8
#define TIMETEST_TRAMP_DM_UART2_ISR              0xd9
#define TIMETEST_TRAMP_DM_DP_RX_DATA2_ISR        0xda
#define TIMETEST_TRAMP_SBI_0_ISR                 0xdb
#define TIMETEST_TRAMP_SBI_1_ISR                 0xdc 
#define TIMETEST_TRAMP_SSBI_0_ISR                0xdd
#define TIMETEST_TRAMP_SSBI_1_ISR                0xde
#define TIMETEST_TRAMP_SSBI_2_ISR                0xdf
#define TIMETEST_TRAMP_SBI_TX_SSBI_ISR           0xe0
#define TIMETEST_TRAMP_SBI_TS_ISR                0xe1
#define TIMETEST_TRAMP_SBI_PMIC_ISR              0xe2
#define TIMETEST_TRAMP_SBI_TX_ISR                0xe3
#define TIMETEST_TRAMP_KEYSENSE_ISR              0xe4
#define TIMETEST_TRAMP_HSSD_ISR                  0xe5
#define TIMETEST_TRAMP_PEN_DOWN_ISR              0xe6
#define TIMETEST_TRAMP_NAND_WR_ERR_DONE_ISR      0xe7
#define TIMETEST_TRAMP_NAND_OP_DONE_ISR          0xe8
#define TIMETEST_TRAMP_TS_APPS_1_ISR             0xe9
#define TIMETEST_TRAMP_TS_APPS_2_ISR             0xea
#define TIMETEST_TRAMP_IMEM_ERR_ISR              0xeb
#define TIMETEST_TRAMP_AXI_ERR_ISR               0xec
#define TIMETEST_TRAMP_PBUS_ERR_ISR              0xed
#define TIMETEST_TRAMP_MSS_AHB_ERR_ISR           0xee
#define TIMETEST_TRAMP_SMI_ERR_ISR               0xee
#define TIMETEST_TRAMP_EBI1_ERR_ISR              0xf0
#define TIMETEST_TRAMP_USB_OTG_ISR               0xf1
#define TIMETEST_TRAMP_USB_HS_ISR                0xf2
#define TIMETEST_TRAMP_UMTS_STMR3_ISR            0xf3
#define TIMETEST_TRAMP_DEM_TIME_2_ISR            0xf4
#define TIMETEST_TRAMP_F9_ISR                    0xf5
#define TIMETEST_TRAMP_EVEN_SEC_ISR              0xf6
#define TIMETEST_TRAMP_UMTS_TX_10MS_ISR          0xf7
#define TIMETEST_TRAMP_MOD_TX_FR_ISR             0xf8
#define TIMETEST_TRAMP_MOD_TX_SLOT_ISR           0xf9
#define TIMETEST_TRAMP_MDSP_INT1_ISR             0xfa

#define TIMETEST_MAX_ISR_ID                      0xfb


#endif /* TIMETEST_MSM_MODEM_H */

