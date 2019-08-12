#ifndef _HDRMRLPSCHEDI_H
#define _HDRMRLPSCHEDI_H

/*===========================================================================
                         H D R M R L P   S C H E D
GENERAL DESCRIPTION
  This file contains the configuration for the scheduling of watermarks 
  belonging to a particular RLP. Each MRLP can bind multiple IP flows,
  each IP flow has one watermark. When the new data needs to get transmitted,
  the RLP should take the data out of these watermarks in a round robin fashion.
  
EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpschedi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when         who        what, where, why
--------     ---        -------------------------------------------------------
12/23/05   ksu        EMPA RSP: two rev watermarks per ip flow
07/06/05   sy         Initial revision.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#ifdef FEATURE_HDR_QOS
#include "dsrsp.h"
/*===========================================================================
                            VARIABLES
===========================================================================*/
struct hdrmrlpsched_wm_node
{
  struct hdrmrlpsched_wm_node *link_next_ptr;

  /*-------------------------------------------------------------------------
    The tx watermark list needs to point to an array of watermarks of
    length (at least) DSRSP_NUM_ROUTES.  This array is stored in the
    common configuration for both RLP instances, but each RLP will
    only access the index that matches its route.
  -------------------------------------------------------------------------*/
  dsm_watermark_type          *tx_wm_ptr[DSRSP_NUM_ROUTES];            /* watermarks per ip */
};

typedef struct hdrmrlpsched_wm_node hdrmrlpsched_wm_node_type;

#endif /*FEATURE_HDR_QOS*/
#endif /* _HDRMRLPSCHEDI_H */
