#ifndef TCXOMGR_ADC_CLIENT_H
#define TCXOMGR_ADC_CLIENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

              T C X O    M A N A G E R   A D C   C L I E N T

                        TCXO Manager Header File

GENERAL DESCRIPTION

  Exported client interface into the ADC DAL.

EXTERNALIZED FUNCTIONS

  tcxomgr_adc_client_init()
  tcxomgr_adc_client_adc_read()


INITIALIZATION AND SEQUENCING REQUIREMENTS

  The tcxomgr_adc_client_init() function must be called prior to initiating
  an ADC read.

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_adc_client.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
08/13/15     tl      Add terminate function to release acquired resources
07/09/15     tl      Clean up ADC interface
11/01/11     ns      Add support for Tcxo mode
07/06/11     ag      Remove ADCDAL featurization 
11/02/10     ns      Store Timestamp of temp read
07/14/10     sg      Added code to perform recalibration for high temp change
11/25/09     ag      Wrapping ADC DAL with FEATURE_TCXOMGR_ADCDAL
07/29/09     ag      Changes made during integration with tcxomgr code.  
06/15/09     adw     Initial revision.

============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "modem_mcs_defs.h"
#include "DDIAdc.h"
#include "time_svc.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*---------------------------------------------------------------------------
  Macros
---------------------------------------------------------------------------*/

/* Number of ADC devices, currently XO_GPS, XO, PA and PMIC */
#define NUM_ADC_DEVICES      5

/* Means "tcxomgr_adc_temp_type" temperature is invalid or unavailable.
   Tcxomgr depends on this value, hence make sure both are consistent */
#define TCXOMGR_ADC_INVALID_TEMP      ( (tcxomgr_adc_temp_type)( 0x80000000 ) )


/*---------------------------------------------------------------------------
  Typedefs
---------------------------------------------------------------------------*/

/* Temperature value type sensitivity: 2^-10 deg C per LSB */
typedef int32        tcxomgr_adc_temp_type;

/* Enum listing types of read offered */
typedef enum
{
  BLOCKING_READ,
  BUFFERED_READ,
}
tcxomgr_adc_client_read_type;


/* Enum listing types of Thermistors supported */
typedef enum
{
  TCXO_PMIC_THERM,
  /* 4 millisecond read, needed for better accuracy of GPS */
  TCXO_XO_THERM_ACCURATE,
  /* 1 millisecond read, needed for fast coarse reads in absence of GPS */
  TCXO_XO_THERM_COARSE,
  TCXO_PA_THERM,
  TCXO_MSM_THERM,
  TCXO_MAX_THERM
}
tcxomgr_adc_client_therm_type;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_INIT

DESCRIPTION    This function initializes the Tcxomgr ADC client. It first
               reads the DAL input properties and then attches to the ADC
               DAL device driver.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_init( void );


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_TERM

DESCRIPTION    This function releases all resources acquired while
               initializing and operating the ADC client.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_term(void);


/*---------------------------------------------------------------------------
  ADC Read
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_ADC_READ

DESCRIPTION    This function initiates an ADC read. 

DEPENDENCIES   The tcxomgr_adc_client_init() must have been called previously.

RETURN VALUE   Result of ADC read.

SIDE EFFECTS   None.

===========================================================================*/
tcxomgr_adc_temp_type tcxomgr_adc_client_adc_read
(
  /* Which ADC do you need to read? */
  tcxomgr_adc_client_therm_type   which_therm, 

  /* What is the type of read - buffered, blocking? */
  tcxomgr_adc_client_read_type    read_type,

  /* Return read time, only for buffered read */
  uint32 *read_time       
);

/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_ADC_RECALIBRATE

DESCRIPTION    This function initiates ADC recalibration

DEPENDENCIES   The tcxomgr_adc_client_init() must have been called previously.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_adc_recalibrate
(
  /* Which ADC do you need to read? */
  tcxomgr_adc_client_therm_type   which_therm     
);


#endif /* TCXOMGR_ADC_CLIENT_H */

