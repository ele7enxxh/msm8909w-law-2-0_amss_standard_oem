/*=============================================================================

FILE:         halqdss_csr.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/csr/hal/src/halqdss_csr.c#1 $
==============================================================================*/
#include "halqdss_csr.h"
#include "qdss_csr_regs.h"


volatile uint32 QDSS_QDSSCSR_REG_BASE;


void HAL_qdss_csr_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_QDSSCSR_REG_BASE = nBaseAddr;
}

void  HAL_qdss_csr_AccessUnLock(void)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_LOCKACCESS,0xC5ACCE55);   
}

void  HAL_qdss_csr_AccessLock(void)
{ 
   HWIO_OUT(QDSS_CS_QDSSCSR_LOCKACCESS,(~0xC5ACCE55));   
}


void HAL_qdss_csr_SetBlkSize(uint8 blksize)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBBAMCTRL,BLKSIZE,blksize);
}


void HAL_qdss_csr_SetPeriodicFlushThreshold(uint16 nETRBAMCycles)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,PERFLSHTHRS,nETRBAMCycles);
}

void HAL_qdss_csr_EnablePeriodicFlush(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,PERFLSH,1);
}


void HAL_qdss_csr_EnablePeriodicFlushEOT(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,PERFLSHEOT,1);
}


void HAL_qdss_csr_ManualEOT(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBFLSHCTRL,EOT,1);
}

void HAL_qdss_csr_EnableUSB(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBBAMCTRL,USBENAB,1);
}

void HAL_qdss_csr_DisableUSB(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_USBBAMCTRL,USBENAB,0);
}



boolean  HAL_qdss_csr_IsUSBEnabled(void)
{
   return ((boolean) HWIO_INF(QDSS_CS_QDSSCSR_USBBAMCTRL,USBENAB));
}


void HAL_qdss_csr_SetAWIDs(uint32 stm_awids) 
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMAWIDCTRL,stm_awids);
}


void HAL_qdss_csr_EnableModemMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB1,1);
}

void HAL_qdss_csr_DisableModemMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB1,0);
}

void HAL_qdss_csr_SetModemChannelOffset(uint32 offset) 
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMCHNOFST0,STMCHNOFST1,offset);
}
void HAL_qdss_csr_EnableBamDMAMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB2,1);
}

void HAL_qdss_csr_DisableBamDMAMaster(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMTRANSCTRL,STMTRANENAB2,0);
}

void HAL_qdss_csr_SetBamDMAChannelOffset(uint32 offset) 
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_STMCHNOFST0,STMCHNOFST2,offset);
}


//Bit mask representing burst masters to enable
void HAL_qdss_csr_EnableBurstMasters(uint32 burst_masters)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMTRANSCTRL,burst_masters);
}

void HAL_qdss_csr_DisableAllBurstMasters(void)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMTRANSCTRL,0x0);
}

void HAL_qdss_csr_DisableTimestampGenerator(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,TIMESTAMPDISABLE,1);
}

void HAL_qdss_csr_EnableTimestampGenerator(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,TIMESTAMPDISABLE,0);
}

void HAL_qdss_csr_TriggerTimestampCapture(void)
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,CAPTURE,0);
   HWIO_OUTF(QDSS_CS_QDSSCSR_TIMESTAMPCTRL,CAPTURE,1);
}

uint64 HAL_qdss_csr_GetTimestampCounter(void) 
{
   return (((uint64)(HWIO_IN(QDSS_CS_QDSSCSR_QDSSTIMEVAL1)) << 32) |
           HWIO_IN(QDSS_CS_QDSSCSR_QDSSTIMEVAL0));
}

void HAL_qdss_csr_SetTimestampCounter(uint64 timeval) 
{
   HWIO_OUT(QDSS_CS_QDSSCSR_QDSSTIMELOAD1,((uint32)(timeval >> 32)));
   HWIO_OUT(QDSS_CS_QDSSCSR_QDSSTIMELOAD0,((uint32)(timeval & 0xffffffff)));
}


uint64 HAL_qdss_csr_GetMPMCounter(void) 
{
   return (((uint64)(HWIO_INF(QDSS_CS_QDSSCSR_AOTIMEVAL1,TIMEVAL)) << 32) |
           HWIO_IN(QDSS_CS_QDSSCSR_AOTIMEVAL0));
}

void HAL_qdss_csr_mux_ctrl_0(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL0, val);
}

void HAL_qdss_csr_mux_ctrl_1(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL1, val);
}

void HAL_qdss_csr_mux_ctrl_2(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL2, val);
}

void HAL_qdss_csr_mux_ctrl_3(uint32 val)
{
   HWIO_OUT(QDSS_CS_QDSSCSR_STMEXTHWCTRL3, val);
}

//These are incorrect in auto generated files
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_PWRUPIGNORE_BMSK           0x2
#define HWIO_QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE_PWRUPIGNORE_SHFT           0x1

void HAL_qdss_csr_PwrReqIgnore(void) 
{
   HWIO_OUTF(QDSS_CS_QDSSCSR_QDSSPWRREQIGNORE,PWRUPIGNORE,1);
}
