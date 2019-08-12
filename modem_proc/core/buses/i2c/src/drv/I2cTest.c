
//DEBUG
#include "I2cDriver.h"
#include "rcinit.h"

int32 i2c_read_eeprom(I2cDrv_I2cBus *pI2c, uint32 offset, uint8* pRdBuff, uint32 size)
{
    I2cIoResult     ioRes;
    I2cRegSeqObj        i2c_seq_obj;

    I2c_RegObj_InitRead(&i2c_seq_obj);
    I2c_RegObj_SetOffset(&i2c_seq_obj, offset, I2cRegOffset_16Bit);
    I2c_RegObj_SetReadIoVector(&i2c_seq_obj, pRdBuff, size);

    return I2cDrv_BatchTransfer(pI2c, &(i2c_seq_obj.seq), &ioRes);
}

int32 i2c_write_eeprom(I2cDrv_I2cBus *pI2c, uint32 offset, uint8* pBuff, uint32 size)
{
    I2cIoResult     ioRes;
    I2cRegSeqObj        i2c_seq_obj;

    I2c_RegObj_InitWrite(&i2c_seq_obj);
    I2c_RegObj_SetOffset(&i2c_seq_obj, offset, I2cRegOffset_16Bit);
    I2c_RegObj_SetWriteIoVector(&i2c_seq_obj, pBuff, size);

    return I2cDrv_BatchTransfer(pI2c, &(i2c_seq_obj.seq), &ioRes);

}

 uint8          store_data[10]={0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xA5,0x5A,0xA5,0x5A};
 uint8          rd_data[10]={0xA5,0x5A,0xA5,0x5A,0xA5, 0x5A,0xA5,0x5A,0xA5,0x5A};
 uint8          wr_data[10] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  

  I2cResult i2cErrorCode = I2C_RES_SUCCESS;
   int32          i2cResult = I2C_RES_ERROR_CLS_DEV_OS;
   int32          ind;
   int32          i;
   int32          testCnt = 2;
   I2cDrv_I2cBus i2c_bus;
   int32          testRes;
   int32          misMatch;
 
   I2cDrvPnocVoteData voteOn_data;
   I2cDrvPnocVoteData voteOff_data;
  
uint32 i2cId = I2CDRV_I2C_1;
  
void i2c_test(void)
{

  for(i=0; i < testCnt; i++)
  {

   i2c_bus.clntCfg.uSlaveAddr = 0x56;
   i2c_bus.clntCfg.uBusFreqKhz = 400;
   i2c_bus.clntCfg.uByteTransferTimeoutUs = 2500;
   testRes = 0;
   misMatch = 0;
      
   voteOn_data.uPnocAb = 10000;
   voteOn_data.uPnocIb = 10000;
   
   voteOff_data.uPnocAb = 0;
   voteOff_data.uPnocIb = 0;
   
   i2cResult = I2cDrv_Open(i2cId, &i2c_bus, 0);
   if (I2C_RES_SUCCESS != i2cResult)
   {
	  testRes = 10;
      return;     
   }

   
   i2cResult = I2cDrv_Cmd(&i2c_bus, I2cDrvCmd_Vote_I2cClocsOn, NULL);
   
   if (I2C_RES_SUCCESS != i2cResult)
   {
   	  testRes = 30;
      return;     
   }
      
   // store the data
   i2cResult = i2c_read_eeprom(&i2c_bus, 0x3000,store_data, 10);
   
   if (I2C_RES_SUCCESS != i2cResult)
   {
      testRes = 40;
      return;     
   }
   
    i2cResult = i2c_write_eeprom(&i2c_bus, 0x3000,wr_data, 10);
    
	if (I2C_RES_SUCCESS != i2cResult)
    {
	  testRes = 50;
      return;     
    }
    
	i2cResult = i2c_read_eeprom(&i2c_bus, 0x3000,rd_data, 10);
	
	if (I2C_RES_SUCCESS != i2cResult)
    {
	  testRes = 60;
      return;     
    }
   for (ind=0; ind<10;ind++)
   {
      if (rd_data[ind] != wr_data[ind]) 
	  {
	    misMatch = 0x1;
	    break;
	  }
   }

   // restore previous content
   i2cResult = i2c_write_eeprom(&i2c_bus, 0x3000,store_data, 10);
   
   if (I2C_RES_SUCCESS != i2cResult)
   {
	  testRes = 70;
      return;     
   }
   
   
   i2cResult = I2cDrv_Cmd(&i2c_bus, I2cDrvCmd_Vote_I2cClocsOff, NULL);
   
   if (I2C_RES_SUCCESS != i2cResult)
   {
	  testRes = 90;
      return;     
   }
   
   i2cResult = I2cDrv_Close(&i2c_bus);
   
   if (I2C_RES_SUCCESS != i2cResult)
   {
	  testRes = 100;
      return;     
   }
 }
  
}

static uint32 test_variable = 0x2;

void i2c_test_srv_task(void)
{
  rcinit_handshake_startup(); // required rcinit handshake

  for(;;)
  {
     if (test_variable == 0x1 )
     {
         i2c_test();
         test_variable = test_variable + 2 ;
         
     }
     else
     {
		test_variable = 0xfc;
     }
     test_variable++;
  }
}





