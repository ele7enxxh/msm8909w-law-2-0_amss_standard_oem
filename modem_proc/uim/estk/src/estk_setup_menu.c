/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SETUP MENU


GENERAL DESCRIPTION :
  Enhanced STK layer handles SETUP MENU commands from multiple (U)SIM cards

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights
Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_setup_menu.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/16   gs      F3 Logging inprovements
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
10/04/13   gm      F3 message reduction
09/13/13   gm      Support for Recovery and hot-swap
06/20/13   vr      Triton TSTS changes
06/11/12   bd      Fix to to correct exact cmd num to upper layers
01/05/11   nb      Added NULL pointer check before accessing callback pointer
12/16/10   nb      Added definition of a dummy enevlope callback function
                   Pass envelope callback pointer back to GSTK
11/02/10   bd      Fix for correctly handling menu selection
09/13/10   xz      Add support of SETUP MENU in raw APDU for DS/DS
09/07/10   bd      Use the title from Slot 2 if card is not inserted in Slot 1
02/05/10   xz      Added support of DS/DS network related command
09/21/09   xz      Initial version (Dual SIM Dual Standby implementation)
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "uim_msg.h"

#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
#include "estk.h"
#include "estk_priv.h"
#include "intconv.h"
#include "gstkutil.h"

#define ESTK_SETUP_MENU_RAW_CMD_BUFFER_SIZE 255

/*===========================================================================

FUNCTION: estk_free_setup_menu_data

DESCRIPTION:
  Free memory allocated for SETUP MENU cmd

PARAMETERS:
  void

DEPENDENCIES:
  cmd_ptr Pointer to the setup menu command to be freed

RETURN VALUE:
  gstk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
void estk_free_setup_menu_data(
  gstk_setup_menu_req_type *cmd_ptr
)
{
  uint32 i = 0;

  if (cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr!");
    return;
  }

  if (cmd_ptr->title.text != NULL)
  {
    gstk_free(cmd_ptr->title.text);
    cmd_ptr->title.text = NULL;
  }

  for(i = 0; i < cmd_ptr->num_items && i < GSTK_MAX_MENU_ITEMS; ++i)
  {
    if (cmd_ptr->items[i].item_text != NULL )
    {
      gstk_free(cmd_ptr->items[i].item_text);
      cmd_ptr->items[i].item_text = NULL;
    }
  }

  if (cmd_ptr->next_action_indicator.next_action_list != NULL)
  {
    gstk_free(cmd_ptr->next_action_indicator.next_action_list);
    cmd_ptr->next_action_indicator.next_action_list = NULL;
  }

  for (i = 0; i < cmd_ptr->icon_id_list.num_items; ++i)
  {
    if (cmd_ptr->icon_id_list.icon_list[i] != NULL)
    {
      if (cmd_ptr->icon_id_list.icon_list[i]->data != NULL)
      {
        gstk_free(cmd_ptr->icon_id_list.icon_list[i]->data);
        cmd_ptr->icon_id_list.icon_list[i]->data = NULL;
      }
      gstk_free(cmd_ptr->icon_id_list.icon_list[i]);
      cmd_ptr->icon_id_list.icon_list[i] = NULL;
    }
  }

  if (cmd_ptr->icon.data != NULL)
  {
    gstk_free(cmd_ptr->icon.data);
    cmd_ptr->icon.data = NULL;
  }

  memset(cmd_ptr, 0x00, sizeof(gstk_setup_menu_req_type));
} /*estk_free_setup_menu_data*/

/*===========================================================================

FUNCTION: estk_save_curr_setup_menu_cmd

DESCRIPTION:
  Copy SETUP MENU cmd data

PARAMETERS:
  dest_ptr Pointer to where the data to be copied to
  src_ptr  Pointer to hwere the data to be copied from

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_copy_setup_menu_data(
  gstk_setup_menu_req_type       *dest_ptr,
  const gstk_setup_menu_req_type *src_ptr
)
{
  uint32                   i               = 0;
  uint32                   len             = 0;
  uint8                    *buff[GSTK_MAX_MENU_ITEMS * 4] = {0};
  uint32                   idx             = 0;
  uint32                   step            = 0;
  gstk_setup_menu_req_type setup_menu_buff = {0};

  UIM_MSG_HIGH_0("estk_copy_setup_menu_data()");

  if (dest_ptr == NULL || src_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null Ptr! 0x%x, 0x%x", dest_ptr, src_ptr);
    return ESTK_ERROR;
  }

  len = sizeof(gstk_setup_menu_req_type);
  (void)gstk_memcpy(&setup_menu_buff, src_ptr, len, len, len);

  for (idx = 0, step = 0; idx < GSTK_MAX_MENU_ITEMS * 4; ++ step)
  {
    switch (step)
    {
    case 0:
      if (src_ptr->title.text != NULL)
      {
        buff[idx] = GSTK_CALLOC(src_ptr->title.length);
        if (buff[idx] == NULL)
        {
          goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
        }
        setup_menu_buff.title.text = buff[idx ++];
        (void)gstk_memcpy(setup_menu_buff.title.text,
                       src_ptr->title.text,
                       src_ptr->title.length,
                       src_ptr->title.length,
                       src_ptr->title.length);
      }
      break;

    case 1:
      for(i = 0; i < src_ptr->num_items && i < GSTK_MAX_MENU_ITEMS; ++i)
      {
        if (idx >= GSTK_MAX_MENU_ITEMS * 4)
        {
          UIM_MSG_ERR_0("don't have sufficient trace buffer!");
          goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
        }
        if (src_ptr->items[i].item_text != NULL )
        {
          buff[idx] = GSTK_CALLOC(src_ptr->items[i].length);
          if (buff[idx] == NULL)
          {
            goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
          }
          setup_menu_buff.items[i].item_text = buff[idx ++];
          (void)gstk_memcpy(setup_menu_buff.items[i].item_text,
                         src_ptr->items[i].item_text,
                         src_ptr->items[i].length,
                         src_ptr->items[i].length,
                         src_ptr->items[i].length);
        }
      }
      break;

    case 2:
      if (src_ptr->next_action_indicator.next_action_list != NULL)
      {
        buff[idx] = GSTK_CALLOC(src_ptr->next_action_indicator.num_items);
        if (buff[idx] == NULL)
        {
          goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
        }
        setup_menu_buff.next_action_indicator.next_action_list = buff[idx ++];
        (void)gstk_memcpy(setup_menu_buff.next_action_indicator.next_action_list,
                       src_ptr->next_action_indicator.next_action_list,
                       src_ptr->next_action_indicator.num_items,
                       src_ptr->next_action_indicator.num_items,
                       src_ptr->next_action_indicator.num_items);
      }
      break;

    case 3:
      for (i = 0;
           i < src_ptr->icon_id_list.num_items && i < GSTK_MAX_MENU_ITEMS;
           ++i)
      {
        if (src_ptr->icon_id_list.icon_list[i] != NULL)
        {
          if (idx >= GSTK_MAX_MENU_ITEMS * 4)
          {
            UIM_MSG_ERR_0("don't have sufficient trace buffer!");
            goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
          }
          len = sizeof(gstk_icon_type);
          buff[idx] = GSTK_CALLOC(len);
          if (buff[idx] == NULL)
          {
            goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
          }
          setup_menu_buff.icon_id_list.icon_list[i] =
            (gstk_icon_type *)buff[idx ++];
          (void)gstk_memcpy(setup_menu_buff.icon_id_list.icon_list[i],
                         src_ptr->icon_id_list.icon_list[i],
                         len,
                         len,
                         len);

          if (src_ptr->icon_id_list.icon_list[i]->data != NULL)
          {
            if (idx >= GSTK_MAX_MENU_ITEMS * 4)
            {
              UIM_MSG_ERR_0("don't have sufficient trace buffer!");
              goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
            }
            buff[idx] = GSTK_CALLOC(src_ptr->icon_id_list.icon_list[i]->size);
            if (buff[idx] == NULL)
            {
              goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
            }
            setup_menu_buff.icon_id_list.icon_list[i]->data = buff[idx ++];
            (void)gstk_memcpy(setup_menu_buff.icon_id_list.icon_list[i]->data,
                           src_ptr->icon_id_list.icon_list[i]->data,
                           src_ptr->icon_id_list.icon_list[i]->size,
                           src_ptr->icon_id_list.icon_list[i]->size,
                           src_ptr->icon_id_list.icon_list[i]->size);
          }
        }
      }
      break;

    case 4:
      if (src_ptr->icon.data != NULL)
      {
        buff[idx] = GSTK_CALLOC(src_ptr->icon.size);
        if (buff[idx] == NULL)
        {
          goto ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR;
        }
        setup_menu_buff.icon.data = buff[idx ++];
        (void)gstk_memcpy(setup_menu_buff.icon.data,
                       src_ptr->icon.data,
                       src_ptr->icon.size,
                       src_ptr->icon.size,
                       src_ptr->icon.size);
      }
      break;

    default:
      UIM_MSG_HIGH_1("copy data finishes! (trace buffer idx = 0x%x)", idx);
      idx = GSTK_MAX_MENU_ITEMS * 4;
      break;
    }
  }
  len = sizeof(gstk_setup_menu_req_type);
  (void)gstk_memcpy(dest_ptr, &setup_menu_buff, len, len, len);
  return ESTK_SUCCESS;

ESTK_COPY_SETUP_MENU_DATA_MALLOC_ERROR:
  UIM_MSG_ERR_3("malloc failed! 0x%x, 0x%x, 0x%x", step, idx, i);
  for (i = 0; i <GSTK_MAX_MENU_ITEMS * 4; ++ i)
  {
    if (buff[i] != NULL)
    {
      gstk_free(buff[i]);
    }
  }
  return ESTK_ERROR;
} /*estk_copy_setup_menu_data*/

/*===========================================================================

FUNCTION: estk_merge_setup_menu_cmd

DESCRIPTION:
  Merge the setup menu commands of all ESTK instances

PARAMETERS:
  out_ptr The pointer to the setup menu command after merge

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_merge_setup_menu_cmd(
  gstk_setup_menu_req_type *out_ptr
)
{
  uint32                i         = 0;
  uint8                 j         = 0;
  estk_priv_info_type   *inst_ptr = NULL;
  estk_result_enum_type result    = ESTK_SUCCESS;
  uint32                total     = 0;
  
  ESTK_RETURN_ERROR_IF_NULL_PTR(out_ptr, ESTK_ERROR);
  memset(out_ptr, 0x00, sizeof (gstk_setup_menu_req_type));

  for (i = 0; i < estk_num_of_instances; ++ i)
  {
    inst_ptr = estk_instances_ptr[i];
    if (i == 0)
    {
      result = estk_copy_setup_menu_data(out_ptr, &inst_ptr->setup_menu_cmd);
      if (result != ESTK_SUCCESS)
      {
        UIM_MSG_ERR_0("failed to copy 1st setup menu cmd!");
        return result;
      }
      for (j = 0; j < out_ptr->num_items; ++ j)
      {
        out_ptr->items[j].item_id = j;
      }
    }
    else
    {
      out_ptr->help_available |= inst_ptr->setup_menu_cmd.help_available;
      out_ptr->softkey_selection |= inst_ptr->setup_menu_cmd.softkey_selection;
      /*copy menu items*/
      total = out_ptr->num_items + inst_ptr->setup_menu_cmd.num_items;
      for (j = out_ptr->num_items; j < GSTK_MAX_MENU_ITEMS && j < total ; ++j)
      {
        out_ptr->items[j].item_id = j;
        out_ptr->items[j].length = 
          inst_ptr->setup_menu_cmd.items[j - out_ptr->num_items].length;
        if (out_ptr->items[j].length > 0)
        {
          out_ptr->items[j].item_text = GSTK_CALLOC(out_ptr->items[j].length);
          if (out_ptr->items[j].item_text == NULL)
          {
            (void)estk_free_setup_menu_data(out_ptr);
            return ESTK_ERROR;
          }
          (void)gstk_memcpy(out_ptr->items[j].item_text,
                         inst_ptr->setup_menu_cmd.items[j - out_ptr->num_items].item_text,
                         out_ptr->items[j].length,
                         out_ptr->items[j].length,
                         out_ptr->items[j].length);
        }
      }
      if (j == GSTK_MAX_MENU_ITEMS)
      {
        UIM_MSG_HIGH_3("trucating menu items. 0x%x, 0x%x, 0x%x",
                       i,
                       out_ptr->num_items,
                       inst_ptr->setup_menu_cmd.num_items);
      }
      out_ptr->num_items = j;

      /* don't merge those data for now:
       * 1) title (except for a condition below)
       * 2) default_item
       * 3) next_action_indicator
       * 4) icon
       * 5) icon_id_list
       * 
       * above values will be from SLOT 1
       */
      /* Conditionally merge the title, for the case in which card is is not inserted in Slot 1 */
      if (out_ptr->title.length == 0 && out_ptr->title.text == NULL && 
            inst_ptr->setup_menu_cmd.title.length != 0)
      {
        out_ptr->title.dcs = inst_ptr->setup_menu_cmd.title.dcs;
        out_ptr->title.length = inst_ptr->setup_menu_cmd.title.length;
        out_ptr->title.text = GSTK_CALLOC(inst_ptr->setup_menu_cmd.title.length);
        if (out_ptr->title.text == NULL)
        {
          return ESTK_ERROR;
        }
        (void)gstk_memcpy(out_ptr->title.text,
                       inst_ptr->setup_menu_cmd.title.text,
                       inst_ptr->setup_menu_cmd.title.length,
                       inst_ptr->setup_menu_cmd.title.length,
                       inst_ptr->setup_menu_cmd.title.length);
      }
    }
  }
  return ESTK_SUCCESS;

} /*estk_merge_setup_menu*/

/*===========================================================================

FUNCTION: estk_setup_menu_pack_tlv

DESCRIPTION:
  This function packs TLV

PARAMETERS:
  out_buff_ptr: [out] the buffer where the output to be copied
  out_buff_size:[in]  the max size of output buffer
  tag:          [in]  the tag
  len:          [in]  the length of value
  value_ptr     [in]  the pointer to the value buffer

DEPENDENCIES:
  None

RETURN VALUE:
  If packing succeeds, the fuction returns the length of bytes having been
  packed; otherwise, return -1

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
int32 estk_setup_menu_pack_tlv(
  uint8        *out_buff_ptr,
  uint32       out_buff_size,
  uint8        tag,
  uint32       len,
  const uint8  *value_ptr
)
{
  uint32    idx = 0;

  if (out_buff_ptr == NULL || value_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL PTR: 0x%x, 0x%x", out_buff_ptr, value_ptr);
    return -1;
  }

  /* pack tag */
  out_buff_ptr[idx++] = tag;

  /* pack length */
  if (len <= 127)
  {
    out_buff_ptr[idx++] = uint32touint8(len);
  }
  else if (len <= 255)
  {
    out_buff_ptr[idx++] = 0x81;
    out_buff_ptr[idx++] = uint32touint8(len);
  }
  else
  {
    UIM_MSG_ERR_1("invalid len: 0x%x", len);
    return -1;
  }

  /* pack value */
  if ((idx + len) > out_buff_size)
  {
    UIM_MSG_ERR_2("output buffer is too small (0x%x vs 0x%x)!",
                  out_buff_size, len);
    return -1;
  }
  if(gstk_memcpy(&out_buff_ptr[idx],
                 value_ptr,
                 len,
                 out_buff_size - idx,
                 len) < len)
  {
    return -1;
  }
  idx += len;

  return idx;
} /*estk_setup_menu_pack_tlv*/

/*===========================================================================

FUNCTION: estk_setup_menu_cmd_to_raw_fmt

DESCRIPTION:
  This function coverts SETUP MENU command from GSTK format to raw APDU format

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_setup_menu_cmd_to_raw_fmt(
  const gstk_setup_menu_req_type *cmd_ptr,
  gstk_generic_data_type         *raw_ptr
)
{
  uint8            raw_cmd_buff[ESTK_SETUP_MENU_RAW_CMD_BUFFER_SIZE];
  uint8            value_buff[ESTK_SETUP_MENU_RAW_CMD_BUFFER_SIZE];
  uint32           buff_size        = ESTK_SETUP_MENU_RAW_CMD_BUFFER_SIZE;
  uint8            setup_menu_hdr[] =
    { 0xD0, 0x00, 0x81, 0x03, 0x01, 0x25, 0x00, 0x82, 0x02, 0x81, 0x82};
  uint32           hdr_size         = sizeof(setup_menu_hdr);
  uint32           len              = 0;
  uint32           idx              = 0;
  uint32           i                = 0;

  if (cmd_ptr == NULL || raw_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL PTR: 0x%x, 0x%x", cmd_ptr, raw_ptr);
    return ESTK_BAD_INPUT_PARAM;
  }

  (void)gstk_memcpy(raw_cmd_buff, setup_menu_hdr, hdr_size, buff_size, hdr_size);
  idx = hdr_size;
  if (cmd_ptr->help_available)
  {
    raw_cmd_buff[6] |= 0x80;
  }
  if (cmd_ptr->softkey_selection)
  {
    raw_cmd_buff[6] |= 0x01;
  }

  /* alpha identifier */
  if (cmd_ptr->title.length != 0 && cmd_ptr->title.text != NULL)
  {
    len = estk_setup_menu_pack_tlv(&raw_cmd_buff[idx],
                                   buff_size - idx,
                                   0x05,
                                   cmd_ptr->title.length,
                                   cmd_ptr->title.text);
    if (len <= 0)
    {
      UIM_MSG_ERR_0("failed to pack alpha TLV!");
      return ESTK_ERROR;
    }
    idx += len;
  }

  /* menu items */
  for (i = 0; i < cmd_ptr->num_items; ++i)
  {
    if (cmd_ptr->items[i].item_text != NULL
        &&
        cmd_ptr->items[i].length != 0)
    {
      value_buff[0] = cmd_ptr->items[i].item_id;
      if(gstk_memcpy(&value_buff[1],
                     cmd_ptr->items[i].item_text,
                     cmd_ptr->items[i].length,
                     buff_size - 1,
                     cmd_ptr->items[i].length) <
         cmd_ptr->items[i].length)
      {
        UIM_MSG_ERR_0("estk_setup_menu_cmd_to_raw_fmt(): gstk_memcpy failed");
        return ESTK_ERROR;
      }
      len  = estk_setup_menu_pack_tlv(&raw_cmd_buff[idx],
                                      buff_size - idx,
                                      0x0F,
                                      cmd_ptr->items[i].length + 1,
                                      value_buff);
      if (len <= 0)
      {
        UIM_MSG_ERR_0("failed to pack item TLV!");
        return ESTK_ERROR;
      }
      idx += len;
    }
  }

  /* items next action indicator */
  if (cmd_ptr->next_action_indicator.num_items > 0
      &&
      cmd_ptr->next_action_indicator.next_action_list != NULL)
  {
    len = estk_setup_menu_pack_tlv(
            &raw_cmd_buff[idx],
            buff_size - idx,
            0x18,
            cmd_ptr->next_action_indicator.num_items,
            cmd_ptr->next_action_indicator.next_action_list);
    if (len <= 0)
    {
      UIM_MSG_ERR_0("failed to pack items next action indicator!");
      return ESTK_ERROR;
    }
    idx += len;
  }

  /* don't convert icon and icon id list:
   * 1) icon
   * 2) icon_id_list
   * 
   * they are NOT supported for now
   */

  /* adjust length of entire proactive cmd */
  if ((idx - 2) > 127)
  {
    if(gstk_memcpy(value_buff,
                   &raw_cmd_buff[2],
                   idx - 2,
                   buff_size,
                   buff_size - 2) <
       (size_t)(idx - 2))
    {
      return ESTK_ERROR;
    }
    len = estk_setup_menu_pack_tlv(raw_cmd_buff,
                                   buff_size,
                                   0xD0,
                                   idx - 2,
                                   value_buff);
    if (len <= 0)
    {
      UIM_MSG_ERR_0("failed to adjust len of pro cmd!");
      return ESTK_ERROR;
    }
    idx = len;
  }
  else
  {
    raw_cmd_buff[1] = uint32touint8(idx - 2);
  }

  /* copy data to output */
  raw_ptr->data_buffer_ptr = (uint8 *)GSTK_CALLOC(idx);
  if (raw_ptr->data_buffer_ptr == NULL)
  {
    return ESTK_ERROR;
  }
  if(gstk_memcpy(raw_ptr->data_buffer_ptr,
                 raw_cmd_buff,
                 idx,
                 idx,
                 buff_size) < idx)
  {
    gstk_free(raw_ptr->data_buffer_ptr);
    raw_ptr->data_buffer_ptr = NULL;
    return ESTK_ERROR;
  }
  raw_ptr->data_len = idx;

  /***** SETUP MENU *****/
  gstk_util_dump_byte_array("", raw_cmd_buff, idx);
  return ESTK_SUCCESS;
} /*estk_setup_menu_cmd_to_raw_fmt*/

/*===========================================================================

FUNCTION: estk_process_setup_menu_req

DESCRIPTION:
  Called  from GSTK when a SETUP MENU command comes

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_process_setup_menu_req(
  const estk_cmd_type *cmd_ptr
)
{
  estk_result_enum_type                     result        = ESTK_SUCCESS;
  gstk_status_enum_type                     gstk_status   = GSTK_SUCCESS;
  gstk_client_id_type                       cli_id        = 0;
  gstk_client_pro_cmd_reg_data_type         gstk_reg_info;
  gstk_client_pro_cmd_reg_data_type         raw_reg_info;
  gstk_client_reg_cb_func_format_enum_type  evt_cb_type   = GSTK_CLI_CB_NONE;
  gstk_toolkit_evt_cb_func_type             evt_cb        = NULL;
  gstk_evt_cb_funct_type                    old_evt_cb    = NULL;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_client_pro_cmd_data_format_enum_type formats[]      = 
    {GSTK_RAW_FORMAT, GSTK_GSTK_FORMAT};
  uint32                                    i              = 0;
  gstk_raw_cmd_from_card_type               raw_cmd;

  if (cmd_ptr == NULL)
  {
    return ESTK_BAD_INPUT_PARAM;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&gstk_reg_info, 0x00, sizeof(gstk_reg_info));
  memset(&raw_reg_info, 0x00, sizeof(raw_reg_info));
  memset(&raw_cmd, 0x00, sizeof(raw_cmd));

  find_cli_reg.cmd_type = GSTK_SETUP_MENU_REQ;
  find_cli_reg.cli_func_type = GSTK_HANDLE_ALL_FUNC;
  find_cli_reg.format_type = GSTK_FORMAT_NONE;
  for (i = 0;
       i < sizeof(formats) / sizeof(gstk_client_pro_cmd_data_format_enum_type);
       ++i)
  {
    find_cli_reg.format_type = formats[i];
    gstk_status = gstk_io_ctrl(
                    GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                    &find_cli_reg,
                    &cli_id);
    if (gstk_status == GSTK_SUCCESS)
    {
      break;
    }
  }

  switch (find_cli_reg.format_type)
  {
  case GSTK_GSTK_FORMAT:
    break;
  case GSTK_RAW_FORMAT:
    raw_reg_info.data_format_type = GSTK_RAW_FORMAT;
    raw_reg_info.pro_cmd_reg_data.raw_req_rsp_ptr = &raw_cmd;
    raw_cmd.raw_hdr.cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;
    raw_cmd.raw_hdr.sim_slot_id = cmd_ptr->hdr.sim_slot_id;
    raw_cmd.raw_hdr.user_data = cmd_ptr->hdr.cli_user_data;
    break;
  default:
    UIM_MSG_ERR_0("can't find client reg SETUP MENU cmd!");
    return ESTK_ERROR;
  }

  gstk_reg_info.data_format_type = GSTK_GSTK_FORMAT;
  gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr = 
    (gstk_cmd_from_card_type *)GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));
  if (gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr == NULL)
  {
    return ESTK_ERROR;
  }
  gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.cmd_detail_reference = 
    cmd_ptr->hdr.cmd_ref_id;
  gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id =
    GSTK_SETUP_MENU_REQ;
  gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_number = 
    cmd_ptr->hdr.command_number;
  gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id = 
    cmd_ptr->hdr.sim_slot_id;
  gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.user_data =
    cmd_ptr->hdr.cli_user_data;

  estk_free_setup_menu_data(&estk_curr_inst_ptr->setup_menu_cmd);

  /*save command data to current ESTK instance*/
  result = estk_copy_setup_menu_data(&estk_curr_inst_ptr->setup_menu_cmd,
                                     &cmd_ptr->cmd_data.setup_menu_req);
  if (result != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to save setup menu cmd! 0x%x", result);
    goto ESTK_PROCESS_SETUP_MENU_REQ_ERROR;
  }

  result = estk_merge_setup_menu_cmd(
             &gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.setup_menu_pro_cmd_req);
  if (result != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to consolidate setup menu cmds!");
    goto ESTK_PROCESS_SETUP_MENU_REQ_ERROR;
  }

  if (gstk_io_ctrl(
        GSTK_IO_GET_CLIENT_TABLE_EVT_CB_TYPE,
        &cli_id,
        &evt_cb_type) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("can't read evt_cb_type!");
    result = ESTK_ERROR;
    goto ESTK_PROCESS_SETUP_MENU_REQ_ERROR;
  }

  if (gstk_io_ctrl(
        GSTK_IO_GET_CLIENT_TABLE_NEW_CLIENT_CB,
        &cli_id,
        &evt_cb) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("can't get evt_cb!");
    result = ESTK_ERROR;
    goto ESTK_PROCESS_SETUP_MENU_REQ_ERROR;
  }

  switch (find_cli_reg.format_type)
  {
  case GSTK_GSTK_FORMAT:
    if((evt_cb_type == GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT) &&
       (evt_cb != NULL))
    {
      (evt_cb)(&gstk_reg_info);
    }
    else if ((evt_cb_type == GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT) &&
             (evt_cb != NULL))
    {
      old_evt_cb = (gstk_evt_cb_funct_type)(evt_cb);
      (old_evt_cb)(gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr);
    }
    else
    {
      UIM_MSG_ERR_2("failed to dispatch setup menu cmd! 0x%x, 0x%x",
                    evt_cb_type, evt_cb);
      result = ESTK_ERROR;
      goto ESTK_PROCESS_SETUP_MENU_REQ_ERROR;
    }
  
    result = ESTK_SUCCESS;
    break;
  case GSTK_RAW_FORMAT:
    if ((evt_cb_type == GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT)
        &&
        (evt_cb != NULL))
    {
      if (estk_setup_menu_cmd_to_raw_fmt
            (&gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.setup_menu_pro_cmd_req,
             &raw_reg_info.pro_cmd_reg_data.raw_req_rsp_ptr->payload) == ESTK_SUCCESS)
      {
        /* ensure correct command number w.r.t ongoing proactive command is passed to upper layer */
        if (raw_reg_info.pro_cmd_reg_data.raw_req_rsp_ptr->payload->data_buffer_ptr[1] == 0x81)
        {
          raw_reg_info.pro_cmd_reg_data.raw_req_rsp_ptr->payload->data_buffer_ptr[5] = cmd_ptr->hdr.command_number;
        }
        else
        {
          raw_reg_info.pro_cmd_reg_data.raw_req_rsp_ptr->payload->data_buffer_ptr[4] = cmd_ptr->hdr.command_number;
        }
        (evt_cb)(&raw_reg_info);
      }
    }
    else
    {
      UIM_MSG_ERR_2("failed to dispatch setup menu cmd! 0x%x, 0x%x",
                    evt_cb_type, evt_cb);
      result = ESTK_ERROR;
      goto ESTK_PROCESS_SETUP_MENU_REQ_ERROR;
    }
    break;
  default:
    UIM_MSG_ERR_1("invalid format: 0x%x", find_cli_reg.format_type);
    break;
  }

ESTK_PROCESS_SETUP_MENU_REQ_ERROR:
  estk_free_setup_menu_data(
    &gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.setup_menu_pro_cmd_req);
  gstk_free(gstk_reg_info.pro_cmd_reg_data.gstk_req_rsp_ptr);
  gstk_free(raw_cmd.payload.data_buffer_ptr);
  return result;
} /*estk_process_setup_menu_req*/

/*===========================================================================

FUNCTION: estk_gstk_env_cb

DESCRIPTION:
  Dummy function to serve as callback for menu selection envelope

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
void  estk_gstk_env_cb (
  gstk_cmd_from_card_type   * req_rsp
)
{
  UIM_MSG_HIGH_1("estk_gstk_env_cb: command = 0x%x",
                 req_rsp->hdr_cmd.command_id);
  (void)req_rsp;
  return;
} /* estk_gstk_env_cb */

/*===========================================================================

FUNCTION: estk_process_menu_sel_req

DESCRIPTION:
  Called  from GSTK when a MENU SELECTION command comes

PARAMETERS:
  void

DEPENDENCIES:
  None

RETURN VALUE:
  gstk_status_enum_type
    GSTK_SUCCESS
    GSTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
estk_result_enum_type estk_process_menu_sel_req(
  const estk_cmd_type *cmd_ptr
)
{
  uint32                i       = 0;
  uint32                idx     = 0;
  gstk_status_enum_type status  = GSTK_SUCCESS;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  idx = cmd_ptr->cmd_data.menu_sel_req.chosen_item_id;
  for (i = 0; i < estk_num_of_instances; ++i)
  {
    if (idx < GSTK_MAX_MENU_ITEMS &&
        idx < estk_instances_ptr[i]->setup_menu_cmd.num_items)
    {
      if(cmd_ptr->estk_raw_evt_cb != NULL)
      {
        status = gstk_send_envelope_menu_sel_command_ext(
                   estk_instances_ptr[i]->gstk_client_id,
                   cmd_ptr->hdr.cli_user_data,
                   estk_instances_ptr[i]->setup_menu_cmd.items[idx].item_id,
                   cmd_ptr->cmd_data.menu_sel_req.request_help,
                   cmd_ptr->estk_raw_evt_cb);
      }
      else
      {
        status = gstk_send_envelope_menu_sel_command_ext(
                   estk_instances_ptr[i]->gstk_client_id,
                   cmd_ptr->hdr.cli_user_data,
                   estk_instances_ptr[i]->setup_menu_cmd.items[idx].item_id,
                   cmd_ptr->cmd_data.menu_sel_req.request_help,
                   estk_gstk_env_cb);
      }

      if (status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_1("failed to send MENU SEL cmd! 0x%x", status);
        return ESTK_ERROR;
      }
      return ESTK_SUCCESS;
    }
    else
    {
      idx -= estk_instances_ptr[i]->setup_menu_cmd.num_items;
    }
  }

  UIM_MSG_ERR_2("invalid menu item ID! 0x%x, 0x%x",
                idx, cmd_ptr->cmd_data.menu_sel_req.chosen_item_id);
  return ESTK_ERROR;
} /*estk_process_menu_sel_req*/
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

