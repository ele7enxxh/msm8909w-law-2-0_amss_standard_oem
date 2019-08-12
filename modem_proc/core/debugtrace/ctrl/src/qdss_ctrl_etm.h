#ifndef QDSS_CTRL_ETM_H
#define QDSS_CTRL_ETM_H

/*=============================================================================

FILE:         qdss_ctrl_etm.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

int qdss_ctrl_etm_enable(unsigned char);
int qdss_ctrl_etm_disable(void);

int qdss_ctrl_etm_rpm_set(unsigned char state);


#endif //QDSS_CTRL_ETM_H
