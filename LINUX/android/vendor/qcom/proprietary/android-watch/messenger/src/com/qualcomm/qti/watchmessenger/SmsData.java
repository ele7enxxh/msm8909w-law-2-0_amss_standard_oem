/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmessenger;

public class SmsData {
    private int smsRowIndex;
    private String smsFrom;
    private String smsDateTime;
    private String smsBody;
    private String smsReadStatus;
    private int smsErrorCode;

    public SmsData() {
    }

    public SmsData(int rowIndex, String from, String dateTime, String body, String status) {
        this.smsRowIndex = rowIndex;
        this.smsFrom = from;
        this.smsDateTime = dateTime;
        this.smsBody = body;
        this.smsReadStatus = status;
    }
    public int getSmsErrorCode() {
        return smsErrorCode;
    }

    public void setSmsErrorCode(int smsErrorCode) {
        this.smsErrorCode = smsErrorCode;
    }

    public void setSmsRowIndex(int rowIndex) {
        this.smsRowIndex = rowIndex;
    }

    public int getSmsRowIndex() {
        return smsRowIndex;
    }

    public void setSmsFrom(String from) {
        this.smsFrom = from;
    }

    public String getSmsFrom() {
        return smsFrom;
    }

    public void setSmsDateTime(String dateTime) {
        this.smsDateTime = dateTime;
    }

    public String getSmsDateTime() {
        return smsDateTime;
    }

    public void setSmsBody(String body) {
        this.smsBody = body;
    }

    public String getSmsBody() {
        return smsBody;
    }

    public void setSmsReadStatus(String status) {
        this.smsReadStatus = status;
    }

    public String getSmsReadStatus() {
        return smsReadStatus;
    }
}
