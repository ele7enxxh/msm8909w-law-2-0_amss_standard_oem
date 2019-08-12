// GenKeyScript.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>

typedef unsigned long uint32;
typedef unsigned __int64 uint64;
typedef unsigned char uint8;
typedef unsigned short uint16;

typedef struct _DEVICE_KEY_STRUCT
{
	uint8 row1;
	uint8 row2;
	uint8 key0;
	uint8 key1;
	uint8 key2;
	uint8 key3;
	uint8 key4;
	uint8 key5;
	uint8 key6;
}DEVICE_KEY_STRUCT,*PDEVICE_KEY_STRUCT;

uint32 byteSwap(uint32 value)
{
	uint8* p_value = (uint8*) &value;

	uint32 newValue;
	uint8* p_newValue  = (uint8*) &newValue;

	p_newValue[0] = p_value[3];
	p_newValue[1] = p_value[2];
	p_newValue[2] = p_value[1];
	p_newValue[3] = p_value[0];

	return newValue;
}

bool getDeviceKeys(char* fileName, uint32 index, DEVICE_KEY_STRUCT keys[16])
{
	FILE* p_file = fopen(fileName, "rb");

	if(!p_file)
	{
		printf("Error: Unable to open file %s\n", fileName);
		return false;
	}

	if(fseek(p_file, 5, SEEK_SET) != 0)
	{
		printf("Error: Problem reading file %s\n", fileName);
		fclose(p_file);
		return false;
	}

	//Get the number of key sets in the file
	uint32 keySetCount;
	if(fread(&keySetCount, 4, 1, p_file) != 1)
	{
		printf("Error: Problem reading number of key sets in file: %s\n", fileName);
		fclose(p_file);
		return false;
	}

	//Get the size of each key set
	uint32 keySetSize;
	if(fread(&keySetSize, 4, 1, p_file) != 1)
	{
		printf("Error: Problem reading key set size in file: %s\n", fileName);
		fclose(p_file);
		return false;
	}

	keySetCount = byteSwap(keySetCount);
	keySetSize = byteSwap(keySetSize);

	//Skip to the key record set requested
	if(fseek(p_file, keySetSize * index, SEEK_CUR) != 0)
	{
		printf("Error: Problem reading key set in file %s\n", fileName);
		fclose(p_file);
		return false;
	}

	//Read the row info from the file
	uint32 column;
	uint8 rowBytes[4];
	for(column = 0; column < 16; column++)
	{
		if(fread(rowBytes, 4, 1, p_file) != 1)
		{
			printf("Error: Problem reading row value in file %s\n", fileName);
			fclose(p_file);
			return false;
		}
		
		keys[column].row1 = rowBytes[2];
		keys[column].row2 = rowBytes[3];
	}

	//Read the 16 keys from the file
	uint8 keyBytes[7];
	for(column = 0; column < 16; column++)
	{
		if(fread(keyBytes, 7, 1, p_file) != 1)
		{
			printf("Error: Problem reading key value in file %s\n", fileName);
			fclose(p_file);
			return false;
		}
		
		keys[column].key0 = keyBytes[0]; 
		keys[column].key1 = keyBytes[1]; 
		keys[column].key2 = keyBytes[2]; 
		keys[column].key3 = keyBytes[3]; 
		keys[column].key4 = keyBytes[4]; 
		keys[column].key5 = keyBytes[5]; 
		keys[column].key6 = keyBytes[6]; 
	}

	return true;
}

bool generateKeyFile(char* fileName, DEVICE_KEY_STRUCT keys[16], uint32 mkbIndex)
{
	//Generate the data to write and store it in KeyArray
	uint8 KeyArray[160];
	uint32 index = 0;
	uint32 column;

    //Get the 16 keys and row information
    for (column = 0; column < 16; column++)
	{
		KeyArray[index++] = (uint8) mkbIndex;
		KeyArray[index++] =  keys[column].row1;
		KeyArray[index++] =  keys[column].row2;
		KeyArray[index++] =  keys[column].key0;
		KeyArray[index++] =  keys[column].key1;
		KeyArray[index++] =  keys[column].key2;
		KeyArray[index++] =  keys[column].key3;
		KeyArray[index++] =  keys[column].key4;
		KeyArray[index++] =  keys[column].key5;
		KeyArray[index++] =  keys[column].key6;
	}

    //Write the data out to the file
	FILE* pfile = fopen(fileName, "w");
	if(pfile)
	{
	    // Write out comment
	    fprintf(pfile, "!CPRM Device Key Provisioning data\n");
	   
	    // Write out system, subsystem, cmd id, length
	    fprintf(pfile, "{75, 58, 4, 0, 160, 0, 0, 0");

	    // Write out Key array
		for(index = 0; index < sizeof(KeyArray); index++)
		{   
		    fprintf(pfile, ", %d", KeyArray[index]);
		}

	    fprintf(pfile, "}");

		fclose(pfile);

		printf("Generated output file %s\n", fileName);
		return true;

 //	    fwrite(&KeyArray, 1, sizeof(KeyArray), pfile);
 //   	fprintf(pfile, "V data_for_tx%s[%d] = 0x%x\n", mkbIndexString, i, rowArray[i]);
 //   	fprintf(pfile, "V test_data%d_valid = 1", mkbIndex);
	}

	printf("Error: Problem opening file %s\n", fileName);
	return false;
}

int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		printf("Usage: GenKeyFile  keyFile  keySetIndex  mkbIndex\n");
		printf("------------------------------------------------------\n");
		printf("keyFile - Location of a 4C file containing key data\n");
		printf("keySetIndex - 0-based index of the key set record in the 4C key file\n");
		printf("mkbIndex - MKB index that the key file matches.  (0 - 15 are supported)\n");
		return 0;
	}

	//Command line parameters
	char*  inputFile = argv[1];
	uint32 inputFileIndex = atoi(argv[2]);
    char*  mkbIndexA = argv[3];
    uint32 mkbIndex = atoi(argv[3]);

	//Validate the MKB Index
	if(mkbIndex > 15)
	{
		printf("Unsupported MKB index.  Use 0 - 15");
		return 0;
	}

	//Define base key file name in the current directory
	//to simplify the command line
	char KeyOutputFile[] = "DevkeysMkb00.inp";
    
    //Modify filename based on MKB index since we need to create
    //a different file for MKB0 -  MKB15 keys
    if (mkbIndex < 10)
    {
        KeyOutputFile[11] = mkbIndexA[0];
    }

    else
    {
        KeyOutputFile[10] = mkbIndexA[0];
        KeyOutputFile[11] = mkbIndexA[1];
    }

    //Get the selected device key record from the input 4C key file
	DEVICE_KEY_STRUCT keys[16];
	if(getDeviceKeys(inputFile, inputFileIndex, keys))
	{
		//If we got the keys successfully generate the output key file
		generateKeyFile(KeyOutputFile, keys, mkbIndex);
    }
  
	return 0;
}

