#ifndef MCCAP_H
#define MCCAP_H
/*===========================================================================

          MAIN CONTROL SYSTEM CAPACITY MANAGER

GENERAL DESCRIPTION
   This module is keeping track of the underlying hardware and system capability.
   All other call processing modules look to this module to figure out if a
   particular parameter or configuration is do-able by the mobile.

DESCRIPTION
   This module has data structures to keep track of the capacity info and
   the interfaces to allow for the other modules to query  or validate any
   parameter

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccap.h_v   1.13   01 Oct 2002 16:00:16   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mccap.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/09   jj      Meta-Comments cleanup for API's exported to DS.
08/31/09   adw     Removed unnecessary customer.h and target.h includes.
05/14/09   jj      Split this file as part of CMI phase2.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
                   Re-featurized FEATURE_IS2000_DYNAMIC_FEATURE.
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
07/07/06   fh      AKA/AES: Cleaned up AES capability helper functions.
06/23/06   pg      Added support for RPC.
03/02      fh      Added AES support
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
09/16/04   fc      Added support for reverse DV.
08/23/04   az      Dynamic feature for Rel D negative SCI
08/09/04   fc      Service negotiation changes for Release C and D.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
04/08/04   bkm     Channel mask updates and new interface for checking
                   simult pdch/sch support. 
01/05/04   bkm     Renamed mccap_check_chind_and_rc to mccap_check_rc_for_chan.
12/02/03   bkm     Added capability check on DV EXT_CH_IND.
10/23/03   va      Added fns for RELC capability reporting
08/13/03   yll     Fixed compile errors.
08/08/03   fc      Added support for IS2000 Release C.
11/05/02   bkm     Added support for JCDMA Max Rate feature.
10/01/02   az      Mainlined FEATURE_IS2000
08/06/02   jrp     Changed MCCAP_REACH_RATE_SUPPORTED from a macro to a
                   function.
07/11/02   ph      Changed mccap_get_preferred_sch_coding() to return the
                   coding type which supports the max rate.
05/31/02   va      Added support for mccap_is_simult_fch_dcch_supported
04/03/02   jrp     Added MCCAP_REACH_RATE_SUPPORTED macro.
03/05/02   fc      Corrected function prototype.
02/22/02   fc      Added support for Release A common channel.
05/04/01   va      Added mcap_is_cs_supported() function prototype. 
02/28/01   ph      merged in from MSM_CP.05.00.55
                   11/29/00   lcc/hrk Added prototype for mccap_get_preferred_sch_coding().
02/27/01   kk      Added prototype for 3x capability.
02/27/01   va      Added prototype for mccap_is_flex_rates_supported
                   and mccap_is_sch_var_rates_supported. 
01/10/01   va      Added prototype for mccap_is_td_supported function, td_mode 
                   is an input argument.
01/10/01   ph      Added prototype for mccap_get_encryption_cap_info().
07/10/00   va      Added prototype for mccap_get_geo_loc_cap().
06/12/00   va      Removed FEATURE_IS2000_SCH to check the num of
                   supported fwd and rev SCHs.
06/02/00   lcc     Added prototype for mccap_is_r_fch_gating_supported.
04/07/00   va      Added support to retrieve min pwr control step size.
03/06/00   lcc     Corrected some comments and added some SCH related function
                   prototypes.
12/20/99   va      Added mccap_get_ext_mux_option_info for supporting
                   Extended mux option IR.
11/15/99   va      Initial revision of the file.
                   Created for the IS2000 Changes. This file keeps the capabaility
                   database and initializes it correctly based on s/w, h/w
                   capability.
===========================================================================*/
#include "cai.h"

/*===========================================================================

FUNCTION mccap_is_f_sch_32x_supported

DESCRIPTION
   This function determines whether the mobile supports 32X Fwd SCH data rate

DEPENDENCIES
  None.

RETURN VALUE
   True if it supports.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_f_sch_32x_supported(void);



/*===========================================================================

FUNCTION mccap_is_r_sch_32x_supported

DESCRIPTION
   This function determines whether the mobile supports 32X Rev SCH data rate

DEPENDENCIES
  None.

RETURN VALUE
   True if it supports.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_r_sch_32x_supported(void);


/*===========================================================================

FUNCTION mccap_set_sch_sys_cap

DESCRIPTION
  This function was called by DS module for JCDMA Max Rate feature, and the
  Dynamic Feature module where the fwd and rev max rate are configurable.
  
  The JCDMA Max Rate feature requires the system capabilities reported by
  the MS to be adjusted to reflect limitations imposed by system level
  requirements rather than the actual capability of the MS software and
  hardware.

  For JCDMA Max Rate, the DS module calls this function to indicate if 
  F-SCH/R-SCH are supported and if so, the maximum rate (1x,2x,4x,8x,16x)
  on each channel and these limits are captured in the system_capability 
  structure. INTLOCK & INTFREE ensure that this update completes (in DS 
  task context) before MC accesses the data structure.

  For JCDMA Max Rate, this function assumes that IS-2000 Release A is 
  not supported and that RS2 SCH is supported, but RS2 16x is not supported.
  
  For Dynamic Feature, this function is called by DF module during powerup,
  when it reads the fwd and rev SCH max rate from NV. The maximum rate can
  be up to 16x or 32x, and RS2 16x can be supported.

DEPENDENCIES
  For JCDMA Max Rate, DS Module must update configuration in SNM to service
  negotiation and capability reporting of service options.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_set_sch_sys_cap
(
  boolean fwd_sch_supported,
  boolean rev_sch_supported,
  cai_sch_rate_type max_fwd_sch_rate, 
  cai_sch_rate_type max_rev_sch_rate
);




#endif // MCCAP_H
