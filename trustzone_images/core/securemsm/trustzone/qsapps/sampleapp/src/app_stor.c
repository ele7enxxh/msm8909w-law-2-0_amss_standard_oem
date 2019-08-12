/*
@file app_stor.c
@brief This file is  storarge test code to test all qsee_stor APIs.

*/
/*===========================================================================
   Copyright (c) 2011 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_stor.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/07/12  phucd   Initial Version
===========================================================================*/
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_stor.h"
#include "qsee_timer.h"


#define TZ_RPMB_TEST_NUM_SECTOR_MAX	8  //number of sectors used to add a new parition in RPMB parition
qsee_stor_device_id_type rpmb_device_id = QSEE_STOR_EMMC_RPMB;
#define TZ_RPMB_PARITION_ID_0  0xF000


#define TZ_GPT_TEST_NUM_SECTOR_MAX	4  //number of sectors used to add a new test parition in GPT physical partition
#define TZ_GPT_PARITION_ID_0  1
qsee_stor_device_id_type gpt_device_id = QSEE_STOR_EMMC_GPP1;

#define TZ_BUFF_SIZE_TEST	1024  // size of buffer hold for 2 sectors
#define TZ_BUFF_BACKUP_SIZE (TZ_RPMB_TEST_NUM_SECTOR_MAX*512)  //size of entire parition


qsee_stor_device_id_type gDeviceID = QSEE_STOR_EMMC_RPMB;
qsee_stor_device_handle_t gDevHandle = NULL;
qsee_stor_client_handle_t gClientHandle = NULL;
qsee_stor_client_info_t   gClientInfo;
uint8 gBackupBuff[TZ_BUFF_BACKUP_SIZE]={0x00};
uint8 gBuff[TZ_BUFF_SIZE_TEST]={0x00};
uint8 gBuff2[TZ_BUFF_SIZE_TEST]={0x00};

/************************ Helper functions **********************************/
int BufferBackup(uint8 *pBuf, uint32 start_sector, uint32 num_sectors)
{
	int       status = QSEE_STOR_SUCCESS;
	if (NULL == pBuf) 
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- invalid parameter---------------");
		return QSEE_STOR_ERROR;
	}
	/* Restore the test sectors */
	status  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
							  start_sector,							// start_sector = 0
							  num_sectors,
							  pBuf);
	if (QSEE_STOR_SUCCESS != status) {
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "Backup: Fail to readSectors, return = %d\n--------------",status);
	}
	return status;
}

int BufferRestore(uint8 *pBuf, uint32 start_sector, uint32 num_sectors)
{
   int       status = QSEE_STOR_SUCCESS;
   if (NULL == pBuf) 
   {
	 QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- invalid parameter---------------");
     return QSEE_STOR_ERROR;
   }
   /* Restore the test sectors */
   status  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
								start_sector,							// start_sector = 0
								num_sectors,
								pBuf);
   if (QSEE_STOR_SUCCESS != status) {
	 QSEE_LOG(QSEE_LOG_MSG_DEBUG, "Restore: Fail to writeSectors, return = %d\n--------------",status);
   }
   return status;
}

void qsee_stor_memset(uint8* pBuff, uint32 sizeBuff,uint8 pattern)
{
	int i=0;
	for(i=0; i< sizeBuff; i++)
	{
		pBuff[i]=pattern;
	}
}

/************************ Test functions **********************************/
int qsee_stor_deviceInitTest()
{
	int ret = 0;//SUCCESS;
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceInitTest Test start...---------------------------");

	ret = qsee_stor_device_init(rpmb_device_id,0,&gDevHandle);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "qsee_stor_device_init return=%ld. Test FAILED!",ret);
	}
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceInitTest Test PASSED---------------------------");
	return ret;
}

int qsee_stor_deviceAddOpenPartitionTest(uint8 partition_guid)
{
	int ret = 0;//SUCCESS;
	uint32 num_sectors = 0;
	uint32 partition_id = 0xF000;
	
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceAddOpenPartitionTest start...---------------------------");
	
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
		num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX;
		
	}
	else{
		gDeviceID = gpt_device_id;
		num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX; //GPT
		
	}
	ret = qsee_stor_open_partition(&gDevHandle,partition_id,&gClientHandle);
	if ((QSEE_STOR_SUCCESS != ret) || (gClientHandle == NULL))
	{
		//QSEE_LOG(QSEE_LOG_MSG_ERROR, "--   qsee_stor_open_partition() FAILED! returned = %ld", ret);
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "--   Add a new parition......");
		ret = qsee_stor_add_partition(&gDevHandle,partition_id,num_sectors);
		if( QSEE_STOR_SUCCESS != ret )
		{
			QSEE_LOG(QSEE_LOG_MSG_ERROR, "--   qsee_stor_add_partition() FAILED! returned = %ld", ret);
			return ret;
		}
		QSEE_LOG(QSEE_LOG_MSG_ERROR, " Successfully added partition  = %ld", ret);
		ret = qsee_stor_open_partition(&gDevHandle,partition_id,&gClientHandle);
		if ((QSEE_STOR_SUCCESS != ret) || (gClientHandle == NULL))
		{
			QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Fail to open parition_id %d , ret= %ld",ret);
			return ret;
		}
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "-- Successfully open  partition  = %ld", partition_id);

	}
	
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceAddOpenPartitionTest PASSED---------------------------");
	return ret;
}

int qsee_stor_devRWSingleContTest(uint8 partition_guid)
{
	int ret = 0;//SUCCESS;
	uint32 nTotalSectors = 0,nSectorsTest = 2,StartSector=0;
//	uint32 partitionSize = 0;
	uint32 i = 0, j=0;
	
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleContTest Test start...---------------------------");
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
	}
	else{
		gDeviceID = gpt_device_id;
	}

	ret = qsee_stor_client_get_info(&gClientHandle,
									   &gClientInfo);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- qsee_stor_client_get_info return = %ld",ret);
		return ret;
	}
	nTotalSectors = gClientInfo.total_sectors;
	//partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Total Sectors =%d",nTotalSectors);
	//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- PartitionSize =%d",partitionSize);

	// Backup buffer before testing read/write operations
	if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to Backup buffer");
		return QSEE_STOR_ERROR;
	}
	qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);
	
	for(j=0; j<nTotalSectors;j=(j+nSectorsTest))
	{
		/*Logic to write data to card*/
		//StartSector = nSectorsTest*j;
		StartSector = j;
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write 2 SectorTest at a time start at=%d sector ---",StartSector);
		
		ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
										 StartSector,							// start_sector = 0
										 nSectorsTest,           //num_sector
										 gBuff);
		if (ret != QSEE_STOR_SUCCESS )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to write buffer,  qsee_stor_write_sectors returns = %ld",ret);
			break;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write ==> Good -- ");

	}
	if( QSEE_STOR_SUCCESS == ret )
	{
		/*Logic to read data from card*/
		for(j=0; j<nTotalSectors; j=(j+nSectorsTest))
		{
			qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
			/*Logic to read data from card*/
			//StartSector = nSectorsTest*j;
			StartSector = j;
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Read 2 SectorTest at a time start at sector %d---",StartSector);
			
			ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
										 StartSector,							// start_sector = 0
										 nSectorsTest,
										 gBuff);
			if (ret != QSEE_STOR_SUCCESS )
			{
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to read buffer, qsee_stor_read_sectors returns  = %ld",ret);
				break;
			}
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Read  ==> Good ---");
			if( (QSEE_STOR_SUCCESS == ret) )
			{
				/* Verify buffer */
				for(i=0; i<TZ_BUFF_SIZE_TEST; i++)
				{
					if( gBuff[i] != 0x5A )
					{
						QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Content mismatched at , gbuff[%d]=0x%x , Test failed ---",i,gBuff[i]);
						ret = QSEE_STOR_ERROR;
						//Restore Data
						BufferRestore(gBackupBuff,0,nTotalSectors);
						QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleContTest FAILED ---------------------------");
						return ret;
					}
				}
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
			}

		}
	}

	//Restore Data
	BufferRestore(gBackupBuff,0,nTotalSectors);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleContTest PASSED ---------------------------");
	return ret;
}

int qsee_stor_devRWSingleAltTest(uint8 partition_guid)
{
	int ret = 0;//SUCCESS;
	uint32 nTotalSectors = 0,nSectorsTest = 2,StartSector=0;
	//uint32 partitionSize = 0;
	uint32 i = 0, j=0;
	uint8 ReadBuff[TZ_BUFF_SIZE_TEST] = {0};
	

	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleAltTest Test start...---------------------------");
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
	}
	else{
		gDeviceID = gpt_device_id;
	}
	ret = qsee_stor_client_get_info(&gClientHandle,
									   &gClientInfo);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to get_info, ret = %ld",ret);
		return ret;
	}
	nTotalSectors = gClientInfo.total_sectors;
	//partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;
	
	// Backup buffer before testing read/write operations
	if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to Backup buffer");
		return QSEE_STOR_ERROR;
	}
	qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);
	for(j=0; j<nTotalSectors; j=(j+nSectorsTest))
	{
		/*Logic to write data to card*/
		//StartSector = nSectorsTest*j;
		StartSector = j;
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write 2 SectorTest at a time start at=%d sector---",StartSector);
		ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
										 StartSector,							// start_sector = 0
										 nSectorsTest,           //num_sector
										 gBuff);
		if (ret != QSEE_STOR_SUCCESS )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to write buffer,  qsee_stor_write_sectors returns = %ld",ret);
			break;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write ==> Good -- ");
		/*Logic to read data from card*/			
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Read 2 SectorTest at a time start at sector %d ---",StartSector);
		qsee_stor_memset(ReadBuff,TZ_BUFF_SIZE_TEST,0x00);
		ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
									 StartSector,							// start_sector = 0
									 nSectorsTest,
									 ReadBuff);
		if (ret != QSEE_STOR_SUCCESS )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to read buffer, qsee_stor_read_sectors returns  = %ld",ret);
			break;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Read  ==> Good ---");

		/* Verify buffer */
		for(i=0; i<TZ_BUFF_SIZE_TEST; i++)
		{
			if( ReadBuff[i] != 0x5A )
			{
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Content mismatched at , Readbuff[%d]= 0x%x, Test failed ---",i,ReadBuff[i]);
				// Restore parition
				BufferRestore(gBackupBuff,0,nTotalSectors);
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleAltTest FAILED---------------------------");
				return ret;

			}
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
	}
	// Restore parition
	BufferRestore(gBackupBuff,0,nTotalSectors);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleAltTest PASSED---------------------------");
	return ret;
}

int qsee_stor_devRWDoubleContTest(uint8 partition_guid)
{
	int ret = 0;//SUCCESS;
	uint32 nTotalSectors = 0,nSectorsTest = 2,StartSector=0;
	//uint32 partitionSize = 0;
	uint32 i = 0, j=0, count = 0;
	uint8 pattern = 0x00;
	
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWDoubleContTest start...---------------------------");
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
	}
	else{
		gDeviceID = gpt_device_id;
	}
	ret = qsee_stor_client_get_info(&gClientHandle,
									   &gClientInfo);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to get_info, ret = %ld",ret);
		return ret;
	}
	nTotalSectors = gClientInfo.total_sectors;
	//partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

	// Backup buffer before testing read/write operations
	if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to Backup buffer");
		return QSEE_STOR_ERROR;
	}
	qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);  // pattern 0x5A
	qsee_stor_memset(gBuff2,TZ_BUFF_SIZE_TEST,0x7F); // pattern 0xFF
	
	for(j=0; j<nTotalSectors; j=(j+nSectorsTest))
	{
		/*Logic to write data to card*/
		//StartSector = nSectorsTest*j;
		StartSector = j;
		//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write 2 SectorTest at a time start at =%d sector ---",StartSector);
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Count = %d",count);
		if( (count%2) )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Write 2 SectorTest at a time start at sector %d with pattern 0x5A---",StartSector);
			ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
										 StartSector,							// start_sector = 0
										 nSectorsTest,           //num_sector
										 gBuff);				// buffer with pattern 0x5A
		}else{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Write 2 SectorTest at a time start at sector %d with pattern 0x7F---",StartSector);
			ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
										 StartSector,							// start_sector = 0
										 nSectorsTest,           //num_sector
										 gBuff2);				// buffer filled with pattern 0xFF

		}
		if (ret != QSEE_STOR_SUCCESS )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--- Fail to write buffer,  qsee_stor_write_sectors returns = %ld",ret);
			//return ret;
			break;
		}
		count++;
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Write ==> Good -- ");
	}
	if (QSEE_STOR_SUCCESS == ret)
	{
		count = 0;
		/*Logic to read data from card*/
		for(j=0; j <nTotalSectors; j=(j+nSectorsTest))
		{
			qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
			/*Logic to read data from card*/
			//StartSector = nSectorsTest*j;
			StartSector = j;
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Read 2 SectorTest at a time start at sector %d ---",StartSector);
			
			ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
										 StartSector,							// start_sector = 0
										 nSectorsTest,
										 gBuff);
			if (ret != QSEE_STOR_SUCCESS )
			{
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Fail to read buffer, qsee_stor_read_sectors returns  = %ld",ret);
				//return ret;
				break;
			}
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Read  ==> Good ---");
			/* Verify buffer */
			for(i=0; i<TZ_BUFF_SIZE_TEST; i++)
			{
				if( count%2 )
					pattern = 0x5A;
				else 
					pattern = 0x7F;
				
				if( gBuff[i] != pattern )
				{
					QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Content mismatched at , pbuff[%d]=0x%x , Test failed ---",i,gBuff[i]);
					ret = QSEE_STOR_ERROR;
					BufferRestore(gBackupBuff,0,nTotalSectors);
					QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-------------------- qsee_stor_devRWDoubleContTest FAILED ---------------------------");
					return ret;
				}
			}
			count++;
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
		}
		// Restore parition
	}

	BufferRestore(gBackupBuff,0,nTotalSectors);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-------------------- qsee_stor_devRWDoubleContTest PASSED ---------------------------");
	return ret;
}

int qsee_stor_devRWDoubleAltTest(uint8 partition_guid)
{
	int ret = 0;//SUCCESS;
	uint32 nTotalSectors = 0,nSectorsTest = 2,StartSector= 0;
	//uint32 partitionSize = 0;
	uint32 i = 0, j=0, count = 0;
	uint8 pattern = 0x00;
	uint8 ReadBuff[TZ_BUFF_SIZE_TEST]={0x00};

	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWDoubleAltTest start...---------------------------");
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
	}
	else{
		gDeviceID = gpt_device_id;
	}
	ret = qsee_stor_client_get_info(&gClientHandle,
									   &gClientInfo);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to get_info, ret = %ld",ret);
		return ret;
	}
	nTotalSectors = gClientInfo.total_sectors;
	//partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;


	// Backup buffer before testing read/write operations
	if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to Backup buffer");
		return QSEE_STOR_ERROR;
	}
	qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);
	qsee_stor_memset(gBuff2,TZ_BUFF_SIZE_TEST,0x7A);
	for(j=0; j<nTotalSectors; j=(j+nSectorsTest))
	{
		/*Logic to write data to card*/
		//StartSector = nSectorsTest*j;
		StartSector = j;
		//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write 2 SectorTest at a time at start @ %d sector ---",StartSector);
		//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Count = %d",count);
		if( count % 2 )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Write 2 SectorTest at a time, start sector %d with pattern 0x5A---",StartSector);
			pattern = 0x5A;
			ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
											 StartSector,							// start_sector = 0
											 nSectorsTest,           //num_sector
											 gBuff);
		}
		else{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Write 2 SectorTest at a time start at sector %d with pattern 0x7A---",StartSector);
			pattern = 0x7A;
			//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "pReadbuff[0]=0x%x ---",gBuff2[i]);
			ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
											 StartSector,							// start_sector = 0
											 nSectorsTest,           // 2
											 gBuff2);
		}
		if (ret != QSEE_STOR_SUCCESS )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Fail to write buffer,  qsee_stor_write_sectors returns = %ld",ret);
			break;
			//return ret;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Write ==> Good -- ");

		/*Logic to read data from card*/			
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Read 2 SectorTest at a time start @ sector %d ---",StartSector);
		qsee_stor_memset(ReadBuff,TZ_BUFF_SIZE_TEST,0x00);
		ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
									 StartSector,							// start_sector = 0
									 nSectorsTest,
									 ReadBuff);
		if (ret != QSEE_STOR_SUCCESS )
		{
			QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Fail to read buffer, qsee_stor_read_sectors returns  = %ld",ret);
			break;
		}
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---- Read  ==> Good ---");
		/* Verify buffer */
		for(i=0; i < TZ_BUFF_SIZE_TEST; i++)
		{
			//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "pattern = 0x%x, pReadbuff[%d]=0x%x ---",pattern,i, ReadBuff[i]);
			if( ReadBuff[i] != pattern )
			{
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Content mismatched at , pReadbuff[%d]=0x%x , Test failed ---",i,ReadBuff[i]);
				ret = QSEE_STOR_ERROR;
				BufferRestore(gBackupBuff,0,nTotalSectors);
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "----------------------- qsee_stor_devRWDoubleAltTest FAILED---------------------------");
				return ret;
				
			}
		}
		
		count++;
	} // end for loop
		// Restore parition
	
	BufferRestore(gBackupBuff,0,nTotalSectors);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "----------------------- qsee_stor_devRWDoubleAltTest PASSED---------------------------");
	return ret;
}

int qsee_stor_AdversarialTest(uint8 partition_guid)
{
	int ret = QSEE_STOR_SUCCESS;
	uint32 num_sectors = 0;
	uint32 partition_id = 0xF000;

	qsee_stor_device_info_t device_info;

	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
		num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX;

	}
	else{
		gDeviceID = gpt_device_id;
		num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX; //GPT

	}
	ret = qsee_stor_open_partition(NULL,partition_id,&gClientHandle);
	if (QSEE_STOR_SUCCESS == ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_ERROR, " Test1: Expecting api returns FAIL , but it returns success, TEST FAILED!!!");
	}
	else {
		QSEE_LOG(QSEE_LOG_MSG_ERROR, " Test1: qsee_stor_open_partition with NULL Handle ret = %ld. Test PASSED!",ret);
	}

	//AddPartitionTwice test 
	ret = qsee_stor_add_partition(&gDevHandle,partition_id,num_sectors);
	if( QSEE_STOR_SUCCESS == ret )
	{
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test2:  Trying to add same partition twice, expecting api returns FAIL, but it return SUCCESS, TEST FAILED");
		//return ret;
	}
	else {
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test2:  Add same partition twice failed as expected, ret = %ld. Test PASSED!",ret);
	}

	//AddPartitionTwice test 
	ret = qsee_stor_add_partition(NULL,partition_id,num_sectors);
	if( QSEE_STOR_SUCCESS == ret )
	{
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test3:  Testing qsee_stor_add_partition with NULL DevHandle FAILED");
		//return ret;
	}
	else {
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test3:  qsee_stor_add_partition with NULL Handle failed as expected, ret = %ld. Test PASSED!",ret);
	}

	//AddNewPartition with numsectors exceeds the RPMB physical parition 
	partition_id = 0xF001;
	ret = qsee_stor_device_get_info(&gDevHandle,
									&device_info);
	if( QSEE_STOR_SUCCESS != ret )
	{ 
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test4: fail to get device info, qsee_stor_device_get_info return = %ld",ret);
	}
	num_sectors = device_info.available_sectors;
	QSEE_LOG(QSEE_LOG_MSG_ERROR, " DeviceInfo.available_sectors = %ld",num_sectors);
	QSEE_LOG(QSEE_LOG_MSG_ERROR, " DeviceInfo.total_sectors = %ld", device_info.total_sectors);
	
	ret = qsee_stor_add_partition(&gDevHandle,partition_id,(num_sectors + 1));
	if( QSEE_STOR_SUCCESS == ret )
	{
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test4: Testing qsee_stor_add_partition with num_sector exceeds available sectors in Physical Parition\n");
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test4: Expecting api returns error but it returns SUCCESS. TEST FAILED!");
		//return ret;
	}
	else{
		QSEE_LOG(QSEE_LOG_MSG_ERROR, "Test4: Adding Test parition w/total sectors exceeds physical parition's sectors, ret = %ld as expected. TEST PASSED",ret);
	}

	return QSEE_STOR_SUCCESS;
}

int qsee_stor_devRWAdversarialTest(uint8 partition_guid)
{
	int ret = QSEE_STOR_SUCCESS;
	uint32 nTotalSectors = 0,nSectorsTest = 2,StartSector=0;
//	uint32 partitionSize = 0;
//	uint32 i = 0, j=0;
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWAdversarialTest start...---------------------------");
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
	}
	else{
		gDeviceID = gpt_device_id;
	}
	ret = qsee_stor_client_get_info(&gClientHandle,
									   &gClientInfo);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to get_info, ret = %ld",ret);
		return ret;
	}
	nTotalSectors = gClientInfo.total_sectors;
	//partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;
	
	// Backup buffer before testing read/write operations
	if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to Backup buffer");
		return QSEE_STOR_ERROR;
	}
	qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);

	QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: Invalid handle ----------------");
	ret  = qsee_stor_write_sectors(NULL,             //ClientHandle
								  StartSector,							// start_sector = 0
								  nSectorsTest,           //num_sector
								  gBuff);
	if (QSEE_STOR_SUCCESS == ret )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: qsee_stor_write_sectors with Invalid handle returns SUCCESS - Test FAILED ---------------");
	}
	else 
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: qsee_stor_write_sectors with Invalid handle returns = %ld - Test PASSED ---------------",ret);
	}

    // 
 	ret  = qsee_stor_read_sectors(NULL,             //ClientHandle
								  StartSector,							// start_sector = 0
								  nSectorsTest,           //num_sector
								  gBuff);
	if (QSEE_STOR_SUCCESS == ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: qsee_stor_read_sectors with Invalid handle returns SUCCESS - Test FAILED ---------------");
	}
	else
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: qsee_stor_read_sectors with Invalid handle returns = %ld as expected - Test PASSED--------------",ret);
	}

    // qsee_stor_ReadSectorEndOfPartition & qsee_storWriteSectorEndOfPartition Test Cases

    StartSector = TZ_RPMB_TEST_NUM_SECTOR_MAX-1; 
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test2: Trying to RW at %d sector address with %d sectors ----------------",StartSector, nSectorsTest);
	ret  = qsee_stor_write_sectors(&gClientHandle,        
								  StartSector,			  
								  nSectorsTest,           
								  gBuff);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test2: qsee_stor_write_sectors return = %ld", ret);

	if (QSEE_STOR_SUCCESS == ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test2:  Write pass EndOFParition - TEST FAILED expecting qsee_stor_write_sectors returns failed but returns SUCCESS");
	}
	else{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test2:  Write pass EndOFParition return %ld - TEST PASSED",ret);
	}

	ret  = qsee_stor_read_sectors(&gClientHandle,           
								  StartSector,				
								  nSectorsTest,           
								  gBuff);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test2: return = %ld", ret);
	if (ret == QSEE_STOR_SUCCESS )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test2:  Read pass EndOFParition - TEST FAILED expecting qsee_stor_write_sectors returns failed but returns SUCCESS");
	}else {
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test2:  Read pass EndOFParition return %ld - TEST PASSED",ret);
	}

	// qsee_stor_ReadSectorInvalidAddr & qsee_stor_WriteSectorInvalidAddr Test Cases
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, " RWAdv Test3: RW @ %d sectors with %d sector ----------------" ,StartSector,nSectorsTest  );
	StartSector = TZ_RPMB_TEST_NUM_SECTOR_MAX + 1; 
	ret  = qsee_stor_write_sectors(&gClientHandle,        
							   StartSector,					
							   nSectorsTest, 
							   gBuff);
	
	if (ret == QSEE_STOR_SUCCESS )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test3:  write Invalid Address  - TEST FAILED, expecting qsee_stor_write_sectors returns ERROR but it returns SUCCESS");
	}
	else{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test3:  Write Invalid Address return %ld - TEST PASSED",ret);
	}
	ret  = qsee_stor_read_sectors(&gClientHandle,        
							   StartSector,					
							   nSectorsTest,           
							   gBuff);
	
	if (ret == QSEE_STOR_SUCCESS )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test3:  Read Invalid Address - TEST FAILED, expecting qsee_stor_write_sectors returns ERRROR but returns QSEE_STOR_SUCCESS");
	}
	else{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "RWAdv Test3:  Read Invalid Address return %ld - TEST PASSED",ret);
	}
   
	//Restore Data
	BufferRestore(gBackupBuff,0,nTotalSectors);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWAdversarialTest END ---------------------------");
	return QSEE_STOR_SUCCESS;
}

int qsee_stor_RWSProfileTest(uint8 partition_guid)
{
	int ret = 0;//SUCCESS;
	uint32 nTotalSectors = 0,nSectorsTest = 2,StartSector=0;
	uint32 i = 0, j=0;
	unsigned long long total_time;
	//int    tput;
	int size = TZ_RPMB_TEST_NUM_SECTOR_MAX*512;
	int iterations = 100;
	
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_RWProfile Test start...---------------------------");
	if( partition_guid==0 )
	{
		gDeviceID = rpmb_device_id;
	}
	else{
		gDeviceID = gpt_device_id;
	}

	ret = qsee_stor_client_get_info(&gClientHandle,
									   &gClientInfo);
	if (QSEE_STOR_SUCCESS!=ret)
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- qsee_stor_client_get_info return = %ld",ret);
		return ret;
	}
	nTotalSectors = gClientInfo.total_sectors;
	//partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Total Sectors =%d",nTotalSectors);
	//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- PartitionSize =%d",partitionSize);

	// Backup buffer before testing read/write operations
	if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to Backup buffer");
		return QSEE_STOR_ERROR;
	}
	qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);
	total_time = qsee_get_uptime();

	QSEE_LOG(QSEE_LOG_MSG_DEBUG,"---Performance qsee_stor_write_sectors Test -with Data Size: %u", size);
	// Profiling Write api for 100 iterations
	for (i=0; i < iterations; i++)
	{
		for(j=0; j<nTotalSectors;j=(j+nSectorsTest))
		{
			/*Logic to write data to card*/
			//StartSector = nSectorsTest*j;
			StartSector = j;
			//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write 2 SectorTest at a time start at=%d sector ---",StartSector);
			
			ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
											 StartSector,							// start_sector = 0
											 nSectorsTest,           //num_sector
											 gBuff);
			if (ret != QSEE_STOR_SUCCESS )
			{
				QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to write buffer,  qsee_stor_write_sectors returns = %ld",ret);
				break;
			}
			//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Write ==> Good -- ");

		}
	}
	total_time = qsee_get_uptime() - total_time;
	//tput = (((float)size / ((float)total_time / (float)iterations)) * 1000.00) / (1024.00 * 1024.00);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
	//QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

	
	if( QSEE_STOR_SUCCESS == ret )
	{
		QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Performance qsee_stor_read_sectors Test -with Data Size: %u", size);
		total_time = qsee_get_uptime();
		
		for (i = 0; i < iterations ; i++)
		{
			/*Logic to read data from card*/
			for(j=0; j<nTotalSectors; j=(j+nSectorsTest))
			{
				qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
				/*Logic to read data from card*/
				//StartSector = nSectorsTest*j;
				StartSector = j;
				//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Read 2 SectorTest at a time start at sector %d---",StartSector);
				
				ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
											 StartSector,							// start_sector = 0
											 nSectorsTest,
											 gBuff);

				if (ret != QSEE_STOR_SUCCESS )
				{
					QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Fail to read buffer, qsee_stor_read_sectors returns  = %ld",ret);
					break;
				}
				//QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- Read  ==> Good ---");

			}
		}// end profile
		total_time = qsee_get_uptime() - total_time;
		//tput = (((float)size / ((float)total_time / (float)iterations)) * 1000.00) / (1024.00 * 1024.00);
		QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
		//QSEE_LOG(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
	}

	//Restore Data
	BufferRestore(gBackupBuff,0,nTotalSectors);
	QSEE_LOG(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_RWProfile PASSED ---------------------------");
	return ret;
}

int run_stor_test()
{

  int ret = 0;
  int retVal = 0;
  uint8 partition_guid=0;
  
  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "---------START API FUNCTIONAL QSEE Storage  TESTS ---------------------------------");
  
  // SdDeviceInit Test
  retVal = qsee_stor_deviceInitTest();
  if( retVal != QSEE_STOR_SUCCESS )
  {
    //QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_deviceInitTest() FAILED! returned = %ld", retVal);
	return 1;
  }

  // SdDeviceAddRPMBPartition Test
  retVal = qsee_stor_deviceAddOpenPartitionTest(partition_guid);
  if( retVal != 0 )
  {
    //QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_deviceAddOpenPartitionTest FAILED! returned = %ld", retVal);
	return 1;
  }

  //Seq Read/Write with 1 pattern Test Cases
  retVal = qsee_stor_devRWSingleContTest(partition_guid);
  if( retVal != 0 )
  {
	//QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWSingleContTest FAILED! returned = %ld", retVal);
	return 1;
  }

  //RandomSeq Read/Write with 1 pattern Test Cases
  retVal = qsee_stor_devRWSingleAltTest(partition_guid);
  if( retVal != 0 )
  {
	//QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWSingleAltTest FAILED! returned = %ld", retVal);
	return 1;
  }
  
    //Seq Read/Write with 2 patterns Test Cases
  retVal = qsee_stor_devRWDoubleContTest(partition_guid);
  if( retVal != 0 )
  {
	//QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWDoubleContTest FAILED! returned = %ld", retVal);
	return 1;
  }

  //RandomSeq Read/Write with 2 pattern Test Cases
  retVal = qsee_stor_devRWDoubleAltTest(partition_guid);
  if( retVal != 0 )
  {
	//QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWDoubleAltTest FAILED! returned = %ld", retVal);
	return 1;
  }

  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "------------------------ END API FUNCTIONAL QSEE Storage UNIT TESTS --------------------");

  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- -------START API ADVERSARIAL QSEE Storage  TESTS ---------------------------------");

  retVal = qsee_stor_AdversarialTest(partition_guid);
  if ( retVal != 0)
  {
	  return 1;
  }

  //RW Adversarial test cases
  retVal = qsee_stor_devRWAdversarialTest(partition_guid);
  if( retVal != 0 )
  {
	//QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWDoubleAltTest FAILED! returned = %ld", retVal);
	return 1;
  }

  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- -------START PROFILING READ/WRITE API TESTS ---------------------------------");
  retVal = qsee_stor_RWSProfileTest(partition_guid);
  if( retVal != 0 )
  {
	//QSEE_LOG(QSEE_LOG_MSG_ERROR, "   qsee_stor_RWSProfileTest FAILED! returned = %ld", retVal);
	return 1;
  }

  QSEE_LOG(QSEE_LOG_MSG_DEBUG, "-- -------PROFILING READ/WRITE TESTS END-------------------------------");
  return ret;
}

