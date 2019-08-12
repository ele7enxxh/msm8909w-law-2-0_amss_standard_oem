#ifndef MCCSCM_H
#define MCCSCM_H
/*********************************************************************
*
*    Supplemental Channel Processing Routines
*
* GENERAL DESCRIPTION
*   This module contains functions to process SCAM messages
* received from a base station.  It also includes functions to
* process the data pattern testing information.  The SCAM processing is
* currently only for version 0.1 of MDR, and the DPR information is
* usable for versions 0.1 or 0.2 of MDR, and possibly also 1.0.
*
*  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc. 
*  All Rights Reserved Qualcomm Proprietary
*
*  Export of this technology or software is regulated by the U.S. Government.
*  Diversion contrary to U.S. law prohibited.
*
*  All ideas, data and information contained in or disclosed by
*  this document are confidential and proprietary information of
*  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
*  By accepting this material the recipient agrees that this material
*  and the information contained therein are held in confidence and in
*  trust and will not be used, copied, reproduced in whole or in part,
*  nor its contents revealed in any manner to others without the express
*  written permission of Qualcomm Technologies, Inc.
*
*                       EDIT HISTORY FOR MODULE
*   This section contains comments describing changes made to the module.
*   Notice that changes are listed in reverse chronological order.
*
*
* $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccscm.h_v   1.1   01 Oct 2002 16:51:40   azafer  $
* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccscm.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $
*
*
* when       who     what, where, why
* --------   ---     -------------------------------------------------------
* 02/04/12   ppr     Feature Cleanup
* 09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                     FEATURE_MODEM_1X_NIKEL.
* 11/15/10   jtm     Initial SU API split for CP.
* 04/03/09   jj      Removed Target specific feaaturization as part of CMI effort.
* 10/10/05   fh      Removed including of dec.h. changed dec_rate_set_type to
                     cai_rate_set_type.
* 10/20/04   an      Replaced inclusion of srch.h with srchmc.h
* 08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
* 10/01/02   az      Mainlined FEATURE_IS2000.
* 10/20/99   lcc     Added MSM5000/IS2000 MDR support.
* 07/26/99   jq      Redefined the timer types to a enum structure
* 07/22/99   jq      Added shared data type for SCAM-GHDM linking
* 03/30/99   ram     Updated the header information for the function
                     prototypes to match those in the c file. Externalised
                     mccscm_cancel_prev_dur_timer since it's used by other
                     modules.
* 03/15/99   ram     Updated scam_timer_msg to be of caii_scam_type rather
                     than mccrxtx_msg_type since it's used only for SCAM.
                     fixed incorrect comment about SCAM timer.
* 03/09/99   ram     Externalized SCAM/GHDM duration related functions.
* 02/19/99   ram     Externalized IS95B SCAM processing function and related
                     data. Removed unused data structures, lint cleanup
                     and added more comments to the function prototypes.
* 02/11/99   ram     Added mdr_pre_process to pre-process the MDR related
                     fields of SCAM and GHDM. Created scam_process function
                     to process SCAM message. Featurized code under
                     IS95B_MDR and SPECIAL_MDR.
* 01/19/99   lcc     Moved FEATURE_IS95B_MDR below #includes for header files.
                     Now it builds correctly with and without FEATURE_IS95B_MDR.
* 01/19/99   ram     Added FEATURE_IS95B_MDR defines so that the file
*                    builds correctly when FEATURE_IS95B is turned off.
* 12/23/98   lcc     Corrected definition of mdr_fwd_dpt_frames to take care
                     of RS2 frames also.
* 12/22/98   nmn     improved output
* 12/16/98   nmn     surrounded all DPT code with #defines
* 12/11/98   nmn     Linted for 0.2
* 12/01/98   ram     Added prototypes for MDR DPT related functions
* 11/23/98   nmn     DPT works sucessfully with two channels
* 11/19/98   nmn     add DPT processing using byte length data
* 10/15/98   nmn     added separate functions to perform different parts
*                    of SCAM processing
* 07/13/98   nmn     linted file
* 07/10/98   nmn     removed variables in favor of those in cai.h
* 07/09/98   nmn     created variables, changed spacing
* 07/08/98   nmn     Initial document
*
*
*
********************************************************************/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "modem_1x_defs.h"
#include "cai.h"
#include "caii.h"
#include "comdef.h"
#include "mccrxtx.h"

#endif /* MCCSCM_H */
