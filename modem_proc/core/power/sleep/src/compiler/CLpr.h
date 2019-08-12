#ifndef CLPR_H
#define CLPR_H

/*=============================================================================

  FILE:            CLpr.h
  
  OVERVIEW:        This file defines a class representation of sleep_lpr struct
                   with some additional fields. This representation will be
                   used by sleep solver. Functions in this files should not be
                   called by LPR owners. This class is for offline synthesis
                   version of sleep.
 
                   Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/core/power/sleep/rel/1h10/src/common/compiler/offline_synthesis/CLpr.h#3

=============================================================================*/

#include "sleepi.h"
#include "LprTypes.h"
#include "CLprRegistry.h"
#include "CLprm.h"


/* 
===============================================================================

                             CLASS DEFINITION

===============================================================================
*/


/**
 * ~CLpr
 *
 * @brief Destructor for the class.
 * 
 * Releases all the resources this object has.
 */
extern void CLpr_deinit(void);

/**
 * createSynth
 *
 * @brief The method returns CLpr representation of input presynthesized
 *        lpr.
 *
 * Since there is no public constructor, this is is the only way to create
 * CLpr for presynthesized LPRs.
 *
 * @param lpr: Input presynthesized lpr 
 *
 * @return Returns pointer to newly created CLpr object.
 */
extern CLpr* CLpr_createSynth(sleep_lpr_synth *lpr);

/**
 * getStatus
 *
 * @brief Function to see if this LPR enabled.
 *
 * The LPR is enabled if it has any mode which is selected by sleep solver 
 * as an enabled (or entered) mode during idle time.
 *        
 * @return Returns SLEEP_ENABLED if the LPR is enabled else SLEEP_DISABLED.
 */
extern sleep_status CLpr_getStatus(CLpr *hCLpr);

/**
 * setRegistry
 *
 * @brief Set the registry field of the CLpr to the input registry.
 *
 * @param registry: Registry in which we want to register this LPR.
 */
extern void CLpr_setRegistry(CLpr *hCLpr, CLprRegistry* registry);

/**
 * getModesList
 *
 * @brief This function is used to obtain modes for the LPR.
 *
 * @return List of modes of the this LPR.
 */
static inline CLprm** CLpr_getModesList(CLpr *hCLpr)
{ 
  return hCLpr->m_modes;
}

/**
 * getNumberEnabledModes
 *
 * @brief To inquire the number of enabled modes this CLpr has.
 *
 * @return Returns how many enabled modes are present in this CLpr.
 */
static inline unsigned int CLpr_getNumberEnabledModes(CLpr *hCLpr)
{ 
  return hCLpr->enabled_mode_count;
}
 
/**
 * getSharingCores
 *
 * @brief Returns the bit mask containing 1's for the cores that share this
 *        CLpr
 */
static inline unsigned int CLpr_getSharingCores(CLpr *hCLpr)
{
  return hCLpr->sharing_cores; 
}

/**
 * modeUpdated
 *
 * @brief This function updates the status of this CLpr with the registry
 *        according to input mode.
 *
 * The status of the CLpr is said to be changed when number of enabled modes
 * is reduced to zero from non-zero or vice versa.
 *
 * @param mode: Mode when made enabled/disabled may cause change in the
 *              status of CLpr.
 */
extern void CLpr_modeUpdated(CLpr *hCLpr, CLprm* mode);

/**
 * updateModes
 *
 * @brief Updates the status of modes of this LPR
 */
extern void CLpr_updateModes(CLpr *hCLpr);

/**
 * CLpr
 *
 * @brief Private constructor for CLpr class
 *
 * @param Pointer to the presynthesized LPR for which we want CLpr object to 
 *        be created.
 */
extern void CLpr_initSynth(CLpr *hCLpr, sleep_lpr_synth *lpr);


#endif // CLPR_H
