/**
 * HW event driver provides an interface to configure various registers
 * that select HW events from different sub-systems.
 */
/*===========================================================================
  FILE: hwevent_cntrl.c

  OVERVIEW:     Diag interface for HW event driver
  
  DEPENDENCIES: Qdss hardware initialized (STM, Funnel, and ETB devices).
  
               Copyright (c) 2013 QUALCOMM Technologies, Inc.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/hwevent/dal/src/hwevent_cntrl.c#1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-05-12 ps Initial revision.
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "comdef.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "qdss.h"
#include "DALSysTypes.h"
#include "DDIHWEvent.h"
#include "qdss_control.h"


/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define QDSSDIAG_PROCESSOR_APPS   0x0100
#define QDSSDIAG_PROCESSOR_MODEM  0x0200
#define QDSSDIAG_PROCESSOR_WCNSS  0x0300
#define QDSSDIAG_PROCESSOR_RPM    0x0400
#define QDSSDIAG_PROCESSOR_LPASS  0x0500
#define QDSSDIAG_PROCESSOR_VENUS  0x0600
#define QDSSDIAG_PROCESSOR_GNSS   0x0700
#define QDSSDIAG_PROCESSOR_SENSOR 0x0800
#define QDSSDIAG_PROCESSOR_AUDIO  0x0900

/**
 * HWEvent Diag command codes: lower 8-bits of subsysCmdCode
 * Ensure that this command code isn't taken by other QDSS diag components
 */
#define QDSS_FILTER_HWEVENT_CONFIGURE  0x31


#define MODEM  1
#define ADSP   2
#define PRONTO 3
#define VENUS  4
#define RPM    5
#define APPS   6
#define SENSOR 7
#define AUDIO  8
#if (EVENT_SUBSYSTEM == MODEM)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_MODEM
#elif (EVENT_SUBSYSTEM == ADSP)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_LPASS
#elif (EVENT_SUBSYSTEM == PRONTO)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_WCNSS
#elif (EVENT_SUBSYSTEM == VENUS)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_VENUS
#elif (EVENT_SUBSYSTEM == RPM)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_RPM
#elif (EVENT_SUBSYSTEM == APPS)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_APPS
#elif (EVENT_SUBSYSTEM == SENSOR)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_SENSOR
#elif (EVENT_SUBSYSTEM == AUDIO)
   #define HWEVENT_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_AUDIO
#else
   #define HWEVENT_DIAG_PROC_ID   0x0
#endif
#undef MODEM
#undef ADSP
#undef PRONTO
#undef VENUS
#undef RPM
#undef APPS
#undef SENSOR
#undef AUDIO



/*Poke registers with a given value for selection of a a HW event*/
#define QDSS_HWEVENT_CONFIGURE_DIAG  0x31


#define QDSS_HANDLE_DIAG_CMD(cmd)                              \
   if (pkt_len < sizeof(cmd##_req)) {                          \
      pRsp = diagpkt_err_rsp(DIAG_BAD_LEN_F, pReq, pkt_len);   \
   }                                                           \
   else {                                                      \
      pRsp =  diagpkt_subsys_alloc(DIAG_SUBSYS_QDSS,           \
                                   pHdr->subsysCmdCode,        \
                                   sizeof(cmd##_rsp));         \
      if (NULL != pRsp) {                                      \
         cmd##_handler((cmd##_req *)pReq,                      \
                       pkt_len,                                \
                       (cmd##_rsp *)pRsp,                      \
                       sizeof(cmd##_rsp));                     \
      }                                                        \
   }

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
typedef PACK(struct)
{
   uint8 cmdCode;        /**< Diag Message ID */
   uint8 subsysId;       /**< Subsystem ID (DIAG_SUBSYS_QDSS)*/
   uint16 subsysCmdCode; /**< Subsystem command code */
} qdss_diag_pkt_hdr;
   
   
typedef PACK(struct)
{
   qdss_diag_pkt_hdr hdr;
   uint32 register_addr;
   uint32 register_value;
} qdss_hwevent_configure_diag_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr; /**< Header */
  uint8 result;            /**< See QDSS_CMDRESP_... definitions */
} qdss_diag_pkt_rsp;

   
typedef qdss_diag_pkt_rsp qdss_hwevent_configure_diag_rsp; 
   

PACK(void *) qdss_hwevent_diag_pkt_handler(PACK(void *) pRqstPkt, uint16 pkt_len);

int qdss_hwevent_configure_diag_handler(qdss_hwevent_configure_diag_req *pReq,
                          int req_len,
                          qdss_hwevent_configure_diag_rsp *pRsp,
                          int rsp_len); 


/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

// Diag packet service - callback table.
static const diagpkt_user_table_entry_type hwevent_diag_pkt_tbl[] =
{
   { HWEVENT_DIAG_PROC_ID | QDSS_HWEVENT_CONFIGURE_DIAG,// range: from
     HWEVENT_DIAG_PROC_ID | QDSS_HWEVENT_CONFIGURE_DIAG,         // range: to
     qdss_hwevent_diag_pkt_handler }                          // handler fcn
};


static DALDEVICEHANDLE hHWEvent;


/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

PACK(void *) qdss_hwevent_diag_pkt_handler(PACK(void *) pReq, uint16 pkt_len)
{
   qdss_diag_pkt_hdr *pHdr;
   PACK(void *)pRsp = NULL;


   if (NULL != pReq)    {
      pHdr = (qdss_diag_pkt_hdr *)pReq;

      switch (pHdr->subsysCmdCode & 0x0FF) {

      case QDSS_HWEVENT_CONFIGURE_DIAG:
         QDSS_HANDLE_DIAG_CMD(qdss_hwevent_configure_diag);
         break;

      default:
         pRsp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pReq, pkt_len);
         break;
      }

      if (NULL != pRsp)
      {
         diagpkt_commit(pRsp);
         pRsp = NULL;
      }
   }
   return (pRsp);
}

int qdss_control_set_hwe_reg(unsigned long addr, unsigned long val)
{
   int nErr = QDSS_HWEVENT_UNKNOWN_ERR;
/*
**Supports ONLY tethered mode.
**QDSS clocks are enabled by the Apps processor by means of a QTF command to 
**the Apps processor. 
*/
   if (NULL == hHWEvent)
   {
      if (DAL_SUCCESS !=
          DAL_HWEventDeviceAttach("DALDEVICEID_HWEVENT",&hHWEvent))
      {
         return nErr;
      }
   }
      
   
   if(DAL_SUCCESS!= DAL_HWEvent_SetRegister(hHWEvent,
                                              addr,
                                              val,
                                              (uint8 *)&nErr))
   {
      return nErr;
   }
   nErr = QDSS_HWEVENT_CONFIG_SUCCESS;
   return nErr;
}

int qdss_hwevent_configure_diag_handler(qdss_hwevent_configure_diag_req *pReq,
                          int req_len,
                          qdss_hwevent_configure_diag_rsp *pRsp,
                          int rsp_len)
{
   int nErr;
   nErr = qdss_control_set_hwe_reg(pReq->register_addr,pReq->register_value);
   pRsp->result = nErr;
   return nErr;
}

// Initialize interface with diag packet service
void hwevent_diag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (
     (diagpkt_subsys_id_type) DIAG_SUBSYS_QDSS, hwevent_diag_pkt_tbl);
  return;
}

