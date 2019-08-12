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
10/09/13    ar      File Created : SW Changes due to new architecture for Bolt Tx 
==========================================================================*/
#if defined(FEATURE_THOR_MODEM)


typedef union {
	struct {
		#define MCALWCDMA_WENC_LTE_IB_PUSCH_DATA_WORD_COUNT	2357
		uint32 samp [2357];
	}; 
	uint32 Words[2357];
} mcalwcdma_wenc_enc_lte_ib_pusch_data_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_UMTS_10MS_WORD_COUNT	3200
		uint32 eib [768];
		uint32 ewb [384];
		uint32 eob_0 [2048];
	}; 
	uint32 Words[3200];
} mcalwcdma_wenc_enc_umts_10ms_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_UMTS_DC_2MS_WORD_COUNT	2688
		uint32 eib [768];
		uint32 ewb [384];
		uint32 eob_0 [768];
		uint32 eob_1 [768];
	}; 
	uint32 Words[2688];
} mcalwcdma_wenc_enc_umts_dc_2ms_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_WCDMA_IB_DCH_WORD_COUNT	440
		uint32 samp [440];
	}; 
	uint32 Words[440];
} mcalwcdma_wenc_enc_wcdma_ib_dch_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_IB_DCH_WORD_COUNT	440
		uint32 samp [440];
	}; 
	uint32 Words[440];
} mcalwcdma_wenc_enc_tds_ib_dch_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_UMTS_SC_2MS_WORD_COUNT	1920
		uint32 eib [768];
		uint32 ewb [384];
		uint32 eob_0 [768];
	}; 
	uint32 Words[1920];
} mcalwcdma_wenc_enc_umts_sc_2ms_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CDMA_IB_WORD_COUNT	224
		uint32 ch1 [16];
		uint32 ch2 [16];
		uint32 ch3 [192];
	}; 
	uint32 Words[224];
} mcalwcdma_wenc_enc_cdma_ib_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_LTE_IB_WORD_COUNT	9428
		mcalwcdma_wenc_enc_lte_ib_pusch_data_struct_type	pusch_data_car0 [2];
		mcalwcdma_wenc_enc_lte_ib_pusch_data_struct_type	pusch_data_car1 [2];
	}; 
	uint32 Words[9428];
} mcalwcdma_wenc_enc_lte_ib_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_WCDMA_IB_EDCH_WORD_COUNT	720
		uint32 samp [720];
	}; 
	uint32 Words[720];
} mcalwcdma_wenc_enc_wcdma_ib_edch_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_MP_ADDR_WORD_COUNT	1
		uint32 word_addr : 30;
		uint32 reserved0 : 2;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_mp_addr_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_CINT_CFG_WORD_COUNT	39
		uint32 dpch [30];
		uint32 epuch [6];
		uint32 erucch [3];
	}; 
	uint32 Words[39];
} mcalwcdma_wenc_enc_tds_cint_cfg_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_EVDO_IB_PACKET_WORD_COUNT	384
		uint32 samp [384];
	}; 
	uint32 Words[384];
} mcalwcdma_wenc_enc_evdo_ib_packet_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CDMA_1X_WORD_COUNT	3328
		uint32 eib [512];
		uint32 reserved [1280];
		uint32 eob_0 [768];
		uint32 eob_1 [768];
	}; 
	uint32 Words[3328];
} mcalwcdma_wenc_enc_cdma_1x_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_EOB_PUSCH_WORD_COUNT	2704
		uint32 ri;
		uint32 ack_nak [2];
		uint32 data_cqi [2701];
	}; 
	uint32 Words[2704];
} mcalwcdma_wenc_enc_eob_pusch_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_LTE_SC_WORD_COUNT	5063
		uint32 eib [2357];
		mcalwcdma_wenc_enc_eob_pusch_struct_type	eob_0;
		uint32 eob_1 [2];
	}; 
	uint32 Words[5063];
} mcalwcdma_wenc_enc_lte_sc_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CDMA_1X_PARAM_WORD_COUNT	50
		uint32 ch1_enc [6];
		uint32 ch1_cint [10];
		uint32 ch2_enc [7];
		uint32 ch2_cint [10];
		uint32 ch3_enc [7];
		uint32 ch3_cint [10];
	}; 
	uint32 Words[50];
} mcalwcdma_wenc_enc_cdma_1x_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_UMTS_PARAM_WORD_COUNT	188
		uint32 r99_enc_param [40];
		uint32 r99_enc_cfg [72];
		uint32 r99_cint_param [14];
		uint32 eul_c0_enc_param [17];
		uint32 eul_c0_cint_param [14];
		uint32 eul_c1_enc_param [17];
		uint32 eul_c1_cint_param [14];
	}; 
	uint32 Words[188];
} mcalwcdma_wenc_enc_umts_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_LTE_PARAM_WORD_COUNT	73
		uint32 pusch_data_enc [27];
		uint32 pusch_ctl_enc [27];
		uint32 pusch_cint [13];
		uint32 pucch_enc [6];
	}; 
	uint32 Words[73];
} mcalwcdma_wenc_enc_lte_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_IB_ERUCCH_WORD_COUNT	2
		uint32 samp [2];
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tds_ib_erucch_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CDMA_DO_WORD_COUNT	3328
		uint32 eib [512];
		uint32 ewb [1280];
		uint32 eob_0 [768];
		uint32 eob_1 [768];
	}; 
	uint32 Words[3328];
} mcalwcdma_wenc_enc_cdma_do_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_PARAM_WORD_COUNT	278
		uint32 r4_enc_param [55];
		uint32 r4_enc_cfg [72];
		uint32 r4_cint_param [29];
		uint32 erucch_enc_param [6];
		uint32 erucch_cint_param [29];
		uint32 eul_enc_param [19];
		uint32 eul_cint_param [29];
		mcalwcdma_wenc_enc_tds_cint_cfg_struct_type	cint_cfg;
	}; 
	uint32 Words[278];
} mcalwcdma_wenc_enc_tds_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CDMA_DO_PARAM_WORD_COUNT	51
		uint32 c0_r0_enc [6];
		uint32 c0_r0_cint [11];
		uint32 c1_enc [6];
		uint32 c1_cint [11];
		uint32 c2_enc [6];
		uint32 c2_cint [11];
	}; 
	uint32 Words[51];
} mcalwcdma_wenc_enc_cdma_do_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_WORD_COUNT	2602
		uint32 eib [768];
		uint32 ewb_r4 [384];
		uint32 ewb_eul [450];
		uint32 eob_0 [500];
		uint32 eob_1 [500];
	}; 
	uint32 Words[2602];
} mcalwcdma_wenc_enc_tds_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_IB_EDCH_WORD_COUNT	350
		uint32 samp [350];
	}; 
	uint32 Words[350];
} mcalwcdma_wenc_enc_tds_ib_edch_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_TDS_IB_WORD_COUNT	1142
		mcalwcdma_wenc_enc_tds_ib_dch_struct_type	dch;
		mcalwcdma_wenc_enc_tds_ib_erucch_struct_type	erucch;
		mcalwcdma_wenc_enc_tds_ib_edch_struct_type	edch [2];
	}; 
	uint32 Words[1142];
} mcalwcdma_wenc_enc_tds_ib_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_CDMA_PARAM_WORD_COUNT	101
		mcalwcdma_wenc_enc_cdma_1x_param_struct_type	onex;
		mcalwcdma_wenc_enc_cdma_do_param_struct_type	evdo;
	}; 
	uint32 Words[101];
} mcalwcdma_wenc_enc_cdma_param_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_MP_PTR_WORD_COUNT	2
		mcalwcdma_wenc_enc_mp_addr_struct_type	mp_addr;
		uint32 reserved0;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_mp_ptr_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_MK_CMD_WORD_COUNT	10
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_TDS_R4	0
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_TDS_ERUCCH	1
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_TDS_EUL	2
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_UMTS_R99	3
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_UMTS_EUL_C0	4
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_UMTS_EUL_C1	5
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_1X_CH1	6
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_1X_CH2	7
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_1X_CH3	8
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REV0	9
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REVAB_C0	10
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REVAB_C1	11
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REVAB_C2	12
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_SCH_DATA_ONLY	13
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_SCH_DATA	14
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_SCH_CTL	15
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_CCH	16
		uint32 tech_mode : 5;
		uint32 reserved : 27;
		mcalwcdma_wenc_enc_mp_ptr_struct_type	enc_in_mp_addr;
		uint32 enc_in_len : 14;
		uint32 reserved1 : 18;
		mcalwcdma_wenc_enc_mp_ptr_struct_type	cint_out_mp_addr;
		uint32 lte_sch_cqi_offset : 12;
		uint32 reserved2 : 20;
		uint32 seq_id : 5;
		uint32 reserved3 : 27;
		#define MCALWCDMA_WENC_MK_CMD_CMD_STATE_IDLE	0
		#define MCALWCDMA_WENC_MK_CMD_CMD_STATE_PENDING	1
		#define MCALWCDMA_WENC_MK_CMD_CMD_STATE_BUSY	2
		uint32 cmd_state : 2;
		uint32 cmd_status : 6;
		uint32 timestamp : 24;
		uint32 reserved4;
	}; 
	uint32 Words[10];
} mcalwcdma_wenc_enc_mk_cmd_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_EVDO_IB_CAR_WORD_COUNT	1536
		mcalwcdma_wenc_enc_evdo_ib_packet_struct_type	packet [4];
	}; 
	uint32 Words[1536];
} mcalwcdma_wenc_enc_evdo_ib_car_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_WCDMA_IB_WORD_COUNT	3320
		mcalwcdma_wenc_enc_wcdma_ib_dch_struct_type	dch;
		mcalwcdma_wenc_enc_wcdma_ib_edch_struct_type	edch_car0 [2];
		mcalwcdma_wenc_enc_wcdma_ib_edch_struct_type	edch_car1 [2];
	}; 
	uint32 Words[3320];
} mcalwcdma_wenc_enc_wcdma_ib_struct_type;


typedef union {
	union {
		#define MCALWCDMA_WENC_PARAM_UNION_WORD_COUNT	278
		mcalwcdma_wenc_enc_cdma_param_struct_type	cdma_param;
		mcalwcdma_wenc_enc_umts_param_struct_type	umts_param;
		mcalwcdma_wenc_enc_tds_param_struct_type	tds_param;
		mcalwcdma_wenc_enc_lte_param_struct_type	lte_param;
	}; 
	uint32 Words[278];
} mcalwcdma_wenc_enc_param_union_type;


typedef union {
	union {
		#define MCALWCDMA_WENC_DATA_BUFF_UNION_WORD_COUNT	5063
		mcalwcdma_wenc_enc_cdma_1x_struct_type	cdma_1x;
		mcalwcdma_wenc_enc_cdma_do_struct_type	cdma_do;
		mcalwcdma_wenc_enc_umts_sc_2ms_struct_type	umts_sc_2ms;
		mcalwcdma_wenc_enc_umts_dc_2ms_struct_type	umts_dc_2ms;
		mcalwcdma_wenc_enc_umts_10ms_struct_type	umts_10ms;
		mcalwcdma_wenc_enc_tds_struct_type	tds;
		mcalwcdma_wenc_enc_lte_sc_struct_type	lte_sc;
	}; 
	uint32 Words[5063];
} mcalwcdma_wenc_enc_data_buff_union_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_ATECH_CMDS_T_WORD_COUNT	30
		mcalwcdma_wenc_enc_mk_cmd_struct_type	c0;
		mcalwcdma_wenc_enc_mk_cmd_struct_type	c1;
		mcalwcdma_wenc_enc_mk_cmd_struct_type	c2;
	}; 
	uint32 Words[30];
} mcalwcdma_wenc_enc_atech_cmds_t_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_EVDO_IB_WORD_COUNT	4608
		mcalwcdma_wenc_enc_evdo_ib_car_struct_type	car [3];
	}; 
	uint32 Words[4608];
} mcalwcdma_wenc_enc_evdo_ib_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_ATECH_SPACE_WORD_COUNT	5904
		mcalwcdma_wenc_enc_atech_cmds_t_struct_type	cmds;
		uint32 reserved0 [2];
		mcalwcdma_wenc_enc_data_buff_union_type	data_buff;
		uint32 reserved1 [9];
		uint32 chint_buffer_0 [256];
		uint32 chint_buffer_1 [256];
		mcalwcdma_wenc_enc_param_union_type	params;
		uint32 reserved2 [10];
	}; 
	uint32 Words[5904];
} mcalwcdma_wenc_enc_atech_space_struct_type;


typedef union {
	struct {
		#define MCALWCDMA_WENC_MMAP_WORD_COUNT	12320
		uint32 iu_work_space [512];
		mcalwcdma_wenc_enc_atech_space_struct_type	at0;
		mcalwcdma_wenc_enc_atech_space_struct_type	at1;
	}; 
	uint32 Words[12320];
} mcalwcdma_wenc_enc_mmap_struct_type;

#else
typedef union {
	struct {
	#define MCALWCDMA_WENC_LTE_IB_PUSCH_DATA_WORD_COUNT	1600
		uint32 samp [1600];
	}; 
	uint32 Words[1600];
} mcalwcdma_wenc_enc_lte_ib_pusch_data_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_UMTS_10MS_WORD_COUNT	3200
		uint32 eib [768];
		uint32 ewb [384];
		uint32 eob_0 [2048];
	}; 
	uint32 Words[3200];
} mcalwcdma_wenc_enc_umts_10ms_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_UMTS_DC_2MS_WORD_COUNT	2688
		uint32 eib [768];
		uint32 ewb [384];
		uint32 eob_0 [768];
		uint32 eob_1 [768];
	}; 
	uint32 Words[2688];
} mcalwcdma_wenc_enc_umts_dc_2ms_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_WCDMA_IB_DCH_WORD_COUNT	440
		uint32 samp [440];
	}; 
	uint32 Words[440];
} mcalwcdma_wenc_enc_wcdma_ib_dch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_MP_VBUF_PARAM_WORD_COUNT	1
		#define MCALWCDMA_WENC_MP_VBUF_PARAM_VBUFF_LEN_VBUFF_LEN	8000
		uint32 vbuff_len : 16;
		#define MCALWCDMA_WENC_MP_VBUF_PARAM_INIT_BANK_INIT_BANK	0
		uint32 init_bank : 3;
		#define MCALWCDMA_WENC_MP_VBUF_PARAM_MIN_BANK_MIN_BANK	0
		uint32 min_bank : 3;
		#define MCALWCDMA_WENC_MP_VBUF_PARAM_MAX_BANK_MAX_BANK	7
		uint32 max_bank : 3;
		uint32 reserved1 : 7;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_mp_vbuf_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_IB_DCH_WORD_COUNT	440
		uint32 samp [440];
	}; 
	uint32 Words[440];
} mcalwcdma_wenc_enc_tds_ib_dch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_UMTS_SC_2MS_WORD_COUNT	1920
		uint32 eib [768];
		uint32 ewb [384];
		uint32 eob_0 [768];
	}; 
	uint32 Words[1920];
} mcalwcdma_wenc_enc_umts_sc_2ms_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CDMA_IB_WORD_COUNT	224
		uint32 ch1 [16];
		uint32 ch2 [16];
		uint32 ch3 [192];
	}; 
	uint32 Words[224];
} mcalwcdma_wenc_enc_cdma_ib_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_LTE_IB_WORD_COUNT	6400
		mcalwcdma_wenc_enc_lte_ib_pusch_data_struct_type	pusch_data_car0 [2];
		mcalwcdma_wenc_enc_lte_ib_pusch_data_struct_type	pusch_data_car1 [2];
	}; 
	uint32 Words[6400];
} mcalwcdma_wenc_enc_lte_ib_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_WCDMA_IB_EDCH_WORD_COUNT	720
		uint32 samp [720];
	}; 
	uint32 Words[720];
} mcalwcdma_wenc_enc_wcdma_ib_edch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_MP_ADDR_WORD_COUNT	1
		uint32 line_addr : 13;
		uint32 reserved0 : 2;
		uint32 start_addr : 17;
	}; 
	uint32 Words[1];
} mcalwcdma_wenc_enc_mp_addr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_CINT_CFG_WORD_COUNT	39
		uint32 dpch [30];
		uint32 epuch [6];
		uint32 erucch [3];
	}; 
	uint32 Words[39];
} mcalwcdma_wenc_enc_tds_cint_cfg_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_EVDO_IB_PACKET_WORD_COUNT	384
		uint32 samp [384];
	}; 
	uint32 Words[384];
} mcalwcdma_wenc_enc_evdo_ib_packet_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CDMA_1X_WORD_COUNT	3328
		uint32 eib [512];
		uint32 reserved [1280];
		uint32 eob_0 [768];
		uint32 eob_1 [768];
	}; 
	uint32 Words[3328];
} mcalwcdma_wenc_enc_cdma_1x_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_EOB_PUSCH_WORD_COUNT	1804
		uint32 ri;
		uint32 ack_nak [2];
		uint32 data_cqi [1801];
	}; 
	uint32 Words[1804];
} mcalwcdma_wenc_enc_eob_pusch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_LTE_SC_WORD_COUNT	3406
		uint32 eib [1600];
		mcalwcdma_wenc_enc_eob_pusch_struct_type	eob_0;
		uint32 eob_1 [2];
	}; 
	uint32 Words[3406];
} mcalwcdma_wenc_enc_lte_sc_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CDMA_1X_PARAM_WORD_COUNT	50
		uint32 ch1_enc [6];
		uint32 ch1_cint [10];
		uint32 ch2_enc [7];
		uint32 ch2_cint [10];
		uint32 ch3_enc [7];
		uint32 ch3_cint [10];
	}; 
	uint32 Words[50];
} mcalwcdma_wenc_enc_cdma_1x_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_UMTS_PARAM_WORD_COUNT	188
		uint32 r99_enc_param [40];
		uint32 r99_enc_cfg [72];
		uint32 r99_cint_param [14];
		uint32 eul_c0_enc_param [17];
		uint32 eul_c0_cint_param [14];
		uint32 eul_c1_enc_param [17];
		uint32 eul_c1_cint_param [14];
	}; 
	uint32 Words[188];
} mcalwcdma_wenc_enc_umts_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_LTE_PARAM_WORD_COUNT	63
		uint32 pusch_data_enc [22];
		uint32 pusch_ctl_enc [22];
		uint32 pusch_cint [13];
		uint32 pucch_enc [6];
	}; 
	uint32 Words[63];
} mcalwcdma_wenc_enc_lte_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_IB_ERUCCH_WORD_COUNT	2
		uint32 samp [2];
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_tds_ib_erucch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CDMA_DO_WORD_COUNT	3328
		uint32 eib [512];
		uint32 ewb [1280];
		uint32 eob_0 [768];
		uint32 eob_1 [768];
	}; 
	uint32 Words[3328];
} mcalwcdma_wenc_enc_cdma_do_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_PARAM_WORD_COUNT	278
		uint32 r4_enc_param [55];
		uint32 r4_enc_cfg [72];
		uint32 r4_cint_param [29];
		uint32 erucch_enc_param [6];
		uint32 erucch_cint_param [29];
		uint32 eul_enc_param [19];
		uint32 eul_cint_param [29];
		mcalwcdma_wenc_enc_tds_cint_cfg_struct_type	cint_cfg;
	}; 
	uint32 Words[278];
} mcalwcdma_wenc_enc_tds_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CDMA_DO_PARAM_WORD_COUNT	51
		uint32 c0_r0_enc [6];
		uint32 c0_r0_cint [11];
		uint32 c1_enc [6];
		uint32 c1_cint [11];
		uint32 c2_enc [6];
		uint32 c2_cint [11];
	}; 
	uint32 Words[51];
} mcalwcdma_wenc_enc_cdma_do_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_WORD_COUNT	2602
		uint32 eib [768];
		uint32 ewb_r4 [384];
		uint32 ewb_eul [450];
		uint32 eob_0 [500];
		uint32 eob_1 [500];
	}; 
	uint32 Words[2602];
} mcalwcdma_wenc_enc_tds_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_IB_EDCH_WORD_COUNT	350
		uint32 samp [350];
	}; 
	uint32 Words[350];
} mcalwcdma_wenc_enc_tds_ib_edch_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_TDS_IB_WORD_COUNT	1142
		mcalwcdma_wenc_enc_tds_ib_dch_struct_type	dch;
		mcalwcdma_wenc_enc_tds_ib_erucch_struct_type	erucch;
		mcalwcdma_wenc_enc_tds_ib_edch_struct_type	edch [2];
	}; 
	uint32 Words[1142];
} mcalwcdma_wenc_enc_tds_ib_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_CDMA_PARAM_WORD_COUNT	101
		mcalwcdma_wenc_enc_cdma_1x_param_struct_type	onex;
		mcalwcdma_wenc_enc_cdma_do_param_struct_type	evdo;
	}; 
	uint32 Words[101];
} mcalwcdma_wenc_enc_cdma_param_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_MP_PTR_WORD_COUNT	2
		mcalwcdma_wenc_enc_mp_addr_struct_type	mp_addr;
		mcalwcdma_wenc_enc_mp_vbuf_param_struct_type	mp_vbuf_param;
	}; 
	uint32 Words[2];
} mcalwcdma_wenc_enc_mp_ptr_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_MK_CMD_WORD_COUNT	10
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_TDS_R4	0
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_TDS_ERUCCH	1
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_TDS_EUL	2
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_UMTS_R99	3
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_UMTS_EUL_C0	4
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_UMTS_EUL_C1	5
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_1X_CH1	6
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_1X_CH2	7
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_1X_CH3	8
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REV0	9
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REVAB_C0	10
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REVAB_C1	11
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_CDMA_DO_REVAB_C2	12
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_SCH_DATA_ONLY	13
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_SCH_DATA	14
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_SCH_CTL	15
		#define MCALWCDMA_WENC_MK_CMD_TECH_MODE_LTE_CCH	16
		uint32 tech_mode : 5;
		uint32 reserved : 27;
		mcalwcdma_wenc_enc_mp_ptr_struct_type	enc_in_mp_addr;
		uint32 enc_in_len : 14;
		uint32 reserved1 : 18;
		mcalwcdma_wenc_enc_mp_ptr_struct_type	cint_out_mp_addr;
		uint32 lte_sch_cqi_offset : 11;
		uint32 reserved2 : 21;
		uint32 seq_id : 5;
		uint32 reserved3 : 27;
		#define MCALWCDMA_WENC_MK_CMD_CMD_STATE_IDLE	0
		#define MCALWCDMA_WENC_MK_CMD_CMD_STATE_PENDING	1
		#define MCALWCDMA_WENC_MK_CMD_CMD_STATE_BUSY	2
		uint32 cmd_state : 2;
		uint32 cmd_status : 6;
		uint32 timestamp : 24;
		uint32 reserved4;
	}; 
	uint32 Words[10];
} mcalwcdma_wenc_enc_mk_cmd_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_EVDO_IB_CAR_WORD_COUNT	1536
		mcalwcdma_wenc_enc_evdo_ib_packet_struct_type	packet [4];
	}; 
	uint32 Words[1536];
} mcalwcdma_wenc_enc_evdo_ib_car_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_WCDMA_IB_WORD_COUNT	3320
		mcalwcdma_wenc_enc_wcdma_ib_dch_struct_type	dch;
		mcalwcdma_wenc_enc_wcdma_ib_edch_struct_type	edch_car0 [2];
		mcalwcdma_wenc_enc_wcdma_ib_edch_struct_type	edch_car1 [2];
	}; 
	uint32 Words[3320];
} mcalwcdma_wenc_enc_wcdma_ib_struct_type;


typedef union {
	union {
	#define MCALWCDMA_WENC_PARAM_UNION_WORD_COUNT	278
		mcalwcdma_wenc_enc_cdma_param_struct_type	cdma_param;
		mcalwcdma_wenc_enc_umts_param_struct_type	umts_param;
		mcalwcdma_wenc_enc_tds_param_struct_type	tds_param;
		mcalwcdma_wenc_enc_lte_param_struct_type	lte_param;
	}; 
	uint32 Words[278];
} mcalwcdma_wenc_enc_param_union_type;


typedef union {
	union {
	#define MCALWCDMA_WENC_DATA_BUFF_UNION_WORD_COUNT	3406
		mcalwcdma_wenc_enc_cdma_1x_struct_type	cdma_1x;
		mcalwcdma_wenc_enc_cdma_do_struct_type	cdma_do;
		mcalwcdma_wenc_enc_umts_sc_2ms_struct_type	umts_sc_2ms;
		mcalwcdma_wenc_enc_umts_dc_2ms_struct_type	umts_dc_2ms;
		mcalwcdma_wenc_enc_umts_10ms_struct_type	umts_10ms;
		mcalwcdma_wenc_enc_tds_struct_type	tds;
		mcalwcdma_wenc_enc_lte_sc_struct_type	lte_sc;
	}; 
	uint32 Words[3406];
} mcalwcdma_wenc_enc_data_buff_union_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_ATECH_CMDS_T_WORD_COUNT	30
		mcalwcdma_wenc_enc_mk_cmd_struct_type	c0;
		mcalwcdma_wenc_enc_mk_cmd_struct_type	c1;
		mcalwcdma_wenc_enc_mk_cmd_struct_type	c2;
	}; 
	uint32 Words[30];
} mcalwcdma_wenc_enc_atech_cmds_t_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_EVDO_IB_WORD_COUNT	4608
		mcalwcdma_wenc_enc_evdo_ib_car_struct_type	car [3];
	}; 
	uint32 Words[4608];
} mcalwcdma_wenc_enc_evdo_ib_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_ATECH_SPACE_WORD_COUNT	3714
		mcalwcdma_wenc_enc_atech_cmds_t_struct_type	cmds;
		mcalwcdma_wenc_enc_data_buff_union_type	data_buff;
		mcalwcdma_wenc_enc_param_union_type	params;
	}; 
	uint32 Words[3714];
} mcalwcdma_wenc_enc_atech_space_struct_type;


typedef union {
	struct {
	#define MCALWCDMA_WENC_MMAP_WORD_COUNT	9128
		uint32 iu_space [1700];
		mcalwcdma_wenc_enc_atech_space_struct_type	at0;
		mcalwcdma_wenc_enc_atech_space_struct_type	at1;
	}; 
	uint32 Words[9128];
} mcalwcdma_wenc_enc_mmap_struct_type;
#endif /* FEATURE_THOR_MODEM */
