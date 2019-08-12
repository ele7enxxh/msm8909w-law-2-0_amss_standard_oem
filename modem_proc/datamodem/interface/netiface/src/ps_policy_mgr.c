
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       P S _ P O L I C Y _ M G R . C

DESCRIPTION
  File defining the API exported by policy manager module

EXTERNAL FUNCTIONS
  PS_POLICY_MGR_INIT()
    Initializes policy mgr module

  PS_POLICY_MGR_GET_PRIORITY()
    Returns priority of a given app

  PS_POLICY_MGR_ADD_RECORD()
    Adds an entry to the policy mgr database

  PS_POLICY_MGR_CLEAN_DB()
    Cleans up the policy mgr database

  PS_POLICY_MGR_REG_GET_PROFILE_CBACK()
    Registers DS function for retrieving profile based on app_identifier

  PS_POLICY_MGR_GET_PROFILE()
    Calls registered DS function to retireve profile of an appilcation

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_policy_mgr.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/08/10    rp     RAM optimization changes.
12/29/09    am     Return Legacy priority for application profiles not
                   present in policy DB rather than Invalid priority.
11/18/08    am     Fixed off-target VS compiler warnings and high/med lints.
06/06/08    am     Updated get_priority() and add_record().
05/29/08    am     Policy manager extensions. Changed default profile
                   behavior.
05/26/08    am     Included get_profile() which calls registered callback
                   from DS.
05/14/08    am     Changed database type from array to list.
04/29/08    am     Updated Arbitration/Policy Manager now supports OMH
                   phase2.
09/27/07    msr    Created the file

===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "msg.h"

#include "ps_policy_mgr.h"
#include "ps_policyi_mgr.h"
#include "ps_crit_sect.h"
#include "ps_mem.h"

#include "list.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                        LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Tuning the number of ps policy buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_POLICY_BUF_SIZE ((sizeof(ps_policy_mgr_policy_record_type) + 3) & ~3)

#define PS_POLICY_BUF_NUM         PS_POLICY_MGR_MAX_POLICY_ENTRIES
#define PS_POLICY_BUF_HIGH_WM     (PS_POLICY_MGR_MAX_POLICY_ENTRIES - 2)
#define PS_POLICY_BUF_LOW_WM      2

/*---------------------------------------------------------------------------
   POLICY INFO DATABASE RECORD

   link           : Link to the next record.
   app_identifier : Unique identifier of an application.
   profile_id     : Profile identifier.
   priority       : Priority associated with this Profile/Application.
---------------------------------------------------------------------------*/
typedef struct
{
  list_link_type            link;
  uint64                    app_identifier;
  int32                     profile_id;
  int32                     priority;
  uint32                    position;
} ps_policy_mgr_policy_record_type;

/*---------------------------------------------------------------------------
  Callback registered by DS for retrieving profile based on application
  identifier.
---------------------------------------------------------------------------*/
static get_app_profile_cb ps_policy_mgr_get_profile_cback_f_ptr = NULL;

/* Position of record in list [1,PS_POLICY_MGR_MAX_POLICY_ENTRIES] */
static uint32 record_pos;

/*---------------------------------------------------------------------------
  Allocate memory to hold ps_policy along with ps_mem header
---------------------------------------------------------------------------*/
static int ps_policy_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                             (
                              PS_POLICY_BUF_NUM,
                              PS_POLICY_BUF_SIZE
                             )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_policy_buf
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ps_policy_buf_hdr[PS_POLICY_BUF_NUM];
static ps_policy_mgr_policy_record_type * ps_policy_buf_ptr[PS_POLICY_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*--------------------------------------------------------------------------
  Indicates whether Policy database has been initialized
--------------------------------------------------------------------------*/
static uint8 is_policy_mgr_db_inited = FALSE;

/*---------------------------------------------------------------------------
   POLICY INFO DATABASE

   List of policy records.
---------------------------------------------------------------------------*/
static list_type ps_policy_mgr_record_list;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_POLICY_MGR_INIT()

DESCRIPTION
  This function is called at power up to initialize the
  policy information database.

  NOT IMPLEMENTED IN PHASE-I

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_policy_mgr_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize Policy Manager mempool.
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_POLICY_TYPE,
                           ps_policy_buf_mem,
                           PS_POLICY_BUF_SIZE,
                           PS_POLICY_BUF_NUM,
                           PS_POLICY_BUF_HIGH_WM,
                           PS_POLICY_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_policy_buf_hdr,
                           (int *) ps_policy_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                           ) == -1)
  {
    LOG_MSG_ERROR_1("ps_policy_mgr_init(): "
                    "Can't init Policy module, pool id %d",
                    (int32)PS_MEM_PS_POLICY_TYPE);
  }

  /*-------------------------------------------------------------------------
    Initialize Policy Manager database.
  -------------------------------------------------------------------------*/
  list_init(&ps_policy_mgr_record_list);

} /* ps_policy_mgr_init() */


/*===========================================================================
FUNCTION PS_POLICY_MGR_GET_PRIORITY()

DESCRIPTION
  Fetches max priority given an app_identifier/profile_id.

PARAMETERS
  app_identifier : App whose priority is needed
  profile_id     : profile of the requesting App

RETURN VALUE
  priority                       : if inputs are valid
  PS_POLICY_MGR_PRIORITY_INVALID : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_policy_mgr_get_priority
(
  uint64 app_identifier,
  int32  profile_id
)
{
  ps_policy_mgr_policy_record_type  * this_policy_ptr = NULL;
  int32                               priority;
  int32                               max_prio = PS_POLICY_MGR_PRIORITY_INVALID;
  int32                               prf_id = -1;
  uint64                              app_id       =  app_identifier;
  uint64                              tmp_app_id   =  0;
  uint32                              tmp_app_id_u =  0;
  uint32                              tmp_app_id_l =  0;
  boolean                             match        =  FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  priority = PS_POLICY_MGR_PRIORITY_INVALID;

  if ( is_policy_mgr_db_inited == FALSE )
  {
    LOG_MSG_INFO2_0("ps_policy_mgr_get_priority(): "
                    "Empty DB");
    return PS_POLICY_MGR_LEGACY_PRIORITY;
  }

  if ( app_identifier == PS_POLICY_MGR_APP_DONT_CARE &&
       profile_id == PS_POLICY_MGR_PROFILE_DONT_CARE )
  {
    /* Bogus inputs */
    LOG_MSG_INFO2_0("ps_policy_mgr_get_priority(): "
                    "Bad inputs");
    return PS_POLICY_MGR_PRIORITY_INVALID;
  }

  /* very verbose, remove later */
  tmp_app_id   = app_identifier;
  tmp_app_id_l = (uint32)(tmp_app_id & 0xFFFFFFFF);
  tmp_app_id_u = (uint32)(tmp_app_id >> 32);
  LOG_MSG_INFO1_3("ps_policy_mgr_get_priority(): "
                  "get priority: req - app_id [0x%x%x], profile [0x%x]",
                  tmp_app_id_u, tmp_app_id_l, profile_id );

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  this_policy_ptr = (ps_policy_mgr_policy_record_type *)
    list_peek_front(&ps_policy_mgr_record_list);

  while (this_policy_ptr != NULL)
  {
    /* very verbose, remove later */
    tmp_app_id   = this_policy_ptr->app_identifier;
    tmp_app_id_l = (uint32)(tmp_app_id & 0xFFFFFFFF);
    tmp_app_id_u = (uint32)(tmp_app_id >> 32);
    LOG_MSG_INFO1_3("ps_policy_mgr_get_priority(): "
                    "get priority: looking - app_id [0x%x%x], profile [0x%x]",
                    tmp_app_id_u, tmp_app_id_l, this_policy_ptr->profile_id );
    LOG_MSG_INFO1_2("ps_policy_mgr_get_priority(): "
                    "get priority: looking - prio [%d] --- pos [%d]",
                    	 this_policy_ptr->priority, this_policy_ptr->position );
	
    if ( ( this_policy_ptr->profile_id == PS_POLICY_MGR_PROFILE_DONT_CARE ||
           this_policy_ptr->profile_id == profile_id ||
           this_policy_ptr->profile_id == PS_POLICY_MGR_ANY_PROFILE) &&
         ( this_policy_ptr->app_identifier == PS_POLICY_MGR_APP_DONT_CARE ||
           this_policy_ptr->app_identifier == app_identifier ||
           this_policy_ptr->app_identifier == PS_POLICY_MGR_ANY_APP ) )
    {
      priority = this_policy_ptr->priority;
      LOG_MSG_INFO1_3("ps_policy_mgr_get_priority(): "
                      "get priority: found - app_id [0x%x%x], profile [0x%lx]",
                      tmp_app_id_u,
                      tmp_app_id_l,
                      this_policy_ptr->profile_id );
      if ( priority >= max_prio )
      {
        max_prio = priority;
        prf_id   = this_policy_ptr->profile_id;
        app_id   = this_policy_ptr->app_identifier;
        LOG_MSG_INFO1_3("ps_policy_mgr_get_priority(): "
                        "get priority: found - new max prio [0x%x], app_id [0x%x%x]",
                        max_prio, tmp_app_id_u, tmp_app_id_l );
      }
      match = TRUE;
    }

    this_policy_ptr = (ps_policy_mgr_policy_record_type *)
      list_peek_next(&ps_policy_mgr_record_list, &(this_policy_ptr->link));
  }/* end while (this_policy_ptr...) */

  tmp_app_id   = app_id;
  tmp_app_id_l = (uint32)(tmp_app_id & 0xFFFFFFFF);
  tmp_app_id_u = (uint32)(tmp_app_id >> 32);

  if ( !match )
  {
    max_prio = PS_POLICY_MGR_LEGACY_PRIORITY;
    LOG_MSG_INFO1_3("ps_policy_mgr_get_priority(): "
                    "get priority: NO MATCH, app_id [0x%x%x], profile [0x%x], treat as legacy",
                    tmp_app_id_u, tmp_app_id_l, profile_id );
  }
  else 
  {
    LOG_MSG_INFO1_3("ps_policy_mgr_get_priority(): "
                    "get priority: MATCH, app_id [0x%x%x], prio [0x%x]",
                    tmp_app_id_u, tmp_app_id_l, max_prio);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_policy_mgr_get_priority(): "
                  "policy mgr lookup got prf [%d], prio [%d]",
                  prf_id, max_prio );

  return max_prio;

} /* ps_policy_mgr_get_priority() */

/*===========================================================================
FUNCTION PS_POLICY_MGR_ADD_RECORD()

DESCRIPTION
  Adds an entry in the policy database.

PARAMETERS
  app_identifier : App identifier value
  profile_id     : Profile of the App
  priority       : App profile's priority

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_policy_mgr_add_record
(
  int64   app_id,
  int32  profile_id,
  int32  priority
)
{
  ps_policy_mgr_policy_record_type  * new_policy_ptr;
  uint32                              policy_list_size;
  uint64                              tmp_app_id;
  uint32                              tmp_app_id_u;
  uint32                              tmp_app_id_l;
  uint32                              tmp_rec_pos = 0;
  uint64                              app_identifier;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((priority > PS_POLICY_MGR_PRIORITY_MAX) ||
      (priority <= PS_POLICY_MGR_PRIORITY_INVALID))
  {
    LOG_MSG_ERROR_1("ps_policy_mgr_add_record(): "
                    "Invalid priority %d", priority);
    return -1;
  }

  app_identifier = (uint64) app_id;
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  policy_list_size = list_size(&ps_policy_mgr_record_list);

  if (policy_list_size >= PS_POLICY_MGR_MAX_POLICY_ENTRIES )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0("ps_policy_mgr_add_record(): "
                    "DB is full");
    return -1;
  }

  new_policy_ptr = (ps_policy_mgr_policy_record_type *)
    ps_mem_get_buf(PS_MEM_PS_POLICY_TYPE);
  if (NULL == new_policy_ptr)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  memset(new_policy_ptr, 0, sizeof (ps_policy_mgr_policy_record_type));

  record_pos++;
  tmp_rec_pos = record_pos;
  
  new_policy_ptr->app_identifier = app_identifier;
  new_policy_ptr->profile_id     = profile_id;
  new_policy_ptr->priority       = priority;
  new_policy_ptr->position       = record_pos;
  /*-------------------------------------------------------------------------
    All new entries will be inserted at the end of the list.
  -------------------------------------------------------------------------*/
  list_push_back(&ps_policy_mgr_record_list,
                    &(new_policy_ptr->link));


  if (is_policy_mgr_db_inited == FALSE)
  {
    is_policy_mgr_db_inited = TRUE;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  tmp_app_id   = app_identifier;
  tmp_app_id_l = (uint32)(tmp_app_id & 0xFFFFFFFF);
  tmp_app_id_u = (uint32)(tmp_app_id >> 32);
  LOG_MSG_INFO2_3("ps_policy_mgr_add_record(): "
                  "add policy: app_id [0x%x%x], profile [0x%x]",
                  tmp_app_id_u, tmp_app_id_l, profile_id );
  LOG_MSG_INFO2_2("ps_policy_mgr_add_record(): "
                  "add policy: prio [%d] --- pos [%d]",
                  priority, tmp_rec_pos );

  return 0;
} /* ps_policy_mgr_add_record() */

/*===========================================================================
FUNCTION PS_POLICY_MGR_CLEAN_DB()

DESCRIPTION
  Cleans up the policy database.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_policy_mgr_clean_db
(
  void
)
{
  ps_policy_mgr_policy_record_type  * this_policy_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  this_policy_ptr = (ps_policy_mgr_policy_record_type *)
    list_peek_front(&ps_policy_mgr_record_list);

  /*-------------------------------------------------------------------------
    Delete all records
  -------------------------------------------------------------------------*/
  while ( this_policy_ptr != NULL )
  {
    list_pop_item(&ps_policy_mgr_record_list, &(this_policy_ptr->link));
    PS_MEM_FREE(this_policy_ptr);

    this_policy_ptr = (ps_policy_mgr_policy_record_type *)
      list_peek_front(&ps_policy_mgr_record_list);
  }

  record_pos = 0;
  
  if ( is_policy_mgr_db_inited == TRUE )
  {
    is_policy_mgr_db_inited = FALSE;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_policy_mgr_clean_db() */

/*===========================================================================
FUNCTION PS_POLICY_MGR_REG_GET_PROFILE_CBACK()

DESCRIPTION
  Registers callback from DS to get application profile based on
  application identifier.

PARAMETERS
  cback_f_ptr  : Ptr to DS function to be registered

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_policy_mgr_reg_get_profile_cback
(
  get_app_profile_cb cback_f_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_policy_mgr_get_profile_cback_f_ptr = cback_f_ptr;

} /* ps_policy_mgr_reg_get_profile_cback() */

/*===========================================================================
FUNCTION PS_POLICY_MGR_GET_PROFILE()

DESCRIPTION
  Calls the registered callback from DS to get application profile based on
  application identifier.

PARAMETERS
  app_identifier : App identifier value

RETURN VALUE
  (int32)profile id on SUCCESS
  PROFILE_INVALID on FAILURE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_policy_mgr_get_profile
(
 int64  app_id
)
{
  uint64 app_identifier;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  app_identifier = (uint64)app_id;

  if (NULL == ps_policy_mgr_get_profile_cback_f_ptr)
  {
    LOG_MSG_ERROR_0("ps_policy_mgr_get_profile(): "
                    "get_profile_cback not set");
    return PS_POLICY_MGR_PROFILE_INVALID;
  }

  return (*ps_policy_mgr_get_profile_cback_f_ptr)((uint32)app_identifier);

} /* ps_policy_mgr_get_profile() */

