/******************************************************************************
  @file    RadioConfigIntegerList.java

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
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

public class RadioConfigIntegerList implements Parcelable {

  List<RadioConfigItem<Integer>> configList;

  public RadioConfigIntegerList() {
  }

  public void setList(List<RadioConfigItem<Integer>> configIntegerList){
      configList = new ArrayList<RadioConfigItem<Integer>>();

      RadioConfigItem<Integer> item;
      for(RadioConfigItem<Integer> temp : configIntegerList){
          item = new RadioConfigItem<Integer>();
          if(temp.hasItem()){
              item.setItem(temp.getItem());
          }
          if(temp.hasError()){
              item.setError(temp.getError());
          }
          if(temp.hasValue()){
              item.setValue(temp.getValue());
          }
          configList.add(item);
      }
  }

  public List<RadioConfigItem<Integer>> getList(){
      return configList;
  }

  public RadioConfigIntegerList(Parcel in){
      //decode from parcel
      configList = new ArrayList<RadioConfigItem<Integer>>();
      RadioConfigItem<Integer> item;
      int size = in.readInt();
      int has_field;
      for(int i = 0; i < size; i++) {
        item = new RadioConfigItem<Integer>();

        //has item
        has_field = in.readInt();
        if(has_field > 0){
            item.setItem(in.readInt());
        }

        //has value
        has_field = in.readInt();
        if(has_field > 0){
            item.setValue(in.readInt());
        }

        has_field = in.readInt();
        if(has_field > 0){
            item.setError(in.readInt());
        }
        configList.add(item);
      }
  }

  @Override
  public int describeContents() {
      return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags){
      //put size of the list first and
      //then the details in order - item -> value -> error

      int size = configList.size();
      dest.writeInt(size);

      if(size > 0) {
          for(RadioConfigItem<Integer> item : configList) {
              //has item
              dest.writeInt(item.hasItem() ? 1 : 0);
              if(item.hasItem()){
                  dest.writeInt(item.getItem());
              }
              //has value
              dest.writeInt(item.hasValue() ? 1 : 0);
              if(item.hasValue()){
                  dest.writeInt(item.getValue());
              }
              //has error
              dest.writeInt(item.hasError() ? 1 : 0);
              if(item.hasError()){
                  dest.writeInt(item.getError());
              }
          }
      }
  }

  public static final Parcelable.Creator<RadioConfigIntegerList> CREATOR =
                                      new Parcelable.Creator<RadioConfigIntegerList>() {

      public RadioConfigIntegerList createFromParcel(Parcel in) {
          return new RadioConfigIntegerList(in);
      }

      public RadioConfigIntegerList[] newArray(int size) {
          return new RadioConfigIntegerList[size];
      }
  };
}
