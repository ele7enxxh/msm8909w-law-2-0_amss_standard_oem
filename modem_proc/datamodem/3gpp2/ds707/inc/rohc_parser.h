/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           ROHC_PARSER . H

GENERAL DESCRIPTION
  This file contains the function prototypes for ROHC parameter parser.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 

 Copyright (c) 2004,2005,2006 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  

--------------------------------------------------------------------------
when        who    what, where, why
--------    ---    -------------------------------------------------------
03/28/07    ac     Added the comment
09/21/06    ac     Initial revision.
===========================================================================*/

void get_default_comp_flow(rohc_params_comp_flow_s_type *);
void get_default_comp_channel(rohc_params_comp_channel_s_type *);
void get_default_decomp_flow(rohc_params_decomp_flow_s_type *);
void get_default_decomp_channel(rohc_params_decomp_channel_s_type *);
sint15 rohc_read_db_from_efs(void);
