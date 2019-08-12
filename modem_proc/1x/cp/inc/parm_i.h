#ifndef PARM_I_H
#define PARM_I_H
/*===========================================================================

         M O B I L E    C A I    P A R A M E T E R    S E R V I C E S
                        H E A D E R    F I L E

DESCRIPTION
  This module contains variables necessary to interface with the
  Mobile CAI parameter services.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/parm.h_v   1.1   01 Oct 2002 18:47:18   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/parm_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
02/04/12   ppr     Feature Cleanup
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
10/01/02   fc      Added BCCH, FCCCH and EACH BA mode parameter support.
06/28/00   lad     Introduce backward compatibility flag that, when enabled,
                   forces PARM DIAG support to use the old DIAG V7 method.
                   The feature is called FEATURE_DIAG_V7_PKT_IFACE.
06/19/00   lad     Removed unused parm_type and legacy DIAG support prototypes
03/30/99   ram     Verified and check-ed in changes made for MDR
                   DM support by aaj. parm_diag_get now always requires
                   10 parameters to accomodate the changes for MDR
                   support.
02/26/99   aaj     Modified parm_fer_type struct to include MDR sup channels
07/16/96   jmk     Split count of TC parameters into MUX1 and MUX2 counts.
02/08/96   jca     Put ()s around macros to make Lint happy.
08/16/95   jmk     Modified parm_diag_get for MUX2 parms.
08/14/95   gb      Added MUX2 parameter support.
01/02/92   jai     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "cai.h"
#include "cai_v.h"
#include "cai_i.h"
#include "parm.h"

/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Return Statuses */
enum
{
  PARM_DONE_S = 0,  /* Operation successfully completed */
  PARM_INV_S,       /* Invalid parameter id */
  PARM_NOSET_S,     /* Attempt to set an non-settable parameter */
  PARM_BADLEN_S     /* Input paramter length incorrect */
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Parameter set type */

typedef struct
{
  uint16 num_parms;
    /* number of parameters in array */
  struct
  {
    uint16 id;
      /* parameter id */
    uint32 value;
      /* value to set parameter to */
  } parm[ CAI_SET_PARM_MAX ];
} parm_set_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Parameter get type */
typedef struct
{
  uint16 num_parms;
    /* number of parameters in array */
  struct
  {
    uint16 id;
      /* parameter id */
    uint16 len;
      /* parameter length */
    uint32 value;
      /* parameter value */
  } parm[ CAI_PARM_RSP_MAX ];
} parm_get_type;

/* Number of statistics for Reverse Traffic Channel */
#define PARM_NUM_REV_TC_MUX1 ( CAI_MUX1_REV_14_ID - CAI_MUX1_REV_1_ID  + 1 )
#define PARM_NUM_REV_TC_MUX2 ( CAI_MUX2_REV_26_ID - CAI_MUX2_REV_1_ID  + 1 )

/* Number of statistics for FORWARD Traffic Channel */
#define PARM_NUM_FOR_TC_MUX1 ( CAI_MUX1_FOR_14_ID - CAI_MUX1_FOR_1_ID  + 1 )
#define PARM_NUM_FOR_TC_MUX2 ( CAI_MUX2_FOR_26_ID - CAI_MUX2_FOR_1_ID  + 1 )

/* Number of statistics for Paging Channel */
#define PARM_NUM_PAG ( CAI_PAG_7_ID - CAI_PAG_1_ID  + 1  )

/* Number of statistics for Access Channel */
#define PARM_NUM_ACC ( CAI_ACC_8_ID - CAI_ACC_1_ID  + 1  )

/* Number of statistics for Layer 2 */
#define PARM_NUM_LAYER2 ( CAI_LAYER2_RTC5_ID - CAI_LAYER2_RTC1_ID  + 1  )


/* <EJECT> */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PARM_SET

DESCRIPTION
  This function sets a specified set of parameters.

DEPENDENCIES
  None.

RETURN VALUE
  PARM_DONE_S if the parameter set is ok.
  PARM_INV_S if a parameter id is out of the valid range.
  PARM_NOSET_S if the a parameter is not a settable parameter.
  PARM_BADLEN_S if a parameter length is not correct.

SIDE EFFECTS
  If the return parameter is PARM_DONE_S,  the parameters are set.  Otherwise
  the invalid parameter is returned where parm_id_ptr is pointed to.

===========================================================================*/

extern uint16 parm_set
(
  parm_set_type *set_ptr,
    /* parameter set to verify */
  uint16 *parm_id_ptr
    /* pointer to where to place 1st invalid parameter if parameter set is
       found to be invalid */
);


/* <EJECT> */
/*===========================================================================

FUNCTION PARM_RET_SYNC_TIME

DESCRIPTION
  This function retrieves the Sync Channel time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  parm_oth_sys_time is stored in the location pointed to by sync_time.

===========================================================================*/

extern void parm_ret_sync_time
(
  qword sync_time
    /* Location where Sync Channel time is to be stored */
);

/* <EJECT> */
/*===========================================================================

FUNCTION PARM_GET

DESCRIPTION
  This function gets a specified parameter set of paramters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If a parameter in the set cannot be retrieved its length will be set to 0.
  Otherwise the parameter length and value will be returned.
===========================================================================*/

extern void parm_get
(
  parm_get_type *parm_ptr
    /* pointer to set of parameters to get */
);

/* <EJECT> */
/*===========================================================================

FUNCTION PARM_INC

DESCRIPTION
  This function increments the value of a specified parameter.

DEPENDENCIES
  None.

RETURN VALUE
  PARM_DONE_S if the parameter is set.
  PARM_INV_S if the input parameter id is out of the valid range.

SIDE EFFECTS
  None.
===========================================================================*/

extern uint16 parm_inc
(
  uint16 parm_id,
    /* ID of paramter found in cai.h */
  uint8  inc
    /* amount to increment parameter by */
);

#if defined(FEATURE_IS2000)
/*===========================================================================

FUNCTION PARM_XTRA_INC

DESCRIPTION
  This function increments the value of a specified xtra parameter.

DEPENDENCIES
  None.

RETURN VALUE
  PARM_DONE_S if the parameter is set.
  PARM_INV_S if the input parameter id is out of the valid range.

SIDE EFFECTS
  None.
===========================================================================*/

uint16 parm_xtra_inc ( uint16 parm_id, uint8 inc );

#endif /* FEATURE_IS2000 */

/* <EJECT> */
/*===========================================================================

FUNCTION PARM_STORE_SYNC_TIME

DESCRIPTION
  This function sets the Sync Channel time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sync channel time is stored in parm_oth_sys_time.

===========================================================================*/

extern void parm_store_sync_time
(
  qword sync_time
    /* Sync Channel time */
);

#endif /*PARM_I_H*/
