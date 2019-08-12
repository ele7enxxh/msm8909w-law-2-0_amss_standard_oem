/*!
  @file 
  ftm_common_ino.h

  @brief
  header file for all common ino typedef and functions that are made available to
  external mododule

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_ino.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/10   tnt     Fix ftm ino data structure to follow universial FTM command
09/16/09   tnt     modify to have common code independent from individual tech
09/02/09   ad      Fix compiler errors
08/10/09   tnt     Initial Check-in
===========================================================================*/
#ifndef FTM_COMMON_INO_H
#define FTM_COMMON_INO_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "ftm.h"
#include "ftmdiag.h"
#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief 
  ino capture type enumeration.

*/
typedef enum
{
	FTM_INO_CONT_CAPTURE  = 0x0,
    FTM_INO_BURST_CAPTURE = 0x1,
    FTM_INO_CAPTURE_MAX_ENUM = 0xFF,
}ftm_common_ino_capture_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  ino command enumeration.

*/
typedef enum {
  FTM_INO_GET_LTE_SAMPLES	=0x00000001,
  FTM_INO_GET_GSM_SAMPLES	=0x00000002,
  FTM_INO_GET_WCDMA_SAMPLES =0x00000003,
  FTM_COMMON_INO_MAX_ENUM   = 0x7FFFFFFF,
} ftm_common_ino_enum;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  typedef for common ino command

*/
typedef enum
{
   FTM_INO_ENABLE  = 0x0001,
   FTM_INO_DISABLE = 0x0002,
   FTM_INO_CAPTURE = 0x0003,
   FTM_INO_CMD_MAX_ENUM = 0xFFFF,
} ftm_common_ino_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  typedef for common ino test bus select

*/
typedef uint8 ftm_common_ino_test_bus_sel_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  data structure for ino function pointer type.  Individual will be responsible for
  initialize these pointer to their respective ino function

*/
typedef PACK(struct)
{
  void (*lte_funcptr)(void *msg_ptr);
  void (*gsm_funcptr)(void *msg_ptr);
  void (*wcdma_funcptr)(void *msg_ptr);
} ftm_common_ino_funcptr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  data structure for ino command. PC will send this data structure to embedded
  code to capture ino

*/
typedef PACK(struct)
{
  ftm_common_ino_cmd_type	ino_cmd; 	  			/*2B*/
  ftm_common_ino_capture_type      capture_type;	/*1B*/
  ftm_common_ino_test_bus_sel_type testbus_sel; 	/*1B*/
  uint32	actual_ino_buffer_size;
  uint32	timed_capture;
  uint32	systime;
} ftm_common_ino_header_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  data structure for ino command. PC will send this data structure to embedded
  code to capture ino

*/
typedef PACK(struct)
{
  ftm_diagpkt_subsys_header_type	diag_hdr;  		
  ftm_cmd_header_type				ftm_cmd_header;
  ftm_common_ino_header_type       	ino_hdr;
} ftm_common_ino_pkt_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
ftm_rsp_pkt_type ftm_common_ino_dispatch
(
 void *msg_ptr     //!< pointer to ftm hwtc msg
);

#endif  /* FEATURE_FACTORY_TESTMODE */ 
#endif /* FTM_COMMON_INO_H */
