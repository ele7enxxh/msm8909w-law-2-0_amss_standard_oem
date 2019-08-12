

#ifndef __MCPM_STATS_H__
#define __MCPM_STATS_H__

/*=========================================================================

           M O D E M   C L O C K   A N D   P O W E R   M A N A G E R 
               
                        H E A D E R   F I L E


  GENERAL DESCRIPTION
    This file contains the definitions for MCPM Stats feature.

  
  EXTERNALIZED FUNCTIONS
    MCPM_Stats_Start_Collection
    MCPM_Stats_End_Collection
    MCPM_Stats_Get_Overall_Active_Time
  
        Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.


==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mpower/api/mcpm_stats.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
08/31/15   ne      Initial version


==========================================================================*/ 


/*==========================================================================
               FUNCTION DEFINITIONS FOR MODULE
==========================================================================*/

/* =========================================================================
**  Function : MCPM_Stats_Get_Overall_Active_Time
** =========================================================================*/
/**
  Begin collection of statistics across all techs.

  @return
    None

  @dependencies
  TBD.

*/
void MCPM_Stats_Start_Collection(void);


/* =========================================================================
**  Function : MCPM_Stats_Get_Overall_Active_Time
** =========================================================================*/
/**
  End collection of statistics across all techs.

  @return
    None

  @dependencies
  TBD.

*/
void MCPM_Stats_End_Collection(void);


/* =========================================================================
**  Function : MCPM_Stats_Get_Overall_Active_Time
** =========================================================================*/
/**
  Get overall time from stats module. This will also reset statistics to zero.

  @return
  uint64 - Total time that any RX or TX has been active for any technology.
            Time is measured in qticks (19.2MHz freq)
           If Counting is not currently enabled (MCPM_Stats_Start_Collection()
            has not been called) then will return 0.

  @dependencies
  TBD.

*/
uint64 MCPM_Stats_Get_Overall_Active_Time(void);

#endif

