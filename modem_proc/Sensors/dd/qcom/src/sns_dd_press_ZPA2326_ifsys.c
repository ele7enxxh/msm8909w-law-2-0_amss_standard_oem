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
/*******************Murata Manufacturing Co,.Ltd. 2014*****************
*
* Filename : sns_dd_press_ZPA2326_ifsys.c
*
* Purpose  : This module has implementation 
*               of OS I/F for PressureSensor device.
*
* $Author  : Noriko Ueno $
* $Date    : 2014/6/20 15:30 $
* $Revision: #1 $
*
**********************************************************************/

/**********************************************************************
							INCLUDE
**********************************************************************/


#include "sns_dd_press_ZPA2326_core.h"		/* core */
#include "sns_dd_press_ZPA2326_ifi2c.h"		/* i2c */
#include "sns_dd_press_ZPA2326_ifsys.h"		/* sys */

/**********************************************************************
							DEFINE
**********************************************************************/


/**********************************************************************
							Proto Type 
**********************************************************************/


/**********************************************************************
    						STRUCT
**********************************************************************/


/**********************************************************************
							GLOBAL
**********************************************************************/


/**********************************************************************
							FUCTION
**********************************************************************/


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_ifsys_alloc
 *
 *  Description: get buffer
 **  Arguments: In)     size_t      size
 *              In/Out) void        **addr_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 * 
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_ifsys_alloc( uint16_t size, void **addr_p )
{
	signed int ret = ATM_SENSOR_OK;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	
	PRINT_INFO_1("START","[size:%d] ",size);
	
	/* confirm to value of parameters */
	if( addr_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	/* get buffer */
	status = sns_ddf_malloc( addr_p, size );
	if( status != SNS_DDF_SUCCESS )
	{
		PRINT_ERR("ALLOC FAIL","");
		ret = ATM_SENSOR_BUF_ERR;
	}
	else
	{
		PRINT_INFO_1("GET_BUF","[addr:%p]",*addr_p);
	}
	PRINT_INFO_1("END","[ret:%d]",ret);

	return ret;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_ifsys_alloc_free
 *
 *  Description: free buffer
 **  Arguments: In) void  *addr_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_ifsys_alloc_free( void *addr_p )
{
	PRINT_INFO_1("START","[addr_p:%p]",addr_p);
	
	/* confirm to value of parameters */
	if( addr_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	PRINT_INFO_1("BEFORE FREE ALLOC","[addr:%p]",addr_p);
	
	/* free buffer */
	sns_ddf_mfree( addr_p );
	
	PRINT_INFO_1("AFTER FREE ALLOC","[addr:%p]",addr_p);
	
	PRINT_INFO("END","");
	
	return ATM_SENSOR_OK;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_ifsys_memhandler_alloc
 *
 *  Description: get buffer
 **  Arguments: In/OUT) sns_ddf_memhandler_s *memhandler
 *              In)     size_t                size
 *              In/Out) void                **addr_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 * 
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_ifsys_memhandler_alloc( 
									sns_ddf_memhandler_s *	memhandler_p,
									uint32_t size, 
									void **addr_p )
{
	signed int ret = ATM_SENSOR_OK;
	
	PRINT_INFO_1("START","[size:%d] ",size);
	
	/* confirm to value of parameters */
	if( ( addr_p == NULL ) || ( memhandler_p == NULL ) )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	/* get buffer */
	*addr_p = sns_ddf_memhandler_malloc( memhandler_p, size );
	if( NULL == *addr_p )
	{
		PRINT_ERR("ALLOC FAIL","");
		ret = ATM_SENSOR_BUF_ERR;
	}
	else
	{
		PRINT_INFO_1("GET_BUF","[addr:%p]",*addr_p);
	}
	PRINT_INFO_1("END","[ret:%d]",ret);

	return ret;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_ifsys_sem_init
 *
 *  Description: init semaphore 
 **  Arguments: In/Out) void  **sem_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes: This is empty function,
 * 			because the utility library doesn't support semaphore.
 *
 *********************************************************************/
signed int atm_sensor_ifsys_sem_init( void **sem_p )
{ 
	return ATM_SENSOR_OK;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_ifsys_sem_get
 *
 *  Description: lock spinlock
 **  Arguments: In/Out void  *sem_p
 *              
 * Return Values:
 *  ATM_SENSOR_OK
 * Notes: This is empty function,
 * 			because the utility library doesn't support semaphore.
 *
 *********************************************************************/
signed int atm_sensor_ifsys_sem_get( void *sem_p )
{
	return ATM_SENSOR_OK;
}


/**********************************************************************
 *
 *  Name: atm_sensor_ifsys_sem_free
 *
 *  Description: unlock sem
 **  Arguments: In/Out) void  *sem_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes: This is empty function,
 * 			because the utility library doesn't support semaphore.
 *
 *********************************************************************/
signed int atm_sensor_ifsys_sem_free( void  *sem_p )
{
	return ATM_SENSOR_OK;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_ifsys_sem_exit
 *
 *  Description: init semaphore 
 **  Arguments: In/Out) void  *sem_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes: This is empty function,
 * 			because the utility library doesn't support semaphore.
 *
 *********************************************************************/
signed int atm_sensor_ifsys_sem_exit( void *sem_p )
{
	return ATM_SENSOR_OK;
}


/**********************************************************************
 * 
 *  Name: atm_sensor_ifsys_delay
 *
 *  Description: delay process
 **  Arguments: In)  unsigned int     usec
 * Return Values: NONE
 *
 * Notes:
 *
 **********************************************************************/
void atm_sensor_ifsys_delay( uint32_t usec )
{
	PRINT_INFO_1("START","[usec:%d]", usec );
	
	/*** delay ***/
	sns_ddf_delay( usec );
	
	PRINT_INFO_1("END","[usec:%d]", usec );

}


/**********************************************************************
 * 
 *  Name: atm_sensor_ifsys_get_timestamp
 *
 *  Description: get timestamp
 **  Arguments: In/Out)  unsigned int  *timestamp_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
**********************************************************************/
signed int atm_sensor_ifsys_get_timestamp(sns_ddf_time_t *timestamp_p)
{
	signed int ret = ATM_SENSOR_OK;
	
	PRINT_INFO_1("START","[timestamp_p:%p]",timestamp_p);
	
	/*** confirm to value of parameters ***/
	if( timestamp_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	/* get timestamp */
	*timestamp_p = sns_ddf_get_timestamp();
	
	PRINT_INFO_1("END","[timestamp:%d]", *timestamp_p );
	
	return ret;
}
