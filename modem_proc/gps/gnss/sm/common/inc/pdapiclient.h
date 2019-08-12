#ifndef PDSMCLIENT_H
#define PDSMCLIENT_H


#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

       Position Determination Session Manager Header File.

DESCRIPTION
  This header file contains definitions necessary to internally interface
  with PDSMCLIENT.C.

  Note that definitions that are necessary to externally interface with
  PDSMCLIENT.C are provided in PDSM.H


  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/inc/pdapiclient.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------    
09/05/14   ssu     GM LOWI Integration
03/14/12   ss      Added support to send error in PA callback
09/03/12   rh      Added support for Cell Info and NI message injection
06/11/12   rk      Enhanced 'pdsm_client_s_type' struct to have extended function types 
                   used by pdsm_client_pa_reg_ex & pdsm_client_pa_event_ntfy functions. 
03/02/09   jd      Added support for Wiper logging over RPC.
01/02/09   atien   Support XTRA-T
06/12/08   lt      Support for external coarse position injection.
07/20/07   gk      Removed obsoleted functions, move pdapi_init to here
07/02/07   rw      added new XTRA client type
05/14/07   gk      added new client type 
04/03/07   gk      added some missing cmd call back fucntions
12/06/06   gk      added ni cleint id get func
12/04/06   gk      Updated LCS response
11/08/06   gk      PDAPI for converged GPS
===========================================================================*/


#include "pdapi.h"        /* External interface to pdsm.c */
#include "queue.h"


/* <EJECT> */
/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                               CLIENT
---------------------------------------------------------------------------*/

#define PDSM_INIT_MASK 0x7F

#define PDSMCLIENT_POOL_SIZE (PDSM_CLIENT_TYPE_MAX)

#define PDSMCLIENT_POOL_SIZE_MAX_ACTIVE_CLIENTS 2  /* Max active should be < pool_size */

#define PDSMCLIENT_POOL_CHECK 1

/* Type for PDSM client.
**
** Client structures keep a record of all the PDSM events for which their
** respective client is registered and thus call on their client callback
** functions as needed.
**
** Each PDSM client needs to allocate such a PDSM client structure in its own
** program space.
**
** Clients register and/or de-register for PDSM events dynamically via methods
** that operate on the PDSM client object that is allocated in their own
** program space.
**
** NOTE THAT NONE OF THE PDSM CLIENT STRUCTURE FIELDS SHOULD BE MODIFIED OR
** ACCESSED DIRECTLY BY THE ITS CLIENT. ANY INITIALIZATION OR MANIPULATION
** OF THE PDSM CLIENT STRUCTURE IS TO BE DONE VIA THE PDSM CLIENT METHODS
** SUPPLIED IN THIS MODULE.
*/
typedef struct pdsm_client_s {

    q_link_type               link;
        /* Link element to link clients into a client-list */
        /* this element has to be the first in this structure
           because the address of link is also used as
           the "self pointer" (to save memory space by eliminating
           self_ptr in q_link_type) to access other elemnet in this
           data structure */

    byte                      init_mask;
        /* Indicate whether this object was properly initialized */

    boolean                   is_active;
        /* Indicate whether client object is active. Note that CM
        ** only notifies active clients of CM events */

     pdsm_client_id_type         client_id;
    struct pdsm_client_s        *client;
        /* Client ID - each initialized client struct has
        ** a unique client ID */


    pdsm_pd_event_type           pd_event_mask;

    pdsm_pa_event_type           pa_event_mask;

#ifdef FEATURE_VECTORONE
    pdsm_or_event_type           or_event_mask;
#endif

    pdsm_pd_event_f_type         *pd_event_func;
        /* Function pointer to notify client of call events */
    void                         *pd_data_ptr;

    pdsm_pa_event_f_type         *pa_event_func;
        /* Function pointer to notify client of call events */
    pdsm_pa_event_ex_f_type      *pa_event_func_ex;
    /* Function pointer to notify client registered for all enum based PA events through pdsm_client_pa_reg_ex interface */

    void                         *pa_data_ptr;
    void                         *pa_data_ptr_ex;
#ifdef FEATURE_VECTORONE
    pdsm_or_event_f_type         *or_event_func;
    void                         *or_data_ptr;
#endif

    pdsm_pd_cmd_err_f_type       *pd_cmd_err_func;
        /* Function to notify client of call command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients - e.g. data services failed to
        ** originate because phone is offline */

    pdsm_pa_cmd_err_f_type       *pa_cmd_err_func;
    pdsm_pa_cmd_err_f_type       *pa_cmd_err_func_ex;

#ifdef FEATURE_VECTORONE
    pdsm_or_cmd_err_f_type       *or_cmd_err_func;
#endif
   pdsm_lcs_event_type            lcs_event_mask;
   pdsm_lcs_event_f_type          *lcs_event_func;
   pdsm_lcs_cmd_err_f_type        *lcs_cmd_err_func;
   void                          *lcs_data_ptr;
   pdsm_ext_status_event_type    ext_status_event_mask;
   pdsm_ext_status_event_f_type  *ext_status_event_func;
   pdsm_ext_status_cmd_err_f_type  *ext_status_cmd_err;
   pdsm_ext_status_cmd_err_f_type        *ext_status_cmd_err_func;
   void                            *ext_status_data_ptr;

   pdsm_xtra_event_type            xtra_event_mask;
   pdsm_xtra_event_f_type          *xtra_event_func;
   pdsm_xtra_cmd_err_f_type        *xtra_cmd_err_func;
   void                            *xtra_data_ptr;

   pdsm_xtra_t_event_type          xtra_t_event_mask;
   pdsm_xtra_t_event_f_type        *xtra_t_event_func;
   pdsm_xtra_t_cmd_err_f_type      *xtra_t_cmd_err_func;
   void                            *xtra_t_data_ptr;

} pdsm_client_s_type;


typedef enum pdsm_cmd_type_e {
    PDSM_CMD_TYPE_NONE  = -1,      /* For internal use only */
    PDSM_CMD_TYPE_PD,              /* PD related commands from User */

#ifdef FEATURE_VECTORONE
    PDSM_CMD_TYPE_OR,              /* OR related commands from User */
#endif

    PDSM_CMD_TYPE_SRCH_MSG,        /* Search commands and responses */
    PDSM_CMD_TYPE_PARAM,
    PDSM_CMD_TYPE_LCS,              /* Response from LCS */
    PDSM_CMD_TYPE_EXT_POS,     /* Use this position */

    PDSM_CMD_TYPE_XTRA,            /* XTRA related commands */
    PDSM_CMD_TYPE_XTRA_T,          /* XTRA-T related commands */
    PDSM_CMD_TYPE_WIPER,            /* WIPER related commands */
    PDSM_CMD_TYPE_XSPI,            /* external SPI related commands */
    PDSM_CMD_TYPE_PERIODIC_PPM,    /* periodic PPM related commands */
    PDSM_CMD_TYPE_MOTION,          /* motion data related commands */
    PDSM_CMD_TYPE_DPO_STATUS_RPT,  /* DPO status report related command */
    PDSM_CMD_TYPE_EXT_CELL,        /* External Cell Info commands */
    PDSM_CMD_TYPE_NI_MSG,          /* NI message command type */
    PDSM_CMD_TYPE_WIFI_ON_DEMAND,  /* WiFi on-demand scan result */
    PDSM_CMD_TYPE_WIFI_ATTACHMENT_STATUS_IND, /*Wifi attachment status indication*/
    PDSM_CMD_TYPE_WIFI_ENABLED_STATUS,  /* WiFi enabled status indication */
    PDSM_CMD_TYPE_MAX,             /* for internal use */
    PDSM_CMD_TYPE_E_SIZE = 0x10000000 /* Fix the enum as int */
} pdsm_cmd_type_e_type;


typedef struct pdsm_pd_cms_end_session_info_s
{
    /* Session type to end */
	 pdsm_pd_end_session_e_type   end_session_type;
	/*Wherthe to turn the receiver off*/
	boolean 					   receiver_off;
}pdsm_pd_cms_end_session_info_s_type;

/* This structure contains all the relevant information provided by user command.
*/
typedef struct pdsm_pd_cmd_info_s {
    pdsm_pd_option_s_type        option_type;
        /* PD Option type */
    pdsm_pd_qos_type             qos;
        /* PD QOS specified by user */
    pdsm_pd_end_e_type           end_status;		
		/* Session End status, reason for ending PD session */
	pdsm_pd_cms_end_session_info_s_type session_end_info;
	/*Session end info*/
    pdsm_pd_cmd_err_e_type       cmd_err;
    /* Error in command request */
    pdsm_pd_external_time_info_type ext_time_info;
    /* external time info. */
    pdsm_pd_external_position_info_type ext_pos_info;
    /* external position info */
} pdsm_pd_cmd_info_s_type;

typedef struct pdsm_ext_pos_cmd_info_s {
  pdsm_ext_use_this_pos_data_type  pos_to_use;
  pdsm_ext_pos_cmd_err_e_type       cmd_err;
    /* Error in command request */
}pdsm_ext_pos_cmd_info_s_type;

#ifdef FEATURE_VECTORONE

typedef union pdsm_or_option_u {

     pdsm_or_option_s_type    meas_option_type;
  #ifdef FEATURE_VECTORONE_DEVICE_OP
     pdsm_or_ca_option_e_type  ca_option_type;
  #endif
} pdsm_or_option_u_type;

typedef struct pdsm_or_cmd_info_s {
    pdsm_or_svc_type             svc_type;
        /* OR service type */
    pdsm_or_option_u_type        option_type;
        /* OR Option type */
    pdsm_or_qos_s_type           qos;
        /* OR QOS specified by user */
    pdsm_or_end_e_type           end_status;
    /* Session End status, reason for ending OR session */
    pdsm_or_cmd_err_e_type       cmd_err;
    /* Error in command request */

} pdsm_or_cmd_info_s_type;

#endif

typedef struct pdsm_pa_cmd_info_s {
  
    pdsm_pa_info_type             pa_set;
        /* PD service type */
    pdsm_pa_e_type                set_type;

    pdsm_pa_cmd_err_e_type        cmd_err;
    /* Error in command request */

} pdsm_pa_cmd_info_s_type;

/* this structure specifies all the data provided by the LCS */
typedef struct pdsm_lcs_cmd_info_s 
{
  pdsm_lcs_cmd_e_type      set_type;
  pdsm_lcs_cmd_info_u_type lcs_cmd_info;
  pdsm_lcs_cmd_err_e_type cmd_err;
} pdsm_lcs_cmd_info_s_type;
/* Type for PD commands.
*/
typedef struct pdsm_pd_cmd_s {

    pdsm_pd_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                          *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_client_id_type           client_id;
        /* Id of the client sending this command */
    pdsm_client_s_type            *client;

    pdsm_pd_cmd_e_type            cmd;
        /* pdsm command */

    pdsm_pd_cmd_info_s_type       info;
        /* pdsm command information */

} pdsm_pd_cmd_s_type;

/* Type for XTRA commands.
*/

/* structure coming down from PDAPI */
typedef struct pdsm_xtra_cmd_info_s {
  /* XTRA ptr to assistance data info */
  byte*                         xtra_data_ptr;
  /* length of data pointed to by data_ptr */
  uint32                        xtra_data_len;
  /* result of download by XTRA.DC */
  pdsm_xtra_dc_status_e_type    xtra_dc_status;
  /* file must be broken into parts by application */
  /* this parameter indicates the current part number */
  byte                          part_number;
  /* total number of parts to be sent by the application */
  byte                          total_parts;
  /* enable/disable XTRA */   
  boolean                       enable_xtra;                  // NOTE - This is not used by the client
  /* enable/disable auto downloads */
  boolean                       enable_auto_download;
  /* download interval (hours) */
  uint16                        download_interval;
  /* Error in command request */

  /* cummulative GPS time (reference date: Jan 6, 1980) (msec) */
  double                        d_TimeMsec;
  /* uncertainty assoc. w/ time (msec) */
  float                         f_TimeUncMsec;
  /* time is referenced to UTC */
  boolean                       b_RefToUtcTime;
  /* force acceptance of time data */
  boolean                       b_ForceFlag;

  pdsm_xtra_cmd_err_e_type      cmd_err;
} pdsm_xtra_cmd_info_s_type;

typedef struct pdsm_xtra_cmd_s {

  pdsm_xtra_cmd_cb_f_type         *cmd_cb_func;
  /* pointer to a client provided callback function */

  void                          *data_block_ptr;
  /* pointer to a client provided callback data block */

  pdsm_client_id_type           client_id;
  /* Id of the client sending this command */
  pdsm_client_s_type            *client;

  pdsm_xtra_cmd_e_type            cmd;
  /* pdsm command */

  pdsm_xtra_cmd_info_s_type       info;
  /* pdsm command information */

} pdsm_xtra_cmd_s_type;


/* ----------------------- XTRA-T ------------------------------------------- */
/* XTRA-T Command Info from PDAPI */
typedef struct pdsm_xtra_t_cmd_info_s 
{
   /* Aries 5.0  - save the command info from XTRA-T client here */

   /* pdsm_xtra_t_get_data related */
   pdsm_xtra_t_get_s_type          get_data;

   /* Error in command request */
   pdsm_xtra_t_cmd_err_e_type      cmd_err;

} pdsm_xtra_t_cmd_info_s_type;


/* XTRA-T Command from PDAPI */
typedef struct pdsm_xtra_t_cmd_s 
{
  pdsm_xtra_t_cmd_cb_f_type     *cmd_cb_func;   /* pointer to a client provided callback function */
  void                          *data_block_ptr;/* pointer to a client provided callback data block */
  pdsm_client_id_type            client_id;     /* Id of the client sending this command */
  pdsm_client_s_type            *client;       

  pdsm_xtra_t_cmd_e_type         cmd;           /* pdsm command */

  pdsm_xtra_t_cmd_info_s_type    info;          /* pdsm command information */
} pdsm_xtra_t_cmd_s_type;

/* structure coming down from PDAPI */
typedef struct pdsm_wiper_cmd_info_s {

  /* WIPER ptr to position report data info */
  p_wiper_position_report_ex_struct_type position_report_ptr;

  pdsm_wiper_cmd_err_e_type      cmd_err;
} pdsm_wiper_cmd_info_s_type;

typedef struct pdsm_wiper_cmd_s {
  pdsm_wiper_cmd_cb_f_type         *cmd_cb_func;
  /* pointer to a client provided callback function */

  void                          *data_block_ptr;
  /* pointer to a client provided callback data block */

  pdsm_client_id_type           client_id;
  /* Id of the client sending this command */
  pdsm_client_s_type            *client;

  pdsm_wiper_cmd_e_type            cmd;
  /* pdsm command */

  pdsm_wiper_cmd_info_s_type       info;
  /* pdsm command information */

} pdsm_wiper_cmd_s_type;

#ifdef FEATURE_VECTORONE
/* Type for OR commands.
*/
typedef struct pdsm_or_cmd_s {

    pdsm_or_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                       *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_client_id_type        client_id;
        /* Id of the client sending this command */
    pdsm_client_s_type         *client;

    pdsm_or_cmd_e_type            cmd;
        /* pdsm command */

    pdsm_or_cmd_info_s_type       info;
        /* pdsm command information */

} pdsm_or_cmd_s_type;

#endif

/* Type for PA commands.
*/
typedef struct pdsm_pa_cmd_s {

    pdsm_pa_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                       *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_client_id_type        client_id;
        /* Id of the client sending this command */

    pdsm_client_s_type         *client;

    pdsm_pa_cmd_e_type            cmd;
        /* pdsm command */

    pdsm_pa_cmd_info_s_type       info;
        /* pdsm command information */

} pdsm_pa_cmd_s_type;

/* Type for LCS commands.
*/
typedef struct pdsm_lcs_cmd_s {

    pdsm_lcs_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                       *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_client_id_type        client_id;
        /* Id of the client sending this command */

    pdsm_client_s_type         *client;

    pdsm_lcs_cmd_e_type            cmd;
        /* LCS command */

    pdsm_lcs_cmd_info_s_type       info;
        /* LCS command information */

} pdsm_lcs_cmd_s_type;

/* Type for LCS commands.
*/
typedef struct pdsm_ext_pos_cmd_s {

    pdsm_ext_pos_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */
          
    void                       *data_block_ptr;
        /* pointer to a client provided callback data block */

    pdsm_client_id_type        client_id;
        /* Id of the client sending this command */

    pdsm_client_s_type         *client;

    pdsm_ext_pos_cmd_e_type            cmd;
        /* LCS command */

    pdsm_ext_pos_cmd_info_s_type       info;
        /* LCS command information */

} pdsm_ext_pos_cmd_s_type;

/* external spi command type */
typedef struct {
   /**< id of caller */
   pdsm_client_id_type        client_id; 
   
   /**< callback notification function */
   pdsm_pd_cmd_cb_f_type      *cmd_cb_func; 
   
   /**< client provided reference data */
   void*                      client_data_ptr; 
   
   /**< injected data */
   pdsm_pd_external_spi_type  xspi_data; 
} pdsm_xspi_cmd_s_type;

/* periodic PPM command type */
typedef struct  {
   /**< id of caller */
   pdsm_client_id_type        client_id; 
   /**< callback notification function */
   pdsm_pd_cmd_cb_f_type      *cmd_cb_func; 
   /**< client provided reference data */
   void*                      client_data_ptr; 
   /**< PPM command is either start or stop, only two cmds supported */
   boolean                    start;
   /**< min time period between two consequtive PPM reports */
   uint16                     period;
} pdsm_periodic_ppm_cmd_s_type;

/* motion data command type */
typedef struct {
   /**< id of caller */
   pdsm_client_id_type        client_id;

   /**< callback notification function */
   pdsm_pd_cmd_cb_f_type      *cmd_cb_func;

   /**< client provided reference data */
   void*                      client_data_ptr;

   /**< injected data */
   pdsm_pd_motion_data_s_type  motion_data;
} pdsm_motion_data_cmd_s_type;

typedef struct {
  boolean enable;
}pdsm_dpo_status_rpt_cmd_s_type;

/* external cell info command type */
typedef struct {
   /**< id of caller */
   pdsm_client_id_type        client_id;

   /**< callback notification function */
   pdsm_pd_cmd_cb_f_type      *cmd_cb_func;

   /**< client provided reference data */
   void*                      client_data_ptr;

   /**< injected data: external cell info */
   pdsm_cell_info_s_type      cell_info;

} pdsm_ext_cell_data_cmd_s_type;

/* NI message command type */
typedef struct {
   /**< id of caller */
   pdsm_client_id_type        client_id;

   /**< callback notification function */
   pdsm_pd_cmd_cb_f_type      *cmd_cb_func;

   /**< client provided reference data */
   void*                      client_data_ptr;

   /**< injected data:  NI message */
   pdsm_ni_message_s_type     ni_msg;

} pdsm_ni_message_data_cmd_s_type;


/* On-demand WiFi scan result message command type */
typedef struct {
   /**< id of caller */
   pdsm_client_id_type        client_id;

   /**< callback notification function */
   pdsm_pd_cmd_cb_f_type      *cmd_cb_func;

   /**< client provided reference data */
   void*                      client_data_ptr;

   /**< injected data:  wifi-on-demand scan result message */
   pdsm_wifi_on_demand_info_list_s_type     wifi_on_demand_info_list;

} pdsm_wifi_on_demand_info_list_cmd_s_type;


typedef union pdsm_cmd_u {
    pdsm_pd_cmd_s_type      pd;
    pdsm_pa_cmd_s_type      pa;
#ifdef FEATURE_VECTORONE
    pdsm_or_cmd_s_type      or;
#endif
    pdsm_lcs_cmd_s_type      lcs;
    pdsm_ext_pos_cmd_s_type  ext_pos;

    pdsm_xtra_cmd_s_type      xtra;
    pdsm_xtra_t_cmd_s_type    xtra_t;
    pdsm_wiper_cmd_s_type      wiper;
    pdsm_xspi_cmd_s_type      xspi;
    pdsm_periodic_ppm_cmd_s_type ppm;
    pdsm_motion_data_cmd_s_type motion;
    pdsm_dpo_status_rpt_cmd_s_type dpo_status_rpt;
    pdsm_ext_cell_data_cmd_s_type ext_cell;
    pdsm_ni_message_data_cmd_s_type ni_msg;
    pdsm_wifi_on_demand_info_list_cmd_s_type   wifi_on_demand_info_list_cmd;
    pdsm_wifi_attachment_status_ind_info  wifi_attachment_status_ind_info;
    pdsm_wifi_enabled_status_ind_type wifi_enabled_status;
} pdsm_cmd_u_type;

typedef struct pdsm_cmd_s  {
    q_link_type             link;
    pdsm_cmd_type_e_type    cmd_type;
    pdsm_cmd_u_type         cmd;
} pdsm_cmd_s_type;

/*===========================================================================

FUNCTION pdsm_client_event_ntfy

DESCRIPTION
  Notify a client of a specified phone event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_client_event_ntfy(

    const pdsm_client_s_type  *client_ptr,
        /* pointer to a client struct */

    pdsm_pd_event_type         pd_event,
        /* notify client of this phone event */

    const pdsm_pd_info_s_type          *pd_info_ptr
        /* pointer to PD information struct */
);

#ifdef FEATURE_VECTORONE
/*===========================================================================

FUNCTION pdsm_client_or_event_ntfy

DESCRIPTION
  Notify a OR client of a specified phone event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_client_or_event_ntfy(

    const pdsm_client_s_type  *client_ptr,
        /* pointer to a client struct */

    pdsm_or_event_type         or_event,
        /* notify client of this phone event */

    const pdsm_or_info_s_type          *or_info_ptr
        /* pointer to OR information struct */
);
#endif

/*===========================================================================

FUNCTION pdsm_client_pa_event_ntfy_ex

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
  void pdsm_client_pa_event_ntfy_ex(
  
    const pdsm_client_s_type    *client_ptr,
        /* pointer to a client struct */
  
    pdsm_pa_event_type          pa_event,
        /* notify client of this PD event */
  
    const pdsm_pa_info_s_type   *pa_info_ptr,    
    /* pointer to pdsm state struct */
  
    pdsm_pa_cmd_err_e_type   pa_cmd_err
    /*CMD Error*/
  
);


/*===========================================================================

FUNCTION pdsm_client_pa_event_ntfy

DESCRIPTION
  Notify a client of a specified PA event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_client_pa_event_ntfy(

    const pdsm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    pdsm_pa_event_type          pa_event,
        /* notify client of this PD event */

    const pdsm_pa_info_s_type   *pa_info_ptr
        /* pointer to pdsm state struct */
);


extern void pdsm_client_ext_status_event_ntfy(

    const pdsm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    pdsm_ext_status_event_type          ext_status_event,
        /* notify client of this PD event */

    const pdsm_ext_status_info_s_type   *ext_status_info_ptr
        /* pointer to pdsm state struct */
);

/* <EJECT> */
/*---------------------------------------------------------------------------
                                CLIENT LIST
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION pdsmclient_list_init

DESCRIPTION
  Initializes the client list.

  This function must be called before using the client list is used in any
  way, place or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void pdsmclient_list_init( void );



/*===========================================================================

FUNCTION pdsmclient_list_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_event_ntfy(

    pdsm_pd_event_type   pd_event,
        /* notify clients of this PD event */

    const pdsm_pd_info_s_type    *pd_info_ptr
        /* pointer to PD information struct */
);

/*===========================================================================

FUNCTION PDSMCLIENT_REG_LIST_PD_EVENT_NTFY

DESCRIPTION
  This function is used to notify all registered clients other than active clients
  about the PD events.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern void pdsmclient_reg_list_pd_event_ntfy( pdsm_pd_event_type,const pdsm_pd_info_s_type *info_ptr, pdsm_client_s_type **list_ptr);


/*===========================================================================

FUNCTION PDSMCLIENT_LIST_PD_EVENT_NTFY

DESCRIPTION
  This function is used inform client list about the PD events.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern void pdsmclient_list_pd_event_ntfy( pdsm_pd_event_type,const pdsm_pd_info_s_type *info_ptr );


/*===========================================================================

FUNCTION pdsmclient_list_pa_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PA event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pa_event_ntfy(

  pdsm_pa_event_type   pa_event,
      /* notify clients of this PD event */

  const pdsm_pa_info_s_type    *pa_info_ptr
      /* pointer to PD information struct */
);

/*===========================================================================

FUNCTION pdsmclient_list_pa_event_ntfy_ex

DESCRIPTION
  Notify all the clients in the client list of a specified PA event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_pa_event_ntfy_ex(

  pdsm_pa_event_type   pa_event,
      /* notify clients of this PD event */

  const pdsm_pa_info_s_type    *pa_info_ptr,
      /* pointer to PD information struct */

  pdsm_pa_cmd_err_e_type   pa_cmd_err
    /*CMD Error*/

);


/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_list_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_pd_cmd_err_ntfy(

    pdsm_pd_cmd_err_e_type      pd_cmd_err,
        /* PD command error code */

    const pdsm_pd_cmd_s_type    *pd_cmd_ptr
        /* pointer to PD command struct */

);

#ifdef FEATURE_VECTORONE
/*===========================================================================

FUNCTION pdsmclient_list_or_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified OR command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_or_cmd_err_ntfy(

    pdsm_or_cmd_err_e_type      or_cmd_err,
        /* OR command error code */

    const pdsm_or_cmd_s_type    *or_cmd_ptr
        /* pointer to OR command struct */

);
#endif

/*===========================================================================

FUNCTION pdsmclient_list_ext_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified ext status event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pdsmclient_list_ext_status_event_ntfy(

  pdsm_ext_status_event_type   ext_status_event,
      /* notify clients of this PD event */

  const pdsm_ext_status_info_s_type    *ext_status_info_ptr
      /* pointer to PD information struct */
);

/*===========================================================================

FUNCTION pdsmclient_list_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_lcs_cmd_err_ntfy(

    pdsm_lcs_cmd_err_e_type      lcs_cmd_err,
        /* LCS command error code */

    const pdsm_lcs_cmd_s_type    *lcs_cmd_ptr
        /* pointer to LCS command struct */

);


/*===========================================================================

FUNCTION pdsmclient_list_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_ext_status_cmd_err_ntfy(

    pdsm_ext_pos_cmd_err_e_type      ext_pos_cmd_err,
        /* EXT POS command error code */

    const pdsm_ext_pos_cmd_s_type    *ext_pos_cmd_ptr
        /* pointer to EXT POS command struct */

);

/*===========================================================================

FUNCTION pdsmclient_list_pa_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_pa_cmd_err_ntfy(

    pdsm_pa_cmd_err_e_type      pa_cmd_err,
        /* PD command error code */

    const pdsm_pa_cmd_s_type    *pa_cmd_ptr
        /* pointer to PD command struct */

);

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_event_ntfy

DESCRIPTION
  Notify a client of a specified PD event if the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsm_client_event_ntfy(

    const pdsm_client_s_type       *client_ptr,
        /* pointer to a client struct */

    pdsm_pd_event_type         pd_event,
        /* notify client of this PD event */

    const pdsm_pd_info_s_type    *pd_info_ptr
        /* pointer to PD information struct */
);

#ifdef FEATURE_VECTORONE

/*===========================================================================

FUNCTION pdsmclient_list_or_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified OR event.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_list_or_event_ntfy(

  pdsm_or_event_type   or_event,
      /* notify clients of this OR event */

  const pdsm_or_info_s_type    *or_info_ptr
      /* pointer to OR information struct */
);

/*===========================================================================

FUNCTION pdsmclient_reg_list_or_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD event. Only the
  registered clients other than those which were active are informed about
  this event. The active client are passed through list_ptr.

DEPENDENCIES
  Client list must have already been initialized with pdsmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void pdsmclient_reg_list_or_event_ntfy(

  pdsm_or_event_type           or_event,
      /* notify clients of this OR event */

  const pdsm_or_info_s_type    *or_info_ptr,
      /* pointer to PD information struct */

  pdsm_client_s_type           **list_ptr
      /* List of clients which are active and would be informed separately */
);

#endif


/*===========================================================================

FUNCTION pdsmclient_free_q_init

DESCRIPTION
  Initialize the client free queue.

  This function must be called before the pdsmclinet free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void pdsmclient_free_q_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_get_client_ptr

DESCRIPTION
  Returns a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_client_s_type* pdsmclient_get_client_ptr( 
    
    pdsm_client_id_type client_id
    /* Client Id */
);

/*===========================================================================

FUNCTION PDSMCLIENT_VALID_CLIENT_ID

DESCRIPTION
  This function is used to validate a client handle.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern boolean pdsmclient_valid_client_id( pdsm_client_id_type client_id );

/*===========================================================================

FUNCTION pdsmclient_get_client_type

DESCRIPTION
  checks if it is a valid client handle. If the client is valid, the corsp
  client type will be returned to caller

DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_client_type_e_type pdsmclient_get_client_type_map( pdsm_client_id_type client_id );

/* <EJECT> */
/*===========================================================================

FUNCTION pdsmclient_get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_client_id_type pdsmclient_get_client_id( 
    
    pdsm_client_s_type *client_ptr
    /* Client object pointer */
);

/*===========================================================================

FUNCTION PDSMCLIENT_Q_INIT

DESCRIPTION
  This function is used for Q initialization

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void pdsmclient_q_init(void);   


/*===========================================================================

FUNCTION PDSMCLIENT_INFO_ALLOC

DESCRIPTION
  This function is used to allocate info buffer for sending Event reports back to clients.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

extern pdsm_pd_info_s_type *pdsmclient_info_alloc( pdsm_client_s_type   *client_ptr );

/*===========================================================================

FUNCTION PDSM_LCS_AGENT_CLIENT_REQUEST

DESCRIPTION
  This function is used to  send a Notification verification request to a 
  registered UI client.
  
RETURN VALUE
  None

DEPENDENCIES
  Called by LCS Agent.

===========================================================================*/
extern boolean pdsm_lcs_agent_client_event_ntfy
(
  pdsm_lcs_event_type      lcs_event,             /* Type of LCS event */
  pdsm_lcs_info_s_type     *lcs_info_ptr            /* data to be sent to UI client */
);
/* #endif */

/*===========================================================================

FUNCTION pdsmclient_get_client_type

DESCRIPTION
  This function provides Client ID based on client type
  
DEPENDENCIES
  None

RETURN VALUE
  Client type for the client ptr

SIDE EFFECTS
  None

===========================================================================*/
extern pdsm_client_id_type pdsmclient_get_client_id_map(
  pdsm_client_type_e_type client_type
    /* Client Id */
);

/*===========================================================================
FUNCTION pdsm_ni_client_id_get
DESCRIPTION
   This function is used to get the pdapi client id for the NI requests
RETURN VALUE 
     void
DEPENDENDENCIES
===========================================================================*/
extern pdsm_client_id_type pdsm_ni_client_id_get (void);



/*===========================================================================

FUNCTION PDAPI_INIT

DESCRIPTION
  Initialize the Position Determination API - called from the PD task

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_TASK or lsm_task

===========================================================================*/

extern void pdapi_init( void );


/*===========================================================================

FUNCTION PDSM_XTRA_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/

extern boolean pdsm_xtra_client_request
(
  pdsm_xtra_event_type      xtra_event,        /* Type of XTRA event */
  pdsm_xtra_info_s_type     *xtra_info_ptr     /* data to be sent to XTRA client */
);



/*===========================================================================

FUNCTION PDSM_XTRA_T_CLIENT_REQUEST

DESCRIPTION
This function is used to send a request to a registered XTRA-T client.

RETURN VALUE
None

DEPENDENCIES

===========================================================================*/
extern boolean pdsm_xtra_t_client_request
(
 pdsm_xtra_t_event_type      xtra_t_event,        /* Type of XTRA-T event */
 pdsm_xtra_t_info_s_type     *xtra_t_info_ptr     /* data to be sent to XTRA-T client */
);

/*===========================================================================

FUNCTION pdsm_set_ready_flag

DESCRIPTION
This function is used to set a global flag which should be set to TRUE when 
PDAPI has been initialized and is ready to accept calls from PDAPI clients.

This function was added because on some targets this was causing crashes if
a client called pdsm_client_init() before TM was PDAPI was ready.

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/
extern void pdsm_set_ready_flag(void);


/*===========================================================================

FUNCTION pdsm_set_gnss_oem_feature_mask

DESCRIPTION
This function is used to get a copy of the NV item "OEM feture mask"


DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/
extern void pdsm_set_gnss_oem_feature_mask(uint32 q_gnss_oem_feature_mask);


/*===========================================================================

FUNCTION pdsm_get_ready_flag

DESCRIPTION
This function is used to get a global flag which should be set to TRUE when 
PDAPI has been initialized and is ready to accept calls from PDAPI clients.

DEPENDENCIES
none

RETURN VALUE
  TRUE: if TM is initialized
  FALSE: if TM is not intialized

SIDE EFFECTS
none

===========================================================================*/
extern boolean pdsm_get_ready_flag(void);


#ifdef __cplusplus
}
#endif


#endif /* PDSMCLIENT_H */


