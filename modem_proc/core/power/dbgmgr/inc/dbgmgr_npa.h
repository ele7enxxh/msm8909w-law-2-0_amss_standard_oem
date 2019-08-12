#ifndef DBGMGR_NPA_H
#define DBGMGR_NPA_H

extern int dbgmgr_mode_idx; 
extern int dbgmgr_protocol;

typedef enum
{
  DBGMGR_NPA_NODE_LIST_CTRL_DISABLE = 0,
  DBGMGR_NPA_NODE_LIST_CTRL_ENABLE
} dbgmgr_npa_node_list_ctrl_t; 

typedef enum
{
  DBGMGR_CFG_START = 0,
  DBGMGR_CFG_MODE = DBGMGR_CFG_START, 
  DBGMGR_CFG_PROTO, 
  DBGMGR_CFG_END
} dbgmgr_cfg_t;


/**
  @brief This function handles the npa_node_list_enable/disable command.
  Essentially a list of npa nodes along that specifies the nodes to which
  we require to enable or disable logs

  @param node_list [in] Valid pointer to the start of the node list string
  @param list_sz   [in] Size of the string
  @param ctrl      [in] Command issued for the list

  @return
  DAL_SUCCESS                 -- NPA node list was processed .
  DAL_ERROR_INVALID_PARAMETER -- Bad parameters were passed into API.
  DAL_ERROR_NOT_SUPPORTED     -- API not supported on the platform.
  DM_ERROR                    -- All other errors.
*/
int dbgmgr_npa_node_list_ctrl
(
  const char* node_list,
  size_t list_sz,
  dbgmgr_npa_node_list_ctrl_t ctrl
);

/**
  @brief This function handles the NPA ULOG configuration commands.
  ULOG protocol number along with mode are some examples of supported
  commands.

  @param in       [in] Valid string containing command payload
  @param in_sz    [in] Size of the string
  @param ctrl     [in] Command issued for the list

  @return
  DAL_SUCCESS                 -- NPA config command was processed.
  DAL_ERROR_INVALID_PARAMETER -- Bad parameters were passed into API.
  DAL_ERROR_NOT_SUPPORTED     -- API not supported on the platform.
  DM_ERROR                    -- All other errors.
*/
int dbgmgr_ctrl
(
  const char* node_list,
  size_t list_sz,
  dbgmgr_cfg_t ctrl
);

#endif //DBGMGR_NPA_H
