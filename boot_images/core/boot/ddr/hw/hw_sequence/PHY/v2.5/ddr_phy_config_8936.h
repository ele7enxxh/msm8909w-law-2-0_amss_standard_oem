#ifndef _DDR_PHY_CONFIG_8936_H_
#define _DDR_PHY_CONFIG_8936_H_
/* SHERE */

#define DQWR_TRAINING_ENABLED        TRUE //NOT USED YET
#define DQRD_TRAINING_ENABLED        TRUE //NOT USED YET
#define WRITE_LVL_ENABLED            TRUE //NOT USED YET
#define CA_TRAINING_ENABLED          TRUE //NOT USED YET
#define REN_RCW_ENABLED              TRUE //NOT USED YET
#define DCC_CAL_ENABLED              TRUE //NOT USED YET
#define IO_PAD_CAL_ENABLED           TRUE //NOT USED YET



//#define FPM_DELAY0_FMAX_IN_KHZ  1066000 // MAXIMUM FREQUENCY POSSIBLE IN THE PRODUCT
#define FPM_DELAY0_FMAX_IN_KHZ  933000   // MAXIMUM FREQUENCY POSSIBLE IN THE PRODUCT
#define FPM_DELAY0_FMIN_IN_KHZ  834029   // 1/12

#define FPM_DELAY1_FMAX_IN_KHZ  834028   // 1/12
#define FPM_DELAY1_FMIN_IN_KHZ  555865   // 1/18

#define FPM_DELAY2_FMAX_IN_KHZ  555864   // 1/18
#define FPM_DELAY2_FMIN_IN_KHZ  400321   // 1/25

#define FPM_DELAY3_FMAX_IN_KHZ  400320   // 1/25
#define FPM_DELAY3_FMIN_IN_KHZ   39215   // 1/255


static const uint16 fine_lut[8] = {0, 12, 26, 39, 53, 63, 74, 89};
#define MAX_COARSE_STEP            16
#define MAX_FINE_STEP              6
#define FAIL_HISTOGRAM_SIZE (MAX_COARSE_STEP * MAX_FINE_STEP)
#define COARSE_STEP                73
 static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = 
 {0,12,26,39,53,63,73,85,99,112,126,136,146,158,172,185,199,209,219,231,245,258,272,282,292,304,318,331,345,355,365,377,391,404,418,428,438,450,464,477,491,501,511,523,537,550,564,574,584,596,
 610,623,637,647,657,669,683,696,710,720,730,742,756,769,783,793,803,815,829,842,856,866,876,888,902,915,929,939,949,961,975,988,1002,1012,1022,1034,1048,1061,1075,1085,1095,1107,1121,1134,1148,1158};
//#define COARSE_STEP 70
//static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] ={0, 12, 26, 39, 53, 63, 70, 82, 96, 109, 123, 133, 140, 152, 166, 179, 193, 203, 210, 222, 236, 249, 263, 273, 280, 292, 306, 319, 333, 343, 350, 362, 376, 389, 403, 413, 420, 432, 446, 459, 473, 483, 490, 502, 516, 529, 543, 553, 560, 572, 586, 599, 613, 623, 630, 642, 656, 669, 683, 693, 700, 712, 726, 739, 753, 763, 770, 782, 796, 809, 823, 833, 840, 852, 866, 879, 893, 903, 910, 922, 936, 949, 963, 973, 980, 992, 1006, 1019, 1033, 1043, 1050, 1062, 1076, 1089, 1103, 1113}; 

static const uint8 dq_remap_lut[1][8]  = 
{{ 1, 3, 0, 2}};
static const uint8 ca_remap_lut[1][10] =
{{ 0, 1, 2, 3, 4, 10, 11, 12, 13, 14}};       //{{ 4, 1, 2, 3, 0, 14, 13, 12, 10, 11}};
static const uint8 bit_remap_lut[4][8] =
{ { 5, 2, 4, 1, 6, 7, 0, 3 },
  { 2, 0, 4, 3, 1, 6, 7, 5 },
  { 2, 1, 3, 4, 0, 5, 6, 7 },
  { 7, 4, 0, 6, 5, 3, 1, 2 }};

static const uint32 wrlvl_freq_plan[WRLVL_TOTAL_FREQ] = { 1065600,931200, 796800,  528000, 460800, 393600, 307200 }; 
//#define  MAX_WRLVL_CDC_DLY 0x4A
#define CHNL_CNT              1
#define NO_DWORDS             1
#define NUM_BYTES             4
#define DELAY_CNT             4
#define DQ_CNT                2

#define BUS_SIZE              32
#define SITE_PER_CA_CNT       1
#define ALL_CA_FAIL_MAP0      0x000001ef
#define ALL_CA_FAIL_MAP1      0x00000210
#define ALL_CA_PASS_PAT       0x00000000

#define DQ_TRAINING_PATTERN_SIZE   48
#define ALL_DQ_FAIL_PAT      0xFFFFFFFF
#define ALL_DQ_PASS_PAT      0x00000000

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



// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
#define CS_CH0_DDRPHY_CA                                   0x0001
#define CS_CH0_DDRPHY_DQ0                                  0x0002
#define CS_CH0_DDRPHY_DQ1                                  0x0004
#define CS_CH0_DDRPHY_DQ2                                  0x0008
#define CS_CH0_DDRPHY_DQ3                                  0x0010
#define CS_CH0_DDRCC                                       0x0020
#define CS_CH1_DDRPHY_CA                                   0x0040
#define CS_CH1_DDRPHY_DQ0                                  0x0080
#define CS_CH1_DDRPHY_DQ1                                  0x0100
#define CS_CH1_DDRPHY_DQ2                                  0x0200
#define CS_CH1_DDRPHY_DQ3                                  0x0400
#define CS_CH1_DDRCC                                       0x0800
#define ALL_CH0_DQs               ( CS_CH0_DDRPHY_DQ0 |CS_CH0_DDRPHY_DQ1  )
#define ALL_CH1_DQs               ( CS_CH0_DDRPHY_DQ0 |CS_CH0_DDRPHY_DQ1  )
#define PHY_BC_DISABLE                                      0
// ***********************************************************************
#define    DDRPHY_CA_IOC_CTLR_CFG_RECVAL                  0x01010715
#define    DDRPHY_CA_IOC_CTLR_PNCNT_CFG_RECVAL            0x00001010
#define    DDRPHY_CA_IOC_CTLR_TIMER_CFG_RECVAL            0xFA000020
#define    DDRPHY_CA_IOC_CTLR_CHAR_CFG_RECVAL             0x00000000
#define    DDRPHY_CA_CA_IOC_SLV_CFG_RECVAL                0x80000000
#define    DDRPHY_CA_CK_IOC_SLV_CFG_RECVAL                0x80000000
#define    DDRPHY_DQ_DQ_IOC_SLV_CFG_RECVAL                0x80000000
#define    DDRPHY_DQ_DQS_IOC_SLV_CFG_RECVAL               0x80000000
#define    DDRPHY_CA_CK_ADJUST_DELAY_RECVAL               0x00000100
#define    DDRPHY_CA_CDC_WR90_DELAY0_RECVAL               0x00000018
#define    DDRPHY_CA_CDC_WR90_DELAY1_RECVAL               0x00000020
#define    DDRPHY_CA_CDC_WR90_DELAY2_RECVAL               0x00000030
#define    DDRPHY_CA_CDC_WR90_DELAY3_RECVAL               0x00000040
#define    DDRPHY_CA_SITE0_WR_DESKEW0_RECVAL              0x00000000
#define    DDRPHY_CA_SITE0_WR_DESKEW1_RECVAL              0x00000000
#define    DDRPHY_CA_CDC_LP_DELAY_RECVAL                  0x00000001
#define    DDRPHY_CA_CDC_DELAY_SEL_RECVAL                 0x00000011
#define    DDRPHY_CA_CDC_TEST_CFG_RECVAL                  0x00002000
#define    DDRPHY_DQ_WRLVL_CFG0_RECVAL                    0x09100100
#define    DDRPHY_DQ_WRLVL_CFG1_RECVAL                    0x0A100100
#define    DDRPHY_DQ_WRLVL_CFG2_RECVAL                    0x0C100100
#define    DDRPHY_DQ_WRLVL_CFG3_RECVAL                    0x0E100100
#define    DDRPHY_DQ_WRLVL_CFG4_RECVAL                    0x12100100
#define    DDRPHY_DQ_WRLVL_CFG5_RECVAL                    0x19100100
#define    DDRPHY_DQ_WRLVL_CFG6_RECVAL                    0x1E100100
#define    DDRPHY_DQ_CDC_WR90_DELAY0_RECVAL               0x00001818
#define    DDRPHY_DQ_CDC_WR90_DELAY1_RECVAL               0x00002020
#define    DDRPHY_DQ_CDC_WR90_DELAY2_RECVAL               0x00003030
#define    DDRPHY_DQ_CDC_WR90_DELAY3_RECVAL               0x00004040
#define    DDRPHY_DQ_CDC_RD90_DELAY0_RECVAL               0x00001818
#define    DDRPHY_DQ_CDC_RD90_DELAY1_RECVAL               0x00002020
#define    DDRPHY_DQ_CDC_RD90_DELAY2_RECVAL               0x00003030
#define    DDRPHY_DQ_CDC_RD90_DELAY3_RECVAL               0x00004040
#define    DDRPHY_DQ_BYTE0_WR_DESKEW_DQ_RECVAL            0x00000000
#define    DDRPHY_DQ_BYTE1_WR_DESKEW_DQ_RECVAL            0x00000000
#define    DDRPHY_DQ_WR_DESKEW_DM_RECVAL                  0x00000000
#define    DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_EVEN_RECVAL       0x00000000
#define    DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_EVEN_RECVAL       0x00000000
#define    DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_ODD_RECVAL        0x00000000
#define    DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_ODD_RECVAL        0x00000000
#define    DDRPHY_DQ_RD_DESKEW_DM_RECVAL                  0x00000000
#define    DDRPHY_DQ_CDC_RD180_DELAY0_RECVAL              0x00000030
#define    DDRPHY_DQ_CDC_RD180_DELAY1_RECVAL              0x00000040
#define    DDRPHY_DQ_CDC_RD180_DELAY2_RECVAL              0x00000060
#define    DDRPHY_DQ_CDC_RD180_DELAY3_RECVAL              0x00000078
#define    DDRPHY_DQ_CDC_LP_DELAY_RECVAL                  0x00000311 //from0x00000111 to 0x00000311  Updated CDC_RD180_LP_DELAY_VALUE to 0x3 from 0x1 (works for all cases including SVS Vmin issue)
#define    DDRPHY_DQ_CDC_DELAY_SEL_RECVAL                 0x00000011
#define    DDRPHY_DQ_CDC_TEST_CFG_RECVAL                  0x02002000
#define    DDRPHY_DQ_REN_CFG0_RECVAL                      0x00000000
#define    DDRPHY_DQ_REN_CFG1_RECVAL                      0x00000000
#define    DDRPHY_DQ_REN_CFG2_RECVAL                      0x00000000
#define    DDRPHY_DQ_REN_CFG3_RECVAL                      0x00000000
#define    DDRPHY_DQ_REN_CFG4_RECVAL                      0x00000000
#define    DDRPHY_DQ_REN_CFG5_RECVAL                      0x00000000
#define    DDRPHY_DQ_REN_CFG6_RECVAL                      0x00000000
#define    DDRPHY_CA_FPM_CFG2_RECVAL                      0x00C00032
#define    DDRPHY_DQ_FPM_CFG2_RECVAL                      0x00C01900
#define    DDRPHY_CA_SITE0_DCC_WR90_CFG_RECVAL            0x121A4210
#define    DDRPHY_CA_SITE0_DCC_WRLVL_CFG_RECVAL           0x121A4210
#define    DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG_RECVAL          0x00000000
#define    DDRPHY_DQ_BYTE0_DCC_WR90_CFG_RECVAL            0x121A4210
#define    DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_RECVAL           0x121A4210
#define    DDRPHY_DQ_BYTE1_DCC_WR90_CFG_RECVAL            0x121A4210
#define    DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_RECVAL           0x121A4210
#define    DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG_RECVAL          0x00000000
#define    DDRPHY_CA_TOP_CFG_RECVAL                       0x7C400000
#define    DDRPHY_CA_TOP_CFG1_RECVAL                      0x000003E0
#define    DDRPHY_CA_HW_INFO_RECVAL                       0x00025107
#define    DDRPHY_CA_HW_VERSION_RECVAL                    0x30020001
#define    DDRPHY_CA_PAD_CFG0_RECVAL                      0x200FF140
#define    DDRPHY_CA_PAD_CFG1_RECVAL                      0x200F00C0
#define    DDRPHY_CA_PAD_CFG2_RECVAL                      0x10000000
#define    DDRPHY_CA_PAD_CFG3_RECVAL                      0x000001E0
#define    DDRPHY_CA_PAD_CFG4_RECVAL                      0x000001E0
#define    DDRPHY_CA_PAD_CFG5_RECVAL                      0x00000000
#define    DDRPHY_CA_CDC_CFG_RECVAL                       0x000C1913
#define    DDRPHY_CA_SITE0_CSD_RX_CFG_RECVAL              0x80000100
#define    DDRPHY_CA_SITE_CK_DP_CFG_RECVAL                0x00001111
#define    DDRPHY_DQ_TOP_CFG_RECVAL                       0x7A200000
#define    DDRPHY_DQ_HW_INFO_RECVAL                       0x00025107
#define    DDRPHY_DQ_HW_VERSION_RECVAL                    0x30020001
#define    DDRPHY_DQ_PAD_CFG0_RECVAL                      0x270FF140
#define    DDRPHY_DQ_PAD_CFG1_RECVAL                      0x170F00C0
#define    DDRPHY_DQ_PAD_CFG2_RECVAL                      0x10010000
#define    DDRPHY_DQ_PAD_CFG3_RECVAL                      0x00000000
#define    DDRPHY_DQ_CDC_CFG_RECVAL                       0x000C1913
#define    DDRPHY_DQ_SITE0_CSD_RX_CFG_RECVAL              0x80000100
#define    DDRPHY_DQ_SITE1_CSD_RX_CFG_RECVAL              0x80000100
#define    DDRPHY_DQ_SITE_CK_DP_CFG_RECVAL                0x00003331

#define    DDRPHY_DQ_FPM_CFG0_RECVAL                      0x0C02C864
#define    DDRPHY_DQ_FPM_CFG1_RECVAL                      0x01800020
#define    DDRPHY_DQ_FPM_CFG3_RECVAL                      0x13041232

#define    DDRPHY_CA_FPM_CFG0_RECVAL                      0x0C02C864
#define    DDRPHY_CA_FPM_CFG1_RECVAL                      0x01800020


#define    END_OF_SETTING                                 0xFFFFFFFF

#if 0
static uint32 ddrphy_ca_mision_mode_cfg[][2] = 
{
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_ADDR(0)                  , DDRPHY_CA_IOC_CTLR_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_PNCNT_CFG_ADDR(0)            , DDRPHY_CA_IOC_CTLR_PNCNT_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_TIMER_CFG_ADDR(0)            , DDRPHY_CA_IOC_CTLR_TIMER_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CHAR_CFG_ADDR(0)             , DDRPHY_CA_IOC_CTLR_CHAR_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CA_IOC_SLV_CFG_ADDR(0)                , DDRPHY_CA_CA_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CK_IOC_SLV_CFG_ADDR(0)                , DDRPHY_CA_CK_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CK_ADJUST_DELAY_ADDR(0)               , DDRPHY_CA_CK_ADJUST_DELAY_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY2_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY3_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0_ADDR(0)              , DDRPHY_CA_SITE0_WR_DESKEW0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1_ADDR(0)              , DDRPHY_CA_SITE0_WR_DESKEW1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_LP_DELAY_ADDR(0)                  , DDRPHY_CA_CDC_LP_DELAY_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_DELAY_SEL_ADDR(0)                 , DDRPHY_CA_CDC_DELAY_SEL_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_TEST_CFG_ADDR(0)                  , DDRPHY_CA_CDC_TEST_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_FPM_CFG2_ADDR(0)                      , DDRPHY_CA_FPM_CFG2_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG_ADDR(0)            , DDRPHY_CA_SITE0_DCC_WR90_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG_ADDR(0)           , DDRPHY_CA_SITE0_DCC_WRLVL_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG_ADDR(0)          , DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_TOP_CFG_ADDR(0)                       , DDRPHY_CA_TOP_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_TOP_CFG1_ADDR(0)                      , DDRPHY_CA_TOP_CFG1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_HW_INFO_ADDR(0)                       , DDRPHY_CA_HW_INFO_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_HW_VERSION_ADDR(0)                    , DDRPHY_CA_HW_VERSION_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_ADDR(0)                      , DDRPHY_CA_PAD_CFG0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_ADDR(0)                      , DDRPHY_CA_PAD_CFG1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG2_ADDR(0)                      , DDRPHY_CA_PAD_CFG2_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG3_ADDR(0)                      , DDRPHY_CA_PAD_CFG3_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG4_ADDR(0)                      , DDRPHY_CA_PAD_CFG4_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG5_ADDR(0)                      , DDRPHY_CA_PAD_CFG5_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_CFG_ADDR(0)                       , DDRPHY_CA_CDC_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_CSD_RX_CFG_ADDR(0)              , DDRPHY_CA_SITE0_CSD_RX_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_ADDR(0)                , DDRPHY_CA_SITE_CK_DP_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_FPM_CFG0_ADDR(0)                      , DDRPHY_CA_FPM_CFG0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_FPM_CFG1_ADDR(0)                      , DDRPHY_CA_FPM_CFG1_RECVAL ,
			// SHOWS END OF SETTING
            END_OF_SETTING                                                 , END_OF_SETTING  
};
static uint32 ddrphy_dq_mision_mode_cfg[][2] = 
{
            HWIO_DDRPHY_DQ_DDRPHY_DQ_DQ_IOC_SLV_CFG_ADDR(0)                , DDRPHY_DQ_DQ_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_DQS_IOC_SLV_CFG_ADDR(0)               , DDRPHY_DQ_DQS_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG4_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG5_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG6_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY0_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY1_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY2_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY3_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_WR_DESKEW_DQ_ADDR(0)            , DDRPHY_DQ_BYTE0_WR_DESKEW_DQ_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_WR_DESKEW_DQ_ADDR(0)            , DDRPHY_DQ_BYTE1_WR_DESKEW_DQ_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WR_DESKEW_DM_ADDR(0)                  , DDRPHY_DQ_WR_DESKEW_DM_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_EVEN_ADDR(0)       , DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_EVEN_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_EVEN_ADDR(0)       , DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_EVEN_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_ODD_ADDR(0)        , DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_ODD_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_ODD_ADDR(0)        , DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_ODD_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_RD_DESKEW_DM_ADDR(0)                  , DDRPHY_DQ_RD_DESKEW_DM_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY0_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY1_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY2_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY3_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_LP_DELAY_ADDR(0)                  , DDRPHY_DQ_CDC_LP_DELAY_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_DELAY_SEL_ADDR(0)                 , DDRPHY_DQ_CDC_DELAY_SEL_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_TEST_CFG_ADDR(0)                  , DDRPHY_DQ_CDC_TEST_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG0_ADDR(0)                      , DDRPHY_DQ_REN_CFG0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG1_ADDR(0)                      , DDRPHY_DQ_REN_CFG1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG2_ADDR(0)                      , DDRPHY_DQ_REN_CFG2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG3_ADDR(0)                      , DDRPHY_DQ_REN_CFG3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG4_ADDR(0)                      , DDRPHY_DQ_REN_CFG4_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG5_ADDR(0)                      , DDRPHY_DQ_REN_CFG5_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG6_ADDR(0)                      , DDRPHY_DQ_REN_CFG6_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG2_ADDR(0)                      , DDRPHY_DQ_FPM_CFG2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG_ADDR(0)            , DDRPHY_DQ_BYTE0_DCC_WR90_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_ADDR(0)           , DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG_ADDR(0)            , DDRPHY_DQ_BYTE1_DCC_WR90_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_ADDR(0)           , DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG_ADDR(0)          , DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG0_ADDR(0)                      , DDRPHY_DQ_FPM_CFG0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG1_ADDR(0)                      , DDRPHY_DQ_FPM_CFG1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG3_ADDR(0)                      , DDRPHY_DQ_FPM_CFG3_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_ADDR(0)                       , DDRPHY_DQ_TOP_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_HW_INFO_ADDR(0)                       , DDRPHY_DQ_HW_INFO_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_HW_VERSION_ADDR(0)                    , DDRPHY_DQ_HW_VERSION_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0_ADDR(0)                      , DDRPHY_DQ_PAD_CFG0_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1_ADDR(0)                      , DDRPHY_DQ_PAD_CFG1_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG2_ADDR(0)                      , DDRPHY_DQ_PAD_CFG2_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG3_ADDR(0)                      , DDRPHY_DQ_PAD_CFG3_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_CFG_ADDR(0)                       , DDRPHY_DQ_CDC_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE0_CSD_RX_CFG_ADDR(0)              , DDRPHY_DQ_SITE0_CSD_RX_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE1_CSD_RX_CFG_ADDR(0)              , DDRPHY_DQ_SITE1_CSD_RX_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_ADDR(0)                , DDRPHY_DQ_SITE_CK_DP_CFG_RECVAL ,
			// SHOWS END OF SETTING
            END_OF_SETTING                                                 , END_OF_SETTING  
};
#endif
#endif //_DDR_PHY_CONFIG_8936_H_
