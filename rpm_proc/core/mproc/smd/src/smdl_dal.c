
/*---------------------------------------------------------------------------
   SMDL_DAL.C  - Shared Memory Driver Lite DAL
---------------------------------------------------------------------------*/
/**
  @file smdl_dal.c

*/
/* This file contains the Standalone SMD Lite OS abstraction functions, using 
   DALSYS system calls. */


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMD_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMD_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smd" group description 
         in the SMD_mainpage file. 
*/

/*--------------------------------------------------------------------------
Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smdl_dal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/13   bt      Add smd_string_copy to OS files.
05/07/14   bm      Modify snd_event_send() to read W//R/U flags to ensure 
                   flush for RPM
01/28/13   bt      Add TZ interrupt definitions for RPM, using SMEM_RSVD3 host.
11/06/12   bt      Enable SMD interrupts for edges based on DevConfig.
10/05/12   bt      Moved taskless-specific loopback functions to new file.
09/19/12   bt      Use 0 for priority of Workloop to fall back to default.
09/05/12   pa      Fix compile errors and warnings.
08/23/12   bm      Fix smd_isr() declaration to support RPM and non-RPM procs.
08/22/12   bt      Rename smd_enable/disable_intr.
07/31/12   pa      Added N-way loopback workloop.
07/18/12   bt      Add Interrupt Enable/Disable APIs, refactor intr register.
05/24/12   bm      Use smem_host_type to identify hosts
05/03/12   bt      Clean up unneeded headers, use on ADSP as well.
04/09/12   bt      Remove smd_target.h, move processor defs to smd_proc.h.
03/30/12   pa      Added smd_lite_init for use with RCINIT
11/30/11   bt      Added smd_cs_event_create for SMDL interrupt context.
11/22/11   bt      Change Tramp interrupt APIs to DALInterruptController.
08/23/11   bt      Set smd_context.process_loopback to support Nway loopback.
04/07/11   rs      Make SMD channel definitions compatible across targets   
12/06/10   rs      Changed SMD Open List critical section to NO PREEMPTION 
                   since it is called from ISR context. 
11/26/10   rs      + Integrated change to register interrupts using the SMD 
                     interrupt table directly
                   + Cleaned up Doxygen comments.
10/15/10   rs      Initial revision (derived from src/os/rex/smd_rex.c).
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smd_os.h"
#include "smd.h"
#include "smd_internal.h"
#include "smd_proc.h"
#include "smd_os_common.h"
#include "smd_main.h"
#include "smd_internal.h"

#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIIPCInt.h"
#include "DDIInterruptController.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/

/**
 * SMD context structure for each running task.
 *
 * Standalone SMD Lite is taskless, hence only one context 
 * exists for all active ports. 
 */
static smd_context_type smd_context;

/** The critical section structure for the dtr function assignment. */
static DALSYSSyncHandle  smd_dtr_cs = NULL;

/** The critical section structure for the IOCTL command count */
static DALSYSSyncHandle  smd_ioctl_cs = NULL;

/** The critical section structure for the Watermark Queue assignment */
static DALSYSSyncHandle  smd_dsm_cs = NULL;

/* The critical section that ensures incoming SMD interrupts are 
 * enabled/disabled atomically. */
static DALSYSSyncHandle  smd_inc_intr_cs = NULL;

/* DAL Outgoing Interrupt (IPCINT) device handle. */
static DalDeviceHandle *smd_intr_out_handle;

/* DAL Incoming Interrupt Controller device handle. */
static DalDeviceHandle *smd_intr_in_handle;

/** Table defining interrupts for each channel in the target */
smd_interrupt_info_type smd_interrupt_table[SMEM_NUM_HOSTS];

/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smd_isr
===========================================================================*/
/**
  Interrupt service routine for Standalone SMD Lite.
 
  This interrupt is received when the other processor wants the SMD Lite
  state-machine to service some channel. The service required may be an update
  of control signals, data has been read and more should be sent, or data has
  been written and needs to be read.
 
  The ISR sends all three signals: UPDATE, TRANSMIT and RECEIVE to the SMD
  handle command function.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  Calls into SMD main to handle the signal.
*/
/*=========================================================================*/
#ifdef SMD_RPM_PROC
void smd_isr( void ) __irq
#else
static void smd_isr( void *cntxt )
#endif
{
  smd_main( smd_context_get_default() );
}

/*===========================================================================
FUNCTION      smd_init_critical_sections
===========================================================================*/
/**
  Sets up critical sections used in Standalone SMD Lite, initializing
  critical sections for DTR, IOCT and DSM.

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  Initializes critical sections.
*/
/*=========================================================================*/
static void smd_init_critical_sections( void )
{
  smd_context_type *context;

  /* Initialize critical section for DTR */
  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                       &smd_dtr_cs, 
                                       NULL) )
  {
    ERR_FATAL("smd_init_critical_sections: DTR cs creation failed", 0, 0, 0);
  }

  /* Initialize critical section for IOCTL */
  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                       &smd_ioctl_cs, 
                                       NULL) )
  {
    ERR_FATAL("smd_init_critical_sections: IOCTL cs creation failed", 0, 0, 0);
  }

  /* Initialize critical section for DSM */
  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                       &smd_dsm_cs, 
                                       NULL) )
  {
    ERR_FATAL("smd_init_critical_sections: DSM cs creation failed", 0, 0, 0);
  }

  /* Initialize critical section for SMD Lite open ports list */
  context = smd_context_get_default();

  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION,
                                       &context->os.open_list_cs, 
                                       NULL) )
  {
    ERR_FATAL("smd_init_critical_sections: SMDL open list cs creation failed",
              0, 0, 0);
  }
  /* Initialize critical section for incoming SMD interrupt enable/disable. */
  smd_inc_intr_cs = smd_cs_event_create();
}

/*===========================================================================
  FUNCTION      smd_register_interrupt
===========================================================================*/
/**
  Registers the SMD ISR for the incoming interrupt specified by intr.

  @param[in]  intr    The internal smd_interrupt_table entry of the
                      interrupt to register for.

  @return
  SMD_STATUS_SUCCESS        Interrupt registered successfully.
  SMD_STATUS_INVALID_PARAM  The interrupt ID was invalid, and not registered.
  SMD_STATUS_NOT_INIT       Internal structures and handles are not 
                            initialized yet.
*/
/*=========================================================================*/
static int32 smd_register_interrupt
(
  smd_interrupt_info_type *intr
)
{
  int32 ret_val;
  
  if( intr->irq_in != SMD_NULL_IN_INTERRUPT )
  {
    if( DalInterruptController_RegisterISR(
            smd_intr_in_handle,
            (uint32)intr->irq_in,
            (DALIRQ)smd_isr,
            (DALIRQCtx)intr->to,
            DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER ) != DAL_SUCCESS )
    {
      ret_val = SMD_STATUS_NOT_INIT;
    }
    ret_val = SMD_STATUS_SUCCESS;
  }
  else
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION      smd_deregister_interrupt
===========================================================================*/
/**
  Deregisters the SMD ISR which was previously registered for the incoming 
  interrupt specified by intr.

  @param[in]  intr    The internal smd_interrupt_table entry of the
                      interrupt to deregister.

  @return
  SMD_STATUS_SUCCESS        Interrupt deregistered successfully.
  SMD_STATUS_INVALID_PARAM  The interrupt ID was invalid, and not deregistered.
  SMD_STATUS_NOT_INIT       Internal structures and handles are not initialized yet, 
                            or interrupt was not registered.

*/
/*=========================================================================*/
static int32 smd_deregister_interrupt
(
  smd_interrupt_info_type *intr
)
{
  int32 ret_val;
  
  if( intr->irq_in != SMD_NULL_IN_INTERRUPT )
  {
    if( DalInterruptController_Unregister(
            smd_intr_in_handle,
            (uint32)intr->irq_in ) != DAL_SUCCESS )
    {
      ret_val = SMD_STATUS_NOT_INIT;
    }
    ret_val = SMD_STATUS_SUCCESS;
  }
  else
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  return ret_val;
}

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smd_event_send
===========================================================================*/
/**
  Sends an event to the specified processor via interrupt.
 
  @param[in] info         SMD Lite port information.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Interrupts the remote processor.
*/
/*=========================================================================*/
void smd_event_send(smd_stream_info_type *info)
{
  /* ensure memory is consistent before sending interrupt */
  SMD_MEMORY_BARRIER();

#ifdef SMD_RPM_PROC
  {
    volatile uint32 sig;
    /* MEMORY_BARRIER on RPM does not guarantee writes flushed to MSG RAM 
     * read back the Data to guarantee flush */
    sig = SMD_READ_IF_SIG(info, SMD_TX_HDR, data_read);
    sig = SMD_READ_IF_SIG(info, SMD_TX_HDR, data_written);
    sig = SMD_READ_IF_SIG(info, SMD_TX_HDR, state_updated);
  
    SMD_MEMORY_BARRIER();
  }
#endif
  /* Interrupt the remote processor */
  smd_send_interrupt( info->channel_type );
}

/*===========================================================================
FUNCTION      smd_event_send_smd
===========================================================================*/
/**
  Sends an event to the Standalone SMD Lite state-machine.
 
  @param[in] event        unused.

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  Handles the SMD Lite event.
*/
/*=========================================================================*/
void smd_event_send_smd( uint32 event )
{

}

/*===========================================================================
FUNCTION      smd_dtr_lock
===========================================================================*/
/**
  Locks/unlocks DTR callback assignment critical section.

  This function protects DTR callback assignment using critical sections.

  @param[in] lock         TRUE locks; FALSE unlocks

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_dtr_lock( boolean lock )
{
  if( lock )
  {
    DALSYS_SyncEnter( smd_dtr_cs );
  }
  else
  {
    DALSYS_SyncLeave( smd_dtr_cs );
  }
}

/*===========================================================================
FUNCTION      smd_ioctl_lock
===========================================================================*/
/**
  Locks/unlocks IOCTL command count critical section.

  This function protects IOCTL command count using critical sections.

  @param[in] lock         TRUE locks; FALSE unlocks

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  Interrupts the target processor.
*/
/*=========================================================================*/
void smd_ioctl_lock( boolean lock )
{
  if( lock )
  {
    DALSYS_SyncEnter( smd_ioctl_cs );
  }
  else
  {
    DALSYS_SyncLeave( smd_ioctl_cs );
  }
}

/*===========================================================================
FUNCTION      smd_dsm_lock
===========================================================================*/
/**
  Locks/unlocks Watermark Queue assignment critical section.

  This function protects Watermark Queue assignment using critical sections.

  @param[in] lock         TRUE locks; FALSE unlocks

@return       None
*/
/*=========================================================================*/
void smd_dsm_lock( boolean lock )
{
  if( lock )
  {
    DALSYS_SyncEnter( smd_dsm_cs );
  }
  else
  {
    DALSYS_SyncLeave( smd_dsm_cs );
  }
}

/*===========================================================================
FUNCTION      smd_cs_lock
===========================================================================*/
/**
  Locks/unlocks SMD Lite port critical section.

  This function protects access to the SMD Lite port.
 
  @param[in] cs           Pointer to the critical section
  @param[in] lock         TRUE locks; FALSE unlocks

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  Lock/unlocks exclusive access to the SMD lite port.
*/
/*=========================================================================*/
void smd_cs_lock( void *cs, boolean lock )
{
  DALSYSSyncHandle *cs_ptr = ( DALSYSSyncHandle * ) cs;

  if( lock )
  {
    DALSYS_SyncEnter(cs_ptr);
  }
  else
  {
    DALSYS_SyncLeave(cs_ptr);
  }
}

/*===========================================================================
FUNCTION      smd_cs_create
===========================================================================*/
/**
  Creates SMD Lite port critical section.

  @return
  Pointer to the critical section.
 
  @dependencies
  None.

  @sideeffects
  Initialized SMD Lite port critical section.
*/
/*=========================================================================*/
void *smd_cs_create(void)
{
  DALSYSSyncHandle  cs = NULL;

  /* Create the new critical section.  On RIVA, using this lock from interrupt 
   * context will cause a crash, so smd_cs_event_create() must be used 
   * instead. */
  if( DAL_SUCCESS != DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE,
                                        &cs, 
                                        NULL ) )
  {
    ERR_FATAL("smd_cs_create: Critical section creation failed", 0, 0, 0);
  }

  return ((void *)cs);
}

/*===========================================================================
FUNCTION      smd_cs_destroy
===========================================================================*/
/**
  Deletes the SMD Lite port critical section.
 
  @param[in] cs           Pointer to the critical section

  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  Initialized SMD Lite port critical section.
*/
/*=========================================================================*/
void smd_cs_destroy( void *cs )
{
  DALSYS_DestroyObject( ( DALSYSSyncHandle ) cs );
}

/*===========================================================================
FUNCTION      smd_target_init
===========================================================================*/
/**
  Performs target specific initialization: interrupt registrations, and
  critical sections used by SMD/SMD Lite.
 
  @return
  None.
 
  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smd_target_init(void)
{
  uint32 i;
  int32  ret_val;

  /* Create Interrupt Handler */
  if( DAL_SUCCESS == DAL_DeviceAttach( DALDEVICEID_IPCINT, &smd_intr_out_handle ) )
  {
    /* Create Incoming Interrupt Controller Handle */
    if( ( DAL_SUCCESS != DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER, 
                                           &smd_intr_in_handle ) )
            || (smd_intr_in_handle == NULL) )
    {
      ERR_FATAL("SMD Target Init: Interrupt Controller Handle creation failed!", 
                0, 0, 0);
    }

    /* Init critical sections */
    smd_init_critical_sections();

    /* Populate SMD interrupt table */
    smd_target_init_interrupts(SMD_THIS_HOST);

    /* Enable Nway SMD loopback server. */
    smd_context.process_loopback = TRUE;
    
    /* Register SMD interrupts */
    for( i = 0; i < SMEM_NUM_HOSTS; i++ )
    {
      smd_interrupt_info_type *intr = &(smd_interrupt_table[i]);
      ret_val = smd_register_interrupt(intr);
      /* If its not a valid interrupt, that's fine, ignore it. */
      if ( ret_val == SMD_STATUS_NOT_INIT )
      {
        ERR_FATAL("smd_target_init: SMD ISR for host %d failed to register.",
                    i, 0, 0);
      }
    }
  }
  else
  {
    ERR_FATAL("SMD Target Init: Interrupt Handler creation failed!", 0, 0, 0);
  }
}

/*===========================================================================
  FUNCTION  smd_send_interrupt
===========================================================================*/
/**
  Sends an interrupt to the target processor in the specified channel. 

  @param[in] channel      Channel between the source and target processors

  @return
  NA.

  @dependencies
  None.

  @sideeffects
  Interrupts the target processor.
*/
/*=========================================================================*/
void smd_send_interrupt( smd_channel_type channel )
{
  smd_interrupt_info_type *intr;

  intr = smd_get_interrupt_info( channel );

  if( SMD_NULL_OUT_INTERRUPT != intr->irq_out )
  {
    DalIPCInt_Trigger(smd_intr_out_handle, intr->processor, intr->irq_out);
  }
}

/*===========================================================================
FUNCTION      smd_assign_context
===========================================================================*/
/**
  Assigns an SMD context to the provided SMD port, based on the
  requested priority.

  This function must be called after populating the info structure
  from the API function that opens the SMD port, before calling
  smd_event_send_smd() to notify the particular task responsible
  for handling the port.

  @param[in,out] info    Pointer to the SMD info structure on which to assign
                         the SMD context. This function will populate
                         info->context with the appropriate task context.

  @param[in] priority    The priority requested by the client. If SMD is
                         implemented using multiple tasks with different OS
                         priorities for ports of different priorities, this
                         function will use this parameter to assign the context
                         to the appropriate task.

  @return
  None.
*/
/*=========================================================================*/
void smd_assign_context( smd_info_type * info, smd_priority_type priority )
{
  ASSERT(priority < SMD_PRIORITY_COUNT);

  info->context = &smd_context;
}

/*===========================================================================
FUNCTION      smd_context_get_default
===========================================================================*/
/**
  Returns a pointer to the default SMD context.

  This function is used by smd_cmd() to get the default SMD
  context used for handling loopback servers.

  @return
  Default SMD context.
*/
/*=========================================================================*/
smd_context_type * smd_context_get_default(void)
{
  return &smd_context;
}

/*===========================================================================
FUNCTION      smd_cmd_broadcast
===========================================================================*/
/**
  Sends an SMD command to all SMD tasks

  This function iterates through the list of SMD tasks and calls
  smd_cmd_context() to send the command to each task.

  This function is implemented by the OS-specific code, which
  alone has visibility into the true number of tasks and which
  ports they correspond to.

  @param[in]    cmd_ptr    Pointer of the command buffer to send

  @return
  None.
*/
/*=========================================================================*/
void smd_cmd_broadcast( smd_cmd_type *cmd_ptr )
{
  smd_cmd_context(cmd_ptr, &smd_context);
}


/*===========================================================================
FUNCTION      smd_open_list_lock
===========================================================================*/
/**
  Locks/unlocks SMD Lite open ports list critical section.

  This function protects the standalone SMD Lite open ports list, accessed from
  both client's and ISR context.

  @param[in] context      Owner of the SMD Lite open ports list
 
  @param[in] lock         TRUE locks; FALSE unlocks

  @return
  None.
*/
/*=========================================================================*/
void smd_open_list_lock( smd_context_type *context, boolean lock )
{
  if( lock )
  {
    DALSYS_SyncEnter( context->os.open_list_cs );
  }
  else
  {
    DALSYS_SyncLeave( context->os.open_list_cs );
  }
}

/*===========================================================================
FUNCTION      smd_cs_event_create
===========================================================================*/
/**
  Initializes a Critical Section suitable for being locked from 
  taskless SMDL's interrupt context.

  @return   The critical section, as a void pointer.
===========================================================================*/
void *smd_cs_event_create(void)
{
  DALSYSSyncHandle  cs = NULL;

  /* Create the new critical section.  The event cs is used throughout the SMDL 
   * event-signaling client-callback-wrappers.  It is also locked 
   * sparingly in other functions for purposes of read synchronization.  The 
   * sync object properies are abstracted to the processor-specific file. */
  if( DAL_SUCCESS != DALSYS_SyncCreate( SMD_EVENT_MUTEX_TYPE,
                                        &cs, 
                                        NULL ) )
  {
    ERR_FATAL("smd_cs_event_create: Critical section creation failed", 0, 0, 0);
  }

  return ((void *)cs);
}

/*===========================================================================
FUNCTION      smd_lite_init
===========================================================================*/
/**
  Wrapper function, called by RC init.  Calls smdl_init.

  @return   None
===========================================================================*/
void smd_lite_init(void)
{
  (void)smdl_init(0);
}

/*===========================================================================
  FUNCTION      smd_enable_intr
===========================================================================*/
/**
  Enables the SMD ISR for the specified processor's incoming interrupt, if it 
  was previously disabled.
  
  This function will do nothing if the interrupt is already enabled.

  @param[in]  host    The smem_host_type for the processor whose incoming 
                      interrupt to enable.

  @return
  SMD_STATUS_SUCCESS        Interrupt enabled successfully, or was already 
                            enabled.
  SMD_STATUS_INVALID_PARAM  The host or interrupt ID was invalid, and not 
                            registered.
  SMD_STATUS_NOT_INIT       Interrupt structures and handles are not 
                            initialized yet.

  @sideeffects
  Will soft-trigger the interrupt, and hence the ISR, upon re-enablement.
*/
/*=========================================================================*/
int32 smd_enable_intr( smem_host_type host )
{
  int32 ret_val;
  
  if ( host >= SMEM_NUM_HOSTS )
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  else if ( smd_intr_in_handle == NULL )
  {
    ret_val = SMD_STATUS_NOT_INIT;
  }
  else 
  {
    smd_interrupt_info_type *intr;
    smd_cs_lock( smd_inc_intr_cs, TRUE );
    intr = &(smd_interrupt_table[host]);
    if ( intr->disabled )
    {
      /* (Re)register the incoming interrupt. */
      if ( (ret_val = smd_register_interrupt(intr)) == SMD_STATUS_SUCCESS )
      {
        /* Soft-trigger the interrupt now, since we probably missed one while 
         * it was previously disabled.  Assert, because at this point the 
         * intr and handle have already been validated. */
        ASSERT(DalInterruptController_InterruptTrigger(smd_intr_in_handle,
                  (uint32)intr->irq_in) == DAL_SUCCESS);
        /* Track that the interrupt is not disabled. */
        intr->disabled = FALSE;
      }
    }
    else
    {
      /* The interrupt is already enabled. */
      ret_val = SMD_STATUS_SUCCESS;
    }
    smd_cs_lock( smd_inc_intr_cs, FALSE );
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION      smd_disable_intr
===========================================================================*/
/**
  Disables the SMD ISR for the specified processor's incoming interrupt, if it 
  was previously enabled.
  
  This function will do nothing if the interrupt is already disabled.

  @param[in]  host    The smem_host_type for the processor whose incoming 
                      interrupt to enable.

  @return
  SMD_STATUS_SUCCESS        Interrupt disabled successfully, or was already 
                            disabled.
  SMD_STATUS_INVALID_PARAM  The host or interrupt ID was invalid.
  SMD_STATUS_NOT_INIT       Interrupt structures and handles are not 
                            initialized yet.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smd_disable_intr( smem_host_type host )
{
  int32 ret_val;
  
  if ( host >= SMEM_NUM_HOSTS )
  {
    ret_val = SMD_STATUS_INVALID_PARAM;
  }
  else if (smd_intr_in_handle == NULL)
  {
    ret_val = SMD_STATUS_NOT_INIT;
  }
  else
  {
    smd_interrupt_info_type *intr;
    smd_cs_lock( smd_inc_intr_cs, TRUE );
    intr = &(smd_interrupt_table[host]);
    if ( !intr->disabled )
    {
      /* Deregister the incoming interrupt. */
      if ( (ret_val = smd_deregister_interrupt(intr)) == SMD_STATUS_SUCCESS )
      {
        /* Track that the interrupt has been disabled. */
        intr->disabled = TRUE;
      }
    }
    else
    {
      /* The interrupt is already disabled. */
      ret_val = SMD_STATUS_SUCCESS;
    }
    smd_cs_lock( smd_inc_intr_cs, FALSE );
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION  smd_target_init_interrupts
===========================================================================*/
/**
  This function initializes the interprocessor interrupts for this target.
  
  @param[in]  this_host   source of the interrupts

  @return
  None

  @sideeffect
  Initializes the interrupt table

  @dependencies
  None
*/
/*==========================================================================*/
void smd_target_init_interrupts( smem_host_type this_host )
{
  uint32 i;
  byte *smd_intr_enbl_arr;
  DALSYSPropertyVar prop;
  /* The handle is a uint32[2], so it won't be null after declaration. */
  DALSYS_PROPERTY_HANDLE_DECLARE(smd_prop_handle);
  
  /* Clear out the interrupt table */
  for( i = 0; i < SMEM_NUM_HOSTS; i++ )
  {
    smd_interrupt_table[i].to = SMEM_INVALID_HOST;
    smd_interrupt_table[i].processor = (smd_os_proc_type)0;
    smd_interrupt_table[i].irq_out = SMD_NULL_OUT_INTERRUPT;
    smd_interrupt_table[i].irq_out_mask = 0;
    smd_interrupt_table[i].irq_in = SMD_NULL_IN_INTERRUPT;
    smd_interrupt_table[i].disabled = FALSE;
  }
  
  if (DALSYS_GetDALPropertyHandleStr("/core/mproc/smd", smd_prop_handle) !=
        DAL_SUCCESS)
  {
    ERR_FATAL("smd_target_init_interrupts: Cannot get DALProp handle.", 
              0, 0, 0);
  }
  else if (DALSYS_GetPropertyValue(smd_prop_handle, "smd_intr_enabled", 0, 
                                   &prop) != DAL_SUCCESS)
  {
    ERR_FATAL("smd_target_init_interrupts: Cannot get smd_intr_enabled DAL "
              "prop.", 0, 0, 0);
  }
  /* The SMD intr enable array will be put in prop.Val.pbVal (array of bytes) */
  smd_intr_enbl_arr = prop.Val.pbVal;
  
  /* From/to APPS Processor */
  if (smd_intr_enbl_arr[SMEM_APPS])
  {
    smd_interrupt_table[SMEM_APPS].to            = SMEM_APPS;
    smd_interrupt_table[SMEM_APPS].processor     = SMD_APPS_IPCPROC;
    smd_interrupt_table[SMEM_APPS].irq_out       = SMD_APPS_OUT;
    smd_interrupt_table[SMEM_APPS].irq_out_mask  = 0; /* Not used. */
    smd_interrupt_table[SMEM_APPS].irq_in        = SMD_APPS_IN;
  }
  
  /* From/to Modem processor */
  if (smd_intr_enbl_arr[SMEM_MODEM])
  {
    smd_interrupt_table[SMEM_MODEM].to           = SMEM_MODEM;
    smd_interrupt_table[SMEM_MODEM].processor    = SMD_MPSS_IPCPROC;
    smd_interrupt_table[SMEM_MODEM].irq_out      = SMD_MPSS_OUT;
    smd_interrupt_table[SMEM_MODEM].irq_out_mask = SMD_MPSS_OUT_M;
    smd_interrupt_table[SMEM_MODEM].irq_in       = SMD_MPSS_IN;
  }

  /* From/to ADSP Q6 (LPASS) processor */
  if (smd_intr_enbl_arr[SMEM_Q6])
  {
    smd_interrupt_table[SMEM_Q6].to              = SMEM_Q6;
    smd_interrupt_table[SMEM_Q6].processor       = SMD_ADSP_IPCPROC;
    smd_interrupt_table[SMEM_Q6].irq_out         = SMD_ADSP_OUT;
    smd_interrupt_table[SMEM_Q6].irq_out_mask    = SMD_ADSP_OUT_M;
    smd_interrupt_table[SMEM_Q6].irq_in          = SMD_ADSP_IN;
  }

  /* From/to RIVA processor */
  if (smd_intr_enbl_arr[SMEM_RIVA])
  {
    smd_interrupt_table[SMEM_RIVA].to            = SMEM_RIVA;
    smd_interrupt_table[SMEM_RIVA].processor     = SMD_WCNSS_IPCPROC;
    smd_interrupt_table[SMEM_RIVA].irq_out       = SMD_WCNSS_OUT;
    smd_interrupt_table[SMEM_RIVA].irq_out_mask  = SMD_WCNSS_OUT_M;
    smd_interrupt_table[SMEM_RIVA].irq_in        = SMD_WCNSS_IN;
  }

  /* From/to RPM processor */
  if (smd_intr_enbl_arr[SMEM_RPM])
  {
    smd_interrupt_table[SMEM_RPM].to             = SMEM_RPM;
    smd_interrupt_table[SMEM_RPM].processor      = SMD_RPM_IPCPROC;
    smd_interrupt_table[SMEM_RPM].irq_out        = SMD_RPM_OUT;
    smd_interrupt_table[SMEM_RPM].irq_out_mask   = SMD_RPM_OUT_M;
    smd_interrupt_table[SMEM_RPM].irq_in         = SMD_RPM_IN;
  }
  
  /* From/to TZ processor - using SMEM_RSVD3 so SMEM isn't affected. */
  smd_interrupt_table[SMEM_RSVD3].to             = SMEM_RSVD3;
  smd_interrupt_table[SMEM_RSVD3].processor      = SMD_TZ_IPCPROC;
  smd_interrupt_table[SMEM_RSVD3].irq_out        = SMD_TZ_OUT;
  smd_interrupt_table[SMEM_RSVD3].irq_out_mask   = SMD_TZ_OUT_M;
  smd_interrupt_table[SMEM_RSVD3].irq_in         = SMD_TZ_IN;
  
}

/*===========================================================================
  FUNCTION  smd_string_copy
===========================================================================*/
/**
  Copies the source string into the destination buffer until 
  size is reached, or until a '\0' is encountered.  If valid,
  the destination string will always be NULL deliminated.
  
  @param[in] dst    The destination string, contents will be updated.
  @param[in] src    The source string
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
char *smd_string_copy(char *dst, const char *src, uint32 size)
{
  (void)strlcpy(dst, src, size);

  return dst;
}
