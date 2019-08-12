#ifndef QFE4373FC_TYPEDEF_AG_H
#define QFE4373FC_TYPEDEF_AG_H


#include "comdef.h"

/*----------------------------------------------------------------------------*/
/*!
   It defines the QFE4373FC pa_port enum.
*/
typedef enum 
{
  QFE4373FC_LTE_BAND34_PORT_TX_PORT_0, 
  QFE4373FC_LTE_BAND39_PORT_TX_PORT_0, 
  QFE4373FC_LTE_BAND7_PORT_TX_PORT_2, 
  QFE4373FC_LTE_BAND40_PORT_TX_PORT_3, 
  QFE4373FC_LTE_BAND38_PORT_TX_PORT_4, 
  QFE4373FC_LTE_BAND41_PORT_TX_PORT_4, 
  QFE4373FC_LTE_BAND41_B_PORT_TX_PORT_4, 
  QFE4373FC_LTE_BAND41_C_PORT_TX_PORT_4, 
  QFE4373FC_TDSCDMA_BAND34_PORT_TX_PORT_1, 
  QFE4373FC_TDSCDMA_BAND39_PORT_TX_PORT_1, 
  QFE4373FC_PORT_NUM, 
  QFE4373FC_PORT_INVALID, 
} qfe4373fc_pa_port_data_type;

#include "qfe4373fc_v2p0_port_typedef_ag.h"
#include "qfe4373fc_v2p51_port_typedef_ag.h"
#include "qfe4373fc_gsm_port_typedef_ag.h"
#include "qfe4373fc_v2p0_gsm_port_typedef_ag.h"
#include "qfe4373fc_ept_port_typedef_ag.h"
#include "qfe4373fc_v2p51_ept_port_typedef_ag.h"

#endif