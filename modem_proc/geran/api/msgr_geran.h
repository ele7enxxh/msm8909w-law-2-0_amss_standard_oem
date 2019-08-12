/*!
  @file msgr_geran.h

  @brief Defines the GERAN modules that use Message Router
*/

/*===========================================================================

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

#ifndef MSGR_GERAN_H
#define MSGR_GERAN_H

/*===========================================================================

  INCLUDE FILES

===========================================================================*/

#include "msgr.h"
#include <msgr_umid.h>  /* UMID definitions */
#include "rfcom.h"

/*===========================================================================

  EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define MAX_CM_RSSI_MEASUREMENTS_REQ  10
/*** IMPORTANT
 ***
 *** GERAN GFW USES MODULE ID's 0x0 and 0x1, don't use them here
 **/
#define MSGR_MODULE_GRR 0x02
#define MSGR_MODULE_GL1 0x03
#define MSGR_MODULE_RF_TASK   0x04
#define MSGR_MODULE_RF_TASK_2 0x05
#define MSGR_MODULE_RF_TASK_3 0x06
#define MSGR_MODULE_GL1_MSGRIF_TASK_1   0x07
#define MSGR_MODULE_GL1_MSGRIF_TASK_2   0x08
#define MSGR_MODULE_GL1_MSGRIF_TASK_3   0x09


#define MSGR_GERAN_GRR MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_GRR)
#define MSGR_GERAN_GL1 MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_GL1)
#define MSGR_GERAN_RF_TASK MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_RF_TASK)
#define MSGR_GERAN_RF_TASK_2 MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_RF_TASK_2)
#define MSGR_GERAN_RF_TASK_3 MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_RF_TASK_3)
#define MSGR_GERAN_GL1_MSGRIF_TASK_1 MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_GL1_MSGRIF_TASK_1)
#define MSGR_GERAN_GL1_MSGRIF_TASK_2 MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_GL1_MSGRIF_TASK_2)
#define MSGR_GERAN_GL1_MSGRIF_TASK_3 MSGR_TECH_MODULE(MSGR_TECH_GERAN, MSGR_MODULE_GL1_MSGRIF_TASK_3)


enum
{
  MSGR_DEFINE_UMID(GERAN, GL1, RSP, ACQ, 0x0,
                   gsm_irat_acquisition_rsp_t),

  MSGR_DEFINE_UMID(GERAN, GL1, RSP, SCH, 0x1,
                   gsm_irat_sch_burst_rsp_t),

  MSGR_DEFINE_UMID(GERAN, GL1, RSP, MEAS, 0x2,
                    gsm_irat_meas_search_rsp_t),
   
  
 MSGR_DEFINE_UMID(GERAN, GL1 , REQ,  W2G_IRAT_CM_GSM_INIT , 0x3,
                    x2g_irat_cm_gsm_init_req_type ),
   /*  Define message  - GERAN_X2G_IRAT_CM_GSM_INIT_RSP */
  MSGR_DEFINE_UMID(GERAN, GL1 , RSP,  W2G_IRAT_CM_GSM_INIT , 0x4,
                    x2g_irat_cm_gsm_init_rsp_type ),
   
  MSGR_DEFINE_UMID(GERAN, GL1 , REQ,  L2G_IRAT_CM_GSM_INIT , 0x5,
                    x2g_irat_cm_gsm_init_req_type ),
   /*  Define message  - GERAN_X2G_IRAT_CM_GSM_INIT_RSP */
  MSGR_DEFINE_UMID(GERAN, GL1 , RSP,  L2G_IRAT_CM_GSM_INIT , 0x6,
                    x2g_irat_cm_gsm_init_rsp_type ),
   
  MSGR_DEFINE_UMID(GERAN, GL1 , REQ,  T2G_IRAT_CM_GSM_INIT , 0x7,
                    x2g_irat_cm_gsm_init_req_type ),
   /*  Define message  - GERAN_X2G_IRAT_CM_GSM_INIT_RSP */
  MSGR_DEFINE_UMID(GERAN, GL1 , RSP,  T2G_IRAT_CM_GSM_INIT , 0x8,
                    x2g_irat_cm_gsm_init_rsp_type ), 

  MSGR_DEFINE_UMID(GERAN, GL1 , IND,  CFCM_CPU_MONITOR , 0x9,
                   cfcm_cmd_msg_type_s ),

//temporary re-defintion of X2G_IRAT_CM_INIT_RSP in order to ease interdependencies with WL1 for Thor
  MSGR_DEFINE_UMID(GERAN, GL1 , REQ,  X2G_IRAT_CM_GSM_INIT , 0xa,
                    x2g_irat_cm_gsm_init_req_type ),
   /*  Define message  - GERAN_X2G_IRAT_CM_GSM_INIT_RSP */
  MSGR_DEFINE_UMID(GERAN, GL1 , RSP,  X2G_IRAT_CM_GSM_INIT , 0xb,
                    x2g_irat_cm_gsm_init_rsp_type )


};


/* MSG_ROUTER Definitions*/



typedef struct
{
  rf_measurement_T  status;
  dBx16_T           dBm_x16;
  rf_gain_range_T   rx_gain_range; /* for enhancing logging only */
  boolean     yield;
//#ifdef GERAN_L1_HLLL_LNASTATE
  uint16            jdet_value;
  uint16            SawlessHLLLState;
//#endif /*GERAN_L1_HLLL_LNASTATE*/
} gl1_hw_measurement_T;

typedef struct
 {
   msgr_hdr_struct_type  hdr;
   ARFCN_T               arfcn;
   boolean               tone_detect;
   int32                 tone_offset;      /* In QS */
   int32                 freq_offset;
   uint16                snr;
   boolean               sch_crc_pass;
   int32                 sch_offset;       /* In QS */
   uint16                message_id;
   uint8                 sch_data[4/*MAX_SCH_OCTETS*/];
   uint32                acq_start_frame;                  /* this should be uint32 */
   uint16                acq_win_start_offset;             /* new */
   void*                 gsm_cell_ptr;
   boolean               error_flag;
 } gsm_irat_acquisition_rsp_t;/* SCH Decode results */

 typedef struct
 {
   msgr_hdr_struct_type  hdr;
   ARFCN_T               arfcn;
   boolean               crc_pass;
   int32                 time_offset;
   uint16                message_id;
   uint8                 sch_data[4/*MAX_SCH_OCTETS*/];
   uint32                sch_frame;
   void*                 gsm_cell_ptr;
   boolean               reconfirm;
   boolean               error_flag;
   boolean               yield;
 } gsm_irat_sch_burst_rsp_t;/* Power measurement results */

 typedef struct
 {
   msgr_hdr_struct_type  hdr;
   uint16                message_id;
   uint8                 num_meas;                          /* Number of results to process  */
   ARFCN_T               arfcn[MAX_CM_RSSI_MEASUREMENTS_REQ];   /* Arfcn of RSSI measurements */
   gl1_hw_measurement_T  samples[MAX_CM_RSSI_MEASUREMENTS_REQ];
   boolean               error_flag;
 } gsm_irat_meas_search_rsp_t;

#endif /* MSGR_GERAN_H */

