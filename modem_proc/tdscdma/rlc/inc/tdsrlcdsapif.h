#ifndef TDSRLCDSAPIF_H
#define TDSRLCDSAPIF_H
/*===========================================================================
                  R L C - D A T A S A P  I N T E R F A C E

                         H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RLC - DSAP Interface.

Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcdsapif.h_v   1.9   08 May 2002 19:13:48   tliou  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rlc/inc/tdsrlcdsapif.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/10   cyx     Remove tdsmvsif.h to clean up l2 interface
10/24/10   guo     Initial release Upmerged WCDMA to TDS

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "tdsuecomdef.h"
#include "dsmutil.h"
#include "dsm.h"
#include "queue.h"





/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/* --------------------------------- */
/* PUBLIC APIs of rlcsdapif.h        */
/* --------------------------------- */

/*---------------------------------------------------------------------------
  Type for registration details for each uplink logical channel.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                  /* Unique identifier for the      */
                                         /* uplink RLC logical channel.    */
                                         /* Can have a value from 0 to 18. */
  dsm_watermark_type  *ul_wm_ptr;        /* Watermark for storing uplink   */
                                         /* data                           */
} tdsrlc_ul_channel_reg_type;


/*---------------------------------------------------------------------------
  Type to register the uplink queue for storing data bytes
  to be transmitted, for one or more uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of uplink logical        */
                                        /* channels to register            */
  tdsrlc_ul_channel_reg_type  rlc_data[TDSUE_MAX_UL_LOGICAL_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} tdsrlc_ul_register_srvc_type;


/*---------------------------------------------------------------------------
  Type to deregister the uplink queue for storing data bytes
  to be transmitted, for one or more uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of uplink logical        */
                                        /* channels to register            */
  rlc_lc_id_type  rlc_id[TDSUE_MAX_UL_LOGICAL_CHANNEL];
                                        /* DeRegistration WM of logical    */
                                        /* channel                         */
} tdsrlc_ul_deregister_srvc_type;


/*---------------------------------------------------------------------------
  Type for registration of the SDU confirmation callback function type.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                  /* Unique identifier for the      */
                                         /* uplink RLC logical channel.    */
                                         /* Can have a value from 0 to 18. */

  /* the callback function to tell RRC this specific SDU w/ MUI has rx'd   */
  /* correctly or not by the other side.                                   */
  void (*rlc_am_sdu_cnf_cb) (rlc_lc_id_type ul_data_lc_id,
                                         /* data logic channel ID.         */
                             uecomdef_status_e_type status,
                                         /* Successful or failed           */
                             uint8  mui, /* MUI of this SDU.               */
                             rlc_sdu_tx_fail_reason_e_type failure_reason
                            );
} tdsrlc_am_sdu_cnf_cb_fcn_type;

typedef struct {
  uint8 nchan;                          /* Number of uplink logical        */
                                        /* channels to register            */
  tdsrlc_am_sdu_cnf_cb_fcn_type  sdu_cnf_cb[TDSUE_MAX_UL_AM_DATA_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} tdsrlc_register_am_sdu_confirm_callback_type;

/*---------------------------------------------------------------------------
  Type for registration details for each downlink logical channel.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the       */
                                        /* downlink RLC logical channel.   */
                                        /* Can have a value from 0 to 18.  */
  dsm_watermark_type  *dl_wm_ptr;       /* Watermark for storing downlink  */
                                        /* data                            */
  boolean context;                      /* Task (FALSE) or Interrupt (TRUE)*/

  void *rlc_post_rx_func_ptr_para;      /* 3rd parameter for               */
                                        /* rlc_post_rx_proc_func_ptr()     */
  void  (*rlc_post_rx_proc_func_ptr)(uint8, uint8, void *);
                                        /* The callback function that is   */
                                        /* called by RLC when data is      */
                                        /* available in the downlink queue,*/
                                        /* for further processing by upper */
                                        /* layers.This function passes     */
                                        /* logical channel id as parameter.*/

} tdsrlc_dl_channel_reg_type;


/*---------------------------------------------------------------------------
  Type to register the downlink queue for storing data
  bytes received over the air and a callback function to be called each
  time data is available in the queue, for one or more downlink
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink logical      */
                                        /* channels to register            */
  tdsrlc_dl_channel_reg_type  rlc_data[TDSUE_MAX_DL_LOGICAL_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} tdsrlc_dl_register_srvc_type;


/*---------------------------------------------------------------------------
  Type to de-register the downlink queue for storing data
  bytes received over the air and a callback function to be called each
  time data is available in the queue, for one or more downlink
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink logical      */
                                        /* channels to de register         */
  rlc_lc_id_type  rlc_id[TDSUE_MAX_DL_LOGICAL_CHANNEL];
                                        /* De-Registration logical channel */
} tdsrlc_dl_deregister_srvc_type;


/*---------------------------------------------------------------------------
  Type for registration of the SDU discard callback function type.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                  /* Unique identifier for the      */
                                         /* downlink RLC logical channel.  */
                                         /* value range from 0 to 18.      */
  void (*rlc_am_sdu_discard_cb) (rlc_lc_id_type dl_data_lc_id,
                                         /* downlink data logical chan ID  */
                                 uint8 mui  /* MUI of the SDU              */
                                );
} tdsrlc_am_sdu_discard_cb_fcn_type;

typedef struct {
  uint8 nchan;                           /* Number of downlink logical     */
                                         /* channels to register           */
  tdsrlc_am_sdu_discard_cb_fcn_type  sdu_discard_cb[TDSUE_MAX_DL_AM_DATA_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} tdsrlc_register_am_sdu_discard_callback_type;



/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#endif
