#ifndef HDRALMPI_H
#define HDRALMPI_H

/*===========================================================================

          A I R   L I N K   M A N A G E M E N T   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the different files of ALMP.

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdralmpi.h_v   1.18   21 Jan 2002 14:13:08   shuang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdralmpi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/13   wsh     Fixed sending multiple UATI, not sending RTC msg in tunnel  
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
01/20/02   sh      Removed process user cmd and process sig 
09/05/01   dna     Added separate handling for in-use vs. in-config messages.
07/10/01   dna     Added hdralmp_reset(), to reset ALMP if a session is closed.
05/08/01   dna     Re-naming for consistency.
03/29/01   dna     Added reacquire command to force ALMP to re-acquire
09/07/00   dna     Changed open_connection to have a reason, 
                   changed hai_ind_data_union_type -> hdrgivei 
02/02/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdralmp.h"
#include "hdrbit.h"
#include "hdrind.h"
#include "dsm.h"
#include "mc.h"

#ifdef FEATURE_CMI
#include "mc_v.h"
#endif

#include "prot.h"

#ifdef FEATURE_CMI
#include "prot_v.h"
#endif


/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                             DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                               ALMP COMMANDS

--------------------------------------------------------------------------*/

/* Command types */
/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRALMP_ACTIVATE_CMD=0,         /* Starts HDR mode operation */
  HDRALMP_DEACTIVATE_CMD=1,       /* Ends HDR mode operation */
  HDRALMP_OPEN_CONNECTION_CMD=2,  /* Open a connection */
  HDRALMP_CLOSE_CONNECTION_CMD=3, /* Close the connection */
  HDRALMP_REACQUIRE_CMD=4,        /* Return to init, new acquisition */
  HDRALMP_RESET_CMD=5             /* ALMP to reset the conneciton layer */
} hdralmp_cmd_name_enum_type;

typedef struct                     /* HDRALMP_DEACTIVATE_CMD */
{
  hdrhai_abort_reason_enum_type reason;
  boolean                       explicit_mmoc_deactivate;
  prot_trans_type               trans_id;
} hdralmp_deactivate_params_type;

typedef struct                     /* HDRALMP_OPEN_CONNECTION_CMD */
{
  hdrhai_open_reason_enum_type open_reason;
} hdralmp_open_connection_params_type;

typedef struct                     /* HDRALMP_CLOSE_CONNECTION_CMD */
{
  hdrhai_abort_reason_enum_type reason;
} hdralmp_close_connection_params_type;

typedef struct                     /* HDRALMP_REACQUIRE_CMD */
{
  hdrhai_abort_reason_enum_type reason;
} hdralmp_reacquire_params_type;

typedef union 
{                                      /* Common command fields */
  hdralmp_deactivate_params_type         deactivate;
  hdralmp_open_connection_params_type    open_connection;
  hdralmp_close_connection_params_type   close_connection;
  hdralmp_reacquire_params_type          reacquire;
} hdralmp_cmd_params_union_type;

typedef struct
{
  hdralmp_cmd_name_enum_type name;       /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command (debug) */
  hdralmp_cmd_params_union_type params;  /* Common command fields */
} hdralmp_cmd_type;

/*===========================================================================

FUNCTION HDRALMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to ALMP.

DEPENDENCIES
  None

PARAMETERS
  almp_cmd_ptr - Pointer to the command for ALMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdralmp_process_cmd( hdralmp_cmd_type * almp_cmd_ptr );

/*===========================================================================

FUNCTION HDRALMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to ALMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for ALMP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdralmp_process_ind( hdrind_ind_name_enum_type ind_name, 
                                 hdrind_ind_data_union_type * ind_data_ptr );

/*===========================================================================

FUNCTION HDRALMP_PROCESS_MSG

DESCRIPTION
  This function processes messages for ALMP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*===========================================================================

FUNCTION HDRAMP_IS_WAITING_FOR_UATI_ASSGN

DESCRIPTION
  Check if AMP is waiting for UATI Assignment

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If AMP is waiting for UATIAssignment
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_is_waiting_for_uati_assgn( void );
#endif               /* HDRALMPI_H */
