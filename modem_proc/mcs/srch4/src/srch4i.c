/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R

GENERAL DESCRIPTION
   This file contains the hardware drivers for searcher 4.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/srch4/src/srch4i.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/04/14   ag      Added api to query current inactivity time for debug
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

#include "srch4i.h"
#include "msmhwioreg.h"
#include "mcs_hwio_modem.h"
#ifdef FEATURE_BOLT_MODEM
#include "vstmr.h"
#endif

/*===========================================================================
             LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/

/* Macros for critical section operations */
#define SRCH4SLOTTEDSCHEDULER_INIT_CRIT_SECT(lock)    rex_init_crit_sect(&(lock))
#define SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(lock)   rex_enter_crit_sect(&(lock))
#define SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(lock)   rex_leave_crit_sect(&(lock))

/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/


/* Used to store information about the slotted scheduling. */
srch4i_slottedscheduler_type srch4i_slottedscheduler_info;



/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/


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
uint64 srch4i_get_current_timetick_in_microsec(void)
{
  uint64 current_time;

  DalTimetick_GetTimetick64(srch4i_slottedscheduler_info.dal_info.handle, 
                            &current_time);

  current_time = srch4i_slottedscheduler_conv_ticks_to_unit(
                      current_time,T_USEC);

  return current_time;
} /* srch4i_get_current_timetick_in_microsec */



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
uint64 srch4i_slottedscheduler_get_current_ustmr(void)
{
  //uint64 current_time;
  uint32 current_time_ustmr;

  /*-----------------------------------------------------------------------*/

  /* USTMR Ticks */
#ifdef FEATURE_BOLT_MODEM
  current_time_ustmr = VSTMR_XO_READ();
#else
  current_time_ustmr = HWIO_IN(UNIV_STMR_MSTMR);
#endif
   
  return current_time_ustmr;

} /* srch4i_slottedscheduler_get_current_ustmr */

/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_INSERT_CXM_INACTIVITY_DATA

DESCRIPTION
  This function inserts cxm inactivity data into the internal buffer.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_insert_cxm_inactivity_data(
  srch4_cxm_inactivity_slot_type* time_slots /* Time Slot Data from CxM: In Ticks */
)
{

  uint8  write_index;
  uint32 counter;
  uint64 time_microsec;
  uint64 duration_microsec;
  srch4i_slottedscheduler_inactivity_queue_type *cirqueue;
   
  /*-----------------------------------------------------------------------*/

  /* This function is called to insert new data obtained from CxM into the
     internal variable for slotting the searches. CxM data is obtained in
     ticks and we would need to first convert it into micro-seconds and
     then store them in our internal variables */

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

  /* Get the circular queue*/
  cirqueue = &srch4i_slottedscheduler_info.inactivity_circular_queue;

  /* initialize the local variables */
  time_microsec     = 0;
  duration_microsec = 0;

  /* Initialize write index and last written write index */
  write_index = cirqueue->write_index;

  /* Check for over-flows */
  ASSERT(time_slots->num_slots < NUM_BUFFER_SLOTS);

  for( counter = 0; counter < time_slots->num_slots; ++counter )
  {
    /* Get the time and duration of the new slot information */      
    time_microsec =  time_slots->slots[counter].starting_time;
    duration_microsec = time_slots->slots[counter].duration;
          
    /* Store starting time in micro-second in circular queue
       at the spot popinted by write index */
    cirqueue->slots[write_index].starting_time = time_microsec;
    
                  
    /* Store duration in micro-second in circular queue
       at the spot popinted by write index */
    cirqueue->slots[write_index].duration = duration_microsec;


    /* Store the info available field*/
    cirqueue->slots[write_index].info_available = TRUE;

    /* Increment the write index field */
    write_index = (write_index + 1) % NUM_BUFFER_SLOTS;

    /* update the write index */
    cirqueue->write_index = write_index;
  }

  /* update the read_index to "write_index+1". We need to
     update read index each time write happens because
     we have a "hard-write" and "soft read" where we never
     fail a write from happening and all the writes need not
     be read. Once the write happens; the oldest record is
     at write_index. Hence, we point the read index there */
  cirqueue->read_index = (write_index + 1) % NUM_BUFFER_SLOTS;


  /* if the info is present and current info overlaps the last info present or
     if info is not at-all present, then we need to update srch4drv */
  /* call the srch4drv call-back to update about change in G Tx Info.
     It will be a no-op for srch4drv if there is no search currently
     under progress else it will reschedule the recent search */
  if( srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb != NULL )
  {
    srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb();
  }

  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

} /* srch4i_slottedscheduler_insert_cxm_inactivity_data */


/*============================================================================

FUNCTION SRCH4I_QUERY_CURRENT_INACTIVITY_INFO

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
)
{
  
  srch4i_slottedscheduler_timetick_type            current_time; 
  uint8                                            read_index;
  uint8                                            write_index;
  srch4_time_slot_type                             slot_read;
  srch4i_slottedscheduler_inactivity_queue_type    *cirqueue;

 /*-----------------------------------------------------------------------*/


  /* Initialize the local variables */
  current_time = srch4i_slottedscheduler_get_current_ustmr();
  cirqueue     = &srch4i_slottedscheduler_info.inactivity_circular_queue;

  /* Initialize the local variables */
  read_index   = (cirqueue->write_index + 1) % NUM_BUFFER_SLOTS;
  write_index  = cirqueue->write_index;


  /* Initialize the return variable */
  time_slot->info_available = FALSE;
  time_slot->duration       = 0xFFFFFFFF;
  time_slot->starting_time  = 0;

   for( ; ; )
  {


    /* End of the circular queue. Exit */
    if( read_index == write_index )
    {
      break;
    }

    slot_read = cirqueue->slots[read_index];


    /* If current time lies between the starting time of the slot
       and the starting time + duration; then that is the current
       inactivity slot */
       /* current time, A and B are after roll-over.. current time,
          A are before roll-over; B is after roll-over */
    if( (slot_read.info_available == TRUE &&    
       (current_time >= slot_read.starting_time) && 
        (current_time < (slot_read.starting_time + slot_read.duration))) ||
         /* working around roll-over of current time:
            A is before roll-over.. current time and B are after roll-over */
         (slot_read.info_available == TRUE &&
         ((current_time + 0x7FFFFFF) >= slot_read.starting_time) && 
         ((current_time + 0x7FFFFFF) <  (slot_read.starting_time + slot_read.duration)))
    )
    {
      time_slot->starting_time = slot_read.starting_time;
      time_slot->duration      = slot_read.duration;
      time_slot->info_available = TRUE;
      break;
    }

    /* read the next inactivity slot */
    read_index = (read_index + 1) % NUM_BUFFER_SLOTS;
  }
}


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
)
{
  uint64  t_unit = 0; /* Converted time in "unit" format */

  /*-----------------------------------------------------------------------*/

  ( void ) DalTimetick_CvtFromTimetick64( 
    srch4i_slottedscheduler_info.dal_info.handle,
    t_ticks,
    unit,
    &t_unit );

  return ( t_unit );

} /* srch4i_slottedscheduler_conv_ticks_to_unit */


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
void srch4i_slottedscheduler_init(void)
{
  DALResult result;

  /*-----------------------------------------------------------------------*/


  SRCH4SLOTTEDSCHEDULER_INIT_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

  /* Initialize the call-back function to null */
  srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb = NULL;

  /* Get handle for SystemTimer */
  result = DalTimetick_Attach( "SystemTimer", &srch4i_slottedscheduler_info.dal_info.handle );
  if( ( result != DAL_SUCCESS ) || ( srch4i_slottedscheduler_info.dal_info.handle == NULL ) )
  {
    ERR_FATAL( "Cannot attach to SystemTimer: result=%d, handle=0x%X",
               result, srch4i_slottedscheduler_info.dal_info.handle, 0 );
  }

  /* clear all the information in the inactivity info */
  srch4i_slottedscheduler_clear_cxm_inactivity_info();

} /* srch4i_slottedscheduler_init */



/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_REGISTER_CHANGE_INFO

DESCRIPTION 
  This function is used by Srch4drv to register for changes in the CxM
  inactivity table.
  
DEPENDENCIES
  srch4i_slottedscheduler_init should have been called. 

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4i_slottedscheduler_register_change_info(
  srch4i_slottedscheduler_change_info_cb_type srch4drv_cxm_info_modified_cb 
  /* callback function to call */
)
{
   /*-----------------------------------------------------------------------*/

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

  /* Store the call-back info to be called when cxm info changes */
  srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb = 
    srch4drv_cxm_info_modified_cb;

  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

} /* srch4i_slottedscheduler_register_change_info */



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
void srch4i_slottedscheduler_deregister_change_info(void)
{

   /*-----------------------------------------------------------------------*/

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

  /* deregister the change info */
  srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb = NULL;

  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

}  /* srch4i_slottedscheduler_deregister_change_info */


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
void srch4i_slottedscheduler_clear_cxm_inactivity_info(void)
{
  uint32 counter;
  srch4i_slottedscheduler_inactivity_queue_type *cirqueue;

 /*-----------------------------------------------------------------------*/
  
  
  cirqueue = &srch4i_slottedscheduler_info.inactivity_circular_queue;

  /* Intialize read and write index to beginning of the queue */
  cirqueue->read_index = 0;
  cirqueue->write_index = 0;

    
  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

  /* tranverse through all the buffers and clear them */
  for( counter=0; counter< NUM_BUFFER_SLOTS; ++counter )
  {
    /* Clear all the information */
    cirqueue->slots[counter].starting_time = 0;
    cirqueue->slots[counter].duration = 0;
    cirqueue->slots[counter].info_available = FALSE;
  }

  /* call the srch4drv call-back to update about change in G Tx Info.
     It will be a no-op for srch4drv if there is no search currently
     under progress else it will reschedule the recent search */
  if( srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb != NULL )
  {
    srch4i_slottedscheduler_info.srch4drv_cxm_info_modified_cb();
  }

  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);


} /* srch4i_slottedscheduler_clear_cxm_inactivity_info */


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
  srch4_cxm_inactivity_slot_type* time_slots /* Time Slot Data from CxM: In Ticks */
)
{

 /*-----------------------------------------------------------------------*/

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);


  if( time_slots->is_slotted == FALSE )
  {
    /* If not slotted(no Tx); then flush the info that is currently there */
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "GTX Inactivity flushed in update slots");
    srch4i_slottedscheduler_clear_cxm_inactivity_info();

  }
  else
  {

    /* CXM is in slotted transmit. Update/Add the new information */
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "GTX Inactivity added in update slots");
    srch4i_slottedscheduler_insert_cxm_inactivity_data(time_slots);

  }
  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

} /* srch4i_slottedscheduler_update_cxm_inactivity_slots */



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
  srch4_time_slot_type *time_slot /* used to store current inactivity info */
)
{
  
  srch4i_slottedscheduler_timetick_type            current_time; 
  uint8                                            read_index;
  uint8                                            write_index;
  srch4_time_slot_type                             slot_read;
  srch4i_slottedscheduler_inactivity_queue_type    *cirqueue;

 /*-----------------------------------------------------------------------*/


  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);

  /* Initialize the local variables */
  current_time = srch4i_slottedscheduler_get_current_ustmr();
  cirqueue     = &srch4i_slottedscheduler_info.inactivity_circular_queue;

  /* Initialize the local variables */
  read_index   = (cirqueue->write_index + 1) % NUM_BUFFER_SLOTS;
  write_index  = cirqueue->write_index;


  /* Initialize the return variable */
  time_slot->info_available = FALSE;
  time_slot->duration       = 0xFFFFFFFF;
  time_slot->starting_time  = 0;

   for( ; ; )
  {


    /* End of the circular queue. Exit */
    if( read_index == write_index )
    {
      break;
    }

    slot_read = cirqueue->slots[read_index];


    /* If current time lies between the starting time of the slot
       and the starting time + duration; then that is the current
       inactivity slot */
       /* current time, A and B are after roll-over.. current time,
          A are before roll-over; B is after roll-over */
    if( (slot_read.info_available == TRUE &&    
       (current_time >= slot_read.starting_time) && 
        (current_time < (slot_read.starting_time + slot_read.duration))) ||
         /* working around roll-over of current time:
            A is before roll-over.. current time and B are after roll-over */
         (slot_read.info_available == TRUE &&
         ((current_time + SRCH4_MAX_USTMR_TIME) >= slot_read.starting_time) && 
         ((current_time + SRCH4_MAX_USTMR_TIME) <  (slot_read.starting_time + slot_read.duration)))
    )
    {
      time_slot->starting_time = slot_read.starting_time;
      time_slot->duration      = slot_read.duration;
      time_slot->info_available = TRUE;
      break;
    }

    /* read the next inactivity slot */
    read_index = (read_index + 1) % NUM_BUFFER_SLOTS;
  }
   
  /* update the global variable from local variable */
  cirqueue->read_index = read_index; 

  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(srch4i_slottedscheduler_info.crit_sect);


} /* srch4i_slottedscheduler_get_current_inactivity_info */


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
)
{

  srch4i_slottedscheduler_timetick_type            current_time; 
  uint8                                            read_index;
  uint8                                            write_index;
  srch4_time_slot_type                             slot_read;
  srch4i_slottedscheduler_inactivity_queue_type    *cirqueue;

 /*-----------------------------------------------------------------------*/

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(
    srch4i_slottedscheduler_info.crit_sect);

  /* Get the pointer to the circular queue */
  cirqueue     = &srch4i_slottedscheduler_info.inactivity_circular_queue;

  /* Initialize the local variables */
  current_time = srch4i_slottedscheduler_get_current_ustmr();
  read_index   = (cirqueue->write_index + 1) % NUM_BUFFER_SLOTS;
  write_index  = cirqueue->write_index;

  /* Initialize the return variable */
  time_slot->info_available = FALSE;
  time_slot->duration       = 0xFFFFFFFF;
  time_slot->starting_time  = 0;

   for( ; ; )
  {


    /* End of the circular queue. Exit */
    if( read_index == write_index )
    {
      break;
    }

    /* get the slot being read */
    slot_read = cirqueue->slots[read_index];



    /* If current time lies between the starting time of the slot
       and the starting time + duration; then that is the current
       inactivity slot */
    if( slot_read.info_available == TRUE && 
	   (!SRCH4_CHECK_OVERRUN(slot_read.starting_time, current_time)) &&
       (current_time <= slot_read.starting_time
        /* handling current time roll-over */ 
        || SRCH4_CHECK_OVERRUN(current_time, slot_read.starting_time)))
    {
      time_slot->starting_time = slot_read.starting_time;
      time_slot->duration      = slot_read.duration;
      time_slot->info_available = TRUE;
      break;
    }

    /* read the next inactivity slot */
    read_index = (read_index + 1) % NUM_BUFFER_SLOTS;
  }

  /* update the global variable from local variable */
  cirqueue->read_index = read_index; 

  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(
    srch4i_slottedscheduler_info.crit_sect);


} /* srch4i_slottedscheduler_get_next_inactivity_info */



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
)
{
  srch4i_slottedscheduler_timetick_type            current_time; 
  uint64                                           duration;
  int64                                            temp_buffer;

 /*-----------------------------------------------------------------------*/

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(
    srch4i_slottedscheduler_info.crit_sect);

  /* Initialize the local variables */
  duration     = 0;
  temp_buffer  = 0;
  current_time = srch4i_slottedscheduler_get_current_ustmr();

  /* get the next inactivity slot */
  srch4i_slottedscheduler_get_next_inactivity_info(time_slot);

  if( !time_slot->info_available )
  {
    /* If there is no inactivity slot; then set duration as zero indicating
       that there is no need to wait for inactivity. */
    duration = 0;
  }
  else if( current_time > time_slot->starting_time 
           /* handling roll-over */
           && !SRCH4_CHECK_OVERRUN(current_time, time_slot->starting_time))
  {
    /* Current time cannot be greater than next slot's starting time.
       Error Fatal and start debugging !! */
    ERR_FATAL("Current time cannot be greater than next slot starting time.\
              Current Time(USTMR): %ld, Next Slot Start(USTMR)%ld",
              current_time, time_slot->starting_time, 0);
  }
  else if( current_time == time_slot->starting_time )
  {
    /* if current time is same as starting time; wait till the
       end of the "next" slot.Must have become current slot from next slot
       in micro seconds. */
    duration = time_slot->duration;
	
  /* convert ustmr ticks to micro seconds */
  duration = srch4i_slottedscheduler_conv_ticks_to_unit(
        duration,
        T_USEC
      );	
  }
  else
  {
    /* Calculate the time to next slot's starting time */

    temp_buffer = time_slot->starting_time - current_time;
    /* handle the roll-over possibility */
    duration    = SRCH4_APPLY_OVERRUN(temp_buffer);

  /* convert ustmr ticks to micro seconds */
  duration = srch4i_slottedscheduler_conv_ticks_to_unit(
        duration,
        T_USEC
      );

    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "Time to Next Inactivity: Current Time(USTMR): %ld, Next Slot Start(USTMR)%ld, Duration: %d",
          current_time,time_slot->starting_time,duration);
	
	
  }


	  
  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(
    srch4i_slottedscheduler_info.crit_sect);

  return duration;

} /* srch4i_slottedscheduler_get_time_to_next_inactivity_slot */




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
)
{
  boolean return_value = FALSE;
  uint32 loop_counter;
 srch4i_slottedscheduler_inactivity_queue_type    *cirqueue;

 /*-----------------------------------------------------------------------*/

  SRCH4SLOTTEDSCHEDULER_ENTER_CRIT_SECT(
    srch4i_slottedscheduler_info.crit_sect);

  /* Get the pointer to circular queue */
  cirqueue     = &srch4i_slottedscheduler_info.inactivity_circular_queue;

  /* Loop through all the buffers */
  for( loop_counter = 0; loop_counter < NUM_BUFFER_SLOTS; ++loop_counter  )
  {

    if( cirqueue->slots[loop_counter].info_available && 
        cirqueue->slots[loop_counter].starting_time == time_to_check )
    {
      /* If info is available and starting time is same as time to check */
      return_value = TRUE;
      break;
    }
  }
  
  if( return_value == FALSE )
  {
    return_value = TRUE;
    for( loop_counter = 0; loop_counter < NUM_BUFFER_SLOTS; ++loop_counter )
    {
      if( cirqueue->slots[loop_counter].info_available )
      {
        return_value = FALSE;
        break;
      }
    }
  }
  
  SRCH4SLOTTEDSCHEDULER_LEAVE_CRIT_SECT(
    srch4i_slottedscheduler_info.crit_sect);

  return return_value;

} /* srch4i_slottedscheduler_is_valid_starting_time */

#endif
