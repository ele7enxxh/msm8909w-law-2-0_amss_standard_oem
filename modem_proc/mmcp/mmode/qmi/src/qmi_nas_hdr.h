// -*- Mode: C -*-
//=========================================================================== 
// FILE: qmi_nas_hdr.h
//
// SERVICE: QMI
//
// DESCRIPTION:
// 
//  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_hdr.h#1 $ 
//   
// Copyright (c) 2011 Qualcomm Technologies Inc. All Rights Reserved. 
//=========================================================================== 
#ifndef QMI_NAS_HDR_H
#define QMI_NAS_HDR_H

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_NAS_HDR 

#include "hdrhai.h"
#include "hdrscp.h"  // get_cusrrent_subtype
#include "hdrovhd.h" // get_color_code
#include "hdrmc_v.h"

///   Initialize any hdr state vars
void qmi_nas_init_hdr( void );
 
///   Callback function registered w/ and invoked by HDR to report HDR events
void qmi_nas_hdr_evt_cb( hdrmc_cb_params_rsp_union_type *payload, void* client_data );

///   Register QMI NAS callback w/HDR
boolean qmi_nas_register_to_hdr(void);

///   Process HDR events
void qmi_nas_hdr_process_evt( const void *p_evt );

#endif // FEATURE_QMI_NAS_HDR

#endif // QMI_NAS_HDR_H
