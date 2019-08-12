/**********************************************************************
* This software is released under the BSD license as follows.
* Copyright (c) 2014, Murata Manufacturing Co.,Ltd.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following 
* conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following
*        disclaimer in the documentation and/or other materials
*        provided with the distribution.
*     3. Neither the name of the Murata Manufacturing Co.,Ltd. nor the
*        names of its contributors may be used to endorse or 
*        promote products derived from this software without specific
*        prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/
/**********************************************************************
*	Murata Manufacturing Co,.Ltd. 2014
*
*	Filename : sns_dd_press_ZPA2326_ifsys.h
*
*	Purpose  : OS I/F header.
*
* $Author  : Noriko Ueno $
* $Date    : 2014/06/10 19:55 $
* $Revision: #1 $
*
**********************************************************************/
#ifndef ZPA2326_IFSYS_H
#define ZPA2326_IFSYS_H


#include "sns_ddf_util.h"	/* utility library */
#include "sns_dd_press_ZPA2326.h"	/* SMGR I/F */

/**********************************************************************
							VERSION
**********************************************************************/
#define ZPA2326_RELEASE_VERSION			0U
#define ZPA2326_IF_VERSION				6U


/**********************************************************************
							I2C
**********************************************************************/
#define ATM_SENSOR_MURATA_DEV			"ZPA2326-0311A"
#define ATM_SENSOR_I2C_SA0				1	/* High */


/**********************************************************************
							DEBUG
**********************************************************************/
//#define ZPA2326_DEBUG /* test only */


/**********************************************************************
							DEFINE
**********************************************************************/
/* Macro for debug print */
#ifdef ZPA2326_DEBUG
#define PRINT_INFO(x,s)	 				MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__)
#define PRINT_INFO_1(x,s,a1)			MSG_2(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1)
#define PRINT_INFO_2(x,s,a1,a2)			MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2)
#define PRINT_INFO_3(x,s,a1,a2,a3)		MSG_4(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2,a3)
#define PRINT_INFO_4(x,s,a1,a2,a3,a4)	MSG_5(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2,a3,a4)
#else
#define PRINT_INFO(x,s)	 				(void) 0
#define PRINT_INFO_1(x,s,a1)			(void) 0
#define PRINT_INFO_2(x,s,a1,a2)			(void) 0
#define PRINT_INFO_3(x,s,a1,a2,a3)		(void) 0
#define PRINT_INFO_4(x,s,a1,a2,a3,a4)	(void) 0
#endif

#define PRINT_ERR(x,s)	 				MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__)
#define PRINT_ERR_1(x,s,a1)				MSG_2(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1)
#define PRINT_ERR_2(x,s,a1,a2)			MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2)
#define PRINT_ERR_3(x,s,a1,a2,a3)		MSG_4(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2,a3)
#define PRINT_ERR_4(x,s,a1,a2,a3,a4)	MSG_5(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2,a3,a4)

#define PRINT_VER(x,s)	 				MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__)
#define PRINT_VER_1(x,s,a1)				MSG_2(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1)
#define PRINT_VER_2(x,s,a1,a2)			MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2)
#define PRINT_VER_3(x,s,a1,a2,a3)		MSG_4(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2,a3)
#define PRINT_VER_4(x,s,a1,a2,a3,a4)	MSG_5(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "ZPA2326 ["x"][LINE:%d]"s,__LINE__,a1,a2,a3,a4)


/**********************************************************************
							Extern Function
**********************************************************************/
/***** OS I/F *******/
signed int atm_sensor_ifsys_alloc( uint16_t size, void **addr_p );
signed int atm_sensor_ifsys_alloc_free( void *addr_p );
signed int atm_sensor_ifsys_memhandler_alloc( 
									sns_ddf_memhandler_s *	memhandler_p,
									uint32_t size, 
									void **addr_p );
signed int atm_sensor_ifsys_sem_init( void **sem_p );
signed int atm_sensor_ifsys_sem_get( void *sem_p );
signed int atm_sensor_ifsys_sem_free( void  *sem_p );
signed int atm_sensor_ifsys_sem_exit( void *sem_p );
void atm_sensor_ifsys_delay( uint32_t usec );
signed int atm_sensor_ifsys_get_timestamp(sns_ddf_time_t *timestamp_p);


#endif
