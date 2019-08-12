/*===========================================================================
  @file gnss_dm_log.c

                  DM Log PACK/UNPACK Utilities

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gnss_dm_log.c#1 $
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

#include "amssassert.h"
#include "gnss_dm_log.h"
#include "log.h"
#include "msg.h"




/* --------------------------------------------------------------------------
 * PRIVATE Data Structures
 * --------------------------------------------------------------------------*/
typedef struct
{
  uint8 *     p_logPkt;
  uint8 *     p_logPayload;
  uint16      w_logPayloadActualSz;
  uint16      w_logPayloadExpectedSz;
} gnssDmLogPackerT;


/* --------------------------------------------------------------------------
 * PRIVATE Macros
 * --------------------------------------------------------------------------*/
#define LOG_HDR_SZ                sizeof( log_hdr_type )
#define GNSS_ASSERT(assertion)    ASSERT(assertion)


/* --------------------------------------------------------------------------
 * PRIVATE Functions
 * --------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 *                       gnssDmLogPack_Start
 * ---------------------------------------------------------------------------*/
/**
@brief This function initializes a gnssDmLogPackerT structure

@param p_this [in/out]
    pointer to a GNSS DM Log Packer instance

@param e_logCode [in]
    DM log code of the packet to be packed

@param w_payloadSz [in]
    Size of the log packet payload

@return
    TRUE if initialization was successful, else FALSE
*/
static boolean gnssDmLogPack_Start(  
                    gnssDmLogPackerT *  p_this, 
                    log_code_type       e_logCode,
                    uint16              w_payloadSz )
{
  GNSS_ASSERT( w_payloadSz < (0xFFFF - LOG_HDR_SZ) );

  /*
   *  Allocate a Log Packet and check for a NULL pointer return
   */
  p_this->p_logPkt = (uint8 *) log_alloc(
                                    e_logCode,
                                    (uint32) (LOG_HDR_SZ + w_payloadSz) );

  if( NULL == p_this->p_logPkt )
  {
    return FALSE;
  }

  p_this->p_logPayload            = (p_this->p_logPkt + LOG_HDR_SZ);
  p_this->w_logPayloadActualSz    = 0;
  p_this->w_logPayloadExpectedSz  = w_payloadSz;

  return TRUE;
}


/* ---------------------------------------------------------------------------
 *                       gnssDmLog_PackMultiple
 * ---------------------------------------------------------------------------*/
/**
@brief This function packs multiple fields into a LOG packet

@param p_this [in/out]
    pointer to a GNSS DM Log Packer instance

@param p_tbl [in]
    Table of source field descriptors (NULL entry at end) 

@return
    The number of bytes packed or 0 if an error occurred
*/
static uint16 gnssDmLog_PackMultiple(
                          gnssDmLogPackerT *                p_this,
                          const gnss_serdesSerializeTblT *  p_serializerTbl )
{
  uint16  w_numBytesAvailable;
  uint16  w_numBytes;

  w_numBytesAvailable =
            p_this->w_logPayloadExpectedSz - p_this->w_logPayloadActualSz;

  w_numBytes =  gnss_serialize(
                      p_serializerTbl->p_primitiveArray,
                      p_serializerTbl->w_numPrimitives,
                      p_this->p_logPayload,
                      w_numBytesAvailable,
                      TRUE );

  p_this->p_logPayload          += w_numBytes;
  p_this->w_logPayloadActualSz  += w_numBytes;

  return w_numBytes;
}


/* ---------------------------------------------------------------------------
 *                       gnssDmLogPack_End
 * ---------------------------------------------------------------------------*/
/**
@brief This function ends the packing process and sends the packet to DIAG

@param p_this [in/out]
    pointer to a GNSS DM Log Packer instance

@return
    TRUE if pack and send completed without errors, else FALSE
*/
static boolean gnssDmLogPack_End( 
                          const gnssDmLogPackerT *  p_this )
{
  GNSS_ASSERT( p_this->w_logPayloadActualSz == p_this->w_logPayloadExpectedSz );

  /*
   *  Output the log packet through DIAG
   */ 
  log_commit( p_this->p_logPkt );

  return TRUE;
}



/* --------------------------------------------------------------------------
 * PUBLIC Functions
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

@param p_packDesc [in]
    Pointer to a DM log packing descriptor

@return
    TRUE if pack and send completed without errors, else FALSE
*/
boolean gnssDmLogPackAndSend( 
                uint16                              w_logCode,
                uint16                              w_payloadSz,
                const gnss_serdesSerializeTblT *    p_packDesc )
{
  boolean           ok;
  gnssDmLogPackerT  z_dmLog;
  uint16            w_numBytes;

  GNSS_ASSERT( NULL != p_packDesc );

  /*
   *  Pack the DM log
   *  Send to DIAG if packing is successful
   */
  ok = gnssDmLogPack_Start( &z_dmLog, w_logCode, w_payloadSz );
  if( ok )
  {
    w_numBytes = gnssDmLog_PackMultiple( &z_dmLog, p_packDesc );
    GNSS_ASSERT( w_numBytes == w_payloadSz );

    ok = gnssDmLogPack_End( &z_dmLog );
  }

  return ok;
}
