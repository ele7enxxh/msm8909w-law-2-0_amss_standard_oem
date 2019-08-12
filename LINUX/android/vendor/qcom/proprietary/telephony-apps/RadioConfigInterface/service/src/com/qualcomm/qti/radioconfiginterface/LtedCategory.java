/******************************************************************************
  @file    LtedCategory.java

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;
import android.os.Parcelable;
import android.os.Parcel;
import com.qualcomm.qti.radioconfiginterface.RadioConfigItem;

/**
 * LTE-D Category.
 * Contains the Application ID (OS App ID) and the corresponding Category.
 */
public class LtedCategory implements Parcelable {
  /** Transmit PACs of associated OS App ID at a higher rate. */
  public static final int CATEGORY_HIGH     = 0;
  /** Transmit PACs of associated OS App ID at a medium rate. */
  public static final int CATEGORY_MEDIUM   = 1;
  /** Transmit PACs of associated OS App ID at a low rate. */
  public static final int CATEGORY_LOW      = 2;
  /** Transmit PACs of associated OS App ID at a very low rate. */
  public static final int CATEGORY_VERY_LOW = 3;
  /** Invalid type identifier. */
  public static final int CATEGORY_INVALID  = 4;

  /** OS App ID */
  private String mOsAppId;
  /** LTE-D Category - the transmission priority of the OS App ID */
  private int    mCategory;

  /**
   * Get OS App ID.
   *
   * @return The OS App ID
   */
  public String getOsAppId() {
      return mOsAppId;
  }

  /**
   * Set OS App ID.
   *
   * @return none
   */
  public void setOsAppId(String osAppId) {
      mOsAppId = osAppId;
  }

  /**
   * Get category
   *
   * @return The category
   */
  public int getCategory() {
      return mCategory;
  }

  /**
   * Set category
   *
   * @return none
   */
  public void setCategory(int category) {
      mCategory = category;
  }


  /**
   * Copy LtedCategory
   *
   * @return none
   */
  public void copyFrom(LtedCategory c) {
      mOsAppId  = c.mOsAppId;
      mCategory = c.mCategory;
  }

  /**
   * Covert the LtedCategory to string.
   *
   * @return string
   */
  public String toString() {
      StringBuilder builder = new StringBuilder();
      builder.append("LtedCategory{");
      builder.append("OsAppId=").append(mOsAppId);
      builder.append(", Category=").append(mCategory);
      builder.append("}");
      return builder.toString();
  }


  public static final Parcelable.Creator<LtedCategory> CREATOR =
                                      new Parcelable.Creator<LtedCategory>() {

      public LtedCategory createFromParcel(Parcel in) {
          return new LtedCategory(in);
      }

      public LtedCategory[] newArray(int size) {
          return new LtedCategory[size];
      }
  };

  /**
   * Default Constructor.
   */
  public LtedCategory(){
  }

  /**
   * Constructor.
   */
  public LtedCategory(Parcel in){
      readFromParcel(in);
  }

  public void readFromParcel(Parcel in) {
      // Os App Id
      if (in.readInt() == 1) {
          mOsAppId = in.readString();
      }

      // Category
      mCategory = in.readInt();
  }

  @Override
  public void writeToParcel(Parcel out, int flags) {
      // Os App Id
      if (mOsAppId != null) {
          out.writeInt(1);
          out.writeString(mOsAppId);
      } else {
          out.writeInt(0);
      }

      // Category
      out.writeInt(mCategory);
  }

  @Override
  public int describeContents() {
      return 0;
  }
}
