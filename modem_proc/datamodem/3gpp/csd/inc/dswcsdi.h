#ifndef DSWCSDI_H
#define DSWCSDI_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                  I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains internal data declarations and function prototypes for 
  WCDMA Circuit-Switched Data Services. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsd.h_v   1.9   06 May 2002 14:52:46   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dswcsdi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
07/23/07   ar      Add call traffic counter support.
05/28/04   ar      Add support for logging synch CSD packets.
04/15/03   dgy     Initial version. Moved some definitions and data 
                   declaration here from dswcsd.h.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "comdef.h"
#include "dsucsdi.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define NUM_BITS_PER_BYTE        8             /* Number of bits in a byte */

#define SDU_SIZE_FOR_64K   80        /* SDU size for 64k CS Data, in bytes */

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
#define FIRST_BYTE_START_VAL     0x61   /* Start value for the first byte  **
                                        ** in a SDU, which is 'a' in ASCII */

#define FIRST_BYTE_VAL_RANGE     26  /* Number of different values allowed **
                                     **       for the first byte in an SDU */

/*---------------------------------------------------------------------------
  Flag for reading from NV whether the Sync Test App is enabled
---------------------------------------------------------------------------*/  

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

/*---------------------------------------------------------------------------
  Call-related information
---------------------------------------------------------------------------*/  
typedef struct
{
  uint16              bytes_per_sdu;          /* Number of bytes in an SDU */
  uint32              sdus_per_tti;           /* Number of SDUs per TTI    */
  ds_ucsd_data_rate_T fnur;                   /* Fixed Network User Rate   */
  cm_call_id_type     cm_call_id;             /* Call Manager call ID      */
  ds_ucsd_set_data_counters_cb_T   set_data_counters_cb;
                                              /* Traffic counter update cb */
} dswcsd_call_info_T;

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDI_H */
