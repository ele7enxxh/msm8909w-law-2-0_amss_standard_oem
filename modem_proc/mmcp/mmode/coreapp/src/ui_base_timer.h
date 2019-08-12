#ifndef _UI_BASE_TIMER_H
#define _UI_BASE_TIMER_H

#include "timer.h"
#include "ui_base.h"

#define TUI_CSG_TIMER_ID 0xFF

#define TUI_CSG_CONFIG_EFS_FILE "/nv/item_files/modem/mmode/tui/csg_search_sel_config"

void ui_base_timer_init(void);

boolean ui_base_is_timer_running(void);

void ui_base_timer_reset(uint16 interval);

void ui_base_timer_cmd_cb( timer_cb_data_type  user_data );

void ui_base_timer_expiry_hdlr(boolean);

#endif /* #define _UI_BASE_TIMER_H*/

