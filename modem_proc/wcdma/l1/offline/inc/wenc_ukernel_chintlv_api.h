/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

			MCALWCDMA_WENC_RIF.H : Auto-generated File
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
09/09/14    cjz     SW Encoder API change for THOR
02/25/14    ar      Moved manually added structures to mcalwcdma_wenc.c
10/09/13    ar      File Created : SW Changes due to new architecture for Bolt Tx 
==========================================================================*/
#if defined(FEATURE_THOR_MODEM)

typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_WORD_COUNT	3
		uint32 empty_word0;
		uint32 empty_word1;
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_CDMA	0
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_UMTS	1
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_TDS	2
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_LTE	3
		uint32 tech_mode : 2;
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_ONEX_RC_12	0
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_ONEX_RC_348	1
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_DO_REV0	2
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_DO_REVAB	3
		uint32 ch_type : 2;
		uint32 ch_en : 20;
		uint32 ch_num : 3;
		uint32 slot_num : 4;
		uint32 reserved2 : 1;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_chintlv_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_NUM_SYM_WORD_COUNT	1
		uint32 ch_0 : 16;
		uint32 ch_1 : 16;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_TDS_PARAM_WORD_COUNT	2
		uint32 cctrch0_intlv_type : 2;
		uint32 cctrch0_iram_addr_offset : 9;
		uint32 reserved0 : 5;
		uint32 cctrch1_intlv_type : 2;
		uint32 cctrch1_iram_addr_offset : 9;
		uint32 reserved1 : 5;
		uint32 eul_mod_mode : 1;
		uint32 eul_constellation_rearrange : 2;
		uint32 reserved2 : 29;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_chintlv_tds_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_DO_PARAM_WORD_COUNT	1
		uint32 r0_rri : 3;
		uint32 rab_rri_c0 : 4;
		uint32 rab_rri_c1 : 4;
		uint32 rab_rri_c2 : 4;
		uint32 reserved : 17;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_do_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_MEM_IRAM_ADDR_WORD_COUNT	1
		uint32 iram_word_addr : 14;
		uint32 reserved : 18;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_mem_iram_addr_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_1X_PARAM_WORD_COUNT	1
		uint32 ach_en : 1;
		uint32 fch_is95c : 1;
		uint32 fch_rate : 2;
		uint32 each_en : 1;
		uint32 sch_intlv_size : 2;
		uint32 reserved : 25;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_1x_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_TDS_CH_CFG_WORD_COUNT	1
		uint32 ch0_type : 2;
		uint32 ch0_sf : 3;
		uint32 ch0_cctrch_src : 1;
		uint32 ch0_cfg_src : 1;
		uint32 reserved0 : 9;
		uint32 ch1_type : 2;
		uint32 ch1_sf : 3;
		uint32 ch1_cctrch_src : 1;
		uint32 ch1_cfg_src : 1;
		uint32 reserved1 : 9;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_tds_ch_cfg_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_LTE_PARAM_WORD_COUNT	5
		uint32 acknak_scr_en : 1;
		uint32 acknak_scr_seq_idx : 2;
		uint32 num_freq_car : 11;
		uint32 mod_type : 2;
		uint32 num_symbols : 2;
		uint32 cp_type : 1;
		uint32 cqi_bc_mode : 1;
		uint32 ri_in_type : 2;
		uint32 acknak_in_type : 2;
		uint32 reserved0 : 8;
		uint32 acknak_rm_sym0 : 13;
		uint32 acknak_rm_sym1 : 13;
		uint32 reserved1 : 6;
		uint32 ri_rm_sym : 11;
		uint32 reserved2 : 21;
		uint32 cqi_rm_sym : 14;
		uint32 reserved3 : 18;
		uint32 scr_init : 31;
		uint32 reserved4 : 1;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_chintlv_lte_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_1X_WORD_COUNT	3
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [2];
		mcalwcdma_wenc_enc_tx_chintlv_1x_param_struct_type	param;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_chintlv_1x_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_UMTS_PARAM_WORD_COUNT	4
		uint32 r99_last_row : 9;
		uint32 r99_last_col : 5;
		uint32 reserved0 : 18;
		uint32 eul_en_16qam : 1;
		uint32 eul_area1_addr : 11;
		uint32 eul_area2_addr : 11;
		uint32 reserved1 : 9;
		uint32 edpdch0_num_bits : 15;
		uint32 edpdch1_num_bits : 15;
		uint32 reserved2 : 2;
		uint32 edpdch2_num_bits : 15;
		uint32 edpdch3_num_bits : 15;
		uint32 reserved3 : 2;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_chintlv_umts_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_TDS_WORD_COUNT	22
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [10];
		mcalwcdma_wenc_enc_tx_chintlv_tds_param_struct_type	param;
		mcalwcdma_wenc_enc_tx_chintlv_tds_ch_cfg_struct_type	ch_cfg [10];
	}; 
	uint32 Words[22];
} mcalwcdma_wenc_enc_tx_chintlv_tds_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_DO_WORD_COUNT	4
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [3];
		mcalwcdma_wenc_enc_tx_chintlv_do_param_struct_type	param;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_chintlv_do_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_COMMON_CFG_WORD_COUNT	7
		mcalwcdma_wenc_enc_tx_chintlv_mem_iram_addr_struct_type	mem_iram_addr [4];
		mcalwcdma_wenc_enc_tx_chintlv_param_struct_type	chintlv_param;
	}; 
	uint32 Words[7];
} mcalwcdma_wenc_enc_tx_chintlv_common_cfg_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_LTE_WORD_COUNT	6
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym;
		mcalwcdma_wenc_enc_tx_chintlv_lte_param_struct_type	param;
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_enc_tx_chintlv_lte_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TX_CHINTLV_UMTS_WORD_COUNT	7
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [3];
		mcalwcdma_wenc_enc_tx_chintlv_umts_param_struct_type	param;
	}; 
	uint32 Words[7];
} mcalwcdma_wenc_enc_tx_chintlv_umts_struct_type;


typedef union {
	union {
		#define MCALWCDMA_WENC_TX_CHINTLV_UNION_WORD_COUNT	22
		mcalwcdma_wenc_enc_tx_chintlv_1x_struct_type	cdma_1x;
		mcalwcdma_wenc_enc_tx_chintlv_do_struct_type	cdma_do;
		mcalwcdma_wenc_enc_tx_chintlv_umts_struct_type	umts;
		mcalwcdma_wenc_enc_tx_chintlv_tds_struct_type	tds;
		mcalwcdma_wenc_enc_tx_chintlv_lte_struct_type	lte;
	}; 
	uint32 Words[22];
} mcalwcdma_wenc_enc_tx_chintlv_union_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CHINTLV_MMAP_WORD_COUNT	29
		mcalwcdma_wenc_enc_tx_chintlv_common_cfg_struct_type	common_cfg;
		mcalwcdma_wenc_enc_tx_chintlv_union_type	shared;
	}; 
	uint32 Words[29];
} mcalwcdma_wenc_enc_chintlv_mmap_struct_type;

#else

typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_WORD_COUNT	3
		uint32 start_trigger : 1;
		uint32 reserved0 : 31;
		uint32 chintlv_mp_wr_addr;
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_CDMA	0
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_UMTS	1
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_TDS	2
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_TECH_MODE_LTE	3
		uint32 tech_mode : 2;
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_ONEX_RC_12	0
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_ONEX_RC_348	1
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_DO_REV0	2
		#define MCALWCDMA_WENC_TX_CHINTLV_PARAM_CH_TYPE_DO_REVAB	3
		uint32 ch_type : 2;
		uint32 ch_en : 20;
		uint32 ch_num : 3;
		uint32 slot_num : 4;
		uint32 reserved1 : 1;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_chintlv_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_NUM_SYM_WORD_COUNT	1
		uint32 ch_0 : 16;
		uint32 ch_1 : 16;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_TDS_PARAM_WORD_COUNT	2
		uint32 cctrch0_intlv_type : 2;
		uint32 cctrch0_iram_addr_offset : 9;
		uint32 reserved0 : 5;
		uint32 cctrch1_intlv_type : 2;
		uint32 cctrch1_iram_addr_offset : 9;
		uint32 reserved1 : 5;
		uint32 eul_mod_mode : 1;
		uint32 eul_constellation_rearrange : 2;
		uint32 reserved2 : 29;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_chintlv_tds_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_DO_PARAM_WORD_COUNT	1
		uint32 r0_rri : 3;
		uint32 rab_rri_c0 : 4;
		uint32 rab_rri_c1 : 4;
		uint32 rab_rri_c2 : 4;
		uint32 reserved : 17;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_do_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_MEM_IRAM_ADDR_WORD_COUNT	1
		uint32 iram_word_addr : 14;
		uint32 reserved : 18;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_mem_iram_addr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_1X_PARAM_WORD_COUNT	1
		uint32 ach_en : 1;
		uint32 fch_is95c : 1;
		uint32 fch_rate : 2;
		uint32 each_en : 1;
		uint32 sch_intlv_size : 2;
		uint32 reserved : 25;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_1x_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_TDS_CH_CFG_WORD_COUNT	1
		uint32 ch0_type : 2;
		uint32 ch0_sf : 3;
		uint32 ch0_cctrch_src : 1;
		uint32 ch0_cfg_src : 1;
		uint32 reserved0 : 9;
		uint32 ch1_type : 2;
		uint32 ch1_sf : 3;
		uint32 ch1_cctrch_src : 1;
		uint32 ch1_cfg_src : 1;
		uint32 reserved1 : 9;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_chintlv_tds_ch_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_LTE_PARAM_WORD_COUNT	5
		uint32 acknak_scr_en : 1;
		uint32 acknak_scr_seq_idx : 2;
		uint32 num_freq_car : 11;
		uint32 mod_type : 2;
		uint32 num_symbols : 2;
		uint32 cp_type : 1;
		uint32 cqi_rm : 1;
		uint32 ri_in_type : 2;
		uint32 acknak_in_type : 2;
		uint32 reserved0 : 8;
		uint32 acknak_rm_len0 : 14;
		uint32 acknak_rm_len1 : 14;
		uint32 reserved1 : 4;
		uint32 ri_rm_len : 10;
		uint32 reserved2 : 22;
		uint32 cqi_rm_len : 14;
		uint32 reserved3 : 18;
		uint32 scr_init : 31;
		uint32 reserved4 : 1;
	}; 
	uint32 Words[5];
} mcalwcdma_wenc_enc_tx_chintlv_lte_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_1X_WORD_COUNT	3
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [2];
		mcalwcdma_wenc_enc_tx_chintlv_1x_param_struct_type	param;
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_chintlv_1x_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_UMTS_PARAM_WORD_COUNT	4
		uint32 r99_last_row : 9;
		uint32 r99_last_col : 5;
		uint32 reserved0 : 18;
		uint32 eul_en_16qam : 1;
		uint32 eul_area1_addr : 11;
		uint32 eul_area2_addr : 11;
		uint32 reserved1 : 9;
		uint32 edpdch0_num_bits : 15;
		uint32 edpdch1_num_bits : 15;
		uint32 reserved2 : 2;
		uint32 edpdch2_num_bits : 15;
		uint32 edpdch3_num_bits : 15;
		uint32 reserved3 : 2;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_chintlv_umts_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_TDS_WORD_COUNT	22
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [10];
		mcalwcdma_wenc_enc_tx_chintlv_tds_param_struct_type	param;
		mcalwcdma_wenc_enc_tx_chintlv_tds_ch_cfg_struct_type	ch_cfg [10];
	}; 
	uint32 Words[22];
} mcalwcdma_wenc_enc_tx_chintlv_tds_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_DO_WORD_COUNT	4
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [3];
		mcalwcdma_wenc_enc_tx_chintlv_do_param_struct_type	param;
	}; 
	uint32 Words[4];
} mcalwcdma_wenc_enc_tx_chintlv_do_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_COMMON_CFG_WORD_COUNT	7
		mcalwcdma_wenc_enc_tx_chintlv_mem_iram_addr_struct_type	mem_iram_addr [4];
		mcalwcdma_wenc_enc_tx_chintlv_param_struct_type	chintlv_param;
	}; 
	uint32 Words[7];
} mcalwcdma_wenc_enc_tx_chintlv_common_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_LTE_WORD_COUNT	6
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym;
		mcalwcdma_wenc_enc_tx_chintlv_lte_param_struct_type	param;
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_enc_tx_chintlv_lte_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_CHINTLV_UMTS_WORD_COUNT	7
		mcalwcdma_wenc_enc_tx_chintlv_num_sym_struct_type	num_sym [3];
		mcalwcdma_wenc_enc_tx_chintlv_umts_param_struct_type	param;
	}; 
	uint32 Words[7];
} mcalwcdma_wenc_enc_tx_chintlv_umts_struct_type;


typedef union {
	union {
	#define MCALWCDMA_WENC_TX_CHINTLV_UNION_WORD_COUNT	22
		mcalwcdma_wenc_enc_tx_chintlv_1x_struct_type	cdma_1x;
		mcalwcdma_wenc_enc_tx_chintlv_do_struct_type	cdma_do;
		mcalwcdma_wenc_enc_tx_chintlv_umts_struct_type	umts;
		mcalwcdma_wenc_enc_tx_chintlv_tds_struct_type	tds;
		mcalwcdma_wenc_enc_tx_chintlv_lte_struct_type	lte;
	}; 
	uint32 Words[22];
} mcalwcdma_wenc_enc_tx_chintlv_union_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CHINTLV_MMAP_WORD_COUNT	29
		mcalwcdma_wenc_enc_tx_chintlv_common_cfg_struct_type	common_cfg;
		mcalwcdma_wenc_enc_tx_chintlv_union_type	shared;
	}; 
	uint32 Words[29];
} mcalwcdma_wenc_enc_chintlv_mmap_struct_type;
#endif /* FEATURE_THOR_MODEM */
