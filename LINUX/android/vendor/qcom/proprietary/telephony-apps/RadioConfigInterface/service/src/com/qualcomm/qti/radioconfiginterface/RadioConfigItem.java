/******************************************************************************
  @file    RadioConfigItem.java

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
package com.qualcomm.qti.radioconfiginterface;

/**
 * This class describes the structure
 * of a RadioConfigItem.
 * Look in the specific APIs for
 * the usage details.
 * NOTE: Use Setter/Getter functions
 * to set/get the variables.
 */
public class RadioConfigItem<T> {

    private boolean hasItem;
    private int item;
    private boolean hasValue;
    private T value;
    private boolean hasError;
    private int error;

    public RadioConfigItem() {
        hasItem = false;
        hasValue = false;
        hasError = false;
    }

    public boolean hasItem(){
        return hasItem;
    }
    public boolean hasError() {
        return hasError;
    }
    public boolean hasValue() {
        return hasValue;
    }

    public void setItem(int item) {
        hasItem = true;
        this.item = item;
    }
    public void setError(int error){
        hasError = true;
        this.error = error;
    }
    public void setValue(T value) {
        hasValue = true;
        this.value = value;
    }

    public T getValue() {
        return value;
    }
    public int getItem() {
        return item;
    }
    public int getError(){
        return error;
    }
}
