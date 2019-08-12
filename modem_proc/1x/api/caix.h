#ifndef CAIX_H
#define CAIX_H
/*===========================================================================

            C D M A   T R A N S L A T I O N    S E R V I C E S
                    D A T A    D E C L A R A T I O N S

DESCRIPTION
  This file contains the data structures needed for use in the
  CDMA translation services.


Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
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


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/caix.h_v   1.8   01 Oct 2002 15:01:32   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/caix.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/12   jj      CSFB: Added new LTE OTA channel over which 1x can receive 
                   messages over tunnel.
11/15/10   jtm     Initial SU API split for CP.
09/28/09   ag      1X Advanced Signaling Support - Phase 2
08/02/09   jtm     Lint fixes.
03/19/07   pg      Lint Cleanup.
01/30/07   sb      Changed function prototype of caix_get_message_type.
01/18/07   sb      Fixed function prototype.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/29/06   fh      Added a new parameter to caix_int_to_ext to indicate whether
                   the function call is for calculating access channel message
                   size or for parsing before transmitting the message.
06/12/06   fc      Changes for UMTS.
06/01/06   fc      Cleanup umts interface.
05/17/06   fh      Merged in new development of AKA/AES features
05/16/06   fh      Changes under AES feature: the parser function caix_int_to_ext
                   carries an extra argument to indicate whether to just
                   get the buffer length for access channel messages, or parsing
                   and AES encrypting the whole message.
04/26/06   ht      Added L2 Ack support when parsing failed.
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
07/12/05   ht      Added MEID Addressing support
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
06/03/04   fc      P_REV changes.
04/13/04   bkm     Added IS2000 Release D support.
09/19/03   fc      Added enum for ECAM buffer size sanity check.
07/21/04   sj      Added support for umts to 1x handover.
06/30/03   fc      Added support for IS2000 Release C messages.
02/05/03   fc      Added IS2000 Release B f-csh changes.
02/05/03   fc      Cleanup for f-csh forward compatability.
11/07/02   az      Mainlined FEATURE_IS95B
10/01/02   az      Mainlined FEATURE_IS2000
06/14/02   sb      Do not define xlate_ext_pc_hdr in SILK, causes compile error.
06/12/02   yll     Added xlate_ext_pc_hdr() prototype.
05/22/02   fc      Changes for message pack unit test.
05/20/02   fc      Added support for Universal Page Message.
01/07/02   sb      Only include cai.h and caii.h if we are not in SILK.
11/19/01   fc      Included cai.h and caii.h.
12/21/00   kk/lh   Added IS-2000 release A support.
06/19/00   yll     Added packing the MSID to be used by PDSM IS801 services.
12/08/99   jw      Basic IS2000 message support.
02/12/99   lcc     Added some IS95B support related to pilot reporting.
11/23/98   ram     merged in IS95B P_REV support from
                   MDR archive. Updated Copyright notice.
01/23/91   jai     Changed name of file to caix.h
10/30/90   jai     Created file.

===========================================================================*/

#include "cai.h"
#include "cai_v.h"
#include "caii.h"
#include "caii_v.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Channels */
typedef enum
{
  CAIX_SC,     /* Sync Channel */
  CAIX_PC,     /* Paging Channel */
  CAIX_AC,     /* Access Channel */
  CAIX_TC,     /* Traffic Channel */

  /* Release A physical channels */
  CAIX_BCCH,    /* Broadcast Control Channel */
  CAIX_FCCCH,   /* Forward Common Control Channel */
  CAIX_REACH,   /* Reverse Enhanced Access Channel */
  CAIX_RCCCH,   /* Reverse Common Control Channel */

  /* Release A logical Channels */
  CAIX_FCSCH,   /* Forward Common Signaling Channel (logical channel ) */
  CAIX_RCSCH,   /* Reverse Common Signaling Channel */
  CAIX_FDSCH,   /* Forward Dedicated Signaling Channel */
  CAIX_RDSCH,   /* Reverse Dedicated Signaling Channel */
  
  CAIX_LTE      /* Channel to receive tunneled CDMA messages over LTE air interface */

} caix_chan_type;

#endif /* CAIX_H */

