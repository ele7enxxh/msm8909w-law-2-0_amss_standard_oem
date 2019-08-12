/*
@file qsee_api_tests.c
@brief Contains test code for most of the QSEE APIs.

*/
/*===========================================================================
   Copyright (c) 2011-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/qsee_api_tests.c#2 $
  $DateTime: 2016/11/17 01:02:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
  10/06/16   dpk     Removed BLSP driver test case
  04/25/16   dpk     Updated SPI testcase and added BLSP config driver to test case.
===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_sfs.h"
#include "qsee_fs.h"
#include "qsee_i2c.h"
#include "qsee_comstr.h"
#include "qsee_spi.h"
#include "qsee_comstr.h"
#include "qsee_prng.h"
#include "qsee_core.h"
#include "tzbsp_hwio.h"

#define TOUCH_I2C_ADDRESS 0x4a                /* Address of the touch controller on the I2C bus */
#define TOUCH_I2C_DEVICE  QSEE_I2CPD_DEVICE_1 /* Bus the touch controller is on, BLSP 1 */

#define SPI_WRITE_ENABLE 1
#define SPI_READ_ENABLE 1
#define SPI_FULL_DUPLEX_ENABLE 1
#define SPI_LOOPBACK_ENABLED 1 //NOTE IF SPI_LOOPBACK_ENABLED is set to 1, then
							   // SPI_LOOPBACK mode should be enabled in 
							   // tzbsp_spi.c, other wise there will be data 
							   // mismatch
#define SPI_WRITE_BUF_SIZE 256
#define SPI_READ_BUF_SIZE  256
#define SPI_DEVICE_ID QSEE_SPI_DEVICE_6

/*external APIs that are not included in a header file*/
extern unsigned int qsee_read_register(unsigned int addr);

/* Registers in the Atmel mXT Memory Map, Figure 2-1 of Protocol Guide */
#define REG_FAMILY_ID		  0x00 

uint32 SpiTransferLen;

static int run_security_state_test(void);

int run_core_test(void* cmd, uint32 cmdlen, void* rsp, uint32 rsplen, void* data, uint32 len)
{
  uint32 retval = 0;
  uint32 output_buffer = 0;
  
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- CORE UNIT TESTS ---------------------------------");

  retval = qsee_prng_getdata((uint8 *)(&output_buffer), sizeof(uint32));
  if(retval != sizeof(uint32))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_prng_getdata() FAILED, returned wrong number of bytes written.");
    return -1;
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_prng_getdata() PASSED, output: %x", output_buffer);
  }

  if( !qsee_is_ns_range(cmd, cmdlen)  && qsee_is_ns_range(data, len))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_is_ns_range PASSED");
  }   
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_is_ns_range FAILED"); 
    return -1;
  }

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_read_jtag_id() output: %x",qsee_read_jtag_id());
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_read_serial_num() output: %x",qsee_read_serial_num());

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_get_secure_stat() tests DONE: %x",run_security_state_test());

  return 0;
}
 
int run_securechannel_test()
{
 QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- SECURE CHANNEL UNIT TESTS ---------------------------------");
 
	/*add tests here*/

 QSEE_LOG(QSEE_LOG_MSG_ERROR, "   Secure channel tests are not yet implemented.");
 return 0;
}

int run_misc_qsee_api_test()
{
  uint32  hw_version = 0 ;
  uint8   tz_app_id[8]={0};
  uint32 *p_app_id = (uint32*)tz_app_id;
  uint32  oem_id = 0;

  hw_version = qsee_read_hw_version();
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_read_hw_version = 0x%x", hw_version);

  if(qsee_get_tz_app_id(tz_app_id, sizeof(tz_app_id))) 
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "Error: qsee_get_tz_app_id() failed");
    return -1; 
  }
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "tz_app_id (low32) = 0x%x", *p_app_id);
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "tz_app_id (high32) = 0x%x", *(uint32*)(p_app_id+1));

  oem_id = qsee_read_register((unsigned int)HWIO_OEM_ID_ADDR);
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_read_register(HWIO_OEM_ID_ADDR) = 0x%x", oem_id);

  return 0;
}
int run_misc_test()
{
  WCHAR* w_a = (WCHAR*)qsee_malloc(128);
  char * a = (char*)qsee_malloc(64);
  char * b = (char*)qsee_malloc(64);

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- COMSTR TESTS begin---------------------------------");

  if(!a || !b || !w_a)
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_malloc() failed");
    return -1;
  }

  if(!qsee_strlcpy(a, "Test string", 32))  //Returns the number of bytes copied, if succssful.  Should be non-zero. 
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strlcpy() failed");
    return -1; 
  }

  if(!qsee_strlcpy(b, "Test string", 32))  //Returns the number of bytes copied, if succssful.  Should be non-zero. 
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strlcpy() failed");
    return -1; 
  }

  if(!qsee_strlcat(a, "-ending-", 32)) // (a's string length is now 19 chars, and then a NULL char at the end.)
  { 
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strlcat() failed");
    return -1; 
  }

  if(0 != qsee_strnicmp(a, b, 4)) //Compare only first 4 bytes.  Return should be 0.
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strnicmp() failed");
    return -1;  
  }

  if(0 == qsee_strnicmp(a, b, 32)) //Compare all 32 bytes.  Return should NOT be 0.
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strnicmp() failed");
    return -1;
  }

  qsee_strtowstr(a, w_a, 64);

  if(19 != qsee_wstrlen(w_a))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strtowstr() or qsee_wstrlen() failed");
    return -1;
  }
  
  if(0 != qsee_wstrcmp(w_a, w_a))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrcmp() failed");
    return -1;
  }

  if(0 == qsee_wstrtostr(w_a, b, 32))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrtostr() failed");
    return -1;
  }

  if(0 != qsee_strnicmp(a, b, 32))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_strnicmp() failed");
    return -1;
  }

  if(0 == qsee_wstrlcat(w_a, w_a, 64))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrlcat() failed");
    return -1;
  }

  if(w_a != qsee_wstrchr(w_a, 'T'))
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "-------------- qsee_wstrchr() failed");
    return -1;
  }

  qsee_free(a);
  qsee_free(b);
  qsee_free(w_a);
  
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- COMSTR TESTS passed---------------------------------");
  
  
  
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "----------- MISC QSEE API TESTS begins ------------");
  
  if (0 != run_misc_qsee_api_test())
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "----------- MISC QSEE API TESTS failed ------------");
    return -1;
  }

  QSEE_LOG(QSEE_LOG_MSG_ERROR, "----------- MISC QSEE API TESTS passed ------------");
  
  return 0;
}

int run_buses_spi_test()
{
	int retval = 0;

#if 1  //Clocks and GPIOs are not enabled in SPI driver, therefore these tests will hang (hence commented out).

	qsee_spi_config_t spi_config;
	qsee_spi_transaction_info_t write_info;
	static uint8 write_data[SPI_WRITE_BUF_SIZE] __attribute__((aligned(64)));
	qsee_spi_transaction_info_t read_info;
	static uint8 read_data[SPI_READ_BUF_SIZE] __attribute__((aligned(64)));
	int i;
	uint32  bSpiTxferLen;

	//START SPI TEST CODE
	spi_config.spi_bits_per_word = 8;
	spi_config.spi_clk_always_on = QSEE_SPI_CLK_NORMAL;
	spi_config.spi_clk_polarity = QSEE_SPI_CLK_IDLE_HIGH;
	spi_config.spi_cs_mode = QSEE_SPI_CS_KEEP_ASSERTED;
	spi_config.spi_cs_polarity = QSEE_SPI_CS_ACTIVE_HIGH;
	spi_config.spi_shift_mode = QSEE_SPI_OUTPUT_FIRST_MODE;
	spi_config.loopback = SPI_LOOPBACK_ENABLED;
	spi_config.max_freq       = 960000;


	for (i = 0; i < SPI_WRITE_BUF_SIZE; i++) {
		write_data[i] = 0xAB;
	}

	SpiTransferLen = SPI_READ_BUF_SIZE;
		
	write_info.buf_addr = write_data;

	for (i = 0; i < SPI_READ_BUF_SIZE; i++) {
		read_data[i] = 0XFF;
	}
	read_info.buf_addr = read_data;

    bSpiTxferLen = 1;

	if (0 != (retval = qsee_spi_open(SPI_DEVICE_ID))) {
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_open1: retval = %d\n", retval);
	}

	do{
		write_info.buf_len = bSpiTxferLen;
		read_info.buf_len = bSpiTxferLen; //SPI_READ_BUF_SIZE*sizeof(uint8);
#if 0
		//PERFORM WRITE
		if (0 != (retval = qsee_spi_write(SPI_DEVICE_ID,&spi_config, &write_info))){
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_write: retval = %d\n", retval);
			break;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_write: retval = %d\n", retval);

		//PEFORM READ
		if (0 != (retval = qsee_spi_read(SPI_DEVICE_ID ,&spi_config, &read_info))){
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_read: retval = %d\n", retval);
			break;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_read: retval = %d\n", retval);
#endif		

		if (0 != (retval = qsee_spi_full_duplex(SPI_DEVICE_ID ,&spi_config, 
						&write_info, &read_info))){
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_full_duplex: retval = %d\n",
					retval);
			break;
		}

		if (retval == -1) {
			break;
		}

		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_full_duplex: retval = %d\n", retval);

		
		// Compare Write and Read buffers
		for (i=0 ; i < bSpiTxferLen; i++)
  		{
		    if ( write_data[i] != read_data[i] ) 
			{
			      //Comparison failed 
			      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "QSEE SPI Loopback test failed: WR[x] RD[x], index=%d", i);
			      break;
		    }			
		}
		
		if (i == bSpiTxferLen)
		{
            QSEE_LOG(QSEE_LOG_MSG_DEBUG, "QSEE SPI Loopback test SUCCESS for %d bytes", i);
        }

		bSpiTxferLen = bSpiTxferLen + 1;
	}while(bSpiTxferLen <= SpiTransferLen);


	if (0 != (retval = qsee_spi_close(SPI_DEVICE_ID ))) {
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_spi_close1: retval = %d\n", retval);
	}
#endif

	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "BUSES SPI TEST COMPLETE, retval = %d\n", retval);
	return retval;

}

int run_buses_test()
{
  int ret = 0;
  int retClose = 0;
    
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- BUSES UNIT TESTS ---------------------------------");
  
  /* Open the bus. This will lock the bus to TZ */
  ret = qsee_i2c_open(TOUCH_I2C_DEVICE);
  if( ret != 0 )
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_i2c_open() FAILED! returned = %ld", ret);
    return ret;
  }
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "   qsee_i2c_open() PASSED");

  /* This call requires the QUP clocks to be on, or will cause an L2 error */
#if 0
  do {
    uint8 familyId;
    qsee_i2c_config_t i2c_config;
    qsee_i2c_transaction_info_t  readInfo;
    qsee_i2c_bus_config_t busConfig;
    qsee_i2c_slave_device_config_t deviceConfig;
    
    i2c_config.p_bus_config = &busConfig;
    i2c_config.p_slave_config = &deviceConfig;
    
    /* swap HIGH and LOW bytes for address , as requested by the Atmel mXT specs */
    readInfo.start_addr = ((REG_FAMILY_ID >> 8) & 0x00FF) | ((REG_FAMILY_ID & 0x00FF) << 8);
    readInfo.p_buf = &familyId;
    readInfo.buf_len = sizeof(familyId);
    readInfo.total_bytes = 0;
  
    ret = qsee_i2c_read(TOUCH_I2C_DEVICE, &i2c_config, &readInfo);
    if( ret != 0 )
    {
      QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_i2c_read() FAILED! returned = %ld", ret);
      break;
    }
    else
    {
      QSEE_LOG(QSEE_LOG_MSG_DEBUG, "   qsee_i2c_read() PASSED");
    }
  } while (0);
#endif
  retClose = qsee_i2c_close(TOUCH_I2C_DEVICE);
  if( retClose != 0 )
  {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_i2c_close() FAILED! returned = %ld", ret);
  }
  else
  {
    QSEE_LOG(QSEE_LOG_MSG_DEBUG, "   qsee_i2c_close() PASSED");
  }
  
  if (ret)
    return ret;
  
  return retClose;
}


int security_state_test()
{
  int ret = 0;
  qsee_secctrl_secure_status_t status;
  memset(&status, 0, sizeof(status));

  ret = qsee_get_secure_state(&status);
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_get_secure_state() pass test: %x %x, ret %d",
               status.value[0], status.value[1], ret);
  if (0 != ret) {
    QSEE_LOG(QSEE_LOG_MSG_ERROR, "qsee_get_secure_state() call fail and returns %d\n", ret);
    return 1;
  }
  if (0 == (status.value[0] & 0x1f)) {
     QSEE_LOG(QSEE_LOG_MSG_DEBUG, "secure boot is setup");
     ret = 0x0;
  } else {
     QSEE_LOG(QSEE_LOG_MSG_DEBUG, "secure boot is NOT setup");
     ret = 0x02;
  }

  if (0 == (status.value[0] & 0x20)) {
     QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RPMB is provisioned");
  } else {
     QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RPMB boot is NOT provisioned");
     ret |= 0x4;
  }

  return ret;
}

static int run_security_state_test()
{
  int ret = 0;
  qsee_secctrl_secure_status_t status;
  memset(&status, 0, sizeof(status));

  ret = qsee_get_secure_state(&status);
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_get_secure_state() pass(=1): %d, val: %x %x",
               (ret == 0), status.value[0], status.value[1]);

  ret = qsee_get_secure_state(NULL);
  QSEE_LOG(QSEE_LOG_MSG_ERROR, "  qsee_get_secure_state() negative, NULL buffer, pass(=1): %d", (ret != 0));
 
  return ret;
}

