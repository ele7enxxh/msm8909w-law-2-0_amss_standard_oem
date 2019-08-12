#ifndef SRCH4TASKSCHEDULER_H
#define SRCH4TASKSCHEDULER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R    H E A D E R

GENERAL DESCRIPTION
   This file contains the hardware drivers for searcher 4.

EXTERNALIZED FUNCTIONS


  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/srch4/src/srch4i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/14   ag      Added api for querying current inactivity time for debug
05/19/14   rj      Handle 24 Bit USTMR for BOLT using VSTMR API
04/20/13   sr      initial implementation

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_MODEM_COEXISTENCE_FW

#include "srch4trans.h"
#include "DalDevice.h"
#include "DDITimetick.h"


/*===========================================================================
                    MACRO DEFINITIONS FOR MODULE
===========================================================================*/
#define NUM_BUFFER_SLOTS 16
#ifdef FEATURE_BOLT_MODEM
#define SRCH4_MAX_USTMR_TIME  (0xFFFFFF)
#else
#define SRCH4_MAX_USTMR_TIME  (0x7FFFFFF)
#endif

/* We should not see time tick difference go more than half the max ustmr ticks */
#define SRCH4_CHECK_OVERRUN(tick1, tick2) (((tick1) - (tick2)) >= SRCH4_MAX_USTMR_TIME/2)  
#define SRCH4_APPLY_OVERRUN(expr) ((expr) & SRCH4_MAX_USTMR_TIME)
#define SRCH4_CHECK_OVERRUN_OVERLAP(expr1, expr2) (((expr1) + SRCH4_MAX_USTMR_TIME) < (expr2))

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* Handling Time Ticks in 64 bits in the module. Using a logical typedef
   instead of uint64 */
typedef uint64  srch4i_slottedscheduler_timetick_type;


/* Call-back Type which is used by Srch4drv to get notification
   each time cxm inactivity info changes. */
typedef void (*srch4i_slottedscheduler_change_info_cb_type)(void);


/* Datatype used for handling DAL interactions. */
typedef struct
{
  DalDeviceHandle*  handle;       /* Handle used by timetick APIs */
} srch4i_slottedscheduler_dal_info_type;


/* Struct for storing the inactivity info in a circular buffer. The slot
   info is obtained from CXM and the number of slots are constant at 15.
   The info is updated always at the end of the 15th slot. Hence, this
   queue contains 16 circular slots so that when update happens, if
   current slot is valid; it is not over-written.
   Write Index always point to the next location where write should happen.
   Read Index is not a hard read index since if the slots are updated but
   if no searches are scheduled in that time; the slots expire. Hence, if
   write is requested; every-time write has to be honored and read index
   should be re-set to write index + 1.
   Example:
   slot[0..14] = t0... t14. slot[15] is invalid. Write Index is at slot[15]
   and Read Index is at slot[8]. How-ever current time t could be t > t14.
   If a write request comes(always 15 slots), read index at 8 should not
   cause write to fail but should be over-written and read index should be
   re-set to slot[15]. */
typedef struct
{
  srch4_time_slot_type      slots[NUM_BUFFER_SLOTS];  /* inactivity slots of CXM */
  uint8                     read_index;               /* Read index */
  uint8                     write_index;              /* Write index */
} srch4i_slottedscheduler_inactivity_queue_type;


/* Datatype used for slotted scheduling. */
typedef struct
{
  /* Critical section - for initialization serialization, etc. */
  rex_crit_sect_type                             crit_sect;

  /* DAL Info - for converting timeticks to ms, micro seconds, etc . */
  srch4i_slottedscheduler_dal_info_type          dal_info;

  /* Inactivity Queue - for storing updated CXM Inactivity Data. */
  srch4i_slottedscheduler_inactivity_queue_type  inactivity_circular_queue;

  /* Info Modified Call-back - Used to call when-ever cxm info is modified. */
  srch4i_slottedscheduler_change_info_cb_type    srch4drv_cxm_info_modified_cb;
} srch4i_slottedscheduler_type;


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_INIT

DESCRIPTION
  This function initializes the internal variables required for slotted
  scheduling of searches.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_init(void);


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_REGISTER_CHANGE_INFO

DESCRIPTION 
  This function is used by Srch4drv to register for changes in the CxM
  inactivity table.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_register_change_info(
  srch4i_slottedscheduler_change_info_cb_type srch4drv_cxm_info_modified_cb
);


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_DEREGISTER_CHANGE_INFO

DESCRIPTION
  This function is used by Srch4drv to deregister for changes in the CxM
  inactivity table.
 
DEPENDENCIES
  None

RETURN VALUE
  trm_client_priority_enum_t

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_deregister_change_info(void);


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_CONV_TICKS_TO_UNIT

DESCRIPTION
  This function is used to convert ticks to a given unit.
 
DEPENDENCIES
  None

RETURN VALUE
  uint64 - converted value of ticks in the requested unit.

SIDE EFFECTS
 None.  

============================================================================*/
uint64 srch4i_slottedscheduler_conv_ticks_to_unit
(
  DalTimetickTime64Type     t_ticks,    /* Time in sclk64(or ticks) format */
  DalTimetickUnitType       unit        /* Desired unit for return value */
);




/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_CLEAR_CXM_INACTIVITY_INFO

DESCRIPTION 
  This method is used to clear the cxm inactivity table. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_clear_cxm_inactivity_info(void);



/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_UPDATE_CXM_INACTIVITY_SLOTS

DESCRIPTION
  Clears the cxm inactivity table indicating that there is no more G Tx
  occurring.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_update_cxm_inactivity_slots(
  srch4_cxm_inactivity_slot_type* time_slots /* New Inactivity Slots Info */
);


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_GET_CURRENT_INACTIVITY_INFO

DESCRIPTION 
  Provides the inactivity info corresponding to the current time.

  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_get_current_inactivity_info(
  srch4_time_slot_type *time_slot   /* used to store current inactivity info */
);




/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_GET_NEXT_INACTIVITY_INFO

DESCRIPTION 
  Provides the next inactivity info corresponding to the current time. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_get_next_inactivity_info(
  srch4_time_slot_type *time_slot  /* used to store next inactivity info */
);


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_GET_TIME_TO_NEXT_INACTIVITY_SLOT

DESCRIPTION 
  Provides the time in micro-seconds left from reaching the next inactivity
  slot. 
  
DEPENDENCIES
  None

RETURN VALUE
  uint64 - Time in micro-seconds left for next inactivity slot.

SIDE EFFECTS
 None.  

============================================================================*/
uint64 srch4i_slottedscheduler_get_time_to_next_inactivity_slot(
  srch4_time_slot_type *time_slot  /* used to store next inactivity info */
);


/*============================================================================

FUNCTION SRCH4I_GET_CURRENT_TIMETICK_IN_MICROSEC

DESCRIPTION
  This function gets the current time-tick and converts it to microseconds.
 
DEPENDENCIES
  None

RETURN VALUE
  Value of current time-tick in microseconds.

SIDE EFFECTS
 None.  

============================================================================*/
uint64 srch4i_get_current_timetick_in_microsec(void);


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_GET_CURRENT_STMR

DESCRIPTION
  This function returns the current ustmr time.
 
DEPENDENCIES
  None

RETURN VALUE
  Current USTMR Time.

SIDE EFFECTS
 None.  

============================================================================*/
uint64 srch4i_slottedscheduler_get_current_ustmr(void);



/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_IS_VALID_STARTING_TIME

DESCRIPTION 
  Indicates if the starting time present with srch4drv is still valid or not. 
  
DEPENDENCIES
  None

RETURN VALUE
  True: Starting time is valid.
  False: Starting time is not valid anymore.

SIDE EFFECTS
 None.  

============================================================================*/
boolean srch4i_slottedscheduler_is_valid_starting_time(
  uint64 time_to_check /* starting time to check */
);

/*============================================================================

FUNCTION SRCH4I_QUERY_GET_CURRENT_INACTIVITY_INFO

DESCRIPTION 
  Provides the inactivity info corresponding to the current time. This does
  not update the circular queue's read/write index. simply does a query to
  check if there is a slot for current ustmr.

  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_query_current_inactivity_info(
  srch4_time_slot_type *time_slot /* used to store current inactivity info */
);

#endif
#endif /* SRCH4TASKSCHEDULER_H */
