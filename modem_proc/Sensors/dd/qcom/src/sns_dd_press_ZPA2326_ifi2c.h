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
/******* Murata Manufacturing Co,.Ltd. 2014 *****************
*
* Filename : sns_dd_press_ZPA2326_ifi2c.h
*
* Purpose  : I2C header.
*
* $Author  : Yusuke Yoshida $
* $Date    : 2014/06/10 20:04 $
* $Revision: #1 $
*
**************************************************************/
#ifndef ZPA2326_IFI2C_H
#define ZPA2326_IFI2C_H


/*************************************************************
							INCLUDE 
**************************************************************/
#include "sns_ddf_comm.h"

#include "sns_dd_press_ZPA2326.h"
/*************************************************************
                 define Registar 
**************************************************************/



/*************************************************************
                 Extern Function
**************************************************************/
signed int atm_sensor_ifi2c_init( sns_ddf_handle_t *port_handle_p, sns_ddf_device_access_s *device_info );
signed int atm_sensor_ifi2c_exit(sns_ddf_handle_t *port_handle_p );
signed int atm_sensor_ifi2c_data_init( void *client_p );
signed int atm_sensor_ifi2c_read( void *client_p, unsigned char *buf_p, unsigned int len );
signed int atm_sensor_ifi2c_write( void *client_p, unsigned char *buf_p, unsigned int len );

#endif
