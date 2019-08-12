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
/******************* Murata Manufacturing Co.,Ltd. 2014 ***************
 *
 *Filename:   sns_dd_press_ZPA2326_ifi2c.c
 *
 *Purpose:    DDF I2C driver
 *
 *  $Author: mplcsds1 $
 *
 *  $Date: 2016/12/13 $
 *
 *  $Revision: #1 $
 * *******************************************************************/

#include "sns_dd_press_ZPA2326_ifsys.h"
#include "sns_dd_press_ZPA2326_core.h"
#include "sns_dd_press_ZPA2326_ifi2c.h"
#include "sns_dd_press_ZPA2326.h"		/* SMGR I/F */


/**********************************************************************
							DEFINE
**********************************************************************/
#define ATM_SENSOR_TRUE								0x01U
#define ATM_SENSOR_FALSE							0x00U

/**********************************************************************
							GLOBAL
**********************************************************************/
unsigned char gbI2C_INIT = ATM_SENSOR_FALSE;


/**********************************************************************
*
*  Name         : atm_sensor_ifi2c_init
*
*  Description  : Initialize I2C interface
*  Arguments    : In)  sns_ddf_handle_t 			port_handle	
*                 In)  sns_ddf_device_access_s 		*device_info
*  Return Value : Out) ret code defined in "ZPA2326_core.h"
*  
*  Notes        : 
*
**********************************************************************/
signed int atm_sensor_ifi2c_init( sns_ddf_handle_t *port_handle_p 
								, sns_ddf_device_access_s *device_info )
{
	sns_ddf_status_e status;
	
	PRINT_INFO_2("START","[port_handle:%p][device_info_p:%p]",\
									port_handle_p,device_info);

	/* Check if arguments is NULL */
	if( ( port_handle_p == NULL ) || ( device_info == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return ATM_SENSOR_I2C_NODATA_ERR ;
	}
	
	status = sns_ddf_open_port(port_handle_p, &(device_info->port_config) );
	if( status != SNS_DDF_SUCCESS )
	{
		PRINT_ERR_1("I2C OPEN FAIL","[status:%d]",status);
		return ATM_SENSOR_I2C_NODEV_ERR ;
	}
	
	gbI2C_INIT = ATM_SENSOR_TRUE;

	PRINT_INFO("END","");
	return ATM_SENSOR_I2C_OK ;
}




/**********************************************************************
*
*  Name         : atm_sensor_ifi2c_exit
*
*  Description  : Finalize I2C interface
*  Arguments    : In)  sns_ddf_handle_t 			port_handle	
*                 In)  
*  Return Value : Out) ret code defined in "ZPA2326_core.h"
*  
*  Notes        : 
*
**********************************************************************/
signed int atm_sensor_ifi2c_exit(sns_ddf_handle_t *port_handle_p )
{
	sns_ddf_status_e status;
	
	PRINT_INFO_1("START","[port_handle:%p]",port_handle_p);
	
	/* Check if arguments is NULL */
	if( port_handle_p == NULL )
	{
		PRINT_ERR("NODATA_ERR","");
		return ATM_SENSOR_I2C_NODATA_ERR ;
	}
	
	status = sns_ddf_close_port( *port_handle_p );
	if( status != SNS_DDF_SUCCESS )
	{
		PRINT_ERR_1("I2C CLOSE FAIL","[status:%d]",status);
		return ATM_SENSOR_I2C_NODEV_ERR ;
	}

	gbI2C_INIT = ATM_SENSOR_FALSE;

	PRINT_INFO("END","");
	return ATM_SENSOR_I2C_OK ;
}



/**********************************************************************
*
*  Name         : atm_sensor_ifi2c_data_init
*
*  Description  : Link I2C client
*  Arguments    : In)  void 	*client_pstruct
*                 In)  
*  Return Value : Out) ret code defined in "ZPA2326_core.h"
*
*  Notes        : 
*
**********************************************************************/
signed int atm_sensor_ifi2c_data_init( void *client_p )
{

	PRINT_INFO("START","");

	/* Check if arguments is NULL */
	if( client_p == NULL)
	{
		PRINT_ERR("NODATA_ERR","");
		return ATM_SENSOR_I2C_NODATA_ERR ;
	}

	if( gbI2C_INIT == ATM_SENSOR_FALSE)
	{
		PRINT_ERR("I2C UNINITIALIZED","");
		return ATM_SENSOR_I2C_NODEV_ERR ;
	}

	PRINT_INFO("END","");
	return ATM_SENSOR_I2C_OK ;
}


/**********************************************************************
*
*  Name         : atm_sensor_ifi2c_read
*
*  Description  : Read received data
*  Arguments    : In)  void 						*client_p
*                 In)  unsigned char				*buf_p
*                 In)  int							*len
*  Return Value : Out) ret code defined in "ZPA2326_core.h"
*  
*  Notes        : 
*
**********************************************************************/
signed int atm_sensor_ifi2c_read( void *client_p, unsigned char *buf_p, unsigned int len )
{
	sns_ddf_status_e status ;
	sns_ddf_handle_t *port_handle_p ;
	uint8_t read_len = 0U ;

	PRINT_INFO_3("START","[CLIENT:%p][buf_p:%p][len:%d]", client_p, buf_p, len ) ;
	/* Check if arguments is NULL */
	if( ( client_p == NULL) || ( buf_p == NULL ) || ( len == 0U ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return ATM_SENSOR_I2C_NODATA_ERR ;
	}
	
	port_handle_p = (sns_ddf_handle_t *)client_p ;

	status = sns_ddf_read_port( *port_handle_p, (uint16_t)buf_p[0], buf_p, (uint8_t)len, &read_len ) ;
	if( ( status != SNS_DDF_SUCCESS ) || ( read_len != len) )
	{

		PRINT_ERR_2("READ I2C TRANSFER FAIL","[status:%d][read_len:%d]",status,read_len);
		return ATM_SENSOR_I2C_READ_TRANSFER_ERR ;
	}


	PRINT_INFO("END","");
	return ATM_SENSOR_I2C_OK ;
}

/**********************************************************************
*
*  Name         : atm_sensor_ifi2c_write
*
*  Description  : Read received data
*  Arguments    : In)  void 						*client_p
*                 In)  unsigned char				*buf_p
*                 In)  int							*len
*  Return Value : Out) ret code defined in "ZPA2326_core.h"
*  
*  Notes        : 
*
**********************************************************************/
signed int atm_sensor_ifi2c_write( void *client_p, unsigned char *buf_p, unsigned int len )
{
	sns_ddf_status_e status ;
	sns_ddf_handle_t *port_handle_p ;
	uint8_t write_len = 0U ;

	PRINT_INFO_3("START","[CLIENT:%p][buf_p:%p][len:%d]", client_p, buf_p, len ) ;
	/* Check if arguments is NULL */
	if( ( client_p == NULL) || ( buf_p == NULL ) || ( len == 0U ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return ATM_SENSOR_I2C_NODATA_ERR ;
	}
	
	port_handle_p = (sns_ddf_handle_t *)client_p ;


	status = sns_ddf_write_port( *port_handle_p, (uint16_t)buf_p[0], &(buf_p[1]), (uint8_t)len, &write_len ) ;


	if( ( status != SNS_DDF_SUCCESS ) || ( write_len != len) )
	{
		PRINT_ERR_2("WRITE I2C TRANSFER FAIL","[status:%d][write_len:%d]",status,write_len);
		return ATM_SENSOR_I2C_WRITE_TRANSFER_ERR ;
	}
	

	PRINT_INFO("END","");
	return ATM_SENSOR_I2C_OK ;

}
