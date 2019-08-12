/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

			MCALWCDMA_WENC_RIF.H : Auto-generated File
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/09/13   ar       File Created : SW Changes due to new architecture for Bolt Tx 
==========================================================================*/
typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_R4R99_ENC_TRCH_CFG1_WORD_COUNT	7
		uint32 coding_rate_type : 2;
		uint32 num_code_segs : 4;
		uint32 trch_tti : 3;
		uint32 reserved0 : 8;
		uint32 num_pre_rm_bits : 15;
		uint32 num_bits_code_seg : 13;
		uint32 reserved1 : 3;
		uint32 num_filler_bits : 6;
		uint32 puncture_flag : 1;
		uint32 reserved2 : 9;
		uint32 initial_error1 : 16;
		uint32 initial_error2 : 16;
		uint32 eminus1 : 16;
		uint32 eminus2 : 16;
		uint32 eplus1 : 16;
		uint32 eplus2 : 16;
		uint32 last_row : 5;
		uint32 last_row_bit_exch : 1;
		uint32 num_row_index : 2;
		uint32 reserved3 : 8;
		uint32 last_column : 8;
		uint32 num_columns_minus1 : 8;
		uint32 prime_number : 9;
		uint32 reserved4 : 7;
		uint32 prime_num_index : 6;
		uint32 reserved5 : 10;
	}; 
	uint32 Words[7];
} mcalwcdma_wenc_enc_tx_r4r99_enc_trch_cfg1_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_R4R99_ENC_TRCH_CFG0_WORD_COUNT	2
		uint32 num_trblks : 5;
		uint32 crc_size : 5;
		uint32 reserved0 : 6;
		uint32 trblk_size : 13;
		uint32 reserved1 : 3;
		uint32 eramb_start_addr : 10;
		uint32 reserved2 : 22;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_r4r99_enc_trch_cfg0_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_DPCH1_SLOT_WORD_COUNT	1
		uint32 start_col : 5;
		uint32 start_row : 9;
		uint32 reserved0 : 2;
		uint32 bits_first_jump : 4;
		uint32 bits_before_first_jump : 10;
		uint32 reserved1 : 2;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_tds_dpch1_slot_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_R4R99_ENC_CRC_CTL_WORD_COUNT	1
		uint32 erama_trch_index_w0 : 3;
		uint32 header_size_w0 : 6;
		uint32 will_continue_w0 : 1;
		uint32 ciphering_en_w0 : 1;
		uint32 reserved0 : 21;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_r4r99_enc_crc_ctl_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_DATA_POS_WORD_COUNT	1
		uint32 col : 5;
		uint32 row : 9;
		uint32 reserved : 18;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_DPCH0_SLOT_WORD_COUNT	1
		uint32 start_col : 5;
		uint32 start_row : 9;
		uint32 reserved0 : 2;
		uint32 num_bits_last_jump : 4;
		uint32 num_jumps : 10;
		uint32 reserved1 : 2;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_tx_tds_dpch0_slot_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_R4R99_ENC_CIPHER_CTL_WORD_COUNT	2
		uint32 length_w1 : 14;
		uint32 bearer_w1 : 5;
		uint32 direction_w1 : 1;
		uint32 ck_index_w1 : 3;
		uint32 ciphering_offset_w1 : 5;
		uint32 algorithm_w1 : 1;
		uint32 reserved0 : 3;
		uint32 count_w1;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tx_r4r99_enc_cipher_ctl_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_EPUCH_CHINTLV_CFG_WORD_COUNT	6
		mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type	last_pos;
		mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type	start_slot_pos [5];
	}; 
	uint32 Words[6];
} mcalwcdma_wenc_enc_tx_tds_epuch_chintlv_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_R4R99_ENC_CFG_WORD_COUNT	72
		mcalwcdma_wenc_enc_tx_r4r99_enc_trch_cfg0_struct_type	cfg0 [8];
		mcalwcdma_wenc_enc_tx_r4r99_enc_trch_cfg1_struct_type	cfg1 [8];
	}; 
	uint32 Words[72];
} mcalwcdma_wenc_enc_tx_r4r99_enc_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_DPCH_CHINTLV_CFG_WORD_COUNT	30
		mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type	last_slot_pos_0 [5];
		mcalwcdma_wenc_enc_tx_tds_dpch0_slot_struct_type	slot_0 [10];
		mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type	last_slot_pos_1 [5];
		mcalwcdma_wenc_enc_tx_tds_dpch1_slot_struct_type	slot_1 [10];
	}; 
	uint32 Words[30];
} mcalwcdma_wenc_enc_tx_tds_dpch_chintlv_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_ERUCCH_CHINTLV_CFG_WORD_COUNT	3
		mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type	last_pos;
		mcalwcdma_wenc_enc_tx_tds_data_pos_struct_type	subframe_start_pos [2];
	}; 
	uint32 Words[3];
} mcalwcdma_wenc_enc_tx_tds_erucch_chintlv_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TX_TDS_CHINTLV_CFG_WORD_COUNT	39
		mcalwcdma_wenc_enc_tx_tds_dpch_chintlv_cfg_struct_type	dpch;
		mcalwcdma_wenc_enc_tx_tds_epuch_chintlv_cfg_struct_type	epuch;
		mcalwcdma_wenc_enc_tx_tds_erucch_chintlv_cfg_struct_type	erucch;
	}; 
	uint32 Words[39];
} mcalwcdma_wenc_enc_tx_tds_chintlv_cfg_struct_type;

