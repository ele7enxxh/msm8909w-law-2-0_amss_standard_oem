#ifndef RFLM_API_AUTOPIN_FW_H
#define RFLM_API_AUTOPIN_FW_H

/*
  @file
  rflm_api_autopin_fw.h

  @brief
  This file contains interfaces defined by RFLM AutoPin for FW.
*/

/*=============================================================================
 
  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved. By accepting this
  material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_autopin_fw.h#1 $

when       who    what, where, why
--------   ---    ------------------------------------------------------------- 
07/16/15   cdb    Abstract Thor/AMAM specific processing 
06/09/15   cdb    Update FW API. Add support for RF managed buffers
06/04/15   whz    Initial version

===========================================================================*/


#define RFLM_API_AUTOPIN_MAJOR_VERSION            1
#define RFLM_API_AUTOPIN_MINOR_VERSION            0
#define RFLM_API_AUTOPIN_VERSION    \
              ((RFLM_API_AUTOPIN_MAJOR_VERSION << 16) | RFLM_API_AUTOPIN_MINOR_VERSION )

/* ===========================================================================
 * AutoPin IQ Copy and Process 
 * =========================================================================*/

/*! @brief  Information regarding offline buffers used for autopin processing.
 
  AMAM based:
 
    The buffers are defined by RFSW at runtime to match the FbRX ILPC buff sizes.
    The number of samples are defined by FBRx AG static settings, and the buffer
    sizes are declared accordingly. [Typically 512 Tx samples, 472 or 474 FBRx
    samples depending on undrooping filter length used].
   
    FbRX ILPC IQ data will be copied to these buffers by Common FW and AMAM
    processing will work on these buffers.
   
    Two buffer pointers are defined, one for capture and one for processing.
    - IQ samples are modified by the AMAM processing.
    - The capture buffer holds an offline copy of the raw FBRx ILPC IQ data.
    - The processing buffer holds the result of AMAM processing.
    - If the pointers are the same, the processing will happen in place.
    - If the pointers are different, the original raw IQ is preserved.
   
    Autopin RFSW shall allocate memory, and ensure that the memory remains
    allocated until processing is completed.
   
    All entries are 32 bits, first 16 bits are Q and the last 16 bits are I.
    IQ is in 16SC15 format.
 
  end AMAM based 
*/
typedef struct
{
  uint16  num_samp;    /* number of 32bit IQ samples, packed 16bit I/Q */
  uint32* iq_cap_buf;  /* where IQ is copied to */
  uint32* iq_proc_buf; /* where in-place IQ processing is done */
} rflm_autopin_iqbuf_t;

/*! @brief  Params passed to Common FW via tx agc return struct API, 
  for providing autopin capture information */
typedef struct
{
  boolean               run_autopin; /* Intruct Common FW to copy the FbRX 
                                        ILPC IQ to offline buffer and process 
                                        AMAM-based autopin */
  uint16                seq_nr;      /* Sequence number to uniquely identify 
                                        the processing result */
  rflm_autopin_iqbuf_t  txiqbuf;     /* TXIQ buffers */
  rflm_autopin_iqbuf_t  rxiqbuf;     /* RXIQ buffers */
  uint32                env_scale;
} rflm_autopin_capture_param_t;

#endif /* RFLM_API_AUTOPIN_FW_H */
