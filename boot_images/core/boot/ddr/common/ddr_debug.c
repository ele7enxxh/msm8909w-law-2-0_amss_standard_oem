/**
 * @file boot_ddr_debug.c
 * @brief
 * Implementation for DDR Debug Mode.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/ddr_debug.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/13   sl      Added more DDR tuning options.
09/13/13   sl      Fixed pslew/nslew order.
08/29/13   sl      Use USB APIs instead of Boot APIs.
08/20/13   sl      Added DDR tuning.
08/07/13   sr      Added watchdog reset support.
06/18/13   sl      Initial version.
================================================================================
                     Copyright 2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "stdio.h"
#include "string.h"
#include "qhsusb_al_bulk.h"
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_hwio.h"
#include "ddr_test.h"
#include "DDR_PHY.h"


/*==============================================================================
                                  MACROS
==============================================================================*/
/* Length of transportation buffer in characters */
#define BUFFER_LENGTH   512

/* Maximum length of command in characters */
#define CMD_MAX_LENGTH  64

/* Watchdog bark and bite time */
#define WATCHDOG_BARK_TIME_SCLK  0x7FFF
#define WATCHDOG_BITE_TIME_SCLK  0xFFFF

/* DDR tuning parameters */
#define DDR_TUNING_LOOP_COUNT  128
#define DQ_ROUT_MAX            0x7
#define DQ_PSLEW_MAX           0x3
#define DQ_NSLEW_MAX           0x3
#define READ_CDC_DELAY_MAX     0xFFF
#define WRITE_CDC_DELAY_MAX    0xFFF
#define CDC_DELAY_INC          0x10


/*==============================================================================
                                  DATA
==============================================================================*/
/* Echo and message buffers: must be addressable by USB and 4-byte aligned */
__attribute__((aligned(4)))
static char rx_buf[BUFFER_LENGTH], tx_buf[BUFFER_LENGTH+1], msg_buf[BUFFER_LENGTH+1] = {0};

/* Number of bytes to be echoed */
static uint32 bytes_to_echo;

/* Indicator whether receiving callback function is called */
static boolean is_rx_called = FALSE;

/* Debug command of DDR Debug Mode */
static struct
{
  char     cmd[CMD_MAX_LENGTH+1];
  uint32   idx;
  boolean  ready;
} debug_cmd = {{0}, 0, FALSE};

/* DDR tuning pattern */
static const uint32 ddr_tuning_pattern[] =
{
  0xDEADBEEF, 0x5A5A5A5A, 0xA5A5A5A5, 0xFEEDFACE,
  0xCAFEBABE, 0xA5A5A5A5, 0x5A5A5A5A, 0x0BADF00D,

  0xA5A5A5A5, 0x5A5A5A5A, 0xA5A5A5A5, 0x5A5A5A5A,
  0x0F0F0F0F, 0xF0F0F0F0, 0x00000000, 0xFFFFFFFF,

  0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
  0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000,
};

/* DDR tuning cookies */
__attribute__((section("DDR_DEBUG_TUNING_COOKIE")))
static void (*volatile reset_cb_func)(boolean) = NULL;

__attribute__((section("DDR_DEBUG_TUNING_COOKIE")))
static volatile uint32 dq_rout, dq_pslew, dq_nslew, read_cdc_delay, write_cdc_delay;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
static void ddr_debug_dog_set_bark_time(unsigned bark_time)
{
  while(!HWIO_INF(MPM2_WDOG_BARK_VAL_REG, SYNC_STATUS));
  HWIO_OUTF(MPM2_WDOG_BARK_VAL_REG, WDOG_BARK_VAL, bark_time);
}

static void ddr_debug_dog_set_bite_time(unsigned bite_time)
{
  while(!HWIO_INF(MPM2_WDOG_BITE_VAL_REG, SYNC_STATUS));
  HWIO_OUTF(MPM2_WDOG_BITE_VAL_REG, WDOG_BITE_VAL, bite_time);
}

static void ddr_debug_dog_init(boolean normal)
{
  HWIO_OUT(MPM2_WDOG_CTL_REG, 0x0);
  HWIO_OUT(BOOT_PARTITION_SELECT, 0x5D1);

  if (normal)
  {
    ddr_debug_dog_set_bark_time(WATCHDOG_BARK_TIME_SCLK);
    ddr_debug_dog_set_bite_time(WATCHDOG_BITE_TIME_SCLK);
  }
  else
  {
    ddr_debug_dog_set_bark_time(WATCHDOG_BARK_TIME_SCLK>>8);
    ddr_debug_dog_set_bite_time(WATCHDOG_BITE_TIME_SCLK>>8);
  }
}

static void ddr_debug_dog_enable(boolean enable)
{
  HWIO_OUT( MPM2_WDOG_CTL_REG, (enable ? 5 : 0) );
}

static void ddr_debug_dog_kick( void )
{
  HWIO_OUT(MPM2_WDOG_RESET_REG, 0x1);
}

static void ddr_debug_force_reset( void )
{
  ddr_debug_dog_init(FALSE);

  ddr_debug_dog_enable(TRUE);

  /* Wait for watchdog reset */
  for (; ;) {}
}

/* =============================================================================
**  Function : ddr_debug_rx_cb
** =============================================================================
*/
/**
*   @brief
*   Callback for transportation layer receiving.
*
*   @param[in]  bytes_read  Number of bytes read
*   @param[in]  err_code    Error code returned
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
static void ddr_debug_rx_cb(uint32 bytes_read, uint32 err_code)
{
  uint32 read, write;

  if (!bytes_read)  { return; }

  for (read = 0, write = 0; read < bytes_read/sizeof(char); read++)
  {
    if (rx_buf[read] == '\r')
    {
      debug_cmd.cmd[debug_cmd.idx] = '\0';
      debug_cmd.idx = 0;
      debug_cmd.ready = TRUE;

      tx_buf[write++] = '\r';
      tx_buf[write++] = '\n';

      break;
    }
    else if (rx_buf[read] == '\b' || rx_buf[read] == '\x7F')
    {
      if (debug_cmd.idx > 0)
      {
        debug_cmd.idx--;

        tx_buf[write++] = rx_buf[read];
      }
    }
    else if ( rx_buf[read] >= 'a' && rx_buf[read] <= 'z' ||
              rx_buf[read] >= 'A' && rx_buf[read] <= 'Z' ||
              rx_buf[read] >= '0' && rx_buf[read] <= '9' )
    {
      if (debug_cmd.idx < CMD_MAX_LENGTH)
      {
        debug_cmd.cmd[debug_cmd.idx++] = rx_buf[read];

        tx_buf[write++] = rx_buf[read];
      }
    }
  }

  bytes_to_echo = write * sizeof(char);

  is_rx_called = TRUE;

} /* ddr_debug_rx_cb */

/* =============================================================================
**  Function : ddr_debug_tx_cb
** =============================================================================
*/
/**
*   @brief
*   Callback for transportation layer transmitting.
*
*   @param[in]  bytes_written  Number of bytes written
*   @param[in]  err_code       Error code returned
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
static void ddr_debug_tx_cb(uint32 bytes_written, uint32 err_code)
{
} /* ddr_debug_tx_cb */

/* =============================================================================
**  Function : ddr_debug_input
** =============================================================================
*/
/**
*   @brief
*   Get input command for DDR Debug Mode.
*
*   @param  None
*
*   @retval  Input command
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
static const char *ddr_debug_input( void )
{
  uint32 err_code;

  debug_cmd.ready = FALSE;

  do
  {
    /* Receive user's inputs */
    qhsusb_al_bulk_receive( (byte *)rx_buf, sizeof(rx_buf),
                            ddr_debug_rx_cb, &err_code );

    while (!is_rx_called) {
      qhsusb_al_bulk_poll();
    }
    is_rx_called = FALSE;

    /* Echo user's inputs */
    qhsusb_al_bulk_transmit( (byte *)tx_buf, bytes_to_echo,
                              ddr_debug_tx_cb, &err_code );
  } while (!debug_cmd.ready);

  return debug_cmd.cmd;

} /* ddr_debug_input */

/* =============================================================================
**  Function : ddr_debug_output
** =============================================================================
*/
/**
*   @brief
*   Output message for DDR Debug Mode.
*
*   @param[in]  msg  Message to be output
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
static void ddr_debug_output(const char *msg)
{
  uint32 err_code;

  strncpy(msg_buf, msg, BUFFER_LENGTH);

  qhsusb_al_bulk_transmit( (byte *)msg_buf, strlen(msg_buf) * sizeof(char),
                           ddr_debug_tx_cb, &err_code );

} /* ddr_debug_output */

static void ddr_debug_test(uint32 *ddr_base, uint32 failures[4])
{
  uint32 idx;
  uint32 result;

  result = 0;

  for (idx = 0; idx < sizeof(ddr_tuning_pattern)/sizeof(uint32); idx++)
  {
    ddr_base[idx] = ddr_tuning_pattern[idx];
  }

  for (idx = 0; idx < sizeof(ddr_tuning_pattern)/sizeof(uint32); idx++)
  {
    result |= ddr_base[idx] ^ ddr_tuning_pattern[idx];
  }

  for (idx = 0; idx < 4; idx++)
  {
    if ( result & (0xFF << (idx*8)) )  { failures[idx]++; }
  }

} /* ddr_debug_test */

static void ddr_debug_do_tuning(boolean in_progress, boolean is_all)
{
  static char str[BUFFER_LENGTH];
  ddr_size_info ddr_info;
  uint32 count;
  uint32 failures[4];

  if (!in_progress)
  {
    ddr_debug_output("\r\nTuning DDR: [rout, pslew, nslew, rdcdc, wrcdc]...\r\n");

    if (is_all) { dq_rout = dq_pslew = dq_nslew = 0; }
    read_cdc_delay = write_cdc_delay = 0;
  }
  else
  {
    snprintf(str, BUFFER_LENGTH, "[%X, %X, %X, %X, %X]: "
                                 "X|X|X|X\r\n",
                                 dq_rout, dq_pslew, dq_nslew, read_cdc_delay, write_cdc_delay);
    ddr_debug_output(str);

    if ( (write_cdc_delay = (write_cdc_delay + CDC_DELAY_INC) & WRITE_CDC_DELAY_MAX) == 0 &&
         (read_cdc_delay = (read_cdc_delay + CDC_DELAY_INC) & READ_CDC_DELAY_MAX) == 0 &&
         ( !is_all || ((dq_nslew = (dq_nslew + 1) & DQ_NSLEW_MAX) == 0 &&
                       (dq_pslew = (dq_pslew + 1) & DQ_PSLEW_MAX) == 0 &&
                       (dq_rout = (dq_rout + 1) & DQ_ROUT_MAX) == 0) )
       )
    {
      return;
    }
  }

  ddr_info = ddr_get_size();

  /* Enable watchdog */
  ddr_debug_dog_enable(TRUE);

  do /* dq_rout */
  {
    do /* dq_pslew */
    {
      do /* dq_nslew */
      {
        /* Update drive strength: rout/pslew/nslew */
        HAL_SDRAM_PHY_Update_Drive_Strength(SDRAM_INTERFACE_0, dq_rout, dq_pslew, dq_nslew);

        do /* read_cdc_delay */
        {
          /* Update read CDC delay */
          HAL_SDRAM_PHY_Update_CDC_Delay(SDRAM_INTERFACE_0, TRUE, read_cdc_delay);

          do /* write_cdc_delay */
          {
            /* Update write CDC delay */
            HAL_SDRAM_PHY_Update_CDC_Delay(SDRAM_INTERFACE_0, FALSE, write_cdc_delay);

            /* Issue manual IOCAL and CDCCAL to make new settings take effect */
            HAL_SDRAM_PHY_Manual_IO_Cal(SDRAM_INTERFACE_0);
            HAL_SDRAM_PHY_Manual_CDC_Cal(SDRAM_INTERFACE_0);

            failures[0] = failures[1] = failures[2] = failures[3] = 0;

            for (count = 0; count < DDR_TUNING_LOOP_COUNT; count++)
            {
              if (ddr_info.sdram0_cs0 != 0)
              {
                ddr_debug_test((uint32 *)(uint32)ddr_info.sdram0_cs0_addr, failures);
              }
              if (ddr_info.sdram0_cs1 != 0)
              {
                ddr_debug_test((uint32 *)(uint32)ddr_info.sdram0_cs1_addr, failures);
              }
            }

            snprintf(str, BUFFER_LENGTH, "[%X, %X, %X, %X, %X]: "
                                         "%u|%u|%u|%u\r\n",
                                         dq_rout, dq_pslew, dq_nslew, read_cdc_delay, write_cdc_delay,
                                         failures[0], failures[1], failures[2], failures[3]);
            ddr_debug_output(str);

            /* Kick watchdog */
            ddr_debug_dog_kick();

          } while ((write_cdc_delay = (write_cdc_delay + CDC_DELAY_INC) & WRITE_CDC_DELAY_MAX) != 0);

        } while ((read_cdc_delay = (read_cdc_delay + CDC_DELAY_INC) & READ_CDC_DELAY_MAX) != 0);

      } while (is_all && (dq_nslew = (dq_nslew + 1) & DQ_NSLEW_MAX) != 0);

    } while (is_all && (dq_pslew = (dq_pslew + 1) & DQ_PSLEW_MAX) != 0);

  } while (is_all && (dq_rout = (dq_rout + 1) & DQ_ROUT_MAX) != 0);

  ddr_debug_output("DDR tuning done: [rout, pslew, nslew, rdcdc, wrcdc].\r\n");

} /* ddr_debug_do_tuning */

static void ddr_debug_tune_cdc(boolean in_progress)
{
  /* Install reset callback function */
  reset_cb_func = &ddr_debug_tune_cdc;

  /* Do tuning */
  ddr_debug_do_tuning(in_progress, FALSE);

  /* Uninstall reset callback function */
  reset_cb_func = NULL;

  /* Force watchdog reset */
  ddr_debug_force_reset();

} /* ddr_debug_tune_cdc */

static void ddr_debug_tune_all(boolean in_progress)
{
  /* Install reset callback function */
  reset_cb_func = &ddr_debug_tune_all;

  /* Do tuning */
  ddr_debug_do_tuning(in_progress, TRUE);

  /* Uninstall reset callback function */
  reset_cb_func = NULL;

  /* Force watchdog reset */
  ddr_debug_force_reset();

} /* ddr_debug_tune_all */

/* =============================================================================
**  Function : ddr_debug
** =============================================================================
*/
/**
*   @brief
*   Command interpreter of DDR Debug Mode.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Transportation layer (e.g., USB) must be set up, and all data must be moved
*   out of DDR (otherwise would be corrupted).
*
*   @sideeffects
*   DDR could be corrupted.
*
*   @sa
*   None
*/
void ddr_debug( void )
{
  static const char ddr_debug_msg_menu[] =
  "--------------------------------------------------\r\n"
  "|       DDR MUST - Memory Unit Shmoo Test        |\r\n"
  "|        Copyright 2013 by QUALCOMM Inc.         |\r\n"
  "--------------------------------------------------\r\n"
  "0: Restart System\r\n"
  "1: Test DDR Read/Write: Basic (< 1sec)\r\n"
  "2: Test DDR Read/Write: Advanced (< 5min)\r\n"
  "3: Tune DDR Settings: CDC Only (< 30min)\r\n"
  "4: Tune DDR Settings: All (overnight)\r\n"
  "\r\n";

  extern void qhsusb_dci_write_scratch(uint32 core_id, uint8 offset, uint32 data);
  const char *cmd;

  /* Start USB enumeration */
  qhsusb_al_bulk_init(0x0, QHSUSB_MODE__SER_ONLY);

  qhsusb_dci_write_scratch(0, 0, 0x0);

  ddr_debug_dog_init(TRUE);

  if (reset_cb_func != NULL)
  {
    reset_cb_func(TRUE);
  }

  while (1)
  {
    ddr_debug_output("\r\nPlease press ENTER to continue: ");
    ddr_debug_input();

    ddr_debug_output(ddr_debug_msg_menu);

    while (1)
    {
      ddr_debug_output("Please select an option: ");
      cmd = ddr_debug_input();

      if (strlen(cmd) != 1 || *cmd < '0' || *cmd > '4') { continue; }

      switch (*cmd)
      {
        case '0':
          ddr_debug_output("\r\nRestarting system...\r\n");
          ddr_debug_force_reset();
          break;

        case '1':
          ddr_debug_output("\r\nTesting data/address lines of DDR...\r\n");
          if (ddr_test(DDR_TEST_SUITE_ACCESS, 0, SDRAM_INTERFACE_0, SDRAM_BOTH, 0))
          {
            ddr_debug_output("DDR test passed.\r\n");
          }
          else
          {
            ddr_debug_output("DDR test failed.\r\n");
          }
          break;

        case '2':
          ddr_debug_output("\r\nTesting all range of DDR by own-address algorithm...\r\n");
          if (ddr_test(DDR_TEST_SUITE_ACCESS, 1, SDRAM_INTERFACE_0, SDRAM_BOTH, 0))
          {
            ddr_debug_output("DDR test passed.\r\n");
          }
          else
          {
            ddr_debug_output("DDR test failed.\r\n");
          }
          break;

        case '3':
          ddr_debug_output("\r\n");
          while (1)
          {
            ddr_debug_output("Please input rout/pslew/nslew (e.g, 522 for r=5/p=2/n=2): ");
            cmd = ddr_debug_input();

            if ( strlen(cmd) == 3 &&
                 cmd[0] >= '0' && cmd[0] <= '0' + DQ_ROUT_MAX &&
                 cmd[1] >= '0' && cmd[1] <= '0' + DQ_PSLEW_MAX &&
                 cmd[2] >= '0' && cmd[2] <= '0' + DQ_NSLEW_MAX )
            {
              dq_rout = cmd[0] - '0';
              dq_pslew = cmd[1] - '0';
              dq_nslew = cmd[2] - '0';

              ddr_debug_tune_cdc(FALSE);
              break;
            }
          }
          break;

        case '4':
          ddr_debug_tune_all(FALSE);
          break;

        default:
          /* Should not reach here */
          break;
      }

      break;
    }
  }

} /* ddr_debug */
