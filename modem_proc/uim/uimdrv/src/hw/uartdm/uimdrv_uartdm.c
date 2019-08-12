/*============================================================================
  FILE:         uimdrv_uartdm.c

  OVERVIEW:     File containing the functions used by uartdm

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/uartdm/uimdrv_uartdm.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
05/30/16    gm         F3 message removal
01/20/16    na         Corrected condition logic
12/23/15    sam        Fix for KW errors
12/10/15    ks         8917 bringup changes
10/16/15    ll         All CR access use prgCR function for cleaner code design
09/16/15    ks         8937 bringup changes
09/16/15    sam        UART 1_4_2 changes
06/01/15    sam        9x07 bringup changes
11/11/14    ks         Using the DAL chip enums instad of hardcoding values
11/06/14    nmb        8952 bring up changes
09/23/14    ks         8909 bring up changes
09/18/14    akv        UIMDRV UT enhancements
08/21/14    ak         Replace 9x45 DAL chip family reference for compilation on BOLT
08/20/14    nmb        9x45 Bring up changes
08/14/14    ks         Hotswap enhancement for flaky mechanical switches
05/30/14    akv        UART_DM register logging
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
02/28/14    ks         BATT ALARM trigger and CARD REMOVED trigger
                       enabling under the feature checks
02/20/14    ak         Maximize the UART clock frequency by setting it directly
02/18/14    sam        Usage of newer style diag macros
01/29/14    akv        RX ISR refactoring changes
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
11/13/13    akv        Program BCR Reg to enable all features for UARTDM 1.4.0
10/08/13    nmb        HW header file cleanup
09/19/13    akv        Reference CLK changes to support 4.8MHz
09/11/13    na         8x62 bring up changes
08/06/13    rm/ak      Bolt RUMI code
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
05/22/13    akv        Configuration of UICC shutdown gpio's and feature
                       enablement based on NV
05/22/13    akv        Hotswap fix for active low card detect polarity not working
03/28/12    akv        Add delay for every CR register access to let UART
                       complete the command and be prepared for the subsequent
                       UARTDM register accesses
02/22/13    js         8x26 bring up changes
02/12/13    js         General F3 macro cleanup
02/08/13    akv        Power optimization fixes
02/01/13    akv        Minimization of uim_clk_busy_waits
01/16/13    akv        Hotswap enablement on Triton
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_uartdm.h"
#include "uimdrv_clk.h"
#include "uimglobals.h"
#include "uim_msg.h"


/* Shadow all of the UART DM registers for UIM1 and UIM2 for easy debugging */
uim_uart_shadow_type gaUartDmRegShadow[UIM_MAX_INSTANCES];

/* Table holding the default UARTDM instance number for different UIM instances. */
const uim_uartdm_num_table_type   uim_uartdm_num_table[] = {

  /* ARAGORN_8974 */  { DALCHIPINFO_FAMILY_MSM8974,
                        /*  UIM1_UART_NUM,           UIM2_UART_NUM,             UIM3_UART_NUM,             UIM4_UART_NUM  */
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* ARAGORN_8974 */

  /* ARAGORN_PRO */   { DALCHIPINFO_FAMILY_MSM8974_PRO,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* ARAGORN_PRO */

     /* ELAN_9X25 */  { DALCHIPINFO_FAMILY_MDM9x25,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* ELAN_9X25 */

     /* DINO_8X10 */  { DALCHIPINFO_FAMILY_MSM8x10,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* DINO_8X10 */

     /* FRODO_8X26 */ { DALCHIPINFO_FAMILY_MSM8x26,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* FRODO_8X26 */

     /* GIMLI_8X26 */ { DALCHIPINFO_FAMILY_MSM8926,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* GIMLI_8X26 */

     /* TORINO_9X35 */{ DALCHIPINFO_FAMILY_MDM9x35,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* TORINO_9X35 */

     /* THORIN_8X62 */{ DALCHIPINFO_FAMILY_MSM8x62,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* THORIN_8X62 */

  /* BAGHEERA_8916 */ { DALCHIPINFO_FAMILY_MSM8916,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* BAGHEERA_8916 */

     /* SHERE_8936 */ { DALCHIPINFO_FAMILY_MSM8936,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* SHERE_8936 */

    /* ELESSAR_8994 */{ DALCHIPINFO_FAMILY_MSM8994,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* ELESSAR_8994 */

    /* TESLA_9x45 */  { DALCHIPINFO_FAMILY_MDM9x45,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      },  /* TESLA_9x45 */

     /* CHEEL_8909 */ { DALCHIPINFO_FAMILY_MSM8909,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* CHEEL_8909 */

    /* TABASCO/SAHI*/ {/* DALCHIPINFO_FAMILY_MSM8952 */ 60,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, BADGER_MSS_UIM2_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* TABASCO_SAHI_8952 */

     /* LYKAN_9x07 */ { DALCHIPINFO_FAMILY_MDM9x07,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* CHEEL_8909 */

     /* FEERO_8937 */ { DALCHIPINFO_FAMILY_MSM8937,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* FEERO_8937 */

/* FEERO_LITE_8917 */ { DALCHIPINFO_FAMILY_MSM8917,
                       { BADGER_MSS_UIM0_UART_DM, BADGER_MSS_UIM1_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM  }
                      },  /* FEERO_LITE_8917 */

     /* DALCHIPINFO_FAMILY_UNKNOWN */{ DALCHIPINFO_FAMILY_UNKNOWN,
                       { INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM, INVALID_BADGER_MSS_UART_DM }
                      }  /* THORIN_8X62 */

}; /* uim_uartdm_num_table[MAX_UIM_CHIPSET_INDEX] */


/* This structure maps a uart clock divider value with the
   value to program into the UART CSR register*/
typedef struct
{
  uint32 bit_clk_div;
  uint32 uart_csr_value;
}bit_clk_div_to_csr_value_type;

/* The purpose of this table is to find the value to program into the UART CSR register in order to
   program a particular uart clock divider value */
bit_clk_div_to_csr_value_type bit_clk_div_to_uart_csr_map[] =
{
  /* uart clk divider factor,  CSR register value */
  {16, 0xFF},
  {32, 0xEE},
  {48, 0xDD},
  {64, 0xCC},
  {96, 0xBB},
  {128, 0xAA},
  {192, 0x99},
  {256, 0x88},
  {384, 0x77},
  {512, 0x66},
  {768, 0x55},
  {1536, 0x44},
  {3072, 0x33},
  {6144, 0x22},
  {12288, 0x11},
  {24576, 0x00}
};

/* uim_log_uartdm_register - Function used for logging the uart register opertaions
                             into a global circular buffer for debugging purposes. */
void uim_log_uartdm_register(uim_instance_enum_type uim_instance, uartdm_register_map_type name,
                             uartdm_register_log_action_type action, dword value)
{
#ifdef FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING
  uim_instance_global_type *uim_ptr = NULL;
  uim_ptr = uim_get_instance_ptr(uim_instance);

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_ptr is NULL in uim_log_uartdm_register");
    return;
  }

  UIM_ENTER_CRIT_SECTION(&uim_ptr->crit_sect.uartdm_logging, uim_ptr);

  if (uim_ptr->debug.uart_reg_log.active_index > UART_REG_LOG_MAX_ENTRY-1)
  {
    uim_ptr->debug.uart_reg_log.active_index  = 0;
  }

  uim_ptr->debug.uart_reg_log.trans_log[uim_ptr->debug.uart_reg_log.active_index].reg_name = name;
  uim_ptr->debug.uart_reg_log.trans_log[uim_ptr->debug.uart_reg_log.active_index].action   = action;
  uim_ptr->debug.uart_reg_log.trans_log[uim_ptr->debug.uart_reg_log.active_index].value    = value;
  uim_ptr->debug.uart_reg_log.active_index++;

  UIM_LEAVE_CRIT_SECTION(&uim_ptr->crit_sect.uartdm_logging, uim_ptr);
#endif /* FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING */
} /* uim_log_uartdm_register */

/* UARTDM configuration functions */
uint32 get_base_address(uimUartEnumList uart_instance)
{
  uint32 uart_base_address = 0;

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  switch(uart_instance)
  {
    case BADGER_MSS_UIM0_UART_DM :
      uart_base_address =  (MSS_UIM0_UART_DM_REG_BASE);
      break;
    case BADGER_MSS_UIM1_UART_DM :
      uart_base_address = (MSS_UIM1_UART_DM_REG_BASE);
      break;
    case BADGER_MSS_UIM2_UART_DM :
      uart_base_address = (MSS_UIM2_UART_DM_REG_BASE);
      break;
    default:
      break;
  }
#else
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  return uart_base_address;
} /* get_base_address */


/* Set up the base address register of the UART DM */
void initialize_uartdm(uim_instance_enum_type uim_instance)
{
  uim_hw_if.uart[uim_instance].start_addr = 0;

  if(!m_HWConfig.uim_slot_config[uim_instance].disableUim)
  {
    uim_hw_if.uart[uim_instance].start_addr =
      get_base_address(m_HWConfig.uim_slot_config[uim_instance].uimUart);
  }

  if(uim_hw_if.uart[uim_instance].start_addr == 0)
  {
    UIMDRV_MSG_ERR_0(uim_instance,"UART base address is 0x00000");
    return;
  }
} /* initialize_uartdm */


/* Program the mode register 1 */
void prgMR1( uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nMR1 = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_MR1, val);
  uim_log_uartdm_register(uim_instance, UART_DM_MR1, UIM_REG_WRITE, val);
} /* prgMR1 */


/* Program the mode register 2 */
void prgMR2(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nMR2 = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_MR2, val);
  uim_log_uartdm_register(uim_instance, UART_DM_MR2, UIM_REG_WRITE, val);
} /* prgMR2 */


/* Program the clock selection register*/
void  prgCSR(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nCSR= val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_CSR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_CSR, UIM_REG_WRITE, val);
} /* prgCSR */


/*
  UART_DM_SR, Access: Read
  UART status registers. This register is used to obtain the current State
  (RX Ready, RX Full, TX Ready, TX empty, Overrun, Parity error, Rx break,
  Hunt Char) of the UART subsystem. This register is updated asynchronously.
*/
dword readStatus(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data =  UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr + UART_DM_SR);
  uim_log_uartdm_register(uim_instance, UART_DM_SR, UIM_REG_READ, data);
  return data;
} /* readStatus */


/* program the command register */
void  prgCR(uim_instance_enum_type uim_instance, dword  val)
{
  dword data  = 0;
  uint8 count = 0;

  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nCR = val;

  if (UIM_UART_DM_HW_VERSION_1_4_2 <= uim_hw_if.uart[uim_instance].uart_version)
  {
    do
    {
      data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr + UART_DM_SR);
      uim_log_uartdm_register(uim_instance, UART_DM_SR, UIM_REG_READ, data);
      if(MSMU_SR_CMD_IN_PROG == (data & MSMU_SR_CMD_IN_PROG))
      {
        if( count >= UIM_MAX_NUM_SR_CHECK )
        {
          /* Future investigation: return ERR or SET command_rsp_sig to trigger recovery*/
          ERR_FATAL("EXCEED max number of SR checks: %d", (int32)count, 0, 0);
        }
        count++;
        /* Wait about 3 UART clk cycle (delta = 3*(1/f_UARTBITCLK~=1ms) and poll 
           SR for slowest clk(4.8MHz)*/
        uim_clk_busy_wait(1); 
      }
      else
      {
        break;
      }
    }while(1);
  }
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_CR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_CR, UIM_REG_WRITE, val);

  if (UIM_UART_DM_HW_VERSION_1_4_2 > uim_hw_if.uart[uim_instance].uart_version)
  {
    /* A delay to to give enough room for the UARTDM to process the
       command and be prepared for the subsequent commands / register
       accesses */
    uim_clk_busy_wait(100);
  }
} /* prgCR */


/* Command to Reset Receiver */
void resetRx(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_RESET_RX);
} /* resetRx */


/* Command to Reset Transmitter */
void resetTx(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_RESET_TX);
} /* resetTx */


/* Command to Reset Error Status, it clears the overrun error
   and hunt char received status bits
*/
void resetErr(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_RESET_ERR);
} /* resetErr */


/* Command to Reset break interrupt  */
void resetBreak(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_RESET_BRK_INT);
} /* resetBreak */


/* Command to start break, Forces the transmitter signal low.
   The transmitter must be enabled. If the transmitter is busy,
   the break is started when all characters in the transmit FIFO
   and the transmit shift register have been completely sent.
*/
void startBreak(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_STA_BRK);
} /* startBreak */


/* Command to stop break, this command causes the transmitter
   signal to go high.
*/
void stopBreak(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_STO_BRK);
} /* stopBreak */


/* Coommand to clear TX error */
void resetTxError(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_CLR_TX_ERROR);
} /* resetTxError */


/* command to turn on the sample data mode, which causes the receiver to
   sample the receive data stream at 16 times the programmed baud rate.
*/
void sampleMode(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_SAMP_MODE);
} /* sampleMode */


/* Command to disable Tx */
void disableTx(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_DIS_TX);
} /* disableTx */


/* Command to enable Tx*/
void enableTx(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_ENA_TX);
} /* enableTx */


/* Command to disable Rx */
void disableRx(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_DIS_RX);
} /* disableRx */


/* Command to enable Rx*/
void enableRx(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_ENA_RX);
} /* enableRx */


/* Command to Turns off the sample data mode */
void resetSampleMode(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_RESET_SAMPLE);
} /* resetSampleMode */


/* Command to Enable Stale Event */
void enableRxStaleInt(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_ENA_STALE_EVT);
} /* enableRxStaleInt */


/* Command to clear Stale Event */
void clearRxStaleInt(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_CLEAR_STALE_EVT);
} /* clearRxStaleInt */


/* Command to force Stale event */
void forceRxStaleInt(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_FORCE_STALE_EVT);
} /* forceRxStaleInt */


/* Command to clear the tx ready interrupt */
void clearTxReadyInt(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_CLR_TX_READY);
} /* clearTxReadyInt */


/* Command to clear the tx complete interrupt */
void clearTxCompInt(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_CLR_TX_COMP);
} /* clearTxCompInt */


/* Command to enable WWT counter */
void enableWwtCounter(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_WWT_CNTR_EN);
} /* enableWwtCounter */


/* Command to disable WWT counter */
void disableWwtCounter(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_WWT_CNTR_DIS);
} /* disableWwtCounter */


/*  Command to clear WWT interrupt in IRQ */
void clearWwtIntStatus(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_CLR_WWT_INT);
} /* clearWwtIntStatus */


/* Command to reset UART */
void resetUARTDM(uim_instance_enum_type uim_instance)
{
  prgCR(uim_instance, MSMU_CR_CLR_TX_READY | MSMU_CR_UART_RESET_INT |
      MSMU_CR_DIS_TX |MSMU_CR_DIS_RX);
} /* resetUARTDM */


/*
  UART_DM_ISR, Access: Read
  UART interrupt status register. This register provides the current status
  of the possible interrupt conditions (PAR_ERR, RX break/lev/stale/hunt ,
  Tx Done\Error\Ready\Lev etc). If one of these bits is set (1), and the
  corresponding bit in the UART_DM_IMR register is set (1), an interrupt
  is generated (with the exception of bit 6, CURRENT_CTS).
  If the corresponding bit in the UART_DM_IMR register is clear (0), setting
  one of these bits has no effect on the UART interrupt request signal.
  This register is updated asynchronously
*/
dword readISR(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr + UART_DM_ISR);
  uim_log_uartdm_register(uim_instance, UART_DM_ISR, UIM_REG_READ, data);
  return data;
} /* readISR */


/*
  UART_DM_IMR, Access: Write
  UART interrupt mask register. This register is used to enable the
  corresponding functions in the UART_DM_ISR register
*/
void  prgIMR(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nIMR = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_IMR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_IMR, UIM_REG_WRITE, val);
} /* prgIMR */


/*
  UART_DM_MISR, Access: Read
  Masked interrupt status register. A read from this register returns the
  AND of the UART_DM_ISR and the UART_DM_IMR registers.
  This register is updated asynchronously
*/
dword readMISR(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr + UART_DM_MISR);
  uim_log_uartdm_register(uim_instance, UART_DM_MISR, UIM_REG_READ, data);
  return data;
} /* readMISR */


/*
  UART_DM_IPR Access: Read\Write
  UART interrupt programming register, This register is used to configure
  stale_timeout info.
*/
void prgIPR(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nIPR= val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_IPR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_IPR, UIM_REG_WRITE, val);
} /* prgIPR */


/*
  UART_DM_TFWR Access: Read\Write
  This register is used to configure transmit Watermarks info
*/
void  prgTxWaterlevel(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nTFWR = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_TFWR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_TFWR, UIM_REG_WRITE, val);
} /* prgTxWaterlevel */


/*
  UART_DM_RFWR Access: Read\Write
  This register is used to configure receive Watermarks info
*/
void  prgRxWaterlevel(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nRFWR = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_RFWR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_RFWR, UIM_REG_WRITE, val);
} /* prgRxWaterlevel */


/*
  UART_DM_DMRX Access: Read\Write
  This register is used to configure number of chars to be received in
  new Rx transfer initialization
*/
void  prgRXInit(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nRXINIT = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_DMRX, val);
  uim_log_uartdm_register(uim_instance, UART_DM_DMRX, UIM_REG_WRITE, val);
} /* prgRXInit */


/* read the UART_DM_DMRX register */
dword readRxInit(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr + UART_DM_DMRX);
  uim_log_uartdm_register(uim_instance, UART_DM_DMRX, UIM_REG_READ, data);
  return data;
} /* readRxInit */


/*
  UART_DM_IRDA, Access: Write
  This register enables the IRDA function,
  Currently, it is not require for UIM communication
*/
void disableIRDA(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nIRDA= UART_IRDA_DISABLE;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_IRDA,
               UART_IRDA_DISABLE);
  uim_log_uartdm_register(uim_instance, UART_DM_IRDA, UIM_REG_WRITE, UART_IRDA_DISABLE);
} /* disableIRDA */


/*
  UART_DM_RX_TOTAL_SNAP Access: Read
  It holds the number of characters received since the end of last Rx transfer.
  Rx transfer ends when one of the conditions is met:
  1. the number of characters which were received since the end of the
     previous transfer equals the value which was written to DMRX at
     Transfer initialization.
  2. A stale event occurred
*/
dword getNumBytesInRX(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr +
                     UART_DM_RX_TOTAL_SNAP);
  uim_log_uartdm_register(uim_instance, UART_DM_RX_TOTAL_SNAP, UIM_REG_READ, data);
  return data;
} /* getNumBytesInRX */


/*
  UART_DM_NO_CHARS_FOR_TX Access: Read\Write
  This register contains total number of characters for transmission.
  It is used by the transmitter to calculate how many characters to
  transmit in the last word
*/
void  numCharToSend(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nTXNUM = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
               UART_DM_NO_CHARS_FOR_TX, val);
  uim_log_uartdm_register(uim_instance, UART_DM_NO_CHARS_FOR_TX, UIM_REG_WRITE, val);
} /* numCharToSend */


/* read the number of characters in a transmission */
dword getnumCharToSend(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr +
                     UART_DM_NO_CHARS_FOR_TX);
  uim_log_uartdm_register(uim_instance, UART_DM_NO_CHARS_FOR_TX, UIM_REG_READ, data);
  return data;
} /* getnumCharToSend */


/*
  UART_DM_BADR Access: Read\Write
  This register is used to configure the base address of RX FIFO
*/
void prgBADR(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nBADR = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
               UART_DM_BADR, val);
  uim_log_uartdm_register(uim_instance, UART_DM_BADR, UIM_REG_WRITE, val);
} /* prgBADR */


/*
  UART_DM_TXFS Access: Read
  This register contains number of bytes waiting in TX FIFO
*/
uint32 getNumBytesWaitingInTx(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr +
                     UART_DM_TXFS);
  uim_log_uartdm_register(uim_instance, UART_DM_TXFS, UIM_REG_READ, data);
  return data;
} /* getNumBytesWaitingInTx */


/*
  UART_DM_RXFS Access: Read
  This register contains number of bytes waiting in RX FIFO
*/
uint32 getNumBytesWaitingInRx(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr +
                     UART_DM_RXFS);
  uim_log_uartdm_register(uim_instance, UART_DM_RXFS, UIM_REG_READ, data);
  return data;
} /* getNumBytesWaitingInRx */


/*
  UART_DM_TF Access: Write
  This register is used to send word to TX FIFO
*/
void sendWord(uim_instance_enum_type uim_instance, dword val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_TF, val);
  uim_log_uartdm_register(uim_instance, UART_DM_TF, UIM_REG_WRITE, val);
} /* sendWord */


/*
  UART_DM_RF Access: Read
  This register is used to receive word from RX FIFO, After the read is
  completed, the FIFO read pointer is updated to make the next character
  available.
*/
dword getWord(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr + UART_DM_RF);
  uim_log_uartdm_register(uim_instance, UART_DM_RF, UIM_REG_READ, data);
  return data;
} /* getWord */


/*
  UART_DM_SIM_CFG Access: Read\Write
  UART_SIM_CFG register is responsible for configuring SIM/UIM  mode such
  as direct\indirect convention, Clock Stop High or Low, UIM TX\RX mode etc..
*/
void prgSIMCFG(uim_instance_enum_type uim_instance, dword cnfg_mask,
                dword cnfg_val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  uim_hw_if.uart[uim_instance].mRegShadow.nSIMCFG=
      (uim_hw_if.uart[uim_instance].mRegShadow.nSIMCFG & (dword)~(cnfg_mask))
      |((dword)((cnfg_val) & (cnfg_mask)));
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_SIM_CFG,
               uim_hw_if.uart[uim_instance].mRegShadow.nSIMCFG);
  uim_log_uartdm_register(uim_instance, UART_DM_SIM_CFG,
                          UIM_REG_WRITE, uim_hw_if.uart[uim_instance].mRegShadow.nSIMCFG);
} /* prgSIMCFG */


/*
  Program UIM Controller Config
*/
void prgUIMCFG(uim_instance_enum_type uim_instance, uint32 uim_cfg_reg_val,
                    boolean mask_required, uint32 mask_value, boolean set_clr)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);

  if(FALSE == mask_required)
  {
    uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG |= uim_cfg_reg_val;
  }
  else if (TRUE == mask_required)
  {
    if(SET == set_clr)
    {
      uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG =
          uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG | mask_value;
    }
    else if (CLR == set_clr)
    {
      uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG =
          uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG & mask_value;
    }
  }
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_UIM_CFG,
               uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG);
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_CFG,
                          UIM_REG_WRITE, uim_hw_if.uart[uim_instance].mRegShadow.nUIMCFG);
  /*
    wait until the write to the UIM controller register is successful
    while ( uimUartdmUimcontrollerIoStatus(slot) & HWIO_FMSK(UIM1_UART_DM_IO_STATUS,
    UIM_IO_WRITE_IN_PROGRESS ){} UIM_IO_WRITE_DONE interrupt is arriving 3
    sleep_clk cycles before new command can be issued". hence, when the
    while loop is completed, the write is still not guaranteed to have
    been successful.
  */
  uim_clk_busy_wait(200);
} /* prgUIMCFG */


/*
  UART DM Set WWT Timer
*/
void setWWT(uim_instance_enum_type uim_instance, uint32 val)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
/* For UART Ver1.4.2 enable feature of re-loading the WWT counter upon receiving a new character*/
  if (UIM_UART_DM_HW_VERSION_1_4_2 <= uim_hw_if.uart[uim_instance].uart_version)
  {
    val = val | MSMU_WWT_CYC_REENA_RX;
  }
  uim_hw_if.uart[uim_instance].mRegShadow.nWWT = val;
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_WWT_TIMEOUT, val);
  uim_log_uartdm_register(uim_instance, UART_DM_WWT_TIMEOUT, UIM_REG_WRITE, val);
} /* setWWT */


/*
  UART DM Reset UART
*/
void resetUart(uim_instance_enum_type uim_instance)
{
    /* The 100us delays between writes to the GSBI1 command register were added
       due to a crash seen during concurrency with a SPI transfer */
    uim_clk_busy_wait(100);
    resetRx(uim_instance);
    uim_clk_busy_wait(100);
    resetTx(uim_instance);
    uim_clk_busy_wait(100);
    resetUARTDM(uim_instance);
    uim_clk_busy_wait(100);
} /* resetUart */


/* Enable receive character mode */
void enableRxCharMode(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
               UART_DM_DMEN, 0x20);
  uim_log_uartdm_register(uim_instance, UART_DM_DMEN, UIM_REG_WRITE, 0x20);
} /* enableRxCharMode */


/*
  Program UARTDM UIM Controller Card Detection Interrupt masking
  and clearing
*/
void uimUartdmUimControllerMaskCarddetInt(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
    UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
                 UART_DM_UIM_IRQ_IMR, 0x0); /* mask card insertion / removal interrupts */
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_IRQ_IMR, UIM_REG_WRITE, 0x0);
    uim_clk_busy_wait(200);
} /* uimUartdmUimControllerMaskCarddetInt */


/* Clear uim controller card detect interrupt */
void uimUartdmUimControllerClearCarddetInt(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
                 UART_DM_UIM_IRQ_CLR, 0x3); /* mask card insertion / removal interrupts */
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_IRQ_CLR, UIM_REG_WRITE, 0x3);
  uim_clk_busy_wait(200);
} /* uimUartdmUimControllerClearCarddetInt */


/*
  UIM Controller Initialize HW deactivation
*/
void uimUartdmUimControllerInitHwDeact(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
    UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
                 UART_DM_UIM_CMD, 0x1); /* initialize hw deact for slot 1*/
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_CMD, UIM_REG_WRITE, 0x1);
    uim_clk_busy_wait(200);
}/* uimUartdmUimControllerInitHwDeact */


/*
  Recover UIM Controller
*/
void uimUartdmUimControllerRecover(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
    UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
                 UART_DM_UIM_CMD, 0x2); /* enable card removal interrupt */
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_CMD, UIM_REG_WRITE, 0x2);
    uim_clk_busy_wait(200);
} /* uimUartdmUimControllerRecover */


/*
  Read UIM Controller IO status
*/
dword uimUartdmUimcontrollerIoStatus(uim_instance_enum_type uim_instance)
{
  dword data = 0;
  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_instance, data);
  data = UIM_HWIO_IN(uim_hw_if.uart[uim_instance].start_addr +
                     UART_DM_UIM_IO_STATUS);
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_IO_STATUS, UIM_REG_READ, data);
  return data;
} /* uimUartdmUimcontrollerIoStatus */


/* Enable card removal interrupt */
void uimUartdmUimControllerCardRemInt(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
                UART_DM_UIM_IRQ_IMR, 0x1); /* enable card removal interrupt */
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_IRQ_IMR, UIM_REG_WRITE, 0x1);
} /* uimUartdmUimControllerCardRemInt */


/* Enable card insertion interrupt */
void uimUartdmUimControllerCardInsInt(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
               UART_DM_UIM_IRQ_IMR, 0x2); /* enable card insertion interrupt */
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_IRQ_IMR, UIM_REG_WRITE, 0x2);
} /* uimUartdmUimControllerCardInsInt */


/* Unmask hotswap interrupt */
void uimUartdmUimControllerUnmaskHotswapInt(uim_instance_enum_type uim_instance)
{
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr +
               UART_DM_UIM_IRQ_IMR_SET, 0x3); /* unmask hotswap interrupt */
  uim_log_uartdm_register(uim_instance, UART_DM_UIM_IRQ_IMR_SET, UIM_REG_WRITE, 0x3);
} /* uimUartdmUimControllerUnmaskHotswapInt */


/* Program the clock control register*/
void uim_prog_clk_ctrl_reg(uim_instance_enum_type uim_instance, uint32 val)
{
  if (uim_instance >= UIM_INSTANCE_INVALID)
  {
    return;
  }
  VALIDATE_UIM_UART_START_ADDRESS(uim_instance);
  UIM_HWIO_OUT(uim_hw_if.uart[uim_instance].start_addr + UART_DM_CLK_CTRL, val);
  uim_log_uartdm_register(uim_instance, UART_DM_CLK_CTRL, UIM_REG_WRITE, val);
  return;
} /* uim_prog_clk_ctrl_reg */


/* Function to configure UIM controller */
void uim_uartdm_uim_controller_config(uim_instance_global_type *uim_ptr,
                                      boolean hotswap_enabled)
{
  uint32 uim_cfg_reg_value = (UIM_EVENT_DEBOUNCE_TIME | UIM_MODE_18);

/* BIT  NAME                   Description
   10   PMIC_ALARM_EN          Enables messaging on the alarm-line to notify PMIC to drop
                               card’s voltage supply. Set to enable messaging, clear to
                               disable.

   9    BATT_ALARM_TRIGGER_EN  Enables triggering of deactivation sequence when batteryalarm
                               detected. Set to enable triggering, clear to disable.

   8    UIM_RMV_TRIGGER_EN     Enables triggering of deactivation sequence when cardremoval
                               detected. Set to enable triggering, clear to disable.  */

  if (TRUE == hotswap_enabled)
  {
    uim_cfg_reg_value |= (  UIM_PMIC_ALARM_EN | UIM_CARD_EVENTS_EN ) ;
  }

  if(TRUE == uim_hw_if.pmic[uim_ptr->id].m_uiccShutBattRemoval)
  {
    uim_cfg_reg_value |= ( UIM_BATT_ALARM_TRIGGER_EN | UIM_PMIC_ALARM_EN );
  }

  if(  TRUE == uim_hw_if.pmic[uim_ptr->id].m_uiccShutCardRemoval &&
       TRUE == uim_hw_if.intctrl[uim_ptr->id].m_hs_enable &&
       /* If flaky hotswap switch support is TRUE then do not set
          UICC shutdown upon card removal feature */
       !(uim_ptr->hotswap.hotswap_flaky_switch_support))
  {
    uim_cfg_reg_value |= ( UIM_REMOVED_TRIGGER_EN | UIM_PMIC_ALARM_EN );
  }

  prgUIMCFG(uim_ptr->id,uim_cfg_reg_value,0,0,0);
  return;
} /* uim_uartdm_uim_controller_config */


/* Function mask card detect interrupt */
void uim_uartdm_uim_controller_mask_carddet_int(uim_instance_global_type *uim_ptr)
{
  uimUartdmUimControllerMaskCarddetInt(uim_ptr->id);
  return;
} /* uim_uartdm_uim_controller_mask_carddet_int */


/* Clear the controller card detect interrupt */
void uim_uartdm_uim_controller_clear_carddet_int(uim_instance_global_type *uim_ptr)
{
  uimUartdmUimControllerClearCarddetInt(uim_ptr->id);
  return;
} /* uim_uartdm_uim_controller_clear_carddet_int */


/* UIM Controller Initialize HW deactivation */
void uim_uartdm_uim_controller_init_hw_deact(uim_instance_enum_type uim_instance)
{
  uimUartdmUimControllerInitHwDeact(uim_instance);
  return;
} /* uim_uartdm_uim_controller_init_hw_deact */


/* Function to recover the UIM controller */
void uim_uartdm_uim_controller_recover(uim_instance_global_type *uim_ptr)
{
  uimUartdmUimControllerRecover(uim_ptr->id);
  return;
} /* uim_uartdm_uim_controller_recover */


/* Function to set WWT value */
void uim_uartdm_set_wwt_val(uim_instance_global_type *uim_ptr, uint32 wwt_val)
{
  setWWT(uim_ptr->id, wwt_val);
  return;
} /* uim_uartdm_set_wwt_val */


/* Function to enable WWT counter */
void uim_uartdm_enable_wwt_counter(uim_instance_global_type *uim_ptr)
{
  enableWwtCounter(uim_ptr->id);
  return;
} /* uim_uartdm_enable_wwt_counter */


/* Function disable WWT counter */
void uim_uartdm_disable_wwt_counter(uim_instance_global_type *uim_ptr)
{
  disableWwtCounter(uim_ptr->id);
  return;
} /* uim_uartdm_disable_wwt_counter */


/* Function to clear WWT interrupt */
void uim_uartdm_clear_wwt_int_status(uim_instance_global_type *uim_ptr)
{
  clearWwtIntStatus(uim_ptr->id);
  return;
} /* uim_uartdm_clear_wwt_int_status */


/* Set the hotswap polarity */
void uim_set_hotswap_polarity(uim_instance_enum_type uim_instance,
                              DALGpioValueType value)
{
  if(SET == value)
  {
    prgUIMCFG(uim_instance, 0, TRUE, UIM_SET_PRESENT_POLARITY_BIT, value);
  }
  else if(CLR == value)
  {
    prgUIMCFG(uim_instance, 0, TRUE, UIM_CLR_PRESENT_POLARITY_BIT, value);
  }
  return;
} /* uim_set_hotswap_polarity */


/* Return the status of the UIM Controller card presence */
uim_card_swap_enum_type uim_uartdm_uim_controller_card_presence
(
   uim_instance_global_type *uim_ptr
)
{
  volatile uim_card_swap_enum_type uim_presence_status = UIM_CARD_UNCHANGED;

  VALIDATE_UIM_UART_START_ADDRESS_ELSE_RETURN(uim_ptr->id, uim_presence_status);

  uim_presence_status =
    (uim_card_swap_enum_type)(UIM_HWIO_IN(
       uim_hw_if.uart[uim_ptr->id].start_addr +
       UART_DM_UIM_IO_STATUS) & UIM_CARD_INSERTED);
  uim_log_uartdm_register(uim_ptr->id, UART_DM_UIM_IO_STATUS,
                          UIM_REG_READ, uim_presence_status);

  return uim_presence_status;
} /* uim_uartdm_uim_controller_card_presence */


/* Enable card insertion interrupt */
void uim_uartdm_uim_controller_card_ins_int
(
  uim_instance_enum_type uim_instance,
  DALGpioValueType uim_ins_level
)
{
  uimUartdmUimControllerCardInsInt(uim_instance);
  uim_clk_busy_wait(200);
  return;
} /* uim_uartdm_uim_controller_card_ins_int */


/* Enable card removal interrupt */
void uim_uartdm_uim_controller_card_rem_int(uim_instance_enum_type uim_instance,
                                            DALGpioValueType uim_ins_level)
{
  uimUartdmUimControllerCardRemInt(uim_instance);
  uim_clk_busy_wait(200);
  return;
} /* uim_uartdm_uim_controller_card_rem_int */


/* Return the number of bytes in the last RX transfer and
   add any bytes in packing buffer */
int32 uim_bytes_waiting_in_rxfifo(uim_instance_global_type *uim_ptr)
{
  int32 bytes = 0;
  int32 num_bytes = 0;

  /* Read the RXFS register to get the number of bytes
     waiting in the RX FIFO and the packing buffer */
  bytes = UIM_GET_NUM_BYTES_WAITING_IN_RX_SLOT( uim_ptr );

  /* Calculate the count of bytes in the RX FIFO and packing buffer */
  num_bytes =
   ( 4 * ( (bytes & UART_DM_RXFS__RXFIFO_STATE_LSB_MASK)
     >> UART_DM_RXFS__RXFIFO_STATE_LSB_SHFT) ) +
   ( (bytes & UART_DM_RXFS__RX_BUFFER_STATE_MASK)
     >> UART_DM_RXFS__RX_BUFFER_STATE_SHFT) ;

  return num_bytes;
} /* uim_bytes_waiting_in_rxfifo */


/* Get the number of bytes in the transfer FIFO */
int32 uim_bytes_waiting_in_txfifo(uim_instance_global_type *uim_ptr)
{
  int32 bytes = 0;
  int32 num_bytes = 0;

  /* Read the TXFS register to get the number of bytes
     waiting in the TX FIFO and the un-packing buffer */
  bytes = (int32)UIM_GET_NUM_BYTES_WAITING_IN_TX_SLOT( uim_ptr );

  /* Calculate the count of bytes in the TX FIFO and un-packing buffer */
  num_bytes =
   ( 4 * ( (bytes & UART_DM_TXFS__TXFIFO_STATE_LSB_MASK)
     >> UART_DM_TXFS__TXFIFO_STATE_LSB_SHFT) ) +
   ( (bytes & UART_DM_TXFS__TX_BUFFER_STATE_MASK)
     >> UART_DM_TXFS__TX_BUFFER_STATE_SHFT) ;

  return num_bytes;
} /* uim_bytes_waiting_in_txfifo */


/*===========================================================================
  FUNCTION UIM_SET_UARTDM_ACTIVE_MODE

  DESCRIPTION
    Enables the UIM2 clocks: SIM, UART, HCLK.
===========================================================================*/
void UIM_SET_UARTDM_ACTIVE_MODE(uim_instance_global_type *uim_ptr)
{
  /* Set default UART clock frequency to let the subsequent CR register
     access to not fail */
  SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, uim_ptr->state.FI_current,
                           uim_ptr->state.DI_current);

  /* Disable clock gating to allow command processing */
  uim_prog_clk_ctrl_reg(uim_ptr->id, UIM_CLK_CTRL__DISABLE_CGC);

  /* Enable UART TX */
  UIM_ENABLE_UART_TX_SLOT( uim_ptr );
  /* Enable UART RX */
  UIM_ENABLE_UART_RX_SLOT( uim_ptr );
  return;
} /* UIM_SET_UARTDM_ACTIVE_MODE */

/*===========================================================================
  FUNCTION UIM_SET_UARTDM_INACTIVE_MODE

  DESCRIPTION
    Enables the UIM2 clocks: SIM, UART, HCLK.
===========================================================================*/
void UIM_SET_UARTDM_INACTIVE_MODE(uim_instance_global_type *uim_ptr)
{
  /* Set default UART clock frequency to let the subsequent CR register
     access to not fail */
  SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, uim_ptr->state.FI_current,
                           uim_ptr->state.DI_current);

  /* Enable clock gating for RX and TX logic */
  uim_prog_clk_ctrl_reg(uim_ptr->id, (UIM_CLK_CTRL__RX_ENABLE_CGC_OPT |
                                       UIM_CLK_CTRL__TX_ENABLE_CGC_OPT));

  /* Disable UART TX */
  UIM_DISABLE_UART_TX_SLOT( uim_ptr );
  /* Disable UART RX */
  UIM_DISABLE_UART_RX_SLOT( uim_ptr );
  return;
} /* UIM_SET_UARTDM_INACTIVE_MODE */

/*===========================================================================
  FUNCTION UIM_SET_UARTDM_BCR_REG_PER_VER

  DESCRIPTION
    Programs the BCR Register to enable all features.
===========================================================================*/
void UIM_SET_UARTDM_BCR_REG_PER_VER(uim_instance_global_type *uim_ptr)
{
  dword val = MSMU_BCR_UART_1_4_0;
  if (uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    return;
  }
  VALIDATE_UIM_UART_START_ADDRESS(uim_ptr->id);
  /* For UART Ver1.4.2 enable feature of re-loading the WWT counter upon receiving a new character*/
  if (UIM_UART_DM_HW_VERSION_1_4_2 <= uim_hw_if.uart[uim_ptr->id].uart_version)
  {
    val = val | MSMU_BCR_NEGLET_NO_CR_PROT;
  }
  UIM_HWIO_OUT(uim_hw_if.uart[uim_ptr->id].start_addr +
               UART_DM_BCR, val);
  uim_log_uartdm_register(uim_ptr->id, UART_DM_BCR, UIM_REG_WRITE, val);
}/* UIM_SET_UARTDM_BCR_REG_PER_VER */


/**
*  This function returns the CSR register value to write in
*  order to program a particular divider value.
*
*  @param bit_clk_div_factor The uart clock divider factor which
*                            produces the bit clock rate from
*                            the uart clock.
*
*  @return uint32 The CSR register value to write.
*/
uint32 uim_find_csr_value(uint32 bit_clk_div_factor)
{
  int i = 0;

  for (i=0;i<ARR_SIZE(bit_clk_div_to_uart_csr_map);i++)
  {
    if (bit_clk_div_to_uart_csr_map[i].bit_clk_div == bit_clk_div_factor)
    {
      return bit_clk_div_to_uart_csr_map[i].uart_csr_value;
    }
  }

  UIM_MSG_ERR_1("Invalid bit_clk_div_factor %d",bit_clk_div_factor);
  return 0xFF; /* default to div factor of 16 */
} /* find_csr_value */


/**
*  This function writes the appropriate value to the UART_DM_CSR
*  register given a uart clock divider value that needs to be
*  programmed.
*
*  @param bit_clk_div_factor The uart clock divider factor to be
*                            programmed
*  @param uim_instance The uim instance number that we are
*                      operating on.
*/
void uim_prg_uart_csr_by_div_factor(uint32 bit_clk_div_factor,
                                    uim_instance_enum_type uim_instance)
{
  uint32 csr_value = 0;

  csr_value = uim_find_csr_value(bit_clk_div_factor);

  prgCSR(uim_instance, csr_value);

}
