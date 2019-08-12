/*=============================================================================

FILE:         qdss_ctrl_diag.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_ctrl_diag.h"
#include "qdss_ctrl_utils.h"


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


PACK(void *) qdss_ctrl_diag_pkt_handler(PACK(void *) pReq, uint16 pkt_len)
{
   qdss_ctrl_diag_pkt_hdr *pHdr;
   PACK(void *)pRsp = NULL;

   if (NULL != pReq)    {
      pHdr = (qdss_ctrl_diag_pkt_hdr *)pReq;

      switch (pHdr->subsysCmdCode & 0x0FF) {

      case QDSS_CTRL_FILTER_ETM:
         QDSS_HANDLE_DIAG_CMD(qdss_ctrl_filter_etm);
         break;

      case QDSS_CTRL_FILTER_ETM_RPM:
         QDSS_HANDLE_DIAG_CMD(qdss_ctrl_filter_etm_rpm);
         break;

      default:
         pRsp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pReq, pkt_len);
         break;
      }

      if (NULL != pRsp) {
         diagpkt_commit(pRsp);
         pRsp = NULL;
      }
   }
   return (pRsp);
}

PACK(void *) qdss_ctrl_diag_pkt_etm_handler(PACK(void *) pReq, uint16 pkt_len)
      {
   qdss_ctrl_diag_pkt_hdr *pHdr;
   PACK(void *)pRsp = NULL;
   qdss_ctrl_etm_mode_req *etm_req = NULL;

   if (NULL != pReq) {
		  pHdr = (qdss_ctrl_diag_pkt_hdr *)pReq;

		  if ((pHdr->subsysCmdCode & 0x0FF) == QDSS_CTRL_ETM_CONFIG ) {
		  
				etm_req = (qdss_ctrl_etm_mode_req *)pReq;
				switch (etm_req->etm_cmd_id) {
				  case QDSS_ETM_CONFIG_TESTBUS:
					 QDSS_HANDLE_DIAG_CMD(qdss_ctrl_etm_testbus);
					 break;
					 
				  case QDSS_ETM_CONFIG_DEFAULT_MODE:
					 QDSS_HANDLE_DIAG_CMD(qdss_ctrl_etm_default);
					 break;
					 
				  default:
					 pRsp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pReq, pkt_len);
					 break;
				}
		  }

		  if (NULL != pRsp) {
			 diagpkt_commit(pRsp);
			 pRsp = NULL;
		  }
   }
   return (pRsp);
}

PACK(void *) qdss_ctrl_diag_pkt_handler_ext(PACK(void *) pReq, uint16 pkt_len)
      {
   qdss_ctrl_diag_pkt_hdr *pHdr;
   PACK(void *)pRsp = NULL;

   if (NULL != pReq) {
      pHdr = (qdss_ctrl_diag_pkt_hdr *)pReq;

      switch (pHdr->subsysCmdCode & 0x0FF) {

      case QDSS_CTRL_TEST_CMD:
         QDSS_HANDLE_DIAG_CMD(qdss_ctrl_test);
         break;

      default:
         pRsp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pReq, pkt_len);
         break;
      }

      if (NULL != pRsp) {
         diagpkt_commit(pRsp);
         pRsp = NULL;
      }
   }
   return (pRsp);
}


// Diag packet service - callback tables.
static const diagpkt_user_table_entry_type qdss_ctrl_diag_pkt_tbl[] =
   {
      { QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_FILTER_ETM,    // range: from
        QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_FILTER_ETM,    // range: to
        qdss_ctrl_diag_pkt_handler },                    // handler fcn
		
	  { QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_ETM_CONFIG,    // range: from
        QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_ETM_CONFIG,    // range: to
        qdss_ctrl_diag_pkt_etm_handler },                    // handler fcn
		
		
      { QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_FILTER_ETM_RPM,  // range: from
        QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_FILTER_ETM_RPM,  // range: to
        qdss_ctrl_diag_pkt_handler },                       // handler fcn

      { QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_TEST_CMD,  // range: from
        QDSS_CTRL_DIAG_PROC_ID | QDSS_CTRL_TEST_CMD,  // range: to
        qdss_ctrl_diag_pkt_handler_ext }              // handler fcn
   };


// Initialize interface with diag packet service
void qdss_ctrl_diag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (
     (diagpkt_subsys_id_type) DIAG_SUBSYS_QDSS, qdss_ctrl_diag_pkt_tbl);

  return;
}

