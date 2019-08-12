/**
  @file policyman_cm.h

  @brief Interface to Policy Manager for Call Manager APIs
*/

/*
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
          All Rights Reserved.
   Qualcomm Confidential and Proprietary
*/

#ifndef _POLICYMAN_CM_H_
#define _POLICYMAN_CM_H_

/**  Database
*/

typedef struct
{
  sd_ss_e_type            stack;          /* Stack in operation - SD stack enum */
  sys_modem_as_id_e_type  asubs_id;       /* Subscription id*/
  sys_sys_id_type_e_type  id_type;        /* PLMN id type */
  sys_plmn_id_s_type      plmn;           /* PLMN of the serving system */
  sys_srv_domain_e_type   srv_domain;     /* Service Domain */
} policyman_cm_serving_info_t;



/*-------- policyman_cm_serving_info_cb --------*/
/**
@brief Get the serving system information from Call Manager.

@param[in]  pCmServingInfo    Pointer to CM serving system information.

@return
  None

*/

void
policyman_cm_serving_info_cb(
  policyman_cm_serving_info_t *pCmServingInfo
  );


#endif /* _POLICYMAN_CM_H_ */

