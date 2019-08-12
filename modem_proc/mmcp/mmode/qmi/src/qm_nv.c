#include "nv.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_mmode_msgr_msg.h"
#include "qm_comdef.h"
#include "qm_nv.h"

struct qm_nv_cache_s
{
  struct
  {
    boolean inited;
    byte    tty;
  } tty;
};

struct qm_nv_global_s
{
  struct qm_nv_cache_s cache[QM_SUBS_MAX];
  boolean bsr_changed;
};

// private data
static struct qm_nv_global_s global;

// process NV_SD_CFG_ITEMS_I read
static nv_stat_enum_type process_bsr_get( enum qm_subs_e subs, nv_item_type *data )
{
  memset( data, 0x00, sizeof(nv_item_type) );
  return qmi_mmode_get_nv_item_ext( NV_SD_CFG_ITEMS_I, data, 0 );
}

// process NV_SD_CFG_ITEMS_I write
static nv_stat_enum_type process_bsr_put( enum qm_subs_e subs, nv_item_type *data )
{
  nv_stat_enum_type ret = NV_FAIL_S; // some error
  ret = qmi_mmode_put_nv_item_ext( NV_SD_CFG_ITEMS_I, data, 0 );
  if(ret == NV_DONE_S)
  {
    global.bsr_changed = TRUE;
  }
  return ret;
}

// process NV_TTY_I read
static nv_stat_enum_type process_tty_get( enum qm_subs_e subs, nv_item_type *data )
{
  nv_stat_enum_type ret = NV_FAIL_S; // some error

  if ( QM_SUBS_MIN < subs && subs < QM_SUBS_MAX )
  {
    if ( global.cache[subs].tty.inited )
    {
      data->tty = global.cache[subs].tty.tty;
      ret       = NV_DONE_S;
    }
    else
    {
      memset( data, 0x00, sizeof(nv_item_type) );
      ret = qmi_mmode_get_nv_item_ext( NV_TTY_I, data, (uint16)subs );

      if ( ret == NV_DONE_S )
      {
        if ( data->tty < QM_NV_TTY_MODE_MAX )
        {
          global.cache[subs].tty.tty    = data->tty;
          global.cache[subs].tty.inited = TRUE;
        }
        else
        {
          memset( data, 0x00, sizeof(nv_item_type) );
          data->tty = QM_NV_TTY_MODE_OFF;

          ret = qmi_mmode_put_nv_item_ext( NV_TTY_I, data, QM_SUBS_PRI );

          if ( ret == NV_DONE_S )
          {
            global.cache[subs].tty.tty    = data->tty;
            global.cache[subs].tty.inited = TRUE;
          }
          else
          {
            QM_MSG_ERROR_1("TTY NV write failed %d", ret);
          }
        }
      }
    }
  }

  QM_MSG_HIGH_3("TTY NV get ret %d val %d subs %d", ret, global.cache[subs].tty.tty, subs);

  return ret;
}

// process NV_TTY_I write
static nv_stat_enum_type process_tty_put( enum qm_subs_e subs, nv_item_type *data )
{
  nv_stat_enum_type                            ret = NV_FAIL_S; // some error
  mmode_qmi_voice_tty_mode_change_ind_msg_type msg_tty_pkt;
  errno_enum_type                              msgr_error;
  boolean                                      write_to_nv = TRUE;
  nv_item_type temp_nv_buf;

  if ( (QM_SUBS_MIN < subs) && (subs < QM_SUBS_MAX) && (data->tty < QM_NV_TTY_MODE_MAX))
  {
    if(global.cache[subs].tty.inited)
    {
      if(data->tty == global.cache[subs].tty.tty)
      {
        // nothing to do as the values are identical
        write_to_nv = FALSE;
        ret = NV_DONE_S;
      }
    }
    else
    {
      /* TTY not initialized yet. Read it and check if write is needed */
      memset(&temp_nv_buf, 0, sizeof(temp_nv_buf));
      if (qmi_mmode_get_nv_item_ext( NV_TTY_I, &temp_nv_buf, (uint16)subs ) == NV_DONE_S)
      {
        global.cache[subs].tty.inited = TRUE;
        global.cache[subs].tty.tty = temp_nv_buf.tty;
        if(temp_nv_buf.tty == data->tty)
        {
          // nothing to do as the values are identical
          write_to_nv = FALSE;
          ret = NV_DONE_S;
        }
      }
    }

    if(write_to_nv)
    {
      ret = qmi_mmode_put_nv_item_ext( NV_TTY_I, data, subs );

      if ( ret == NV_DONE_S )
      {
        global.cache[subs].tty.tty    = data->tty;
        global.cache[subs].tty.inited = TRUE;

        // send msgr after this NV is updated
        memset( &msg_tty_pkt, 0x00, sizeof(msg_tty_pkt) );

        msgr_init_hdr( (msgr_hdr_struct_type *)&msg_tty_pkt, MSGR_QMI_VOICE, QMI_VOICE_VOICE_TTY_MODE_CHANGE_IND_IND );

        msg_tty_pkt.tty_mode = (mmode_qmi_voice_tty_mode_e_type)data->tty;

        msgr_error = msgr_send((msgr_hdr_struct_type*)&msg_tty_pkt, sizeof(mmode_qmi_voice_tty_mode_change_ind_msg_type));
      
        QM_MSG_HIGH_4("TTY NV put: subs %d val %d ret %d msg_error %d",
                      subs, global.cache[subs].tty.tty, ret, msgr_error);
      }
      else
      {
        QM_MSG_ERROR_3("TTY NV write failed %d, subs %d val %d",
                        ret, subs, global.cache[subs].tty.tty);
        ret = NV_FAIL_S;
      }
    }
    }

  return ret;
}

#ifdef FEATURE_MODEM_CONFIG_REFRESH
//called in case of MCFG refresh event (sim swap)
void qm_nv_mcfg_refresh_tty( uint16 subs_index )
{
  nv_stat_enum_type ret = NV_FAIL_S; // some error
  nv_item_type data;
  memset( &data, 0x00, sizeof(nv_item_type) );

  ret = qmi_mmode_get_nv_item_ext( NV_TTY_I, &data, subs_index );

  if ( ret == NV_DONE_S )
  {
    if ( data.tty < QM_NV_TTY_MODE_MAX )
    {
      global.cache[subs_index].tty.tty    = data.tty;
      global.cache[subs_index].tty.inited = TRUE;
    }
    else
    {
      global.cache[subs_index].tty.inited = FALSE;
    }
  }
  else
  {
    global.cache[subs_index].tty.inited = FALSE;
  }
  QM_MSG_HIGH_4("qm_nv_mcfg_refresh_tty: ret %d subs %d tty %d tty_inited %d",
                ret, subs_index, data.tty, global.cache[subs_index].tty.inited);
}
#endif // FEATURE_MODEM_CONFIG_REFRESH

//determines if bsr value has been changed and is pending reset
boolean qm_nv_bsr_changed( void )
{
  return global.bsr_changed;
}

// init nv module
void qm_nv_init( void )
{
  int i;

  for ( i=0; i<QM_SUBS_MAX; i++ )
  {
    global.cache[i].tty.inited = FALSE;
    global.cache[i].tty.tty    = QM_NV_TTY_MODE_OFF;
  }
}

// read NV item
nv_stat_enum_type qm_nv_get( nv_items_enum_type item, nv_item_type *data, enum qm_subs_e subs )
{
  nv_stat_enum_type ret = NV_FAIL_S; // some error
  
  switch ( item )
  {
    case NV_TTY_I:
      ret = process_tty_get( subs, data );
      break;

    case NV_SD_CFG_ITEMS_I:
      ret = process_bsr_get( subs, data );
      break;
	  
    default:
      break;
  }

  return ret;
}

// write NV item
nv_stat_enum_type qm_nv_put( nv_items_enum_type item, nv_item_type *data, enum qm_subs_e subs )
{
  nv_stat_enum_type ret = NV_FAIL_S; // some error
  
  switch ( item )
  {
    case NV_TTY_I:
      ret = process_tty_put( subs, data );
      break;

    case NV_SD_CFG_ITEMS_I:
      ret = process_bsr_put( subs, data );
      break;
	  
    default:
      break;
  }

  return ret;
}

