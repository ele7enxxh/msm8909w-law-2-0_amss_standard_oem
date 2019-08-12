
#ifndef RFLM_NLIC_TYPEDEF_AG_H
#define RFLM_NLIC_TYPEDEF_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Tue Aug 12 11:37:46 2014
Generated using: nlic_autogen.pl
Generated from:  v0.0.21 of Bolt_NLICLM_settings.xlsx
*/

/*=============================================================================

              A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_nlic.

Copyright (c) 2009, 2010, 2011, 2012, 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2016/12/13 07:59:25 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/nlic/inc/rflm_nlic_typedef_ag.h#1 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version   Author   Date   
         Comments   
0.0.21   shwang   8/12/2014   
         B4+B2, B3+B3, and B12+B4 (20MHz) addition reverted.   
0.0.20   shwang   8/11/2014   
         clock mode forced to NOM always for NLIC   
0.0.19   shwang   7/30/2014   
         B4+B2, B3+B3, and B12+B4 (20MHz) added   
0.0.18   shwang   7/25/2014   
         clock_mode type changed to an enum. Also names are shortened. added nb clock mode    
0.0.17   shwang   7/10/2014   
         clock mode added. 20MHz BW for B3B5 added. B12B4 is added   
0.0.16   shwang   6/30/2014   
         BW combinations 5+5 and 5+10 added   
0.0.15   shwang   6/25/2014   
         delay data type: uint8 -> int16   
0.0.14   shwang   6/24/2014   
         5+10 and 5+5 are not supported for MAV 6/30 CS   
0.0.13   shwang   6/23/2014   
         delay data type uint16 -> uint8   
0.0.12   shwang   6/19/2014   
         removing 5+10, 5+5 from B3B5   
0.0.11   shwang   6/19/2014   
         adding clock_mode to API. adding 5+10, 5+5 to B17B4 and B3B5   
0.0.10   shwang   6/7/2014   
         adding time delay to API. adding 10+5 to B17B4 and B3B5   
0.0.9   shwang   6/6/2014   
         removed all band combinations other than B17B4 and B3B5: Unsupported use cases.   
0.0.8   shwang   4/11/2014   
         Diversity antenna also enabled for B17-B4, B12-B4, B8-B3   
0.0.7   shwang   4/1/2014   
         empty rows are filled for b3_b19 and later. nlic_mode added for wb+nb mode change   
0.0.6   shwang   3/31/2014   
         freature added: 1-reducing num_kernels. 2-selecting one antenna   
0.0.5   shwang   3/20/2014   
         FTL corrections. New equations to support PCC cancellation.   
0.0.4_B3B5   shwang   2/11/2014   
         B3-B3 FTL correction, NLIC enable always on for B3-B3, victim_carrier_id = 1, aggr_carrier_id = 0, f_offset_nom = 0   
0.0.4   shwang   12/9/2013   
         algorithm bug fix: kernel_term_extra_rxy_dly_acq = 2 for B8_B3 (was 1)   
0.0.3   shwang   11/25/2013   
         fix the algorithm: kernel_terms_cancel_mode_l0 = (1-k7_en)*kl0_on_off   
0.0.2   shwang   10/1/2013   
         added k7_en as static param   
0.0.1   shwang   9/27/2013   
         Initial Revision   

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "comdef.h" 



typedef enum
{
	USE_EQ,
	SET_TRUE,
	SET_FALSE
}rflm_compute_kl0_type;

typedef enum
{
	CONST_VAL,
	USE_EQ_A_B
}rflm_compute_ab_type;

typedef enum
{
	EQ1,
	EQ2,
	EQ3_EQ4
}rflm_compute_ftl_type ;

typedef enum
{
	SET_SCC,
	USE_EQ_PCC_SCC
}rflm_compute_victim_type  ;

typedef enum
{
	NLIC_CLOCK_MODE_MCVS_SVS2,
	NLIC_CLOCK_MODE_MCVS_SVS,
	NLIC_CLOCK_MODE_MCVS_NOM,
	NLIC_CLOCK_MODE_MCVS_TURBO,
	NLIC_CLOCK_MODE_MAX_MODE,
	NLIC_CLOCK_MODE_INVALID
}rflm_compute_clock_mode  ;

typedef struct 
{
	uint8 kernel_term_sel_extra_rxy_dly_acq;
	uint8 kernel_terms_cancel_mode_l0_reduce;
	uint8 kernel_terms_cancel_mode_nl0;
	uint8 kernel_terms_cancel_mode_nl1_normal;
	uint8 kernel_terms_cancel_mode_nl1_reduce;
	boolean conj_linear;
	boolean rsb_enable;
	rflm_compute_kl0_type compute_mode_kl0;
	uint16 nlic_on_off_sinr_coeff_a;
	uint16 trk_on_off_th_1;
	uint16 trk_on_off_th_2;
	uint16 trk_on_off_coeff_a;
	uint8 trk_on_off_rb_1;
	uint8 trk_on_off_rb_2;
	boolean k7_en;
	uint8 aggr_carrier_idx;
	uint8 rx_enable_normal;
	uint8 rx_enable_reduce;
	rflm_compute_ab_type mode_select_a_b;
	rflm_compute_ftl_type compute_mode_f_offset_ftl;
	rflm_compute_victim_type mode_compute_victim_carrier_idx;
	int8 a_pcc0;
	int8 a_pcc1;
	int8 a_pcc2;
	int8 b_pcc0;
	int8 b_pcc1;
	int8 b_pcc2;
	int8 a_scc0;
	int8 a_scc1;
	int8 a_scc2;
	int8 b_scc0;
	int8 b_scc1;
	int8 b_scc2;
	int16 m;
	int16 q;
	rflm_compute_clock_mode clock_mode_wb;
	rflm_compute_clock_mode clock_mode_nb;
	rflm_compute_clock_mode clock_mode_wbnb;
	uint8 nlic_mode;
	boolean nlic_enable_slot0;
	boolean nlic_enable_slot1;
	boolean nlic_enable_srs;
	uint8 rx_enable;
	boolean delay_tracking_enable;
	uint8 kernel_terms_cancel_mode_l0;
	uint8 kernel_terms_cancel_mode_nl1;
	uint8 kernel_terms_cancel_mode_nl2;
	int32 freq_offset;
	int16 tx_intf_power_db;
	uint8 ul_num_rb;
	uint32 ul_p_freq;
	uint32 dl_p_freq;
	uint32 dl_s_freq;
	boolean nlic_on_off_chan;
	boolean nlic_on_off_snir;
	boolean kl0_on_off;
	boolean trk_I1;
	boolean trk_I2;
	boolean trk_I3;
	int8 a0;
	int8 a1;
	int8 a2;
	int8 b0;
	int8 b1;
	int8 b2;
	int32 f_offset_nom;
	uint32 f_edge;
	uint32 f_edge_kl0;
	int32 f_offset_ftl;
	uint8 victim_carrier_idx;
	uint32 DL_SCC_earfcn;
	uint8 dl_bw;
	uint32 UL_PCC_earfcn;
	uint8 ul_bw;
	uint32 DL_PCC_earfcn;
	int16 time_delay;
	uint8 clock_mode;
}rflm_nlic_data_type;


typedef enum 
{
	NLIC_STATE_B17_B4_UL5_DL5,
	NLIC_STATE_B17_B4_UL5_DL10,
	NLIC_STATE_B17_B4_UL10_DL5,
	NLIC_STATE_B17_B4_UL10_DL10,
	NLIC_STATE_B12_B4_UL5_DL5,
	NLIC_STATE_B12_B4_UL5_DL10,
	NLIC_STATE_B12_B4_UL10_DL5,
	NLIC_STATE_B12_B4_UL10_DL10,
	NLIC_STATE_B3_B5_UL10_DL5,
	NLIC_STATE_B3_B5_UL10_DL10,
	NLIC_STATE_B3_B5_UL20_DL5,
	NLIC_STATE_B3_B5_UL20_DL10,
	NLIC_STATE_B3_B5_UL20_DL20,
	NLIC_STATE_NUM,
	NLIC_STATE_INVALID
}nlic_state_enum_type;

#ifdef __cplusplus
}
#endif



#endif


