#ifndef PS_MIP_TASK_H
#define PS_MIP_TASK_H

/**
  @file ps_mip_task.h

  @brief
   Header file containing the MIP task specific information.
   This is intended to isolate all of the task specific
   information to a single module.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the mip group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2002-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MM_DATA/vcs/ps_mip_task.h_v   1.1   16 Sep 2002 14:52:18   jayanthm  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_mip_task.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/13/14    sd     Removing MIP_SEND_CMD as part of PS signal clean up.
10/18/12    sd     Posting PS command for MIP registration when PS task is not running
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/06/11    sd     (Tech Pubs) Edited Doxygen markup and comments.
02/28/11    op     Doxygen writeup
03/26/09    pp     CMI De-featurization.
12/19/08    pp     Common Modem Interface: Public/Private split.
01/09/07    as     Support for PS task self start dmu pre-encryption
12/04/06  as/msr   MIP dereg support
09/19/06    as     Added mip_task_rand_data_ready() API for secutil to
                   indicate when random data is available.
08/13/04    kvd    Added new cmd MIP_PHYS_IFACE_UP_CMD.
02/02/04    jd     Replace definitions for MIP signal handling with command.
                   Added MIP_RAND_DATA_READY() macro for secutil to call when
                   DMU can poll for random data without blocking.
01/27/02    jd     MIP uses commands now instead of signals
09/10/02    jd/jay created file
===========================================================================*/
/* group: mip */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/** @addtogroup constants
  @{
*/
/*---------------------------------------------------------------------------
  define all of the MIP related commands that can be sent in terms of the
  commands for the hosting task: in this case PS
---------------------------------------------------------------------------*/
#define MIP_META_SM_MIN_CMD         PS_MIP_META_SM_MIN_CMD
/**< Minimum meta state machine command. */

#define MIP_PHYS_IFACE_UP_CMD       PS_MIP_PHYS_IFACE_UP_CMD
/**< Physical up link that starts the MIP. */

#define MIP_CFG_IFACE_CMD           PS_MIP_CFG_IFACE_CMD
/**< Starts the MIP registration to configure the interface. */

#define MIP_EXIT_CMD                PS_MIP_EXIT_CMD
/**< Posts the MIP exit command (i.e., the client is no longer needed). */

#define MIP_REG_FAILURE_CMD         PS_MIP_REG_FAILURE_CMD
/**< Tells the meta SM that the initial registration failed. */

#define MIP_BRING_DOWN_CMD          PS_MIP_BRING_DOWN_CMD
/**< Posts the MIP bring down command, i.e., Starts the MIP deregistration */

#define MIP_META_SM_MAX_CMD         PS_MIP_META_SM_MAX_CMD
/**< Maximum meta SM command. */

#define MIP_SOCKET_EVENT_CMD        PS_MIP_SOCKET_EVENT_CMD
/**< Socket event. */

#define DMU_PREENCRYPT_CMD          PS_DMU_PREENCRYPT_CMD
/**< DMU random event. */

#define MIP_SEND_IMMED_REG_CMD      PS_MIP_SEND_IMMED_REG_CMD
/**< Send immediate MIP registration command */

/** @} */ /* end_addtogroup constants */

/*===========================================================================
FUNCTION    MIP_TASK_RAND_DATA_READY
===========================================================================*/
/** @ingroup mip_task_rand_data_ready
  Called by the security utility when random data is available via 
  secutil_get_rand().

  @return
  None.

  @dependencies
  None.
  @newpage
*/
void mip_task_rand_data_ready( void );


#endif /* PS_MIP_TASK_H */
