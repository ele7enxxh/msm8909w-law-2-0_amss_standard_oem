/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

			MCALWCDMA_WENC_RIF.H : Auto-generated File
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
02/25/14   ar       Moved manually added structures to mcalwcdma_wenc.c
10/09/13   ar       File Created : SW Changes due to new architecture for Bolt Tx 
==========================================================================*/
typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_ERUCCH_MEM_PTR_WORD_COUNT	1
		uint32 rmram_addr : 14;
		uint32 eram_addr : 14;
		uint32 reserved : 4;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_tds_enc_erucch_mem_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_DO_REV0_MEM_PTR_WORD_COUNT	1
		uint32 iram_addr : 14;
		uint32 eram_addr : 14;
		uint32 reserved : 4;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_do_rev0_mem_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_EUL_MEM_PTR_WORD_COUNT	1
		uint32 eram_addr : 14;
		uint32 rmram_addr : 14;
		uint32 reserved : 4;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_enc_eul_mem_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_R4_TRCH_MUX_ORDER_CCTRCH2_WORD_COUNT	1
		uint32 trch_start_addr : 9;
		uint32 reserved0 : 7;
		uint32 trch_cfg1_index : 3;
		uint32 reserved1 : 13;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_tds_enc_r4_trch_mux_order_cctrch2_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_UMTS_ENC_R99_WORD_COUNT	1
		uint32 phy_ch_type : 2;
		uint32 reserved : 30;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_umts_enc_r99_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_R4R99_MEM_PTR_WORD_COUNT	2
		uint32 erama_addr : 14;
		uint32 eramb_addr : 14;
		uint32 reserved0 : 4;
		uint32 cfgram_addr : 14;
		uint32 rmram_addr : 14;
		uint32 reserved1 : 4;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_enc_r4r99_mem_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_EUL_WORD_COUNT	3
		uint32 numbits_post_rm : 16;
		uint32 reserved0 : 16;
		uint32 tds_eul_bit_coll_mem_ptr : 14;
		uint32 reserved1 : 18;
		uint32 mod_mode : 1;
		uint32 reserved2 : 31;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_tds_enc_eul_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_R4R99_TRCH_MUX_ORDER_CCTRCH1_WORD_COUNT	1
		uint32 trch_start_addr : 10;
		uint32 reserved0 : 6;
		uint32 trch_cfg1_index : 3;
		uint32 reserved1 : 13;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_enc_r4r99_trch_mux_order_cctrch1_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_R4R99_DCH_CIPHER_WORD_COUNT	4
		uint32 key_word0;
		uint32 key_word1;
		uint32 key_word2;
		uint32 key_word3;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_enc_r4r99_dch_cipher_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_MEM_PTR_WORD_COUNT	1
		uint32 iram_addr : 14;
		uint32 eram_addr : 14;
		uint32 reserved : 4;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_cdma_1x_mem_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_WORD_COUNT	1
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_TDS_R4	0
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_TDS_EUL	1
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_TDS_ERUCCH	2
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_UMTS_R99	3
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_UMTS_EUL	4
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_1X_CH1	5
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_1X_CH2	6
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_1X_CH3	7
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_DO_REV0	8
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_DO_REVAB_C0	9
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_DO_REVAB_C1	10
		#define MCALWCDMA_WENC_TX_ENC_COMMON_CFG_MODE_CDMA_DO_REVAB_C2	11
		uint32 mode : 4;
		uint32 reserved : 28;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_enc_common_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_DO_ENC_REV0_CFG_WORD_COUNT	1
		uint32 rev0_rr_index : 3;
		uint32 reserved : 29;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_do_enc_rev0_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_R4_CCTRCH_PARAMS_WORD_COUNT	8
		uint32 second_intlv_type_cctrch1 : 1;
		uint32 second_intlv_type_cctrch2 : 1;
		uint32 reserved0 : 30;
		uint32 cctrch1_rmram_addr : 9;
		uint32 reserved1 : 1;
		uint32 cctrch2_rmram_addr : 9;
		uint32 reserved2 : 13;
		uint32 slot1_data_num_cctrch1 : 11;
		uint32 reserved3 : 5;
		uint32 slot2_data_num_cctrch1 : 11;
		uint32 reserved4 : 5;
		uint32 slot3_data_num_cctrch1 : 11;
		uint32 reserved5 : 5;
		uint32 slot4_data_num_cctrch1 : 11;
		uint32 reserved6 : 5;
		uint32 slot5_data_num_cctrch1 : 11;
		uint32 reserved7 : 21;
		uint32 slot1_data_num_cctrch2 : 11;
		uint32 reserved8 : 5;
		uint32 slot2_data_num_cctrch2 : 11;
		uint32 reserved9 : 5;
		uint32 slot3_data_num_cctrch2 : 11;
		uint32 reserved10 : 5;
		uint32 slot4_data_num_cctrch2 : 11;
		uint32 reserved11 : 5;
		uint32 slot5_data_num_cctrch2 : 11;
		uint32 reserved12 : 21;
	}; 
	uint32 Words[8];
} mcalwcdma_wenc_enc_tx_tds_enc_r4_cctrch_params_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_R4R99_CFG_WORD_COUNT	4
		uint32 num_erama_trchs : 4;
		uint32 num_active_trchs_cctrch1 : 4;
		uint32 num_active_trchs_cctrch2 : 4;
		uint32 reserved0 : 20;
		uint32 enc_endian_format : 1;
		uint32 reserved1 : 31;
		uint32 frame_cnt : 3;
		uint32 reserved2 : 29;
		uint32 rfrm_eq_filler_bit_cctrch1 : 1;
		uint32 rfrm_eq_filler_bit_cctrch2 : 1;
		uint32 reserved3 : 30;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_enc_r4r99_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_DO_REVAB_MEM_PTR_WORD_COUNT	2
		uint32 eram_addr : 14;
		uint32 reserved0 : 18;
		uint32 iram_in_addr : 14;
		uint32 iram_out_addr : 14;
		uint32 reserved1 : 4;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_do_revab_mem_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_DO_ENC_REVAB_CAR_CFG_WORD_COUNT	3
		uint32 scramb_init : 17;
		uint32 reserved0 : 15;
		uint32 reserved1 : 2;
		uint32 revab_rr_index : 4;
		uint32 reserved2 : 26;
		uint32 revab_subpkg_index : 2;
		uint32 reserved3 : 30;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_do_enc_revab_car_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_ENC_CH1_CFG_WORD_COUNT	4
		uint32 reserved0 : 1;
		uint32 ch1_frame_size : 2;
		uint32 ch1_enc_rate : 1;
		uint32 ch1_radio_cfg : 3;
		uint32 ch1_code_rate : 2;
		uint32 ch1_crc_len : 5;
		uint32 reserved1 : 18;
		uint32 ch1_crc_polynomial : 16;
		uint32 reserved2 : 16;
		uint32 dcch_punct_pattern_23_16 : 8;
		uint32 reserved3 : 24;
		uint32 dcch_punct_pattern_15_0 : 16;
		uint32 reserved4 : 16;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch1_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_EUL_CFG_WORD_COUNT	14
		uint32 num_pre_crc_bits : 15;
		uint32 reserved0 : 1;
		uint32 eul_crc_en : 1;
		uint32 reserved1 : 15;
		uint32 num_pre_rm_bits : 17;
		uint32 num_code_segs : 4;
		uint32 reserved2 : 11;
		uint32 num_bits_code_seg : 13;
		uint32 num_filler_bits : 6;
		uint32 reserved3 : 13;
		uint32 last_row : 5;
		uint32 last_row_bit_exch : 1;
		uint32 num_row_index : 2;
		uint32 last_column : 8;
		uint32 num_columns_minus1 : 8;
		uint32 reserved4 : 8;
		uint32 prime_number : 9;
		uint32 prime_num_index : 6;
		uint32 reserved5 : 17;
		uint32 initial_error_s : 16;
		uint32 eul_punct_en_sys : 1;
		uint32 reserved6 : 15;
		uint32 eminus_s : 16;
		uint32 eplus_s : 16;
		uint32 initial_error_p1 : 16;
		uint32 eul_punct_en_parity : 1;
		uint32 reserved7 : 15;
		uint32 eplus_p1 : 17;
		uint32 reserved8 : 15;
		uint32 eminus_p1 : 17;
		uint32 reserved9 : 15;
		uint32 initial_error_p2 : 16;
		uint32 reserved10 : 16;
		uint32 eminus_p2 : 16;
		uint32 eplus_p2 : 16;
		uint32 nt_sys_numbits : 16;
		uint32 reserved11 : 16;
		uint32 nt_p1_numbits : 16;
		uint32 nt_p2_numbits : 16;
	}; 
	uint32 Words[14];
} mcalwcdma_wenc_enc_tx_enc_eul_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_ENC_CH3_CFG_WORD_COUNT	5
		uint32 reserved0 : 1;
		uint32 sch_enc_rate : 5;
		uint32 sch_code_rate : 2;
		uint32 sch_crc_length : 5;
		uint32 turbo_encode : 1;
		uint32 reserved1 : 18;
		uint32 ch3_frame_size : 2;
		uint32 ch3_sel : 3;
		uint32 reserved2 : 4;
		uint32 low_repeat : 3;
		uint32 reserved3 : 20;
		uint32 ch3_crc_polynomial : 16;
		uint32 reserved4 : 16;
		uint32 sch_puncpattern_23_16 : 8;
		uint32 reserved5 : 24;
		uint32 sch_puncpattern_15_0 : 16;
		uint32 reserved6 : 16;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch3_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_ERUCCH_CFG_WORD_COUNT	4
		uint32 initial_error : 16;
		uint32 reserved0 : 16;
		uint32 eminus : 16;
		uint32 reserved1 : 16;
		uint32 eplus : 16;
		uint32 reserved2 : 16;
		uint32 tti : 1;
		uint32 reserved3 : 31;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_tds_enc_erucch_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_UMTS_ENC_EUL_WORD_COUNT	1
		uint32 area1_rm_base_addr : 11;
		uint32 reserved0 : 5;
		uint32 area2_rm_base_addr : 11;
		uint32 reserved1 : 5;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_umts_enc_eul_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_ENC_CH2_CFG_WORD_COUNT	5
		uint32 reserved0 : 2;
		uint32 fch_enc_rate : 2;
		uint32 reserved1 : 1;
		uint32 fch_code_rate : 2;
		uint32 reserved2 : 1;
		uint32 fch_crc_length : 5;
		uint32 reserved3 : 19;
		uint32 ch2_frame_size : 2;
		uint32 ch2_radio_cfg : 3;
		uint32 reserved4 : 27;
		uint32 ch2_crc_poly : 16;
		uint32 reserved5 : 16;
		uint32 fch_puncpattern_24_16 : 9;
		uint32 reserved6 : 23;
		uint32 fch_puncpattern_15_0 : 16;
		uint32 reserved7 : 16;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch2_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_ERUCCH_WORD_COUNT	5
		mcalwcdma_wenc_enc_tx_tds_enc_erucch_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_tds_enc_erucch_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_tds_enc_erucch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_DO_ENC_REV0_FRAME_WORD_COUNT	2
		mcalwcdma_wenc_enc_tx_do_rev0_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_do_enc_rev0_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_cdma_do_enc_rev0_frame_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_EUL_COMMON_WORD_COUNT	15
		mcalwcdma_wenc_enc_tx_enc_eul_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_enc_eul_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[15];
} mcalwcdma_wenc_enc_tx_enc_eul_common_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_ENC_R4R99_COMMON_WORD_COUNT	38
		mcalwcdma_wenc_enc_tx_enc_r4r99_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_enc_r4r99_trch_mux_order_cctrch1_struct_type	trch_mux_order_cctrch1 [8];
		mcalwcdma_wenc_enc_tx_enc_r4r99_cfg_struct_type	enc_cfg;
		mcalwcdma_wenc_enc_tx_enc_r4r99_dch_cipher_struct_type	cipher_keys [6];
	}; 
	uint32 Words[38];
} mcalwcdma_wenc_enc_tx_enc_r4r99_common_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_DO_ENC_REVAB_CAR_WORD_COUNT	5
		mcalwcdma_wenc_enc_tx_do_revab_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_do_enc_revab_car_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_cdma_do_enc_revab_car_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_ENC_CH1_WORD_COUNT	5
		mcalwcdma_wenc_enc_tx_cdma_1x_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch1_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch1_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_ENC_CH2_WORD_COUNT	6
		mcalwcdma_wenc_enc_tx_cdma_1x_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch2_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch2_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ENC_R4_WORD_COUNT	16
		mcalwcdma_wenc_enc_tx_tds_enc_r4_trch_mux_order_cctrch2_struct_type	trch_mux_order_cctrch2 [8];
		mcalwcdma_wenc_enc_tx_tds_enc_r4_cctrch_params_struct_type	cctrch_params;
	}; 
	uint32 Words[16];
} mcalwcdma_wenc_enc_tx_tds_enc_r4_struct_type;


typedef union {
	union {
	#define MCALWCDMA_WENC_TX_ENC_EUL_UNION_WORD_COUNT	3
		mcalwcdma_wenc_enc_tx_umts_enc_eul_struct_type	umts;
		mcalwcdma_wenc_enc_tx_tds_enc_eul_struct_type	tds;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_enc_eul_union_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CDMA_1X_ENC_CH3_WORD_COUNT	6
		mcalwcdma_wenc_enc_tx_cdma_1x_mem_ptr_struct_type	mem_ptr;
		mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch3_cfg_struct_type	enc_cfg;
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch3_struct_type;


typedef union {
	union {
	#define MCALWCDMA_WENC_TX_ENC_R4R99_UNION_WORD_COUNT	16
		mcalwcdma_wenc_enc_tx_umts_enc_r99_struct_type	r99;
		mcalwcdma_wenc_enc_tx_tds_enc_r4_struct_type	r4;
	}; 
	uint32 Words[16];
} mcalwcdma_wenc_enc_tx_enc_r4r99_union_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_R4R99_MMAP_WORD_COUNT	54
		mcalwcdma_wenc_enc_tx_enc_r4r99_common_struct_type	common;
		mcalwcdma_wenc_enc_tx_enc_r4r99_union_type	shared;
	}; 
	uint32 Words[54];
} mcalwcdma_wenc_enc_r4r99_mmap_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_EUL_MMAP_WORD_COUNT	18
		mcalwcdma_wenc_enc_tx_enc_eul_common_struct_type	common;
		mcalwcdma_wenc_enc_tx_enc_eul_union_type	shared;
	}; 
	uint32 Words[18];
} mcalwcdma_wenc_enc_eul_mmap_struct_type;


typedef union {
	union {
	#define MCALWCDMA_WENC_ENC3G_UNION_WORD_COUNT	54
		mcalwcdma_wenc_enc_r4r99_mmap_struct_type	r4r99;
		mcalwcdma_wenc_enc_eul_mmap_struct_type	eul;
		mcalwcdma_wenc_enc_tx_tds_enc_erucch_struct_type	tds_erucch;
		mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch1_struct_type	cdma_1x_ch1;
		mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch2_struct_type	cdma_1x_ch2;
		mcalwcdma_wenc_enc_tx_cdma_1x_enc_ch3_struct_type	cdma_1x_ch3;
		mcalwcdma_wenc_enc_tx_cdma_do_enc_rev0_frame_struct_type	cdma_do_rev0;
		mcalwcdma_wenc_enc_tx_cdma_do_enc_revab_car_struct_type	cdma_do_revb_car;
	}; 
	uint32 Words[54];
} mcalwcdma_wenc_enc_enc3g_union_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_ENC3G_MMAP_WORD_COUNT	55
		mcalwcdma_wenc_enc_tx_enc_common_cfg_struct_type	common_cfg;
		mcalwcdma_wenc_enc_enc3g_union_type	shared;
	}; 
	uint32 Words[55];
} mcalwcdma_wenc_enc_enc3g_mmap_struct_type;
