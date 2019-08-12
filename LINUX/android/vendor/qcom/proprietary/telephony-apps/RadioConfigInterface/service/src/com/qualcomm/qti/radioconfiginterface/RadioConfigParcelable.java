/******************************************************************************
  @file    RadioConfigParcelable.java

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import android.util.Log;
import java.lang.Class;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;
import android.os.Parcelable;
import android.os.Parcel;
import com.qualcomm.qti.radioconfiginterface.LtedConfig;
import com.qualcomm.qti.radioconfiginterface.LtedCategory;


public class RadioConfigParcelable<T extends Parcelable> implements Parcelable {
  private final String LOG_TAG = "RadioConfigParcelable";

  public static final int CONFIG_ITEM_SET_LTED_CONFIG = 100;
  public static final int CONFIG_ITEM_GET_LTED_CONFIG = 101;
  public static final int CONFIG_ITEM_SET_LTED_CATEGORY = 102;
  public static final int CONFIG_ITEM_GET_LTED_CATEGORY = 103;

  private int mItem;
  private T   mData;

  public static final Parcelable.Creator<RadioConfigParcelable> CREATOR =
      new Parcelable.Creator<RadioConfigParcelable>() {
          @Override
              public RadioConfigParcelable createFromParcel(Parcel in) {
                  return new RadioConfigParcelable(in);
              }

          @Override
              public RadioConfigParcelable[] newArray(int size) {
                  return new RadioConfigParcelable[size];
              }
      };

  public RadioConfigParcelable() {
  }

  public RadioConfigParcelable(T data, int item) {
      mData = data;
      mItem = item;
  }

  public void setData(T data) {
      mData = data;
  }

  public T getData() {
      return mData;
  }

  public void setItem(int item) {
      mItem = item;
  }

  public int getItem() {
      return mItem;
  }

  public RadioConfigParcelable(Parcel in) {
      readFromParcel(in);
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
      Log.e(LOG_TAG, "writeToParcel: mItem = " + mItem);
      dest.writeInt(mItem);
      if (mData != null) {
          dest.writeInt(1);
          dest.writeValue(mData);
          Log.e(LOG_TAG, "writeToParcel: mData = " + mData);
      } else {
          dest.writeInt(0);
          Log.e(LOG_TAG, "writeToParcel: mData is null!!");
      }
  }

  public void readFromParcel(Parcel in) {
      mItem = in.readInt();

      Log.e(LOG_TAG, "readFromParcel: mItem = " + mItem);

      if (in.readInt() == 1) {
          switch(mItem) {
              case CONFIG_ITEM_SET_LTED_CONFIG:
              case CONFIG_ITEM_GET_LTED_CONFIG:
                  mData = (T) in.readValue(LtedConfig.class.getClassLoader());
                  break;
              case CONFIG_ITEM_SET_LTED_CATEGORY:
              case CONFIG_ITEM_GET_LTED_CATEGORY:
                  mData = (T) in.readValue(LtedCategory.class.getClassLoader());
                  break;
              default:
                  Log.e(LOG_TAG, "readFromParcel: unknown item : " + mItem);
                  break;
          }
      }

      if (mData != null) {
          Log.e(LOG_TAG, "readFromParcel: mData = " + mData);
      } else {
          Log.e(LOG_TAG, "readFromParcel: mData is null!!");
      }
  }

  @Override
  public int describeContents() {
      return 0;
  }
}
