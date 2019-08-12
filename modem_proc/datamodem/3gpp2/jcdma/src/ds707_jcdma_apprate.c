/*===========================================================================

                     D S 7 0 7 _ J C D M A _ A P P R A T E
GENERAL DESCRIPTION

 This file contains the API's and functionality to implement the JCDMA
 functionality for an application to set it's data rate.

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_jcdma_apprate.c_v   1.8   12 Dec 2002 17:06:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/jcdma/src/ds707_jcdma_apprate.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
03/30/11    ms     Global variable cleanup.
10/18/10    op     Migrated to MSG 2.0 macros
08/15/07    ack    Fixed Lint Warnings
10/09/03    ak     Fixed compiler typos.
09/01/03    ak     Mainline RS2 and account for mux list to always be 18
                   items long.
04/29/03    ak     Updated to modify snm capabilities record.
11/06/02    ak     Update initialization to LOW speed.
11/04/02    ak     Updated to new SNM interface.
10/22/02    rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/10/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"


#include "cai.h"
#include "ds707_jcdma_apprate.h"
#include "ds707_roaming.h"
#include "ds707_so_pkt.h"
#include "err.h"
#include "mccap.h"
#include "msg.h"
#include "data_msg.h"
//#include "snm.h"
#include "snm_v.h"

/*===========================================================================
                          GLOBAL VARIABLES
===========================================================================*/
static ds707_jcdma_apprate_type     ds707_jcdma_apprate_val;

/*===========================================================================
                          INTERNAL STRUCTS
===========================================================================*/
static snm_sch_mo_list_type  ds707_jcdma_sch_null =
{
  0,
  {
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0
  }
};

static snm_sch_mo_list_type  ds707_jcdma_sch_1X =
{
  2,
  {
    (word)CAI_MUX_1X_RS2,                 /* MuxPDU 2,  1x, single sch     */
    (word)CAI_MUX_1X_RS1,                 /* MuxPDU 1,  1x, single sch     */
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0
  }
};

static snm_sch_mo_list_type  ds707_jcdma_sch_8X =
{
  14,
  {
    (word)CAI_MUX_8X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  8x, double sch */
    (word)CAI_MUX_8X_RS2_SINGLE,       /* RS2 MuxPDU 3,  8x, single sch */
    (word)CAI_MUX_8X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  8x, double sch */
    (word)CAI_MUX_8X_RS1_SINGLE,       /* RS1 MuxPDU 3,  8x, single sch */

    (word)CAI_MUX_4X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  4x, double sch */
    (word)CAI_MUX_4X_RS2_SINGLE,       /* RS2 MuxPDU 3,  4x, single sch */
    (word)CAI_MUX_4X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  4x, double sch */
    (word)CAI_MUX_4X_RS1_SINGLE,       /* RS1 MuxPDU 3,  4x, single sch */

    (word)CAI_MUX_2X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  2x, double sch */
    (word)CAI_MUX_2X_RS2_SINGLE,       /* RS2 MuxPDU 3,  2x, single sch */
    (word)CAI_MUX_2X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  2x, double sch */
    (word)CAI_MUX_2X_RS1_SINGLE,       /* RS1 MuxPDU 3,  2x, single sch */

    (word)CAI_MUX_1X_RS2,              /* RS2 MuxPDU 2,  1x, single sch */
    (word)CAI_MUX_1X_RS1,              /* RS1 MuxPDU 1,  1x, single sch */
                                 
    (word)0,(word)0,(word)0,(word)0
  }
};

static snm_sch_mo_list_type  ds707_jcdma_sch_16X =
{
#ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
  16,
#else
  15,
#endif /* RS2 16X */
  {
#ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
    (word)CAI_MUX_16X_RS2_DOUBLE,      /* RS2 MuxPDU 3, 16x, double sch */
#endif /* RS2 16X */

    (word)CAI_MUX_16X_RS1_DOUBLE,      /* RS1 MuxPDU 3, 16x, double sch */

    (word)CAI_MUX_8X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  8x, double sch */
    (word)CAI_MUX_8X_RS2_SINGLE,       /* RS2 MuxPDU 3,  8x, single sch */
    (word)CAI_MUX_8X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  8x, double sch */
    (word)CAI_MUX_8X_RS1_SINGLE,       /* RS1 MuxPDU 3,  8x, single sch */

    (word)CAI_MUX_4X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  4x, double sch */
    (word)CAI_MUX_4X_RS2_SINGLE,       /* RS2 MuxPDU 3,  4x, single sch */
    (word)CAI_MUX_4X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  4x, double sch */
    (word)CAI_MUX_4X_RS1_SINGLE,       /* RS1 MuxPDU 3,  4x, single sch */

    (word)CAI_MUX_2X_RS2_DOUBLE,       /* RS2 MuxPDU 3,  2x, double sch */
    (word)CAI_MUX_2X_RS2_SINGLE,       /* RS2 MuxPDU 3,  2x, single sch */
    (word)CAI_MUX_2X_RS1_DOUBLE,       /* RS1 MuxPDU 3,  2x, double sch */
    (word)CAI_MUX_2X_RS1_SINGLE,       /* RS1 MuxPDU 3,  2x, single sch */

    (word)CAI_MUX_1X_RS2,              /* RS2 MuxPDU 2,  1x, single sch */
    (word)CAI_MUX_1X_RS1,              /* RS1 MuxPDU 1,  1x, single sch */

#ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
    (word)0,(word)0
#else
    (word)0,(word)0,(word)0
#endif /* RS 2, 16x */
  }
};

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
==========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_INIT

DESCRIPTION   Called once, at phone startup.  Initializes the app rate to
              the default (HIGH).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_apprate_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds707_roaming_is_curr_mode_jcdma() == FALSE )
  {
   DATA_IS707_MSG0(MSG_LEGACY_ERROR,"JCDMA NV not enabled");
   return;
  }

  ds707_jcdma_apprate_val = DS707_JCDMA_APPRATE_HIGH;
  ds707_jcdma_apprate_set_rate(DS707_JCDMA_APPRATE_HIGH);
}

/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_SET_RATE

DESCRIPTION   Called by application to set its data rate.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_apprate_set_rate
(
  ds707_jcdma_apprate_type     rate
)
{
  boolean   so_exist_print_flag = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((rate <= DS707_JCDMA_APPRATE_MIN) ||
      (rate >= DS707_JCDMA_APPRATE_MAX)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "invalid application rate");
    return;
  }

  switch(rate)
  {
    case DS707_JCDMA_APPRATE_LOW:
      ds707_jcdma_apprate_val = rate;
      mccap_set_sch_sys_cap(
                             FALSE,
                             FALSE,
                             CAI_SCH_RATE_NULL,
                             CAI_SCH_RATE_NULL
                           );
      if (snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_FORWARD,
                           &ds707_jcdma_sch_null)== SNM_FAILURE)
      {
        so_exist_print_flag = FALSE;
      }
      if (snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_REVERSE,
                           &ds707_jcdma_sch_null)== SNM_FAILURE)
      {
        so_exist_print_flag = FALSE;
      }
      break;

    case DS707_JCDMA_APPRATE_MED:
      ds707_jcdma_apprate_val = rate;
      mccap_set_sch_sys_cap(
                             TRUE,
                             TRUE,
                             CAI_SCH_RATE_RS1_16X_RS2_16X,
                             CAI_SCH_RATE_RS1_1X_RS2_1X
                           );
      if (snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_FORWARD,
                           &ds707_jcdma_sch_16X)== SNM_FAILURE)
      {
        so_exist_print_flag = FALSE;
      }
      if (snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_REVERSE,
                           &ds707_jcdma_sch_1X)== SNM_FAILURE)
      {
        so_exist_print_flag = FALSE;
      }
      break;

    case DS707_JCDMA_APPRATE_HIGH:
      ds707_jcdma_apprate_val = rate;
      mccap_set_sch_sys_cap(
                             TRUE,
                             TRUE,
                             CAI_SCH_RATE_RS1_16X_RS2_16X,
                             CAI_SCH_RATE_RS1_8X_RS2_8X
                           );
      if (snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_FORWARD,
                           &ds707_jcdma_sch_16X)== SNM_FAILURE)
      {
        so_exist_print_flag = FALSE;
      }
      if (snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_REVERSE,
                           &ds707_jcdma_sch_8X)== SNM_FAILURE)
      {
        so_exist_print_flag = FALSE;
      }
      break;

    default:
      break;
  }

  if (so_exist_print_flag == FALSE)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "SO Doesn't Exist for rate %d", rate);
  }

  /*-------------------------------------------------------------------------
    Need to enable/disable various SO's based on new setting
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "New app rate %d", rate);
}

/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_GET_RATE

DESCRIPTION   Called by application to get its data rate.

DEPENDENCIES  None

RETURN VALUE  ds707_jcdma_apprate_val

SIDE EFFECTS  None
===========================================================================*/
ds707_jcdma_apprate_type ds707_jcdma_apprate_get_rate
(
  void
)
{
  return ds707_jcdma_apprate_val;
}

