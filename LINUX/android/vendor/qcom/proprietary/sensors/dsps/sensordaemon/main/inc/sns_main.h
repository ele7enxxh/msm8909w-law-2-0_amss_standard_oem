/*============================================================================
  @file sns_main.h

  @brief Header file for sns_main.h. Provides externalized functions for the
  Linux Android sensors library.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/

#define SNS_USERNAME "sensors"
#define SNS_GROUPNAME "sensors"

#define USERNAME_NOBODY "nobody"
#define GROUPNAME_NOBODY "nobody"

/*============================================================================
  Function Declarations
  ============================================================================*/

/*===========================================================================

  FUNCTION:   sns_main_exit

  ===========================================================================*/
/*!
  @brief Causes the main thread to exit.
*/
/*=========================================================================*/
void sns_main_exit( void );
