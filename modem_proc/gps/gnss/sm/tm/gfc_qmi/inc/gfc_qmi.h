/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI Interface to PDAPI Header file.

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/gfc_qmi/inc/gfc_qmi.h#1 $
$DateTime: 2016/12/13 07:59:45 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
11/15/11   ssu     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifndef GFC_QMI_H
#define GFC_QMI_H

#include "pdapi.h"

/*
******************************************************************************
* tm_gfc_qmi_prtl_init
*
* Function description:
*
*   This function initialises the GFC_QMI module . This sets up the internal
*   states of the GFC_QMI and registers with GM core.
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void tm_gfc_qmi_prtl_init(void);

/*
******************************************************************************
* tm_gfc_qmi_inject_ext_pos
*
* Function description:
*
*   This function handles Ext position injection
*
* Parameters: None.
*   
* Return value: None.
*
******************************************************************************
*/
void tm_gfc_qmi_inject_ext_pos(pdsm_pd_external_position_info_type* ext_pos_ptr);

#endif /* GFC_QMI_H */
