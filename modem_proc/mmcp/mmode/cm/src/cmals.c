/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

C A L L   M A N A G E R   A L S   M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager ALS custom logic module
  (FEATURE_ALS - Alternate Line Service).

  ALS custom logic is responsible for mapping CM's generic LINE_N /
  line selection disabled/enabled indication into a set of fields in GSM LL
  messages and SIM that need to be read or written to convey the information as
  to what ALS setting they represent and vice versa.

  On MO call / SIM write, CM will request ALS custom logic to do the mapping
  of CM enums into GSM LL message / SIM fields and then send the request down
  to GSM stack / GSDI or MMGSDI.

  On MT call / SIM read, CM will request ALS custom logic to do the mapping
  of GSM LL message / SIM fields into CM ALS enums and then pass those to
  clients if needed.

  Currently ALS custom logic supports default and Orange-specific
  custom logic functions.
  All other vendor-specific ALS custom logic functions would need to be
  placed in this module as well.


EXTERNALIZED FUNCTIONS

  All functions

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2009 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited. 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmals.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/12   gm      Data synchronization changes for freefloating platform
05/10/11   sv      Make GSM Auxilary Speech value EFS configurable 
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/11/11   rm      GSDI clean-up
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
07/01/09   fj      Changes due to LTE merge.
11/21/08   am      Fixing compilation error for Running off-target tests
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
08/07/07   ic      Lint clean up
09/05/06   ic      If we're in USIM mode and failed to write USIM ALS file, 
                   switch to its corresponding GSM file (CR#100334)
08/08/06   ic      If we're in USIM mode and failed to read USIM ALS file, 
                   switch to its corresponding GSM file (CR#97222)
07/31/06   ic      Lint fixes
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
12/13/05   ic      Fixed compiler warning "..\..\services\cm\cmals.c", 
                   Warning: C2871W: static 'cmals_dummy' declared but not used
12/11/05   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
11/12/04   ic      Fixed RVCT 2.1 compiler warnings / Lint clean up
11/11/04   ic      Lint cleanup 
09/30/04   ic      Fixed ADS 1.2 compiler warnings: removed als_ptr variables
                   that were initialized but never used afterwards.
                   Fixed RVCT 2.1 compiler warnings: "#188-D: enumerated type
                   mixed with another type": used UIM_NO_SUCH_ITEM to initialize
                   filename instead of 0x00
09/27/04   ic      Added cmals_dummy() to get rid of ADS 1.2 compiler warning
                   "Warning: C3040E: no external declaration in translation
                   unit" - apparently it got lost in the merge
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/04/04   ic      Removed target.h as it's not needed by the file.
                   Added cmals_dummy() to get rid of ADS 1.2 compiler warning
                   "Warning: C3040E: no external declaration in translation unit"
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/02/04   ic      Merged in bug fixes for problems found during integration
                   tests
09/02/04   ic      Synced up to MSMSHARED_CM.02.00.19.00.07 and its unit test
                   (to help with FEATURE_ALS integration on M6250CTMQG4230)
09/01/04   ic      Changes to FEATURE_ALS.
                   Added support for FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
07/12/04   ic      Initial release

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal
        - External

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h)*/
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)

#include "cmals.h"
#include "cmmmgsdi.h"
#include "cmefs.h"

#ifdef CM_DEBUG
#error code not present
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Invalid values for selected line and line switching EF setting 
*/
#define CMALS_INVALID_LINE_SWITCHING_EF_SETTING 0xFF
#define CMALS_INVALID_SELECTED_LINE_EF_SETTING  0xFF

/*
** Bit masks used to specify what ALS related item was read from the SIM.
** There is one such mask for each ALS item we need to read from the SIM.
*/
#define CMALS_CPHS_READ_MASK           ( (uint32) (1<<0)  )
#define CMALS_CSP_READ_MASK            ( (uint32) (1<<1)  )
#define CMALS_LINE_SWITCHING_READ_MASK ( (uint32) (1<<2)  )
#define CMALS_SELECTED_LINE_READ_MASK  ( (uint32) (1<<3)  )


/*
** SIM read is completed when all ALS items are read from the SIM
*/
#define CMALS_SIM_READ_COMPLETED  ( CMALS_CPHS_READ_MASK           | \
                                    CMALS_CSP_READ_MASK            | \
                                    CMALS_LINE_SWITCHING_READ_MASK | \
                                    CMALS_SELECTED_LINE_READ_MASK )

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Static data
** --------------------------------------------------------------------------
*/

/*
** PH ALS commands we need to reply to once
** we know whether SIM write is successful
*/
static cm_ph_cmd_s_type* ph_als_line_switching_cmd;
static cm_ph_cmd_s_type* ph_als_selected_line_cmd;

/*
** PH event we need to send once we read ALS info from the SIM.
** Either CM_PH_EVENT_SUBSCRIPTION_AVAILABLE or 
** CM_PH_EVENT_SUBSCRIPTION_CHANGED and it will be send by CM only after 
** it got confirmation from GSDI or MMGSDI that ALS information is available.
*/
static cm_ph_event_e_type subscr_avail_chng_event;

/* to keep track of what all sim_items have been read*/
static uint32                     sim_items_read = 0x0;

/**--------------------------------------------------------------------------
** Functions - internal, common
** --------------------------------------------------------------------------
*/

static byte cmals_map_line_switching_cm_to_orange
( 
   cm_als_line_switching_e_type line_switching
)
{
  /*
  ** Map CM enum into format expected by Orange
  */

  CM_MSG_MED_1("cmals_map_line_switching_cm_to_orange line_switching=%d", line_switching);
  
  if( line_switching == CM_ALS_LINE_SWITCHING_ALLOWED )
  {
    return CMALS_ORANGE_SIM_EF_LINE_SWITCHING_ALLOWED;
  }
  else
  if (line_switching == CM_ALS_LINE_SWITCHING_NOT_ALLOWED )
  {
    return CMALS_ORANGE_SIM_EF_LINE_SWITCHING_NOT_ALLOWED;
  }
  else
  {
    CM_ERR_1("Bad line switching setting = %d",line_switching);
    return CMALS_INVALID_LINE_SWITCHING_EF_SETTING;
  }
}

static byte cmals_map_selected_line_cm_to_orange
( 
   cm_als_line_e_type line
)
{
  /*
  ** Map CM enum into format expected by Orange
  */
  CM_MSG_LOW_1("Write CMALS_ORANGE_SIM_EF_LINE_%d to the SIM",line);
  
  if( line == CM_ALS_LINE_01 )
  {
   
    return CMALS_ORANGE_SIM_EF_LINE_01;
  }
  else
  if (line == CM_ALS_LINE_02 )
  {
   
    return CMALS_ORANGE_SIM_EF_LINE_02;
  }
  else
  {
    CM_ERR_1("Bad line = %d",line);
    return CMALS_INVALID_SELECTED_LINE_EF_SETTING;
  }
}


/*===========================================================================
FUNCTION cmals_custom_reg_mmgsdi

DESCRIPTION

  Register or de-register with CM ALS custom logic functions

  If mmgsdi_read_*_func arguments are not NULL and GW subscription is
  available, call mmgsdi_read_*_func functions to read ALS settings from
  the SIM.

  To de-register, provide NULL pointer as argument

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL boolean                  cmals_custom_reg_mmgsdi(

  cm_als_resolve_mo_call_request_f_type     *resolve_mo_call_req_func,
  /*
  ** pointer to a function that maps currently selected line
  ** to set of fields in GSM LL MO call request
  */

  cm_als_resolve_mt_call_ind_f_type         *resolve_mt_call_ind_func,
  /*
  ** pointer to a function that maps set of fields in GSM LL MT call
  ** indication to a line on which the call is to be received
  */

  cm_als_mmgsdi_read_line_switching_f_type  *mmgsdi_read_line_switching_func,
  /*
  ** pointer to a function that knows how to read line switching
  ** setting from the SIM
  */

  cm_als_mmgsdi_read_selected_line_f_type   *mmgsdi_read_selected_line_func,
  /*
  ** pointer to a function that knows how to read selected line
  ** from the SIM
  */

  cm_als_mmgsdi_write_line_switching_f_type *mmgsdi_write_line_switching_func,
  /*
  ** pointer to a function that knows how to write line switching
  ** setting to the SIM
  */

  cm_als_mmgsdi_write_selected_line_f_type  *mmgsdi_write_selected_line_func
  /*
  ** pointer to a function that knows how to write selected line
  ** to the SIM
  */
)
{

  cm_als_custom_logic_mmgsdi_s_type    *als_ptr = cmals_ptr_mmgsdi();
    /* point at ALS custom logic object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( als_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Set ALS custom logic functions
  ** Use defaults if none provided.
  */

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Resolve MO call with respect to line it should use
  */
  als_ptr->resolve_mo_call_req_func = resolve_mo_call_req_func;

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Resolve MT call with respect to line it should use
  */
  als_ptr->resolve_mt_call_ind_func = resolve_mt_call_ind_func;

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/

  /*
  ** Read line switching allowed/forbidden from the SIM
  */
  als_ptr->mmgsdi_read_line_switching_func = mmgsdi_read_line_switching_func;

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Read selected line from the SIM
  */
  /*Moving selected line reading invocation after response from
   ** MMGSDI for line switching
      */
  als_ptr->mmgsdi_read_selected_line_func = mmgsdi_read_selected_line_func;

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Write line switching allowed/forbidden to the SIM
  */
  als_ptr->mmgsdi_write_line_switching_func = mmgsdi_write_line_switching_func;

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Write selected line to the SIM
  */
  als_ptr->mmgsdi_write_selected_line_func = mmgsdi_write_selected_line_func;

  /* - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (mmgsdi_read_line_switching_func != NULL)
  {
    if (cmph_ptr()->is_gwl_subscription_available)
    {
      /*
      ** GW subscription is available so read line switching setting
      ** from the SIM so CM can update its ALS info and notify clients
      */
      als_ptr->mmgsdi_read_line_switching_func();
    }
  }

  return TRUE;

}  /* cmals_custom_reg_mmgsdi() */


/*==========================================================================

FUNCTION cmals_mmgsdi_read_cphs()

DESCRIPTION
  This fucnction is used to read the Common PCN Handset Specifications File
  from the SIM/USIM (EF-CPHS).
  If not found, or not supported, then CPHS is not supported which indicates
  that ALS is not supported.

  The event passed to function is either CM_PH_EVENT_SUBSCRIPTION_AVAILABLE
  or CM_PH_EVENT_SUBSCRIPTION_CHANGED and it will be send by CM only after
  it got confirmation from MMGSDI that ALS information is available.

  If found and successfully read, CPHS is supported, and the Customer
  Service Profile (EF-CSP) must be seperately checked to see if ALS is
  supported.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : If cmmmgsdi_cmals_mmgsdi_async_sim_read() retruns TRUE
  FALSE : If cmmmgsdi_cmals_mmgsdi_async_sim_read() retruns FALSE

SIDE EFFECTS
  None

==========================================================================*/
EXTERNAL boolean              cmals_mmgsdi_read_cphs(

   cm_ss_e_type         ss,
	  /* current ss */
    cm_ph_event_e_type   event
      /* Event to send after all ALS SIM items have been read */

)
{
  mmgsdi_file_enum_type               filename = MMGSDI_NO_FILE_ENUM;
  boolean                             status = FALSE;

  mmgsdi_session_type_enum_type   session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_app_enum_type            app_type;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

  /*
  ** Save the event that CM will need to send
  ** once all ALS related items are read from the SIM
  */
  subscr_avail_chng_event = event;

  if(ss == CM_SS_HYBR_2)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }

  /* Determine the Card Applications Type */
  /*lint -e{746} -e{718}*/
  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type);

  if (app_type == MMGSDI_APP_USIM)
  {
    filename = MMGSDI_USIM_CPHSI;
  }
  else
  {
    filename = MMGSDI_GSM_CPHSI;
  }

  /*lint -e{611} */
  if(cmmmgsdi_ptr()->sim_busy_status.is_sim_busy == TRUE)
  {
    /* if sim is BUSY due to APP installation ,we will return error (FR 15489) */
    status = FALSE;
  }
  else
  {
  status = cmmmgsdi_cmals_mmgsdi_async_sim_read(
                            filename,
                            0,
                            MMGSDI_MAX_DATA_BLOCK_LEN,
                            transparent,
                            (void *)cmals_orange_mmgsdi_read_completed_cb,
                            (uint32)filename
                            );
  }
  
  return status;
} /* cmals_mmgsdi_read_cphs() */


/*==========================================================================

FUNCTION cmals_mmgsdi_read_csp()

DESCRIPTION

  This function is used to retrieve the Customer Service Profile Information
  from the SIM/USIM (EF-CSP).
  This file will indicate whether or not ALS is supported as defined in the
  CPHS Specifications.

  EF CSP
  BYTE6 (Data offset 5), Bit 8 or 0x40.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : If cmmmgsdi_cmals_mmgsdi_async_sim_read() returns TRUE
  FALSE : If cmmmgsdi_cmals_mmgsdi_async_sim_read() returns FALSE

SIDE EFFECTS
  None

==========================================================================*/
EXTERNAL boolean cmals_mmgsdi_read_csp(
    void
)
{
  mmgsdi_file_enum_type           filename = MMGSDI_NO_FILE_ENUM;
  boolean                         status = FALSE;
  mmgsdi_app_enum_type            app_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine the Card Applications Type */
  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(
                  MMGSDI_GW_PROV_PRI_SESSION);

  if (app_type == MMGSDI_APP_USIM)
  {
    filename = MMGSDI_USIM_CSP;
  }
  else
  {
    filename = MMGSDI_GSM_CSP;
  }

  /*lint -e{611} */
  status = cmmmgsdi_cmals_mmgsdi_async_sim_read(
                        filename,
                        0,
                        MMGSDI_MAX_DATA_BLOCK_LEN,
                        transparent,
                        (void *)cmals_orange_mmgsdi_read_completed_cb,
                        (uint32)filename
                        );

  return status;

} /* cmals_mmgsdi_read_csp() */


/*==========================================================================

FUNCTION cmals_init

DESCRIPTION

  Initializing the ALS object.

  This function must be called before the ALS object
  is being used, in any way, place, or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
EXTERNAL void cmals_init(
    void
)
{
  
  cm_als_custom_logic_mmgsdi_s_type *als_ptr  =  cmals_ptr_mmgsdi();
   

  CM_ASSERT( als_ptr != NULL );
  

  /*
  ** Verify object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_ASSERT( !CM_INIT_CHECK(als_ptr->init_mask) );
  CM_INIT_MARK( als_ptr->init_mask );

  cmals_reset();

} /* cmals_init() */



/*==========================================================================

FUNCTION cmals_orange_mmgsdi_read_completed_cb

DESCRIPTION

  Callback function to be called by MMGSDI when ALS item has been read from
  the SIM by cmmmgsdi_als_mmgsdi_async_sim_read()

  This function:

  - maps SIM read results to CM ALS enums

  - keeps track of whether all ALS-related items have been read from the SIM

  - once all ALS-related items have been read, queue 
    CM_PH_CMD_ALS_SIM_INFO_AVAIL to CM so it can update its settings

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
/*lint -esym(715,cnf) */
/*lint -esym(715,status) */
EXTERNAL void     cmals_orange_mmgsdi_read_completed_cb(

    mmgsdi_return_enum_type        status,
    mmgsdi_cnf_enum_type           cnf,
    const mmgsdi_cnf_type          *cnf_ptr

)
{
  cm_ph_cmd_s_type                  *ph_cmd_ptr;
  cm_ph_cmd_info_s_type             *cmd_info_ptr;
  static cm_cphs_proc_e_type        cphs_allowed = CM_CPHS_PROC_NOT_ALLOWED;
  static cm_als_proc_e_type         als_allowed = CM_ALS_PROC_NOT_ALLOWED;
  static cm_als_line_switching_e_type line_switching = CM_ALS_LINE_SWITCHING_NONE;
  static cm_als_line_e_type         selected_line = CM_ALS_LINE_NONE;
  mmgsdi_file_enum_type             sim_filename = MMGSDI_NO_FILE_ENUM; 
  boolean                           is_als_valid = FALSE;
  cmmmgsdi_s_type  *mmgsdi_ptr = cmmmgsdi_ptr();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  /* Verify the response is the one we are looking for
  */
  /*while posting a read cmd to MMGSDI  ,
  **we will start a timer of 5secs and set cmph_ptr()->read_als_uptime.uptime
  ** Hence we expect read_als_uptime is non zero when we recieve the response 
  ** Also we set client_data to the filename being read 
  ** Hence we expect the same to be returned */    
  if (cnf != MMGSDI_READ_CNF ||
        mmgsdi_ptr->read_als_uptime.uptime == 0 ||
        cnf_ptr->response_header.client_data != mmgsdi_ptr->read_als_uptime.client_data)
  {
    CM_MSG_HIGH_3("Incorrect Response Rcvd: %d %d %d",cnf,
                 cnf_ptr->response_header.client_data,
                 mmgsdi_ptr->read_als_uptime.client_data);
    return;
  }

  /*while posting a read cmd to MMGSDI  ,
  **we will start a timer of 5secs and set cmph_ptr()->read_als_uptime.uptime
  ** Hence we expect read_als_uptime is non zero when we recieve the response */
  
  if (mmgsdi_ptr->read_als_uptime.uptime)
  {
  /*
  ** See what we read
  */
  sim_filename = cnf_ptr->read_cnf.access.file.file_enum;
  
  mmgsdi_ptr->read_als_uptime.uptime = 0;

  CM_MSG_LOW_1("Read CSP sim_filename=%d", sim_filename);

  switch (sim_filename)
  {

    /*
    ** Selected line is stored in MMGSDI_SIM_7F40_PROP1_DFS
    */
    case MMGSDI_SIM_7F40_PROP1_DFS:
    case MMGSDI_USIM_7F40_PROP1_DFS:
      sim_items_read |= CMALS_SELECTED_LINE_READ_MASK;

      if (cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {

        /*
        ** See if we read enough data and whether it's meaningful
        */
        if (cnf_ptr->read_cnf.read_data.data_len <=
            CM_ALS_ORANGE_MAX_DYNAMIC_FLAGS_BUFFER_LEN)
        {

          /*
          ** See what line the data read from the SIM corresponds to
          */
          if (*(cnf_ptr->read_cnf.read_data.data_ptr + 0) ==
                  CMALS_ORANGE_SIM_EF_LINE_01)
          {
            selected_line = CM_ALS_LINE_01;
          }
          else if (*(cnf_ptr->read_cnf.read_data.data_ptr + 0) ==
                     CMALS_ORANGE_SIM_EF_LINE_02)
          {
            selected_line = CM_ALS_LINE_02;
          }
		  CM_MSG_LOW_1("Selected line CMALS_ORANGE_SIM_EF_LINE = %d",
                  selected_line);
          break;
        }

        CM_ERR_1("Bad selected line read from the SIM = %x",
            *(cnf_ptr->read_cnf.read_data.data_ptr + 0));
      }
      else /* !MMGSDI_SUCCESS */
      {

        /* CR97222 - if we're in USIM mode, try to read
        ** MMGSDI_USIM_7F40_PROP1_DFS and it fails, then try to read
        ** MMGSDI_SIM_7F40_PROP1_DFS.
        */
        if(sim_filename == MMGSDI_USIM_7F40_PROP1_DFS)
        {

          /* Reset flag that indicates we read selected line 
          */
          sim_items_read &= ~(CMALS_SELECTED_LINE_READ_MASK);
          CM_MSG_HIGH_0("MMGSDI_USIM_7F40_PROP1_DFS read failed, try MMGSDI_SIM_7F40_PROP1_DFS");

          /*lint -e{611} */
          if(!cmmmgsdi_cmals_mmgsdi_async_sim_read(
                           MMGSDI_SIM_7F40_PROP1_DFS,
                           0,
                           MMGSDI_MAX_DATA_BLOCK_LEN,
                           transparent,
                           (void *)cmals_orange_mmgsdi_read_completed_cb,
                           (uint32)MMGSDI_SIM_7F40_PROP1_DFS
                           ))
            {
              CM_ERR_0("Failed to queue cmd to MMGSDI!!!");
              sim_items_read |= CMALS_LINE_SWITCHING_READ_MASK;
            }
        }
        else if(sim_filename == MMGSDI_SIM_7F40_PROP1_DFS)
        {
          if(cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_NOT_FOUND)
          {
            #ifdef FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
            CM_MSG_HIGH_0("MMGSDI_SIM_7F40_PROP1_DFS not found, using defaults ...");
            selected_line = CM_ALS_DEFAULT_LINE;
            #else /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
            CM_ERR_0("MMGSDI_SIM_7F40_PROP1_DFS/MMGSDI_USIM_7F40_PROP1_DFS not found!");
            #endif /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
          }
          else /* !MMGSDI_NOT_FOUND */
          {
            CM_MSG_HIGH_1("MMGSDI_SIM_7F40_PROP1_DFS MMGSDI read error = %d",
                          cnf_ptr->read_cnf.response_header.mmgsdi_status);
          }
        }
      } /* !MMGSDI_SUCCESS */
      break;

    /*
    ** Line switching is stored in MMGSDI_SIM_7F40_PROP1_D2FS
    */
    case MMGSDI_SIM_7F40_PROP1_D2FS:
    case MMGSDI_USIM_7F40_PROP1_D2FS:
      sim_items_read |= CMALS_LINE_SWITCHING_READ_MASK;

      if (cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {

        /*
        ** See if we read enough data and whether it's meaningful
        */
        if (cnf_ptr->read_cnf.read_data.data_len <=
             CM_ALS_ORANGE_MAX_DYNAMIC_FLAGS_BUFFER_LEN)
        {

          /*
          ** See what setting the data read from the SIM corresponds to
          */
          if( *(cnf_ptr->read_cnf.read_data.data_ptr + 0) ==
                  CMALS_ORANGE_SIM_EF_LINE_SWITCHING_ALLOWED)
          {
            CM_MSG_LOW_1("Line switching CMALS_ORANGE_SIM_EF_LINE_SWITCHING_ENABLED = %x",
                CMALS_ORANGE_SIM_EF_LINE_SWITCHING_ALLOWED);
            line_switching = CM_ALS_LINE_SWITCHING_ALLOWED;
          }
          else if(*(cnf_ptr->read_cnf.read_data.data_ptr+ 0) ==
                      CMALS_ORANGE_SIM_EF_LINE_SWITCHING_NOT_ALLOWED)
          {
            CM_MSG_LOW_1("Line switching CMALS_ORANGE_SIM_EF_LINE_SWITCHING_DISABLED = %x",
                CMALS_ORANGE_SIM_EF_LINE_SWITCHING_NOT_ALLOWED);
            line_switching = CM_ALS_LINE_SWITCHING_NOT_ALLOWED;
          }
          if((sim_items_read & CMALS_LINE_SWITCHING_READ_MASK)
            &&cmph_ptr()->is_gwl_subscription_available
            && (cmals_ptr_mmgsdi()->mmgsdi_read_selected_line_func != NULL)
            )
          {
            cmals_ptr_mmgsdi()->mmgsdi_read_selected_line_func();
          }
          break;
        }
        CM_ERR_1("Bad line switching read from the SIM = %x",
            *(cnf_ptr->read_cnf.read_data.data_ptr + 0));
      }
      else /* !MMGSDI_SUCCESS */
      {

        /* As per CR97222, if we're in USIM mode, try to read
        ** MMGSDI_USIM_7F40_PROP1_D2FS and it fails, then try to read
        ** MMGSDI_SIM_7F40_PROP1_D2FS
        */
        if(sim_filename == MMGSDI_USIM_7F40_PROP1_D2FS)
        {

          /* Reset flag that indicates we read line switching 
          */
          sim_items_read &= ~(CMALS_LINE_SWITCHING_READ_MASK);
          CM_MSG_HIGH_0("MMGSDI_USIM_7F40_PROP1_D2FS read failed, try MMGSDI_SIM_7F40_PROP1_D2FS");
           /*lint -e{611} */
          if(!cmmmgsdi_cmals_mmgsdi_async_sim_read(
                             MMGSDI_SIM_7F40_PROP1_D2FS,
                             0,
                             MMGSDI_MAX_DATA_BLOCK_LEN,
                             transparent,
                             (void *)cmals_orange_mmgsdi_read_completed_cb,
                             (uint32)MMGSDI_SIM_7F40_PROP1_D2FS
                             ))
          {
            CM_ERR_0("Failed to queue cmd to MMGSDI!!!");
            sim_items_read |= CMALS_LINE_SWITCHING_READ_MASK;
          }
        } /* sim_filename == MMGSDI_USIM_7F40_PROP1_D2FS */
        else if(sim_filename == MMGSDI_SIM_7F40_PROP1_D2FS)
        {
          if(cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_NOT_FOUND)
          {
            #ifdef FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
            CM_MSG_HIGH_0("MMGSDI_SIM_7F40_PROP1_D2FS not found, using defaults ...");
            line_switching = CM_ALS_DEFAULT_LINE_SWITCHING;
            #else /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
            CM_ERR_0("MMGSDI_SIM_7F40_PROP1_D2FS not found!");
            #endif /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
          }
          else /* !MMGSDI_NOT_FOUND */
          {
            CM_MSG_HIGH_1("MMGSDI_SIM_7F40_PROP1_D2FS MMGSDI read error = %d",
                          cnf_ptr->read_cnf.response_header.mmgsdi_status);
          } /* !MMGSDI_NOT_FOUND */
        } /* sim_filename == MMGSDI_SIM_7F40_PROP1_D2FS */

        if((sim_items_read & CMALS_LINE_SWITCHING_READ_MASK)
          &&cmph_ptr()->is_gwl_subscription_available
          && (cmals_ptr_mmgsdi()->mmgsdi_read_selected_line_func != NULL)
          )
        {
          cmals_ptr_mmgsdi()->mmgsdi_read_selected_line_func();
        }
      } /* ! MMGSDI_SUCCESS */
      break;

    case MMGSDI_GSM_CPHSI:
    case MMGSDI_USIM_CPHSI:
      sim_items_read |= CMALS_CPHS_READ_MASK;

      if (cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {

        /*
        ** Save the setting
        */
        cphs_allowed = CM_CPHS_PROC_ALLOWED;

        /*
        ** The SIM or USIM provided allows for CPHS procedures.
        ** Customer Service Profile (EF-CSP) must now be seperately
        ** checked to see if ALS is supported
        */
        CM_MSG_LOW_0("ALS - CPHS procedures allowed, reading CSP ...");
        if(!cmals_mmgsdi_read_csp())
        {
          CM_ERR_0("ALS - failed to queue read CSP cmd to MMGSDI!");
          sim_items_read = CMALS_SIM_READ_COMPLETED;
        }
      }
      else if(sim_filename == MMGSDI_USIM_CPHSI)
      {

        /* Reset bit that indicated we read CPHS 
        */
        sim_items_read &= ~(CMALS_CPHS_READ_MASK);

        /* CR97222 if we're in USIM mode, try to read
        ** MMGSDI_USIM_CPHSI and it fails, then try to read
        ** MMGSDI_GSM_CPHSI
        */
        CM_MSG_HIGH_0("MMGSDI_USIM_CPHSI read failed, try MMGSDI_GSM_CPHSI");

         /*lint -e{611} */
        (void)cmmmgsdi_cmals_mmgsdi_async_sim_read(
                           MMGSDI_GSM_CPHSI,
                           0,
                           MMGSDI_MAX_DATA_BLOCK_LEN,
                           transparent,
                           (void *)cmals_orange_mmgsdi_read_completed_cb,
                           (uint32)MMGSDI_GSM_CPHSI
                           );
      }
      else
      {
        CM_MSG_HIGH_1("MMGSDI_GSM_CPHSI/MMGSDI_USIM_CPHSI GSDI read error = %d",
            cnf_ptr->read_cnf.response_header.mmgsdi_status);
        sim_items_read = CMALS_SIM_READ_COMPLETED;
      }
      break;

    case MMGSDI_GSM_CSP:
    case MMGSDI_USIM_CSP:

      CM_MSG_LOW_0("Read CSP (MMGSDI_GSM_CSP/MMGSDI_USIM_CSP)");
      sim_items_read |= CMALS_CSP_READ_MASK;

      if (cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {
        /*
        ** Check the CSP File to determine whether or not the ALS Procedures
        ** are allowed
        */
        int32 data_len =0;
        int32 i;
        data_len = MIN( cnf_ptr->read_cnf.read_data.data_len, CMALS_CSP_LENGTH_MAX);
        for(i = 0; i < data_len; i=i+2)
        {
          if(*(cnf_ptr->read_cnf.read_data.data_ptr + i) == CMALS_CSP_SERVICE_CPHS_TS)
          {
            if((*(cnf_ptr->read_cnf.read_data.data_ptr + (i+1))) & CMALS_CSP_ALS_ALLOWED_MASK)
            {  
              is_als_valid = TRUE;
              /*
              ** Save the setting
              */
              als_allowed = CM_ALS_PROC_ALLOWED;

              /*
              ** Now register ALS custom logic functions
              ** This will result in line_switching / line events being sent
              ** to clients
              */
              CM_MSG_LOW_0("ALS allowed, register ALS custom logic functions ...");
              (void) cmals_custom_reg_mmgsdi( 
                              cmals_orange_resolve_mo_call_req_func,
                              cmals_orange_resolve_mt_call_ind_func,
                              cmals_orange_mmgsdi_read_line_switching_func,
                              cmals_orange_mmgsdi_read_selected_line_func,
                              cmals_orange_mmgsdi_write_line_switching_func,
                              cmals_orange_mmgsdi_write_selected_line_func
                              );
              break;
            }
            break;
          }
        }
        if(!is_als_valid)
        {
          CM_MSG_LOW_0("ALS procedures not allowed by CSP EF!");
          sim_items_read = CMALS_SIM_READ_COMPLETED;
        }
      } /* !MMGSDI_SUCCESS for reading MMGSDI_USIM_CSP / MMGSDI_GSM_CSP */
      else if(sim_filename == MMGSDI_USIM_CSP)
      {

        /* Reset mask that indicates we read CSP
        */
        sim_items_read &= ~(CMALS_CSP_READ_MASK);

        /* As per CR97222 if we're in USIM mode, try to read
        ** MMGSDI_USIM_CSP and it fails, then try to read
        ** MMGSDI_GSM_CSP
        */
        CM_MSG_HIGH_0("MMGSDI_USIM_CSP read failed, try MMGSDI_GSM_CSP");

        /*lint -e{611} */
        (void)cmmmgsdi_cmals_mmgsdi_async_sim_read(
                           MMGSDI_GSM_CSP,
                           0,
                           MMGSDI_MAX_DATA_BLOCK_LEN,
                           transparent,
                           (void *)cmals_orange_mmgsdi_read_completed_cb,
                           (uint32)MMGSDI_GSM_CSP
                           );
      }
      else /* !MMGSDI_USIM_CSP */
      {
        CM_MSG_HIGH_1("MMGSDI_GSM_CSP/MMGSDI_USIM_CSP MMGSDI read error = %d",
            cnf_ptr->read_cnf.response_header.mmgsdi_status);
        sim_items_read = CMALS_SIM_READ_COMPLETED;
      }
      break;

    default:
      CM_ERR_1("Unknown File received = %d",
          cnf_ptr->read_cnf.access.file.file_enum);
      break;
  } /* switch() */

  /*
  ** Send CM_PH_CMD_ALS_SIM_INFO_AVAIL once all ALS-related items
  ** have been read from the SIM
  */
  if (sim_items_read == CMALS_SIM_READ_COMPLETED)
  {

    ph_cmd_ptr = cm_cmd_alloc_ph_init();
    cmd_info_ptr                 = CMD_INFO_PTR( ph_cmd_ptr );
    ph_cmd_ptr->cmd_cb_func      = NULL;

    /*
    ** Instruct CM to send CM_PH_EVENT_SUBSCRIPTION_AVAILABLE or
    ** CM_PH_EVENT_SUBSCRIPTION_CHANGED event now that we read all
    ** ALS-related items from the SIM
    */
    ph_cmd_ptr->data_block_ptr   = &subscr_avail_chng_event;
    ph_cmd_ptr->client_id        = CM_CLIENT_ID_ANONYMOUS;
    ph_cmd_ptr->cmd              = CM_PH_CMD_ALS_SIM_INFO_AVAIL;

    /* Copy into the command what we read from the SIM
    */
    cmd_info_ptr->cphs_allowed   = cphs_allowed;
    cmd_info_ptr->als_allowed    = als_allowed;
    cmd_info_ptr->line_switching = line_switching;
    cmd_info_ptr->line           = selected_line;

    CM_MSG_LOW_0("Queue CM_PH_CMD_ALS_SIM_INFO_AVAIL");
    cm_cmd_queue((cm_cmd_type*)ph_cmd_ptr);

    /*
    ** Reset all static variables
    */
    sim_items_read = 0x0;
    cphs_allowed   = CM_CPHS_PROC_NOT_ALLOWED;
    als_allowed    = CM_ALS_PROC_NOT_ALLOWED;
    line_switching = CM_ALS_LINE_SWITCHING_NONE;
    selected_line  = CM_ALS_LINE_NONE;

  } /* sim_items_read == CMALS_SIM_READ_COMPLETED */
  }



} /* cmals_orange_mmgsdi_read_completed_cb() */
/*lint +esym(715,status) */
/*lint +esym(715,cnf) */


/*==========================================================================

FUNCTION cmals_orange_mmgsdi_read_line_switching_func

DESCRIPTION

  Read from the SIM all the files needed to figure out
  whether line switching is allowed/forbidden.

  Use cmmmgsdi_cmals_mmgsdi_async_sim_read() to issue all read commands one 
  by one and pass cmals_orange_mmgsdi_read_completed_cb() as a callback 
  function to CM MMGSDI that will be called by MMGSDI for each read
  completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL void cmals_orange_mmgsdi_read_line_switching_func(
    void
)
{
  mmgsdi_file_enum_type         filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_app_enum_type          app_type;

  /* - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - -  -*/

  /* First Determine Filename to use
  */

  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(
                  MMGSDI_GW_PROV_PRI_SESSION);

  if (app_type == MMGSDI_APP_USIM)
  {
    filename = MMGSDI_USIM_7F40_PROP1_D2FS;
  }
  else
  {
    filename = MMGSDI_SIM_7F40_PROP1_D2FS;
  }

  /*
  ** Lock status of line in use - i.e. line switching allowed / forbidden
  ** is stored in MMGSDI_SIM_7F40_PROP1_D2FS, so initiate MMGSDI read of 
  ** the file
  */
  /*lint -e{611} */
  if(!cmmmgsdi_cmals_mmgsdi_async_sim_read(
                           filename,
                           0,
                           MMGSDI_MAX_DATA_BLOCK_LEN,
                           transparent,
                           (void *)cmals_orange_mmgsdi_read_completed_cb,
                           (uint32)filename
                           ))
  {
    CM_MSG_HIGH_0("Failed to queue cmd to MMGSDI!!!");
    sim_items_read |= CMALS_LINE_SWITCHING_READ_MASK;
  }
  else
  {
  	CM_MSG_LOW_1(" Call to cmmmgsdi_cmals_mmgsdi_async_sim_read() successful for filename %d  ", filename);
  }
 
} /* cmals_orange_mmgsdi_read_line_switching_func() */



/*==========================================================================

FUNCTION cmals_orange_mmgsdi_read_selected_line_func

DESCRIPTION

  Read from the SIM all the files needed to figure out
  what the currently selected line is.

  Use cmmmgsdi_cmals_mmgsdi_async_sim_read() to issue all read commands one
  by one and pass cmals_orange_mmgsdi_read_completed_cb() as callback 
  function to CM from MMGSDI that will be called by MMGSDI for each read
  completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL void cmals_orange_mmgsdi_read_selected_line_func(
    void
)
{
  mmgsdi_file_enum_type          filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_app_enum_type           app_type;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

  /* First Determine Filename to use
  */
  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(
                  MMGSDI_GW_PROV_PRI_SESSION);

  if (app_type == MMGSDI_APP_USIM)
  {
    filename = MMGSDI_USIM_7F40_PROP1_DFS;
  }
  else
  {
    filename = MMGSDI_SIM_7F40_PROP1_DFS;
  }

  /*
  ** Selected line is stored in MMGSDI_SIM_7F40_PROP1_DFS
  ** so initiate MMGSDI read for the file
  */
  /*lint -e{611} */
  if(!cmmmgsdi_cmals_mmgsdi_async_sim_read(
                           filename,
                           0,
                           MMGSDI_MAX_DATA_BLOCK_LEN,
                           transparent,
                           (void *)cmals_orange_mmgsdi_read_completed_cb,
                           (uint32)filename
                           ))
  {
    CM_MSG_HIGH_0("Failed to queue cmd to MMGSDI!!!");
    sim_items_read |= CMALS_SELECTED_LINE_READ_MASK;
  }
  else
  {
  	CM_MSG_LOW_1(" Call to cmmmgsdi_cmals_mmgsdi_async_sim_read() successful for filename %d  ", filename);
  }

} /* cmals_orange_mmgsdi_read_selected_line_func() */


/*==========================================================================

FUNCTION cmals_orange_mmgsdi_write_completed_cb

DESCRIPTION

  Callback function to be called by MMGSDI when ALS item has been written to
  the SIM by cmmmgsdi_cmals_mmgsdi_async_sim_write()

  - check MMGSDI write status

  - depending on MMGSDI write status and what we wrote, call on client's
    command callbacks for CM_PH_CMD_SELECTED_LINE / CM_PH_CMD_LINE_SWITCHING
    with appropriate command status

  - if SIM write was successfull, queue CM_PH_CMD_SELECTED_LINE_SAVED /
    CM_PH_CMD_LINE_SWITCHING_SAVED to CM so it can update its settings and 
    send CM_PH_EVENT_SELECTED_LINE / CM_PH_EVENT_LINE_SWITCHING to
    commanding client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -esym(715,status)*/
/*lint -esym(715,cnf)*/
EXTERNAL void  cmals_orange_mmgsdi_write_completed_cb
(
     mmgsdi_return_enum_type   status,
     mmgsdi_cnf_enum_type      cnf,
     const mmgsdi_cnf_type     *cnf_ptr
)
{

  cm_als_line_switching_e_type  line_switching = CM_ALS_LINE_SWITCHING_NONE;
  cm_als_line_e_type            selected_line  = CM_ALS_LINE_NONE;
  cm_ph_cmd_err_e_type          cmd_err        = CM_PH_CMD_ERR_NOERR;
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(cnf_ptr != NULL); /*lint !e774 */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  /* NULL ptr check for callback 
  */

  /*
  ** Boolean within 'if' always evaluates to False
  ** Yup, but only if CM_DEBUG is on 
  */
  if (cnf_ptr == NULL) /*lint !e774 */
  {
    CM_ERR_0("MMGSD cnf_ptr is NULL");
    return;
  }

  /* Verify the response is the one we are looking for
  */
  if (cnf_ptr->write_cnf.response_header.response_type != MMGSDI_WRITE_CNF)
  {
    CM_ERR_1("Incorrect Response Rcvd: %x",
        cnf_ptr->write_cnf.response_header.response_type);
    return;
  }

  /*
  ** See what we wrote
  */
  switch (cnf_ptr->write_cnf.access.file.file_enum)
  {

    /*
    ** Selected line is stored in
    ** MMGSDI_SIM_7F40_PROP1_DFS / MMGSDI_USIM_7F40_PROP1_DFS
    */
    case MMGSDI_SIM_7F40_PROP1_DFS:
    case MMGSDI_USIM_7F40_PROP1_DFS:
      if (cnf_ptr->write_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {
        CM_MSG_LOW_0("Wrote selected line to the SIM");
      }
      else
      {

        /* As per the CR#100334 if write of MMGSDI_USIM_7F40_PROP1_DFS
        ** failed, try to write MMGSDI_SIM_7F40_PROP1_DFS
        */
        if(cnf_ptr->write_cnf.access.file.file_enum ==
               MMGSDI_USIM_7F40_PROP1_DFS)
        {
          byte selected_line_ef_setting =
               CMALS_INVALID_SELECTED_LINE_EF_SETTING;

          CM_MSG_HIGH_0("MMGSDI_USIM_7F40_PROP1_DFS write failed, switch to MMGSDI_SIM_7F40_PROP1_DFS");

          /* Get selected line setting from CM command, map it to EF
          ** try to resend it to GSDI
          */
          selected_line_ef_setting =
            cmals_map_selected_line_cm_to_orange(ph_als_selected_line_cmd->info.line);

          if(selected_line_ef_setting ==
                 CMALS_INVALID_SELECTED_LINE_EF_SETTING)
          {
            CM_ERR_1("Bad selected_line_ef_setting = %d ",
                selected_line_ef_setting);
            return;
          }

          /*
          ** Write it to the SIM
          */
          /*lint -e{611} */
          if(!cmmmgsdi_cmals_mmgsdi_async_sim_write(
                             MMGSDI_SIM_7F40_PROP1_DFS,
                             0,
                             1,
                             (uint8 *)&selected_line_ef_setting,
                             transparent,
                             (void *)cmals_orange_mmgsdi_write_completed_cb
                             ))
          {
            CM_ERR_0("Failed to queue cmd to MMGSDI!!!");
          }

          /* Return so we can reply to clients after write to
          ** MMGSDI_SIM_7F40_PROP1_DFS completes
          */
          return;
        } /* file_enum == MMGSDI_USIM_7F40_PROP1_DFS */
        else if(cnf_ptr->write_cnf.response_header.mmgsdi_status ==
                  MMGSDI_NOT_FOUND)
        {
          #ifdef FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
          CM_MSG_HIGH_0("MMGSDI_SIM_7F40_PROP1_DFS/MMGSDI_USIM_7F40_PROP1_DFS not found ...");
          #else /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
          CM_ERR_0("MMGSDI_SIM_7F40_PROP1_DFS/MMGSDI_USIM_7F40_PROP1_DFS not found!");
          cmd_err = CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S;
          #endif /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
        }
        else
        {
          CM_MSG_HIGH_1("MMGSDI_SIM_7F40_PROP1_DFS/MMGSDI_USIM_7F40_PROP1_DFS write error = %d",
             cnf_ptr->write_cnf.response_header.mmgsdi_status);
          cmd_err = CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S;
        }
      } /* file_enum = MMGSDI_USIM_7F40_PROP1_DFS */

      /* Notify the client, of the status of the command
      */
      ph_als_selected_line_cmd->cmd_cb_func(
                   ph_als_selected_line_cmd->data_block_ptr,
                   CM_PH_CMD_SELECTED_LINE,
                   cmd_err
                   );

      /*
      ** Set selected_line to what the client had requested
      */
      if(cmd_err == CM_PH_CMD_ERR_NOERR)
      {
        selected_line = ph_als_selected_line_cmd->info.line;
      }
      else
      {

        /*
        ** If command error is OTHER than NO ERROR,
        ** notify the client list of command error, as well
        */
        cmclient_list_ph_cmd_err_ntfy(cmd_err, ph_als_selected_line_cmd);
      }
      break;

    /*
    ** Line switching is stored in
    ** MMGSDI_SIM_7F40_PROP1_D2FS / MMGSDI_USIM_7F40_PROP1_D2FS
    */
    case MMGSDI_SIM_7F40_PROP1_D2FS:
    case MMGSDI_USIM_7F40_PROP1_D2FS:
      if (cnf_ptr->write_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {
        CM_MSG_LOW_0("Line switching stored in the SIM");
      }
      else /* !MMGSDI_SUCCESS */
      {
        /* As per CR#100334 if write of MMGSDI_USIM_7F40_PROP1_D2FS failed,
        ** try to write MMGSDI_SIM_7F40_PROP1_D2FS
        */
        if(cnf_ptr->write_cnf.access.file.file_enum ==
               MMGSDI_USIM_7F40_PROP1_D2FS)
        {
          byte line_switching_ef_setting =
                       CMALS_INVALID_LINE_SWITCHING_EF_SETTING;

          CM_MSG_HIGH_0("MMGSDI_USIM_7F40_PROP1_D2FS write failed, switch to MMGSDI_SIM_7F40_PROP1_D2FS");

          /* Get line switching setting from CM command, map it to EF
          ** try to resend it to MMGSDI 
          */
          line_switching_ef_setting =
            cmals_map_line_switching_cm_to_orange(ph_als_line_switching_cmd->info.line_switching);

          if(line_switching_ef_setting ==
                       CMALS_INVALID_LINE_SWITCHING_EF_SETTING)
          {
            CM_ERR_1("Bad line_switching_ef_setting = %d ",
                line_switching_ef_setting);
            return;
          }

          /*
          ** Write it to the SIM
          */
          /*lint -e{611} */
          if(!cmmmgsdi_cmals_mmgsdi_async_sim_write(
                             MMGSDI_SIM_7F40_PROP1_D2FS,
                             0,
                             1,
                             (uint8 *)&line_switching_ef_setting,
                             transparent,
                             (void *)cmals_orange_mmgsdi_write_completed_cb
            ))
          {
            CM_ERR_0("Failed to queue cmd to MMGSDI!!!");
          }

          /* Return so we can reply to clients after write to
          ** MMGSDI_SIM_7F40_PROP1_D2FS
          ** completes 
          */
          return;
        } /* file_enum == MMGSDI_USIM_7F40_PROP1_D2FS */
        else if(cnf_ptr->write_cnf.response_header.mmgsdi_status ==
                   MMGSDI_NOT_FOUND)
        {
          #ifdef FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
          CM_MSG_HIGH_0("MMGSDI_SIM_7F40_PROP1_D2FS/MMGSDI_USIM_7F40_PROP1_D2FS not found ...");
          #else /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
          CM_ERR_0("MMGSDI_SIM_7F40_PROP1_D2FS/MMGSDI_USIM_7F40_PROP1_D2FS not found!");
          cmd_err = CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S;
          #endif /* !FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT */
        }
        else
        {
          CM_MSG_HIGH_1("MMGSDI_SIM_7F40_PROP1_D2FS/MMGSDI_USIM_7F40_PROP1_D2FS write error = %d",
               status);
          cmd_err = CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S;
        }
      } /* !MMGSDI_SUCCESS */

      /* Notify the client of the status of the command
      */
      ph_als_line_switching_cmd->cmd_cb_func(
                    ph_als_line_switching_cmd->data_block_ptr,
                    CM_PH_CMD_LINE_SWITCHING,
                    cmd_err
                    );

      /*
      ** Set line_switching to what the client had requested
      */
      if(cmd_err == CM_PH_CMD_ERR_NOERR)
      {
        line_switching = ph_als_line_switching_cmd->info.line_switching;
      }
      else
      {

        /*
        ** If command error is OTHER than NO ERROR,
        ** notify the client list of command error, as well.
        */
        cmclient_list_ph_cmd_err_ntfy(cmd_err, ph_als_line_switching_cmd);
      }
      break;

    default:
      CM_ERR_1("Unknown File received = %d",
           cnf_ptr->write_cnf.access.file.file_enum);
      break;
  } /* switch() */


  /*
  ** See whether line switching info is available
  */
  if (line_switching != CM_ALS_LINE_SWITCHING_NONE)
  {
    ph_cmd_ptr = cm_cmd_alloc_ph_init();

    cmd_info_ptr                 = CMD_INFO_PTR( ph_cmd_ptr );
    ph_cmd_ptr->cmd_cb_func      = NULL;
    ph_cmd_ptr->data_block_ptr   = NULL;
    ph_cmd_ptr->client_id        = CM_CLIENT_ID_ANONYMOUS;
    ph_cmd_ptr->cmd              = CM_PH_CMD_LINE_SWITCHING_SAVED;

    /* Copy client's setting into the command now that we wrote it into 
    ** the SIM
    */
    cmd_info_ptr->line_switching = line_switching;

    CM_MSG_LOW_0("Queue CM_PH_CMD_LINE_SWITCHING_SAVED for line switching");
    cm_cmd_queue((cm_cmd_type*)ph_cmd_ptr);

  }

  /* cmals_als_info_avail_notify
  ** See whether selected line info is available
  */
  if (selected_line != CM_ALS_LINE_NONE)
  {
    ph_cmd_ptr = cm_cmd_alloc_ph_init();

    cmd_info_ptr                 = CMD_INFO_PTR( ph_cmd_ptr );
    ph_cmd_ptr->cmd_cb_func      = NULL;
    ph_cmd_ptr->data_block_ptr   = NULL;
    ph_cmd_ptr->client_id        = CM_CLIENT_ID_ANONYMOUS;
    ph_cmd_ptr->cmd              = CM_PH_CMD_SELECTED_LINE_SAVED;

    /* Copy into the command what we read from the SIM
    */
    cmd_info_ptr->line           = selected_line;

    CM_MSG_LOW_0("Queue CM_PH_CMD_LINE_SWITCHING_SAVED for selected line");
    cm_cmd_queue((cm_cmd_type*)ph_cmd_ptr);

  }

  if (ph_als_selected_line_cmd != NULL)
  {
    cm_mem_free ( ph_als_selected_line_cmd );
    ph_als_selected_line_cmd = NULL;
  }
  if (ph_als_line_switching_cmd != NULL)
  {
    cm_mem_free ( ph_als_line_switching_cmd );
    ph_als_line_switching_cmd = NULL;
  }


} /* cmals_orange_mmgsdi_write_completed_cb() */
/*lint +esym(715,status)*/
/*lint +esym(715,cnf)*/



/*==========================================================================

FUNCTION cmals_orange_mmgsdi_write_line_switching_func

DESCRIPTION

  Map given line_switching setting to SIM files that need to be written in
  order to save the setting.

  Use cmmmgsdi_cmals_mmgsdi_async_sim_write() to issue all write commands 
  one by one and pass cmals_orange_mmgsdi_write_completed_cb() as callback
  function to CM from MMGSDI that will be called by MMGSDI for each write
  completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
EXTERNAL void            cmals_orange_mmgsdi_write_line_switching_func(

    cm_als_line_switching_e_type line_switching,
      /* Setting to be written to the SIM */

    const cm_ph_cmd_s_type      *ph_cmd_ptr
      /* CM_PH_CMD_LINE_SWITCHING command */

)
{

  mmgsdi_file_enum_type  filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_app_enum_type            app_type;
  byte line_switching_ef_setting = CMALS_INVALID_LINE_SWITCHING_EF_SETTING; 


  ph_als_line_switching_cmd = (cm_ph_cmd_s_type*) cm_mem_malloc 
                                      (sizeof (cm_ph_cmd_s_type));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Copy and save PH cmd
  ** We'll need to call cmd callback function once
  ** we know whether SIM write succeeded
  */
  *ph_als_line_switching_cmd = *ph_cmd_ptr;

  /* First determine filename to use
  */
  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(
                  MMGSDI_GW_PROV_PRI_SESSION);

  if (app_type == MMGSDI_APP_USIM)
  {
    filename = MMGSDI_USIM_7F40_PROP1_D2FS;
  }
  else
  {
    filename = MMGSDI_SIM_7F40_PROP1_D2FS;
  }

  line_switching_ef_setting = cmals_map_line_switching_cm_to_orange(line_switching);

  /* Return on error 
  */ 
  if(line_switching_ef_setting == CMALS_INVALID_LINE_SWITCHING_EF_SETTING)
  {
    return; 
  }

  CM_MSG_LOW_1("call cmmmgsdi_cmals_mmgsdi_async_sim_write() for MMGSDI_SIM_7F40_PROP1_D2FS/MMGSDI_USIM_7F40_PROP1_D2FS = %d",
       filename);

  /*
  ** Write it to the SIM
  */
   /*lint -e{611} */
  if(!cmmmgsdi_cmals_mmgsdi_async_sim_write(
                 filename,
                 0,
                 1,
                 &line_switching_ef_setting,
                 transparent,
                 (void *)cmals_orange_mmgsdi_write_completed_cb))
  {
    CM_ERR_0("Failed to queue cmd to MMGSDI!!!");
  }

} /* cmals_orange_mmgsdi_write_line_switching_func() */


/*===========================================================================

FUNCTION cmals_orange_mmgsdi_write_selected_line_func

DESCRIPTION

  Map given line setting to SIM files that need to be written in
  order to save the setting.

  Use cmmmgsdi_cmals_mmgsdi_async_sim_write() to issue all write commands one by one
  and pass cmals_orange_mmgsdi_write_completed_cb() as callback function to 
  CM from MMGSDI that will be called by MMGSDI for each write completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL void                     cmals_orange_mmgsdi_write_selected_line_func
(

    cm_als_line_e_type       line,
      /* Line to be written to the SIM */

    const cm_ph_cmd_s_type  *ph_cmd_ptr
      /* CM_PH_CMD_SELECTED_LINE command */

)
{
  mmgsdi_file_enum_type  filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_app_enum_type            app_type;
  byte             line_ef_setting = CMALS_INVALID_SELECTED_LINE_EF_SETTING;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*
  ** Copy and save PH cmd
  ** We'll need to call cmd callback function once
  ** we know whether SIM write succeeded
  */

  ph_als_selected_line_cmd = (cm_ph_cmd_s_type*) cm_mem_malloc 
                                      (sizeof (cm_ph_cmd_s_type));

  *ph_als_selected_line_cmd = *ph_cmd_ptr;

  /* First Determine Filename to use
  */
  app_type = (mmgsdi_app_enum_type)
                cm_mmgsdi_ssn_tbl_get_app_type_for_type(
                  MMGSDI_GW_PROV_PRI_SESSION);

  if (app_type == MMGSDI_APP_USIM)
  {
    filename = MMGSDI_USIM_7F40_PROP1_DFS;
  }
  else
  {
    filename = MMGSDI_SIM_7F40_PROP1_DFS;
  }

  line_ef_setting = cmals_map_selected_line_cm_to_orange(line);

  /* Return on error
  */
  if(line_ef_setting == CMALS_INVALID_SELECTED_LINE_EF_SETTING)
  {
    return;
  }

  CM_MSG_LOW_0("call cmmgsdi_cmals_mmgsdi_async_sim_write() for MMGSDI_USIM_7F40_PROP1_DFS/MMGSDI_SIM_7F40_PROP1_DFS");

  /*
  ** Write it to the SIM
  */
  /*lint -e{611} Suspicious cast, function expects 6th arg void pointer */
  if(!cmmmgsdi_cmals_mmgsdi_async_sim_write(
         filename,
         0,
         1,
         &line_ef_setting,
         transparent,
         (void *)cmals_orange_mmgsdi_write_completed_cb))
  {
    CM_ERR_0("Failed to queue cmd to GSDI!!!");
  }

} /* cmals_orange_mmgsdi_write_selected_line_func() */


/*===========================================================================

FUNCTION cmals_orange_resolve_mo_call_req_func

DESCRIPTION

  By default, all MO calls are to go on currently selected line.
  This function should be called with currently selected line passed in as
  parameter.

  Orange-specific:

  The definition of the Information Transfer Capability (octet 3, bits 3..1)
  of the Bearer Capability Information Element - BCIE (as defined in GSM 04.08)
  shall be extended to set the following bit to denote LINE_01 / LINE_02

  Bits: 3 2 1 (1 is least significant bit)

        0 0 0 - Speech           (LINE_01)
        1 0 0 - Auxiliary speech (LINE_02)

  Set fields in mo_call_req_ptr as per the above so they correspond to
  given line parameter.


DEPENDENCIES
  None

RETURN VALUE
  TRUE if fields in mo_call_req_ptr were set, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL boolean                   cmals_orange_resolve_mo_call_req_func(

         cm_als_line_e_type        line,
           /*
           ** Line on which the call is to be originated
           ** By default, currently selected line for MO calls
           */

         mn_cnm_cmd_type           *mo_call_req_ptr
           /*
           ** Pointer to MO voice call request to be passed to
           ** GSM call processing layer.
           ** ALS custom is to set fields in the call request
           ** so that they correspond to given line.
           */
)
{

  byte *itc_ptr = NULL;
    /* pointer to information transfer capability field */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - */


  if(mo_call_req_ptr == NULL)
  {
    CM_MSG_MED_0("NULL mo_call_req_ptr!");
    return FALSE;
  }

  if(mo_call_req_ptr->cmd.hdr.message_id != CM_MO_NORMAL_CALL_REQ )
  {
    /*
    ** We're supposed to set those fields only in CM_MO_NORMAL_CALL_REQ
    */
    CM_MSG_MED_1("Bad MN command = %d",mo_call_req_ptr->cmd.hdr.message_id);
    return FALSE;
  }

  if(!mo_call_req_ptr->cmd.cm_mo_normal_call_req.bearer_capability_1.present)
  {
    /*
    ** Field we need is not present
    */
    CM_MSG_MED_0("No bearer_capability_1 element!");
    return FALSE;
  }

  /*
  ** Now set the fields to correspond to given line
  */
  itc_ptr = &mo_call_req_ptr->cmd.cm_mo_normal_call_req.bearer_capability_1.information_transfer_capability;

  CM_MSG_LOW_1("Set line %d",line); 
  switch(line)
  {
    case CM_ALS_LINE_01:
      
      *itc_ptr = CMALS_GSM_LL_SPEECH;
      break;

    case CM_ALS_LINE_02:
      
      *itc_ptr = cmph_ptr()->itc_aux_val_to_use;
      break;

    default:
      CM_ERR_1("Bad line = %d",line);
      return FALSE;
  }

  return TRUE;

} /* cmals_orange_resolve_mo_call_req_func() */

/*===========================================================================

FUNCTION cmals_orange_resolve_mt_call_ind_func

DESCRIPTION

  MO call may come on any line.
  Look at fields in rpt_ptr and figure out what line they correspond to,
  return the line to the caller.
  Return default line on errors.

  Orange-specific:

  The definition of the Information Transfer Capability (octet 3, bits 3..1)
  of the Bearer Capability Information Element - BCIE (as defined in GSM 04.08)
  shall be extended to set the following bit to denote LINE_01 / LINE_02

  Bits: 3 2 1 (1 is least significant bit)

        0 0 0 - Speech           (LINE_01)
        1 0 0 - Auxiliary speech (LINE_02)

  Look at fields in rpt_ptr (as per above), map them to LINE_01 / LINE_02
  and return it to the user


DEPENDENCIES
  None

RETURN VALUE
  Line on which the call is incoming or default line on error

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL cm_als_line_e_type     cmals_orange_resolve_mt_call_ind_func(

         const cm_rpt_type     *rpt_ptr
           /* Incoming call indication report */
)
{

  const byte *itc_ptr = NULL;
    /* pointer to information transfer capability */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - */


  if(rpt_ptr == NULL)
  {
    CM_MSG_MED_0("rpt_ptr is NULL, use default line (line 1)");
    return CM_ALS_DEFAULT_LINE;
  }

  if(rpt_ptr->hdr.cmd != CM_MT_SETUP_IND)
  {
    /*
    ** We're supposed to read those fields only for CM_MT_SETUP_IND
    */
    CM_MSG_MED_1("Bad LL report = %d, use default line (line 1)",rpt_ptr->hdr.cmd);
    return CM_ALS_DEFAULT_LINE;
  }

  if(!rpt_ptr->cmd.setup_ind.bearer_capability_1.present)
  {
    /*
    ** Field we need is not present
    */
    CM_MSG_MED_0("No bearer_capability_1 element, use default line (line 1)");
    return CM_ALS_DEFAULT_LINE;
  }

  /*
  ** Now map the fields and set call_ptr->line to indicate on
  ** what line the call is incoming
  */
  itc_ptr = &rpt_ptr->cmd.setup_ind.bearer_capability_1.information_transfer_capability;

  if(*itc_ptr == CMALS_GSM_LL_SPEECH)
  {
    CM_MSG_LOW_0("line 1");
    return CM_ALS_LINE_01;
  }

  if(*itc_ptr == cmph_ptr()->itc_aux_val_to_use)
  {
    CM_MSG_LOW_0("line 2");
    return CM_ALS_LINE_02;
  }

  /*
  ** If we got here, we have some garbage
  ** in the field.
  ** Use CM_ALS_DEFAULT_LINE
  */
  CM_MSG_MED_1("Garbage in ITC field = %x, using default line (line 1)",*itc_ptr);
  return CM_ALS_DEFAULT_LINE;

} /* cmals_orange_resolve_mt_call_ind_func() */


/*==========================================================================

FUNCTION cmals_ptr_mmgsdi

DESCRIPTION

  Return a pointer to the one and only ALS custom logic object.

DEPENDENCIES
  None

RETURN VALUE
  Return a pointer to the one and only ALS custom logic object

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL cm_als_custom_logic_mmgsdi_s_type   *cmals_ptr_mmgsdi(
    void
)
{
  static cm_als_custom_logic_mmgsdi_s_type cmals_local;
    /* The one and only ALS object
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return &cmals_local;

} /* cmals_ptr_mmgsdi() */


/*===========================================================================

FUNCTION cmals_reset

DESCRIPTION

  Reset the ALS object.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL void cmals_reset( void )
{


  /*
  ** Set all ALS custom logic functions to NULL
  */
  
  (void)cmals_custom_reg_mmgsdi( NULL,NULL,NULL,NULL,NULL,NULL );
  
} /* cmals_reset() */

#else /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

/*===========================================================================
FUNCTION cmals_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmals_dummy( void )
/*lint -esym(714,cmals_dummy) */
/*lint -esym(528,cmals_dummy) */
/*lint -esym(765,cmals_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */





