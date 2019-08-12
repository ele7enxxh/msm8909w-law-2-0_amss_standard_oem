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
/******* Murata Manufacturing Co,.Ltd. 2014 ***************************
 *
 * Filename : sns_dd_press_ZPA2326_core.c
 *
 * Purpose  : This module has implementation 
 *               of main function for PressureSensor device.
 *
 * $Author  : Noriko Ueno $
 * $Date    : 2013/12/20 17:00 $
 * $Reviison: 0.3 $
 *
 *********************************************************************/

/* core */
#include "sns_dd_press_ZPA2326_core.h"
#include "sns_dd_press_ZPA2326_ifi2c.h"
#include "sns_memmgr.h"

/**********************************************************************
							GLOBAL
 *********************************************************************/
tagATM_SENSOR_DATA   gATM_SENSOR_DATA = {
	.eSensor_status = e_Sensor_no_init,
};

/**
 * Treate as void pointer                                          
 *  because core doesn't know the type for semaphore and i2c client.
 */
void *gZPA2326_SEM_p = NULL; /* semaphore */
void *gZPA2326_I2C_CLIENT_p = NULL; /* I2C Client info */


/**********************************************************************
							Proto Type 
 *********************************************************************/
static signed int atm_sensor_set_init_reg( void );
static signed int atm_sensor_get_pressure( signed int *press_p );
static signed int atm_sensor_get_temperature( signed int *temp_p );
static signed int atm_sensor_update( unsigned char reg_addr,
									unsigned char values,
									unsigned char mask );
static signed int atm_sensor_stop_measurement( void );
static signed int atm_sensor_start_one_shot( void );
static signed int atm_sensor_start_odr( void );
static signed int atm_sensor_clear_fifo( unsigned char *buf_p );
static signed int atm_sensor_stop_autozero( void );
static signed int atm_sensor_set_autozero( void );
static signed int atm_sensor_disable_semfree( void );


/**********************************************************************
							FUCTION
 *********************************************************************/

/**********************************************************************
 *
 * 
 *  Name: atm_sensor_init
 *
 *  Description: init core 
 **  Arguments: In/Out) void  *client_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 **********************************************************************/
signed int atm_sensor_init( void *client_p )
{
	signed   int  ret  = ATM_SENSOR_OK;
	signed   int  ret2 = ATM_SENSOR_OK;
	unsigned int  len  = INIT_LENGTH;
	unsigned char buf[INIT_LENGTH];
	
	PRINT_INFO_1("START","[client_p:%p]",client_p);
	
	/**
	 * This function is executed only once 
	 * as long as the exit func is not called. 
	 */
	if(gATM_SENSOR_DATA.eSensor_status != e_Sensor_no_init )
	{
		PRINT_ERR_1("ERR_RE_INIT","[STATUS:%d]",gATM_SENSOR_DATA.eSensor_status);
		return ATM_SENSOR_INIT_FAIL;
	}
	
	/*** init global value ***/
	/**-- struct ATM_SENSOR_DATA member --**/
	gATM_SENSOR_DATA.eMeasureMode   = e_Sensor_measure_stop;
	gATM_SENSOR_DATA.eAutozero      = e_Sensor_dis_autozero;
	gATM_SENSOR_DATA.odr_time_count = ATM_SENSOR_ODR_TIM_CNT;
	gATM_SENSOR_DATA.odr_ave_count  = ATM_SENSOR_INIT_RES_CONF;
	
	/**-- init struct semaphore --**/
	gZPA2326_SEM_p = NULL;
	
	/**-- init Client_p --**/
	gZPA2326_I2C_CLIENT_p = NULL;
	
	/**-- inif buf --**/
	memset( buf, 0, INIT_LENGTH );
	
	/*** Confirm value ***/
	if( client_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	else
	{
		/* store i2c client info */
		gZPA2326_I2C_CLIENT_p = client_p;
		PRINT_INFO_1("COPY I2C CLIENT INFO POINTER","[gZPA2326_I2C_CLIENT_p:%p]",gZPA2326_I2C_CLIENT_p);
	}
	
	/*** init semaphore ***/
	ret = atm_sensor_ifsys_sem_init( &(gZPA2326_SEM_p) );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM_INFO INIT FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_2("SEM_INFO INIT OK","[ret:%d] [gZPA2326_SEM_p:%p]", ret, gZPA2326_SEM_p);
	
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]", ret);
	
	/*** Confirm I2C info ***/
	ret = atm_sensor_ifi2c_data_init( gZPA2326_I2C_CLIENT_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("CONFIRM I2C FAIL","[ret:%d]",ret);
		
		/**-- unlock semaphore --**/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("CONFIRM I2C FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("CONFIRM I2C OK","[ret:%d]", ret);
	
	/*** Enable Sensor ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		
		/**-- unlock semaphore --**/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("SENSOR ENABLE OK","[ret:%d]", ret);
	
	/*** Confirm Murata ID ***/
	buf[0] = ATM_SENSOR_DEVICE_ID;
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_INFO_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("READ FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("MURATA ID READ OK","[ret:%d]", ret);
	
	/*** Judge read ID ***/
	if( buf[0] != ATM_SENSOR_MURATA_DEV_ID )
	{
		PRINT_ERR_2("DIFFERENT MURATA SENSOR","[%d] [MURATA ID:%d]",buf[0],ATM_SENSOR_MURATA_DEV_ID);
		/*** Disable and free semaphore ***/
		ret = atm_sensor_disable_semfree();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret:%d]",ret);
		}
		PRINT_ERR_1("DIFFERENT MURATA SENSOR:DISABLE SEM FREE","[ret:%d]",ret);
		return ATM_SENSOR_NO_MURATA_DEV_ERR;
	}

	/*** sensor init setting ***/
	ret = atm_sensor_set_init_reg();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SENSOR INIT SET FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("SET_INIT","[ret:%d]",ret);
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE","[ret2:%d]",ret2);
	
	PRINT_VER("------------------------------------------------", "");
	PRINT_VER_3("[Sensor@] Version : ","[%d.%d.%02d]", MAJOR_VER, MINOR_VER,TEST_VER);
	PRINT_VER("------------------------------------------------", "");
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_exit
 *
 *  Description: exit core 
 **  Arguments: In/Out) In/Out atm_sensor_data_t  *atm_sensor_p
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_exit( void )
{
	signed int ret  = ATM_SENSOR_OK;
	signed int ret2 = ATM_SENSOR_OK;
	
	PRINT_INFO("START","");
	
	/*** SW Reset and Disable ***/
	ret = atm_sensor_software_reset();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SW RESET FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("SW RESET","[ret:%d]", ret);
	
	/*** lock semaphore for setting flow ***/
	ret2 = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret2);
	}
	PRINT_INFO_1("SEM GET","[ret2:%d]", ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("RETURN1","[ret:%d]", ret);
	
	/* Update status flag "no Init" */
	gATM_SENSOR_DATA.eSensor_status = e_Sensor_no_init;
	
	/*** unlock semaphore ***/
	ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("SEM FREE","[ret:%d]", ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("RETURN2","[ret:%d]", ret);
	
	/*** free semaphore buffer ***/
	ret2 = atm_sensor_ifsys_sem_exit( gZPA2326_SEM_p );
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM BUF FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("SEM BUF FREE","[ret2:%d]", ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_software_reset
 *
 *  Description: SW reset
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_software_reset( void )
{

#ifdef PRE_VER /* Previous ver */
	int i=0;
	signed   int  ret  = ATM_SENSOR_OK;
	signed   int  ret2 = ATM_SENSOR_OK;
	unsigned int  len = PRE_SW_RESET_LENGTH;
	unsigned char buf[PRE_SW_RESET_LENGTH+1];
	
	PRINT_INFO("START","");
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	memset( buf, 0, (PRE_SW_RESET_LENGTH+1) );
	
	/*** Enable Sensor ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/*** SW reset ***/
	/**-- REF_P_XL(0x08)=0x00 --**/
	buf[0] = ATM_SENSOR_REF_P_XL | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
	buf[1] = ATM_SENSOR_DFLT_REF_P_XL;
	/**-- REF_P_L(0x09)=0x00 --**/
	buf[2] = ATM_SENSOR_DFLT_REF_P_L;
	/**-- REG_P_H(0x0A)=0x00 --**/
	buf[3] = ATM_SENSOR_DFLT_REF_P_H;
	/**-- KREF_COMP(0x0B)=0x1F --**/
	buf[4] = ATM_SENSOR_DFLT_KREF_COMP;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("WRITE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("REF_P_x/KREF_COMP WRITE OK","[ret:%d]",ret);
	
	/**-- RES_CONF(0x10)=0x00 --**/
	buf[0] = ATM_SENSOR_RES_CONF;
	buf[1] = ATM_SENSOR_DFLT_RES_CONF;
	len = SET_RES_CONF_LENGTH;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("WRITE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("RES_COMF OK","[ret:%d]",ret);
	
	/**-- Write CTRL_REG1-2-3 --**/
	buf[0] = ATM_SENSOR_CTRL_REG1 | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */;
	/**-- CTRL_REG1(0x21)=0x3F --**/
	buf[1] = ATM_SENSOR_DFLT_CTRL_REG1;
	/**-- CTRL_REG2(0x22)=0x00 --**/
	buf[2] = ATM_SENSOR_DFLT_CTRL_REG2;
	/**-- CTRL_REG3(0x23)=0x00 --**/
	buf[3] = ATM_SENSOR_DFLT_CTRL_REG3;
	len = PRE_SW_RESET_LENGTH - 1;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK ) /* sensor write error */
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("WRITE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("CTRL_REG1/2/3 OK","[ret:%d]",ret);
	
	/**-- THS_P_LOW_REG(0x25)=0x00 --**/
	buf[0] = ATM_SENSOR_THS_P_LOW_REG | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
	buf[1] = ATM_SENSOR_DFLT_THS_L_HIGH_REG;
	/**-- THS_P_HIGH_REG(0x26)=0xFF --**/
	buf[2] = ATM_SENSOR_DFLT_THS_P_HIGH_REG;
	len = SET_THS_P_LENGTH;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR__1("WRITE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("WRITE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("THS_P_HIGH/LOW_REG","[ret:%d]",ret);

	/* FIFO_MAX is 16*/
	/*** Read Register ***/
	for( i=0; i<FIFO_MAX ; i++ )
	{
		PRINT_INFO_1("CLEAR FIFO","[i:%d]",i);
		/**-- PRESS_OUT_XL(0x28) FIFO_MAX(16) count --**/
		buf[0] = ATM_SENSOR_PRESS_OUT_XL | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
		/**-- PRESS_OUT_L(0x29) FIFO_MAX(16) count --**/
		/**-- PRESS_OUT_H(0x2A) FIFO_MAX(16) count --**/
		len = GET_PRESS_LENGTH;
		ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
		if( ret != ATM_SENSOR_OK ) /* sensor read error */
		{
			PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
			/*** Disable and free semaphore ***/
			ret2 = atm_sensor_disable_semfree();
			if( ret2 != ATM_SENSOR_OK )
			{
				PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
			}
			PRINT_INFO_1("READ FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
			return ret;
		}
		PRINT_INFO_4("READ PRESS OUT","[i:%d][0x%02X,0x%02X,0x%02X]",i,buf[2],buf[1],buf[0]);
	}/* READ 16 times */
	
	/**-- TEMP_OUT_L(0x2B) 1 count --**/
	buf[0] = ATM_SENSOR_TEMP_OUT_L | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
	/**-- TEMP_OUT_H(0x2C) 1 count --**/
	len = GET_TEMP_LENGTH;
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK ) /* sensor read error */
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("READ FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("READ PRESS OUT","[ret:%d]",ret);
	
	/*** Set init register ***/
	ret = atm_sensor_set_init_reg();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET INIT VALUE FAIL","[ret:%d]",ret);
	}
	else
	{
		PRINT_INFO_1("SET INIT VALUE OK","[ret:%d]",ret);
		/*** Clear flg ***/
		gATM_SENSOR_DATA.eMeasureMode = e_Sensor_measure_stop;
		gATM_SENSOR_DATA.eAutozero = e_Sensor_dis_autozero;
	}
	PRINT_INFO_3("FLG","[ret:%d] [measure:%d] [autozero:%d]",ret,gATM_SENSOR_DATA.eMeasureMode,gATM_SENSOR_DATA.eAutozero);
	
		/**-- CTRL_REG0(0x20)=0x00 --**/
	buf[0] = ATM_SENSOR_CTRL_REG0;
	buf[1] = ATM_SENSOR_DFLT_CTRL_REG0;
	len = SW_RESET_LENGTH;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK ) /* sensor write error */
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("WRITE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("CTRL_REG0 OK","[ret:%d]",ret);
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE OK","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;


#else /******************* FINAL VERSION ******************/

	signed   int ret  = ATM_SENSOR_OK;
	signed   int ret2 = ATM_SENSOR_OK;
	unsigned int len  = SW_RESET_LENGTH;
	unsigned char buf[SW_RESET_LENGTH+1];
	
	PRINT_INFO("START","");
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	memset( buf, 0, (SW_RESET_LENGTH+1) );
	
	/*** Enable Sensor ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/*** SW reset ***/
	/**-- CTRL_REG2(0x22) SWRESET[Bit2]=1 --**/
	buf[0] = ATM_SENSOR_CTRL_REG2;
	buf[1] = ATM_SENSOR_SW_RESET;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("WRITE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ATM_SENSOR_RESET_WRITE_ERR;
	}
	PRINT_INFO_1("WRITE OK","[ret:%d]",ret);
	
	/*** Set init register ***/
	ret = atm_sensor_set_init_reg();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET INIT VALUE FAIL","[ret:%d]",ret);
	}
	else
	{
		PRINT_INFO_1("SET INIT VALUE OK","[ret:%d]",ret);
		/*** Clear flg ***/
		gATM_SENSOR_DATA.eMeasureMode = e_Sensor_measure_stop;
		gATM_SENSOR_DATA.eAutozero = e_Sensor_dis_autozero;
	}
	PRINT_INFO_3("FLG","[ret:%d] [measure:%d] [autozero:%d]",ret,gATM_SENSOR_DATA.eMeasureMode,gATM_SENSOR_DATA.eAutozero);
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
	
#endif /* -DPRE_SW */
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_measure
 *
 *  Description: measure sensor
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_measure( E_SENSOR_MEASURE_MODE  measure_mode )
{
	signed int ret  = ATM_SENSOR_OK;
	signed int ret2 = ATM_SENSOR_OK;
	
	/**** debug ****/
	unsigned char buf[MEASURE_LENGTH];
	memset( buf, 0, MEASURE_LENGTH );
	
	PRINT_INFO_1("START","[measure_mode:%d]",measure_mode);
	
	/**
	 * measure_mode is the enum type. 
	 * Need not to confirm whether the range.
	 */
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	/**
	 * Confirm the meausrement mode
	 * measure_mode = 0: Stop the measurement.
	 * measure_mode = 1: The One shot measurement.
	 * meausre_mode = 2: The ODR measurement.
	 */
	switch( measure_mode )
	{
	case e_Sensor_measure_stop: /* 0 */
		/**-- Stop the measurement mode --**/
		ret = atm_sensor_stop_measurement();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("STOP MEASUREMENT MODE FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("STOP MEASUREMENT MODE","[ret:%d]",ret);
		break;
	case e_Sensor_oneshot: /* 1 */
		/**-- One shot mode --**/
		ret = atm_sensor_start_one_shot();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("START ONE SHOT MEASURE MODE FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("ONE SHOT MEASUREMENT MODE","[ret:%d]",ret);
		break;
	case e_Sensor_odr: /* 2 */
		/**-- ODR mode --**/
		/**
		 * Can't run to enable the measurement mode once more 
		 * during measurement mode 
		 */
		if( gATM_SENSOR_DATA.eMeasureMode == e_Sensor_odr )
		{
			ret = ATM_SENSOR_DURING_MEASUREMENT_ERR;
			PRINT_ERR_1("ALREADY START MEASUREMENT MODE FAIL","[ret:%d]",ret);
			break;
		}
		/** setting measurement mode **/
		ret = atm_sensor_start_odr();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("START ODR MEASUREMENT MODE FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("START ODR MEASUREMENT MODE","[ret:%d]",ret);
		break;
	default:
		ret = ATM_SENSOR_INVALID;
		PRINT_ERR_1("MODE INVALID","[ret:%d]",ret);
		break;
	}
	
	/*** Update flag if return code is 0. ***/
	if( ret == ATM_SENSOR_OK )
	{
		gATM_SENSOR_DATA.eMeasureMode = measure_mode;
		PRINT_INFO_1("UPDATE SENSOR_FLG","[Mode:%d]", gATM_SENSOR_DATA.eMeasureMode);
	}
	
	/*** unlock semaphore ***/
	ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("SEM FREE OK","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
*
*
*  Name: atm_sensor_autozero
*
*  Description: Set Autozero mode
**  Arguments: void
*              
* Return Values: ret code defined in "ZPA2326_core.h"
*
* Notes:
*
**********************************************************************/
signed int atm_sensor_autozero( E_SENSOR_AUTOZERO autozero_mode )
{
	signed int ret  = ATM_SENSOR_OK;
	signed int ret2 = ATM_SENSOR_OK;
	
	PRINT_INFO_2("START","[measure:%d] [autozero:%d]",gATM_SENSOR_DATA.eMeasureMode,autozero_mode);
	
	/**
	 * measure_mode is the enum type. 
	 * Need not to confirm whether the range.
	 */
	
	/*** Whether Measurement mode or not ***/
	if( gATM_SENSOR_DATA.eMeasureMode != e_Sensor_measure_stop )
	{
		PRINT_ERR("DURING MEASUREMENT MODE FAIL","");
		return ATM_SENSOR_DURING_MEASUREMENT_ERR;
	}
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	/*** Sensor Enable ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE OK","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/**
	 * Confirm the meausrement mode
	 * autozero_mode = 0: Stop autozero mode.
	 * autozero_mode = 1: Set autozero mode.
	 */
	/***  Autozero setting ***/
	switch( autozero_mode )
	{
	case e_Sensor_dis_autozero: /* stop autozero mode */
		ret = atm_sensor_stop_autozero();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("STOP AUTOZERO FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("STOP AUTOZERO","[ret:%d]",ret);
		break;
	case e_Sensor_ena_autozero: /* enable autozero mode */
		/**
		 * Can't run to enable the measurement mode once more 
		 * during measurement mode 
		 */
		if( gATM_SENSOR_DATA.eAutozero == e_Sensor_ena_autozero )
		{
			ret = ATM_SENSOR_ALREADY_SET_AUTOZERO_ERR;
			PRINT_ERR_1("ALREADY SET AUTOZERO MODE","[ret:%d]",ret);
			break;
		}
		
		/* set autozero */
		ret = atm_sensor_set_autozero();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SET AUTOZERO FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("SET AUTOZERO","[ret:%d]",ret);
		break;
	default:
		ret = ATM_SENSOR_INVALID;
		PRINT_ERR_1("FLG INVALID","[ret:%d]",ret);
		break;
	}
	
	/*** Update flag if return code is 0. ***/
	if( ret == ATM_SENSOR_OK )
	{
		gATM_SENSOR_DATA.eAutozero = autozero_mode;
		PRINT_INFO_1("UPDATE SENSOR_FLG","[FLG:%d]", gATM_SENSOR_DATA.eAutozero);
	}
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE OK","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
*
* 
*  Name: atm_sensor_set_parameter
*
*  Description: set sensor parameter
*				(REF_Pressure, Thread interrupt, KREF_COMP)
**  Arguments: 
*              In) unsigned char       reg_addr
*              In) unsigned char       values
*
* Return Values: ret code defined in "ZPA2326_core.h"
* 
* Notes:
*
**********************************************************************/
signed int atm_sensor_set_parameter( unsigned char reg_addr,
										unsigned char values )
{
	signed int ret  = ATM_SENSOR_OK;
	signed int ret2 = ATM_SENSOR_OK;
	unsigned char mask = ATM_SENSOR_MASK;
	
	PRINT_INFO_2("START","[reg_addr:0x%02X] [values:0x%02X]",reg_addr, values);
	
	/*** Whether Measurement mode or not ***/
	if( gATM_SENSOR_DATA.eMeasureMode != e_Sensor_measure_stop )
	{
		PRINT_ERR_1("DURING MEASUREMENT MODE FAIL","[MODE:%d]",gATM_SENSOR_DATA.eMeasureMode);
		return ATM_SENSOR_DURING_MEASUREMENT_ERR;
	}
	
	/*** Confirm reg_addr and set mask ***/
	if( (reg_addr == ATM_SENSOR_REF_P_XL) ||
		(reg_addr == ATM_SENSOR_REF_P_L)  ||
		(reg_addr == ATM_SENSOR_REF_P_H)  ||
		(reg_addr == ATM_SENSOR_THS_P_LOW_REG) ||
		(reg_addr == ATM_SENSOR_THS_P_HIGH_REG) )
	{
		mask = ATM_SENSOR_MASK;
		PRINT_INFO_3("REF_P_x/THS_P_x_REG","[mask:0x%02X] [reg_addr:0x%02X] [values:0x%02X]",mask,reg_addr, values);
	}
	else if( reg_addr == ATM_SENSOR_KREF_COMP )
	{
		if( values > MAX_KREF_COMP )
		{
			PRINT_ERR_1("KREF_COMP Value FAIL","[values:0x%02X]",values);
			return ATM_SENSOR_INVALID;
		}
		mask = ATM_SENSOR_MASK_KREF_COMP;
		PRINT_INFO_3("KREF_COMP","[mask:0x%02X] [reg_addr:0x%02X] [values:0x%02X]",mask,reg_addr, values);
	}
	else
	{
		PRINT_ERR_1("INVALID REG_ADDR","[values:0x%02X]",values);
		return ATM_SENSOR_INVALID;
	}
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	/*** Sensor Enable ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("SENSOR ENABLE OK","[ret:%d]",ret);
	
	/*** Set parameter ***/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET PARAM FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("SET PARAM","[ret:%d]",ret);
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
*
* 
*  Name: atm_sensor_set_odr_time_count
*
*  Description: set sensor ODR time count
**  Arguments: 
*              In) unsigned char       reg_addr
*              In) unsigned char       values
*
* Return Values: ret code defined in "ZPA2326_core.h"
*
* Notes:
*
**********************************************************************/
signed int atm_sensor_set_odr_time_count( unsigned char values )
{
	signed   int  ret      = ATM_SENSOR_OK;
	signed   int  ret2     = ATM_SENSOR_OK;
	unsigned int  len      = SET_ODR_TIM_CNT_LENGTH;
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG3;
	unsigned char mask     = ATM_SENSOR_MASK_ODR_TIM_CNT;
	unsigned char buf[SET_ODR_TIM_CNT_LENGTH];
	E_SENSOR_STATUS	eCurrent_status = e_Sensor_disable;
	
	PRINT_INFO_1("START","[values:0x%02X]",values);

	/*** confirm value ***/
	if( ( MIN_ODR_TCNT > values ) || ( MAX_ODR_TCNT < values ) )
	{
		PRINT_ERR("OUT OF RANGE FAIL","");
		return ATM_SENSOR_INVALID;
	}
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	/*** Init buf **/
	memset( buf, 0, SET_ODR_TIM_CNT_LENGTH );
	
	/*** remember sensor status ***/
	eCurrent_status = gATM_SENSOR_DATA.eSensor_status;
	PRINT_INFO_1("STATE","[values:%d]",eCurrent_status);
	
	/*** Sensor Enable ***/
	PRINT_INFO("ENABLE","");
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/* CTRL_REG_3(0x23) ODR2-1-0 */
	/*** Write ODR time count ***/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET ODR TIME COUNT FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("SET ODR TIME COUNT FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("SET ODR TIME OK","[ret:%d]",ret);
	
	/*** Read value ***/
	buf[0] = reg_addr;
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK ) /* sensor enable error */
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
	}
	else
	{
		gATM_SENSOR_DATA.odr_time_count = buf[0] & ATM_SENSOR_MASK_ODR_TIM_CNT;
		PRINT_INFO_2("READ OK","[ret:%d] [ODR TIME COUNT:0x%02X]",ret,gATM_SENSOR_DATA.odr_time_count);
	}
	
	/*** If sensor is measurement mode, it doesn't set "disable" ***/
	if(eCurrent_status == e_Sensor_enable)
	{
		PRINT_INFO("NOT DISABLE","");
		/**-- unlock semaphore --**/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("SEM FREE","[ret2:%d]",ret2);
	}
	else
	{
		PRINT_INFO("DISABLE","");
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("DISABLE SEM FREE","[ret2:%d]",ret2);
	}
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_set_res_conf
 *
 *  Description: set sensor average count
 **  Arguments: 
 *              In) unsigned char       values
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_set_res_conf( unsigned char values )
{
	signed   int  ret      = ATM_SENSOR_OK;
	signed   int  ret2     = ATM_SENSOR_OK;
	unsigned int  len      = SET_RES_CONF_LENGTH;
	unsigned char reg_addr = ATM_SENSOR_RES_CONF;
	unsigned char mask     = ATM_SENSOR_MASK_RES_CONF;
	unsigned char buf[SET_RES_CONF_LENGTH];
	
	PRINT_INFO_1("START","[values:0x%02X]",values);

	/*** Whether Measurement mode or not ***/
	if( gATM_SENSOR_DATA.eMeasureMode != e_Sensor_measure_stop )
	{
		PRINT_ERR_1("DURING MEASUREMENT MODE FAIL","[MODE:%d]",gATM_SENSOR_DATA.eMeasureMode);
		return ATM_SENSOR_DURING_MEASUREMENT_ERR;
	}
	
	/*** confirm value ***/
	if( values > MAX_ODR_AVE_CNT )
	{
		PRINT_ERR("OUT OF RANGE FAIL","");
		return ATM_SENSOR_INVALID;
	}
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	memset( buf, 0, SET_ODR_TIM_CNT_LENGTH );
	
	/*** Sensor Enable ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/*** write ODR average count ***/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET ODR AVE FAIL","[ret:%d]",ret);
		/*** Disable and free semaphore ***/
		ret2 = atm_sensor_disable_semfree();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("SET ODR AVE FAIL:DISABLE SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("SET ODR AVE OK","[ret:%d]",ret);
	
	/*** Read value ***/
	buf[0] = ATM_SENSOR_RES_CONF;
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK ) /* sensor enable error */
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
	}
	else
	{
		gATM_SENSOR_DATA.odr_ave_count = buf[0];
		PRINT_INFO_2("READ OK","[ret:%d] [ODR AVE:0x%02X]",ret,gATM_SENSOR_DATA.odr_ave_count);
	}
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_get_pressure_temperature
 *
 *  Description: init core 
 **  Arguments: 
 *              In/Out) signed int      *press_p
 *              In/Out) signed int      *temp_p
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_get_pressure_temperature( signed int *press_p, 
												signed int *temp_p )
{
	signed int ret  = ATM_SENSOR_OK;
	signed int ret2 = ATM_SENSOR_OK;
	
	PRINT_INFO_2("START","[press_p:%p] [temp_p:%p]",press_p, temp_p);
	
	/*** confirm to value of parameters ***/
	if( (press_p == NULL) || (temp_p == NULL) )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	/*** Get pressure data ***/
	ret = atm_sensor_get_pressure( press_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("GET PRESS FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )	
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("GET PRESS","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("GET PRESS OK","[ret:%d]",ret);
	
	/*** Get temperature data ***/
	ret = atm_sensor_get_temperature( temp_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("GET TEMP FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("GET TEMP","[ret:%d]",ret);
	
	/*** unlock semaphore ***/
	ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("SEM FREE","[ret:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_parameter
 *
 *  Description: get sensor parameter 
 *				(REF_PRESSURE, Thread interrupt, KREF_COMP, Status)
 **  Arguments: 
 *              In/Out) unsigned char     *buf_p
 *              In) unsigned char       reg_addr
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *				This function must not run during measurement.
 **********************************************************************/
signed int atm_sensor_get_parameter( unsigned char *buf_p,
										unsigned char reg_addr )
{
	signed   int  ret  = ATM_SENSOR_OK;
	signed   int  ret2 = ATM_SENSOR_OK;
	unsigned int  len  = GET_PARAM_LENGTH;
	unsigned char buf[GET_PARAM_LENGTH];
	
	PRINT_INFO_2("START","[buf_p:%p] [reg_addr:0x%02X]",buf_p, reg_addr);
	
	/*** confirm to value of parameters ***/
	if( buf_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	/*** Whether Measurement mode or not ***/
	if( gATM_SENSOR_DATA.eMeasureMode != e_Sensor_measure_stop )
	{
		PRINT_ERR_1("DURING MEASUREMENT MODE FAIL","[MODE:%d]",gATM_SENSOR_DATA.eMeasureMode);
		return ATM_SENSOR_DURING_MEASUREMENT_ERR;
	}
	
	/*** confirm reg_addr ***/
	if( (reg_addr != ATM_SENSOR_REF_P_XL)       &&
		(reg_addr != ATM_SENSOR_REF_P_L)        &&
		(reg_addr != ATM_SENSOR_REF_P_H)        &&
		(reg_addr != ATM_SENSOR_THS_P_LOW_REG)  &&
		(reg_addr != ATM_SENSOR_THS_P_HIGH_REG) && 
		(reg_addr != ATM_SENSOR_KREF_COMP)      &&
		(reg_addr != ATM_SENSOR_STATUS_REG))
	{
		PRINT_ERR("INVALID REG_ADDR FAIL","");
		return ATM_SENSOR_INVALID;
	}
	
	/*** lock semaphore for setting flow ***/
	ret = atm_sensor_ifsys_sem_get( gZPA2326_SEM_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM GET FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SEM GET OK","[ret:%d]",ret);
	
	memset( buf, 0, GET_PARAM_LENGTH );
	
	/*** Sensor Enable ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		/*** unlock semaphore ***/
		ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ENABLE FAIL:SEM FREE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/*** Read data ***/
	buf[0] = reg_addr;
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK ) /* read error */
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
	}
	else
	{
		buf_p[0] = buf[0]; /* return get values */
		PRINT_INFO_2("GET PARAMETER VALUE","[ret:%d] [value:0x%02X]",ret,buf_p[0]);
	}
	
	/*** Disable and free semaphore ***/
	ret2 = atm_sensor_disable_semfree();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE SEM FREE FAIL","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE SEM FREE","[ret2:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_sensor_status
 *
 *  Description: get sensor status
 *				( enable, disable, no Init )
 **  Arguments: 
 *               Void
 *
 *
 * Return Values:
 *                gATM_SENSOR_DATA.eSensor_status
 * Notes:
 *
 **********************************************************************/
E_SENSOR_STATUS atm_sensor_get_sensor_status( void )
{
	PRINT_INFO_1("SENSOR STATUS","[STATUS:%d]",gATM_SENSOR_DATA.eSensor_status);
	return gATM_SENSOR_DATA.eSensor_status;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_measure_mode
 *
 *  Description: get sensor measure mode
 *				( stop, one-shot, ODR )
 **  Arguments: 
 *               Void
 *
 *
 * Return Values:
 *                gATM_SENSOR_DATA.eMeasureMode
 * Notes:
 *
 **********************************************************************/
E_SENSOR_MEASURE_MODE atm_sensor_get_measure_mode( void )
{
	PRINT_INFO_1("SENSOR MEASUREMENT MODE","[Mode:%d]",gATM_SENSOR_DATA.eMeasureMode);
	return gATM_SENSOR_DATA.eMeasureMode;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_autozero_mode
 *
 *  Description: get sensor autozero mode
 *				( disable autozero, autozero )
 **  Arguments: 
 *               Void
 *
 *
 * Return Values:
 *                gATM_SENSOR_DATA.eAutozero
 * Notes:
 *				This function must not run during measurement.
 **********************************************************************/
E_SENSOR_AUTOZERO atm_sensor_get_autozero_mode( void )
{
	PRINT_INFO_1("SENSOR Autozero","[Autozero Mode:%d]",gATM_SENSOR_DATA.eAutozero);
	return gATM_SENSOR_DATA.eAutozero;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_odr_time_count
 *
 *  Description: get sensor time interval
 *
 **  Arguments: 
 *               Void
 *
 *
 * Return Values:
 *                gATM_SENSOR_DATA.odr_time_count
 * Notes:
 *
***********************************************************************/
unsigned char atm_sensor_get_odr_time_count( void )
{
	PRINT_INFO_1("SENSOR ODR TIME CNT","[TIME CNT:0x%02X]",gATM_SENSOR_DATA.odr_time_count);
	return gATM_SENSOR_DATA.odr_time_count;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_odr_ave_count
 *
 *  Description: get sensor odr average count (RES_CONF)
 *
 **  Arguments: 
 *               Void
 *
 *
 * Return Values:
 *                gATM_SENSOR_DATA.odr_count
 * Notes:
 *
 ***********************************************************************/
unsigned char atm_sensor_get_odr_ave_count( void )
{
	PRINT_INFO_1("SENSOR ODR AVE CNT","[AVE CNT:0x%02X]",gATM_SENSOR_DATA.odr_ave_count);
	return gATM_SENSOR_DATA.odr_ave_count;
}


/**********************************************************************
 *
 * 
 *  Name: atm_sensor_enable
 *
 *  Description: enable sensor
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
signed int atm_sensor_enable( void )
{
	signed int ret = ATM_SENSOR_OK;
	
	/*** CTRL_REG0(0x20) register ENABLE[Bit 1] = 1 ***/
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG0;
	unsigned char values   = ATM_SENSOR_ENA_CTRL0; 
	unsigned char mask     = ATM_SENSOR_MASK_ENA_CTRL0;
	
	PRINT_INFO("START","");

	/*** update data ***/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		return ret;
	}
	
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	/*** enable flg ***/
	gATM_SENSOR_DATA.eSensor_status = e_Sensor_enable;
	PRINT_INFO_1("UPDATE FLG","[STATUS:%d]",gATM_SENSOR_DATA.eSensor_status);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
	
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_disable
 *
 *  Description: disable sensor
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 **********************************************************************/
signed int atm_sensor_disable( void )
{
	signed int ret = ATM_SENSOR_OK;
	
	/*** CTRL_REG0(0x20) register ENABLE[Bit 1] = 0 ***/
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG0;
	unsigned char values   = ATM_SENSOR_DIS_CTRL0;
	unsigned char mask     = ATM_SENSOR_MASK_ENA_CTRL0;

	PRINT_INFO("START","");
	
	/*** update data ***/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("DISABLE OK","[ret:%d]",ret);
	/*** disable flg ***/
	gATM_SENSOR_DATA.eSensor_status = e_Sensor_disable;
	PRINT_INFO_1("UPDATE FLG","[STATUS:%d]",gATM_SENSOR_DATA.eSensor_status);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
 
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_pressure
 *
 *  Description: get pressure data 
 **  Arguments: 
 *              In/Out) signed int         *press_p
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_get_pressure( signed int *press_p )
{
	signed   int  ret = ATM_SENSOR_OK;
	signed   int  pressure_value;
	unsigned int  len = GET_PRESS_LENGTH;
	unsigned char buf[GET_PRESS_LENGTH];
	
	PRINT_INFO_1("START","[press_p:%p]",press_p);
	
	/*** confirm to value of parameters ***/
	if( press_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	memset( buf, 0, GET_PRESS_LENGTH );
	
	/*** Read Pressure data PRESS_OUT_XL/L/H ***/
	buf[0] = ATM_SENSOR_PRESS_OUT_XL | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_4("READ OK","[ret:%d] [buf[2]:%d][buf[1]:%d][buf[0]:%d]",ret,buf[2],buf[1],buf[0]);
	
	/*** Get values ***/
	if( (buf[2] & TWOS_COMPLEMENT) == TWOS_COMPLEMENT ) /* value is minus(-) */
	{
		PRINT_INFO("VALUE IS MINUS","");
		pressure_value =( 0xFF000000 | ( (buf[2] << 16) | (buf[1] << 8) | buf[0] ) );
	}
	else /* value is plus(+) */
	{
		PRINT_INFO("VALUE IS PLUS","");
		pressure_value = ( (buf[2] << 16) | (buf[1] << 8) | buf[0] );
	}
	PRINT_INFO_1("GET PRESS","[press_value:%d]",pressure_value);
	
	/*** If Autozero mode is disable and get value is 0 ***/
	if( gATM_SENSOR_DATA.eAutozero == e_Sensor_dis_autozero ) 
	{
		PRINT_INFO("GET PRESS:DISABLE AUTOMODE","");
		/* pressure value == 0 is exception */
		if( pressure_value == 0 )
		{
			ret = ATM_SENSOR_NO_DATA;
			PRINT_INFO_2("GET PRESS ZERO","[ret:%d] [pres value:%d]",ret,pressure_value);
		}
	}
	
	*press_p = pressure_value;
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_get_temperature
 *
 *  Description: get temperature data 
 **  Arguments: 
 *              In/Out) signed int         *temp_p
 *              
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_get_temperature( signed int *temp_p )
{
	signed   int  ret = ATM_SENSOR_OK;
	signed   int  temperature_value;
	unsigned int  len = GET_TEMP_LENGTH;
	unsigned char buf[GET_TEMP_LENGTH];
	
	PRINT_INFO_1("START","[temp_p:%p]",temp_p);
	
	/*** confirm to value of parameters ***/
	if( temp_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	memset( buf, 0, GET_TEMP_LENGTH );
	
	/*** read Temperature data TEMP_OUT_L/H ***/
	buf[0] = ATM_SENSOR_TEMP_OUT_L | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_3("READ OK","[ret:%d] [buf[1]:%d][buf[0]:%d]",ret,buf[1],buf[0]);
	
	/*** get value ***/
	temperature_value =( (buf[1] << 8) | buf[0] );
	PRINT_INFO_1("GET TEMP","[temperature_value:%d]",temperature_value);
	
	*temp_p = temperature_value;
	PRINT_INFO_1("END","[ret:%d]",ret);
	
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_update
 *
 *  Description: register update 
 **  Arguments:
 *				In)     unsigned char      reg_addr
 *				In)     unsigned char      values
 *				In)     unsigned char      mask
 *				In)     int                len
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_update( unsigned char reg_addr,
									unsigned char values,
									unsigned char mask )
{
	signed   int  ret = ATM_SENSOR_OK;
	unsigned int  len = UPDATE_LENGTH;
	unsigned char buf[UPDATE_LENGTH+1];
	
	PRINT_INFO_3("START","[reg_addr:0x%02X] [values:0x%02X] [mask:0x%02X]",reg_addr, values, mask);

	/*** Confirm reg_addr ***/
	if( (reg_addr != ATM_SENSOR_REF_P_XL ) && ( reg_addr != ATM_SENSOR_REF_P_L ) &&
		(reg_addr != ATM_SENSOR_REF_P_H ) && ( reg_addr != ATM_SENSOR_KREF_COMP ) &&
		(reg_addr != ATM_SENSOR_RES_CONF ) && ( reg_addr != ATM_SENSOR_CTRL_REG0 ) &&
		(reg_addr != ATM_SENSOR_CTRL_REG1 ) && ( reg_addr != ATM_SENSOR_CTRL_REG2 ) &&
		(reg_addr != ATM_SENSOR_CTRL_REG3 ) && ( reg_addr != ATM_SENSOR_THS_P_LOW_REG ) &&
		(reg_addr != ATM_SENSOR_THS_P_HIGH_REG )
	)
	{
		PRINT_INFO("OUT OF RANGE TO WRITE","");
		return ATM_SENSOR_INVALID;
	}
	
	memset( buf, 0, (UPDATE_LENGTH + 1) );
	
	/*** read value ***/
	buf[0] = reg_addr;
	
	/*** i2c read ***/
	ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("READ OK","[ret:%d]",ret);
	PRINT_INFO_1("UPDATE_READ","[buf:0x%02X]",buf[0]);
	
	/*** compare address ***/
	buf[1] = ( buf[0] & (~mask) ); /* clear data */
	buf[1] = ( buf[1] | values );
	buf[0] = reg_addr; /* once more set register address */
	
	PRINT_INFO_2("UPDATE_WRITE!!","[buf0:0x%02X][buf1:0x%02X]",buf[0],buf[1]);
	
	/* write value */
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("WRITE","[ret:%d]",ret);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}

/**********************************************************************
 *
 *
 *  Name: atm_sensor_set_init_reg
 *
 *  Description: get sensor information address
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_set_init_reg( void )
{
	signed   int  ret = ATM_SENSOR_OK;
	unsigned char reg_addr;
	unsigned char values;
	unsigned char mask;
	
	PRINT_INFO("START","");
	
	/*** Set init register ***/
	/**-- ODR time count --**/
	/* CTRL_REG_3(0x23) ODR2-1-0[0x50] 5Hz */
	reg_addr = ATM_SENSOR_CTRL_REG3; 
	values   = ATM_SENSOR_ODR_TIM_CNT;
	mask     = ATM_SENSOR_MASK_ODR_TIM_CNT;
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET ODR TIME CNT FAIL","[ret:%d]",ret);
		return ret;
	}
	
	PRINT_INFO_1("SET ODR TIME CNT OK","[ret:%d]",ret);
	/**-- Update flg --**/
	gATM_SENSOR_DATA.odr_time_count = ATM_SENSOR_ODR_TIM_CNT;
	PRINT_INFO_1("UPDATE FLG", "[ODR TIME CNT:0x%02X]",gATM_SENSOR_DATA.odr_time_count);
	
	/**-- ODR ave --**/
	/* RES_CONF(0x10) AVGT/P0[0x00] 1count */
	reg_addr = ATM_SENSOR_RES_CONF;
	values = ATM_SENSOR_INIT_RES_CONF;
	mask = ATM_SENSOR_MASK_RES_CONF;
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET ODR AVE CNT FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SET ODR AVE CNT OK","[ret:%d]",ret);
	/**-- Update flg --**/
	gATM_SENSOR_DATA.odr_ave_count = ATM_SENSOR_INIT_RES_CONF;
	PRINT_INFO_1("UPDATE FLG", "[ODR AVE CNT:%d]",gATM_SENSOR_DATA.odr_ave_count);
	
	/**-- KREF_COMP --**/
	/* KREF_COMP(0x0B) KREF_COMP[0x1F] */
	reg_addr = ATM_SENSOR_KREF_COMP; 
	values = ATM_SENSOR_DFLT_KREF_COMP;
	mask = ATM_SENSOR_MASK_KREF_COMP;
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET KREF_COMP FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("SET KREF_COMP","[ret:%d]",ret);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_stop_measurement
 *
 *  Description: stop measurement mode
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_stop_measurement( void )
{
	signed   int  ret  = ATM_SENSOR_OK;
	signed   int  ret2 = ATM_SENSOR_OK;
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG3;
	unsigned char mask = ATM_SENSOR_MASK_START_ODR;
	unsigned char values = ATM_SENSOR_STOP_ODR;

	PRINT_INFO("START","");
	/*** CTRL_REG3(0x23) ENABLE_MEAS[Bit7] = 0 ***/
	PRINT_INFO("STOP MEASUREMENT","");
	
	/* Wirte to stop the measurement */
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("STOP MEASURENT FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("STOP MEASURENT","[ret:%d]",ret);
	
	/*** Disable Sensor ***/
	ret2 = atm_sensor_disable();
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE FAIL","[ret:%d]",ret2);
	}
	PRINT_INFO_1("DISABLE","[ret:%d]",ret2);
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
	
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_start_one_shot
 *
 *  Description: Start one shot mode
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_start_one_shot( void )
{
	signed   int  ret = ATM_SENSOR_OK;
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG0;
	unsigned char mask = ATM_SENSOR_MASK_ONE_SHOT;
	unsigned char values = ATM_SENSOR_START_ONE_SHOT;
	unsigned char buf[GET_PRESS_LENGTH];
	
	PRINT_INFO("START","");
	/* CTRL_REG0(0x20) ONE_SHOT[Bit0] = 1 */
	PRINT_INFO("ONE SHOT MEASUREMENT","");
	
	memset( buf, 0, GET_PRESS_LENGTH );
	/*** Enable sensor ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	/* Clear FIFO */
	ret = atm_sensor_clear_fifo( buf );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("FIFO CLEAR FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("FIFO CLEAR OK","[ret:%d]",ret);
	
	/*** Disable Sensor ***/
	ret = atm_sensor_disable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("DISABLE FAIL","[ret2:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("DISABLE OK","[ret:%d]",ret);
	
	/** START MEASUREMENT **/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ONE SHOT MEASUREMENT FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("ONE SHOT MEASUREMENT OK","[ret:%d]",ret);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}

/**********************************************************************
 *
 *
 *  Name: atm_sensor_start_odr
 *
 *  Description: Start odr mode
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_start_odr( void )
{
	signed   int  ret  = ATM_SENSOR_OK;
	signed   int  ret2 = ATM_SENSOR_OK;
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG3;
	unsigned char mask = ATM_SENSOR_MASK_START_ODR;
	unsigned char values = ATM_SENSOR_START_ODR;
	unsigned char buf[GET_PRESS_LENGTH];
	
	PRINT_INFO("START","");
	/* CTRL_REG3(0x23) ENABLE_MEAS[Bit7] = 1 */
	PRINT_INFO("ODR MEASUREMENT","");
	
	/*** Enable sensor ***/
	ret = atm_sensor_enable();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ENABLE FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("ENABLE OK","[ret:%d]",ret);
	
	memset( buf, 0, GET_PRESS_LENGTH );
	
	/* Clear FIFO */
	ret = atm_sensor_clear_fifo( buf );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("FIFO CLEAR FAIL","[ret:%d]",ret);
		/*** Disable Sensor ***/
		ret2 = atm_sensor_disable();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("FIFO CLEAR FAIL:DISABLE","[ret2:%d]",ret2);
		return ret;
	}
	PRINT_INFO_1("FIFO CLEAR OK","[ret:%d]",ret);
	
	/** START MEASUREMENT **/
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ODR MEASUREMENT FAIL","[ret:%d]",ret);
		
		/*** Disable Sensor ***/
		ret2 = atm_sensor_disable();
		if( ret2 != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE FAIL","[ret2:%d]",ret2);
		}
		PRINT_INFO_1("ODR MEASUREMENT FAIL:DISABLE","[ret2:%d]",ret2);
	}
	PRINT_INFO_1("ODR MEASUREMENT","[ret:%d]",ret);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_clear_fifo
 *
 *  Description: Clear FIFO
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_clear_fifo( unsigned char *buf_p )
{
	signed int ret = ATM_SENSOR_OK;
	int i;
	unsigned char buf[GET_PRESS_LENGTH];
	unsigned char mask = ATM_SENSOR_STATUS_FIFO_E_MASK;
	
	PRINT_INFO_1("START","[buf:%p]",buf_p);
	/** Confirm **/
	if( buf_p == NULL )
	{
		PRINT_ERR("NULL POINTER","");
		return ATM_SENSOR_INVALID;
	}
	
	/* init buf */
	memset( buf, 0, GET_PRESS_LENGTH );
	
	/* If FIFO has some data, need to delete FIFO data. */
	for( i=0 ; i< FIFO_MAX ;i++ ) /* FIFO_MAX = 16 */
	{
		PRINT_INFO_1("CLEAR FIFO","[i:%d]",i);
		
		/*-- confirm FIFO_E --*/
		/* STATUS_REG(0x27) FIFO_E[Bit2] */
		buf[0] = ATM_SENSOR_STATUS_REG;
		ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, GET_STATUS_LENGTH );
		if( ret != ATM_SENSOR_OK ) /* read error */
		{
			PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
			break;
		}
		PRINT_INFO_1("READ OK","[ret:%d]",ret);
		/*-- Judge to whether FIFO is empty or not --*/
		if( (buf[0] & mask) == 0x00 ) /* If FIFO_E's value is 0, exist value in FIFO.*/
		{/* exist FIFO data */
			PRINT_INFO_1("GET PRESS OUT","[i:%d]",i);
			PRINT_INFO_1("FIFO EXIST DATA","[value:%02X]",(buf[0] & mask));
			/* Read PRESS_OUT_x */
			buf[0] = ATM_SENSOR_PRESS_OUT_XL | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
			ret = atm_sensor_ifi2c_read( gZPA2326_I2C_CLIENT_p, buf, GET_PRESS_LENGTH );
			if( ret != ATM_SENSOR_OK )
			{
				PRINT_ERR_1("GET PRESS FAIL","[ret:%d]",ret);
				break;
			}
			PRINT_INFO_4("READ PRESS OUT","[i:%d][0x%02X,0x%02X,0x%02X]",i,buf[2],buf[1],buf[0]);
			PRINT_INFO_1("GET PRESS OK","[ret:%d]",ret);
			SNS_OS_MEMSCPY(buf_p, sizeof(buf), buf, sizeof(buf));
		} /* end if exist FIFO data */
		else /* FIFO is empty */
		{
			PRINT_INFO_1("FIFO IS CLEAR","[i:%d]",i);
			break;
		}/* END check FIFO_E value */
	}/**-- END confirm FIFO_E reg roop --**/
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_stop_autozero
 *
 *  Description: Stop autozero mode
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_stop_autozero( void )
{
	signed   int  ret = ATM_SENSOR_OK;
	unsigned int  len = AUTOZERO_LENGTH;
	unsigned char buf[AUTOZERO_LENGTH+1];
	
	PRINT_INFO("START","");
	/*** Init buf ***/
	memset( buf, 0, AUTOZERO_LENGTH );
	
	/*** Clear REF_P_x ***/
	/**-- write REF_P_XL registar [press_data] --**/
	/**-- REF_P_XL(0x08)=0x00 --**/
	buf[0] = ATM_SENSOR_REF_P_XL | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
	buf[1] = ATM_SENSOR_DFLT_REF_P_XL;
	/**-- REF_P_L(0x09)=0x00 --**/
	buf[2] = ATM_SENSOR_DFLT_REF_P_L;
	/**-- REG_P_H(0x0A)=0x00 --**/
	buf[3] = ATM_SENSOR_DFLT_REF_P_H;
	ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("WRITE OK","[ret:%d]",ret);
	
	/*** Clear FIFO ***/
	ret = atm_sensor_clear_fifo( buf );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("FIFO CLEAR FAIL","[ret:%d]",ret);
	}
	PRINT_INFO_1("FIFO CLEAR OK","[ret:%d]",ret);
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
	
}


/**********************************************************************
 *
 *
 *  Name: atm_sensor_set_autozero
 *
 *  Description: Start autozero mode
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_set_autozero( void )
{
	signed   int  ret = ATM_SENSOR_OK;
	unsigned int  len = AUTOZERO_LENGTH;
	unsigned char buf[AUTOZERO_LENGTH+1];
	unsigned char press_data[AUTOZERO_LENGTH];
	unsigned char reg_addr = ATM_SENSOR_CTRL_REG2;
	unsigned char mask = ATM_SENSOR_MASK_AUTOZERO;
	unsigned char values = ATM_SENSOR_ENA_AUTOZERO;
	
	PRINT_INFO("START","");
	
	/*** Init buf ***/
	memset( buf, 0, AUTOZERO_LENGTH );
	memset( press_data, 0, AUTOZERO_LENGTH );
	
	/*** Clear FIFO ***/
	ret = atm_sensor_clear_fifo( press_data );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("FIFO CLEAR FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("FIFO CLEAR OK","[ret:%d]",ret);
	
	/**-- Set Autozero mode --**/ 
	/* CTRL_REG2(0x22) Autozero[Bit1]=1 */
	ret = atm_sensor_update( reg_addr, values, mask );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SET AUTOZERO FAIL","[ret:%d]",ret);
		return ret;
	}
	PRINT_INFO_1("SET AUTOZERO OK","[ret:%d]",ret);
	
	/*** In case that there are new data ***/
	/* Update REF_PRESSURE */
	if( (press_data[0] != 0 ) && (press_data[1] != 0 ) && (press_data[2] != 0) )
	{
		PRINT_INFO("EXIST NEW DATA","");
		
		/**-- write REF_P_x registar [press_data] --**/
		/**-- REF_P_XL(0x08)=0x00 --**/
		buf[0] = ATM_SENSOR_REF_P_XL | ATM_SENSOR_I2C_MULTI_MODE; /* read more than two register */
		buf[1] = press_data[0];
		/**-- REF_P_L(0x09)=0x00 --**/
		buf[2] = press_data[1];
		/**-- REG_P_H(0x0A)=0x00 --**/
		buf[3] = press_data[2];
		ret = atm_sensor_ifi2c_write( gZPA2326_I2C_CLIENT_p, buf, len );
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("WRITE FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("EXIST NEW DATA:WRITE OK","[ret:%d]",ret);
	}
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}

/**********************************************************************
 *
 *
 *  Name: atm_sensor_disable_semfree
 *
 *  Description: Run disable and Free semaphore
 **  Arguments: void
 *
 * Return Values: ret code defined in "ZPA2326_core.h"
 *
 * Notes:
 *
 *********************************************************************/
static signed int atm_sensor_disable_semfree( void )
{
	signed int ret  = ATM_SENSOR_OK;
	signed int ret2 = ATM_SENSOR_OK;
	
	PRINT_INFO("START","");
	
	/*** Whether Measurement mode or not ***/
	if( gATM_SENSOR_DATA.eMeasureMode == e_Sensor_odr )
	{
		PRINT_INFO("STOP MEASUREMENT MODE AND DISABLE","");
		ret = atm_sensor_stop_measurement();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("STOP MEASUREMENT MODE FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("STOP MEASUREMENT MODE","[ret:%d]",ret);
	}
	else
	{
		PRINT_INFO("SENSOR DISABLE","");
		/**-- Disable Sensor --**/
		ret = atm_sensor_disable();
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("DISABLE FAIL","[ret:%d]",ret);
		}
		PRINT_INFO_1("DISABLE","[ret:%d]",ret);
	}
	
	/**-- unlock semaphore --**/
	ret2 = atm_sensor_ifsys_sem_free( gZPA2326_SEM_p );
	if( ret2 != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SEM FREE FAIL","[ret2:%d]",ret2);
	}
	
	/*** If ret2 is fail, update return value ***/
	if( (ret == ATM_SENSOR_OK) && (ret2 != ATM_SENSOR_OK) )
	{
		ret = ret2;
	}
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return ret;
}

