#ifndef AdieCtlRegDefs_H
#define AdieCtlRegDefs_H
/*==============================================================================

FILE:      AdieCtlRegDefs.h

DESCRIPTION: Register defines for Adie Controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Edit History

 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/AdieCtlRegDefs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/17/09   nk      Fixed compilation warnings
03/13/09   nk      Created


        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x00	    ADIECTL_MARIMBA_MODE 

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_MARIMBA_MODE_R               0x00
#define ADIECTL_MARIMBA_MODE_M               0x1F

/* This field controlls whether I2C SDA_OUT is driven or not */ 
#define ADIECTL_MARIMBA_MODE_I2C_ACTIVE_M                          0x10
#define ADIECTL_MARIMBA_MODE_I2C_ACTIVE_DRIVE_EN_V                 0x10
#define ADIECTL_MARIMBA_MODE_I2C_ACTIVE_DRIVE_DIS_V                0x00

/* These bits controll the Test Clock Divide by X (DIV_X)  */
#define ADIECTL_MARIMBA_MODE_TEST_CLK_DIV_M                        0x0C
#define ADIECTL_MARIMBA_MODE_TEST_CLK_DIV_4_V                      0x0C
#define ADIECTL_MARIMBA_MODE_TEST_CLK_DIV_3_V                      0x08
#define ADIECTL_MARIMBA_MODE_TEST_CLK_DIV_2_V                      0x04
#define ADIECTL_MARIMBA_MODE_TEST_CLK_DIV_1_V                      0x00

/* This controls reset state of QMEMBIST Controller  */ 
#define ADIECTL_MARIMBA_MODE_QMEMBISTCTL_RESET_M                   0x02
#define ADIECTL_MARIMBA_MODE_QMEMBISTCTL_RESET_ON_V                0x00
#define ADIECTL_MARIMBA_MODE_QMEMBISTCTL_RESET_OFF_V               0x02

/* This controls whether we are in QMEMBIST Mode or not  */ 
#define ADIECTL_MARIMBA_MODE_QMEMBIST_MODE_M                       0x01
#define ADIECTL_MARIMBA_MODE_QMEMBIST_MODE_ON_V                    0x01
#define ADIECTL_MARIMBA_MODE_QMEMBIST_MODE_OFF_V                   0x00

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x01	    SLAVE_ID_FM 

This register reads the 7 bit FM Slave ID for the I2C Bus. 
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_SLAVE_ID_FM_R               0x01
#define ADIECTL_SLAVE_ID_FM_M               0x7F
	
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x02	    SLAVE_ID_CDC

This register reads the 7 bit CDC Slave ID for the I2C Bus. 
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_SLAVE_ID_CDC_R              0x02
#define ADIECTL_SLAVE_ID_CDC_M              0x7F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x03	    SLAVE_ID_QMEMBIST

This register reads the 7 bit QMEMBIST Controller ID for the I2C Bus. 
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_SLAVE_ID_QMEMBIST_R          0x03
#define ADIECTL_SLAVE_ID_QMEMBIST_M          0x7F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x04	    XO_BUFFER_CTL0


*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_XO_BUFFER_CTL0_R            0x04
#define ADIECTL_XO_BUFFER_CTL0_M            0x7F

/* This bit is read only and indicates the status of the xo buffer block */ 
#define ADIECTL_XO_BUFFER_CTL0_STATUS_M                                0x40

/* This bit disables the XO_BUFFER_CTL (it overrides the enable bit
   and any other controll bits from FM and TSCTL) 
*/ 
#define ADIECTL_XO_BUFFER_CTL0_DIG_DISABLE_M                           0x20
#define ADIECTL_XO_BUFFER_CTL0_DIG_DISABLE_ON_V                        0x20
#define ADIECTL_XO_BUFFER_CTL0_DIG_DISABLE_OFF_V                       0x00

/* This bit enables the XO_BUFFER_CTL */ 
#define ADIECTL_XO_BUFFER_CTL0_DIG_EN_M                                0x10
#define ADIECTL_XO_BUFFER_CTL0_DIG_EN_ON_V                             0x10
#define ADIECTL_XO_BUFFER_CTL0_DIG_EN_OFF_V                            0x00

/* This bit enables the use of XO_BUFFER_CTL_0_FM_EN bit below  */
#define ADIECTL_XO_BUFFER_CTL0_FM_FORCE_M                              0x08
#define ADIECTL_XO_BUFFER_CTL0_FM_FORCE_EN_V                           0x08
#define ADIECTL_XO_BUFFER_CTL0_FM_FORCE_DIS_V                          0x00

/* This bit enables the FM (works only when FM_FORCE bit is set) */
#define ADIECTL_XO_BUFFER_CTL0_FM_EN_M                                 0x04
#define ADIECTL_XO_BUFFER_CTL0_FM_EN_ON_V                              0x04
#define ADIECTL_XO_BUFFER_CTL0_FM_EN_OFF_V                             0x00

/* This bit enables the use of XO_BUFFER_CTL_0_BT_EN bit below  */
#define ADIECTL_XO_BUFFER_CTL0_BT_FORCE_M                              0x02
#define ADIECTL_XO_BUFFER_CTL0_BT_FORCE_EN_V                           0x02
#define ADIECTL_XO_BUFFER_CTL0_BT_FORCE_DIS_V                          0x00

/* This bit enables the FM (works only when BT_FORCE bit is set) */
#define ADIECTL_XO_BUFFER_CTL0_BT_EN_M                                 0x01
#define ADIECTL_XO_BUFFER_CTL0_BT_EN_ON_V                              0x01
#define ADIECTL_XO_BUFFER_CTL0_BT_EN_OFF_V                             0x00

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x05	    XO_BUFFER_CTL1


*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_XO_BUFFER_CTL1_R            0x05
#define ADIECTL_XO_BUFFER_CTL1_M            0x07

/* This bit enables the use of XO_BUFFER_CTL_1_HIGH_POWER_EN bit below  */
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_FORCE_M                      0x04
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_FORCE_EN_V                   0x04
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_FORCE_DIS_V                  0x00

/* This selects the hardware signal to controll high power */
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_SELECT_M                     0x02
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_SELECT_ACLK_REQ_V            0x02
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_SELECT_CLK_PWR_REQ_OUT_V     0x00

/* This bit enables high power */ 
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_EN_M                         0x01
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_EN_ON                        0x01
#define ADIECTL_XO_BUFFER_CTL1_HIGH_POWER_EN_OFF                       0x00


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x06	    RBIAS_CTL0	


*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_RBIAS_CTL0_R            0x06
#define ADIECTL_RBIAS_CTL0_M            0xFF

#define ADIECTL_RBIAS_CTL0_INIT_M (ADIECTL_RBIAS_CTL0_VREF_ADJ_NOMINAL | \
                                    ADIECTL_BANDGAP_ADJUST_NOMINAL_V  )

/* This field adjusts the VREF by % */ 
#define ADIECTL_RBIAS_CTL0_VREF_ADJ_M                                  0xF0
#define ADIECTL_RBIAS_CTL0_VREF_ADJ_MINUS_5                            0x00
#define ADIECTL_RBIAS_CTL0_VREF_ADJ_NOMINAL                            0x80
#define ADIECTL_RBIAS_CTL0_VREF_ADJ_PLUS_5                             0xF0

/* This field is used to trim current ouptuts (Default: 0x8) */ 
#define ADIECTL_BANDGAP_ADJUST_M                                       0x0F
#define ADIECTL_BANDGAP_ADJUST_NOMINAL_V                               0x08


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x07	    RBIAS_CTL1	


*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_RBIAS_CTL1_R            0x07
#define ADIECTL_RBIAS_CTL1_M            0xFF

#define ADIECTL_RBIAS_CTL1_INIT_M (ADIECTL_RBIAS_CTL0_VREF_ADJ_NOMINAL | \
                                    ADIECTL_BANDGAP_ADJUST_NOMINAL_V  )

/* This field controlls JS to RBIAS Block % */ 
#define ADIECTL_RBIAS_CTL1_JS_M                                        0x80
#define ADIECTL_RBIAS_CTL1_JS_EN_V                                     0x80
#define ADIECTL_RBIAS_CTL1_JS_DIS_V                                    0x00

/* This bit enables the use of BT_EN bit below  */
#define ADIECTL_RBIAS_CTL1_BT_FORCE_M                                  0x40
#define ADIECTL_RBIAS_CTL1_BT_FORCE_EN_V                               0x40
#define ADIECTL_RBIAS_CTL1_BT_FORCE_DIS_V                              0x00

/* This bit enables the use of FM_EN bit below  */
#define ADIECTL_RBIAS_CTL1_FM_FORCE_M                                  0x20
#define ADIECTL_RBIAS_CTL1_FM_FORCE_EN_V                               0x20
#define ADIECTL_RBIAS_CTL1_FM_FORCE_DIS_V                              0x20

/* This field controlls LPF_EN to RBIAS Block % */ 
#define ADIECTL_RBIAS_CTL1_LPF_EN_M                                    0x10
#define ADIECTL_RBIAS_CTL1_LPF_EN_ON_V                                 0x10
#define ADIECTL_RBIAS_CTL1_LPF_EN_OFF_V                                0x00

/* This field controlls DIV_R to RBIAS Block % */ 
#define ADIECTL_RBIAS_CTL1_DIV_R_EN_M                                  0x08
#define ADIECTL_RBIAS_CTL1_DIV_R_EN_ON_V                               0x08
#define ADIECTL_RBIAS_CTL1_DIV_R_EN_OFF_V                              0x00

/* This bit enables the RBIAS BT(works only when BT_FORCE bit is set) */
#define ADIECTL_RBIAS_CTL1_BT_EN_M                                     0x04
#define ADIECTL_RBIAS_CTL1_BT_EN_ON_V                                  0x04
#define ADIECTL_RBIAS_CTL1_BT_EN_OFF_V                                 0x00

/* This bit enables the RBIAS FM(works only when FM_FORCE bit is set) */
#define ADIECTL_RBIAS_CTL1_FM_EN_M                                     0x02
#define ADIECTL_RBIAS_CTL1_FM_EN_ON_V                                  0x02
#define ADIECTL_RBIAS_CTL1_FM_EN_OFF_V                                 0x00

/* This field controlls MS_EN to RBIAS Block % */ 
#define ADIECTL_RBIAS_CTL1_MS_EN_M                                     0x01
#define ADIECTL_RBIAS_CTL1_MS_EN_ON_V                                  0x01
#define ADIECTL_RBIAS_CTL1_MS_EN_OFF_V                                 0x00

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x08	    HDRIVE


*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_HDRIVE_R            0x08
#define ADIECTL_HDRIVE_M            0x07

#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_M                            0x07
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_2MA_V                        0x00
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_4MA_V                        0x01
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_6MA_V                        0x02
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_8MA_V                        0x03
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_10MA_V                       0x04
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_12MA_V                       0x05
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_14MA_V                       0x06
#define ADIECTL_HDRIVE_PAD_DRIVE_STRENGTH_16MA_V                       0x07	

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x09	    PIN_CTL_OE0

This register enables output for the listed pads
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_OE0_R            0x09
#define ADIECTL_PIN_CTL_OE0_M            0xFF

#define ADIECTL_PIN_CTL_OE0_BT_HCI_2_M                                0x80
#define ADIECTL_PIN_CTL_OE0_BT_HCI_2_EN_V                             0x80
#define ADIECTL_PIN_CTL_OE0_BT_HCI_2_DIS_V                            0x00  

#define ADIECTL_PIN_CTL_OE0_BT_HCI_3_M                                0x40
#define ADIECTL_PIN_CTL_OE0_BT_HCI_3_EN_V                             0x40
#define ADIECTL_PIN_CTL_OE0_BT_HCI_3_DIS_V                            0x00  

#define ADIECTL_PIN_CTL_OE0_BT_HOST_WAKE_M                            0x20
#define ADIECTL_PIN_CTL_OE0_BT_HOST_WAKE_EN_V                         0x20
#define ADIECTL_PIN_CTL_OE0_BT_HOST_WAKE_DIS_V                        0x00  

#define ADIECTL_PIN_CTL_OE0_BT_EXT_WAKE_M                             0x10
#define ADIECTL_PIN_CTL_OE0_BT_EXT_WAKE_EN_V                          0x10
#define ADIECTL_PIN_CTL_OE0_BT_EXT_WAKE_DIS_V                         0x00  

#define ADIECTL_PIN_CTL_OE0_BT_COEX_2_M                               0x08
#define ADIECTL_PIN_CTL_OE0_BT_COEX_2_EN_V                            0x08
#define ADIECTL_PIN_CTL_OE0_BT_COEX_2_DIS_V                           0x00  

#define ADIECTL_PIN_CTL_OE0_BT_COEX_1_M                               0x04
#define ADIECTL_PIN_CTL_OE0_BT_COEX_1_EN_V                            0x04
#define ADIECTL_PIN_CTL_OE0_BT_COEX_1_DIS_V                           0x00  

#define ADIECTL_PIN_CTL_OE0_BT_COEX_0_M                               0x02
#define ADIECTL_PIN_CTL_OE0_BT_COEX_0_EN_V                            0x02
#define ADIECTL_PIN_CTL_OE0_BT_COEX_0_DIS_V                           0x00  

#define ADIECTL_PIN_CTL_OE0_BT_CLK_PWR_REQ_M                          0x01
#define ADIECTL_PIN_CTL_OE0_BT_BT_CLK_PWR_REQ_EN_V                    0x01
#define ADIECTL_PIN_CTL_OE0_BT_BT_CLK_PWR_REQ_DIS_V                   0x00  
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x0A	    PIN_CTL_OE1

This register enables output for the listed pads
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_OE1_R            0x0A
#define ADIECTL_PIN_CTL_OE1_M            0xFF


#define ADIECTL_PIN_CTL_OE1_TSADC_PENIRQ_N_M                          0x80
#define ADIECTL_PIN_CTL_OE1_TSADC_PENIRQ_N_EN_V                       0x80
#define ADIECTL_PIN_CTL_OE1_TSADC_PENIRQ_N_DIS_V                      0x00  

#define ADIECTL_PIN_CTL_OE1_BT_SLEEP_CLK_M                            0x40
#define ADIECTL_PIN_CTL_OE1_BT_SLEEP_CLK_EN_V                         0x40
#define ADIECTL_PIN_CTL_OE1_BT_SLEEP_CLK_DIS_V                        0x00  

#define ADIECTL_PIN_CTL_OE1_BT_PCM_SYNC_M                             0x20
#define ADIECTL_PIN_CTL_OE1_BT_PCM_SYNC_EN_V                          0x20
#define ADIECTL_PIN_CTL_OE1_BT_PCM_SYNC_DIS_V                         0x00  

#define ADIECTL_PIN_CTL_OE1_BT_PCM_DOUT_M                             0x10
#define ADIECTL_PIN_CTL_OE1_BT_PCM_DOUT_EN_V                          0x10
#define ADIECTL_PIN_CTL_OE1_BT_PCM_DOUT_DIS_V                         0x00  

#define ADIECTL_PIN_CTL_OE1_BT_PCM_DIN_M                              0x08
#define ADIECTL_PIN_CTL_OE1_BT_PCM_DIN_EN_V                           0x08
#define ADIECTL_PIN_CTL_OE1_BT_PCM_DIN_DIS_V                          0x00  

#define ADIECTL_PIN_CTL_OE1_BT_PCM_BCLK_M                              0x04
#define ADIECTL_PIN_CTL_OE1_BT_PCM_BCLK_EN_V                           0x04
#define ADIECTL_PIN_CTL_OE1_BT_PCM_BCLK_DIS_V                          0x00  

#define ADIECTL_PIN_CTL_OE1_BT_HCI_0_M                                 0x02
#define ADIECTL_PIN_CTL_OE1_BT_HCI_0_EN_V                              0x02
#define ADIECTL_PIN_CTL_OE1_BT_HCI_0_DIS_V                             0x00  

#define ADIECTL_PIN_CTL_OE1_BT_HCI_1_M                                 0x01
#define ADIECTL_PIN_CTL_OE1_BT_HCI_1_EN_V                              0x01
#define ADIECTL_PIN_CTL_OE1_BT_HCI_1_DIS_V                             0x00  
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x0B	    PIN_CTL_OE2

This register enables output for the listed pads
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_OE2_R            0x0B
#define ADIECTL_PIN_CTL_OE2_M            0xFF


#define ADIECTL_PIN_CTL_OE2_CDC_RX_I2S_SD_M                                0x80
#define ADIECTL_PIN_CTL_OE2_CDC_RX_I2S_SD_EN_V                             0x80
#define ADIECTL_PIN_CTL_OE2_CDC_RX_I2S_SD_DIS_V                            0x00  

#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_SCK_M                               0x40
#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_SCK_EN_V                            0x40
#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_SCK_DIS_V                           0x00  

#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_WS_M                                0x20
#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_WS_EN_V                             0x20
#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_WS_DIS_V                            0x00  

#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_SD_M                                0x10
#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_SD_EN_V                             0x10
#define ADIECTL_PIN_CTL_OE2_CDC_TX_I2S_SD_DIS_V                            0x00  

#define ADIECTL_PIN_CTL_OE2_CDC_MCLK2_M                                    0x08
#define ADIECTL_PIN_CTL_OE2_CDC_MCLK2_EN_V                                 0x08
#define ADIECTL_PIN_CTL_OE2_CDC_MCLK2_DIS_V                                0x00  

#define ADIECTL_PIN_CTL_OE2_CDC_MCLK1_M                                    0x04
#define ADIECTL_PIN_CTL_OE2_CDC_MCLK1_EN_V                                 0x04
#define ADIECTL_PIN_CTL_OE2_CDC_MCLK1_DIS_V                                0x00  

#define ADIECTL_PIN_CTL_OE2_TSADC_EOC_M                                    0x02
#define ADIECTL_PIN_CTL_OE2_TSADC_EOC_EN_V                                 0x02
#define ADIECTL_PIN_CTL_OE2_TSADC_EOC_DIS_V                                0x00  

#define ADIECTL_PIN_CTL_OE2_TSADC_SSBI_M                                   0x01
#define ADIECTL_PIN_CTL_OE2_TSADC_SSBI_EN_V                                0x01
#define ADIECTL_PIN_CTL_OE2_TSADC_SSBI_DIS_V                               0x00  

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x0C	    PIN_CTL_OE3

This register enables output for the listed pads
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_OE3_R            0x0C
#define ADIECTL_PIN_CTL_OE3_M            0x3F
 

#define ADIECTL_PIN_CTL_OE3_FM_I2S_SCK_M                              0x20
#define ADIECTL_PIN_CTL_OE3_FM_I2S_SCK_EN_V                           0x20
#define ADIECTL_PIN_CTL_OE3_FM_I2S_SCK_DIS_V                          0x00  

#define ADIECTL_PIN_CTL_OE3_FM_I2S_WS_M                               0x10
#define ADIECTL_PIN_CTL_OE3_FM_I2S_WS_EN_V                            0x10
#define ADIECTL_PIN_CTL_OE3_FM_I2S_WS_DIS_V                           0x00  

#define ADIECTL_PIN_CTL_OE3_FM_I2S_SD_M                               0x08
#define ADIECTL_PIN_CTL_OE3_FM_I2S_SD_EN_V                             0x08
#define ADIECTL_PIN_CTL_OE3_FM_I2S_SD_DIS_V                            0x00  
  
#define ADIECTL_PIN_CTL_OE3_FM_RDS_INT_M                               0x04
#define ADIECTL_PIN_CTL_OE3_FM_RDS_INT_EN_V                            0x04
#define ADIECTL_PIN_CTL_OE3_FM_RDS_INT_DIS_V                           0x00  

#define ADIECTL_PIN_CTL_OE3_CDC_RX_I2S_SCK_M                           0x02
#define ADIECTL_PIN_CTL_OE3_CDC_RX_I2S_SCK_EN_V                        0x02
#define ADIECTL_PIN_CTL_OE3_CDC_RX_I2S_SCK_DIS_V                       0x00  

#define ADIECTL_PIN_CTL_OE3_CDC_RX_I2S_WS_M                            0x01
#define ADIECTL_PIN_CTL_OE3_CDC_RX_I2S_WS_EN_V                         0x01
#define ADIECTL_PIN_CTL_OE3_CDC_RX_I2S_WS_DIS_V                        0x00  

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x0D	    PIN_CTL_DATA0

This register reads/sets data for pads listed in corresponding enable registers (above)
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_DATA0_R            0x0D
#define ADIECTL_PIN_CTL_DATA0_M            0xFF

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x0E	    PIN_CTL_DATA1

This register reads/sets data for pads listed in corresponding enable registers (above)
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_DATA1_R            0x0E
#define ADIECTL_PIN_CTL_DATA1_M            0xFF

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x0F	    PIN_CTL_DATA2

This register reads/sets data for pads listed in corresponding enable registers (above)
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_DATA2_R            0x0F
#define ADIECTL_PIN_CTL_DATA2_M            0xFF

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x10	    PIN_CTL_DATA3

This register reads/sets data for pads listed in corresponding enable registers (above)
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_PIN_CTL_DATA3_R            0x10
#define ADIECTL_PIN_CTL_DATA3_M            0x3F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x11	    VERSION

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_VERSION_R                  0x11
#define ADIECTL_VERSION_M                  0x7F

/* LSB's of Marimba Slave ID (i2c) */
#define ADIECTL_VERSION_SLAVE_ID_M                                   0x60

/* Marimba Version */
#define ADIECTL_VERSION_VERSION_M                                    0x1F




/********************************************************************
                          *************
                             T L M M
                          *************
*************************************************************************/

/************************************************************************
The following registers are for TLMM configuration. The bit values for 
the registers are not listed here at thist time since it is expected
that these registers will only be used for debugging. Please refer to 
SWI for more information on bit values. If needed we can add the bit 
values here.
*********************************************************************/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x80	    BT_CLK_PWR_REQ_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_CLK_PWR_REQ_MODE_R                  0x80
#define ADIECTL_BT_CLK_PWR_REQ_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x81	    BT_COEX_0_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_COEX_0_MODE_R                        0x81
#define ADIECTL_BT_COEX_0_MODE_M                        0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x82	    BT_COEX_1_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_COEX_1_MODE_R                  0x82
#define ADIECTL_BT_COEX_1_MODE_M                  0x1F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x83	    BT_COEX_2_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_COEX_2_MODE_R                  0x83
#define ADIECTL_BT_COEX_2_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x84	    BT_EXT_WAKE_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_EXT_WAKE_MODE_R                  0x84
#define ADIECTL_BT_EXT_WAKE_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x85	    BT_HOST_WAKE_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_HOST_WAKE_MODE_R                  0x85
#define ADIECTL_BT_HOST_WAKE_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x86	    BT_HCI_3_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_HCI_3_MODE_R                  0x86
#define ADIECTL_BT_HCI_3_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x87	    BT_HCI_2_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_HCI_2_MODE_R                  0x87
#define ADIECTL_BT_HCI_2_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x88	    BT_HCI_1_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_HCI_1_MODE_R                  0x88
#define ADIECTL_BT_HCI_1_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x89	    BT_HCI_O_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_HCI_O_MODE_R                  0x89
#define ADIECTL_BT_HCI_O_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x8A	    BT_PCM_BCLK_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_PCM_BCLK_MODE_R                  0x8A
#define ADIECTL_BT_PCM_BCLK_MODE_M                  0x1F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x8B	    BT_PCM_DIN_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_PCM_DIN_MODE_R                  0x8B
#define ADIECTL_BT_PCM_DIN_MODE_M                  0x1F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x8C	    BT_PCM_DOUT_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_PCM_DOUT_MODE_R                   0x8C
#define ADIECTL_BT_PCM_DOUT_MODE_M                  0x1F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x8D	    BT_PCM_SYNC_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_PCM_SYNC_MODE_R                  0x8D
#define ADIECTL_BT_PCM_SYNC_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x8E	    BT_SLEEP_CLK_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_SLEEP_CLK_MODE_R                  0x8E
#define ADIECTL_BT_SLEEP_CLK_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x8F	    TSADC_PENIRQ_N_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_TSADC_PENIRQ_N_MODE_R                  0x8F
#define ADIECTL_TSADC_PENIRQ_N_MODE_M                  0x1F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x90	    TSADC_SSBI_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_TSADC_SSBI_MODE_R                  0x90
#define ADIECTL_TSADC_SSBI_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x91	    TSADC_EOC_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_TSADC_EOC_MODE_R                  0x91
#define ADIECTL_TSADC_EOC_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x92	    CDC_MCLK1_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_MCLK1_MODE_R                  0x92
#define ADIECTL_CDC_MCLK1_MODE_M                  0x1F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x93	    CDC_MCLK2_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_MCLK2_MODE_R                  0x93
#define ADIECTL_CDC_MCLK2_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x94	    CDC_TX_I2S_SD_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_TX_I2S_SD_MODE_R                  0x94
#define ADIECTL_CDC_TX_I2S_SD_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x95	    CDC_TX_I2S_WS_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_TX_I2S_WS_MODE_R                  0x95
#define ADIECTL_CDC_TX_I2S_WS_MODE_M                  0x1F
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x96	    CDC_TX_I2S_SCK_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_TX_I2S_SCK_MODE_R                  0x96
#define ADIECTL_CDC_TX_I2S_SCK_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x97	    CDC_RX_I2S_SD_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_RX_I2S_SD_MODE_R                  0x97
#define ADIECTL_CDC_RX_I2S_SD_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x98	    CDC_RX_I2S_WS_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_RX_I2S_WS_MODE_R                  0x98
#define ADIECTL_CDC_RX_I2S_WS_MODE_M                  0x1F
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x99	    CDC_RX_I2S_SCK_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_CDC_RX_I2S_SCK_MODE_R                  0x99
#define ADIECTL_CDC_RX_I2S_SCK_MODE_M                  0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x9A	    FM_RDS_INT_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_FM_RDS_INT_MODE_R                      0x9A
#define ADIECTL_FM_RDS_INT_MODE_M                      0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x9B	    FM_I2S_SD_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_FM_I2S_SD_MODE_R                       0x9B
#define ADIECTL_FM_I2S_SD_MODE_M                       0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x9C	    FM_I2S_WS_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_FM_I2S_WS_MODE_R                       0x9C
#define ADIECTL_FM_I2S_WS_MODE_M                       0x1F

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0x9D	    FM_I2S_SCK_MODE

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_FM_I2S_SCK_MODE_R                  0x9D
#define ADIECTL_FM_I2S_SCK_MODE_M                  0x1F




/*****************************************************************
                 *****************************
                   BT Configuration Registers 
                 ****************************
*******************************************************************/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE0	    BT_FM_PLL_CTL0

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_FM_PLL_CTL0_R                  0xE0
#define ADIECTL_BT_FM_PLL_CTL0_M                  0x0F

/*CGPLL fm override controll */
#define ADIECTL_BT_FM_PLL_CTL0_PLL_OVERRIDE_M                          0x08
#define ADIECTL_BT_FM_PLL_CTL0_PLL_OVERRIDE_ON_V                       0x08
#define ADIECTL_BT_FM_PLL_CTL0_PLL_OVERRIDE_OFF_V                      0x00

/* cgpll pre/post settings */
#define ADIECTL_BT_FM_PLL_CTL0_PLL_CONFIG_19_17_M                      0x07


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE1	    BT_FM_PLL_CTL1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_FM_PLL_CTL1_R                  0xE1
#define ADIECTL_BT_FM_PLL_CTL1_M                  0x3F

#define ADIECTL_BT_FM_PLL_CTL1_PLL_L_VAL_M                            0x3F


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE2	    BT_FM_PLL_CTL2

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_FM_PLL_CTL2_R                  0xE2
#define ADIECTL_BT_FM_PLL_CTL2_M                  0x01

#define ADIECTL_BT_FM_PLL_CTL1_PLL_RESET_N_M                         0x01

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE3	    BT_COEX_CTL0

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_COEX_CTL0_R                  0xE3
#define ADIECTL_BT_COEX_CTL0_M                  0x7f

#define ADIECTL_BT_COEX_CTL0_OUT_ENA_M                               0x70

#define ADIECTL_BT_COEX_CTL0_TX_CONFX_M                              0x0C
#define ADIECTL_BT_COEX_CTL0_TX_CONFX_COEX2_V                        0x08
#define ADIECTL_BT_COEX_CTL0_TX_CONFX_COEX1_V                        0x04
#define ADIECTL_BT_COEX_CTL0_TX_CONFX_COEX0_V                        0x00

#define ADIECTL_BT_COEX_CTL0_CHAN_DATA_M                              0x03
#define ADIECTL_BT_COEX_CTL0_CHAN_DATA_COEX2_V                        0x02
#define ADIECTL_BT_COEX_CTL0_CHAN_DATA_COEX1_V                        0x01
#define ADIECTL_BT_COEX_CTL0_CHAN_DATA_COEX0_V                        0x00

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE4	    BT_COEX_CTL1

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_COEX_CTL1_R                  0xE4
#define ADIECTL_BT_COEX_CTL1_M                  0x3F

#define ADIECTL_BT_COEX_CTL1_COEX2_OUT_M                               0x30
#define ADIECTL_BT_COEX_CTL1_COEX2_OUT_BT_PRIORITY_V                   0x30
#define ADIECTL_BT_COEX_CTL1_COEX2_OUT_RF_ACTIVE_V                     0x20
#define ADIECTL_BT_COEX_CTL1_COEX2_OUT_STATUS_v                        0x10
#define ADIECTL_BT_COEX_CTL1_COEX2_OUT_FREQ_V                          0x00

#define ADIECTL_BT_COEX_CTL1_COEX1_OUT_M                               0x0C
#define ADIECTL_BT_COEX_CTL1_COEX1_OUT_BT_PRIORITY_V                   0x0C
#define ADIECTL_BT_COEX_CTL1_COEX1_OUT_RF_ACTIVE_V                     0x08
#define ADIECTL_BT_COEX_CTL1_COEX1_OUT_STATUS_v                        0x04
#define ADIECTL_BT_COEX_CTL1_COEX1_OUT_FREQ_V                          0x00

#define ADIECTL_BT_COEX_CTL1_COEX0_OUT_M                               0x03
#define ADIECTL_BT_COEX_CTL1_COEX0_OUT_BT_PRIORITY_V                   0x03
#define ADIECTL_BT_COEX_CTL1_COEX0_OUT_RF_ACTIVE_V                     0x02
#define ADIECTL_BT_COEX_CTL1_COEX0_OUT_STATUS_v                        0x01
#define ADIECTL_BT_COEX_CTL1_COEX0_OUT_FREQ_V                          0x00

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE5	    BT_BOOT_CONFIG

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_BOOT_CONFIG_R                  0xE5
#define ADIECTL_BT_BOOT_CONFIG_M                  0xFF

/* Bypass CGPLL selection  */
#define ADIECTL_BT_BOOT_CONFIG_BT_BYPASS_PLL_M                           0x80
#define ADIECTL_BT_BOOT_CONFIG_BT_BYPASS_PLL_ON_V                        0x80
#define ADIECTL_BT_BOOT_CONFIG_BT_BYPASS_PLL_OFF_V                       0x00

/* Enable baseband interference */ 
#define ADIECTL_BT_BOOT_CONFIG_BBIF_ENABLE_M                             0x40
#define ADIECTL_BT_BOOT_CONFIG_BBIF_ENABLE_ON_V                          0x40
#define ADIECTL_BT_BOOT_CONFIG_BBIF_ENABLE_OFF_V                         0x00

/* CGPLL Output Enable  */
#define ADIECTL_BT_BOOT_CONFIG_BT_TEST_CGPLL_OE_M                        0x20
#define ADIECTL_BT_BOOT_CONFIG_BT_TEST_CGPLL_OE_ON_V                     0x20
#define ADIECTL_BT_BOOT_CONFIG_BT_TEST_CGPLL_OE_OFF_V                    0x00

/* Emulation Clock Select   */ 
#define ADIECTL_BT_BOOT_CONFIG_BT_EMUL_CLK_SEL_M                         0x10
#define ADIECTL_BT_BOOT_CONFIG_BT_EMUL_CLK_SEL_0N_V                      0x10
#define ADIECTL_BT_BOOT_CONFIG_BT_EMUL_CLK_SEL_OFF_V                     0x00

#define ADIECTL_BT_BOOT_CONFIG_CONFIG_M                                  0x0F
		
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE6	    BT_OVERRIDE

Leakage Controll Register
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_OVERRIDE_R                 0xE6
#define ADIECTL_BT_OVERRIDE_M                 0xDE

/* TODO add bits for this register. SWI seems to have incorrect 
   information. 
*/ 

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xE7	    BT_CTL0


*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_BT_CTL0_R                     0xE7
#define ADIECTL_BT_CTL0_M                     0xFF

#define ADIECTL_BT_CTL0_RAM_SLEEP_N_M                                 0x80
#define ADIECTL_BT_CTL0_RAM_SLEEP_N_ON                                0x80
#define ADIECTL_BT_CTL0_RAM_SLEEP_N_OFF                               0x00

#define ADIECTL_BT_CTL0_RAM_EN_FEW_N_M                                0x40
#define ADIECTL_BT_CTL0_RAM_EN_FEW_N_ON_V                             0x40
#define ADIECTL_BT_CTL0_RAM_EN_FEW_N_OFF_V                            0x00

#define ADIECTL_BT_CTL0_CLK_ENA_M                                     0x20
#define ADIECTL_BT_CTL0_CLK_ENA_ON_V                                  0x20
#define ADIECTL_BT_CTL0_CLK_ENA_OFF_V                                 0x00

#define ADIECTL_BT_CTL0_ANA_VTRANS_EN_M                               0x10
#define ADIECTL_BT_CTL0_ANA_VTRANS_EN_ON_V                            0x10
#define ADIECTL_BT_CTL0_ANA_VTRANS_EN_OFF_V                           0x00

#define ADIECTL_BT_CTL0_GDFS_CLAMP_EN_M                               0x08
#define ADIECTL_BT_CTL0_GDFS_CLAMP_EN_ON_V                            0x08
#define ADIECTL_BT_CTL0_GDFS_CLAMP_EN_OFF_V                           0x00

#define ADIECTL_BT_CTL0_GDFS_EN_REST_M                                0x04
#define ADIECTL_BT_CTL0_GDFS_EN_REST_M_ON_V                           0x04
#define ADIECTL_BT_CTL0_GDFS_EN_REST_M_OFF_V                          0x00

#define ADIECTL_BT_CTL0_GDFS_EN_FEW_M                                 0x02
#define ADIECTL_BT_CTL0_GDFS_EN_FEW_M_ON_V                            0x02
#define ADIECTL_BT_CTL0_GDFS_EN_FEW_M_OFF_V                           0x00

#define ADIECTL_BT_CTL0_RESET_M                                       0x01
#define ADIECTL_BT_CTL0_RESET_ON_V                                    0x00
#define ADIECTL_BT_CTL0_RESET_OFF_V                                   0x01

/*****************************************************************
                 *****************************
                  Touch Screen Controller
                 ****************************
*******************************************************************/


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
Address   Register
0xFF	    TS_CTL0

*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define ADIECTL_TS_CTL0_R                  0xFF
#define ADIECTL_TS_CTL0_M                  0x1F

#define ADIECTL_TS_CTL0_PENIRQ_N_OE_M                                 0x10
#define ADIECTL_TS_CTL0_PENIRQ_N_OE_ON_V                              0x10
#define ADIECTL_TS_CTL0_PENIRQ_N_OE_OFF_V                             0x00

#define ADIECTL_TS_CTL0_EOC_OE_M                                      0x08
#define ADIECTL_TS_CTL0_EOC_OE_ON_V                                   0x08
#define ADIECTL_TS_CTL0_EOC_OE_OFF_V                                  0x00

#define ADIECTL_TS_CTL0_XO_ENA_M                                      0x04
#define ADIECTL_TS_CTL0_XO_ENA_ON_V                                   0x04
#define ADIECTL_TS_CTL0_XO_ENA_OFF_V                                  0x00

#define ADIECTL_TS_CTL0_CLK_ENA_M                                     0x02
#define ADIECTL_TS_CTL0_CLK_ENA_ON_V                                  0x02
#define ADIECTL_TS_CTL0_CLK_ENA_OFF_V                                 0x00

#define ADIECTL_TS_CTL0_RESET_M                                       0x01
#define ADIECTL_TS_CTL0_RESET_OFF_V                                   0x01
#define ADIECTL_TS_CTL0_RESET_ON_V                                    0x00


#endif /* AdieCtlRegDefs_H */
