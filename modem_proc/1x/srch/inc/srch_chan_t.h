#ifndef SRCH_CHAN_T_H
#define SRCH_CHAN_T_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   S E A R C H   T A S K  --  C H A N N E L  M A N A G E R
         T Y P E S   A N D   D E F I N E S

GENERAL DESCRIPTION
   This module contains functions types and defines for the channel manager

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_chan_t.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/24/09   adw     Added NUM_CHANNELS to srch_chan_name_type enum.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
02/10/09   adw     Merge from 2h08 branch, based on VU_MODEM_1X_SRCH.13.00.01
11/19/08   adw     Merge from CMI sandbox, based on CL 787995
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
01/16/06   awj     Lint cleanup
05/17/05   kwo     Moved types from srch_chan.h

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Mux */
#include "rxc_i.h"
#include "rxcmc_i.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#define SRCH_CHAN_NONE_MASK    0x0000
#define SRCH_CHAN_PCH_MASK     0x0001
#define SRCH_CHAN_BCCH_MASK    0x0002
#define SRCH_CHAN_BCCH_1_MASK  0x0004
#define SRCH_CHAN_BCCH_2_MASK  0x0008
#define SRCH_CHAN_BCCH_3_MASK  0x0010
#define SRCH_CHAN_BCCH_4_MASK  0x0020
#define SRCH_CHAN_BCCH_5_MASK  0x0040
#define SRCH_CHAN_BCCH_6_MASK  0x0080
#define SRCH_CHAN_BCCH_7_MASK  0x0100
#define SRCH_CHAN_FCCCH_MASK   0x0200
#define SRCH_CHAN_QPCH_MASK    0x0400

/* mask inclueds BCCH, sec BCCHs, FCCCH */
#define SRCH_CHAN_CC_MASK      0x03FE

/* mask inclueds pri and sec BCCH */
#define SRCH_CHAN_ALL_BCCH_MASK  0x01FE

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* The order of this enum is IMPORTANT
 * Secondary BCCH CH1 maps to BCN = 2 which maps to 2 in this enum
 */
typedef enum
{
  SRCH_CHAN_NONE = -1,
  SRCH_CHAN_PCH = 0,
  SRCH_CHAN_BCCH,       //bcn 1
  SRCH_CHAN_BCCH_1 = 2, //bcn 2
  SRCH_CHAN_BCCH_2,
  SRCH_CHAN_BCCH_3,
  SRCH_CHAN_BCCH_4,
  SRCH_CHAN_BCCH_5,
  SRCH_CHAN_BCCH_6,
  SRCH_CHAN_BCCH_7,
  SRCH_CHAN_FCCCH,
  SRCH_CHAN_QPCH,
  SRCH_CHAN_NUM_CHANNELS
} srch_chan_name_type;

typedef enum
{
  DEMOD_CHAN_UNKNOWN = -2,
  DEMOD_CHAN_NONE = -1,
  DEMOD_CHAN_0 = 0,
  DEMOD_CHAN_1,
  DEMOD_CHAN_2,
  DEMOD_CHAN_3,
  DEMOD_CHAN_4,
  DEMOD_CHAN_5,
  DEMOD_CHAN_6,
  DEMOD_CHAN_7,
  DEMOD_CHAN_8,
  DEMOD_MAX_CHANS
} srch_chan_demod_chan_type;

typedef struct
{
  srch_chan_name_type       chan_name;  /* channel PCH, BCCH, FCCCH, etc */
  uint16                    chan_mask;  /* srch chan channel mask */
  srch_chan_demod_chan_type demod_chan; /* demod channel 0,1,2 */
  uint16                    walsh_chan; /* walsh code */
  qword                     ulpn_mask;  /* ulpn mask for the channel */
  rxc_idle_chan_info_type   rxc_info;   /* info to configure rxc */
  rxc_chan_demod_config_param_type demod_info; /* info for fingers */
} srch_chan_type;

/* Type used to communicate frame scale for channels 0/1 to be set */
typedef enum
{
  FRAME_SCALE_9600,
  FRAME_SCALE_4800,
  FRAME_SCALE_2400
} srch_chan_frame_scale_type;

#endif /* SRCH_CHAN_T_H */
