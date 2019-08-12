/*===========================================================================

  gpio_toggle.h - resource to allow masters to toggle GPIO pins

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "comdef.h"
#include "tlmm_hwio.h"

#define REQ_GPIO_NUM            0x626d756e  // 'numb' in little endian
#define REQ_GPIO_STATE          0x74617473  // 'stat' in little endian
#define REQ_GPIO_SETTTLING_TIME 0x74746573  // 'sett' in little endian

#define GPIO_ADR_BASE (TLMM_CSR_REG_BASE + 0x00003020)
#define GPIO_BNK_OFFSET 0x4
#define GPIO_STATE_OFFSET 0x20

/* The internal representation of the GPIO toggle config                     *
 * state         - state of the pin: 0(clear), 1(set)                        *
 * pin           - GPIO pin to toggle                                        *
 * settling_time - settling time                                             */
typedef struct
{
  uint8  state;
  uint8  pin;
  uint16 settling_time;
} gpio_toggle_inrep;

