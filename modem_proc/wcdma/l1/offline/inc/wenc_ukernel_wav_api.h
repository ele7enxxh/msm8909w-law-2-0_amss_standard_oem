/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

			MCALWCDMA_WENC_RIF.H : Auto-generated File
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/09/13    ar      File Created : SW Changes due to new architecture for Bolt Tx 
==========================================================================*/
typedef union {
	struct {
	#define WCDMA_TX_STATIC_CFG_WORD_COUNT	2
		#define WCDMA_TX_STATIC_CFG_CARR_CFG_CAR0_ONLY_EN	0
		#define WCDMA_TX_STATIC_CFG_CARR_CFG_CAR0_CAR1_EN	1
		uint32 carr_cfg : 1;
		uint32 reserved0 : 31;
		uint32 flush : 1;
		uint32 reserved1 : 31;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_static_cfg_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_RESAMPLE_WORD_COUNT	5
		uint32 delta_val : 31;
		uint32 reserved0 : 1;
		uint32 pda_val_init : 1;
		uint32 reserved1 : 31;
		uint32 pda_val : 31;
		uint32 reserved2 : 1;
		uint32 threshold : 11;
		uint32 reserved3 : 21;
		#define WCDMA_TX_RESAMPLE_CLIP_EN_DISABLE	0
		#define WCDMA_TX_RESAMPLE_CLIP_EN_ENABLE	2
		uint32 clip_en : 2;
		uint32 notch_en : 1;
		uint32 bbfir_resamp_en : 2;
		uint32 bbfir_mode : 3;
		uint32 bbfir_cdma_qshift : 1;
		uint32 resamp_dnsample : 1;
		uint32 resamp_override : 1;
		uint32 reserved4 : 21;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_wcdma_tx_resample_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_SYNCP_WORD_COUNT	4
		#define WCDMA_TX_SYNCP_OPERATING_STATE_SLEEP_STATE	0
		#define WCDMA_TX_SYNCP_OPERATING_STATE_INIT_STATE	1
		#define WCDMA_TX_SYNCP_OPERATING_STATE_CONNECTED_STATE	256
		#define WCDMA_TX_SYNCP_OPERATING_STATE_SOFT_RESET	32768
		uint32 operating_state : 16;
		#define WCDMA_TX_SYNCP_PING_PONG_STATE_PING_STATE	0
		#define WCDMA_TX_SYNCP_PING_PONG_STATE_PONG_STATE	1
		uint32 ping_pong_state : 16;
		#define WCDMA_TX_SYNCP_CMD_VALID_SYNC_INVALID	0
		#define WCDMA_TX_SYNCP_CMD_VALID_SYNC_VALID	1
		uint32 cmd_valid : 16;
		#define WCDMA_TX_SYNCP_AUTO_SLEEP_NORMAL_MODE	0
		#define WCDMA_TX_SYNCP_AUTO_SLEEP_AUTO_SLEEP_MODE	1
		uint32 auto_sleep : 16;
		uint32 stmr : 24;
		uint32 reserved0 : 8;
		uint32 reserved1;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_wcdma_tx_syncp_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_DATA_STACK_VP_WORD_COUNT	16
		uint32 stack_vp [16];
	}; 
	uint32 Words[16];
} mcalwcdma_wenc_wcdma_tx_dmem_data_stack_vp_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_VBUF_DESC_WORD_COUNT	3
		uint32 vbuf_base_address : 13;
		uint32 reserved0 : 6;
		uint32 vbuf_format : 6;
		uint32 fc_bias_gain_src : 5;
		uint32 reserved1 : 2;
		uint32 length : 12;
		uint32 start_vaddr : 18;
		uint32 reserved2 : 2;
		uint32 vbuf_size : 18;
		uint32 vr_format : 3;
		uint32 vr_bank : 3;
		uint32 reserved3 : 8;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_wcdma_tx_vbuf_desc_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_TXFE_CFG_WORD_COUNT	5
		uint32 update_en : 1;
		uint32 reserved0 : 31;
		uint32 block_idx : 6;
		uint32 reserved1 : 26;
		#define WCDMA_TX_TXFE_CFG_SAMPLE_IDX_PARAM_UPDATE_DISABLE	0xFFFF
		uint32 sample_idx_param_update : 16;
		uint32 reserved2 : 16;
		#define WCDMA_TX_TXFE_CFG_SAMPLE_IDX_LOG_START_DISABLE	0xFFFF
		uint32 sample_idx_log_start : 16;
		uint32 reserved3 : 16;
		#define WCDMA_TX_TXFE_CFG_SAMPLE_IDX_LOG_STOP_DISABLE	0xFFFF
		uint32 sample_idx_log_stop : 16;
		uint32 reserved4 : 16;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_wcdma_tx_txfe_cfg_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_IRQ_STATUS_WORD_COUNT	1
		uint32 err_mmi_pend_status_vp3 : 8;
		uint32 reserved : 24;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_wcdma_tx_irq_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_CHAN_INFO_WORD_COUNT	8
		uint32 gain : 15;
		uint32 reserved0 : 17;
		uint32 alt_gain : 15;
		uint32 reserved1 : 17;
		#define WCDMA_TX_CHAN_INFO_MODULATION_UMTS_4PAM_ON_I	29
		#define WCDMA_TX_CHAN_INFO_MODULATION_UMTS_4PAM_ON_Q	30
		#define WCDMA_TX_CHAN_INFO_MODULATION_MULTI_BPSK_ON_I	31
		#define WCDMA_TX_CHAN_INFO_MODULATION_MULTI_BPSK_ON_Q	32
		uint32 modulation : 6;
		uint32 reserved2 : 26;
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_DPDCH_C0_BASE_PING	0
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_DPDCH_C0_BASE_PONG	10240
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_PRACH_PRE_C0_BASE_PING	20480
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_PRACH_PRE_C0_BASE_PONG	20608
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_HSDPCCH_C0_BASE_PING	20736
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_HSDPCCH_C0_BASE_PONG	20768
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_DPCCH_C0_BASE_PING	20800
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_DPCCH_C0_BASE_PONG	20832
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPCCH_C0_BASE_PING	20864
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPCCH_C0_BASE_PONG	20896
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C0_BASE_0_BPSK	20928
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C0_BASE_1_BPSK	78528
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C0_BASE_0_4PAM	10464
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C0_BASE_1_4PAM	39264
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_DPCCH_C1_BASE_PING	136128
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_DPCCH_C1_BASE_PONG	136160
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPCCH_C1_BASE_PING	136192
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPCCH_C1_BASE_PONG	136224
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C1_BASE_0_BPSK	136256
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C1_BASE_1_BPSK	193856
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C1_BASE_0_4PAM	68128
		#define WCDMA_TX_CHAN_INFO_VBUF_OFFSET_EDPDCH0_C1_BASE_1_4PAM	96928
		uint32 vbuf_offset : 18;
		uint32 reserved3 : 14;
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF8	1
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF16	2
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF32	3
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF64	4
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF128	5
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF256	6
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF1	8
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF2	9
		#define WCDMA_TX_CHAN_INFO_SPRD_FACTOR_SF4	10
		uint32 sprd_factor : 8;
		uint32 ovsf_num : 8;
		uint32 reserved4 : 16;
		uint32 sprd_len : 8;
		uint32 reserved5 : 24;
		uint32 carr_idx : 1;
		uint32 reserved6 : 31;
		#define WCDMA_TX_CHAN_INFO_ADD_CTRL_CAR0	4194432
		#define WCDMA_TX_CHAN_INFO_ADD_CTRL_CAR1	289407104
		uint32 add_ctrl;
	}; 
	uint32 Words[8];
} mcalwcdma_wenc_wcdma_tx_chan_info_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_CHAN_IDX_WORD_COUNT	1
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_DPCCH_CAR0	0
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPCCH_CAR0	1
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH0_CAR0	2
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH1_CAR0	3
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH2_CAR0	4
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH3_CAR0	5
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_DPDCH_CAR0	4
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_HSDPCCH_CAR0	6
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_PRACH_PRE_CAR0	5
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_DPCCH_CAR1	7
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPCCH_CAR1	8
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH0_CAR1	9
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH1_CAR1	10
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH2_CAR1	11
		#define WCDMA_TX_CHAN_IDX_CHAN_IDX_EDPDCH3_CAR1	12
		uint32 chan_idx : 4;
		uint32 reserved0 : 28;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_wcdma_tx_chan_idx_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_SYNCP_OP_STATE_WORD_COUNT	2
		uint32 hw_state;
		uint32 uk_state;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_syncp_op_state_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_WAV_SCRATCH_WORD_COUNT	16
		uint32 samp [16];
	}; 
	uint32 Words[16];
} mcalwcdma_wenc_wcdma_tx_wav_scratch_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_WAV_VBUF_ADDR_STATUS_WORD_COUNT	14
		uint32 vb_addr_offset [13];
		uint32 block_idx;
	}; 
	uint32 Words[14];
} mcalwcdma_wenc_wcdma_tx_wav_vbuf_addr_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_TXFE_CFG_LIST_WORD_COUNT	20
		mcalwcdma_wenc_wcdma_tx_txfe_cfg_struct_type	txfe_cfg_list [4];
	}; 
	uint32 Words[20];
} mcalwcdma_wenc_wcdma_tx_txfe_cfg_list_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_RUN_TIME_CONSTS_WORD_COUNT	1
		uint32 dummy;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_wcdma_tx_run_time_consts_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_TXFE_IB_WORD_COUNT	2
		uint32 txfe_ib_addr_init : 1;
		uint32 reserved0 : 31;
		uint32 txfe_ib_addr_in : 16;
		uint32 reserved1 : 16;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_txfe_ib_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_BUILD_WORD_COUNT	3
		#define WCDMA_TX_DMEM_BUILD_TARGET_CHIP_NIKEL	0
		#define WCDMA_TX_DMEM_BUILD_TARGET_CHIP_DIME	1
		#define WCDMA_TX_DMEM_BUILD_TARGET_CHIP_TRITON	2
		#define WCDMA_TX_DMEM_BUILD_TARGET_CHIP_BOLT	3
		#define WCDMA_TX_DMEM_BUILD_TARGET_CHIP_BOLT_PLUS	4
		uint32 target_chip : 8;
		uint32 release_patch : 8;
		uint32 release_minor : 8;
		uint32 release_major : 8;
		uint32 day : 8;
		uint32 month : 8;
		uint32 year : 16;
		uint32 minute : 8;
		uint32 hour : 8;
		uint32 reserved : 16;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_wcdma_tx_dmem_build_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_ROT_PARAM_WORD_COUNT	2
		uint32 reserved0 : 8;
		uint32 phase : 24;
		uint32 reserved1 : 8;
		uint32 step : 24;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_rot_param_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_VPE_STATUS_WORD_COUNT	1
		uint32 vpe_status;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_wcdma_tx_vpe_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_CHAN_INFO_LIST_WORD_COUNT	104
		mcalwcdma_wenc_wcdma_tx_chan_info_struct_type	chan_info [13];
	}; 
	uint32 Words[104];
} mcalwcdma_wenc_wcdma_tx_chan_info_list_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_SCR_CODE_WORD_COUNT	2
		uint32 data0;
		uint32 data1;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_scr_code_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_COMPLEX_SPRD_STATUS_WORD_COUNT	2
		mcalwcdma_wenc_wcdma_tx_scr_code_struct_type	scr_code;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_complex_sprd_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_SYNCP_ERROR_STATUS_WORD_COUNT	1
		#define WCDMA_TX_SYNCP_ERROR_STATUS_ERROR_CODE_SYNCP_ERROR_FW_LATE	1
		#define WCDMA_TX_SYNCP_ERROR_STATUS_ERROR_CODE_SYNCP_ERROR_MKERNEL_LATE	2
		uint32 error_code;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_wcdma_tx_syncp_error_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_TXFE_IB_STATUS_WORD_COUNT	1
		uint32 addr : 16;
		uint32 reserved0 : 16;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_wcdma_tx_txfe_ib_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_DATA_STACK_WORD_COUNT	64
		mcalwcdma_wenc_wcdma_tx_dmem_data_stack_vp_struct_type	dmem_data_stack_vp [4];
	}; 
	uint32 Words[64];
} mcalwcdma_wenc_wcdma_tx_dmem_data_stack_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_IAL_SYNCP_WORD_COUNT	8
		uint32 syncp_int_block_count;
		uint32 syncp_done_trigger;
		uint32 syncp_error_trigger;
		uint32 syncp_next_fw_stmr_match_bkp;
		mcalwcdma_wenc_wcdma_tx_syncp_op_state_struct_type	syncp_op_state;
		mcalwcdma_wenc_wcdma_tx_syncp_error_status_struct_type	syncp_err_cd_id;
		uint32 syncp_auto_sleep_flag;
	}; 
	uint32 Words[8];
} mcalwcdma_wenc_wcdma_tx_dmem_ial_syncp_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_RESAMPLE_STATUS_WORD_COUNT	2
		uint32 pda_val;
		uint32 num_samp;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_resample_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_SPRD_WORD_COUNT	20
		uint32 num_chan_car0 : 3;
		uint32 reserved0 : 29;
		uint32 num_chan_car1 : 3;
		uint32 reserved1 : 29;
		mcalwcdma_wenc_wcdma_tx_chan_idx_struct_type	chan_idx_list [13];
		uint32 zero_chan;
		uint32 hsdpcch_offset_c0 : 4;
		uint32 reserved2 : 28;
		uint32 hsdpcch_offset_c1 : 4;
		uint32 reserved3 : 28;
		uint32 dtx_car0 : 10;
		uint32 reserved4 : 22;
		uint32 dtx_car1 : 10;
		uint32 reserved5 : 22;
	}; 
	uint32 Words[20];
} mcalwcdma_wenc_wcdma_tx_sprd_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_WAV_SHARED_WORD_COUNT	168
		mcalwcdma_wenc_wcdma_tx_static_cfg_struct_type	static_cfg;
		mcalwcdma_wenc_wcdma_tx_sprd_struct_type	sprd;
		mcalwcdma_wenc_wcdma_tx_chan_info_list_struct_type	chan_info_list;
		mcalwcdma_wenc_wcdma_tx_resample_struct_type	resample;
		mcalwcdma_wenc_wcdma_tx_txfe_ib_struct_type	txfe_ib;
		mcalwcdma_wenc_wcdma_tx_txfe_ib_status_struct_type	txfe_ib_status;
		mcalwcdma_wenc_wcdma_tx_txfe_cfg_list_struct_type	txfe_cfg_list;
		mcalwcdma_wenc_wcdma_tx_wav_vbuf_addr_status_struct_type	wav_vbuf_addr_status;
	}; 
	uint32 Words[168];
} mcalwcdma_wenc_wcdma_tx_wav_shared_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_VBUF_DESC_LIST_WORD_COUNT	6
		mcalwcdma_wenc_wcdma_tx_vbuf_desc_struct_type	vbuf_desc_wav_ib;
		mcalwcdma_wenc_wcdma_tx_vbuf_desc_struct_type	vbuf_desc_txfe_ib;
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_wcdma_tx_vbuf_desc_list_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_COMPLEX_SPRD_WORD_COUNT	5
		uint32 scr_en : 10;
		uint32 reserved0 : 22;
		#define WCDMA_TX_COMPLEX_SPRD_SCR_CODE_TYPE_LONG	0
		#define WCDMA_TX_COMPLEX_SPRD_SCR_CODE_TYPE_PRACH_PREAMBLE	1
		#define WCDMA_TX_COMPLEX_SPRD_SCR_CODE_TYPE_SHORT	2
		uint32 scr_code_type : 2;
		uint32 reserved1 : 30;
		uint32 scr_code_init : 1;
		uint32 reserved2 : 31;
		mcalwcdma_wenc_wcdma_tx_scr_code_struct_type	scr_code_in;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_wcdma_tx_complex_sprd_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_ROT_WORD_COUNT	4
		#define WCDMA_TX_ROT_ROT_ENABLE_DISABLE	0
		#define WCDMA_TX_ROT_ROT_ENABLE_ENABLE	1
		uint32 rot_enable : 1;
		uint32 reserved0 : 31;
		uint32 rot_init : 1;
		uint32 reserved1 : 31;
		mcalwcdma_wenc_wcdma_tx_rot_param_struct_type	rot_param_in;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_wcdma_tx_rot_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_ROT_STATUS_WORD_COUNT	2
		mcalwcdma_wenc_wcdma_tx_rot_param_struct_type	rot_param;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_wcdma_tx_rot_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_WAV_STATUS_WORD_COUNT	6
		mcalwcdma_wenc_wcdma_tx_complex_sprd_status_struct_type	complex_sprd_status;
		mcalwcdma_wenc_wcdma_tx_resample_status_struct_type	resample_status;
		mcalwcdma_wenc_wcdma_tx_rot_status_struct_type	rot_status;
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_wcdma_tx_wav_status_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_IAL_VP_WORD_COUNT	32
		mcalwcdma_wenc_wcdma_tx_irq_status_struct_type	ial_err_mmi_pend;
		uint32 ial_event_cd;
		uint32 ial_irq2l_mask;
		mcalwcdma_wenc_wcdma_tx_dmem_ial_syncp_struct_type	ial_syncp;
		uint32 reserved0 [8];
		uint32 reserved1 [13];
	}; 
	uint32 Words[32];
} mcalwcdma_wenc_wcdma_tx_dmem_ial_vp_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_LOAD_TIME_CONSTS_WORD_COUNT	8
		uint32 lmem_base_addr;
		mcalwcdma_wenc_wcdma_tx_vbuf_desc_list_struct_type	vbuf_desc_list;
		uint32 enc_ib_base_addr;
	}; 
	uint32 Words[8];
} mcalwcdma_wenc_wcdma_tx_load_time_consts_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_IAL_WORD_COUNT	128
		mcalwcdma_wenc_wcdma_tx_dmem_ial_vp_struct_type	dmem_ial_vp [4];
	}; 
	uint32 Words[128];
} mcalwcdma_wenc_wcdma_tx_dmem_ial_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_WAV_CAR_WORD_COUNT	15
		mcalwcdma_wenc_wcdma_tx_complex_sprd_struct_type	complex_sprd;
		mcalwcdma_wenc_wcdma_tx_rot_struct_type	rot;
		mcalwcdma_wenc_wcdma_tx_wav_status_struct_type	wav_status;
	}; 
	uint32 Words[15];
} mcalwcdma_wenc_wcdma_tx_wav_car_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_WAV_WORD_COUNT	198
		mcalwcdma_wenc_wcdma_tx_wav_shared_struct_type	wav_shared;
		mcalwcdma_wenc_wcdma_tx_wav_car_struct_type	wav_car [2];
	}; 
	uint32 Words[198];
} mcalwcdma_wenc_wcdma_tx_wav_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_CONSTS_WORD_COUNT	9
		mcalwcdma_wenc_wcdma_tx_run_time_consts_struct_type	run_time_consts;
		mcalwcdma_wenc_wcdma_tx_load_time_consts_struct_type	load_time_consts;
	}; 
	uint32 Words[9];
} mcalwcdma_wenc_wcdma_tx_dmem_consts_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_COMMON_MAP_WORD_COUNT	8192
		mcalwcdma_wenc_wcdma_tx_syncp_struct_type	syncp [4];
		uint32 dmem_api_base_addr [4];
		uint32 dmem_prog_base_addr [4];
		uint32 padding_0[7964];
		mcalwcdma_wenc_wcdma_tx_dmem_data_stack_struct_type	data_stack;
		mcalwcdma_wenc_wcdma_tx_dmem_ial_struct_type	ial;
		mcalwcdma_wenc_wcdma_tx_dmem_build_struct_type	build [4];
	}; 
	uint32 Words[8192];
} mcalwcdma_wenc_wcdma_tx_dmem_common_map_struct_type;


typedef union {
	struct {
	#define WCDMA_TX_DMEM_MAP_WORD_COUNT	422
		mcalwcdma_wenc_wcdma_tx_wav_struct_type	wav_ping;
		mcalwcdma_wenc_wcdma_tx_wav_struct_type	wav_pong;
		mcalwcdma_wenc_wcdma_tx_vpe_status_struct_type	vpe_status;
		mcalwcdma_wenc_wcdma_tx_wav_scratch_struct_type	wav_scratch;
		mcalwcdma_wenc_wcdma_tx_dmem_consts_struct_type	dmem_consts;
	}; 
	uint32 Words[422];
} mcalwcdma_wenc_wcdma_tx_dmem_map_struct_type;

