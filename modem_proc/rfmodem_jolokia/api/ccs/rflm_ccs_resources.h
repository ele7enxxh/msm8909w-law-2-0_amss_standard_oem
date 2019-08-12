#ifndef RFLM_CCS_RESOURCES_H
#define RFLM_CCS_RESOURCES_H


/*
   @file
   rflm_resources.h

   @brief
   RFLM CCS Driver's interface for resources request from other clients.

   @details

*/

/*===========================================================================
Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/ccs/rflm_ccs_resources.h#1 $
$DateTime: 2016/12/13 07:59:25 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/13   ra      Added support for CCS memory collapse 

==========================================================================*/


typedef struct
{
  uint32 mem_slp_cntl;    /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL */
  uint32 mem_slp_cntl2;  /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL2 */
  uint32 spare[2];              /*!< Future use */
} rfm_sleep_cfg_t;


void rfm_get_mcpm_rfi_sleep_control(rfm_sleep_cfg_t *);
#endif  /* RFLM_CCS_RESOURCES.H */
