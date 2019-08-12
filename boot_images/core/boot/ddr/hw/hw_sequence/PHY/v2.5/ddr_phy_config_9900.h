#ifndef _DDR_PHY_CONFIG_9900_H_
#define _DDR_PHY_CONFIG_9900_H_
/* Trahira */
#define COARSE_STEP                79
static const uint16 fine_lut[8] = {0,12,27,55,68,79,92,108};
#define MAX_COARSE_STEP            16
#define MAX_FINE_STEP              5
#define FAIL_HISTOGRAM_SIZE (MAX_COARSE_STEP * MAX_FINE_STEP)
static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = 
{0,12,27,55,68,79,91,106,134,147,158,170,185,213,226,237,249,264,292,305,316,328,343,371,384,395,407,422,450,463,474,486,501,529,542,553,565,580,
608,621,632,644,659,687,700,711,723,738,766,779,790,802,817,845,858,869,881,896,924,937,948,948,960,975,1003,1095,1027,1039,1054,1082,1174,1106,1118,1133,1161,1253,1185,1197,1212,1240};

static const uint8 dq_remap_lut[2][8] = 
{{0, 1, 2, 3, 4, 5, 6, 7},
 {0, 1, 2, 3, 4, 5, 6, 7}};
static const uint8 ca_remap_lut[2][10] = 
{{ 0,  1,  2,  3, 4, 5, 11, 12, 13, 14},
 {14, 13, 12, 11, 5, 4,  3,  2,  1,  0}};
//static const uint8 bit_remap_lut[8] = {7, 6, 5, 4, 3, 2, 1, 0};
static const uint8 bit_remap_lut[8][8] = 
{ {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0},
  {7, 6, 5, 4, 3, 2, 1, 0}};

static const uint32 wrlvl_freq_plan[WRLVL_TOTAL_FREQ] = { 800000, 667000, 664200, 533000, 528000, 420000, 400000 }; 
//#define  MAX_WRLVL_CDC_DLY 0x68  
#define CHNL_CNT              2
#define NO_DWORDS             2
#define NUM_BYTES             8
#define DELAY_CNT             8
#define DQ_CNT                4

#define BUS_SIZE  64
#define SITE_PER_CA_CNT 2

#define NUM_BYTES_8
#define CHNL_CNT_2
#define SITE_PER_CA_CNT_2

#define DQ_TRAINING_PATTERN_SIZE   96


// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  PCDDR3 CA : CH0/CH1 Recommended values from SW CAlc
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// CH0 Recommended value for CA
#define CH0_CA_PAD_CFG3_PC_REC_VALUE 0x1103C0
#define CH0_CA_PAD_CFG4_PC_REC_VALUE 0xFFFFFFFF

// CH1 Recommended value for CA
#define CH1_CA_PAD_CFG3_PC_REC_VALUE CH0_CA_PAD_CFG3_PC_REC_VALUE
#define CH1_CA_PAD_CFG4_PC_REC_VALUE CH0_CA_PAD_CFG4_PC_REC_VALUE

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PCDDR3 DQ : CH0/CH1 Recommended values from SW CAlc
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// V1.0
#define DQ_PAD_CFG0_PC_REC_VALUE 0x350FF170
#define DQ_PAD_CFG1_PC_REC_VALUE 0x1D0F00F0
#define DQ_PAD_CFG2_PC_REC_VALUE 0x10010000
#define DQ_PAD_CFG3_PC_REC_VALUE 0x0

#endif //_DDR_PHY_CONFIG_H_
