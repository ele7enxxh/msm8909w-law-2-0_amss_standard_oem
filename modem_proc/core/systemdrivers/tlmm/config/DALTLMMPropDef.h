#ifndef __DALTLMMPROPDEF_H__
#define __DALTLMMPROPDEF_H__
#include "DALStdDef.h" 
/*
============================================================================

                 D A L T L M M   S T A T E   S T R U C T U R E
                             H E A D E R   F I L E

FILE:         DALTLMMDefines.h

DESCRIPTION:  
              This file contains definitions for use solely with the
              DALTLMM Properties XML file.  It's use is not recommended by
              any drivers and must not be used in any DALTLMM APIs.

============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/tlmm/config/DALTLMMPropDef.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/30/10   dcf     Created the initial revision. 
 
============================================================================
             Copyright (c) 2010 - 2013 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
*/


/*==========================================================================

                     DEFINITIONS FOR THIS MODULE

==========================================================================*/

/*
 * The following definitions are used solely in the TLMM.xml files to 
 * set up initialization configuration for each GPIO.  Definitions are 
 * based on the DAL_GPIO_CFG_OUT() macro in the DDITlmm.h file.  The below 
 * definitions incorporate the shifted values from this macro in order 
 * to accommodate the XML preprocessor mechanism for DAL Properties. 
 *  
 * These are based on DAL_GPIO_VERSION defined as 0x20000000 with 
 * DALTLMM_INTERFACE_VERSION defined as DALINTERFACE_VERSION(1,1) 
*/


#define DALTLMM_INPUT            0          /* Defines 0 for direction INPUT */
#define DALTLMM_OUTPUT           0x4000     /* Defines 1 for direction OUTPUT */
#define DALTLMM_NO_PULL          0          /* Defines 0 for NO-PULL */
#define DALTLMM_PULL_DOWN        0x8000     /* Defines 1 for PULL-DOWN */
#define DALTLMM_KEEPER           0x10000    /* Defines 2 for KEEPER */
#define DALTLMM_PULL_UP          0x18000    /* Defines 3 for PULL-UP */
#define DALTLMM_OUT_LOW          0x400000   /* OUTPUT LOW value in HAL format */
#define DALTLMM_OUT_HIGH         0x600000   /* OUTPUT HIGH value in HAL format */
#define DALTLMM_PRG_YES          0x10000000 /* Program this GPIO */
#define DALTLMM_PRG_NO           0x0        /* Do not program this GPIO. */


/** 
  TLMM port selection enumerations.
    
  The DALGpioPortType is a port configuration enumeration for publicly available 
  TLMM ports, which abstract miscellaneous TLMM registers and are used with the 
  DalTlmm_SetPort API.
  
  @note Additions to this enumeration and associated functionality are added 
  on a needed basis, and not every enumeration is available or defined on every 
  target). 
    
  @sa
  DalTlmm_SetPort
*/

/* ====================================================================
   New Ports must be added to the end of this enumeration (just before 
   DAL_TLMM_NUM_PORTS).
=====================================================================*/

#define TLMM_USB_PORT_SEL           0
#define TLMM_PORT_CONFIG_USB        1
#define TLMM_PORT_TSIF              2
#define TLMM_AUD_PCM                2
#define TLMM_UART1                  4
#define TLMM_UART3                  5
#define TLMM_LCDC_CFG               6
#define TLMM_KEYSENSE_SC_IRQ        7
#define TLMM_KEYSENSE_A9_IRQ        8
#define TLMM_TCXO_EN                9
#define TLMM_SSBI_PMIC              10
#define TLMM_PA_RANGE1              11
#define TLMM_SPARE_WR_UART_SEL      12
#define TLMM_PAD_ALT_FUNC_SDIO_EN   13
#define TLMM_SPARE_WR_TCXO_EN       14
#define TLMM_SPARE_WR_PA_ON         15
#define TLMM_SDC3_CTL               16
#define TLMM_SDC4_CTL               17
#define TLMM_UIM1_PAD_CTL           18
#define TLMM_UIM2_PAD_CTL           19
#define TLMM_RFFE_CTL               20

#define TLMM_NUM_PORTS              21



/*
 * TLMMGpioIdType
 *
 * GPIO Identifier for configuring a GPIO based on device config data for
 * a particular platform.
 *
 *  nGpioNumber:     GPIO Pin Number.
 *  nFunctionSelect: The function select associated with this GPIO.
 */
typedef struct {  
   uint32 nGpioNumber;
   uint32 nFunctionSelect;
}TLMMGpioIdType;


/*
 * TLMMGpioIdCfgType
 *
 * GPIO Identifier for configuration of a pin based on on device config data for
 * a particular platform and used in association with the TLMMGpioIdType.
 *
 *  nDir:     The direction of the GPIO (input/output).
 *  nPull:    The pull associated with this GPIO.
 *  nDrive:   The drive strength associated with this GPIO.
 *  nOutput:  The output value associated with a generic output configuration.
 */
typedef struct {
  uint32 nDir;
  uint32 nPull;
  uint32 nDrive;
  uint32 nOutput;
}TLMMGpioIdCfgType;


#endif /* __DALTLMMPROPDEF_H__ */

