#ifndef UIMDRV_CLK_H
#define UIMDRV_CLK_H
/*============================================================================
  FILE:          uimdrv_clk.h

  OVERVIEW:    File conatining the inclusions and definitions needed for clk interface.

  DEPENDENCIES: N/A

                Copyright (c) 2012-2013QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/clkregime/uimdrv_clk.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
01/09/14    akv        Handle to vote for mss bus freq made uim instance specific
10/08/13    nmb        HW header file cleanup
09/19/13    akv        Reference CLK changes to support 4.8MHz
08/06/13    rm/ak      Bolt RUMI code
07/12/13    nmb        Global Reorganization
02/22/13    js         8x26 bring up changes
02/15/13    ak         Replace MCPM calls with NPA calls to /clk/mss/config_bus
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "comdef.h"
#include "uim_v.h"
#include "npa.h"

#define UIMDRV_CLK_CONFIG_BUS_FREQ 72000 /* config bus frequency when activity is required */
#define UIMDRV_CLK_CONFIG_BUS_SLEEP_FREQ 19200 /* config bus frequency when bus is not needed */

#define UIM_TCXO_FREQ 19200000

/* This type defines the allowable UIM clock control.  The controls include
   leaving the clock signal HIGH, leaving it LOW and oscillating it at defined
   frequencies.
   Possible clock frequencies values are 3.84MHz and 4.8MHz
*/
typedef enum {
  UIMDRV_CLK_FREQ_3_84_MHZ, /* Program the clock at tcxo/5  */
  UIMDRV_CLK_FREQ_4_8_MHZ,  /* Program the clock at tcxo/4  */
  UIMDRV_CLK_STOP_HIGH,     /* Leave the clock signal HIGH  */
  UIMDRV_CLK_STOP_LOW       /* Leave the clock signal LOW   */
}uim_clock_control_type;

/* The UIM clock rate conversion factor is the index value received from the
   UIM card in TA(1) of the ATR or in the PPS.  This index refers to the clock
   rate conversion factor and the MAX clock rate for the UIM.  The clock rate
   conversion factor helps determine the baudrate of the UIM interface */
typedef enum {
  UIM_CRCF_372_0,         /* Conversion factor = 372   */
  UIM_CRCF_372_1,         /* Conversion factor = 372   */
  UIM_CRCF_558,           /* Conversion factor = 568   */
  UIM_CRCF_744,           /* Conversion factor = 744   */
  UIM_CRCF_1116,          /* Conversion factor = 1116  */
  UIM_CRCF_1488,          /* Conversion factor = 1488  */
  UIM_CRCF_1860,          /* Conversion factor = 1860  */
  UIM_CRCF_RFU1,          /* Reserved for future use   */
  UIM_CRCF_RFU2,          /* Reserved for future use   */
  UIM_CRCF_512,           /* Conversion factor = 512   */
  UIM_CRCF_768,           /* Conversion factor = 768   */
  UIM_CRCF_1024,          /* Conversion factor = 1024  */
  UIM_CRCF_1536,          /* Conversion factor = 1536  */
  UIM_CRCF_2048,          /* Conversion factor = 2048  */
  UIM_CRCF_RFU3,          /* Reserved for future use   */
  UIM_CRCF_RFU4,          /* Reserved for future use   */
  UIM_CRCF_SIZE           /* Number of constants of this type */
} uim_clock_rate_conversion_factor_type;

/* The UIM baud rate conversion factor is the index value received from the
   UIM card in TA(1) of the ATR or in the PPS.  This index refers to the baud
   rate adjustment factor for the UIM.  The baud rate adjustment factor helps
   determine the baudrate of the UIM interface */
typedef enum {
  UIM_BRAF_RFU1,          /* Reserved for future use  */
  UIM_BRAF_1,             /* Adjustment factor = 1    */
  UIM_BRAF_2,             /* Adjustment factor = 2    */
  UIM_BRAF_4,             /* Adjustment factor = 4    */
  UIM_BRAF_8,             /* Adjustment factor = 8    */
  UIM_BRAF_16,            /* Adjustment factor = 16   */
  UIM_BRAF_32,            /* Adjustment factor = 32   */
  UIM_BRAF_64,            /* Adjustment factor = 64   */
  UIM_BRAF_12,            /* Adjustment factor = 12   */
  UIM_BRAF_20,            /* Adjustment factor = 20   */
  UIM_BRAF_RFU3,          /* Reserved for future use  */
  UIM_BRAF_RFU4,          /* Reserved for future use  */
  UIM_BRAF_RFU5,          /* Reserved for future use  */
  UIM_BRAF_RFU6,          /* Reserved for future use  */
  UIM_BRAF_RFU7,          /* Reserved for future use  */
  UIM_BRAF_RFU8,          /* Reserved for future use  */
  UIM_BRAF_SIZE           /* Number of constants of this type */
} uim_baud_rate_adjustment_factor_type;

typedef enum{
  FI_0,
  FI_1,
  FI_2,
  FI_3,
  FI_4,
  FI_5,
  FI_6,
  FI_7,
  FI_8,
  FI_9,
  FI_10,
  FI_11,
  FI_12,
  FI_13,
  FI_14,
  FI_15
}uim_atr_FI_value_type;

extern const byte braf_values[UIM_BRAF_SIZE];
extern const dword crcf_values[UIM_CRCF_SIZE];
extern const dword uim_clock_frequency[2];

/**
  @brief This procedure determines if the platform can support any {FI, DI}
  combination.
  @param  uim_instance:- Current UIM task instance.
  @param  FI:- Clock rate conversion factor.
  @param  DI:- Baud rate adjustment factor.
  @return TRUE:- If FI/DI values are supported, FALSE - if FI/DI
          values are not supported.
*/
boolean is_fi_di_supported (uim_instance_enum_type                 uim_instance, 
                            uim_clock_rate_conversion_factor_type  FI,
                            uim_baud_rate_adjustment_factor_type   DI );

/*
  @brief  Initializes the clocks for the active slot.
  @param  uim_instance:- Current UIM task instance.
  @return None
*/
void initialize_clkregime(uim_instance_enum_type uim_instance);

/*
  @brief Configure the /clk/mss/config_bus NPA node frequency
*/
void uim_mss_bus_clk_config(npa_client_handle mss_bus_clk_handle, uint32 nFreqKhz);

/*
  @brief Programs the UIM clock for either a digital value or a 
         frequency.
*/
void uim_clock_control( uim_instance_enum_type uim_instance,
                        uim_clock_control_type uim_clock_control_req );

/*
  @brief Set the UART clock frequency based on the UIM baud rate
         adjustment factor nDI and the UIM clock rate conversion
         factor nFI.
*/
void SET_UIM_BAUD_RATE_SLOT(uim_instance_enum_type                 uim_instance,
                            uim_clock_rate_conversion_factor_type  nFI,
                            uim_baud_rate_adjustment_factor_type   nDI);

/*
  @brief Disables the UIM clocks: SIM, UART, HCLK.
*/
void UIM_CLOCK_REGIME_DISABLE_SLOT(uim_instance_enum_type uim_instance);

/*
  @brief Enables the UIM clocks: SIM, UART, HCLK.
*/
void UIM_CLOCK_REGIME_ENABLE_SLOT(uim_instance_enum_type uim_instance);

/*
  @brief Checks if ATR FI parameter is 0 or invalid.
*/
boolean UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID(uim_instance_enum_type uim_instance);

/*
  @brief Changes the SIM clk frequency to 3.8MHz.
*/
void UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_instance_enum_type uim_instance);

/*
  @brief Resets the SIM clk frequency to 4.8MHz.
*/
void UIM_RESET_CLK_FREQ_TO_DEFAULT_4_8_MHZ(uim_instance_enum_type uim_instance);

/*
  @brief Checks if the SIM clk frequency is at 4.8MHz.
*/
boolean UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_instance_enum_type uim_instance);

#endif /* UIMDRV_CLK_H */
