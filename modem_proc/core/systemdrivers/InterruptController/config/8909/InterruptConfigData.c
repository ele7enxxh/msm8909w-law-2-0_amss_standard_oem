/*==============================================================================

  D A L I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
 This file Contains the Interrupt configuration data for all the interrupts for 
 this platform.

REFERENCES

        Copyright © 2013 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/InterruptController/config/8909/InterruptConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DALInterruptControllerConfig.h"



/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * Interrupt_Configs
 * BSP data for this target's interrupts.
 * The data layout is the following :
 * {  l2VIC interrupt vector number, Interrupt Service thread stack size, interrupt name}
 *
 */
static InterruptConfigType InterruptConfigs[] = 
{
  {1, 4, DAL_IST_STACK_SIZE, "qdsp6_isdb_irq"},
  {2, 4, DAL_IST_STACK_SIZE, "qtmr_phy_irq[0]"},
  {3,  6, DAL_IST_STACK_SIZE, "qtmr_phy_irq[1]"},
  {10, 4, DAL_IST_STACK_SIZE, "stmr_evt0"}, 
  {11, 4, DAL_IST_STACK_SIZE, "stmr_evt1"}, 
  {12, 4, DAL_IST_STACK_SIZE, "stmr_evt2"}, 
  {13, 4, DAL_IST_STACK_SIZE, "rxfe_wbpwr0_irq"},
  {14, 6, DAL_IST_STACK_SIZE, "rxfe_wbpwr1_irq"},
  {15, 4, DAL_IST_STACK_SIZE, "rxfe_wbpwr2_irq"},
  {16, 4, DAL_IST_STACK_SIZE, "rxfe_wbpwr3_irq"},
  {17, 4, DAL_IST_STACK_SIZE, "umts_stmr_snp0_irq"},
  {18, 6, DAL_IST_STACK_SIZE, "umts_stmr_snp1_irq"},
  {19, 4, DAL_IST_STACK_SIZE, "umts_stmr_tx_chipx256_irq"},
  {21, 6, DAL_IST_STACK_SIZE, "o_stmr0_irq"}, 
  {22, 4, DAL_IST_STACK_SIZE, "sys_online2_irq"},
  {23, 4, DAL_IST_STACK_SIZE, "tds_stmr_wall_irq"},
  {24, 4, DAL_IST_STACK_SIZE, "stmr_evt3"}, 
  {25, 4, DAL_IST_STACK_SIZE, "stmr_evt4"}, 
  {26, 6, DAL_IST_STACK_SIZE, "stmr_evt5"}, 
  {27, 4, DAL_IST_STACK_SIZE, "ccs_swi0_irq"},
  {28, 5, DAL_IST_STACK_SIZE, "ccs_swi1_irq"},
  {29, 2, DAL_IST_STACK_SIZE, "ccs_swi2_irq"},
  {30, 0, DAL_IST_STACK_SIZE, "q6ss_idle_timer_irq"},
  {31, 2, DAL_IST_STACK_SIZE, "q6ss_cti_irq"},
  {32, 5, DAL_IST_STACK_SIZE, "rpm_ipc[12]"},
  {33, 6, DAL_IST_STACK_SIZE, "rpm_ipc[13]"},
  {34, 2, DAL_IST_STACK_SIZE, "rpm_ipc[14]"},
  {35, 5, DAL_IST_STACK_SIZE, "rpm_ipc[15]"},
  {36, 6, DAL_IST_STACK_SIZE, "APCS_mssIPCInterrupt[0]"},
  {37, 6, DAL_IST_STACK_SIZE, "APCS_mssIPCInterrupt[1]"},
  {38, 6, DAL_IST_STACK_SIZE, "APCS_mssIPCInterrupt[2]"},
  {39, 6, DAL_IST_STACK_SIZE, "APCS_mssIPCInterrupt[3]"},
  {40, 6, DAL_IST_STACK_SIZE, "o_wcss_lpass_mbox_intr"},
  {41, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_sensors[0]"},
  {42, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_sensors[1]"},
  {43, 4, DAL_IST_STACK_SIZE, "summary_irq_sensors"},
  {44, 4, DAL_IST_STACK_SIZE, "o_wcss_lpass_bt_tx_intr"},
  {45, 4, DAL_IST_STACK_SIZE, "avtimer_int0_irq"},
  {46, 4, DAL_IST_STACK_SIZE, "audio_out1_irq"},
  {47, 4, DAL_IST_STACK_SIZE, "o_wcss_lpass_bt_rx_intr"},
  {48, 4, DAL_IST_STACK_SIZE, "o_wcss_mss_sw_intr_0"},
  {49, 4, DAL_IST_STACK_SIZE, "o_wcss_mss_sw_intr_1"},
  {50, 4, DAL_IST_STACK_SIZE, "o_wcss_mss_sw_intr_2"},
  {51, 4, DAL_IST_STACK_SIZE, "o_wcss_mss_sw_intr_3"},
  {52, 4, DAL_IST_STACK_SIZE, "qpic_nandc_irq"},
  {53, 4, DAL_IST_STACK_SIZE, "qpic_nandc_op_done_irq"},
  {54, 4, DAL_IST_STACK_SIZE, "qpic_nandc_wr_er_done_irq"},
  {55, 4, DAL_IST_STACK_SIZE, "qpic_lcdc_irq"},
  {56, 4, DAL_IST_STACK_SIZE, "o_wcss_lpass_fm_intr"},
  {59, 4, DAL_IST_STACK_SIZE, "vfr_irq_mux_out[0]"},
  {60, 4, DAL_IST_STACK_SIZE, "vfr_irq_mux_out[1]"},
  {61, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_lpa_dsp[0]"},
  {62, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_lpa_dsp[1]"},
  {63, 6, DAL_IST_STACK_SIZE, "dir_conn_irq_lpa_dsp[2]"},
  {66, 4, DAL_IST_STACK_SIZE, "q6ss_avs_sw_done_irq"},
  {68, 4, DAL_IST_STACK_SIZE, "q6_wdog_irq"},
  {69, 4, DAL_IST_STACK_SIZE, "o_tcsr_xpu2_msa_summary_intr"},
  {70, 4, DAL_IST_STACK_SIZE, "xpu2_msa_intr"},
  {71, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_mss[0]"},
  {72, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_mss[1]"},
  {73, 4, DAL_IST_STACK_SIZE, "crypto_core_irq[0]"},
  {74, 6, DAL_IST_STACK_SIZE, "bam_irq[0]"},
  {75, 4, DAL_IST_STACK_SIZE, "ee2_mss_spmi_periph_irq"},
  {76, 4, DAL_IST_STACK_SIZE, "channel2_mss_trans_done_irq"},
  {77, 4, DAL_IST_STACK_SIZE, "qpic_bam_irq[1]"},
  {81, 4, DAL_IST_STACK_SIZE, "spdm_realtime_irq[3]"},
  {82, 4, DAL_IST_STACK_SIZE, "spdm_offline_irq"},
  {83, 4, DAL_IST_STACK_SIZE, "mss_ahb_access_err_irq"},
  {84, 4, DAL_IST_STACK_SIZE, "nav_irq"},
  {85, 4, DAL_IST_STACK_SIZE, "nav_dm_irq"},
  {86, 4, DAL_IST_STACK_SIZE, "tsync_irq"},
  {87, 4, DAL_IST_STACK_SIZE, "uart_dm_intr"},
  {88, 4, DAL_IST_STACK_SIZE, "uart_dm_intr"},
  {89, 4, DAL_IST_STACK_SIZE, "uart_dm_uim_intr"},
  {90, 4, DAL_IST_STACK_SIZE, "uart_dm_uim_intr"},
  {93, 4, DAL_IST_STACK_SIZE, "cpr_mux_bypass_en[6]"},
  {96, 4, DAL_IST_STACK_SIZE, "sys_online0_irq"},
  {97, 4, DAL_IST_STACK_SIZE, "sys_online1_irq"},
  {98, 4, DAL_IST_STACK_SIZE, "umts_stmr_pri0_irq"},
  {99, 4, DAL_IST_STACK_SIZE, "umts_stmr_pri1_irq"},
  {100, 4, DAL_IST_STACK_SIZE, "umts_stmr_pri2_irq"},
  {101, 4, DAL_IST_STACK_SIZE, "umts_stmr_st_event_irq"},
  {102, 4, DAL_IST_STACK_SIZE, "umts_stmr_tx_10ms_irq"},
  {116, 4, DAL_IST_STACK_SIZE, "o_stmr1_irq"},
  {117, 4, DAL_IST_STACK_SIZE_4k, "tds_stmr_rx_irq"},
  {118, 4, DAL_IST_STACK_SIZE, "tds_stmr_st_event_irq"},
  {120, 4, DAL_IST_STACK_SIZE, "mcdma_ch0_done"},
  {121, 4, DAL_IST_STACK_SIZE, "mcdma_ch1_done"},
  {122, 4, DAL_IST_STACK_SIZE, "mcdma_ch2_done"},
  {123, 4, DAL_IST_STACK_SIZE, "mcdma_ch3_done"},
  {124, 4, DAL_IST_STACK_SIZE, "mcdma_vbuf_done"},
  {125, 4, DAL_IST_STACK_SIZE, "error_irq"},
  {138, 5, DAL_IST_STACK_SIZE, "a2_ulper_irq"},
  {139, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig0_irq"},
  {140, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig1_irq"},
  {141, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig2_irq"},
  {142, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig3_irq"},
  {143, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig4_irq"},
  {144, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig5_irq"},
  {145, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig6_irq"},
  {146, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr0_irq"},
  {147, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr1_irq"},
  {148, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr2_irq"},
  {149, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr3_irq"},
  {150, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr4_irq"},
  {151, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr5_irq"},
  {152, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr6_irq"},
  {153, 5, DAL_IST_STACK_SIZE, "a2_frag0_irq"},
  {154, 5, DAL_IST_STACK_SIZE, "a2_frag1_irq"},
  {155, 5, DAL_IST_STACK_SIZE, "tx_error_irq"}, 
  {156, 5, DAL_IST_STACK_SIZE, "tx_axi_wr_done_irq"}, 
  {157, 5, DAL_IST_STACK_SIZE, "tx_debug_irq"}, 
  {158, 5, DAL_IST_STACK_SIZE, "tx_ahb_err_irq"}, 
  {159, 5, DAL_IST_STACK_SIZE, "tx_axi_reader_0_done_irq"}, 
  {160, 5, DAL_IST_STACK_SIZE, "tx_axi_reader_1_done_irq"}, 
  {161, 5, DAL_IST_STACK_SIZE_4k, "rxfe_wbpwr4_irq"}, 
  {162, 5, DAL_IST_STACK_SIZE, "rxfe_spare0_irq"}, 
  {163, 5, DAL_IST_STACK_SIZE, "rxfe_spare1_irq"}, 
  {164, 5, DAL_IST_STACK_SIZE, "rxfe_spare2_irq"}, 
  {165, 5, DAL_IST_STACK_SIZE, "rxfe_spare_irq"}, 
  {166, 5, DAL_IST_STACK_SIZE, "encrypt_a5_done_irq"}, 
  {167, 5, DAL_IST_STACK_SIZE, "encrypt_gea_done_irq"}, 
  {174, 2, DAL_IST_STACK_SIZE, "a2_tmr0_irq"},
  {175, 2, DAL_IST_STACK_SIZE, "a2_tmr1_irq"},
  {176, 2, DAL_IST_STACK_SIZE, "a2_tmr2_irq"},
  {177, 2, DAL_IST_STACK_SIZE, "a2_tmr3_irq"},
  {178, 2, DAL_IST_STACK_SIZE, "a2_decob_irq"},
  {179, 2, DAL_IST_STACK_SIZE, "demback_sch_deint_done_irq"},
  {180, 2, DAL_IST_STACK_SIZE, "demback_hsscch_p2_irq"},
  {181, 2, DAL_IST_STACK_SIZE, "demback_hsscch_p1_irq"},
  {182, 2, DAL_IST_STACK_SIZE, "demback_eagch_dn_irq"},
  {183, 2, DAL_IST_STACK_SIZE, "demback_eagch_cl_dn_irq"},
  {184, 2, DAL_IST_STACK_SIZE, "demback_decib_wr_done_irq"},
  {185, 2, DAL_IST_STACK_SIZE, "demback_mdsp_td_done_irq"},
  {186, 2, DAL_IST_STACK_SIZE, "demback_mdsp_fch_done_irq"},
  {187, 2, DAL_IST_STACK_SIZE, "demback_dec_done_irq"},
  {188, 2, DAL_IST_STACK_SIZE, "tdec_c_cp_ping_irq"},
  {189, 2, DAL_IST_STACK_SIZE, "tdec_c_dn_ping_irq"},
  {190, 2, DAL_IST_STACK_SIZE, "tdec_evt_irq"},
  {192, 2, DAL_IST_STACK_SIZE, "stmr_evt6"}, 
  {193, 2, DAL_IST_STACK_SIZE, "stmr_evt7"}, 
  {194, 2, DAL_IST_STACK_SIZE, "stmr_evt8"}, 
  {195, 2, DAL_IST_STACK_SIZE, "stmr_evt9"}, 
  {196, 2, DAL_IST_STACK_SIZE, "stmr_evt10"}, 
  {197, 1, DAL_IST_STACK_SIZE, "stmr_evt11"}, 
  {198, 4, DAL_IST_STACK_SIZE, "a2_dma0_irq"},
  {199, 1, DAL_IST_STACK_SIZE, "a2_dma1_irq"},
  {200, 1, DAL_IST_STACK_SIZE, "a2_dma2_irq"},
  {201, 1, DAL_IST_STACK_SIZE, "a2_dma3_irq"},
  {202, 1, DAL_IST_STACK_SIZE, "a2_dma4_irq"},
  {203, 1, DAL_IST_STACK_SIZE, "stmr_evt12"}, 
  {204, 1, DAL_IST_STACK_SIZE, "stmr_evt13"}, 
  {205, 1, DAL_IST_STACK_SIZE, "stmr_evt14"}, 
  {206, 4, DAL_IST_STACK_SIZE, "stmr_evt15"}, 
  {208, 4, DAL_IST_STACK_SIZE, "ccs_swi3_irq"},
  {209, 4, DAL_IST_STACK_SIZE, "ccs_dma0_irq"},
  {210, 4, DAL_IST_STACK_SIZE, "ccs_dma1_irq"},
  {211, 4, DAL_IST_STACK_SIZE, "ccs_dma2_irq"},
  {212, 4, DAL_IST_STACK_SIZE, "ccs_dma3_irq"},
  {213, 4, DAL_IST_STACK_SIZE, "ccs_dma4_irq"},
  {214, 4, DAL_IST_STACK_SIZE, "ccs_dma5_irq"},
  {215, 2, DAL_IST_STACK_SIZE, "ccs_dma6_irq"},
  {216, 2, DAL_IST_STACK_SIZE, "ccs_dma7_irq"},
  {217, 2, DAL_IST_STACK_SIZE, "stmr_evt16"}, 
  {218, 2, DAL_IST_STACK_SIZE, "stmr_evt17"}, 
  {219, 2, DAL_IST_STACK_SIZE, "stmr_evt18"}, 
  {220, 2, DAL_IST_STACK_SIZE, "stmr_evt19"}, 
  {221, 4, DAL_IST_STACK_SIZE, "stmr_evt20"}, 
  {222, 4, DAL_IST_STACK_SIZE, "stmr_evt21"}, 
  {223, 4, DAL_IST_STACK_SIZE, "stmr_evt22"}, 
  {224, 4, DAL_IST_STACK_SIZE, "stmr_evt23"},
  {225, 4, DAL_IST_STACK_SIZE, "stmr_evt24"},
  {226, 4, DAL_IST_STACK_SIZE, "stmr_evt25"}, 
  {227, 4, DAL_IST_STACK_SIZE, "stmr_evt26"}, 
  {228, 4, DAL_IST_STACK_SIZE, "stmr_evt27"}, 
  {229, 4, DAL_IST_STACK_SIZE, "stmr_evt28"}, 
  {230, 4, DAL_IST_STACK_SIZE, "stmr_evt29"}, 
  {231, 4, DAL_IST_STACK_SIZE, "stmr_evt30"}, 
  {232, 4, DAL_IST_STACK_SIZE, "stmr_evt31"},  
  {238, 8, DAL_IST_STACK_SIZE, "o_timeout_slave_mss_summary_intr"},
  {239, 4, DAL_IST_STACK_SIZE, "summary_irq_mss"},
  {242, 8, DAL_IST_STACK_SIZE, "qtmr_phy_irq[2]"},
  {243, 4, DAL_IST_STACK_SIZE, "phss_uart_mss_int[0]"},
  {244, 2, DAL_IST_STACK_SIZE, "bam_irq[2]"},
  {245, 4, DAL_IST_STACK_SIZE, "phss_qup_mss_int[0]"},
  {249, 4, DAL_IST_STACK_SIZE, "qdss_etrbytecnt_irq"},
  {253, 4, DAL_IST_STACK_SIZE, "uart_dm_intr"},
  {254, 4, DAL_IST_STACK_SIZE, "uart_dm_uim_intr"},
  {255, 4, DAL_IST_STACK_SIZE, "smmu_bus_intr[25]"},
  {259, 4, DAL_IST_STACK_SIZE, "bus_irq"},
  {262, 4, DAL_IST_STACK_SIZE, "smmu_bus_intr[26]"},
  {INVALID_INTERRUPT,0, DAL_IST_STACK_SIZE, ""}
};

/*
 * InterruptPlatformDataType 
 * This is used by the Interrupt controller platform to query platform specific data. 
 */
InterruptPlatformDataType  pInterruptControllerConfigData[] =
{
  {
    InterruptConfigs,           /* pIRQConfigs */
    264,
  }
};
