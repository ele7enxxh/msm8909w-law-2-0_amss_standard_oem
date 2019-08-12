/******************************************************************************
 * @file    TestFunc.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import com.samautotester.model.CardPowerDownData;
import com.samautotester.model.CardPowerUpData;
import com.samautotester.model.ConnectServiceData;
import com.samautotester.model.GetCardATRData;
import com.samautotester.model.GetCardStateData;
import com.samautotester.model.GetSlotMappingData;
import com.samautotester.model.RegisteredCallBackData;
import com.samautotester.model.State;
import com.samautotester.model.StreamApduData;
import com.samautotester.model.SwitchSlotData;

import java.util.List;
import java.util.concurrent.atomic.AtomicReference;

public enum TestFunc {
    INDIVIDUAL, MULTIPLE;

    private GetCardStateData mGetCardStateData;
    private StreamApduData mStreamApduData;
    private SwitchSlotData mSwitchSlotData;
    private CardPowerUpData mCardPowerUpData;
    private CardPowerDownData mCardPowerDownData;
    private RegisteredCallBackData mRegisteredCallBackData;
    private ConnectServiceData mConnectServiceData;
    private GetCardATRData mGetCardATRData;
    private GetSlotMappingData mGetSlotMapping;
    private volatile boolean set_StreamApdu_callbackResult = false;
    private volatile boolean set_SwitchSlotData_callbackResult = false;
    private volatile boolean set_CardPowerUpData_callbackResult = false;
    private volatile boolean set_CardPowerDown_callbackResult = false;

    public void flushData() {
        mGetCardStateData = null;
        mStreamApduData = null;
        mSwitchSlotData = null;
        mCardPowerUpData = null;
        mCardPowerDownData = null;
        mRegisteredCallBackData = null;
        mConnectServiceData = null;
        mGetCardATRData = null;
        mGetSlotMapping = null;
    }

    public void createNewData() {
        mGetCardStateData = new GetCardStateData();
        mStreamApduData = new StreamApduData();
        mSwitchSlotData = new SwitchSlotData();
        mCardPowerUpData = new CardPowerUpData();
        mCardPowerDownData = new CardPowerDownData();
        mRegisteredCallBackData = new RegisteredCallBackData();
        mConnectServiceData = new ConnectServiceData();
        mGetCardATRData = new GetCardATRData();
        mGetSlotMapping = new GetSlotMappingData();
    }

    public boolean is_GetCardStatusDataAvailable() {
        if (mGetCardStateData == null)
            return false;
        return true;
    }

    public boolean is_StreamAPDUDataAvailable() {
        if (mStreamApduData == null)
            return false;
        return true;
    }

    public boolean is_SwitchSlotDataAvailable() {
        if (mSwitchSlotData == null)
            return false;
        return true;
    }

    public boolean is_CardPowerUpDataAvailable() {
        if (mCardPowerUpData == null)
            return false;
        return true;
    }

    public boolean is_CardPowerDownDataAvailable() {
        if (mCardPowerDownData == null)
            return false;
        return true;
    }

    /*
     * Below functionality is for 'GetCardState'
     *
     * Input: slotID
     *
     * Output: returnValue result
     *
     * State
     */

    public String get_CardState_slotID() {
        return mGetCardStateData.mSlotID;
    }

    public void set_CardState_slotID(String slotID) {
        mGetCardStateData.mSlotID = slotID;
    }

    public int get_CardState_returnValue() {
        return mGetCardStateData.mReturnValue;
    }

    public void set_CardState_returnValue(int value) {
        mGetCardStateData.mReturnValue = value;
    }

    public Object get_CardState_result() {
        return mGetCardStateData.mResult;
    }

    public void set_CardState_result(Object result) {
        mGetCardStateData.mResult = result;
    }

    public void set_CardState_state(State state) {
        mGetCardStateData.state = state;
    }

    public State get_CardState_state() {
        return mGetCardStateData.state;
    }

    public void set_CardState_callback(Object callback) {
        mGetCardStateData.mCardStateCallback = callback;
    }

    public Object get_CardState_callback() {
        return mGetCardStateData.mCardStateCallback;
    }

    /*
     * Below functionality is for 'GetCardATR'
     *
     * Input: slotID
     *
     * Output: returnValue result
     *
     * State
     */

    public String get_CardATR_slotID() {
        return mGetCardATRData.mSlotID;
    }

    public void set_CardATR_slotID(String slotID) {
        mGetCardATRData.mSlotID = slotID;
    }

    public int get_CardATR_returnValue() {
        return mGetCardATRData.mReturnValue;
    }

    public void set_CardATR_returnValue(int value) {
        mGetCardATRData.mReturnValue = value;
    }

    public Object get_CardATR_result() {
        return mGetCardATRData.mResult;
    }

    public void set_CardATR_result(Object result) {
        mGetCardATRData.mResult = result;
    }

    public void set_CardATR_state(State state) {
        mGetCardATRData.state = state;
    }

    public State get_CardATR_state() {
        return mGetCardATRData.state;
    }

    public void set_CardATR_callback(Object obj) {
        mGetCardATRData.mCardATRCallback = obj;
    }

    public Object get_CardATR_callback() {
        return mGetCardATRData.mCardATRCallback;
    }

    /*
     * Below functionality is for 'GetSlotMapping'
     *
     * //Input: //slotMap
     *
     * Output: returnValue result
     *
     * State
     */

    public void set_GetSlotMapping_slotMap(int[] value) {
        mGetSlotMapping.mSlotMap = value;
    }

    public int[] get_GetSlotMapping_slotMap() {
        return mGetSlotMapping.mSlotMap;
    }

    public void set_GetSlotMapping_returnValue(int value) {
        mGetSlotMapping.mReturnValue = value;
    }

    public int get_GetSlotMapping_returnValue() {
        return mGetSlotMapping.mReturnValue;
    }

    public void set_GetSlotMapping_result(Object value) {
        mGetSlotMapping.mResult = value;
    }

    public Object get_GetSlotMapping_result() {
        return mGetSlotMapping.mResult;
    }

    public void set_GetSlotMapping_state(State state) {
        mGetSlotMapping.state = state;
    }

    public State get_GetSlotMapping_state() {
        return mGetSlotMapping.state;
    }

    /*
     * Below functionality is for 'StreamAPDU'
     *
     * Input: slotID class_type instruction param1 param2 param3 apduCmd
     *
     * Output: returnValue result
     *
     * State
     */

    public String get_StreamApdu_slotID() {
        return mStreamApduData.mSlotID;
    }

    public void set_StreamApdu_slotID(String slotID) {
        mStreamApduData.mSlotID = slotID;
    }

    public String get_StreamApdu_classType() {
        return mStreamApduData.mClassType;
    }

    public void set_StreamApdu_classType(String classType) {
        mStreamApduData.mClassType = classType;
    }

    public String get_StreamApdu_instruction() {
        return mStreamApduData.mInstruction;
    }

    public void set_StreamApdu_instruction(String instruction) {
        mStreamApduData.mInstruction = instruction;
    }

    public String get_StreamApdu_param1() {
        return mStreamApduData.mParam1;
    }

    public void set_StreamApdu_param1(String param1) {
        mStreamApduData.mParam1 = param1;
    }

    public String get_StreamApdu_param2() {
        return mStreamApduData.mParam2;
    }

    public void set_StreamApdu_param2(String param2) {
        mStreamApduData.mParam2 = param2;
    }

    public String get_StreamApdu_param3() {
        return mStreamApduData.mParam3;
    }

    public void set_StreamApdu_param3(String param3) {
        mStreamApduData.mParam3 = param3;
    }

    public String get_StreamApdu_command() {
        return mStreamApduData.mAPDUCmd;
    }

    public void set_StreamApdu_command(String apduCommand) {
        mStreamApduData.mAPDUCmd = apduCommand;
    }

    public void set_StreamApdu_returnValue(int returnValue) {
        mStreamApduData.mReturnValue = returnValue;
    }

    public int get_StreamApdu_returnValue() {
        return mStreamApduData.mReturnValue;
    }

    public void set_StreamApdu_result(Object result) {
        mStreamApduData.mResult = result;
    }

    public Object get_StreamApdu_result() {
        return mStreamApduData.mResult;
    }

    public void set_StreamApdu_callbackResult(AtomicReference<String> result) {
        mStreamApduData.mCallbackResult = result;
        set_StreamApdu_callbackResult = true;
    }

    public String get_StreamApdu_callbackResult() {
        int count = 0;
        boolean timeout = false;
        while (!set_StreamApdu_callbackResult) {
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            count++;
            if (count == 20) {
                timeout = true;
                break;
            }
        }
        set_StreamApdu_callbackResult = false;
        return timeout ? "ASYNC RESPONSE TIMEOUT - 5:Seconds"
                : mStreamApduData.mCallbackResult.get();
    }

    public void set_StreamApdu_state(State state) {
        mStreamApduData.state = state;
    }

    public State get_StreamApdu_state() {
        return mStreamApduData.state;
    }

    /*
     * Below functionality is for 'SwitchSlot'
     *
     * Input: slotList
     *
     * Output: returnValue result
     *
     * State
     */

    public List<Integer> get_SwitchSlot_slotList() {
        return mSwitchSlotData.mSlotList;
    }

    public void set_SwitchSlot_slotList(List<Integer> list) {
        mSwitchSlotData.mSlotList = list;
    }

    public void set_SwitchSlot_returnValue(int returnValue) {
        mSwitchSlotData.mReturnValue = returnValue;
    }

    public int get_SwitchSlot_returnValue() {
        return mSwitchSlotData.mReturnValue;
    }

    public void set_SwitchSlot_result(Object result) {
        mSwitchSlotData.mResult = result;
    }

    public Object get_SwitchSlot_result() {
        return mSwitchSlotData.mResult;
    }

    public void set_SwitchSlot_state(State state) {
        mSwitchSlotData.state = state;
    }

    public State get_SwitchSlot_state() {
        return mStreamApduData.state;
    }

    public void set_SwitchSlot_callbackResult(AtomicReference<String> result) {
        mSwitchSlotData.mCallbackResult = result;
        set_SwitchSlotData_callbackResult = true;
    }

    public String get_SwitchSlot_callbackResult() {
        int count = 0;
        boolean timeout = false;
        while (!set_SwitchSlotData_callbackResult) {
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            count++;
            if (count == 20) {
                timeout = true;
                break;
            }
        }
        set_SwitchSlotData_callbackResult = false;
        return timeout ? "ASYNC RESPONSE TIMEOUT - 5:Seconds"
                : mSwitchSlotData.mCallbackResult.get();
    }

    /*
     * Below functionality is for 'CardPowerUp'
     *
     * Input: slotID
     *
     * Output: returnValue result
     *
     * State
     */

    public String get_CardPowerUp_slotID() {
        return mCardPowerUpData.mSlotID;
    }

    public void set_CardPowerUp_slotID(String slotID) {
        mCardPowerUpData.mSlotID = slotID;
    }

    public int get_CardPowerUp_returnValue() {
        return mCardPowerUpData.mReturnValue;
    }

    public void set_CardPowerUp_returnValue(int value) {
        mCardPowerUpData.mReturnValue = value;
    }

    public Object get_CardPowerUp_result() {
        return mCardPowerUpData.mResult;
    }

    public void set_CardPowerUp_result(Object result) {
        mCardPowerUpData.mResult = result;
    }

    public void set_CardPowerUp_state(State state) {
        mCardPowerUpData.state = state;
    }

    public State get_CardPowerUp_state() {
        return mCardPowerUpData.state;
    }

    public void set_CardPowerUp_callbackResult(AtomicReference<String> result) {
        mCardPowerUpData.mCallbackResult = result;
        set_CardPowerUpData_callbackResult = true;
    }

    public String get_CardPowerUp_callbackResult() {
        int count = 0;
        boolean timeout = false;
        while (!set_CardPowerUpData_callbackResult) {
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            count++;
            if (count == 20) {
                timeout = true;
                break;
            }
        }
        set_CardPowerUpData_callbackResult = false;
        return timeout ? "ASYNC RESPONSE TIMEOUT - 5:Seconds"
                : mCardPowerUpData.mCallbackResult.get();
    }

    /*
     * Below functionality is for 'CardPowerDown'
     *
     * Input: slotID
     *
     * Output: returnValue result
     *
     * State
     */

    public String get_CardPowerDown_slotID() {
        return mCardPowerDownData.mSlotID;
    }

    public void set_CardPowerDown_slotID(String slotID) {
        mCardPowerDownData.mSlotID = slotID;
    }

    public int get_CardPowerDown_returnValue() {
        return mCardPowerDownData.mReturnValue;
    }

    public void set_CardPowerDown_returnValue(int value) {
        mCardPowerDownData.mReturnValue = value;
    }

    public Object get_CardPowerDown_result() {
        return mCardPowerDownData.mResult;
    }

    public void set_CardPowerDown_result(Object result) {
        mCardPowerDownData.mResult = result;
    }

    public void set_CardPowerDown_state(State state) {
        mCardPowerDownData.state = state;
    }

    public State get_CardPowerDown_state() {
        return mCardPowerDownData.state;
    }

    public void set_CardPowerDown_callbackResult(AtomicReference<String> result) {
        mCardPowerDownData.mCallbackResult = result;
        set_CardPowerDown_callbackResult = true;
    }

    public String get_CardPowerDown_callbackResult() {
        int count = 0;
        boolean timeout = false;
        while (!set_CardPowerDown_callbackResult) {
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            count++;
            if (count == 20) {
                timeout = true;
                break;
            }
        }
        set_CardPowerDown_callbackResult = false;
        return timeout ? "ASYNC RESPONSE TIMEOUT - 5:Seconds"
                : mCardPowerDownData.mCallbackResult.get();
    }

    /*
     * Below functionality is for 'RegisterCallBack'
     *
     * Output: returnValue
     *
     * State
     */
    public void set_RegisteredCallBack_returnValue(int value) {
        mRegisteredCallBackData.mReturnValue = value;
    }

    public int get_RegisteredCallBack_returnValue() {
        return mRegisteredCallBackData.mReturnValue;
    }

    public void set_RegisteredCallBack_state(State state) {
        mRegisteredCallBackData.state = state;
    }

    public State get_RegisteredCallBack_state() {
        return mRegisteredCallBackData.state;
    }

    /*
     * Below functionality is for 'ConnectService'
     *
     * Output: returnValue
     *
     * State
     */

    public void set_ConnectService_returnValue(boolean value) {
        mConnectServiceData.mReturnValue = value;
    }

    public boolean get_ConnectService_returnValue() {
        return mConnectServiceData.mReturnValue;
    }

    public void set_ConnectService_state(State state) {
        mConnectServiceData.state = state;
    }

    public State get_ConnectService_state() {
        return mConnectServiceData.state;
    }

}