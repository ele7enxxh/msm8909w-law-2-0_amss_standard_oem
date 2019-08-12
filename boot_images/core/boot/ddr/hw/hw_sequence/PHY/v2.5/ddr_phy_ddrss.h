
#ifndef _DDR_PHY_DCC_DDRSS_SEQ_HEADER_
#define _DDR_PHY_DCC_DDRSS_SEQ_HEADER_
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "ddr_phy_config.h"

#include "ddr_phy_ebi.h"
#include "ddr_params.h"
#include "bimc_scale.h"
#include "bimc_mc_shke.h"

void DDRSS_dcc_cal( uint32 _inst_, uint32 chnl, uint32 ca_dcc_training[2][2][2][2], uint32 dq_dcc_training[2][4][2][2][2]);
void DDRSS_dcc_cal_set_trained_results( uint32 _inst_, uint32 ca_dcc_training[2][2][2][2], uint32 dq_dcc_training[2][4][2][2][2]);


#define INIT_WRLVL_DLY                                               0
#define MIN_FREQ_PERIOD                                              0x32
#define MAX_WRLVL_DELAY                                              0x7F
#define MID_WRLVL_DELAY                                              0x4E
#define WRLVL_COARSE_DELAY                                           79
#define WRLVL_FINE_DELAY                                             15
#define TMIN                                                         314


void DDRSS_populate_wrlvl_freq_plan( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 clk_plan[WRLVL_TOTAL_FREQ] );
uint32 DDRSS_wrlvl_training( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 chip_select, uint32 clk_freq_in_khz, uint32 max_loop_count, uint16 dq_wrlvl_retimer_training[2][4][7][2], uint16 dq_wrlvl_delay_training[2][4][7][2]);
void DDRSS_wrlvl_training_pre_switch( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 chip_select );
void DDRSS_wrlvl_training_post_switch( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 chip_select, uint32 clk_freq_in_khz, uint32 max_loop_count );
void DDRSS_wrlvl_retimer_cfg( uint32 _inst_, SDRAM_INTERFACE channel_sel,uint16 dq_wrlvl_retimer_training[2][4][7][2], uint16 dq_wrlvl_delay_training[2][4][7][2], uint32 clk_freq_in_khz);
void DDRSS_check_wrlvl_status( uint32 _inst_, SDRAM_INTERFACE channel_sel );
uint32 DDRSS_retimer_sel_LUT( uint32 _inst_, uint32 clk_period, uint32 cdc_wrlvl_delay );
uint32 DDRSS_retimer_oe_ie_LUT( uint32 _inst_, uint32 clk_period, uint32 cdc_wrlvl_delay );
uint32 DDRSS_retimer_sel_func( uint32 _inst_, uint32 clk_freq_in_khz, uint32 cdc_wrlvl_delay );
uint32 DDRSS_retimer_oe_ie_func( uint32 _inst_, uint32 clk_freq_in_khz, uint32 cdc_wrlvl_delay );
void DDRSS_wrlvl_set_trained_results( uint32 _inst_, uint16 dq_wrlvl_retimer_training[2][4][7][2], uint16 dq_wrlvl_delay_training[2][4][7][2]);
void DDRSS_check_wrlvl_status( uint32 _inst_, SDRAM_INTERFACE channel_sel );
void print_stmt1(SDRAM_INTERFACE channel_sel, uint32 clk_freq_in_khz);
void print_stmt2(SDRAM_INTERFACE channel_sel, uint32 byte0_done[DQ_CNT],uint32 byte1_done[DQ_CNT],uint32 byte0_hit_ck_low[DQ_CNT], uint32 byte1_hit_ck_low[DQ_CNT]);
void Set_Apply_Half_Cycle( uint32 flag );
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

void DDRSS_histogram_min_max_scan(uint32 _inst_, uint8 *min, uint8 *max, uint8 fail_count_histogram[FAIL_HISTOGRAM_SIZE], uint8 histogram_size);
void DDRSS_look_up_table_full_cdc(uint32 _inst_, uint16 middle, uint32 clk_freq_in_khz, uint16 full_cdc_delay[4]);

extern void ddr_mem_write_low_speed(uint16 channel_num, uint16 rank, uint32 *pattern, uint16 size, uint16 burst);
extern void ddr_mem_write_high_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst);
extern void ddr_mem_read(uint16 channel_num, uint16 rank, uint32 *data, uint16 size, uint16 burst);


uint16 DDRSS_ddr_phy_dq_rd_training_ddrss( uint32 _inst_, SDRAM_INTERFACE chnl, uint16 rank, uint16 fine_delay_training, uint16
per_bit_deskew_training, uint16 max_loop_count, uint16 coarse_init_val, uint16 fine_init_val, uint16 per_bit_deskew_init_val,
uint16 dq_read_training[2][4][2][4], uint32 dq_read_perbit_training[2][4][2][2],uint32 dq_read_perbit_dm_training[2][4],
uint32 clk_freq_in_khz );


void DDRSS_dq_rd_training_set_trained_results( uint32 _inst_,  uint16 dq_read_training[2][4][2][4] , uint32 dq_read_perbit_training[2][4][2][2], uint32 dq_read_perbit_dm_training[2][4]);
void DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 coarse, uint16 fine );
void DDRSS_dq_rd_set_dq_cdc_rd90_value( uint32 _inst_,SDRAM_INTERFACE  interface,  uint16 dq_read_training[2][4][2][4], uint32 dq_read_perbit_training[2][4][2][2], uint32 dq_read_perbit_dm_training[2][4]);

uint32 DDRSS_dq_rd_training_histogram_update(SDRAM_INTERFACE interface, uint8 delay_step, uint32 read[DQ_TRAINING_PATTERN_SIZE], uint32 expected[DQ_TRAINING_PATTERN_SIZE], uint8 fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE]);

void print_table_header(void);
void clear_histogram(void);
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

void DDRSS_dq_set_all_dq_cdc_wr90_to_coarse_fine( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 coarse, uint16 fine );
void DDRSS_dq_wr_set_dq_cdc_wr90_value( uint32 _inst_,SDRAM_INTERFACE  interface,  uint16 dq_write_training[2][4][2][4], uint32 dq_write_perbit_training[2][4][2], uint32 dq_write_perbit_dm_training[2][4]);
void DDRSS_dq_wr_training_set_trained_results( uint32 _inst_,  uint16 dq_write_training[2][4][2][4], uint32 dq_write_perbit_training[2][4][2], uint32 dq_write_perbit_dm_training[2][4]);
uint16 DDRSS_ddr_phy_dq_wr_training_ddrss( uint32 _inst_,SDRAM_INTERFACE  chnl, uint16 rank, uint16 fine_delay_training, uint16 per_bit_deskew_training, uint16 max_loop_count, uint16 coarse_init_val, uint16 fine_init_val, uint16 per_bit_deskew_init_val, uint16 dq_write_training[2][4][2][4],  uint32 dq_write_perbit_training[2][4][2],  uint32 dq_write_perbit_dm_training[2][4],uint32 clk_freq_in_khz );


uint32 DDRSS_dq_wr_training_histogram_update( SDRAM_INTERFACE  interface,uint16 delay_step, uint32 read[DQ_TRAINING_PATTERN_SIZE], uint32  expected[DQ_TRAINING_PATTERN_SIZE] , uint8  fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE]);

;
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

#define PRECS_RISE                                                   0
#define PRECS_FALL                                                   1
#define CS_RISE                                                      2
#define CS_FALL                                                      3
#define POSTCS_RISE                                                  4
#define POSTCS_FALL                                                  5

#define CA_PATTERN_NUM 4


void DDRSS_ca_training_set_trained_results( uint32 _inst_, uint16 restore_full_cdc_value[2][2][4], uint32 deskew_delay_value[2][2][2]);
extern void DDRSS_look_up_table_full_cdc_err_a( uint32 _inst_, uint16 middle , uint16 training_period_100ps  , uint16 full_cdc_delay[4] );
uint16 DDRSS_ddr_phy_ca_training( uint32 _inst_, SDRAM_INTERFACE interface, uint16 chipselect, uint16 fine_delay_training, uint16 per_bit_deskew_training, uint16 ca_training_full_cdc[2][2][4], uint32 ca_training_perbit[2][2][2],  uint16 max_loop_count, uint16 coarse_init_val, uint16 fine_init_val, uint16 per_bit_deskew_init_val , uint32 clk_freq_in_khz);
uint16 DDRSS_ca_training_histogram_update(uint8 ca_mapping, uint8 site, uint8 delay_step, uint16 read, uint16 expected, uint8 fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE]);


boolean DDRSS_EBI1_PHY_Set_Training_Data(SDRAM_INTERFACE interface, union ddr_trained_params tr_data);
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************


#endif
