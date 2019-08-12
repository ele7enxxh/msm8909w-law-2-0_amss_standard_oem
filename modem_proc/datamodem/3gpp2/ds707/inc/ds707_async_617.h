#ifndef DS707_ASYNC_617_H
#define DS707_ASYNC_617_H
/*===========================================================================

                        D S 7 0 7 _ A S Y N C _ 6 1 7
DESCRIPTION
  This file contains definitions for TIA/EIA-617 processing. Used in IS707
  circuit switched calls (Async/Fax).

EXTERNALIZED FUNCTIONS


 Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_617.h_v   1.2   02 Oct 2002 13:22:06   ajithp  $
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_async_617.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

-----------------------------------------------------------------------------
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC

/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                      EIA-617 In-Band Signals Definitions
===========================================================================*/

/*---------------------------------------------------------------------------
  Leading byte.
---------------------------------------------------------------------------*/
#define EIA617_EM               0x19

/*---------------------------------------------------------------------------
  Primary codes mobile to base.
---------------------------------------------------------------------------*/
#define EIA617_EXTEND1_M2B      0x41

/*---------------------------------------------------------------------------
  Sub codes for EXTEND1_M2B.
---------------------------------------------------------------------------*/
#define EIA617_ESCAPE           0x42
#define EIA617_UNREC            0x44
#define EIA617_PARAM            0x45
#define EIA617_VALID            0x46
#define EIA617_RESP_2B          0x47     
#define EIA617_SINGLE_EM_M2B    0x5C
#define EIA617_DOUBLE_EM_M2B    0x5D

/*---------------------------------------------------------------------------
  Primary codes base to mobile.
---------------------------------------------------------------------------*/
#define EIA617_EM_109_OFF       0x66
#define EIA617_EM_109_ON        0x67
#define EIA617_EM_125_OFF       0x6A
#define EIA617_EM_125_ON        0x6B
#define EIA617_FLOW_OFF         0x7A
#define EIA617_FLOW_ON          0x7B
#define EIA617_SINGLE_EM_B2M    0x7C
#define EIA617_DOUBLE_EM_B2M    0x7D
#define EIA617_LEN_OFFSET       0x1F

#define EIA617_EXTEND0_B2M      0x60
#define EIA617_EXTEND1_B2M      0x61

/*---------------------------------------------------------------------------
  Sub codes for EXTEND0_B2M.
---------------------------------------------------------------------------*/
#define EIA617_CONTROL          0x42
#define EIA617_STATUS           0x62

/*---------------------------------------------------------------------------
  Sub codes for EXTEND1_B2M.
---------------------------------------------------------------------------*/
#define EIA617_RESP_2M          0x43

/*===========================================================================
                      IS-99 In-Band Signals Definitions
===========================================================================*/
#define IS99_IAC                 255
#define IS99_DONT                254
#define IS99_DO                  253
#define IS99_WONT                252
#define IS99_WILL                251
#define IS99_SB                  250
#define IS99_SE                  240


#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DS707_ASYNC_617_H   */
