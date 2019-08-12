#ifndef HDRSMPI_H
#define HDRSMPI_H

/*===========================================================================

            S E S S I O N   M A N A G E M E N T   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional functions exported by SMP,
  and any types, macros, or variables used in the different files of SMP.

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrsmpi.h_v   1.14   14 Mar 2003 11:16:18   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrsmpi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
02/25/04   mpa     Added payload to the deactivate cmd.
03/12/03   mpa     Added hdrsmp_send_session_close_ind()
04/15/02   mpa     Removed the hdrsmp_time_source_enum_type enum type.
02/07/02   mpa     Added the hdrsmp_time_source_enum_type enum type.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
11/21/00   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrsmp.h"
#include "bit.h"
#include "dsmbit.h"
#include "hdrmc_v.h"


/*===========================================================================

                             DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
                              SMP COMMANDS

---------------------------------------------------------------------------*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRSMP_TASK_PTR hdrmc_tcb_ptr

/*
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

/* Command type */
typedef enum
{
  HDRSMP_ACTIVATE_CMD=0,            /* Connection is Active */
  HDRSMP_DEACTIVATE_CMD=1,          /* Deactivate the Protocol */
  HDRSMP_KA_OUTCOME_CMD=2,          /* Keep Alive Request Outcome Command */
  HDRSMP_SC_COMPLETE_CMD=3          /* Session Close Complete Command */
} hdrsmp_cmd_name_enum_type;

/* data for the KA_OUTCOME_CMD */
typedef struct
{
  hdrerrno_enum_type err_val;     /* error value for the KA outcome callback */
} hdrsmp_ka_outcome_cmd_type;

/* data for the SC_COMPLETE_CMD */
typedef struct
{
  hdrerrno_enum_type err_val;     /* error value for the SC complete
                                     callback */
} hdrsmp_sc_complete_cmd_type;

/* data for the DEACTIVATE_CMD */
typedef struct
{
  boolean close_is_silent; /* session close method (T-silently, F-explicitely) */
} hdrsmp_deactivate_cmd_type;

typedef union                     /* Common command fields */
{
  hdrsmp_ka_outcome_cmd_type ka_outcome; /* HDRSMP_KA_OUTCOME_CMD */
  hdrsmp_sc_complete_cmd_type sc_complete; /* HDRSMP_SC_COMPLETE_CMD */
  hdrsmp_deactivate_cmd_type deactivate; /* HDRSMP_DEACTIVATE_CMD */
} hdrsmp_cmd_params_union_type;

typedef struct
{
  hdrsmp_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command,
                                            used for debug */
  hdrsmp_cmd_params_union_type params; /* Common command fields */
} hdrsmp_cmd_type;

/*---------------------------------------------------------------------------
                        SMP PROCESS FUNCTIONS

---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to SMP.

DEPENDENCIES
  None

PARAMETERS
  smp_cmd_ptr - Pointer to the command for SMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsmp_process_cmd
(
  hdrsmp_cmd_type * smp_cmd_ptr
);

/*===========================================================================

FUNCTION HDRSMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to SMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for SMP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsmp_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/*===========================================================================

FUNCTION HDRSMP_PROCESS_MSG

DESCRIPTION
  This function processes messages for SMP.

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
extern void hdrsmp_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
);

/*=============================================================================
FUNCTION HDRSMP_PROCESS_SIG

DESCRIPTION
  This function processes signals for SMP.

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrsmp_process_sig
(
  rex_sigs_type sig
);

/*===========================================================================

FUNCTION HDRSMP_SEND_SESSION_CLOSE_IND

DESCRIPTION
  This helper function sends a session close indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsmp_send_session_close_ind( void );

#endif               /* HDRSMPI_H */
