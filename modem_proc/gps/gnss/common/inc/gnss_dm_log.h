#ifndef __GNSS_DM_LOG_H__
#define __GNSS_DM_LOG_H__

/*===========================================================================
  @file gnss_dm_log.h

                  DM Log PACK/UNPACK Abstract Data Type

DESCRIPTION
  This file contains a DM Log Abstract Data Type

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

 This program is confidential and a trade secret of Qualcomm Technologies, Inc.  The
 receipt or possession of this program does not convey any rights to
 reproduce or disclose its contents or to manufacture, use or sell anything
 that this program describes in whole or in part, without the express written
 consent of Qualcomm Technologies, Inc.  The recipient and/or possessor of this program
 shall not reproduce or adapt or disclose or use this program except as
 expressly allowed by a written authorization from Qualcomm Technologies, Inc.

===========================================================================*/

/*=============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gnss_dm_log.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/10   kgj     Original

=============================================================================*/

/* --------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include "comdef.h"

#include "gnss_serdes.h"

/* --------------------------------------------------------------------------
 * PUBLIC Data Types
 * --------------------------------------------------------------------------*/

/*
 *  These data types are simply aliases for types defined in 
 *  the SERDES module
 */
typedef gnss_serializePrimitiveT   gnss_dmLogPrimitiveT;
typedef gnss_serdesSerializeTblT   gnss_dmLogSerializeTblT;


/* --------------------------------------------------------------------------
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *                            N O T E
 *
 *  This abstract data type DOES NOT support bit packing of DM logs.
 *  The smallest quantity of packing is a byte.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
 * HELPER macros
 * --------------------------------------------------------------------------*/
#define DM_LOG_GEN_TBL_SCALAR(p) { &(p), (uint16) sizeof(p), 1 }
#define DM_LOG_GEN_TBL_ARRAY(a,n_elem) { a, (uint16) sizeof(a[0]), n_elem }

#define DM_LOG_SERIALIZER(tbl,tbl_sz) { tbl, 0, tbl_sz }


/* --------------------------------------------------------------------------
 * PUBLIC Functions
 * --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
 *                      GNSS DM LOG PACKING ADT
 * --------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 *                       gnssDmLogPackAndSend
 * ---------------------------------------------------------------------------*/
/**
@brief This function packs an entire DM log and outputs it to DIAG

@param w_logCode [in]
    DM Log Code

@param w_payloadSz [in]
    DM Log Payload size (in bytes) 

@param p_serializerTbl [in]
    Pointer to a DM log packing descriptor

@return
    TRUE if pack and send completed without errors, else FALSE
*/
boolean gnssDmLogPackAndSend( 
                uint16                              w_logCode,
                uint16                              w_payloadSz,
                const gnss_dmLogSerializeTblT *     p_serializerTbl );

#endif  /* __GNSS_DM_LOG_H__ */
