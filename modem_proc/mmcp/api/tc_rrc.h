#ifndef TC_RRC_H
#define TC_RRC_H

/*===========================================================================
                        T C -  R R C  Header File

DESCRIPTION

   Definition of Primitive message types between Test Control Entity
   and Radio Resource Control Entity (TC <-> RRC)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/tc_rrc.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/03/02   mks      Added dummy_signaling_enabled flag to 
                    rrctc_test_loopback_ind data type
01/28/02   mks      Created file.
            
===========================================================================*/
#include "comdef.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between Test Control (TC) entity and the RRC.
* --------------------------------------------------------------------------*/
                              
/* Primitives from TC to RRC */

/* ---------------------------------------------------------------------------
* Commands from TC to RRC - These definitions will be removed once RRC defines
* them in one of its header files
* --------------------------------------------------------------------------*/
#define RRCTC_MODE_CHANGE       0x01
#define RRCTC_TEST_LOOPBACK_IND 0x02

/* ---------------------------------------------------------------------------
* Definitions for mode_type
* --------------------------------------------------------------------------*/
#define RRCTC_NORMAL_MODE       0x21
#define RRCTC_RB_TEST_MODE      0x22

/* ---------------------------------------------------------------------------
* Definitions for lb_mode_type 
* --------------------------------------------------------------------------*/
#define RRCTC_LB_MODE1          0x41
#define RRCTC_LB_MODE2          0x42

#define RRCTC_LB_MODE3          0x43


/* Commands from RRC to TC */
/* These are defined in the TCTASK.H file */

/* --------------------------------------------------------------------------
** Data types used in the Primitives in RRCTC-SAP
** ------------------------------------------------------------------------*/

/**************************************************************************
          Messages from TC to RRC.
**************************************************************************/                                                                  

/* --------------------------------------------------------------------------
** Primitive : RRCTC_MODE_CHANGE, used by TC to notify RRC about mode change     
** ------------------------------------------------------------------------*/
typedef struct rrctc_mode_change
{ 
  byte mode_type; 
} rrctc_mode_change_T;

/* --------------------------------------------------------------------------
** Primitive : RRCTC_TEST_LOOPBACK_IND, used by TC to notify RRC about the 
**             loopback mode type. This is a synchronous event.
** ------------------------------------------------------------------------*/
typedef struct rrctc_test_loopback_ind
{ 
  byte lb_mode_type;
  boolean dummy_signaling_enabled;
} rrctc_test_loopback_ind_T;


/**************************************************************************
            Messages from RRC to TC.
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : RRCTC_TEST_LOOPBACK_CNF, RRC confirms TC about LB mode 
**             indication event after configuring L1/RLC and MAC
** ------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------
** Note : This primitive doesn't have any fields, the cmd_id in the header 
          field of TC_CMD_TYPE (defined in tctask.h) is sufficient.
** ------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
** Primitive : RRCTC_TEST_LOOPBACK_REJ, RRC rejects LB mode indication from TC
** ------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------
** Note : This primitive doesn't have any fields, the cmd_id in the header 
          field of TC_CMD_TYPE (defined in tctask.h) is sufficient.
** ------------------------------------------------------------------------*/

#endif
