/**
@file ps_iface_fast_dormancy.h
@brief
This file defines functions, enums and structs used for fast dormancy feature implementation
*/

#ifndef PS_IFACE_FAST_DORMANCY_H
#define PS_IFACE_FAST_DORMANCY_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S _ FAST_DORMANCY . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used for Fast Dormancy statistics
  related operations and interacts with ps sys

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_fast_dormancy.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when             who    what, where, why
--------    ---    ---------------------------------------------------------- 
 04/23/14     tyk     created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ps_iface_defs.h"
#include "ps_stat_iface.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/

/* structure type to cache statistics of all UMTS  IFACEs*/
typedef struct
{
  ps_iface_type*          iface_p;  /* PS IFACE pointer  */
  ps_stat_iface_i_s_type  stats_info; /* statistics for this PS IFACE */

}ps_iface_fast_dormancy_stats_info_s;


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_INIT_IFACE_STATS()

DESCRIPTION
  This function initializes IFACE Stats table and takes snapshot of 
  current statistics before starting Fast Dormancy Timer 

PARAMETERS
None

RETURN VALUE
0 -  Success
-1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_fast_dormancy_init_iface_stats
(
  ps_iface_name_enum_type       input_name
);

/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_UPDATE_3GPP_IFACE_STATS()

DESCRIPTION
  This function updates cached 3GPP IFACE Stats with current statistics
  in IFACE stats table

PARAMETERS
None

RETURN VALUE
0 -  Success
-1 - Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_fast_dormancy_update_3gpp_iface_stats(void);

/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_CHECK_DATA_ACTIVITY()

DESCRIPTION
  This function checks if there is any data activity in DL/UL by
  comparing cached stats to current IFACE statistics and also
  stops fast dormancy if valid data activity is found

PARAMETERS
*stop_fast_dormancy : This variable will be updated whether 
                                  fast dormancy needs to be stopped or not

RETURN VALUE
0 -  Activity is present
-1 - No Data activity

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_fast_dormancy_check_data_activity
(
  int *stop_fast_dormancy
);

/*===========================================================================
FUNCTION PS_IFACE_FAST_DORMANCY_FREE_IFACE_STATS_TBL()

DESCRIPTION
  This function frees the dynamic memory allocated for IFACE Stats table for
  fast dormancy

PARAMETERS
None

RETURN VALUE
None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_fast_dormancy_free_iface_stats_tbl(void);


#endif

