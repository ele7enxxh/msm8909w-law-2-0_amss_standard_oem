#ifndef _DDR_PHY_CONFIG_H_
#define _DDR_PHY_CONFIG_H_
//#define ENABLE_PRINTF
#include "ddr_target.h"
// for SW compilation to go through in generic way we need to include ddr_target.h in this file , so that the #define of PC32,PC64 , PCLP64 will be picked correctly from SW .
// for compilation independent of SW file , we just need to enable them here according to the chipset .

//*************************************************************************************************************************************************
//common definitions
//*************************************************************************************************************************************************
#define WRLVL_TOTAL_FREQ   7

#define BYTE_PER_DQ_CNT 2
#define JCPLL_CNT                  2
#define DCC_CNT                    2
#define REN_CFG_CNT                7
#define WRLVL_CFG_CNT              7
#define MAX_WRLVL_CDC_DLY          0x7F

#define BYTE_SIZE        8
#define WORD_SIZE        32
#define PHY_SIZE         16
#define WORD_PER_BUS     (BUS_SIZE  / WORD_SIZE)
#define PHY_PER_BUS      (BUS_SIZE  / PHY_SIZE)
#define BYTE_PER_BUS     (BUS_SIZE  / BYTE_SIZE)
#define BYTE_PER_WORD    (WORD_SIZE / BYTE_SIZE)
#define BYTE_PER_PHY     (PHY_SIZE  / BYTE_SIZE)

#define DESKEW_CDC_INITIAL_VALUE    7
#define DM_CSR_INITIAL              0x7777
#define PER_BIT_CSR_INITIAL         0x77777777

//#define DESKEW_CDC_INITIAL_VALUE 0
//#define DM_CSR_INITIAL 0x0
//#define PER_BIT_CSR_INITIAL 0x0

// ***********************************************************************
/// -------------------------------------------------------------
//  LPDDR3 CA : CH0/CH1 Recommended values from SW CAlc 
/// -------------------------------------------------------------
// ***********************************************************************
#define CA_PAD_CFG0_LP_REC_VALUE 0x200FF140
#define CA_PAD_CFG1_LP_REC_VALUE 0x200F00C0
#define CA_PAD_CFG2_LP_REC_VALUE 0x10000000
#define CA_PAD_CFG3_LP_REC_VALUE 0x000001E0
#define CA_PAD_CFG4_LP_REC_VALUE 0x000001E0
#define CA_PAD_CFG5_LP_REC_VALUE 0x0

// ***********************************************************************
/// -------------------------------------------------------------
//  LPDDR3 DQ : CH0/CH1 Recommended values from SW CAlc
/// -------------------------------------------------------------
// ***********************************************************************
#define DQ_PAD_CFG0_LP_REC_VALUE 0x270FF140
#define DQ_PAD_CFG1_LP_REC_VALUE 0x170F00C0
#define DQ_PAD_CFG2_LP_REC_VALUE 0x10010000
#define DQ_PAD_CFG3_LP_REC_VALUE 0x0

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  PCDDR3 CA : CH0/CH1 Recommended values from SW CAlc
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// CH0 Recommended value for CA
#define CH0_CA_PAD_CFG0_PC_REC_VALUE 0x2000F170
#define CH0_CA_PAD_CFG1_PC_REC_VALUE 0x200000F0
#define CH0_CA_PAD_CFG2_PC_REC_VALUE 0x10000000
// #define CH0_CA_PAD_CFG3_PC_REC_VALUE 0x1103C0
// #define CH0_CA_PAD_CFG4_PC_REC_VALUE 0xFFFFFFFF
#define CH0_CA_PAD_CFG5_PC_REC_VALUE 0x0

// CH1 Recommended value for CA
#define CH1_CA_PAD_CFG0_PC_REC_VALUE CH0_CA_PAD_CFG0_PC_REC_VALUE
#define CH1_CA_PAD_CFG1_PC_REC_VALUE CH0_CA_PAD_CFG1_PC_REC_VALUE
#define CH1_CA_PAD_CFG2_PC_REC_VALUE CH0_CA_PAD_CFG2_PC_REC_VALUE
// #define CH1_CA_PAD_CFG3_PC_REC_VALUE CH0_CA_PAD_CFG3_PC_REC_VALUE
// #define CH1_CA_PAD_CFG4_PC_REC_VALUE CH0_CA_PAD_CFG4_PC_REC_VALUE
#define CH1_CA_PAD_CFG5_PC_REC_VALUE CH0_CA_PAD_CFG5_PC_REC_VALUE

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PCDDR3 DQ : CH0/CH1 Recommended values from SW CAlc
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// V1.1
#define DQ_PAD_CFG0_PC_REC_VALUE_V1P1 0x370FF150
#define DQ_PAD_CFG1_PC_REC_VALUE_V1P1 0x1F0F00D0
#define DQ_PAD_CFG2_PC_REC_VALUE_V1P1 0x10010000
#define DQ_PAD_CFG3_PC_REC_VALUE_V1P1 0x0


//*************************************************************************************************************************************************
// product spesific definitions
//l*************************************************************************************************************************************************
/* Luxor */
#if MPQ8092
#define COARSE_STEP                79
static const uint16 fine_lut[8] = {0,12,27,55,68,79,92,108};
#define MAX_COARSE_STEP            16
#define MAX_FINE_STEP              5
static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = {0,12,27,55,68,79,91,106,134,147,158,170,185,213,226,237,249,264,292,305,316,328,343,371,384,395,407,422,450,463,474,486,501,529,542,553,565,580,
608,621,632,644,659,687,700,711,723,738,766,779,790,802,817,845,858,869,881,896,924,937,948,948,960,975,1003,1095,1027,1039,1054,1082,1174,1106,1118,1133,1161,1253,1185,1197,1212,1240};

static const uint8 dq_remap_lut[2][4]  = 
{{ 0, 1, 2, 3},
 { 1, 0, 3, 2}};
static const uint8 ca_remap_lut[2][10] = 
{{0, 1, 2, 3, 4, 5, 11, 12, 13, 14},
 {14, 13, 12, 11, 5, 4, 3, 2, 1, 0}};
//static const uint8 bit_remap_lut[8] = {6, 4, 2, 0, 1, 3, 5, 7};
static const uint8 bit_remap_lut[4][8] = 
{ {6, 4, 2, 0, 1, 3, 5, 7},
  {6, 4, 2, 0, 1, 3, 5, 7},
  {6, 4, 2, 0, 1, 3, 5, 7},
  {6, 4, 2, 0, 1, 3, 5, 7}};

static const uint32 wrlvl_freq_plan[WRLVL_TOTAL_FREQ] = { 1065600,931200, 796800,  528000, 460800, 393600, 307200 }; 
//#define  MAX_WRLVL_CDC_DLY 0x4A
#define CHNL_CNT              2
#define NO_DWORDS             1
#define NUM_BYTES             4
#define DELAY_CNT             4
#define DQ_CNT                2

#define BUS_SIZE  32
#define SITE_PER_CA_CNT 2

#define CHNL_CNT_2
#define SITE_PER_CA_CNT_2

#define DQ_TRAINING_PATTERN_SIZE   48

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PCDDR3 CA: CH0/CH1 Recommended values from SW CAlc for CA
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// CH0 Recommended value for CA
#define CH0_CA_PAD_CFG3_PC_REC_VALUE 0x30000080
#define CH0_CA_PAD_CFG4_PC_REC_VALUE 0x30000080

// CH1 Recommended value for CA
#define CH1_CA_PAD_CFG3_PC_REC_VALUE 0x40802000
#define CH1_CA_PAD_CFG4_PC_REC_VALUE 0x40802000

// ***********************************************************************
/// -------------------------------------------------------------
// PCDDR3: CH0/CH1 Recommended values from SW CAlc for DQ
/// -------------------------------------------------------------
// ***********************************************************************
// V1.0 (Same as V1.1)
#define DQ_PAD_CFG0_PC_REC_VALUE 0x370FF150
#define DQ_PAD_CFG1_PC_REC_VALUE 0x1F0F00D0
#define DQ_PAD_CFG2_PC_REC_VALUE 0x10010000
#define DQ_PAD_CFG3_PC_REC_VALUE 0x0

#endif

//t*************************************************************************************************************************************************
/* Trahira */
#if FSM9900
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

#endif


//g*******************************************************************************************************************************************
/* Gandalf */
#if APQ8084
#define COARSE_STEP                79
static const uint16 fine_lut[8] = {0,12,27,55,68,79,92,108};
#define MAX_COARSE_STEP            16
#define MAX_FINE_STEP              5
#define FAIL_HISTOGRAM_SIZE (MAX_COARSE_STEP * MAX_FINE_STEP)
static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = 
{0,12,27,55,68,79,91,106,134,147,158,170,185,213,226,237,249,264,292,305,316,328,343,371,384,395,407,422,450,463,474,486,501,529,542,553,565,580,
608,621,632,644,659,687,700,711,723,738,766,779,790,802,817,845,858,869,881,896,924,937,948,948,960,975,1003,1095,1027,1039,1054,1082,1174,1106,1118,1133,1161,1253,1185,1197,1212,1240};

static const uint8 dq_remap_lut[2][8]  = 
{ {0, 3, 1, 2, 4, 7, 5, 6},
  {0, 3, 1, 2, 4, 7, 5, 6}};
static const uint8 ca_remap_lut[2][10] = 
{ {  0,  1,  2,  3, 4, 5, 11, 12, 13, 14},
  { 14, 13, 12, 11, 5, 4,  3,  2,  1,  0}};
//static const uint8 bit_remap_lut[8] = {0, 1, 2, 3, 4, 5, 6, 7};
static const uint8 bit_remap_lut[8][8] = 
{ {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {0, 1, 2, 3, 4, 5, 6, 7}};

static const uint32 wrlvl_freq_plan[WRLVL_TOTAL_FREQ] = { 931200, 796800, 662400, 528000, 460800, 393600, 307200 };  
//#define  MAX_WRLVL_CDC_DLY 0x7F
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

#endif






//Sh*************************************************************************************************************************************************
/* SHERE */
#if MSM8936
#define COARSE_STEP                73
static const uint16 fine_lut[8] = {0, 12, 26, 39, 53, 63, 74, 89};
#define MAX_COARSE_STEP            16
#define MAX_FINE_STEP              6
#define FAIL_HISTOGRAM_SIZE (MAX_COARSE_STEP * MAX_FINE_STEP)
//static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = {0,12,27,55,68,79,91,106,134,147,158,170,185,213,226,237,249,264,292,305,316,328,343,371,384,395,407,422,450,463,474,486,501,529,542,553,565,580,608,621,632,644,659,687,700,711,723,738,766,779,790,802,817,845,858,869,881,896,924,937,948,948,960,975,1003,1095,1027,1039,1054,1082,1174,1106,1118,1133,1161,1253,1185,1197,1212,1240};
  static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = 
  {0,12,26,39,53,63,73,85,99,112,126,136,146,158,172,185,199,209,219,231,245,258,272,282,292,304,318,331,345,355,365,377,391,404,418,428,438,450,464,477,491,501,511,523,537,550,564,574,584,596,
  610,623,637,647,657,669,683,696,710,720,730,742,756,769,783,793,803,815,829,842,856,866,876,888,902,915,929,939,949,961,975,988,1002,1012,1022,1034,1048,1061,1075,1085,1095,1107,1121,1134,1148,1158};

static const uint8 bit_remap_lut[4][8] =
{ { 1, 4, 0, 3, 2, 7, 5, 6},
  { 2, 6, 7, 5, 1, 4, 3, 0},
  { 6, 3, 1, 7, 2, 0, 4, 5},
  { 4, 1, 0, 2, 3, 5, 6, 7}};
static const uint8 dq_remap_lut[1][8]  = {{ 2, 3, 0, 1}};
static const uint8 ca_remap_lut[1][10] = {{ 4, 1, 2, 3, 0, 14, 13, 12, 10, 11}};

static const uint32 wrlvl_freq_plan[WRLVL_TOTAL_FREQ] = { 1065600,931200, 796800,  528000, 460800, 393600, 307200 }; 
//#define  MAX_WRLVL_CDC_DLY 0x4A
#define CHNL_CNT              1
#define NO_DWORDS             1
#define NUM_BYTES             4
#define DELAY_CNT             4
#define DQ_CNT                2

#define BUS_SIZE              32
#define SITE_PER_CA_CNT       1

#define DQ_TRAINING_PATTERN_SIZE   48

// ***********************************************************************
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PCDDR3 CA: CH0/CH1 Recommended values from SW CAlc for CA
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ***********************************************************************
// CH0 Recommended value for CA
#define CH0_CA_PAD_CFG3_PC_REC_VALUE 0x30000080
#define CH0_CA_PAD_CFG4_PC_REC_VALUE 0x30000080

// CH1 Recommended value for CA
#define CH1_CA_PAD_CFG3_PC_REC_VALUE 0x40802000
#define CH1_CA_PAD_CFG4_PC_REC_VALUE 0x40802000

// ***********************************************************************
/// -------------------------------------------------------------
// PCDDR3: CH0/CH1 Recommended values from SW CAlc for DQ
/// -------------------------------------------------------------
// ***********************************************************************
// V1.0 (Same as V1.1)
#define DQ_PAD_CFG0_PC_REC_VALUE 0x370FF150
#define DQ_PAD_CFG1_PC_REC_VALUE 0x1F0F00D0
#define DQ_PAD_CFG2_PC_REC_VALUE 0x10010000
#define DQ_PAD_CFG3_PC_REC_VALUE 0x0

#endif

#endif //_DDR_PHY_CONFIG_H_
