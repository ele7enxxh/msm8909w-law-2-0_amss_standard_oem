#ifndef HDRTXI_H
#define HDRTXI_H
/*===========================================================================

       I N T E R N A L   H D R   T X   T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the HDR TX task

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrtxi.h_v   1.6   20 Jun 2002 11:28:06   kevins  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrtxi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/11   cnx     Merged Optimized Handoff changes.
06/21/11   rmg     Added support for commands to be posted to hdrmod.
09/21/07   kss     Include file cleanup.
08/08/05   kss     Moved hdrtx_enable/disable_tx() prototypes to hdrtx.h.
06/20/02   kss     Removed NV write/read functions since they are not used.
01/03/02   kss     Added callback for hdrtx_enable_tx().
07/16/01   kss     Added Tx task context functions for Tx enable/disable 
                   and NV write/read.
05/24/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "hdrbuf.h"

#include "hdramac.h"
#include "hdrrmac.h"
#include "hdrlmaci.h"
#include "hdrmod.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsaptx.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                    Commands, Indications, and Messages for 
                    protocols running in the TX task context
--------------------------------------------------------------------------*/

/* Because each task handles a disjoint set of commands, it makes sense
 * to have the data type used for command buffers be unique to the task level.
 * Indications are common to all protocols, so the data type used for 
 * indication buffers is shared by all tasks that contain HDR protocols.  
 * Similarly, because DSM items are used for messages, the data type used 
 * for message buffers is shared by all tasks that contain HDR protocols.
 * See hdrbuf.h for more details.
 */
 
typedef union
{
  hdramac_cmd_union_type amac;          /* ACMAC Protocol Command  */
  hdrrmac_cmd_union_type rmac;          /* RTCMAC Protocol Command */
  hdrmod_cmd_union_type  mod;           /* Modulator command       */
  hdrlmac_cmd_type lmac;
  #ifdef FEATURE_LTE_TO_HDR_OH
  hdrsaptx_cmd_type saptx;
  #endif   /* FEATURE_LTE_TO_HDR_OH */
} hdrtx_protocol_cmd_union_type;

typedef struct
{
  hdrbuf_hdr_type                hdr;   /* Header for q item      */
  hdrtx_protocol_cmd_union_type  cmd;   /* Union of all commands  */
} hdrtx_cmd_struct_type;

#endif   /* HDRTXI_H */
