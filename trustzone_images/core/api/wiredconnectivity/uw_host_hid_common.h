/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file uw_host_hid_common.h

USB Human Interface Device (HID) class driver definitions and macros.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_host_hid_common.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/26/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/

/* Common HID Class Driver API */

#ifndef _UW_HOST_HID_COMMON_H_
#define _UW_HOST_HID_COMMON_H_

#include "jtypes.h"


/** @ingroup hsusb_jungo_api 
@{ */

/*******************************
 * USB HID Descriptor Requests *
 *******************************/
#define UR_GET_HID_DESCRIPTOR   0x06
#define UDESC_HID               0x21
#define UDESC_REPORT            0x22
#define UDESC_PHYSICAL          0x23
#define UR_SET_HID_DESCRIPTOR   0x07
#define UR_GET_REPORT           0x01
#define UR_SET_REPORT           0x09
#define UR_GET_IDLE             0x02
#define UR_SET_IDLE             0x0a
#define UR_GET_PROTOCOL         0x03
#define UR_SET_PROTOCOL         0x0b
#define USB_HID_DESCRIPTOR_SIZE(n) (9+(n)*3)

/***********
 * Reports *
 ***********/
/* Report types */
#define UHID_INPUT_REPORT 0x01
#define UHID_OUTPUT_REPORT 0x02
#define UHID_FEATURE_REPORT 0x03

/* Input/Output/Feature report item Bits */
#define HIO_CONST       0x001
#define HIO_VARIABLE    0x002
#define HIO_RELATIVE    0x004
#define HIO_WRAP        0x008
#define HIO_NONLINEAR   0x010
#define HIO_NOPREF      0x020
#define HIO_NULLSTATE   0x040
#define HIO_VOLATILE    0x080
#define HIO_BUFBYTES    0x100
#define HIO_MASK        0x1ff

/********************
 * Usage Attributes *
 ********************/
/** Usage attributes information structure.
*/
typedef struct
{
    juint32_t usage;       /**< Usage code created using the HID_USAGE2 macro. */
    jint32_t logical_min;  /**< Minimum logical value. */
    jint32_t logical_max;  /**< Maximum logical value. */
    jint32_t physical_min; /**< Minimum physical value. */
    jint32_t physical_max; /**< Maximum physical value. */
    jint32_t usage_min;    /**< Minimum usage value. */
    jint32_t usage_max;    /**< Maximum usage value. */
    jint32_t unit;         /**< Usage measurement unit. */
    juint32_t flags;       /**< Usage flags. */
} usage_att_t;

/***************
 * Usage Codes *
 ***************/

/* ///////////////
 *  Basic usages 
 * ////////////// */
/* Page Usages */
#define HUP_UNDEFINED           0x0000UL
#define HUP_GENERIC_DESKTOP     0x0001UL
#define HUP_SIMULATION          0x0002UL
#define HUP_VR_CONTROLS         0x0003UL
#define HUP_SPORTS_CONTROLS     0x0004UL
#define HUP_GAMING_CONTROLS     0x0005UL
#define HUP_KEYBOARD            0x0007UL
#define HUP_KEYPAD              0x0007UL
#define HUP_LEDS                0x0008UL
#define HUP_BUTTON              0x0009UL
#define HUP_ORDINALS            0x000aUL
#define HUP_TELEPHONY           0x000bUL
#define HUP_CONSUMER            0x000cUL
#define HUP_DIGITIZERS          0x000dUL
#define HUP_PHYSICAL_IFACE      0x000eUL
#define HUP_UNICODE             0x0010UL
#define HUP_ALPHANUM_DISPLAY    0x0014UL
#define HUP_MONITOR             0x0080UL
#define HUP_MONITOR_ENUM_VAL    0x0081UL
#define HUP_VESA_VC             0x0082UL
#define HUP_VESA_CMD            0x0083UL
#define HUP_POWER               0x0084UL
#define HUP_BATTERY_SYSTEM      0x0085UL
#define HUP_BARCODE_SCANNER     0x008bUL
#define HUP_SCALE               0x008cUL
#define HUP_CAMERA_CONTROL      0x0090UL
#define HUP_ARCADE              0x0091UL
#define HUP_MICROSOFT           0xff00UL
#define HUP_MOTOR               0x0100UL

/* Generic Desktop Usages */
#define HUG_POINTER             0x0001UL
#define HUG_MOUSE               0x0002UL
#define HUG_JOYSTICK            0x0004UL
#define HUG_GAME_PAD            0x0005UL
#define HUG_KEYBOARD            0x0006UL
#define HUG_KEYPAD              0x0007UL
#define HUG_X                   0x0030UL
#define HUG_Y                   0x0031UL
#define HUG_Z                   0x0032UL
#define HUG_RX                  0x0033UL
#define HUG_RY                  0x0034UL
#define HUG_RZ                  0x0035UL
#define HUG_SLIDER              0x0036UL
#define HUG_DIAL                0x0037UL
#define HUG_WHEEL               0x0038UL
#define HUG_HAT_SWITCH          0x0039UL
#define HUG_COUNTED_BUFFER      0x003aUL
#define HUG_BYTE_COUNT          0x003bUL
#define HUG_MOTION_WAKEUP       0x003cUL
#define HUG_START               0x003dUL
#define HUG_SELECT              0x003eUL
#define HUG_VX                  0x0040UL
#define HUG_VY                  0x0041UL
#define HUG_VZ                  0x0042UL
#define HUG_VBRX                0x0043UL
#define HUG_VBRY                0x0044UL
#define HUG_VBRZ                0x0045UL
#define HUG_VNO                 0x0046UL
#define HUG_SYSTEM_CONTROL      0x0080UL
#define HUG_SYSTEM_POWER_DOWN   0x0081UL
#define HUG_SYSTEM_SLEEP        0x0082UL
#define HUG_SYSTEM_WAKEUP       0x0083UL
#define HUG_SYSTEM_CONTEXT_MENU 0x0084UL
#define HUG_SYSTEM_MAIN_MENU    0x0085UL
#define HUG_SYSTEM_APP_MENU     0x0086UL
#define HUG_SYSTEM_MENU_HELP    0x0087UL
#define HUG_SYSTEM_MENU_EXIT    0x0088UL
#define HUG_SYSTEM_MENU_SELECT  0x0089UL
#define HUG_SYSTEM_MENU_RIGHT   0x008aUL
#define HUG_SYSTEM_MENU_LEFT    0x008bUL
#define HUG_SYSTEM_MENU_UP      0x008cUL
#define HUG_SYSTEM_MENU_DOWN    0x008dUL
#define HUG_DPAD_UP             0x0090UL
#define HUG_DPAD_DOWN           0x0091UL
#define HUG_DPAD_RIGHT          0x0092UL
#define HUG_DPAD_LEFT           0x0093UL

/* Digitizers Usages */
#define HUD_UNDEFINED           0x0000UL
#define HUD_TIP_PRESSURE        0x0030UL
#define HUD_BARREL_PRESSURE     0x0031UL
#define HUD_IN_RANGE            0x0032UL
#define HUD_TOUCH               0x0033UL
#define HUD_UNTOUCH             0x0034UL
#define HUD_TAP                 0x0035UL
#define HUD_QUALITY             0x0036UL
#define HUD_DATA_VALID          0x0037UL
#define HUD_TRANSDUCER_INDEX    0x0038UL
#define HUD_TABLET_FKEYS        0x0039UL
#define HUD_PROGRAM_CHANGE_KEYS 0x003aUL
#define HUD_BATTERY_STRENGTH    0x003bUL
#define HUD_INVERT              0x003cUL
#define HUD_X_TILT              0x003dUL
#define HUD_Y_TILT              0x003eUL
#define HUD_AZIMUTH             0x003fUL
#define HUD_ALTITUDE            0x0040UL
#define HUD_TWIST               0x0041UL
#define HUD_TIP_SWITCH          0x0042UL
#define HUD_SEC_TIP_SWITCH      0x0043UL
#define HUD_BARREL_SWITCH       0x0044UL
#define HUD_ERASER              0x0045UL
#define HUD_TABLET_PICK         0x0046UL

/* Keyboard Usages */
#define HUK_ERR_ROLL_OVER       0x0001UL
#define HUK_POSTFAIL            0x0002UL
#define HUK_ERR_UNDEF           0x0003UL
#define HUK_A                   0x0004UL
#define HUK_B                   0x0005UL
#define HUK_C                   0x0006UL
#define HUK_D                   0x0007UL
#define HUK_E                   0x0008UL
#define HUK_F                   0x0009UL
#define HUK_G                   0x000AUL
#define HUK_H                   0x000BUL
#define HUK_I                   0x000CUL
#define HUK_J                   0x000DUL
#define HUK_K                   0x000EUL
#define HUK_L                   0x000FUL
#define HUK_M                   0x0010UL
#define HUK_N                   0x0011UL
#define HUK_O                   0x0012UL
#define HUK_P                   0x0013UL
#define HUK_Q                   0x0014UL
#define HUK_R                   0x0015UL
#define HUK_S                   0x0016UL
#define HUK_T                   0x0017UL
#define HUK_U                   0x0018UL
#define HUK_V                   0x0019UL
#define HUK_W                   0x001AUL
#define HUK_X                   0x001BUL
#define HUK_Y                   0x001CUL
#define HUK_Z                   0x001DUL
#define HUK_1                   0x001EUL
#define HUK_2                   0x001FUL
#define HUK_3                   0x0020UL
#define HUK_4                   0x0021UL
#define HUK_5                   0x0022UL
#define HUK_6                   0x0023UL
#define HUK_7                   0x0024UL
#define HUK_8                   0x0025UL
#define HUK_9                   0x0026UL
#define HUK_0                   0x0027UL
#define HUK_ENTER               0x0028UL
#define HUK_ESCAPE              0x0029UL
#define HUK_BACKSPACE           0x002AUL
#define HUK_TAB                 0x002BUL
#define HUK_SPACEBAR            0x002CUL
#define HUK_MINUS               0x002DUL
#define HUK_EQUAL               0x002EUL
#define HUK_SQR_BRACKET_OPEN    0x002FUL
#define HUK_SQR_BRACKET_CLOSE   0x0030UL
#define HUK_BACKSLASH           0x0031UL
#define HUK_NON_US_HASH         0x0032UL
#define HUK_SEMICOLON           0x0033UL
#define HUK_APOSTROPHE          0x0034UL
#define HUK_GRAVEACCENT         0x0035UL
#define HUK_COMMA               0x0036UL
#define HUK_PERIOD              0x0037UL
#define HUK_SLASH               0x0038UL
#define HUK_CAPSLOCK            0x0039UL
#define HUK_F1                  0x003AUL
#define HUK_F2                  0x003BUL
#define HUK_F3                  0x003CUL
#define HUK_F4                  0x003DUL
#define HUK_F5                  0x003EUL
#define HUK_F6                  0x003FUL
#define HUK_F7                  0x0040UL
#define HUK_F8                  0x0041UL
#define HUK_F9                  0x0042UL
#define HUK_F10                 0x0043UL
#define HUK_F11                 0x0044UL
#define HUK_F12                 0x0045UL
#define HUK_PRINTSCREEN         0x0046UL
#define HUK_SCROLLLOCK          0x0047UL
#define HUK_PAUSE               0x0048UL
#define HUK_INSERT              0x0049UL
#define HUK_HOME                0x004AUL
#define HUK_PAGEUP              0x004BUL
#define HUK_DELETE              0x004CUL
#define HUK_END                 0x004DUL
#define HUK_PAGEDOWN            0x004EUL
#define HUK_RIGHTARROW          0x004FUL
#define HUK_LEFTARROW           0x0050UL
#define HUK_DOWNARROW           0x0051UL
#define HUK_UPARROW             0x0052UL
#define HUKP_NUMLOCK            0x0053UL
#define HUKP_SLASH              0x0054UL
#define HUKP_ASTERISK           0x0055UL
#define HUKP_MINUS              0x0056UL
#define HUKP_PLUS               0x0057UL
#define HUKP_ENTER              0x0058UL
#define HUKP_1                  0x0059UL
#define HUKP_2                  0x005AUL
#define HUKP_3                  0x005BUL
#define HUKP_4                  0x005CUL
#define HUKP_5                  0x005DUL
#define HUKP_6                  0x005EUL
#define HUKP_7                  0x005FUL
#define HUKP_8                  0x0060UL
#define HUKP_9                  0x0061UL
#define HUKP_0                  0x0062UL
#define HUKP_PERIOD             0x0063UL
#define HUK_NON_US_BACKSLASH    0x0064UL
#define HUK_APP                 0x0065UL
#define HUK_POWER               0x0066UL
#define HUKP_EQUAL              0x0067UL
#define HUK_F13                 0x0068UL
#define HUK_F14                 0x0069UL
#define HUK_F15                 0x006AUL
#define HUK_F16                 0x006BUL
#define HUK_F17                 0x006CUL
#define HUK_F18                 0x006DUL
#define HUK_F19                 0x006EUL
#define HUK_F20                 0x006FUL
#define HUK_F21                 0x0070UL
#define HUK_F22                 0x0071UL
#define HUK_F23                 0x0072UL
#define HUK_F24                 0x0073UL
#define HUK_EXE                 0x0074UL
#define HUK_HELP                0x0075UL
#define HUK_MENU                0x0076UL
#define HUK_SELECT              0x0077UL
#define HUK_STOP                0x0078UL
#define HUK_AGAIN               0x0079UL
#define HUK_UNDO                0x007AUL
#define HUK_CUT                 0x007BUL
#define HUK_COPY                0x007CUL
#define HUK_PASTE               0x007DUL
#define HUK_FIND                0x007EUL
#define HUK_MUTE                0x007FUL
#define HUK_VOL_UP              0x0080UL
#define HUK_VOL_DOWN            0x0081UL
#define HUK_LOCKING_CAPS        0x0082UL
#define HUK_LOCKING_NUM         0x0083UL
#define HUK_LOCKING_SCROLL      0x0084UL
#define HUKP_COMMA              0x0085UL
#define HUKP_EQUALSIGN          0x0086UL
#define HUK_INT1                0x0087UL
#define HUK_INT2                0x0088UL
#define HUK_INT3                0x0089UL
#define HUK_INT4                0x008AUL
#define HUK_INT5                0x008BUL
#define HUK_INT6                0x008CUL
#define HUK_INT7                0x008DUL
#define HUK_INT8                0x008EUL
#define HUK_INT9                0x008FUL
#define HUK_LANG1               0x0090UL
#define HUK_LANG2               0x0091UL
#define HUK_LANG3               0x0092UL
#define HUK_LANG4               0x0093UL
#define HUK_LANG5               0x0094UL
#define HUK_LANG6               0x0095UL
#define HUK_LANG7               0x0096UL
#define HUK_LANG8               0x0097UL
#define HUK_LANG9               0x0098UL
#define HUK_ALT_ERASE           0x0099UL
#define HUK_SYSREQ              0x009AUL
#define HUK_CANCEL              0x009BUL
#define HUK_CLEAR               0x009CUL
#define HUK_PRIOR               0x009DUL
#define HUK_RETURN              0x009EUL
#define HUK_SEPARATOR           0x009FUL
#define HUK_OUT                 0x00A0UL
#define HUK_OPER                0x00A1UL
#define HUK_CLEAR_AGAIN         0x00A2UL
#define HUK_CRSEL               0x00A3UL
#define HUK_EXSEL               0x00A4UL
#define HUKP_00                 0x00B0UL
#define HUKP_000                0x00B1UL
#define HUKP_THOUSANDS_SEP      0x00B2UL
#define HUKP_DECIMAL_SEP        0x00B3UL
#define HUKP_CURRENCY_UNIT      0x00B4UL
#define HUKP_CURRENCY_SUBUNIT   0x00B5UL
#define HUKP_PARENTESIS_OPEN    0x00B6UL
#define HUKP_PARENTESIS_CLOSE   0x00B7UL
#define HUKP_BRACES_OPEN        0x00B8UL
#define HUKP_BRACES_CLOSE       0x00B9UL
#define HUKP_TAB                0x00BAUL
#define HUKP_BACKSPACE          0x00BBUL
#define HUKP_A                  0x00BCUL
#define HUKP_B                  0x00BDUL
#define HUKP_C                  0x00BEUL
#define HUKP_D                  0x00BFUL
#define HUKP_E                  0x00C0UL
#define HUKP_F                  0x00C1UL
#define HUKP_XOR                0x00C2UL
#define HUKP_CIRCUMFLEX         0x00C3UL
#define HUKP_PERCENT            0x00C4UL
#define HUKP_LESS               0x00C5UL
#define HUKP_BIGGER             0x00C6UL
#define HUKP_AMPERSAND          0x00C7UL
#define HUKP_AND                0x00C8UL
#define HUKP_PIPE               0x00C9UL
#define HUKP_OR                 0x00CAUL
#define HUKP_COLON              0x00CBUL
#define HUKP_HASH               0x00CCUL
#define HUKP_SPACE              0x00CDUL
#define HUKP_AT                 0x00CEUL
#define HUKP_EXECUT_MARK        0x00CFUL
#define HUKP_MEMSTORE           0x00D0UL
#define HUKP_MEMRECALL          0x00D1UL
#define HUKP_MEMCLEAR           0x00D2UL
#define HUKP_MEMADD             0x00D3UL
#define HUKP_MEMSUBTRACT        0x00D4UL
#define HUKP_MEMMULTIPLY        0x00D5UL
#define HUKP_MEMDIVIDE          0x00D6UL
#define HUKP_SIGN               0x00D7UL
#define HUKP_CLEAR              0x00D8UL
#define HUKP_CLEARENTRY         0x00D9UL
#define HUKP_BINARY             0x00DAUL
#define HUKP_OCTAL              0x00DBUL
#define HUKP_DECIMAL            0x00DCUL
#define HUKP_HEXADECIMAL        0x00DDUL
#define HUK_LEFT_CTRL           0x00e0UL
#define HUK_LEFT_SHIFT          0x00e1UL
#define HUK_LEFT_ALT            0x00e2UL
#define HUK_LEFT_GUI            0x00e3UL
#define HUK_RIGHT_CTRL          0x00e4UL
#define HUK_RIGHT_SHIFT         0x00e5UL
#define HUK_RIGHT_ALT           0x00e6UL
#define HUK_RIGHT_GUI           0x00e7UL

/* LEDs Usages */
#define HUL_NUM_LOCK            0x0001UL
#define HUL_CAPS_LOCK           0x0002UL
#define HUL_SCROLL_LOCK         0x0003UL
#define HUL_COMPOSE             0x0004UL
#define HUL_KANA                0x0005UL
#define HUL_POWER               0x0006UL
#define HUL_SHIFT               0x0007UL
#define HUL_GENERIC_IND         0x004bUL

/* Simulation Usages */
#define HUS_THROTTLE            0x00bbUL
#define HUS_FLIGHT_SIM          0x00bcUL

/* Motor Usages*/
#define HUM_BIG_MOTOR           0x0001UL
#define HUM_LITTLE_MOTOR        0x0002UL
/* //////////////
 *  Usage Codes 
 * ////////////// */
/** Usage code creation macro.\ It creates usage codes from the basic usages. */
#define HID_USAGE2(p,u) (((p) << 16) | u)

/* Predefined Usage Codes */
#define USAGE_DIAL              HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DIAL)
#define USAGE_WHEEL             HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_WHEEL)
#define USAGE_SLIDER            HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_SLIDER)
#define USAGE_HATSWITCH         HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_HAT_SWITCH)
#define USAGE_BYTECOUNTER       HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_BYTE_COUNT)
#define USAGE_MOTIONWAKEUP      HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_MOTION_WAKEUP)
#define USAGE_START             HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_START) 
#define USAGE_SELECT            HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_SELECT) 
#define USAGE_SYSCONTRL         HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_CONTROL)
#define USAGE_SYSPOWER          HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_POWER_DOWN)
#define USAGE_SYSSLEEP          HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_SLEEP)
#define USAGE_SYSWAKE           HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_WAKEUP)
#define USAGE_SYSCONMENU        HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_CONTEXT_MENU)
#define USAGE_SYSMAINMENU       HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MAIN_MENU)
#define USAGE_SYSAPPMENU        HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_APP_MENU)
#define USAGE_SYSMENUHELP       HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MENU_HELP)
#define USAGE_SYSMENUEXIT       HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MENU_EXIT)
#define USAGE_SYSMENURIGHT      HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MENU_RIGHT)
#define USAGE_SYSMENULEFT       HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MENU_LEFT)
#define USAGE_SYSMENUUP         HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MENU_UP)
#define USAGE_SYSMENUDOWN       HID_USAGE2(HUP_GENERIC_DESKTOP, \
    HUG_SYSTEM_MENU_DOWN)
#define USAGE_DPADUP            HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_UP)
#define USAGE_DPADDOWN          HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_DOWN)
#define USAGE_DPADRIGHT         HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_RIGHT)
#define USAGE_DPADLEFT          HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_LEFT)
#define USAGE_X                 HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X)
#define USAGE_Y                 HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y)
#define USAGE_Z                 HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z)
#define USAGE_RX                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_RX)
#define USAGE_RY                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_RY)
#define USAGE_RZ                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_RZ)
#define USAGE_VX                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VX)
#define USAGE_VY                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VY)
#define USAGE_VZ                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VZ)
#define USAGE_VBRX              HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VBRX)
#define USAGE_VBRY              HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VBRY)
#define USAGE_VBRZ              HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VBRZ)
#define USAGE_VNO               HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_VNO)

#define USAGE_KBD_A                 HID_USAGE2(HUP_KEYBOARD, HUK_A)
#define USAGE_KBD_B                 HID_USAGE2(HUP_KEYBOARD, HUK_B)
#define USAGE_KBD_C                 HID_USAGE2(HUP_KEYBOARD, HUK_C)
#define USAGE_KBD_D                 HID_USAGE2(HUP_KEYBOARD, HUK_D)
#define USAGE_KBD_E                 HID_USAGE2(HUP_KEYBOARD, HUK_E)
#define USAGE_KBD_F                 HID_USAGE2(HUP_KEYBOARD, HUK_F)
#define USAGE_KBD_G                 HID_USAGE2(HUP_KEYBOARD, HUK_G)
#define USAGE_KBD_H                 HID_USAGE2(HUP_KEYBOARD, HUK_H)
#define USAGE_KBD_I                 HID_USAGE2(HUP_KEYBOARD, HUK_I)
#define USAGE_KBD_J                 HID_USAGE2(HUP_KEYBOARD, HUK_J)
#define USAGE_KBD_K                 HID_USAGE2(HUP_KEYBOARD, HUK_K)
#define USAGE_KBD_L                 HID_USAGE2(HUP_KEYBOARD, HUK_L)
#define USAGE_KBD_M                 HID_USAGE2(HUP_KEYBOARD, HUK_M)
#define USAGE_KBD_N                 HID_USAGE2(HUP_KEYBOARD, HUK_N)
#define USAGE_KBD_O                 HID_USAGE2(HUP_KEYBOARD, HUK_O)
#define USAGE_KBD_P                 HID_USAGE2(HUP_KEYBOARD, HUK_P)
#define USAGE_KBD_Q                 HID_USAGE2(HUP_KEYBOARD, HUK_Q)
#define USAGE_KBD_R                 HID_USAGE2(HUP_KEYBOARD, HUK_R)
#define USAGE_KBD_S                 HID_USAGE2(HUP_KEYBOARD, HUK_S)
#define USAGE_KBD_T                 HID_USAGE2(HUP_KEYBOARD, HUK_T)
#define USAGE_KBD_U                 HID_USAGE2(HUP_KEYBOARD, HUK_U)
#define USAGE_KBD_V                 HID_USAGE2(HUP_KEYBOARD, HUK_V)
#define USAGE_KBD_W                 HID_USAGE2(HUP_KEYBOARD, HUK_W)
#define USAGE_KBD_X                 HID_USAGE2(HUP_KEYBOARD, HUK_X)
#define USAGE_KBD_Y                 HID_USAGE2(HUP_KEYBOARD, HUK_Y)
#define USAGE_KBD_Z                 HID_USAGE2(HUP_KEYBOARD, HUK_Z)
#define USAGE_KBD_1                 HID_USAGE2(HUP_KEYBOARD, HUK_1)
#define USAGE_KBD_2                 HID_USAGE2(HUP_KEYBOARD, HUK_2)
#define USAGE_KBD_3                 HID_USAGE2(HUP_KEYBOARD, HUK_3)
#define USAGE_KBD_4                 HID_USAGE2(HUP_KEYBOARD, HUK_4)
#define USAGE_KBD_5                 HID_USAGE2(HUP_KEYBOARD, HUK_5)
#define USAGE_KBD_6                 HID_USAGE2(HUP_KEYBOARD, HUK_6)
#define USAGE_KBD_7                 HID_USAGE2(HUP_KEYBOARD, HUK_7)
#define USAGE_KBD_8                 HID_USAGE2(HUP_KEYBOARD, HUK_8)
#define USAGE_KBD_9                 HID_USAGE2(HUP_KEYBOARD, HUK_9)
#define USAGE_KBD_0                 HID_USAGE2(HUP_KEYBOARD, HUK_0)
#define USAGE_KBD_ENTER             HID_USAGE2(HUP_KEYBOARD, HUK_ENTER)
#define USAGE_KBD_ESCAPE            HID_USAGE2(HUP_KEYBOARD, HUK_ESCAPE)
#define USAGE_KBD_BACKSPACE         HID_USAGE2(HUP_KEYBOARD, HUK_BACKSPACE)
#define USAGE_KBD_TAB               HID_USAGE2(HUP_KEYBOARD, HUK_TAB)
#define USAGE_KBD_SPACEBAR          HID_USAGE2(HUP_KEYBOARD, HUK_SPACEBAR)
#define USAGE_KBD_MINUS             HID_USAGE2(HUP_KEYBOARD, HUK_MINUS)
#define USAGE_KBD_EQUAL             HID_USAGE2(HUP_KEYBOARD, HUK_EQUAL)
#define USAGE_KBD_SQR_BRACKET_OPEN  HID_USAGE2(HUP_KEYBOARD, \
        HUK_SQR_BRACKET_OPEN)
#define USAGE_KBD_SQR_BRACKET_CLOSE HID_USAGE2(HUP_KEYBOARD, \
        HUK_SQR_BRACKET_CLOSE)
#define USAGE_KBD_BACKSLASH         HID_USAGE2(HUP_KEYBOARD, HUK_BACKSLASH)
#define USAGE_KBD_NON_US_HASH       HID_USAGE2(HUP_KEYBOARD, HUK_NON_US_HASH)
#define USAGE_KBD_SEMICOLON         HID_USAGE2(HUP_KEYBOARD, HUK_SEMICOLON)
#define USAGE_KBD_APOSTROPHE        HID_USAGE2(HUP_KEYBOARD, HUK_APOSTROPHE)
#define USAGE_KBD_GRAVEACCENT       HID_USAGE2(HUP_KEYBOARD, HUK_GRAVEACCENT)
#define USAGE_KBD_COMMA             HID_USAGE2(HUP_KEYBOARD, HUK_COMMA)
#define USAGE_KBD_PERIOD            HID_USAGE2(HUP_KEYBOARD, HUK_PERIOD)
#define USAGE_KBD_SLASH             HID_USAGE2(HUP_KEYBOARD, HUK_SLASH)
#define USAGE_KBD_CAPSLOCK          HID_USAGE2(HUP_KEYBOARD, HUK_CAPSLOCK)
#define USAGE_KBD_F1                HID_USAGE2(HUP_KEYBOARD, HUK_F1)
#define USAGE_KBD_F2                HID_USAGE2(HUP_KEYBOARD, HUK_F2)
#define USAGE_KBD_F3                HID_USAGE2(HUP_KEYBOARD, HUK_F3)
#define USAGE_KBD_F4                HID_USAGE2(HUP_KEYBOARD, HUK_F4)
#define USAGE_KBD_F5                HID_USAGE2(HUP_KEYBOARD, HUK_F5)
#define USAGE_KBD_F6                HID_USAGE2(HUP_KEYBOARD, HUK_F6)
#define USAGE_KBD_F7                HID_USAGE2(HUP_KEYBOARD, HUK_F7)
#define USAGE_KBD_F8                HID_USAGE2(HUP_KEYBOARD, HUK_F8)
#define USAGE_KBD_F9                HID_USAGE2(HUP_KEYBOARD, HUK_F9)
#define USAGE_KBD_F10               HID_USAGE2(HUP_KEYBOARD, HUK_F10)
#define USAGE_KBD_F11               HID_USAGE2(HUP_KEYBOARD, HUK_F11)
#define USAGE_KBD_F12               HID_USAGE2(HUP_KEYBOARD, HUK_F12)
#define USAGE_KBD_PRINTSCREEN       HID_USAGE2(HUP_KEYBOARD, HUK_PRINTSCREEN)
#define USAGE_KBD_SCROLLLOCK        HID_USAGE2(HUP_KEYBOARD, HUK_SCROLLLOCK)
#define USAGE_KBD_PAUSE             HID_USAGE2(HUP_KEYBOARD, HUK_PAUSE)
#define USAGE_KBD_INSERT            HID_USAGE2(HUP_KEYBOARD, HUK_INSERT)
#define USAGE_KBD_HOME              HID_USAGE2(HUP_KEYBOARD, HUK_HOME)
#define USAGE_KBD_PAGEUP            HID_USAGE2(HUP_KEYBOARD, HUK_PAGEUP)
#define USAGE_KBD_DELETE            HID_USAGE2(HUP_KEYBOARD, HUK_DELETE)
#define USAGE_KBD_END               HID_USAGE2(HUP_KEYBOARD, HUK_END)
#define USAGE_KBD_PAGEDOWN          HID_USAGE2(HUP_KEYBOARD, HUK_PAGEDOWN)
#define USAGE_KBD_RIGHTARROW        HID_USAGE2(HUP_KEYBOARD, HUK_RIGHTARROW)
#define USAGE_KBD_LEFTARROW         HID_USAGE2(HUP_KEYBOARD, HUK_LEFTARROW)
#define USAGE_KBD_DOWNARROW         HID_USAGE2(HUP_KEYBOARD, HUK_DOWNARROW)
#define USAGE_KBD_UPARROW           HID_USAGE2(HUP_KEYBOARD, HUK_UPARROW)
#define USAGE_KEYPAD_NUMLOCK         HID_USAGE2(HUP_KEYPAD, HUKP_NUMLOCK)
#define USAGE_KEYPAD_SLASH           HID_USAGE2(HUP_KEYPAD, HUKP_SLASH)
#define USAGE_KEYPAD_ASTERISK        HID_USAGE2(HUP_KEYPAD, HUKP_ASTERISK)
#define USAGE_KEYPAD_MINUS           HID_USAGE2(HUP_KEYPAD, HUKP_MINUS)
#define USAGE_KEYPAD_PLUS            HID_USAGE2(HUP_KEYPAD, HUKP_PLUS)
#define USAGE_KEYPAD_ENTER           HID_USAGE2(HUP_KEYPAD, HUKP_ENTER)
#define USAGE_KEYPAD_1               HID_USAGE2(HUP_KEYPAD, HUKP_1)
#define USAGE_KEYPAD_2               HID_USAGE2(HUP_KEYPAD, HUKP_2)
#define USAGE_KEYPAD_3               HID_USAGE2(HUP_KEYPAD, HUKP_3)
#define USAGE_KEYPAD_4               HID_USAGE2(HUP_KEYPAD, HUKP_4)
#define USAGE_KEYPAD_5               HID_USAGE2(HUP_KEYPAD, HUKP_5)
#define USAGE_KEYPAD_6               HID_USAGE2(HUP_KEYPAD, HUKP_6)
#define USAGE_KEYPAD_7               HID_USAGE2(HUP_KEYPAD, HUKP_7)
#define USAGE_KEYPAD_8               HID_USAGE2(HUP_KEYPAD, HUKP_8)
#define USAGE_KEYPAD_9               HID_USAGE2(HUP_KEYPAD, HUKP_9)
#define USAGE_KEYPAD_0               HID_USAGE2(HUP_KEYPAD, HUKP_0)
#define USAGE_KEYPAD_PERIOD          HID_USAGE2(HUP_KEYPAD, HUKP_PERIOD)
#define USAGE_KBD_NON_US_BACKSLASH  HID_USAGE2(HUP_KEYBOARD, \
        HUK_NON_US_BACKSLASH)
#define USAGE_KBD_APP               HID_USAGE2(HUP_KEYBOARD, HUK_APP)
#define USAGE_KBD_POWER             HID_USAGE2(HUP_KEYBOARD, HUK_POWER)
#define USAGE_KEYPAD_EQUAL           HID_USAGE2(HUP_KEYPAD, HUKP_EQUAL)
#define USAGE_KBD_F13               HID_USAGE2(HUP_KEYBOARD, HUK_F13)
#define USAGE_KBD_F14               HID_USAGE2(HUP_KEYBOARD, HUK_F14)
#define USAGE_KBD_F15               HID_USAGE2(HUP_KEYBOARD, HUK_F15)
#define USAGE_KBD_F16               HID_USAGE2(HUP_KEYBOARD, HUK_F16)
#define USAGE_KBD_F17               HID_USAGE2(HUP_KEYBOARD, HUK_F17)
#define USAGE_KBD_F18               HID_USAGE2(HUP_KEYBOARD, HUK_F18)
#define USAGE_KBD_F19               HID_USAGE2(HUP_KEYBOARD, HUK_F19)
#define USAGE_KBD_F20               HID_USAGE2(HUP_KEYBOARD, HUK_F20)
#define USAGE_KBD_F21               HID_USAGE2(HUP_KEYBOARD, HUK_F21)
#define USAGE_KBD_F22               HID_USAGE2(HUP_KEYBOARD, HUK_F22)
#define USAGE_KBD_F23               HID_USAGE2(HUP_KEYBOARD, HUK_F23)
#define USAGE_KBD_F24               HID_USAGE2(HUP_KEYBOARD, HUK_F24)
#define USAGE_KBD_EXE               HID_USAGE2(HUP_KEYBOARD, HUK_EXE)
#define USAGE_KBD_HELP              HID_USAGE2(HUP_KEYBOARD, HUK_HELP)
#define USAGE_KBD_MENU              HID_USAGE2(HUP_KEYBOARD, HUK_MENU)
#define USAGE_KBD_SELECT            HID_USAGE2(HUP_KEYBOARD, HUK_SELECT)
#define USAGE_KBD_STOP              HID_USAGE2(HUP_KEYBOARD, HUK_STOP)
#define USAGE_KBD_AGAIN             HID_USAGE2(HUP_KEYBOARD, HUK_AGAIN)
#define USAGE_KBD_UNDO              HID_USAGE2(HUP_KEYBOARD, HUK_UNDO)
#define USAGE_KBD_CUT               HID_USAGE2(HUP_KEYBOARD, HUK_CUT)
#define USAGE_KBD_COPY              HID_USAGE2(HUP_KEYBOARD, HUK_COPY) 
#define USAGE_KBD_PASTE             HID_USAGE2(HUP_KEYBOARD, HUK_PASTE)
#define USAGE_KBD_FIND              HID_USAGE2(HUP_KEYBOARD, HUK_FIND)
#define USAGE_KBD_MUTE              HID_USAGE2(HUP_KEYBOARD, HUK_MUTE)
#define USAGE_KBD_VOL_UP            HID_USAGE2(HUP_KEYBOARD, HUK_VOL_UP)
#define USAGE_KBD_VOL_DOWN          HID_USAGE2(HUP_KEYBOARD, HUK_VOL_DOWN)
#define USAGE_KBD_LOCKING_CAPS      HID_USAGE2(HUP_KEYBOARD, HUK_LOCKING_CAPS)
#define USAGE_KBD_LOCKING_NUM       HID_USAGE2(HUP_KEYBOARD, HUK_LOCKING_NUM)
#define USAGE_KBD_LOCKING_SCROLL    HID_USAGE2(HUP_KEYBOARD, HUK_LOCKING_SCROLL)
#define USAGE_KEYPAD_COMMA           HID_USAGE2(HUP_KEYPAD, HUKP_COMMA)
#define USAGE_KEYPAD_EQUALSIGN       HID_USAGE2(HUP_KEYPAD, HUKP_EQUALSIGN)
#define USAGE_KBD_INT1              HID_USAGE2(HUP_KEYBOARD, HUK_INT1)
#define USAGE_KBD_INT2              HID_USAGE2(HUP_KEYBOARD, HUK_INT2)
#define USAGE_KBD_INT3              HID_USAGE2(HUP_KEYBOARD, HUK_INT3)
#define USAGE_KBD_INT4              HID_USAGE2(HUP_KEYBOARD, HUK_INT4)
#define USAGE_KBD_INT5              HID_USAGE2(HUP_KEYBOARD, HUK_INT5)
#define USAGE_KBD_INT6              HID_USAGE2(HUP_KEYBOARD, HUK_INT6)
#define USAGE_KBD_INT7              HID_USAGE2(HUP_KEYBOARD, HUK_INT7)
#define USAGE_KBD_INT8              HID_USAGE2(HUP_KEYBOARD, HUK_INT8)
#define USAGE_KBD_INT9              HID_USAGE2(HUP_KEYBOARD, HUK_INT9)
#define USAGE_KBD_LANG1             HID_USAGE2(HUP_KEYBOARD, HUK_LANG1)
#define USAGE_KBD_LANG2             HID_USAGE2(HUP_KEYBOARD, HUK_LANG2)
#define USAGE_KBD_LANG3             HID_USAGE2(HUP_KEYBOARD, HUK_LANG3)
#define USAGE_KBD_LANG4             HID_USAGE2(HUP_KEYBOARD, HUK_LANG4)
#define USAGE_KBD_LANG5             HID_USAGE2(HUP_KEYBOARD, HUK_LANG5)
#define USAGE_KBD_LANG6             HID_USAGE2(HUP_KEYBOARD, HUK_LANG6)
#define USAGE_KBD_LANG7             HID_USAGE2(HUP_KEYBOARD, HUK_LANG7)
#define USAGE_KBD_LANG8             HID_USAGE2(HUP_KEYBOARD, HUK_LANG8)
#define USAGE_KBD_LANG9             HID_USAGE2(HUP_KEYBOARD, HUK_LANG9)
#define USAGE_KBD_ALT_ERASE         HID_USAGE2(HUP_KEYBOARD, HUK_ALT_ERASE)
#define USAGE_KBD_SYSREQ            HID_USAGE2(HUP_KEYBOARD, HUK_SYSREQ)
#define USAGE_KBD_CANCEL            HID_USAGE2(HUP_KEYBOARD, HUK_CANCEL)
#define USAGE_KBD_CLEAR             HID_USAGE2(HUP_KEYBOARD, HUK_CLEAR)
#define USAGE_KBD_PRIOR             HID_USAGE2(HUP_KEYBOARD, HUK_PRIOR)
#define USAGE_KBD_RETURN            HID_USAGE2(HUP_KEYBOARD, HUK_RETURN)
#define USAGE_KBD_SEPARATOR         HID_USAGE2(HUP_KEYBOARD, HUK_SEPARATOR)
#define USAGE_KBD_OUT               HID_USAGE2(HUP_KEYBOARD, HUK_OUT)
#define USAGE_KBD_OPER              HID_USAGE2(HUP_KEYBOARD, HUK_OPER)
#define USAGE_KBD_CLEARAGAIN        HID_USAGE2(HUP_KEYBOARD, HUK_CLEAR_AGAIN)
#define USAGE_KBD_CRSEL             HID_USAGE2(HUP_KEYBOARD, HUK_CRSEL)
#define USAGE_KBD_EXSEL             HID_USAGE2(HUP_KEYBOARD, HUK_EXSEL)
#define USAGE_KEYPAD_00              HID_USAGE2(HUP_KEYPAD, HUKP_00)
#define USAGE_KEYPAD_000             HID_USAGE2(HUP_KEYPAD, HUKP_000)
#define USAGE_KEYPAD_THOUSANDS_SEP   HID_USAGE2(HUP_KEYPAD, HUKP_THOUSANDS_SEP)
#define USAGE_KEYPAD_DECIMAL_SEP     HID_USAGE2(HUP_KEYPAD, HUKP_DECIMAL_SEP)
#define USAGE_KEYPAD_CURRENCY_UNIT   HID_USAGE2(HUP_KEYPAD, HUKP_CURRENCY_UNIT)
#define USAGE_KEYPAD_CURRENCY_SUBUNIT HID_USAGE2(HUP_KEYPAD, \
        HUKP_CURRENCY_SUBUNIT)
#define USAGE_KEYPAD_PARENTESIS_OPEN HID_USAGE2(HUP_KEYPAD, \
        HUKP_PARENTESIS_OPEN)
#define USAGE_KEYPAD_PARENTESIS_CLOSE HID_USAGE2(HUP_KEYPAD, \
        HUKP_PARENTESIS_CLOSE)
#define USAGE_KEYPAD_BRACES_OPEN     HID_USAGE2(HUP_KEYPAD, HUKP_BRACES_OPEN)
#define USAGE_KEYPAD_BRACES_CLOSE    HID_USAGE2(HUP_KEYPAD, HUKP_BRACES_CLOSE)
#define USAGE_KEYPAD_TAB             HID_USAGE2(HUP_KEYPAD, HUKP_TAB)
#define USAGE_KEYPAD_BACKSPACE       HID_USAGE2(HUP_KEYPAD, HUKP_BACKSPACE)
#define USAGE_KEYPAD_A               HID_USAGE2(HUP_KEYPAD, HUKP_A)
#define USAGE_KEYPAD_B               HID_USAGE2(HUP_KEYPAD, HUKP_B)
#define USAGE_KEYPAD_C               HID_USAGE2(HUP_KEYPAD, HUKP_C)
#define USAGE_KEYPAD_D               HID_USAGE2(HUP_KEYPAD, HUKP_D)
#define USAGE_KEYPAD_E               HID_USAGE2(HUP_KEYPAD, HUKP_E)
#define USAGE_KEYPAD_F               HID_USAGE2(HUP_KEYPAD, HUKP_F)
#define USAGE_KEYPAD_XOR             HID_USAGE2(HUP_KEYPAD, HUKP_XOR)
#define USAGE_KEYPAD_CIRCUMFLEX      HID_USAGE2(HUP_KEYPAD, HUKP_CIRCUMFLEX)
#define USAGE_KEYPAD_PERCENT         HID_USAGE2(HUP_KEYPAD, HUKP_PERCENT)
#define USAGE_KEYPAD_LESS            HID_USAGE2(HUP_KEYPAD, HUKP_LESS)
#define USAGE_KEYPAD_BIGGER          HID_USAGE2(HUP_KEYPAD, HUKP_BIGGER)
#define USAGE_KEYPAD_AMPERSAND       HID_USAGE2(HUP_KEYPAD, HUKP_AMPERSAND)
#define USAGE_KEYPAD_AND             HID_USAGE2(HUP_KEYPAD, HUKP_AND)
#define USAGE_KEYPAD_PIPE            HID_USAGE2(HUP_KEYPAD, HUKP_PIPE)
#define USAGE_KEYPAD_OR              HID_USAGE2(HUP_KEYPAD, HUKP_OR)
#define USAGE_KEYPAD_COLON           HID_USAGE2(HUP_KEYPAD, HUKP_COLON)
#define USAGE_KEYPAD_HASH            HID_USAGE2(HUP_KEYPAD, HUKP_HASH)
#define USAGE_KEYPAD_SPACE           HID_USAGE2(HUP_KEYPAD, HUKP_SPACE)
#define USAGE_KEYPAD_AT              HID_USAGE2(HUP_KEYPAD, HUKP_AT)
#define USAGE_KEYPAD_EXECUT_MARK     HID_USAGE2(HUP_KEYPAD, HUKP_EXECUT_MARK)
#define USAGE_KEYPAD_MEMSTORE        HID_USAGE2(HUP_KEYPAD, HUKP_MEMSTORE)
#define USAGE_KEYPAD_MEMRECALL       HID_USAGE2(HUP_KEYPAD, HUKP_MEMRECALL)
#define USAGE_KEYPAD_MEMCLEAR        HID_USAGE2(HUP_KEYPAD, HUKP_MEMCLEAR)
#define USAGE_KEYPAD_MEMADD          HID_USAGE2(HUP_KEYPAD, HUKP_MEMADD)
#define USAGE_KEYPAD_MEMSUBTRACT     HID_USAGE2(HUP_KEYPAD, HUKP_MEMSUBTRACT)
#define USAGE_KEYPAD_MEMMULTIPLY     HID_USAGE2(HUP_KEYPAD, HUKP_MEMMULTIPLY)
#define USAGE_KEYPAD_MEMDIVIDE       HID_USAGE2(HUP_KEYPAD, HUKP_MEMDIVIDE)
#define USAGE_KEYPAD_SIGN            HID_USAGE2(HUP_KEYPAD, HUKP_SIGN)
#define USAGE_KEYPAD_CLEAR           HID_USAGE2(HUP_KEYPAD, HUKP_CLEAR)
#define USAGE_KEYPAD_CLEARENTRY      HID_USAGE2(HUP_KEYPAD, HUKP_CLEARENTRY)
#define USAGE_KEYPAD_BINARY          HID_USAGE2(HUP_KEYPAD, HUKP_BINARY)
#define USAGE_KEYPAD_OCTAL           HID_USAGE2(HUP_KEYPAD, HUKP_OCTAL)
#define USAGE_KEYPAD_DECIMAL         HID_USAGE2(HUP_KEYPAD, HUKP_DECIMAL)
#define USAGE_KEYPAD_HEXADECIMAL     HID_USAGE2(HUP_KEYPAD, HUKP_HEXADECIMAL)
#define USAGE_BIG_MOTOR              HID_USAGE2(HUP_MOTOR,  HUM_BIG_MOTOR)
#define USAGE_LITTLE_MOTOR           HID_USAGE2(HUP_MOTOR,  HUM_LITTLE_MOTOR)

/* Usage page macro */
#define HID_USAGE_PAGE(u) (u)>>16
/* General usage macro */
#define HID_USAGE_USAGE(u) ((u) & 0xffff)

/** @} */ /* end_group hsusb_jungo_api */

#endif
