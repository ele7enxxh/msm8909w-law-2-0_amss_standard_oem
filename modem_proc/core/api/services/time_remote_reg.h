#ifndef TIME_REMOTE_REG_H
#define TIME_REMOTE_REG_H

/**
@file time_remote_reg.h
@brief
Implements modem <-> applications processor remote callback registration functions.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_remote group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2010 - 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M                 
                        
                
GENERAL DESCRIPTION

  Implements modem <-> applications processor remote caqllback registration functions


EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_remote_reg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
12/5/09    din     Added mechainsm to remote Generic Framework.
08/27/09   pc      file created

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff.h"
#include "timetick.h"

/** @addtogroup time_remote
@{ */

/* ========================================================================= */
/* ========================== Client group data types ====================== */
/* ========================================================================= */

/** Configuration event types. 
*/
typedef enum
{
  ATS_CFG_EVENT_TOD_CHANGE = 0,    /**< Time of Day callback event. */
  ATS_CFG_EVENT_GENOFF_CHANGE,     /**< Genoff callback event. */
  ATS_CFG_EVENT_TIMER_IPC_CHANGE,  /**< Timer IPC callback event. */
  ATS_CFG_EVENT_MAX32 = 0x10000000 /**< End value for the enumeration. */
} ats_cfg_event_e_type;

/** End value for the enumeration. */
#define ATS_CFG_EVENT_MAX  ( ATS_CFG_EVENT_TIMER_IPC_CHANGE + 1 )  

/** @brief TOD update configuration.
*/
typedef struct ats_tod_update_s
{
  timetick_type tick; /**< Base timetick. */
  time_type stamp;    /**< Base timestamp. */
  uint32 freq;        /**< Frequency information from the timetick module. */
} ats_tod_update_s_type;

/** @brief Genoff update configuration. 
*/
typedef struct ats_genoff_update_s
{
  time_bases_type   base_type; /**< Time base to be updated. */
  int64 offset;                /**< Updated the generic offset. */
} ats_genoff_update_s_type;

/** @brief IPC timer update configuration.
*/
typedef struct ats_timer_ipc_update_s
{
  uint32 expired_handle;       /**< Expired handle. */
} ats_timer_ipc_update_s_type;


/** @brief Configuration event information for each event.
*/
typedef union ats_cfg_event_info_u
{
  ats_tod_update_s_type           ats_tod_update; 
    /**< TOD update. */
  /* ATS_CFG_EVENT_TOD_CHANGE */
  /*~ CASE ATS_CFG_EVENT_TOD_CHANGE ats_cfg_event_info_u.ats_tod_update */

  ats_genoff_update_s_type       ats_genoff_update; 
    /**< Genoff update. */
  /* ATS_CFG_EVENT_GENOFF_CHANGE */
  /*~ CASE ATS_CFG_EVENT_GENOFF_CHANGE ats_cfg_event_info_u.ats_genoff_update */

  ats_timer_ipc_update_s_type    ats_timer_ipc_update; 
    /**< Timer IPC update. */
  /* ATS_CFG_EVENT_GENOFF_CHANGE */
  /*~ CASE ATS_CFG_EVENT_TIMER_IPC_CHANGE ats_cfg_event_info_u.ats_timer_ipc_update */

} ats_cfg_event_info_s_type;

/** Client types that can be chosen.
*/
typedef enum
{
  ATS_CLIENT_TYPE_GEN1 = 0,   /**< Symbian application update. */
  ATS_CLIENT_FLOATING1,       /**< Client type 1. */
  ATS_CLIENT_FLOATING2,       /**< Client type 2. */
  /** @cond 
  */
  ATS_CLIENT_TYPE_INTERNAL,   /* For internal use only! */
  /** @endcond */
  ATS_CLIENT_TYPE_GENOFF_UPDATE,      /**< Client accepts genoff updates. */
  ATS_CLIENT_TYPE_MM_GENOFF_UPDATE,   /**< MM client acceptss genoff updates. */
  ATS_CLIENT_TYPE_MAX32 = 0x10000000  /**< Padded to 32 bits. */
} ats_client_type_e_type;

/** Rename of ats_client_type_e_type for MM systems.
*/
typedef ats_client_type_e_type ats_mm_client_type_e_type;

/** Buffer value for the enumeration. */
#define ATS_CLIENT_TYPE_MAX (ATS_CLIENT_TYPE_MM_GENOFF_UPDATE+1)

/** Duplicate value for MM clients. */
#define ATS_MM_CLIENT_TYPE_MAX ATS_CLIENT_TYPE_MAX 

/** Types of processors that can be selected.
*/
typedef enum
{
  ATS_CLIENT_PROCESSOR_NONE   = 0, /**< Invalid value for the enumeration. */
  ATS_CLIENT_PROCESSOR_MODEM,      /**< Modem processor. */
  ATS_CLIENT_PROCESSOR_APP1,       /**< Type 1 applications processor. */
  ATS_CLIENT_PROCESSOR_APP2,       /**< Type 2 applications processor. */
  ATS_CLIENT_PROCESSOR_MM_MODEM,   /**< MM modem processor. */
  ATS_CLIENT_PROCESSOR_MAX32       /**< Buffer value for the enumeration. */
} ats_client_processor_e_type;
/*~ SENTINEL ats_client_processor_e_type.ATS_CLIENT_PROCESSOR_MAX32*/ 

/** Buffer value for the enumeration. */
#define ATS_CLIENT_PROCESSOR_MAX ( ATS_CLIENT_PROCESSOR_MM_MODEM + 1 )

/** Client ID. 
*/
typedef uint8 ats_client_id_type;

/** Defined client request errors.
*/
typedef enum
{
  ATS_CLIENT_ERR_NONE,              /**< Invalid value for the enumeration. */
  ATS_CLIENT_ERR_CLIENT_ID_PTR,     /**< Invalid client ID pointer. */
  ATS_CLIENT_ERR_CLIENT_TYPE,       /**< Invalid client type. */
  ATS_CLIENT_ERR_CLIENT_ID,         /**< Invalid client ID. */
  ATS_CLIENT_ERR_TASK_NOT_READY,    /**< ATS task is not ready for clients. */
  ATS_CLIENT_ERR_INVALID_PROCESSOR, /**< Invalid processor ID. */
  ATS_CLIENT_ERR_UNSUPPORTED,       /**< Unsupported request. */
  ATS_CLIENT_ERR_GENERAL,           /**< Any general error. */
  ATS_CLIENT_ERR_RPC,               /**< Any ONCRPC error. */  
  ATS_CLIENT_ERR_ALREADY_REG,       /**< Client already registered. */ 
  ATS_CLIENT_ERR_MAX32 = 0x10000000 /**< Padded to 32-bit integer. */
} ats_client_err_e_type;

/** Buffer value for the enumeration. */
#define ATS_CLIENT_ERR_MAX (ATS_CLIENT_ERR_ALREADY_REG + 1)


/** Configuration event callback. 
*/
typedef ats_client_err_e_type (*ats_cfg_event_cb_type)
(
  ats_cfg_event_e_type         event,    /**< Type of event. */
  ats_cfg_event_info_s_type    *info_ptr /**< Additional information about the event. */
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK ats_cfg_event_cb_type 
    ONERROR return ATS_CLIENT_ERR_RPC */

/** Configuration event callback.
*/
typedef ats_client_err_e_type (*ats_mm_cfg_event_cb_type)
(
  ats_cfg_event_e_type         event,    /**< Type of event. */
  ats_cfg_event_info_s_type    *info_ptr /**< Additional information about the event. */
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK ats_mm_cfg_event_cb_type 
    ONERROR return ATS_CLIENT_ERR_RPC */

/** @brief Describes the client (remote processor) with which communication is 
taking place.
*/
typedef struct
{
  boolean                      initialized;  /**< TRUE if the connection has 
                                                  been initialized. */
  boolean                      activated;    /**< If TRUE, activated and 
                                                  callback can be called. */
  boolean                      force_genoff_update; /**< if TRUE, send over genoff
                                                         bases that have their valid flag
                                                         set to FALSE. */
  ats_cfg_event_cb_type        cfg_event_cb; /**< Callback function. */
  ats_client_processor_e_type  processor;    /**< Processor type of the client. */
} ats_client_s_type;

/** @brief Describes the modem-to-modem fusion client (remote processor) with 
which communication is taking place. 
*/
typedef struct
{
  boolean                      initialized;  /**< TRUE if the connection has 
                                                  been initialized. */
  boolean                      activated;    /**< If TRUE, activated and 
                                                  callback can be called. */
  ats_mm_cfg_event_cb_type     cfg_event_cb; /**< Callback function. */
  ats_client_processor_e_type  processor;    /**< Processor type of the client. */
} ats_mm_client_s_type;

/** @} */ /* end_addtogroup time_remote */

#endif /* TIME_REMOTE_REG_H */


