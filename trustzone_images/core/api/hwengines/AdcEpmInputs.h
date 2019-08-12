#ifndef ADC_EPM_INPUTS_H
#define ADC_EPM_INPUTS_H
/*============================================================================
  @file AdcInputs.h

  Analog-to-Digital Converter driver EPM ADC input names.

  This header provides macros to provide common names to access EPM ADC input
  properties.

               Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/AdcEpmInputs.h#1 $ */

/** I_EPM_VBATT_DEBUG - nPhysical units are in microamps */
#define ADC_INPUT_I_EPM_VBATT_DEBUG    "I_EPM_VBATT_DEBUG"

/** I_EPM - nPhysical units are in microamps */
#define ADC_INPUT_I_EPM                "I_EPM"

/** VDDMX_1 - nPhysical units are in microamps */
#define ADC_INPUT_VDDMX_1              "VDDMX_1"

/** VDDCX_1 - nPhysical units are in microamps */
#define ADC_INPUT_VDDCX_1              "VDDCX_1"

/** APC0 - nPhysical units are in microamps */
#define ADC_INPUT_APC0                 "APC0"

/** APC1 - nPhysical units are in microamps */
#define ADC_INPUT_APC1                 "APC1"

/** I_APC0 - nPhysical units are in microamps */
#define ADC_INPUT_I_APC0               "I_APC0"

/** I_APC1 - nPhysical units are in microamps */
#define ADC_INPUT_I_APC1               "I_APC1"

/** I_VDDPX_1 - nPhysical units are in microamps */
#define ADC_INPUT_I_VDDPX_1            "I_VDDPX_1"

/** I_VDDPX_3 - nPhysical units are in microamps */
#define ADC_INPUT_I_VDDPX_3            "I_VDDPX_3"

/** I_1VDD2 - nPhysical units are in microamps */
#define ADC_INPUT_I_1VDD2              "I_1VDD2"

/** I_VDD1 - nPhysical units are in microamps */
#define ADC_INPUT_I_VDD1               "I_VDD1"

/** I_USB_MSM_IO - nPhysical units are in microamps */
#define ADC_INPUT_I_USB_MSM_IO         "I_USB_MSM_IO"

/** I_1P2V_VDIG - nPhysical units are in microamps */
#define ADC_INPUT_I_1P2V_VDIG          "I_1P2V_VDIG"

/** I_MIPI_VGA_CAMERA - nPhysical units are in microamps */
#define ADC_INPUT_I_MIPI_VGA_CAMERA    "I_MIPI_VGA_CAMERA"

/** ADSP - nPhysical units are in microamps */
#define ADC_INPUT_ADSP                 "ADSP"

/** I_ADSP - nPhysical units are in microamps */
#define ADC_INPUT_I_ADSP               "I_ADSP"

/** I_TABLA_VDD_CDC_D - nPhysical units are in microamps */
#define ADC_INPUT_I_TABLA_VDD_CDC_D    "I_TABLA_VDD_CDC_D"

/** I_TABLA_VDDA_CP - nPhysical units are in microamps */
#define ADC_INPUT_I_TABLA_VDDA_CP      "I_TABLA_VDDA_CP"

/** I_TABLA_VDDA_TXRX - nPhysical units are in microamps */
#define ADC_INPUT_I_TABLA_VDDA_TXRX    "I_TABLA_VDDA_TXRX"

/** I_EMMC_VDDQ - nPhysical units are in microamps */
#define ADC_INPUT_I_EMMC_VDDQ          "I_EMMC_VDDQ"

/** I_SD_MMC_VDD - nPhysical units are in microamps */
#define ADC_INPUT_I_SD_MMC_VDD         "I_SD_MMC_VDD"

/** I_EMMC_VCC - nPhysical units are in microamps */
#define ADC_INPUT_I_EMMC_VCC           "I_EMMC_VCC"

/** I_HDMI_AVDD - nPhysical units are in microamps */
#define ADC_INPUT_I_HDMI_AVDD          "I_HDMI_AVDD"

/** I_VDDA_MIPI_DSI - nPhysical units are in microamps */
#define ADC_INPUT_I_VDDA_MIPI_DSI      "I_VDDA_MIPI_DSI"

/** I_LCD1_MIPI - nPhysical units are in microamps */
#define ADC_INPUT_I_LCD1_MIPI          "I_LCD1_MIPI"

/** I_SENSOR_TS - nPhysical units are in microamps */
#define ADC_INPUT_I_SENSOR_TS    "I_SENSOR_TS"

/** I_TS_VCPIN_SENSOR - nPhysical units are in microamps */
#define ADC_INPUT_I_TS_VCPIN_SENSOR    "I_TS_VCPIN_SENSOR"

/** I_TS_VDD_SENSOR - nPhysical units are in microamps */
#define ADC_INPUT_I_TS_VDD_SENSOR      "I_TS_VDD_SENSOR"

/** I_CAMERA_CORE - nPhysical units are in microamps */
#define ADC_INPUT_I_CAMERA_CORE        "I_CAMERA_CORE"

/** I_GPS - nPhysical units are in microamps */
#define ADC_INPUT_I_GPS                "I_GPS"

/** I_UIM1 - nPhysical units are in microamps */
#define ADC_INPUT_I_UIM1               "I_UIM1"

/** I_GSM_PA - nPhysical units are in microamps */
#define ADC_INPUT_I_GSM_PA             "I_GSM_PA"

/** I_CDMA_PA - nPhysical units are in microamps */
#define ADC_INPUT_I_CDMA_PA            "I_CDMA_PA"

/** I_WCDMA_PA - nPhysical units are in microamps */
#define ADC_INPUT_I_WCDMA_PA           "I_WCDMA_PA"

/** QDSP6_SW - nPhysical units are in microamps */
#define ADC_INPUT_QDSP6_SW             "QDSP6_SW"

/** QDSP6_FW - nPhysical units are in microamps */
#define ADC_INPUT_QDSP6_FW             "QDSP6_FW"

/** I_QDSP6_SW - nPhysical units are in microamps */
#define ADC_INPUT_I_QDSP6_SW           "I_QDSP6_SW"

/** I_QDSP6_FW - nPhysical units are in microamps */
#define ADC_INPUT_I_QDSP6_FW           "I_QDSP6_FW"

/** I_BASEBAND_RX - nPhysical units are in microamps */
#define ADC_INPUT_I_BASEBAND_RX        "I_BASEBAND_RX"

/** I_VDDA_COMBODAC - nPhysical units are in microamps */
#define ADC_INPUT_I_VDDA_COMBODAC      "I_VDDA_COMBODAC"

/** I_RTR0_1P3V - nPhysical units are in microamps */
#define ADC_INPUT_I_RTR0_1P3V          "I_RTR0_1P3V"

/** I_RTR0_2P2V - nPhysical units are in microamps */
#define ADC_INPUT_I_RTR0_2P2V          "I_RTR0_2P2V"

/** I_SVLTE_1P3V - nPhysical units are in microamps */
#define ADC_INPUT_I_SVLTE_1P3V         "I_SVLTE_1P3V"

/** I_SVLTE_2P2V - nPhysical units are in microamps */
#define ADC_INPUT_I_SVLTE_2P2V         "I_SVLTE_2P2V"

/** I_WCN3660_1P3V - nPhysical units are in microamps */
#define ADC_INPUT_I_WCN3660_1P3V       "I_WCN3660_1P3V"

/** I_WCN3660_2P9V - nPhysical units are in microamps */
#define ADC_INPUT_I_WCN3660_2P9V       "I_WCN3660_2P9V"

/** I_VREG_S1 - nPhysical units are in microamps */
#define ADC_INPUT_I_VREG_S1            "I_VREG_S1"

/** I_VREG_S2 - nPhysical units are in microamps */
#define ADC_INPUT_I_VREG_S2            "I_VREG_S2"

/** I_VREG_S3 - nPhysical units are in microamps */
#define ADC_INPUT_I_VREG_S3            "I_VREG_S3"

/** I_VREG_S4 - nPhysical units are in microamps */
#define ADC_INPUT_I_VREG_S4            "I_VREG_S4"

/** I_VREG_S8 - nPhysical units are in microamps */
#define ADC_INPUT_I_VREG_S8            "I_VREG_S8"

/** I_ALL - nPhysical units are in microamps */
#define ADC_INPUT_I_ALL                "I_ALL"

/** I_CDC_RX_TX - nPhysical units are in microamps */
#define ADC_INPUT_I_CDC_RX_TX          "I_CDC_RX_TX"

/** I_3W_CLASSD_AMP - nPhysical units are in microamps */
#define ADC_INPUT_I_3W_CLASSD_AMP  "I_3W_CLASSD_AMP"

/** I_AMBIENT_LIGHT - nPhysical units are in microamps */
#define ADC_INPUT_I_AMBIENT_LIGHT  "I_AMBIENT_LIGHT"

/** I_AMLOED_ELVDD - nPhysical units are in microamps  */
#define ADC_INPUT_I_AMLOED_ELVDD   "I_AMLOED_ELVDD"

/** I_AMLOED_IO - nPhysical units are in microamps     */
#define ADC_INPUT_I_AMLOED_IO      "I_AMLOED_IO"

/** I_AMLOED_MEMORY - nPhysical units are in microamps */
#define ADC_INPUT_I_AMLOED_MEMORY  "I_AMLOED_MEMORY"

/** I_ARM11 - nPhysical units are in microamps         */
#define ADC_INPUT_I_ARM11          "I_ARM11"

/** I_AUDIO_DSP - nPhysical units are in microamps     */
#define ADC_INPUT_I_AUDIO_DSP      "I_AUDIO_DSP"

/** I_BACKLIGHT - nPhysical units are in microamps     */
#define ADC_INPUT_I_BACKLIGHT      "I_BACKLIGHT"

/** I_BATTERY - nPhysical units are in microamps       */
#define ADC_INPUT_I_BATTERY        "I_BATTERY"

/** I_BT - nPhysical units are in microamps            */
#define ADC_INPUT_I_BT             "I_BT"

/** I_CAMERA_ANALOG - nPhysical units are in microamps */
#define ADC_INPUT_I_CAMERA_ANALOG  "I_CAMERA_ANALOG"

/** I_CAMERA_CSI_IF - nPhysical units are in microamps */
#define ADC_INPUT_I_CAMERA_CSI_IF  "I_CAMERA_CSI_IF"

/** I_CAMERA_DIGITAL - nPhysical units are in microamps*/
#define ADC_INPUT_I_CAMERA_DIGITAL "I_CAMERA_DIGITAL"

/** I_CAMERA_IO - nPhysical units are in microamps     */
#define ADC_INPUT_I_CAMERA_IO      "I_CAMERA_IO"

/** I_CODEC_ANALOG - nPhysical units are in microamps  */
#define ADC_INPUT_I_CODEC_ANALOG   "I_CODEC_ANALOG"

/** I_CODEC_IO - nPhysical units are in microamps      */
#define ADC_INPUT_I_CODEC_IO       "I_CODEC_IO"

/** I_CODEC_VDDCX_1 - nPhysical units are in microamps */
#define ADC_INPUT_I_CODEC_VDDCX_1  "I_CODEC_VDDCX_1"

/** I_DDR2_CORE1 - nPhysical units are in microamps    */
#define ADC_INPUT_I_DDR2_CORE1     "I_DDR2_CORE1"

/** I_DDR2_CORE2 - nPhysical units are in microamps    */
#define ADC_INPUT_I_DDR2_CORE2     "I_DDR2_CORE2"

/** I_DDR2_IO - nPhysical units are in microamps       */
#define ADC_INPUT_I_DDR2_IO        "I_DDR2_IO"

/** I_DIGITAL_CORE - nPhysical units are in microamps  */
#define ADC_INPUT_I_DIGITAL_CORE   "I_DIGITAL_CORE"

/** I_DRAM_VDD1 - nPhysical units are in microamps     */
#define ADC_INPUT_I_DRAM_VDD1      "I_DRAM_VDD1"

/** I_DRAM_VDD2 - nPhysical units are in microamps     */
#define ADC_INPUT_I_DRAM_VDD2      "I_DRAM_VDD2"

/** I_EBI - nPhysical units are in microamps           */
#define ADC_INPUT_I_EBI            "I_EBI"

/** I_EMMC_NAND - nPhysical units are in microamps     */
#define ADC_INPUT_I_EMMC_NAND      "I_EMMC_NAND"

/** I_EMMC_NAND_VCC - nPhysical units are in microamps */
#define ADC_INPUT_I_EMMC_NAND_VCC  "I_EMMC_NAND_VCC"

/** I_EMMC_VCC - nPhysical units are in microamps      */
#define ADC_INPUT_I_EMMC_VCC       "I_EMMC_VCC"

/** I_EMMC_VCCQ - nPhysical units are in microamps     */
#define ADC_INPUT_I_EMMC_VCCQ      "I_EMMC_VCCQ"

/** I_GPS_ADC - nPhysical units are in microamps       */
#define ADC_INPUT_I_GPS_ADC        "I_GPS_ADC"

/** I_HAPTICS - nPhysical units are in microamps       */
#define ADC_INPUT_I_HAPTICS        "I_HAPTICS"

/** I_HDMI - nPhysical units are in microamps          */
#define ADC_INPUT_I_HDMI           "I_HDMI"

/** I_HDMI_5V - nPhysical units are in microamps       */
#define ADC_INPUT_I_HDMI_5V        "I_HDMI_5V"

/** I_IMEM - nPhysical units are in microamps          */
#define ADC_INPUT_I_IMEM           "I_IMEM"

/** I_IO_PAD2 - nPhysical units are in microamps       */
#define ADC_INPUT_I_IO_PAD2        "I_IO_PAD2"

/** I_ISM_VDD2 - nPhysical units are in microamps      */
#define ADC_INPUT_I_ISM_VDD2       "I_MSM_ISM_VDD2"

/** I_LCD - nPhysical units are in microamps           */
#define ADC_INPUT_I_LCD            "I_LCD"

/** I_LCD_BACKLIGHT - nPhysical units are in microamps */
#define ADC_INPUT_I_LCD_BACKLIGHT  "I_LCD_BACKLIGHT"

/** I_LPDDR2_CORE - nPhysical units are in microamps   */
#define ADC_INPUT_I_LPDDR2_CORE    "I_LPDDR2_CORE"

/** I_LPDDR2_VDD1 - nPhysical units are in microamps   */
#define ADC_INPUT_I_LPDDR2_VDD1    "I_LPDDR2-1_VDD1"

/** I_MAIN_POWER - nPhysical units are in microamps    */
#define ADC_INPUT_I_MAIN_POWER     "I_MAIN_POWER"

/** I_MARIMBA_1P2V - nPhysical units are in microamps  */
#define ADC_INPUT_I_MARIMBA_1P2V   "I_MARIMBA_1P2V"

/** I_MARIMBA_1P8V - nPhysical units are in microamps  */
#define ADC_INPUT_I_MARIMBA_1P8V   "I_MARIMBA_1P8V"

/** I_MARIMBA_2P2V - nPhysical units are in microamps  */
#define ADC_INPUT_I_MARIMBA_2P2V   "I_MARIMBA_2P2V"

/** I_MICRO_SD - nPhysical units are in microamps      */
#define ADC_INPUT_I_MICRO_SD       "I_MICRO_SD"

/** I_MIPI_DSI - nPhysical units are in microamps      */
#define ADC_INPUT_I_MIPI_DSI       "I_MIPI_DSI"

/** I_MIPI_LDO15 - nPhysical units are in microamps    */
#define ADC_INPUT_I_MIPI_LDO15     "I_MIPI_LDO15"

/** I_MIPI_VDIG - nPhysical units are in microamps     */
#define ADC_INPUT_I_MIPI_VDIG      "I_MIPI_VDIG"

/** I_MIPI_VIO_VDD - nPhysical units are in microamps  */
#define ADC_INPUT_I_MIPI_VIO_VDD   "I_MIPI_VIO_VDD"

/** I_MSM_PAD - nPhysical units are in microamps       */
#define ADC_INPUT_I_MSM_PAD        "I_MSM_PAD"

/** I_MSM_IO_PAD3 - nPhysical units are in microamps   */
#define ADC_INPUT_I_MSM_IO_PAD3    "I_MSM_IO_PAD3"

/** I_MSM_ISM_VDD1 - nPhysical units are in microamps */
#define ADC_INPUT_I_ISM_VDD1       "I_MSM_ISM_VDD1"

/** I_MSM-VDD_PXO - nPhysical units are in microamps   */
#define ADC_INPUT_I_PXO            "I_MSM-VDD_PXO"

/** I_NAND - nPhysical units are in microamps          */
#define ADC_INPUT_I_NAND           "I_NAND"

/** I_OPTICAL_MOUSE - nPhysical units are in microamps */
#define ADC_INPUT_I_OPTICAL_MOUSE  "I_OPTICAL_MOUSE"

/** I_PAD3 - nPhysical units are in microamps          */
#define ADC_INPUT_I_PAD3           "I_PAD3"

/** I_PLL2 - nPhysical units are in microamps          */
#define ADC_INPUT_I_PLL2           "I_PLL2"

/** I_PLL8 - nPhysical units are in microamps          */
#define ADC_INPUT_I_PLL8           "I_PLL8"

/** I_PLL10 - nPhysical units are in microamps         */
#define ADC_INPUT_I_PLL10          "I_PLL10"

/** I_PLLS - nPhysical units are in microamps          */
#define ADC_INPUT_I_PLLS           "I_PLLS"

/** I_PRIMARY_LCD - nPhysical units are in microamps   */
#define ADC_INPUT_I_PRIMARY_LCD    "I_PRIMARY_LCD"

/** I_PRIMARY_LCD_VDD - nPhysical units are in microamps */
#define ADC_INPUT_I_PRIMARY_LCD_VDD "I_PRIMARY_LCD_VDD"

/** I_SCORPION - nPhysical units are in microamps      */
#define ADC_INPUT_I_SCORPION       "I_SCORPION"

/** I_SCORPION_CORE0 - nPhysical units are in microamps*/
#define ADC_INPUT_I_SCORPION_CORE0 "I_SCORPION_CORE0"

/** I_SCORPION_CORE1 - nPhysical units are in microamps*/
#define ADC_INPUT_I_SCORPION_CORE1 "I_SCORPION_CORE1"

/** I_SD_CARD - nPhysical units are in microamps       */
#define ADC_INPUT_I_SD_CARD        "I_SD_CARD"

/** I_SIM1 - nPhysical units are in microamps          */
#define ADC_INPUT_I_SIM1           "I_SIM1"

/** I_SIM2 - nPhysical units are in microamps          */
#define ADC_INPUT_I_SIM2           "I_SIM2"

/** I_SMI_PAD - nPhysical units are in microamps       */
#define ADC_INPUT_I_SMI_PAD        "I_SMI_PAD"

/** I_SMPS0B - nPhysical units are in microamps        */
#define ADC_INPUT_I_SMPS0B         "I_PM8901_SMPS0b"

/** I_SMPS1B - nPhysical units are in microamps        */
#define ADC_INPUT_I_SMPS1B         "I_PM8901_SMPS1b"

/** I_SMPS3B - nPhysical units are in microamps        */
#define ADC_INPUT_I_SMPS3B         "I_PM8901_SMPS3b"

/** I_SPEAKER_R-VDD - nPhysical units are in microamps */
#define ADC_INPUT_I_SPEAKER_RVDD   "I_SPEAKER_R-VDD"

/** I_TCODEC - nPhysical units are in microamps        */
#define ADC_INPUT_I_TCODEC         "I_TCODEC"

/** I_TOUCH_SCREEN - nPhysical units are in microamps  */
#define ADC_INPUT_I_TOUCH_SCREEN   "I_TOUCH_SCREEN"

/** I_UBM - nPhysical units are in microamps           */
#define ADC_INPUT_I_UBM            "I_UBM"

/** I_UBM2M_VDD - nPhysical units are in microamps     */
#define ADC_INPUT_I_UBM2M_VDD      "I_UBM2M_VDD"

/** I_UBM2M_VDD_AL - nPhysical units are in microamps  */
#define ADC_INPUT_I_UBM2M_VDD_AL   "I_UBM2M_VDD_AL"

/** I_UBM2M_VDD_FURY - nPhysical units are in microamps*/
#define ADC_INPUT_I_UBM2M_VDD_FURY "I_UBM2M_VDD_FURY"

/** I_UBM2M_VDD_2A - nPhysical units are in microamps  */
#define ADC_INPUT_I_UBM2M_VDD_2A   "I_UBM2M_VDD_2A"

/** I_VBATT - nPhysical units are in microamps         */
#define ADC_INPUT_I_VBATT          "I_VBATT"

/** I_VBATT_EPM - nPhysical units are in microamps     */
#define ADC_INPUT_I_VBATT_EPM      "I_VBATT_EPM"

/** I_VDD_DIG - nPhysical units are in microamps       */
#define ADC_INPUT_I_VDD_DIG        "I_VDD_DIG"

/** I_VDD_MEM - nPhysical units are in microamps       */
#define ADC_INPUT_I_VDD_MEM        "I_VDD_MEM"

/** I_VDDA_DAC - nPhysical units are in microamps      */
#define ADC_INPUT_I_VDDA_DAC       "I_VDDA_DAC"

/** I_VDDA_HDMI - nPhysical units are in microamps     */
#define ADC_INPUT_I_VDDA_HDMI      "I_VDDA_HDMI"

/** I_VDDCX_1 - nPhysical units are in microamps       */
#define ADC_INPUT_I_VDDCX_1        "I_VDDCX_1"

/** I_VDDMX_1 - nPhysical units are in microamps       */
#define ADC_INPUT_I_VDDMX_1        "I_VDDMX_1"

/** I_VDDPX1_LPDDR2 - nPhysical units are in microamps */
#define ADC_INPUT_I_VDDPX1_LPDDR2  "I_VDDPX1_LPDDR2"

/** I_VGREG_L13A - nPhysical units are in microamps    */
#define ADC_INPUT_I_VGREG_L13A     "I_VGREG_L13A"

/** I_VGREG_L21A - nPhysical units are in microamps    */
#define ADC_INPUT_I_VGREG_L21A     "I_VGREG_L21A"

/** I_VREG_L16A - nPhysical units are in microamps     */
#define ADC_INPUT_I_VREG_L16A      "I_VREG_L16A"

/** I_VDAC_AVDD - nPhysical units are in microamps     */
#define ADC_INPUT_I_VDAC_AVDD      "I_VDAC_AVDD"

/** I_VDAC_DVDD - nPhysical units are in microamps     */
#define ADC_INPUT_I_VDAC_DVDD      "I_VDAC_DVDD"

/** I_VEE - nPhysical units are in microamps           */
#define ADC_INPUT_I_VEE            "I_VEE"

/** I_VIDEO_AMP - nPhysical units are in microamps     */
#define ADC_INPUT_I_VIDEO_AMP      "I_VIDEO_AMP"

/** I_WLAN_VDD_1P3V - nPhysical units are in microamps */
#define ADC_INPUT_I_WLAN_VDD_1P3V  "I_WLAN_VDD_1P3V"

/** I_WLAN_VDD_1P8V - nPhysical units are in microamps */
#define ADC_INPUT_I_WLAN_VDD_1P8V  "I_WLAN_VDD_1P8V"

/** I_WLAN_VDD_2P2V - nPhysical units are in microamps */
#define ADC_INPUT_I_WLAN_VDD_2P2V  "I_WLAN_VDD_2P2V"

/** I_WLAN_VDD_2P5V - nPhysical units are in microamps */
#define ADC_INPUT_I_WLAN_VDD_2P5V  "I_WLAN_VDD_2P5V"

/** I_WLAN_VDD_2P9V - nPhysical units are in microamps */
#define ADC_INPUT_I_WLAN_VDD_2P9V  "I_WLAN_VDD_2P9V"

#endif
