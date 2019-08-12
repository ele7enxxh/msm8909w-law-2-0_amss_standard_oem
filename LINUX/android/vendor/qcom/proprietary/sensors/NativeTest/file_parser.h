/*============================================================================
  @file file_parser.h

  @brief
    Loop back test for inserting sensor events which can be received back at sensor HAL.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
  ==========================================================================*/
#define ACCEL_DEVICE_ID     0
#define PROXIMITY_DEVICE_ID 1
#define LIGHT_DEVICE_ID     2
#define GYROSCOPE_DEVICE_ID 3

#define TOTAL_NUM_OF_SENSORS  4

#define MAX_DATA_IN_ROW  6 /* Maximum number of values present in any valid row (including 2 fields for timestamp, one for sec and other for nanosecond) */
#define MIN_DATA_IN_ROW  4 /* Minimum number of values present in any valid row (including 2 fields for timestamp, one for sec and other for nanosecond) */

extern int stopFlag;
extern const char sensorNames[TOTAL_NUM_OF_SENSORS][50];

int readFile(char * fileName);
int getData(int *value, int cnt);
void setAlarm(int duration);
int getSensorID(char *pSensorName);
