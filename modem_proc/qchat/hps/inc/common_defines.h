/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc. All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * Qualcomm Technologies, Inc. ("Proprietary Information").  You shall
 * not disclose such Proprietary Information, and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with Qualcomm Technologies, Inc.
 *
 */

/*===========================================================================
                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qchat/hps/inc/common_defines.h#1 $ 
$DateTime: 2016/12/13 07:57:51 $ $Author: mplcsds1 $

when        who        what, where, why
--------    ---     --------------------------------------------------------
03/20/11   Suren    Created initial version
===========================================================================*/

#ifndef _COMMON_DEFINES_H_
#define _COMMON_DEFINES_H_

//Suren: This macro is to disable copy constructors
#define DISABLE_COPY_AND_ASSIGN(ClazzName) ClazzName(const ClazzName&); \
                                           ClazzName& operator=(const ClazzName &);

#endif /* _COMMON_DEFINES_H_ */

