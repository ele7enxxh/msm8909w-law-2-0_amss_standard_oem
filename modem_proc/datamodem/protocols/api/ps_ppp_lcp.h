#ifndef PS_PPP_LCP_H
#define PS_PPP_LCP_H
/*===========================================================================

                           P S _ P P P _ L C P . H

DESCRIPTION

  Copyright 1992-1995 William Allen Simpson
  Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_lcp.h_v   1.2   12 Feb 2003 14:31:50   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_lcp.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/11    op     Data SU API cleanup
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
05/04/08    scb    Bug fixes caught during PPP STA development
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific LCP constants
10/10/06    ssh    Added lcp_setup_device()
05/25/06    ssh    #defines for LCP option lengths and option value lengths
07/05/04    ifk    Added prototypes for lcp_mru_get() and lcp_mru_set()
10/24/03    ifk    Added prototype for lcp_set_device_constants()
09/12/03    mvl    Added some JCDMA timing changes.
05/23/03    atp    Reverted number of request attempts to 20.
04/29/03    jd     Reduce term-req tries to 2 from 10.  Added multiplier for
                   term-req timeout to extend for dormant reorigination case.
                   Fixed compile error with SHOW_STAT off.
04/24/03    mvl    Removed duplicate definition of option bits.
03/26/03    mvl    removed authentication featurization
02/11/03    mvl    Reduced the number of Request attemps to 10 and the number
                   or Term attempts to 3.
08/02/02    mvl    moved the auth definitions to either the .c files or defs
07/25/02    mvl    removed netmodel dependency.  Updates for PPP renaming.
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
12/01/01    vas    Changed function prototypes to take the ppp iface as a
                   parameter instead of being hardcoded. Made ppp_do_auth
                   part of ppp ctl blk. lcp_chap_info maintained per iface.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
07/30/01    mvl    Modified prototypes to match new call signatures.  Only
                   defining LCP_N_AP to its non-zero value if an auth
                   protocol is supported.
07/28/01    mvl    moved the lcp_values data struct to pppvalues.h
07/27/01    mvl    Modified to use pstimers and shortened retry to 1000ms
07/25/01    mvl    Added prototypes for powerup initialization and timer
                   cancellation.
04/24/00    snn    Increased the value of Max_passwd_len from 14 to 16,
                   because during resync, the CHAP password used has a
                   length of 16.
03/10/00    snn    added CHAP support
10/28/99    mvl    Added code review changes
09/27/99    snn    Changed ppp_auth_info_type to a typedef to follow coding
                   standards. Fixed commenting styles. Added comments for
                   the PAP prototypes.
08/25/99    snn    Updated the sizes of the arrays in PAP data structure
08/19/99    mvl    Added header for lcp_save_work_opt()
08/09/99    snn    Added support for PAP timeouts.
07/26/99    mvl    Changed the NAK_TRY value to 3 from 10 as 10 nak cycles
                   will take more than 3 seconds.  Avoids infinite loop.
07/26/99    mvl    removed references to quality reporting period which is
                   not supported.
07/26/99    na     Changed header for lcp_init() for network model builds.
07/26/99    mvl    Added typedef for various structures
07/09/99    snn    Added support for PAP implementation
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  LCP option types
---------------------------------------------------------------------------*/
#define LCP_MRU           1
#define LCP_ACCM          2
#define LCP_AP            3
#define LCP_QMP           4
#define LCP_MAGIC         5
#define LCP_PFC           7
#define LCP_ACFC          8
#define LCP_OPTION_LIMIT  8     /* highest # we can handle                 */

/*---------------------------------------------------------------------------
  The minimum and maximum values for the Length field in the option header.

    0                   1
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |    Length     |    Data ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
#define LCP_MRU_OPTION_SIZE       4
#define LCP_ACCM_OPTION_SIZE      6
#define LCP_AP_OPTION_SIZE        4
#define LCP_QUALITY_OPTION_SIZE   4
#define LCP_MAGIC_OPTION_SIZE     6
#define LCP_PFC_OPTION_SIZE       2
#define LCP_ACFC_OPTION_SIZE      2
#define LCP_AP_PAP_OPTION_SIZE    4
#define LCP_AP_CHAP_OPTION_SIZE   5
#define LCP_AP_EAP_OPTION_SIZE    4

/*---------------------------------------------------------------------------
  The length of the option field in the data portion of the 
  configuration option. An example (Maximum-Receive-Unit) Configuration 
  Option is shown below. The length of the MRU field is 2 octets.
  Similary, this value is 0 for PFC and ACFC options.

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |    Length     |      Maximum-Receive-Unit     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

---------------------------------------------------------------------------*/
#define LCP_MRU_LEN               2
#define LCP_ACCM_LEN              4
#define LCP_AP_LEN                2
#define LCP_QUALITY_LEN           2
#define LCP_MAGIC_LEN             4 
#define LCP_PFC_LEN               0 
#define LCP_ACFC_LEN              0

/*---------------------------------------------------------------------------
  Other configuration option values
---------------------------------------------------------------------------*/
#define LCP_ACCM_DEFAULT  0xffffffffUL
#define LCP_MRU_DEFAULT   1500
#define LCP_MRU_HI        4096
#define LCP_MRU_LO        128

/*===========================================================================
FUNCTION LCP_SAVE_WORK_OPT()

DESCRIPTION
   This function stores the value of the LCP option that is passed in. It is
   being saved into the work and work_pdv the want values are saved by the
   caller.

DEPENDENCIES
  lcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void lcp_save_work_opt
(
  struct fsm_side_s *opts,       /* pointer to where the values are stored */
  byte opt_num,                                       /* the option number */
  byte *opt_val                                 /* the value of the option */
);

/*===========================================================================
FUNCTION LCP_SETUP_DEVICE()

DESCRIPTION
  This function performs the necessary LCP configuration on the specified
  PPP device.

DEPENDENCIES
  LCP negotiation on the device must be complete and negotiated
  options/values must be stored in LCP control block.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_setup_device
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION LCP_SET_NET_MODEL_RESYNC_FNC()

DESCRIPTION
  This function sets the callback function pointer for the LCP established
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lcp_set_net_model_resync_fnc
(
  net_model_resync_fptr_type fnc_ptr,
  ppp_dev_enum_type          device
);

#endif /* _PS_PPP_LCP_H */
