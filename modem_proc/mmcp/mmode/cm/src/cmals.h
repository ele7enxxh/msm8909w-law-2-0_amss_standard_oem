#ifndef CMALS_H
#define CMALS_H
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

  Currently ALS custom logic supports Orange-specific custom logic functions.
  All other vendor-specific ALS custom logic functions would need to be
  placed in this module as well.


EXTERNALIZED FUNCTIONS

  All functions listed in cmals.h

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmals.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/11   rm      GSDI clean-up
03/24/09   sv      updated to cmtaski.h
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
07/31/06   ic      Lint fixes
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   jar     Changed CMALS_CSP_ALS_ALLOWED_MASK from 0x40 to 0x80
09/02/04   ic      Synced up to MSMSHARED_CM.02.00.19.00.07 and its unit test
                   (to help with FEATURE_ALS integration on M6250CTMQG4230)
09/01/04   ic      Changes to FEATURE_ALS.
                   Added support for FEATURE_ALS_ALLOWED_WITH_DFS_ABSENT
07/12/04   ic      Initial release

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "cm.h"
#include "cm_v.h"
#include "cmi.h"
#include "comdef.h"
#include "sys.h"
#include "sys_v.h"
#include "cmdbg.h"
#include "cmtaski.h"
#include "cmclient.h"

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)

#include "cmwll.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/*
** ALS allowed settings as written on the SIM
*/
#define CMALS_CSP_ALS_ALLOWED_MASK 0x80
#define CMALS_CSP_LENGTH_MAX       18 /* Mandatory Maximum Number of bytes */
#define CMALS_CSP_SERVICE_CPHS_TS  0x06 /* Service group code for CPHS 
                                        ** Teleservices
                                        */
/*
** UIM_ORANGE_DFS/UIM_USIM_ORANGE_DFS
** This is an Orange specific EF which contains the Dynamic Flags status.
** This coding is used by the handset to determine selected line
*/
#define CMALS_ORANGE_SIM_EF_LINE_01   0x01
#define CMALS_ORANGE_SIM_EF_LINE_02   0x00

/*
** UIM_ORANGE_D2FS/UIM_USIM_ORANGE_D2FS
** This is an Orange specific EF which contains the Dynamic2 Flag setting.
** This coding is used by the handset to determine the lock status of the
** selected line (i.e whether line switching is allowed/not allowed)
*/
#define CMALS_ORANGE_SIM_EF_LINE_SWITCHING_ALLOWED     0x00
#define CMALS_ORANGE_SIM_EF_LINE_SWITCHING_NOT_ALLOWED 0x01

/*
** Length of dynamic flag fields for files UIM_ORANGE_DFS / UIM_ORANGE_D2FS
** (1 byte)
*/
#define CM_ALS_ORANGE_MAX_DYNAMIC_FLAGS_BUFFER_LEN 0x01

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/*
** Type for a function used to map CM ALS enum LINE_#N to
** set of fields needed to be set in GSM MO call request
*/
typedef boolean (cm_als_resolve_mo_call_request_f_type) (

  cm_als_line_e_type        line,
    /* Line on which the call is to be originated */

  mn_cnm_cmd_type          *mo_call_request
    /*
    ** Pointer to MO call request to be passed to
    ** GSM call processing layer.
    ** ALS custom is to set fields in the call request
    ** so that they correspond to given line.
    ** These may be different for different phone manufacturers
    */

);

/*
** Type for a function used to map set of fields
** set in MT call indication to CM ALS enum LINE_#N
** on which this call is coming
*/
typedef cm_als_line_e_type (cm_als_resolve_mt_call_ind_f_type) (

  const cm_rpt_type  *mt_call_ind_rpt

);



/*
** Type for a function used to read
** line switching setting from the SIM
*/
typedef void (cm_als_mmgsdi_read_line_switching_f_type) (void);

/*
** Type for a function used to read
** selected line setting from the SIM
*/
typedef void (cm_als_mmgsdi_read_selected_line_f_type) (void);


/*
** Type for a function used to write
** line switching setting to the SIM
*/
typedef void (cm_als_mmgsdi_write_line_switching_f_type) (

      cm_als_line_switching_e_type       line_switching,
        /* Setting to be written to the SIM */

      const cm_ph_cmd_s_type             *ph_cmd
        /* CM_PH_CMD_LINE_SWITCHING command */

);


/*
** Type for a function used to write
** selected line setting to the SIM
*/
typedef void (cm_als_mmgsdi_write_selected_line_f_type) (

      cm_als_line_e_type      line,
        /* Setting to be written to the SIM */

      const cm_ph_cmd_s_type  *ph_cmd
        /* CM_PH_CMD_SELECTED_LINE command */

);


/* Type to hold ALS custom logic functions
*/
typedef struct cm_als_custom_logic_mmgsdi_s
{

  cm_init_mask                              init_mask;
    /* Indicate whether this object was properly initialized
    */

  cm_als_resolve_mo_call_request_f_type     *resolve_mo_call_req_func;
    /* Set GSM LL MO call request fields to correspond to selected line
    */

  cm_als_resolve_mt_call_ind_f_type         *resolve_mt_call_ind_func;
    /* Return line corresponding to fields set in GSM LL MT call indication
    */

  cm_als_mmgsdi_read_line_switching_f_type  *mmgsdi_read_line_switching_func;
    /* Read from the SIM line switching settings
    */

  cm_als_mmgsdi_read_selected_line_f_type   *mmgsdi_read_selected_line_func;
    /* Read from the SIM selected line
    */

  cm_als_mmgsdi_write_line_switching_f_type *mmgsdi_write_line_switching_func;
    /* Write to the SIM line switching setting
    */

  cm_als_mmgsdi_write_selected_line_f_type  *mmgsdi_write_selected_line_func;
    /* Write to the SIM selected line
    */

  byte                                       gsm_ll_auxilary_speech;
    /* Custom auxilary speech line value
    */

} cm_als_custom_logic_mmgsdi_s_type;



/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/

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
EXTERNAL boolean                                  cmals_custom_reg_mmgsdi(

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
);



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
  TRUE : If cmmgsdi_cmals_mmgsdi_async_sim_read() retruns TRUE
  FALSE : If cmmmgsdi_cmals_mmgsdi_async_sim_read() retruns FALSE

SIDE EFFECTS
  None

==========================================================================*/
EXTERNAL boolean              cmals_mmgsdi_read_cphs(

    cm_ss_e_type         ss,
      /* Current ss */	  
    cm_ph_event_e_type   event
      /* Event to send after all ALS SIM items have been read */

);



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
);


/*===========================================================================

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

===========================================================================*/
extern void cmals_init(
       void
);


/*==========================================================================

FUNCTION cmals_orange_mmgsdi_read_completed_cb

DESCRIPTION

  Callback function to be called by MMGSDI when ALS item has been read from
  the SIM by cmmmgsdi_cmals_mmgsdi_async_sim_read()

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
EXTERNAL void     cmals_orange_mmgsdi_read_completed_cb(

    mmgsdi_return_enum_type        status,
    mmgsdi_cnf_enum_type           cnf,
    const mmgsdi_cnf_type          *cnf_ptr

);



/*==========================================================================

FUNCTION cmals_orange_mmgsdi_read_line_switching_func

DESCRIPTION

  Read from the SIM all the files needed to figure out
  whether line switching is allowed/forbidden.

  Use cmmmgsdi_cmals_mmgsdi_async_sim_read() to issue all read commands one by one
  and pass cmals_orange_mmgsdi_read_completed_cb() as a callback function to
  CM MMGSDI that will be called by MMGSDI for each read completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL void cmals_orange_mmgsdi_read_line_switching_func(
    void
);




/*==========================================================================

FUNCTION cmals_orange_mmgsdi_read_selected_line_func

DESCRIPTION

  Read from the SIM all the files needed to figure out
  what the currently selected line is.

  Use cmmmgsdi_cmals_mmgsdi_async_sim_read() to issue all read commands one by one
  and pass cmals_orange_mmgsdi_read_completed_cb() as callback function to 
  CM from MMGSDI that will be called by MMGSDI for each read completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERNAL void cmals_orange_mmgsdi_read_selected_line_func(
    void
);



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
EXTERNAL void          cmals_orange_mmgsdi_write_completed_cb
(
     mmgsdi_return_enum_type   status,
     mmgsdi_cnf_enum_type      cnf,
     const mmgsdi_cnf_type     *cnf_ptr
);




/*==========================================================================

FUNCTION cmals_orange_mmgsdi_write_line_switching_func

DESCRIPTION

  Map given line_switching setting to SIM files that need to be written in
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

==========================================================================*/
EXTERNAL void            cmals_orange_mmgsdi_write_line_switching_func(

    cm_als_line_switching_e_type line_switching,
      /* Setting to be written to the SIM */

    const cm_ph_cmd_s_type      *ph_cmd_ptr
      /* CM_PH_CMD_LINE_SWITCHING command */

);



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

);
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
extern boolean                   cmals_orange_resolve_mo_call_req_func(

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
);

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
extern cm_als_line_e_type     cmals_orange_resolve_mt_call_ind_func(

       const cm_rpt_type     *rpt_ptr
         /* Incoming call indication report */
);


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
);


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
extern void cmals_reset(
       void
);

#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */
#endif /* CMALS_H */

