/*----------------------------------------------------------------------------*/
/*!

  @brief
  This is the header file for the embedded FTM WCDMA Commands 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007, 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_gsm_dispatch.h#1 $ $DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/11/14   zbz     Added ftm_gsm_rssi_prx_drx_return_type
08/05/13   cj      Added ftm_gsm_meas_results
11/30/12   tws     Correct the dispatch function prototype.
04/03/12   ggs     Added ftm_gsm_msgr_dispatch_response_handler for handling 
                   response messages sent from the GSM module in RF task 
03/23/11   sr      Made the file C++ compatible. 
02/07/11   av      FTM IRAT support : Added RSSI to DB api for GSM. (from scmm)  
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
02/28/07   ad      Undo PACKED for ftm_gsm_extended_result_type
12/14/07   ad      Changed ftm_gsm_extended_result_type to PACKED type
03/11/04    ka     Added support for gsm extended dispatch.
10/18/04   fpe     Removed FTM_HAS_ASYNC_PKT
12/15/03   jwh     Added resp to ftm_mc_dispatch() for FTM_HAS_ASYNC_PKT.
01/29/03   lcl     Added support for MSM6300
08-21-02   rjr     Created
*/
/*----------------------------------------------------------------------------*/

#ifndef FTM_GSM_DISPATCH_H
#define FTM_GSM_DISPATCH_H

#include "rfa_variation.h"
#include "comdef.h"
#include "ftmicap.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmdiag.h"
#ifdef FEATURE_GSM
#include "rfgsm_ftm_msg.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef PACK(struct) 
{
  uint16 error_code;
} ftm_gsm_extended_result_type;

typedef PACK(struct) /*Added from SCMM*/
{
  int16 db_x16;
  uint16 error_code;
}ftm_gsm_rssi_to_dbm_return_type;

typedef PACK(struct) /*Added from SCMM*/
{
  int16 db_x16;
  int32 raw_rssi;
  uint16 error_code;
}ftm_gsm_rssi_return_type;

typedef struct
{
  boolean x2g_irat_enabled;
  int16 db_x16;
}ftm_gsm_meas_results;

typedef PACK(struct)
{
  int16 db_x16_prx;
  int32 raw_rssi_prx;
  int16 db_x16_drx;
  int32 raw_rssi_drx;
  uint16 error_code;
}ftm_gsm_rssi_prx_drx_return_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles GSM FTM requests to run tests and other primitives
   by dispatching the appropriate functions.

   @param
   cmd_ptr - 

   @retval
   Response packet
*/
ftm_rsp_pkt_type ftm_gsm_dispatch( rfm_device_enum_type rfm_dev,
                                   ftm_pkt_type  *cmd_ptr);

/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This function is used to handle RF GSM Responses which are received via
   Message Router.
   For the initial implementation, all this function does is print the message ID.
   Future enhancements may require additional handling/processing of the response message.
 
   @param
   msg_ptr, msg_size
 
   @retval
   void

*/
/*-------------------------------------------------------------------------------------*/
void ftm_gsm_msgr_dispatch_response_handler( rfgsm_ftm_msg_u *msg_ptr, uint32 msg_size );
#ifdef FTM_HAS_GSM_EXTENDED
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles requests to run GSM tests and other primitives
   by dispatching the appropriate functions.

   @param 
   cmd_ptr - 

   @retval
   Response packet
*/
ftm_rsp_pkt_type ftm_gsm_extended_dispatch( rfm_device_enum_type rfm_dev,
                                            ftm_gsm_extended_pkt_type *cmd_ptr);
#endif /* FTM_HAS_GSM_EXTENDED */


#ifdef __cplusplus
}
#endif

#endif  /* FTM_HAS_GSM */
#endif  /* FEATURE_FACTORY_TESTMODE */
#endif  /* FTM_GSM_DISPATCH_H */
