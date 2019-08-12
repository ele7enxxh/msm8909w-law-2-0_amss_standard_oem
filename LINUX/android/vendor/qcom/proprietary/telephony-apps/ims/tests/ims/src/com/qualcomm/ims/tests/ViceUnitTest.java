/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.ims.tests;

import android.test.InstrumentationTestCase;
import android.util.Log;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.lang.reflect.*;
import java.lang.Exception;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.codeaurora.ims.parser.*;

public class ViceUnitTest extends InstrumentationTestCase {

    private ImsViceParser mViceParser;
    private ImsViceParser.DialogInfo mDialog;
    private Method mIsCallPullableMethod;

    private static final int DEFAULT_BUFFER_SIZE = 1024 * 4;

    public ViceUnitTest() {
    }

    @Override
    public void setUp() throws Exception {
        mViceParser = new ImsViceParser(getInstrumentation().getContext());
        mDialog = new ImsViceParser.DialogInfo();
    }

    @Override
    public void tearDown() throws Exception{
        mViceParser = null;
        mDialog = null;
    }

    // Invoke public APIs once the object is created
    // TODO: Vice code fails for this test. Needs to be fixed and this UT
    // should be enabled
    /*
    public void testBasicPublicAPIAfterInit() {
        //Excercise public APIs. Verifies for correct instantiation, will
        //identify any failures/crashes
        mViceParser.getDialogInfo();
        mViceParser.getViceDialogInfoAsString();
    }
    */

    public void testgetCallPullInfoForValidXML() {

        /* This is the expected value from the test file being used */
        int callsInTestFile = 2;
        try {
            byte[] fileBytes = getXmlFromFile("two_calls.xml");
            mViceParser.updateViceXmlBytes(fileBytes);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        /* Unused for this test but required as a parameter */
        boolean videoSupported = true;
        boolean volteSupported = true;

        List<String[]> strList = mViceParser.getCallPullInfo(videoSupported, volteSupported);
        assertEquals(callsInTestFile, strList.size());
        Log.d("UT", "List size " + strList.size());
    }

    private String invokeIsCallPullableMethod(ImsViceParser.DialogInfo dialog,
            boolean videoSupported, boolean volteSupported) {
        String outputStr = "";
        try {
            Object arguments[] = new Object[3];
            arguments[0] = dialog;
            arguments[1] = videoSupported;
            arguments[2] = volteSupported;
            outputStr = (String)mIsCallPullableMethod.invoke(mViceParser, arguments);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return outputStr;
    }

    private void openAccessToRequiredPrivateMethods() {
        try {
            // Open up required methods.
            Class cls = mViceParser.getClass();
            Class params[] = new Class[3];
            params[0] = ImsViceParser.DialogInfo.class;
            params[1] = boolean.class;
            params[2] = boolean.class;
            mIsCallPullableMethod = cls.getDeclaredMethod("isCallPullable", params);

            mIsCallPullableMethod.setAccessible(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Util functions for this UT file
     */
    private byte[] getXmlFromFile(String fileName) {
            byte[] fileAsBytes = null;

            //Read the lines of text into an ArrayList
            try {
                String files =
                    Arrays.toString(getInstrumentation().getContext().getAssets().list(""));

                InputStream is =
                    getInstrumentation().getContext().getAssets().open("xml/" + fileName);
                fileAsBytes = toByteArray(is);

                is.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (fileAsBytes != null)
                Log.d("UT", "XML File:" + fileName + ", # of bytes:" + fileAsBytes.length);
            return fileAsBytes;
    }

    public static long copyLarge(InputStream input, OutputStream output)
            throws IOException {
        byte[] buffer = new byte[DEFAULT_BUFFER_SIZE];
        long count = 0;
        int n = 0;
        while (-1 != (n = input.read(buffer))) {
            output.write(buffer, 0, n);
            count += n;
        }
        return count;
    }

    private static int copy(InputStream input, OutputStream output) throws IOException {
        long count = copyLarge(input, output);
        if (count > Integer.MAX_VALUE) {
            return -1;
        }
        return (int) count;
    }

    private static byte[] toByteArray(InputStream input) throws IOException {
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        copy(input, output);
        return output.toByteArray();
    }
}
