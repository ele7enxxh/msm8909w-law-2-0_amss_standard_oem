/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "drTsController.h"
#include "i2c_stub.h"
#include "atmel_mxt_ts.h"


/* Registers in the Memory Map, Figure 2-1*/
#define REG_FAMILY_ID		  0x00
#define REG_VARIANT_ID		0x01
#define REG_VERSION		    0x02
#define REG_BUILD		      0x03
#define REG_MATRIX_X_SIZE	0x04
#define REG_MATRIX_Y_SIZE	0x05
#define REG_OBJECT_NUM		0x06
#define REG_OBJECT_START	0x07 /* After this objects start, 6 bytes each */

#define MXT_OBJECT_SIZE 6

/* Object types, only the ones we're interested in */
#define T5_GEN_MESSAGEPROCESSOR		  5
#define T9_TOUCH_MULTITOUCHSCREEN		9
#define T15_TOUCH_KEYARRAY		      15
#define T42_PROCI_tOUCHSUPPRESSION  42

/** Maintain the cunfiguration for the current screen layout
 */
struct panel_config {
  uint32_t width; /**< Width of the touch panel area */
  uint32_t height; /**< Height of the touch panel area */
};

/** Represents the MXT elements as returned in the object table
 */
struct object_table_element {
	uint8_t   type; /**< Object type, i.e. T5, T6, etc.*/
	uint16_t  start_address; /**< Start address when reading its messages */
	uint8_t   size; /**< Size - 1*/
	uint8_t   instances; /**< Instances - 1*/
	uint8_t   num_report_ids; /**< Number of reportIDs per instance */

	uint8_t   reportid_min; /**< this will be the minimum index for the report id */
  uint8_t   reportid_max; /**< this will be the maximum index for the report id */
};

#pragma pack(push, mxt, 1)
/** Message from the controller.
 * The message member is supposed to be variable in size, but the largest we're
 * going to read comes from T9, and it's 7 bytes.
 * This structure goes on the bus, so it needs to be packed.
 */
struct message {
	uint8_t reportid; /**< where it comes from */
	uint8_t message[7]; /**< the data, T9 is the largest with 7 bytes */
	uint8_t checksum; /**< we'll never check it...*/
};
#pragma pack(pop, mxt)

/** Placeholder where we keep track of the elements we care of.
 * They are just a small subset of the whole object table, but here we assume
 * the driver has already been configured, so we don't need any more.
 * Actually, T5 and T9 would be enough...
 */
struct mxt_data {
  // we don't need a full object table, just the objects we use
  struct object_table_element t5; /**< We write it */
  struct object_table_element t9; /**< We read it */
  //struct object_table_element t15; /**< Might read it in the future */
  //struct object_table_element t42; /**< Will read it in the future */
};

/** T9 message, with data on the touch event for a single finger.
 */
struct T9_Message {
  struct {
    uint8_t UNGRIP : 1;
    uint8_t SUPPRESS : 1;
    uint8_t AMP : 1;
    uint8_t VECTOR : 1;
    uint8_t MOVE : 1;
    uint8_t RELEASE : 1;
    uint8_t PRESS : 1;
    uint8_t DETECT : 1;
  } STATUS;
  uint8_t XPOSMSB;
  uint8_t YPOSMSB;
  struct {
    uint8_t Y : 4;
    uint8_t X : 4;
  } XYPOSLSB;
  uint8_t TCHAREA;
  uint8_t TCHAMPLITUDE;
  struct {
    uint8_t Component2 : 4;
    uint8_t Component1 : 4;
  } TCHVECTOR;
};


static struct mxt_data g_mxt_data = {0};

/** Panel configuration, with default values for MSM8974 Fluid
 */
static struct panel_config g_panel_config = {0, 0};

static uint32_t opened = 0;

/** Wrapper to read a single MXT message
 */
static int32_t mxtReadMessage(struct message *mex)
{
  return i2cReadReg(g_mxt_data.t5.start_address,mex,sizeof(struct message));
}

/** Logging function, T9 message
 */
void dumpT9(const struct message *mex) {
  LOG_D("ReportID=%u",mex->reportid);
  LOG_D("STATUS=%u %u %u %u %u %u %u %u",
    (mex->message[0] & 0x80) >> 7,
    (mex->message[0] & 0x40) >> 6,
    (mex->message[0] & 0x20) >> 5,
    (mex->message[0] & 0x10) >> 4,
    (mex->message[0] & 0x08) >> 3,
    (mex->message[0] & 0x04) >> 2,
    (mex->message[0] & 0x02) >> 1,
    (mex->message[0] & 0x01));
  LOG_D("XPOSMSB=0x%08X",mex->message[1]);
  LOG_D("YPOSMSB=0x%08X",mex->message[2]);
  LOG_D("XYPOSLSB= 0x%04X / 0x%04X",
    (mex->message[3] & 0xF0) >> 4,
    (mex->message[3] & 0x0F)
    );
  LOG_D("TCHAREA=0x%08X",mex->message[4]);
  LOG_D("TCHAMPLITUDE=0x%08X",mex->message[5]);
  LOG_D("TCHVECTOR= %d / %d",
    (mex->message[6] & 0xF0) >> 4,
    (mex->message[6] & 0x0F)
    );
}

/** Logging function, T15 message
 */
void dumpT15(const struct message *mex) {
  LOG_D("ReportID=%u",mex->reportid);
  LOG_D("STATUS=DETECT=%u",
    (mex->message[0] & 0x80) >> 7);
  LOG_D("KEYSTATE 7-0  =%u %u %u %u %u %u %u %u",
    (mex->message[1] & 0x80) >> 7,
    (mex->message[1] & 0x40) >> 6,
    (mex->message[1] & 0x20) >> 5,
    (mex->message[1] & 0x10) >> 4,
    (mex->message[1] & 0x08) >> 3,
    (mex->message[1] & 0x04) >> 2,
    (mex->message[1] & 0x02) >> 1,
    (mex->message[1] & 0x01));
  LOG_D("KEYSTATE 15-8  =%u %u %u %u %u %u %u %u",
    (mex->message[2] & 0x80) >> 7,
    (mex->message[2] & 0x40) >> 6,
    (mex->message[2] & 0x20) >> 5,
    (mex->message[2] & 0x10) >> 4,
    (mex->message[2] & 0x08) >> 3,
    (mex->message[2] & 0x04) >> 2,
    (mex->message[2] & 0x02) >> 1,
    (mex->message[2] & 0x01));
  LOG_D("KEYSTATE 23-16=%u %u %u %u %u %u %u %u",
    (mex->message[3] & 0x80) >> 7,
    (mex->message[3] & 0x40) >> 6,
    (mex->message[3] & 0x20) >> 5,
    (mex->message[3] & 0x10) >> 4,
    (mex->message[3] & 0x08) >> 3,
    (mex->message[3] & 0x04) >> 2,
    (mex->message[3] & 0x02) >> 1,
    (mex->message[3] & 0x01));
  LOG_D("KEYSTATE 31-24=%u %u %u %u %u %u %u %u",
    (mex->message[4] & 0x80) >> 7,
    (mex->message[4] & 0x40) >> 6,
    (mex->message[4] & 0x20) >> 5,
    (mex->message[4] & 0x10) >> 4,
    (mex->message[4] & 0x08) >> 3,
    (mex->message[4] & 0x04) >> 2,
    (mex->message[4] & 0x02) >> 1,
    (mex->message[4] & 0x01));
}

/** Logging function, T42 message
 */
void dumpT42(const struct message *mex) {
  LOG_D("ReportID=%u",mex->reportid);
  LOG_D("status=TCHSUP=%u",
    (mex->message[0] & 0x01));
}

/** Process a T9 message, decoding the position in it
 */
void processT9(const struct T9_Message *mex, uint32_t *x, uint32_t *y, uint32_t *code)
{
  *code = TOUCH_EVENT_NONE;
  do {
    if (g_panel_config.width < 1024 ) { // 10-bit format
      *x = (mex->XPOSMSB << 2) | (mex->XYPOSLSB.X >> 2);
    } else { // 12-bit format
      *x = (mex->XPOSMSB << 4) | (mex->XYPOSLSB.X);
    }
    if (g_panel_config.height < 1024 ) { // 10-bit format
      *y = (mex->YPOSMSB << 2) | (mex->XYPOSLSB.Y >> 2);
    } else { // 12-bit format
      *y = (mex->YPOSMSB << 4) | (mex->XYPOSLSB.Y);
    }
    if (mex->STATUS.DETECT == 0) {
      if (mex->STATUS.RELEASE == 1) {
        *code |= TOUCH_EVENT_UP;
      }
    }
    if (mex->STATUS.MOVE) {
      *code |= TOUCH_EVENT_MOVE;
    }
    if (mex->STATUS.PRESS) {
      *code |= TOUCH_EVENT_DOWN;
    }
  } while (0);
}

/** Open the I2C bus and initializes our MXT structures
 */
uint32_t drTsOpen(const uint32_t width, const uint32_t height)
{
  int32_t rv = 0;
  uint8_t obj_num = 0;
  uint8_t i = 0;
  uint32_t reportid = 0;
  struct object_table_element element = {0};
  ENTER;
  do {
    rv = i2cOpen();
    if (rv != 0) {
      LOG_E("Error opening the I2C bus: 0x%08X", rv);
      break;
    }
    opened = 1;
    
    // how many object is the object table made of
    rv = i2cReadReg(REG_OBJECT_NUM, &obj_num, sizeof(obj_num));
    if (rv) break;
    
    LOG_D("Number of objects: %u",obj_num);
    // now we iterate through them to fill in the ones we actually care for
    for (i = 0; i < obj_num; i++) {
      uint8_t buffer[MXT_OBJECT_SIZE];
      rv = i2cReadReg(REG_OBJECT_START + MXT_OBJECT_SIZE*i, &buffer, sizeof(buffer));
      if (rv) break;
      element.type = buffer[0];
      element.start_address = (buffer[2] << 8) | buffer[1];
      element.size = buffer[3];
      element.instances = buffer[4];
      element.num_report_ids = buffer[5];
      element.reportid_min = element.reportid_max; // the previous max
      reportid += element.num_report_ids * (element.instances + 1);
      element.reportid_max = reportid;
      LOG_D("Object: %u/%04X/%u/%u/%u/%u %u-%u",
        element.type,element.start_address,element.size,
        element.instances+1,element.num_report_ids,
        element.reportid_min,element.reportid_max);

      switch(element.type) {
        case T5_GEN_MESSAGEPROCESSOR: g_mxt_data.t5 = element; break;
        case T9_TOUCH_MULTITOUCHSCREEN: g_mxt_data.t9 = element; break;
        //case T15_TOUCH_KEYARRAY: g_mxt_data.t15 = element; break;
        //case T42_PROCI_tOUCHSUPPRESSION: g_mxt_data.t42 = element; break;
      }
    }
    if (rv) break;
    LOG_D("T5: %u/%04X/%u/%u/%u/%u/%u",
      g_mxt_data.t5.type,g_mxt_data.t5.start_address,g_mxt_data.t5.size,
      g_mxt_data.t5.instances,g_mxt_data.t5.num_report_ids,
      g_mxt_data.t5.reportid_min,g_mxt_data.t5.reportid_max
    );
    LOG_D("T9: %u/%04X/%u/%u/%u/%u/%u",
      g_mxt_data.t9.type,g_mxt_data.t9.start_address,g_mxt_data.t9.size,
      g_mxt_data.t9.instances,g_mxt_data.t9.num_report_ids,
      g_mxt_data.t9.reportid_min,g_mxt_data.t9.reportid_max
    );
  } while (0);
  // cleanup if needed
  if (opened && (rv != 0)) {
    i2cClose();
    opened = 0;
  } else {
    if (width != 0)
      g_panel_config.width = width;
    else
      g_panel_config.width = DEFAULT_PANEL_WIDTH;
    if (height != 0)
      g_panel_config.height = height;
    else
      g_panel_config.height = DEFAULT_PANEL_HEIGHT;
  }
  EXITV((uint32_t)rv);
}

/** Release the I2C bus
 */
uint32_t drTsClose(void)
{
  int32_t rv = 0;
  ENTER;
  do {
    if (!opened) {
      LOG_E("Device not opened.");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    rv = i2cClose();
    if (rv != 0) {
      LOG_E("Error closing the I2C bus: 0x%08X", rv);
      break;
    }
  } while (0);
  EXITV((uint32_t)rv);
}

/** Process all available touch events
 */
uint32_t drTsProcessEvents(void)
{
  int32_t rv = 0;
  uint8_t reportid = 0;
  struct message mex;
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t code = 0;
  ENTER;
  do {
    if (!opened) {
      LOG_E("Device not opened.");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    // now read all available events (=messages) from the controller
    do {
      rv = mxtReadMessage(&mex);
      if (rv) break;
      reportid = mex.reportid;
      LOG_D("reportid=%02u",reportid);

      if (reportid == 0) {
        // reserved to Atmel
        continue;
      }
      
      if ((reportid >= g_mxt_data.t9.reportid_min) &&
          (reportid <= g_mxt_data.t9.reportid_max)) {
        dumpT9(&mex);
        x=y=code=0;
        processT9((struct T9_Message *)(mex.message), &x, &y, &code);
        drNotifyEvent(x, y, code, reportid - g_mxt_data.t9.reportid_min - 1);
      }

    } while (reportid != 0xFF); // 255 = invalid message
    if (rv) {
      LOG_E("Failed to read from MXT controller: 0x%08X");
      break;
    }
  } while (0);
  EXITV((uint32_t)rv);
}
