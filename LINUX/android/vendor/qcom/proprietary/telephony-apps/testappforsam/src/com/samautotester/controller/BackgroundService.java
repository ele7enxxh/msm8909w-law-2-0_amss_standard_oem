/******************************************************************************
 * @file    BackgroundService.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

import com.samautotester.view.AutomatedFunctionalityFragment;

import android.Manifest;
import android.app.IntentService;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.ResultReceiver;
import android.util.Log;
import android.widget.Toast;

public class BackgroundService extends IntentService {

    public static volatile boolean shouldContinue = true;

    public BackgroundService() {
        super(BackgroundService.class.getSimpleName());
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        ResultReceiver receiver = intent
                .getParcelableExtra(AutomatedFunctionalityFragment.RECEIVER_TAG);
        Log.d(BackgroundService.class.getSimpleName(), "onHandleIntent() ... ");
        StringBuilder builder = new StringBuilder();

        // Read the file located at path: /data/app/sam/input.txt
        // File file = new
        // File(Environment.getDataDirectory()+"/app/sam/input.txt");

        File file = new File(Environment.getExternalStorageDirectory()
                + "/input_sam.txt");

        Log.d(BackgroundService.class.getSimpleName(),
                "filePath - " + file.getAbsolutePath());

        if (file.exists()) {
            BufferedReader reader = null;

            try {
                reader = new BufferedReader(new FileReader(file));
                String val;

                while ((val = reader.readLine()) != null) {
                    if (shouldContinue) {

                        String[] temp = (val.trim()).split(",");
                        builder.append(temp[0]);
                        Log.d(BackgroundService.class.getSimpleName(),
                                "temp[0]= " + temp[0]);

                        switch (temp[0]) {
                        case "#1": // getCardState
                            // read input fields
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#1 -> " + builder.toString());
                            int result_code_1 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().getCardState(
                                            Integer.parseInt(reader.readLine()
                                                    .trim()),
                                            TestFunc.INDIVIDUAL);
                            builder.append("\t" + "GetCardState"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_1
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_1)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : "
                                    + TestFunc.INDIVIDUAL
                                            .get_CardState_callback()
                                            .toString()
                                    + System.getProperty("line.separator"));
                            break;

                        case "#2": // getCardATR
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#2 -> " + builder.toString());
                            int result_code_2 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().getCardATR(
                                            Integer.parseInt(reader.readLine()
                                                    .trim()),
                                            TestFunc.INDIVIDUAL);
                            builder.append("\t" + "GetCardATR"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_2
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_2)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : "
                                    + TestFunc.INDIVIDUAL
                                            .get_CardATR_callback().toString()
                                    + System.getProperty("line.separator"));

                            break;

                        case "#3": // streamAPDU , async response
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#3 -> " + builder.toString());
                            String[] arr_str = reader.readLine().trim()
                                    .split(",");

                            int slotId = Integer.parseInt(arr_str[0]);
                            int class_type = Integer.parseInt(arr_str[1]);
                            int instruction = Integer.parseInt(arr_str[2]);
                            int param1 = Integer.parseInt(arr_str[3]);
                            int param2 = Integer.parseInt(arr_str[4]);
                            int param3 = Integer.parseInt(arr_str[5]);
                            String dataCmd = "";
                            if (arr_str.length == 7)
                                dataCmd = arr_str[6];

                            int result_code_3 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().streamApdu(slotId,
                                            class_type, instruction, param1,
                                            param2, param3, dataCmd);
                            builder.append("\t" + "StreamAPDU"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_3
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_3)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : "
                                    + TestFunc.INDIVIDUAL
                                            .get_StreamApdu_callbackResult()
                                    + System.getProperty("line.separator"));

                            break;

                        case "#4": // switchSlot , async response
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#4 -> " + builder.toString());
                            String[] arr = reader.readLine().trim().split(",");
                            int[] arr_int = new int[arr.length];

                            for (int i = 0; i < arr.length; i++)
                                arr_int[i] = Integer.parseInt(arr[i]);

                            int result_code_4 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().switchSlot(arr_int);
                            builder.append("\t" + "SwitchSlot"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_4
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_4)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : "
                                    + TestFunc.INDIVIDUAL
                                            .get_SwitchSlot_callbackResult()
                                    + System.getProperty("line.separator"));

                            break;

                        case "#5": // getSlotMapping
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#5 -> " + builder.toString());
                            int result_code_5 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().getSlotMapping(
                                            TestFunc.INDIVIDUAL);
                            builder.append("\t" + "GetSlotMapping"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_5
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_5)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : ");
                            int slotmap[] = TestFunc.INDIVIDUAL
                                    .get_GetSlotMapping_slotMap();
                            if (slotmap == null) {
                                builder.append(" NULL");
                            } else {
                                for (int ii : slotmap) {
                                    builder.append(ii + ",");
                                }
                            }
                            builder.append(System.getProperty("line.separator"));

                            break;

                        case "#6": // cardPowerUp , async response
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#6 -> " + builder.toString());
                            int result_code_6 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().cardPowerUp(
                                            Integer.parseInt(reader.readLine()
                                                    .trim()));
                            builder.append("\t" + "CardPowerUp"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_6
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_6)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : "
                                    + TestFunc.INDIVIDUAL
                                            .get_CardPowerUp_callbackResult()
                                    + System.getProperty("line.separator"));

                            break;

                        case "#7": // cardPowerDown , async response
                            Log.d(BackgroundService.class.getSimpleName(),
                                    "#7 -> " + builder.toString());
                            int result_code_7 = SAMExecutor.INSTANCE
                                    .getSAMManagerWrapper().cardPowerDown(
                                            Integer.parseInt(reader.readLine()
                                                    .trim()));
                            builder.append("\t" + "CardPowerDown"
                                    + System.getProperty("line.separator"));
                            builder.append("Return_Code : "
                                    + result_code_7
                                    + " , "
                                    + SAMExecutor.INSTANCE
                                            .getResultValue(result_code_7)
                                    + System.getProperty("line.separator"));
                            builder.append("Result : "
                                    + TestFunc.INDIVIDUAL
                                            .get_CardPowerDown_callbackResult()
                                    + System.getProperty("line.separator"));

                            break;
                        }
                    } else {
                        Toast.makeText(this, "Task Cancelled !!! ",
                                Toast.LENGTH_SHORT).show();
                        return;
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
                writeToFile(builder.toString(), receiver);
                Bundle bundle = new Bundle();
                bundle.putSerializable("message", e.getMessage());
                receiver.send(
                        AutomatedFunctionalityFragment.UNABLE_TO_PARSE_FILE,
                        bundle);
                return;
            } finally {
                if (reader != null)
                    try {
                        reader.close();
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
            }

        } else {
            if (shouldContinue) {
                receiver.send(
                        AutomatedFunctionalityFragment.FILE_NOT_AVAILABLE, null);
                return;
            }
        }

        // writing content to output file...
        if (writeToFile(builder.toString(), receiver))
            receiver.send(AutomatedFunctionalityFragment.SUCCESSFUL_CODE, null);
    }

    private boolean writeToFile(String content, ResultReceiver receiver) {
        Log.d(BackgroundService.class.getSimpleName(), "Content - " + content);
        File fileDirectory = null;
        File file = null;
        FileWriter writer = null;

        try {
            // fileDirectory = new
            // File(Environment.getDataDirectory()+"/app/sam/output.txt");
            file = new File(Environment.getExternalStorageDirectory()
                    + "/output_sam.txt");
            // file = new File(fileDirectory,"output.txt");

            // file = new
            // File(Environment.getExternalStorageDirectory()+"/output.txt");
            // FileOutputStream fstream = new FileOutputStream(file);
            // PrintWriter pWriter = new PrintWriter(fstream);
            // pWriter.write(content);
            writer = new FileWriter(file, false);
            writer.write(content);

        } catch (Exception e) {
            e.printStackTrace();
            Bundle bundle = new Bundle();
            bundle.putSerializable("message", e.getMessage());
            receiver.send(-1, bundle);
            Log.d(BackgroundService.class.getSimpleName(),
                    e.getLocalizedMessage());
            return false;
        } finally {
            if (writer != null) {
                try {
                    writer.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    Bundle bundle = new Bundle();
                    bundle.putSerializable("message", e.getMessage());
                    receiver.send(-1, bundle);
                    Log.d(BackgroundService.class.getSimpleName(),
                            e.getLocalizedMessage());
                }
            }
        }
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

}
