#ifndef BSP_H
#define BSP_H
/*===========================================================================

                    Board Support Package Header File

DESCRIPTION
   This header file includes the appropriate GPIO mappings necessary for
   the particular hardware platform.

Copyright (c) 1999 - 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/bsp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/22/12   id      Added support for Tx freq to chan conversion
03/03/11   tks     Added support for WCDMA Band XIX.  
05/12/10   ac      BC11 bringup for j8220
07/30/07   ad      Featurized for linux build
01/24/07   lcl     Merge BC4 PLL calculation fix from tip.
12/08/06   jfc     Separate 1x and UMTS defs of RF_PA_DVS_VOLTAGE
10/17/06   ycl     Modifications for initial MSM7600 build.
06/23/05    cr     Cut oscilator constants into a seperate header file so 
                   that low level software dependent on these constants does not
                   have to include bsp.h which results in inclusion of RF, CAI
                   and REX headers. 
08/19/04   sar     Removed reference to BSP_RF_H.
08/11/04   sar     Replaced bsp_rf_ezrf6500.h with rf_cards.h in include sect.
5/19/04    sar     updated for Feature Enhanced Band Class.
11/19/02   dyc     Provide correct header line.
06/06/02   dyc     Cougar port.
                   Define BSP_TCXO_FREQUENCY_IN_KHZ
12/11/00   rmd     Removed BSP_CPU_CLK_HZ, constant not used.
10/06/00   dsb     RF include filse are now automatically chosen by a make
                   macro.
08/22/00   dsb     Added BSP_TCXO constant defines to replace the TCXO magic
                   numbers inside the bsp_rf*.h files.
07/28/00   dsb     Added QRF3300 support.
01/25/99   jc      Creation.
===========================================================================*/

#include "rfa_variation.h"
#include "customer.h"
#include "target.h"
#ifndef FEATURE_LINUX_STANDALONE_AUDIO
#include "pm.h"
#endif



/* ------------------------------------------------------------------- */
/*           R E F E R E N C E   O S C I L L A T O R (TCXO)            */
/* ------------------------------------------------------------------- */
  
/* TCXO frequencies */
#define BSP_TCXO_13P0_MHZ    13000  /* Khz, 13    Mhz */
#define BSP_TCXO_26P0_MHZ    26000  /* Khz, 26    Mhz */
#define BSP_TCXO_19P2_MHZ    19200  /* Khz, 19.2  Mhz */
#define BSP_TCXO_19P8_MHZ    19800  /* Khz, 19.8  Mhz */
#define BSP_TCXO_19P68_MHZ   19680  /* Khz, 19.68 Mhz */

#ifdef FEATURE_TCXO1920
  #define BSP_RF_OSCILLATOR_IN     (BSP_TCXO_19P2_MHZ)
#endif

#define BSP_OSCILLATOR_IN           BSP_TCXO_19P2_MHZ             /* 19.2 MHz  */

/* TCXO frequency used in KHz */

#define BSP_TCXO_FREQUENCY_IN_KHZ   BSP_TCXO_19P2_MHZ
/* Sleep frequency used in KHz */

#define BSP_SLEEP_FREQUENCY_IN_KHZ   32

/* ------------------------------------------------------------------- */
/*              V O L T A G E   C O N S T A N T S                      */
/* ------------------------------------------------------------------- */

/* Voltage for the PA dynamic voltage swiching voltage regulator */
//TODO consolidate with 1x
#define RF_PA_DVS_VOLTAGE_UMTS    1500                          /* mV */

/* ------------------------------------------------------------------- */
/*              R F  U H F  P A R A M E T E R S                        */
/* ------------------------------------------------------------------- */

/* --------------------------------- */
/*  ALL WCMDA BANDS                  */                  
/* --------------------------------- */
   
#define BSP_RF_FREQ_WCDMA_CH_STEP                   200     /* WCDMA band chan step in kHz */    


/* ------------------------------------------------------------------- */
/* These are the channel numbers used by each band class. Note that    */
/* they do not exactly match those specified in 3GPP 25.101 v7.2.0.    */
/* The top channels have been extended and the channel span is a       */
/* multiple of 10. This is to accomodate the L1 raw frequncy scan      */
/* which runs in 2MHz steps (10 channels                               */
/* ------------------------------------------------------------------- */

/* --------------------------------- */
/*  BC-1 IMT                         */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC1_CH_SPAN               280     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC1_MIN                10562   /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC1_MAX                10842   

#define BSP_RF_WCDMA_BC1_RX_TX_CHAN_NUMBER_OFFSET   950     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC1_MIN                9612    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC1_MAX                9892


/* --------------------------------- */
/*  BC-2 PCS                         */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC2_CH_SPAN               280     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC2_MIN                9662    /* Downlink channel numbers */ 
#define BSP_RF_RX_CHAN_WCDMA_BC2_MAX                9942

#define BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET   400     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC2_MIN                9262    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC2_MAX                9542

/* ADDITIONAL CHANNELS */
#define BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET         9250    /* Delta between regular and additional channel numbers */

#define BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MIN     412     /* Additional downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MAX     687                     

#define BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MIN     12      /* Additional uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MAX     287


/* --------------------------------- */
/*  BC-3 1800                        */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC3_CH_SPAN               360     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC3_MIN                1162    /* Downlink channel numbers */  
#define BSP_RF_RX_CHAN_WCDMA_BC3_MAX                1522

#define BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC3_MIN                937	    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC3_MAX                1297

/* FREQUENCY CALCULATION OFFSETS */
#define BSP_RF_RX_BC3_FREQUENCY_OFFSET              1575    /* Offset used in UARFCN to Freq calculations */
#define BSP_RF_TX_BC3_FREQUENCY_OFFSET              1525


/* --------------------------------- */
/*  BC-4 1700(uplink)/2100(downlink) */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC4_CH_SPAN               210     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC4_MIN                1537    /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC4_MAX                1747
 
#define BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */
#define BSP_RF_WCDMA_BC4_FDL_OFFSET_MINUS_FUL_OFFSET   355  /* Fdl_offset - Ful_offset */

#define BSP_RF_TX_CHAN_WCDMA_BC4_MIN                1312    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC4_MAX                1522

/* ADDITIONAL CHANNELS */                    
#define BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET         (-350)  /* Delta between regular and additional channel numbers */

#define BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MIN     1887    /* Additional downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MAX     2087

#define BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MIN     1662    /* Additional uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MAX     1862

/* FREQUENCY CALCULATION OFFSETS */
#define BSP_RF_RX_BC4_FREQUENCY_OFFSET              1805    /* Offset used in UARFCN to Freq calculations */
#define BSP_RF_TX_BC4_FREQUENCY_OFFSET              1450


/* --------------------------------- */
/*  BC-5 800                         */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC5_CH_SPAN               110     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC5_MIN                4357    /* Downlink channel numbers */ 
#define BSP_RF_RX_CHAN_WCDMA_BC5_MAX                4467

#define BSP_RF_WCDMA_BC5_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC5_MIN                4132    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC5_MAX                4242

/* ADDITIONAL CHANNELS */                    
#define BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET         3350    /* Delta between regular and additional channel numbers */

#define BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MIN     1007    /* Additional downlink channel numbers */ 
#define BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MAX     1087

#define BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MIN     782     /* Additional uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MAX     862


/* --------------------------------- */
/*  BC-6 800                         */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC6_CH_SPAN               30      /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC6_MIN                4387    /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC6_MAX                4417

#define BSP_RF_WCDMA_BC6_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC6_MIN                4162    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC6_MAX                4192

/* ADDITIONAL CHANNELS */                    
#define BSP_BC6_REG_TO_ADDITIONAL_CH_OFFSET         3350    /* Delta between regular and additional channel numbers */

#define BSP_RF_RX_CHAN_WCDMA_BC6_ADDITIONAL_MIN     1037    /* Additional downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC6_ADDITIONAL_MAX     1062

#define BSP_RF_TX_CHAN_WCDMA_BC6_ADDITIONAL_MIN     812     /* Additional uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC6_ADDITIONAL_MAX     837

              
/* --------------------------------- */
/*  BC-7 2600                        */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC7_CH_SPAN               330     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC7_MIN                2237    /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC7_MAX                2567
 
#define BSP_RF_WCDMA_BC7_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC7_MIN                2012    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC7_MAX                2342

/* ADDITIONAL CHANNELS */                    
#define BSP_BC7_REG_TO_ADDITIONAL_CH_OFFSET         (-350)  /* Delta between regular and additional channel numbers */

#define BSP_RF_RX_CHAN_WCDMA_BC7_ADDITIONAL_MIN     2587    /* Additional downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC7_ADDITIONAL_MAX     2912

#define BSP_RF_TX_CHAN_WCDMA_BC7_ADDITIONAL_MIN     2362    /* Additional uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC7_ADDITIONAL_MAX     2687

/* FREQUENCY CALCULATION OFFSETS */
#define BSP_RF_RX_BC7_FREQUENCY_OFFSET              2175    /* Offset used in UARFCN to Freq calculations */
#define BSP_RF_TX_BC7_FREQUENCY_OFFSET              2100

/* --------------------------------- */
/*  BC-8 900                         */                  
/* --------------------------------- */
          
#define BSP_RF_FREQ_WCDMA_BC8_CH_SPAN               160     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC8_MIN                2937    /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC8_MAX                3097

#define BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC8_MIN                2712    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC8_MAX                2872
                      
/* FREQUENCY CALCULATION OFFSETS */
#define BSP_RF_RX_BC8_FREQUENCY_OFFSET              340     /* Offset used in UARFCN to Freq calculations */
#define BSP_RF_TX_BC8_FREQUENCY_OFFSET              340


/* --------------------------------- */
/*  BC-9 1800                         */                  
/* --------------------------------- */  

#define BSP_RF_FREQ_WCDMA_BC9_CH_SPAN               150     /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC9_MIN                9237    /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC9_MAX                9387

#define BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET   475     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC9_MIN                8762    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC9_MAX                8912


/* --------------------------------- */
/*  BC-11 1500                       */                  
/* --------------------------------- */  

#define BSP_RF_FREQ_WCDMA_BC11_CH_SPAN               75      /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC11_MIN                3712    /* Downlink channel numbers */
#define BSP_RF_RX_CHAN_WCDMA_BC11_MAX                3787

#define BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET   225     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC11_MIN                3487    /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC11_MAX                3562

/* FREQUENCY CALCULATION OFFSETS */
#define BSP_RF_RX_BC11_FREQUENCY_OFFSET              736    /* Offset used in UARFCN to Freq calculations */
#define BSP_RF_TX_BC11_FREQUENCY_OFFSET              733

/* --------------------------------- */
/*  BC-19 830-875                    */                  
/* --------------------------------- */

#define BSP_RF_FREQ_WCDMA_BC19_CH_SPAN               54      /* Number of channels */

#define BSP_RF_RX_CHAN_WCDMA_BC19_MIN                712     /* Downlink channel numbers */ 
#define BSP_RF_RX_CHAN_WCDMA_BC19_MAX                763

#define BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET   400     /* channel number offset between downlink and uplink */

#define BSP_RF_TX_CHAN_WCDMA_BC19_MIN                312     /* Uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC19_MAX                363

/* ADDITIONAL CHANNELS */                    
#define BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET         75     /* Delta between regular and additional channel numbers */

#define BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MIN     787    /* Additional downlink channel numbers */ 
#define BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MAX     837

#define BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MIN     387    /* Additional uplink channel numbers */
#define BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MAX     437

/* FREQUENCY CALCULATION OFFSETS */
#define BSP_RF_RX_BC19_FREQUENCY_OFFSET              735    /* Offset used in UARFCN to Freq calculations */
#define BSP_RF_TX_BC19_FREQUENCY_OFFSET              770
          
/* ------------------------------------------------------------------- */
/*              L E G A C Y   D E F I N E S                            */
/* ------------------------------------------------------------------- */

 
/* ------------------------------------------------------------------- */
/* These defines are used in WCDMA L1/RRC for the search algorithms.   */
/* Some are also used in the RF driver, though the new bands supported */
/* by Platform E (BC3, 4, 8 and 9) use the new defines above.          */
/* ------------------------------------------------------------------- */

/* IMT band reference freq */     
#define BSP_RF_FREQ_IMT_REF                         200    /* kHz */     

/* IMT band chan step */                  
#define BSP_RF_FREQ_IMT_CH_STEP                     200     

/* CHANNEL SPAN */

/* IMT band channel span */
#define BSP_RF_FREQ_IMT_CH_SPAN                    BSP_RF_FREQ_WCDMA_BC1_CH_SPAN

/* WCDMA PCS band ch span */
#define BSP_RF_FREQ_WCDMA_PCS_CH_SPAN              BSP_RF_FREQ_WCDMA_BC2_CH_SPAN 

/* WCDMA 1800 band ch span */
#define BSP_RF_FREQ_WCDMA_1800_CH_SPAN             BSP_RF_FREQ_WCDMA_BC3_CH_SPAN 

/* WCDMA 1700/2100 band ch span */
#define BSP_RF_FREQ_WCDMA_1700_2100_CH_SPAN        BSP_RF_FREQ_WCDMA_BC4_CH_SPAN 

/* WCDMA 900 band ch span */
#define BSP_RF_FREQ_WCDMA_900_CH_SPAN              BSP_RF_FREQ_WCDMA_BC8_CH_SPAN

/* WCDMA 800 band ch span */
#define BSP_RF_FREQ_WCDMA_800_CH_SPAN              BSP_RF_FREQ_WCDMA_BC5_CH_SPAN

/* WCDMA 1500 band ch span */
#define BSP_RF_FREQ_WCDMA_1500_CH_SPAN              BSP_RF_FREQ_WCDMA_BC11_CH_SPAN

/* WCDMA B19 band ch span */
#define BSP_RF_FREQ_WCDMA_B19_CH_SPAN              BSP_RF_FREQ_WCDMA_BC19_CH_SPAN

/* CHANNEL NUMBER OFFSET */

/* channel number offset between downlink and uplink for IMT */
#define BSP_RF_WCDMA_RX_TX_CHAN_NUMBER_OFFSET      BSP_RF_WCDMA_BC1_RX_TX_CHAN_NUMBER_OFFSET  

/* channel number offset between downlink and uplink for PCS */
#define BSP_RF_WCDMA_PCS_RX_TX_CHAN_NUMBER_OFFSET  BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET

/* channel number offset between downlink and uplink for 1800 */
#define BSP_RF_WCDMA_1800_RX_TX_CHAN_NUMBER_OFFSET BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET

/* channel number offset between downlink and uplink for 1700/2100 */
#define BSP_RF_WCDMA_1700_2100_RX_TX_CHAN_NUMBER_OFFSET  BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET

/* channel number offset between downlink and uplink for 900 */
#define BSP_RF_WCDMA_900_RX_TX_CHAN_NUMBER_OFFSET  BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET

/* channel number offset between downlink and uplink for 800 */
#define BSP_RF_WCDMA_800_RX_TX_CHAN_NUMBER_OFFSET  BSP_RF_WCDMA_BC5_RX_TX_CHAN_NUMBER_OFFSET

/* delta of regular PCS to 100Khz PCS channel, 9262 - 12 */
#define BSP_PCS_TO_100KHZ_PCS_CH_OFFSET            BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET

/* delta of 800 regular to additional channel, 4132-782 */
#define BSP_800_REG_TO_ADDITIONAL_CH_OFFSET        BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET

/* delta of 1700/2100 (BC4) regular to additional channel, 8562-1162 */
#define BSP_1700_2100_REG_TO_ADDITIONAL_CH_OFFSET  BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET

/* channel number offset between downlink and uplink for UMTS B19 */
#define BSP_RF_WCDMA_B19_RX_TX_CHAN_NUMBER_OFFSET  BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET

/* delta of B19 regular to additional channel */
#define BSP_B19_REG_TO_ADDITIONAL_CH_OFFSET        BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET

/* DOWNLINK CHANNEL */

/* Downlink channel boundary for IMT band (I)*/
#define BSP_RF_RX_CHAN_WCDMA_MIN                   BSP_RF_RX_CHAN_WCDMA_BC1_MIN
#define BSP_RF_RX_CHAN_WCDMA_MAX                   BSP_RF_RX_CHAN_WCDMA_BC1_MAX

/* Downlink channel boundary for PCS band (II)*/
#define BSP_RF_RX_CHAN_WCDMA_PCS_MIN               BSP_RF_RX_CHAN_WCDMA_BC2_MIN   
#define BSP_RF_RX_CHAN_WCDMA_PCS_MAX               BSP_RF_RX_CHAN_WCDMA_BC2_MAX

/* Downlink channel boundary for PCS 100khz offset band */
#define BSP_RF_RX_CHAN_WCDMA_PCS_100KHZ_OFFSET_MIN BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MIN
#define BSP_RF_RX_CHAN_WCDMA_PCS_100KHZ_OFFSET_MAX BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MAX

/* Down link channel boundary for BC3 */
#define BSP_RF_RX_CHAN_WCDMA_1800_MIN              BSP_RF_RX_CHAN_WCDMA_BC3_MIN   
#define BSP_RF_RX_CHAN_WCDMA_1800_MAX              BSP_RF_RX_CHAN_WCDMA_BC3_MAX

/* Down link channel boundary for AWS 1700/2100 band (IV) */
#define BSP_RF_RX_CHAN_WCDMA_1700_2100_MIN         BSP_RF_RX_CHAN_WCDMA_BC4_MIN   
#define BSP_RF_RX_CHAN_WCDMA_1700_2100_MAX         BSP_RF_RX_CHAN_WCDMA_BC4_MAX

/* Down link channel boundary for 900 band (VIII) */
#define BSP_RF_RX_CHAN_WCDMA_900_MIN               BSP_RF_RX_CHAN_WCDMA_BC8_MIN
#define BSP_RF_RX_CHAN_WCDMA_900_MAX               BSP_RF_RX_CHAN_WCDMA_BC8_MAX

/* Downlink channel boundary for 850 band (V)*/
#define BSP_RF_RX_CHAN_WCDMA_850_MIN               BSP_RF_RX_CHAN_WCDMA_BC5_MIN   
#define BSP_RF_RX_CHAN_WCDMA_850_MAX               BSP_RF_RX_CHAN_WCDMA_BC5_MAX

/* Downlink channel boundary for 800 band (VI)*/
#define BSP_RF_RX_CHAN_WCDMA_800_MIN               BSP_RF_RX_CHAN_WCDMA_BC6_MIN   
#define BSP_RF_RX_CHAN_WCDMA_800_MAX               BSP_RF_RX_CHAN_WCDMA_BC6_MAX

/* Downlink channel boundary for 1700/2100 band additional channel (BC4)*/
#define BSP_RF_RX_CHAN_WCDMA_1700_2100_ADDITIONAL_MIN     BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MIN   
#define BSP_RF_RX_CHAN_WCDMA_1700_2100_ADDITIONAL_MAX     BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MAX

/* Downlink channel boundary for 850 band additional channel */
#define BSP_RF_RX_CHAN_WCDMA_850_ADDITIONAL_MIN    BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MIN   
#define BSP_RF_RX_CHAN_WCDMA_850_ADDITIONAL_MAX    BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MAX

/* Downlink channel boundary for 800 band additional channel */
#define BSP_RF_RX_CHAN_WCDMA_800_ADDITIONAL_MIN    BSP_RF_RX_CHAN_WCDMA_BC6_ADDITIONAL_MIN   
#define BSP_RF_RX_CHAN_WCDMA_800_ADDITIONAL_MAX    BSP_RF_RX_CHAN_WCDMA_BC6_ADDITIONAL_MAX

/* Downlink channel boundary for BC9 */
#define BSP_RF_RX_CHAN_WCDMA_1700_MIN              BSP_RF_RX_CHAN_WCDMA_BC9_MIN
#define BSP_RF_RX_CHAN_WCDMA_1700_MAX              BSP_RF_RX_CHAN_WCDMA_BC9_MAX

/* Downlink channel boundary for BC11 */
#define BSP_RF_RX_CHAN_WCDMA_1500_MIN              BSP_RF_RX_CHAN_WCDMA_BC11_MIN
#define BSP_RF_RX_CHAN_WCDMA_1500_MAX              BSP_RF_RX_CHAN_WCDMA_BC11_MAX

/* Downlink channel boundary for UMTS B19*/
#define BSP_RF_RX_CHAN_WCDMA_B19_MIN               BSP_RF_RX_CHAN_WCDMA_BC19_MIN   
#define BSP_RF_RX_CHAN_WCDMA_B19_MAX               BSP_RF_RX_CHAN_WCDMA_BC19_MAX

/* Downlink channel boundary for UMTS B19 band additional channel */
#define BSP_RF_RX_CHAN_WCDMA_B19_ADDITIONAL_MIN    BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MIN   
#define BSP_RF_RX_CHAN_WCDMA_B19_ADDITIONAL_MAX    BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MAX

/* UPLINK CHANNEL */

/* Uplink channel boundary for IMT band */
#define BSP_RF_TX_CHAN_WCDMA_MIN                   BSP_RF_TX_CHAN_WCDMA_BC1_MIN
#define BSP_RF_TX_CHAN_WCDMA_MAX                   BSP_RF_TX_CHAN_WCDMA_BC1_MAX

/* Uplink channel boundary for PCS band */
#define BSP_RF_TX_CHAN_WCDMA_PCS_MIN               BSP_RF_TX_CHAN_WCDMA_BC2_MIN   
#define BSP_RF_TX_CHAN_WCDMA_PCS_MAX               BSP_RF_TX_CHAN_WCDMA_BC2_MAX

/* Uplink channel boundary for BC3 */
#define BSP_RF_TX_CHAN_WCDMA_1800_MIN              BSP_RF_TX_CHAN_WCDMA_BC3_MIN
#define BSP_RF_TX_CHAN_WCDMA_1800_MAX              BSP_RF_TX_CHAN_WCDMA_BC3_MAX

/* Uplink channel boundary for AWS (1700/2100) band (IV) */
#define BSP_RF_TX_CHAN_WCDMA_1700_2100_MIN         BSP_RF_TX_CHAN_WCDMA_BC4_MIN
#define BSP_RF_TX_CHAN_WCDMA_1700_2100_MAX         BSP_RF_TX_CHAN_WCDMA_BC4_MAX

/* Uplink channel boundary for PCS 100khz offset band */
#define BSP_RF_TX_CHAN_WCDMA_PCS_100KHZ_OFFSET_MIN BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MIN
#define BSP_RF_TX_CHAN_WCDMA_PCS_100KHZ_OFFSET_MAX BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MAX

/* Uplink channel boundary for 900 band (VIII) */
#define BSP_RF_TX_CHAN_WCDMA_900_MIN               BSP_RF_TX_CHAN_WCDMA_BC8_MIN
#define BSP_RF_TX_CHAN_WCDMA_900_MAX               BSP_RF_TX_CHAN_WCDMA_BC8_MAX

/* Uplink channel boundary for 850 band */
#define BSP_RF_TX_CHAN_WCDMA_850_MIN               BSP_RF_TX_CHAN_WCDMA_BC5_MIN   
#define BSP_RF_TX_CHAN_WCDMA_850_MAX               BSP_RF_TX_CHAN_WCDMA_BC5_MAX

/* Uplink channel boundary for 800 band */
#define BSP_RF_TX_CHAN_WCDMA_800_MIN               BSP_RF_TX_CHAN_WCDMA_BC6_MIN   
#define BSP_RF_TX_CHAN_WCDMA_800_MAX               BSP_RF_TX_CHAN_WCDMA_BC5_MAX

/* Uplink channel boundary for 1700/2100 (BC4) band additional channel (BC4) */
#define BSP_RF_TX_CHAN_WCDMA_1700_2100_ADDITIONAL_MIN     BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MIN
#define BSP_RF_TX_CHAN_WCDMA_1700_2100_ADDITIONAL_MAX     BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MAX

/* Uplink channel boundary for 850 band additional channel */
#define BSP_RF_TX_CHAN_WCDMA_850_ADDITIONAL_MIN    BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MIN
#define BSP_RF_TX_CHAN_WCDMA_850_ADDITIONAL_MAX    BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MAX

/* Uplink channel boundary for 800 band additional channel */
#define BSP_RF_TX_CHAN_WCDMA_800_ADDITIONAL_MIN    BSP_RF_TX_CHAN_WCDMA_BC6_ADDITIONAL_MIN
#define BSP_RF_TX_CHAN_WCDMA_800_ADDITIONAL_MAX    BSP_RF_TX_CHAN_WCDMA_BC6_ADDITIONAL_MAX

/* Uplink channel boundary for BC9 */
#define BSP_RF_TX_CHAN_WCDMA_1700_MIN              BSP_RF_TX_CHAN_WCDMA_BC9_MIN            
#define BSP_RF_TX_CHAN_WCDMA_1700_MAX              BSP_RF_TX_CHAN_WCDMA_BC9_MAX

/* Uplink channel boundary for BC11 */
#define BSP_RF_TX_CHAN_WCDMA_1500_MIN              BSP_RF_TX_CHAN_WCDMA_BC11_MIN            
#define BSP_RF_TX_CHAN_WCDMA_1500_MAX              BSP_RF_TX_CHAN_WCDMA_BC11_MAX

/* Uplink channel boundary for UMTS B19 */
#define BSP_RF_TX_CHAN_WCDMA_B19_MIN               BSP_RF_TX_CHAN_WCDMA_BC19_MIN   
#define BSP_RF_TX_CHAN_WCDMA_B19_MAX               BSP_RF_TX_CHAN_WCDMA_BC19_MAX

/* Uplink channel boundary for UMTS B19 band additional channel */
#define BSP_RF_TX_CHAN_WCDMA_B19_ADDITIONAL_MIN    BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MIN
#define BSP_RF_TX_CHAN_WCDMA_B19_ADDITIONAL_MAX    BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MAX


/* Frequency boundaries for UMTS bands */
#define BSP_RF_FREQ_WCDMA_MULT_FACTOR 5

/* Frequency boundary for UMTS B1 */
//#define BSP_RF_TX_CHAN_WCDMA_MIN 9612
#define BSP_RF_RX_FREQ_WCDMA_MIN 2110000
#define BSP_RF_RX_FREQ_WCDMA_MAX 2170000
#define BSP_RF_TX_FREQ_WCDMA_MIN 1920000
#define BSP_RF_TX_FREQ_WCDMA_MAX 1980000

/* Frequency boundary and offset for UMTS B11 */
#define BSP_RF_RX_FREQ_WCDMA_BC11_MIN 1475900
#define BSP_RF_RX_FREQ_WCDMA_BC11_MAX 1498400
#define BSP_RF_TX_FREQ_WCDMA_BC11_MIN 1427900
#define BSP_RF_TX_FREQ_WCDMA_BC11_MAX 1450400
#define BSP_RF_RX_BC11_FREQUENCY_OFFSET 736
#define BSP_RF_TX_BC11_FREQUENCY_OFFSET 733

/* Frequency boundary and offset for UMTS B2 */
#define BSP_RF_RX_FREQ_WCDMA_BC2_MIN 1930000
#define BSP_RF_RX_FREQ_WCDMA_BC2_MAX 1990000
#define BSP_RF_TX_FREQ_WCDMA_BC2_MIN 1850000
#define BSP_RF_TX_FREQ_WCDMA_BC2_MAX 1910000
#define BSP_RF_RX_FREQ_WCDMA_BC2_ADDITIONAL_MIN 1932500
#define BSP_RF_RX_FREQ_WCDMA_BC2_ADDITIONAL_MAX 1987500
#define BSP_RF_TX_FREQ_WCDMA_BC2_ADDITIONAL_MIN 1852500
#define BSP_RF_TX_FREQ_WCDMA_BC2_ADDITIONAL_MAX 1907500
#define BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET 9250

/* Frequency boundary and offset for UMTS B3 */
#define BSP_RF_RX_FREQ_WCDMA_BC3_MIN 1805000
#define BSP_RF_RX_FREQ_WCDMA_BC3_MAX 1880000
#define BSP_RF_TX_FREQ_WCDMA_BC3_MIN 1710000
#define BSP_RF_TX_FREQ_WCDMA_BC3_MAX 1785000
#define BSP_RF_RX_BC3_FREQUENCY_OFFSET 1575
#define BSP_RF_TX_BC3_FREQUENCY_OFFSET 1525

/* Frequency boundary and offset for UMTS B9 */
#define BSP_RF_RX_FREQ_WCDMA_BC9_MIN 1844900
#define BSP_RF_RX_FREQ_WCDMA_BC9_MAX 1879900
#define BSP_RF_TX_FREQ_WCDMA_BC9_MIN 1749900
#define BSP_RF_TX_FREQ_WCDMA_BC9_MAX 1784900

/* Frequency boundary and offset for UMTS B4 */
#define BSP_RF_TX_BC4_FREQUENCY_OFFSET 1450
#define BSP_RF_RX_BC4_FREQUENCY_OFFSET 1805
//#define BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET -350
#define BSP_RF_RX_FREQ_WCDMA_BC4_MIN 2110000
#define BSP_RF_RX_FREQ_WCDMA_BC4_MAX 2155000
#define BSP_RF_TX_FREQ_WCDMA_BC4_MIN 1710000
#define BSP_RF_TX_FREQ_WCDMA_BC4_MAX 1755000
#define BSP_RF_RX_FREQ_WCDMA_BC4_ADDITIONAL_MIN 2112500
#define BSP_RF_RX_FREQ_WCDMA_BC4_ADDITIONAL_MAX 2152500
#define BSP_RF_TX_FREQ_WCDMA_BC4_ADDITIONAL_MIN 1712500
#define BSP_RF_TX_FREQ_WCDMA_BC4_ADDITIONAL_MAX 1752500

/* Frequency boundary and offset for UMTS B19 */
#define BSP_RF_RX_BC19_FREQUENCY_OFFSET 735
#define BSP_RF_TX_BC19_FREQUENCY_OFFSET 770
//#define BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET -75
#define BSP_RF_RX_FREQ_WCDMA_BC19_MIN 875000
#define BSP_RF_RX_FREQ_WCDMA_BC19_MAX 890000
#define BSP_RF_TX_FREQ_WCDMA_BC19_MIN 830000
#define BSP_RF_TX_FREQ_WCDMA_BC19_MAX 845000
#define BSP_RF_RX_FREQ_WCDMA_BC19_ADDITIONAL_MIN 877500
#define BSP_RF_RX_FREQ_WCDMA_BC19_ADDITIONAL_MAX 887500
#define BSP_RF_TX_FREQ_WCDMA_BC19_ADDITIONAL_MIN 832500
#define BSP_RF_TX_FREQ_WCDMA_BC19_ADDITIONAL_MAX 842500

/* Frequency boundary and offset for UMTS B5 */
#define BSP_RF_RX_FREQ_WCDMA_BC5_MIN 869000
#define BSP_RF_RX_FREQ_WCDMA_BC5_MAX 894000
#define BSP_RF_TX_FREQ_WCDMA_BC5_MIN 824000
#define BSP_RF_TX_FREQ_WCDMA_BC5_MAX 849000
#define BSP_RF_RX_FREQ_WCDMA_BC5_ADDITIONAL_MIN 871500
#define BSP_RF_RX_FREQ_WCDMA_BC5_ADDITIONAL_MAX 887500
#define BSP_RF_TX_FREQ_WCDMA_BC5_ADDITIONAL_MIN 826500
#define BSP_RF_TX_FREQ_WCDMA_BC5_ADDITIONAL_MAX 842500
#define BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET 3350

/* Frequency boundary and offset for UMTS B8 */
#define BSP_RF_RX_FREQ_WCDMA_BC8_MIN 925000
#define BSP_RF_RX_FREQ_WCDMA_BC8_MAX 960000
#define BSP_RF_TX_FREQ_WCDMA_BC8_MIN 880000
#define BSP_RF_TX_FREQ_WCDMA_BC8_MAX 915000
#define BSP_RF_RX_BC8_FREQUENCY_OFFSET 340
#define BSP_RF_TX_BC8_FREQUENCY_OFFSET 340

///#include "rf_cards.h"

#endif /* BSP_H */
