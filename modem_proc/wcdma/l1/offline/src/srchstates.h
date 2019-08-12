#ifndef SRCH_STATES_H
#define SRCH_STATES_H

/*===========================================================================

                          Search States Header File

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchstates.h_v   1.4   27 Apr 2001 13:32:02   wnew  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchstates.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
-------    ---     --------------------------------------------------------
04/22/09   stk     Compiler warning fixes	
09/12/08   nd      changes for cell deconfig during state cleanup 
06/20/2006 bd      Renamed global vars/functions to avoid conflict with 1x names
01/18/2001 mdl     get next L1M state as input parameter to cleanup routines.


===========================================================================*/
#include "comdef.h"


/*===========================================================================

FUNCTION SEARCH_INIT

FILE NAME

DESCRIPTION
  This function initializes searcher global parameters - write-once parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void search_init( void );


/*===========================================================================

FUNCTION SRCH_CLEANUP

FILE NAME

DESCRIPTION
  This function is a one time cleanup routine called when Layer 1 is exiting.
  
DEPENDENCIES
  None.

RETURN VALUE
  Boolean vallue TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_cleanup( l1m_state_enum_type next_state );


/*===========================================================================

FUNCTION SRCHIDLE_INIT

FILE NAME

DESCRIPTION
  This function performs search module idle state initializations. It is called
  by L1 manager when L1 enters the Idle state. 

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchidle_init(void);



/*===========================================================================
FUNCTION        SRCH_IDLE_CLEANUP

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_idle_cleanup(l1m_state_enum_type next_state);


/*===========================================================================

FUNCTION SRCHACQ_INIT

FILE NAME

DESCRIPTION
  This function performs search module acquisiton initializations. It is called
  by L1 manager when L1 enters the "Acquisition" state. 

DEPENDENCIES
  It is assumed that RF has tuned to correct WCDMA frequency before this 
  function is called.

RETURN VALUE
  Boolean value TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchacq_init(void);



/*===========================================================================
FUNCTION        

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  Boolean value TRUE.

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_acq_cleanup(l1m_state_enum_type next_state);


/*===========================================================================

FUNCTION SRCH_BCH_INIT

FILE NAME

DESCRIPTION
  This function performs search module BCH initializations. It is called
  by L1 manager when L1 enters the "BCH" state. 

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_bch_init( void );

/*===========================================================================

FUNCTION SRCH_BCH_CLEANUP

FILE NAME

DESCRIPTION
  This function performs search module BCH cleanup. It is called
  by L1 manager when L1 exits the "BCH" state. 

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_bch_cleanup( l1m_state_enum_type next_state );


/*===========================================================================

FUNCTION SRCH_PCH_INIT

FILE NAME

DESCRIPTION
  This function performs search module BCH initializations. It is called
  by L1 manager when L1 enters the "PCH" state. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_pch_init( void );


/*===========================================================================

FUNCTION SRCH_PCH_CLEANUP

FILE NAME

DESCRIPTION
  This function performs search module PCH cleanup. It is called
  by L1 manager when L1 exits the PCH state. 

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_pch_cleanup( l1m_state_enum_type next_state );


/*===========================================================================

FUNCTION SRCH_DCH_INIT

FILE NAME

DESCRIPTION
  This function performs search module DCH initializations. It is called
  by L1 manager when L1 enters the "DCH" state. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_dch_init( void );

/*===========================================================================

FUNCTION SRCH_DCH_CLEANUP

FILE NAME

DESCRIPTION
  This function performs search module DCH cleanup. It is called
  by L1 manager when L1 exits the DCH state. 

DEPENDENCIES
  None.

RETURN VALUE
  Boolean value TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_dch_cleanup( l1m_state_enum_type next_state );

#endif
