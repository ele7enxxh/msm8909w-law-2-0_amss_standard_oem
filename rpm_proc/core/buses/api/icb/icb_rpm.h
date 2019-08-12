#ifndef ICB_RPM_H
#define ICB_RPM_H
/*
===========================================================================

FILE:         icb_rpm.h

DESCRIPTION:  Header file for the ICB-on-RPM library.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/api/icb/icb_rpm.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/03/04   sds     Annotated save and restore APIs to note that they expect
                     all required clocks to be externally enabled.
2013/04/02   sds     Added hardware save and restore apis.
2012/01/19   sds     Moved and renamed.
2011/11/08   sds     Initial revision.

===========================================================================
             Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary and Confidential
===========================================================================
*/

/**
@brief This function initializes the ICB lower layer.
*/
extern void icb_init( void );

/**
@brief This function is to be called when icb hardware state should be saved.
@pre All required clocks to perform this operation must be enabled.
*/
void icb_hw_save( void );

/**
@brief This function is to be called when icb hardware state should be restored
       from a previously saved state.
@pre All required clocks to perform this operation must be enabled.
*/
void icb_hw_restore( void );

#endif /* ICB_RPM_H */

