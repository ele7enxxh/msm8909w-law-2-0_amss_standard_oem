#ifndef TIME_REMOTE_REG_H
#define TIME_REMOTE_REG_H

/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M                 
                        
                
GENERAL DESCRIPTION

  Implements modem <-> applications processor remote caqllback registration functions


EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None


  Copyright(c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_remote_reg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/5/09    din     Added mechainsm to remote Generic Framework.
08/27/09   pc      file created

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff.h"
#include "timetick.h"


/* ========================================================================= */
/* ========================== Client group data types ====================== */
/* ========================================================================= */

/* Configuration event types */
typedef enum
{
  ATS_CFG_EVENT_TOD_CHANGE = 0,
  ATS_CFG_EVENT_GENOFF_CHANGE,
  ATS_CFG_EVENT_TIMER_IPC_CHANGE,
  ATS_CFG_EVENT_MAX32 = 0x10000000
} ats_cfg_event_e_type;

#define ATS_CFG_EVENT_MAX  ( ATS_CFG_EVENT_TIMER_IPC_CHANGE + 1 )  

/* TOD update configuration */
typedef struct ats_tod_update_s
{
  /* Base time tick */
  timetick_type tick;

  /* Base time stamp */
  time_type stamp;

  /* Frequency info from timetick module */
  uint32 freq;

} ats_tod_update_s_type;

/* GENOFF update configuration */
typedef struct ats_genoff_update_s
{
 /* Time base to be updated */
  time_bases_type   base_type;

  /* Updated Generic Offset */ 
  int64 offset;

} ats_genoff_update_s_type;

typedef struct ats_timer_ipc_update_s
{
  /* expired_handle */ 
  uint32 expired_handle ;

} ats_timer_ipc_update_s_type;


/* Configuration event information for each event */
typedef union ats_cfg_event_info_u
{
  ats_tod_update_s_type           ats_tod_update;
  /* ATS_CFG_EVENT_TOD_CHANGE */
  /*~ CASE ATS_CFG_EVENT_TOD_CHANGE ats_cfg_event_info_u.ats_tod_update */

  ats_genoff_update_s_type       ats_genoff_update;
  /* ATS_CFG_EVENT_GENOFF_CHANGE */
  /*~ CASE ATS_CFG_EVENT_GENOFF_CHANGE ats_cfg_event_info_u.ats_genoff_update */

  ats_timer_ipc_update_s_type    ats_timer_ipc_update;
  /* ATS_CFG_EVENT_GENOFF_CHANGE */
  /*~ CASE ATS_CFG_EVENT_TIMER_IPC_CHANGE ats_cfg_event_info_u.ats_timer_ipc_update */


} ats_cfg_event_info_s_type;

/* Client types */
typedef enum
{
  ATS_CLIENT_TYPE_GEN1 = 0,   /* Symbian App update :: Ed Morris needs to use this */
  ATS_CLIENT_FLOATING1,
  ATS_CLIENT_FLOATING2,
  ATS_CLIENT_TYPE_INTERNAL,   /* Internal use only! */
  ATS_CLIENT_TYPE_GENOFF_UPDATE,
  ATS_CLIENT_TYPE_MM_GENOFF_UPDATE,
  ATS_CLIENT_TYPE_MAX32 = 0x10000000   /* pad to 32 bit */
} ats_client_type_e_type;

typedef ats_client_type_e_type ats_mm_client_type_e_type;

#define ATS_CLIENT_TYPE_MAX (ATS_CLIENT_TYPE_MM_GENOFF_UPDATE+1)
#define ATS_MM_CLIENT_TYPE_MAX ATS_CLIENT_TYPE_MAX 

typedef enum
{
  ATS_CLIENT_PROCESSOR_NONE   = 0,
  ATS_CLIENT_PROCESSOR_MODEM,
  ATS_CLIENT_PROCESSOR_APP1,
  ATS_CLIENT_PROCESSOR_APP2,
  ATS_CLIENT_PROCESSOR_MM_MODEM,  
  ATS_CLIENT_PROCESSOR_MAX32
} ats_client_processor_e_type;
/*~ SENTINEL ats_client_processor_e_type.ATS_CLIENT_PROCESSOR_MAX32*/ 

#define ATS_CLIENT_PROCESSOR_MAX ( ATS_CLIENT_PROCESSOR_MM_MODEM + 1 )

/* Client ID */
typedef uint8 ats_client_id_type;

/* Client request errors */
typedef enum
{
  ATS_CLIENT_ERR_NONE,
  ATS_CLIENT_ERR_CLIENT_ID_PTR,     /* Invalid client ID pointer*/
  ATS_CLIENT_ERR_CLIENT_TYPE,       /* Invalid client type */
  ATS_CLIENT_ERR_CLIENT_ID,         /* Invalid client ID */
  ATS_CLIENT_ERR_TASK_NOT_READY,    /* ATS task is not ready for clients */
  ATS_CLIENT_ERR_INVALID_PROCESSOR, /* Invalid processor id */
  ATS_CLIENT_ERR_UNSUPPORTED,       /* Unsupported request */
  ATS_CLIENT_ERR_GENERAL,           /* Any General Error */
  ATS_CLIENT_ERR_RPC,               /* Any ONCRPC Error */  
  ATS_CLIENT_ERR_ALREADY_REG,        /* Client already registered */ 
  ATS_CLIENT_ERR_MAX32 = 0x10000000    /* pad to 32 bit int */
} ats_client_err_e_type;

#define ATS_CLIENT_ERR_MAX (ATS_CLIENT_ERR_ALREADY_REG + 1)


/* Configuration event callback */
typedef ats_client_err_e_type (*ats_cfg_event_cb_type)
(
  ats_cfg_event_e_type         event,
  ats_cfg_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK ats_cfg_event_cb_type 
    ONERROR return ATS_CLIENT_ERR_RPC */

/* Configuration event callback */
typedef ats_client_err_e_type (*ats_mm_cfg_event_cb_type)
(
  ats_cfg_event_e_type         event,
  ats_cfg_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK ats_mm_cfg_event_cb_type 
    ONERROR return ATS_CLIENT_ERR_RPC */


typedef struct
{
  boolean                      initialized;

  /* if true, activated and cb can be called */
  boolean                      activated;
  
  ats_cfg_event_cb_type        cfg_event_cb;

  ats_client_processor_e_type  processor;

} ats_client_s_type;

/*This structure is for mm fusion*/
typedef struct
{
  boolean                      initialized;

  /* if true, activated and cb can be called */
  boolean                      activated;
  
  ats_mm_cfg_event_cb_type     cfg_event_cb;

  ats_client_processor_e_type  processor;

} ats_mm_client_s_type;

#endif /* TIME_REMOTE_REG_H */


