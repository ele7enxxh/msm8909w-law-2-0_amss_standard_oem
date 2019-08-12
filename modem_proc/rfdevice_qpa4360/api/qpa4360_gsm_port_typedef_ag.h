#ifndef QPA4360_GSM_TYPEDEF_AG_H
#define QPA4360_GSM_TYPEDEF_AG_H


#include "comdef.h"

/*----------------------------------------------------------------------------*/
/*!
  It defines the QPA4360_GSM pa_port enum.
*/
typedef enum 
{
  QPA4360_GSM_GSM_BAND850_PORT_RFIN_2G_LB__RFOUT_2G_LB__850, 
  QPA4360_GSM_GSM_BAND900_PORT_RFIN_2G_LB__RFOUT_2G_LB__EGSM, 
  QPA4360_GSM_GSM_BAND1800_PORT_RFIN_2G_HB__RFOUT_2G_HB__DCS, 
  QPA4360_GSM_GSM_BAND1900_PORT_RFIN_2G_HB__RFOUT_2G_HB__PCS, 
  QPA4360_GSM_PORT_NUM, 
  QPA4360_GSM_PORT_INVALID, 
} qpa4360_gsm_pa_port_data_type;


#endif