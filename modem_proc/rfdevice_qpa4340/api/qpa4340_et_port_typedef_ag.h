#ifndef QPA4340_ET_TYPEDEF_AG_H
#define QPA4340_ET_TYPEDEF_AG_H


#include "comdef.h"

/*----------------------------------------------------------------------------*/
/*!
  It defines the QPA4340_ET pa_port enum.
*/
typedef enum 
{
  QPA4340_ET_LTE_BAND7_PORT_HB, 
  QPA4340_ET_LTE_BAND38_PORT_HB, 
  QPA4340_ET_LTE_BAND38_PORT_HB_B41, 
  QPA4340_ET_LTE_BAND40_PORT_HB, 
  QPA4340_ET_LTE_BAND41_PORT_HB, 
  QPA4340_ET_LTE_BAND41_B_PORT_HB, 
  QPA4340_ET_LTE_BAND41_C_PORT_HB, 
  QPA4340_ET_LTE_BAND30_PORT_HB, 
  QPA4340_ET_PORT_NUM, 
  QPA4340_ET_PORT_INVALID, 
} qpa4340_et_pa_port_data_type;


#endif