/**
* @file modem_xpu_int.c
* @brief Configure Badger Modem XPU Violations
*
* This file implements the XPU interrupt Handling. All XPU violations are reported
* over DIAG (Subsystem QDSP6) and to a ring buffer referenced by 'modem_sec_xpu_buffer'.
*
*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/src/8976/modem_xpu_int.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
01/04/2013 rs       Initial version
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdio.h>
#include <IxErrno.h>
#include <HALhwio.h>
#include <stdlib.h>
#include "qurt.h"
#include "HALxpu2DataTypes.h"
#include "HALxpu2.h"
#include <string.h>
#include "msmhwiobase.h"
#include "modem_hwio.h"
#include "modem_xpu_int.h"
#include "msg_diag_service.h"

#define TZBSP_XPU_ERR_INT_STATUS_REG_NUM    1
#define TZBSP_XPU_ERR_NUM_PER_REG           32
#define MODEM_SEC_BUFFER_SPACE_LIMIT        30

/* Maximum value for XPU error status bit position.
 * Error status register is 32 bit so position ranges
 * from 0 to 31
 */
#define TZBSP_XPU_ERR_BIT_POS_MAX   (sizeof(uint32)*8 - 1)

#define MODEM_SEC_XPU_STACK_SIZE             1024

#define ALWAYS(xx)                                                      \
  do                                                                    \
  {                                                                     \
    if(!(xx))                                                           \
    {                                                                   \
      printf("%s:%d Failed!\n", __FUNCTION__, __LINE__);                 \
    }                                                                   \
  } while(0)

/* Debug Messaging Macros that logs to QXDM and Ring Buffer */
#define MODEM_SEC_MSG_1(xx_buff_pointer, xx_size, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, xx_fmt, xx_arg1); \
  modem_sec_print_buffer(snprintf( xx_buff_pointer, xx_size, xx_fmt, xx_arg1));

#define MODEM_SEC_MSG_2(xx_buff_pointer, xx_size, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2); \
  modem_sec_print_buffer(snprintf( xx_buff_pointer, xx_size, xx_fmt, xx_arg1, xx_arg2));
  
#define MODEM_SEC_MSG_3(xx_buff_pointer, xx_size, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2); \
  modem_sec_print_buffer(snprintf( xx_buff_pointer, xx_size, xx_fmt, xx_arg1, xx_arg2, xx_arg3));
  
#define DUMP_XPU_ERR_FLAG(xx_buff_pointer, xx_size, xx, bit) \
  if((xx) & (bit)) MODEM_SEC_MSG_1(xx_buff_pointer, xx_size, "xpu:  %s", #bit)
 
/* Size of XPU Violation Ring Buffer */
#define XPU_BUFFER_SIZE              1024

/* Function Headers */
void modem_sec_print_buffer (int print_buf_len);
void modem_sec_xpu_error_log(HAL_xpu2_XPU2Type xpu);

/* The cumulative number of XPU errors. */
volatile uint32 g_tzbsp_xpu_err_count;

/* XPU Violation Ring Buffer */
char modem_sec_xpu_buffer[XPU_BUFFER_SIZE]; 

static char *modem_sec_ring_buff = modem_sec_xpu_buffer;
static int modem_sec_buffer_limit = XPU_BUFFER_SIZE;

typedef struct
{
  /* Bit position in status register */
  uint8 bit_pos;
  /* Corresponding HAL XPU index */
  uint8 xpu;
} xpu_err_pos_to_hal_map_t;

static xpu_err_pos_to_hal_map_t xpu_err_pos_to_hal_map
                [TZBSP_XPU_ERR_INT_STATUS_REG_NUM][TZBSP_XPU_ERR_NUM_PER_REG] =
{
  {
    {0, HAL_XPU2_MSS},
    {1, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {2, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {3, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {4, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {5, HAL_XPU2_BIMC_S_DDR0},            //Obsolete bit for ELAN
    {6, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {7, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {8, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {9, HAL_XPU2_COUNT},            //Obsolete bit for ELAN
    {10, HAL_XPU2_COUNT},           //Obsolete bit for ELAN
    {11, HAL_XPU2_COUNT},           //Obsolete bit for ELAN
    {12, HAL_XPU2_COUNT},           //Obsolete bit for ELAN
    {13, HAL_XPU2_BOOT_ROM},
    {14, HAL_XPU2_COUNT},           //Obsolete bit for ELAN
    {15, HAL_XPU2_COUNT},           //Obsolete bit for ELAN
    {16, HAL_XPU2_COUNT},           //Obsolete bit for ELAN
    {17, HAL_XPU2_COUNT},
    {18, HAL_XPU2_COUNT},
    {19, HAL_XPU2_COUNT},
    {20, HAL_XPU2_COUNT},
    {21, HAL_XPU2_COUNT},
    {22, HAL_XPU2_COUNT},           //Obsolete bit foe ELAN
    {23, HAL_XPU2_COUNT},
    {24, HAL_XPU2_COUNT},
    {25, HAL_XPU2_COUNT},
    {26, HAL_XPU2_COUNT},
    {27, HAL_XPU2_COUNT},
    {28, HAL_XPU2_COUNT},
    {29, HAL_XPU2_COUNT},
    {30, HAL_XPU2_COUNT},
    {31, HAL_XPU2_COUNT},
  },
};

/** 
* Enable all MSA XPU Interrupts 
**/
static void modem_sec_xpu_enable_interrupts(void)
{
  /* Enable the interrupts propagation */
  HWIO_OUT(TCSR_SS_XPU2_MSA_INTR0_ENABLE, 0x00002020);
}

/** 
* Inline function used to get the next XPU Error 
**/
static inline uint32 get_next_error_xpu(uint32 bitmask, uint32 *lastval)
{
  uint32 i = *lastval;
  /*
   * Loop over the 32 bits of input bitmask to find the first bit set starting
   * from the bit position pointed to by lastval
   */
  for (; (i <= TZBSP_XPU_ERR_BIT_POS_MAX) && (0 != bitmask); i++)
  {
    if((bitmask >> i) & 0x1)
    {
      break;
    }
  }
  /*
   * Set starting position for next iteration to be 1 more than current
   * set bit found. If no bit found, then it makes lastval as invalid
   */
  *lastval = i + 1;
  return i;
}

/** 
* Configure XPU Error reporting and Initialize Interrupt Service Thread 
**/
void modem_sec_config_xpu_error_reporting (void)
{
  uint32 xpu_id;
  const HAL_xpu2_XPU2Type g_tzbsp_xpu_cfg[] = { HAL_XPU2_BIMC_S_DDR0, 
                                                HAL_XPU2_BOOT_ROM, 
                                                HAL_XPU2_MSS, 
                                                HAL_XPU2_QPIC_NAND, 
                                                HAL_XPU2_COUNT };
  HAL_xpu2_ErrorOptionsType opt = HAL_XPU2_ERROR_O_CLIENT_RPT_EN |
                                        HAL_XPU2_ERROR_O_CFG_RPT_EN |
                                        HAL_XPU2_ERROR_O_DECODE_EN |
                                        HAL_XPU2_ERROR_O_INTERRUPT_EN;
  
  for (xpu_id = 0; xpu_id < sizeof(g_tzbsp_xpu_cfg)/sizeof(HAL_xpu2_XPU2Type); xpu_id++)
  {
    HAL_xpu2_ConfigErrorReporting(g_tzbsp_xpu_cfg[xpu_id], opt);
  }
  
  modem_sec_xpu_enable_interrupts();
  modem_sec_xpu_IST_init();
  
  return;
}

/** 
* Register XPU Violation ISR
**/
void modem_sec_xpu_IST (void *arg)
{
  int ret;
  qurt_anysignal_t int_signal;
  int modem_sec_xpu_int = 69;
  unsigned int int_mask = 1 ;

  /* Register XPU Violation Interrupt with QURT */
   qurt_anysignal_init(&int_signal);

   if( qurt_interrupt_register (modem_sec_xpu_int, &int_signal, int_mask) != QURT_EOK)
   {
      printf("Failed to register XPU interrupt\n");
   }
   
   do
   {
      ret = qurt_anysignal_wait(&int_signal, int_mask);
      qurt_anysignal_clear(&int_signal, ret);

       //Processing XPU interrupt in IST context
      modem_sec_xpu_isr();

      (void)qurt_interrupt_acknowledge(modem_sec_xpu_int);

   }while(1);
}

/** 
* Initialize an Interrupt Service Thread 
**/
void modem_sec_xpu_IST_init (void)
{
   void *stack;
   unsigned int ret;
   int priority;
   qurt_thread_attr_t thread_attr;
   qurt_thread_t thread_id;

   qurt_thread_attr_init(&thread_attr);

   stack = qurt_malloc(MODEM_SEC_XPU_STACK_SIZE);
   if (NULL == stack)
   {
      printf("Memory allocation failed\n");
      ALWAYS(0);
   }
   thread_id = qurt_thread_get_id();
   priority = qurt_thread_get_priority(thread_id);

   ALWAYS(stack!=NULL);
   qurt_thread_attr_set_stack_addr(&thread_attr, stack);
   qurt_thread_attr_set_name(&thread_attr, "modem_sec_xpu_IST");
   qurt_thread_attr_set_stack_size(&thread_attr, MODEM_SEC_XPU_STACK_SIZE);
   qurt_thread_attr_set_priority(&thread_attr, priority);
   if ( (ret=qurt_thread_create(&thread_id, &thread_attr, modem_sec_xpu_IST, NULL)) == QURT_EFATAL)  {
        printf("Failed to start modem sec XPU IST\n");
        ALWAYS(0);     
   }

   printf ("Modem Sec XPU ISR Thread started\n");
}

/** 
* Interrupt Service routine upon an XPU Violation 
**/
void modem_sec_xpu_isr()
{
  uint32 err_bitmask[TZBSP_XPU_ERR_INT_STATUS_REG_NUM] = {0};
  uint32 xpu_position = 0;
  HAL_xpu2_XPU2Type xpu = HAL_XPU2_COUNT;
  uint32 i = 0, j = 0;
  uint32 err_bitmask_ctr = 0;

  err_bitmask[0] = HWIO_TCSR_SS_XPU2_MSA_INTR0_IN;
 

  for(err_bitmask_ctr = 0; err_bitmask_ctr < TZBSP_XPU_ERR_INT_STATUS_REG_NUM;
      ++err_bitmask_ctr)
  {
    if(0 != err_bitmask[err_bitmask_ctr])
    {
      i = 0;
      do
      {
        xpu_position = get_next_error_xpu(err_bitmask[err_bitmask_ctr], &i);
        if(xpu_position <= TZBSP_XPU_ERR_BIT_POS_MAX)
        {
          for (j = 0; j < (sizeof(xpu_err_pos_to_hal_map[err_bitmask_ctr]) /
                           sizeof(xpu_err_pos_to_hal_map_t)); ++j)
          {
            if( xpu_position ==
                             xpu_err_pos_to_hal_map[err_bitmask_ctr][j].bit_pos)
            {
              xpu = (HAL_xpu2_XPU2Type)
              xpu_err_pos_to_hal_map[err_bitmask_ctr][j].xpu;
              if(HAL_XPU2_COUNT != xpu)
              {
                modem_sec_xpu_error_log(xpu);
              }
             } 
          }
        }
      } while (i < TZBSP_XPU_ERR_BIT_POS_MAX);
    }
  }

  return;
}

/**
 * Log the XPU Errors to QXDM and Ring Buffer
 */
void modem_sec_xpu_error_log(HAL_xpu2_XPU2Type xpu)
{
  HAL_xpu2_ErrorType err;
  if(TRUE == HAL_xpu2_IsError(xpu))
  {
    g_tzbsp_xpu_err_count++;
    HAL_xpu2_GetError((HAL_xpu2_XPU2Type)xpu, &err); 
    
    /* Log violation to QXDM and Buffer (modem_sec_xpu_buffer) */
    MODEM_SEC_MSG_1(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uAMemType: %08x", (unsigned int)err.uAMemType);             
    MODEM_SEC_MSG_2(modem_sec_ring_buff, modem_sec_buffer_limit, "xpu:>>> [%u] XPU error dump, XPU id %u <<<", (unsigned int)g_tzbsp_xpu_err_count, xpu);
    MODEM_SEC_MSG_1(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uErrorFlags: %08x", (unsigned int)err.uErrorFlags);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uErrorFlags, HAL_XPU2_ERROR_F_CONFIG_PORT);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uErrorFlags, HAL_XPU2_ERROR_F_CLIENT_PORT);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uErrorFlags, HAL_XPU2_ERROR_F_MULTIPLE);

    MODEM_SEC_MSG_1(modem_sec_ring_buff, modem_sec_buffer_limit, " uBusFlags: %08x", (unsigned int)err.uBusFlags);
    
    /* Dump XPU Error flags to QXDM As well */
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_ERROR_AC);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_ERROR_DCD);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_ASHARED);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AINNERSHARED);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_APRIV);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_APROTNS);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AINST);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AWRITE);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AOOO);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AFULL);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_ABURST);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_BURSTLEN);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_ARDALLOCATE);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AEXCLUSIVE);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_ARDBEADNDXEN);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_AMSSSELFAUTH);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_SAVERESTORE_IN_PROG);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_MSA_RG_MATCH);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_SECURE_RG_MATCH);
    DUMP_XPU_ERR_FLAG(modem_sec_ring_buff, modem_sec_buffer_limit, err.uBusFlags, HAL_XPU2_BUS_F_NONSECURE_RG_MATCH);

    MODEM_SEC_MSG_1(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uPhysicalAddress: %08x",
              (unsigned int)err.uPhysicalAddressLower32);
    MODEM_SEC_MSG_2(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uMasterId: %08x, uAVMID   : %08x",
              (unsigned int)err.uMasterId, (unsigned int)err.uAVMID);
    MODEM_SEC_MSG_2(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uATID    : %08x, uABID    : %08x",
              (unsigned int)err.uATID, (unsigned int)err.uABID);
    MODEM_SEC_MSG_2(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uAPID    : %08x, uALen    : %08x",
              (unsigned int)err.uAPID, (unsigned int)err.uALen);
    MODEM_SEC_MSG_2(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uASize   : %08x, uAPReqPriority   : %08x",
              (unsigned int)err.uASize, (unsigned int)err.uAPReqPriority);  
    MODEM_SEC_MSG_1(modem_sec_ring_buff, modem_sec_buffer_limit, " xpu: uAMemType: %08x", (unsigned int)err.uAMemType);             

    /* Clear the Error */
    HAL_xpu2_ClearError(xpu);
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, MSG_LEGACY_ERROR, "XPU ERROR: XPU %d reported no error!!", xpu);
  }
}

/** 
* Track debug message buffer pointer 
**/
void modem_sec_print_buffer (int print_buf_len) 
{
  modem_sec_ring_buff += print_buf_len; 
  modem_sec_buffer_limit -= print_buf_len;
  if (modem_sec_buffer_limit <= MODEM_SEC_BUFFER_SPACE_LIMIT) {
    modem_sec_ring_buff = modem_sec_xpu_buffer;
    modem_sec_buffer_limit = sizeof(modem_sec_xpu_buffer);
  }
}