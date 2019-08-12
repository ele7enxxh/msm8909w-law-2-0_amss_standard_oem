#ifndef __GNSS_PERSISTENT_CFG_H__
#define __GNSS_PERSISTENT_CFG_H__

/*============================================================================
  @file gnss_persistent_cfg.h

                  GNSS Persistent Configurable Parameter Module

GENERAL DESCRIPTION

  This file defines the API to be used to read/write GNSS Persistent
  Configurable Parameter blocks.  These blocks are similar to "NV" parameters
  but they can't be viewed or modified by QXDM and thus they are easier
  to manage and maintain.  The code is far simpler as well.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gnss_persistent_cfg.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/10   kgj     Original

=============================================================================*/


/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "comdef.h"   /* Definition for basic types and macros */


/* ---------------------------------------------------------------------------
 * Data Types
 * ---------------------------------------------------------------------------*/

/*
 *  This module must know the module that is calling it.  In the current
 *  implementation, this is used to hide the filenames and directory
 *  structure.
 *
 *  The following enum is used for this purpose
 */
typedef enum
{
  GPC_CLIENT_SDP,
  GPC_CLIENT_PE,
  GPC_CLIENT_LBS
} gnss_persistentCfgClientT;

/*
 *  NV parameters are broken up into 3 blocks:
 *    RUNTIME_MODIFIED_PARAMETER -
 *                    This block contains parameters that are
 *                    actively modified by the software at runtime and 
 *                    stored back to persistent memory at the end of a session
 *                    or at system shutdown.  External tools should never
 *                    modify this block by directly writing to the file
 *                    system, since that can interfere with the operation
 *                    of the embedded software.
 *    PRIVATE_PARAMETER - 
 *                    This block contains "algorithm tuning" parameters
 *                    that are read from persistent memory at bootup
 *                    and never changed at runtime.  They can be configured
 *                    by writing to persistent memory with an external tool
 *                    but the change does not take effect until the next reboot.
 *    PUBLIC_PARAMETER -  
 *                    This block contains "algorithm tuning" parameters
 *                    that are read from persistent memory at bootup, but
 *                    may be changed at runtime via QMI messages.  External
 *                    tools must never attempt to change these parameters
 *                    by writing directly to the file system because those
 *                    changes will be overwritten by the embedded SW which
 *                    writes any changes to the file system (made through
 *                    QMI commands) at the end of each session or at
 *                    graceful shutdown of the modem.
 *
 *  These 3 blocks will be stored in 3 separate files.  Each block must
 *  be read/written as a whole.
 *
 *  The following enumeration type is used to select the particular block.
 */
typedef enum {
  GPC_RUNTIME_MODIFIED_PARAMETER,
  GPC_PRIVATE_PARAMETER,
  GPC_PUBLIC_PARAMETER
} gnss_persistentCfgBlockT;


/*
 *  The following values represent the possible error return values
 *  from the API functions below:
 */
typedef enum {
  GPC_ERROR_NONE,        /* No Errors (returned by both READ and WRITE */
  GPC_ERROR_NOEXIST,     /* File does not exist (returned by READ only) */
  GPC_ERROR_MISC_READ,   /* Misc. READ error */
  GPC_ERROR_MISC_WRITE   /* Misc. WRITE error */
} gnss_persistentCfgErrorT;


/* --------------------------------------------------------------------------
 *                             HOW TO USE THIS API
 *
 *  The parameter block is free form and without limits (other than a 64KB
 *  limit in size).  In spite of this, it is strongly recommended that
 *  the parameter block be designed such that it can accommodate changes
 *  in the future without breaking backward compatibility.  There are numerous
 *  ways to achieve this, including the use of TLV's, version numbers, etc.
 *  The simplest solution is probably just to use a version number to
 *  denote changes that break backward compatibility.
 *
 *  The recommendation made by the author of this API is that the
 *  user's parameter buffer start with a version number pair consisting of
 *  a major version number and a minor version number.  The major version
 *  number would be incremented only when there is a change that breaks
 *  backward compatibility.  The minor number would be changed for any
 *  backward compatible change.  The major and minor version numbers should
 *  probably start with 1 and 0 respectively.  All tools which read the
 *  parameters from non-volatile storage (including embedded software and
 *  external tools) should be designed to properly handle the case of
 *  version number changes.
 * ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------
 *                          gnss_persistentCfgRead
 * ---------------------------------------------------------------------------*/
/**
@brief This function fetches a parameter block from non-volatile storage

@param e_client
    the client requesting this operation

@param e_blockType 
    the block type to be read

@param p_paramBuffer 
    pointer to location in which to store the parameter block

@param w_paramBufferSz 
    max size of the parameter block to be read (in bytes)

@param pw_numBytesRead 
    actual number of bytes read

@return
    Returns an error code telling the user if the operation was successful
    or what type of error occurred.

@par Errors:
    GPC_ERROR_NONE : no error
    GPC_ERROR_NOEXIST : The parameter block does not exist in NV memory
    GPC_ERROR_MISC_READ : Could not read the parameter block from NV
*/
gnss_persistentCfgErrorT  gnss_persistentCfgRead(
                        gnss_persistentCfgClientT   e_client,
                        gnss_persistentCfgBlockT    e_blockType,
                        void *                      p_paramBuffer,
                        uint16                      w_paramBufferSz,
                        uint16 *                    pw_numBytesRead );

/* ---------------------------------------------------------------------------
 *                       gnss_persistentCfgWrite
 * ---------------------------------------------------------------------------*/
/**
@brief This function writes a parameter block to non-volatile storage

@param e_client
    the client requesting this operation

@param e_blockType 
    the block type to be written

@param p_paramBuffer 
    pointer to parameter block to be written

@param w_paramBufferSz 
    size of the parameter block to be written (in bytes)

@param pw_numBytesWritten 
    actual number of bytes written

@return
    Returns an error code telling the user if the operation was successful

@par Errors:
    GPC_ERROR_NONE : no error
    GPC_ERROR_MISC_WRITE : Could not write the parameter block to NV
*/
gnss_persistentCfgErrorT  gnss_persistentCfgWrite(
                              gnss_persistentCfgClientT   e_client,
                              gnss_persistentCfgBlockT    e_blockType,
                              const void *                p_paramBuffer,
                              uint16                      w_paramBufferSz,
                              uint16 *                    pw_numBytesWritten );
 
#endif  /* __GNSS_PERSISTENT_CFG_H__ */
