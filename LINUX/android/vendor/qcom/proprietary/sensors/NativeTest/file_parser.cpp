/*============================================================================
  @file file_parser.cpp

  @brief
    Loop back test for inserting sensor events which can be received back at sensor HAL.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
  ==========================================================================*/

#include "file_parser.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cutils/log.h>


#define MAX_ROWS 1024

int data[MAX_ROWS][MAX_DATA_IN_ROW];
FILE *pInputFile;
int validDataFlag = 0;
int stopFlag;
const char sensorNames[TOTAL_NUM_OF_SENSORS][50] = {"MPU6050-accel",
                                                    "proximity",
                                                    "light",
                                                    "gyroscope"
                                                    };

int checkIfComment(char *pLine)
{ /* This function checks if a given line from the text file is comment or not. */
    pLine = strtok(pLine, " \t");
    if(pLine != NULL && pLine[0] == '#')
    {/* the Line is a comment. */
        return 1;
    }
    return 0;
}

int getSensorID(char *pSensorName)
{ /*This function to retrieve the sensor id using the name of the sensor. */
    int i;
    int id = -1;
    for(i = 0; i < TOTAL_NUM_OF_SENSORS; i++)
    {
        if(strcmp(pSensorName, sensorNames[i]) == 0)
        {
            id = i;
            break;
        }
    }
    return id;
}

int openFile(char * pFileName)
{
    if(pFileName == NULL)
    {
        printf("Error: Invalid File name [in function openFile()]\r\n");
        return -1;
    }
    pInputFile = NULL;
    pInputFile = fopen(pFileName, "r");
    if(pInputFile == NULL)
    {
        printf("Error: Failed to open file %s [in function openFile()]\r\n", pFileName);
        return -1;
    }
    return 0;
}

void closeFile()
{
    if(pInputFile != NULL)
    {
        fclose(pInputFile);
    }
}
int parseFile()
{/* This function is for parsing the given file and filling up the data array. */
    char sLine[100];
    char tempLine[100];
    char *pLine;
    int row = 0;
    int col  = 0;
    int invalidDataFlag = 0;
    int length;

    while(1)
    {
        if(fgets(sLine , sizeof(sLine) , pInputFile) == NULL)
        {
            if(validDataFlag)
            {
                fseek (pInputFile , 0 , SEEK_SET );
                continue;
            }
            else
            {
                printf("Error: No valid data in file [in function parseFile()]\r\n");
                return -1;
            }
        }
        invalidDataFlag =1;
        col = 0;
        if(checkIfComment(sLine))
        {/* The line is a comment. So skipping the line. */
            continue;
        }
        length = strlen(sLine);
        while(sLine[length-1] == 0xd || sLine[length-1] == 0xa)
        {
            if(length < 3)
            {
            break;
            }
            sLine[length-1] = 0; /*To avoid \n and \r at the end of line. */
            length = strlen(sLine);
        }
        if(length < 3)
        {/*Invalid line. So skipping the line.*/
            continue;
        }
        strcpy(tempLine, sLine);
        pLine = tempLine;
        pLine = strtok(pLine, ";");
        if(pLine == NULL) {
            printf("Error %s\n",pLine);
        }
        while(pLine != NULL)
        {
            if(col == 0)
            {
                data[row][col] = getSensorID(pLine);
                if(data[row][col] < 0)
                {/* The sensor name in the file is invalid. */
                    invalidDataFlag = 1;
                    printf("Information: Invalid sensor name\r\n");
                    break;
                }else{
                    invalidDataFlag = 0;
                }
            }
            else
            {
                data[row][col] = (int)atof(pLine);
            }
            pLine = strtok(NULL, ";");
            col++;
            if(col >= MIN_DATA_IN_ROW)
            {
                validDataFlag = 1;
            }
        }
        if(invalidDataFlag)
        {
            printf("Information: Skipping the line \"%s\". Invalid Data\r\n", sLine);
            continue;
        }
        row++;
        if(row >= MAX_ROWS)
        {
            break;
        }
    }
    return 0;
}

int readFile(char * fileName)
{ /*This function to retrieve the sensor name using the sensor ID. */
    if(openFile(fileName))
    {
        printf("Error: Opening of file %s failed [in readFile()]\r\n", fileName);
        return -1;
    }
    validDataFlag = 0;
    if(parseFile())
    {
        printf("Error: Failed to read from file. Invalid Data [in readFile()]\r\n");
        return -1;
    }
    if(validDataFlag == 1)
    {
        fseek(pInputFile, 0, SEEK_SET );
    }
    else
    {
        printf("Error: Failed to read from file. Invalid Data [in readFile()]\r\n");
        return -1;
    }
    return 0;
}


int getData(int *value, int cnt)
{
    int index;
    int i;
    index = cnt%MAX_ROWS;
    if(index == 0)
    {
        if(parseFile())
        {
        printf("Error: Failed reading values from the file (in function getData()]\r\n");
        return -1;
        }
    }
    for(i = 0; i < MAX_DATA_IN_ROW; i++){
        value[i] = data[index][i];
    }
    return 0;
}

void alarmHandler(int signalVal)
{
    stopFlag = 1;
}

void setAlarm(int duration)
{
    signal(SIGALRM, alarmHandler);
    alarm(duration);
}
