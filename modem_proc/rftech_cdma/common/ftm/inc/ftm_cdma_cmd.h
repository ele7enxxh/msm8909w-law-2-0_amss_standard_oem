#ifndef FTM_CDMA_CMD_H
#define FTM_CDMA_CMD_H

/*!
  @file
  CDMA FTM Command

  @brief
  This module contains list of FTM commands specific to 1x and HDR.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_cmd.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/10/13   aro     Migrated CDMA commands
04/02/13   aro     Command for enable debug level for calv3
03/01/13   aro     Added command to perform HW timer test
01/23/13   aro     Command to perform slave resource configuration in 
                   Tx resource manager
11/08/12   cd      Removed common radio Tx AGC override
08/28/12   rsr     Added support for TXAGC override.
08/01/12   aro     Added EPT override Test function
06/21/12    vb     Added support for QFE1510
06/08/12   aro     EPT IQ capture command
03/18/12   aro     Added Start and End commands for each tech 
11/22/11   aro     Added interface to disable iC for debug purpose
11/12/11   aro     Moved CDMA common commands from 1x to CDMA module
11/12/11   aro     Added cdma common radio commands
11/12/11   aro     documentation update
10/13/11   bmg     FTM RFM interface support to set SAR state.
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
08/29/11   aro     Created a Driver verification sandbox
08/12/11   aro     Added support for HDR LNA override clear
08/11/11   aro     Moved Cal v3 configure rx to be done under ISR context
08/08/11   aro     Added commands to test calv3 sequence and calv3 config rx
08/03/11   aro     Documentation Update
07/27/11   aro     Added CDMA Cal v3 commands to config, retune and tear
                   down radio
07/12/11   aro     Added support to query intelliceiver power mode
07/01/11   aro     Added Debug interface to override Power Mode
06/13/11   aro     Added 1x Tx Override command
05/16/11   aro     Added Clear LNA override interface
03/16/11   aro     Added C++ protection
02/18/11   aro     Fixed bad command ID
02/18/11   aro     Updated Command ID
02/17/11   aro     Added FTM_RFM_C request packet definition documentation
02/12/11   aro     Fixed the command number for new iterface
02/12/11   aro     Compile Error Fix 
02/12/11   aro     Moved around command
02/08/11   aro     Created QDART Doxygen group
02/08/11   aro     Changed CDMA Radio Command Names
02/08/11   aro     Added CDMA Radio Control commands
01/31/11   aro     Initial Release : Re-organized 1xHDR FTM Files

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_command_id.h"

#endif /* FTM_CDMA_CMD_H */

